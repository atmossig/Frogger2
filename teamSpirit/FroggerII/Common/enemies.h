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
};

// Enemy actor state types
enum
{
	NMESTATE_NONE,

	NMESTATE_SNAPPER_IDLE,
	NMESTATE_SNAPPER_READYTOSNAP,
	NMESTATE_SNAPPER_SNAPPING,

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


// ENEMY FLAGS
#define ENEMY_NONE				0
#define ENEMY_HASPATH			(1 << 0)
#define ENEMY_PATHCYCLE			(1 << 1)
#define ENEMY_PATHBOUNCE		(1 << 2)
#define ENEMY_PATHFORWARDS		(1 << 3)
#define ENEMY_PATHBACKWARDS		(1 << 4)
#define ENEMY_PATHEND2START		(1 << 5)
#define ENEMY_FLATLEVEL			(1 << 6)
#define ENEMY_NOJUMPOVER		(1 << 7)
#define ENEMY_PATHFORWARD_BACK	(1 << 8)
#define ENEMY_ACCELERATECONST	(1 << 9)
#define ENEMY_DECELERATECONST	(1 << 10)


typedef struct TAGENEMY
{
	struct TAGENEMY			*next,*prev;			// ptr to next / prev platform
	
	ACTOR2					*nmeActor;				// actor used as enemy
	unsigned long			flags;					// enemy flags
	
	float					startSpeed;				// enemy start speed
	float					endSpeed;				// enemy end speed
	float					speed;					// enemy current speed
	float					accel;					// enemy acceleration
	
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

extern void InitEnemiesForLevel(unsigned long worldID, unsigned long levelID);
extern ENEMY *CreateAndAddEnemy(char *eActorName,unsigned long *pathIndex,float offset,float offset2,int startNode,float eSpeed,unsigned long eFlags);

extern void InitEnemyLinkedList();
extern void FreeEnemyLinkedList();
extern void AddEnemy(ENEMY *enemy);
extern void SubEnemy(ENEMY *enemy);

extern PATH *CreateEnemyPathFromTileList(unsigned long *pIndex,float offset,float offset2);
extern void UpdateEnemies();

extern void ProcessNMEMower(ACTOR2 *nme);
extern void ProcessNMERoller(ACTOR2 *nme);

extern void ProcessNMESnapperPlant(ACTOR2 *nme);
extern void ProcessNMEWasp(ACTOR2 *nme);
extern void ProcessNMESnake(ACTOR2 *nme);
extern void ProcessNMECar(ACTOR2 *nme);
extern void ProcessNMETruck(ACTOR2 *nme);
extern void ProcessNMEDog(ACTOR2 *nme);
extern void ProcessNMEShark(ACTOR2 *nme);

extern BOOL EnemyHasArrivedAtNode(ENEMY *enemy, int toNodeID);


#endif
