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

float standardHopHeight = 6.0F;
float standardHopSpeed	= 10.0F;
float superHopHeight	= 24.0F;
float superHopSpeed		= 4.0F;
float longHopHeight		= 24.0F;
float longHopSpeed		= 9.0F;

float frogGravity		= -4.0F;
float gravityModifier	= 1.0F;


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
	SetVector(&frog[p]->actor->pos,&startTile->centre);

	autoHop			= 0;
	longHop			= 0;
	longTongue		= 0;

	camFacing		= 0;

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
	ZeroVector(&frog[p]->actor->vel);

	player[p].canJump			= 1;
	player[p].isSuperHopping	= 0;
	player[p].isLongHopping		= 0;

	player[p].isCroakFloating	= 0;

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

/*

void SpringFrog( EVENT *event )
{
	int fNum = (int)event->data[0],
		tNum = (int)event->data[1],
		time = (int)event->data[2];

	float t, ht = (int)event->data[3] / (float)0x10000;

	TRIGGER *trigger = (TRIGGER *)event->data[4];

	static int start = 0, end = 0;

	GAMETILE *tile = GetTileFromNumber(tNum);
	static VECTOR D, H, sPos;
	VECTOR dd, dh;

	if( !end )
	{
		start = actFrameCount;
		end = start + ((time / (float)0x10000) * 60 );

		// Calculate frog position from height, tile positions and actFrameCount.
		SubVector( &D, &tile->centre, &currTile[fNum]->centre );
		AddVector( &H, &tile->normal, &currTile[fNum]->normal );
		MakeUnit( &H );
		ScaleVector( &H, ht );

		SetVector( &sPos, &frog[fNum]->actor->pos );
	}

	t = (float)(actFrameCount-start) / (float)(end-start);

	SetVector( &dh, &H );
	ScaleVector( &dh, 1.0 - (((2.0 * t) - 1.0) * ((2.0 * t) - 1.0)) );
	SetVector( &dd, &D );
	ScaleVector( &dd, t );

	AddVector( &frog[fNum]->actor->pos, &sPos, &dd );
	AddVector( &frog[fNum]->actor->pos, &frog[fNum]->actor->pos, &dh );

	// TODO: Slurp frog orientation between source and destination tiles

	// Check if position is close enough to destination tile. If it's landed, set currTile and cancel this event
	if( DistanceBetweenPoints(&tile->centre,&frog[fNum]->actor->pos) < 5 )
	{
		currTile[fNum] = tile;
		SetVector( &frog[fNum]->actor->pos, &tile->centre );
		player[fNum].frogState &= ~FROGSTATUS_ISTELEPORTING;
		player[fNum].frogState |= FROGSTATUS_ISSTANDING;

		CreateAndAddFXSmoke(SMOKE_TYPE_NORMAL,&frog[fNum]->actor->pos,128,0,0.5,15);

		start = 0;
		end = 0;

		// Delete this trigger/event pair
		SubTrigger( trigger );
		JallocFree( trigger );
	}
}

*/

void UpdateFroggerPos(long pl)
{
	float x,y,z;
	FX_RIPPLE *rip;
	VECTOR effectPos;
	PLANE2 ground;
	float dist;

	//--------------------------------------------------------------------------------------------

	// firstly, update frog's velocity (e.g. affected by gravity ?)
	if(!(player[pl].canJump) && !(player[pl].frogState & (FROGSTATUS_ISWANTINGU | FROGSTATUS_ISWANTINGD | FROGSTATUS_ISWANTINGL | FROGSTATUS_ISWANTINGR)))
	{
		// frog is jumping - affected by gravity
		frog[pl]->actor->vel.v[X] += ((frogGravity * gravityModifier) * currTile[pl]->normal.v[X]);
		frog[pl]->actor->vel.v[Y] += ((frogGravity * gravityModifier) * currTile[pl]->normal.v[Y]);
		frog[pl]->actor->vel.v[Z] += ((frogGravity * gravityModifier) * currTile[pl]->normal.v[Z]);
	}
	else
	{
		// frog is not moving - zero velocity
		ZeroVector(&frog[pl]->actor->vel);
	}

	// update frog's position
	AddToVector(&frog[pl]->actor->pos,&frog[pl]->actor->vel);

	//--------------------------------------------------------------------------------------------
	
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
/*
	if(player[pl].frogState & FROGSTATUS_ISFALLINGTOGROUND)
	{
	}
	else if(player[pl].frogState & FROGSTATUS_ISFALLINGTOPLATFORM)
	{
	}
*/

	if(player[pl].frogState & FROGSTATUS_ISFREEFALLING)
	{
		dprintf"FROGSTATUS_ISFREEFALLING\n"));

		// frog is free-falling under gravity
		if(destPlatform[pl])
			CheckForFroggerLanding(JUMPING_TOPLATFORM,pl);
		else
			CheckForFroggerLanding(JUMPING_TOTILE,pl);

		return;
	}

	if(player[pl].frogState & FROGSTATUS_ISFLOATING)
	{
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
			if(currPlatform[pl])
				player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;

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
			if(currPlatform[pl])
				player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;

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
		if(player[pl].frogState & FROGSTATUS_ISWANTINGLONGHOPU)			dir = MOVE_UP;
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
//		SlurpFroggerPosition(JUMPING_TOTILE,pl);
		CheckForFroggerLanding(JUMPING_TOTILE,pl);
	}
	else if(player[pl].frogState & FROGSTATUS_ISJUMPINGTOPLATFORM)
	{
		// frog is jumping between a game tile and a platform
//		SlurpFroggerPosition(JUMPING_TOPLATFORM,pl);
		CheckForFroggerLanding(JUMPING_TOPLATFORM,pl);
	}

	/* ---------------------------------------------------- */

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
			// extended hop - superhop
			speedTest = superHopSpeed;
			t = superHopHeight;
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
		frog[pl]->actor->vel.v[X] = (speedTest * moveVec.v[X]) + (t * currTile[pl]->normal.v[X]);
		frog[pl]->actor->vel.v[Y] = (speedTest * moveVec.v[Y]) + (t * currTile[pl]->normal.v[Y]);
		frog[pl]->actor->vel.v[Z] = (speedTest * moveVec.v[Z]) + (t * currTile[pl]->normal.v[Z]);
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
//		if(player[pl].frogState & FROGSTATUS_ISLONGHOPPING)
		if(player[pl].isLongHopping)
		{
			switch(dir)
			{
				case MOVE_UP:
					GetNextTileLongHop(0,pl);
					break;
				case MOVE_LEFT:
					GetNextTileLongHop(1,pl);
					break;
				case MOVE_DOWN:
					GetNextTileLongHop(2,pl);
					break;
				case MOVE_RIGHT:
					GetNextTileLongHop(3,pl);
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
					player[pl].isLongHopping = 0;

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
				player[pl].isLongHopping = 0;

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
					player[pl].isLongHopping = 0;

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
					player[pl].isLongHopping = 0;

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

		player[pl].canJump = 1;
		player[pl].isSuperHopping = 0;
		player[pl].isLongHopping = 0;
		
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
	VECTOR babyup;
	VECTOR v1,v2,v3;
	unsigned long i;
		
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
			RotateVectorByQuaternion(&babyup,&upVec,&babies[i]->actor->qRot);
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
		if((distance > 0) && (frog[pl]->actor->vel.v[Y] < 0))
		{
			// Okay - Frogger has landed - snap him to the centre of the platform
			SetVector(&frog[pl]->actor->pos,&destPlatform[pl]->pltActor->actor->pos);
			
			player[pl].canJump = 1;
			player[pl].isSuperHopping = 0;
			player[pl].isLongHopping = 0;

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
		if((distance > 0) && (frog[pl]->actor->vel.v[Y] < 0))
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
				frog[pl]->action.deathBy = -1;
				frog[pl]->action.dead	 = 0;

				player[pl].canJump = 1;
				player[pl].isSuperHopping = 0;
				player[pl].isLongHopping = 0;

				frog[pl]->actor->scale.v[X] = globalFrogScale;	//0.09F;
				frog[pl]->actor->scale.v[Y] = globalFrogScale;	//0.09F;
				frog[pl]->actor->scale.v[Z] = globalFrogScale;	//0.09F;

				player[pl].frogState &= ~(FROGSTATUS_ISJUMPINGTOTILE | FROGSTATUS_ISJUMPINGTOPLATFORM |
					FROGSTATUS_ISONMOVINGPLATFORM | FROGSTATUS_ISSUPERHOPPING | FROGSTATUS_ISLONGHOPPING |
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

	dprintf"Height: %f\n",height));

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


/*	--------------------------------------------------------------------------------
	Function		: GetNextTileLongHop
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void GetNextTileLongHop(unsigned long direction,long pl)
{
	VECTOR cDir;
	unsigned long i,j,n;
	unsigned long closest[4] = { -1,-1,-1,-1 };
	float distance,t;
	unsigned long newCamFacing = camFacing;

	GAMETILE *joiningTile = NULL;
	VECTOR vecUp,moveVec;
	float t2,at2;

	GAMETILE *longHopDest = NULL;
	PLATFORM *longHopPlat = NULL;

	dprintf"LONGHOPPING...\n"));
	
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

		dprintf"  LHOP: destTile (stage 1) found\n"));

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
					if(!player[pl].isLongHopping)
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
	}

	if(destTile[pl])
	{
		longHopDest = destTile[pl]->tilePtrs[(direction + camFacing + 2) & 3]; // hmm...

		if(!longHopDest)
		{
			// no destination tile - frog is effectively just superhopping

			// determine frog's jump velocity
			SubVector(&moveVec,&destTile[pl]->centre,&frog[pl]->actor->pos);
			MakeUnit(&moveVec);

			// extended hop - superhop
			speedTest = superHopSpeed;
			t = superHopHeight;
			frog[pl]->actor->animation->anims->animStart	= 2;
			frog[pl]->actor->animation->anims->animEnd		= 14;
			frogAnimSpeed = 1.0F;

			player[pl].isLongHopping = 0;
			player[pl].isSuperHopping = 1;

			// play frog animation
			AnimateActor(frog[pl]->actor,FROG_ANIM_STDJUMP,NO,NO,frogAnimSpeed,0,0);
			AnimateActor(frog[pl]->actor,FROG_ANIM_BREATHE,YES,YES,0.5F,0,0);

			// set frog's jump velocity
			frog[pl]->actor->vel.v[X] = (speedTest * moveVec.v[X]) + (t * currTile[pl]->normal.v[X]);
			frog[pl]->actor->vel.v[Y] = (speedTest * moveVec.v[Y]) + (t * currTile[pl]->normal.v[Y]);
			frog[pl]->actor->vel.v[Z] = (speedTest * moveVec.v[Z]) + (t * currTile[pl]->normal.v[Z]);

			return;
		}

		joiningTile = longHopDest;

		dprintf"  LHOP: longHopDest (stage 2) found\n"));

		if((longHopDest->state == TILESTATE_SUPERHOP) || (longHopDest->state == TILESTATE_JOIN))
		{
			SetVector(&vecUp,&destTile[pl]->normal);

			distance = -1000;
			
			for(i=0; i<4; i++)
			{
				if(joiningTile->tilePtrs[i])
				{					
					t2 = DotProduct(&vecUp,&joiningTile->dirVector[i]);
					t = Fabs(t2);
					if(t > distance)
					{
						if(destTile[pl] != joiningTile->tilePtrs[i])
						{
							distance = t;
							n = i;
							at2 = t2;
						}
					}
				}
			}

			longHopDest = joiningTile->tilePtrs[n];

			if((joiningTile->state == TILESTATE_SUPERHOP))
			{
				if(DotProduct(&vecUp,&joiningTile->dirVector[n]) < 0)
				{
					if(!player[pl].isLongHopping)
					{
						longHopDest = NULL;						
						return;
					}
				}
			}
		}

		// frog is jumping to available tile
		player[pl].frogState |= FROGSTATUS_ISJUMPINGTOTILE;

		// check if a platform is in the destination tile
		longHopPlat = JumpingToTileWithPlatform(longHopDest,pl);

		// ---------------------------------------------------------------------------------------

		distance = -1000;
		
		for(i=0; i<4; i++)
		{
			t = DotProduct(&destTile[pl]->dirVector[camFacing],&longHopDest->dirVector[i]);
			if(t > distance)
			{
				distance = t;
				newCamFacing = i;			
			}
		}

		nextCamFacing = newCamFacing;
		
		// determine frog's jump velocity
		SubVector(&moveVec,&longHopDest->centre,&frog[pl]->actor->pos);
		MakeUnit(&moveVec);

		// check if frog is doing extended hop (i.e. longhop)
		// (and set relevant animation ranges - may change)
		// (anim 0 = frog jump animation)

		speedTest = longHopSpeed;
		t = longHopHeight;
		frog[pl]->actor->animation->anims->animStart	= 2;
		frog[pl]->actor->animation->anims->animEnd		= 14;
		frogAnimSpeed = 1.0F;

		// play frog animation
		AnimateActor(frog[pl]->actor,FROG_ANIM_STDJUMP,NO,NO,frogAnimSpeed,0,0);
		AnimateActor(frog[pl]->actor,FROG_ANIM_BREATHE,YES,YES,0.5F,0,0);

		// set frog's jump velocity
		frog[pl]->actor->vel.v[X] = (speedTest * moveVec.v[X]) + (t * currTile[pl]->normal.v[X]);
		frog[pl]->actor->vel.v[Y] = (speedTest * moveVec.v[Y]) + (t * currTile[pl]->normal.v[Y]);
		frog[pl]->actor->vel.v[Z] = (speedTest * moveVec.v[Z]) + (t * currTile[pl]->normal.v[Z]);

		destTile[pl] = longHopDest;
		destPlatform[pl] = longHopPlat;
	}
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
