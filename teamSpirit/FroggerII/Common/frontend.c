/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: frontend.c
	Programmer	: James Healey
	Date		: 30/03/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>
#include "incs.h"


//------ [ GLOBALS ] ---------------------------------------------------------------------------//

int frameCount			= 0;
int	frameCount2			= 0;
int dispFrameCount		= 0;

char ActiveController	= 0;



/*	--------------------------------------------------------------------------------
	Function		: GameLoop
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void GameLoop(void)
{
	int i;

	switch (gameState.mode)
	{
		case INGAME_MODE:
		case FRONTEND_MODE:
			if(frameCount == 15)
				StartDrawing("gameloop");

	#ifdef PC_VERSION
			UseAAMode = 2;
			UseZMode = 1;
	#endif
	
			RunGameLoop();
			frameCount++;
			break;

		case LEVELCOMPLETE_MODE:
			RunLevelComplete( );
			frameCount++;
			break;

		case WORLDCOMPLETE_MODE:
			RunWorldComplete( );
			frameCount++;
			break;

		case GAMECOMPLETE_MODE:
			RunGameComplete( );
			frameCount++;
			break;

		case GAMEOVER_MODE:
			RunGameOver( );
			frameCount++;
			break;
			
		case INTRO_MODE:
			RunGameIntro( );
			break;

		case CAMEO_MODE:
		case PAUSE_MODE:
			if(frameCount == 15)
				StartDrawing("gameloop");

			RunPauseMenu();
			frameCount++;
			break;

		// MENU MODE IS JUST INCLUDED FOR DEVELOPMENT PURPOSES - WILL BE REMOVED FOR RELEASE
		case MENU_MODE:
			if(frameCount == 15)
				StartDrawing("gameloop");

			switch(gameState.menuMode)
			{
				case TITLE_MODE:
					RunTitleScreen();
					break;

				case LEVELSELECT_MODE:
					RunLevelSelect();
					break;

#ifdef N64_VERSION
				case DEVELOPMENT_MODE:
					switch(developmentMode)
					{
						case OBJVIEW_MODE:
							RunObjectViewer();
							break;

						case SNDVIEW_MODE:
							RunSndView();
							break;

						case WATERVIEW_MODE:
							RunWaterDemo();
							break;

						case PROCVIEW_MODE:
							RunProcDemo();
							break;

						default:
							RunDevelopmentMenu();
					}
					break;
#endif

				default:
					dprintf""));
			}
			frameCount++;
			break;


#ifdef N64_VERSION
		case DEVELOPMENT_MODE:
			if(frameCount == 15)
				StartDrawing("gameloop");

			RunDevelopmentMenu();
			frameCount++;
			break;
#endif
	}

	i = NUM_FROGS;
	while(i--)
		if(player[i].inputPause > 0)
			player[i].inputPause = 0;
}



/*	--------------------------------------------------------------------------------
	Function		: CreateOverlaysFromLogo
	Purpose			: Creates a series of sprite overlays from a LOGO, at screen resolution
	Parameters		: LOGO*, left, top
	Returns			: 
	Info			: 
*/
void CreateOverlaysFromLogo(const LOGO* logo, int x, int y)
{
	int i, j;
	char texture[32];
	unsigned short *tex, t;

	tex = (short*)logo->tiles;

	for (i = 0; i < logo->height; i++)
		for (j = 0; j < logo->width; j++)
		{
			if (t = *(tex++))
			{
				sprintf(texture, "%s%02d.bmp", logo->texname, t);
				CreateAndAddSpriteOverlay(x + j*32, y + i*32, texture, 32, 32, 255, 0);
			}
		}
}


/*	--------------------------------------------------------------------------------
	Function		: RunLevelComplete
	Purpose			: What happens at the end of a level
	Parameters		: 
	Returns			: 
	Info			: 
*/
void RunLevelComplete( )
{
	if( showEndLevelScreen )
	{
		RunLevelCompleteSequence();
	}

	GTUpdate( &modeTimer, -1 );

	if(!modeTimer.time)
	{
		DoHiscores( );

		// Only go to next level if in normal level progression.
		if( showEndLevelScreen )
		{
			player[0].worldNum = WORLDID_FRONTEND;
			player[0].levelNum = LEVELID_FRONTEND2;

#ifndef PC_VERSION
			StoreSaveSlot(0, 0); // Write data for Player 0 into Slot 0
			SaveGame(); // Write save games into eeprom
#else
//				SaveGameData();
#endif
		}

		FreeAllLists();
		frameCount = 0;
		gameState.mode = INGAME_MODE;

		player[0].numSpawn	= 0;
//		spawnCounter = 0;

		worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].levelOpen |= LEVEL_OPEN;
		InitLevel(player[0].worldNum,player[0].levelNum);

		showEndLevelScreen = 1; // Normal level progression is default
	}
}


/*	--------------------------------------------------------------------------------
	Function		: RunWorldComplete
	Purpose			: What happens at the end of a world
	Parameters		: 
	Returns			: 
	Info			: 
*/
void RunWorldComplete( )
{

}


/*	--------------------------------------------------------------------------------
	Function		: RunGameComplete
	Purpose			: The final complete screen
	Parameters		: 
	Returns			: 
	Info			: 
*/
void RunGameComplete( )
{

}


/*	--------------------------------------------------------------------------------
	Function		: RunGameOver
	Purpose			: The frog has lost all his lives :(
	Parameters		: 
	Returns			: 
	Info			: 
*/
void RunGameOver( )
{
/*	if (NUM_FROGS == 1)
	{
		DisableTextOverlay(livesTextOver);
		DisableTextOverlay(scoreTextOver);
	}

	DisableTextOverlay(timeTextOver);
*/

	GTUpdate( &modeTimer, -1 );
	if(!modeTimer.time)
	{
		StopDrawing("game over");
		FreeAllLists();
		gameState.mode = INGAME_MODE;

		player[0].levelNum = LEVELID_FRONTEND1;
		player[0].worldNum = WORLDID_FRONTEND;
		player[0].frogState &= ~FROGSTATUS_ISDEAD;
		InitLevel(player[0].worldNum,player[0].levelNum);

		frameCount = 0;
		StartDrawing("game over");
	}
}


void StartGameOver()
{
	gameState.mode = GAMEOVER_MODE;
	GTInit( &modeTimer, 10 );

	FreeAllGameLists();

	CreateAndAddTextOverlay(0, 120, "Game over screen", YES, 255, currFont, 0, 0);
	CreateAndAddTextOverlay(0, 140, "goes here", YES, 255, currFont, 0, 0);
}

/*	--------------------------------------------------------------------------------
	Function		: RunGameIntro
	Purpose			: Title screens and what have you
	Parameters		: 
	Returns			: 
	Info			: 
*/
void RunGameIntro( )
{

}
