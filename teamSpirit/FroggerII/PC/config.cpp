
#include <windows.h>
#include <windowsx.h>
#include <crtdbg.h>
#include <commctrl.h>
#include <cguid.h>
#include <ddraw.h>
#include <d3d.h>
#include <dsound.h>
#include <dinput.h>
#include <dplay.h>
#include <dplobby.h>
#include <stdio.h>
#include "network.h"
#include "netchat.h"
#include "netgame.h"
#include "config.h"
#include "mdxDDraw.h"

extern "C" {

#include <ultra64.h>
#include "block.h"
#include "directx.h"
#include "controll.h"
#include "types.h"
#include "define.h"
#include "font.h"
#include "actor.h"
#include "overlays.h"
#include "frogger.h"
#include "maths.h"
#include "babyfrogs.h"
#include "game.h"
#include "specfx.h"
#include "audio.h"
#include "frogmove.h"

extern unsigned long USE_MENUS;
extern long winMode;
extern long scaleMode;
extern unsigned long synchSpeed;
extern unsigned long pingOffset;
extern unsigned long synchRecovery;
extern unsigned long rKeying;
extern unsigned long rPlaying;
extern int audioEnabled;

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

					case 'M': case 'm':
						USE_MENUS = 1;
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
	unsigned long lenCmd,ccmd;
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

}