/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: platform.h
	Programmer	: Andrew Eder
	Date		: 1/21/99

----------------------------------------------------------------------------------------------- */

#ifndef PLATFORM_H_INCLUDED
#define PLATFORM_H_INCLUDED

#define NODE_RANGETHRESHOLD				10.0F

#define PLATFORM_GENEROSITY				65.0F

// PLATFORM FLAGS

#define PLATFORM_NONE					0
#define PLATFORM_HASPATH				(1 << 0)	// platform has a path (more than one node)
#define PLATFORM_CARRYINGFROG			(1 << 1)	// platform currently carrying frog
#define PLATFORM_PATHFORWARDS			(1 << 2)	// platform moves forward thru nodes
#define PLATFORM_PATHBACKWARDS			(1 << 3)	// platform move backwards thru nodes
#define PLATFORM_PATHEND2START			(1 << 4)	// platform moves to start of path as soon as end is reached
#define PLATFORM_SEGMENT				(1 << 5)	// platform is a segment of a larger platform
#define PLATFORM_FLATLEVEL				(1 << 6)	// platform is moving over a flat level
#define PLATFORM_MOVEUP					(1 << 7)
#define PLATFORM_MOVEDOWN				(1 << 8)
#define PLATFORM_PATHBOUNCE				(1 << 9)
#define PLATFORM_PATHCYCLE				(1 << 10)
#define PLATFORM_UPDOWNONLY				(1 << 11)
#define PLATFORM_SINKABLE				(1 << 12)
#define PLATFORM_RISABLE				(1 << 13)

#define PLATFORM_CANWALKUNDER			(1 << 14)
#define PLATFORM_DISAPPEARING			(1 << 15)
#define PLATFORM_BRIDGE					(1 << 16)
#define PLATFORM_PATHFORWARD_BACK		(1 << 17)
#define PLATFORM_PATH_MOVEUPDOWN		(1 << 18)
#define PLATFORM_NOORIENTATE			(1 << 19)
#define PLATFORM_INWATER				(1 << 20)
#define PLATFORM_INACTIVE				(1 << 21)	// platform is inactive initially - e.g. triggered later
#define PLATFORM_RADIUSBASEDCOLLISION	(1 << 22)

typedef struct TAGPLATFORM
{
	struct TAGPLATFORM		*next,*prev;			// ptr to next / prev platform
	struct TAGPLATFORM		*pltPtrs[4];			// ptrs to adjacent platforms (if any)

	VECTOR					dirVector[4];			// 4 possible direction vectors
	VECTOR					initDirVector[4];		// initial 4 direction vectors
		
	ACTOR2					*pltActor;				// actor used as platform

	UBYTE					active;					// 1 = active ; 0 = non-active

	unsigned long			flags;					// platform flags
	unsigned long			originalFlags;			// copy of original flags

	float					currSpeed;				// platform current speed
	short					isWaiting;				// platform node pause time

	GAMETILE				*inTile;				// tile platform is currently 'in'

	PATH					*path;					// ptr to platform path data

	ACTOR2					*carrying;				// current actor platform is carrying

} PLATFORM;


typedef struct TAGPLATFORMLIST
{
	int				numEntries;
	PLATFORM		head;

} PLATFORMLIST;


extern PLATFORMLIST platformList;


extern PLATFORM *destPlatform[4];		// platform that frog is about to attempt to jump to
extern PLATFORM *currPlatform[4];		// platform that frog is currently on

extern GAMETILE	*oldTile[4];

extern void InitPlatformsForLevel(unsigned long worldID, unsigned long levelID);

extern void InitPlatformLinkedList();
extern void FreePlatformLinkedList();
extern void AddPlatform(PLATFORM *plat);
extern void SubPlatform(PLATFORM *plat);

extern void UpdatePlatforms();
extern void GetNextLocalPlatform(unsigned long direction);
extern PLATFORM *GetPlatformForTile(GAMETILE *tile);

extern BOOL PlatformTooHigh(PLATFORM *plat);
extern BOOL PlatformTooLow(PLATFORM *plat);


PLATFORM *GetPlatformFrogIsOn(long pl);
extern PLATFORM *JumpingToTileWithPlatform(GAMETILE *tile,long pl);

//------------------------------------------------------------------------------------------------

#define PLATFORM_NEW_NONE				0
#define PLATFORM_NEW_FOLLOWPATH			(1 << 0)	// platform follows a path (>1 node)
#define PLATFORM_NEW_FORWARDS			(1 << 1)	// platform moves forwards thru path nodes
#define PLATFORM_NEW_BACKWARDS			(1 << 2)	// platform moves backwards thru path nodes
#define PLATFORM_NEW_PINGPONG			(1 << 3)	// platform moves back and forth thru nodes
#define PLATFORM_NEW_CYCLE				(1 << 4)	// platform cycles thru nodes continuously
#define PLATFORM_NEW_MOVEUP				(1 << 5)	// platform moves up (1 node paths only)
#define PLATFORM_NEW_MOVEDOWN			(1 << 6)	// platform moves down (1 node paths only)
#define PLATFORM_NEW_STEPONACTIVATED	(1 << 7)	// platform is activated when frog is on it
#define PLATFORM_NEW_SINKWITHFROG		(1 << 8)	// platform sinks when frog is on it
#define PLATFORM_NEW_RISEWITHFROG		(1 << 9)	// platform sinks when frog is on it
#define PLATFORM_NEW_CARRYINGFROG		(1 << 10)	// platform is carrying a frog


PLATFORM *CreateAndAddPlatform(char *pActorName);
void AssignPathToPlatform(PLATFORM *pform,unsigned long platformFlags,PATH *path,unsigned long pathFlags);
BOOL PlatformHasArrivedAtNode(PLATFORM *pform);
BOOL PlatformReachedTopOrBottomPoint(PLATFORM *pform);
void UpdatePlatformPathNodes(PLATFORM *pform);

//------------------------------------------------------------------------------------------------


#endif
