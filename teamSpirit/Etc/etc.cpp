/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: etc.cpp
	Programmer	: David Swift
	Date		: 02/07/99

	Event/Trigger Compiler for Frogger2
	See "\\server\TeamSpirit\pcversion\docs\Frogger 2 Scripting Manual.doc"

-------------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "etc.h"
#include "buffer.h"
#include "parser.h"
#include "errors.h"
#include "util.h"

/*-------------------------------------------------------------------------*/

typedef unsigned char UBYTE;

#define MAX_PATH 256
#define OUTPUT_FILE_EXT ".fev"

const UBYTE		ETC_VERSION			= 3;
const int		DEBUG_BLOCK_SIZE	= (1 + 2 + 2);

bool
	hold = false,
	save = true,
	quiet = false,
	writeheader = false,
	writedebug = false; 

int verbose = 0;	// indicates HOW verbose we want to go..

int line, files = 0, triggers = 0, events = 0;
int debugLineNo = 0;
char error[80];
char headerfile[256];

char baseDirectory[1024];

#define s(n) ((n != 1) ? "s" : "")

ScriptTokenList triggerList, commandList;


struct FileList
{
	char filename[MAX_PATH];
	FileList *next;
};

FileList *filenames = NULL;

/*-------------------------------------------------------------------------*/

bool AddEvent(Buffer &buffer);
bool AddBlock(Buffer &buffer);
bool AddTrigger(Buffer &output);
bool AddParamsToBuffer(Buffer &buffer, ParamType* params);

/*-------------------------------------------------------------------------*/

bool LoadCommandTable(const char* filename, ScriptTokenList &list)
{
	bool err = false;
	int i;

	ParamType params[10];
	
	if (!OpenFile(filename)) return false;
	
	SetVariable("p_void",	"0");
	SetVariable("p_byte",		"1");
	SetVariable("p_word",		"2");
	SetVariable("p_integer",	"3");
	SetVariable("p_float",		"4");
	SetVariable("p_string",		"5");
	SetVariable("p_trigger",	"6");
	SetVariable("p_block",		"7");

	while (!err)
	{
		char name[40], define[40];
		int tokenVal;
		int pnum = 0;
		
		if (!GetIntegerToken(&i))
		{
			if (tokenType == T_NONE) break; // end of file! we can break out.

			Error("Expected token number"); err = true; break;
		}
		tokenVal = i;

		if (tokenVal < 0 || tokenVal > 255)
		{
			Error("Tokens must be 0-255"); err = true; break;
		}
	
		NextToken();

		if (tokenType != T_COMMAND) {
			Error("Expected C/C++ #define name"); err = true; break;
		}
		strcpy(define, token);

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
			int i;
			if (!GetIntegerToken(&i))
			{
				Error("Expecting param type"); err = true; break;
			}

			if (i < 0 || i >= NUMPARAMTYPE)
			{
				Error("Param type out of range"); err = true; break;
			}
			params[pnum++] = (ParamType)i;

			if (!NextToken() ||	tokenType == T_SYMBOL)
			{
				if (token[0] == ')')
				{
					list.AddEntry(name, define, tokenVal, params, pnum);	// end of params! break out.
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
	char filename[1024];

	GetPath(baseDirectory, exename);

	includePath = baseDirectory;

	strcpy(filename, baseDirectory);
	strcat(filename, "triggers.ec");

	// Load trigger table

	if (!LoadCommandTable(filename, triggerList))
	{
		fprintf(stderr, "Error loading trigger table from %s\n", filename);
		return false;	
	} 

	strcpy(filename, baseDirectory);
	strcat(filename, "commands.ec");

	// Load command table

	if (!LoadCommandTable(filename, commandList))
	{
		fprintf(stderr, "Error loading command table from %s\n", filename);
		return false;	
	} 

	// Add compiler commands

	ParamType params[] = { 0 };
	commandList.AddEntry("Include", NULL, C_INCLUDE, params, 1);
	commandList.AddEntry("Set", NULL, C_SET, params, 1);
	//commandList.AddEntry("Sub", NULL, C_SUB, params, 1);
	//commandList.AddEntry("Call", NULL, C_CALL, params, 1);

	commandList.AddEntry("", "C_DEBUG", C_DEBUG, params, 1);

	return true;
}


/*-------------------------------------------------------------------------*/

bool AddParamsToBuffer(Buffer &b, ParamType* params)
{
	ParamType *type;
	
	const char *c;
	double v; int i;

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
			b.AddString(c);
			break;

		case PARAM_INT:
			if (!GetIntegerToken(&i)) {
				Error("Expecting integer"); return false;
			}
			b.AddInt(i);
			break;

		case PARAM_BYTE:
			if (!GetIntegerToken(&i)) {
				Error("Expecting integer"); return false;
			}
			else if (i < 0 || i > 0xFF)
			{
				Error("Integer out of range (0-255)"); return false;
			}
			b.AddUchar(i);
			break;

		case PARAM_WORD:
			if (!GetIntegerToken(&i)) {			
				Error("Expecting integer"); return false;
			}
			else if (i < -0xFFFF || i > 0xFFFF)
			{
				Error("Integer out of range (-65535 to 65535)"); return false;
			}
			b.AddWord(i);
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

	debugLineNo = CurrentLineNum();

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
			const char *filename = GetStringToken();
			if (!filename) { Error(ERR_EXPECTFILENAME); return false; }
			
			if (!OpenFile(filename)) return false;
			
			if (verbose)
				printf("includes %s\n", filename);
			
			NextToken();
			return AddEvent(buffer);
		}

	case C_SET:
		{
			char name[80];

			NextToken();
			if (tokenType != T_VARIABLE)
			{
				Error(ERR_EXPECTVARIABLE); return false;
			}

			strcpy(name, token);

			NextToken();
			SetVariable(name, token);
			return true;
		}
	case C_ON:
		{
			int v;

			buffer.AddChar(C_ON);
			if (!AddTrigger(buffer)) return false;

			if (!GetIntegerToken(&v) || v < 0 || v > 255) {
				Error("Expecting trigger flags"); return false;
			}
			buffer.AddUchar(v);

			return AddBlock(buffer);
		}

	default:
		if (verbose > 1)
		{
			sprintf(error, "Adding \"%s\" event (%d 0x%02x)", token, tokeninfo->token, tokeninfo->token);
			Error(error);
		}

		buffer.AddUchar(tokeninfo->token);
		if (!AddParamsToBuffer(buffer, tokeninfo->params)) return false;
		events++;
		return true;
	}
}

bool WriteDebug(Buffer &buffer)
{
	buffer.AddUchar(C_DEBUG);
	buffer.AddWord(0);	// placeholder
	buffer.AddWord(debugLineNo);
	return true;
}

bool AddBlock(Buffer &buffer)
{
	Buffer b, bb;
	int e = 0;

	if (!NextToken())
	{
		Error(ERR_EXPECTEVENT);
		return false;	
	}

	if (tokenType != T_SYMBOL)
	{
		if (!AddEvent(bb)) return false;
		if (!bb.Size() && !quiet)
			Error("Warning: Command does nothing in interpreter");

		e = 1;

		if (writedebug)
		{
			WriteDebug(b);
			e = 2;
		}

		b.Append(bb);
	}
	else if (token[0] = '{')
	{
		while (true)
		{
			NextToken();
			if (tokenType == T_SYMBOL && token[0] == '}')
				break;
			else
			{
				bb.Clear();
				if (!AddEvent(bb)) return false;
				if (bb.Size())
				{
					if (writedebug)
					{
						WriteDebug(b);
						e++;
					}
					
					b.Append(bb);
					e++;
				}
				else
					Error("fuzzy");
			}
		}
		
		if (!e)
			Error("Warning: Block contains no commands");
	}
	else
	{
		Error(ERR_INVALIDCHAR); return false;
	}

	buffer.AddInt(b.Size() + 2);	// size of [block + number of events]
	buffer.AddWord(e);				// number of events
	buffer.Append(b);				// buffer
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

	if (!(tokeninfo = triggerList.GetEntry(token)))
	{
		sprintf(error, "'%s' is not a valid trigger", token);
		Error(error);
		return false;
	}

	if (verbose > 1)
	{
		sprintf(error, "Adding \"%s\" trigger (%d 0x%02x)", token, tokeninfo->token, tokeninfo->token);
		Error(error);
	}

	output.AddChar(tokeninfo->token);

	if (!AddParamsToBuffer(output, tokeninfo->params)) return false;

	triggers++;
	return true;
}




bool compile(const char* filename, bool save)
{
	int err = 0, e = 0;
	Buffer buffer, b;
	line = 0;
	
	if (!OpenFile(filename))
	{
		fprintf(stderr, "Couldn't read '%s'\n", filename);
		return false; // && AddBlock(buffer))) return 1;	
	} 

	while (NextToken())
	{
		b.Clear();

		if (!AddEvent(b)) return false;
		if (b.Size())
		{
			if (writedebug)
			{
				WriteDebug(buffer);
				e++;
			}

			//buffer.AddInt(b.Size());
			buffer.Append(b);
			e++;
		}
	}

	if (save)
	{
		char outfile[255];
		GetFilenameStart(outfile, filename);
		strcat(outfile, OUTPUT_FILE_EXT);

		FILE *f = fopen(outfile, "wb");
		if (!f)
		{
			fprintf(stderr, "Couldn't open '%s' for writing\n");
			return false;
		}

		Buffer header;
		header.AddChar(ETC_VERSION);
		header.AddInt(buffer.Size() + 2);	// size of entire script (i.e. outmost block)
		header.AddWord(e);					// number of events in outer block

		fwrite(header.Data(), header.Size(), 1, f);
		fwrite(buffer.Data(), buffer.Size(), 1, f);
		fclose(f);

		int size = header.Size() + buffer.Size();

		if (verbose)
			printf("Saved %s (version %d, %d bytes)\n", outfile, ETC_VERSION, size);
	}

	if (!quiet) puts(filename);
	files++;
	return true;
}

bool OutputHeader(const char* filename)
{
	FILE *headerFile;
	struct tm *newtime;
	time_t aclock;

	time(&aclock); newtime = localtime(&aclock);

	if (!(headerFile = fopen(filename, "w"))) return false;

	fprintf(headerFile, "#ifndef _CODES_H_INCLUDED\n#define _CODES_H_INCLUDED\n\n"
		  "/* codes.h automatically generated by ETC\n");
	fprintf(headerFile,"   Created: %s\n", asctime(newtime));
	fprintf(headerFile, "----------------------------------------------------------------*/\n\n/* Commands */\n\n");
	commandList.OutputDefines(headerFile);
	fprintf(headerFile, "\n/* Triggers */\n\n");
	triggerList.OutputDefines(headerFile);
	fprintf(headerFile, "#endif\n");
	fclose(headerFile);

	return true;
}


void show_splash()
{
	puts(
		"Event-Trigger Compiler v1.0 for Frogger 2\n"
		"(c) 1999 Interactive Studios Ltd.\n"
		"Build " __DATE__ "\n");
}

void show_helpscreen()
{
	puts(
		"Usage: etc [-ptv] [-hfilename] filename ...\n"
		"\n"
		"  filename  Specifies a file or list of files to be compiled.\n"
		"  -h        Output a C/C++ header file from the command tables for use in\n"
		"            the interpreter.\n"
		"  -p        Pause after compiling.\n"
		"  -q        Quiet compile, do not output anything except on errors.\n"
		"  -t        Test compile, do not save any files.\n"
		"  -v        Produce verbose output. Successive v's increase verbosity.");
}

bool interpretCmdLine(int argc, char **argv)
{
	int i;
	char *param;
	char *p;

	FileList *fn = NULL;

	if (argc>1)
	{
		for (i=1; i<argc; i++)
		{
			//strcpy(param, argv[i]);
			param = argv[i];

			if (param[0] == '-')
				for (p = param + 1; *p; p++)
				{
					switch (*p)
					{
					case 'p':
						hold = true; break;

					case 't':
						save = false; break;

					case 'v':
						verbose++; quiet = false; break;

					case 'q':
						quiet = true; verbose = false; break;

					case 'h':
						p++;
						if (!*p)
						{
							fprintf(stderr, "Filename expected after -h\n");
						}
						else
						{
							strcpy(headerfile, p); writeheader = true;
							p = NULL;
						}
						break;

					case 'd':
						writedebug = true;
						break;

					default:
						fprintf(stderr, "Unrecognised switch '%c'\n", *p);
						return false;
					}
					if (!p) break;
				}
			else
			{
				FileList *entry = new FileList;
				strcpy(entry->filename, param);
				entry->next = NULL;

				if (fn) fn->next = entry; else filenames = entry;

				fn = entry;

				//if (!compile(param, save)) errors++;
			}
		}
	}

	if (!quiet)
	{
		show_splash();
		if (argc < 2) show_helpscreen();
	}

	return true;
}

int CompileList(void)
{
	int errors = 0;

	FileList *n, *f;

	for (f = filenames; f; f = n)
	{
		n = f->next;

		if (!compile(f->filename, save)) errors++;
		delete f;
	}

	return errors;
}

int main(int argc, char **argv)
{
	int errors;

	if (interpretCmdLine(argc, argv) && InitTables(argv[0]))
	{
		if (writeheader)
		{
			if (verbose) printf("Writing C/C++ header file to %s\n", headerfile);
			OutputHeader(headerfile);
		}

		errors = CompileList();
		
		if (errors)
			printf("\n%d error%s or warning%s\n", errors, s(errors), s(errors));
		else if (verbose)
		{
			printf("\nCompiled %d trigger%s and %d event%s in %d file%s\n",
				triggers, s(triggers), events, s(events), files, s(files));

			if (writedebug)
				printf("Debug information written to file\n");
		}
		
		if (!save)
			printf("-t was used, so no files were saved\n");
	}
	else
		errors = 1;

	if (hold)
	{
		puts("\nPress return");
		getchar();
	}

  	return errors;
}