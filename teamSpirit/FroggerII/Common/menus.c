/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: menus.c
	Programmer	: James Healey
	Date		: 27/05/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"

TEXTOVERLAY *worldNames	[ MAX_WORLDS ];
TEXTOVERLAY *levelN		[ MAX_LEVELS ];
TEXTOVERLAY *closed;

SPRITEOVERLAY *atari = NULL;
SPRITEOVERLAY *konami = NULL;
SPRITEOVERLAY *sprOver = NULL;


/*	--------------------------------------------------------------------------------
	Function 	: RunTitleScreen
	Purpose 	: The first menu
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
char mpText[] = "Multiplayer 1";

void RunTitleScreen()
{
	static unsigned long currentSelection = 0;
	static u16 button;
	static u16 lastbutton;

	static TEXTOVERLAY *startText;
	static TEXTOVERLAY *selectText;
	static TEXTOVERLAY *multiText;
	static TEXTOVERLAY *devText;
	static long numPlayers = 1;

	int i = 0,xPos,j;

	if(frameCount == 1)
	{
		FreeAllLists();

		LoadTextureBank(SYSTEM_TEX_BANK);
#ifdef PC_VERSION
		LoadTextureBank(TITLES_TEX_BANK);
#else
		LoadTextureBank(TITLESGENERIC_TEX_BANK);
#endif

		currFont = smallFont;
		startText = CreateAndAddTextOverlay(100,112,"start game",YES,NO,255,255,255,255,currFont,TEXTOVERLAY_NORMAL,6,0);
		selectText = CreateAndAddTextOverlay(100,132,"level select",YES,NO,255,255,255,255,currFont,TEXTOVERLAY_NORMAL,6,0);
		multiText = CreateAndAddTextOverlay(100,152,mpText,YES,NO,255,255,255,255,currFont,TEXTOVERLAY_NORMAL,6,0);
		devText = CreateAndAddTextOverlay(100,172,"develop",YES,NO,255,255,255,255,currFont,TEXTOVERLAY_NORMAL,6,0);

		konami = CreateAndAddSpriteOverlay(240,35,"konami.bmp",32,32,255,255,255,192,0 );
		atari = CreateAndAddSpriteOverlay(40,35,"atari.bmp",32,32,255,255,255,192,0 );

		CreateOverlays();
		ResetParameters();
	}

	startText->a = 100;
	selectText->a = 100;
	multiText->a = 100;
	devText->a = 100;
	sprintf(mpText,"Multiplayer %i",numPlayers);
	button = controllerdata [ ActiveController ].button;

	if((button & CONT_UP) && !(lastbutton & CONT_UP))
    {
		if( currentSelection > 0 )
		{
			currentSelection--;
			PlaySampleNot3D(237,192,128,128);
		}
	}
	    
	if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
    {
		if( currentSelection < 3 )
		{
			currentSelection++;
			PlaySampleNot3D(237,192,128,128);
		}
	}
	
	if(frameCount > 15)
	{
		if(currentSelection == 2)
		{
			if((button & CONT_RIGHT) && !(lastbutton & CONT_RIGHT))
				if(numPlayers<MAX_FROGS)
					numPlayers++;

			if((button & CONT_LEFT) && !(lastbutton & CONT_LEFT))
				if(numPlayers>1)
					numPlayers--;
		}

		if(	(button & CONT_A) && !(lastbutton & CONT_A) ||
			(button & CONT_START) && !(lastbutton & CONT_START))
		{
			switch(currentSelection)
			{
				case 0:
					// start game
					FreeAllLists();
					InitLevel(WORLDID_GARDEN,LEVELID_GARDENLEV1);
					gameState.mode = INGAME_MODE;
					frameCount = 0;
					lastbutton = 0;
					NUM_FROGS = 1;
					gameState.multi = SINGLEPLAYER;
					break;

				case 1:
					// level select menu
					FreeAllLists();
					gameState.menuMode = LEVELSELECT_MODE;
					frameCount = 0;
					lastbutton = 0;
					break;
			
				case 2:
					// select multi player game
					FreeAllLists();
					gameState.multi = MULTILOCAL;
					gameState.menuMode = LEVELSELECT_MODE;
					frameCount = 0;
					lastbutton = 0;
					NUM_FROGS = numPlayers;
					break;
			
				case 3:
					// develepoment mode
					FreeAllLists();
					gameState.menuMode = DEVELOPMENT_MODE;
					frameCount = 0;
					lastbutton = 0;
					break;
			}

			return;		// DON'T try to do any menu stuff! Duh
		}			

		switch (currentSelection)
		{
			case 0: startText->a = 255; break;
			case 1: selectText->a = 255; break;
			case 2: multiText->a = 255; break;
			case 3: devText->a = 255; break;
		}
	}

	lastbutton = button;
}


/*	--------------------------------------------------------------------------------
	Function 	: RunLevelSelect
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void RunLevelSelect()
{
	static u16 button;
	static u16 lastbutton;

	static TEXTOVERLAY *worldSelText[MAX_WORLDS];
	static TEXTOVERLAY *levelSelText[MAX_LEVELS];

	static unsigned long lrSelect = 0;				// Select worlds or levels
	static unsigned long oldWorldSelect = 0;        // To check if selected world has changed
	static unsigned long currentWorldSelect = 0;	// Which world
	static unsigned long currentLevelSelect = 0;	// Which level

	static LNAMESTRUCT lNames[MAX_LEVELS];

	unsigned int i,j;

	if(frameCount == 1)
	{
		FreeAllLists();

		LoadTextureBank(SYSTEM_TEX_BANK);
#ifdef PC_VERSION
		LoadTextureBank(TITLES_TEX_BANK);
#else
		LoadTextureBank(TITLESGENERIC_TEX_BANK);
#endif

		currFont = smallFont;

		// draw list of worlds on left
		for( i=0; i < MAX_WORLDS; i++ )
			worldSelText[i] = CreateAndAddTextOverlay( 20, (i*20)+30,
														worldVisualData[i].description,
														NO,NO,255,255,255,100,
														currFont, TEXTOVERLAY_NORMAL,0,0 );

		// draw list of levels in current selected world on right
		for( i=0; i < MAX_LEVELS; i++ )
		{
			sprintf( lNames[i].name, worldVisualData[currentWorldSelect].levelVisualData[i].description );
			levelSelText[i] = CreateAndAddTextOverlay( 160, (i*20)+30,
														lNames[i].name,
														NO,NO,255,255,255,100,		
														currFont,TEXTOVERLAY_NORMAL,0,0);
		}

		ResetParameters();
	}


	// Reset alpha values
	worldSelText[currentWorldSelect]->a = 100;
	if(currentLevelSelect != 255)
		levelSelText[currentLevelSelect]->a = 100;

	button = controllerdata [ ActiveController ].button;

	// move between worlds or levels
	if((button & CONT_UP) && !(lastbutton & CONT_UP))
    {
		if( lrSelect == 0 && currentWorldSelect > 0 ) // World select
		{
			currentWorldSelect--;
			for( i=0; i < MAX_LEVELS; i++ )
			{
				if( worldVisualData[currentWorldSelect].levelVisualData[i].collBank != -1 )
				{
					currentLevelSelect = i;
					break;
				}
				else currentLevelSelect = 255;
			}
		}
		else if( lrSelect == 1 && currentLevelSelect > 0 ) // Level select
		{
			for( i=currentLevelSelect-1; i >= 0; i-- )
			{
				if( worldVisualData[currentWorldSelect].levelVisualData[i].collBank != -1 )
				{
					currentLevelSelect = i;
					break;
				}
			}
		}
	}
	    
	if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
    {
		if( lrSelect == 0 && currentWorldSelect < MAX_WORLDS-1 ) // World select
		{
			currentWorldSelect++;
			for( i=0; i < MAX_LEVELS; i++ )
			{
				if( worldVisualData[currentWorldSelect].levelVisualData[i].collBank != -1 )
				{
					currentLevelSelect = i;
					break;
				}
				else currentLevelSelect = 255;
			}
		}
		else if( lrSelect == 1 && currentLevelSelect < MAX_LEVELS )
		{
			for( i=currentLevelSelect+1; i < MAX_LEVELS; i++ )
			{
				if( worldVisualData[currentWorldSelect].levelVisualData[i].collBank != -1 )
				{
					currentLevelSelect = i;
					break;
				}
			}
		}
	}

	// move between world and level select
	if((button & CONT_LEFT) && !(lastbutton & CONT_LEFT))
		if(lrSelect == 1)
			lrSelect = 0;
	    
	if((button & CONT_RIGHT) && !(lastbutton & CONT_RIGHT))
		if(lrSelect == 0 && currentLevelSelect != 255)
			lrSelect = 1;

	//	check if world selection has changed. If so, update level list.

	if(currentWorldSelect != oldWorldSelect)
	{
		// change old level textovers
		for(i=0; i < MAX_LEVELS; i++)
			sprintf(lNames[i].name,worldVisualData[currentWorldSelect].levelVisualData[i].description);
	}


	//	don't allow menu changing for the first fifteen frames

	if(frameCount > 15)
	{
		// move back in menus
		if((button & CONT_B) && !(lastbutton & CONT_B))
		{
			FreeAllLists();
			gameState.menuMode = TITLE_MODE;
			frameCount = 0;
			lastbutton = 0;
			return;
		}

		// run selected world and level
		if( (((button & CONT_A) && !(lastbutton & CONT_A)) ||
			((button & CONT_START) && !(lastbutton & CONT_START)))
			&& currentLevelSelect != 255)
		{
			player[0].worldNum = currentWorldSelect;
			player[0].levelNum = currentLevelSelect;

			FreeAllLists();
			InitLevel(currentWorldSelect,currentLevelSelect);

			gameState.mode = INGAME_MODE;
				
			if(gameState.multi != MULTIREMOTE)
				gameState.multi = SINGLEPLAYER;

			frameCount = 0;
			lastbutton = 0;
			return;
		}			
	}

	// set selected text solid
	if(lrSelect == 0)
	{
		worldSelText[currentWorldSelect]->a = 255;
		if( currentLevelSelect != 255 ) levelSelText[currentLevelSelect]->a = 175;
	}
	else
	{
		worldSelText[currentWorldSelect]->a = 175;
		if( currentLevelSelect != 255 ) levelSelText[currentLevelSelect]->a = 255;
	}

	lastbutton = button;
	oldWorldSelect = currentWorldSelect;
}


/*	--------------------------------------------------------------------------------
	Function 	: RunPauseMenu
	Purpose 	: In game screen that appears when start is pressed.
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void RunPauseMenu()
{
	static unsigned long currentSelection = 0;
	static u16 button,lastbutton = 0;
	
	button = controllerdata[ActiveController].button;

	pauseTitle->a -= 16;

	if((button & CONT_UP) && !(lastbutton & CONT_UP))
		if(currentSelection > 0)
			currentSelection--;

	if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
		if(currentSelection < 1)
			currentSelection++;

	if((button & CONT_A) && !(lastbutton & CONT_A))
	{
		lastbutton = button;

		DisableTextOverlay ( pauseTitle );
		DisableTextOverlay ( continueText );
		DisableTextOverlay ( quitText );
		pauseMode = 0;

		switch(currentSelection)
		{
			case 0:   // continue game
			{
				long i;
				gameState.mode	= INGAME_MODE;

				livesTextOver->a = livesTextOver->oa;
				timeTextOver->a = timeTextOver->oa;
				scoreTextOver->a = scoreTextOver->oa;

				for ( i = 0; i < 3; i++ )
					sprHeart[i]->draw = 1;

				for(i=0; i<numBabies; i++)
					babyIcons[i]->draw = 1;

#ifdef N64_VERSION
				// free memory associated with screen grab effects
				FreeGrabData();
#endif
				return;
			}
			
			case 1:   // quit game
				FreeAllLists();

#ifndef USE_MENUS
				gameState.mode = INGAME_MODE;
				InitLevel(WORLDID_FRONTEND,LEVELID_FRONTEND2);
#else
				gameState.mode = MENU_MODE;
				gameState.menuMode = TITLE_MODE;
#endif

				frameCount = 0;
				lastbutton = 0;
				return;
		}

	}

	switch(currentSelection)
	{
		case 0:
			continueText->a = 255;
			continueText->r = continueText->g = continueText->b = 255;

			quitText->a = 91; //255;
			quitText->r = quitText->b = 255;
			quitText->g = 255;
			break;
		
		case 1:
			quitText->a = 255;
			quitText->r = quitText->g = quitText->b = 255;

			continueText->a = 91; //255;
			continueText->r = continueText->b = 255;
			continueText->g = 255;
			break;

	}

	lastbutton = button;
}


/*	--------------------------------------------------------------------------------
	Function		: CreateOverlays
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void CreateOverlays()
{
	// add the texture tiles that comprise the Frogger2 logo / Hasbro logo....
	sprOver = CreateAndAddSpriteOverlay(98,20,"flogo01.bmp",32,32,255,255,255,255,0);
	sprOver = CreateAndAddSpriteOverlay(130,20,"flogo02.bmp",32,32,255,255,255,255,0);
	sprOver = CreateAndAddSpriteOverlay(162,20,"flogo03.bmp",32,32,255,255,255,255,0);
	sprOver = CreateAndAddSpriteOverlay(194,20,"flogo04.bmp",32,32,255,255,255,255,0);
	sprOver = CreateAndAddSpriteOverlay(98,52,"flogo05.bmp",32,32,255,255,255,255,0);
	sprOver = CreateAndAddSpriteOverlay(130,52,"flogo06.bmp",32,32,255,255,255,255,0);
	sprOver = CreateAndAddSpriteOverlay(162,52,"flogo07.bmp",32,32,255,255,255,255,0);
	sprOver = CreateAndAddSpriteOverlay(194,52,"flogo08.bmp",32,32,255,255,255,255,0);
	sprOver = CreateAndAddSpriteOverlay(162,84,"flogo09.bmp",32,32,255,255,255,255,0);
	sprOver = CreateAndAddSpriteOverlay(194,84,"flogo10.bmp",32,32,255,255,255,255,0);
	
	konami = CreateAndAddSpriteOverlay(240,35,"konami.bmp",32,32,255,255,255,192,0 );
	atari = CreateAndAddSpriteOverlay(40,35,"atari.bmp",32,32,255,255,255,192,0 );
}
