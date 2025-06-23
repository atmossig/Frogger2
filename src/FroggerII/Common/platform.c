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

#define PLATFORM_NEAREST_DIST  500	// radius in which to check for nearest platform

PLATFORMLIST platformList;								// the platform list
PLATFORM *destPlatform[MAX_FROGS] = { NULL,NULL,NULL,NULL };	// platform that frog is about to attempt to jump to
PLATFORM *currPlatform[MAX_FROGS] = { NULL,NULL,NULL,NULL };	// platform that frog is currently on

/*	Note! This array is now only a handy helper for the hopping *to platform* code
	Don't try to use it to find the actual nearest platform, cause it isn't! */
PLATFORM *nearestPlatform[MAX_FROGS];
float nearestPlatDist[MAX_FROGS];

void CalcNextPlatformDest(PLATFORM *plat);

//----- [ PLATFORM UPDATE FUNCTIONS ] ------------------------------------------------------------

void UpdatePathPlatform(PLATFORM *);
void UpdatePathUpDownPlatform(PLATFORM *);
void UpdateUpDownPlatform(PLATFORM *);
void UpdateNonMovingPlatform(PLATFORM *);
void UpdateStepOnActivatedPlatform(PLATFORM *);
void UpdateSlerpPathPlatform(PLATFORM *);


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
	VECTOR fromPosition;
	int pl;

	if(platformList.numEntries == 0)
		return;

	for (pl = 0; pl < MAX_FROGS; pl++)
	{
		nearestPlatform[pl] = NULL;
		// don't bother with platforms outside our sphere of generosity
		
		// Sphere of Generosity. Sounds almost like an RPG skill
		// Roll 1 D12, spells less than 4 miss. 12 heals fully
		// Sphere restores 2 D12 HP to all sentients within 10 metres
		
		// But I digress.

		nearestPlatDist[pl] = (PLATFORM_NEAREST_DIST*PLATFORM_NEAREST_DIST);
	}

	for(cur = platformList.head.next; cur != &platformList.head; cur = next)
	{
		next = cur->next;

		// check if platform is active
		if(!cur->active)
		{
			// update regenerating platforms
			if(cur->flags & PLATFORM_NEW_REGENERATES)
			{
				if((cur->countdown -= gameSpeed) < 0)
				{
					//cur->pltActor->actor->xluOverride = 100;
					cur->pltActor->draw = 1;
					GetPositionForPathNode(&cur->pltActor->actor->pos, &cur->path->nodes[0]);
					cur->active	= 1;
					cur->pltActor->actor->xluOverride = 0;
					cur->countdown = -1;
				}
				else continue;
			}
			else continue;
		}

		// call update function for individual platform type
		if(cur->Update && !cur->isWaiting)
			cur->Update(cur);

		// keep track of nearest platforms
		for (pl = 0; frog[pl] && pl < MAX_FROGS; pl++)
		{
			float dist;

			// and only consider platforms in the dest tile?? (TODO: debug!)

			if (cur->inTile[0] != destTile[pl]) continue;

			dist = DistanceBetweenPointsSquared(&cur->pltActor->actor->pos, &frog[pl]->actor->pos);
			if (dist < nearestPlatDist[pl])
			{
				nearestPlatform[pl] = cur;
				nearestPlatDist[pl] = dist;
			}
		}

		// check if this platform is currently 'waiting' at a node
		if(cur->isWaiting)
		{
			if (((cur->isWaiting != -1) && (actFrameCount > cur->path->startFrame)) ||
				(cur->flags & PLATFORM_NEW_STEPONACTIVATED && cur->carrying))
				cur->isWaiting = 0;
			else
				continue;
		}

		// update frog if on the current platform
		if (cur->flags & PLATFORM_NEW_CARRYINGFROG)
		{
			// move frog with platform
			SetVector(&cur->carrying->actor->pos,&cur->pltActor->actor->pos);

			if(!cur->flags & PLATFORM_NEW_NONMOVING)
				SetQuaternion(&cur->carrying->actor->qRot,&cur->pltActor->actor->qRot);
		}

		// check if this is a disappearing or crumbling platform
		if(cur->flags & (PLATFORM_NEW_DISAPPEARWITHFROG| PLATFORM_NEW_CRUMBLES))
		{
			if (cur->pltActor->actor->xluOverride < 100)
			{
				if ((cur->pltActor->actor->xluOverride += (gameSpeed * 4)) > 100)
				{
					cur->pltActor->actor->xluOverride = 100;
					cur->pltActor->actor->flags &= ~OBJECT_FLAGS_XLU;
				}
				else
					cur->pltActor->actor->flags |= OBJECT_FLAGS_XLU;
			}
	
			if (cur->countdown < 0)
			{
				if (cur->flags & PLATFORM_NEW_CARRYINGFROG)
					cur->countdown = cur->path->nodes->speed;
			}
			else	// we're counting down...
			{
				// Wibble platform
				GetPositionForPathNode(&cur->pltActor->actor->pos, &cur->path->nodes[0]);
				cur->pltActor->actor->pos.v[X] += (-2 + Random(5));
				cur->pltActor->actor->pos.v[Y] += (-2 + Random(5));
				cur->pltActor->actor->pos.v[Z] += (-2 + Random(5));

				cur->countdown -= gameSpeed;

				if(cur->countdown <= 0)	// .. timer has reached zero; collapse
				{
					int i;

					if (cur->flags & PLATFORM_NEW_CARRYINGFROG)
					{
						// What frog are we carrying?
						for (i = 0, pl = -1; i<MAX_FROGS; i++)
							if (frog[i] == cur->carrying)
							{
								pl = i; break;
							}

						if (pl != -1)
						{
							currTile[pl] = destTile[pl] = cur->inTile[0];
							currPlatform[pl] = NULL;
							player[pl].frogState |= FROGSTATUS_ISJUMPINGTOTILE;
							cur->flags &= ~PLATFORM_NEW_CARRYINGFROG;
							cur->carrying = NULL;

							CalculateFrogJump(&frog[pl]->actor->pos, &destTile[pl]->centre, &destTile[pl]->normal, 0, 20, pl);
						}
					}

					//cur->pltActor->actor->xluOverride = 0;
					cur->pltActor->draw = 0;
					cur->active = 0;
					cur->carrying = NULL;
					cur->countdown = cur->path->nodes->waitTime;

					if( cur->path->nodes->sample )
						PlaySample( cur->path->nodes->sample, &cur->pltActor->actor->pos, 0, SAMPLE_VOLUME, -1 );

					if(cur->flags & PLATFORM_NEW_CRUMBLES)
					{
						//cur->countdown = cur->regenTime;	//0;
						CreateAndAddSpecialEffect( FXTYPE_SMOKEBURST, &cur->pltActor->actor->pos, &cur->inTile[0]->normal, 50, 4, 0, 1 );
						CreateAndAddSpecialEffect( FXTYPE_SMOKEBURST, &cur->pltActor->actor->pos, &cur->inTile[0]->normal, 40, 2, 0, 1.5 );
					}
				}
			}
		}
		else
		{
			// platform is not carrying a frog
			if(cur->countdown < cur->path->nodes->speed)
				cur->countdown += gameSpeed;
		}

		// special fx associated with platform
		if(cur->pltActor->effects)
			ProcessAttachedEffects((void *)cur,ENTITY_PLATFORM);
	}
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

	if(plat->path) JallocFree((UBYTE**)&plat->path);

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
	int pl;

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

	for (pl=0; pl<MAX_FROGS; pl++)
		currPlatform[pl] = NULL;

	// initialise list for future use
	InitPlatformLinkedList();
}


PLATFORM *CreateAndAddPlatform(char *pActorName,int flags,long ID,PATH *path,float animSpeed,unsigned char facing)
{
	int initFlags,i;
	int platformType = 0;
	float shadowRadius = 0;

	PLATFORM *newItem = (PLATFORM *)JallocAlloc(sizeof(PLATFORM),YES,"PLAT");
	AddPlatform(newItem);
	newItem->flags = flags;
	newItem->facing = facing;
	newItem->Update = NULL;

	initFlags |= INIT_ANIMATION;

	if( flags & PLATFORM_NEW_SHADOW )
	{
		initFlags |= INIT_SHADOW;
		shadowRadius = 20;
	}

	// create and add platform actor
	newItem->pltActor = CreateAndAddActor(pActorName,0,0,0,initFlags);
	newItem->pltActor->animSpeed = animSpeed;

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

	if (newItem->flags & PLATFORM_NEW_STEPONACTIVATED)
	{
		if (newItem->flags & PLATFORM_NEW_MOVEUP)
		{
			newItem->flags &= ~PLATFORM_NEW_MOVEUP;
			newItem->flags |= PLATFORM_NEW_RISEWITHFROG;
		}
		else if (newItem->flags & PLATFORM_NEW_MOVEDOWN)
		{
			newItem->flags &= ~PLATFORM_NEW_MOVEDOWN;
			newItem->flags |= PLATFORM_NEW_SINKWITHFROG;
		}
	}

	// determine relevant platform update function
	if (newItem->flags & PLATFORM_NEW_SLERPPATH)
		newItem->Update = UpdateSlerpPathPlatform;
	else if(newItem->flags & PLATFORM_NEW_FOLLOWPATH)
		newItem->Update = UpdatePathPlatform;
	else if(newItem->flags & (PLATFORM_NEW_MOVEUP | PLATFORM_NEW_MOVEDOWN))
	{
		newItem->Update = UpdateUpDownPlatform;
		Orientate( &newItem->pltActor->actor->qRot, &newItem->path->nodes->worldTile->dirVector[newItem->facing], &newItem->path->nodes->worldTile->normal );
	}
	else if(newItem->flags & PLATFORM_NEW_STEPONACTIVATED)
	{
		newItem->Update = UpdateStepOnActivatedPlatform;
		Orientate( &newItem->pltActor->actor->qRot, &newItem->path->nodes->worldTile->dirVector[newItem->facing], &newItem->path->nodes->worldTile->normal );
	}
	else if( (newItem->flags & PLATFORM_NEW_NONMOVING) || (newItem->pltActor->actor && newItem->path && newItem->path->nodes) )
		Orientate( &newItem->pltActor->actor->qRot, &newItem->path->nodes->worldTile->dirVector[newItem->facing], &newItem->path->nodes->worldTile->normal );

	return newItem;
}

void AssignPathToPlatform(PLATFORM *pform,PATH *path,unsigned long pathFlags)
{
	int i;
	VECTOR platformStartPos, fwd;

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
	else if(pform->flags & (PLATFORM_NEW_MOVEUP | PLATFORM_NEW_MOVEDOWN))
	{
		// this platform moves up or down
		pform->path->fromNode = pform->path->toNode = 0;
	}
	else
	{
		// this platform does not move - why use this at all ever?
//		pform->flags |= PLATFORM_NEW_NONMOVING;
		pform->path->fromNode = pform->path->toNode = 0;
	}

	// set platform position to relevant point on path
	if (pform->flags & PLATFORM_NEW_MOVEDOWN)
		GetPositionForPathNodeOffset2(&platformStartPos,&path->nodes[pform->path->fromNode]);
	else
		GetPositionForPathNode(&platformStartPos,&path->nodes[pform->path->fromNode]);

	SetVector(&pform->pltActor->actor->pos,&platformStartPos);
	//NormalToQuaternion(&pform->pltActor->actor->qRot,&path->nodes[pform->path->fromNode].worldTile->normal);

	SubVector(&fwd,
		&pform->path->nodes[pform->path->toNode].worldTile->centre,
		&pform->path->nodes[pform->path->fromNode].worldTile->centre);
	MakeUnit(&fwd);

	if (!(pform->flags & PLATFORM_NEW_FACEFORWARDS))
		Orientate(&pform->pltActor->actor->qRot, &fwd, &pform->path->nodes[pform->path->fromNode].worldTile->normal);
	else
		Orientate( &pform->pltActor->actor->qRot, &pform->path->nodes->worldTile->dirVector[pform->facing], &pform->path->nodes->worldTile->normal );

	// set platform current 'in' tile and speeds and pause times
	pform->inTile[0]	= path->nodes[pform->path->fromNode].worldTile;
	pform->currSpeed	= path->nodes[pform->path->fromNode].speed;
	pform->isWaiting	= path->nodes[pform->path->fromNode].waitTime;

	pform->path->startFrame = actFrameCount;
	pform->path->endFrame = (actFrameCount+(60*pform->currSpeed));
	pform->countdown = -1;

	CalcPlatformNormalInterps(pform);
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
	VECTOR fwd;

	path = pform->path;
	if(path->numNodes < 2)
		return;

	fromNode	= &path->nodes[path->fromNode];
	toNode		= &path->nodes[path->toNode];

	pform->srcOrientation = pform->pltActor->actor->qRot;

	if(!(pform->flags & PLATFORM_NEW_FACEFORWARDS))
	{
		SubVector(&fwd, &toNode->worldTile->centre, &fromNode->worldTile->centre);
		MakeUnit(&fwd);

		Orientate(&pform->destOrientation, &fwd, &toNode->worldTile->normal);
	}
	else
	{
		Orientate( &pform->destOrientation, &pform->path->nodes->worldTile->dirVector[pform->facing], &toNode->worldTile->normal );
	}

/*	 v v v v This isn't game speed independant at ALL! v v v v

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

	 ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^
*/

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

		if( path->nodes[path->fromNode].sample )
			PlaySample( path->nodes[path->fromNode].sample, &pform->pltActor->actor->pos, 0, SAMPLE_VOLUME, -1 );
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

		if( path->nodes[path->fromNode].sample )
			PlaySample( path->nodes[path->fromNode].sample, &pform->pltActor->actor->pos, 0, SAMPLE_VOLUME, -1 );
	}
	else if( flags & (PLATFORM_NEW_MOVEUP | PLATFORM_NEW_MOVEDOWN) )
	{
		if( flags & PLATFORM_NEW_PINGPONG )
		{
			pform->flags	^= (PLATFORM_NEW_MOVEUP | PLATFORM_NEW_MOVEDOWN);	// invert flags

			if( path->nodes->sample )
				PlaySample( path->nodes->sample, &pform->pltActor->actor->pos, 0, SAMPLE_VOLUME, -1 );
		}
		/*else
		{
			if( flags & PLATFORM_NEW_MOVEUP )
				GetPositionForPathNode( &pform->pltActor->actor->pos, &path->nodes[0] );
			else
				GetPositionForPathNodeOffset2( &pform->pltActor->actor->pos, &path->nodes[0] );
		}*/
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

	if (plat->isWaiting && (plat->Update))
	{
		//CalcNextPlatformDest(plat);
		plat->Update(plat);
	}
}


void FrogLeavePlatform(long pl)
{
	PLATFORM *plt;

	if ((plt = currPlatform[pl]))
	{
		plt->flags &= ~PLATFORM_NEW_CARRYINGFROG;	// plat is NOT carrying a frog
		plt->carrying = NULL;
		currPlatform[pl] = NULL;
	}
}


//----- [ PLATFORM UPDATE FUNCTIONS ] ------------------------------------------------------------

/*	--------------------------------------------------------------------------------
	Function		: UpdatePathSlerpPlatform
	Purpose			: Platforms that move in a smooth curve along a path (using Jim's slerpy code)
	Parameters		: PLATFORM*
	Returns			: void
	Info			: Use offset2 for slerp speed.
*/
void UpdateSlerpPathPlatform( PLATFORM *cur )
{
	QUATERNION q1, q2, q3;
	VECTOR fromPosition,toPosition, fwd, moveVec;
	PATH *path = cur->path;
	ACTOR *act = cur->pltActor->actor;
	float speed, t;
	long i;
	
	// Find the position of the current 2 nodes
	GetPositionForPathNode(&toPosition,&path->nodes[path->toNode]);
	
	// Set direction to desired point
	SubVector(&fwd,&toPosition,&act->pos);
	MakeUnit(&fwd);

	SetQuaternion( &q1, &act->qRot );

	// Skewer a line to rotate around, and make a rotation
	CrossProduct((VECTOR *)&q3,&inVec,&fwd);
	MakeUnit((VECTOR *)&q3);
	t = DotProduct(&inVec,&fwd);
	if (t<-0.999)
		t=-0.999;
	if (t>0.999)
		t = 0.999;
	q3.w=acos(t);
	// Duh.
	GetQuaternionFromRotation(&q2,&q3);

	// Slerp between current and desired orientation
	speed = path->nodes[path->fromNode].offset2 * gameSpeed;
	if( speed > 0.999 ) speed = 0.999;
	QuatSlerp( &q1, &q2, speed, &act->qRot );

	// Move forwards a bit in direction of facing
	RotateVectorByQuaternion( &fwd, &inVec, &act->qRot );
	ScaleVector( &fwd, cur->currSpeed*gameSpeed );

	AddToVector( &act->pos, &fwd );

//	AddToVector(&cur->currNormal,&cur->deltaNormal);

	// Update path nodes
	if( DistanceBetweenPointsSquared(&act->pos,&toPosition) < 100 )
	{
		UpdatePlatformPathNodes(cur);

		cur->path->startFrame = actFrameCount + cur->isWaiting * waitScale;
	}

	// Set inTile
	GetPositionForPathNode(&fromPosition,&path->nodes[path->fromNode]);
	GetPositionForPathNode(&toPosition,&path->nodes[path->toNode]);

	t = DistanceBetweenPoints(&fromPosition,&toPosition) / 2.0F;

	if(DistanceBetweenPointsSquared(&fromPosition,&act->pos) < (t*t))
		cur->inTile[0] = path->nodes[path->fromNode].worldTile;
	else
		cur->inTile[0] = path->nodes[path->toNode].worldTile;

	// We need to find which frog we're carrying (yeeeeeesh)
	for (i=0; i<4; i++)
		if (frog[i] == cur->carrying)
			break;

	if( i!=4 ) CheckTileForCollectable( cur->inTile[0], i );
}

/*	--------------------------------------------------------------------------------
	Function		: UpdatePathPlatform
	Purpose			: updates platforms that follow a path
	Parameters		: PLATFORM *
	Returns			: void
	Info			: 
*/
void UpdatePathPlatform(PLATFORM *plat)
{
	VECTOR fromPosition,toPosition,fwd,moveVec,norm;
	float length, n;
	long progress;

	// check if this platform has arrived at a path node
	if(actFrameCount >= plat->path->endFrame)
	{
		do {
			UpdatePlatformPathNodes(plat);
			plat->path->startFrame = plat->path->endFrame + plat->isWaiting * waitScale;
			plat->path->endFrame = plat->path->startFrame + (60*plat->currSpeed);
		}
		while (actFrameCount >= plat->path->endFrame && plat->isWaiting >= 0);

		if (plat->isWaiting) return;
	}
	else if (actFrameCount > ((plat->path->startFrame + plat->path->endFrame) / 2))
	{
		plat->inTile[0] = plat->path->nodes[plat->path->toNode].worldTile;
	}

	// update the platform position
	GetPositionForPathNode(&toPosition,&plat->path->nodes[plat->path->toNode]);
	GetPositionForPathNode(&fromPosition,&plat->path->nodes[plat->path->fromNode]);
	
	SubVector(&fwd,&toPosition,&fromPosition);
	
	progress = actFrameCount - plat->path->startFrame;
	if( !progress ) progress = 1;

	length = (float)progress/(float)(plat->path->endFrame - plat->path->startFrame);
	
	ScaleVector(&fwd,length);
	AddVector(&plat->pltActor->actor->pos,&fwd,&fromPosition);
	//MakeUnit(&fwd);

	if(!(plat->flags & PLATFORM_NEW_FACEFORWARDS))
	{
		//Orientate(&plat->pltActor->actor->qRot,&fwd,&inVec,&plat->currNormal);

		QuatSlerp(&plat->srcOrientation, &plat->destOrientation, length, &plat->pltActor->actor->qRot);
	}
	else
		Orientate( &plat->pltActor->actor->qRot, &plat->path->nodes->worldTile->dirVector[plat->facing], &plat->inTile[0]->normal );

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
	VECTOR moveVec;
	float start_offset, end_offset, t;

	// check if this platform has arrived at a path node
	if( actFrameCount >= plat->path->endFrame )
	{
		UpdatePlatformPathNodes(plat);

		plat->path->startFrame = plat->path->endFrame + plat->isWaiting * waitScale;
		plat->path->endFrame = plat->path->startFrame + (60*plat->currSpeed);

		if (plat->isWaiting) return;
	}

	if (plat->flags & PLATFORM_NEW_MOVEUP)
	{
		start_offset = plat->path->nodes->offset;
		end_offset = plat->path->nodes->offset2;
	}
	else
	{
		start_offset = plat->path->nodes->offset2;
		end_offset = plat->path->nodes->offset;
	}

	// get up vector for this platform
	SetVector(&moveVec,&plat->path->nodes[0].worldTile->normal);
	
	t = (float)(actFrameCount- plat->path->startFrame)/(float)(plat->path->endFrame - plat->path->startFrame);
	ScaleVector(&moveVec, t * end_offset + (1-t) * start_offset);
	AddVector(&plat->pltActor->actor->pos, &plat->path->nodes->worldTile->centre, &moveVec);
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
//	Orientate( &plat->pltActor->actor->qRot, &plat->path->nodes->worldTile->dirVector[plat->facing], &plat->path->nodes->worldTile->normal );
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
	VECTOR moveVec, v;
	float offs, startOffs, destOffs, speed;

	startOffs = plat->path->nodes->offset;
	destOffs = plat->path->nodes->offset2;

	// get up vector for this platform
	SetVector(&moveVec,&plat->path->nodes[0].worldTile->normal);

	SubVector(&v, &plat->pltActor->actor->pos, &plat->path->nodes->worldTile->centre);
	offs = DotProduct(&v, &moveVec);

	speed = plat->currSpeed * gameSpeed;

	ScaleVector(&moveVec, speed);

	// platform only moves when frog is it - otherwsie returns to start position
	if(plat->flags & PLATFORM_NEW_CARRYINGFROG)
	{
		if (offs > (destOffs + speed))
		{
			SubFromVector(&plat->pltActor->actor->pos, &moveVec);
		}
		else if (offs < (destOffs - speed))
		{
			AddToVector(&plat->pltActor->actor->pos, &moveVec);
		}
/*		

		TODO: generalise for multiplayer

		if(offs < 0 && plat->flags & PLATFORM_NEW_KILLSFROG)
		{
			if (!(player[0].frogState & FROGSTATUS_ISDEAD))
			{
				//AnimateActor(frog[0]->actor,FROG_ANIM_,NO,NO,0.5F,0,0);
				p;ayer[0].deathBy = DEATHBY_DROWNING;
				player[0].frogState |= FROGSTATUS_ISDEAD;
				player[0].dead = 50;
			}
		}
*/
	}
	else
	{
		if (offs > (startOffs + speed))
		{
			SubFromVector(&plat->pltActor->actor->pos, &moveVec);
		}
		else if (offs < (startOffs - speed))
		{
			AddToVector(&plat->pltActor->actor->pos, &moveVec);
		}
	}
}

/*	--------------------------------------------------------------------------------
    Function	: EnumPlatforms
	Purpose		: Calls a function for every platform with a given UID
	Parameters	: 
	Returns		: 

	func takes two params, the platform and the 'param' passed to EnumPlatforms
*/

int EnumPlatforms(long id, int (*func)(PLATFORM*, int), int param)
{
	PLATFORM *cur;
	int count;

	for(cur = platformList.head.next; cur != &platformList.head; cur = cur->next, count++)
	{
		if (!id || cur->uid == id)
		{
			if (!func(cur, param)) break;
		}
	}

	return count;
}

void SetPlatformVisible(PLATFORM *plt, int visible)
{
	if (visible)
	{
		plt->active = 1;
		plt->pltActor->draw = 1;
		plt->countdown = -1;
		
		if (plt->isWaiting != -1)
		{
			plt->isWaiting = -1;
			SetPlatformMoving(plt, 1);
		}
	}
	else
	{
		plt->pltActor->draw = 0;
		plt->active = 0;
	}
}

void SetPlatformMoving(PLATFORM *plt, int moving)
{
	if (moving)
	{
		if (plt->isWaiting)
		{
			plt->isWaiting = 0;
			plt->path->toNode = plt->path->fromNode;
			plt->path->startFrame = actFrameCount;
			plt->path->endFrame = actFrameCount + (60*plt->currSpeed);
		}
		//plt->Update(plt);
	}
	else
	{
		plt->isWaiting = -1;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: MovePlatform
	Purpose			: moves a platform to a given node in its path
	Parameters		: PLATFORM*, int [can be used with EnumPlatforms]
	Returns			: 1 for success
*/
int MovePlatformToNode(PLATFORM *plt, int flag)
{
	VECTOR fwd;

	if (flag >= 0 && flag < plt->path->numNodes)
	{
		plt->path->toNode = flag;
		plt->inTile[0] = plt->path->nodes[flag].worldTile;
		plt->path->endFrame = actFrameCount;
		plt->Update(plt);

		plt->pltActor->actor->qRot = plt->srcOrientation = plt->destOrientation;
	}
	else
		dprintf"MoveEnemyToNode(): Flag (%d) out of range\n", flag));

	return 1;
}

