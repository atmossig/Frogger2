/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: platform.c
	Programmer	: Andrew Eder
	Date		: 1/21/99

	NOTES FOR PLATFORM USAGE: (or the platform bible according to Sharky) (_|_) <- ass

	Examples of simple platform usage....

	Platform that...						Flags
	--------------------------------------------------------------------------

	moves up and down on the spot			PLATFORM_NEW_MOVEUP | PLATFORM_NEW_PINGPONG
											PLATFORM_NEW_MOVEDOWN | PLATFORM_NEW_PINGPONG

	moves up and down on the spot and		PLATFORM_NEW_MOVEUP | PLATFORM_NEW_PINGPONG | PLATFORM_NEW_NOWALKUNDER
	cannot be walked under (e.g. solid		PLATFORM_NEW_MOVEDOWN | PLATFORM_NEW_PINGPONG | PLATFORM_NEW_NOWALKUNDER
	stone pillar)

	moves up on the spot (rises) when a		PLATFORM_NEW_STEPONACTIVATED | PLATFORM_NEW_MOVEUP
	frog is on it, and returns to start		
	position otherwise

	moves down on the spot (sinks) when a	PLATFORM_NEW_STEPONACTIVATED | PLATFORM_NEW_MOVEDOWN
	frog is on it, and returns to start
	position otherwise

	moves along a path (>1 node) and		PLATFORM_NEW_FORWARDS
	returns immediately to start of path
	when the last node has been reached

	moves along a path (>1 node) and		PLATFORM_NEW_FORWARDS | PLATFORM_NEW_CYCLE
	continues to cycle through all nodes	PLATFORM_NEW_BACKWARDS | PLATFORM_NEW_CYCLE
	in a 'closed' path fashion

	moves back and forth along a path		PLATFORM_NEW_FORWARDS | PLATFORM_NEW_PINGPONG
	(>1 node) in a ping-pong movement.		PLATFORM_NEW_BACKWARDS | PLATFORM_NEW_PINGPONG

	is a non-moving platform (single node)	PLATFORM_NEW_NONMOVING


----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


PLATFORMLIST platformList;								// the platform list

PLATFORM *destPlatform[4] = { NULL,NULL,NULL,NULL };	// platform that frog is about to attempt to jump to
PLATFORM *currPlatform[4] = { NULL,NULL,NULL,NULL };	// platform that frog is currently on
PLATFORM *nearestPlatform[4] = { NULL,NULL,NULL,NULL };	// platform nearest to the frog

static void	GetPlatformActiveTile(PLATFORM *pform);
void CalcNextPlatformDest(PLATFORM *plat);

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
PLATFORM *GetPlatformFromUID(long uid)
{
	PLATFORM *cur;

	for(cur = platformList.head.next; cur != &platformList.head; cur = cur->next)
		if (cur->uid == uid)
			return cur;

	dprintf"GetPlatformFromUID: Couldn't find platform %d\n", uid));
	return NULL;
}

/*	--------------------------------------------------------------------------------
	Function		: InitPlatformsForLevel
	Purpose			: initialise platforms and their movement for specified level
	Parameters		:
	Returns			: void
	Info			:
*/
void InitPlatformsForLevel(unsigned long worldID, unsigned long levelID)
{
}

/*	--------------------------------------------------------------------------------
	Function		: UpdatePlatforms
	Purpose			: updates all platforms and their states / movements, etc.
	Parameters		: 
	Returns			: void
	Info			: 
*/

float speedScale = 0.04;
float waitScale = 5;
void UpdatePlatforms()
{
	PLANE2 rebound;
	PLATFORM *cur,*next,*platBelow = NULL;
	VECTOR fromPosition,toPosition;
	VECTOR fwd;
	VECTOR moveVec;

	if(platformList.numEntries == 0)
		return;
	
	for(cur = platformList.head.next; cur != &platformList.head; cur = next)
	{
		next = cur->next;

		// update regenerating platforms
		if(cur->flags & PLATFORM_NEW_REGENERATES)
		{
			if(cur->regen)
			{
				cur->regen--;
				if(!cur->regen)
				{
					//cur->pltActor->flags &= ~ACTOR_DRAW_ALWAYS;
					//cur->pltActor->flags &= ~ACTOR_DRAW_NEVER;
					//cur->pltActor->flags |= ACTOR_DRAW_CULLED;
					cur->pltActor->actor->xluOverride = 100;
					cur->pltActor->draw = 1;

					cur->active	= 1;
					cur->visible = cur->visibleTime;
				}
			}
		}

		// check if platform is active
		if(!cur->active)
			continue;

		// check if this platform is currently 'waiting' at a node
		if(cur->isWaiting)
		{
			if(cur->isWaiting == -1)
				continue;
		
			if (actFrameCount>cur->path->startFrame)
				cur->isWaiting = 0;
			else
				continue;
		}

		if(cur->flags & PLATFORM_NEW_FOLLOWPATH)
		{
			float length;
			// process platforms that follow a path (>1 node in path)

			// first, update the platform position

			GetPositionForPathNode(&toPosition,&cur->path->nodes[cur->path->toNode]);
			GetPositionForPathNode(&fromPosition,&cur->path->nodes[cur->path->fromNode]);
			
			SubVector(&fwd,&toPosition,&fromPosition);
			
			length = (float)(actFrameCount-cur->path->startFrame)/(float)(cur->path->endFrame-cur->path->startFrame);
			
			ScaleVector(&fwd,length);
			AddVector (&cur->pltActor->actor->pos,&fwd,&fromPosition);
			MakeUnit( &fwd );

//--------------------->

			AddToVector(&cur->currNormal,&cur->deltaNormal);

			if (!(cur->flags & PLATFORM_NEW_FACEFORWARDS))
			{
				Orientate(&cur->pltActor->actor->qRot,&fwd,&inVec,&cur->currNormal);
			}
			else
			{
				SubVector( &moveVec, &cur->path->nodes[cur->path->startNode+1].worldTile->centre, &cur->path->nodes[cur->path->startNode].worldTile->centre );
				if (cur->flags & PLATFORM_NEW_BACKWARDS) ScaleVector (&fwd,-1);
				MakeUnit( &moveVec );
				Orientate(&cur->pltActor->actor->qRot,&moveVec,&inVec,&cur->currNormal);
			}
			

//--------------------->

			// check if this platform has arrived at a path node
			if(PlatformHasArrivedAtNode(cur))
			{
				UpdatePlatformPathNodes(cur);
				
				cur->path->startFrame = cur->path->endFrame + cur->isWaiting * waitScale;
				cur->path->endFrame = cur->path->startFrame + (60*cur->currSpeed);
			}
		}
		else
		{
			// process platforms that are based on a single node

			// get up vector for this platform
			SetVector(&moveVec,&cur->path->nodes[0].worldTile->normal);
			
			moveVec.v[X] *= cur->currSpeed;
			moveVec.v[Y] *= cur->currSpeed;
			moveVec.v[Z] *= cur->currSpeed;

			if(cur->flags & PLATFORM_NEW_STEPONACTIVATED)
			{
				// only move up or down when frog is on them - otherwise return to start pos

				if(cur->flags & PLATFORM_NEW_CARRYINGFROG)
				{
					// platform has frog on it

					if(cur->flags & PLATFORM_NEW_SINKWITHFROG)
					{
						GetPositionForPathNode(&fromPosition,&cur->path->nodes[0]);
						GetPositionForPathNodeOffset2(&toPosition,&cur->path->nodes[0]);
		
						if(DistanceBetweenPointsSquared(&cur->pltActor->actor->pos,&fromPosition) < DistanceBetweenPointsSquared(&fromPosition,&toPosition))
						{
							SubFromVector(&cur->pltActor->actor->pos,&moveVec);
						}
						else
							if(cur->flags & PLATFORM_NEW_KILLSFROG)
							{
								if (!(player[0].frogState & FROGSTATUS_ISDEAD))
								{
									AnimateActor(frog[0]->actor,FROG_ANIM_FWDSOMERSAULT,NO,NO,0.5F,0,0);
									frog[0]->action.deathBy = DEATHBY_DROWNING;
									player[0].frogState |= FROGSTATUS_ISDEAD;
									frog[0]->action.dead = 50;
								}
							}
					}
					else if(cur->flags & PLATFORM_NEW_RISEWITHFROG)
					{
						GetPositionForPathNode(&fromPosition,&cur->path->nodes[0]);
						GetPositionForPathNodeOffset2(&toPosition,&cur->path->nodes[0]);

						if(DistanceBetweenPointsSquared(&cur->pltActor->actor->pos,&fromPosition) < DistanceBetweenPointsSquared(&fromPosition,&toPosition))
							AddToVector(&cur->pltActor->actor->pos,&moveVec);
					}
				}
				else
				{
					// platform has no frog on it - return to original position
					GetPositionForPathNode(&fromPosition,&cur->path->nodes[0]);
					if(DistanceBetweenPointsSquared(&cur->pltActor->actor->pos,&fromPosition) > 5.0F)
					{
						if(cur->flags & PLATFORM_NEW_SINKWITHFROG)
						{
							AddToVector(&cur->pltActor->actor->pos,&moveVec);
						}
						else if(cur->flags & PLATFORM_NEW_RISEWITHFROG)
						{
							SubFromVector(&cur->pltActor->actor->pos,&moveVec);
						}
					}
				}
			}
			else
			{
				// check if this platform is moving up or down
				if(cur->flags & PLATFORM_NEW_MOVEUP)
				{
					// platform is moving up
					AddToVector(&cur->pltActor->actor->pos,&moveVec);
				}
				else if(cur->flags & PLATFORM_NEW_MOVEDOWN)
				{
					// platform is moving down
					SubFromVector(&cur->pltActor->actor->pos,&moveVec);
				}
	
				if(PlatformReachedTopOrBottomPoint(cur))
					UpdatePlatformPathNodes(cur);
			}
		}

		// determine which world tile the platform is currently 'in'
		GetPlatformActiveTile(cur);

		// determine if platform is carrying frog
		if(cur->flags & PLATFORM_NEW_CARRYINGFROG)
		{

			if (!cur->carrying)
			{
				dprintf"Platform is NOT carrying but flag is set!\n"));
				cur->flags &= ~PLATFORM_NEW_CARRYINGFROG;
			}
			else
			{

				// check if this is a disappearing or crumbling platform
				if((cur->flags & PLATFORM_NEW_DISAPPEARWITHFROG) || (cur->flags & PLATFORM_NEW_CRUMBLES))
				{
					if(cur->visible)
					{
						// give some visual indication that this platform is about to vanish
						if(cur->visible < 30)
						{
							SetVector(&cur->pltActor->actor->pos,&frog[0]->actor->pos);
							cur->pltActor->actor->pos.v[X] += (-2 + Random(5));
							cur->pltActor->actor->pos.v[Y] += (-2 + Random(5));
							cur->pltActor->actor->pos.v[Z] += (-2 + Random(5));
						}

						cur->visible--;
						if(!cur->visible)
						{
							cur->pltActor->actor->xluOverride = 0;
							cur->pltActor->draw = 0;
							
						//cur->pltActor->flags &= ~ACTOR_DRAW_ALWAYS;
						//	cur->pltActor->flags &= ~ACTOR_DRAW_CULLED;
					//		cur->pltActor->flags |= ACTOR_DRAW_NEVER;

							cur->active = 0;
							cur->flags &= ~PLATFORM_NEW_CARRYINGFROG;
							cur->carrying = NULL;
							currTile[0] = cur->inTile;

							if(cur->flags & PLATFORM_NEW_DISAPPEARWITHFROG)
							{
								// platform disappears and will regenerate
								cur->regen = cur->regenTime;
							}
							else
							{
								// platform crumbles and will not regenerate
								SetVector(&rebound.point,&cur->inTile->centre);
								SetVector(&rebound.normal,&cur->inTile->normal);
								cur->regen = cur->regenTime;	//0;
								CreateAndAddFXExplodeParticle(EXPLODEPARTICLE_TYPE_SMOKEBURST,&cur->pltActor->actor->pos,&rebound.normal,6,16,&rebound,30);
								CreateAndAddFXExplodeParticle(EXPLODEPARTICLE_TYPE_SMOKEBURST,&cur->pltActor->actor->pos,&rebound.normal,12,32,&rebound,40);
							}

							CreateAndAddFXSmoke(&cur->pltActor->actor->pos,128,30);
							
							SetVector(&frog[0]->actor->vel,&currTile[0]->normal);
							FlipVector(&frog[0]->actor->vel);

							GetPositionForPathNode(&fromPosition,&cur->path->nodes[0]);
							SetVector(&cur->pltActor->actor->pos,&fromPosition);

							// check if there are any platforms below the frog over the same tile
	/*
							if(destPlatform[0] = GetNearestPlatformBelowFrog(cur->inTile,0))
							{
								// set frog falling to nearest dest platform below
								player[0].frogState |= FROGSTATUS_ISFALLINGTOPLATFORM;
							}
							else
							{
								// set frog falling to game tile below
								player[0].frogState |= FROGSTATUS_ISFALLINGTOGROUND;
							}
	*/
							
							// frog is free-falling - check for platform below the frog
							destPlatform[0] = GetNearestPlatformBelowFrog(cur->inTile,0);
							player[0].frogState |= FROGSTATUS_ISFREEFALLING;
						}
					}

				}
				else
				{
					float distance;
					long nCamFac = 0, j = 0;

					distance = -10000;
				
					for(j=0; j<4; j++)
					{
						float t = DotProduct(&(cur->inTile->dirVector[j]),&(currTile[0]->dirVector[camFacing]));
						if(t > distance)
						{
							distance = t;
							nCamFac = j;
						}							
					}		
		
					for(j=0; j<NUM_FROGS; j++)
						if (cur->carrying == frog[j])
							currTile[j] = cur->inTile;

					camFacing = nCamFac;
					//cur->carrying = frog[0];

					SetVector(&cur->carrying->actor->pos,&cur->pltActor->actor->pos);

					if(!cur->flags & PLATFORM_NEW_NONMOVING)
						SetQuaternion(&cur->carrying->actor->qRot,&cur->pltActor->actor->qRot);
				}
			}
		}
		else
		{
			// platform is not carrying a frog
			if(cur->visible < cur->visibleTime)
				cur->visible++;
		}

		// Add ripples around a platform
		if( cur->pltActor->effects & EF_RIPPLE_RINGS )
		{
			long r;
			VECTOR rPos;
			if( cur->pltActor->value1 )
				r = Random(cur->pltActor->value1)+1;
			else
				r = 10;

			SetVector( &rPos, &cur->pltActor->actor->pos );
			rPos.v[Y] = cur->inTile->centre.v[Y];

			if( !(actFrameCount%r) )
			{
				if( cur->flags & PLATFORM_NEW_FOLLOWPATH ) // More of a wake effect when moving
					CreateAndAddFXRipple( RIPPLE_TYPE_RING, &rPos, &cur->currNormal, 30, cur->currSpeed, 1, 5 );
				else // Gentle ripples
					CreateAndAddFXRipple( RIPPLE_TYPE_RING, &rPos, &cur->currNormal, 50, 1, 0.1, 20 );
			}
		}
		if( cur->pltActor->effects & EF_SMOKE_CLOUDS )
		{
			long r;
			if( cur->pltActor->value1 )
				r = Random(cur->pltActor->value1)+1;
			else
				r = 10;

			if( !(actFrameCount%r) )
				CreateAndAddFXSmoke(&cur->pltActor->actor->pos,80,20);
		}
		if( cur->pltActor->effects & EF_SPARK_BURSTS )
		{
			long r;
			if( cur->pltActor->value1 )
				r = Random(cur->pltActor->value1)+1;
			else
				r = 10;

			if( !(actFrameCount%r) )
			{
				SetVector(&rebound.point,&cur->inTile->centre);
				SetVector(&rebound.normal,&cur->inTile->normal);
				CreateAndAddFXExplodeParticle( EXPLODEPARTICLE_TYPE_NORMAL, &cur->pltActor->actor->pos, &cur->currNormal, 5, 30, &rebound, 20 );
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: JumpingToTileWithPlatform
	Purpose			: checks if frog is jumping to a tile that has a platform(s)
	Parameters		: GAMETILE *,long
	Returns			: PLATFORM *
	Info			: platform returned is platform frog is attempting to jump to
*/
PLATFORM *JumpingToTileWithPlatform(GAMETILE *tile,long pl)
{
	PLATFORM *cur,*next;
	float distance = 999999999;
	float height;
	VECTOR vec1;
	float t;

	// check if jumping to a platform
	nearestPlatform[pl] = NULL;

	if(platformList.numEntries == 0)
		return NULL;

	// go thru platform list and check for platforms in the specified tile
	for(cur = platformList.head.next; cur != &platformList.head; cur = next)
	{
		next = cur->next;

		// process only the platforms that are visible
		if((!cur->pltActor->draw) || (cur == currPlatform[pl]))
			continue;

		t = DistanceBetweenPointsSquared(&frog[pl]->actor->pos,&cur->pltActor->actor->pos);
		if(t < (distance * distance))
		{
			distance = t;
			nearestPlatform[pl] = cur;
		}

		if(cur->inTile == tile)
		{
			player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOTILE;
			player[pl].frogState |= FROGSTATUS_ISJUMPINGTOPLATFORM;

			return cur;
		}
	}

	// if we get here then no platform was in the specified tile - check for nearest platform
	if(nearestPlatform[pl])
	{
		// determine if the nearest platform is moving 'into' the specified tile
		if(nearestPlatform[pl]->path->nodes[nearestPlatform[pl]->path->toNode].worldTile == tile)
		{
			// this platform is about to move into the specified tile
			if(distance < (PLATFORM_GENEROSITY * PLATFORM_GENEROSITY))
			{
				player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOTILE;
				player[pl].frogState |= FROGSTATUS_ISJUMPINGTOPLATFORM;
			
				return nearestPlatform[pl];
			}
		}
	}

	// if we get here then frog is not jumping to a platform (e.g. no platform, missed a platform)
	player[pl].frogState |= FROGSTATUS_ISJUMPINGTOTILE;
	player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOPLATFORM;

	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function		: GetNextLocalPlatform
	Purpose			: checks if there is an adjacent 'local' platform / platform segment
	Parameters		: unsigned long
	Returns			: void
	Info			: 
*/
void GetNextLocalPlatform(unsigned long direction)
{
	VECTOR cDir;
	float t,distance;
	int newCamFacing = camFacing;
	int i,j;
	unsigned long closest[4] = { -1,-1,-1,-1 };
	int newFrogFacing = frogFacing[0];
	
	for(i=0; i<4; i++)
	{
		if(currPlatform[0]->pltPtrs[i])
		{
			SubVector(&cDir,&currPlatform[0]->pltPtrs[i]->pltActor->actor->pos,&currPlatform[0]->pltActor->actor->pos);
			MakeUnit(&cDir);
			
			distance = -10000;
			
			for(j=0; j<4; j++)
			{	
				float t = DotProduct(&cDir,&currPlatform[0]->dirVector[j]);
				if(t > distance)
				{
					distance = t;
					closest[i] = j;					
				}
			}
		}
	}

	destPlatform[0] = NULL;
		
	for(i=0; i<4; i++)
	{
		if(closest[i] == ((direction + camFacing) & 3))
		{
			distance = -10000;
			
			destPlatform[0] = currPlatform[0]->pltPtrs[i];

			for(j=0; j<4; j++)
			{
				float t = DotProduct(&destPlatform[0]->dirVector[j],&currPlatform[0]->dirVector[camFacing]);
				if(t > distance)
				{
					distance = t;
					newCamFacing = j;
				}
			}		
		}
	}

	camFacing = newCamFacing;
}



/*	--------------------------------------------------------------------------------
	Function		: GetPlatformFrogIsOn
	Purpose			: determines the platform that the frog is currently on
	Parameters		: 
	Returns			: PLATFORM *
	Info			: 
*/
PLATFORM *GetPlatformFrogIsOn(long pl)
{
	PLATFORM *cur,*next;

	if(platformList.numEntries == 0)
		return NULL;

	for(cur = platformList.head.next; cur != &platformList.head; cur = next)
	{
		next = cur->next;

		// process only the platforms that are active
		if(!cur->pltActor->draw)
			continue;

		if(cur->flags & PLATFORM_NEW_CARRYINGFROG)
			return cur;
	}

	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function		: GetPlatformForTile
	Purpose			: gets platform that is associated with tile
	Parameters		: GAMETILE *
	Returns			: void
	Info			: 
*/
PLATFORM *GetPlatformForTile(GAMETILE *tile)
{
	PLATFORM *cur,*next;

	if(platformList.numEntries == 0)
		return NULL;

	// search through platform list until required platform is found
	for(cur = platformList.head.next; cur != &platformList.head; cur = next)
	{
		next = cur->next;

		if(cur->inTile == tile)
			return cur;
	}

	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function		: ResetPlatformFlags
	Purpose			: resets platform flags
	Parameters		: 
	Returns			: void
	Info			: 
*/
void ResetPlatformFlags()
{
	PLATFORM *cur,*next;

	if(platformList.numEntries == 0)
		return;

	for(cur = platformList.head.next; cur != &platformList.head; cur = next)
	{
		next = cur->next;

		cur->flags		&= ~PLATFORM_NEW_CARRYINGFROG;
		cur->carrying	= NULL;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: GetPlatformActiveTile
	Purpose			: gets the currently active tile for the platform
	Parameters		: PLATFORM *
	Returns			: void
	Info			: 
*/
static void	GetPlatformActiveTile(PLATFORM *pform)
{
	VECTOR v1,v2,diff;
	float halfdist;
	
	if((pform->flags & PLATFORM_NEW_FOLLOWPATH ) || (pform->flags & PLATFORM_NEW_MOVEUP) || (pform->flags & PLATFORM_NEW_MOVEDOWN))
	{
		GetPositionForPathNode(&v1,&pform->path->nodes[pform->path->fromNode]);
		GetPositionForPathNode(&v2,&pform->path->nodes[pform->path->toNode]);

		halfdist = DistanceBetweenPoints(&v1,&v2) / 2.0F;

		if(DistanceBetweenPointsSquared(&v1,&pform->pltActor->actor->pos) < (halfdist * halfdist))
			pform->inTile = pform->path->nodes[pform->path->fromNode].worldTile;
		else
			pform->inTile = pform->path->nodes[pform->path->toNode].worldTile;
	}
	else
	{
		pform->inTile = pform->path->nodes[0].worldTile;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: PlatformTooHigh
	Purpose			: checks is destination platform is too high to jump to
	Parameters		: PLATFORM *,long
	Returns			: BOOL
	Info			: 
*/
BOOL PlatformTooHigh(PLATFORM *plat,long pl)
{
	VECTOR vec;
	float height,h;
	VECTOR diff;

	SubVector(&diff,&plat->pltActor->actor->pos,&frog[pl]->actor->pos);
	h = Magnitude(&diff);
	MakeUnit(&diff);
	height = (h * DotProduct(&diff,&plat->inTile->normal));

	// too high
	if(height > 51.0F)
	{
		// too high - cannot be jumped up to either with or without superhop
		return TRUE;
	}

	// too high for normal hop
	if(height > 25.0F && !player[pl].isSuperHopping)
	{
		// too high - need superhop for this jump up
		return TRUE;
	}
	
	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function		: PlatformTooLow
	Purpose			: checks is destination platform is too low to jump to
	Parameters		: PLATFORM *,long
	Returns			: BOOL
	Info			: 
*/
BOOL PlatformTooLow(PLATFORM *plat,long pl)
{
	VECTOR vec;
	float height,h;
	VECTOR diff;
		
	SubVector(&diff,&plat->pltActor->actor->pos,&frog[pl]->actor->pos);
	h = Magnitude(&diff);
	MakeUnit(&diff);
	height = (h * DotProduct(&diff,&plat->inTile->normal));

	if(height < -125.0F)
	{
		// platform too far below
		return TRUE;
	}
	
	// platform can be jumped down to
	return FALSE;
}



/*	--------------------------------------------------------------------------------
	Function		: InitPlatformLinkedList
	Purpose			: initialises the platform linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void InitPlatformLinkedList()
{
	platformList.numEntries = 0;
	platformList.head.next = platformList.head.prev = &platformList.head;
}

/*	--------------------------------------------------------------------------------
	Function		: AddPlatform
	Purpose			: adds a platform to the linked list
	Parameters		: PLATFORM *
	Returns			: void
	Info			: 
*/
void AddPlatform(PLATFORM *plat)
{
	if(plat->next == NULL)
	{
		platformList.numEntries++;
		plat->prev = &platformList.head;
		plat->next = platformList.head.next;
		platformList.head.next->prev = plat;
		platformList.head.next = plat;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: SubPlatform
	Purpose			: removes a platform from the linked list
	Parameters		: PLATFORM *
	Returns			: void
	Info			: 
*/
void SubPlatform(PLATFORM *plat)
{
	if(plat->next == NULL)
		return;

	if(plat->path)
	{
		if(plat->path->nodes)
			JallocFree((UBYTE**)&plat->path->nodes);

		JallocFree((UBYTE**)&plat->path);
	}

	plat->prev->next = plat->next;
	plat->next->prev = plat->prev;
	plat->next = NULL;
	platformList.numEntries--;

	JallocFree((UBYTE **)&plat);
}


/*	--------------------------------------------------------------------------------
	Function		: FreePlatformLinkedList
	Purpose			: frees the platform linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreePlatformLinkedList()
{
	PLATFORM *cur,*next;

	// check if any elements in list
	if(platformList.numEntries == 0)
		return;

	dprintf"Freeing linked list : PLATFORM : (%d elements)\n",platformList.numEntries));

	// traverse platform list and free elements
	for(cur = platformList.head.next; cur != &platformList.head; cur = next)
	{
		next = cur->next;

		SubPlatform(cur);
	}

	// initialise list for future use
	InitPlatformLinkedList();
}



//------------------------------------------------------------------------------------------------
// NEW PLATFORM CODE - UNDER DEVELOPMENT - ANDYE - NEW PLATFORM CODE - UNDER DEVELOPMENT - ANDYE -
//------------------------------------------------------------------------------------------------

PLATFORM *CreateAndAddPlatform(char *pActorName)
{
	int i;

	PLATFORM *newItem = (PLATFORM *)JallocAlloc(sizeof(PLATFORM),YES,"PLAT");
	AddPlatform(newItem);

	//stringChange ( pActorName );

	newItem->pltActor = CreateAndAddActor(pActorName,0,0,0,INIT_ANIMATION,0,0);
	if(newItem->pltActor->actor->objectController)
	{
		InitActorAnim(newItem->pltActor->actor);
		AnimateActor(newItem->pltActor->actor,0,YES,NO,1.0F*newItem->pltActor->animSpeed,0,0);
	}

	// currently set all surrounding platform ptrs to null
	for(i=0; i<4; i++)
		newItem->pltPtrs[i] = NULL;

	// set the platform to be active (i.e. is updated)
	newItem->active			= 1;
	
	newItem->path			= NULL;
	newItem->inTile			= NULL;

	newItem->currSpeed		= 1.0F;

	// set this platform to be carrying no actors (frogs)
	newItem->carrying		= NULL;
	newItem->flags			= 0;

	return newItem;
}

void AssignPathToPlatform(PLATFORM *pform,unsigned long platformFlags,PATH *path,unsigned long pathFlags)
{
	int i;
	VECTOR platformStartPos;

	// assign the path to this platform
	pform->flags	|= platformFlags;
	pform->path		= path;

	dprintf"Add pform path : "));

	// check if pathnode indices need converting to game tile pointers
	if(pathFlags & PATH_MAKENODETILEPTRS)
	{
		for(i=0; i<path->numNodes; i++)
		{
			// convert integer to a valid game tile
			dprintf"%d, ",(unsigned long)path->nodes[i].worldTile));
			pform->path->nodes[i].worldTile = &firstTile[(unsigned long)path->nodes[i].worldTile];
		}
	}

	// set the start position for the platform
	pform->path->fromNode = pform->path->startNode;
	
	if(platformFlags & PLATFORM_NEW_FORWARDS)
	{
		if (path->numNodes < 2)
			pform->flags &= ~PLATFORM_NEW_FORWARDS;
		else // this platform moves forward thru path nodes
		{
			pform->flags			|= PLATFORM_NEW_FOLLOWPATH;
			pform->path->toNode = pform->path->fromNode + 1;
			if(pform->path->toNode > GET_PATHLASTNODE(path))
			{
				if (platformFlags & PLATFORM_NEW_PINGPONG)
				{
					pform->flags &= ~PLATFORM_NEW_FORWARDS;
					pform->flags |= PLATFORM_NEW_BACKWARDS;
					pform->path->toNode = GET_PATHLASTNODE(path)-1;
				}
				else
				{
					pform->path->fromNode = 0;
					pform->path->toNode = 1;
				}
			}
		}
	}
	else if(platformFlags & PLATFORM_NEW_BACKWARDS)
	{
		if (path->numNodes < 2)
			pform->flags &= ~PLATFORM_NEW_BACKWARDS;
		else // this platform moves backward thru path nodes
		{
			pform->flags			|= PLATFORM_NEW_FOLLOWPATH;
			pform->path->toNode = pform->path->fromNode - 1;
			if(pform->path->toNode < 0)
			{
				if (platformFlags & PLATFORM_NEW_PINGPONG)
				{
					pform->flags &= ~PLATFORM_NEW_BACKWARDS;
					pform->flags |= PLATFORM_NEW_FORWARDS;
					pform->path->toNode = 1;
					pform->path->fromNode = 0;
				}
				else
				{
					pform->path->fromNode = GET_PATHLASTNODE(path);
					pform->path->toNode = GET_PATHLASTNODE(path)-1;
				}
			}
		}
	}
	else if(platformFlags & PLATFORM_NEW_MOVEUP)
	{
		// this platform moves up
		pform->path->fromNode = pform->path->toNode = 0;

		// rise under frog weight ?
		if(platformFlags & PLATFORM_NEW_STEPONACTIVATED)
			pform->flags |= PLATFORM_NEW_RISEWITHFROG;
	}
	else if(platformFlags & PLATFORM_NEW_MOVEDOWN)
	{
		// this platform moves down
		pform->path->fromNode = pform->path->toNode = 0;

		// sinks under frog weight ?
		if(platformFlags & PLATFORM_NEW_STEPONACTIVATED)
			pform->flags |= PLATFORM_NEW_SINKWITHFROG;
	}
	else
	{
		// this platform does not move
		pform->flags |= PLATFORM_NEW_NONMOVING;
		pform->path->fromNode = pform->path->toNode = 0;
	}

	// set platform position to relevant point on path
	if (platformFlags & PLATFORM_NEW_MOVEDOWN)
		GetPositionForPathNodeOffset2(&platformStartPos,&path->nodes[pform->path->fromNode]);
	else
		GetPositionForPathNode(&platformStartPos,&path->nodes[pform->path->fromNode]);

	SetVector(&pform->pltActor->actor->pos,&platformStartPos);
	NormalToQuaternion(&pform->pltActor->actor->qRot,&path->nodes[pform->path->fromNode].worldTile->normal);

	// set platform current 'in' tile and speeds and pause times
	pform->inTile		= path->nodes[pform->path->fromNode].worldTile;
	pform->currSpeed	= path->nodes[pform->path->fromNode].speed;
	pform->isWaiting	= path->nodes[pform->path->fromNode].waitTime;

	pform->path->startFrame = actFrameCount;
	pform->path->endFrame = (actFrameCount+(60*pform->currSpeed));

	CalcPlatformNormalInterps(pform);

	if (pform->flags & PLATFORM_NEW_CRUMBLES)
		pform->visible = pform->visibleTime = path->nodes[pform->path->fromNode].waitTime;

	dprintf"\n"));
}


/*	--------------------------------------------------------------------------------
	Function		: PlatformHasArrivedAtNode
	Purpose			: checks if a platform has arrived at a node
	Parameters		: PLATFORM *
	Returns			: BOOL - TRUE if node reached, else FALSE
	Info			: 
*/
BOOL PlatformHasArrivedAtNode(PLATFORM *pform)
{
	VECTOR nodePos;
	PATH *path = pform->path;

	if (actFrameCount>path->endFrame)
	{
		if (pform->flags & PLATFORM_NEW_CARRYINGFROG)
		{
			CheckTileForCollectable(path->nodes[path->toNode].worldTile,0);
		}

		return TRUE;
	}
	// check if path node is reached
/*	GetPositionForPathNode(&nodePos,&path->nodes[path->toNode]);
	if(DistanceBetweenPointsSquared(&pform->pltActor->actor->pos,&nodePos) <  ((pform->currSpeed + 0.1F) * (pform->currSpeed + 0.1F)))
	{
		dprintf""));
		return TRUE;
	}
*/
	return FALSE;
}


/*	--------------------------------------------------------------------------------
	Function		: PlatformReachedTopOrBottomPoint
	Purpose			: checks if a platform has arrived at top or bottom point (1 node based)
	Parameters		: PLATFORM *
	Returns			: BOOL - TRUE if node reached, else FALSE
	Info			: 
*/
BOOL PlatformReachedTopOrBottomPoint(PLATFORM *pform)
{
	VECTOR toPos;
	PATH *path = pform->path;

	// check if path extreme point is reached
	if(pform->flags & PLATFORM_NEW_MOVEUP)
	{
		// moving up
		GetPositionForPathNodeOffset2(&toPos,&path->nodes[0]);

		if(DistanceBetweenPointsSquared(&pform->pltActor->actor->pos,&toPos) < (pform->currSpeed * pform->currSpeed))
			return TRUE;
	}
	else if(pform->flags & PLATFORM_NEW_MOVEDOWN)
	{
		// moving down
		GetPositionForPathNode(&toPos,&path->nodes[0]);

		if(DistanceBetweenPointsSquared(&pform->pltActor->actor->pos,&toPos) < (pform->currSpeed * pform->currSpeed))
			return TRUE;
	}

	return FALSE;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdatePlatformPathNodes
	Purpose			: updates platform path move status
	Parameters		: PLATFORM *
	Returns			: void
	Info			: 
*/
void UpdatePlatformPathNodes(PLATFORM *pform)
{
	VECTOR pformPos;
	PATH *path = pform->path;
	unsigned long flags = pform->flags;
	
	path->fromNode = path->toNode;

	CalcNextPlatformDest(pform);

	pform->currSpeed = GetSpeedFromIndexedNode(path,path->fromNode);
	pform->isWaiting = GetWaitTimeFromIndexedNode(path,path->fromNode);

	// Stop overshoot when waiting on a path node
	if (pform->isWaiting)
	{
		if (pform->flags & PLATFORM_NEW_MOVEDOWN)
			GetPositionForPathNodeOffset2(&pformPos, &path->nodes[path->fromNode]);
		else
			GetPositionForPathNode(&pformPos, &path->nodes[path->fromNode]);

		SetVector(&pform->pltActor->actor->pos, &pformPos);
	}

	CalcPlatformNormalInterps(pform);
}


/*	--------------------------------------------------------------------------------
	Function		: CalcPlatformNormalInterps
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void CalcPlatformNormalInterps(PLATFORM *pform)
{
	PATH *path;
	PATHNODE *fromNode,*toNode;
	float numSteps;
	VECTOR destNormal,fromPos,toPos;

	path = pform->path;
	if(path->numNodes < 2)
		return;

	fromNode	= &path->nodes[path->fromNode];
	toNode		= &path->nodes[path->toNode];

	// set the current platform normal to that of the 'from' node and get the dest normal
	SetVector(&pform->currNormal,&fromNode->worldTile->normal);
	SetVector(&destNormal,&toNode->worldTile->normal);

	// calculate deltas for linear interpolation of platform normal during movement
	SubVector(&pform->deltaNormal,&destNormal,&pform->currNormal);

	// determine number of 'steps' over which to interpolate
	GetPositionForPathNode(&fromPos,fromNode);
	GetPositionForPathNode(&toPos,toNode);
	
	numSteps = DistanceBetweenPoints(&fromPos,&toPos);
	if(numSteps == 0)
		numSteps = 1;
	numSteps /= pform->currSpeed;

	pform->deltaNormal.v[X] /= numSteps;
	pform->deltaNormal.v[Y] /= numSteps;
	pform->deltaNormal.v[Z] /= numSteps;
}


/*	--------------------------------------------------------------------------------
	Function		: SetPlatformVisibleTime
	Purpose			: sets the visible time for disappearing / crumbling platforms
	Parameters		: PLATFORM *,short
	Returns			: void
	Info			: 
*/
void SetPlatformVisibleTime(PLATFORM *pform,short time)
{
	pform->visibleTime	= time;
	pform->visible		= time;
}

/*	--------------------------------------------------------------------------------
	Function		: SetPlatformRegenerateTime
	Purpose			: sets the regeneration time for a platform
	Parameters		: PLATFORM *,short
	Returns			: void
	Info			: 
*/
void SetPlatformRegenerateTime(PLATFORM *pform,short time)
{
	pform->regenTime	= time;
	pform->regen		= time;
}


/*	--------------------------------------------------------------------------------
	Function		: GetNearestPlatformBelowFrog
	Purpose			: returns the nearest platform below the frog on the specified tile
	Parameters		: GAMETILE *,long
	Returns			: PLATFORM *
	Info			: 
*/
PLATFORM *GetNearestPlatformBelowFrog(GAMETILE *tile,long pl)
{
	int i,numPlats = 0;
	VECTOR diff;
	PLATFORM *plts[8],*pltNearest;
	PLATFORM *cur,*next;
	float dists[8],t = 9999999;
	float frogDist;
	
	if(platformList.numEntries == 0)
		return NULL;

	frogDist = DistanceBetweenPoints(&frog[pl]->actor->pos,&tile->centre);

	for(cur = platformList.head.next; cur != &platformList.head; cur = next)
	{
		next = cur->next;

		if((cur->inTile == tile) && (cur->active))
		{
			// this platform is over the same tile as the frog
			t = DistanceBetweenPoints(&frog[pl]->actor->pos,&cur->pltActor->actor->pos);

			dprintf"Platform %f\n",t));

			if((t > 0) && (t < frogDist))
			{
				plts[numPlats]	= cur;
				dists[numPlats]	= t;
				numPlats++;
			}
		}
	}

	// any platforms below ?
	if(!numPlats)
		return NULL;

	pltNearest = plts[0];
	t = dists[0];

	// determine closest platform
	if(numPlats == 1)
	{
		// return the only platform below the frog
		return pltNearest;
	}
	else
	{
		// determine the next closest platform below the frog
		for(i=1; i<numPlats; i++)
		{
			if(dists[i] < t)
			{
				t = dists[i];
				pltNearest = plts[i];
			}
		}
	}

	return pltNearest;
}

/*	--------------------------------------------------------------------------------
	Function		: CalcNextPlatformDest
	Purpose			: Figures out what tile to go to next, on a path
	Parameters		: PLATFORM *
	Returns			: void
	Info			: 
*/
void CalcNextPlatformDest(PLATFORM *pform)
{
	PATH *path = pform->path;
	unsigned long flags = pform->flags;

	if(pform->flags & PLATFORM_NEW_FORWARDS)	// platform moves forward through path nodes
	{
		path->toNode = path->fromNode + 1;

		if(path->toNode > GET_PATHLASTNODE(path))
		{
			if(flags & PLATFORM_NEW_PINGPONG)
			{
				pform->flags	^= (PLATFORM_NEW_FORWARDS | PLATFORM_NEW_BACKWARDS);
				path->fromNode	= GET_PATHLASTNODE(path);
				path->toNode	= GET_PATHLASTNODE(path) - 1;
			}
			else if(flags & PLATFORM_NEW_CYCLE)
			{
				// platform has cyclic movement
				path->fromNode	= GET_PATHLASTNODE(path);
				path->toNode	= 0;
			}
			else
			{
				path->fromNode	= 0;
				path->toNode	= 1;
				//GetPositionForPathNode(&pformPos,&path->nodes[0]);
				//SetVector(&pform->pltActor->actor->pos,&pformPos);
			}
		}
	}
	else if(flags & PLATFORM_NEW_BACKWARDS)
	{
		// platform moves backwards through path nodes
		path->toNode = path->fromNode - 1;

		if(path->toNode < 0)
		{
			if(flags & PLATFORM_NEW_PINGPONG)
			{
				pform->flags	^= (PLATFORM_NEW_FORWARDS | PLATFORM_NEW_BACKWARDS);
				path->fromNode	= 0;
				path->toNode	= 1;
			}
			else if(flags & PLATFORM_NEW_CYCLE)
			{
				path->fromNode	= 0;
				path->toNode	= GET_PATHLASTNODE(path);
			}
			else
			{
				path->fromNode	= GET_PATHLASTNODE(path);
				path->toNode	= GET_PATHLASTNODE(path) - 1;
				//GetPositionForPathNode(&pformPos,&path->nodes[GET_PATHLASTNODE(path)]);
				//SetVector(&pform->pltActor->actor->pos,&pformPos);
			}
		}
	}
	else if((flags & PLATFORM_NEW_PINGPONG) && flags & (PLATFORM_NEW_MOVEUP | PLATFORM_NEW_MOVEDOWN))
	{
		pform->flags	^= (PLATFORM_NEW_MOVEUP | PLATFORM_NEW_MOVEDOWN);
	}
}


/*	--------------------------------------------------------------------------------
	Function		: RecalculatePlatform
	Parameters		: PLATFORM *
	Returns			: void
*/

void RecalculatePlatform(PLATFORM *plat)
{
	// only recalculate when we're actually waiting, otherwise when it gets to the next
	// path node it'll sort itself out anyway.

	if (plat->isWaiting && (plat->flags & PLATFORM_NEW_FOLLOWPATH))
	{
		CalcNextPlatformDest(plat);
	}
}


void FrogLeavePlatform(long pl)
{
	PLATFORM *plt;

	if ((plt = currPlatform[pl]))
	{
		plt->flags |= PLATFORM_NEW_CARRYINGFROG;	// plat is NOT carrying a frog
		plt->carrying = NULL;
		currPlatform[pl] = NULL;
	}
}