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

GAMETILE *destTile[4]			= {0,0,0,0};
GAMETILE *currTile[4]			= {0,0,0,0};
GAMETILE *prevTile				= NULL;

static float frogAnimSpeed		= 0.4F;
static float frogAnimSpeed2		= 0.9F;

int	frogFacing[4]				= {0,0,0,0};
int nextFrogFacing[4]			= {0,0,0,0};

unsigned long standardHopFrames = 8;
unsigned long superHopFrames	= 32;
unsigned long doubleHopFrames	= 44;
unsigned long quickHopFrames	= 4;
unsigned long floatFrames       = 30;
unsigned long iceFrames			= 10;
unsigned long conveyorFrames[3] = { 60, 30, 15 };

unsigned long standardHopJumpDownDivisor	= 10;
unsigned long superHopJumpDownDivisor		= 12;

float superGravity		= -0.7F;
float hopGravity		= -5.0F;
float frogGravity		= -9.0F;
float doubleGravity		= -0.5F;
float floatGravity		= -1.0F;

BOOL cameoMode			= FALSE;


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
	longTongue		= 0;

	camFacing		= 0;

	InitActorAnim(frog[p]->actor);
	AnimateActor(frog[p]->actor,FROG_ANIM_DANCE1,YES,NO,0.25F,0,0);

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
	player[p].frogState			= 0;
	player[p].isSuperHopping	= 0;
	player[p].isSinking			= 0;
	player[p].isQuickHopping	= 0;
	player[p].idleTime			= MAX_IDLE_TIME;
	player[p].heightJumped		= 0;
	player[p].jumpStartFrame	= 0;
	player[p].jumpEndFrame		= 0;

	for (i=0; i<numBabies; i++)
	{
		if ( babies[i] )
		{
			if(!babyList[i].isSaved)
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
				AnimateActor(babies[i]->actor,0,YES,NO,0.5F,0,0);
			}
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function		: UpdateFroggerControls
	Parameters		: player number
	Returns			: void
	Info			:
*/
BOOL UpdateFroggerControls(long pl)
{
	/* ----------------------- Frog wants to HOP u/d/l/r ----------------------------- */

	if(player[pl].frogState & (FROGSTATUS_ISWANTINGU|FROGSTATUS_ISWANTINGL|FROGSTATUS_ISWANTINGR|FROGSTATUS_ISWANTINGD))
	{
		int dir;

		if(player[pl].frogState & FROGSTATUS_ISWANTINGU)		dir = MOVE_UP;
		else if(player[pl].frogState & FROGSTATUS_ISWANTINGD)	dir = MOVE_DOWN;
		else if(player[pl].frogState & FROGSTATUS_ISWANTINGL)	dir = MOVE_LEFT;
		else dir = MOVE_RIGHT;

		AnimateFrogHop((dir + camFacing) & 3,pl);
		frogFacing[pl] = (camFacing + dir) & 3;

		nextFrogFacing[pl] = (nextFrogFacing[pl] + ((camFacing + dir) - frogFacing[pl])) & 3;

		PlayActorBasedSample(GEN_FROG_HOP, frog[pl]->actor,255,128);

		MoveToRequestedDestination(dir,pl);
	}

  	/* ----------------------- Frog wants to SUPERHOP u/d/l/r ----------------------------- */

	else if(player[pl].frogState & (FROGSTATUS_ISWANTINGSUPERHOPU|FROGSTATUS_ISWANTINGSUPERHOPL|FROGSTATUS_ISWANTINGSUPERHOPR|FROGSTATUS_ISWANTINGSUPERHOPD))
	{
		int dir;
		if(player[pl].frogState & FROGSTATUS_ISWANTINGSUPERHOPU)		dir = MOVE_UP;
		else if(player[pl].frogState & FROGSTATUS_ISWANTINGSUPERHOPD)	dir = MOVE_DOWN;
		else if(player[pl].frogState & FROGSTATUS_ISWANTINGSUPERHOPL)	dir = MOVE_LEFT;
		else dir = MOVE_RIGHT;

		AnimateFrogHop((dir + camFacing) & 3,pl);

		player[pl].frogState |= FROGSTATUS_ISSUPERHOPPING;

		MoveToRequestedDestination(dir,pl);

		PlayActorBasedSample(GEN_SUPER_HOP, frog[pl]->actor,255,128);
	}
	else
		return FALSE;	// nope, we didn't do nuffink

	return TRUE;	// yep, we did somefink
}


float freeFall = 2.0F;

/*	--------------------------------------------------------------------------------
	Function		: UpdateFroggerPos
	Parameters		: player number
	Returns			: void
	Info			:
*/
void UpdateFroggerPos(long pl)
{
	float x,y,z;
	VECTOR effectPos;
	PLANE2 ground;
	VECTOR moveVec;
	float dist,glowRange;
	static float glowSeed = 0.0F;

	if (cameoMode)
	{
		player[pl].frogState &= 
			~(FROGSTATUS_ISWANTINGU | FROGSTATUS_ISWANTINGD |
			FROGSTATUS_ISWANTINGL | FROGSTATUS_ISWANTINGR);
		//player[pl].canJump = 0;
	}
	
	if( player[pl].isQuickHopping )
		player[pl].isQuickHopping--;

	/*	--------------------------------------------------------------------------------------------
		Consider effects of special tile types
	*/

	player[pl].frogState &= ~FROGSTATUS_ISSAFE;

	// But first... platforms
	if (currPlatform[pl])
	{
		PLATFORM *plat = currPlatform[pl];
		GAMETILE *dest = plat->inTile[0];

		if (dest != currTile[pl])
		{
			camFacing = GetTilesMatchingDirection(currTile[pl], camFacing, dest);
			frogFacing[pl] = GetTilesMatchingDirection(currTile[pl], frogFacing[pl], dest);
			currTile[pl] = dest;
		}
	}
	else if( currTile[pl]->state == TILESTATE_SINK )
	{
		if( player[pl].frogState & (FROGSTATUS_ISWANTINGU | FROGSTATUS_ISWANTINGD | FROGSTATUS_ISWANTINGL | FROGSTATUS_ISWANTINGR) )
		{
			if( player[pl].isSinking > 2 )
				player[pl].isSinking-=3;
			else
				player[pl].isSinking = 0;

			SetVector( &moveVec, &currTile[pl]->normal );
			ScaleVector( &moveVec, 0.6 );
			AddVector( &frog[pl]->actor->pos, &frog[pl]->actor->pos, &moveVec );

			dprintf"Climb little frog, climb! %d\n",player[pl].isSinking));
		}
		else if( player[pl].isSinking )
		{
			player[pl].isSinking++;
			SetVector( &moveVec, &currTile[pl]->normal );
			ScaleVector( &moveVec, -0.2 );
			AddVector( &frog[pl]->actor->pos, &frog[pl]->actor->pos, &moveVec );
			
			dprintf"Frog is SINKING! %d\n",player[pl].isSinking));
		}

		if( player[pl].isSinking > 50 )
		{
			frog[pl]->action.dead = 50;
			frog[pl]->action.healthPoints = 3;
			frog[pl]->action.deathBy = DEATHBY_DROWNING;
			player[pl].frogState |= FROGSTATUS_ISDEAD;
			player[pl].isSinking = 0;
		}
		else if( player[pl].isSinking )
		{
			AnimateActor(frog[pl]->actor,FROG_ANIM_DROWNING,NO,NO,0.25F,0,0);
			return;
		}

	}
	else if ( currTile[pl]->state == TILESTATE_SAFE )
	{
		player[pl].frogState |= FROGSTATUS_ISSAFE;
	}
	else if ( currTile[pl]->state & TILESTATE_CONVEYOR )
	{
		/*	If we're on a conveyor and can jump - i.e. we're not already jumping -
			consider the player to be on its destination tile once it goes past half-way
			along its 'jump'.
		*/
		if (destTile[pl] &&
			player[pl].canJump &&
			(actFrameCount > (player[pl].jumpEndFrame + player[pl].jumpStartFrame)/2))
		{
			currTile[pl] = destTile[pl];
		}
	}

	/*	--------------------------------------------------------------------------------------------
		Calculate frog hop
	*/

	if( actFrameCount < player[pl].jumpEndFrame )
	{
		VECTOR newPos;
		float t,s,a,hs,vs;

		// If superjumping during a double jump, start floating - if button released, stop floating
		if( player[pl].hasDoubleJumped )
		{
			if( (controllerdata[pl].button & CONT_A) && !(controllerdata[pl].lastbutton & CONT_A) )
			{
				player[pl].frogState |= FROGSTATUS_ISFLOATING;
			}
			else if( !(controllerdata[pl].button & CONT_A) && (controllerdata[pl].lastbutton & CONT_A) )
			{
				player[pl].frogState &= ~FROGSTATUS_ISFLOATING;
			}
		}

/*		if( player[pl].frogState & FROGSTATUS_ISFLOATING )
		{
			a = frogGravity*0.5;
			hs = player[pl].hInitialVelocity * 0.5;
			vs = player[pl].vInitialVelocity * 0.5;
		}
		else
		{
*/			a = frogGravity;
			hs = player[pl].hInitialVelocity;
			vs = player[pl].vInitialVelocity;
//		}

		t = actFrameCount - player[pl].jumpStartFrame;

		// calculate position considering vertical motion using s = ut + 0.5at^2
		s = (vs * t) + (0.5F * a * (t * t));
		SetVector(&player[pl].vMotionDelta,&player[pl].jumpUpVector);
		ScaleVector(&player[pl].vMotionDelta,s);

		// calculate position considering horizontal motion using s = ut + 0.5at^2
		a = 0;
		s = (hs * t) + (0.5F * a * (t * t));
		SetVector(&player[pl].hMotionDelta,&player[pl].jumpFwdVector);
		ScaleVector(&player[pl].hMotionDelta,s);

		SetVector(&newPos,&player[pl].jumpOrigin);
		AddToVector(&newPos,&player[pl].vMotionDelta);
		AddToVector(&newPos,&player[pl].hMotionDelta);
		SetVector(&frog[pl]->actor->pos,&newPos);
	}

	//--------------------------------------------------------------------------------------------
	
	if(player[pl].frogState & FROGSTATUS_ISDEAD)
	{
		KillFrog(pl);
		return;
	}
	
	// update frog tongue
	UpdateFrogTongue();

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

	// frog is croaking
	if(player[pl].frogState & FROGSTATUS_ISCROAKING)
	{
		SPECFX *fx;

		if((frog[pl]->action.isCroaking & 3) == 0)
		{
			SetVector(&effectPos,&frog[pl]->actor->pos);
			effectPos.v[Y] += 15;
			fx = CreateAndAddSpecialEffect( FXTYPE_POLYRING, &effectPos, &currTile[pl]->normal, 20, 1, 0.1, 1.5 );
			fx->r = 191;
			fx->g = 255;
			fx->b = 0;
		}

		frog[pl]->action.isCroaking--;
		if(!frog[pl]->action.isCroaking)
		{
			player[pl].frogState &= ~FROGSTATUS_ISCROAKING;

			// check for nearest baby frog - do radius check ????
			if(nearestBaby = GetNearestBabyFrog())
			{
//				fx = CreateAndAddSpecialEffect(	FXTYPE_POLYRING, &babies[nearestBaby]->actor->pos, &upVec, 15, 1, 0.1, 1.2 );
//				fx->r = babyList[nearestBaby].fxColour[R];
//				fx->g = babyList[nearestBaby].fxColour[G];
//				fx->b = babyList[nearestBaby].fxColour[B];
			}
		}
	}


	/* ---------------------------------------------------- */

	if (cameoMode || !UpdateFroggerControls(pl))
	{
		// process frog's jump
		if (player[pl].frogState & FROGSTATUS_ISJUMPINGTOPLATFORM)
			CheckForFroggerLanding(JUMPING_TOPLATFORM,pl);
		else if (player[pl].frogState & FROGSTATUS_ISJUMPINGTOTILE)
			CheckForFroggerLanding(JUMPING_TOTILE,pl);
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
			CreateAndAddSpecialEffect( FXTYPE_SMOKE_GROWS, &effectPos, &currTile[pl]->normal, 30, 5, 0, 2.5 );
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

long GetTilesMatchingDirection(GAMETILE *me, long direction, GAMETILE *next)
{
	long dirNumber,i;
	float t,distance = 0;
	
	for(i=0; i<4; i++)
	{
		t = DotProduct(&me->dirVector[direction],&next->dirVector[i]);
		if(t >= distance)
		{
			distance = t;
			dirNumber = i;			
		}
	}

	return dirNumber;
}

/*	--------------------------------------------------------------------------------
	Function		: GetNextTile(unsigned long direction)
	Purpose			: Finds the next tile in a given direction
	Parameters		: direction, player
	Returns			: GAMETILE*
*/
GAMETILE *GetNextTile(unsigned long direction,long pl)
{
	VECTOR cDir;
	unsigned long i,j,n;
	unsigned long closest[4] = { -1,-1,-1,-1 };
	float distance,t;
	unsigned long newCamFacing = camFacing;

	GAMETILE *dest = NULL;
	VECTOR vecUp;
	float t2,at2;
		
	if(pl == 0)
	{
		i = (direction + camFacing + 2) & 3;
		dest = currTile[pl]->tilePtrs[i]; // hmm...
	}
	else
	{
		/*	Alas, this only works when this player's tile and player 0's tile are orientated the same
			
			destTile[pl] = currTile[pl]->tilePtrs[(direction + camFacing + 2) & 3];

			We have to find the direction on this player's tile corresponding to the direction on
			player 0's tile!
		*/
			
		i = GetTilesMatchingDirection(currTile[0], (direction + camFacing + 2) & 3, currTile[pl]);
		dest = currTile[pl]->tilePtrs[i];
	}	

	if (!dest || dest->state == TILESTATE_BARRED)
		return NULL;

	if((dest->state == TILESTATE_SUPERHOP) || (dest->state == TILESTATE_JOIN))
	{
		GAMETILE *joiningTile = dest;

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

		dest = joiningTile->tilePtrs[n];

		if((joiningTile->state == TILESTATE_SUPERHOP))
		{
			if(DotProduct(&vecUp,&joiningTile->dirVector[n]) < 0)
			{
				if(!player[pl].isSuperHopping)
				{
					return NULL;
				}
			}
		}
	}

	// frog is jumping to available tile
	player[pl].frogState |= FROGSTATUS_ISJUMPINGTOTILE;

	return dest;
}

/*	-------------------------------------------------------------------------
	Function:	AnimateFrogHop
	Purpose:	Runs the appropriate hop animation
	Parameters:	direction, player
	Returns:
*/
void AnimateFrogHop( unsigned long direction, long pl )
{
	float speed, speed2;
	int anim;

	if(player[pl].frogState & (FROGSTATUS_ISWANTINGSUPERHOPU|FROGSTATUS_ISWANTINGSUPERHOPL|FROGSTATUS_ISWANTINGSUPERHOPR|FROGSTATUS_ISWANTINGSUPERHOPD))
	{
		anim = FROG_ANIM_SUPERHOP;
		speed = 0.35;
		speed2 = 0.35;
	}
	else
	{
		anim = FROG_ANIM_STDJUMP;
		speed = frogAnimSpeed;
		speed2 = frogAnimSpeed2;
	}

	switch( direction )
	{
	case 0:
		switch( frogFacing[pl] )
		{
		case 3:	AnimateActor(frog[pl]->actor,anim,NO,NO,speed2,0,0); break;
		case 1:	AnimateActor(frog[pl]->actor,anim,NO,NO,speed2,0,0); break;
		default: AnimateActor(frog[pl]->actor,anim,NO,NO,speed,0,0); break;
		}
		break;
	case 1:
		switch( frogFacing[pl] )
		{
		case 2:	AnimateActor(frog[pl]->actor,anim,NO,NO,speed2,0,0); break;
		case 0:	AnimateActor(frog[pl]->actor,anim,NO,NO,speed2,0,0); break;
		default: AnimateActor(frog[pl]->actor,anim,NO,NO,speed,0,0); break;
		}
		break;
	case 2:
		switch( frogFacing[pl] )
		{
		case 1:	AnimateActor(frog[pl]->actor,anim,NO,NO,speed2,0,0); break;
		case 3:	AnimateActor(frog[pl]->actor,anim,NO,NO,speed2,0,0); break;
		default: AnimateActor(frog[pl]->actor,anim,NO,NO,speed,0,0); break;
		}
		break;
	case 3:
		switch( frogFacing[pl] )
		{
		case 2:	AnimateActor(frog[pl]->actor,anim,NO,NO,speed2,0,0); break;
		case 0:	AnimateActor(frog[pl]->actor,anim,NO,NO,speed2,0,0); break;
		default: AnimateActor(frog[pl]->actor,anim,NO,NO,speed,0,0); break;
		}
		break;
	}
	AnimateActor(frog[pl]->actor,FROG_ANIM_BREATHE,YES,YES,0.6F,0,0);
}

/*	--------------------------------------------------------------------------------
	Function		: MoveToRequestedDestination
	Purpose			: Moves a player in the requested destination
	Parameters		: direction, player
	Returns			: TRUE if we did a hop, FALSE otherwise
*/
long speedFrameCount = 0;

BOOL MoveToRequestedDestination(int dir,long pl)
{
	GAMETILE *dest, *from;
	float t,t2,sV=NOINIT_VELOCITY;
	
	// clear movement request flags
	player[pl].frogState &=	~(	FROGSTATUS_ISWANTINGU | FROGSTATUS_ISWANTINGD | 
								FROGSTATUS_ISWANTINGL | FROGSTATUS_ISWANTINGR);

	if(tongueState & TONGUE_BEINGUSED)
		RemoveFrogTongue();

	// see if frog is currently on a moving platform (i.e. platform that follows a path)
	if(currPlatform[pl] = GetPlatformFrogIsOn(pl))
	{
		// frog is on a platform - get current tile
		currTile[pl] = currPlatform[pl]->inTile[0];
		currPlatform[pl]->carrying = frog[pl];

		// check if the platform follows a path
		if(currPlatform[pl]->flags & PLATFORM_NEW_FOLLOWPATH)
			player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
	}

	if (frog[pl]->action.frogunder==-1)
	{
		dest = GetNextTile(dir, pl);
	}

	// clear all 'wanting movement' flags
	player[pl].frogState &= ~FROGSTATUS_ALLHOPFLAGS;

	if (!dest)
	{
		player[pl].canJump = 1;
		player[pl].isSuperHopping = 0;
		
		return FALSE;
	}

	// check if a platform is in the destination tile
	destPlatform[pl] = JumpingToTileWithPlatform(dest,pl);

	// ---------------------------------------------------------
	
	/*
	
	  TODO: Jumping on other frogs could do with re-working slightly to use
	  player.canJump

	if(dest)
	{
		int i;

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
	*/

	if(currPlatform[pl])
	{
		currPlatform[pl]->flags &= ~PLATFORM_NEW_CARRYINGFROG;
		currPlatform[pl]->carrying = NULL;
	}

	// ------------------ Check for valid jumps -----------------------

	if(destPlatform[pl])
	{
		// check if platform is too high to reach
		if(PlatformTooHigh(destPlatform[pl],pl))
		{
			// platform too high (smoked too many funny fags ?)
			dprintf"Platform TOO HIGH\n"));

			player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOPLATFORM;

			if(destPlatform[pl]->flags & PLATFORM_NEW_NOWALKUNDER)
			{
				// cannot fall to tile 'under' platform as cannot walk under this platform
				player[pl].canJump = 1;
				player[pl].isSuperHopping = 0;

				player[pl].frogState &= ~( FROGSTATUS_ISJUMPINGTOTILE | FROGSTATUS_ISJUMPINGTOPLATFORM | FROGSTATUS_ISSUPERHOPPING );

				// if frog was on platform, place back on the platform
				if(currPlatform[pl])
				{
					currPlatform[pl]->flags |= PLATFORM_NEW_CARRYINGFROG;
					currPlatform[pl]->carrying = frog[pl];
				}

				destPlatform[pl] = NULL;
				return FALSE;
			}

			destPlatform[pl] = NULL;

			player[pl].frogState |= FROGSTATUS_ISJUMPINGTOTILE;
		}
	}
	
	if (!destPlatform[pl])
	{
		if(GameTileTooHigh(dest, pl))
		{			
			// gametile is too high
			player[pl].canJump = 1;
			player[pl].isSuperHopping = 0;

			player[pl].frogState &= ~( FROGSTATUS_ISJUMPINGTOTILE | FROGSTATUS_ISJUMPINGTOPLATFORM | FROGSTATUS_ISSUPERHOPPING );

			// if frog was on platform, place back on the platform
			if(currPlatform[pl])
			{
				currPlatform[pl]->flags |= PLATFORM_NEW_CARRYINGFROG;
				currPlatform[pl]->carrying = frog[pl];
			}
			
			return FALSE;
		}
		else if(dest->state == TILESTATE_SMASH)
		{
			// smash tiles

			if(!(player[pl].frogState & FROGSTATUS_ISSUPERHOPPING))
			{
				dprintf"Must superhop to smash\n"));

				player[pl].canJump = 1;
				player[pl].isSuperHopping = 0;

				player[pl].frogState &= ~( FROGSTATUS_ISJUMPINGTOTILE | FROGSTATUS_ISJUMPINGTOPLATFORM | FROGSTATUS_ISSUPERHOPPING );

				// if frog was on platform, place back on the platform
				if(currPlatform[pl])
				{
					currPlatform[pl]->flags |= PLATFORM_NEW_CARRYINGFROG;
					currPlatform[pl]->carrying = frog[pl];
				}

				return FALSE;
			}
			else
			{
				dest->state = TILESTATE_NORMAL;
				
				player[pl].canJump = 1;
				player[pl].isSuperHopping = 0;

				player[pl].frogState &= ~( FROGSTATUS_ISJUMPINGTOTILE | FROGSTATUS_ISJUMPINGTOPLATFORM | FROGSTATUS_ISSUPERHOPPING );

				// if frog was on platform, place back on the platform
				if(currPlatform[pl])
				{
					currPlatform[pl]->flags |= PLATFORM_NEW_CARRYINGFROG;
					currPlatform[pl]->carrying = frog[pl];
				}

				return FALSE;
			}
		}
	}

	// ------------------------------------------------------------------------------------------
	// If we get this far, it's a valid jump! Celebrations! Fizzy pop and lime jelly all round.

	destTile[pl] = dest;

	if (currPlatform[pl])
	{
		from = currPlatform[pl]->inTile[0];
		currPlatform[pl] = NULL;
	}
	else
	{
		from = currTile[pl];
	}

	nextCamFacing = GetTilesMatchingDirection(from, camFacing, dest);
	nextFrogFacing[pl] = GetTilesMatchingDirection(from, frogFacing[pl], dest);

	if( player[pl].hasDoubleJumped )
	{
		float time = (float)(actFrameCount - player[pl].jumpStartFrame)/(float)(player[pl].jumpEndFrame-player[pl].jumpStartFrame),
			v = player[pl].vInitialVelocity;
		t = doubleHopFrames;
		t2 = doubleGravity;
		// Current velocity is initial velocity of superhop + (acceleration * time)
		sV = v + (superGravity*time);
	}
	else if(player[pl].isSuperHopping)
	{
		t = superHopFrames;
		t2 = superGravity;
	}
	else if(player[pl].isQuickHopping)
	{
		t = quickHopFrames;
		t2 = hopGravity;
	}
	else
	{
		t = standardHopFrames;
		t2 = hopGravity;
	}

	// determine where frog is going to land...

	if(player[pl].frogState & FROGSTATUS_ISJUMPINGTOTILE)
	{	
		CalculateFrogJump(
			&frog[pl]->actor->pos, &currTile[pl]->normal,
			&destTile[pl]->centre, &destTile[pl]->normal,
			t,pl,t2,sV);
	}
	else if(player[pl].frogState & FROGSTATUS_ISJUMPINGTOPLATFORM)
	{
		CalculateFrogJump(
			&frog[pl]->actor->pos, &currTile[pl]->normal,
			&destPlatform[pl]->pltActor->actor->pos, &destPlatform[pl]->inTile[0]->normal,
			t,pl,t2,NOINIT_VELOCITY);
	}

	// ------------------------------------------------------------------------------------------

	numHops_TOTAL++;
	if(speedFrameCount)
		speedHops_TOTAL += frameCount - speedFrameCount;	
	speedFrameCount = frameCount;

	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function		: CheckForFroggerLanding
	Purpose			: determine whether the player has finished its jump
	Parameters		: int
	Returns			: void
	Info			:
*/
void CheckForFroggerLanding(int whereTo,long pl)
{
	VECTOR telePos;
	unsigned long i;
	float distance;

	if (actFrameCount < player[pl].jumpEndFrame) return;

	// Frog has landed - set camera to new rotation, face frog correctly, blahblahblah

	frogFacing[pl] = nextFrogFacing[pl];
	
	if (pl == 0)
		camFacing = nextCamFacing;

	frog[pl]->action.deathBy = -1;
	frog[pl]->action.dead	 = 0;

	player[pl].canJump = 1;
	player[pl].isSuperHopping = 0;
	player[pl].hasDoubleJumped = 0;
	if( frogTrail[pl] && frogTrail[pl]->follow )
	{
		frogTrail[pl]->follow = NULL;
		frogTrail[pl] = NULL;
	}

	if(whereTo == JUMPING_TOPLATFORM)
	{
		// ok - frog has landed
		SetVector(&frog[pl]->actor->pos,&destPlatform[pl]->pltActor->actor->pos);

		if(pl == 0)
			camFacing = GetTilesMatchingDirection(currTile[pl],camFacing,destPlatform[pl]->inTile[0]);

		destPlatform[pl]->flags		|= PLATFORM_NEW_CARRYINGFROG;
		player[pl].frogState		|= FROGSTATUS_ISONMOVINGPLATFORM;

		destPlatform[pl]->carrying	= frog[pl];
		
		frog[pl]->actor->scale.v[X] = globalFrogScale;	//0.09F;
		frog[pl]->actor->scale.v[Y] = globalFrogScale;	//0.09F;
		frog[pl]->actor->scale.v[Z] = globalFrogScale;	//0.09F;

		player[pl].frogState &= ~(	FROGSTATUS_ISJUMPINGTOTILE | FROGSTATUS_ISFLOATING |
									FROGSTATUS_ISJUMPINGTOPLATFORM | FROGSTATUS_ISSUPERHOPPING);

		currPlatform[pl] = destPlatform[pl];

		if(player[pl].heightJumped < -125.0F)
		{
			if(!frog[pl]->action.dead)
			{
				CreateAndAddSpecialEffect( FXTYPE_BASICRING, &destTile[pl]->centre, &destTile[pl]->normal, 25, 1, 0.1, 0.8 );
				frog[pl]->action.deathBy = DEATHBY_NORMAL;
				AnimateActor(frog[pl]->actor,FROG_ANIM_BASICSPLAT,NO,NO,0.25F,0,0);

				player[pl].frogState |= FROGSTATUS_ISDEAD;
				frog[pl]->action.dead = 50;

				PlayActorBasedSample(2,frog[pl]->actor,255,128);
			}
			return;
		}

		CheckTileForCollectable(NULL,0);
	}
	else
	{
		GAMETILE *tile;
		int state;
		
		if (!destTile[pl])
			tile = currTile[pl];
		else
		{
			currTile[pl] = tile = destTile[pl];
			destTile[pl] = NULL;

			// set frog to centre of tile
			SetVector(&frog[pl]->actor->pos, &tile->centre);
		}

		state = tile->state;

		frog[pl]->actor->scale.v[X] = globalFrogScale;	//0.09F;
		frog[pl]->actor->scale.v[Y] = globalFrogScale;	//0.09F;
		frog[pl]->actor->scale.v[Z] = globalFrogScale;	//0.09F;

		player[pl].frogState &= ~(FROGSTATUS_ISJUMPINGTOTILE | FROGSTATUS_ISJUMPINGTOPLATFORM |
			FROGSTATUS_ISONMOVINGPLATFORM | FROGSTATUS_ISSUPERHOPPING | FROGSTATUS_ISFLOATING);


		// check tile to see if frog has jumped onto a certain tile type
		if((state == TILESTATE_DEADLY) || (player[pl].heightJumped < -125.0F))
		{
			if(!frog[pl]->action.dead)
			{
				if(state == TILESTATE_DEADLY)
				{
					SPECFX *fx;
					fx = CreateAndAddSpecialEffect( FXTYPE_SPLASH, &tile->centre, &tile->normal, 10, 1, 0, 2 );
					fx->gravity = 0.05;
					fx = CreateAndAddSpecialEffect( FXTYPE_SPLASH, &tile->centre, &tile->normal, 20, 1, 0, 2 );
					fx->gravity = 0.05;

					CreateAndAddSpecialEffect( FXTYPE_WATERRIPPLE, &tile->centre, &tile->normal, 20, 0.8, 0.1, 0.6 );
					frog[pl]->action.deathBy = DEATHBY_DROWNING;
					AnimateActor(frog[pl]->actor,FROG_ANIM_DROWNING,NO,NO,0.25F,0,0);
				}
				else
				{
					CreateAndAddSpecialEffect( FXTYPE_BASICRING, &tile->centre, &tile->normal, 25, 1, 0.1, 1 );
					frog[pl]->action.deathBy = DEATHBY_NORMAL;
					AnimateActor(frog[pl]->actor,FROG_ANIM_BASICSPLAT,NO,NO,0.25F,0,0);
				}

				player[pl].frogState |= FROGSTATUS_ISDEAD;
				frog[pl]->action.dead = 50;

				PlayActorBasedSample(2,frog[pl]->actor,255,128);
			}
			return;
		}
		else if(state == TILESTATE_SINK)
		{
			player[pl].isSinking = 10;
		}
		if (state & TILESTATE_CONVEYOR)
		{	
			// -------------------------------- Conveyors ----------------------------

			int res = MoveToRequestedDestination(
				((state & (TILESTATE_CONVEYOR-1)) - camFacing) & 3, pl);

			if (res)
			{
				int speed;

				if (state > TILESTATE_CONVEYOR_FAST)
					speed = 2;
				else if (state > TILESTATE_CONVEYOR_MED)
					speed = 1;
				else
					speed = 0;

				if (player[pl].frogState & FROGSTATUS_ISJUMPINGTOPLATFORM)
					CalculateFrogJump(
						&frog[pl]->actor->pos, &tile->normal,
						&destPlatform[pl]->pltActor->actor->pos, &destPlatform[pl]->inTile[0]->normal,
						conveyorFrames[speed], pl, 0.0, NOINIT_VELOCITY);
				else
					CalculateFrogJump(
						&frog[pl]->actor->pos, &tile->normal, 
						&destTile[pl]->centre, &destTile[pl]->normal,
						conveyorFrames[speed], pl, 0.0, NOINIT_VELOCITY);
			}
		}
		else if (state == TILESTATE_ICE)
		{
			int res = MoveToRequestedDestination((nextFrogFacing[pl] - camFacing) & 3, pl);

			if (res)
			{
				if (player[pl].frogState & FROGSTATUS_ISJUMPINGTOPLATFORM)
					CalculateFrogJump(
						&frog[pl]->actor->pos, &tile->normal,
						&destPlatform[pl]->pltActor->actor->pos, &destPlatform[pl]->inTile[0]->normal,
						iceFrames, pl, 0.0, NOINIT_VELOCITY);
				else
					CalculateFrogJump(
						&frog[pl]->actor->pos, &tile->normal, 
						&destTile[pl]->centre, &destTile[pl]->normal,
						iceFrames, pl, 0.0, NOINIT_VELOCITY);

				player[pl].canJump = FALSE;
			}
		}
		// Check for camera transitions on the tile
		CheckForDynamicCameraChange(tile);

		// Next, check if frog has landed on a collectable
		CheckTileForCollectable(tile, pl);
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

	// tile can be jumped to
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
	TEXTURE *txtr1,*txtr2;
	float modifier;
	int rVal;
	
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

		if(frog[pl]->action.deathBy == DEATHBY_ELECTRICSHOCK)
		{
			// reset frog texture !
#ifndef PC_VERSION
			FindTexture(&txtr1,UpdateCRC("fg.bmp"),NO);
			FindTexture(&txtr2,UpdateCRC("ai_wake.bmp"),NO);
			ReplaceTextureInDrawList(frog[pl]->actor->objectController->drawList,(u32)txtr2->data,(u32)txtr1->data,NO);
#endif
		}

		SetFroggerStartPos(gTStart[0],pl);
		ResetPlatformFlags();

		return FALSE;
	}

	switch(frog[pl]->action.deathBy)
	{
		case DEATHBY_NORMAL:
			// throw some stars about
			if(!(actFrameCount & 31))
			{
				CreateAndAddSpecialEffect( FXTYPE_SPLASH, &currTile[pl]->centre, &currTile[pl]->normal, 6, 15, 0, 2 );
			}
			break;

		case DEATHBY_RUNOVER:
			break;

		case DEATHBY_DROWNING:
			// create some ripples round the drowing frog
//			if(!(actFrameCount & 31))
//				CreateAndAddSpecialEffect( FXTYPE_WATERRIPPLE, &destTile[pl]->centre, &destTile[pl]->normal, 15, 1, 0.1, 2.5 );
			break;

		case DEATHBY_SQUASHED:
			break;

		case DEATHBY_EXPLOSION:
			break;

		case DEATHBY_ELECTRICSHOCK:
			frog[pl]->actor->pos.v[X] += (-0.5F + Random(2));
			frog[pl]->actor->pos.v[Y] += (-0.5F + Random(2));
			frog[pl]->actor->pos.v[Z] += (-0.5F + Random(2));

#ifndef PC_VERSION

			FindTexture(&txtr1,UpdateCRC("fg.bmp"),NO);
			FindTexture(&txtr2,UpdateCRC("ai_wake.bmp"),NO);

			rVal = Random(10);
			if(rVal > 5)
				ReplaceTextureInDrawList(frog[pl]->actor->objectController->drawList,(u32)txtr1->data,(u32)txtr2->data,NO);
			else
				ReplaceTextureInDrawList(frog[pl]->actor->objectController->drawList,(u32)txtr2->data,(u32)txtr1->data,NO);
#endif
			break;

		case DEATHBY_CHOCOLATE:
			break;
	}

	return TRUE;
}


/*	--------------------------------------------------------------------------------
	Function		: HopFrogToTile
	Purpose			: Hop the frog to a tile
	Parameters		: 
	Returns			: void
*/

void HopFrogToTile(GAMETILE *tile, long pl)
{
	VECTOR v;
	float maxdot = 0.0, dot;
	int j, dir = 0;

	player[pl].frogState |= FROGSTATUS_ISJUMPINGTOTILE;
	player[pl].canJump = 0;
	destTile[pl] = tile;

	SubVector(&v, &currTile[pl]->centre, &tile->centre);

	for(j=0; j<4; j++)
	{	
		dot = DotProduct(&currTile[pl]->dirVector[j], &v);
		if(dot > maxdot)
		{
			maxdot = dot;
			dir = j; //(j - camFacing + 2) & 3;
		}
	}

	AnimateFrogHop(dir, pl);	
	nextFrogFacing[pl] = (dir + 2) & 3;

	CalculateFrogJump(
		&frog[pl]->actor->pos, &currTile[pl]->normal,
		&destTile[pl]->centre, &destTile[pl]->normal,
		standardHopFrames, pl, hopGravity, NOINIT_VELOCITY);
}

/*	--------------------------------------------------------------------------------
	Function		: PushFrog
	Purpose			: Push the frog in a given direction
	Parameters		: VECTOR, VECTOR, long
	Returns			: void
*/
void PushFrog(VECTOR *where, VECTOR *direction, long pl)
{
	VECTOR v;
	GAMETILE *dest;
	float maxdot = 0.0, dot;
	int j, dir = 0;

	// Find nearest matching direction vector
	for(j=0; j<4; j++)
	{	
		dot = DotProduct(&currTile[pl]->dirVector[j], direction);
		if(dot > maxdot)
		{
			maxdot = dot;
			dir = j; //(j - camFacing + 2) & 3;
		}
	}

	if (!MoveToRequestedDestination((dir - camFacing) & 3, pl)) return;
	//dest = currTile[pl]->tilePtrs[dir];

	if (player[pl].frogState & FROGSTATUS_ISJUMPINGTOPLATFORM)
		CalculateFrogJump(
			&frog[pl]->actor->pos, &currTile[pl]->normal,
			&destPlatform[pl]->pltActor->actor->pos, &destPlatform[pl]->inTile[0]->normal,
			standardHopFrames, pl, 0.0, NOINIT_VELOCITY);
	else
		CalculateFrogJump(
			&frog[pl]->actor->pos, &currTile[pl]->normal,
			&destTile[pl]->centre, &destTile[pl]->normal,
			standardHopFrames, pl, 0.0, NOINIT_VELOCITY);

	currTile[pl] = destTile[pl];
}

/*	--------------------------------------------------------------------------------
	Function		: CalculateFrogJump
	Purpose			: calculates values to describe frog jump movement
	Parameters		: VECTOR *,VECTOR *,float,long
	Returns			: void
	Info			: 
*/

void CalculateFrogJump(VECTOR *startPos,VECTOR *startNormal,VECTOR *endPos,VECTOR *endNormal,float t,long pl,float gravity, float initVelocity)
{
	float sH,sV,a;

	// STAGE 1 - frog's movement - new stuff - AndyE

	// get the up vector and the jump forward vector (up vector is average of curr and dest normals)
	SetVector(&player[pl].jumpUpVector,startNormal);
	AddToVector(&player[pl].jumpUpVector,endNormal);
	ScaleVector(&player[pl].jumpUpVector,0.5F);

	SubVector(&player[pl].jumpFwdVector,endPos,startPos);
	sH = Magnitude(&player[pl].jumpFwdVector);
	MakeUnit(&player[pl].jumpFwdVector);
	sV = (sH * DotProduct(&player[pl].jumpFwdVector,&player[pl].jumpUpVector));

	// STAGE 2 - considering vertical motion

	// modify t and a depending on sV - i.e. jumping up or down
	frogGravity = gravity;

	if (!player[pl].hasDoubleJumped)
	{
		if(player[pl].isSuperHopping)
		{
			if(sV < -40)
			{
				// alter t to modify jump heights
				t += (Fabs(sV) / superHopJumpDownDivisor);

				if(sV < -125)
				{
					// frog has fallen too far !!!
					t += 15;
					frogGravity = -0.75F;
					AnimateActor(frog[pl]->actor,FROG_ANIM_TRYTOFLY,NO,NO,0.75F,0,0);
				}
			}
		}
		else
		{
			if(sV < -10)
			{
				// alter t to modify jump heights
				t += (Fabs(sV) / standardHopJumpDownDivisor);

				if(sV < -125)
				{
					// frog has fallen too far !!!
					t += 15;
					frogGravity = -0.75F;
					AnimateActor(frog[pl]->actor,FROG_ANIM_TRYTOFLY,NO,NO,0.75F,0,0);
				}
			}
		}
	}

	// using s = ut + 0.5at^2
	a = frogGravity;

	if (initVelocity > NOINIT_VELOCITY)
		player[pl].vInitialVelocity = initVelocity;
	else
		player[pl].vInitialVelocity = ((-a * t * t) + (2 * sV)) / (2 * t);

	// STAGE 3 - considering horizontal motion
	a = 0;
	player[pl].hInitialVelocity = ((-a * t * t) + (2 * sH)) / (2 * t);

	// set variables to process frog's jump
	player[pl].jumpStartFrame = actFrameCount;
	player[pl].jumpEndFrame	= actFrameCount + (unsigned long)t;

	// get frogs origin of jump and height jumped between source and dest
	SetVector(&player[pl].jumpOrigin,&frog[pl]->actor->pos);
	player[pl].heightJumped = sV;
}
