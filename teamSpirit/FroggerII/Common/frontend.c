/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: frontend.c
	Programmer	: Jim & Dave
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

struct INTRO
{
	long timer;
	int stage;
	TEXTOVERLAY *text[4];
	GAMETILE *tiles;
};

struct INTRO* intro = NULL;

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
		//RunLevelCompleteSequence();
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


void StartLevelComplete()
{
	FreeAllGameLists();
	CreateAndAddTextOverlay(0, 110, "Level complete screen", YES, 255, currFont, 0, 0);
	CreateAndAddTextOverlay(0, 130, "goes here", YES, 255, currFont, 0, 0);
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
	Function		: StartGameOver
	Purpose			: Die, froggy! Wuaahahahahahahaha
	Parameters		: 
	Returns			: 
*/
void StartGameOver()
{
	gameState.mode = GAMEOVER_MODE;
	GTInit( &modeTimer, 10 );

	FreeAllGameLists();

	CreateAndAddTextOverlay(0, 110, "Game over screen", YES, 255, currFont, 0, 0);
	CreateAndAddTextOverlay(0, 130, "goes here", YES, 255, currFont, 0, 0);
}

/*	--------------------------------------------------------------------------------
	Function		: RunGameOver
	Purpose			: Runs the game-over screen
	Parameters		: 
	Returns			: 
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

/*	--------------------------------------------------------------------------------
	Function		: StartGameIntro
	Purpose			: Runs the intro sequence, ISL logos and so on
	Parameters		: 
	Returns			: 

	NOTE! This is a very crude placeholder intro sequence. Replace it!
*/
void StartGameIntro()
{
	int i, j;
	GAMETILE *tile, *ptile = NULL;

	const static GAMETILE ex_tile = {
		{ NULL, NULL, NULL, NULL }, NULL, { NULL },
		TILESTATE_NORMAL,
		{ 0, 0, 0 },
		{ 0, 1, 0 },
		{ { 1, 0, 0 }, { 0, 0, -1 }, { -1, 0, 0 }, { 0, 0, 1 } }
	};		

	gameState.mode = INTRO_MODE;

	// load the generic banks
	LoadTextureBank(SYSTEM_TEX_BANK);
	LoadTextureBank(INGAMEGENERIC_TEX_BANK);
	LoadObjectBank(INGAMEGENERIC_OBJ_BANK);

	InitGameLists();
	
	// Create intro stuff
	
	intro = (struct INTRO*)JallocAlloc(sizeof(struct INTRO), YES, "intro");

	// Generate a crap collision mesh

	firstTile = intro->tiles = (GAMETILE*)JallocAlloc(sizeof(GAMETILE) * 11, YES, "introtiles");

	for (i=0, tile = intro->tiles; i<=10; i++, tile++)
	{
		//memcpy(tile, &ex_tile, sizeof(GAMETILE));

		*tile = ex_tile;

		tile->centre.v[0] = ((i-5)*50.0f);
		
		if (ptile)
		{
			tile->tilePtrs[0] = ptile;
			ptile->tilePtrs[2] = ptile->next = tile;
		}

		ptile = tile;
	}

	// load frogger (for no particular reason)

	CreateFrogActor(&tile, "frogger.obe", 0);
	CameraLookAtFrog();
	UpdateCameraPosition(0);

	CreateVector(&currCamSource[0], 0, 0, 200);
	SetVector(&camSource[0], &currCamSource[0]);

	CreateVector(&currCamTarget[0], 0, 0, 0);
	SetVector(&camTarget[0], &currCamTarget[0]);

	SetVector(&camVect, &upVec);

	controlCamera = 1;

	// set up player

	destTile[0] = currTile[0] = &intro->tiles[5];
	player[0].jumpTime = 1;

	// Create overlays

	intro->text[0] = CreateAndAddTextOverlay(0, 120, "Atari presents", YES, 255, currFont, 0, 0);

	intro->text[1] = CreateAndAddTextOverlay(0, 100, "An", YES, 255, currFont, 0, 0);
	intro->text[2] = CreateAndAddTextOverlay(0, 120, "Interactive Studios", YES, 255, currFont, 0, 0);
	intro->text[3] = CreateAndAddTextOverlay(0, 140, "Game", YES, 255, currFont, 0, 0);

	for (i=1;i<=3;i++)
		intro->text[i]->draw = 0;
	
	fog.r = fog.g = fog.b = 0;
	fog.mode = FOG_OFF;

	intro->timer = 0;
	intro->stage = 0;

	lastActFrameCount = 0;
}



/*	--------------------------------------------------------------------------------
	Function		: RunGameIntro
	Purpose			: Title screens and what have you
	Parameters		: 
	Returns			: 

	NOTE: This is a placeholder! DO NOT MODIFY THIS CODE! Delete it and start again.
*/
void RunGameIntro( )
{
	long t = actFrameCount - intro->timer;
	int alpha, i;

	//CameraLookAtFrog();
	//UpdateCameraPosition(0);

/*	FroggerHop(0);	// just run the movement part of the frog jump code..

	if (player[0].jumpTime >= 1.0f)
	{
		if (destTile[0])
		{
			currTile[0] = destTile[0];
			destTile[0] = NULL;
		}

		SetVector(&frog[0]->actor->pos, &currTile[0]->centre);
		player[0].jumpTime = -1;
		AnimateActor(frog[0]->actor, FROG_ANIM_BREATHE, YES, YES, 1.0f, NO, NO);
	}

	if (player[0].jumpTime < 0 && Random(40) == 0)
	{
		if ((Random(2) == 1) && currTile[0]->tilePtrs[2])
			HopFrogToTile(currTile[0]->tilePtrs[2], 0);
		else if (currTile[0]->tilePtrs[0])
			HopFrogToTile(currTile[0]->tilePtrs[0], 0);
	}

	//CheckForFroggerLanding(0);
*/

	switch (intro->stage)
	{
	case 0:
		intro->timer = actFrameCount;
		intro->stage++;
		break;

	case 1:
		{
			if (t < 120) break;
			t -= 120;

			alpha = 255 - (t * 255)/120;
			
			if (alpha > 0)
				intro->text[0]->a = alpha;
			else
			{
				intro->stage = 2;
				intro->timer = actFrameCount;
				
				intro->text[0]->draw = 0;

				for (i=1;i<=3;i++)
				{
					intro->text[i]->draw = 1;
					intro->text[i]->a = 255;
				}
			}

			break;
		}

	case 2:
		{
			t -= 120;
			if (t < 0) break;

			alpha = 255 - (t * 255)/120;

			if (alpha > 0)
			{
				for (i=1;i<=3;i++)
					intro->text[i]->a = alpha;
			}
			else
			{
				intro->stage = 1;
				intro->timer = actFrameCount;
				
				intro->text[0]->draw = 1;
				intro->text[0]->a = 255;

				for (i=1;i<=3;i++)
					intro->text[i]->draw = 0;

				/*JallocFree((UBYTE**)&intro);

				player[0].worldNum = WORLDID_FRONTEND;
				player[0].levelNum = LEVELID_FRONTEND1;
				
				controlCamera = 0;

				gameState.mode = LEVELCOMPLETE_MODE;
				GTInit( &modeTimer, 1 );
				showEndLevelScreen = 0;*/
			}
			break;
		}
	}
}

