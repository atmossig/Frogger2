/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: menus.c
	Programmer	: James Healey
	Date		: 27/05/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>
#include <limits.h>

#include "incs.h"

TEXTOVERLAY *worldNames	[ MAX_WORLDS ];
TEXTOVERLAY *levelN		[ MAX_LEVELS ];
TEXTOVERLAY *closed;
TEXTOVERLAY *posText;
TEXTOVERLAY *levelnameText;

SPRITEOVERLAY *atari = NULL;
SPRITEOVERLAY *konami = NULL;
SPRITEOVERLAY *sprOver = NULL;
SPRITEOVERLAY *islLogo[] = {NULL,NULL,NULL};
SPRITEOVERLAY *flogo[10];

#ifdef PC_VERSION
unsigned long USE_MENUS = 0;
#endif

/* LOGO information for Frogger 2 logo */

LOGO Frogger2Logo =
{
	"flogo", 4, 3,
	{
		1, 2, 3, 4,
		5, 6, 7, 8,
		0, 0, 9, 10
	}
};

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
		currFont = bigFont;
#else
		LoadTextureBank(TITLESGENERIC_TEX_BANK);
		currFont = smallFont;
#endif

		startText = CreateAndAddTextOverlay(100,112,"start game",YES,255,currFont,TEXTOVERLAY_NORMAL,6);

		selectText = CreateAndAddTextOverlay(100,132,"level select",YES,255,currFont,TEXTOVERLAY_NORMAL,6);
		multiText = CreateAndAddTextOverlay(100,152,mpText,YES,255,currFont,TEXTOVERLAY_NORMAL,6);
		devText = CreateAndAddTextOverlay(100,172,"develop",YES,255,currFont,TEXTOVERLAY_NORMAL,6);

		konami = CreateAndAddSpriteOverlay(240,35,"konami.bmp",32,32,192,0 );
		atari = CreateAndAddSpriteOverlay(40,35,"atari.bmp",32,32,192,0 );

		islLogo[0] = CreateAndAddSpriteOverlay(220,195,"isl1.bmp",32,32,192,0 );
		islLogo[1] = CreateAndAddSpriteOverlay(252,195,"isl2.bmp",32,32,192,0 );
		islLogo[2] = CreateAndAddSpriteOverlay(284,195,"isl3.bmp",32,32,192,0 );

		CreateOverlaysFromLogo(&Frogger2Logo,98,20);

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
			switch (currentSelection--)
			{
				case 0: startText->draw = 1; break;
				case 1: selectText->draw = 1; break;
				case 2: multiText->draw = 1; break;
				case 3: devText->draw = 1; break;
			}
		}
	}
	    
	if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
    {
		if( currentSelection < 3 )
		{
			switch (currentSelection++)
			{
				case 0: startText->draw = 1; break;
				case 1: selectText->draw = 1; break;
				case 2: multiText->draw = 1; break;
				case 3: devText->draw = 1; break;
			}
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

		if ((button & CONT_START) && !(lastbutton & CONT_START))
		{
			PostQuitMessage(0);
			return;
		}

		if ((button & CONT_A) && !(lastbutton & CONT_A))
		{
			switch(currentSelection)
			{
				case 0:
					player[0].worldNum = WORLDID_FRONTEND;
					player[0].levelNum = LEVELID_FRONTEND1;

					FreeAllLists();
					InitLevel(player[0].worldNum,player[0].levelNum);

					gameState.mode = INGAME_MODE;
					gameState.multi = SINGLEPLAYER;

					ACTOR_DRAWDISTANCEINNER = 300000;
					ACTOR_DRAWDISTANCEOUTER = 350000;

					NUM_FROGS = 1;
					frameCount = 0;
					lastbutton = 0;
					break;

				case 1:
					// level select menu
					FreeMenuItems();
					gameState.menuMode = LEVELSELECT_MODE;

					if(gameState.multi != MULTIREMOTE)
					{
						NUM_FROGS = 1;
						gameState.multi = SINGLEPLAYER;
					}

					ACTOR_DRAWDISTANCEINNER = 300000;
					ACTOR_DRAWDISTANCEOUTER = 350000;

					frameCount = 0;
					lastbutton = 0;
					break;
			
				case 2:
					// select multi player game
					FreeMenuItems();
					if(gameState.multi != MULTIREMOTE)
						gameState.multi = MULTILOCAL;
					gameState.menuMode = LEVELSELECT_MODE;
					ACTOR_DRAWDISTANCEINNER = ULONG_MAX-1;
					ACTOR_DRAWDISTANCEOUTER = ULONG_MAX;
					frameCount = 0;
					lastbutton = 0;
					NUM_FROGS = numPlayers;
					break;
#ifdef N64_VERSION
				case 3:
					// develepoment mode
					FreeMenuItems();
					gameState.menuMode = DEVELOPMENT_MODE;
					frameCount = 0;
					lastbutton = 0;
					break;
#endif
			}

			return;		// DON'T try to do any menu stuff! Duh
		}			

		switch (currentSelection)
		{
			case 0: startText->a = 255; if( !(frameCount%32) ) startText->draw = !startText->draw; break;
			case 1: selectText->a = 255; if( !(frameCount%32) ) selectText->draw = !selectText->draw; break;
			case 2: multiText->a = 255; if( !(frameCount%32) ) multiText->draw = !multiText->draw; break;
			case 3: devText->a = 255; if( !(frameCount%32) ) devText->draw = !devText->draw;break;
		}
	}

	lastbutton = button;
}

#define NUM_CHARS 3
char texNames[3][16] = {"frogger","lillie","toad"};
char frogNames[3][16] = {"frogger.obe","femfrog.obe","toad.obe"};

extern char frogModel[4][16];

void RunCharSelect()
{
	static u16 button;
	static u16 lastbutton;
	static TEXTOVERLAY *charSelText[NUM_CHARS];
	static TEXTOVERLAY *charSelTitle;
	static int curSel = 0;
	unsigned long i;

	if (frameCount == 1)
	{
		FreeMenuItems();

		for( i=0; i < NUM_CHARS; i++ )
			charSelText[i] = CreateAndAddTextOverlay( 150, (i*20)+90, texNames[i],	YES,100, currFont, TEXTOVERLAY_NORMAL,0 );		
		charSelTitle = CreateAndAddTextOverlay( 150, 30, "Char Select",	YES,100, currFont, TEXTOVERLAY_NORMAL,0 );		
	}

	// Sod it, it's only a menu.
	for( i=0; i < NUM_CHARS; i++ )
		charSelText[i]->a = 100;
	
	charSelText[curSel]->a = 0xff;
	if( !(frameCount%32) ) charSelText[curSel]->draw = !charSelText[curSel]->draw;

	button = controllerdata [ ActiveController ].button;

	// move between worlds or levels
	if((button & CONT_UP) && !(lastbutton & CONT_UP))
    {
		if (curSel>0)
			charSelText[curSel--]->draw = 1;
	}
	    
	if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
    {
		if (curSel<NUM_CHARS-1)
			charSelText[curSel++]->draw = 1;
	}

	if (frameCount>15)
	{
		if ((button & CONT_A) && !(lastbutton & CONT_A))
		{
			strcpy(frogModel[0],frogNames[curSel]);

			FreeAllLists();
			InitLevel(player[0].worldNum,player[0].levelNum);

			gameState.mode = INGAME_MODE;
				
			frameCount = 0;
			lastbutton = 0;
		}

		if( (button & CONT_B) && !(lastbutton & CONT_B) )
		{
			FreeMenuItems();
			gameState.menuMode = TITLE_MODE;
			frameCount = 0;
			lastbutton = 0;
			return;
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
		FreeMenuItems();

		//currFont = smallFont;

		// draw list of worlds on left
		for( i=0; i < MAX_WORLDS; i++ )
			worldSelText[i] = CreateAndAddTextOverlay( 5, (i*20)+30, worldVisualData[i].description,
														NO,100, currFont, TEXTOVERLAY_NORMAL,0 );

		// draw list of levels in current selected world on right
		for( i=0; i < MAX_LEVELS; i++ )
		{
			sprintf( lNames[i].name, worldVisualData[currentWorldSelect].levelVisualData[i].description );
			levelSelText[i] = CreateAndAddTextOverlay( 150, (i*20)+30, lNames[i].name,
														NO,100,	currFont,TEXTOVERLAY_NORMAL,0);
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
			worldSelText[currentWorldSelect--]->draw = 1;
			for( i=0; i < MAX_LEVELS; i++ )
			{
				if ((worldVisualData[currentWorldSelect].levelVisualData[i].collBank != -1 ) &&	(worldVisualData[currentWorldSelect].levelVisualData[i].description[0] != ' '))
				{
					currentLevelSelect = i;
					break;
				}
				else currentLevelSelect = 255;
			}
		}
		else if( lrSelect == 1 && currentLevelSelect > 0 ) // Level select
		{
			levelSelText[currentLevelSelect]->draw = 1;
			for( i=currentLevelSelect-1; i >= 0; i-- )
			{
				if ((worldVisualData[currentWorldSelect].levelVisualData[i].collBank != -1 ) &&	(worldVisualData[currentWorldSelect].levelVisualData[i].description[0] != ' '))
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
			
			worldSelText[currentWorldSelect++]->draw = 1;
			for( i=0; i < MAX_LEVELS; i++ )
			{
				if ((worldVisualData[currentWorldSelect].levelVisualData[i].collBank != -1 ) &&	(worldVisualData[currentWorldSelect].levelVisualData[i].description[0] != ' '))
				{
					currentLevelSelect = i;
					break;
				}
				else currentLevelSelect = 255;
			}
		}
		else if( lrSelect == 1 && currentLevelSelect < MAX_LEVELS )
		{
			levelSelText[currentLevelSelect]->draw = 1;
			for( i=currentLevelSelect+1; i < MAX_LEVELS; i++ )
			{
				if ((worldVisualData[currentWorldSelect].levelVisualData[i].collBank != -1 ) &&	(worldVisualData[currentWorldSelect].levelVisualData[i].description[0] != ' '))
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
		{
			if (worldVisualData[currentWorldSelect].levelVisualData[i].description[0] == ' ')
				break;
			
			sprintf(lNames[i].name,worldVisualData[currentWorldSelect].levelVisualData[i].description);
		}
	}


	//	don't allow menu changing for the first fifteen frames

	if(frameCount > 15)
	{
		// move back in menus
		if((button & CONT_B) && !(lastbutton & CONT_B))
		{
			FreeMenuItems();
			gameState.menuMode = TITLE_MODE;
			frameCount = 0;
			lastbutton = 0;
			return;
		}

		if ((button & CONT_START) && !(lastbutton & CONT_START))
		{
			PostQuitMessage(0);
			return;
		}

		// run selected world and level
		if( (((button & CONT_A) && !(lastbutton & CONT_A)))
			&& currentLevelSelect != 255)
		{
				FreeMenuItems();
				
				player[0].worldNum = currentWorldSelect;
				player[0].levelNum = currentLevelSelect;
				
				gameState.menuMode = CHARSELECT_MODE;

				frameCount = 0;
				lastbutton = 0;
				return;
		}



		/*	player[0].worldNum = currentWorldSelect;
			player[0].levelNum = currentLevelSelect;

			FreeAllLists();
			InitLevel(currentWorldSelect,currentLevelSelect);

			gameState.mode = INGAME_MODE;
				
			frameCount = 0;
			lastbutton = 0;*/

		//	return;
		//}			
	}

	// set selected text solid
	if(lrSelect == 0)
	{
		worldSelText[currentWorldSelect]->a = 255;
		if( currentLevelSelect != 255 ) 
		{
			levelSelText[currentLevelSelect]->a = 175;
			levelSelText[currentLevelSelect]->draw = 1;
		}
		if( !(frameCount%32) ) worldSelText[currentWorldSelect]->draw = !worldSelText[currentWorldSelect]->draw;
	}
	else
	{
		worldSelText[currentWorldSelect]->a = 175;
		worldSelText[currentWorldSelect]->draw = 1;
		if( currentLevelSelect != 255 ) levelSelText[currentLevelSelect]->a = 255;
		if( !(frameCount%32) ) levelSelText[currentLevelSelect]->draw = !levelSelText[currentLevelSelect]->draw;
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
extern unsigned long actTickCount,actTickCountModifier;
extern unsigned long idleCamera;
extern float pauseGameSpeed;
void RunPauseMenu()
{
	static unsigned long currentSelection = 0;
	static u16 button,lastbutton = 0;
	
	button = controllerdata[ActiveController].button;

	if( ((button & CONT_UP) && !(lastbutton & CONT_UP)) || ((button & CONT_DOWN) && !(lastbutton & CONT_DOWN)) )
		((currentSelection ^= 1)?(continueText):(quitText))->draw = 1; // If I have to do this I'm damn well going to hack it

	if((button & CONT_A) && !(lastbutton & CONT_A))
	{
		lastbutton = button;

//		DisableTextOverlay ( pauseTitle );
		DisableTextOverlay ( continueText );
		DisableTextOverlay ( quitText );
		DisableTextOverlay ( posText );
		DisableTextOverlay ( levelnameText );

		FreeScreenTextures(screenTextureList,screenTextureList2,screenTexList);

		pauseMode = 0;
		idleCamera = 0;

		switch(currentSelection)
		{
			case 0:   // continue game
			{
				long i;
				gameState.mode	= INGAME_MODE;

				if( gameState.multi==SINGLEPLAYER )
				{
					livesTextOver->a = livesTextOver->oa;
					scoreTextOver->a = scoreTextOver->oa;
				}
				timeTextOver->a = timeTextOver->oa;

				for ( i = 0; i < 3; i++ )
					sprHeart[i]->draw = 1;

				for(i=0; i<numBabies; i++)
					babyIcons[i]->draw = 1;

				actTickCountModifier = (GetTickCount()-actTickCount);
				return;
			}
			
			case 1:   // quit game
				FreeAllLists();

			if (!USE_MENUS)
			{
				if (player[0].worldNum == WORLDID_FRONTEND)
				{
					#ifdef PC_VERSION
						if (player[0].levelNum == LEVELID_FRONTEND1)
							PostQuitMessage(0);
//							DestroyWindow (winInfo.hWndMain);
						else
							gameState.mode = INGAME_MODE;
							InitLevel(WORLDID_FRONTEND,LEVELID_FRONTEND1);						
					#endif
				}
				else
				{
					gameState.mode = INGAME_MODE;
					InitLevel(WORLDID_FRONTEND,LEVELID_FRONTEND1);
				}
			}
			else
			{
				gameState.mode = MENU_MODE;
				gameState.menuMode = TITLE_MODE;
			}

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
			if( !(actTickCount%4) ) continueText->draw = !continueText->draw;

			quitText->a = 91; //255;
			quitText->r = quitText->b = 255;
			quitText->g = 255;
			break;
		
		case 1:
			quitText->a = 255;
			quitText->r = quitText->g = quitText->b = 255;
			if( !(actTickCount%4) ) quitText->draw = !quitText->draw;

			continueText->a = 91; //255;
			continueText->r = continueText->b = 255;
			continueText->g = 255;
			break;

	}

	lastbutton = button;

	gameSpeed = pauseGameSpeed;
	UpdateCameraPosition();
	gameSpeed = 0;
			
}

/*	--------------------------------------------------------------------------------
	Function		: FreeMenuItems
	Purpose			: free text / sprite overlays
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeMenuItems()
{
	StopDrawing("FreeMenuItems");

	// free text and sprite overlays
	FreeTextOverlayLinkedList();
	FreeSpriteOverlayLinkedList();
	//FreeSpriteLinkedList();

	StartDrawing("FreeMenuItems");
}

