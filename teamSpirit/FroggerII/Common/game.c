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
//TEXTOVERLAY *faceNum = NULL;

//----------------------------------------------------------------------

unsigned long INPUT_POLLPAUSE	= 0;
extern unsigned long attractTime;

struct gameStateStruct gameState;
unsigned long butPressed = 0;
unsigned short screenNum = 0;
unsigned long fadingLogos = 0;
unsigned long runAttractMode = 0;

GAMETILE *firstTile;
GAMETILE **gTStart;

long hopAmt = 10;
long idleCamera = 0;

unsigned long autoPlaying = 0;
unsigned long recordKeying = 0;
char tileString[16];
char faceString[16];

long award = 2;
short showEndLevelScreen = 1;

long bby = 0;
long babySaved = 0;

//short spawnCounter = 0;
long displayingTile=0;

TIMER scoreTimer;
TIMER modeTimer;

void UpdateCompletedLevel(unsigned long worldID,unsigned long levelID);
void DoHiscores();
extern float gCamDist;
extern TEXTURE *frogEyeOpen,*frogEyeClosed;
extern unsigned long rPlaying;
extern long rEndFrame;
extern TEXTOVERLAY *demoText;

char playDemos = 1;
extern TEXTOVERLAY *posText;
extern TEXTOVERLAY *levelnameText;
extern char levelString[];
extern char posString[];

// Switch for irritating swingy camera
unsigned char swingCam = 1;

unsigned long cLevelPlay = 0;
unsigned long levelPlayList[] = 
{
	4,1,
	1,2,
	5,2,
	1000,1000
};

void StopKeying(void)
{
	PlayKeyDone();
	player[0].worldNum = WORLDID_FRONTEND;
	player[0].levelNum = LEVELID_FRONTEND1;
	lastActFrameCount = actFrameCount;
	gameState.mode = LEVELCOMPLETE_MODE;
	GTInit( &modeTimer, 1 );
	showEndLevelScreen = 0;
}

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: GameProcessController

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

void GameProcessController(long pl)
{
	u16 button[4],lastbutton[4];

	button[pl] = controllerdata[pl].button;
	lastbutton[pl] = controllerdata[pl].lastbutton;
	if (button[pl] != lastbutton[pl])
		runAttractMode = actFrameCount+attractTime;

	player[pl].hasJumped = 0;

	if( gameState.multi != SINGLEPLAYER && multiplayerMode == MULTIMODE_BATTLE )
		return;
	
	// check if frog is using extended hop ability
	if(player[pl].isSuperHopping)
		player[pl].canJump = 0;

	if( player[pl].autohop.time )
		lastbutton[pl] &= ~(CONT_UP | CONT_DOWN | CONT_LEFT | CONT_RIGHT);

	if(player[pl].hasDoubleJumped)
	{
		if ((button[pl] & CONT_A) && (player[pl].jumpTime > 0.5f*player[pl].jumpMultiplier))
		{
			AnimateActor(frog[pl]->actor, FROG_ANIM_FLOATFALL, NO, NO, 0.5f, NO, NO);
			player[pl].frogState |= FROGSTATUS_ISFLOATING;
		}
		else
		{
			if (player[pl].frogState & FROGSTATUS_ISFLOATING)
				AnimateActor(frog[pl]->actor, FROG_ANIM_GETUPFROMFLOAT, NO, NO, 0.5f, NO, NO);
			player[pl].frogState &= ~FROGSTATUS_ISFLOATING;
		}
	}

	if((button[pl] & CONT_UP) && !(lastbutton[pl] & CONT_UP) && player[pl].canJump)
	{
		if(!player[pl].inputPause )
		{
			player[pl].inputPause = INPUT_POLLPAUSE;
//			UpdateScore(frog[pl],hopAmt);

			player[pl].canJump = 0;
			player[pl].frogState |= FROGSTATUS_ISWANTINGU;

			// update player idleTime
			player[pl].idleTime = MAX_IDLE_TIME;
			idleCamera = 0;

			if ( recordKeying )
				AddPlayingActionToList ( MOVEMENT_UP, frameCount );
			player[pl].extendedHopDir = MOVE_UP;
		}
	}	    

	else if((button[pl] & CONT_RIGHT) && !(lastbutton[pl] & CONT_RIGHT) && player[pl].canJump)
	{
		if(!player[pl].inputPause)
		{
			player[pl].inputPause = INPUT_POLLPAUSE;
//			UpdateScore(frog[pl],hopAmt);

			player[pl].canJump = 0;
			player[pl].frogState |= FROGSTATUS_ISWANTINGR;

			// update player idleTime
			player[pl].idleTime = MAX_IDLE_TIME;
			idleCamera = 0;

			if ( recordKeying )
				AddPlayingActionToList ( MOVEMENT_RIGHT, frameCount );
			player[pl].extendedHopDir = MOVE_RIGHT;
		}
	}
    
	else if((button[pl] & CONT_DOWN) && !(lastbutton[pl] & CONT_DOWN) && player[pl].canJump)
	{
		if(!player[pl].inputPause)
		{
			player[pl].inputPause = INPUT_POLLPAUSE;
//			UpdateScore(frog[pl],hopAmt);
	    	
			player[pl].canJump = 0;
			player[pl].frogState |= FROGSTATUS_ISWANTINGD;

			// update player idleTime
			player[pl].idleTime = MAX_IDLE_TIME;
			idleCamera = 0;

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
//			UpdateScore(frog[pl],hopAmt);

			player[pl].canJump = 0;
			player[pl].frogState |= FROGSTATUS_ISWANTINGL;

			// update player idleTime
			player[pl].idleTime = MAX_IDLE_TIME;
			idleCamera = 0;
			
			if ( recordKeying )
				AddPlayingActionToList ( MOVEMENT_LEFT, frameCount );
			player[pl].extendedHopDir = MOVE_LEFT;
		}
	}

	if((button[pl] & CONT_A) && !(lastbutton[pl] & CONT_A))
    {
		int dir = player[pl].extendedHopDir;

		if( button[pl] & CONT_UP )
			dir = MOVE_UP;
		else if( button[pl] & CONT_DOWN )
			dir = MOVE_DOWN;
		else if( button[pl] & CONT_LEFT )
			dir = MOVE_LEFT;
		else if( button[pl] & CONT_RIGHT )
			dir = MOVE_RIGHT;

		frogFacing[pl] = nextFrogFacing[pl];
		//camFacing = nextCamFacing;

		nextFrogFacing[pl] = frogFacing[pl] = (dir+camFacing) &3;
		player[pl].extendedHopDir = dir;
		Orientate( &frog[pl]->actor->qRot, &currTile[pl]->dirVector[frogFacing[pl]], &currTile[pl]->normal );

		if ( gameState.mode == CAMEO_MODE )
			gameState.mode = INGAME_MODE;

		// update player idleTime
		player[pl].idleTime = MAX_IDLE_TIME;
		idleCamera = 0;

		// ------------------- DOUBLE JUMP -------------------------

		if( player[pl].isSuperHopping && !player[pl].hasDoubleJumped ) // && (player[pl].heightJumped > -125.0F)
		{
			GAMETILE *old;
			int oldCamFacing;
			PlaySample(genSfx[GEN_DOUBLE_HOP],&frog[pl]->actor->pos,0,SAMPLE_VOLUME,-1/*60*/);

			// we have to keep track here of the previous tile so if it fails we don't
			// just sit in mid-air...
			old = currTile[pl];
			oldCamFacing = camFacing;

			camFacing = GetTilesMatchingDirection(currTile[pl], camFacing, destTile[pl]);
			currTile[pl] = destTile[pl];

			// player is superhopping - make frog double jump
			if (MoveToRequestedDestination( dir, pl ))
			{
				player[pl].hasDoubleJumped = 1;
				player[pl].canJump = 0;

				//camFacing = GetTilesMatchingDirection(currTile[pl], camFacing, destTile[pl]);
				nextFrogFacing[pl] = frogFacing[pl] = (dir+camFacing) &3;
				player[pl].extendedHopDir = dir;
				Orientate( &frog[pl]->actor->qRot, &currTile[pl]->dirVector[frogFacing[pl]], &currTile[pl]->normal );
			}
			else
			{
				// Restore old state :oP
				destTile[pl] = currTile[pl];
				currTile[pl] = old;
				camFacing = oldCamFacing;
				player[pl].canJump = 0;
				
				CalculateFrogJump(
					&frog[pl]->actor->pos, &destTile[pl]->centre, &destTile[pl]->normal,
					10, doubleHopFrames, pl);
			}

			AnimateActor(frog[pl]->actor,FROG_ANIM_FORWARDSOMERSAULT,NO,NO,0.35F,0,0);
			//AnimateActor(frog[pl]->actor,FROG_ANIM_BREATHE,YES,YES,0.75F,0,0);
#ifdef N64_VERSION
			StartRumble(100,1,3,ActiveController);
#endif
		}
		else if(!(player[pl].isSuperHopping) && !(player[pl].inputPause))
		{
			// Check if the player can jump.. if not, check if we're just hopping
			// What a horrible hack
			if (player[pl].canJump ||
			   (player[pl].frogState & (FROGSTATUS_ISJUMPINGTOTILE | FROGSTATUS_ISJUMPINGTOPLATFORM)))
			{
				// frog is wanting superhop
				player[pl].isSuperHopping = 1;
				player[pl].hasDoubleJumped = 0;

				player[pl].inputPause = INPUT_POLLPAUSE;
//				UpdateScore(frog[pl],hopAmt);

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
	}
	
	if((button[pl] & CONT_B) && !(lastbutton[pl] & CONT_B) && (tongue[pl].flags & TONGUE_IDLE))
    {
		// want to use tongue
		PlaySample(genSfx[GEN_FROG_TONGUE],&frog[pl]->actor->pos,0,SAMPLE_VOLUME,-1/*64*/);
		tongue[pl].flags = TONGUE_NONE | TONGUE_SEARCHING;

		// update player idleTime
		player[pl].idleTime = MAX_IDLE_TIME;
		idleCamera = 0;
	}

	if((button[pl] & CONT_E) && !(lastbutton[pl] & CONT_E))
    {
		StartDrawing("start");
	}

	if((button[pl] & CONT_D) && !(lastbutton[pl] & CONT_D))
    {
		// toggle between zoom in or out camera view
//		ChangeCameraSetting();
		StopDrawing("end");
	}

	if( !fixedPos && !fixedDir )
	{
		if((button[pl] & CONT_F) && !(lastbutton[pl] & CONT_F))
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
	if(!(player[pl].frogState & FROGSTATUS_ISDEAD) && !(player[pl].isCroaking.time))
	{
		if((button[pl] & CONT_L) && !(lastbutton[pl] & CONT_L))
		{
			GTInit( &player[pl].isCroaking, 2 );
			player[pl].frogState |= FROGSTATUS_ISCROAKING;
//			PlayActorBasedSample(218,frog[pl]->actor,255,128);

			// update player idleTime
			player[pl].idleTime = MAX_IDLE_TIME;
			idleCamera = 0;
		}
	}

#ifdef _DEBUG
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
#endif

	if((button[pl] & CONT_START) && !(lastbutton[pl] & CONT_START))
	{
		long i;

		if (rPlaying)
		{
			StopKeying();
		}
		else
		{
			
			gameState.mode = PAUSE_MODE;
			pauseMode = 1;
			idleCamera = 1;

	//		grabData.afterEffect = NO_EFFECT;
	//		EnableTextOverlay ( pauseTitle );
			sprintf(levelString,"%s",worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].description);
			sprintf(posString,"Pos %04li %04li %04li",(int)(frog[0]->actor->pos.v[0]/10),(int)(frog[0]->actor->pos.v[1]/10),(int)(frog[0]->actor->pos.v[2]/10));
			
			EnableTextOverlay ( continueText );
			EnableTextOverlay ( quitText );
#ifdef _DEBUG
			EnableTextOverlay ( posText );
			EnableTextOverlay ( levelnameText );
#endif
			if (gameState.multi==SINGLEPLAYER)
			{
				livesTextOver->oa = livesTextOver->a;
				scoreTextOver->oa = scoreTextOver->a;

				livesTextOver->a = 0;
				scoreTextOver->a = 0;

				for ( i = 0; i < 3; i++ )
					sprHeart[i]->draw = 0;
			}

			timeTextOver->oa = timeTextOver->a;
			timeTextOver->a = 0;

			for(i=0; i<numBabies; i++)
				babyIcons[i]->draw = 0;
		}
    }
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
			backGnd = CreateAndAddActor (ts->name,ts->pos.v[0],ts->pos.v[2],ts->pos.v[1],INIT_ANIMATION);
			backGnd->actor->scale.v[0] = 5;
			backGnd->actor->scale.v[1] = 5;
			backGnd->actor->scale.v[2] = 5;
			actList = actList->next;
			if (actList)
				actList->prev = NULL;
		}
		else
		{

			theActor = CreateAndAddActor (ts->name,ts->pos.v[0],ts->pos.v[2],ts->pos.v[1],INIT_ANIMATION);
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
				theActor->flags = ACTOR_WATER | ACTOR_DRAW_ALWAYS;

				if (ts->name[4]=='f')
					theActor->flags |= ACTOR_SLIDYTEX;

#ifdef N64_VERSION
				AddN64WaterObjectResource(theActor->actor);
#endif
			}

			if( !gstrcmp( tmp, "slu_\0" ) )
			{
				theActor->flags = ACTOR_WATER | ACTOR_SLUDGE | ACTOR_DRAW_ALWAYS;

				if (ts->name[4]=='f')
					theActor->flags |= ACTOR_SLIDYTEX;

#ifdef N64_VERSION
				AddN64WaterObjectResource(theActor->actor);
#endif
			}

			if( !gstrcmp( tmp, "lea_\0" ) )
			{
				theActor->flags = ACTOR_WATER | ACTOR_LEAVES | ACTOR_DRAW_ALWAYS;

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
long clearTiles = 0;


//////////////////////////////////////////
void Modify3DText(TEXT3D *t3d, char *str,unsigned char alpha);

TEXT3D *cText[4];
TEXT3D *rText[4];

unsigned char *credits[] = 
{
	"","Programming",	"Matthew Cloy",		"",
	"","\x1",			"Andy Eder",		"",
	"","\x1",			"James Hubbard",	"",
	"","\x1",			"David Swift",		"",
	"","\x1",			"James Healey",		"",


	"","Art",			"Alex Rigby","",
	"","\x1",			"Joff Scarcliffe","", 
	"","\x1",			"Bruce Millar","",
	"","\x1",			"Lauren Grindrod","",
	"","\x1",			"Simon Little","",
	"","\x1",			"Sandro Da Cruz","",
	"","\x1",			"Richard Whale","",
	"","\x1",			"Mark Turner","",
	0,0,0,0,0,0,0,0
};

unsigned long cNumber = 0;
unsigned long cFrame = 0;
unsigned long creditsActive = 0;
GAMETILE *credTile;

#define CRED_SPEED		(60*3)
#define CRED_BORDERIN	(60*0.8)
#define CRED_BORDEROUT	(60*0.8)


void RunCredits()
{
	unsigned long cn;

	if (creditsActive==0)
		return;

	if (actFrameCount>cFrame)
	{
		if (creditsActive==2)
		{
			creditsActive=3;
			return;
		}

		cFrame = actFrameCount + CRED_SPEED;
		cNumber+= 4;

		if (credits[cNumber] == 0)
			cNumber = 0;
	
		Modify3DText(cText[0], credits[cNumber],220);
		Modify3DText(cText[1], credits[cNumber+1],220);
		Modify3DText(cText[2], credits[cNumber+2],220);
		Modify3DText(cText[3], credits[cNumber+3],220);

		Modify3DText(rText[1], credits[cNumber+1],220);
		Modify3DText(rText[2], credits[cNumber+2],220);

	}

	for (cn=0; cn<4; cn++)
		cText[cn]->angle = 90+360*2;

	
	if (actFrameCount<(cFrame-CRED_SPEED+CRED_BORDERIN))
	{
		unsigned long amt;
		amt = 0xff - ((((cFrame-CRED_SPEED+CRED_BORDERIN)-actFrameCount) * 255) / CRED_BORDEROUT);
		for (cn=0; cn<4; cn++)
			if ((credits[cNumber+cn]) && (credits[cNumber+cn][0]!=1))
			{
				cText[cn]->vA = amt; 
				//cText[cn]->angle = 90+ ((amt * 360 * 2) / 0xff);
				cText[cn]->yScale = 3-(amt/128.0);
				cText[cn]->sinA = 6-(amt/(256.0/6));
			}
		
	}

	if (actFrameCount>(cFrame-CRED_BORDEROUT))
	{
		unsigned long amt;
		amt = 0xff - (((actFrameCount-(cFrame-CRED_BORDEROUT)) * 255) / CRED_BORDEROUT);
		for (cn=0; cn<4; cn++)
			if ((creditsActive == 2) || ((credits[cNumber+cn+4]) && (credits[cNumber+cn+4][0]!=1)))
			{
				cText[cn]->vA = amt; 
				//cText[cn]->angle = 90+ ((amt * 360 * 2) / 0xff);
				cText[cn]->yScale = 3-(amt/128.0);
				cText[cn]->sinA = 6-(amt/(256.0/6));
			}
	}

	for (cn=1; cn<3; cn++)
	{
		rText[cn]->angle = (360*8) - (cText[cn]->angle);
		rText[cn]->vA = ((unsigned char) cText[cn]->vA) / (6-(cn*2));
		rText[cn]->yScale = cText[cn]->yScale;
	}
}

void DeactivateCredits(void)
{
	unsigned long cn;
	if (creditsActive)
	{
		if (creditsActive == 1)
		{
			creditsActive = 2;
			cFrame = actFrameCount + CRED_BORDEROUT;
		}

		if (creditsActive == 3)
		{
			for (cn=0; cn<4; cn++)
				cText[cn]->motion &= ~T3D_CREATED;
			for (cn=1; cn<3; cn++)
				rText[cn]->motion &= ~T3D_CREATED;
			creditsActive = 0;
			
		}
	}
}

void ActivateCredits(void)
{
	unsigned long cn;

	if (creditsActive == 0)
	{
		for (cn=0; cn<4; cn++)
			cText[cn]->motion |= T3D_CREATED;
		for (cn=1; cn<3; cn++)
			rText[cn]->motion |= T3D_CREATED;
		creditsActive = 1;

		cFrame = actFrameCount + CRED_SPEED;
		cNumber = 0;

		Modify3DText(cText[0], credits[cNumber],220);
		Modify3DText(cText[1], credits[cNumber+1],220);
		Modify3DText(cText[2], credits[cNumber+2],220);
		Modify3DText(cText[3], credits[cNumber+3],220);

		Modify3DText(rText[1], credits[cNumber+1],220);
		Modify3DText(rText[2], credits[cNumber+2],220);

	}
}

void InitCredits(void)
{
	cText[0] = CreateAndAdd3DText( credits[cNumber], 180,
		255,30,0,220,
		T3D_HORIZONTAL,
		T3D_MOVE_MODGE | T3D_MOVE_SPIN | T3D_ALIGN_CENTRE,
		&zero,
		0,0,
		-30,120,-440,
		0, 0.2, 0.0 );
	
	
	cText[1] = CreateAndAdd3DText( credits[cNumber+1], 180,
		255,30,0,220,
		T3D_HORIZONTAL,
		T3D_MOVE_MODGE | T3D_MOVE_SPIN | T3D_ALIGN_CENTRE,
		&zero,
		0,0,
		-30,90,-440,
		0, 0.3, 0.0 );

	cText[2] = CreateAndAdd3DText(credits[cNumber+2], 250,
		255,220,30,220,
		T3D_HORIZONTAL,
		T3D_MOVE_MODGE | T3D_MOVE_SPIN | T3D_ALIGN_CENTRE,
		&zero,
		0,0,
		-30,60,-440,
		0, 0.25, 0.0 );

	cText[3] = CreateAndAdd3DText( credits[cNumber+3], 250,
		255,220,30,220,
		T3D_HORIZONTAL,
		T3D_MOVE_MODGE | T3D_MOVE_SPIN | T3D_ALIGN_CENTRE,
		&zero,
		0,0,
		-30,30,-440,
		0, 0.28, 0.0 );
			
				
	rText[1] = CreateAndAdd3DText( credits[cNumber+1], 180,
		255,30,130,220,
		T3D_HORIZONTAL,
		T3D_MOVE_MODGE | T3D_MOVE_SPIN | T3D_ALIGN_CENTRE,
		&zero,
		0,0,
		-30,-90-30,-440,
		3, 0.3, 0.0 );

	rText[2] = CreateAndAdd3DText(credits[cNumber+2], 250,
		255,220,130,220,
		T3D_HORIZONTAL,
		T3D_MOVE_MODGE | T3D_MOVE_SPIN | T3D_ALIGN_CENTRE,
		&zero,
		0,0,
		-30,-60-30,-440,
		3, 0.25, 0.0 );
}

///////////////////////////////////////////////////////

void RunGameLoop (void)
{
	unsigned long i,j;
	GAMETILE *cur;
	
	// setup for frogger point of interest
	pOIDistance = 50000.0;
	pointOfInterest = NULL;

	// Take this out for release
#ifdef SHOW_ME_THE_TILE_NUMBERS
	if( frameCount==1 )
	{
/*		TEXTURE *tex;
		VECTOR pos;

		FindTexture( &tex, UpdateCRC("prc_frog.bmp"), YES );
		SetVector( &pos, &gTStart[0]->centre );
		pos.v[Z] -= 100;
		pos.v[Y] += 20;

		CreateAndAddRandomPoly( tex, &pos, &gTStart[0]->normal, 64, 64 );
*/
		tileNum = CreateAndAddTextOverlay(0,35,tileString,YES,255,bigFont,0,0);
	}

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
//	if (faceNum)
//		if (faceNum->text)
//			sprintf(faceNum->text,"%d",camFacing);
	if (tileNum)
		if (tileNum->text)
		{
			if (displayingTile)
				sprintf(tileNum->text,"%d",currTileNum);
			else
				sprintf(tileNum->text,"",currTileNum);
		}
#endif

#ifdef PC_VERSION
	if (player[0].worldNum == WORLDID_FRONTEND)
			if (player[0].levelNum == LEVELID_FRONTEND1)
			{
				if ((currTile[0] == &(firstTile[22])))
				{
					if (creditsActive==0)
						ActivateCredits();
				}
				else if( currTile[0] == &firstTile[2] )
				{
					FreeAllLists();

					frameCount = 0;
					gameState.mode = MENU_MODE;
					gameState.menuMode = LEVELSELECT_MODE;
					return;
				}
				else
					DeactivateCredits();
				
				RunCredits();
			}

	if (rPlaying)
	{
		if (actFrameCount > rEndFrame)
		{
			StopKeying();
			return;
		}
		demoText->oa = demoText->a = (sinf(actFrameCount*0.05)+1) * 128;
	}

	if	((player[0].worldNum == WORLDID_FRONTEND) &&
	     (player[0].levelNum == LEVELID_FRONTEND1) )
	{
		if ((frameCount > 15) && playDemos && (gameState.mode != LEVELCOMPLETE_MODE) && (runAttractMode < actFrameCount))
		{
			player[0].worldNum = levelPlayList[cLevelPlay];
			player[0].levelNum = levelPlayList[cLevelPlay+1];
			
			cLevelPlay+=2;
			if (levelPlayList[cLevelPlay]>100)
				cLevelPlay = 0;
			
			rPlaying = 1;
			lastActFrameCount = actFrameCount;
			gameState.mode = LEVELCOMPLETE_MODE;
			GTInit( &modeTimer, 1 );
			showEndLevelScreen = 0;
		}

		if( fadingLogos )
		{
			if (atari->xPos < 500)
			{
				atari->xPos+=2*gameSpeed;
				konami->xPos-=2*gameSpeed;
				for (i=0; i<10; i++)
					flogo[i]->yPos+=3*gameSpeed;

				islLogo[0]->xPos+=2*gameSpeed;
				islLogo[1]->xPos+=2*gameSpeed;
				islLogo[2]->xPos+=2*gameSpeed;
			}
			else
			{
				atari->draw = 0;
				konami->draw = 0;
				for (i=0; i<10; i++)
					flogo[i]->draw = 0;

				islLogo[0]->draw = 0;
				islLogo[1]->draw = 0;
				islLogo[2]->draw = 0;
			}
		}
		else
		{
			if (player[0].hasJumped == 1)
			{

				fadingLogos = 1;
			}
		}
	}
#endif

//		if( babySaved )
//			RunBabySavedSequence(lastBabySaved);

	if ( babiesSaved==numBabies && numBabies && (worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum ].multiPartLevel == NO_MULTI_LEV) )
	{
		camDist.v[X]	= 0;
		camDist.v[Y]	= 680;
		camDist.v[Z]	= 192;
		UpdateCompletedLevel(player[0].worldNum,player[0].levelNum);
		gameState.mode = LEVELCOMPLETE_MODE;
		GTInit( &modeTimer, 8 );

		StartLevelComplete();
	}
	else
	{
		if( frameCount > 15 )
		{
			for (i=0; i<NUM_FROGS; i++)
				if( !player[i].dead.time && !(player[i].frogState & FROGSTATUS_ISDEAD) )
					if ( player[i].stun.time)
					{
						GTUpdate( &player[i].stun, -1 );
					}
					else
					{
						GameProcessController(i);                                      
					}
		}

		UpdateCameraPosition();
	}

	for( i=0; i<NUM_FROGS; i++ )
	{
		if( frog[i] )
		{
			UpdateFroggerPos(i);

			if( !(player[i].frogState & FROGSTATUS_ISDEAD) )
			{
				frog[i]->draw = 1;
				if (player[i].safe.time)
				{
					SPECFX *fx;
					VECTOR pos;

					GTUpdate( &player[i].safe, -1 );

					SetVector( &pos, &currTile[i]->normal );
					ScaleVector( &pos, 15 );
					pos.v[X] += Random(2)?(Random(20)+5):(-Random(20)-5);
					pos.v[Y] += Random(2)?(Random(20)+5):(-Random(20)-5);
					pos.v[Z] += Random(2)?(Random(20)+5):(-Random(20)-5);
					AddToVector( &pos, &frog[i]->actor->pos );

					if( (fx = CreateAndAddSpecialEffect( FXTYPE_TWINKLE, &pos, &currTile[i]->normal, 20+Random(10), 0, 0, Random(3)+1 )) )
					{
						fx->tilt = 2;
						SetFXColour( fx, 220, 220, 255 );
					}
				}

				if( player[i].idleEnable )
				{
					player[i].idleTime-=gameSpeed;
					if(player[i].idleTime<1)
					{
						FroggerIdleAnim(i);
						player[i].idleTime = MAX_IDLE_TIME + Random(MAX_IDLE_TIME);
					}
				}
			}
		}
	}  

	UpdatePlatforms();
	UpdateEnemies();
	UpdateSpecialEffects();
	UpdateEvents();
	UpdateAmbientSounds();

	ProcessCharacters( );
	ProcessCollectables();

	if( gameState.multi == SINGLEPLAYER )
		UpDateOnScreenInfo();
	else
		RunMultiplayer();

	// Send network update packet
#ifdef PC_VERSION
	if( gameState.multi == MULTIREMOTE )
		if( controllerdata[0].button == 0 || (controllerdata[0].button != controllerdata[0].lastbutton) )
			SendUpdateMessage( );
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		hs->time = scoreTimer.time;
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
		hiScoreData[place].time = scoreTimer.time;
		hiScoreData[place].cup = award;
	}*/

}






















void LevelSelProcessController(long pl);

#define MAX_LEVELSTRING 10
#define MAX_OPTIONS 5
#define MAX_SPARKLES 15

#define DEC_ALPHA(x) {if (x->a > gameSpeed * fadeSpeed)	{x->a -= gameSpeed * fadeSpeed;} else	{x->a = 0; x->draw = 0;}}
#define INC_ALPHA(x,y) {if (x->a < y-gameSpeed * fadeSpeed)	{x->a += gameSpeed * fadeSpeed;} else	{x->a = y;}}

TEXTOVERLAY *worldText, *arcadeText,*selectText,*statusText,*pcText,*bestText,*parText;
TEXTOVERLAY *levelText[MAX_LEVELSTRING];
SPRITEOVERLAY *worldBak,*titleBak,*statusBak, *infoBak, *levelPic;
SPRITEOVERLAY *sparkles[MAX_SPARKLES];

char arcadeStr[64];
char *selectStr = "select level to play";
char *statusStr = "Cursors select   Enter confirm   Esc cancel";

char *timeStr = "2 51 Matthew";
char *parStr =  "Par  Set By";

char worldStr[64] = "WorldID";
char pcStr[64] = "00 percent";
char levelStr[MAX_LEVELSTRING][64] = {"","","","","","","","","",""};
unsigned long cLevel,cWorld;
float fadeSpeed = 5;
unsigned long numPlayers = 2;

char options[MAX_OPTIONS][64] = {"Music Volume","SFX Volume","Controller Config","Play Music Track 0","Play SFX 0"};

void RunFrontendGameLoop (void)
{
	unsigned long i,j;
	GAMETILE *cur;
	
	// setup for frogger point of interest
	pOIDistance = 50000.0;
	pointOfInterest = NULL;


	if (frameCount == 1)
	{
		strcpy(arcadeStr,"No Mode");
		for (i=0; i<MAX_SPARKLES; i++)
		{
			sparkles[i] = CreateAndAddSpriteOverlay(20,90,"flash.bmp",8,8,(rand()*0xff)/RAND_MAX,XLU_ADD);
			sparkles[i]->num = 1;
		}
		
		arcadeText = CreateAndAddTextOverlay(20,10,arcadeStr,YES,255,bigFont,0,0); 
		selectText = CreateAndAddTextOverlay(20,28,selectStr,YES,255,smallFont,0,0); 
		worldText = CreateAndAddTextOverlay(20,65,worldStr,NO,255,smallFont,0,0);
		statusText = CreateAndAddTextOverlay(20,240-18,statusStr,YES,255,smallFont,0,0);
		pcText = CreateAndAddTextOverlay(320,65+80,pcStr,NO,255,smallFont,0,0);
		
		parText = CreateAndAddTextOverlay(320,110+75,parStr,NO,255,smallFont,0,0);		
		bestText = CreateAndAddTextOverlay(320,120+75,timeStr,NO,255,smallFont,TEXTOVERLAY_WAVECHARS,0);
		bestText->waveAmplitude = 4;
		
		selectText->r = statusText->r = 0;
		selectText->g = statusText->g = 0xff;
		selectText->b = statusText->b = 10;
		for (i=0; i<MAX_LEVELSTRING; i++)
			levelText[i] = CreateAndAddTextOverlay(20,85+i*10,levelStr[i],NO,255,smallFont,TEXTOVERLAY_WAVECHARS,0);		
			
		
		cLevel = 0;
		cWorld = 0;
		worldBak = CreateAndAddSpriteOverlay(10,55,"wback.bmp",130,80,190,0);
		worldBak->r = 200;
		worldBak->g = 100;
		worldBak->b = 10;

		levelPic = CreateAndAddSpriteOverlay(320-10-130+10,55+10+80,"garden1.bmp",34,30,230,0);
		
		infoBak = CreateAndAddSpriteOverlay(320-10-130,55+80,"wback.bmp",130,80,190,0);
		infoBak->r = worldBak->r;
		infoBak->g = worldBak->g;
		infoBak->b = worldBak->b;
		
		
		titleBak = CreateAndAddSpriteOverlay(20,5,"wback.bmp",320-40,16+8+10,190,0);
		titleBak->r = worldBak->r;
		titleBak->g = worldBak->g;
		titleBak->b = worldBak->b;

		statusBak = CreateAndAddSpriteOverlay(20,240-5-18,"wback.bmp",320-40,18,190,0);
		statusBak->r = worldBak->r;
		statusBak->g = worldBak->g;
		statusBak->b = worldBak->b;

		arcadeText->draw = 0;
		selectText->draw = 0;
		worldText->draw = 0;
		statusText->draw = 0;
		worldBak->draw = 0;
		infoBak->draw = 0;
		titleBak->draw = 0;
		statusBak->draw = 0;
		levelPic->draw = 0;
		pcText->draw = 0;
		bestText->draw = 0;
		parText->draw = 0;
		
	}

	
	
	for (i=0; i<MAX_LEVELSTRING; i++)
		levelStr[i][0] = 0;

	for (i=0; i<10; i++)
		flogo[i]->num = 1;
		
	if( fadingLogos )
	{
		if (atari->xPos < 500)
		{
			atari->xPos+=2*gameSpeed;
			konami->xPos-=2*gameSpeed;
			for (i=0; i<10; i++)
				flogo[i]->yPos+=3*gameSpeed;

			islLogo[0]->xPos+=2*gameSpeed;
			islLogo[1]->xPos+=2*gameSpeed;
			islLogo[2]->xPos+=2*gameSpeed;
		}
		else
		{
			atari->draw = 0;
			konami->draw = 0;
			for (i=0; i<10; i++)
				flogo[i]->draw = 0;

			islLogo[0]->draw = 0;
			islLogo[1]->draw = 0;
			islLogo[2]->draw = 0;
		}
	}
	else
	{
		if (player[0].hasJumped == 1)
		{

			fadingLogos = 1;
		}
	}
	
	sprintf(pcStr,"%02i percent",worldVisualData[cWorld].levelVisualData[cLevel].percent);
	
	for (i=0; i<MAX_SPARKLES; i++)
	{
		unsigned long spkLev;

		if (sparkles[i]->a > gameSpeed * 3)
			sparkles[i]->a-= gameSpeed * 3;
		else
		{
			if (currTileNum == 5 || currTileNum == 4)
				spkLev = cLevel;
			else
				spkLev = cWorld;
			
			sparkles[i]->yPosTo = sparkles[i]->yPos = (rand()*8)/RAND_MAX + levelText[spkLev]->yPos-4;			
			sparkles[i]->a = (rand()*160)/RAND_MAX;
			sparkles[i]->xPos = levelText[spkLev]->xPos-4 + (rand() * (levelText[spkLev]->wtotal/2))/RAND_MAX;		
		}
			
		sparkles[i]->speed = 0;
		sparkles[i]->draw = levelText[cLevel]->draw;
	}
		
	// Take this out for release
	if( frameCount==1 )
		tileNum = CreateAndAddTextOverlay(0,35,tileString,YES,255,bigFont,0,0);		

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
	if (currTileNum==164)
	{
		frogFacing[0] = 3;
		Orientate( &frog[0]->actor->qRot, &currTile[0]->dirVector[frogFacing[0]], &currTile[0]->normal );	
	}

	
	if (currTileNum!=5 && currTileNum!=3 && currTileNum!=1 && currTileNum!=4)
	{
		if( frameCount > 15 )
			GameProcessController(0);                                      
		
		DEC_ALPHA(arcadeText);
		DEC_ALPHA(pcText);
		DEC_ALPHA(bestText);
		DEC_ALPHA(parText);
		
		DEC_ALPHA(selectText);
		DEC_ALPHA(worldText);
		DEC_ALPHA(statusText);
		DEC_ALPHA(worldBak);
		DEC_ALPHA(infoBak);
		DEC_ALPHA(titleBak);
		DEC_ALPHA(statusBak);
		DEC_ALPHA(levelPic);

		for (i=0; i<MAX_LEVELSTRING; i++)
			DEC_ALPHA(levelText[i]);
		
		worldText->centred = 0;
		if (arcadeText->draw == 0)
		{
			worldBak->xPos = -100;
			infoBak->xPos = 350;
			titleBak->yPos = -100;
			statusBak->yPos = 300;
			levelPic->xPos = 400;

			worldBak->speed = 1;
			infoBak->speed = 1;
			titleBak->speed = 1;
			statusBak->speed = 1;
			levelPic->speed = 1;

			for (i=0; i<MAX_LEVELSTRING; i++)
			{
				levelText[i]->draw = 0;
				levelText[i]->centred = 0;
			}
		}

		
	}
	if (currTileNum==1)
	{
		worldBak->xPosTo = 95;
		worldBak->height = 80+20;
		strcpy(arcadeStr,"Options");
		sprintf(worldStr,"Some title text",numPlayers);
		
		frogFacing[0] = 3;
		Orientate( &frog[0]->actor->qRot, &currTile[0]->dirVector[frogFacing[0]], &currTile[0]->normal );

		for (i=0; i<6; i++)
		{
				strcpy (levelStr[i],options[i]);
				levelText[i]->b = 0;
				levelText[i]->g = 100;
				levelText[i]->r = 200;
				levelText[i]->waveAmplitude = 0;
		}

		levelText[cWorld]->waveAmplitude = 4;
		
		worldBak->xPos = worldBak->xPosTo;
		
				
		bestText->r = levelText[cWorld]->r = 128+(sinf(1+actFrameCount*0.09)+1)*64;
		bestText->g = levelText[cWorld]->g = 128+(sinf(2+actFrameCount*0.07)+1)*64;
		bestText->b = levelText[cWorld]->b = 10;
	
		arcadeText->yPos = titleBak->yPos+5;
		selectText->yPos = titleBak->yPos+23;
		statusText->yPos = statusBak->yPos+5;
		
		worldBak->draw = 1;
		titleBak->draw = 1;
		statusBak->draw = 1;
		statusText->draw = 1;
		arcadeText->draw = 1;
		selectText->draw = 1;
		worldText->draw = 1;
		
		worldText->centred = 1;
		
		INC_ALPHA(titleBak,0xff);
		INC_ALPHA(statusBak,0xff);
		INC_ALPHA(selectText,0xff);
		INC_ALPHA(arcadeText,0xff);
		INC_ALPHA(statusText,0xff);
		INC_ALPHA(worldBak,0xff);
		INC_ALPHA(worldText,0xff);

		DEC_ALPHA(pcText);
		DEC_ALPHA(bestText);
		DEC_ALPHA(parText);
				
		
		DEC_ALPHA(infoBak);
		DEC_ALPHA(levelPic);

		for (i=0; i<MAX_LEVELSTRING; i++)
			levelText[i]->xPos = worldText->xPos;
		
		for (i=0; i<MAX_LEVELSTRING; i++)
		{
			levelText[i]->draw = 1;
			INC_ALPHA(levelText[i],0xff);
			levelText[i]->centred = 1;
		}
		
		if (arcadeText->draw == 0)
		{
			worldBak->xPos = -100;
			infoBak->xPos = 350;
			levelPic->xPos = 400;

			worldBak->speed = 1;
			infoBak->speed = 1;
			levelPic->speed = 1;
		}

		LevelSelProcessController(0);
	}
	

	if (currTileNum==3)
	{
		worldBak->xPosTo = 95;
		worldBak->height = 80+20;
		strcpy(arcadeStr,"Multiplayer Mode");
		sprintf(worldStr,"%i player mode",numPlayers);
		
		frogFacing[0] = 3;
		Orientate( &frog[0]->actor->qRot, &currTile[0]->dirVector[frogFacing[0]], &currTile[0]->normal );

		for (i=0; i<6; i++)
		{
				strcpy (levelStr[i],worldVisualData[i].levelVisualData[3].description);
				levelText[i]->b = 0;
				levelText[i]->g = 100;
				levelText[i]->r = 200;
				levelText[i]->waveAmplitude = 0;
		}
		levelText[cWorld]->waveAmplitude = 4;
		strcpy (levelStr[0],worldVisualData[0].levelVisualData[1].description);

		worldBak->xPos = worldBak->xPosTo;
		
				
		bestText->r = levelText[cWorld]->r = 128+(sinf(1+actFrameCount*0.09)+1)*64;
		bestText->g = levelText[cWorld]->g = 128+(sinf(2+actFrameCount*0.07)+1)*64;
		bestText->b = levelText[cWorld]->b = 10;
	
		arcadeText->yPos = titleBak->yPos+5;
		selectText->yPos = titleBak->yPos+23;
		statusText->yPos = statusBak->yPos+5;
		
		worldBak->draw = 1;
		titleBak->draw = 1;
		statusBak->draw = 1;
		statusText->draw = 1;
		arcadeText->draw = 1;
		selectText->draw = 1;
		worldText->draw = 1;
		
		worldText->centred = 1;
		
		INC_ALPHA(titleBak,0xff);
		INC_ALPHA(statusBak,0xff);
		INC_ALPHA(selectText,0xff);
		INC_ALPHA(arcadeText,0xff);
		INC_ALPHA(statusText,0xff);
		INC_ALPHA(worldBak,0xff);
		INC_ALPHA(worldText,0xff);

		DEC_ALPHA(pcText);
		DEC_ALPHA(bestText);
		DEC_ALPHA(parText);
				
		
		DEC_ALPHA(infoBak);
		DEC_ALPHA(levelPic);

		for (i=0; i<MAX_LEVELSTRING; i++)
			levelText[i]->xPos = worldText->xPos;
		
		for (i=0; i<MAX_LEVELSTRING; i++)
		{
			levelText[i]->draw = 1;
			INC_ALPHA(levelText[i],0xff);
			levelText[i]->centred = 1;
		}
		
		if (arcadeText->draw == 0)
		{
			worldBak->xPos = -100;
			infoBak->xPos = 350;
			levelPic->xPos = 400;

			worldBak->speed = 1;
			infoBak->speed = 1;
			levelPic->speed = 1;
		}

		LevelSelProcessController(0);
	}
		
	if (currTileNum==5)
	{
		strcpy (worldStr,worldVisualData[cWorld].description);
		worldBak->height = worldVisualData[cWorld].numLevels*10 + 40;
		worldBak->xPosTo = 10;

		levelText[cLevel]->waveAmplitude = 4;
		for (i=0; i<worldVisualData[cWorld].numLevels; i++)
		{
				strcpy (levelStr[i],worldVisualData[cWorld].levelVisualData[i].description);
				levelText[i]->b = 0;
				levelText[i]->g = 100;
				levelText[i]->r = 200;
				levelText[i]->waveAmplitude = 0;
		}

		bestText->r = levelText[cLevel]->r = 128+(sinf(1+actFrameCount*0.09)+1)*64;
		bestText->g = levelText[cLevel]->g = 128+(sinf(2+actFrameCount*0.07)+1)*64;
		bestText->b = levelText[cLevel]->b = 10;
	
		levelText[cLevel]->waveAmplitude = 4;

		strcpy(arcadeStr,"Arcade Mode");

		frogFacing[0] = 3;
		Orientate( &frog[0]->actor->qRot, &currTile[0]->dirVector[frogFacing[0]], &currTile[0]->normal );
	
		LevelSelProcessController(0);
		arcadeText->yPos = titleBak->yPos+5;
		selectText->yPos = titleBak->yPos+23;
		pcText->xPos = infoBak->xPos+52;
		bestText->xPos = infoBak->xPos+10;
		parText->xPos = infoBak->xPos+10;
		
		statusText->yPos = statusBak->yPos+5;
		worldText->xPos = worldBak->xPos + 10;
		for (i=0; i<MAX_LEVELSTRING; i++)
			levelText[i]->xPos = worldText->xPos;

		pcText->draw = 1;
		bestText->draw = 1;
		parText->draw = 1;
		
		arcadeText->draw = 1;
		selectText->draw = 1;
		worldText->draw = 1;
		statusText->draw = 1;
		worldBak->draw = 1;
		infoBak->draw = 1;
		titleBak->draw = 1;
		statusBak->draw = 1;
		levelPic->draw = 1;
		
		INC_ALPHA(arcadeText,0xff);
		INC_ALPHA(pcText,0xff);
		
		INC_ALPHA(bestText,0xff);
		INC_ALPHA(parText,0xff);
		INC_ALPHA(selectText,0xff);
		INC_ALPHA(worldText,0xff);
		INC_ALPHA(statusText,0xff);
		INC_ALPHA(worldBak,0xff);
		INC_ALPHA(infoBak,0xff);
		INC_ALPHA(titleBak,0xff);
		INC_ALPHA(statusBak,0xff);
		INC_ALPHA(levelPic,0xff);
		
		for (i=0; i<MAX_LEVELSTRING; i++)
		{
			levelText[i]->draw = 1;
			INC_ALPHA(levelText[i],0xff);
		}
	}

	if (currTileNum==4)
	{
		sprintf (worldStr,"Chapter %i",cWorld+1);
		worldBak->height = worldVisualData[cWorld].numLevels*10 + 40;
		worldBak->xPosTo = 95;

		levelText[cLevel]->waveAmplitude = 4;
		for (i=0; i<worldVisualData[cWorld].numLevels; i++)
		{
				strcpy (levelStr[i],worldVisualData[cWorld].levelVisualData[i].description);
				levelText[i]->b = 0;
				levelText[i]->g = 100;
				levelText[i]->r = 200;
				levelText[i]->waveAmplitude = 0;
		}

		bestText->r = levelText[cLevel]->r = 128+(sinf(1+actFrameCount*0.09)+1)*64;
		bestText->g = levelText[cLevel]->g = 128+(sinf(2+actFrameCount*0.07)+1)*64;
		bestText->b = levelText[cLevel]->b = 10;
	
		levelText[cLevel]->waveAmplitude = 4;

		strcpy(arcadeStr,"Story Mode");

		frogFacing[0] = 3;
		Orientate( &frog[0]->actor->qRot, &currTile[0]->dirVector[frogFacing[0]], &currTile[0]->normal );
		
		worldBak->xPos = worldBak->xPosTo;
		
		LevelSelProcessController(0);
		arcadeText->yPos = titleBak->yPos+5;
		selectText->yPos = titleBak->yPos+23;
		pcText->xPos = infoBak->xPos+52;
		bestText->xPos = infoBak->xPos+10;
		parText->xPos = infoBak->xPos+10;
		
		statusText->yPos = statusBak->yPos+5;
		worldText->xPos = worldBak->xPos + 10;
		for (i=0; i<MAX_LEVELSTRING; i++)
			levelText[i]->xPos = worldText->xPos;

		arcadeText->draw = 1;
		selectText->draw = 1;
		worldText->draw = 1;
		statusText->draw = 1;
		worldBak->draw = 1;
		infoBak->draw = 1;
		titleBak->draw = 1;
		statusBak->draw = 1;
		
		INC_ALPHA(arcadeText,0xff);
		INC_ALPHA(pcText,0xff);
		
		//INC_ALPHA(bestText,0xff);
		//INC_ALPHA(parText,0xff);
		INC_ALPHA(selectText,0xff);
		INC_ALPHA(worldText,0xff);
		INC_ALPHA(statusText,0xff);
		INC_ALPHA(worldBak,0xff);
		INC_ALPHA(titleBak,0xff);
		INC_ALPHA(statusBak,0xff);
		//INC_ALPHA(levelPic,0xff);
		
		for (i=0; i<MAX_LEVELSTRING; i++)
		{
			levelText[i]->draw = 1;
			INC_ALPHA(levelText[i],0xff);
			levelText[i]->centred = 1;
		}
	}

	UpdateCameraPosition();
	
	for( i=0; i<NUM_FROGS; i++ )
	{
		if( frog[i] )
		{
			UpdateFroggerPos(i);

			if( !(player[i].frogState & FROGSTATUS_ISDEAD) )
			{
				frog[i]->draw = 1;
				if (player[i].safe.time)
				{
					SPECFX *fx;
					VECTOR pos;

					GTUpdate( &player[i].safe, -1 );

					SetVector( &pos, &currTile[i]->normal );
					ScaleVector( &pos, 15 );
					pos.v[X] += Random(2)?(Random(20)+5):(-Random(20)-5);
					pos.v[Y] += Random(2)?(Random(20)+5):(-Random(20)-5);
					pos.v[Z] += Random(2)?(Random(20)+5):(-Random(20)-5);
					AddToVector( &pos, &frog[i]->actor->pos );

					if( (fx = CreateAndAddSpecialEffect( FXTYPE_TWINKLE, &pos, &currTile[i]->normal, 20+Random(10), 0, 0, Random(3)+1 )) )
					{
						fx->tilt = 2;
						SetFXColour( fx, 220, 220, 255 );
					}
				}

				if( player[i].idleEnable )
				{
					player[i].idleTime-=gameSpeed;
					if(player[i].idleTime<1)
					{
						FroggerIdleAnim(i);
						player[i].idleTime = MAX_IDLE_TIME + Random(MAX_IDLE_TIME);
					}
				}
			}
		}
	}  

	UpdatePlatforms();
	UpdateEnemies();
	UpdateSpecialEffects();
	UpdateEvents();
	UpdateAmbientSounds();

	ProcessCharacters( );
	ProcessCollectables();

	UpDateOnScreenInfo();

	// Send network update packet
}


void LevelSelProcessController(long pl)
{
	unsigned long changedLevel = 0;
	u16 button[4],lastbutton[4];

	button[pl] = controllerdata[pl].button;
	lastbutton[pl] = controllerdata[pl].lastbutton;
	
	
	if((button[pl] & CONT_UP) && !(lastbutton[pl] & CONT_UP) && player[pl].canJump)
	{
		if (currTileNum==5 || currTileNum==4)
		{		
			if (cLevel>0)
				cLevel--;
			changedLevel = 1;
		}

		if (currTileNum==3  || currTileNum==1)
		{
			if (cWorld>0)
				cWorld--;
			changedLevel = 1;		
		}
	}	    
	else if((button[pl] & CONT_RIGHT) && !(lastbutton[pl] & CONT_RIGHT) && player[pl].canJump)
	{
		if (currTileNum==5 || currTileNum==4)
		{				
			if (cWorld<8)
				cWorld++;
			changedLevel = 1;
		}

		if (currTileNum==3)
		{
			if (numPlayers<4)
				numPlayers++;
		}
	}
    else if((button[pl] & CONT_DOWN) && !(lastbutton[pl] & CONT_DOWN) && player[pl].canJump)
	{
		if (currTileNum==5 || currTileNum==4)
		{				
			if (cLevel<worldVisualData[cWorld].numLevels-1)
				cLevel++;
			changedLevel = 1;
		}

		if (currTileNum==3 || currTileNum==1)
		{		
			if (cWorld<8)
				cWorld++;
			changedLevel = 1;
		}
	}
    else if((button[pl] & CONT_LEFT) && !(lastbutton[pl] & CONT_LEFT) && (player[pl].canJump))
	{
		if (currTileNum==5 || currTileNum==4)
		{		
		
			if (cWorld>0)
				cWorld--;
			changedLevel = 1;
		}

		if (currTileNum==3)
		{
			if (numPlayers>2)
				numPlayers--;
		}
	}

	if (changedLevel)
		FindTexture(&levelPic->frames[0],UpdateCRC(worldVisualData[cWorld].levelVisualData[cLevel].texCRC),YES);

	if((button[pl] & CONT_A) && !(lastbutton[pl] & CONT_A))
    {
		if (currTileNum==5)
		{
			if(gameState.multi != MULTIREMOTE)
			{
				NUM_FROGS = 1;
				gameState.multi = SINGLEPLAYER;
			}
			player[0].worldNum = cWorld;
			player[0].levelNum = cLevel;			
		}
					
		if (currTileNum==3)
		{
			if(gameState.multi != MULTIREMOTE)
				gameState.multi = MULTILOCAL;
			NUM_FROGS = numPlayers;
			player[0].worldNum = cWorld;
			player[0].levelNum = 3;
			if (cWorld==WORLDID_GARDEN)
				player[0].levelNum = 1;			
		}

		lastActFrameCount = actFrameCount;
		gameState.mode = LEVELCOMPLETE_MODE;
		GTInit( &modeTimer, 1 );
		showEndLevelScreen = 0;		
	}
	
	if((button[pl] & CONT_B) && !(lastbutton[pl] & CONT_B) && (tongue[pl].flags & TONGUE_IDLE))
    {
	}

	if((button[pl] & CONT_E) && !(lastbutton[pl] & CONT_E))
    {
	}

	if((button[pl] & CONT_D) && !(lastbutton[pl] & CONT_D))
    {
	}

	if((button[pl] & CONT_START) && !(lastbutton[pl] & CONT_START))
	{
		
		player[pl].inputPause = INPUT_POLLPAUSE;
	    
		player[pl].canJump = 0;
		player[pl].frogState |= FROGSTATUS_ISWANTINGD;

		// update player idleTime
		player[pl].idleTime = MAX_IDLE_TIME;
		idleCamera = 0;

		if ( recordKeying )
			AddPlayingActionToList ( MOVEMENT_DOWN, frameCount );
		player[pl].extendedHopDir = MOVE_DOWN;
	}
}
