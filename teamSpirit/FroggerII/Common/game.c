/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: game.c
	Programmer	: Matthew Cloy
	Date		: 12/11/98

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#define SHOW_ME_THE_TILE_NUMBERS

#include <ultra64.h>
#include "incs.h"


//----------------------------------------------------------------------

#ifdef SHOW_ME_THE_TILE_NUMBERS
TEXTOVERLAY *tileNum = NULL;
unsigned long currTileNum = 0;
#endif

//----------------------------------------------------------------------

unsigned long INPUT_POLLPAUSE	= 10;

struct gameStateStruct gameState;

unsigned short screenNum = 0;
unsigned long fadingLogos = 0;

GAMETILE *firstTile;
GAMETILE **gTStart;

long hopAmt = 10;

unsigned long autoPlaying = 0;
unsigned long recordKeying = 0;
char tileString[16];
long award = 2;
short showEndLevelScreen = 1;

long bby = 0;
long babySaved = 0;

short spawnCounter = 0;
long displayingTile=0;

void DoHiscores();


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: GameProcessController

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/
void GameProcessController(long pl)
{
	static u16 button[4],lastbutton[4];
	static s16 stickX[4], stickY[4],lastStickX[4],lastStickY[4];
    
	button[pl] = controllerdata[pl].button;
	stickX[pl] = controllerdata[pl].stick_x;
	stickY[pl] = controllerdata[pl].stick_y;
		
	player[pl].hasJumped = 0;
	
	if ( autoPlaying )
	{
		LevelPlayingProcessController();
		button[pl] = 0;
		stickX[pl] = 0;
		stickY[pl] = 0;
		// find frame keys
		button[pl] = GetCurrentRecordKey();
	}

	// check if frog is using extended hop ability
	if(player[pl].isSuperHopping || player[pl].isLongHopping)
		player[pl].canJump = 0;

	if((button[pl] & CONT_UP) && !(lastbutton[pl] & CONT_UP) && (player[pl].canJump))
	{
		if(!player[pl].inputPause)
		{
			player[pl].inputPause = INPUT_POLLPAUSE;
			UpdateScore(frog[pl],hopAmt);

			player[pl].canJump = 0;
			player[pl].hasJumped = 1;
			player[pl].frogState |= FROGSTATUS_ISWANTINGU;

			// update player idleTime
			player[pl].idleTime = MAX_IDLE_TIME;

			if ( recordKeying )
				AddPlayingActionToList ( MOVEMENT_UP, frameCount );
			player[pl].extendedHopDir = MOVE_UP;
		}
	}	    

	else if((button[pl] & CONT_RIGHT) && !(lastbutton[pl] & CONT_RIGHT) && (player[pl].canJump))
	{
		if(!player[pl].inputPause)
		{
			player[pl].inputPause = INPUT_POLLPAUSE;
			UpdateScore(frog[pl],hopAmt);

			player[pl].canJump = 0;
			player[pl].hasJumped = 1;
			player[pl].frogState |= FROGSTATUS_ISWANTINGR;

			// update player idleTime
			player[pl].idleTime = MAX_IDLE_TIME;

			if ( recordKeying )
				AddPlayingActionToList ( MOVEMENT_RIGHT, frameCount );
			player[pl].extendedHopDir = MOVE_RIGHT;
		}
	}
    
	else if((button[pl] & CONT_DOWN) && !(lastbutton[pl] & CONT_DOWN) && (player[pl].canJump))
	{
		if(!player[pl].inputPause)
		{
			player[pl].inputPause = INPUT_POLLPAUSE;
			UpdateScore(frog[pl],hopAmt);
	    	
			player[pl].canJump = 0;
			player[pl].hasJumped = 1;
			player[pl].frogState |= FROGSTATUS_ISWANTINGD;

			// update player idleTime
			player[pl].idleTime = MAX_IDLE_TIME;

			if ( recordKeying )
				AddPlayingActionToList ( MOVEMENT_DOWN, frameCount );
			player[pl].extendedHopDir = MOVE_DOWN;
		}
	}
    
	else if((button[pl] & CONT_LEFT) && !(lastbutton[pl] & CONT_LEFT) && (player[pl].canJump))
	{
		if(!player[pl].inputPause)
		{
			player[pl].inputPause = INPUT_POLLPAUSE;
			UpdateScore(frog[pl],hopAmt);

			player[pl].canJump = 0;
			player[pl].hasJumped = 1;
			player[pl].frogState |= FROGSTATUS_ISWANTINGL;

			// update player idleTime
			player[pl].idleTime = MAX_IDLE_TIME;
			
			if ( recordKeying )
				AddPlayingActionToList ( MOVEMENT_LEFT, frameCount );
			player[pl].extendedHopDir = MOVE_LEFT;
		}
	}

	if((button[pl] & CONT_A) && !(lastbutton[pl] & CONT_A))
    {
		if ( gameState.mode == CAMEO_MODE )
			gameState.mode = INGAME_MODE;

		// update player idleTime
		player[pl].idleTime = MAX_IDLE_TIME;

//		frogTrail[pl] = CreateAndAddSpecialEffect( FXTYPE_TRAIL, &frog[pl]->actor->pos, &currTile[pl]->normal, 20, 0.95, 0.05, 1 );
//		frogTrail[pl]->follow = frog[pl]->actor;
//		SetFXColour( frogTrail[pl], 50, 200, 50 );

		if (((player[pl].isSuperHopping) && (player[pl].heightJumped > -125.0F)) && (!player[pl].hasDoubleJumped))
		{
			int dir=-1;

			if( button[pl] & CONT_UP )
				dir = MOVE_UP;
			else if( button[pl] & CONT_DOWN )
				dir = MOVE_DOWN;
			else if( button[pl] & CONT_LEFT )
				dir = MOVE_LEFT;
			else if( button[pl] & CONT_RIGHT )
				dir = MOVE_RIGHT;

			if( dir != -1 )
			{
				// player is superhopping - make frog double jump
				player[pl].hasDoubleJumped = 1;
				player[pl].canJump = 0;
				currTile[pl] = destTile[pl];
				frogFacing[pl] = nextFrogFacing[pl];
				camFacing = nextCamFacing;

				MoveToRequestedDestination( dir, pl );
				nextFrogFacing[pl] = frogFacing[pl] = (dir+camFacing) &3;
				player[pl].extendedHopDir = dir;

				AnimateActor(frog[pl]->actor,FROG_ANIM_FORWARDSOMERSAULT,NO,NO,0.5F,0,0);
				AnimateActor(frog[pl]->actor,FROG_ANIM_BREATHE,YES,YES,0.75F,0,0);
			}
			// To enable endless double jumping
			//player[pl].hasDoubleJumped = 0;
		}

		if((longHop) && !(player[pl].isLongHopping) && !(player[pl].inputPause))
		{
			// frog is wanting longhop
			player[pl].isLongHopping = 1;
			player[pl].hasJumped = 1;

			player[pl].inputPause = INPUT_POLLPAUSE;
			UpdateScore(frog[pl],hopAmt);

			switch(player[pl].extendedHopDir)
			{
				case MOVE_UP:
					player[pl].frogState |= FROGSTATUS_ISWANTINGLONGHOPU;
					break;
				case MOVE_RIGHT:
					player[pl].frogState |= FROGSTATUS_ISWANTINGLONGHOPR;
					break; 
				case MOVE_DOWN:
					player[pl].frogState |= FROGSTATUS_ISWANTINGLONGHOPD;
					break;
				case MOVE_LEFT:
					player[pl].frogState |= FROGSTATUS_ISWANTINGLONGHOPL;
					break;
			}

		}
		else if(!(player[pl].isSuperHopping) && !(player[pl].inputPause))
		{
			// frog is wanting superhop
			player[pl].isSuperHopping = 1;
			player[pl].hasJumped = 1;
			player[pl].hasDoubleJumped = 0;

			player[pl].inputPause = INPUT_POLLPAUSE;
			UpdateScore(frog[pl],hopAmt);

			switch(player[pl].extendedHopDir)
			{
				case MOVE_UP:
					player[pl].frogState |= FROGSTATUS_ISWANTINGSUPERHOPU;
					break;
				case MOVE_LEFT:
					player[pl].frogState |= FROGSTATUS_ISWANTINGSUPERHOPL;
					break;
				case MOVE_DOWN:
					player[pl].frogState |= FROGSTATUS_ISWANTINGSUPERHOPD;
					break;
				case MOVE_RIGHT:
					player[pl].frogState |= FROGSTATUS_ISWANTINGSUPERHOPR;
					break;
			}
		}
	}
	
	if((button[pl] & CONT_B) && (player[pl].canJump) && (tongueState & TONGUE_IDLE))
    {
		// want to use tongue
		tongueState	= TONGUE_NONE | TONGUE_SEARCHING;
		player[pl].hasJumped = 1;

		// update player idleTime
		player[pl].idleTime = MAX_IDLE_TIME;
	}

	if((button[0] & CONT_E) && !(lastbutton[0] & CONT_E))
    {
		StartDrawing("start");
	}

	if((button[0] & CONT_D) && !(lastbutton[0] & CONT_D))
    {
		// toggle between zoom in or out camera view
//		ChangeCameraSetting();
		StopDrawing("end");
	}

	if( !fixedPos && !fixedDir )
	{
		if((button[0] & CONT_F) && !(lastbutton[0] & CONT_F))
		{
			camFacing--;
			camFacing &= 3;
			if ( recordKeying )
				AddPlayingActionToList ( CAMERA_LEFT, frameCount );
		}

		if((button[pl] & CONT_C) && !(lastbutton[pl] & CONT_C))
		{
			camFacing++;
			camFacing &= 3;		
			if ( recordKeying )
				AddPlayingActionToList ( CAMERA_RIGHT, frameCount );
		}
	}

	// Croak and Croak Float
	if(!(player[pl].frogState & FROGSTATUS_ISDEAD) && !(frog[pl]->action.isCroaking))
	{
		if((button[pl] & CONT_L) && !(lastbutton[pl] & CONT_L))
		{
			frog[pl]->action.isCroaking	= 15;
			player[pl].frogState |= FROGSTATUS_ISCROAKING;
			PlayActorBasedSample(218,frog[pl]->actor,255,128);

			// update player idleTime
			player[pl].idleTime = MAX_IDLE_TIME;
		}
	}

	if(button[pl] & CONT_G)
	{
		player[pl].frogState |= FROGSTATUS_ISFLOATING;
	}
	else
	{
		player[pl].frogState &= ~FROGSTATUS_ISFLOATING;
	}


	if((button[pl] & CONT_R) && !(lastbutton[pl] & CONT_R))
	{
		if(numBabies)
		{
			bby++;
			bby %= numBabies;
			if(bTStart[bby])
				SetFroggerStartPos(bTStart[bby],pl);
		}
    }

	if((button[pl] & CONT_START) && !(lastbutton[pl] & CONT_START))
	{
		long i;
		//ScreenShot();
		
		gameState.mode = PAUSE_MODE;
		pauseMode = 1;
	
//		grabData.afterEffect = NO_EFFECT;

//		EnableTextOverlay ( pauseTitle );
		EnableTextOverlay ( continueText );
		EnableTextOverlay ( quitText );

		if (NUM_FROGS == 1)
		{
			livesTextOver->oa = livesTextOver->a;
			scoreTextOver->oa = scoreTextOver->a;
			timeTextOver->oa = timeTextOver->a;

			livesTextOver->a = 0;
			scoreTextOver->a = 0;
			timeTextOver->a = 0;
		}

		if (NUM_FROGS == 1)
		{
		
			for ( i = 0; i < 3; i++ )
				sprHeart[i]->draw = 0;
		}

		for(i=0; i<numBabies; i++)
			babyIcons[i]->draw = 0;

		lastbutton[pl] = button[pl];
    }

	lastbutton[pl] = button[pl];
	lastStickX[pl] = stickX[pl];
	lastStickY[pl] = stickY[pl];
}


/*	--------------------------------------------------------------------------------
	Function		: CreateLevelObjects
	Purpose			: creates the objects for the world / level
	Parameters		: 
	Returns			: void
	Info			: 
*/
extern ACTOR2 *backGnd;

void CreateLevelObjects(unsigned long worldID,unsigned long levelID)
{
	ACTOR2 *theActor;
	SCENIC *ts = Sc_000;
	int actCount = 0;
	char tmp[5];
	long i;
	
	// go through and add items
	while (ts)
	{
		float tv;

		stringChange(ts->name);

		if(gstrcmp(ts->name,"backdrop.obe") == 0)
		{
			backGnd = CreateAndAddActor (ts->name,ts->pos.v[0],ts->pos.v[2],ts->pos.v[1],INIT_ANIMATION,0,0);
			backGnd->actor->scale.v[0] = 5;
			backGnd->actor->scale.v[1] = 5;
			backGnd->actor->scale.v[2] = 5;
			actList = actList->next;
			if (actList)
				actList->prev = NULL;
		}
		else
		{
			
			theActor = CreateAndAddActor (ts->name,ts->pos.v[0],ts->pos.v[2],ts->pos.v[1],INIT_ANIMATION,0,0);
			dprintf"Added actor '%s'\n",ts->name));

			if(gstrcmp(ts->name,"world.obe") == 0)
			{
				theActor->flags = ACTOR_DRAW_ALWAYS;
				globalLevelActor = theActor;
			}

			for( i=0; i<4; i++ )
				tmp[i] = ts->name[i];

			tmp[4] = '\0';
			// If a water object, draw always
			if( !gstrcmp( tmp, "wat_\0" ) )
			{
				theActor->flags |= ACTOR_WATER;

				if (ts->name[4]=='f')
					theActor->flags |= ACTOR_SLIDYTEX;
			}

			tv = ts->rot.y;
			ts->rot.y = ts->rot.z;
			ts->rot.z = tv;

			GetQuaternionFromRotation (&theActor->actor->qRot,&ts->rot);

			AnimateActor(theActor->actor,0,YES,NO,0.35,0,0);
			if(ts->name[0] == 'a')
			{
				float rMin,rMax,rNum;
				if(ts->name[2] == '_')
				{
					rMin = ts->name[1] - 30;
					if(rMin = 0) 
						rMin = 10;
					rMin /= 10.0;
					AnimateActor(theActor->actor,0,YES,NO,rMin, 0, 0);
				}
				else if(ts->name[3] == '_')
				{
					rMin = ts->name[1] - 30;
					if(rMin == 0) 
						rMin = 10;
					rMin /= 10.0;
				
					rMax = ts->name[1] - 30;
					if (rMax == 0) 
						rMax = 10;
					rMax /= 10.0;

					rNum = Random(1000);
					rNum= rMin + ((rNum * (rMax - rMin)) / 1000);
										
					AnimateActor(theActor->actor,0,YES,NO,rNum, 0, 0);
				}
			}
			actCount++;
		}

		ts = ts->next;
	}

	dprintf"\n\n** ADDED %d ACTORS **\n\n",actCount));	
}


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: RunGameLoop 

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

extern float camSideOfs;
long playMode = NORMAL_PMODE;
long carryOnBabies = 1;
long clearTiles = 0;

void KillMPFrog(int num)
{
	int j;
	for (j=0; j<NUM_FROGS; j++)
	{
		if (IsPointVisible(&frog[j]->actor->pos))
		{
			TeleportActorToTile(frog[num],currTile[j],num);
			destTile[num] = currTile[j];
		}
	}

	frog[num]->action.stun = 50;
	frog[num]->action.safe = 80;
	
	if (frog[num]->action.healthPoints > 0)
		sprHeart[num*3+(--frog[num]->action.healthPoints)]->draw = 0;
}

SPRITEOVERLAY *atari,*konami,*flogo[10];
float sideSwaySpeed = 0.01,sideSwayAmt=50;

void RunGameLoop (void)
{
	unsigned long i,j;
	GAMETILE *cur;
	
	// setup for frogger point of interest
	pOIDistance = 50000.0;
	pointOfInterest = NULL;

	if(frameCount == 1)
	{	
		if ( worldVisualData [ player[0].worldNum ].levelVisualData [ player[0].levelNum ].multiPartLevel == NO_MULTI_LEV )
		{
			player[0].lives				= 3;
			player[0].score				= 0;
			player[0].timeSec			= (90*30);
			player[0].spawnTimer		= 0;
			player[0].spawnScoreLevel	= 1;
		}

		gameIsOver = 0;
		levelIsOver = 0;

		if (player[0].worldNum == WORLDID_LANGUAGE)
			if (player[0].levelNum == LEVELID_FRONTEND1)
			{
				frogFacing[0] = 3;
				atari = CreateAndAddSpriteOverlay(270,195,"atari.bmp",32,32,255,255,255,255,0);
				konami = CreateAndAddSpriteOverlay(18,195,"konami.bmp",32,32,255,255,255,255,0);
				i = 0;
				flogo[i++] = CreateAndAddSpriteOverlay(98,136,"flogo01.bmp",32,32,255,255,255,255,0);
				flogo[i++] = CreateAndAddSpriteOverlay(130,136,"flogo02.bmp",32,32,255,255,255,255,0);
				flogo[i++] = CreateAndAddSpriteOverlay(162,136,"flogo03.bmp",32,32,255,255,255,255,0);
				flogo[i++] = CreateAndAddSpriteOverlay(194,136,"flogo04.bmp",32,32,255,255,255,255,0);
				flogo[i++] = CreateAndAddSpriteOverlay(98,168,"flogo05.bmp",32,32,255,255,255,255,0);
				flogo[i++] = CreateAndAddSpriteOverlay(130,168,"flogo06.bmp",32,32,255,255,255,255,0);
				flogo[i++] = CreateAndAddSpriteOverlay(162,168,"flogo07.bmp",32,32,255,255,255,255,0);
				flogo[i++] = CreateAndAddSpriteOverlay(194,168,"flogo08.bmp",32,32,255,255,255,255,0);
				flogo[i++] = CreateAndAddSpriteOverlay(162,200,"flogo09.bmp",32,32,255,255,255,255,0);
				flogo[i++] = CreateAndAddSpriteOverlay(194,200,"flogo10.bmp",32,32,255,255,255,255,0);
				fadingLogos = 0;
			}

		bronzeCup[0] = CreateAndAddSpriteOverlay(230,20,"bronz001.bmp",32,32,255,255,255,255,0);
		bronzeCup[1] = CreateAndAddSpriteOverlay(262,20,"bronz002.bmp",32,32,255,255,255,255,0);
		bronzeCup[2] = CreateAndAddSpriteOverlay(230,52,"bronz003.bmp",32,32,255,255,255,255,0);
		bronzeCup[3] = CreateAndAddSpriteOverlay(262,52,"bronz004.bmp",32,32,255,255,255,255,0);
		bronzeCup[4] = CreateAndAddSpriteOverlay(20,20,"bronz001.bmp",32,32,255,255,255,255,0);
		bronzeCup[5] = CreateAndAddSpriteOverlay(52,20,"bronz002.bmp",32,32,255,255,255,255,0);
		bronzeCup[6] = CreateAndAddSpriteOverlay(20,52,"bronz003.bmp",32,32,255,255,255,255,0);
		bronzeCup[7] = CreateAndAddSpriteOverlay(52,52,"bronz004.bmp",32,32,255,255,255,255,0);
		for(i=0; i<8; i++)
			DisableSpriteOverlay(bronzeCup[i]);
		
		silverCup[0] = CreateAndAddSpriteOverlay(230,20,"silv001.bmp",32,32,255,255,255,255,0);
		silverCup[1] = CreateAndAddSpriteOverlay(262,20,"silv002.bmp",32,32,255,255,255,255,0);
		silverCup[2] = CreateAndAddSpriteOverlay(230,52,"silv003.bmp",32,32,255,255,255,255,0);
		silverCup[3] = CreateAndAddSpriteOverlay(262,52,"silv004.bmp",32,32,255,255,255,255,0);
		silverCup[4] = CreateAndAddSpriteOverlay(20,20,"silv001.bmp",32,32,255,255,255,255,0);
		silverCup[5] = CreateAndAddSpriteOverlay(52,20,"silv002.bmp",32,32,255,255,255,255,0);
		silverCup[6] = CreateAndAddSpriteOverlay(20,52,"silv003.bmp",32,32,255,255,255,255,0);
		silverCup[7] = CreateAndAddSpriteOverlay(52,52,"silv004.bmp",32,32,255,255,255,255,0);
		for(i=0; i<8; i++)
			DisableSpriteOverlay(silverCup[i]);

		goldCup[0] = CreateAndAddSpriteOverlay(230,20,"gold001.bmp",32,32,255,255,255,255,0);
		goldCup[1] = CreateAndAddSpriteOverlay(262,20,"gold002.bmp",32,32,255,255,255,255,0);
		goldCup[2] = CreateAndAddSpriteOverlay(230,52,"gold003.bmp",32,32,255,255,255,255,0);
		goldCup[3] = CreateAndAddSpriteOverlay(262,52,"gold004.bmp",32,32,255,255,255,255,0);
		goldCup[4] = CreateAndAddSpriteOverlay(20,20,"gold001.bmp",32,32,255,255,255,255,0);
		goldCup[5] = CreateAndAddSpriteOverlay(52,20,"gold002.bmp",32,32,255,255,255,255,0);
		goldCup[6] = CreateAndAddSpriteOverlay(20,52,"gold003.bmp",32,32,255,255,255,255,0);
		goldCup[7] = CreateAndAddSpriteOverlay(52,52,"gold004.bmp",32,32,255,255,255,255,0);
		for(i=0; i<8; i++)
			DisableSpriteOverlay(goldCup[i]);

		if (player[0].worldNum==9)
		{
		//	CreateOverlays();
			timeTextOver->draw = 0;
			livesTextOver->draw = 0;
			countdownTimer = 0;
			i = 3;
			while(i--)
				sprHeart[i]->draw = 0;			
		}

#ifdef SHOW_ME_THE_TILE_NUMBERS
		tileNum = CreateAndAddTextOverlay(0,35,tileString,YES,NO,255,255,255,255,smallFont,0,0,0);
#endif

		if(clearTiles)
		{
			cur = firstTile;
			while ( cur )
			{
				cur->state = TILESTATE_NORMAL;
				cur = cur->next;
			}
		}

		ChangeCameraSetting();
		CheckForDynamicCameraChange(currTile[0]); // TEMPORARY FIX!!

		lastActFrameCount = 0;
	}

	if (player[0].hasJumped == 1)
		fadingLogos = 1;


	if	((player[0].worldNum == WORLDID_LANGUAGE) &&
	     (player[0].levelNum == LEVELID_FRONTEND1) &&
		 (fadingLogos))
	{
		if (atari->xPos < 500)
		{
			atari->xPos+=2*gameSpeed;
			konami->xPos-=2*gameSpeed;
			for (i=0; i<10; i++)
				flogo[i]->yPos+=3*gameSpeed;
		}
	}

	if(keyFound)
		RunLevelKeyFound();

	if(babySaved && !gameIsOver && !levelIsOver)
		RunBabySavedSequence(lastBabySaved);
	
	if(player[0].frogState & FROGSTATUS_ISDEAD)
	{
		if(gameIsOver)
		{
			if (NUM_FROGS == 1)
			{
				DisableTextOverlay(livesTextOver);
				DisableTextOverlay(timeTextOver);
				DisableTextOverlay(scoreTextOver);
			}
//				livesIcon->active = 0;


			RunGameOverSequence();
			gameIsOver--;
			if(!gameIsOver)
			{
				StopDrawing("game over");
				FreeAllLists();

				player[0].levelNum = LEVELID_FRONTEND1;
				player[0].worldNum = WORLDID_FRONTEND;
				player[0].frogState &= ~FROGSTATUS_ISDEAD;
				InitLevel(player[0].worldNum,player[0].levelNum);

				frameCount = 0;
				StartDrawing("game over");
				return;
			}
		}
	}
	else
	{
		if ( levelIsOver )
		{
			if( showEndLevelScreen )
			{
				RunLevelCompleteSequence();

				if(!levelComplete1->draw)
				{
					darkenedLevel = 0;
					pauseMode = 1;
					//sprintf(levelComplete3->text,"you scored %s",scoreText);
					EnableTextOverlay(levelComplete1);
					EnableTextOverlay(levelComplete2);
					scoreTextOver->a = 255;
					EnableTextOverlay(scoreTextOver);
					//EnableTextOverlay(levelComplete3);
				}

				levelComplete1->a -= (levelComplete1->a - 255) / 20.0F;
				levelComplete2->a -= (levelComplete2->a - 255) / 20.0F;
				//levelComplete3->a -= (levelComplete3->a - 255) / 20.0F;
			}

			levelIsOver--;

			if(!levelIsOver)
			{
				DoHiscores( );

				// Only go to next level if in normal level progression.
				if( showEndLevelScreen )
				{
#ifdef MBR_DEMO
					player[0].levelNum = 0;
					player[0].worldNum = WORLDID_LABORATORY;
#else
					player[0].levelNum++;
#endif

#ifndef PC_VERSION
					StoreSaveSlot(0, 0); // Write data for Player 0 into Slot 0

					SaveGame(); // Write save games into eeprom
#endif
				}

				// Save out the game data to the currently selected slot
//				SaveGameData();

				FreeAllLists();

				frameCount = 0;

				player[0].numSpawn	= 0;
				player[0].timeSec	= 90;

				spawnCounter = 0;

				worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].levelOpen |= LEVEL_OPEN;

				InitLevel(player[0].worldNum,player[0].levelNum);

				showEndLevelScreen = 1; // Normal level progression is default

				return;
			}
				
		}
		else
		{
			if ( ( babiesSaved == numBabies ) && ( numBabies ) &&
				 ( worldVisualData [ player[0].worldNum ].levelVisualData [ player[0].levelNum ].multiPartLevel == NO_MULTI_LEV ) )
			{
				{
					camDist.v[X]	= 0;
					camDist.v[Y]	= 680;
					camDist.v[Z]	= 192;

					levelIsOver		= 400;	

//					PlaySample ( GEN_LEVEL_COMP, 0, 0, 0 );

				
					/*for ( i = 0; i < 3; i++ )
					{
						if ( 90 - ( player[0].timeSec/30 ) < worldHiScoreData[player[0].worldNum].levelScoreData[ player[0].levelNum ].scoreData[i].time )
							EnableTextOverlay ( newLevelScore );		
					}*/
				}
			}
			else
			{
				if ( ( frameCount > 15 ) )
				{
					if(!frog[0]->action.dead)	
					{
						for (i=0; i<NUM_FROGS; i++)
                        if (frog[i]->action.stun)
							frog[i]->action.stun--;
                        else
							GameProcessController(i);                                      
        			}
				}

				if(frog[0])
				{
					for (i=0; i<numBabies; i++)
					{
						if ( babies[i] )
						{
							if( babyFollow[i] )
								SetVector( &babies[i]->actor->pos, &babyFollow[i]->pos );

							AnimateActor(babies[i]->actor,0,YES,NO,1.0,0,0);
						}
					}

					CheckForDynamicCameraChange(currTile[0]); // TEMPORARY FIX!!

					CameraLookAtFrog();
					UpdateCameraPosition(0);

					for (i=0; i<NUM_FROGS; i++)
					{
					
						if ( !( player[i].frogState & FROGSTATUS_ISFLOATING ) )
							if (frog[i])
								SitAndFace(frog[i],currTile[i],frogFacing[i]);
						else if ( player[i].frogState & FROGSTATUS_ISFLOATING )
						{
							//SitAndFace(frog,currTile,frogFacing);
							//RotateFrog ( frog, frogFacing );
							//switch ( turnDir )
						}
					}
				}	  
			}
			// ENDIF 
		}
		// endif

	}
	// ENDELSEIF

	UpdatePlatforms();
	UpdateEnemies();
	UpdateSpecialEffects();
	UpdateEvents();

	//UpdateAmbientSounds();
	//	UpdateWater();

	ProcessCollectables();

	for (i=0; i<NUM_FROGS; i++)
	{
		UpdateFroggerPos(i);
		if (frog[i]->action.frogon!=-1)
		{
		  	if(player[i].canJump)
			{
				frog[i]->actor->pos.v[X] = frog[frog[i]->action.frogon]->actor->pos.v[X]+sinf(frameCount/30.0)*5;
				frog[i]->actor->pos.v[Y] = frog[frog[i]->action.frogon]->actor->pos.v[Y]+35;
				frog[i]->actor->pos.v[Z] = frog[frog[i]->action.frogon]->actor->pos.v[Z]+cosf(frameCount/27.0)*5;
			}
		}

		if (NUM_FROGS != 1)
		if ((frog[i]->action.safe == 0) && (frameCount > 20))
		if (!IsPointVisible(&frog[i]->actor->pos))
			KillMPFrog(i);
	}
	
	UpDateOnScreenInfo();

	for (i=0; i<NUM_FROGS; i++)
	{
		if (frog[i])
		{
			if (frog[i]->actor)
			{
				if (frog[i]->action.healthPoints > 0)
					frog[i]->draw = 1;
				else
					frog[i]->draw = 0;

			} 
			if (frog[i]->action.safe) 
			{
				frog[i]->actor->xluOverride = 50;
				
				if( !(--frog[i]->action.safe) )
					frog[i]->actor->xluOverride = 100;
			}
		}
	}  

	// check if player is idle
	i = NUM_FROGS;
	
//	camSideOfs = sinf(actFrameCount*sideSwaySpeed)*sideSwayAmt;

	while(i--)
	{
		player[i].idleTime-=gameSpeed;
		if(player[i].idleTime<1)
		{
			unsigned long iAnim = Random(4);
			
			switch (iAnim)
			{
				case 0:
					AnimateActor(frog[i]->actor,FROG_ANIM_SCRATCHHEAD,NO,NO,0.4F,0,0);
					if (Random(10)>6)
						AnimateActor(frog[i]->actor,FROG_ANIM_SCRATCHHEAD,NO,YES,0.4F,0,0);
					AnimateActor(frog[i]->actor,FROG_ANIM_BREATHE,YES,YES,0.4F,0,0);
					break;
				case 1:
					AnimateActor(frog[i]->actor,FROG_ANIM_DANCE1,YES,NO,0.3F,0,0);
					break;
				case 2:
					AnimateActor(frog[i]->actor,FROG_ANIM_DANCE2,YES,NO,0.3F,0,0);
					break;
				case 3:
					AnimateActor(frog[i]->actor,FROG_ANIM_DANCE3,NO,NO,0.3F,0,0);
					if (Random(10)>6)
						AnimateActor(frog[i]->actor,FROG_ANIM_DANCE1,YES,YES,0.3F,0,0);
					else
						AnimateActor(frog[i]->actor,FROG_ANIM_BREATHE,YES,YES,0.4F,0,0);
					break;
				case 4:
					AnimateActor(frog[i]->actor,FROG_ANIM_BREATHE,YES,YES,0.4F,0,0);
					break;
			}

			player[i].idleTime = 180 + Random(100);
		}
	}


#ifdef PC_VERSION
	if( gameState.multi == MULTIREMOTE )
		if( controllerdata[0].button == 0 || (controllerdata[0].button != controllerdata[0].lastbutton) )
			SendUpdateMessage( );
#endif

#ifdef SHOW_ME_THE_TILE_NUMBERS
	// displays the tile numbers
	cur = &firstTile[0];
	currTileNum = 0;
	while(cur)
	{
		cur = cur->next;

		if(cur == currTile[0])
		{
			currTileNum++;
			break;
		}

		currTileNum++;
	}
	
	if (tileNum)
	if (tileNum->text)
	{
		if (displayingTile)
			sprintf(tileNum->text,"%d",currTileNum);
		else
			sprintf(tileNum->text,"",currTileNum);
	}

#endif


}


////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RunLevelCompleteSequence()
{
	long i;

	DisableTextOverlay(livesTextOver);
	DisableTextOverlay(timeTextOver);
	//DisableTextOverlay(scoreTextOver);
	DisableTextOverlay(babySavedText);

	if(carryOnBabies)
	{					
		if(numHops_TOTAL < 110)
				award = 1;
		if(numHops_TOTAL < 105)
			award = 0;
	}

	clock->draw = 1;
	spawn->draw = 1;
	
	spawnCollected->draw = 1;
	time->draw = 1;

	nextLev1->draw = 1;
	nextLev2->draw = 1;

	if (NUM_FROGS == 1)
	{
		for ( i = 0; i < 3; i++ )
			sprHeart[i]->draw = 0;
	}

	scoreTextOver->xPos -= ((float)scoreTextOver->xPos - (100.0F)) / 15.0F;
	scoreTextOver->yPos -= ((float)scoreTextOver->yPos - (110.0F)) / 16.0F;
	
	if ( spawnCounter != player[0].numSpawn )
	{
		spawnCounter++;
	}

	sprintf ( spawnCollected->text, "%d / 150", spawnCounter );

	sprintf ( timeTemp, "%i secs", 90-(player[0].timeSec/30) );

	i = numBabies;
	while(i--)
	{
		babyIcons[i]->xPos -= ((float)babyIcons[i]->xPos - ((20.0F*i)+115.0F)) / 16.0F;
		babyIcons[i]->yPos -= ((float)babyIcons[i]->yPos - (65.0F)) / 16.0F;
		babyIcons[i]->animSpeed = 1.5F;
	}
							
	switch(award)
	{
		case 0:
			for(i=0; i<8; i++)
				EnableSpriteOverlay(goldCup[i]);
			break;
		case 1:
			for(i=0; i<8; i++)
				EnableSpriteOverlay(silverCup[i]);
			break;
		case 2:
			for(i=0; i<8; i++)
				EnableSpriteOverlay(bronzeCup[i]);
			break;
	}
}

void DoHiscores( )
{
/*	HISCORE *hs = &worldHiScoreData[player[0].worldNum][player[0].levelNum];
	long i, place = MAX_HISCORE_SLOTS;
	
	// TEST
	player[0].score = 9999999;

	// Level hiscores
	if( player[0].score > hs->score )
	{
		hs->score = player[0].score;
		hs->name[0] = player[0].name[0];
		hs->name[1] = player[0].name[1];
		hs->name[2] = player[0].name[2];
		hs->time = player[0].timeSec;
		hs->cup = award;
	}

	// Overall hiscores
	for( i=MAX_HISCORE_SLOTS-1; i >= 0; i-- )
	{
		if( hiScoreData[i].score < player[0].score )
			place = i;
	}

	if( place != MAX_HISCORE_SLOTS )
	{
		// Sort hiscore table to make room for new entry
		for( i=MAX_HISCORE_SLOTS-1; i>place; i-- )
			cmemcpy( (char *)&hiScoreData[i], (char *)&hiScoreData[i-1], sizeof(HISCORE) );

		hiScoreData[place].score = player[0].score;
		hiScoreData[place].name[0] = player[0].name[0];
		hiScoreData[place].name[1] = player[0].name[1];
		hiScoreData[place].name[2] = player[0].name[2];
		hiScoreData[place].time = player[0].timeSec;
		hiScoreData[place].cup = award;
	}*/
}
