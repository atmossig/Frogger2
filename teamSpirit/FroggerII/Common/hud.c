/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: hud.c
	Programmer	: James Healey
	Date		: 31/03/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


char scoreText[32]	= "10000000";
char livesText[8]	= "xxxx";
char timeText[32]	= "00:00      ";
char garibText[8], creditText[8];

char timeTemp[6];

char countdownTimer	= 1;
char displayFullScreenPoly = 0;

ANIM_STRUCTURE *livesIcon;

TEXTOVERLAY	*livesTextOver,*timeTextOver,*scoreTextOver;
TEXTOVERLAY	*babySavedText;

TEXTOVERLAY *keyCollected;

//TEXTOVERLAY *pauseTitle;

TEXTOVERLAY *continueText;
TEXTOVERLAY *quitText;
TEXTOVERLAY *garibCount, *creditCount;
TEXTOVERLAY *time;
TEXTOVERLAY *build;
TEXTOVERLAY *posText;
TEXTOVERLAY *levelnameText;

TEXTOVERLAY *wholeKeyText = NULL;

char levelString[] = "Levelname Goes in here";
char posString[] = "-----------------------";

void UpDateOnScreenInfo ( void )
{
	int i;
	static char tickTock = 0;
	static long lastCount = 0;

/*	if( player[0].worldNum == WORLDID_SUPERRETRO ) // credits are lives in super retro
	{
		sprintf(livesText,"*%lu",player[0].numCredits);	
		sprintf(scoreText,"%lu",player[0].score);
	}
	else
	{
*/		sprintf(livesText,"%lu",player[0].lives);	
		sprintf(scoreText,"%lu",player[0].score);
		sprintf(garibText,"%lu",player[0].numSpawn);	
		sprintf(creditText,"%lu",player[0].numCredits);
//	}

	if(scoreTextOver->a && !(gameState.mode==LEVELCOMPLETE_MODE && modeTimer.time) )
	{
		scoreTextOver->a -= 8;
		if(scoreTextOver->a < 8)
			scoreTextOver->a = 0;
	}

	if(countdownTimer)
	{
		GTUpdate( &scoreTimer, -1 );

		if( !scoreTimer.time )
		{
			countdownTimer = 0;
//			PlaySample(GEN_TIME_OUT,0,0,0);
			sprintf(timeText,"NO BONUS !");
			if (NUM_FROGS==1)
				timeTextOver->draw = 50;
		}

		if( (scoreTimer.time < 11) && !(scoreTimer.time%2) )
		{
			if( tickTock )
			{
//				PlaySample(GEN_CLOCK_TOCK,0,0,0);
				tickTock = 0;
			}
			else
			{
//				PlaySample(GEN_CLOCK_TICK,0,0,0);
				tickTock = 1;
			}
		}
								   
		if( scoreTimer.time ) sprintf(timeText,"%02lu",scoreTimer.time);
	}
	else
	{
		if(timeTextOver->draw)
			timeTextOver->draw-=gameSpeed;
		timeTextOver->a -= 16*gameSpeed;
		timeTextOver->a &= 255;
	}

	i = 3;
	while(i--)
	{
		if ( frog[0] )
		{
			if(player[0].healthPoints <= i)
				sprHeart[i]->a = 64;
			else
				sprHeart[i]->a = 255;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateScore
	Purpose			: updates the current score
	Parameters		: ACTOR2 *,long
	Returns			: void
	Info			: 
*/
void UpdateScore(ACTOR2 *act,long scoreUpdate)
{
	if (NUM_FROGS == 1)
	{
		player[0].score += scoreUpdate;
		scoreTextOver->a = 255;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: InitInGameTextOverlays
	Purpose			: creates and initialises in game text overlay messages
	Parameters		: unsigned long,unsigned long
	Returns			: void
	Info			: 
*/
char levelCompleteText[32];
char gameOverText[32];

extern char levelString[];
extern char posString[];

void InitInGameTextOverlays(unsigned long worldID,unsigned long levelID)
{
	currFont = currFont;

	/* PAUSE menu */

//	pauseTitle		= CreateAndAddTextOverlay ( 50, 70, "pause", YES, NO, 255, 255, 255, 255, currFont, 0, 0, 0 );
	continueText	= CreateAndAddTextOverlay ( 50, 110, "continue", YES, 255, currFont, 0,0 );
	quitText		= CreateAndAddTextOverlay ( 50, 130, "quit", YES, 255, currFont, 0,0 );
	posText			= CreateAndAddTextOverlay ( 50, 50, posString, YES, 255, smallFont, 0, 0);
	levelnameText	= CreateAndAddTextOverlay ( 50, 70, levelString, YES, 255, smallFont, 0, 0);

	//	DisableTextOverlay ( pauseTitle );
	DisableTextOverlay ( continueText );
	DisableTextOverlay ( quitText );
	DisableTextOverlay ( posText );
	DisableTextOverlay ( levelnameText );

	/* Lives and score */

	if (NUM_FROGS == 1)
	{
		livesTextOver	= CreateAndAddTextOverlay(50,205,livesText,NO,255,currFont,0,0);
		scoreTextOver	= CreateAndAddTextOverlay(230,20,scoreText,NO,255,currFont,0,0);
	}
	timeTextOver	= CreateAndAddTextOverlay(25,20,timeText,NO,255,currFont,0,0);

	build = CreateAndAddTextOverlay(40,230,"Toy Fair Demo Build      In Development",NO,255,smallFont,0,0);
	build->r			= 55;
	build->g			= 235;
	build->b			= 55;
	build->a			= 210;

	/* Garib and Credit counters */

	garibCount = CreateAndAddTextOverlay ( 100, 205, garibText, NO, 255, currFont, 0, 0 );
	creditCount = CreateAndAddTextOverlay ( 150, 205, creditText, NO, 255, currFont, 0, 0 );

	/* Baby display */

	babySavedText = CreateAndAddTextOverlay(0,110,"BABY SAVED",YES,255,currFont,TEXTOVERLAY_WAVECHARS,4);
	DisableTextOverlay(babySavedText);
	babySaved = 0;
}


