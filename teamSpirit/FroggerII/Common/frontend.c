/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: frontend.c
	Programmer	: James Healey
	Date		: 30/03/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

#include <ultra64.h>
#include "incs.h"


//------ [ GLOBALS ] ---------------------------------------------------------------------------//

struct frontEndStateStruct frontEndState;

int frameCount			= 0;
int	frameCount2			= 0;
int dispFrameCount		= 0;

long multiplayerRun		= 0;

char ActiveController	= 0;


//------ [ FUNCTION IMPLEMENTATION ] -----------------------------------------------------------//

void RunFrontEnd ( void )
{
	switch ( frontEndState.mode )
	{
		case DEMO_MODE:
				RunDemoOption();
			break;

		case SLOT_SELECT_MODE:
				RunSaveLoadSelect();
			break;

		case TITLE_MODE:
				RunTitleScreen();
			break;

		case GAMETYPE_MODE:
				RunGameMode();
			break;

		case LEVELSELECT_MODE:
				multiplayerRun = 0;
				RunLevelSelect();
			break;

		case DEVELOPMENTMENU_MODE:
//				RunDevelopmentMenu();
			break;

		case SNDVIEW_MODE:
				RunSndView();
			break;

		case MULTIPLAYER_MODE:	
				multiplayerRun = 1;
				RunLevelSelect();
			break;
		
		case OPTIONS_MODE:
				RunOptionsMode();
			break;

		case SOUNDADJUST_MODE:
				RunSoundAdjust();
			break;

		case CONTROLCONFIG_MODE:
//				RunHiScoresMode();
			break;

		case HIGHSCORE_MODE:
				RunHiScoreMode();
			break;

		case DEVELOPMENT_MODE:
//				RunDevelopmentRoutines();
			break;

	};
	// ENDIF - frontEndState.mode

};


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: GameLoop 

	Purpose		:
	Parameters	: (void)
	Returns		: static void 
*/

long ingameRate = 3;

void GameLoop(void)
{
	switch (gameState.mode)
	{
		case GAME_MODE:
			desiredFrameRate = newDesiredFrameRate = ingameRate;
//			runningWaterStuff = 1;
			//currFont = bigFont;
			if(frameCount == 15)
				StartDrawing("gameloop");

			UseAAMode = 2;
			UseZMode = 1;
		
			RunGameLoop();
			frameCount++;
			break;
		case CAMEO_MODE:
		case PAUSE_MODE:
			desiredFrameRate = newDesiredFrameRate = ingameRate;
//			runningWaterStuff = 1;
			//currFont = bigFont;
			if(frameCount == 15)
				StartDrawing("gameloop");

			RunPauseMenu();
			frameCount++;
			break;
	
		//case 



		case FRONTEND_MODE:
//			currFont = bigFont;
			desiredFrameRate = newDesiredFrameRate = 1;
			if ( frameCount == 15 )
				StartDrawing ( "gameloop" );
			// ENDIF - frameCount == 15
			RunFrontEnd();
			frameCount++;
			break;

		case OLDEFROGGER_MODE:

/*			// olde original 2D Frogger mode
			desiredFrameRate = newDesiredFrameRate = 1;
			currFont = bigFont;
			if(frameCount == 15)
				StartDrawing("gameloop");

			RunOldeFrogger();

			frameCount++;*/
			gameState.mode = FRONTEND_MODE;
			break;
	}
}
