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

struct gameStateStruct gameState;

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
extern TEXTOVERLAY *demoText;

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
	0,0,
	2,2,
	5,2,
	1000,1000
};

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
	
	// check if frog is using extended hop ability
	if(player[pl].isSuperHopping)
		player[pl].canJump = 0;

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
			UpdateScore(frog[pl],hopAmt);

			player[pl].canJump = 0;
			player[pl].hasJumped = 1;
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
			UpdateScore(frog[pl],hopAmt);

			player[pl].canJump = 0;
			player[pl].hasJumped = 1;
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
			UpdateScore(frog[pl],hopAmt);
	    	
			player[pl].canJump = 0;
			player[pl].hasJumped = 1;
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
			UpdateScore(frog[pl],hopAmt);

			player[pl].canJump = 0;
			player[pl].hasJumped = 1;
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

//		frogFacing[pl] = nextFrogFacing[pl];
		camFacing = nextCamFacing;

		nextFrogFacing[pl] = frogFacing[pl] = (dir+camFacing) &3;
		player[pl].extendedHopDir = dir;
		SitAndFace(frog[pl],currTile[pl],frogFacing[pl]);

		if ( gameState.mode == CAMEO_MODE )
			gameState.mode = INGAME_MODE;

		// update player idleTime
		player[pl].idleTime = MAX_IDLE_TIME;
		idleCamera = 0;

		if( player[pl].isSuperHopping && !player[pl].hasDoubleJumped ) // && (player[pl].heightJumped > -125.0F)
		{
			GAMETILE *old;
			PlaySample(GEN_FROG_DOUBLEHOP,&frog[pl]->actor->pos,0,200,60);

			old = currTile[pl];
			currTile[pl] = destTile[pl];

			// player is superhopping - make frog double jump
			if (MoveToRequestedDestination( dir, pl ))
			{
				player[pl].hasDoubleJumped = 1;
				player[pl].canJump = 0;

				nextFrogFacing[pl] = frogFacing[pl] = (dir+camFacing) &3;
				player[pl].extendedHopDir = dir;
				SitAndFace(frog[pl],currTile[pl],frogFacing[pl]);
			}
			else
				currTile[pl] = old;

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
	}
	
	if((button[pl] & CONT_B) && !(lastbutton[pl] & CONT_B) && (tongue[pl].flags & TONGUE_IDLE))
    {
		// want to use tongue
		PlaySample(GEN_FROG_TONGUE,&frog[pl]->actor->pos,0,255,64);
		tongue[pl].flags = TONGUE_NONE | TONGUE_SEARCHING;
		player[pl].hasJumped = 1;

		// update player idleTime
		player[pl].idleTime = MAX_IDLE_TIME;
		idleCamera = 0;
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
		
		if (rPlaying)
		{
			PlayKeyDone();
			player[0].worldNum = WORLDID_FRONTEND;
			player[0].levelNum = LEVELID_FRONTEND1;
			lastActFrameCount = actFrameCount;
			gameState.mode = LEVELCOMPLETE_MODE;
			GTInit( &modeTimer, 1 );
			showEndLevelScreen = 0;
		}
		else
		{
			
			gameState.mode = PAUSE_MODE;
			pauseMode = 1;
		
	//		grabData.afterEffect = NO_EFFECT;
	//		EnableTextOverlay ( pauseTitle );
			sprintf(levelString,"%s",worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].description);
			sprintf(posString,"Pos %04li %04li %04li",(int)(frog[0]->actor->pos.v[0]/10),(int)(frog[0]->actor->pos.v[1]/10),(int)(frog[0]->actor->pos.v[2]/10));
			
			EnableTextOverlay ( continueText );
			EnableTextOverlay ( quitText );
			EnableTextOverlay ( posText );
			EnableTextOverlay ( levelnameText );

			if (NUM_FROGS == 1)
			{
				livesTextOver->oa = livesTextOver->a;
				scoreTextOver->oa = scoreTextOver->a;

				livesTextOver->a = 0;
				scoreTextOver->a = 0;
			}

			timeTextOver->oa = timeTextOver->a;
			timeTextOver->a = 0;

			if (NUM_FROGS == 1)
			{
				for ( i = 0; i < 3; i++ )
					sprHeart[i]->draw = 0;
			}

			for(i=0; i<numBabies; i++)
				babyIcons[i]->draw = 0;
		}
		lastbutton[pl] = button[pl];
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

SPRITEOVERLAY *atari,*konami,*flogo[10];


//////////////////////////////////////////
void Modify3DText(TEXT3D *t3d, char *str,unsigned char alpha);

TEXT3D *cText[4];
TEXT3D *rText[4];

unsigned char *credits[] = 
{
	"","Programmers",		"Matthew Cloy",		"",
	"","\x1",				"Andy Eder",		"",
	"","\x1",				"James Hubbard",	"",
	"","\x1",				"David Swift",		"",
	"","\x1",				"James Healey",		"",


	"","Artists",			"Alex Rigby","",
	"","\x1",				"Joff Scarcliffe","", 
	"","\x1",				"Bruce Millar","",
	"","\x1",				"Lauren Grindrod","",
	"","\x1",				"Simon Little","",
	"","\x1",				"Sandro Da Cruz","",
	"","\x1",				"Richard Whale","",
	"","\x1",				"Mark Turner","",
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
				else
					DeactivateCredits();
				
				RunCredits();
			}

	if (rPlaying)
	{
		demoText->oa = demoText->a = (sinf(actFrameCount*0.05)+1) * 128;
	}

	if	((player[0].worldNum == WORLDID_FRONTEND) &&
	     (player[0].levelNum == LEVELID_FRONTEND1) )
	{
		if ((frameCount > 15) && (gameState.mode != LEVELCOMPLETE_MODE) && (runAttractMode < actFrameCount))
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
			}
			else
			{
				atari->draw = 0;
				for (i=0; i<10; i++)
					flogo[i]->draw = 0;
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
		GTInit( &modeTimer, 15 );

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

		CheckForDynamicCameraChange(currTile[0]); // TEMPORARY FIX!!

		CameraLookAtFrog();
		UpdateCameraPosition(0);
	}

	UpdatePlatforms();
	UpdateEnemies();
	UpdateSpecialEffects();
	UpdateEvents();
	UpdateAmbientSounds();

	ProcessCharacters( );
	ProcessCollectables();

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
					frog[i]->actor->xluOverride = 50;
					
					GTUpdate( &player[i].safe, -1 );
					if( !player[i].safe.time )
						frog[i]->actor->xluOverride = 100;
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

	if( gameState.multi == SINGLEPLAYER )
		UpDateOnScreenInfo();
	else
	{
		int res;
		switch( player[0].worldNum )
		{
		case WORLDID_GARDEN:
			res = UpdateRace( );
			break;
		case WORLDID_SUBTERRANEAN:
		case WORLDID_HALLOWEEN:
			res = UpdateCTF( );
			break;
		}

		if( !res ) return;
	}

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
