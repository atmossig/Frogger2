/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: menus.c
	Programmer	: James Healey
	Date		: 27/05/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

#include <ultra64.h>

#include "incs.h"

TEXTOVERLAY *worldNames	[ MAX_WORLDS ];
TEXTOVERLAY *levelN		[ MAX_LEVELS ];
TEXTOVERLAY *closed;

SPRITEOVERLAY *atari = NULL;
SPRITEOVERLAY *konami = NULL;

SPRITE *sp = NULL;


/*	--------------------------------------------------------------------------------
	Function 	: RunTitleScreen
	Purpose 	: The first menu
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
char mpText[] = "Multiplayer 1";

void RunTitleScreen( )
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

	if ( frameCount == 1 )
	{
		StopDrawing ( "Title Screen" );
		FreeAllLists();

		LoadTextureBank(SYSTEM_TEX_BANK);
		LoadTextureBank(TITLES_TEX_BANK);

		//myBackdrop = CreateAndInitBackdrop ( NULL, "objvscrn.bmp", 0, 0, 0, 0, 0, 320, 240, 1024, 1024, 0 );

		currFont	= smallFont;
		startText = CreateAndAddTextOverlay(100,112,"Start Game",YES,NO,255,255,255,255,currFont,TEXTOVERLAY_NORMAL,6,0);
		selectText = CreateAndAddTextOverlay(100,132,"Level Select",YES,NO,255,255,255,255,currFont,TEXTOVERLAY_NORMAL,6,0);
		multiText = CreateAndAddTextOverlay(100,152,mpText,YES,NO,255,255,255,255,currFont,TEXTOVERLAY_NORMAL,6,0);
		devText = CreateAndAddTextOverlay(100,172,"develope",YES,NO,255,255,255,255,currFont,TEXTOVERLAY_NORMAL,6,0);

		konami = CreateAndAddSpriteOverlay(240,35,"konami.bmp",32,32,255,255,255,192,0 );
		atari = CreateAndAddSpriteOverlay(40,35,"atari.bmp",32,32,255,255,255,192,0 );

		CreateOverlays();
		ResetParameters();

		StartDrawing ( "Title Screen" );
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
			PlaySample ( 237, NULL, 255, 128 );
		}
	}
	    
	if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
    {
		if( currentSelection < 3 )
		{
			currentSelection++;
			PlaySample ( 237, NULL, 255, 128 );
		}
	}
	
	if(frameCount > 15)
	{
		if (currentSelection == 2)
		{
		if( (button & CONT_RIGHT) && !(lastbutton & CONT_RIGHT) )
			if (numPlayers<MAX_FROGS)
				numPlayers++;

		if( (button & CONT_LEFT) && !(lastbutton & CONT_LEFT) )
			if (numPlayers>1)
				numPlayers--;
		}

		if( (button & CONT_A) && !(lastbutton & CONT_A) )
		{
			//osMotorStart ( &rumble );

			switch ( currentSelection )
			{
			case 0:
				FreeAllLists();
				player[0].worldNum = 0;
				player[0].levelNum = 0;
				InitLevel ( 0, 0 );

				gameState.oldMode = FRONTEND_MODE;
				gameState.mode = GAME_MODE;

				frameCount = 0;
				lastbutton = 0;
				PlaySample ( 2,NULL,255,128);
				NUM_FROGS = 1;
				break;
			case 1:
				FreeAllLists();
				frameCount = 0;
				lastbutton = 0;
				frontEndState.mode = LEVELSELECT_MODE;
				PlaySample ( 2,NULL,255,128);
				break;
			case 2:
				FreeAllLists();
				frameCount = 0;
				lastbutton = 0;
				frontEndState.mode = LEVELSELECT_MODE;
				PlaySample ( 2,NULL,255,128);
				NUM_FROGS = numPlayers;
				break;
			case 3:
				FreeAllLists();
				gameState.oldMode = FRONTEND_MODE;
				frontEndState.mode = DEVELOPMENTMENU_MODE;
				frameCount = 0;
				lastbutton = 0;
				PlaySample ( 2,NULL,255,128);
				break;
			}
		}			

		if( (button & CONT_START) && !(lastbutton & CONT_START) )
		{
			StopDrawing( "Title Screen" );

			FreeAllLists();

#ifndef PC_VERSION
			ReadSaveSlot(0, 0);
#endif

			InitLevel( player[0].worldNum, player[0].levelNum );

			gameState.oldMode = FRONTEND_MODE;
			gameState.mode = GAME_MODE;

			frameCount = 0;
			lastbutton = 0;
			PlaySample ( 2,NULL,255,128);

			StartDrawing ( "Title Screen" );
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
void RunLevelSelect( )
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

	if ( frameCount == 1 )
	{
		StopDrawing ( "demo option" );

		FreeAllLists();

		LoadTextureBank(SYSTEM_TEX_BANK);
		LoadTextureBank(TITLES_TEX_BANK);

		currFont = smallFont;

		// Draw list of worlds on left
		for( i=0; i < MAX_WORLDS; i++ )
			worldSelText[i] = CreateAndAddTextOverlay( 20, (i*20)+30,
														worldVisualData[i].description,
														NO,NO,255,255,255,100,
														currFont, TEXTOVERLAY_NORMAL,0,0 );

		// Draw list of levels in current selected world on right
		for( i=0; i < MAX_LEVELS; i++ )
		{
			sprintf( lNames[i].name, worldVisualData[currentWorldSelect].levelVisualData[i].description );
			levelSelText[i] = CreateAndAddTextOverlay( 160, (i*20)+30,
														lNames[i].name,
														NO,NO,255,255,255,100,		
														currFont,TEXTOVERLAY_NORMAL,0,0);
		}

		ResetParameters();

		StartDrawing ( "demo option" );
	}


	// Reset alpha values
	worldSelText[currentWorldSelect]->a = 100;
	if( currentLevelSelect != 255 ) levelSelText[currentLevelSelect]->a = 100;

/*
*	Update buttons
*/
	button = controllerdata [ ActiveController ].button;

	// Move between worlds or levels
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
			PlaySample ( 237, NULL, 255, 128 );
		}
		else if( lrSelect == 1 && currentLevelSelect > 0 ) // Level select
		{
			for( i=currentLevelSelect-1; i >= 0; i-- )
			{
				if( worldVisualData[currentWorldSelect].levelVisualData[i].collBank != -1 )
				{
					currentLevelSelect = i;
					PlaySample ( 237, NULL, 255, 128 );
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
			PlaySample ( 237, NULL, 255, 128 );
		}
		else if( lrSelect == 1 && currentLevelSelect < MAX_LEVELS )
		{
			for( i=currentLevelSelect+1; i < MAX_LEVELS; i++ )
			{
				if( worldVisualData[currentWorldSelect].levelVisualData[i].collBank != -1 )
				{
					currentLevelSelect = i;
					PlaySample ( 237, NULL, 255, 128 );
					break;
				}
			}
		}
	}

	// Move between world and level select
	if((button & CONT_LEFT) && !(lastbutton & CONT_LEFT))
    {
		if( lrSelect == 1 )
		{
			lrSelect = 0;
			PlaySample ( 237, NULL, 255, 128 );
		}
	}
	    
	if((button & CONT_RIGHT) && !(lastbutton & CONT_RIGHT))
    {
		if( lrSelect == 0 && currentLevelSelect != 255 )
		{
			lrSelect = 1;
			PlaySample ( 237, NULL, 255, 128 );
		}
	}

/*
*	Check if world selection has changed. If so, update level list.
*/

	if( currentWorldSelect != oldWorldSelect )
	{
		// Change old level textovers
		for( i=0; i < MAX_LEVELS; i++ )
			sprintf( lNames[i].name, worldVisualData[currentWorldSelect].levelVisualData[i].description );
	}

/*
*	Don't allow menu changing for the first fifteen frames
*/
	if(frameCount > 15)
	{
		// Move back in menus
		if ((button & CONT_B) && !(lastbutton & CONT_B))
		{
			FreeAllLists();
			frameCount = 0;
			lastbutton = 0;
			frontEndState.mode	= TITLE_MODE;
			PlaySample ( 2,NULL,255,128);
			return;
		}

		// Run selected world and level
		if ( (button & CONT_A) && !(lastbutton & CONT_A) && currentLevelSelect != 255 )
		{
			StopDrawing( "Title Screen" );

			FreeAllLists();

			player[0].worldNum = currentWorldSelect;
			player[0].levelNum = currentLevelSelect;

			InitLevel ( currentWorldSelect, currentLevelSelect );

			gameState.oldMode = FRONTEND_MODE;
			gameState.mode = GAME_MODE;

			frameCount = 0;
			lastbutton = 0;
			PlaySample ( 2,NULL,255,128);

			StartDrawing ( "Title Screen" );
			return;
		}			
	}

	// Set selected text solid
	if( lrSelect == 0 )
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
void RunPauseMenu( )
{
	static unsigned long currentSelection = 0;
	static u16 button,lastbutton = 0;

	button = controllerdata[ActiveController].button;

	pauseTitle->a -= 16;

	if((button & CONT_UP) && !(lastbutton & CONT_UP))
    {
		if(currentSelection > 0)
		{
			currentSelection--;
			PlaySample ( 237,NULL,255,128);
		}
	}

	if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
    {
		if(currentSelection < 1)
		{
			currentSelection++;
			PlaySample ( 237,NULL,255,128);
		}
	}

	if( (button & CONT_A) && !(lastbutton & CONT_A) )
	{
		lastbutton = button;
		PlaySample(2,NULL,255,128);

		DisableTextOverlay ( pauseTitle );
		DisableTextOverlay ( continueText );
		DisableTextOverlay ( quitText );
		testPause = 0;

		/*
		if(backPanel)
		{
			backPanel->xPos		= 50;
			backPanel->yPos		= 83;
			backPanel->width	= 220;
			backPanel->height	= 50;
			backPanel->r		= 15;
			backPanel->g		= 63;
			backPanel->b		= 255;
			backPanel->a		= 127;
			backPanel->draw		= 0;
		}
		*/
		switch (currentSelection)
		{
			case 0:   // Continue Game
			{
				gameState.mode	= GAME_MODE;
				EnableHUD( );
				grabData.afterEffect = PAUSE_EXIT;
				return;
			}
			case 1:   // Quit Game
				StopDrawing("Quit Game");
				FreeAllLists();
				gameState.mode = FRONTEND_MODE;
				frameCount = 0;
				StartDrawing("Quit Game");
				return;
		}

	}

	switch (currentSelection)
	{
		case 0:
//			continueText->a = 255 * fabs(sinf(frameCount/12.5));
//			continueText->waveAmplitude = 6.0F;
			continueText->a = 255;
			continueText->r = continueText->g = continueText->b = 255;

			quitText->a = 91; //255;
			quitText->r = quitText->b = 255;
			quitText->g = 255;

			//if(quitText->waveAmplitude > 1)
			//	quitText->waveAmplitude -= 0.10F;
			break;
		
		case 1:
			//quitText->a = 255 * fabs(sinf(frameCount/12.5));
			//quitText->waveAmplitude = 6.0F;
			quitText->a = 255;
			quitText->r = quitText->g = quitText->b = 255;

			continueText->a = 91; //255;
			continueText->r = continueText->b = 255;
			continueText->g = 255;

			//if(continueText->waveAmplitude > 1)
			//	continueText->waveAmplitude -= 0.10F;
			break;

	}

	lastbutton = button;
}





























































































































/*********************************************
********** This stuff is no longer used !*****
*********************************************/

#ifdef OLDMENUSTUFF

typedef struct _s
{
	char scoreTxt[32];
	char worldTxt[32];
	char levelTxt[32];
} s;


unsigned char enteredName[3] = "000";

char let1;
char let2;
char let3;

s slots[NUM_SAVE_SLOTS];

typedef struct _lscores
{
	char scoreTxt[32];
} lscores;

lscores scoresTxt[3];

TEXTOVERLAY *scoreNames [ 3 ];
TEXTOVERLAY *scores		[ 3 ];

SPRITEOVERLAY *soundIcon		= NULL;
SPRITEOVERLAY *controllerIcon	= NULL;

static VECTOR vecPos;

TEXTOVERLAY *tablePosition	[ MAX_HISCORE_SLOTS ];
TEXTOVERLAY *tableNames		[ MAX_HISCORE_SLOTS ];
TEXTOVERLAY *tableScores	[ MAX_HISCORE_SLOTS ];


struct slotSelectStateStruct slotSelectState;

void RunSaveLoadSelect ( void )
{
	int c;
	static unsigned long currentOptionSelection = 0;
	static unsigned long currentSlotSelection = 0;
	static unsigned long currentSelection = 0;
	static unsigned long currentLetterSelection = 0;
	static unsigned long currentLetterSlot = 0;
	static u16 button;
	static u16 lastbutton;


	static TEXTOVERLAY *selectEmptySlot = NULL;

	static TEXTOVERLAY *slotNames[NUM_SAVE_SLOTS];
	static TEXTOVERLAY *slotScores[NUM_SAVE_SLOTS];
	static TEXTOVERLAY *slotWorlds[NUM_SAVE_SLOTS];
	static TEXTOVERLAY *slotLevels[NUM_SAVE_SLOTS];

	static TEXTOVERLAY *continueText;
	static TEXTOVERLAY *newGameText;
	static TEXTOVERLAY *noSlotText;

	static TEXTOVERLAY *yesNoText;
	static TEXTOVERLAY *yesText;
	static TEXTOVERLAY *noText;

	static TEXTOVERLAY *letters[26];

	static TEXTOVERLAY *name;

	if ( frameCount == 1 )
	{
		StopDrawing ( "save load select" );

		FreeAllLists();

		memset (enteredName,0,3);

		LoadTextureBank(SYSTEM_TEX_BANK);
		LoadTextureBank(TITLES_TEX_BANK);

		currFont	= bigFont;
		letters[0] = CreateAndAddTextOverlay(42,32,"a",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[1] = CreateAndAddTextOverlay(74,32,"b",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[2] = CreateAndAddTextOverlay(106,32,"c",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[3] = CreateAndAddTextOverlay(138,32,"d",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[4] = CreateAndAddTextOverlay(170,32,"e",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[5] = CreateAndAddTextOverlay(202,32,"f",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[6] = CreateAndAddTextOverlay(234,32,"g",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[7] = CreateAndAddTextOverlay(42,64,"h",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[8] = CreateAndAddTextOverlay(74,64,"i",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[9] = CreateAndAddTextOverlay(106,64,"j",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[10] = CreateAndAddTextOverlay(138,64,"k",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[11] = CreateAndAddTextOverlay(170,64,"l",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[12] = CreateAndAddTextOverlay(202,64,"m",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[13] = CreateAndAddTextOverlay(234,64,"n",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[14] = CreateAndAddTextOverlay(42,96,"o",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[15] = CreateAndAddTextOverlay(74,96,"p",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[16] = CreateAndAddTextOverlay(106,96,"q",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[17] = CreateAndAddTextOverlay(138,96,"r",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[18] = CreateAndAddTextOverlay(170,96,"s",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[19] = CreateAndAddTextOverlay(202,96,"t",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[20] = CreateAndAddTextOverlay(234,96,"u",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[21] = CreateAndAddTextOverlay(42,128,"v",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[22] = CreateAndAddTextOverlay(74,128,"w",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[23] = CreateAndAddTextOverlay(106,128,"x",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[24] = CreateAndAddTextOverlay(138,128,"y",NO,NO,255,255,255,255,currFont,0,0,0);
		letters[25] = CreateAndAddTextOverlay(170,128,"z",NO,NO,255,255,255,255,currFont,0,0,0);

		name = CreateAndAddTextOverlay(0,160,"000",YES,NO,255,255,255,255,currFont,0,0,0);
		DisableTextOverlay ( name );

		for ( c = 0; c < 26; c++ )
			DisableTextOverlay ( letters[c] );
		// ENDFOR

		currFont			= smallFont;
		selectEmptySlot		= CreateAndAddTextOverlay(0,16,"select memory slot",YES,NO,255,255,255,255,currFont,0,0,0);

		continueText = CreateAndAddTextOverlay(0,144,"continue",YES,NO,255,255,255,255,currFont,0,0,0);
		newGameText  = CreateAndAddTextOverlay(0,160,"new game",YES,NO,255,255,255,255,currFont,0,0,0);
		noSlotText   = CreateAndAddTextOverlay(0,176,"no slot",YES,NO,255,255,255,255,currFont,0,0,0);

		yesNoText	= CreateAndAddTextOverlay(0,98,"are you sure???",YES,NO,255,255,255,255,currFont,0,0,0);
		DisableTextOverlay ( yesNoText );

		currFont	= bigFont;
		yesText		= CreateAndAddTextOverlay(0,128,"yes",YES,NO,255,255,255,255,currFont,0,0,0);
		noText		= CreateAndAddTextOverlay(0,160,"no",YES,NO,255,255,255,255,currFont,0,0,0);

		DisableTextOverlay ( yesText );
		DisableTextOverlay ( noText );

		for ( c = 0; c < NUM_SAVE_SLOTS; c++ )
		{
			currFont		 = smallFont;
			slotNames  [ c ] = CreateAndAddTextOverlay(18+(c*(16*3)),48,saveSlot[c].name,NO,NO,255,255,255,255,currFont,0,0,0);
			currFont = oldeFont;

			slotScores [ c ] = CreateAndAddTextOverlay(0,80,slots[c].scoreTxt,YES,NO,255,255,255,255,currFont,0,0,0);
			sprintf ( slots[c].scoreTxt, "Score: %d", saveSlot[c].score );

			slotWorlds [ c ] = CreateAndAddTextOverlay(0,96,slots[c].worldTxt,YES,NO,255,255,255,255,currFont,0,0,0);
			sprintf ( slots[c].worldTxt, "World: %s", worldVisualData[saveSlot[c].currentWorld].description );

			slotLevels [ c ] = CreateAndAddTextOverlay(0,112,slots[c].levelTxt,YES,NO,255,255,255,255,currFont,0,0,0);
			sprintf ( slots[c].levelTxt, "Level: %s", worldVisualData[saveSlot[c].currentWorld].levelVisualData[saveSlot[c].currentLevel].description );

			DisableTextOverlay ( slotScores [ c ] );
			DisableTextOverlay ( slotWorlds [ c ] );
			DisableTextOverlay ( slotLevels [ c ] );
		}

		CreateAndAddSpriteOverlay(200,205,"isl1.bmp",32,32,255,255,255,120,0);
		CreateAndAddSpriteOverlay(232,205,"isl2.bmp",32,32,255,255,255,120,0);
		CreateAndAddSpriteOverlay(264,205,"isl3.bmp",32,32,255,255,255,120,0);

		slotSelectState.mode = OPTION_MODE;

		ResetParameters();

//		runningDevStuff = 1;

		StartDrawing ( "save load select" );
	}
	// ENDIF

	button = controllerdata [ ActiveController ].button;

	switch ( slotSelectState.mode )
	{
		case OPTION_MODE:

				if((button & CONT_UP) && !(lastbutton & CONT_UP))
				{
					if(currentOptionSelection > 0)
					{
						currentOptionSelection--;
						PlaySample ( 237,NULL,255,128);
					}
				}
			
				if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
				{
					if(currentOptionSelection < 3)
					{
						currentOptionSelection++;
						PlaySample ( 237,NULL,255,128);
					}
				}

				if ( ( button & CONT_A ) && !( lastbutton & CONT_A ) )
				{
					switch ( currentOptionSelection )
					{
						case 0:
								slotSelectState.mode = SLOT_MODE;
								lastbutton		= 0;
							break;
						case 1:
								slotSelectState.mode = SLOT_MODE;
								lastbutton		= 0;
							break;
						case 2:
//								runningDevStuff = 0;
								FreeAllLists();
								frontEndState.mode = TITLE_MODE;
								frameCount		= 0;
								lastbutton		= 0;
							break;
					}
				}
				// ENDIF - ( button & CONT_A ) && !( lastbutton & CONT_A )
				switch (currentOptionSelection)
				{
					case 0:
						continueText->a = 255;
						continueText->r = continueText->g = continueText->b = 255;

						newGameText->a = 91; //255;
						newGameText->r = newGameText->b = 200;
						newGameText->g = 255;
						noSlotText->a = 91; //255;
						noSlotText->r = noSlotText->b = 200;
						noSlotText->g = 255;
						break;
					case 1:
						newGameText->a = 255;
						newGameText->r = newGameText->g = newGameText->b = 255;

						continueText->a = 91; //255;
						continueText->r = continueText->b = 200;
						continueText->g = 255;
						noSlotText->a = 91; //255;
						noSlotText->r = noSlotText->b = 200;
						noSlotText->g = 255;
						break;
					case 2:
						noSlotText->a = 255;
						noSlotText->r = noSlotText->g = noSlotText->b = 255;

						newGameText->a = 91; //255;
						newGameText->r = newGameText->b = 200;
						newGameText->g = 255;
						continueText->a = 91; //255;
						continueText->r = continueText->b = 200;
						continueText->g = 255;
						break;
				}
			break;
		case SLOT_MODE:
				if((button & CONT_LEFT) && !(lastbutton & CONT_LEFT))
				{
					if(currentSlotSelection > 0)
					{
						currentSlotSelection--;
						PlaySample ( 237,NULL,255,128);
					}
				}
			
				if((button & CONT_RIGHT) && !(lastbutton & CONT_RIGHT))
				{
					if(currentSlotSelection < 5)
					{
						currentSlotSelection++;
						PlaySample ( 237,NULL,255,128);
					}
				}

				if ( ( button & CONT_B ) && !( lastbutton & CONT_B ) )
				{
					for ( c = 0; c < 6; c++ )
					{
						slotNames[c]->a = 91;
						slotNames[c]->r = 200;
						slotNames[c]->b = 200;
						slotNames[c]->g = 255;
						DisableTextOverlay ( slotScores[c] );
						DisableTextOverlay ( slotWorlds[c] );
						DisableTextOverlay ( slotLevels[c] );
					}
					// ENDFOR
					slotSelectState.mode = OPTION_MODE;
					lastbutton		= 0;
					return;
				}
				// ENDIF

				if ( ( button & CONT_A ) && !( lastbutton & CONT_A ) )
				{
					if ( currentOptionSelection == 1 )
					{
						if ( gstrcmp ( saveSlot [ currentSlotSelection ].name, "" ) == 0 )
						{
						}
						else
						{
							DisableTextOverlay ( continueText );
							DisableTextOverlay ( newGameText );
							DisableTextOverlay ( noSlotText );

							EnableTextOverlay ( yesNoText );
							EnableTextOverlay ( yesText );
							EnableTextOverlay ( noText );
							for ( c = 0; c < 6; c++ )
							{
								DisableTextOverlay ( slotScores[c] );
								DisableTextOverlay ( slotWorlds[c] );
								DisableTextOverlay ( slotLevels[c] );
							}
							// ENDFOR
							slotSelectState.mode = YESNO_MODE;
							lastbutton = button;
							return;

						}
						// ENDELSEIF
					}
					// ENDIF
				}
				// ENDIF - ( button & CONT_A ) && !( lastbutton & CONT_A )
				for ( c = 0; c < 6; c++ )
				{
					if ( c != currentSlotSelection )
					{
						slotNames[c]->a = 91;
						slotNames[c]->r = 200;
						slotNames[c]->b = 200;
						slotNames[c]->g = 255;
						DisableTextOverlay ( slotScores[c] );
						DisableTextOverlay ( slotWorlds[c] );
						DisableTextOverlay ( slotLevels[c] );
					}
					else
					{
						slotNames[c]->a = 255;
						slotNames[c]->r = 255;
						slotNames[c]->g = 255;
						slotNames[c]->b = 255;
						EnableTextOverlay ( slotScores[c] );
						EnableTextOverlay ( slotWorlds[c] );
						EnableTextOverlay ( slotLevels[c] );
					}
					// ENDIF
				}
				// ENDFOR


			break;
		case YESNO_MODE:

				if((button & CONT_UP) && !(lastbutton & CONT_UP))
				{
					if(currentSelection > 0)
					{
						currentSelection--;
						PlaySample ( 237,NULL,255,128);
					}
				}
			
				if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
				{
					if(currentSelection < 1)
					{
						currentSelection++;
						PlaySample ( 237,NULL,255,128);
					}
				}

				/*if ( ( button & CONT_A ) && !( lastbutton & CONT_A ) )
				{
					for ( c = 0; c < 6; c++ )
					{
						slotNames[c]->a = 91;
						slotNames[c]->r = 200;
						slotNames[c]->b = 200;
						slotNames[c]->g = 255;
						DisableTextOverlay ( slotScores[c] );
						DisableTextOverlay ( slotWorlds[c] );
						DisableTextOverlay ( slotLevels[c] );
					}
					// ENDFOR
					slotSelectState.mode = OPTION_MODE;
					lastbutton		= 0;
					return;
				}
				// ENDIF
						 */
				if ( ( button & CONT_A ) && !( lastbutton & CONT_A ) )
				{
					switch ( currentSelection )
					{
						case 0:
								for ( c = 0; c < 26; c++ )
									EnableTextOverlay ( letters[c] );
								// ENDFOR
								for ( c = 0; c < NUM_SAVE_SLOTS; c++ )
								{
									DisableTextOverlay ( slotNames[c] );
								}
								// ENDFOR
								EnableTextOverlay ( name );

								DisableTextOverlay ( yesNoText );
								DisableTextOverlay ( yesText );
								DisableTextOverlay ( noText );

								DisableTextOverlay ( continueText );
								DisableTextOverlay ( newGameText );
								DisableTextOverlay ( noSlotText );
								slotSelectState.mode = NAME_MODE;
								lastbutton		= 0;
							break;
						case 1:
								//FreeAllLists();
								slotSelectState.mode = SLOT_MODE;
								//frameCount		= 0;
								lastbutton		= 0;
								//runningDevStuff = 0;
								DisableTextOverlay ( yesNoText );
								DisableTextOverlay ( yesText );
								DisableTextOverlay ( noText );

								EnableTextOverlay ( continueText );
								EnableTextOverlay ( newGameText );
								EnableTextOverlay ( noSlotText );
//								return;
							break;
					}	  
				}
				// ENDIF - ( button & CONT_A ) && !( lastbutton & CONT_A )
				switch (currentSelection)
				{
					case 0:
						yesText->a = 255 * fabs(sinf(frameCount/12.5));
						yesText->waveAmplitude = 6.0F;
						yesText->r = yesText->g = yesText->b = 255;

						noText->a = 91; //255;
						noText->r = noText->b = 200;
						noText->g = 255;
						break;
					case 1:
						noText->a = 255 * fabs(sinf(frameCount/12.5));
						noText->waveAmplitude = 6.0F;
						noText->r = noText->g = noText->b = 255;

						yesText->a = 91; //255;
						yesText->r = yesText->b = 200;
						yesText->g = 255;
						break;
				}

				  
			break;
		case NAME_MODE:
				if((button & CONT_LEFT) && !(lastbutton & CONT_LEFT))
				{
					if(currentLetterSelection > 0)
					{
						currentLetterSelection--;
						PlaySample ( 237,NULL,255,128);
					}
				}
				// ENDIF
			
				if((button & CONT_RIGHT) && !(lastbutton & CONT_RIGHT))
				{
					if(currentLetterSelection < 25)
					{
						currentLetterSelection++;
						PlaySample ( 237,NULL,255,128);
					}
				}
				// ENDIF

				if((button & CONT_UP) && !(lastbutton & CONT_UP))
				{
					if(currentLetterSelection > 0+6)
					{
						currentLetterSelection -= 7;
						PlaySample ( 237,NULL,255,128);
					}
				}
				// ENDIF

				if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
				{
					if ( currentLetterSelection < 26-7 )
					{
						currentLetterSelection += 7;
						PlaySample ( 237,NULL,255,128);
					}
				}
				// ENDIF
				if ( ( button & CONT_A ) && !( lastbutton & CONT_A ) && ( currentLetterSlot < 3 ) )
				{
					enteredName[currentLetterSlot] = letters[currentLetterSelection]->text[0];
					currentLetterSlot++;
					sprintf ( name->text, enteredName );
					if ( currentLetterSlot == 3 )
					{
						sprintf ( player[0].name, "%s", enteredName );
//						runningDevStuff = 0;
						FreeAllLists();
						frameCount		= 0;
						lastbutton		= 0;
						frontEndState.mode = TITLE_MODE;
						return;
					}
					// ENDIF
				}
				// ENDIF

				if ( ( button & CONT_B ) && !( lastbutton & CONT_B ) && ( currentLetterSlot > 0 ) )
				{
					currentLetterSlot--;
					enteredName[currentLetterSlot] = '0';
					sprintf ( name->text, enteredName );
				}
				// ENDIF

				if ( ( button & CONT_START ) && !( lastbutton & CONT_START ) )
				{
					frontEndState.mode	= TITLE_MODE;
					sprintf ( player[0].name, enteredName );
				}
				// ENDIF

				for ( c = 0; c < 26; c++ )
				{
					if ( c != currentLetterSelection )
					{
						letters[c]->a = 91;
						letters[c]->r = 200;
						letters[c]->b = 200;
						letters[c]->g = 255;
					}
					else
					{
						letters[c]->a = 255;
						letters[c]->r = 255;
						letters[c]->g = 255;
						letters[c]->b = 255;
					}
					// ENDIF
				}
				// ENDFOR
				
			break;

	}

	lastbutton = button;
}



void RunTitleScreen()
{
	static unsigned long currentSelection = 0;
	static u16 button;
	static u16 lastbutton;
	static TEXTOVERLAY *start,*options,*dev;
	int i = 0,xPos,j;

	if(frameCount == 1)
	{	
		StopDrawing("title screen");

		FreeAllLists();

		LoadTextureBank(SYSTEM_TEX_BANK);
		LoadTextureBank(TITLES_TEX_BANK);

//		levelPlayingTimer =  ( 60 / desiredFrameRate ) * numSecs;

		currFont	= smallFont;
		start		= CreateAndAddTextOverlay(100,122,"start",YES,NO,255,255,255,255,currFont,TEXTOVERLAY_WAVECHARS,1,0);
		options		= CreateAndAddTextOverlay(100,140,"options",YES,NO,255,255,255,255,currFont,TEXTOVERLAY_WAVECHARS,1,0);
		dev			= CreateAndAddTextOverlay(100,158,"development",YES,NO,255,255,255,255,currFont,TEXTOVERLAY_WAVECHARS,1,0);
					
		CreateOverlays();

		ResetParameters();

//		runningDevStuff = 1;

		StartDrawing("title screen");
	}	

		
	button = controllerdata[ActiveController].button;

	if((button & CONT_UP) && !(lastbutton & CONT_UP))
    {
		if(currentSelection > 0)
		{
			currentSelection--;
	//		levelPlayingTimer =  ( 60 / desiredFrameRate ) * numSecs;
			PlaySample ( 237,NULL,255,128);
		}
	}
	    
	if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
    {
		if(currentSelection < 2)
		{
			currentSelection++;
	//		levelPlayingTimer =  ( 60 / desiredFrameRate ) * numSecs;
			PlaySample ( 237,NULL,255,128);
		}
	}
	
	if(frameCount > 15)
	{
		if((button & CONT_B) && !(lastbutton & CONT_B))
		{
			FreeAllLists();
			frontEndState.mode = TITLE_MODE;
			frameCount = 0;
			lastbutton = 0;
//			runningDevStuff = 0;
			return;
		}

		if (((button & CONT_A) && !(lastbutton & CONT_A)) ||
		   ((button & CONT_START) && !(lastbutton & CONT_START)))
		{
//			runningDevStuff = 0;
			frameCount = 0;
			lastbutton = 0;
			FreeAllLists();
			PlaySample(2,NULL,255,128);
			switch (currentSelection)
			{
				case 0:   // Start Game
					frontEndState.mode	= GAMETYPE_MODE;
					return;
				case 1:   // Options Mode
					frontEndState.mode	= OPTIONS_MODE;
					return;
				case 2:   // Development Mode
					frontEndState.mode	= DEVELOPMENTMENU_MODE;
					return;
			}
		}			
	
		switch (currentSelection)
		{
			case 0:
				start->a = 255 * fabs(sinf(frameCount/12.5));
				start->waveAmplitude = 6.0F;
				start->r = start->g = start->b = 255;

				options->a = 91; //255;
				options->r = options->b = 200;
				options->g = 255;
				dev->a = 91; //255;
				dev->r = dev->b = 200;
				dev->g = 255;

				if(options->waveAmplitude > 1)
					options->waveAmplitude -= 0.10F;
				if(dev->waveAmplitude > 1)
					dev->waveAmplitude -= 0.10F;
				break;
			
			case 1:
				options->a = 255 * fabs(sinf(frameCount/12.5));
				options->waveAmplitude = 6.0F;
				options->r = options->g = options->b = 255;

				start->a = 91; //255;
				start->r = start->b = 200;
				start->g = 255;
				dev->a = 91; //255;
				dev->r = dev->b = 200;
				dev->g = 255;

				if(start->waveAmplitude > 1)
					start->waveAmplitude -= 0.10F;
				if(dev->waveAmplitude > 1)
					dev->waveAmplitude -= 0.10F;
				break;

			case 2:
				dev->a = 255 * fabs(sinf(frameCount/12.5));
				dev->waveAmplitude = 6.0F;
				dev->r = dev->g = dev->b = 255;

				start->a = 91; //255;
				start->r = start->b = 200;
				start->g = 255;
				options->a = 91; //255;
				options->r = options->b = 200;
				options->g = 255;

				if(start->waveAmplitude > 1)
					start->waveAmplitude -= 0.10F;
				if(options->waveAmplitude > 1)
					options->waveAmplitude -= 0.10F;
				break;
		}
/*		levelPlayingTimer--;
		if ( levelPlayingTimer == 0 )
		{
			FreeActorList();
			FreePlatformList();
			FreeEnemyList();
			FreeSpriteOverlayList();
//			FreeTextOverlayList();
			FreeAnimationList();
			FreeTextureList();
			FreeBackdrop(myBackdrop);
			myBackdrop = NULL;
			FreeLevel();
			FreeAllObjectBanks();
			FreeAllTextureBanks();

			frameCount = 0;
			lastbutton = 0;

			InitLevel ( 2,0 );
			gameState.mode = LEVELPLAYING_MODE;
			autoPlaying = 1;
			FreeBackdrop(myBackdrop);
			myBackdrop = NULL;
			return;

		}
*/		// endif - levelPlayingTimer == 0
	}

	lastbutton = button;
}

void RunLevelSelect ( void )
{
	static u16 button;
	static u16 lastbutton;
	static unsigned long currentSelection		= 0;
	static unsigned long currentLevelSelection	= 0;
	static unsigned long worldSelecting			= 1;
	static unsigned long levelSelecting			= 0;

	unsigned long i;
	unsigned long c;

	if ( frameCount == 1 )
	{
		StopDrawing  ( "levelsel" );

		FreeAllLists();

		LoadTextureBank ( SYSTEM_TEX_BANK );
		LoadTextureBank ( TITLES_TEX_BANK );

		currFont	= smallFont;
		closed		= CreateAndAddTextOverlay ( 160, 115, "closed", NO, NO, 255, 255, 255, 255, smallFont, 0, 0, 0 );
																						  
		for ( i = 0; i < 3; i++ )
		{
			scoreNames[i]	= CreateAndAddTextOverlay ( 180, 140+(20*i), levelTable[(currentSelection+1)*i].name, NO, NO, 255, 255, 255, 255, smallFont, 0, 0, 0 );
			scores[i]		= CreateAndAddTextOverlay ( 230, 140+(20*i), scoresTxt[i].scoreTxt, NO, NO, 255, 255, 255, 255, smallFont, 0, 0, 0 );
			sprintf ( scoresTxt[i].scoreTxt, "%i", levelTable[(currentSelection+1)*i].score );
		}
		// ENDFOR
		for ( i = 0; i < MAX_WORLDS; i++ )
		{
			worldNames[i]	    = CreateAndAddTextOverlay ( 20, 20, worldVisualData [ i ].description, YES, NO, 255, 255, 255, 255, smallFont, 0, 0, 0 );
			DisableTextOverlay ( worldNames[i] );
		}
		// ENDFOR
		EnableTextOverlay ( worldNames [ currentSelection ] );
		if ( worldVisualData[ currentSelection ].worldOpen & WORLD_OPEN )
		{
			DisableTextOverlay ( closed );
			for ( c = 0; c < MAX_LEVELS; c++ )
			{
				if ( worldVisualData [ currentSelection ].levelVisualData[c].levelOpen & LEVEL_OPEN )
					levelN[c]	 = CreateAndAddTextOverlay ( 20, 50+(20*c), worldVisualData[currentSelection].levelVisualData[c].description, NO, NO, 255, 255, 255, 255, smallFont, 0, 0, 0 );
				// ENDIF
			}
			// ENDFOR
			levelN[currentSelection]->a = 255;
		}
		else
		{
			for ( i = 0; i < MAX_LEVELS; i++ )
			{
				if ( levelN[i] )
					DisableTextOverlay ( levelN[i] );
			}
			// ENDFOR
			EnableTextOverlay ( closed );
		}
		// ENDELSEIF

		CreateAndAddSpriteOverlay(200,185,"isl1.bmp",32,32,255,255,255,255,0);
		CreateAndAddSpriteOverlay(232,185,"isl2.bmp",32,32,255,255,255,255,0);
		CreateAndAddSpriteOverlay(264,185,"isl3.bmp",32,32,255,255,255,255,0);

//		runningDevStuff = 1;

		StartDrawing ( "levelsel" );
	}
	// ENDIF

	button = controllerdata [ ActiveController ].button;

	/*if ( ( button & CONT_B ) && !( lastbutton & CONT_B ) )
	{
		runningDevStuff = 0;
		FreeAllLists();
		frontEndState.mode	= GAMETYPE_MODE;
		frameCount		= 0;
		lastbutton		= 0;
		return;
	}
	// ENDIF
	   */
	if ( frameCount > 15 )
	{
		if ( ( button & CONT_L ) && !( lastbutton & CONT_L ) )
		{
			if ( currentSelection > 0 )
			{
				DisableTextOverlay ( worldNames[currentSelection] );
				currentSelection--;
				EnableTextOverlay ( worldNames[currentSelection] );
				if ( worldVisualData[currentSelection].worldOpen & WORLD_OPEN )
				{
					for ( i = 0; i < MAX_LEVELS; i++ )
					{
						if ( levelN[i] )
						{
							levelN[i]->text = worldVisualData[currentSelection].levelVisualData[i].description;
							EnableTextOverlay ( levelN[i] );
						}
						// ENDIF
					}
					// ENDFOR
					DisableTextOverlay ( closed );
				}
				else
				{
					for ( i = 0; i < MAX_LEVELS; i++ )
					{
						if ( levelN[i] )
							DisableTextOverlay ( levelN[i] );
						// ENDIF
					}
					// ENDFOR
					EnableTextOverlay ( closed );
				}
				// ENDELSEIF
			}
			// ENDIF
		}
		// ENDIF


		if ( ( button & CONT_R ) && !( lastbutton & CONT_R ) )
		{
			if ( currentSelection < (MAX_WORLDS-1) )
			{
				DisableTextOverlay ( worldNames[currentSelection] );
				currentSelection++;
				EnableTextOverlay ( worldNames[currentSelection] );
				if ( worldVisualData[currentSelection].worldOpen & WORLD_OPEN )
				{
					for ( i = 0; i < MAX_LEVELS; i++ )
					{
						if ( levelN[i] )
						{
							levelN[i]->text = worldVisualData[currentSelection].levelVisualData[i].description;
							EnableTextOverlay ( levelN[i] );
						}
						// ENDIF
					}
					// ENDFOR
					DisableTextOverlay ( closed );
				}
				else
				{
					for ( i = 0; i < MAX_LEVELS; i++ )
					{
						if ( levelN[i] )
							DisableTextOverlay ( levelN[i] );
					}
					// ENDFOR
					EnableTextOverlay ( closed );
				}
				// ENDELSEIF
			}
			// ENDIF
		}
		// ENDIF

		if ( ( button & CONT_UP ) && !( lastbutton & CONT_UP ) )
		{
			if ( currentLevelSelection > 0 )
			{
				currentLevelSelection--;
				levelN [ currentLevelSelection ]->a		= 255;
				levelN [ currentLevelSelection ]->a		= 255;
				levelN [ currentLevelSelection+1 ]->a	= 50;
				levelN [ currentLevelSelection+1 ]->a	= 50;
			}
			// ENDIF
		}
		// ENDIF
		if ( ( button & CONT_DOWN ) && !( lastbutton & CONT_DOWN ) )
		{
			if ( currentLevelSelection < (MAX_LEVELS-1) )
			{
				if ( worldVisualData [ currentSelection ].levelVisualData[currentLevelSelection+1].levelOpen & LEVEL_OPEN )
				{
					currentLevelSelection++;
					levelN [ currentLevelSelection ]->a		= 255;
					levelN [ currentLevelSelection ]->a		= 255;
					levelN [ currentLevelSelection-1 ]->a	= 50;
					levelN [ currentLevelSelection-1 ]->a	= 50;
				}
				// ENDIF
			}
			// ENDIF
		}
		// ENDIF
		  
		/*if((button & CONT_LEFT) && !(lastbutton & CONT_LEFT))
		{
			worldSelecting  = 1;
			levelSelecting  = 0;
			worldNames [ currentSelection ]->a		= 255;
			worldNames [ currentSelection ]->a		= 255;
			levelN [ currentLevelSelection ]->a		= 140;
			levelN [ currentLevelSelection ]->a		= 140;
		}
		*/	
		/*if ( ( button & CONT_RIGHT) && !(lastbutton & CONT_RIGHT) &&
		     ( worldVisualData[currentSelection].worldOpen & WORLD_OPEN ) )
		{
			worldSelecting  = 0;
			levelSelecting  = 1;
			worldNames [ currentSelection ]->a		= 140;
			worldNames [ currentSelection ]->a		= 140;
			levelN [ currentLevelSelection ]->a		= 255;
			levelN [ currentLevelSelection ]->a		= 255;
		}
		*/
		if((button & CONT_A) && !(lastbutton & CONT_A))
		{
			if ( worldVisualData[currentSelection].levelVisualData[currentLevelSelection].levelOpen & LEVEL_OPEN )
			{
//				runningDevStuff = 0;
				FreeAllLists();
				player[0].worldNum = currentSelection;
				player[0].levelNum = currentLevelSelection;
				InitLevel ( currentSelection, currentLevelSelection );
				gameState.oldMode = LEVELSELECT_MODE;
				if ( currentSelection == 0 && currentLevelSelection == 0 )
					gameState.mode	  = CAMEO_MODE;
				else
					gameState.mode	  = GAME_MODE;
				// ENDELSEIF
				frameCount = 0;
				lastbutton = 0;
				PlaySample ( 2,NULL,255,128);
				return;
			}
			// ENDIF
			
		} 

	}
	// ENDIF

	lastbutton = button;
}


void RunOptionsMode()
{
	static unsigned long currentSelection = 0;
	static u16 button,lastbutton;
	static TEXTOVERLAY *sound,*cntrl,*hiscore;
	SPRITEOVERLAY *sprOver;
	int i = 0,xPos,j;

	if(frameCount == 1)
	{	
		StopDrawing("options");
		FreeAllLists();

		LoadTextureBank(SYSTEM_TEX_BANK);
		LoadTextureBank(TITLES_TEX_BANK);

//		myBackdrop = CreateAndInitBackdrop("objvscrn.bmp");

		currFont = bigFont;		//smallFont;
		sound	= CreateAndAddTextOverlay(0,100,"sound",YES,NO,255,255,255,255,currFont,TEXTOVERLAY_WAVECHARS,1,0);
		cntrl	= CreateAndAddTextOverlay(0,140,"controller",YES,NO,255,255,255,255,currFont,TEXTOVERLAY_WAVECHARS,1,3.14/2);
		hiscore	= CreateAndAddTextOverlay(0,180,"high score",YES,NO,255,255,255,255,currFont,TEXTOVERLAY_WAVECHARS,1,3.14/4);

		sprOver = CreateAndAddSpriteOverlay(98,20,"flogo01.bmp",32,32,255,255,255,127,0);
		sprOver = CreateAndAddSpriteOverlay(130,20,"flogo02.bmp",32,32,255,255,255,127,0);
		sprOver = CreateAndAddSpriteOverlay(162,20,"flogo03.bmp",32,32,255,255,255,127,0);
		sprOver = CreateAndAddSpriteOverlay(194,20,"flogo04.bmp",32,32,255,255,255,127,0);
		sprOver = CreateAndAddSpriteOverlay(98,52,"flogo05.bmp",32,32,255,255,255,127,0);
		sprOver = CreateAndAddSpriteOverlay(130,52,"flogo06.bmp",32,32,255,255,255,127,0);
		sprOver = CreateAndAddSpriteOverlay(162,52,"flogo07.bmp",32,32,255,255,255,127,0);
		sprOver = CreateAndAddSpriteOverlay(194,52,"flogo08.bmp",32,32,255,255,255,127,0);
		sprOver = CreateAndAddSpriteOverlay(162,84,"flogo09.bmp",32,32,255,255,255,127,0);
		sprOver = CreateAndAddSpriteOverlay(194,84,"flogo10.bmp",32,32,255,255,255,127,0);

		soundIcon = CreateAndAddSpriteOverlay(30,30,"spkr1.bmp",32,32,255,255,255,191,ANIMATION_RANDOMFRAMES);
		AddFrameToSpriteOverlay(soundIcon,"spkr2.bmp");
		AddFrameToSpriteOverlay(soundIcon,"spkr3.bmp");
		AddFrameToSpriteOverlay(soundIcon,"spkr4.bmp");
		DisableSpriteOverlay(soundIcon);

		controllerIcon = CreateAndAddSpriteOverlay(30,30,"cntrl1.bmp",32,32,255,255,255,191,ANIMATION_RANDOMFRAMES);
		AddFrameToSpriteOverlay(controllerIcon,"cntrl2.bmp");
		AddFrameToSpriteOverlay(controllerIcon,"cntrl3.bmp");
		AddFrameToSpriteOverlay(controllerIcon,"cntrl4.bmp");
		DisableSpriteOverlay(controllerIcon);

		ResetParameters();

		StartDrawing("options");
	}	

	button = controllerdata[ActiveController].button;

	if((button & CONT_UP) && !(lastbutton & CONT_UP))
    {
		if (currentSelection>0)
			currentSelection--;
		PlaySample ( 237,NULL,255,128);
	}

	if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
    {
		if (currentSelection<(3-1))
			currentSelection++;
		PlaySample ( 237,NULL,255,128);
	}
	
	if(frameCount > 15)
	{
		if((button & CONT_B) && !(lastbutton & CONT_B))
		{
			FreeAllLists();
			frontEndState.mode = TITLE_MODE;
			frameCount = 0;
			lastbutton = 0;
			return;
		}

		if(	((button & CONT_START) && !(lastbutton & CONT_START)) ||
			((button & CONT_A) && !(lastbutton & CONT_A)))
		{
//			runningDevStuff = 0;
			FreeAllLists();
			frameCount		= 0;
			lastbutton		= 0;
			PlaySample ( 2, NULL, 255, 128 );
			switch (currentSelection)
			{
				case 0:   // Sound Mode
					frontEndState.mode	= SOUNDADJUST_MODE;
					return;
				case 1:  // Controller Config Mode
					frontEndState.mode	= CONTROLCONFIG_MODE;
					return;
				case 2:	 // High Score Mode
					frontEndState.mode	= HIGHSCORE_MODE;
					return;
			}
		}			
	
		switch (currentSelection)
		{
			case 0:
				EnableSpriteOverlay(soundIcon);
				DisableSpriteOverlay(controllerIcon);

				sound->a = 255 * fabs(sinf(frameCount/12.5));
				sound->waveAmplitude = 6.0F;
				cntrl->a = 91; //255;
				hiscore->a = 91; //255;				
				cntrl->r = cntrl->b = 200;
				cntrl->g = 255;
				hiscore->r = hiscore->b = 200;
				hiscore->g = 255;
				sound->r = sound->g = sound->b = 255;

				if(cntrl->waveAmplitude > 1)
					cntrl->waveAmplitude -= 0.10F;
				if(hiscore->waveAmplitude > 1)
					hiscore->waveAmplitude -= 0.10F;
				break;
			case 1:
				EnableSpriteOverlay(controllerIcon);
				DisableSpriteOverlay(soundIcon);

				cntrl->a = 255 * fabs(sinf(frameCount/12.5));
				cntrl->waveAmplitude = 6.0F;
				sound->a = 91; //255;				
				hiscore->a = 91; //255;				
				sound->r = sound->b = 200;
				sound->g = 255;
				hiscore->r = hiscore->b = 200;
				hiscore->g = 255;
				cntrl->r = cntrl->g = cntrl->b = 255;

				if(sound->waveAmplitude > 1)
					sound->waveAmplitude -= 0.10F;
				if(hiscore->waveAmplitude > 1)
					hiscore->waveAmplitude -= 0.10F;
				break;

			case 2:
				DisableSpriteOverlay(soundIcon);
				DisableSpriteOverlay(controllerIcon);

				hiscore->a = 255  * fabs(sinf(frameCount/12.5));				
				hiscore->waveAmplitude = 6.0F;
				sound->a = 91; //255;				
				cntrl->a = 91; //255;				
				sound->r = sound->b = 200;
				sound->g = 255;
				cntrl->r = cntrl->b = 200;
				cntrl->g = 255;
				hiscore->r = hiscore->g = hiscore->b = 255;

				if(sound->waveAmplitude > 1)
					sound->waveAmplitude -= 0.10F;
				if(cntrl->waveAmplitude > 1)
					cntrl->waveAmplitude -= 0.10F;
				break;
		}
	}

	lastbutton = button;
}



void RunSoundAdjust()
{
	static unsigned long currentSelection = 0;
	static u16 button,lastbutton;
	static TEXTOVERLAY *start,*retro;
	static TEXTOVERLAY *musVolText;
	static TEXTOVERLAY *sfxVolText;
	static SPRITEOVERLAY *sprOver = NULL;
	static char *musVolTmp;
	static char *sfxVolTmp;

	if ( frameCount == 1 )
	{
		StopDrawing  ( "sound adjust" );

		FreeAllLists();

		LoadTextureBank ( SYSTEM_TEX_BANK );
		LoadTextureBank ( TITLES_TEX_BANK );

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

		sprOver = CreateAndAddSpriteOverlay(24,150,"haslg001.bmp",32,32,255,255,255,255,0);
		sprOver = CreateAndAddSpriteOverlay(56,150,"haslg002.bmp",32,32,255,255,255,255,0);
		sprOver = CreateAndAddSpriteOverlay(24,182,"haslg003.bmp",32,32,255,255,255,255,0);
		sprOver = CreateAndAddSpriteOverlay(56,182,"haslg004.bmp",32,32,255,255,255,255,0);

		currFont	= smallFont;

		start		= CreateAndAddTextOverlay(150,122,"music",YES,NO,255,255,255,255,currFont,TEXTOVERLAY_WAVECHARS,1,0);
		retro		= CreateAndAddTextOverlay(90,140,"sfx",YES,NO,255,255,255,255,currFont,TEXTOVERLAY_WAVECHARS,1,3.14/2);

		musVolText	= CreateAndAddTextOverlay ( 230,122,"",NO,NO,255,255,255,255,currFont,0,1,0);
		sfxVolText	= CreateAndAddTextOverlay ( 230,140,"",NO,NO,255,255,255,91,currFont,0,1,0);
//		musicVol	= 1;
//		sfxVol		= 10;
//		sprintf ( musVolText->text, "%lu", musicVol );
//		sprintf ( sfxVolText->text, "%lu", sfxVol );

//		runningDevStuff = 1;

		StartDrawing ( "sound adjust" );
	}
	// ENDIF - frameCount == 1

	button = controllerdata[ActiveController].button;
	    
	if((button & CONT_UP) && !(lastbutton & CONT_UP))
    {
		if(currentSelection > 0)
		{
			currentSelection--;
			PlaySample ( 237,NULL,255,128);
		}
	}

	if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
    {
		if(currentSelection < (2-1))
		{
			currentSelection++;
			PlaySample ( 237,NULL,255,128);
		}
	}
	
	if(frameCount > 15)
	{
		if((button & CONT_B) && !(lastbutton & CONT_B))
		{
			FreeAllLists();
			frontEndState.mode = OPTIONS_MODE;
			frameCount = 0;
			lastbutton = 0;
		}

		if((button & CONT_R) && !(lastbutton & CONT_R))
		{
			if(currentSelection == 0)
			{
//				musicVol++;
//				sprintf ( musVolText->text, "%lu", musicVol );
			}
			else if(currentSelection == 1)
			{
//				sfxVol++;
//				sprintf ( sfxVolText->text, "%lu", sfxVol );
			}
			// ENDELSEIF - 
		}
		else if((button & CONT_L) && !(lastbutton & CONT_L ))
		{
			if(currentSelection == 0)
			{
//				musicVol--;
//				sprintf ( musVolText->text, "%lu", musicVol );
			}
			else if(currentSelection == 1)
			{
//				sfxVol--;
//				sprintf ( sfxVolText->text, "%lu", sfxVol );
			}
			// ENDELSEIF - 
		}
		// ENDIF - 

		switch (currentSelection)
		{
			case 0:
				start->a = 255 * fabs(sinf(frameCount/12.5));
				start->waveAmplitude = 6.0F;
				retro->a = 91; //255;
				retro->r = retro->b = 200;
				retro->g = 255;
				start->r = start->g = start->b = 255;

				if(retro->waveAmplitude > 1)
					retro->waveAmplitude -= 0.10F;
				break;
			case 1:
				retro->a = 255 * fabs(sinf(frameCount/12.5));
				retro->waveAmplitude = 6.0F;
				start->a = 91; //255;				
				start->r = start->b = 200;
				start->g = 255;
				retro->r = retro->g = retro->b = 255;

				if(start->waveAmplitude > 1)
					start->waveAmplitude -= 0.10F;
				break;

		}
	}

	lastbutton = button;
}


typedef struct _HISCORETABLE
{
	char positionTxt[32];
	char scoreTxt[32];
} HISCORETABLE;


HISCORETABLE table [ MAX_HISCORE_SLOTS ];

char* t;


void RunHiScoreMode ( void )
{
	short i;

	static unsigned long currentSelection = 0;
	static u16 button,lastbutton = 0;

	button = controllerdata[ActiveController].button;

	if ( frameCount == 1 )
	{
		StopDrawing  ( "hiscore table" );

		FreeAllLists();

		LoadTextureBank ( SYSTEM_TEX_BANK );
		LoadTextureBank ( TITLES_TEX_BANK );

		for ( i = 0; i < MAX_HISCORE_SLOTS; i++ )
		{

			currFont = smallFont;

			tablePosition[i] = CreateAndAddTextOverlay ( 60,20+(i*18),table[i].positionTxt,NO,NO,255,255,255,255,currFont,0,1,0);
			sprintf ( table[i].positionTxt, "%d", i+1 );

			tableNames[i] = CreateAndAddTextOverlay ( 120,20+(i*18),hiScoreData[i].name,NO,NO,255,255,255,255,currFont,0,1,0);

			tableScores[ i ] = CreateAndAddTextOverlay(180,20+(i*18),table[i].scoreTxt,NO,NO,255,255,255,255,currFont,0,0,0);
			sprintf ( table[i].scoreTxt, "%lu", hiScoreData[i].score );

		}
		// ENDFOR	
		CreateAndAddSpriteOverlay(200,185,"isl1.bmp",32,32,255,255,255,128,0);
		CreateAndAddSpriteOverlay(232,185,"isl2.bmp",32,32,255,255,255,128,0);
		CreateAndAddSpriteOverlay(264,185,"isl3.bmp",32,32,255,255,255,128,0);
		ResetParameters();

		StartDrawing  ( "hiscore table" );
	}
	// ENDIF

	if((button & CONT_B) && !(lastbutton & CONT_B))
	{
		FreeAllLists();
		frontEndState.mode = OPTIONS_MODE;
		frameCount = 0;
		lastbutton = 0;
	}
	// ENDIF

	lastbutton = button;
}

/*	--------------------------------------------------------------------------------
	Function 	: RunGameMode
	Purpose 	: In game
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void RunGameMode()
{
	static unsigned long currentSelection = 0;
	static u16 button,lastbutton;
	static TEXTOVERLAY *single,*multi,*classic;
	SPRITEOVERLAY *sprOver = NULL;
	int i = 0,xPos,j;

	if(frameCount == 1)
	{	
		StopDrawing("gamemode");
		FreeAllLists();

//		recordKeying = 0;

		LoadTextureBank(SYSTEM_TEX_BANK);
		LoadTextureBank(TITLES_TEX_BANK);

//		levelPlayingTimer =  ( 60 / desiredFrameRate ) * numSecs;

		currFont = smallFont;
		single	= CreateAndAddTextOverlay(100,122,"single player",YES,NO,255,255,255,255,currFont,TEXTOVERLAY_WAVECHARS,1,0);
		multi	= CreateAndAddTextOverlay(90,140,"multiplayer",YES,NO,255,255,200,255,currFont,TEXTOVERLAY_WAVECHARS,1,3.14/2);
		classic	= CreateAndAddTextOverlay(80,158,"classic frogger",YES,NO,255,255,200,255,currFont,TEXTOVERLAY_WAVECHARS,1,3.14/4);

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

		sprOver = CreateAndAddSpriteOverlay(24,150,"haslg001.bmp",32,32,255,255,255,127,0);
		sprOver = CreateAndAddSpriteOverlay(56,150,"haslg002.bmp",32,32,255,255,255,127,0);
		sprOver = CreateAndAddSpriteOverlay(24,182,"haslg003.bmp",32,32,255,255,255,127,0);
		sprOver = CreateAndAddSpriteOverlay(56,182,"haslg004.bmp",32,32,255,255,255,127,0);

		sprOver = CreateAndAddSpriteOverlay(200,185,"isl1.bmp",32,32,255,255,255,128,0);
		sprOver = CreateAndAddSpriteOverlay(232,185,"isl2.bmp",32,32,255,255,255,128,0);
		sprOver = CreateAndAddSpriteOverlay(264,185,"isl3.bmp",32,32,255,255,255,128,0);

		ResetParameters();

		StartDrawing("gamemode");
	}	

		
	button = controllerdata[ActiveController].button;

	if((button & CONT_UP) && !(lastbutton & CONT_UP))
    {
		if (currentSelection>0)
			currentSelection--;
//		levelPlayingTimer =  ( 60 / desiredFrameRate ) * numSecs;
		PlaySample ( 237,NULL,255,128);
	}
	    
	if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
    {
		if (currentSelection<(3-1))
			currentSelection++;
//		levelPlayingTimer =  ( 60 / desiredFrameRate ) * numSecs;
		PlaySample ( 237,NULL,255,128);
	}
	
	if(frameCount > 15)
	{
		if((button & CONT_B) && !(lastbutton & CONT_B))
		{
			FreeAllLists();
			frontEndState.mode = TITLE_MODE;
			frameCount = 0;
			lastbutton = 0;
		}

		if (((button & CONT_A) && !(lastbutton & CONT_A)) ||
		   ((button & CONT_START) && !(lastbutton & CONT_START)))
		{
			switch (currentSelection)
			{
				case 0:   // Single Player Mode
					FreeAllLists();
					frameCount		= 0;
					lastbutton		= 0;
					frontEndState.mode	= LEVELSELECT_MODE;
					PlaySample ( 2, NULL, 255, 128 );
					return;
				case 1:  // Multiplayer Mode
					FreeAllLists();
					frameCount		= 0;
					lastbutton		= 0;
					frontEndState.mode	= MULTIPLAYER_MODE;
					PlaySample ( 2, NULL, 255, 128 );
					return;
				case 2:	 // Classic Frogger Menu Mode
					FreeAllLists();
					frameCount		= 0;
					lastbutton		= 0;
					gameState.mode	= OLDEFROGGER_MODE;
					PlaySample ( 2,NULL,255,128);
					return;
			}
		}			
	
		switch (currentSelection)
		{
			case 0:
				single->a = 255 * fabs(sinf(frameCount/12.5));
				single->waveAmplitude = 6.0F;
				multi->a = 91; //255;
				classic->a = 91; //255;				
				multi->r = multi->b = 200;
				multi->g = 255;
				classic->r = classic->b = 200;
				classic->g = 255;
				single->r = single->g = single->b = 255;

				if(multi->waveAmplitude > 1)
					multi->waveAmplitude -= 0.10F;
				if(classic->waveAmplitude > 1)
					classic->waveAmplitude -= 0.10F;
				break;
			case 1:
				multi->a = 255 * fabs(sinf(frameCount/12.5));
				multi->waveAmplitude = 6.0F;
				single->a = 91; //255;				
				classic->a = 91; //255;				
				single->r = single->b = 200;
				single->g = 255;
				classic->r = classic->b = 200;
				classic->g = 255;
				multi->r = multi->g = multi->b = 255;

				if(single->waveAmplitude > 1)
					single->waveAmplitude -= 0.10F;
				if(classic->waveAmplitude > 1)
					classic->waveAmplitude -= 0.10F;
				break;

			case 2:
				classic->a = 255  * fabs(sinf(frameCount/12.5));				
				classic->waveAmplitude = 6.0F;
				single->a = 91; //255;				
				multi->a = 91; //255;				
				single->r = single->b = 200;
				single->g = 255;
				multi->r = multi->b = 200;
				multi->g = 255;
				classic->r = classic->g = classic->b = 255;

				if(single->waveAmplitude > 1)
					single->waveAmplitude -= 0.10F;
				if(multi->waveAmplitude > 1)
					multi->waveAmplitude -= 0.10F;
				break;


		}

/*		levelPlayingTimer--;
		if ( levelPlayingTimer == 0 )
		{
			FreeActorList();
			FreePlatformList();
			FreeEnemyList();
			FreeSpriteOverlayList();
//			FreeTextOverlayList();
			FreeAnimationList();
			FreeTextureList();
			FreeBackdrop(myBackdrop);
			myBackdrop = NULL;
			FreeLevel();
			FreeAllObjectBanks();
			FreeAllTextureBanks();

			frameCount = 0;
			lastbutton = 0;

			InitLevel ( 2,0 );
			gameState.mode = LEVELPLAYING_MODE;
			autoPlaying = 1;
			FreeBackdrop(myBackdrop);
			myBackdrop = NULL;
			return;

		}
*/		// endif - levelPlayingTimer == 0
	}

	lastbutton = button;
}

#endif