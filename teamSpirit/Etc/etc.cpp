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

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "etc.h"
#include "buffer.h"

/*-------------------------------------------------------------------------*/

typedef unsigned char UBYTE;

#define OUTPUT_FILE_EXT ".fev"
#define ETC_VERSION 1

int line, files = 0, triggers = 0, events = 0;
char filename[80] = "~~standard input~~";
char error[80];

HANDLE output;

#define s(n) ((n != 1) ? "s" : "")

/*-------------------------------------------------------------------------*/

typedef struct TAGTRIGGER {
	BYTE type;
	int numEvents, size;
	BUFFER* params;
	BUFFER* events[10];
} TRIGGER;


TRIGGER trigger;

void AddStringToBuffer(char *str, BUFFER *buffer)
{
	BYTE len = strlen(str);

	AddToBuffer(&len, 1, buffer);
	AddToBuffer(str, len, buffer);
}

void AddFloatToBuffer(float value, BUFFER *buffer)
{
	int v = (int)(value * (float)0x10000);
	AddToBuffer(&v, 4, buffer);
}

void AddIntToBuffer(int v, BUFFER *buffer)
{
	AddToBuffer(&v, 4, buffer);
}

int SizeOfParams(PARAM *params)
{
	PARAM *p;
	int size = 0;

	for (p = params; p->type; p++)
	{
		switch (p->type)
		{
		case PARAM_STRING:
			size += strlen(p->vString) + 1;
			break;
		case PARAM_INT:
			size += 4;
			break;
		case PARAM_FLOAT:
			size += 4;
			break;
		}
	}

	return size;
}

void AddParamsToBuffer(PARAM* params, BUFFER *buffer)
{
	PARAM *p;

	AddIntToBuffer(SizeOfParams(params), buffer);
	
	for (p = params; p->type; p++)
	{
		switch (p->type)
		{
		case PARAM_STRING:
			AddStringToBuffer(p->vString, buffer);
			break;

		case PARAM_INT:
			AddToBuffer(&p->vInt, 4, buffer);
			break;

		case PARAM_FLOAT:
			AddFloatToBuffer(p->vFloat, buffer);
			break;
		}
	}
}

/*-------------------------------------------------------------------------*/

int bytesWritten, bytesRead;

void WriteByte(UBYTE v, HANDLE f)	{ WriteFile(f, &v, 1, &bytesWritten, NULL); }
void WriteInt(int v, HANDLE f)		{ WriteFile(f, &v, 4, &bytesWritten, NULL); }
void WriteFloat(float v, HANDLE f)	{ WriteInt((int)(v * 0x10000), f); }

void WriteBuffer(BUFFER *buffer, HANDLE f)
{
	//WriteInt(buffer->size, f);
	WriteFile(f, buffer->p, buffer->size, &bytesWritten, NULL);
}

void WriteTrigger(TRIGGER *trigger)
{
	int i;
	BUFFER **b;

	WriteInt(trigger->size + 2, output);	// +2 bytes for trigger type & number of events
	WriteByte(trigger->type, output);
	WriteBuffer(trigger->params, output);
	WriteByte((BYTE)trigger->numEvents, output);

	for (i=trigger->numEvents, b = trigger->events; i; i--, b++)
		WriteBuffer(*b, output);
}

void WriteCurrentTrigger(void)
{
	if (trigger.size) WriteTrigger(&trigger);
}

BOOL OpenOutput(const char *filename)
{
	char s[80];

	int i;

	strcpy(s, filename);

	for (i=strlen(filename); i; i--)
	{
		if (s[i] == '.') { s[i] = 0; break; }
	}

	strcat(s, OUTPUT_FILE_EXT);

	output = CreateFile(s, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (output == INVALID_HANDLE_VALUE) return 0;

	WriteByte(ETC_VERSION, output);
	return 1;
}

void CloseOutput(void)
{
	CloseHandle(output);
}


/*-------------------------------------------------------------------------*/

void Error(const char* message)
{
	fprintf(stderr, "%s[%d] : %s\n", filename, line, message);
}

int IsNumeric(char *token)
{
	while (*(token++))
		if (!strchr(NUMERIC, *token)) return 0;

	return 1;
}

int IsInteger(char *token)
{
	if (strchr(token, '.')) return 0;
	return 1;
}

PARAM GetParam(char *token)
{
	PARAM p;

	if (IsNumeric(token))
	{
		if (IsInteger(token))
		{
			p.type = PARAM_INT;
			p.vInt = atoi(token);
		}
		else
		{
			p.type = PARAM_FLOAT;
			p.vFloat = (float)atof(token);
		}
	}
	else
	{
		p.type = PARAM_STRING;
		p.vString = (char*)malloc(strlen(token));
		strcpy(p.vString, token);
	}

	return p;
}

int CheckParameters(PARAMTYPE *expect, PARAM *params)
{
	PARAM *p;
	
	for (p = params; p->type; p++, expect++)
	{
		if (*expect == 0)
		{
			Error("Too many parameters");
			return 1;
		}
		else if (p->type != *expect)
		{
			sprintf(error, "Found %s when expecting %s",
				PARAMNAMESTRING[p->type], PARAMNAMESTRING[*expect]);
			Error(error);
			return 1;
		}
	}

	if (*expect != PARAM_NONE)
	{
		Error("Not enough parameters");
		return 1;
	}

	return 0;
}

int AddEvent(BYTE command, PARAM *params)
{
	BUFFER *b;

	if (triggers < 1)
	{
		Error("No trigger defined for this event");
		return 1;
	}
	
	if (CheckParameters(commandLookup[command].params, params)) return 1;

	b = trigger.events[trigger.numEvents];
	if (!b) b = trigger.events[trigger.numEvents] = MakeBuffer();

	b->size = 0;
	AddToBuffer(&command, 1, b);
	AddParamsToBuffer(params, b);

	trigger.numEvents++;
	trigger.size += b->size;
	events++;
	return 0;
}

int AddTrigger(PARAM *params)
{
	BYTE command;
	int i;

	strupr(params[0].vString);

	if (params[0].type != PARAM_STRING)
	{
		sprintf(error, "Found %s when expecting a trigger name", PARAMNAMESTRING[params[0].type]);
		Error(error);
		return 1;
	}

	command = NOSUCHCOMMAND;
	for (i = 0; i < NUMTRIGGERS; i++)
		if (strcmp(params[0].vString, triggerLookup[i].str) == 0)
		{
			command = i; break;
		}

	if (command == NOSUCHCOMMAND)
	{
		sprintf(error, "'%s' is not a valid trigger", params[0].vString);
		Error(error);
		return 1;
	}

	if (CheckParameters(triggerLookup[i].params, params+1)) return 1;

	WriteCurrentTrigger();

	trigger.type = triggerLookup[command].token;
	trigger.numEvents = 0;
	trigger.params->size = 0;
	AddParamsToBuffer(params+1, trigger.params);

	trigger.size = trigger.params->size;

	triggers++;
	return 0;
}

void Tokenise(char *buffer, PARAM *params)
{
	PARAM *p = params;
	char *token;
	
	token = buffer;
/*
	for (;;)
	{
		while (strchr(WHITESPACE, *(token++)));
		if (!*token) break;
	}
  	token = strtok(buffer, WHITESPACE);
	if (!token) return;

	for (p = params; token; p++)
	{
		*p = GetParam(token);
		token = strtok(NULL, WHITESPACE);
	}
*/

	p->type = PARAM_NONE;
}

int compile(const char* filename)
{
	char buffer[128];
	PARAM params[10];
	PARAM *p;
	char *token;
	int i, err;
	BYTE command;
	
	line = 0; err = 0;

	printf("Compiling %s...\n", filename);

	while (!feof(f) && !err)
	{
		if (!fgets(buffer, 127, f)) break;
		line++;

		if ((token = strchr(buffer, ';')) != NULL) *token = 0;	// strip comments

		if (!buffer[0]) continue; // skip empty lines

		Tokenise(buffer, params);
		p->type = PARAM_NONE;

		if (params[0].type != PARAM_STRING)
		{
			Error("Line does not start with a command");
			err = 1; break;
		}

		strupr(params[0].vString);

		command = NOSUCHCOMMAND;
		for (i = 0; i < NUMCOMMANDS; i++)
			if (strcmp(params[0].vString, commandLookup[i].str) == 0)
			{
				command = i; break;
			}

		if (command == NOSUCHCOMMAND)
		{
			sprintf(error, "'%s' is not a valid command", params[0].vString);
			Error(error);
			err = 1; break;
		}

		switch (commandLookup[command].token)
		{
		case C_IF:
			err = AddTrigger(params+1);
			// interpret conditional
			break;

		default:
			Error("Unsupported command"); err= 1;
			break;
		}
	}

	if (!err) WriteCurrentTrigger();

	return err;
}

int main(int argc, void **argv)
{
	FILE *f;
	int error = 0;
	int i;

	trigger.params = MakeBuffer();
	for (i=10; i; i--) trigger.events[i] = MakeBuffer();

	if (argc>1)
	{
		for (i=1; i<argc; i++)
		{
			strcpy(filename, argv[i]);

			f = fopen(filename, "r");
			if (!f)
			{
				fprintf(stderr, "Could not open %s, skipping\n", filename);
				error = 1;
				continue;
			}

			if (!OpenOutput(filename))
			{
				fprintf(stderr, "Couldn't open output file!\n");
				error = 1;
				continue;
			}
			if (!compile(f)) error = 1;
			fclose(f);
			CloseOutput();
			files++;
		}
	}
	else
	{
		OpenOutput("etc");
		compile(stdin);
	}

	printf("\nCompiled %d trigger%s and %d event%s in %d file%s\n\n",
		triggers, s(triggers), events, s(events), files, s(files));

#ifdef _DEBUG
	puts("Press return");
	getchar();
#endif

	return error;
}