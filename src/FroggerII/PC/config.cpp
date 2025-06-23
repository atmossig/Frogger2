/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: config.cpp
	Programmer	:
	Date		: 

----------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <stdio.h>

extern "C" {

#include "ultra64.h"
#include "incs.h"
#include "config.h"

extern long winMode;
extern long scaleMode;
extern unsigned long synchSpeed;
extern unsigned long pingOffset;
extern unsigned long synchRecovery;
extern unsigned long rKeying;
extern unsigned long rPlaying;
extern int audioEnabled;

int debugKeys = 0;
}

CONFIG cfgOptList[] = 
{
	"*ping_offset"		,"%i"			,&pingOffset,
	"*synch_rate"		,"%i"			,&synchSpeed,
	"*synch_recovery"	,"%i"			,&synchRecovery,

	"" ,NULL,NULL
};

const char *cfg_file = "frogcfg.txt";


/*	--------------------------------------------------------------------------------
	Function		: GetArgs
	Purpose			: Process Cmd Line Args
	Parameters		: 
	Returns			: 
	Info			: 
*/

void GetArgs(char *arglist)
{
	char cmdMode;

	do
	{
		if (*arglist=='+' || *arglist=='-' || *arglist=='/')
		{
			cmdMode = 1;
			while (cmdMode)
				switch(*(++arglist))
				{
					case 'W': case 'w':
						winMode = 1;
						break;

					case 'S': case 's':
						scaleMode = 1;
						break;

					case 'C': case 'c':
						swingCam = 0;
						break;

					case 'R': case 'r':
						rKeying = 1;
						break;

					case 'P': case 'p':
						rPlaying = 1;
						break;

					case 'a': case 'A':
						audioEnabled = !audioEnabled;
						dprintf"Audio %s\n",audioEnabled?"enabled":"disabled"));
						break;

					case 'f': case 'F':
						rFlipOK = !rFlipOK;
						dprintf"Buffer swap by %s\n", rFlipOK?"flip":"blit"));
						break;

					case 'i': case 'I':
						shotMode = !shotMode;
						dprintf"Screenshot mode, using old flip style\n"));
						break;

					case 'D': case 'd':
						playDemos = !playDemos;
						dprintf"Demo mode DISABLED!\n"));
						break;

					case 'K': case 'k':
						debugKeys = !debugKeys;
						dprintf"Debug Keys Enabled\n"));
						break;

					case ' ':
					case 0:
						cmdMode = 0;
						break;
				}
		}
	} while ((*arglist)++);
}

/*	--------------------------------------------------------------------------------
	Function		: GetArgs
	Purpose			: Process Cmd Line Args
	Parameters		: 
	Returns			: 
	Info			: 
*/

void ProcessCfgLine(char *line)
{
	unsigned long lenCmd;
	CONFIG *cur;

	// Skip leading spaces and comments
	while ((*line) && (*line!='*') && (*line!=';')) line++;

	// Nothing Line, or comment
	if (!*line) return;
	if (*line == ';') return;
	
	// Find length of command section
	lenCmd=0;
	while ((line[lenCmd]) && (line[lenCmd]!=' ') && (line[lenCmd]!='\t')) lenCmd++;

	// No Separator
	if (!(line[lenCmd])) return;
	
	// Process command
	cur = cfgOptList;
	while (*cur->name)
	{
		if (strncmp(line,cur->name,lenCmd)==0)
		{
			line += lenCmd;
			
			while ((*line) && ((*line==' ') || (*line=='\t'))) line++;
			
			sscanf(line,cur->type,cur->var);

			return;
		}
		cur++;
	}
}

void ReadConfigFile (void)
{
	char filename[MAX_PATH];
	char line[2048];

	FILE *fp;

	strcpy (filename,baseDirectory);
	strcat (filename,cfg_file);

	fp = fopen(filename,"rt");
	
	if (fp)
	{
		while (!feof(fp))
		{
			fgets(line,2048,fp);
			ProcessCfgLine(line);
		}	
		fclose(fp);
	}
}

