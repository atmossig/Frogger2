/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: frogmove.c
	Programmer	: Andrew Eder
	Date		: 11/13/98

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI
//#include "oddballs.h"

#include <ultra64.h>

#include "incs.h"


long   numHops_TOTAL = 0;
long   speedHops_TOTAL = 0;
long   numHealth_TOTAL = 0;

//GAMETILE *worldTilesList	= NULL;
ACTOR2 *temp;

GAMETILE *destTile[4]		= {0,0,0,0};
GAMETILE *longHopDestTile	= NULL;
GAMETILE *currTile[4]		= {0,0,0,0};
GAMETILE *prevTile			= NULL;

GAMETILE *destTile2			= NULL;
GAMETILE *currTile2			= NULL;


//unsigned long frogState		= FROGSTATUS_ISSTANDING;
unsigned long frogState2	= FROGSTATUS_ISSTANDING;

unsigned long d = 2;

unsigned long iceTileX;
unsigned long iceTileY;
unsigned long iceTileZ;
unsigned long wasSuperHop = 0;

float landRadius		= 20.0F;
static float frogAnimSpeed	= 3.5F;
float frogSlurpDivisor		= 2.0F;

int	frogFacing				= 0;
int	frogFacing2				= 0;

float globalFloat			= 0.1F;

/**/
long isJump = 0;
float jumpSpeed = 0;
float jumpAmt = 0;

float changeJumpSpeed = -3;
float bounceJumpSpeed = 0;
float finalJumpSpeed = 0.1;
float startJumpSpeed = 13.0F;

long  isLong	= 0;
float longSpeed = 0;
float longAmt	= 0;

float changeLongSpeed	= -10;
float startLongSpeed	= 25.0f;

long move =20;
long moveVal = 20;

/*	--------------------------------------------------------------------------------
	Function		: SetFroggerStartPos(_GAMETILE *startTile)
	Purpose			: sets initial position of Frogger on the world
	Parameters		: start tile,actor2
	Returns			: none
	Info			:
*/
void SetFroggerStartPos(GAMETILE *startTile,ACTOR2 *act, long p)
{
	unsigned long i;

	// Change frog's position to reflect that of the start tile
	act->actor->pos.v[X] = startTile->centre.v[X];
	act->actor->pos.v[Y] = startTile->centre.v[Y];
	act->actor->pos.v[Z] = startTile->centre.v[Z];


	// Reset the powerup variables
	autoHop			= 0;
	superHop		= 0;
	longTongue		= 0;

	camFacing		= 0;
	camFacing2		= 0;
	currCamSetting	= 0;

	isJump			= 0;
	jumpSpeed		= 0;
	jumpAmt			= 0;

	finalJumpSpeed	= 0.1F;
	startJumpSpeed	= 13.0F;
	changeJumpSpeed	= -3.0F;
	bounceJumpSpeed	= 0.0F;
	
	isLong		= 0;
	longSpeed	= 0;
	longAmt		= 0;

	changeLongSpeed	= -10;
	startLongSpeed	= 25.0f;

	ChangeCameraSetting();

	InitActorAnim(act->actor);
	AnimateActor(act->actor,3,YES,NO,1.0F);

	player[p].frogState	= FROGSTATUS_ISSTANDING;
	frogState2	= FROGSTATUS_ISSTANDING;

	currTile[p]		= startTile;
	currTile2		= startTile;

	destTile[p]		= NULL;
	currPlatform	= NULL;
	destPlatform	= NULL;

	act->action.lives		= 3;
	act->action.isCroaking	= 0;
	act->action.isOnFire	= 0;

	for (i=0; i<numBabies; i++)
	{
		// ***** ANDYE *********************************

		if(!babies[i]->action.isSaved)
		{
			babies[i]->actor->pos.v[X] = bTStart[i]->centre.v[X];
			babies[i]->actor->pos.v[Y] = bTStart[i]->centre.v[Y];
			babies[i]->actor->pos.v[Z] = bTStart[i]->centre.v[Z];
			NormalToQuaternion(&babies[i]->actor->qRot,&bTStart[i]->normal);

			InitActorAnim(babies[i]->actor);
			AnimateActor(babies[i]->actor,0,YES,NO,1.0F);
		}
		
		// *********************************************
	}
}

/*	--------------------------------------------------------------------------------
	Function		: UpdateFroggerPos()
	Purpose			: updates Frogger's position on game world
	Parameters		: none
	Returns			: none
	Info			:
*/
void UpdateFroggerPos(long pl)
{
	float x,y,z;
	FX_RIPPLE *rip;
	VECTOR effectPos;

	if(player[pl].frogState & FROGSTATUS_ISDEAD)
	{
		KillFrog(frog[pl],pl);
		return;
	}

	if(player[pl].frogState & FROGSTATUS_ISTELEPORTING)
	{
		// frog is in state of teleportation
		frog[pl]->action.isTeleporting--;
		if(!frog[pl]->action.isTeleporting)
		{
			TeleportActorToTile(frog[pl],currTile[pl]->teleportTo,pl);
			fadeDir		= FADE_IN;
			fadeOut		= 255;
			fadeStep	= 8;
			doScreenFade = 63;
		}
		
		return;
	}

	
	// update frog tongue
	UpdateFrogTongue();

	if(player[pl].frogState & FROGSTATUS_ISFALLINGTODEATH)
	{
	}
	if(player[pl].frogState & FROGSTATUS_ISSTANDING)
	{
	}
	if(player[pl].frogState & FROGSTATUS_ISONMOVINGPLATFORM)
	{
	}

	if(player[pl].frogState & FROGSTATUS_ISFLOATING)
	{
		if ( ( destPlatform ) && ( destPlatform->inTile != currTile[pl] ) )
		{
			player[pl].frogState |= FROGSTATUS_ISJUMPINGTOTILE;
			player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOPLATFORM;
		}
		// ENDIF
	}
	// ENDIF

	// frog is croaking
	if(player[pl].frogState & FROGSTATUS_ISCROAKING)
	{
		if((frog[pl]->action.isCroaking & 3) == 0)
		{
			SetVector(&effectPos,&frog[pl]->actor->pos);
			effectPos.v[Y] += 15;
			rip = CreateAndAddFXRipple(RIPPLE_TYPE_CROAK,&effectPos,&upVec,15,2,1,15);
			rip->r = 191;
			rip->g = 255;
			rip->b = 0;
		}

		frog[pl]->action.isCroaking--;
		if(!frog[pl]->action.isCroaking)
		{
			player[pl].frogState &= ~FROGSTATUS_ISCROAKING;

			// check for nearest baby frog - do radius check ????
			if(nearestBaby = GetNearestBabyFrog())
			{
				rip = CreateAndAddFXRipple(RIPPLE_TYPE_CROAK,&nearestBaby->actor->pos,&upVec,15,2,3,20);
				rip->r = 191;
				rip->g = 255;
				rip->b = 0;
			}
		}
	}

	if(player[pl].frogState & FROGSTATUS_ISWANTINGU)
	{
		if (player[pl].frogState & FROGSTATUS_ISFLOATING )
			prevTile = currTile[pl];
		// requests up
		if(!MoveToRequestedDestination(MOVE_UP,pl))
		{
			if ( !( player[pl].frogState & FROGSTATUS_ISFLOATING ) )
			{
				frog[pl]->actor->animation->animTime = 0;
				AnimateActor(frog[pl]->actor,0,NO,NO,frogAnimSpeed);
				AnimateActor(frog[pl]->actor,3,YES,YES,1.0F);

				if ( currPlatform )
					player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
				else
					player[pl].frogState |= FROGSTATUS_ISSTANDING;			
			}
			else
			{
				destTile[pl] = prevTile;
			}
			// ENDELSEIF
			
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGU;
		}
		move = moveVal;
		frogFacing = camFacing;
		PlaySample ( 24,NULL,255,128);
	}
	else if(player[pl].frogState & FROGSTATUS_ISWANTINGD)
	{
		if (player[pl].frogState & FROGSTATUS_ISFLOATING )
			prevTile = currTile[pl];
		// requests up
		if(!MoveToRequestedDestination(MOVE_DOWN,pl))
		{
			if ( !(player[pl].frogState & FROGSTATUS_ISFLOATING )	)
			{
				frog[pl]->actor->animation->animTime = 0;
				AnimateActor(frog[pl]->actor,0,NO,NO,frogAnimSpeed);
				AnimateActor(frog[pl]->actor,3,YES,YES,1.0F);

				if ( currPlatform )
					player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
				else
					player[pl].frogState |= FROGSTATUS_ISSTANDING;			
			}
			else
			{
				destTile[pl] = prevTile;
			}
			// ENDELSEIF
			
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGD;
		}
		move = moveVal;
		PlaySample ( 24,NULL,255,128);
		frogFacing = (camFacing + 2) & 3;
	}
	else if(player[pl].frogState & FROGSTATUS_ISWANTINGL)
	{
		if (player[pl].frogState & FROGSTATUS_ISFLOATING )
			prevTile = currTile[pl];
		// requests up
		if(!MoveToRequestedDestination(MOVE_LEFT,pl))
		{
			if ( !(player[pl].frogState & FROGSTATUS_ISFLOATING )	)
			{
				frog[pl]->actor->animation->animTime = 0;
				AnimateActor(frog[pl]->actor,0,NO,NO,frogAnimSpeed);
				AnimateActor(frog[pl]->actor,3,YES,YES,1.0F);

				if ( currPlatform )
					player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
				else
					player[pl].frogState |= FROGSTATUS_ISSTANDING;			
			}
			else
			{
				destTile[pl] = prevTile;
			}
			// ENDELSEIF
			
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGL;
		}
		move = moveVal;
		frogFacing = (camFacing + 1) & 3;		
		PlaySample ( 24,NULL,255,128);
	}
	else if(player[pl].frogState & FROGSTATUS_ISWANTINGR)
	{
		if (player[pl].frogState & FROGSTATUS_ISFLOATING )
			prevTile = currTile[pl];
		// requests up
		if(!MoveToRequestedDestination(MOVE_RIGHT,pl))
		{
			if ( !(player[pl].frogState & FROGSTATUS_ISFLOATING )	)
			{
				frog[pl]->actor->animation->animTime = 0;
				AnimateActor(frog[pl]->actor,0,NO,NO,frogAnimSpeed);
				AnimateActor(frog[pl]->actor,3,YES,YES,1.0F);

				if ( currPlatform )
					player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
				else
					player[pl].frogState |= FROGSTATUS_ISSTANDING;			
			}
			else
			{
				destTile[pl] = prevTile;
			}
			// ENDELSEIF
			
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGR;
		}
		move = moveVal;
		frogFacing = (camFacing + 3) & 3;		
		PlaySample ( 24,NULL,255,128);
	}

	if(player[pl].frogState & FROGSTATUS_ISWANTINGLONGHOPU)
	{
		frogFacing = camFacing;
		player[pl].frogState |= FROGSTATUS_ISLONGHOPPING;
		// requests long hop up
		if(!MoveToRequestedDestination(MOVE_UP,pl))
		{
			frog[pl]->actor->animation->animTime = 0;
			AnimateActor(frog[pl]->actor,0,NO,NO,frogAnimSpeed);
			AnimateActor(frog[pl]->actor,3,YES,YES,1.0F);
			
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGLONGHOPU;

			if ( currPlatform )
				player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
			else
				player[pl].frogState |= FROGSTATUS_ISSTANDING;			

		}
		move = moveVal;
		//frogFacing = camFacing;
		PlaySample ( 24,NULL,255,128);

		CreateAndAddFXRipple(RIPPLE_TYPE_CROAK,&frog[pl]->actor->pos,&upVec,15,1,0.3,15);
	}
	else if(player[pl].frogState & FROGSTATUS_ISWANTINGLONGHOPD)
	{
		frogFacing = (camFacing + 2) & 3;
		player[pl].frogState |= FROGSTATUS_ISLONGHOPPING;
		// requests long hop up
		if(!MoveToRequestedDestination(MOVE_DOWN,pl))
		{
			frog[pl]->actor->animation->animTime = 0;
			AnimateActor(frog[pl]->actor,0,NO,NO,frogAnimSpeed);
			AnimateActor(frog[pl]->actor,3,YES,YES,1.0F);
			
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGLONGHOPD;

			if ( currPlatform )
				player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
			else
				player[pl].frogState |= FROGSTATUS_ISSTANDING;			

		}
		move = moveVal;
		//frogFacing = camFacing;
		PlaySample ( 24,NULL,255,128);

		CreateAndAddFXRipple(RIPPLE_TYPE_CROAK,&frog[pl]->actor->pos,&upVec,15,1,0.3,15);
	}
	else if(player[pl].frogState & FROGSTATUS_ISWANTINGLONGHOPL)
	{
		frogFacing = (camFacing + 1) & 3;
		player[pl].frogState |= FROGSTATUS_ISLONGHOPPING;
		// requests long hop up
		if(!MoveToRequestedDestination(MOVE_LEFT,pl))
		{
			frog[pl]->actor->animation->animTime = 0;
			AnimateActor(frog[pl]->actor,0,NO,NO,frogAnimSpeed);
			AnimateActor(frog[pl]->actor,3,YES,YES,1.0F);
			
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGLONGHOPL;

			if ( currPlatform )
				player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
			else
				player[pl].frogState |= FROGSTATUS_ISSTANDING;			

		}
		move = moveVal;
		//frogFacing = (camFacing + 1) & 3;		
		PlaySample ( 24,NULL,255,128);

		CreateAndAddFXRipple(RIPPLE_TYPE_CROAK,&frog[pl]->actor->pos,&upVec,15,1,0.3,15);
	}
	else if(player[pl].frogState & FROGSTATUS_ISWANTINGLONGHOPR)
	{
		frogFacing = (camFacing + 3) & 3;
		player[pl].frogState |= FROGSTATUS_ISLONGHOPPING;
		// requests long hop up
		if(!MoveToRequestedDestination(MOVE_RIGHT,pl))
		{
			frog[pl]->actor->animation->animTime = 0;
			AnimateActor(frog[pl]->actor,0,NO,NO,frogAnimSpeed);
			AnimateActor(frog[pl]->actor,3,YES,YES,1.0F);
			
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGLONGHOPR;

			if ( currPlatform )
				player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
			else
				player[pl].frogState |= FROGSTATUS_ISSTANDING;			

		}
		move = moveVal;
		//frogFacing = camFacing;
		PlaySample ( 24,NULL,255,128);

		CreateAndAddFXRipple(RIPPLE_TYPE_CROAK,&frog[pl]->actor->pos,&upVec,15,1,0.3,15);
	}
	
	
	if(player[pl].frogState & FROGSTATUS_ISWANTINGSUPERHOPU)
	{
		// requests superhop up
		frogFacing = camFacing;
		player[pl].frogState |= FROGSTATUS_ISSUPERHOPPING;
		if(!MoveToRequestedDestination(MOVE_UP,pl))
		{
			isJump = 0;
			superHop = 0;
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPU;
			player[pl].frogState &= ~FROGSTATUS_ISSUPERHOPPING;
			player[pl].frogState |= FROGSTATUS_ISSTANDING;
		}
		move = moveVal;
		PlaySample ( 104,NULL,255,128);

		CreateAndAddFXRipple(RIPPLE_TYPE_DUST,&frog[pl]->actor->pos,&upVec,5,1,0.15,15);
	}
	else if(player[pl].frogState & FROGSTATUS_ISWANTINGSUPERHOPD)
	{
		// requests superhop down
		frogFacing = (camFacing + 2) & 3;
		player[pl].frogState |= FROGSTATUS_ISSUPERHOPPING;
		if(!MoveToRequestedDestination(MOVE_DOWN,pl))
		{
			isJump = 0;
			superHop = 0;
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPD;
			player[pl].frogState &= ~FROGSTATUS_ISSUPERHOPPING;
			player[pl].frogState |= FROGSTATUS_ISSTANDING;
		}
		move = moveVal;
		PlaySample ( 104,NULL,255,128);

		CreateAndAddFXRipple(RIPPLE_TYPE_DUST,&frog[pl]->actor->pos,&upVec,5,1,0.15,15);
	}
	else if(player[pl].frogState & FROGSTATUS_ISWANTINGSUPERHOPL)
	{
		// requests superhop left
		frogFacing = (camFacing + 1) & 3;
		player[pl].frogState |= FROGSTATUS_ISSUPERHOPPING;
		if(!MoveToRequestedDestination(MOVE_LEFT,pl))
		{
			isJump = 0;
			superHop = 0;
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPL;
			player[pl].frogState &= ~FROGSTATUS_ISSUPERHOPPING;
			player[pl].frogState |= FROGSTATUS_ISSTANDING;
		}
		move = moveVal;
		PlaySample ( 104,NULL,255,128);

		CreateAndAddFXRipple(RIPPLE_TYPE_DUST,&frog[pl]->actor->pos,&upVec,5,1,0.15,15);
	}
	else if(player[pl].frogState & FROGSTATUS_ISWANTINGSUPERHOPR)
	{
		// requests superhop right
		frogFacing = (camFacing + 3) & 3;
		player[pl].frogState |= FROGSTATUS_ISSUPERHOPPING;
		if(!MoveToRequestedDestination(MOVE_RIGHT,pl))
		{
			isJump = 0;
			superHop = 0;
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPR;
			player[pl].frogState &= ~FROGSTATUS_ISSUPERHOPPING;
			player[pl].frogState |= FROGSTATUS_ISSTANDING;
		}
		move = moveVal;
		PlaySample ( 104,NULL,255,128);

		CreateAndAddFXRipple(RIPPLE_TYPE_DUST,&frog[pl]->actor->pos,&upVec,5,1,0.15,15);
	}

	// process frog's jump
	if ( (player[pl].frogState & FROGSTATUS_ISJUMPINGTOTILE ) && ( destTile[pl] ) )
	{
		SlurpFroggerPosition(JUMPING_TOTILE,pl);
		CheckForFroggerLanding(JUMPING_TOTILE,pl);
	}
	else if(player[pl].frogState & FROGSTATUS_ISJUMPINGTOPLATFORM)
	{
		// frog is jumping between a game tile and a platform
		SlurpFroggerPosition(JUMPING_TOPLATFORM,pl);
		CheckForFroggerLanding(JUMPING_TOPLATFORM,pl);
	}

	// check if frog is on fire, etc.
	if(frog[pl]->action.isOnFire)
	{
		frog[pl]->action.isOnFire--;
		if((frog[pl]->action.isOnFire & 3) == 0)
		{
			SetVector(&effectPos,&frog[pl]->actor->pos);
			effectPos.v[Y] += 25;
			CreateAndAddFXSmoke(SMOKE_TYPE_NORMAL,&effectPos,128,1,0.4,30);
		}
	}
}

/*---------------------------------------------------------------------------------------------
	Function	: UpdateFroggerPos
	Parameters	: ()
	Returns		: void pl
*/

void UpdateFroggerPos2()
{
	if(frogState2 & FROGSTATUS_ISDEAD)
	{
		KillFrog(frog2,0);
		return;
	}


	if(frogState2 & FROGSTATUS_ISFALLINGTODEATH)
	{
	}
	if(frogState2 & FROGSTATUS_ISSTANDING)
	{
	}
	if(frogState2 & FROGSTATUS_ISONMOVINGPLATFORM)
	{
	}


	if(frogState2 & FROGSTATUS_ISWANTINGU)
	{
		// requests up
		if(!MoveToRequestedDestination2(MOVE_UP))
		{
			frog2->actor->animation->animTime = 0;
			AnimateActor(frog2->actor,0,NO,NO,frogAnimSpeed);
			AnimateActor(frog2->actor,3,YES,YES,1.0F);
			
			frogState2 &= ~FROGSTATUS_ISWANTINGU;
			frogState2 |= FROGSTATUS_ISSTANDING;			

		}
		move = moveVal;
		frogFacing2 = camFacing2;
		
	}
	else if(frogState2 & FROGSTATUS_ISWANTINGD)
	{
		// requests down
		if(!MoveToRequestedDestination2(MOVE_DOWN))
		{
			frog2->actor->animation->animTime = 0;
			AnimateActor(frog2->actor,0,NO,NO,frogAnimSpeed);
			AnimateActor(frog2->actor,3,YES,YES,1.0F);
		
			frogState2 &= ~FROGSTATUS_ISWANTINGD;
			frogState2 |= FROGSTATUS_ISSTANDING;
		}
		move = moveVal;
		frogFacing2 = (camFacing2 + 2) & 3;
	}
	else if(frogState2 & FROGSTATUS_ISWANTINGL)
	{
		// requests left
		if(!MoveToRequestedDestination2(MOVE_LEFT))
		{
			frog2->actor->animation->animTime = 0;
			AnimateActor(frog2->actor,0,NO,NO,frogAnimSpeed);
			AnimateActor(frog2->actor,3,YES,YES,1.0F);
		
			frogState2 &= ~FROGSTATUS_ISWANTINGL;
			frogState2 |= FROGSTATUS_ISSTANDING;
		}
		move = moveVal;
		frogFacing2 = (camFacing2 + 1) & 3;				
	}
	else if(frogState2 & FROGSTATUS_ISWANTINGR)
	{
		// requests right
		if(!MoveToRequestedDestination2(MOVE_RIGHT))
		{
			frog2->actor->animation->animTime = 0;
			AnimateActor(frog2->actor,0,NO,NO,frogAnimSpeed);
			AnimateActor(frog2->actor,3,YES,YES,1.0F);
		
			frogState2 &= ~FROGSTATUS_ISWANTINGR;
			frogState2 |= FROGSTATUS_ISSTANDING;
		}
		move = moveVal;
		frogFacing2 = (camFacing2 + 3) & 3;				
	}

	// process frog's jump
	if(frogState2 & FROGSTATUS_ISJUMPINGTOTILE)
	{
		SlurpFroggerPosition2(JUMPING_TOTILE);
		CheckForFroggerLanding2(JUMPING_TOTILE);
	}
	else if(frogState2 & FROGSTATUS_ISJUMPINGTOPLATFORM)
	{
		// frog is jumping between a game tile and a platform
		SlurpFroggerPosition2(JUMPING_TOPLATFORM);
		CheckForFroggerLanding2(JUMPING_TOPLATFORM);
	}	
}

/*	--------------------------------------------------------------------------------
	Function		: GetNextTile(unsigned long direction)
	Purpose			: determines the next tile
	Parameters		:
	Returns			:
	Info			:
*/
void GetNextTile(unsigned long direction,long pl)
{
	VECTOR cDir;
	unsigned long i,j,n;
	unsigned long closest[4] = {-1,-1,-1,-1};
	float distance,t;
	unsigned long newCamFacing = camFacing;

	GAMETILE *joiningTile = NULL;
	VECTOR vecUp,newVec;
	float t2,at2;
	
	for (i=0; i<4; i++)
		if (currTile[0]->tilePtrs[i])
		{
			SubVector (&cDir,(&currTile[0]->centre),(&(currTile[0]->tilePtrs[i]->centre)));
			MakeUnit (&cDir);
			
			distance = 10000;
			
			for (j=0; j<4; j++)
			{	
				t = DotProduct(&cDir,&(currTile[0]->dirVector[j]));
				if (t<distance)
				{
					distance = t;
					closest[i] = j;					
				}
			}
		}
		
	destTile[0] = NULL;
		
	for (i=0; i<4; i++)
	{
		if (closest[i] == ((direction+camFacing)&3))
		{
			distance = -10000;
			
			destTile[0] = currTile[0]->tilePtrs[i];
			for (j=0; j<4; j++)
			{
				t = DotProduct(&(destTile[0]->dirVector[j]),&(currTile[0]->dirVector[camFacing]));
				if (t>distance)
				{
					distance = t;
					newCamFacing = j;							
				}
				
			}		
		}
	}

	if(destTile[0])
	{
		joiningTile = destTile[0];

		if ((destTile[0]->state == TILESTATE_SUPERHOP) || (destTile[0]->state == TILESTATE_JOIN))
		{
			
			SetVector(&vecUp,&currTile[0]->normal);

			distance = -1000;
			
			for(i=0; i<4; i++)
			{
				if(joiningTile->tilePtrs[i])
				{					
					t2 = DotProduct(&vecUp,&joiningTile->dirVector[i]);
					t = fabs(t2);
					if(t > distance)
					{
						if (currTile[0] != joiningTile->tilePtrs[i])
						{
							distance = t;
							n = i;
							at2 = t2;
						}
					}
				}
			}

			destTile[0] = joiningTile->tilePtrs[n];

			if ((joiningTile->state == TILESTATE_SUPERHOP))
			{
				wasSuperHop = 1;
				if (DotProduct(&vecUp,&joiningTile->dirVector[n])<0)
				{
					if(!superHop)
					{
						destTile[0] = NULL;						
						return;
					}
				}
			}
		}

		// frog is jumping to available tile
		player[0].frogState |= FROGSTATUS_ISJUMPINGTOTILE;

		// check if a platform is in the destination tile
		destPlatform = JumpingToTileWithPlatform(destTile[0]);
	}

	if(destTile[0])
	{
		distance = -1000;
		
		for(i=0; i<4; i++)
		{
			t = DotProduct(&currTile[0]->dirVector[camFacing],&destTile[0]->dirVector[i]);
			if(t > distance)
			{
				distance = t;
				newCamFacing = i;			
			}
		}

		camFacing = newCamFacing;

		//currTile = destTile;
	}

	if ( ( destTile[0] ) && ( player[0].frogState & FROGSTATUS_ISFLOATING ) )
		currTile[0] = destTile[0];

	if ( ( longHop ) && ( ( player[0].frogState & FROGSTATUS_ISWANTINGLONGHOPU ) ||
						  ( player[0].frogState & FROGSTATUS_ISWANTINGLONGHOPD ) ||
						  ( player[0].frogState & FROGSTATUS_ISWANTINGLONGHOPL ) ||
						  ( player[0].frogState & FROGSTATUS_ISWANTINGLONGHOPR ) ) )
	{
//		dprintf"LONG HOP"));

/*		longHopDestTile =  destTile->tilePtrs[n];
		if ( ( longHopDestTile->state == TILESTATE_SUPERHOP) || (longHopDestTile->state == TILESTATE_JOIN))
			longHopDestTile =  longHopDestTile->tilePtrs[n];
/*		for (i=0; i<4; i++)
			if (destTile->tilePtrs[i])
			{
				SubVector (&cDir,(&destTile->centre),(&(destTile->tilePtrs[i]->centre)));
				MakeUnit (&cDir);
				
				distance = 10000;
				
				for (j=0; j<4; j++)
				{	
					t = DotProduct(&cDir,&(destTile->dirVector[j]));
					if (t<distance)
					{
						distance = t;
						closest[i] = j;					
					}
				}
			}
			
		longHopDestTile = NULL;
			
		for (i=0; i<4; i++)
		{
			if (closest[i] == ((direction+camFacing)&3))
			{
				distance = -10000;
				
				longHopDestTile = destTile->tilePtrs[i];

				for (j=0; j<4; j++)
				{
					t = DotProduct(&(destTile->dirVector[j]),&(longHopDestTile->dirVector[camFacing]));
					if (t>distance)
					{
						distance = t;
						newCamFacing = j;							
					}
					
				}		
			}
		}

//		dprintf"Found destTile\n"));
		if(longHopDestTile)
		{
			joiningTile = longHopDestTile;

			if ((longHopDestTile->state == TILESTATE_SUPERHOP) || (longHopDestTile->state == TILESTATE_JOIN))
			{
				
//				dprintf"Found JOIN TILE\n"));
				SetVector(&vecUp,&destTile->normal);

				distance = -1000;
				
				for(i=0; i<4; i++)
				{
					if(joiningTile->tilePtrs[i])
					{					
						t2 = DotProduct(&vecUp,&joiningTile->dirVector[i]);
						t = fabs(t2);
						if(t > distance)
						{
							if (destTile != joiningTile->tilePtrs[i])
							{
								distance = t;
								n = i;
								at2 = t2;
							}
						}
					}
				}

				longHopDestTile = joiningTile->tilePtrs[n];
//				dprintf"Found New destTile"));
				
				if ((joiningTile->state == TILESTATE_SUPERHOP))
				{
					dprintf"----\n"));
//					dprintf"SUPERHOP TILE\n"));
					wasSuperHop = 1;
					if (DotProduct(&vecUp,&joiningTile->dirVector[n])<0)
					{
//						dprintf"IS UP TILE\n"));
						if(!superHop)
						{
//							dprintf"S-HOP NEEDED!\n"));
							destTile = NULL;						
							return;
						}
					}
//					dprintf"----\n"));
				}
			}

//			dprintf"Jumping To Tile\n"));
			// frog is jumping to available tile
			frogState |= FROGSTATUS_ISJUMPINGTOTILE;

			// check if a platform is in the destination tile
			destPlatform = JumpingToTileWithPlatform(longHopDestTile);
		}

		if(longHopDestTile)
		{
			distance = -1000;
			
			for(i=0; i<4; i++)
			{
				t = DotProduct(&destTile->dirVector[camFacing],&longHopDestTile->dirVector[i]);
				if(t > distance)
				{
					distance = t;
					newCamFacing = i;			
				}
			}

			camFacing = newCamFacing;

			//currTile = destTile;
		}
			*/
		//destTile = longHopDestTile;
	}
	// ENDIF - 
}

/*---------------------------------------------------------------------------------------------
	Function	: GetNextTile2
	Parameters	: (unsigned long direction)
	Returns		: void 
*/

void GetNextTile2(unsigned long direction)
{
	VECTOR cDir;
	unsigned long i,j,n;
	unsigned long closest[4] = {-1,-1,-1,-1};
	float distance,t;
	unsigned long newCamFacing = camFacing2;

	GAMETILE *joiningTile = NULL;
	VECTOR vecUp,newVec;
	float t2,at2;
	
	for (i=0; i<4; i++)
		if (currTile2->tilePtrs[i])
		{
			SubVector (&cDir,(&currTile2->centre),(&(currTile2->tilePtrs[i]->centre)));
			MakeUnit (&cDir);
			
			distance = 10000;
			
			for (j=0; j<4; j++)
			{	
				t = DotProduct(&cDir,&(currTile2->dirVector[j]));
				if (t<distance)
				{
					distance = t;
					closest[i] = j;					
				}
			}
		}
		
	destTile2 = NULL;
		
	for (i=0; i<4; i++)
	{
		if (closest[i] == ((direction+camFacing2)&3))
		{
			distance = -10000;
			
			destTile2 = currTile2->tilePtrs[i];

			for (j=0; j<4; j++)
			{
				t = DotProduct(&(destTile2->dirVector[j]),&(currTile2->dirVector[camFacing2]));
				if (t>distance)
				{
					distance = t;
					newCamFacing = j;							
				}
				
			}		
		}
	}

	if(destTile2)
	{
		joiningTile = destTile2;

		if ((destTile2->state == TILESTATE_SUPERHOP) || (destTile2->state == TILESTATE_JOIN))
		{
			
			SetVector(&vecUp,&currTile2->normal);

			distance = -1000;
			
			for(i=0; i<4; i++)
			{
				if(joiningTile->tilePtrs[i])
				{					
					t2 = DotProduct(&vecUp,&joiningTile->dirVector[i]);
					t = fabs(t2);
					if(t > distance)
					{
						if (currTile2 != joiningTile->tilePtrs[i])
						{
							distance = t;
							n = i;
							at2 = t2;
						}
					}
				}
			}

			destTile2 = joiningTile->tilePtrs[n];
			
			if ((joiningTile->state == TILESTATE_SUPERHOP))
			{
//				dprintf"----\n"));
//				dprintf"SUPERHOP TILE\n"));
				wasSuperHop = 1;
				if (DotProduct(&vecUp,&joiningTile->dirVector[n])<0)
				{
//					dprintf"IS UP TILE\n"));
					if(!superHop)
					{
//						dprintf"S-HOP NEEDED!\n"));
						destTile2 = NULL;						
						return;
					}
				}
//				dprintf"----\n"));
			}
		}

		// frog is jumping to available tile
		frogState2 |= FROGSTATUS_ISJUMPINGTOTILE;		
	}

	if(destTile2)
	{
		distance = -1000;
		
		for(i=0; i<4; i++)
		{
			t = DotProduct(&currTile[0]->dirVector[camFacing2],&destTile2->dirVector[i]);
			if(t > distance)
			{
				distance = t;
				newCamFacing = i;			
			}
		}

		camFacing2 = newCamFacing;
	}

}

/*	--------------------------------------------------------------------------------
	Function		: MoveToRequestedDestination
	Purpose			: determines if we can move to the requested destination
	Parameters		: int
	Returns			: TRUE if valid destination, else FALSE
	Info			:
*/
extern ACTOR2 *front;
long speedFrameCount = 0;


BOOL MoveToRequestedDestination2(int dir)
{
	// clear movement request flags
	frogState2 &= ~FROGSTATUS_ISWANTINGU;
	frogState2 &= ~FROGSTATUS_ISWANTINGD;
	frogState2 &= ~FROGSTATUS_ISWANTINGL;
	frogState2 &= ~FROGSTATUS_ISWANTINGR;

	// see if frog is currently on a moving platform (i.e. platform that follows a path)
	
	// Select the requested destination tile based on current position and direction
	switch(dir)
	{
		case MOVE_UP:
			GetNextTile2(0);
			if(destTile2 != NULL)
			{
			}
			break;

		case MOVE_LEFT:
			GetNextTile2(1);
			if(destTile2 != NULL)
			{
			}
			break;

		case MOVE_DOWN:
			GetNextTile2(2);
			if(destTile2 != NULL)
			{
			}
			break;

		case MOVE_RIGHT:
			GetNextTile2(3);
			if(destTile2 != NULL)
			{
			}
			break;
	}

	// check if jumping to a valid tile
	if(destTile2)
	{
	
	}
	else
	{
		// clear all superhop flags
		frogState2 &= ~FROGSTATUS_ISWANTINGSUPERHOPU; 
		frogState2 &= ~FROGSTATUS_ISWANTINGSUPERHOPD; 
		frogState2 &= ~FROGSTATUS_ISWANTINGSUPERHOPL; 
		frogState2 &= ~FROGSTATUS_ISWANTINGSUPERHOPR; 
		
		return FALSE;
	}

	// clear all superhop flags
	frogState2 &= ~FROGSTATUS_ISWANTINGSUPERHOPU; 
	frogState2 &= ~FROGSTATUS_ISWANTINGSUPERHOPD; 
	frogState2 &= ~FROGSTATUS_ISWANTINGSUPERHOPL; 
	frogState2 &= ~FROGSTATUS_ISWANTINGSUPERHOPR; 

	frog2->actor->animation->animTime = 0;

	AnimateActor(frog2->actor,0,NO,NO,frogAnimSpeed);
	AnimateActor(frog2->actor,3,YES,YES,1.0F);
	
	return TRUE;
}


BOOL MoveToRequestedDestination(int dir,long pl)
{
	char moveLocal		= 0;
	
	// clear movement request flags
	player[pl].frogState &= ~FROGSTATUS_ISWANTINGU;
	player[pl].frogState &= ~FROGSTATUS_ISWANTINGD;
	player[pl].frogState &= ~FROGSTATUS_ISWANTINGL;
	player[pl].frogState &= ~FROGSTATUS_ISWANTINGR;

	if(tongueState & TONGUE_BEINGUSED)
		RemoveFrogTongue();

	// see if frog is currently on a moving platform (i.e. platform that follows a path)
	if(currPlatform = GetPlatformFrogIsOn())
	{
		currTile[pl] = currPlatform->inTile;

		if(currPlatform->flags & PLATFORM_HASPATH)
		{
			// frog is on a platform that follows a path
			if(currPlatform->flags & PLATFORM_SEGMENT)
			{
				// frog is on a platform that follows a path and has local platform segments

				// check if frog is jumping to a local platform
				switch(dir)
				{
					case MOVE_UP:
						GetNextLocalPlatform(0);
						if(destPlatform != NULL)
						{
							if(player[pl].frogState & FROGSTATUS_ISWANTINGSUPERHOPU)
							{
								player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPU;
							}
						}
						break;

					case MOVE_LEFT:
						GetNextLocalPlatform(1);
						if(destPlatform != NULL)
						{
							if(player[pl].frogState & FROGSTATUS_ISWANTINGSUPERHOPL)
							{
								player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPL; 
							}
						}
						break;

					case MOVE_DOWN:
						GetNextLocalPlatform(2);
						if(destPlatform != NULL)
						{
							if(player[pl].frogState & FROGSTATUS_ISWANTINGSUPERHOPD)
							{
								player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPD; 
							}
						}
						break;

					case MOVE_RIGHT:
						GetNextLocalPlatform(3);
						if(destPlatform != NULL)
						{
							if(player[pl].frogState & FROGSTATUS_ISWANTINGSUPERHOPR)
							{
								player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPR; 
							}
						}
						break;
				}

				if(destPlatform)
				{
					// frog is jumping to local platform
					destTile[pl] = destPlatform->inTile;
					moveLocal = 1;
				}
			}

			player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
		}
	}
	wasSuperHop = 0;

	// Select the requested destination tile based on current position and direction
	if(!moveLocal)
	{
		if ( player[pl].frogState & FROGSTATUS_ISLONGHOPPING )
		{
			switch(dir)
			{
				case MOVE_UP:
					GetNextTileLongHop ( 0 );
				break;
				case MOVE_LEFT:
					GetNextTileLongHop ( 1 );
				break;
				case MOVE_DOWN:
					GetNextTileLongHop ( 2 );
				break;
				case MOVE_RIGHT:
					GetNextTileLongHop ( 3 );
				break;
			}
		}
		else
		{
			switch(dir)
			{
				case MOVE_UP:
					GetNextTile(0,pl);
					if(destTile[pl] != NULL)
					{
					}
					break;

				case MOVE_LEFT:
					GetNextTile(1,pl);
					if(destTile[pl] != NULL)
					{
					}
					break;

				case MOVE_DOWN:
					GetNextTile(2,pl);
					if(destTile[pl] != NULL)
					{
					}

					break;

				case MOVE_RIGHT:
					GetNextTile(3,pl);
					if(destTile[pl] != NULL)
					{
					}
					break;
			}
		}
		// ENDELSEIF
	}
	else
	{
		player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOTILE;
		player[pl].frogState |= FROGSTATUS_ISJUMPINGTOPLATFORM;
	}

	// check if jumping to a valid tile
	if(destTile[pl])
	{
		if(currPlatform)
			currPlatform->flags &= ~PLATFORM_CARRYINGFROG;

		if(destPlatform)
		{
			// check if platform is too high to reach
			if(PlatformTooHigh(destPlatform))
			{
				// platform too high
				dprintf"Platform TOO HIGH\n"));

				destTile[pl] = NULL;
				destPlatform = NULL;
				isJump = 0;
				superHop = 0;

				player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOTILE;
				player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOPLATFORM;
				player[pl].frogState &= ~FROGSTATUS_ISSUPERHOPPING;

				return FALSE;
			}
			else if(PlatformTooLow(destPlatform))
			{
				// platform too far below
				dprintf"Platform TOO LOW\n"));

				if ( !( player[pl].frogState & FROGSTATUS_ISFLOATING ) )
				{
					player[pl].frogState |= FROGSTATUS_ISDEAD;
					frog[pl]->action.deathBy = DEATHBY_FALLINGTOPLATFORM;
					frog[pl]->action.dead = 50;
				}
				// ENDIF

				player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOTILE;
				player[pl].frogState |= FROGSTATUS_ISJUMPINGTOPLATFORM;
				player[pl].frogState &= ~FROGSTATUS_ISSUPERHOPPING;

				return FALSE;
			}
		}

		// check if gametile is too high to reach
		
		if ( !destPlatform )
		{
			if( GameTileTooHigh ( destTile[pl] ) )
			{			
					// tile too high
					dprintf"GameTile TOO HIGH\n"));

					destTile[pl] = NULL;
					destPlatform = NULL;
					isJump = 0;
					superHop = 0;

					player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOTILE;
					player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOPLATFORM;
					player[pl].frogState &= ~FROGSTATUS_ISSUPERHOPPING;
					player[pl].frogState &= ~FROGSTATUS_ISLONGHOPPING;
				
//					if(currPlatform)
//						currPlatform->flags |= PLATFORM_CARRYINGFROG;

					return FALSE;			
			}
			else if ( GameTileTooLow ( destTile[pl] ) && ( !( player[pl].frogState & FROGSTATUS_ISFLOATING ) ) )
			{
					// platform too far below
					dprintf"GameTile TOO LOW\n"));

					player[pl].frogState |= FROGSTATUS_ISDEAD;
					frog[pl]->action.deathBy = DEATHBY_FALLINGTOTILE;
					frog[pl]->action.dead = 50;

					player[pl].frogState |= FROGSTATUS_ISJUMPINGTOTILE;
					player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOPLATFORM;
					player[pl].frogState &= ~FROGSTATUS_ISSUPERHOPPING;
					player[pl].frogState &= ~FROGSTATUS_ISLONGHOPPING;

					return FALSE;			
			}
		}
		// ENDIF

	}
	else
	{
		// clear all superhop flags
		player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPU; 
		player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPD; 
		player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPL; 
		player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPR; 

		player[pl].frogState &= ~FROGSTATUS_ISWANTINGLONGHOPU;
		player[pl].frogState &= ~FROGSTATUS_ISWANTINGLONGHOPD;
		player[pl].frogState &= ~FROGSTATUS_ISWANTINGLONGHOPL;
		player[pl].frogState &= ~FROGSTATUS_ISWANTINGLONGHOPR;
		
		return FALSE;
	}

	// clear all superhop flags
	player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPU; 
	player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPD; 
	player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPL; 
	player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPR; 

	player[pl].frogState &= ~FROGSTATUS_ISWANTINGLONGHOPU;
	player[pl].frogState &= ~FROGSTATUS_ISWANTINGLONGHOPD;
	player[pl].frogState &= ~FROGSTATUS_ISWANTINGLONGHOPL;
	player[pl].frogState &= ~FROGSTATUS_ISWANTINGLONGHOPR;

	if ( !( player[pl].frogState & FROGSTATUS_ISFLOATING ) )
	{
		frog[pl]->actor->animation->animTime = 0;

		AnimateActor(frog[pl]->actor,0,NO,NO,frogAnimSpeed);
		AnimateActor(frog[pl]->actor,3,YES,YES,1.0F);
	}
	// ENDIF
	numHops_TOTAL++;
	if (speedFrameCount)
		speedHops_TOTAL += frameCount-speedFrameCount;	
	speedFrameCount = frameCount;

	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function		: SlurpFroggerPosition
	Purpose			: moves Frogger to a new position
	Parameters		: int
	Returns			: void
	Info			:
*/

float speedTest = 11;

void SlurpFroggerPosition(int whereTo,long pl)
{
	VECTOR vfd	= { 0,0,1 };
	VECTOR vup	= { 0,1,0 };
	VECTOR babyup;
	VECTOR v1,v2,v3;
	unsigned long i;
	float frogScale;
		
	VECTOR fwd;
	
	if(whereTo == JUMPING_TOPLATFORM)
	{
		player[pl].frogState &= ~FROGSTATUS_ISSTANDING;

		SubVector(&fwd,&destPlatform->pltActor->actor->pos,&frog[pl]->actor->pos);
		MakeUnit(&fwd);
		frog[pl]->actor->pos.v[X] += (fwd.v[X] * speedTest);
		frog[pl]->actor->pos.v[Y] += (fwd.v[Y] * speedTest);
		frog[pl]->actor->pos.v[Z] += (fwd.v[Z] * speedTest);
	}
	else
	{
		if ( player[pl].frogState & FROGSTATUS_ISFLOATING )
		{
			player[pl].frogState &= ~FROGSTATUS_ISSTANDING;		
			SubVector(&fwd,&destTile[pl]->centre,&frog[pl]->actor->pos);
			MakeUnit(&fwd);
			frog[pl]->actor->pos.v[X] += (fwd.v[X] * 55);
			frog[pl]->actor->pos.v[Y] += (fwd.v[Y] * speedTest);
			frog[pl]->actor->pos.v[Z] += (fwd.v[Z] * 55);
			if (frog[pl])
				if (frog[pl]->actor->animation->animTime > 10)
					frog[pl]->actor->animation->animTime = 10;
			CheckTileForCollectable(destTile[pl]);
		} else if ( player[pl].frogState & FROGSTATUS_ISSPRINGING )
		{
			player[pl].frogState &= ~FROGSTATUS_ISSTANDING;		
			//SubVector(&fwd,&destTile->centre,&frog->actor->pos);
			//MakeUnit(&fwd);
			//frog->actor->pos.v[X] += (fwd.v[X] * 55);
			frog[pl]->actor->pos.v[Y] += /*(fwd.v[Y] * */speedTest/*)*/;
			speedTest -= 1.0f;
			if ( speedTest < 2.0f )
			{
// ???			speedTest == 2.0f;
				speedTest = 2.0f;
				croakFloat = 90;
				camSpeed = 9;
				player[pl].frogState &= ~FROGSTATUS_ISSPRINGING;
				player[pl].frogState |= FROGSTATUS_ISFLOATING;

			}
			// endif
			//frog->actor->pos.v[Z] += (fwd.v[Z] * 55);
			if (frog[pl])
				if (frog[pl]->actor->animation->animTime > 10)
					frog[pl]->actor->animation->animTime = 10;
			CheckTileForCollectable(destTile[pl]);
		}
		else
		{
			player[pl].frogState &= ~FROGSTATUS_ISSTANDING;		
			SubVector(&fwd,&destTile[pl]->centre,&frog[pl]->actor->pos);
			MakeUnit(&fwd);
			frog[pl]->actor->pos.v[X] += (fwd.v[X] * speedTest);
			frog[pl]->actor->pos.v[Y] += (fwd.v[Y] * speedTest);
			frog[pl]->actor->pos.v[Z] += (fwd.v[Z] * speedTest);
		}
		// ENDELSEIF
	}

	if (isJump)
	{
		jumpAmt += jumpSpeed;
		jumpSpeed += changeJumpSpeed;
		if (frog[pl])
			if (frog[pl]->actor->animation->animTime > 9)
				frog[pl]->actor->animation->animTime = 9;

		if(whereTo == JUMPING_TOPLATFORM)
			frog[pl]->actor->pos.v[Y] = destPlatform->pltActor->actor->pos.v[Y] + jumpAmt;
		else
		{
			frog[pl]->actor->pos.v[Y] = destTile[pl]->centre.v[Y] + jumpAmt;
//			dprintf"\n\n%f - %f - %f\n\n", frog->actor->pos.v[Y], destTile->centre.v[Y], jumpAmt));
		}

		frogScale = jumpAmt * 0.001F;
		frog[pl]->actor->scale.v[X] = 0.09F + frogScale;
		frog[pl]->actor->scale.v[Y] = 0.09F + frogScale;
		frog[pl]->actor->scale.v[Z] = 0.09F + frogScale;
	}
	
	if (isLong)
	{
		longAmt += longSpeed;
		longSpeed += changeLongSpeed;
		if (frog[pl])
			if (frog[pl]->actor->animation->animTime > 10)
				frog[pl]->actor->animation->animTime = 10;

		if(whereTo == JUMPING_TOPLATFORM)
			frog[pl]->actor->pos.v[Y] = destPlatform->pltActor->actor->pos.v[Y] + longAmt;
		else
		{
			frog[pl]->actor->pos.v[Y] = destTile[pl]->centre.v[Y] + longAmt;
//			dprintf"\n\n%f - %f - %f\n\n", frog->actor->pos.v[Y], destTile->centre.v[Y], jumpAmt));
		}

		frogScale = longAmt * 0.001F;
		frog[pl]->actor->scale.v[X] = 0.09F + frogScale;
		frog[pl]->actor->scale.v[Y] = 0.09F + frogScale;
		frog[pl]->actor->scale.v[Z] = 0.09F + frogScale;
	}
	
	for (i=0; i<numBabies; i++)
	{
		if(!(babies[i]->action.isSaved))
		{
			babies[i]->actor->pos.v[X] = bTStart[i]->centre.v[X];
			babies[i]->actor->pos.v[Y] = bTStart[i]->centre.v[Y];
			babies[i]->actor->pos.v[Z] = bTStart[i]->centre.v[Z];
			babies[i]->action.isSaved = 0;
		}

		// Face babies towards frog
		SubVector(&v1,&babies[i]->actor->pos,&frog[pl]->actor->pos);
		MakeUnit(&v1);

		// Calculate babys up vector
		RotateVectorByQuaternion(&babyup,&vup,&babies[i]->actor->qRot);
		CrossProduct(&v2,&v1,&babyup);
		CrossProduct(&v3,&v2,&babyup);
		Orientate(&babies[i]->actor->qRot,&v3,&vfd,&babyup);
	}
}

void SlurpFroggerPosition2(int whereTo)
{
	VECTOR fwd;
	
	if(whereTo == JUMPING_TOPLATFORM)
	{
		player[0].frogState &= ~FROGSTATUS_ISSTANDING;

		SubVector(&fwd,&destPlatform->pltActor->actor->pos,&frog[0]->actor->pos);
		MakeUnit(&fwd);
		frog[0]->actor->pos.v[X] += (fwd.v[X] * 16.0F);
		frog[0]->actor->pos.v[Y] += (fwd.v[Y] * 16.0F);
		frog[0]->actor->pos.v[Z] += (fwd.v[Z] * 16.0F);
	}
	else
	{
		frogState2 &= ~FROGSTATUS_ISSTANDING;		

		SubVector(&fwd,&destTile2->centre,&frog2->actor->pos);
		MakeUnit(&fwd);
		frog2->actor->pos.v[X] += (fwd.v[X] * 16.0F);
		frog2->actor->pos.v[Y] += (fwd.v[Y] * 16.0F);
		frog2->actor->pos.v[Z] += (fwd.v[Z] * 16.0F);		
	}
	
}

/*	--------------------------------------------------------------------------------
	Function		: CheckForFroggerLanding
	Purpose			: determine if Frogger has finished his jump !
	Parameters		: int
	Returns			: void
	Info			:
*/
void CheckForFroggerLanding(int whereTo,long pl)
{
	VECTOR telePos;
	unsigned long i,j;
	float distance;

	if(whereTo == JUMPING_TOPLATFORM)
	{
		distance = DistanceBetweenPointsSquared(&frog[pl]->actor->pos,&destPlatform->pltActor->actor->pos);

		if(distance < (landRadius * landRadius))
		{
			// Okay - Frogger has landed - snap him to the centre of the platform
			SetVector(&frog[pl]->actor->pos,&destPlatform->pltActor->actor->pos);
			isJump = 0;
			superHop = 0;
			if(player[pl].frogState & FROGSTATUS_ISDEAD)
			{
				cameraShake = 35;
				AnimateActor(frog[pl]->actor,2,NO,NO,0.367);
				frog[pl]->action.deathBy = DEATHBY_NORMAL;
				frog[pl]->action.dead = 35;
			}
			else
			{
				frog[pl]->action.deathBy = -1;
			}

			destPlatform->flags |= PLATFORM_CARRYINGFROG;
			player[pl].frogState			|= FROGSTATUS_ISONMOVINGPLATFORM;
			player[pl].frogState			|= FROGSTATUS_ISSTANDING;
			
			player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOTILE;
			player[pl].frogState &= ~FROGSTATUS_ISFLOATING;
			player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOPLATFORM;
			player[pl].frogState &= ~FROGSTATUS_ISSUPERHOPPING;
			currPlatform = destPlatform;
		}
	}
	else
	{
		distance = DistanceBetweenPointsSquared(&frog[pl]->actor->pos,&destTile[pl]->centre);
		if(distance < (landRadius * landRadius))
		{
			if ( destTile[pl]->state == TILESTATE_SPRING )
			{
				player[pl].frogState |= FROGSTATUS_ISSPRINGING;
//				frogState &= ~FROGSTATUS_ISJUMPINGTOTILE;
				player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOPLATFORM;
				player[pl].frogState &= ~FROGSTATUS_ISONMOVINGPLATFORM;
				player[pl].frogState &= ~FROGSTATUS_ISSUPERHOPPING;
				player[pl].frogState &= ~FROGSTATUS_ISLONGHOPPING;
				player[pl].frogState &= ~FROGSTATUS_ISFLOATING;
				speedTest  = 44.0f;
				croakFloat = 0;
				camSpeed   = 1;
				frog[pl]->action.deathBy = DEATHBY_FALLINGTOTILE;
				dprintf"SPRING TILE\n"));
			}
			else
			{
				// Okay - Frogger has landed - snap him to the centre of the current tile
				SetVector(&frog[pl]->actor->pos,&destTile[pl]->centre);
				isLong = 0;
				isJump = 0;
				superHop = 0;
				speedTest = 11.0f;
				frog[pl]->action.deathBy = -1;
				frog[pl]->action.dead	 = 0;
				if(player[pl].frogState & FROGSTATUS_ISDEAD)
				{
					cameraShake = 35;
					AnimateActor(frog[pl]->actor,2,NO,NO,0.367);
					frog[pl]->action.deathBy = DEATHBY_NORMAL;
					frog[pl]->action.dead = 35;
				}

				player[pl].frogState |= FROGSTATUS_ISSTANDING;
				player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOTILE;
				player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOPLATFORM;
				player[pl].frogState &= ~FROGSTATUS_ISONMOVINGPLATFORM;
				player[pl].frogState &= ~FROGSTATUS_ISSUPERHOPPING;
				player[pl].frogState &= ~FROGSTATUS_ISLONGHOPPING;
				player[pl].frogState &= ~FROGSTATUS_ISFLOATING;

				// Next, check if Frogger has landed on a collectable, too....
				CheckTileForCollectable(destTile[pl]);
				currTile[pl] = destTile[pl];
				landRadius = 20.0f;

				// check if the tile is a teleport tile
				if(IsATeleportTile(currTile[pl]))
				{
					// frog is teleporting
					player[pl].frogState &= ~FROGSTATUS_ISSTANDING;
					player[pl].frogState |= FROGSTATUS_ISTELEPORTING;
					frog[pl]->action.isTeleporting = 25;
					frog[pl]->action.safe = 65;

					fadeDir		= FADE_OUT;
					fadeOut		= 1;
					fadeStep	= 8;
					doScreenFade = 63;

					SetVector(&telePos,&frog[pl]->actor->pos);
					CreateAndAddFXRipple(RIPPLE_TYPE_TELEPORT,&telePos,&upVec,30,0,0,15);
					telePos.v[Y] += 20;
					CreateAndAddFXRipple(RIPPLE_TYPE_TELEPORT,&telePos,&upVec,25,0,0,20);
					telePos.v[Y] += 40;
					CreateAndAddFXRipple(RIPPLE_TYPE_TELEPORT,&telePos,&upVec,20,0,0,25);
					telePos.v[Y] += 60;
					CreateAndAddFXRipple(RIPPLE_TYPE_TELEPORT,&telePos,&upVec,15,0,0,30);
					PlaySample(88,NULL,255,128);
				}
			}
			// ENDELSEIF
		}
	}
}

void CheckForFroggerLanding2(int whereTo)
{
	unsigned long i,j;
	float distance;

	{
		distance = DistanceBetweenPointsSquared(&frog2->actor->pos,&destTile2->centre);
		if(distance < (landRadius * landRadius))
		{
			// Okay - Frogger has landed - snap him to the centre of the current tile
			SetVector(&frog2->actor->pos,&destTile2->centre);
			isJump = 0;
			superHop = 0;
			if(player[0].frogState & FROGSTATUS_ISDEAD)
			{
				cameraShake = 35;
				AnimateActor(frog2->actor,2,NO,NO,0.367);
				frog2->action.deathBy = DEATHBY_NORMAL;
				frog2->action.dead = 35;
			}

			frogState2 |= FROGSTATUS_ISSTANDING;
			frogState2 &= ~FROGSTATUS_ISJUMPINGTOTILE;
			frogState2 &= ~FROGSTATUS_ISJUMPINGTOPLATFORM;
			frogState2 &= ~FROGSTATUS_ISONMOVINGPLATFORM;
			frogState2 &= ~FROGSTATUS_ISSUPERHOPPING;

			currTile2 = destTile2;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function		: GameTileTooHigh
	Purpose			: checks is destination tile is too high to jump to
	Parameters		: GAMETILE *
	Returns			: BOOL
	Info			: 
*/
BOOL GameTileTooHigh(GAMETILE *tile)
{
	VECTOR vec;
	float height;

	height = (tile->centre.v[Y] - frog[0]->actor->pos.v[Y]);

//	dprintf"%.f, %.f %.f", tile->centre.v[Y], frog->actor->pos.v[Y], height));
	if(height > 51.0F)
	{
		// cannot be jumped up to either with or without superhop
		return TRUE;
	}

	if(height > 25.0F && !superHop)
	{
		// too high - need superhop for this jump up
		return TRUE;
	}
	
	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function		: GameTileTooLow
	Purpose			: checks is destination tile is too low to jump to
	Parameters		: GAMETILE *
	Returns			: BOOL
	Info			: 
*/
BOOL GameTileTooLow(GAMETILE *tile)
{
	VECTOR vec;
	float height;

	height = (frog[0]->actor->pos.v[Y] - tile->centre.v[Y]);

	if(height > 125.0F)
	{
		// tile too far below
		return TRUE;
	}
	
	// tile can be jumped down to
	return FALSE;
}


/*	--------------------------------------------------------------------------------
	Function		: KillFrog
	Purpose			: kills the specified frog - runs the relevant death sequence
	Parameters		: ACTOR2 *
	Returns			: BOOL - true if running death 'sequence'
	Info			: 
*/

PLANE2 debug_plane2;

BOOL KillFrog(ACTOR2 *frogAct,long pl)
{
	frogAct->action.dead--;
	if(!frogAct->action.dead || frogAct->action.deathBy == DEATHBY_INSTANT)
	{
		numHealth_TOTAL++;
		// lose a life
		player[pl].lives--;
		if(!player[pl].lives)
		{
//			runningDevStuff = 1;
			gameIsOver = 250;
			return FALSE;
		}

		player[pl].frogState &= ~FROGSTATUS_ISDEAD;
		SetFroggerStartPos(gTStart[0],frogAct,pl);
		ResetPlatformFlags();
		camFacing = 2;
		currCamSetting = 1;
		ChangeCameraSetting();

		return FALSE;
	}

	switch(frogAct->action.deathBy)
	{
		case DEATHBY_NORMAL:
			break;

		case DEATHBY_RUNOVER:
			frogAct->actor->qRot.w += 0.1F;
			break;

		case DEATHBY_DROWNING:
			frogAct->actor->pos.v[X] -= (currTile[pl]->normal.v[X] * 2.0F);
			frogAct->actor->pos.v[Y] -= (currTile[pl]->normal.v[Y] * 2.0F);
			frogAct->actor->pos.v[Z] -= (currTile[pl]->normal.v[Z] * 2.0F);
			frogAct->actor->qRot.w -= 0.2F;
			break;

		case DEATHBY_SQUASHED:
			break;

		case DEATHBY_EXPLOSION:
			break;

		case DEATHBY_FALLINGTOPLATFORM:
			dprintf"FALLINGTOPLATFORM\n"));
			SlurpFroggerPosition(JUMPING_TOPLATFORM,pl);
			CheckForFroggerLanding(JUMPING_TOPLATFORM,pl);
			break;

		case DEATHBY_FALLINGTOTILE:
			dprintf"FALLINGTOTILE\n"));
			SlurpFroggerPosition(JUMPING_TOTILE,pl);
			CheckForFroggerLanding(JUMPING_TOTILE,pl);
			break;

		case DEATHBY_CHOCOLATE:
			break;
	}

	return TRUE;
}


void GetNextTileLongHop ( unsigned long direction )
{
	VECTOR cDir;
	unsigned long i,j,n, tileDir;
	unsigned long closest[4] = {-1,-1,-1,-1};
	float distance,t;
	VECTOR temp;
	unsigned long newCamFacing = camFacing;

	GAMETILE *joiningTile = NULL;
	VECTOR vecUp,newVec;
	float t2,at2;
	
	for (i=0; i<4; i++)
		if (currTile[0]->tilePtrs[i])
		{
			SubVector (&cDir,(&currTile[0]->centre),(&(currTile[0]->tilePtrs[i]->centre)));
			MakeUnit (&cDir);
			
			distance = 10000;
			
			for (j=0; j<4; j++)
			{	
				t = DotProduct(&cDir,&(currTile[0]->dirVector[j]));
				if (t<distance)
				{
					distance = t;
					closest[i] = j;					
				}
			}
		}
		
	destTile[0] = NULL;
		
	for (i=0; i<4; i++)
	{
		if (closest[i] == ((direction+camFacing)&3))
		{
			distance = -10000;
			
			destTile[0] = currTile[0]->tilePtrs[i];
			tileDir = i;
			for (j=0; j<4; j++)
			{
				t = DotProduct(&(destTile[0]->dirVector[j]),&(currTile[0]->dirVector[camFacing]));
				if (t>distance)
				{
					distance = t;
					newCamFacing = j;							
				}
				
			}		
		}
	}

	if(destTile[0])
	{
//		dprintf"Found destTile\n"));
		joiningTile = destTile[0];

		if ((destTile[0]->state == TILESTATE_SUPERHOP) || (destTile[0]->state == TILESTATE_JOIN))
		{
			
			SetVector(&vecUp,&currTile[0]->normal);

			distance = -1000;
			
			for(i=0; i<4; i++)
			{
				if(joiningTile->tilePtrs[i])
				{					
					t2 = DotProduct(&vecUp,&joiningTile->dirVector[i]);
					t = fabs(t2);
					if(t > distance)
					{
						if (currTile[0] != joiningTile->tilePtrs[i])
						{
							distance = t;
							n = i;
							at2 = t2;
						}
					}
				}
			}

			destTile[0] = joiningTile->tilePtrs[n];
			//dprintf"Found JOIN TILE"));
			//dprintf"Found new destTile\n"));

			if ((joiningTile->state == TILESTATE_SUPERHOP))
			{
//				dprintf"----\n"));
//				dprintf"SUPERHOP TILE\n"));
				wasSuperHop = 1;
				if (DotProduct(&vecUp,&joiningTile->dirVector[n])<0)
				{
//					dprintf"IS UP TILE\n"));
					if(!superHop)
					{
//						dprintf"S-HOP NEEDED!\n"));
						destTile[0] = NULL;						
						return;
					}
				}
//				dprintf"----\n"));
			}
		}

		// frog is jumping to available tile
		player[0].frogState |= FROGSTATUS_ISJUMPINGTOTILE;

		// check if a platform is in the destination tile
		destPlatform = JumpingToTileWithPlatform(destTile[0]);
	}

	if(destTile[0])
	{
		distance = -1000;
		
		for(i=0; i<4; i++)
		{
			t = DotProduct(&currTile[0]->dirVector[camFacing],&destTile[0]->dirVector[i]);
			if(t > distance)
			{
				distance = t;
				newCamFacing = i;			
			}
		}

		camFacing = newCamFacing;

		//currTile = destTile;
	}


		for (i=0; i<4; i++)
			if (destTile[0]->tilePtrs[i])
			{
				SubVector (&cDir,(&destTile[0]->centre),(&(destTile[0]->tilePtrs[i]->centre)));
				MakeUnit (&cDir);
				
				distance = 10000;
				
				for (j=0; j<4; j++)
				{	
					t = DotProduct(&cDir,&(destTile[0]->dirVector[j]));
					if (t<distance)
					{
						distance = t;
						closest[i] = j;					
					}
				}
			}
			
		longHopDestTile = NULL;
			
		for (i=0; i<4; i++)
		{
			if (closest[i] == ((direction+camFacing)&3))
			{
				distance = -10000;
				
				longHopDestTile = destTile[0]->tilePtrs[i];

				for (j=0; j<4; j++)
				{
					t = DotProduct(&(destTile[0]->dirVector[j]),&(longHopDestTile->dirVector[camFacing]));
					if (t>distance)
					{
						distance = t;
						newCamFacing = j;							
					}
					
				}		
			}
		}

//		dprintf"Found destTile\n"));
		if(longHopDestTile)
		{
			joiningTile = longHopDestTile;

			if ((longHopDestTile->state == TILESTATE_SUPERHOP) || (longHopDestTile->state == TILESTATE_JOIN))
			{
				
//				dprintf"Found JOIN TILE\n"));
				SetVector(&vecUp,&destTile[0]->normal);

				distance = -1000;
				
				for(i=0; i<4; i++)
				{
					if(joiningTile->tilePtrs[i])
					{					
						t2 = DotProduct(&vecUp,&joiningTile->dirVector[i]);
						t = fabs(t2);
						if(t > distance)
						{
							if (destTile[0] != joiningTile->tilePtrs[i])
							{
								distance = t;
								n = i;
								at2 = t2;
							}
						}
					}
				}

				longHopDestTile = joiningTile->tilePtrs[n];
//				dprintf"Found New destTile"));
				
				if ((joiningTile->state == TILESTATE_SUPERHOP))
				{
					dprintf"----\n"));
//					dprintf"SUPERHOP TILE\n"));
					wasSuperHop = 1;
					if (DotProduct(&vecUp,&joiningTile->dirVector[n])<0)
					{
//						dprintf"IS UP TILE\n"));
						if(!superHop)
						{
//							dprintf"S-HOP NEEDED!\n"));
							destTile[0] = NULL;						
							return;
						}
					}
//					dprintf"----\n"));
				}
			}

//			dprintf"Jumping To Tile\n"));
			// frog is jumping to available tile
			player[0].frogState |= FROGSTATUS_ISJUMPINGTOTILE;

			// check if a platform is in the destination tile
			destPlatform = JumpingToTileWithPlatform(longHopDestTile);
		}

		if(longHopDestTile)
		{
			distance = -1000;
			
			for(i=0; i<4; i++)
			{
				t = DotProduct(&destTile[0]->dirVector[camFacing],&longHopDestTile->dirVector[i]);
				if(t > distance)
				{
					distance = t;
					newCamFacing = i;			
				}
			}

			camFacing = newCamFacing;

			//currTile = destTile;
		}
			
		destTile[0] = longHopDestTile;

/*	longHopDestTile =  destTile->tilePtrs[tileDir];
	if ( ( longHopDestTile->state == TILESTATE_SUPERHOP) || (longHopDestTile->state == TILESTATE_JOIN))
		longHopDestTile =  longHopDestTile->tilePtrs[tileDir];

	destTile = longHopDestTile;*/
}


QUATERNION rot = {0,1,0,0.01};

void RotateFrog ( ACTOR2 *frog, unsigned long fFacing )
{
/*
	QUATERNION q1,destQ;
	float m,n,qrspd = 0.0F;
	VECTOR nmeup;
	VECTOR v1,v2,v3;
	VECTOR vfd	= { 0,0,1 };
	VECTOR vup	= { 0,1,0 };

	float x, y, z;

	if ( frogState & FROGSTATUS_ROTL )
	{
//		frog->actor->qRot.y += 0.1;
//		if ( frog->actor->qRot.y >= 1.0 )
//			frogState &= ~FROGSTATUS_ROTL;
		// ENDIF
	}
	// ENDIF
	if ( frogState & FROGSTATUS_ROTR )
	{
		//QuatSlerp(&frog->actor->qRot,&rot,0.1,&destQ);
		//SetQuaternion(&frog->actor->qRot,&destQ);
//		RotateVectorByRotation(&frog->actor->pos,&frog->actor->pos,&rot);	
		//frog->actor->qRot.y -= 0.1;
		//if ( frog->actor->qRot.y <= -1.0 )
		//	frogState &= ~FROGSTATUS_ROTR;
		// ENDIF


/*		RotateVectorByRotation(&frog->actor->pos,&frog->actor->pos,&rotn);	
		if ( 
//		frog->actor->qRot.y += 0.2;
  */
//	}
	// ENDIF
}
