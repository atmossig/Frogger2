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
		case GAME_MODE:
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
	
		case CAMEO_MODE:
		case PAUSE_MODE:
			if(frameCount == 15)
				StartDrawing("gameloop");

			RunPauseMenu();
			frameCount++;
			break;

		case DEVELOPMENT_MODE:
			if(frameCount == 15)
				StartDrawing("gameloop");

			RunDevelopmentMenu();
			frameCount++;
			break;
	}

	i = NUM_FROGS;
	while(i--)
		if(player[i].inputPause > 0)
			player[i].inputPause = 0;
}
