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

void RunFrontEndStates( )
{
	switch ( frontEndState.mode )
	{
	case TITLE_MODE:
		RunTitleScreen();
		break;
	case LEVELSELECT_MODE:
		multiplayerRun = 0;
		RunLevelSelect();
		break;
	case HISCORE_MODE:
		RunHiscoreScreen();
		break;

	}
}



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
		//runningWaterStuff = 1;
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
		//runningWaterStuff = 1;
		if(frameCount == 15)
			StartDrawing("pausemode");

		RunPauseMenu();
		frameCount++;
		break;
	case FRONTEND_MODE:
		desiredFrameRate = newDesiredFrameRate = 1;
		if ( frameCount == 15 )
			StartDrawing ( "frontend" );

		RunFrontEndStates();
		frameCount++;
		break;
	case OLDEFROGGER_MODE:
		// olde original 2D Frogger mode
		/*desiredFrameRate = newDesiredFrameRate = 1;
		currFont = bigFont;
		if(frameCount == 15)
			StartDrawing("gameloop");

		RunOldeFrogger();
		frameCount++;*/
		gameState.mode = FRONTEND_MODE;
		break;
	}
}
