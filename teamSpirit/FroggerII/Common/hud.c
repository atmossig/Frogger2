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
char timeText[32]	= "00:00";
char spawnText[8];

char timeTemp[6];

char countdownTimer	= 1;
char displayFullScreenPoly = 0;

//long timeMin	 = 0, timeSec = 0;
//long timeMin	 = 2, timeSec = (90*30);

//long score		 = 0l
//long lives   = 0;
long gameIsOver  = 0;
long levelIsOver = 0;

ANIM_STRUCTURE *livesIcon;

TEXTOVERLAY	*livesTextOver,*timeTextOver,*scoreTextOver;
TEXTOVERLAY	*gameOver1;
TEXTOVERLAY	*gameOverScore;
TEXTOVERLAY	*babySavedText;

TEXTOVERLAY	*levelComplete1,
			*levelComplete2,
			*levelComplete3;

TEXTOVERLAY *keyCollected;

TEXTOVERLAY *pauseTitle;

TEXTOVERLAY *continueText;
TEXTOVERLAY *quitText;
TEXTOVERLAY *spawnCollected;
TEXTOVERLAY *time;
TEXTOVERLAY *nextLev1;
TEXTOVERLAY *nextLev2;

SPRITEOVERLAY *clock = NULL;
SPRITEOVERLAY *spawn = NULL;

TEXTOVERLAY *wholeKeyText = NULL;

void UpDateOnScreenInfo ( void )
{
	int i;
	static char tickTock = 0;

	sprintf(livesText,"*%lu",player[0].lives);	
	sprintf(scoreText,"%lu",player[0].score);

	if (NUM_FROGS == 1)
	{
		if(scoreTextOver->a && !levelIsOver)
		{
			scoreTextOver->a -= 8;
			if(scoreTextOver->a < 8)
				scoreTextOver->a = 0;
		}
	}


	if(countdownTimer)
	{
		player[0].timeSec--;
		if (player[0].timeSec < 0)
		{
			countdownTimer = 0;
//			PlaySample(GEN_TIME_OUT,0,0,0);
			sprintf(timeText,"NO BONUS !");
			if (NUM_FROGS==1)
				timeTextOver->draw = 50;
		}

		if((player[0].timeSec < (11*30)) && ((player[0].timeSec%30) == 0))
		{
			if ( tickTock )
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
		// ENDIF
								   
		if(player[0].timeSec >= 0)
			sprintf(timeText,"%02lu",(player[0].timeSec/30));
	}
	else
		if (NUM_FROGS==1)
		{
			if(timeTextOver->draw)
				timeTextOver->draw--;
			timeTextOver->a -= 16;
			timeTextOver->a &= 255;
		} 


	if (NUM_FROGS == 1)
	{
		i = 3;
		while(i--)
		{
			if ( frog[0] )
			{
				if(frog[0]->action.healthPoints <= i)
					sprHeart[i]->a = 64;
				else
					sprHeart[i]->a = 255;
			}
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

void InitInGameTextOverlays(unsigned long worldID,unsigned long levelID)
{
	currFont = smallFont;

	pauseTitle		= CreateAndAddTextOverlay ( 50, 70, "pause", YES, NO, 255, 255, 255, 255, smallFont, 0, 0, 0 );
	continueText	= CreateAndAddTextOverlay ( 50, 110, "continue", YES, NO, 255, 255, 255, 255, smallFont, 0,0, 0 );
	quitText		= CreateAndAddTextOverlay ( 50, 130, "quit", YES, NO, 255, 255, 255, 255, smallFont, 0,0, 0 );
	DisableTextOverlay ( pauseTitle );
	DisableTextOverlay ( continueText );
	DisableTextOverlay ( quitText );

	if (NUM_FROGS == 1)
	{
		livesTextOver	= CreateAndAddTextOverlay(50,205,livesText,NO,NO,255,255,255,255,currFont,0,0,0);
		timeTextOver	= CreateAndAddTextOverlay(25,20,timeText,NO,NO,255,255,255,255,currFont,0,0,0);
		scoreTextOver	= CreateAndAddTextOverlay(230,20,scoreText,NO,NO,255,255,255,255,currFont,0,0,0);
	}

	gameOver1 = CreateAndAddTextOverlay(60,239,"game over",NO,NO,255,255,255,255,smallFont,TEXTOVERLAY_WAVECHARS,6,0);
	DisableTextOverlay(gameOver1);
	gameOverScore = CreateAndAddTextOverlay(110,0,gameOverText,YES,NO,255,255,255,255,smallFont,0,0,0);
	DisableTextOverlay(gameOverScore);

	babySavedText = CreateAndAddTextOverlay(0,110,"BABY SAVED",YES,NO,255,255,255,255,smallFont,TEXTOVERLAY_WAVECHARS,4,0);
	DisableTextOverlay(babySavedText);
	babySaved = 0;

	levelComplete1 = CreateAndAddTextOverlay(60,80,"",YES,NO,255,255,255,0,smallFont,0,0,0);
	DisableTextOverlay(levelComplete1);
	levelComplete2 = CreateAndAddTextOverlay(0,120,"Level complete !",YES,NO,255,255,255,0,smallFont,TEXTOVERLAY_WAVECHARS,6,0);
	DisableTextOverlay(levelComplete2);
	levelComplete3 = CreateAndAddTextOverlay(0,145,levelCompleteText,YES,NO,255,255,255,0,smallFont,0,6,0);
	DisableTextOverlay(levelComplete3);
	  
//	keyCollected = CreateAndAddTextOverlay(0,100,"",YES,NO,255,255,255,255,smallFont,TEXTOVERLAY_LIFE,0,0);
//	DisableTextOverlay(keyCollected);


	//  Awards screen

	levelComplete1 = CreateAndAddTextOverlay ( 0, 20, "level", YES, NO, 255, 255, 255, 255, smallFont, 0, 0, 0 );
	DisableTextOverlay(levelComplete1);
	levelComplete2 = CreateAndAddTextOverlay ( 0, 38, "complete", YES, NO, 255, 255, 255, 255, smallFont, 0, 0, 0 );
	DisableTextOverlay(levelComplete2);

	spawnCollected = CreateAndAddTextOverlay ( 100, 130, spawnText, NO, NO, 255, 255, 255, 255, smallFont, 0, 0, 0 );
	sprintf ( spawnCollected->text, "%d", player[0].numSpawn );
	DisableTextOverlay ( spawnCollected );

	time = CreateAndAddTextOverlay ( 100, 160, timeTemp, NO, NO, 255, 255, 255, 255, smallFont, 0, 0, 0 );
	DisableTextOverlay ( time );

	nextLev1 = CreateAndAddTextOverlay ( 0, 180, "next", YES, NO, 255, 255, 255, 255, smallFont, 0, 0, 0 );
	DisableTextOverlay ( nextLev1 );

	nextLev2 = CreateAndAddTextOverlay ( 0, 200, worldVisualData [ player[0].worldNum ].levelVisualData [ player[0].levelNum+1 ].description, YES, NO, 255, 255, 255, 255, smallFont, 0, 0, 0 );
	DisableTextOverlay ( nextLev2 );

	clock = CreateAndAddSpriteOverlay(50,160,"clock001.bmp",32,32,255,255,255,192,ANIMATION_FORWARDS | ANIMATION_PINGPONG );
	AddFrameToSpriteOverlay(clock,"clock002.bmp");
	AddFrameToSpriteOverlay(clock,"clock003.bmp");
	AddFrameToSpriteOverlay(clock,"clock004.bmp");
	AddFrameToSpriteOverlay(clock,"clock005.bmp");
	AddFrameToSpriteOverlay(clock,"clock006.bmp");
	AddFrameToSpriteOverlay(clock,"clock007.bmp");
	AddFrameToSpriteOverlay(clock,"clock008.bmp");
	SetSpriteOverlayAnimSpeed(clock,0.5F);
	clock->draw = 0;

	spawn = CreateAndAddSpriteOverlay(50,120,"spa01.bmp",32,32,255,255,255,192,ANIMATION_FORWARDS | ANIMATION_CYCLE );
	AddFrameToSpriteOverlay(spawn,"spa02.bmp");
	AddFrameToSpriteOverlay(spawn,"spa03.bmp");
	AddFrameToSpriteOverlay(spawn,"spa04.bmp");
	SetSpriteOverlayAnimSpeed(spawn,0.5F);
	spawn->draw = 0;
		  

	wholeKeyText = CreateAndAddTextOverlay ( 0, 110, "World Open", YES, NO, 255, 255, 255, 255, smallFont, 0, 0, 0 );;
	wholeKeyText->draw = 0;
}



/*	--------------------------------------------------------------------------------
	Function		: RunGameOverSequence
	Purpose			: runs the game over sequence
	Parameters		: 
	Returns			: void
	Info			: 
*/
void RunGameOverSequence ( void )
{
	if(!gameOver1->draw)
	{
		sprintf(gameOverScore->text,"you scored %s !!",scoreText);
		EnableTextOverlay(gameOver1);
		EnableTextOverlay(gameOverScore);
	}

	if((gameOverScore->yPos < 120) && (gameIsOver > 200))
		gameOverScore->yPos += 8;
	else if(gameIsOver < 50)
	{
		gameOverScore->centred = 0;
		gameOverScore->xPos += 8;
	}

	if((gameOver1->yPos > 80) && (gameIsOver > 150))
	{
		gameOver1->yPos -= 5;
	}
	else if(gameIsOver < 90)
	{
		gameOver1->centred = 0;
		gameOver1->xPos -= 4;
	}
}



