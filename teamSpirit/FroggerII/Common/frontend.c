/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: frontend.c
	Programmer	: James Healey
	Date		: 30/03/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

#include <ultra64.h>
#include "incs.h"


struct frontEndStateStruct frontEndState;

long multiplayerRun = 0;

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
				RunDevelopmentMenu();
			break;
		case OBJVIEW_MODE:
				RunObjectViewer();
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
				RunDevelopmentRoutines();
			break;

	};
	// ENDIF - frontEndState.mode

};

