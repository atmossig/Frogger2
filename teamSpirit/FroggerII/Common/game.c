/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: game.c
	Programmer	: Matthew Cloy
	Date		: 12/11/98

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

#define SHOW_ME_THE_TILE_NUMBERS

#include <ultra64.h>
#include "incs.h"


//----------------------------------------------------------------------

#ifdef SHOW_ME_THE_TILE_NUMBERS
TEXTOVERLAY *tileNum = NULL;
unsigned long currTileNum = 0;
#endif

//----------------------------------------------------------------------

struct gameStateStruct gameState;

unsigned short screenNum = 0;

GAMETILE *firstTile;
GAMETILE **gTStart;

float rZ = 0,rX = 0 ,rY = 0;
long hopAmt = 10;

unsigned long iceMoveDir[4] = {-1,-1,-1,-1};


float seed = 0.0F;
float upVal = 1;

unsigned long displayCount = 0;
long babySaved = 0;

unsigned long autoPlaying = 0;
unsigned long recordKeying = 0;

unsigned long num = 0;

unsigned long globalLong = 4;
unsigned long myVar = 4;
short showEndLevelScreen = 1;

ACTOR2 *demoTug = NULL;
OBJECT_CONTROLLER *waterObjectCont = NULL;
OBJECT *waterObject = NULL;
float demoTugSeed = 0.0F;
float demoTugAmp = 7.5F;
float demoTugInc = 0.075F;
long bby = 0;

long INPUT_PENALTY = 0;

VECTOR currCamDist = {0,0,10};

short spawnCounter = 0;

VECTOR debug_globalVector = { 0,1,0 };
float debug_globalFloat = 8;
VECTOR debug_globalEffectPos = { 0,0,0 };
PLANE2 debug_plane2;



/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: GameProcessController

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

void GameProcessController2(void)
{
	static u16 button,lastbutton;
	static s16 stickX, stickY,lastStickX,lastStickY;
    
	button = controllerdata[ActiveController+2].button;
	stickX = controllerdata[ActiveController+2].stick_x;
	stickY = controllerdata[ActiveController+2].stick_y;

	if((button & CONT_F) && !(lastbutton & CONT_F))
    {
		camFacing2--;
		camFacing2 &= 3;		
	}

	if((button & CONT_C) && !(lastbutton & CONT_C))
    {
		camFacing2++;
		camFacing2 &= 3;		
	}

	if((button & CONT_UP) && (autoHop?1:!(lastbutton & CONT_UP)) && (frogState2 & FROGSTATUS_ISSTANDING || player[0].frogState & FROGSTATUS_ISONMOVINGPLATFORM))
	{
		if(!playerInputPause2)
		{
			frogState2 |= FROGSTATUS_ISWANTINGU;
//			playerInputPause2 = INPUT_POLLPAUSE;
		}
		else
			playerInputPause2 += INPUT_PENALTY;
	}	    

	if((button & CONT_RIGHT) && (autoHop?1:!(lastbutton & CONT_RIGHT)) &&
	   (frogState2 & FROGSTATUS_ISSTANDING || player[0].frogState & FROGSTATUS_ISONMOVINGPLATFORM))
	{
		if(!playerInputPause2)
		{
			frogState2 |= FROGSTATUS_ISWANTINGR;
//			playerInputPause2 = INPUT_POLLPAUSE;
		}
		else
			playerInputPause2 += INPUT_PENALTY;
	}
    
	if((button & CONT_DOWN) && (autoHop?1:!(lastbutton & CONT_DOWN)) && (frogState2 & FROGSTATUS_ISSTANDING || player[0].frogState & FROGSTATUS_ISONMOVINGPLATFORM))
	{
		if(!playerInputPause2)
		{
			frogState2 |= FROGSTATUS_ISWANTINGD;
//			playerInputPause2 = INPUT_POLLPAUSE;
		}
		else
			playerInputPause2 += INPUT_PENALTY;
	}
    
	if((button & CONT_LEFT) && (autoHop?1:!(lastbutton & CONT_LEFT)) && (frogState2 & FROGSTATUS_ISSTANDING || player[0].frogState & FROGSTATUS_ISONMOVINGPLATFORM))
	{
		if(!playerInputPause2)
		{			
			frogState2 |= FROGSTATUS_ISWANTINGL;			
//			playerInputPause2 = INPUT_POLLPAUSE;
		}
		else
			playerInputPause2 += INPUT_PENALTY;
	}
  
	lastbutton = button;
	lastStickX = stickX;
	lastStickY = stickY;
}


void GameProcessController(long pl)
{
	static u16 button[4],lastbutton[4];
	static s16 stickX[4], stickY[4],lastStickX[4],lastStickY[4];
    
	VECTOR fwd = { 0,0,1 };
	VECTOR vec;

	button[pl] = controllerdata[pl].button;
	stickX[pl] = controllerdata[pl].stick_x;
	stickY[pl] = controllerdata[pl].stick_y;
		
	if ( autoPlaying )
	{
		LevelPlayingProcessController();
		button[pl] = 0;
		stickX[pl] = 0;
		stickY[pl] = 0;
		// find frame keys
		button[pl] = GetCurrentRecordKey();
	}
	else if ( ( gameState.mode == CAMEO_MODE ) && !( button && CONT_A ) )
	{
		CameoProcessController();
		button[pl] = 0;
		stickX[pl] = 0;
		stickY[pl] = 0;

		// Find any movement keys for the frog
		button[pl] = GetCurrentCameoKey();
	}
	// ENDELSEIF

	if((stickX[pl] > 15) && (player[pl].frogState & FROGSTATUS_ISSTANDING || player[pl].frogState & FROGSTATUS_ISONMOVINGPLATFORM))
	{
		if (lastStickX[pl] < 15)
		{
	    	player[pl].frogState |= FROGSTATUS_ISWANTINGR;
		}
	}
	else if((stickX[pl] < -15) && (player[pl].frogState & FROGSTATUS_ISSTANDING || player[pl].frogState & FROGSTATUS_ISONMOVINGPLATFORM))
	{
		if (lastStickX[pl] > -15)
		{
	    	player[pl].frogState |= FROGSTATUS_ISWANTINGL;
	    }
	}

	if ((stickY[pl] > 15) && (player[pl].frogState & FROGSTATUS_ISSTANDING || player[pl].frogState & FROGSTATUS_ISONMOVINGPLATFORM))
	{
		if (lastStickY[pl] < 15)
		{
		   	player[pl].frogState |= FROGSTATUS_ISWANTINGU;
	    }
	}
	else 
	if ((stickY[pl] < -15) && (player[pl].frogState & FROGSTATUS_ISSTANDING || player[pl].frogState & FROGSTATUS_ISONMOVINGPLATFORM))
	{
		if (lastStickY[pl] > -15)
		{
	    	player[pl].frogState |= FROGSTATUS_ISWANTINGD;
	    }
	}


	if((button[pl] & CONT_UP) && (autoHop?1:!(lastbutton[pl] & CONT_UP)) && (player[pl].frogState & FROGSTATUS_ISSTANDING || player[pl].frogState & FROGSTATUS_ISONMOVINGPLATFORM || player[pl].frogState & FROGSTATUS_ISFLOATING ))
	{
		if(!playerInputPause)
		{
			UpdateScore(frog[pl],hopAmt);

			player[pl].frogState |= FROGSTATUS_ISWANTINGU;

			if ( recordKeying )
				AddPlayingActionToList ( MOVEMENT_UP, frameCount );
			iceMoveDir[pl] = MOVE_UP;
//			if(!autoHop)
//				playerInputPause = INPUT_POLLPAUSE;
		}
		else
			if (playerInputPause>=INPUT_PENALTY)
				playerInputPause -= INPUT_PENALTY;
	}	    

	if((button[pl] & CONT_RIGHT) && (autoHop?1:!(lastbutton[pl] & CONT_RIGHT)) && (player[pl].frogState & FROGSTATUS_ISSTANDING || player[pl].frogState & FROGSTATUS_ISONMOVINGPLATFORM || player[pl].frogState & FROGSTATUS_ISFLOATING))
	{
		if(!playerInputPause)
		{
			UpdateScore(frog[pl],hopAmt);

			player[pl].frogState |= FROGSTATUS_ISWANTINGR;

			if ( recordKeying )
				AddPlayingActionToList ( MOVEMENT_RIGHT, frameCount );
			iceMoveDir[pl] = MOVE_RIGHT;
//			if(!autoHop)
//				playerInputPause = INPUT_POLLPAUSE;
		}
		else
			if (playerInputPause>=INPUT_PENALTY)
				playerInputPause -= INPUT_PENALTY;
	}
    
	if((button[pl] & CONT_DOWN) && (autoHop?1:!(lastbutton[pl] & CONT_DOWN)) && (player[pl].frogState & FROGSTATUS_ISSTANDING || player[pl].frogState & FROGSTATUS_ISONMOVINGPLATFORM || player[pl].frogState & FROGSTATUS_ISFLOATING))
	{
		if(!playerInputPause)
		{
			UpdateScore(frog[pl],hopAmt);
	    	
			player[pl].frogState |= FROGSTATUS_ISWANTINGD;

			if ( recordKeying )
				AddPlayingActionToList ( MOVEMENT_DOWN, frameCount );
			iceMoveDir[pl] = MOVE_DOWN;
//			if(!autoHop)
//				playerInputPause = INPUT_POLLPAUSE;
		}
		else
			if (playerInputPause>=INPUT_PENALTY)
				playerInputPause -= INPUT_PENALTY;
	}
    
	if((button[pl] & CONT_LEFT) && (autoHop?1:!(lastbutton[pl] & CONT_LEFT)) && (player[pl].frogState & FROGSTATUS_ISSTANDING || player[pl].frogState & FROGSTATUS_ISONMOVINGPLATFORM || player[pl].frogState & FROGSTATUS_ISFLOATING))
	{
		if(!playerInputPause)
		{
			UpdateScore(frog[pl],hopAmt);

			player[pl].frogState |= FROGSTATUS_ISWANTINGL;
			
			if ( recordKeying )
				AddPlayingActionToList ( MOVEMENT_LEFT, frameCount );
			iceMoveDir[pl] = MOVE_LEFT;
//			if(!autoHop)
//				playerInputPause = INPUT_POLLPAUSE;
		}
		else
			if (playerInputPause>=INPUT_PENALTY)
				playerInputPause -= INPUT_PENALTY;
	}


	if((button[pl] & CONT_A) && !(lastbutton[pl] & CONT_A))
    {
		if ( gameState.mode == CAMEO_MODE )
			gameState.mode = GAME_MODE;
		// ENDIF
		if(longHop)
		{
			isLong		= 1;
			longSpeed	= 30.0f;
			jumpAmt[pl]		= 0;
			landRadius	= 41.0f;
			speedTest	= 12.0f;
			switch(iceMoveDir[pl])
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
		else
		{
			if(!isJump[pl])
			{
				// frog is wanting superhop
				superHop = 5;

				isJump[pl] = 1;
				jumpSpeed[pl] = startJumpSpeed;
				jumpAmt[pl] = 0;

				switch(iceMoveDir[pl])
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


	
	if((button[pl] & CONT_B) && (player[pl].frogState & FROGSTATUS_ISSTANDING) && (tongueState & TONGUE_IDLE))
    {
		// want to use tongue
		tongueState	= TONGUE_NONE | TONGUE_SEARCHING;
	}

	if((button[pl] & CONT_E) && !(lastbutton[pl] & CONT_E))
    {
		// *** DEBUG - ANDYE ***
		ShowCamInfo();
		// *********************
	}

	if((button[0] & CONT_D) && !(lastbutton[0] & CONT_D))
    {
		// toggle between zoom in or out camera view
		ChangeCameraSetting();
	}

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

	// Croak and Croak Float
	if ( ( frog[pl]->action.deathBy != DEATHBY_FALLINGTOTILE ) && 
		 ( !(player[pl].frogState & FROGSTATUS_ISDEAD) ) && !frog[pl]->action.isCroaking )
		if ( ( button[pl] & CONT_L ) && !( lastbutton[pl] & CONT_L ) )
		{
			frog[pl]->action.isCroaking	= 15;
			player[pl].frogState |= FROGSTATUS_ISCROAKING;
			PlaySample(218,NULL,255,128);
		}

	// croak / croak-float
	if ( ( button[pl] & CONT_L ) && ( croakFloat ) )
    {
		if(croakFloat)
		{
			// following added by JamesH
			if ( frog[pl]->action.deathBy == DEATHBY_FALLINGTOTILE )
			{
				player[pl].frogState		    &= ~FROGSTATUS_ISDEAD;
				frog[pl]->action.dead	 = 0;
				player[pl].frogState		    |= FROGSTATUS_ISFLOATING;

				yFOVNew		= 80.0f;
				speedTest	= 2.0f;
			}
			// ENDIF
		}
		// ENDIF
    }
	else
	{
		if ( ( croakFloat ) && ( ( frog[pl]->action.deathBy == DEATHBY_FALLINGTOTILE ) ) &&
							 ( player[pl].frogState & FROGSTATUS_ISFLOATING ) )
		{
			player[pl].frogState		    &= ~FROGSTATUS_ISFLOATING;
			player[pl].frogState			|= FROGSTATUS_ISDEAD;
			frog[pl]->action.deathBy = DEATHBY_FALLINGTOTILE;
			frog[pl]->action.dead	 = 50;
			speedTest = 18.0f;
		}
		// ENDIF
	}
	// ENDELSEIF

	// End Croak And CroakFloat

	if((button[pl] & CONT_R) && !(lastbutton[pl] & CONT_R))
    {
		bby++;
		bby %= numBabies;
		SetFroggerStartPos(bTStart[bby],frog[pl],0);
    }

	if((button[pl] & CONT_START) && !(lastbutton[pl] & CONT_START))
	{
		//ScreenShot();

		gameState.mode = PAUSE_MODE;
		testPause = 1;
		EnableTextOverlay ( pauseTitle );
		EnableTextOverlay ( continueText );
		EnableTextOverlay ( quitText );
		lastbutton[pl] = button[pl];
		
		if(backPanel)
		{
			backPanel->xPos		= 50;
			backPanel->yPos		= 60;
			backPanel->width	= 220;
			backPanel->height	= 95;
			backPanel->r		= 15;
			backPanel->g		= 63;
			backPanel->b		= 255;
			backPanel->a		= 127;
			backPanel->draw		= 1;
		}

//		return;	
    }
  
	lastbutton[pl] = button[pl];
	lastStickX[pl] = stickX[pl];
	lastStickY[pl] = stickY[pl];
}

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: InitCamera

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

void InitCamera ( unsigned long worldID, unsigned long levelID )
{
	int i;
	for (i=0; i<4; i++)
	{
		switch ( worldID )
		{
			case WORLDID_GARDEN:
					switch ( levelID )
					{
						case LEVELID_GARDENLAWN:
								camSource[i].v[X]	= 0;
								camSource[i].v[Y]	= 300;
								camSource[i].v[Z]	= 1165;
								camTarget[i].v[X]	= 0;
								camTarget[i].v[Y]	= 0;
								camTarget[i].v[Z]	= 900;
								frogFacing[0] = ( camFacing+2 ) & 3;
							break;
					}
					// ENDSWITCH
				break;

		}
		// ENDSWITCH
		SetVector(&currCamSource[i],&camSource[i]);
		SetVector(&currCamTarget[i],&camTarget[i]);
		//SetVector(&currCamDist,&camDist);
	}
}

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: CameraLookAtFrog

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

extern long multiplayerRun;
void CameraLookAtFrog(void)
{
	if(frog[0] && !controlCamera)
	{
	
		float afx,afy,afz;
		int i;
		afx = afy = afz = 0;

		for (i=0; i<NUM_FROGS; i++)
		{
			afx += frog[i]->actor->pos.v[0];
			afy += frog[i]->actor->pos.v[1];
			afz += frog[i]->actor->pos.v[2];
		}
		
		afx/=NUM_FROGS;
		afy/=NUM_FROGS;
		afz/=NUM_FROGS;

		{
			camTarget[0].v[0] = afx+currTile[0]->dirVector[camFacing].v[0]*camLookOfs + currTile[0]->normal.v[0]*upVal;	
			camTarget[0].v[1] = afy+currTile[0]->dirVector[camFacing].v[1]*camLookOfs + currTile[0]->normal.v[1]*upVal;	
			camTarget[0].v[2] = afz+currTile[0]->dirVector[camFacing].v[2]*camLookOfs + currTile[0]->normal.v[2]*upVal;
		}
		
	}
	
}


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: SlurpCamPosition

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

float fovSpd = 2;
float camSpeed4 = 8;

void SlurpCamPosition(long cam)
{
	currCamSource[cam].v[0] -= (currCamSource[cam].v[0] - camSource[cam].v[0])/camSpeed;
	currCamSource[cam].v[1] -= (currCamSource[cam].v[1] - camSource[cam].v[1])/camSpeed;
	currCamSource[cam].v[2] -= (currCamSource[cam].v[2] - camSource[cam].v[2])/camSpeed;

	currCamTarget[cam].v[0] -= (currCamTarget[cam].v[0] - camTarget[cam].v[0])/camSpeed3;
	currCamTarget[cam].v[1] -= (currCamTarget[cam].v[1] - camTarget[cam].v[1])/camSpeed3;
	currCamTarget[cam].v[2] -= (currCamTarget[cam].v[2] - camTarget[cam].v[2])/camSpeed3;

	currCamDist.v[0] -= (currCamDist.v[0] - camDist.v[0]*scaleV)/camSpeed4;
	currCamDist.v[1] -= (currCamDist.v[1] - camDist.v[1]*scaleV)/camSpeed4;
	currCamDist.v[2] -= (currCamDist.v[2] - camDist.v[2]*scaleV)/camSpeed4;

	if ( gameState.mode != CAMEO_MODE )
	{
		camVect.v[0] -= (camVect.v[0] - currTile[0]->normal.v[0])/camSpeed2;
		camVect.v[1] -= (camVect.v[1] - currTile[0]->normal.v[1])/camSpeed2;
		camVect.v[2] -= (camVect.v[2] - currTile[0]->normal.v[2])/camSpeed2;
//		camVect.v[0] = 0;
//		camVect.v[1] = 1;
//		camVect.v[2] = 0;
	}
	// ENDIF

	xFOV		-= (xFOV - xFOVNew) / (camSpeed*fovSpd);
	yFOV		-= (yFOV - yFOVNew) / (camSpeed*fovSpd);
	camLookOfs	-= (camLookOfs - camLookOfsNew) / camSpeed;

	if(cameraShake)
	{
		currCamSource[cam].v[0] += (-16 + Random(32));
		currCamSource[cam].v[1] += (-16 + Random(32));
		currCamSource[cam].v[2] += (-16 + Random(32));
		cameraShake--;
	}
	else
	{
//		osMotorStop ( &rumble );
	}
}


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: UpdateCameraPosition

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/
void UpdateCameraPosition(long cam)
{
	VECTOR result;

	if(!frog[0] || !currTile[0] || controlCamera)
		return;
	
	if ( gameState.mode != CAMEO_MODE )
	{
		float afx,afy,afz;
		int i;
		afx = afy = afz = 0;

		for (i=0; i<NUM_FROGS; i++)
		{
			afx += frog[i]->actor->pos.v[0];
			afy += frog[i]->actor->pos.v[1];
			afz += frog[i]->actor->pos.v[2];
		}
		
		afx/=NUM_FROGS;
		afy/=NUM_FROGS;
		afz/=NUM_FROGS;

		camSource[cam].v[0] = afx+((currTile[0]->normal.v[0]*currCamDist.v[1]-currTile[0]->dirVector[camFacing].v[0]*currCamDist.v[2]));
		camSource[cam].v[1] = afy+((currTile[0]->normal.v[1]*currCamDist.v[1]-currTile[0]->dirVector[camFacing].v[1]*currCamDist.v[2]));
		camSource[cam].v[2] = afz+((currTile[0]->normal.v[2]*currCamDist.v[1]-currTile[0]->dirVector[camFacing].v[2]*currCamDist.v[2]));
	}
	// ENDIF

	SlurpCamPosition(cam);
}

/*	--------------------------------------------------------------------------------
	Function		: CreateLevelObjects
	Purpose			: creates the objects for the world / level
	Parameters		: 
	Returns			: void
	Info			: 
*/
void CreateLevelObjects(unsigned long worldID,unsigned long levelID)
{
	unsigned long flags = 0;
//	static int firstObject = 1;
		
	ACTOR2 *theActor;
	SCENIC *ts = Sc_000;
	int actCount = 0;
	OBJECT_CONTROLLER *objCont;
	
//	QUATERNION t1 = { 0,0,1,((PI)/2) };
	QUATERNION t2,t3;
	
	// get water sections for the relevant world levels and set the teleporters - ANDYE
	if(worldID == WORLDID_GARDEN)
	{
		if(levelID == LEVELID_GARDENLAWN)
		{
			//MakeTeleportTile(&firstTile[33],&firstTile[420],TELEPORT_TWOWAY);
			
			// teleport to secret section (and back)
			MakeTeleportTile(&firstTile[389],&firstTile[292],TELEPORT_ONEWAY);
			MakeTeleportTile(&firstTile[573],&firstTile[388],TELEPORT_ONEWAY);
		}
	}

	// Go through and add them!
	while (ts)
	{
		float tv;

		theActor = CreateAndAddActor (ts->name,ts->pos.v[0],ts->pos.v[2],ts->pos.v[1],INIT_ANIMATION,0,0);
		dprintf"Added actor '%s'\n",ts->name));

		if ( gstrcmp ( ts->name, "world.ndo" ) == 0 )
			flags |= ACTOR_DRAW_ALWAYS;
		
		theActor->flags = flags;

		// ----- FOR DEMO PURPOSES - ANDYE -----
		if(gstrcmp(ts->name,"tug.ndo") == 0)
			demoTug = theActor;
		// ----- FOR DEMO PURPOSES - ANDYE -----

		tv = ts->rot.y;
		ts->rot.y = ts->rot.z;
		ts->rot.z = tv;

//		GetQuaternionFromRotation (&t2,&t1);
		GetQuaternionFromRotation (&theActor->actor->qRot,&ts->rot);
//		GetQuaternionFromRotation (&t3,&ts->rot);

//		QuaternionMultiply (&t->actor->qRot,&t2,&t3);

		ts = ts->next;

		actCount++;
	}

//	firstObject = 1;

	dprintf"\n\n** ADDED %d ACTORS **\n\n",actCount));	

}

void Orientate(QUATERNION *me, VECTOR *fd, VECTOR *mfd, VECTOR *up)
{
	VECTOR dirn,dirn2;
	QUATERNION rotn,q;
	float frogMatrix[4][4];
	float frogMatrix2[4][4];
	float dp,m;
	
	CalculateQuatForPlane2(0,me,up);
	SetVector (&dirn,mfd);
	RotateVectorByQuaternion(&dirn2,&dirn,me);
	dp = DotProduct(fd,&dirn2);
	CrossProduct((VECTOR *)&rotn,&dirn2,fd);
	if(dp > -0.99)
	{
		m = Magnitude((VECTOR *)&rotn);
		if(m > 0.0001)
		{
			ScaleVector((VECTOR *)&rotn,1/m);
			rotn.w = acos(dp);
			GetQuaternionFromRotation(&q,&rotn);
			QuaternionMultiply(me,&q,me);
		}
	}
	else
	{
		vertQ.w = PI;
		GetQuaternionFromRotation(&q,&vertQ);
		QuaternionMultiply(me,me,&q);
	}
}

void SitAndFace(ACTOR2 *me, GAMETILE *tile, long fFacing)
{
	VECTOR fwdVec = { 0,0,1 };
	VECTOR dirn2;
	QUATERNION rotn,q;
	float frogMatrix[4][4];
	float frogMatrix2[4][4];
	float dp,m;
	
	CalculateQuatForPlane2(0,&me->actor->qRot,&tile->normal);
	RotateVectorByQuaternion(&dirn2,&fwdVec,&me->actor->qRot);
	dp = DotProduct(&tile->dirVector[fFacing],&dirn2);
	CrossProduct((VECTOR *)&rotn,&dirn2,&tile->dirVector[fFacing]);
	if(dp > -0.99)
	{
		m = Magnitude((VECTOR *)&rotn);
		if(m > 0.0001)
		{
			ScaleVector((VECTOR *)&rotn,1/m);
			rotn.w = acos(dp);
			GetQuaternionFromRotation(&q,&rotn);
			QuaternionMultiply(&me->actor->qRot,&q,&me->actor->qRot);
		}
	}
	else
	{
		vertQ.w = PI;
		GetQuaternionFromRotation(&q,&vertQ);
		QuaternionMultiply(&me->actor->qRot,&me->actor->qRot,&q);
	}
}

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: RunGameLoop 

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

float sinkScale = 5;
//extern long move;
//extern long playMode;
//extern long multiplayerRun;
long move;
long playMode = NORMAL_PMODE;
long multiplayerRun;

char tmpBuffer[16];

long carryOnBabies = 1;
long clearTiles = 0;


void RunGameLoop (void)
{	    	
	VECTOR fvec = { 0,0,1 };
	VECTOR v1;
	QUATERNION q;
	unsigned long i,j;
	GAMETILE *cur;
	FX_SMOKE *smoke;

	ACTOR2 *bee;

	// Ambient Sound test
	VECTOR ambpos = { -75,0,-375 };
	static int ambrad = 10;

	if(frameCount == 1)
	{	
		//StopDrawing("game");
		
		if (multiplayerRun)
			playMode = 1;

		currCamSetting = 1;
		ChangeCameraSetting();

		player[0].lives				= 3;
		player[0].score				= 0;
		player[0].timeSec			= (90*30);
		player[0].spawnTimer		= 0;
		player[0].spawnScoreLevel	= 1;

		gameIsOver = 0;
		levelIsOver = 0;

//		LoadTextureBank ( SYSTEM_TEX_BANK );
		//LoadObjectBank  ( SYSTEM_OBJ_BANK );

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

		backPanel = CreateAndAddSpriteOverlay(50,83,"backpanel.bmp",32,32,255,255,255,255,0);
		backPanel->width	= 220;
		backPanel->height	= 50;
		backPanel->r		= 15;
		backPanel->g		= 63;
		backPanel->b		= 255;
		backPanel->a		= 0;


//		sp = AddNewSpriteToList(firstTile[],firstTile[],firstTile[],0,"wholekey.bmp",0);

#ifdef SHOW_ME_THE_TILE_NUMBERS
		tileNum = CreateAndAddTextOverlay(0,35,"TILENUM",YES,NO,255,255,255,255,oldeFont,0,0,0);
#endif

		CreateAndAddTextOverlay(0,218,"milestone 5",YES,NO,255,255,255,91,smallFont,0,0,0);

		//firstTile[137].state = TILESTATE_SPRING;
		//firstTile[261].state = TILESTATE_SPRING;
		//firstTile[301].state = TILESTATE_SPRING;

		if (clearTiles)
		{
			cur = firstTile;
			while ( cur )
			{
				cur->state = TILESTATE_NORMAL;
				cur = cur->next;
			}
		}

//		runningWaterStuff = 0;
		ChangeCameraSetting();

		//AddAmbientSfxAtPoint( FX_CHICKEN_BELCH, 150, &ambpos, 50, 50, 10, 0, 0, 0, ambrad );

	}

	if(!goText->kill)
	{
		if(frameCount > 50)
		{
			goText->yPos += 4;
			goText->a -= 7;
			
			
			if(goText->yPos > 239)
				goText->kill = 1;

			if (backPanel)
			{
				backPanel->yPos -= 4;
				backPanel->a -= 4;

				if((backPanel->yPos + backPanel->height) < 0)
				{
					backPanel->draw = 0;
					backPanel->a = 127;
					backPanel->yPos = 83;
				}
			}
		}
		else
			if (backPanel)
				backPanel->a += 3;
	}
		 
	if(keyFound)
		RunLevelKeyFound();

	if(babySaved && !gameIsOver && !levelIsOver)
		RunBabySavedSequence(lastBabySaved);
	
	if(player[0].frogState & FROGSTATUS_ISDEAD)
	{
		if(gameIsOver)
		{
			DisableTextOverlay(livesTextOver);
			DisableTextOverlay(timeTextOver);
			DisableTextOverlay(scoreTextOver);
//				livesIcon->active = 0;

			RunGameOverSequence();

			gameIsOver--;
			if(!gameIsOver)
			{
				StopDrawing ( "game over" );
				FreeAllLists();
				gameState.mode = FRONTEND_MODE;
				frontEndState.mode = TITLE_MODE;
				frameCount = 0;
				StartDrawing ( "game over" );
				return;
			}

			seed += 0.5F;
		}
		else
		{
			//osMotorStop ( &rumble );
		}
		 
		camDist.v[Y] -= (camDist.v[Y] - 100) / 10.0F;
		camDist.v[Z] -= (camDist.v[Z] - 125) / 10.0F;

		//UpdateCameraPosition(0);
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
					testPause = 1;
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
				StopDrawing ( "EndGame" );
				FreeAllLists();

				frog[0] = NULL;
				frameCount = 0;
				// Only go to next level if in normal level progression.
				if( showEndLevelScreen )
					player[0].levelNum++;

				player[0].numSpawn	= 0;
				player[0].timeSec	= 90;

				spawnCounter = 0;

				if ( player[0].levelNum == 4 )
				{
					frontEndState.mode	= TITLE_MODE;
					gameState.mode		= FRONTEND_MODE;
				}

				worldVisualData [ player[0].worldNum ].levelVisualData [ player[0].levelNum ].levelOpen |= LEVEL_OPEN;

				InitLevel ( player[0].worldNum, player[0].levelNum );
				
				showEndLevelScreen = 1; // Normal level progression is default

				StartDrawing ( "EndGame" );
				return;
			}
				
		}
		else
		{
			if ( babiesSaved == numBabies )
			{
				camDist.v[X]	= 0;
				camDist.v[Y]	= 680;
				camDist.v[Z]	= 192;

				levelIsOver		= 400;	

				
/*					for ( i = 0; i < 3; i++ )
				{
					if ( 90 - ( player[0].timeSec/30 ) < worldHiScoreData[player[0].worldNum].levelScoreData[ player[0].levelNum ].scoreData[i].time )
					{
						EnableTextOverlay ( newLevelScore );

					}
					// ENDIF
				}
				// ENDFOR  */

			}
			else
			{
				if ( ( frameCount > 15 ) )
				{
					if( triggerList.numEntries )
						UpdateEvents( );

					if(!frog[0]->action.dead)	
					{
						for (i=0; i<NUM_FROGS; i++)
                        if (frog[i]->action.stun)
							frog[i]->action.stun--;
                        else
							GameProcessController(i);                                      
        			}

					if (!frog2->action.dead)
					{
						GameProcessController2();
					}
					// ENDIF
				}
				// ENDIF  
				if(frog[0])
				{
					for (i=0; i<numBabies; i++)
					{
				//		InitActorAnim(babies[i]->actor);
						AnimateActor(babies[i]->actor,0,YES,NO,1.0);
					}
					if ( gameState.mode != CAMEO_MODE )
					{
						CheckForDynamicCameraChange(currTile[0]);
						CameraLookAtFrog();
					}
					// ENDIF
					UpdateCameraPosition(0);
					for (i=0; i<4; i++)
					{
					
						if ( !( player[i].frogState & FROGSTATUS_ISFLOATING ) )
							SitAndFace(frog[i],currTile[i],frogFacing[i]);
						else if ( player[i].frogState & FROGSTATUS_ISFLOATING )
						{
							//SitAndFace(frog,currTile,frogFacing);
							//RotateFrog ( frog, frogFacing );
							//switch ( turnDir )
						}
					}
					
					// ENDELSEIF
					SitAndFace(frog2,currTile2,frogFacing2);

				}	  
			}
			// ENDIF 
		}
		// endif

	}
	// ENDELSEIF

	//***** ANDYE *****//
	UpdatePlatforms();
	UpdateEnemies();
	UpdateSpecialFX();
	//UpdateAmbientSounds();
	//	UpdateWater();
	
	//*****************//

	ProcessCollectables();

	for (i=0; i<NUM_FROGS; i++)
	{
		UpdateFroggerPos(i);
		if (frog[i]->action.frogon!=-1)
		{
		  	if (player[i].frogState & FROGSTATUS_ISSTANDING)
			{
				frog[i]->actor->pos.v[X] = frog[frog[i]->action.frogon]->actor->pos.v[X]+sinf(frameCount/30.0)*5;
				frog[i]->actor->pos.v[Y] = frog[frog[i]->action.frogon]->actor->pos.v[Y]+35;
				frog[i]->actor->pos.v[Z] = frog[frog[i]->action.frogon]->actor->pos.v[Z]+cosf(frameCount/27.0)*5;
			}
		}

		if (!IsPointVisible(&frog[i]->actor->pos))
		{
			int j;
			for (j=0; j<NUM_FROGS; j++)
			{
				if (IsPointVisible(&frog[j]->actor->pos))
				{
					TeleportActorToTile(frog[i],currTile[j],i);
					destTile[i] = currTile[j];
					frog[i]->action.stun = 50;
					frog[i]->action.safe = 80;
				}
			}
		}
	}
	
	UpdateFroggerPos2();

	UpDateOnScreenInfo();

	if(player[0].frogState & FROGSTATUS_ISSTANDING)
	{
		if((currTile[0]->state == TILESTATE_DEADLY) && !(player[0].frogState & FROGSTATUS_ISONMOVINGPLATFORM))
		{
			if(!frog[0]->action.dead)
			{
				CreateAndAddFXRipple(RIPPLE_TYPE_WATER,&currTile[0]->centre,&upVec,25,1,0.1,30);

				frog[0]->action.deathBy = DEATHBY_DROWNING;
				player[0].frogState |= FROGSTATUS_ISDEAD;
				frog[0]->action.dead = 75;
				PlaySample ( 2,NULL,255,128 );
			}
		}
	} 

// ----- FOR DEMO PURPOSES - ANDYE -----
	if(actList && demoTug)
	{
		demoTug->actor->pos.v[Y] = (demoTug->actor->oldpos.v[Y] - 20.0F) + (sinf(demoTugSeed) * demoTugAmp);
		demoTug->actor->pos.v[Z] = (demoTug->actor->oldpos.v[Z]) + (sinf(demoTugSeed + PI_OVER_2) * (demoTugAmp));

		demoTug->actor->qRot.z = (sinf(demoTugSeed) * 0.05F);
		
		demoTugSeed += demoTugInc;
		if ( frameCount % 100 == 0 )
			PlaySample ( 3,NULL,128,128 );
	} 

	for (i=0; i<NUM_FROGS; i++)
	if (frog[i])
	{
		if (frog[i]->actor)
		{
			frog[i]->actor->xluOverride=100;
	
			if (!multiplayerRun)
				frog2->actor->xluOverride=0;
			else
				frog2->actor->xluOverride=100;

		} 
		if (frog[i]->action.safe) 
		{
			if ((frameCount % 2)==0)
				frog[i]->actor->xluOverride=0;
			frog[i]->action.safe--;
		}  
		// play some ambient effects
		if(Random(50) > 48)
		{
			PlaySample(Random(3)+226,NULL,92,128);
		} 
	}  


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
		sprintf(tileNum->text,"%d",currTileNum);
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RunLevelCompleteSequence()
{
	long award = 2;
	long i;
	extern long numHops_TOTAL;
	extern long speedHops_TOTAL;
	extern long numHealth_TOTAL;

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

	for ( i = 0; i < 3; i++ )
		sprHeart[i]->draw = 0;
	// ENDFOR

	testA = 220;

	scoreTextOver->xPos -= ((float)scoreTextOver->xPos - (100.0F)) / 15.0F;
	scoreTextOver->yPos -= ((float)scoreTextOver->yPos - (110.0F)) / 16.0F;
	
	if ( spawnCounter != player[0].numSpawn )
	{
		spawnCounter++;
	}
	// ENDIF

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
