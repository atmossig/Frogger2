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

//----- [ PLATFORM FLAGS ] ---------------------------------------------------------------------//

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
#define PLATFORM_NEW_DISAPPEARWITHFROG	(1 << 11)	// platform disappears when frog is on it (after period of time)
#define PLATFORM_NEW_CRUMBLES			(1 << 12)	// platform crumbles when frog is on it (after period of time)
#define PLATFORM_NEW_NONMOVING			(1 << 13)	// platform does not move
#define PLATFORM_NEW_REGENERATES		(1 << 14)	// platform regenerates after specified time
#define PLATFORM_NEW_NOWALKUNDER		(1 << 15)	// platform cannot be walked under
#define PLATFORM_NEW_KILLSFROG			(1 << 16)	// platform kills when it moves over frog
#define PLATFORM_NEW_MAKERIPPLES		(1 << 17)	// generate ripples under the platform
#define PLATFORM_NEW_MAKESMOKE			(1 << 18)	// generate smoke
#define PLATFORM_NEW_MAKESPARKS			(1 << 19)	// generate sparks
#define PLATFORM_NEW_FACEFORWARDS		(1 << 20)	// Face same direction


typedef struct TAGPLATFORM
{
	struct TAGPLATFORM		*next,*prev;			// ptr to next / prev platform
	struct TAGPLATFORM		*pltPtrs[4];			// ptrs to adjacent platforms (if any)

	long					uid;					// Unique ID

	VECTOR					dirVector[4];			// 4 possible direction vectors
	VECTOR					initDirVector[4];		// initial 4 direction vectors
		
	ACTOR2					*pltActor;				// actor used as platform

	UBYTE					active;					// 1 = active ; 0 = non-active

	unsigned long			flags;					// platform flags

	float					currSpeed;				// platform current speed
	short					isWaiting;				// platform node pause time

	VECTOR					currNormal;				// platform current normal
	VECTOR					deltaNormal;			// platform delta normal (for linear interp)

	short					visibleTime;			// platforms time visible if it disappears
	short					visible;				// platforms current visibility counter

	short					regenTime;				// platforms regeneration time
	short					regen;					// platforms current regeneration counter

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


extern void InitPlatformsForLevel(unsigned long worldID, unsigned long levelID);

extern void InitPlatformLinkedList();
extern void FreePlatformLinkedList();
extern void AddPlatform(PLATFORM *plat);
extern void SubPlatform(PLATFORM *plat);

extern void UpdatePlatforms();
extern void GetNextLocalPlatform(unsigned long direction);
extern PLATFORM *GetPlatformForTile(GAMETILE *tile);

extern BOOL PlatformTooHigh(PLATFORM *plat,long pl);
extern BOOL PlatformTooLow(PLATFORM *plat,long pl);

PLATFORM *GetPlatformFrogIsOn(long pl);
extern PLATFORM *JumpingToTileWithPlatform(GAMETILE *tile,long pl);

//------------------------------------------------------------------------------------------------

PLATFORM *CreateAndAddPlatform(char *pActorName);
void AssignPathToPlatform(PLATFORM *pform,unsigned long platformFlags,PATH *path,unsigned long pathFlags);
BOOL PlatformHasArrivedAtNode(PLATFORM *pform);
BOOL PlatformReachedTopOrBottomPoint(PLATFORM *pform);
void UpdatePlatformPathNodes(PLATFORM *pform);
void CalcPlatformNormalInterps(PLATFORM *pform);

void SetPlatformVisibleTime(PLATFORM *pform,short time);
void SetPlatformRegenerateTime(PLATFORM *pform,short time);

PLATFORM *GetNearestPlatformBelowFrog(GAMETILE *tile,long pl);
PLATFORM *GetPlatformFromUID(long uid);

//------------------------------------------------------------------------------------------------


#endif
