/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: enemies.h
	Programmer	: Andrew Eder
	Date		: 1/12/99

----------------------------------------------------------------------------------------------- */

#ifndef ENEMIES_H_INCLUDED
#define ENEMIES_H_INCLUDED

// Enemy actor types
enum
{
	NMETYPE_NONE,
	NMETYPE_SNAPPER,
	NMETYPE_WASP,
	NMETYPE_SNAKE,
	NMETYPE_CAR,
	NMETYPE_TRUCK,

	NMETYPE_FORK,
	NMETYPE_RAT,
	NMETYPE_DOG,
	NMETYPE_GULL,
	NMETYPE_SHARK,

	NMETYPE_MOWER,
	NMETYPE_ROLLER,

	NMETYPE_MOLE,

};

// Enemy actor state types
enum
{
	NMESTATE_NONE,

	NMESTATE_SNAPPER_IDLE,
	NMESTATE_SNAPPER_READYTOSNAP,
	NMESTATE_SNAPPER_SNAPPING,

	NMESTATE_MOLE_IDLE,
	NMESTATE_MOLE_SNAPPING,
	NMESTATE_MOLE_UNDER_GROUND,
	NMESTATE_MOLE_LOOK,
	NMESTATE_MOLE_SCRATCH,


	NMESTATE_WASP_IDLE,
	NMESTATE_WASP_MOVING,

	NMESTATE_SNAKE_IDLE,
	NMESTATE_SNAKE_MOVING,
	NMESTATE_SNAKE_ATTACKING,

	NMESTATE_CAR_IDLE,
	NMESTATE_CAR_MOVING,

	NMESTATE_TRUCK_IDLE,
	NMESTATE_TRUCK_MOVING,

	NMESTATE_DOG_IDLE,
	NMESTATE_DOG_SNAPPING,
	NMESTATE_DOG_YAP,
	NMESTATE_DOG_RETURN,

	NMESTATE_SHARK_IDLE,

	NMESTATE_MOWER_IDLE,
	NMESTATE_ROLLER_IDLE,

};

/*
// ENEMY FLAGS
#define ENEMY_NONE					0
#define ENEMY_HASPATH				(1 << 0)
#define ENEMY_PATHCYCLE				(1 << 1)
#define ENEMY_PATHBOUNCE			(1 << 2)
#define ENEMY_PATHFORWARDS			(1 << 3)
#define ENEMY_PATHBACKWARDS			(1 << 4)
#define ENEMY_PATHEND2START			(1 << 5)
#define ENEMY_FLATLEVEL				(1 << 6)
#define ENEMY_NOJUMPOVER			(1 << 7)
#define ENEMY_PATHFORWARD_BACK		(1 << 8)
#define ENEMY_ACCELERATECONST		(1 << 9)
#define ENEMY_DECELERATECONST		(1 << 10)
#define ENEMY_RADIUSBASEDCOLLISION	(1 << 11)
*/

//----- [ PLATFORM FLAGS ] ---------------------------------------------------------------------//

#define ENEMY_NEW_NONE					0
#define ENEMY_NEW_FOLLOWPATH			(1 << 0)	// enemy follows a path (>1 node)
#define ENEMY_NEW_FORWARDS				(1 << 1)	// enemy moves forwards thru path nodes
#define ENEMY_NEW_BACKWARDS				(1 << 2)	// enemy moves backwards thru path nodes
#define ENEMY_NEW_PINGPONG				(1 << 3)	// enemy moves back and forth thru nodes
#define ENEMY_NEW_CYCLE					(1 << 4)	// enemy cycles thru nodes continuously
#define ENEMY_NEW_MOVEUP				(1 << 5)	// enemy moves up (1 node paths only)
#define ENEMY_NEW_MOVEDOWN				(1 << 6)	// enemy moves down (1 node paths only)
#define ENEMY_NEW_NOJUMPOVER			(1 << 7)	// enemy cannot be jumped over
#define ENEMY_NEW_RADIUSBASEDCOLLISION	(1 << 8)	// enemy collision based on radius
#define ENEMY_NEW_WATCHFROG				(1 << 9)	// enemy faces frogger (1 node paths only)
#define ENEMY_NEW_SNAPFROG				(1 << 10)	// enemy snaps at frogger (watching enemies only)
#define ENEMY_NEW_RANDOMSPEED			(1 << 11)	// enemy has random fluctuations in its speed
#define ENEMY_NEW_FACEFORWARDS			(1 << 12)	// enemy faces forwards
#define ENEMY_NEW_PUSHESFROG			(1 << 13)	// enemy pushes the frog, (Doesn't harm him!)
#define ENEMY_NEW_HOMING				(1 << 14)	// No path, moves blindly towards frog
#define ENEMY_NEW_SWARM					(1 << 15)	// No different mechanics, but does swarming thing about centre
#define ENEMY_NEW_ROTATEPATH_XZ			(1 << 16)	// Move in a circle around a point. Use tiles though.
#define ENEMY_NEW_ROTATEPATH_XY			(1 << 17)	// Move in a circle around a point. Use tiles though.
#define ENEMY_NEW_ROTATEPATH_ZY			(1 << 18)	// Move in a circle around a point. Use tiles though.


typedef struct TAGENEMY
{
	struct TAGENEMY			*next,*prev;			// ptr to next / prev enemy
	
	long					uid;					// Unique ID

	ACTOR2					*nmeActor;				// actor used as enemy
	unsigned long			flags;					// enemy flags
	
	float					startSpeed;				// enemy start speed
	float					speed;					// enemy current speed
	float					accel;					// enemy acceleration

	VECTOR					currNormal;
	VECTOR					deltaNormal;

	UBYTE					active;					// enemy active state
	short					isWaiting;				// enemy pause time at node
	long					isSnapping;				// enemy is snapping (snapping time left)
	short					isIdle;					// enemy idle state for starting animation

	GAMETILE				*inTile;				// tile enemy is currently 'in'
	PATH					*path;					// ptr to enemy path data

} ENEMY;


typedef struct TAGENEMYLIST
{
	int				numEntries;
	ENEMY			head;

} ENEMYLIST;


extern ENEMYLIST enemyList;

extern ENEMY *testEnemy;


// FUNCTION PROTOTYPES

void InitEnemiesForLevel(unsigned long worldID, unsigned long levelID);

void InitEnemyLinkedList();
void FreeEnemyLinkedList();
void AddEnemy(ENEMY *enemy);
void SubEnemy(ENEMY *enemy);

void UpdateEnemies();


void ProcessNMEMole ( ENEMY *nme );

void ProcessNMESnapperPlant(ACTOR2 *nme);
void ProcessNMECar(ACTOR2 *nme);
void ProcessNMEDog(ACTOR2 *nme);
void ProcessNMEMower(ACTOR2 *nme);

//------------------------------------------------------------------------------------------------

ENEMY *CreateAndAddEnemy(char *eActorName);
void AssignPathToEnemy(ENEMY *nme,unsigned long enemyFlags,PATH *path,unsigned long pathFlags);
BOOL EnemyHasArrivedAtNode(ENEMY *nme);
BOOL EnemyReachedTopOrBottomPoint(ENEMY *nme);
void UpdateEnemyPathNodes(ENEMY *nme);
void CalcEnemyNormalInterps(ENEMY *nme);
ENEMY *GetEnemyFromUID (long uid);

//------------------------------------------------------------------------------------------------

#endif
