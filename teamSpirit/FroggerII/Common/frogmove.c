/*
	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: frogmove.c
	Programmer	: Andrew Eder
	Date		: 11/13/98

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


long numHops_TOTAL = 0;
long speedHops_TOTAL = 0;
long numHealth_TOTAL = 0;
long nextCamFacing = 0;

GAMETILE *destTile[4]		= {0,0,0,0};
GAMETILE *longHopDestTile	= NULL;
GAMETILE *currTile[4]		= {0,0,0,0};
GAMETILE *prevTile			= NULL;

float landRadius			= 20.0F;
static float frogAnimSpeed	= 1.0F;

int	frogFacing[4]				= {0,0,0,0};
int nextFrogFacing[4]			= {0,0,0,0};

long  isLong	= 0;
float longSpeed = 0;
float longAmt	= 0;

float changeLongSpeed	= -10;
float startLongSpeed	= 25.0f;

float standardHopHeight = 8.0F;
float standardHopSpeed	= 10.0F;
float extendedHopHeight = 24.0F;
float extendedHopSpeed	= 4.0F;

float frogGravity		= -4.0F;


/*	--------------------------------------------------------------------------------
	Function		: SetFroggerStartPos
	Purpose			: sets initial position of Frogger on the world
	Parameters		: GAMETILE *,long
	Returns			: none
	Info			:
*/
void SetFroggerStartPos(GAMETILE *startTile,long p)
{
	unsigned long i;
	VECTOR tmpVec;

	// Change frog's position to reflect that of the start tile
	frog[p]->actor->pos.v[X] = startTile->centre.v[X];
	frog[p]->actor->pos.v[Y] = startTile->centre.v[Y];
	frog[p]->actor->pos.v[Z] = startTile->centre.v[Z];

	autoHop			= 0;
	longTongue		= 0;

	camFacing		= 0;
	
	isLong		= 0;
	longSpeed	= 0;
	longAmt		= 0;

	changeLongSpeed	= -10;
	startLongSpeed	= 25.0f;

	InitActorAnim(frog[p]->actor);
	AnimateActor(frog[p]->actor,FROG_ANIM_DANCE1,YES,NO,0.5F,0,0);

	currTile[p]		= startTile;

	destTile[p]		= NULL;
	currPlatform[p]	= NULL;
	destPlatform[p]	= NULL;

	frog[p]->action.healthPoints	= 3;
	frog[p]->action.isCroaking		= 0;
	frog[p]->action.isOnFire		= 0;

	// set frog action movement variables
	player[p].canJump			= 1;
	player[p].isSuperHopping	= 0;
	player[p].isLongHopping		= 0;

	for (i=0; i<numBabies; i++)
	{
		if ( babies[i] )
		{
			if(!babies[i]->action.isSaved)
			{
				if ( bTStart[i] )
				{
					babies[i]->actor->pos.v[X] = bTStart[i]->centre.v[X];
					babies[i]->actor->pos.v[Y] = bTStart[i]->centre.v[Y];
					babies[i]->actor->pos.v[Z] = bTStart[i]->centre.v[Z];
					NormalToQuaternion(&babies[i]->actor->qRot,&bTStart[i]->normal);
				}
				else
				{
					babies[i]->actor->pos.v[X] = 0;
					babies[i]->actor->pos.v[Y] = 0;
					babies[i]->actor->pos.v[Z] = 0;
					tmpVec.v[X] = 0;
					tmpVec.v[Y] = 0;
					tmpVec.v[Z] = 0;
					NormalToQuaternion(&babies[i]->actor->qRot,&tmpVec);
				}

				InitActorAnim(babies[i]->actor);
				AnimateActor(babies[i]->actor,0,YES,NO,1.0F,0,0);
			}
		}
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
	PLANE2 ground;
	float dist;
	
	if(player[pl].frogState & FROGSTATUS_ISDEAD)
	{
		KillFrog(pl);
		return;
	}
/*
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
*/
	
	// update frog tongue
	UpdateFrogTongue();

	if(player[pl].frogState & FROGSTATUS_ISFALLINGTOGROUND)
	{
		// the frog is falling to the ground
		SetVector(&ground.point,&currTile[pl]->centre);
		SetVector(&ground.normal,&currTile[pl]->normal);

		// update frog position in relation to ground
//		frog[pl]->actor->vel.v[X] *= 0.95F;
		frog[pl]->actor->vel.v[Y] += frogGravity;
//		frog[pl]->actor->vel.v[Z] *= 0.95F;

		AddToVector(&frog[pl]->actor->pos,&frog[pl]->actor->vel);
		ground.J = -DotProduct(&ground.point,&ground.normal);
		dist = -(DotProduct(&frog[pl]->actor->pos,&ground.normal) + ground.J);						

		// check if frog has hit (or passed through) the ground plane
		if(dist > 0)
		{
			SetVector(&frog[pl]->actor->pos,&ground.point);
			CalcBounce(&frog[pl]->actor->vel,&ground.normal);
//			frog[pl]->actor->vel.v[X] *= 0.75F;
			frog[pl]->actor->vel.v[Y] *= 0.75F;
//			frog[pl]->actor->vel.v[Z] *= 0.75F;

			CreateAndAddFXSmoke(&ground.point,128,15);

			if(MagnitudeSquared(&frog[pl]->actor->vel) < 5.0F)
			{
				// stop the frog from bouncing - set to standing
				SetVector(&frog[pl]->actor->pos,&ground.point);
				player[pl].frogState &= ~FROGSTATUS_ISFALLINGTOGROUND;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGU;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGD;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGL;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGR;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPU;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPD;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPL;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPR;
			}
		}

		return;
	}
	else if(player[pl].frogState & FROGSTATUS_ISFALLINGTOPLATFORM)
	{
		// the frog is falling to a platform below
		SetVector(&ground.point,&destPlatform[pl]->pltActor->actor->pos);
		SetVector(&ground.normal,&destPlatform[pl]->inTile->normal);

		// update frog position in relation to ground
//		frog[pl]->actor->vel.v[X] *= 0.95F;
		frog[pl]->actor->vel.v[Y] += frogGravity;
//		frog[pl]->actor->vel.v[Z] *= 0.95F;

		AddToVector(&frog[pl]->actor->pos,&frog[pl]->actor->vel);
		ground.J = -DotProduct(&ground.point,&ground.normal);
		dist = -(DotProduct(&frog[pl]->actor->pos,&ground.normal) + ground.J);						

		// check if frog has hit (or passed through) the platform
		if(dist > 0)
		{
			SetVector(&frog[pl]->actor->pos,&ground.point);
			CalcBounce(&frog[pl]->actor->vel,&ground.normal);
//			frog[pl]->actor->vel.v[X] *= 0.75F;
			frog[pl]->actor->vel.v[Y] *= 0.75F;
//			frog[pl]->actor->vel.v[Z] *= 0.75F;

			CreateAndAddFXSmoke(&ground.point,128,15);

			if(MagnitudeSquared(&frog[pl]->actor->vel) < 5.0F)
			{
				// stop the frog from bouncing - set to standing on the platform
				currPlatform[pl] = destPlatform[pl];
				currPlatform[pl]->flags |= PLATFORM_NEW_CARRYINGFROG;
				currPlatform[pl]->carrying = frog[pl];

				SetVector(&frog[pl]->actor->pos,&ground.point);
				player[pl].frogState &= ~FROGSTATUS_ISFALLINGTOPLATFORM;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGU;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGD;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGL;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGR;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPU;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPD;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPL;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPR;
			}
		}

		return;
	}

	if(player[pl].frogState & FROGSTATUS_ISSTANDING)
	{
	}

	if(player[pl].frogState & FROGSTATUS_ISONMOVINGPLATFORM)
	{
	}

	if(player[pl].frogState & FROGSTATUS_ISFLOATING)
	{
		if ( ( destPlatform[pl] ) && ( destPlatform[pl]->inTile != currTile[pl] ) )
		{
			player[pl].frogState |= FROGSTATUS_ISJUMPINGTOTILE;
			player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOPLATFORM;
		}
	}

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
				rip->r = nearestBaby->action.fxColour[R];
				rip->g = nearestBaby->action.fxColour[G];
				rip->b = nearestBaby->action.fxColour[B];
			}
		}
	}

	/* ----------------------- Frog wants to HOP u/d/l/r ----------------------------- */

	if(player[pl].frogState & (FROGSTATUS_ISWANTINGU|FROGSTATUS_ISWANTINGL|FROGSTATUS_ISWANTINGR|FROGSTATUS_ISWANTINGD))
	{
		int dir;
		if(player[pl].frogState & FROGSTATUS_ISWANTINGU)		dir = MOVE_UP;
		else if(player[pl].frogState & FROGSTATUS_ISWANTINGD)	dir = MOVE_DOWN;
		else if(player[pl].frogState & FROGSTATUS_ISWANTINGL)	dir = MOVE_LEFT;
		else dir = MOVE_RIGHT;

		if(player[pl].frogState & FROGSTATUS_ISFLOATING) prevTile = currTile[pl];
		
		if(!MoveToRequestedDestination(dir,pl))
		{
			if(!(player[pl].frogState & FROGSTATUS_ISFLOATING))
			{
				if(currPlatform[pl])
					player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
			}
			else
			{
				destTile[pl] = prevTile;
			}
			
			player[pl].canJump = 1;

			player[pl].frogState &=
				~(FROGSTATUS_ISWANTINGU|FROGSTATUS_ISWANTINGL|
				FROGSTATUS_ISWANTINGR|FROGSTATUS_ISWANTINGD);
		}
		nextFrogFacing[pl] = frogFacing[pl] = (camFacing + dir) & 3;
//		PlaySample ( GEN_FROG_HOP, 0, 0, 0 );
	}

	/* ----------------------- Frog wants to SUPERHOP u/d/l/r ----------------------------- */

	if(player[pl].frogState & (FROGSTATUS_ISWANTINGSUPERHOPU|FROGSTATUS_ISWANTINGSUPERHOPL|FROGSTATUS_ISWANTINGSUPERHOPR|FROGSTATUS_ISWANTINGSUPERHOPD))
	{
		int dir;
		if(player[pl].frogState & FROGSTATUS_ISWANTINGSUPERHOPU)		dir = MOVE_UP;
		else if(player[pl].frogState & FROGSTATUS_ISWANTINGSUPERHOPD)	dir = MOVE_DOWN;
		else if(player[pl].frogState & FROGSTATUS_ISWANTINGSUPERHOPL)	dir = MOVE_LEFT;
		else dir = MOVE_RIGHT;

		if(player[pl].frogState & FROGSTATUS_ISFLOATING) prevTile = currTile[pl];

		nextFrogFacing[pl] = nextCamFacing;
		player[pl].frogState |= FROGSTATUS_ISSUPERHOPPING;

		if(!MoveToRequestedDestination(dir,pl))
		{
			if(!(player[pl].frogState & FROGSTATUS_ISFLOATING))
			{
				if(currPlatform[pl])
					player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
			}
			else
			{
				destTile[pl] = prevTile;
			}
			
			player[pl].frogState &=
				~(FROGSTATUS_ISWANTINGSUPERHOPU|FROGSTATUS_ISWANTINGSUPERHOPL|
				FROGSTATUS_ISWANTINGSUPERHOPR|FROGSTATUS_ISWANTINGSUPERHOPD);
		}
		nextFrogFacing[pl] = frogFacing[pl] = (camFacing + dir) & 3;
//		PlaySample ( GEN_FROG_HOP, 0, 0, 0 );
	}

	/* ----------------------- Frog wants to LONG HOP u/d/l/r ----------------------------- */

	if(player[pl].frogState & (FROGSTATUS_ISWANTINGLONGHOPU|FROGSTATUS_ISWANTINGLONGHOPL|FROGSTATUS_ISWANTINGLONGHOPR|FROGSTATUS_ISWANTINGLONGHOPD))
	{
		int dir;
		if(player[pl].frogState & FROGSTATUS_ISWANTINGLONGHOPU)		dir = MOVE_UP;
		else if(player[pl].frogState & FROGSTATUS_ISWANTINGLONGHOPD)	dir = MOVE_DOWN;
		else if(player[pl].frogState & FROGSTATUS_ISWANTINGLONGHOPL)	dir = MOVE_LEFT;
		else dir = MOVE_RIGHT;

		nextFrogFacing[pl] = nextCamFacing;
		player[pl].frogState |= FROGSTATUS_ISLONGHOPPING;
		
		if(!MoveToRequestedDestination(dir,pl))
		{
			if(currPlatform[pl])
				player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
			
			player[pl].frogState &=
				~(FROGSTATUS_ISWANTINGLONGHOPU|FROGSTATUS_ISWANTINGLONGHOPL|
				FROGSTATUS_ISWANTINGLONGHOPR|FROGSTATUS_ISWANTINGLONGHOPD);
		}
		nextFrogFacing[pl] = frogFacing[pl] = (camFacing + dir) & 3;
//		PlaySample ( GEN_FROG_HOP, 0, 0, 0 );
	}

	/* ---------------------------------------------------- */

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
			CreateAndAddFXSmoke(&effectPos,128,30);
		}
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
	unsigned long closest[4] = { -1,-1,-1,-1 };
	float distance,t;
	unsigned long newCamFacing = camFacing;

	GAMETILE *joiningTile = NULL;
	VECTOR vecUp,moveVec;
	float t2,at2;
	
	if(pl == 0)
		destTile[pl] = currTile[pl]->tilePtrs[(direction + camFacing + 2) & 3]; // hmm...
	else
	{
		distance = 0;
			
		for(j=0; j<4; j++)
		{	
			t = DotProduct(&(currTile[pl]->dirVector[(direction + camFacing + 2) & 3]),
						   &(currTile[pl]->dirVector[j]));
			if(t > distance)
			{
				distance = t;
				destTile[pl] = currTile[pl]->tilePtrs[j];					
			}
		}
	}	

	if(destTile[pl])
	{
		joiningTile = destTile[pl];

		if((destTile[pl]->state == TILESTATE_SUPERHOP) || (destTile[pl]->state == TILESTATE_JOIN))
		{
			SetVector(&vecUp,&currTile[pl]->normal);

			distance = -1000;
			
			for(i=0; i<4; i++)
			{
				if(joiningTile->tilePtrs[i])
				{					
					t2 = DotProduct(&vecUp,&joiningTile->dirVector[i]);
					t = Fabs(t2);
					if(t > distance)
					{
						if(currTile[pl] != joiningTile->tilePtrs[i])
						{
							distance = t;
							n = i;
							at2 = t2;
						}
					}
				}
			}

			destTile[pl] = joiningTile->tilePtrs[n];

			if((joiningTile->state == TILESTATE_SUPERHOP))
			{
				if(DotProduct(&vecUp,&joiningTile->dirVector[n]) < 0)
				{
					if(!player[pl].isSuperHopping)
					{
						destTile[pl] = NULL;						
						return;
					}
				}
			}
		}

		// frog is jumping to available tile
		player[pl].frogState |= FROGSTATUS_ISJUMPINGTOTILE;

		// check if a platform is in the destination tile
		destPlatform[pl] = JumpingToTileWithPlatform(destTile[pl],pl);
	}

	if(destTile[pl])
	{
		distance = -1000;
		
		for(i=0; i<4; i++)
		{
			t = DotProduct(&currTile[pl]->dirVector[camFacing],&destTile[pl]->dirVector[i]);
			if(t > distance)
			{
				distance = t;
				newCamFacing = i;			
			}
		}

		nextCamFacing = newCamFacing;

		// determine frog's jump velocity
		SubVector(&moveVec,&destTile[pl]->centre,&frog[pl]->actor->pos);
		MakeUnit(&moveVec);

		// check if frog is doing extended hop (i.e. superhop or longhop)
		// (and set relevant animation ranges - may change)
		// (anim 0 = frog jump animation)
		if(player[pl].isSuperHopping)
		{
			// extended hop
			speedTest = extendedHopSpeed;
			t = extendedHopHeight;
			frog[pl]->actor->animation->anims->animStart	= 2;
			frog[pl]->actor->animation->anims->animEnd		= 14;
			frogAnimSpeed = 1.0F;
		}
		else
		{
			// normal hop
			speedTest = standardHopSpeed;
			t = standardHopHeight;
			frog[pl]->actor->animation->anims->animStart	= 6;
			frog[pl]->actor->animation->anims->animEnd		= 11;
			frogAnimSpeed = 1.5F;
		}

		// play frog animation
		AnimateActor(frog[pl]->actor,FROG_ANIM_STDJUMP,NO,NO,frogAnimSpeed,0,0);
		AnimateActor(frog[pl]->actor,FROG_ANIM_BREATHE,YES,YES,0.5F,0,0);

		// set frog's jump velocity
		player[pl].jumpVelocity.v[X] = (speedTest * moveVec.v[X]);
		player[pl].jumpVelocity.v[Y] = (speedTest * moveVec.v[Y]) + t;
		player[pl].jumpVelocity.v[Z] = (speedTest * moveVec.v[Z]);
	}

	if((destTile[pl]) && (player[pl].frogState & FROGSTATUS_ISFLOATING))
		currTile[pl] = destTile[pl];
}


/*	--------------------------------------------------------------------------------
	Function		: MoveToRequestedDestination
	Purpose			: determines if we can move to the requested destination
	Parameters		: int
	Returns			: TRUE if valid destination, else FALSE
	Info			:
*/
long speedFrameCount = 0;

BOOL MoveToRequestedDestination(int dir,long pl)
{
	char moveLocal = 0;
	unsigned long dirFlag = 0;

	// clear movement request flags
	player[pl].frogState &= ~FROGSTATUS_ISWANTINGU;
	player[pl].frogState &= ~FROGSTATUS_ISWANTINGD;
	player[pl].frogState &= ~FROGSTATUS_ISWANTINGL;
	player[pl].frogState &= ~FROGSTATUS_ISWANTINGR;

	if(tongueState & TONGUE_BEINGUSED)
		RemoveFrogTongue();

	// see if frog is currently on a moving platform (i.e. platform that follows a path)
	if(currPlatform[pl] = GetPlatformFrogIsOn(pl))
	{
		currTile[pl] = currPlatform[pl]->inTile;
		currPlatform[pl]->carrying = frog[pl];

		// check if frog is on a platform that follows a path
		if(currPlatform[pl]->flags & PLATFORM_NEW_FOLLOWPATH)
			player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
	}

	// Select the requested destination tile based on current position and direction
	if(!moveLocal)
	{
		int i;
		if(player[pl].frogState & FROGSTATUS_ISLONGHOPPING)
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
			if (frog[pl]->action.frogunder==-1)
			{
				switch(dir)
				{
					case MOVE_UP:
						dirFlag = FROGSTATUS_ISWANTINGU;
						GetNextTile(0,pl);
						break;

					case MOVE_LEFT:
						dirFlag = FROGSTATUS_ISWANTINGL;
						GetNextTile(1,pl);
						break;

					case MOVE_DOWN:
						dirFlag = FROGSTATUS_ISWANTINGD;
						GetNextTile(2,pl);
						break;

					case MOVE_RIGHT:
						dirFlag = FROGSTATUS_ISWANTINGR;
						GetNextTile(3,pl);
						break;
				}		
		
				if(destTile[pl] != NULL)
				{
					if (frog[pl]->action.frogon!=-1)
					{
						frog[frog[pl]->action.frogon]->action.frogunder = -1;
						frog[pl]->action.frogon = -1;
					}
	
					for (i=0; i<NUM_FROGS; i++)
					{
						if (currTile[i] == destTile[pl])
						{
							if (frog[i]->action.frogunder==-1)
							{
								frog[i]->action.frogunder = pl;
								frog[pl]->action.frogon = i;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOTILE;
		player[pl].frogState |= FROGSTATUS_ISJUMPINGTOPLATFORM;
	}

	// check if jumping to a valid tile
	if(destTile[pl])
	{
		if(currPlatform[pl])
		{
			currPlatform[pl]->flags &= ~PLATFORM_NEW_CARRYINGFROG;
			currPlatform[pl]->carrying = NULL;
		}

		if(destPlatform[pl])
		{
			// check if platform is too high to reach
			if(PlatformTooHigh(destPlatform[pl],pl))
			{
				// platform too high
				dprintf"Platform TOO HIGH\n"));

				player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOPLATFORM;

				if(destPlatform[pl]->flags & PLATFORM_NEW_NOWALKUNDER)
				{
					// platform cannot be walked under
					player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOTILE;
					player[pl].frogState &= ~FROGSTATUS_ISSUPERHOPPING;
					destPlatform[pl] = NULL;
					return FALSE;
				}

				destPlatform[pl] = NULL;
				player[pl].frogState |= FROGSTATUS_ISJUMPINGTOTILE;
			}
			else if(PlatformTooLow(destPlatform[pl],pl))
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
		
		if ( !destPlatform[pl] )
		{
			if( GameTileTooHigh ( destTile[pl],pl ) )
			{			
					// tile too high
					dprintf"GameTile TOO HIGH\n"));

					destTile[pl] = NULL;
					destPlatform[pl] = NULL;
					
					player[pl].isSuperHopping = 0;

					player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOTILE;
					player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOPLATFORM;
					player[pl].frogState &= ~FROGSTATUS_ISSUPERHOPPING;
					player[pl].frogState &= ~FROGSTATUS_ISLONGHOPPING;
				
					return FALSE;			
			}
			else if ( GameTileTooLow ( destTile[pl],pl ) && ( !( player[pl].frogState & FROGSTATUS_ISFLOATING ) ) )
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
			else if( destTile[pl]->state == TILESTATE_BARRED )
			{
				// Can't get into this tile at the moment
				dprintf"GameTile BARRED\n"));

				destTile[pl] = NULL;
				destPlatform[pl] = NULL;
				
				player[pl].isSuperHopping = 0;

				player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOTILE;
				player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOPLATFORM;
				player[pl].frogState &= ~FROGSTATUS_ISSUPERHOPPING;
				player[pl].frogState &= ~FROGSTATUS_ISLONGHOPPING;
			}
			else if( destTile[pl]->state == TILESTATE_SMASH )
			{
				if( !(player[pl].frogState & FROGSTATUS_ISSUPERHOPPING) )
				{
					dprintf"Must superhop to smash\n"));

					destTile[pl] = NULL;
					destPlatform[pl] = NULL;
					
					player[pl].isSuperHopping = 0;

					player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOTILE;
					player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOPLATFORM;
					player[pl].frogState &= ~FROGSTATUS_ISSUPERHOPPING;
					player[pl].frogState &= ~FROGSTATUS_ISLONGHOPPING;

					return FALSE;
				}
				else
				{
					destTile[pl]->state = TILESTATE_NORMAL;
					destTile[pl] = currTile[pl];
					destPlatform[pl] = NULL;
					
					player[pl].isSuperHopping = 0;

					player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOTILE; // Replace with smash action
					player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOPLATFORM;
					player[pl].frogState &= ~FROGSTATUS_ISSUPERHOPPING;
					player[pl].frogState &= ~FROGSTATUS_ISLONGHOPPING;

					return FALSE;
				}
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

	numHops_TOTAL++;
	if(speedFrameCount)
		speedHops_TOTAL += frameCount - speedFrameCount;	
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
//		SubVector(&fwd,&destPlatform[pl]->pltActor->actor->pos,&frog[pl]->actor->pos);
//		MakeUnit(&fwd);
//		frog[pl]->actor->pos.v[X] += (fwd.v[X] * speedTest);
//		frog[pl]->actor->pos.v[Y] += (fwd.v[Y] * speedTest);
//		frog[pl]->actor->pos.v[Z] += (fwd.v[Z] * speedTest);

		// update frog's position and velocity
		player[pl].jumpVelocity.v[Y] += frogGravity;
		AddToVector(&frog[pl]->actor->pos,&player[pl].jumpVelocity);
	}
	else
	{
		if(player[pl].frogState & FROGSTATUS_ISFLOATING)
		{
//			SubVector(&fwd,&destTile[pl]->centre,&frog[pl]->actor->pos);
//			MakeUnit(&fwd);
//			frog[pl]->actor->pos.v[X] += (fwd.v[X] * 55);
//			frog[pl]->actor->pos.v[Y] += (fwd.v[Y] * speedTest);
//			frog[pl]->actor->pos.v[Z] += (fwd.v[Z] * 55);
			
			// update frog's position and velocity
			player[pl].jumpVelocity.v[Y] += frogGravity;
			AddToVector(&frog[pl]->actor->pos,&player[pl].jumpVelocity);

			if(frog[pl]->actor->animation->animTime > 10)
				frog[pl]->actor->animation->animTime = 10;

			CheckTileForCollectable(destTile[pl],pl);
		}
		else if ( player[pl].frogState & FROGSTATUS_ISSPRINGING )
		{
			frog[pl]->actor->pos.v[Y] += /*(fwd.v[Y] * */speedTest/*)*/;
			speedTest -= 1.0f;
			if ( speedTest < 2.0f )
			{
				speedTest = 2.0f;
				croakFloat = 90;
				camSpeed = 9;
				player[pl].frogState &= ~FROGSTATUS_ISSPRINGING;
				player[pl].frogState |= FROGSTATUS_ISFLOATING;

			}

			if (frog[pl])
				if (frog[pl]->actor->animation->animTime > 10)
					frog[pl]->actor->animation->animTime = 10;
			CheckTileForCollectable(destTile[pl],pl);
		}
		else
		{
			// update frog's position and velocity
			player[pl].jumpVelocity.v[Y] += frogGravity;
			AddToVector(&frog[pl]->actor->pos,&player[pl].jumpVelocity);
		}
	}
	
	if (isLong)
	{
		longAmt += longSpeed;
		longSpeed += changeLongSpeed;
		if (frog[pl])
			if (frog[pl]->actor->animation->animTime > 10)
				frog[pl]->actor->animation->animTime = 10;

		if(whereTo == JUMPING_TOPLATFORM)
			frog[pl]->actor->pos.v[Y] = destPlatform[pl]->pltActor->actor->pos.v[Y] + longAmt;
		else
		{
			frog[pl]->actor->pos.v[Y] = destTile[pl]->centre.v[Y] + longAmt;
//			dprintf"\n\n%f - %f - %f\n\n", frog->actor->pos.v[Y], destTile->centre.v[Y], jumpAmt));
		}

		frogScale = longAmt * 0.001F;
		frog[pl]->actor->scale.v[X] = globalFrogScale + frogScale;
		frog[pl]->actor->scale.v[Y] = globalFrogScale + frogScale;
		frog[pl]->actor->scale.v[Z] = globalFrogScale + frogScale;
	}
	
	for (i=0; i<numBabies; i++)
	{
		if ( babies [i] )
		{
			if(!(babies[i]->action.isSaved))
			{
				if ( bTStart[i] )
				{
					babies[i]->actor->pos.v[X] = bTStart[i]->centre.v[X];
					babies[i]->actor->pos.v[Y] = bTStart[i]->centre.v[Y];
					babies[i]->actor->pos.v[Z] = bTStart[i]->centre.v[Z];
				}
				else
				{
					babies[i]->actor->pos.v[X] = 0;
					babies[i]->actor->pos.v[Y] = 0;
					babies[i]->actor->pos.v[Z] = 0;
				}
				// ENDIF
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
		// ENDIF
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
	static PLANE2 tilePlane;
	
	if(whereTo == JUMPING_TOPLATFORM)
	{
		// get plane description from destination platform
		SetVector(&tilePlane.point,&destPlatform[pl]->pltActor->actor->pos);
		SetVector(&tilePlane.normal,&destPlatform[pl]->inTile->normal);

		tilePlane.J = -DotProduct(&tilePlane.point,&tilePlane.normal);
		distance = -(DotProduct(&frog[pl]->actor->pos,&tilePlane.normal) + tilePlane.J);						

//		distance = DistanceBetweenPointsSquared(&frog[pl]->actor->pos,&destPlatform[pl]->pltActor->actor->pos);
//		if(distance < (landRadius * landRadius))
		if((distance > 0) && (player[pl].jumpVelocity.v[Y] < 0))
		{
			// Okay - Frogger has landed - snap him to the centre of the platform
			SetVector(&frog[pl]->actor->pos,&destPlatform[pl]->pltActor->actor->pos);
			
			player[pl].canJump = 1;
			player[pl].isSuperHopping = 0;

			if(player[pl].frogState & FROGSTATUS_ISDEAD)
			{
				cameraShake = 35;
				AnimateActor(frog[pl]->actor,FROG_ANIM_FWDSOMERSAULT,NO,NO,0.367,0,0);
				frog[pl]->action.deathBy = DEATHBY_NORMAL;
				frog[pl]->action.dead = 35;
			}
			else
			{
				frog[pl]->action.deathBy = -1;
			}

			destPlatform[pl]->flags		|= PLATFORM_NEW_CARRYINGFROG;
			player[pl].frogState		|= FROGSTATUS_ISONMOVINGPLATFORM;
			destPlatform[pl]->carrying	= frog[pl];
			
			player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOTILE;
			player[pl].frogState &= ~FROGSTATUS_ISFLOATING;
			player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOPLATFORM;
			player[pl].frogState &= ~FROGSTATUS_ISSUPERHOPPING;
			currPlatform[pl] = destPlatform[pl];

			frog[pl]->actor->scale.v[X] = globalFrogScale;	//0.09F;
			frog[pl]->actor->scale.v[Y] = globalFrogScale;	//0.09F;
			frog[pl]->actor->scale.v[Z] = globalFrogScale;	//0.09F;

			CheckTileForCollectable(NULL,0);
		}
	}
	else
	{
		// get plane description from destination tile
		SetVector(&tilePlane.point,&destTile[pl]->centre);
		SetVector(&tilePlane.normal,&destTile[pl]->normal);

		tilePlane.J = -DotProduct(&tilePlane.point,&tilePlane.normal);
		distance = -(DotProduct(&frog[pl]->actor->pos,&tilePlane.normal) + tilePlane.J);						
		
//		if(distance > 0)
		if((distance > 0) && (player[pl].jumpVelocity.v[Y] < 0))
		{
			// set frog to centre of tile
			SetVector(&frog[pl]->actor->pos,&destTile[pl]->centre);

			if ( destTile[pl]->state == TILESTATE_SPRING )
			{
				player[pl].frogState |= FROGSTATUS_ISSPRINGING;
//				frogState &= ~FROGSTATUS_ISJUMPINGTOTILE;
				player[pl].frogState &= ~(FROGSTATUS_ISJUMPINGTOPLATFORM | FROGSTATUS_ISONMOVINGPLATFORM |
						FROGSTATUS_ISSUPERHOPPING | FROGSTATUS_ISLONGHOPPING | FROGSTATUS_ISFLOATING);
				croakFloat = 0;
				camSpeed   = 1;
				frog[pl]->action.deathBy = DEATHBY_FALLINGTOTILE;
				dprintf"SPRING TILE\n"));
			}
			else
			{
				isLong = 0;

				frog[pl]->action.deathBy = -1;
				frog[pl]->action.dead	 = 0;

				player[pl].canJump = 1;
				player[pl].isSuperHopping = 0;

				frog[pl]->actor->scale.v[X] = globalFrogScale;	//0.09F;
				frog[pl]->actor->scale.v[Y] = globalFrogScale;	//0.09F;
				frog[pl]->actor->scale.v[Z] = globalFrogScale;	//0.09F;

				player[pl].frogState &= ~(FROGSTATUS_ISJUMPINGTOTILE | FROGSTATUS_ISJUMPINGTOPLATFORM |
					FROGSTATUS_ISONMOVINGPLATFORM |	FROGSTATUS_ISSUPERHOPPING | FROGSTATUS_ISLONGHOPPING |
					FROGSTATUS_ISFLOATING);

				// check tile to see if frog has jumped onto a certain tile type
				if(destTile[pl]->state == TILESTATE_DEADLY)
				{
					if(!frog[pl]->action.dead)
					{
						CreateAndAddFXRipple(RIPPLE_TYPE_WATER,&destTile[pl]->centre,&upVec,25,1,0.1,30);

						frog[pl]->action.deathBy = DEATHBY_DROWNING;
						player[pl].frogState |= FROGSTATUS_ISDEAD;
						frog[pl]->action.dead = 50;
						AnimateActor(frog[pl]->actor,FROG_ANIM_DROWNING,NO,NO,0.5F,0,0);
						PlaySample(2,NULL,255,128);
					}
					return;
				}

				// Next, check if frog has landed on a collectable
				CheckTileForCollectable(destTile[pl],pl);

				if (pl == 0)
					camFacing = nextCamFacing;

				frogFacing[pl] = nextFrogFacing[pl];

				// check if the tile is a teleport tile
/*				if(IsATeleportTile(destTile[pl]))
				{
					// frog is teleporting
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
*/
				// Check for camera transitions on the tile
				CheckForDynamicCameraChange(currTile[pl]);

				// update and get the new current tile
				currTile[pl] = destTile[pl];
			}
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
BOOL GameTileTooHigh(GAMETILE *tile,long pl)
{
	VECTOR vec;
	float height,h;
	VECTOR diff;
		
	SubVector(&diff,&tile->centre,&frog[pl]->actor->pos);
	h = Magnitude(&diff);
	MakeUnit(&diff);
	height = (h * DotProduct(&diff,&tile->normal));

	if(height > 51.0F)
	{
		// cannot be jumped up to either with or without superhop
		return TRUE;
	}

	if(height > 25.0F && !player[pl].isSuperHopping)
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
BOOL GameTileTooLow(GAMETILE *tile,long pl)
{
	VECTOR vec;
	float height,h;
	VECTOR diff;
		
	SubVector(&diff,&tile->centre,&frog[pl]->actor->pos);
	h = Magnitude(&diff);
	MakeUnit(&diff);
	height = (h * DotProduct(&diff,&tile->normal));

	if(height < -125.0F)
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
	Parameters		: long
	Returns			: BOOL - true if running death 'sequence'
	Info			: 
*/
BOOL KillFrog(long pl)
{
	frog[pl]->action.dead--;
	if(!frog[pl]->action.dead || frog[pl]->action.deathBy == DEATHBY_INSTANT)
	{
		numHealth_TOTAL++;
		// lose a life
		player[pl].lives--;
		if(!player[pl].lives)
		{
			gameIsOver = 250;
			return FALSE;
		}

		player[pl].frogState &= ~FROGSTATUS_ISDEAD;
		SetFroggerStartPos(gTStart[0],pl);
		ResetPlatformFlags();

		return FALSE;
	}

	switch(frog[pl]->action.deathBy)
	{
		case DEATHBY_NORMAL:
			break;

		case DEATHBY_RUNOVER:
			break;

		case DEATHBY_DROWNING:
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
					t = Fabs(t2);
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
/*
			if ((joiningTile->state == TILESTATE_SUPERHOP))
			{
				if (DotProduct(&vecUp,&joiningTile->dirVector[n])<0)
				{
					if(!superHop)
					{
						destTile[0] = NULL;						
						return;
					}
				}
			}
*/
		}

		// frog is jumping to available tile
		player[0].frogState |= FROGSTATUS_ISJUMPINGTOTILE;

		// check if a platform is in the destination tile
		destPlatform[0] = JumpingToTileWithPlatform(destTile[0],0);
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

		if(longHopDestTile)
		{
			joiningTile = longHopDestTile;

			if ((longHopDestTile->state == TILESTATE_SUPERHOP) || (longHopDestTile->state == TILESTATE_JOIN))
			{
				SetVector(&vecUp,&destTile[0]->normal);

				distance = -1000;
				
				for(i=0; i<4; i++)
				{
					if(joiningTile->tilePtrs[i])
					{					
						t2 = DotProduct(&vecUp,&joiningTile->dirVector[i]);
						t = Fabs(t2);
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
				
				if ((joiningTile->state == TILESTATE_SUPERHOP))
				{
					dprintf"----\n"));
					if (DotProduct(&vecUp,&joiningTile->dirVector[n])<0)
					{
/*
						if(!superHop)
						{
							destTile[0] = NULL;						
							return;
						}
*/
					}
				}
			}

			// frog is jumping to available tile
			player[0].frogState |= FROGSTATUS_ISJUMPINGTOTILE;

			// check if a platform is in the destination tile
			destPlatform[0] = JumpingToTileWithPlatform(longHopDestTile,0);
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
