
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "errors.h"
#include "util.h"

/* ------------------------------------------------------------------------ */

const char* WHITESPACE = " \t\n\r";
const char* SYMBOLS = "{}(),";
const char* VARIABLE = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";

const char* tokenNames[] = { NULL, "NUMBER", "STRING", "VARIABLE", "COMMAND", "SYMBOL" };

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
int error;

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

void Error(int code)
{
	fprintf(stderr, "%s(%d) : error %d\n",
		currfileentry->filename, currfileentry->line, code);
	error = code;
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
	if (!f) return false;

	FILESTACKENTRY *fe = &FileStack[++currFile];
	
	strcpy(fe->filename, filename);
	fe->input = f;
	fe->line = 0;

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

	return c;
}


/* ------------------------------------------------------------------------ */

int NextToken(void)
{
	static int c = ' ';

	tokenType = T_NONE;
	token[0] = 0;

	if (c == EOF) return 0;
	while (strchr(WHITESPACE, c)) c = NextChar(); // skip whitespace
	if (c == EOF) return 0;

	char *t = token;

	if (strchr(SYMBOLS, c))
	{
		*t = c; *(t+1) = 0;
		tokenType = T_SYMBOL;
		c = NextChar();
	}

	else if (c == '%')
	{
		c = NextChar();
		while (c != EOF && VARIABLESET.is_member(c)) { *(t++) = c; c = NextChar(); }
		*t = 0;
		tokenType = T_VARIABLE;
		//ungetc(c, input);
	}

	else if (strchr("0123456789+-", c))
	{
		while (c >= '0' && c <= '9') { *(t++) = c; c = NextChar(); }
		if (c == '.')
			do { *(t++) = c; c = NextChar(); } while ((c >= '0' && c <= '9'));
		
		*t = 0;
		tokenType = T_NUMBER;
		//ungetc(c, input);
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
	}

	else if (VARIABLESET.is_member(c))
	{
		do {
			*(t++) = c;
			c = NextChar();
		} while (c != EOF && VARIABLESET.is_member(c));
		*t = 0;
		tokenType = T_COMMAND;
	}

	else
	{
		Error(ERR_INVALIDCHAR);
		return 0;
	}

	printf("%-6s%-3d%-10s%s\n",
		FileStack[currFile].filename,
		FileStack[currFile].line,
		tokenNames[tokenType], token);

	return 1;
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

/* ------------------------------------------------------------------------ */

bool GetTokenParams(void)
{
	double v;
	int tok, i, nparam;
	char name[80];
	TokenType paramList[20];
	char *s;

	NextToken();
	if (tokenType != T_SYMBOL || token[0] != '(') { Error(ERR_EXPECTPARAMS); return false; }

	if (!GetNumberToken(&v)) { Error(ERR_EXPECTNUMBER); return false; }
	tok = (int)v;

	if (!(s = GetStringToken())) { Error(ERR_EXPECTSTRING); return false; }
	strcpy(name, s);

	nparam = 0;

	while (true)
	{
		GetNextToken();

		if (!GetNumberToken(&v)) { Error(ERR_EXPECTNUMBER); return false; }
		i = (int)v;
		if (i < 1 || i >= NUMTOKENTYPE) { Error(ERR_INVALIDTOKENNUM); return false; }
		paramList[nparam++] = i;



		if (tokenType == T_SYMBOL && token[0] == ')') break;
	}


}


void CloseAllFiles(void)
{
	for (int i = currFile; i>=0; i--)
		fclose(FileStack[currFile].input);
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