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

/*-------------------------------------------------------------------------*/

typedef unsigned char UBYTE;

#define OUTPUT_FILE_EXT ".fev"
#define ETC_VERSION 1

int line, files = 0, triggers = 0, events = 0;
char error[80];

#define s(n) ((n != 1) ? "s" : "")

/*-------------------------------------------------------------------------*/

bool AddEvent(Buffer &buffer);
bool AddBlock(Buffer &buffer);
bool AddTrigger(Buffer &output);
bool AddParamsToBuffer(Buffer &buffer, ParamType* params);

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
			buffer.AddInt(b.Size());
			buffer.Append(b);
		return true;
		}
	}
	return false;
}

bool AddEvent(Buffer &buffer)
{
	strupr(token);

	int command = NOSUCHCOMMAND;
	for (char i = 0; i < NUMEVENTS; i++)
		if (strcmp(token, eventLookup[i].str) == 0)
		{
			command = i; break;
		}

	if (command == NOSUCHCOMMAND)
	{
		sprintf(error, "'%s' is not a valid event", token);
		Error(error);
		return false;
	}

	buffer.AddChar(command);

	if (!AddParamsToBuffer(buffer, eventLookup[command].params)) return false;

	events++;
	return true;
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
	BYTE command;
	int i;

	NextToken();

	if (tokenType != T_COMMAND)
	{
		sprintf(error, "Found %s when expecting a trigger name", tokenNames[tokenType]);
		Error(error);
		return false;
	}

	strupr(token);
	command = NOSUCHCOMMAND;
	for (i = 0; i < NUMTRIGGERS; i++)
		if (strcmp(token, triggerLookup[i].str) == 0)
		{
			command = i; break;
		}

	if (command == NOSUCHCOMMAND)
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

	output.AddChar(command);

	if (!AddParamsToBuffer(output, triggerLookup[command].params)) return false;

	triggers++;
	return true;
}

int compile(const char* filename)
{
	int err = 0;
	Buffer buffer;
	line = 0;
	
	printf("Compiling %s\n", filename);

	OpenFile(filename);

	if (!AddBlock(buffer)) return 1;

	files++;
	return 0;
}

int main(int argc, char **argv)
{
	bool hold = false;
	int error = 0;
	int i;
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
					}
			else
				compile(param);
		}
	}

	printf("\nCompiled %d trigger%s and %d event%s in %d file%s\n",
		triggers, s(triggers), events, s(events), files, s(files));

	if (hold)
	{
		puts("\nPress return");
		getchar();
	}

  	return error;
}