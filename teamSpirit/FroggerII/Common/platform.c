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


----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

#include <ultra64.h>

#include "incs.h"

/**************************************************************************************************************/
/******  GARDEN MAZE LEVEL  ***********************************************************************************/
/**************************************************************************************************************/

PLATFORM *lillyList1[4];

PATHNODE lilly1Nodes[]	= {	273,0,0,4,0,	115,0,0,4,0,	123,0,0,4,0,	132,0,0,4,0,
							139,0,0,4,0,	148,0,0,4,0,	158,0,0,4,0,	167,0,0,4,0 };

PATHNODE lilly2Nodes[]	= {	272,0,0,4,0,	114,0,0,4,0,	122,0,0,4,0,	131,0,0,4,0,
							138,0,0,4,0,	147,0,0,4,0,	157,0,0,4,0,	166,0,0,4,0 };

PATHNODE lilly3Nodes[]	= {	186,0,0,4,0,	188,0,0,4,0,	189,0,0,4,0,	190,0,0,4,0,
							191,0,0,4,0,	192,0,0,4,0,	193,0,0,4,0,	194,0,0,4,0 };

PATHNODE lilly4Nodes[]	= {	207,0,0,4,0,	208,0,0,4,0,	209,0,0,4,0,	210,0,0,4,0,
							211,0,0,4,0,	212,0,0,4,0,	213,0,0,4,0,	214,0,0,4,0 };

PATH lilly1Path[]		= { 8,0,0,0,lilly1Nodes };
PATH lilly2Path[]		= { 8,0,0,0,lilly2Nodes };
PATH lilly3Path[]		= { 8,0,0,0,lilly3Nodes };
PATH lilly4Path[]		= { 8,0,0,0,lilly4Nodes };


/**************************************************************************************************************/
/******  GARDEN VEGPATCH LEVEL  *******************************************************************************/
/**************************************************************************************************************/

PLATFORM *lillyList2[2];

unsigned long lillyPath8[]		 = { 14,	178,187,199,208,217,226,227,228,219,210,201,189,180,179 };
unsigned long lillyHeightPath8[] = { 14,	0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
float		  lillySpeedPath8[]  = { 14,	4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0 };

	   
PLATFORMLIST platformList;					// the platform list

PLATFORM *destPlatform[4];					// platform that frog is about to attempt to jump to
PLATFORM *currPlatform[4];					// platform that frog is currently on
PLATFORM *nearestPlatform[4];				// platform nearest to the frog

GAMETILE *oldTile[4];

PLATFORM *devPlat1	= NULL;
PLATFORM *devPlat2	= NULL;



PATHNODE debug_pathNodes1[] =					// TEST PATH - ANDYE
{ 
	20,20,0,6,0,	33,20,0,6,0,	46,20,0,6,0,	59,20,0,6,0,	72,20,0,6,0,	85,20,0,6,0,
	98,20,0,6,0,	111,20,0,6,0,	112,20,0,6,0,	113,20,0,6,0,	502,20,0,6,0,	499,20,0,6,0,	
	500,20,0,6,0,	501,20,0,6,0,	504,20,0,6,0,	114,20,0,6,0,	115,20,0,6,0,	105,20,0,6,0,
	92,20,0,6,0,	79,20,0,6,0,	66,20,0,6,0,	65,20,0,6,0,	64,20,0,6,0,	51,20,0,6,0,
	38,20,0,6,0,	37,20,0,6,0,	36,20,0,6,0,	23,20,0,6,0,	15,20,0,6,0,	11,20,0,6,0,
	10,20,0,6,0,	9,20,0,6,0,		8,20,0,6,0,		12,20,0,6,0
};

PATH debug_path1 = { 34,0,0,0,debug_pathNodes1 };

PATHNODE debug_pathNodes2[] = { 14,5,45,1,0 };	// TEST PATH - ANDYE
PATH debug_path2 = { 1,0,0,0,debug_pathNodes2 };


static void	GetActiveTile(PLATFORM *pform);


/*	--------------------------------------------------------------------------------
	Function		: InitPlatformsForLevel
	Purpose			: initialise platforms and their movement for specified level
	Parameters		:
	Returns			: void
	Info			:
*/
void InitPlatformsForLevel(unsigned long worldID, unsigned long levelID)
{
	int i,c;

	dprintf"Initialising platforms (world %d, level %d)...\n",worldID,levelID));

	if(worldID == WORLDID_GARDEN)
	{
		if(levelID == LEVELID_GARDENLAWN)
		{
			devPlat1 = CreateAndAddPlatform("pltlilly.ndo");
			devPlat1->currSpeed = 6.0F;
			AssignPathToPlatform(devPlat1,PLATFORM_NEW_FORWARDS | PLATFORM_NEW_CYCLE,&debug_path1,PATH_MAKENODETILEPTRS);

			devPlat2 = CreateAndAddPlatform("pltlilly.ndo");
			devPlat2->currSpeed = 2.0F;
			AssignPathToPlatform(devPlat2,PLATFORM_NEW_MOVEUP | PLATFORM_NEW_STEPONACTIVATED,&debug_path2,PATH_MAKENODETILEPTRS);
		}

		if(levelID == LEVELID_GARDENMAZE)
		{
			lillyList1[0] = CreateAndAddPlatform("pltlilly.ndo");
			AssignPathToPlatform(lillyList1[0],PLATFORM_NEW_FORWARDS | PLATFORM_NEW_PINGPONG,&lilly1Path,PATH_MAKENODETILEPTRS);
			lillyList1[1] = CreateAndAddPlatform("pltlilly.ndo");
			AssignPathToPlatform(lillyList1[1],PLATFORM_NEW_BACKWARDS | PLATFORM_NEW_PINGPONG,&lilly2Path,PATH_MAKENODETILEPTRS);
			lillyList1[2] = CreateAndAddPlatform("pltlilly.ndo");
			AssignPathToPlatform(lillyList1[2],PLATFORM_NEW_FORWARDS | PLATFORM_NEW_PINGPONG,&lilly3Path,PATH_MAKENODETILEPTRS);
			lillyList1[3] = CreateAndAddPlatform("pltlilly.ndo");
			AssignPathToPlatform(lillyList1[3],PLATFORM_NEW_BACKWARDS | PLATFORM_NEW_PINGPONG,&lilly4Path,PATH_MAKENODETILEPTRS);
		}	   

		if(levelID == LEVELID_GARDENVEGPATCH)
		{
		}
	}

	else if(worldID == WORLDID_SUPERRETRO)
	{
	}
	
	else if(worldID == WORLDID_CITY)
	{
		if(levelID == LEVELID_CITYDOCKS)
		{
		}
		else if(levelID == LEVELID_CITYSTREETS)
		{
		}
		else if(levelID == LEVELID_CITYWAREHOUSE)
		{
		}
		else if(levelID == LEVELID_CITYBONUS)
		{
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: UpdatePlatforms
	Purpose			: updates all platforms and their states / movements, etc.
	Parameters		: 
	Returns			: void
	Info			: 
*/
void UpdatePlatforms()
{
	PLATFORM *cur,*next;
	VECTOR fromPosition,toPosition;
	VECTOR fwd;
	VECTOR moveVec;
	float lowest,t;
	int i,newCamFacing,newFrogFacing;
		
	if(platformList.numEntries == 0)
		return;
	
	for(cur = platformList.head.next; cur != &platformList.head; cur = next)
	{
		next = cur->next;

		if(!cur->active)
			continue;

		// check if this platform is currently 'waiting' at a node
		if(cur->isWaiting)
		{
			cur->isWaiting--;
			continue;
		}

		if(cur->flags & PLATFORM_NEW_FOLLOWPATH)
		{
			// process platforms that follow a path (>1 node in path)

			// first, update the platform position
			GetPositionForPathNode(&toPosition,&cur->path->nodes[cur->path->toNode]);
			SubVector(&fwd,&toPosition,&cur->pltActor->actor->pos);
			MakeUnit(&fwd);

			cur->pltActor->actor->pos.v[X] += (cur->currSpeed * fwd.v[X]);
			cur->pltActor->actor->pos.v[Y] += (cur->currSpeed * fwd.v[Y]);
			cur->pltActor->actor->pos.v[Z] += (cur->currSpeed * fwd.v[Z]);

			// check if this platform has arrived at a path node
			if(PlatformHasArrivedAtNode(cur))
				UpdatePlatformPathNodes(cur);
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
							SubFromVector(&cur->pltActor->actor->pos,&moveVec);
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
					if(DistanceBetweenPointsSquared(&cur->pltActor->actor->pos,&fromPosition) > 25.0F)
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
		oldTile[0] = currTile[0];
		GetActiveTile(cur);

		// determine if platform is carrying frog
		if(cur->flags & PLATFORM_NEW_CARRYINGFROG)
		{
			currTile[0] = cur->inTile;
			cur->carrying = frog[0];
			SetVector(&cur->carrying->actor->pos,&cur->pltActor->actor->pos);
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: JumpingToTileWithPlatform
	Purpose			: checks if frog is jumping to a tile that has a platform
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

	// go thru platform list and check for platform in the specified tile
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
			// check if this platform can be walked under
			if(cur->flags & PLATFORM_CANWALKUNDER)
			{
				// check height of platform
				if(Fabs(cur->pltActor->actor->pos.v[Y] - frog[pl]->actor->pos.v[Y]) > 25.0F)
				{
					player[pl].frogState |= FROGSTATUS_ISJUMPINGTOTILE;
					player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOPLATFORM;
					return NULL;
				}
			}

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
	Function		: GetActiveTile
	Purpose			: gets the currently active tile for the platform
	Parameters		: PLATFORM *
	Returns			: void
	Info			: 
*/
static void	GetActiveTile(PLATFORM *pform)
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
	Parameters		: PLATFORM *
	Returns			: BOOL
	Info			: 
*/
BOOL PlatformTooHigh(PLATFORM *plat)
{
	VECTOR vec;
	float height;

	height = (plat->pltActor->actor->pos.v[Y] - frog[0]->actor->pos.v[Y]);

	if(height > 50.0F)
	{
		// platform cannot be jumped up to
		return TRUE;
	}
	
	if(height > 25.0F && !superHop)
	{
		// platform too high - need superhop for this jump up
		return TRUE;
	}
	
	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function		: PlatformTooLow
	Purpose			: checks is destination platform is too low to jump to
	Parameters		: PLATFORM *
	Returns			: BOOL
	Info			: 
*/
BOOL PlatformTooLow(PLATFORM *plat)
{
	VECTOR vec;
	float height;

	height = (plat->pltActor->actor->pos.v[Y] - frog[0]->actor->pos.v[Y]);

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

	newItem->pltActor = CreateAndAddActor(pActorName,0,0,0,INIT_ANIMATION,0,0);
	if(newItem->pltActor->actor->objectController)
	{
		InitActorAnim(newItem->pltActor->actor);
		AnimateActor(newItem->pltActor->actor,0,YES,NO,1.0F);
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

	return newItem;
}

void AssignPathToPlatform(PLATFORM *pform,unsigned long platformFlags,PATH *path,unsigned long pathFlags)
{
	int i;
	VECTOR platformStartPos;

	// assign the path to this platform
	pform->flags	|= platformFlags;
	pform->path		= path;

	dprintf"Add path : "));

	// check if pathnode indices need converting to game tile pointers
	if(pathFlags & PATH_MAKENODETILEPTRS)
	{
		for(i=0; i<path->numNodes; i++)
		{
			// convert integer to a valid game tile
			dprintf"%d, ",(int)path->nodes[i].worldTile));
			pform->path->nodes[i].worldTile = &firstTile[(int)path->nodes[i].worldTile];
		}
	}

	if(platformFlags & PLATFORM_NEW_FORWARDS)
	{
		// this platform moves forward thru path nodes
		pform->flags			|= PLATFORM_NEW_FOLLOWPATH;
		pform->path->fromNode	= 0;
		pform->path->toNode		= 1;
	}
	else if(platformFlags & PLATFORM_NEW_BACKWARDS)
	{
		// this platform moves backward thru path nodes
		pform->flags			|= PLATFORM_NEW_FOLLOWPATH;
		pform->path->fromNode	= GET_PATHLASTNODE(path);
		pform->path->toNode		= GET_PATHLASTNODE(path) - 1;
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

	// set platform position to relevant point on path
	GetPositionForPathNode(&platformStartPos,&path->nodes[pform->path->fromNode]);
	SetVector(&pform->pltActor->actor->pos,&platformStartPos);

	// set platform current 'in' tile and speeds and pause times
	pform->inTile		= path->nodes[pform->path->fromNode].worldTile;
	pform->currSpeed	= path->nodes[pform->path->fromNode].speed;
	pform->isWaiting	= path->nodes[pform->path->fromNode].waitTime;

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

	// check if path node is reached
	GetPositionForPathNode(&nodePos,&path->nodes[path->toNode]);
	if(DistanceBetweenPointsSquared(&pform->pltActor->actor->pos,&nodePos) <  ((pform->currSpeed + 0.1F) * (pform->currSpeed + 0.1F)))
	{
		dprintf""));
		return TRUE;
	}

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
	int nextToNode,nextFromNode;
	PATH *path = pform->path;
	unsigned long flags = pform->flags;

	// determine to/from nodes for the current platform

	if(flags & PLATFORM_NEW_FORWARDS)
	{
		// platform moves forward through path nodes
		nextToNode = path->toNode + 1;

		if(nextToNode > GET_PATHLASTNODE(path))
		{
			// reached end of path nodes
			// check if this platform has ping-pong movement
			if(flags & PLATFORM_NEW_PINGPONG)
			{
				// reverse platform path movement
				pform->flags	&= ~PLATFORM_NEW_FORWARDS;
				pform->flags	|= PLATFORM_NEW_BACKWARDS;
				path->fromNode	= GET_PATHLASTNODE(path);
				path->toNode	= GET_PATHLASTNODE(path) - 1;

				pform->currSpeed = GetSpeedFromIndexedNode(path,path->fromNode);
				pform->isWaiting = GetWaitTimeFromIndexedNode(path,path->fromNode);
				return;
			}
			else if(flags & PLATFORM_NEW_CYCLE)
			{
				// platform has cyclic movement
				path->fromNode	= GET_PATHLASTNODE(path);
				path->toNode	= 0;
				
				pform->currSpeed = GetSpeedFromIndexedNode(path,path->fromNode);
				pform->isWaiting = GetWaitTimeFromIndexedNode(path,path->fromNode);
				return;
			}

			path->fromNode	= 0;
			path->toNode	= 1;

			pform->currSpeed = GetSpeedFromIndexedNode(path,path->fromNode);
			pform->isWaiting = GetWaitTimeFromIndexedNode(path,path->fromNode);

			GetPositionForPathNode(&pformPos,&path->nodes[0]);
			SetVector(&pform->pltActor->actor->pos,&pformPos);
		}
		else
		{
			nextFromNode = path->fromNode + 1;

			if((pform->flags & PLATFORM_NEW_CYCLE) && (nextFromNode > GET_PATHLASTNODE(path)))
			{
				path->fromNode	= 0;
				path->toNode	= 1;

				pform->currSpeed = GetSpeedFromIndexedNode(path,path->fromNode);
				pform->isWaiting = GetWaitTimeFromIndexedNode(path,path->fromNode);
				return;
			}
			
			path->fromNode++;
			path->toNode++;

			pform->currSpeed = GetSpeedFromIndexedNode(path,path->fromNode);
			pform->isWaiting = GetWaitTimeFromIndexedNode(path,path->fromNode);
		}
	}
	else if(flags & PLATFORM_NEW_BACKWARDS)
	{
		// platform moves backwards through path nodes
		nextToNode = path->toNode - 1;

		if(nextToNode < 0)
		{
			// reached beginning of path nodes
			// check if this platform has ping-pong movement
			if(flags & PLATFORM_NEW_PINGPONG)
			{
				// reverse platform path movement
				pform->flags	&= ~PLATFORM_NEW_BACKWARDS;
				pform->flags	|= PLATFORM_NEW_FORWARDS;
				path->fromNode	= 0;
				path->toNode	= 1;

				pform->currSpeed = GetSpeedFromIndexedNode(path,path->fromNode);
				pform->isWaiting = GetWaitTimeFromIndexedNode(path,path->fromNode);
				return;
			}
			else if(flags & PLATFORM_NEW_CYCLE)
			{
				// platform has cyclic movement
				path->fromNode	= 0;
				path->toNode	= GET_PATHLASTNODE(path);

				pform->currSpeed = GetSpeedFromIndexedNode(path,path->fromNode);
				pform->isWaiting = GetWaitTimeFromIndexedNode(path,path->fromNode);
				return;
			}

			path->fromNode	= GET_PATHLASTNODE(path);
			path->toNode	= GET_PATHLASTNODE(path) - 1;

			pform->currSpeed = GetSpeedFromIndexedNode(path,path->fromNode);
			pform->isWaiting = GetWaitTimeFromIndexedNode(path,path->fromNode);

			GetPositionForPathNode(&pformPos,&path->nodes[GET_PATHLASTNODE(path)]);
			SetVector(&pform->pltActor->actor->pos,&pformPos);
		}
		else
		{
			nextFromNode = path->fromNode - 1;

			if((pform->flags & PLATFORM_NEW_CYCLE) && (nextFromNode < 0))
			{
				path->fromNode	= GET_PATHLASTNODE(path);
				path->toNode	= GET_PATHLASTNODE(path) - 1;

				pform->currSpeed = GetSpeedFromIndexedNode(path,path->fromNode);
				pform->isWaiting = GetWaitTimeFromIndexedNode(path,path->fromNode);
				return;
			}

			path->fromNode--;
			path->toNode--;

			pform->currSpeed = GetSpeedFromIndexedNode(path,path->fromNode);
			pform->isWaiting = GetWaitTimeFromIndexedNode(path,path->fromNode);
		}
	}
	else if(flags & PLATFORM_NEW_MOVEUP)
	{
		// path has reached 'top' of path
		// check if this platform has ping-pong movement
		if(flags & PLATFORM_NEW_PINGPONG)
		{
			// reverse platform movement
			pform->flags	&= ~PLATFORM_NEW_MOVEUP;
			pform->flags	|= PLATFORM_NEW_MOVEDOWN;

			pform->currSpeed = GetSpeedFromIndexedNode(path,path->fromNode);
			pform->isWaiting = GetWaitTimeFromIndexedNode(path,path->fromNode);
			return;
		}
	}
	else if(flags & PLATFORM_NEW_MOVEDOWN)
	{
		// path has reached 'bottom' of path
		// check if this platform has ping-pong movement
		if(flags & PLATFORM_NEW_PINGPONG)
		{
			// reverse platform movement
			pform->flags	|= PLATFORM_NEW_MOVEUP;
			pform->flags	&= ~PLATFORM_NEW_MOVEDOWN;

			pform->currSpeed = GetSpeedFromIndexedNode(path,path->fromNode);
			pform->isWaiting = GetWaitTimeFromIndexedNode(path,path->fromNode);
			return;
		}
	}
}


//------------------------------------------------------------------------------------------------
