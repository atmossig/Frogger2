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

float PLATFORM_GENEROSITY	= 10.0F;					// platform 'forgiveness'


void CalcNextPlatformDest(PLATFORM *plat);


//----- [ PLATFORM UPDATE FUNCTIONS ] ------------------------------------------------------------

void UpdatePathPlatform(PLATFORM *plat);
void UpdateSlerpPathPlatform(PLATFORM *plat);
void UpdateUpDownPlatform(PLATFORM *plat);
void UpdateNonMovingPlatform(PLATFORM *plat);
void UpdateStepOnActivatedPlatform(PLATFORM *plat);


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
float waitScale = 5;
void UpdatePlatforms()
{
	PLATFORM *cur,*next;
	PLANE2 rebound;
	VECTOR fromPosition;

	if(platformList.numEntries == 0)
		return;

	for(cur = platformList.head.next; cur != &platformList.head; cur = next)
	{
		next = cur->next;

		// check if platform is active
		if(!cur->active)
			continue;

		// update regenerating platforms
		if(cur->flags & PLATFORM_NEW_REGENERATES)
		{
			if(cur->regen)
			{
				cur->regen--;
				if(!cur->regen)
				{
					cur->pltActor->actor->xluOverride = 100;
					cur->pltActor->draw = 1;

					cur->active	= 1;
					cur->visible = cur->visibleTime;
				}
			}
		}

		// check if this enemy is currently 'waiting' at a node
		if(cur->isWaiting)
		{
			if(cur->isWaiting == -1)
				continue;

			if(actFrameCount > cur->path->startFrame)
				cur->isWaiting = 0;
			else
			{
				// if enemy is following a path, do a slerp so it'll rotate nicely
				// (except the way I do it is a bit poo - Dave)
				if(cur->flags & PLATFORM_NEW_FOLLOWPATH && !(cur->flags & PLATFORM_NEW_FACEFORWARDS))
					RotateWaitingPlatform(cur);

				continue;
			}
		}

		// call update function for individual platform type
		if(cur->Update)
			cur->Update(cur);

		// update frog if on the current platform
		if(cur->flags & PLATFORM_NEW_CARRYINGFROG)
		{

			if(!cur->carrying)
			{
				// DEBUG CHECK !!!!
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
							
							cur->active = 0;
							cur->flags &= ~PLATFORM_NEW_CARRYINGFROG;
							cur->carrying = NULL;
							currTile[0] = cur->inTile[0];

							if(cur->flags & PLATFORM_NEW_DISAPPEARWITHFROG)
							{
								// platform disappears and will regenerate
								cur->regen = cur->regenTime;
							}
							else
							{
								// platform crumbles and will not regenerate
								SetVector(&rebound.point,&cur->inTile[0]->centre);
								SetVector(&rebound.normal,&cur->inTile[0]->normal);
								cur->regen = cur->regenTime;	//0;
								CreateAndAddSpecialEffect( FXTYPE_SMOKEBURST, &cur->pltActor->actor->pos, &cur->inTile[0]->normal, 30, 6, 0, 1 );
								CreateAndAddSpecialEffect( FXTYPE_SMOKEBURST, &cur->pltActor->actor->pos, &cur->inTile[0]->normal, 24, 12, 0, 1.5 );
							}

//							CreateAndAddSpecialEffect( FXTYPE_EXHAUSTSMOKE, &cur->pltActor->actor->pos, &cur->inTile[0]->normal, 32, 10, 0, 2 );
							
							SetVector(&frog[0]->actor->vel,&currTile[0]->normal);
							FlipVector(&frog[0]->actor->vel);

							GetPositionForPathNode(&fromPosition,&cur->path->nodes[0]);
							SetVector(&cur->pltActor->actor->pos,&fromPosition);

							// frog is free-falling - check for platform below the frog
							destPlatform[0] = GetNearestPlatformBelowFrog(cur->inTile[0],0);
							player[0].frogState |= FROGSTATUS_ISFREEFALLING;
						}
					}

				}
				else
				{
					float distance = -10000;
					long nCamFac = 0, j = 0;

					for(j=0; j<4; j++)
					{
						float t = DotProduct(&(cur->inTile[0]->dirVector[j]),&(currTile[0]->dirVector[camFacing]));
						if(t > distance)
						{
							distance = t;
							nCamFac = j;
						}							
					}		
		
					for(j=0; j<NUM_FROGS; j++)
						if(cur->carrying == frog[j])
							currTile[j] = cur->inTile[0];

					camFacing = nCamFac;

					// move frog with platform
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

		// special fx associated with platform
		if(cur->pltActor->effects)
			ProcessAttachedEffects((void *)cur,0);
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
	// VERSION FROM (21.10.1999) - ANDYE

	PLATFORM *cur,*next;

	// determine if a platform is in the tile(s) that the frog is jumping to

	if(!platformList.numEntries)
		return NULL;				// no platforms in the platform list

	// traverse platform list and search for platform in the specified tile(s)
	for(cur = platformList.head.next; cur != &platformList.head; cur = next)
	{
		next = cur->next;

		// process only visible platforms and not the current one (if any)
		if((!cur->pltActor->draw) || (cur == currPlatform[pl]))
			continue;				// skip to next platform in list

		if(cur->inTile[0] && cur->inTile[1])
		{
			// platform is 'in' two tiles currently
			if(cur->inTile[0] == tile)
			{
				// platform found - return this platform
				player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOTILE;
				player[pl].frogState |= FROGSTATUS_ISJUMPINGTOPLATFORM;
				return cur;
			}
			else if(cur->inTile[1] == tile)
			{
				// platform found - return this platform
				player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOTILE;
				player[pl].frogState |= FROGSTATUS_ISJUMPINGTOPLATFORM;
				return cur;
			}
		}
		else
		{
			// platform is 'in' a single tile
			if(cur->inTile[0] == tile)
			{
				// platform found - return this platform
				player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOTILE;
				player[pl].frogState |= FROGSTATUS_ISJUMPINGTOPLATFORM;
				return cur;
			}
		}
	}

	// so....frog is not jumping to a platform (i.e. no platform detected)
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

		if(cur->inTile[0] == tile)
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
	height = (h * DotProduct(&diff,&plat->inTile[0]->normal));

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
	height = (h * DotProduct(&diff,&plat->inTile[0]->normal));

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

PLATFORM *CreateAndAddPlatform(char *pActorName,int flags,long ID,PATH *path)
{
	int initFlags,i;
	int platformType = 0;
	float shadowRadius = 0;

	PLATFORM *newItem = (PLATFORM *)JallocAlloc(sizeof(PLATFORM),YES,"PLAT");
	AddPlatform(newItem);
	newItem->flags = flags;

	initFlags |= INIT_ANIMATION;
	initFlags |= INIT_SHADOW;
	shadowRadius = 20;

	// create and add platform actor
	newItem->pltActor = CreateAndAddActor(pActorName,0,0,0,initFlags,0,0);
	if(newItem->pltActor->actor->objectController)
	{
		InitActorAnim(newItem->pltActor->actor);
		AnimateActor(newItem->pltActor->actor,0,YES,NO,newItem->pltActor->animSpeed,0,0);
	}

	// set shadow radius (if applicable)
	if(shadowRadius)
		if(newItem->pltActor->actor->shadow)
			newItem->pltActor->actor->shadow->radius = shadowRadius;

	// currently set all surrounding platform ptrs to null
	for(i=0; i<4; i++)
		newItem->pltPtrs[i] = NULL;

	// set the platform to be active (i.e. is updated)
	newItem->active			= 1;
	newItem->path			= NULL;
	newItem->inTile[0]		= NULL;
	newItem->inTile[1]		= NULL;
	newItem->uid			= ID;

	newItem->currSpeed		= 1.0F;

	// set this platform to be carrying nowt
	newItem->carrying		= NULL;

	AssignPathToPlatform(newItem,path,0);

	// determine relevant platform update function
	if(newItem->flags & PLATFORM_NEW_FOLLOWPATH)
		newItem->Update = UpdatePathPlatform;
	else if(newItem->flags & PLATFORM_NEW_SLERPPATH)
		newItem->Update = UpdateSlerpPathPlatform;
	else if((newItem->flags & PLATFORM_NEW_MOVEUP) || (newItem->flags & PLATFORM_NEW_MOVEDOWN))
		newItem->Update = UpdateUpDownPlatform;
	else if(newItem->flags & PLATFORM_NEW_NONMOVING)
		newItem->Update = UpdateNonMovingPlatform;
	else if(newItem->flags & PLATFORM_NEW_STEPONACTIVATED)
		newItem->Update = UpdateStepOnActivatedPlatform;

	return newItem;
}

void AssignPathToPlatform(PLATFORM *pform,PATH *path,unsigned long pathFlags)
{
	int i;
	VECTOR platformStartPos;

	pform->path	= path;

	// check if pathnode indices need converting to game tile pointers
	if(pathFlags & PATH_MAKENODETILEPTRS)
		for(i=0; i<path->numNodes; i++)
			pform->path->nodes[i].worldTile = &firstTile[(unsigned long)path->nodes[i].worldTile];

	// set the start position for the platform
	pform->path->fromNode = pform->path->startNode;
	
	if(pform->flags & PLATFORM_NEW_FORWARDS)
	{
		// this platform moves forward thru path nodes
		pform->flags |= PLATFORM_NEW_FOLLOWPATH;
		pform->path->toNode = pform->path->fromNode + 1;
		
		if(pform->path->toNode > GET_PATHLASTNODE(path))
			pform->path->toNode = 0;
	}
	else if(pform->flags & PLATFORM_NEW_BACKWARDS)
	{
		// this platform moves backward thru path nodes
		pform->flags |= PLATFORM_NEW_FOLLOWPATH;
		pform->path->toNode = pform->path->fromNode - 1;
		
		if(pform->path->toNode < 0)
			pform->path->fromNode = GET_PATHLASTNODE(path);
	}
	else if((pform->flags & PLATFORM_NEW_MOVEUP) ||
			(pform->flags & PLATFORM_NEW_MOVEDOWN))
	{
		// this platform moves up or down
		pform->path->fromNode = pform->path->toNode = 0;
	}
	else
	{
		// this platform does not move
		pform->flags |= PLATFORM_NEW_NONMOVING;
		pform->path->fromNode = pform->path->toNode = 0;
	}

	// set platform position to relevant point on path
	GetPositionForPathNode(&platformStartPos,&path->nodes[pform->path->fromNode]);
	SetVector(&pform->pltActor->actor->pos,&platformStartPos);
	NormalToQuaternion(&pform->pltActor->actor->qRot,&path->nodes[pform->path->fromNode].worldTile->normal);

	// set platform current 'in' tile and speeds and pause times
	pform->inTile[0]	= path->nodes[pform->path->fromNode].worldTile;
	pform->currSpeed	= path->nodes[pform->path->fromNode].speed;
	pform->isWaiting	= path->nodes[pform->path->fromNode].waitTime;

	pform->path->startFrame = actFrameCount;
	pform->path->endFrame = (actFrameCount+(60*pform->currSpeed));

	if(pform->flags & PLATFORM_NEW_CRUMBLES)
		pform->visible = pform->visibleTime = path->nodes[pform->path->fromNode].waitTime;

	CalcPlatformNormalInterps(pform);
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

	pform->currSpeed = path->nodes[path->fromNode].speed;
	pform->isWaiting = path->nodes[path->fromNode].waitTime;

	// when we've gone past half-way (i.e. after half the current movement time),
	// set the current "in" tile to halfway.
	if (actFrameCount > (pform->path->startFrame + pform->path->endFrame) / 2)
	{
		GAMETILE *nextTile = pform->path->nodes[pform->path->toNode].worldTile;

		pform->inTile[0] = nextTile;

		// if we're moving onto a barred tile, push the frog in the other direction
		if (pform->carrying && nextTile->state == TILESTATE_BARRED)
		{
			int pl;
			VECTOR v;
			
			SubVector(&v,
				&pform->path->nodes[pform->path->fromNode].worldTile->centre,
				&pform->path->nodes[pform->path->toNode].worldTile->centre);
				
			pform->inTile[0] = nextTile;

			// We need to find which frog we're carrying (yeeeeeesh)
			for (pl=0; pl<4; pl++)
				if (currPlatform[pl] == pform)
				{
					PushFrog(&pform->pltActor->actor->pos, &v, pl);
					break;
				}
		}
	}

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

		if((cur->inTile[0] == tile) && (cur->active))
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
	else if( flags & (PLATFORM_NEW_MOVEUP | PLATFORM_NEW_MOVEDOWN) )
	{
		if( flags & PLATFORM_NEW_PINGPONG )
			pform->flags	^= (PLATFORM_NEW_MOVEUP | PLATFORM_NEW_MOVEDOWN);
		else if( flags & PLATFORM_NEW_CYCLE )
		{
			if( flags & PLATFORM_NEW_MOVEUP )
				GetPositionForPathNode( &pform->pltActor->actor->pos, &path->nodes[path->fromNode] );
			else
				GetPositionForPathNodeOffset2( &pform->pltActor->actor->pos, &path->nodes[path->fromNode] );
		}
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


/*	--------------------------------------------------------------------------------
	Function		: RotateWaitingPlatform
	Purpose			: rotates a currently waiting platform
	Parameters		: PLATFORM *
	Returns			: void
	Info			: 
*/
void RotateWaitingPlatform(PLATFORM *cur)
{
	QUATERNION q,res;
	float t;
	VECTOR fromPosition,toPosition,fwd;
	long start_t,end_t,time;
	
	end_t = cur->path->startFrame;
	time = cur->isWaiting*waitScale;
	start_t = end_t - time;

	t = 1.0 - (float)(actFrameCount-start_t)/(float)(time);

	GetPositionForPathNode(&toPosition,&cur->path->nodes[cur->path->toNode]);
	GetPositionForPathNode(&fromPosition,&cur->path->nodes[cur->path->fromNode]);
	
	SubVector(&fwd,&toPosition,&fromPosition);
	MakeUnit(&fwd);
	Orientate(&q,&fwd,&inVec,&cur->currNormal);
	QuatSlerp(&q,&cur->pltActor->actor->qRot,t,&res);
	SetQuaternion(&cur->pltActor->actor->qRot,&res);
}


//----- [ PLATFORM UPDATE FUNCTIONS ] ------------------------------------------------------------


/*	--------------------------------------------------------------------------------
	Function		: UpdatePathPlatform
	Purpose			: updates platforms that follow a path
	Parameters		: PLATFORM *
	Returns			: void
	Info			: 
*/
void UpdatePathPlatform(PLATFORM *plat)
{
	VECTOR fromPosition,toPosition,fwd,moveVec;
	float length;

	float n;

	// update the platform position
	GetPositionForPathNode(&toPosition,&plat->path->nodes[plat->path->toNode]);
	GetPositionForPathNode(&fromPosition,&plat->path->nodes[plat->path->fromNode]);
	
	SubVector(&fwd,&toPosition,&fromPosition);
	
	length = (float)(actFrameCount - plat->path->startFrame)/(float)(plat->path->endFrame - plat->path->startFrame);
	
	ScaleVector(&fwd,length);
	AddVector(&plat->pltActor->actor->pos,&fwd,&fromPosition);
	MakeUnit(&fwd);

	AddToVector(&plat->currNormal,&plat->deltaNormal);

	if(!(plat->flags & PLATFORM_NEW_FACEFORWARDS))
	{
		Orientate(&plat->pltActor->actor->qRot,&fwd,&inVec,&plat->currNormal);
	}
	else
	{
		SubVector(&moveVec,&plat->path->nodes[plat->path->startNode+1].worldTile->centre, &plat->path->nodes[plat->path->startNode].worldTile->centre );
		if (plat->flags & PLATFORM_NEW_BACKWARDS) ScaleVector (&fwd,-1);
		MakeUnit(&moveVec);
		Orientate(&plat->pltActor->actor->qRot,&moveVec,&inVec,&plat->currNormal);
	}

	// check if this platform has arrived at a path node
	if(actFrameCount > plat->path->endFrame)
	{
		UpdatePlatformPathNodes(plat);

		plat->path->startFrame = plat->path->endFrame + plat->isWaiting * waitScale;
		plat->path->endFrame = plat->path->startFrame + (60*plat->currSpeed);
	}

	GetPositionForPathNode(&fromPosition,&plat->path->nodes[plat->path->fromNode]);
	GetPositionForPathNode(&toPosition,&plat->path->nodes[plat->path->toNode]);

	length = DistanceBetweenPoints(&fromPosition,&toPosition) / 2.0F;

	// determine if platform is 'in' one or two tiles...

	n = DistanceBetweenPoints(&plat->pltActor->actor->pos,&fromPosition);

	if((n < (length + PLATFORM_GENEROSITY)) && (n > (length - PLATFORM_GENEROSITY)))
	{
		// platform is 'in' two tiles
		plat->inTile[0] = plat->path->nodes[plat->path->toNode].worldTile;
		plat->inTile[1] = plat->path->nodes[plat->path->fromNode].worldTile;
	}
	else
	{
		// platform is in a single tile
		if(n < length)
			plat->inTile[0] = plat->path->nodes[plat->path->fromNode].worldTile;
		else
			plat->inTile[0] = plat->path->nodes[plat->path->toNode].worldTile;

		plat->inTile[1] = NULL;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateSlerpPathPlatform
	Purpose			: updates platforms that follow a path with a slerp
	Parameters		: PLATFORM *
	Returns			: void
	Info			: 
*/
void UpdateSlerpPathPlatform(PLATFORM *plat)
{
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateUpDownPlatform
	Purpose			: updates platforms that rise and sink
	Parameters		: PLATFORM *
	Returns			: void
	Info			: 
*/
void UpdateUpDownPlatform(PLATFORM *plat)
{
	VECTOR moveVec;;

	// get up vector for this platform
	SetVector(&moveVec,&plat->path->nodes[0].worldTile->normal);
	ScaleVector(&moveVec,plat->currSpeed);

	// check if this platform is moving up or down
	if(plat->flags & PLATFORM_NEW_MOVEUP)
	{
		// platform is moving up
		AddToVector(&plat->pltActor->actor->pos,&moveVec);
	}
	else if(plat->flags & PLATFORM_NEW_MOVEDOWN)
	{
		// platform is moving down
		SubFromVector(&plat->pltActor->actor->pos,&moveVec);
	}
	
	if(PlatformReachedTopOrBottomPoint(plat))
		UpdatePlatformPathNodes(plat);
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateNonMovingPlatform
	Purpose			: updates platforms that don't move
	Parameters		: PLATFORM *
	Returns			: void
	Info			: 
*/
void UpdateNonMovingPlatform(PLATFORM *plat)
{
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateStepOnActivatedPlatform
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void UpdateStepOnActivatedPlatform(PLATFORM *plat)
{
	VECTOR moveVec,fromPosition,toPosition;

	// get up vector for this platform
	SetVector(&moveVec,&plat->path->nodes[0].worldTile->normal);
	ScaleVector(&moveVec,plat->currSpeed);

	// platform only moves when frog is it - otherwsie returns to start position
	if(plat->flags & PLATFORM_NEW_CARRYINGFROG)
	{
		// platform has frog on it
		if(plat->flags & PLATFORM_NEW_SINKWITHFROG)
		{
			GetPositionForPathNode(&fromPosition,&plat->path->nodes[0]);
			GetPositionForPathNodeOffset2(&toPosition,&plat->path->nodes[0]);
		
			if(DistanceBetweenPointsSquared(&plat->pltActor->actor->pos,&fromPosition) < DistanceBetweenPointsSquared(&fromPosition,&toPosition))
			{
				SubFromVector(&plat->pltActor->actor->pos,&moveVec);
			}
			else
			{
				if(plat->flags & PLATFORM_NEW_KILLSFROG)
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
		}
		else if(plat->flags & PLATFORM_NEW_RISEWITHFROG)
		{
			GetPositionForPathNode(&fromPosition,&plat->path->nodes[0]);
			GetPositionForPathNodeOffset2(&toPosition,&plat->path->nodes[0]);

			if(DistanceBetweenPointsSquared(&plat->pltActor->actor->pos,&fromPosition) < DistanceBetweenPointsSquared(&fromPosition,&toPosition))
				AddToVector(&plat->pltActor->actor->pos,&moveVec);
		}
	}
	else
	{
		// platform has no frog on it - return to original position
		GetPositionForPathNode(&fromPosition,&plat->path->nodes[0]);
		if(DistanceBetweenPointsSquared(&plat->pltActor->actor->pos,&fromPosition) > 5.0F)
		{
			if(plat->flags & PLATFORM_NEW_SINKWITHFROG)
			{
				AddToVector(&plat->pltActor->actor->pos,&moveVec);
			}
			else if(plat->flags & PLATFORM_NEW_RISEWITHFROG)
			{
				SubFromVector(&plat->pltActor->actor->pos,&moveVec);
			}
		}
	}
}

