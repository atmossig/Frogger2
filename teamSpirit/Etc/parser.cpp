
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "errors.h"
#include "util.h"

/* ------------------------------------------------------------------------ */

const char* WHITESPACE = " \t\n\r";
const char* SYMBOLS = "{}(),";
const char* VARIABLE = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";

const char* tokenNames[] = { NULL, "number", "string", "variable", "command", "symbol" };

enum CommandCodes
{
	C_NONE = 0,
	C_INCLUDE,
	C_SET
};

#define FILESTACKSIZE 10

CharSet VARIABLESET(VARIABLE);
VarTableEntry *varTable = NULL;

Lookup tokenList;

/* ------------------------------------------------------------------------ */

char token[80];
TokenType tokenType;

struct FILESTACKENTRY
{
	char filename[80];
	FILE *input;
	//char *buffer, *index;

	long line;
} FileStack[FILESTACKSIZE];

int currFile = -1;
FILESTACKENTRY *currfileentry = NULL;


VarTableEntry *GetVariable(const char* name)
{
	VarTableEntry *vte;

	for (vte = varTable; vte; vte = vte->link)
		if (!strcmp(vte->name, name))
			return vte;

	return NULL;
}

/* ------------------------------------------------------------------------ */

const char* CurrentFilename()
{
	return currfileentry->filename;
}

void Error(const char* message)
{
	fprintf(stderr, "%s(%d) : %s\n",
		currfileentry->filename, currfileentry->line, message);
	//error = code;
}

bool OpenFile(const char* filename)
{
	FILE *f;
	
	if (currFile == FILESTACKSIZE-1)
	{
		Error(ERR_TOOMANYFILES);
		return false;
	}

	f = fopen(filename, "r");
	if (!f)
	{
		if (currFile >= 0)
		{
			char err[255];
			sprintf(err, "Couldn't open '%s'", filename);
			Error(err);
		}
		return false;
	}

	FILESTACKENTRY *fe = &FileStack[++currFile];
	
	strcpy(fe->filename, filename);
	fe->input = f;
	fe->line = 1;

	currfileentry = fe;

	return true;
}

void CloseCurrFile(void)
{
	fclose(FileStack[currFile].input);
	currfileentry = &FileStack[--currFile];
}

int NextChar(void) {
	char c =  getc(FileStack[currFile].input);

	if (c == EOF && currFile > 0)
	{
		CloseCurrFile();
		return NextChar();
	}
	else if (c == '\n')
		FileStack[currFile].line++;
	else if (c == ';')
	{
		do { c = getc(FileStack[currFile].input); } while (c != EOF && c != '\n');
		FileStack[currFile].line++;
		return NextChar();
	}

	return c;
}


/* ------------------------------------------------------------------------ */

int NextToken(void)
{
	static int c = ' ';

	tokenType = T_NONE;
	token[0] = 0;

	while (strchr(WHITESPACE, c)) { c = NextChar(); } ; // skip whitespace
	if (c == EOF) { c = ' '; return 0; }

	char *t = token;

	if (strchr(SYMBOLS, c))
	{
		*t = c; *(t+1) = 0;
		tokenType = T_SYMBOL;
		c = NextChar();
		return 1;
	}

	else if (c == '%')
	{
		c = NextChar();
		while (c != EOF && VARIABLESET.is_member(c)) { *(t++) = c; c = NextChar(); }
		*t = 0;
		tokenType = T_VARIABLE;
		return 1;
	}

	else if (strchr("0123456789+-", c))
	{
		while (c >= '0' && c <= '9') { *(t++) = c; c = NextChar(); }
		if (c == '.')
			do { *(t++) = c; c = NextChar(); } while ((c >= '0' && c <= '9'));
		
		*t = 0;
		tokenType = T_NUMBER;
		return 1;
	}

	else if (c == '"')
	{
		for (;;) {
			c = NextChar();
			if (c == '"')
				break;
			else if (c == EOF || c == '\n' || c == '\r')
			{
				Error(ERR_NOCLOSINGQUOTE); return 0;
			}
			else
				*(t++) = c;
		};
			
		*t = 0;
		tokenType = T_STRING;
		c = NextChar();
		return 1;
	}

	else if (VARIABLESET.is_member(c))
	{
		do {
			*(t++) = c;
			c = NextChar();
		} while (c != EOF && VARIABLESET.is_member(c));
		*t = 0;
		tokenType = T_COMMAND;
		return 1;
	}

	else
		Error(ERR_INVALIDCHAR);
/*
	printf("%-6s%-3d%-10s%s\n",
		FileStack[currFile].filename,
		FileStack[currFile].line,
		tokenNames[tokenType], token);
*/
	c = ' ';
	return 0;	// failure
}

char *GetStringToken(void)
{
	NextToken();

	if (tokenType == T_VARIABLE)
		return GetVariable(token)->value;
	else if (tokenType != T_STRING) return NULL;

	return token;
}

bool GetNumberToken(double *value)
{
	NextToken();

	if (tokenType == T_VARIABLE)
	{
		char *c, *p;
		c = p = GetVariable(token)->value;
		while (*c >= '0' && *c <= '9') c++;
		if (*c == '.') while ((*c >= '0' && *c <= '9')) c++;
		if (*c) return false;

		*value = atof(p);
	}
	if (tokenType != T_NUMBER) return false;

	*value = atof(token);
	return true;
}



void CloseAllFiles(void)
{
	for (int i = currFile; i>=0; i--)
		fclose(FileStack[currFile].input);
	currFile = -1;
}

/* ------------------------------------------------------------------------

int main(int argc, char**argv)
{
	if (!OpenFile("foo")) return -1;

	compile();

	for (int i = currFile; i>=0; i--)
		fclose(FileStack[currFile].input);
	
	fflush(stdout);

	getchar();

	return 0;
}
*/