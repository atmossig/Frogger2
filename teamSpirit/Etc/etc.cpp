/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: etc.cpp
	Programmer	: David Swift
	Date		: 02/07/99


	Event/Trigger Compiler for Frogger2
	Very (very) simple compiler

	Syntax
	------

	IF condition param ...
	command param ...
	command param ...

	IF condition param ...
	AND condition param ...
	command param

	IF ...
	OR ...
	command ...

	; Comments start with a semi-colon

-------------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "etc.h"
#include "buffer.h"
#include "parser.h"
#include "errors.h"
#include "util.h"

/*-------------------------------------------------------------------------*/

typedef unsigned char UBYTE;

#define OUTPUT_FILE_EXT ".fev"
#define ETC_VERSION 2

bool hold = false, save = true, verbose = false;

int line, files = 0, triggers = 0, events = 0;
char error[80];

#define s(n) ((n != 1) ? "s" : "")

ScriptTokenList triggerList, commandList;

/*-------------------------------------------------------------------------*/

bool AddEvent(Buffer &buffer);
bool AddBlock(Buffer &buffer);
bool AddTrigger(Buffer &output);
bool AddParamsToBuffer(Buffer &buffer, ParamType* params);

/*-------------------------------------------------------------------------*/

bool LoadCommandTable(const char* filename, ScriptTokenList &list)
{
	bool err = false;
	double v;

	ParamType params[10];

	if (!OpenFile(filename)) return false;
	
	while (!err)
	{
		char name[40];
		int tokenVal;
		int pnum = 0;
		
		if (!GetNumberToken(&v))
		{
			if (tokenType == T_NONE) break; // end of file! we can break out.

			Error("Expected token number"); err = true; break;
		}
		tokenVal = (int)v;

		if (tokenVal < 0 || tokenVal > 255)
		{
			Error("Tokens must be 0-255"); err = true; break;
		}
	
		NextToken();

		if (tokenType != T_COMMAND) {
			Error("Expected token name"); err = true; break;
		}
		strcpy(name, token);

		if (!(NextToken() && tokenType == T_SYMBOL && token[0] == '(')) {
			Error("Expecting '('"); err = true; break;
		}

		while (!err)
		{
			if (!GetNumberToken(&v))
			{
				Error("Expecting param type"); err = true; break;
			}

			int i = (int)v;
			if (i < 0 || i >= NUMPARAMTYPE)
			{
				Error("Param type out of range"); err = true; break;
			}
			params[pnum++] = (ParamType)i;

			if (!NextToken() ||	tokenType == T_SYMBOL)
			{
				if (token[0] == ')')
				{
					list.AddEntry(name, tokenVal, params, pnum);	// end of params! break out.
					break;
				}
				else if (token[0] != ',')
				{
					Error("Invalid Symbol"); err = true;
				}
			}
		}
	}

	CloseCurrFile();

	return !err;
}

bool InitTables(const char* exename)
{
	char basedir[256], filename[256];

	GetPath(basedir, exename);

	strcpy(filename, basedir);
	strcat(filename, "triggers.ec");

	// Load trigger table

	if (!LoadCommandTable(filename, triggerList))
	{
		fprintf(stderr, "Error loading trigger table from %s", filename);
		return false;	
	} 

	strcpy(filename, basedir);
	strcat(filename, "commands.ec");

	// Load command table

	if (!LoadCommandTable(filename, commandList))
	{
		fprintf(stderr, "Error loading command table from %s", filename);
		return false;	
	} 

	// Add "INCLUDE" compiler command

	ParamType params[] = { PARAM_STRING };
	commandList.AddEntry("Include", C_INCLUDE, params, 1);

	return true;
}


/*-------------------------------------------------------------------------*/

bool AddParamsToBuffer(Buffer &b, ParamType* params)
{
	ParamType *type;
	
	char *c;
	double v;

	//AddIntToBuffer(SizeOfParams(params), buffer);

	if (!(NextToken() && tokenType == T_SYMBOL && token[0] == '(')) {
		Error("Expecting '('"); return false;
	}
	
	type = params;
	while(true)
	{
		switch (*type)
		{
		case PARAM_STRING:
			if (!(c = GetStringToken())) {
				Error("Expecting string"); return false;
			}
			b.AddString(token);
			break;

		case PARAM_INT:
			if (!GetNumberToken(&v)) {
				Error("Expecting number"); return false;
			}
			b.AddInt((int)v);
			break;

		case PARAM_FLOAT:
			if (!GetNumberToken(&v)) {
				Error("Expecting number"); return false;
			}
			b.AddFloat((float)v);
			break;

		case PARAM_BLOCK:
			if (!AddBlock(b)) return false;
			break;

		case PARAM_TRIGGER:
			if (!AddTrigger(b)) return false;
			break;

		case PARAM_NONE:
			
			if (!(NextToken() && tokenType == T_SYMBOL && token[0] == ')')) {
				Error("Expecting ')'"); return false;
			}
			return true;
		}

		if (*++type)
		{
			if (!(NextToken() && tokenType == T_SYMBOL && token[0] == ','))
			{
				Error("Expecting ','"); return false;
			}
		}
		else
		{
			if (!(NextToken() && tokenType == T_SYMBOL && token[0] == ')'))
			{
				Error("Expecting ')'"); return false;
			}
		return true;
		}
	}
	return false;
}

bool AddEvent(Buffer &buffer)
{
	ScriptToken *tokeninfo;

/*
	strupr(token);
	int command = NOSUCHCOMMAND;
	for (char i = 0; i < NUMEVENTS; i++)
		if (strcmp(token, eventLookup[i].str) == 0)
		{
			command = i; break;
		}
*/
	if (!(tokeninfo = commandList.GetEntry(token)))
	{
		sprintf(error, "'%s' is not a valid event", token);
		Error(error);
		return false;
	}

	switch (tokeninfo->token)
	{
	case C_INCLUDE:
		{
			char foo[255];
			char *filename = GetStringToken();
			if (!filename) { Error(ERR_EXPECTFILENAME); return false; }
			
			GetPath(foo, CurrentFilename());
			strcat(foo, filename);

			if (!OpenFile(foo)) return false;
			
			if (verbose)
				printf("includes %s\n", foo);
			
			NextToken();
			return AddEvent(buffer);
		}

	default:
		buffer.AddChar(tokeninfo->token);
		if (!AddParamsToBuffer(buffer, tokeninfo->params)) return false;
		events++;
		return true;
	}
}

bool AddBlock(Buffer &buffer)
{
	Buffer b;

	if (!NextToken())
	{
		Error(ERR_EXPECTEVENT);
		return false;	
	}

	if (tokenType != T_SYMBOL)
	{
		if (!AddEvent(b)) return false;
		buffer.AddInt(1);
		buffer.AddInt(b.Size());
		buffer.Append(b);
	}
	else if (token[0] = '{')
	{
		int e = 0;
		while (true)
		{
			NextToken();
			if (tokenType == T_SYMBOL && token[0] == '}')
				break;
			else
			{
				Buffer bb;
				if (!AddEvent(bb)) return false;
				b.AddInt(bb.Size());
				b.Append(bb);
				e++;
			}
		}
		buffer.AddInt(e);
		buffer.Append(b);
	}
	else
	{
		Error(ERR_INVALIDCHAR); return false;
	}
	return true;
}

bool AddTrigger(Buffer &output)
{
	ScriptToken *tokeninfo;
	
	NextToken();

	if (tokenType != T_COMMAND)
	{
		sprintf(error, "Found %s when expecting a trigger name", tokenNames[tokenType]);
		Error(error);
		return false;
	}
/*
	strupr(token);
	command = NOSUCHCOMMAND;
	for (i = 0; i < NUMTRIGGERS; i++)
		if (strcmp(token, triggerLookup[i].str) == 0)
		{
			command = i; break;
		}
*/
	if (!(tokeninfo = triggerList.GetEntry(token)))
	{
		sprintf(error, "'%s' is not a valid trigger", token);
		Error(error);
		return false;
	}

/*
	trigger.type = triggerLookup[command].token;
	trigger.numEvents = 0;
	trigger.params->size = 0;
	AddParamsToBuffer(params+1, trigger.params);
	trigger.size = trigger.params->size;
*/

	output.AddChar(tokeninfo->token);

	if (!AddParamsToBuffer(output, tokeninfo->params)) return false;

	triggers++;
	return true;
}




bool compile(const char* filename, bool save)
{
	int err = 0, e = 0;
	Buffer buffer;
	line = 0;
	
	puts(filename);

	if (!OpenFile(filename))
	{
		fprintf(stderr, "Couldn't read '%s'\n", filename);
		return false; // && AddBlock(buffer))) return 1;	
	} 

	while (NextToken())
	{
		Buffer b;
		if (!AddEvent(b)) return false;
		buffer.AddInt(b.Size());
		buffer.Append(b);
		e++;
	}

	if (save && e)
	{
		char outfile[255];
		GetFilenameStart(outfile, filename);
		strcat(outfile, OUTPUT_FILE_EXT);

		Buffer header;

		header.AddChar(ETC_VERSION);
		header.AddInt(e);

		FILE *f = fopen(outfile, "wb");
		if (!f)
		{
			fprintf(stderr, "Couldn't open '%s' for writing\n");
			return false;
		}

		fwrite(header.Data(), header.Size(), 1, f);
		fwrite(buffer.Data(), buffer.Size(), 1, f);
		fclose(f);

		int size = header.Size() + buffer.Size();

		if (verbose)
			printf("Saved %s (%d bytes)\n", outfile, size);
	}

	files++;
	return true;
}

void show_splash()
{
	puts(
		"Event-Trigger Compiler v0.2 for Frogger 2 by David Swift\n"
		"(c) 1999 Interactive Studios Ltd.\n");
}

int interpretCmdLine(int argc, char **argv)
{
	int i, errors = 0;
	char param[80];
	char *p;

	if (argc>1)
	{
		for (i=1; i<argc; i++)
		{
			strcpy(param, argv[i]);

			if (param[0] == '-')
				for (p = param + 1; *p; p++)
					switch (*p)
					{
					case 'p':
						hold = true; break;

					case 't':
						save = false; break;

					case 'v':
						verbose = true; break;
					}
			else
				if (!compile(param, save)) errors++;
		}
	}
	else
		puts("Usage: etc [ -p ] filename ...");

	return errors;
}

int main(int argc, char **argv)
{
	int errors = 0;
	show_splash();

	if (InitTables(argv[0]))
	{
		errors = interpretCmdLine(argc, argv);
	}
	
	if (errors)
		printf("\n%d errors\n", errors);
	else if (verbose)
		printf("\nCompiled %d trigger%s and %d event%s in %d file%s\n",
			triggers, s(triggers), events, s(events), files, s(files));
	
	if (!save)
		printf("-t was used, so no files were saved\n");

	if (hold)
	{
		puts("\nPress return");
		getchar();
	}

  	return errors;
}