/************************************************************************************
	ACTION MAN PS	(c) 1998-9 ISL

	gametext.c:		Game text strings

************************************************************************************/


#include "global.h" 
#include "texture.h"
#include "poly.h"
#include "gametext.h"
#include "fileio.h"
#include "pad.h"
#include "texture.h"
#include "maingame.h"
#include "menu.h"
extern GsOTA *PolyList;



void	*gameTextStr[TEXTSTR_MAX], *gameTextBuffer;
int		gameTextLang = LANG_DEFAULT, *gameTextTimings;

static int		oldLang, currStr, incDelay, holdTime, bkg = 0;


/**************************************************************************
	FUNCTION:	gameTextInitFromFile
	PURPOSE:	Read in the text from a gametext file
	PARAMETERS:	Filename, no of strings, current language
	RETURNS:	
**************************************************************************/

void gameTextInitFromFile(char *fName, int numStrings, int currLang)
{
	char	*bufPtr, *compactBuf;
	int		loop, lang, len = 0;

	gameTextBuffer = fileLoad(fName);
	bufPtr = (char *)gameTextBuffer;
	for(loop=0; loop<numStrings; loop++)
	{
		for(lang=-2; lang<LANG_NUMLANGS; lang++)
		{
			if (*bufPtr=='\"')
				bufPtr++;
			if (lang==currLang)
				gameTextStr[loop] = bufPtr;
			while((*bufPtr!=13) && (*bufPtr!=10) && (*bufPtr!='\t') && (*bufPtr!='\0'))
				bufPtr++;
			if (*(bufPtr-1)=='\"')
				*(bufPtr-1) = '\0';
			*bufPtr++ = '\0';
			if ((*bufPtr==13)||(*bufPtr==10))
				bufPtr++;
			if (lang==currLang)
				len += strlen(gameTextStr[loop]);
		}
	}

	bufPtr = compactBuf = MALLOC(len+numStrings, "gametext");
	debugPrintf("small game text = %d\n", len+numStrings);
	for(loop=0; loop<numStrings; loop++)
	{
		strcpy(bufPtr, gameTextStr[loop]);
		gameTextStr[loop] = bufPtr;
		bufPtr += strlen(gameTextStr[loop])+1;
//		debugPrintf("[%d][%s]\n", loop, gameTextStr[loop]);
	}
	FREE(gameTextBuffer);
	gameTextBuffer = compactBuf;
	compactBuf = MALLOC(len+numStrings, "gametext");
	memcpy(compactBuf, gameTextBuffer, len+numStrings);
	for(loop=0; loop<numStrings; loop++)
	{
		gameTextStr[loop] -= (ULONG)gameTextBuffer;
		gameTextStr[loop] += (ULONG)compactBuf;
	}
	FREE(gameTextBuffer);
	gameTextBuffer = compactBuf;

	gameTextTimings = (int *)fileLoad("GAMETEXT.BIN");
	len = (TEXTSTR_MAX-TEXTSTR_MISSION_CITY1A)*4;
	compactBuf = MALLOC(len, "gametimes");
	memcpy(compactBuf, ((char *)gameTextTimings)+len*currLang, len);
	FREE(gameTextTimings);
	gameTextTimings = compactBuf;
}


/**************************************************************************
	FUNCTION:	gameTextInit
	PURPOSE:	Read in the text from the gametext file
	PARAMETERS:	Current language
	RETURNS:	
**************************************************************************/

void gameTextInit(int currLang)
{
	gameTextInitFromFile("GAMETEXT.TXT", TEXTSTR_MAX, currLang);
}


/**************************************************************************
	FUNCTION:	gameTextDestroy
	PURPOSE:	Free up text
	PARAMETERS:	
	RETURNS:	
**************************************************************************/

void gameTextDestroy()
{
	FREE(gameTextBuffer);
	FREE(gameTextTimings);
}




void gameTextScrInitialise()
{
	oldLang = gameTextLang;
}


void gameTextScrDestroy()
{
	gameTextLang = oldLang;
}


static void gameTextBkg()
{
	POLY_F4 	*si = (POLY_F4 *) GsOUT_PACKET_P; 

	si->x0 = -320;
	si->y0 = -120-PALMODE*8;
	si->x1 = 320;
	si->y1 = -120-PALMODE*8;
	si->x2 = -320;
	si->y2 = 120+PALMODE*8;
	si->x3 = 320;
	si->y3 = 120+PALMODE*8;
	si->r0 = bkg;
	si->g0 = bkg;
	si->b0 = bkg;
	si->code = GPU_COM_F4;
	PUTPACKETINTABLE(si, (GsOTA*)PolyList->org+100, POLYF4_LEN);
	si++;
	(POLY_F4 *)GsOUT_PACKET_P = si;
}

void gameTextScrFrame()
{
	char	buffer[1024], *str;
	int		lines, loop, y;

	gameTextBkg();
	fontPrint(gameInfo.font, -300,-100, TEXTSTR(TEXTSTR_LANG), 128,128,0);
	sprintf(buffer, "String %d/%d", currStr+1, TEXTSTR_MAX);
	fontPrint(gameInfo.font, 300-fontExtentW(gameInfo.font, buffer),-100, buffer, 128,128,0);

	lines = fontFitToWidth(gameInfo.font, 600, TEXTSTR(currStr), buffer);
	str = buffer;
	y = -50;
	for(loop=0; loop<lines; loop++)
	{	
		fontPrint(gameInfo.font, -300,y, str, 128,128,128);
		str += strlen(str)+1;
		y += 15;
	}

	if (debounce[0] & PAD_CROSS)
		gameTextLang = (gameTextLang+1) % LANG_NUMLANGS;

	if ((pad[0] & PAD_LEFT) && (currStr>0) && (incDelay>4))
	{
		currStr--;
		incDelay = 0;
	}
	if ((pad[0] & PAD_RIGHT) && (currStr<TEXTSTR_MAX-1) && (incDelay>4))
	{
		currStr++;
		incDelay = 0;
	}
	if (!(pad[0] & (PAD_LEFT|PAD_RIGHT)))
	{
		incDelay = 10;
		holdTime = 0;
	}
	else
		holdTime++;
	if ((pad[0] & PAD_L1) && (bkg>0))
		bkg-=3;
	if ((pad[0] & PAD_R1) && (bkg<255))
		bkg+=3;

	incDelay++;
	if (holdTime>40)
		incDelay++;
	if (holdTime>80)
		incDelay++;
	if (holdTime>120)
		incDelay++;
	if (holdTime>160)
		incDelay++;

	if (debounce[0] & PAD_START)			// Exit this screen
	{
		gameDestroy();
		gameInfo.mode = GAMEMODE_MENU;
		menuInfo.mode = MENU_MAIN;
	}

	menuInfo.frame++;
}
