/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: enemies.c
	Programmer	: Andrew Eder
	Date		: 1/12/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

#include <ultra64.h>

#include "incs.h"


ENEMY *testEnemy			= NULL;			// test enemy

ENEMYLIST enemyList;						// the enemy linked list

VECTOR snapPos;


// TEST ARRAYS DEFINING MOVEMENT PATHS - ANDYE //
// First number in array is number of nodes in path //

/**************************************************************************************************************/
/******  GARDEN LAWN LEVEL  ***********************************************************************************/
/**************************************************************************************************************/

unsigned long lawnPath1[] = { 17,	287,284,42,43,44,45,46,47,48,49,50,51,52,53,54,278,281 };
unsigned long lawnPath2[] = { 17,	288,285,68,69,70,71,72,73,74,75,76,77,78,79,80,279,282 };
unsigned long lawnPath3[] = { 17,	289,286,94,95,96,97,98,99,100,101,102,103,104,105,106,280,283 };

unsigned long lawnPath4[] = { 55,	141,142,143,144,145,146,147,148,149,150,
									151,152,153,166,179,178,177,176,175,174,
									173,172,171,170,169,168,167,180,193,194,
									195,196,197,198,199,200,201,202,203,204,
									205,218,231,230,229,228,227,226,225,224,
									223,222,221,220,219 };

unsigned long lawnPath5[] = { 8,	235,236,237,250,261,260,259,248 };
unsigned long lawnPath6[] = { 8,	238,239,240,253,264,263,262,251 };

unsigned long lawnPath7[] = { 8,	415,416,417,428,441,440,439,426 };
unsigned long lawnPath8[] = { 8,	421,422,423,434,447,446,445,432 };
unsigned long lawnPath9[] = { 8,	467,468,469,482,493,492,491,480 };
unsigned long lawnPath10[] = { 8,	461,462,463,476,487,486,485,474 };
unsigned long lawnPath11[] = { 8,	453,454,455,466,479,478,477,464 };

unsigned long lawnPath12[] = { 8,	403,414,425,438,449,460,473,484 };

unsigned long lawnPath13[] = { 8,	232,233,234,247,258,257,256,245 };
unsigned long lawnPath14[] = { 8,	241,242,243,294,295,266,265,254 };

unsigned long lawnPath15[] = { 7,	581,578,514,515,516,572,575 };
unsigned long lawnPath16[] = { 7,	582,579,520,521,522,573,576 };
unsigned long lawnPath17[] = { 7,	583,580,526,527,528,574,577 };

unsigned long lawnPath18[] = { 13,	532,533,534,535,536,541,546,551,571,554,553,552,570 };
unsigned long lawnPath19[] = { 8,	538,539,540,545,550,549,548,543 };


/**************************************************************************************************************/
/******  GARDEN MAZE LEVEL  ***********************************************************************************/
/**************************************************************************************************************/

unsigned long mazePath1[] = { 12,	5,369,6,368,7,17,30,29,28,27,26,16 };




/*unsigned long mazePath2[] = { 9,	9,19,32,39,51,63,76,86,98 };
unsigned long mazePath3[] = { 13,	3,15,26,37,44,57,70,81,92,91,90,89,88 };


unsigned long mazePath4[] = { 8,	41,42,43,56,69,68,67,55 };
unsigned long mazePath5[] = { 8,	153,154,155,164,173,172,171,163 };
unsigned long mazePath6[] = { 4,	120,121,129,128 };

unsigned long mazePath7[] = { 7,	170,162,151,143,135,127,119 };
unsigned long mazePath8[] = { 7,	169,161,150,141,134,125,118 };
unsigned long mazePath9[] = { 7,	168,159,149,140,133,124,117 };

unsigned long mazePath10[] = { 12,	46,47,48,49,50,62,75,74,73,72,71,58 };
			*/

/*unsigned long rotoPath11[] = { 13,	3,15,26,37,44,57,70,81,92,91,90,89,88 };
unsigned long rotoPath12[] = { 10,	9,19,32,39,51,63,76,86,98,108 };
unsigned long rotoPath13[] = { 5,	27,28,29,30,31 };
unsigned long rotoPath14[] = { 7,	127,136,150,161,175,190,200 };
unsigned long rotoPath15[] = { 7,	128,137,151,163,176,192,201 };
unsigned long rotoPath16[] = { 7,	129,139,152,165,177,193,202 };

unsigned long waspPath12[] = { 8,	41,55,67,68,69,56,43,42 };
unsigned long waspPath13[] = { 4,	47,59,60,48 };
unsigned long waspPath14[] = { 4,	49,61,62,50 };
unsigned long waspPath15[] = { 4,	71,82,83,72 };
unsigned long waspPath16[] = { 4,	74,84,85,75 };
unsigned long waspPath17[] = { 8,	179,194,203,204,205,195,181,180 };
unsigned long waspPath18[] = { 8,	10,20,33,34,35,22,12,11 };
unsigned long waspPath19[] = { 4,	130,140,141,131 };
unsigned long waspPath20[] = { 5,	100,101,109,118,119 };
				  */
/**************************************************************************************************************/
/******  GARDEN VEGPATCH LEVEL  *******************************************************************************/
/**************************************************************************************************************/

unsigned long rotoPath17[] = { 11,	7,16,24,32,41,49,58,67,75,84,90 };
unsigned long rotoPath18[] = { 11,	8,17,25,33,42,50,59,68,76,85,91 };
unsigned long rotoPath19[] = { 11,	9,18,26,34,43,51,60,69,77,86,92 };

unsigned long rotoPath20[] = { 29,	320,319,15,14,13,12,20,28,29,30,31,40,48,47,46,45,54,63,64,65,66,74,83,82,88,94,95,317,318 };

unsigned long rotoPath21[] = { 9,	191,203,212,221,230,239,246,321,322 };
unsigned long rotoPath22[] = { 7,	214,223,232,241,248,323,324 };

/*unsigned long waspPath12[] = { 8,	41,55,67,68,69,56,43,42 };
unsigned long waspPath13[] = { 4,	47,59,60,48 };
unsigned long waspPath14[] = { 4,	49,61,62,50 };
unsigned long waspPath15[] = { 4,	71,82,83,72 };
unsigned long waspPath16[] = { 4,	74,84,85,75 };
unsigned long waspPath17[] = { 8,	179,194,203,204,205,195,181,180 };
unsigned long waspPath18[] = { 8,	10,20,33,34,35,22,12,11 };
unsigned long waspPath19[] = { 4,	130,140,141,131 };
unsigned long waspPath20[] = { 5,	100,101,109,118,119 };
  */

/////////////////////////////////////////////////////////////////////////////////////
unsigned long gatePath1[]    = { 1,		12};
unsigned long gatePath2[]    = { 1,		15};


static BOOL JumpedOnEnemy(ENEMY *enemy);
static void GetActiveTile(ENEMY *enemy);

/*	--------------------------------------------------------------------------------
	Function		: InitEnemiesForLevel
	Purpose			: initialise enemies and their movement for the specified level
	Parameters		: unsigned long
	Returns			: void
	Info			:
*/
void InitEnemiesForLevel(unsigned long worldID, unsigned long levelID)
{
	dprintf"Initialising enemies for level...\n"));

	if ( worldID == WORLDID_GARDEN )
	{
		if ( levelID == LEVELID_GARDENLAWN )
		{
			testEnemy = CreateAndAddEnemy("roll.ndo",lawnPath1,0,0,5,5.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHEND2START | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("roll.ndo",lawnPath1,0,0,12,5.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHEND2START | ENEMY_FLATLEVEL);

			testEnemy = CreateAndAddEnemy("roll.ndo",lawnPath2,0,0,5,5.0F, ENEMY_HASPATH | ENEMY_PATHBACKWARDS | ENEMY_PATHEND2START | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("roll.ndo",lawnPath2,0,0,10,5.0F, ENEMY_HASPATH | ENEMY_PATHBACKWARDS | ENEMY_PATHEND2START | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("roll.ndo",lawnPath2,0,0,15,5.0F, ENEMY_HASPATH | ENEMY_PATHBACKWARDS | ENEMY_PATHEND2START | ENEMY_FLATLEVEL);

			testEnemy = CreateAndAddEnemy("moa.ndo",lawnPath3,0,0,2,10.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHEND2START | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("moa.ndo",lawnPath3,0,0,9,10.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHEND2START | ENEMY_FLATLEVEL);

			testEnemy = CreateAndAddEnemy("moa.ndo",lawnPath4,0,0,3,5.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHEND2START | ENEMY_FLATLEVEL | ENEMY_ACCELERATECONST);
			testEnemy = CreateAndAddEnemy("moa.ndo",lawnPath4,0,0,13,5.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHEND2START | ENEMY_FLATLEVEL | ENEMY_ACCELERATECONST);
			testEnemy = CreateAndAddEnemy("moa.ndo",lawnPath4,0,0,23,5.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHEND2START | ENEMY_FLATLEVEL | ENEMY_ACCELERATECONST);
			testEnemy = CreateAndAddEnemy("moa.ndo",lawnPath4,0,0,33,5.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHEND2START | ENEMY_FLATLEVEL | ENEMY_ACCELERATECONST);
			testEnemy = CreateAndAddEnemy("moa.ndo",lawnPath4,0,0,43,5.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHEND2START | ENEMY_FLATLEVEL | ENEMY_ACCELERATECONST);

			testEnemy = CreateAndAddEnemy("wasp.ndo",lawnPath5,35,35,3,3.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHCYCLE | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("wasp.ndo",lawnPath6,35,35,5,3.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHCYCLE | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("wasp.ndo",lawnPath7,35,35,1,5.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHCYCLE | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("wasp.ndo",lawnPath8,35,35,2,5.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHCYCLE | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("wasp.ndo",lawnPath9,35,35,3,5.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHCYCLE | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("wasp.ndo",lawnPath10,35,35,4,5.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHCYCLE | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("wasp.ndo",lawnPath11,35,35,5,5.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHCYCLE | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("roll.ndo",lawnPath12,0,0,5,5.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHBOUNCE | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("wasp.ndo",lawnPath13,35,35,1,5.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHCYCLE | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("wasp.ndo",lawnPath14,35,35,6,5.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHCYCLE | ENEMY_FLATLEVEL);

			testEnemy = CreateAndAddEnemy("roll.ndo",lawnPath15,0,0,2,3.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHBOUNCE | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("roll.ndo",lawnPath16,0,0,3,3.0F, ENEMY_HASPATH | ENEMY_PATHBACKWARDS | ENEMY_PATHBOUNCE | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("roll.ndo",lawnPath17,0,0,2,3.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHBOUNCE | ENEMY_FLATLEVEL);

			testEnemy = CreateAndAddEnemy("wasp.ndo",lawnPath18,35,35,4,10.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHCYCLE | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("wasp.ndo",lawnPath19,35,35,4,5.0F, ENEMY_HASPATH | ENEMY_PATHBACKWARDS | ENEMY_PATHCYCLE | ENEMY_FLATLEVEL);

		} else if ( levelID == LEVELID_GARDENMAZE )
		{
			testEnemy = CreateAndAddEnemy("moa.ndo",mazePath1,0,0,3,7.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHCYCLE | ENEMY_FLATLEVEL);

			 /*
			testEnemy = CreateAndAddEnemy("moa.ndo",mazePath1,0,0,3,7.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHBOUNCE | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("moa.ndo",mazePath2,0,0,3,7.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHEND2START | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("moa.ndo",mazePath3,0,0,3,7.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHEND2START | ENEMY_FLATLEVEL);

			testEnemy = CreateAndAddEnemy("wasp.ndo",mazePath4,50,50,1,6.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHCYCLE | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("wasp.ndo",mazePath5,50,50,1,6.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHCYCLE | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("wasp.ndo",mazePath6,50,50,1,4.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHCYCLE | ENEMY_FLATLEVEL);
			  
			testEnemy = CreateAndAddEnemy("moa.ndo",mazePath7,0,0,2,7.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHEND2START | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("moa.ndo",mazePath8,0,0,3,7.0F, ENEMY_HASPATH | ENEMY_PATHBACKWARDS | ENEMY_PATHEND2START | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("moa.ndo",mazePath9,0,0,5,7.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHEND2START | ENEMY_FLATLEVEL);
					
			testEnemy = CreateAndAddEnemy("wasp.ndo",mazePath10,50,50,1,4.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHCYCLE | ENEMY_FLATLEVEL);
					*/
		} else if ( levelID == LEVELID_GARDENVEGPATCH )
		{
			/*testEnemy = CreateAndAddEnemy("roto.ndo",rotoPath17,0,0,5,8.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHBOUNCE | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("roto.ndo",rotoPath18,0,0,5,8.0F, ENEMY_HASPATH | ENEMY_PATHBACKWARDS | ENEMY_PATHBOUNCE | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("roto.ndo",rotoPath19,0,0,5,8.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHBOUNCE | ENEMY_FLATLEVEL);

			testEnemy = CreateAndAddEnemy("moa.ndo",rotoPath20,0,0,6,10.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHEND2START | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("roto.ndo",rotoPath20,0,0,12,10.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHEND2START | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("moa.ndo",rotoPath20,0,0,18,10.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHEND2START | ENEMY_FLATLEVEL);
			testEnemy = CreateAndAddEnemy("roto.ndo",rotoPath20,0,0,24,10.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHEND2START | ENEMY_FLATLEVEL);
				 	   */
			//testEnemy = CreateAndAddEnemy("moa.ndo",rotoPath21,0,0,2,6.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHBOUNCE | ENEMY_FLATLEVEL);
			//testEnemy = CreateAndAddEnemy("moa.ndo",rotoPath22,0,0,5,6.0F, ENEMY_HASPATH | ENEMY_PATHBACKWARDS | ENEMY_PATHBOUNCE | ENEMY_FLATLEVEL);
		}
	}
	else if ( worldID == WORLDID_CITY )
	{
		if ( levelID == LEVELID_CITYDOCKS )
		{
		}
		else if ( levelID == LEVELID_CITYWAREHOUSE )
		{
		}
		else if ( levelID == LEVELID_CITYSTREETS )
		{
		}
		else if ( levelID == LEVELID_CITYTOWER )
		{
		}
		else if ( levelID == LEVELID_CITYBONUS )
		{
		}
		// ENDELSEIF

	} // ENDIF
}

/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddEnemy
	Purpose			: creates and initialises an enemy
	Parameters		: char *,unsigned long *,float,float,int,float,unsigned int
	Returns			: ENEMY *
	Info			: 
*/
ENEMY *CreateAndAddEnemy(char *eActorName,unsigned long *pathIndex,float offset,float offset2,int startNode,float eSpeed,unsigned long eFlags)
{
	VECTOR toPosition;
	VECTOR vfd	= { 0,0,1 };
	VECTOR vup	= { 0,1,0 };
	VECTOR v1;
	int enemyType;
	int initFlags = 0;
	short shadowRadius = 0;
	
	ENEMY *newItem = (ENEMY *)JallocAlloc(sizeof(ENEMY),YES,"nme");
	AddEnemy(newItem);

	// check the actor name to determine the enemy type
	if(gstrcmp(eActorName,"roto.ndo") == 0)
		enemyType = NMETYPE_MOWER;
	else if(gstrcmp(eActorName,"lomoa.ndo") == 0)
		enemyType = NMETYPE_MOWER;
	else if(gstrcmp(eActorName,"moa.ndo") == 0)
		enemyType = NMETYPE_MOWER;
	else if(gstrcmp(eActorName,"roll.ndo") == 0)
		enemyType = NMETYPE_ROLLER;
	else if(gstrcmp(eActorName,"snke.ndo") == 0)
		enemyType = NMETYPE_SNAKE;
	else if(gstrcmp(eActorName,"wasp.ndo") == 0)
		enemyType = NMETYPE_WASP;
	else if(gstrcmp(eActorName,"snappr.ndo") == 0)
		enemyType = NMETYPE_SNAPPER;
	else if(gstrcmp(eActorName,"car.ndo") == 0)
		enemyType = NMETYPE_CAR;
	else if(gstrcmp(eActorName,"trk.ndo") == 0)
		enemyType = NMETYPE_TRUCK;
	else if(gstrcmp(eActorName,"carred.ndo") == 0)
		enemyType = NMETYPE_CAR;
	else if(gstrcmp(eActorName,"carblue.ndo") == 0)
		enemyType = NMETYPE_CAR;
	else if(gstrcmp(eActorName,"cargreen.ndo") == 0)
		enemyType = NMETYPE_CAR;
	else if(gstrcmp(eActorName,"fork.ndo") == 0)
		enemyType = NMETYPE_CAR;
	else if(gstrcmp(eActorName,"rat.ndo") == 0)
		enemyType = NMETYPE_SNAKE;
	else if(gstrcmp(eActorName,"dog.ndo") == 0)
		enemyType = NMETYPE_DOG;
	else if(gstrcmp(eActorName,"andy.ndo") == 0)
		enemyType = NMETYPE_SHARK;
	else if(gstrcmp(eActorName,"forkplat.ndo") == 0)
		enemyType = NMETYPE_CAR;
	else if(gstrcmp(eActorName,"bus.ndo") == 0)
	{
		newItem->nmeActor->flags = ACTOR_DRAW_ALWAYS;
		enemyType = NMETYPE_CAR;
	}

	// check nme type and assign shadow if necessary
	initFlags |= INIT_ANIMATION;
	switch(enemyType)
	{
		case NMETYPE_WASP:
			initFlags |= INIT_SHADOW;
			shadowRadius = 20;
			break;
	}

	// add and initialise the actor
	newItem->nmeActor = CreateAndAddActor(eActorName,0,0,0,initFlags,0,0);
	if(shadowRadius)
		newItem->nmeActor->actor->shadow->radius = shadowRadius;

	// specify enemy radius if the enemy is radius based
	if(initFlags & ENEMY_RADIUSBASEDCOLLISION)
	{
		// set a default radius
		newItem->nmeActor->radius = 25.0F;
	}
	else
	{
		// set radius to zero - not used for collision detection
		newItem->nmeActor->radius = 0.0F;
	}

	// set animation depending on enemy type
	if(enemyType > NMETYPE_NONE)
	{
		// This actor is an enemy so add to enemy actor list
		InitActorAnim(newItem->nmeActor->actor);

		switch(enemyType)
		{
			case NMETYPE_MOWER:
			case NMETYPE_ROLLER:
				AnimateActor(newItem->nmeActor->actor,0,YES,NO,1.5F);
				newItem->nmeActor->actor->status = NMESTATE_MOWER_IDLE;
				break;
			case NMETYPE_DOG:
				AnimateActor(newItem->nmeActor->actor,0,NO,NO,0.75F);
				newItem->nmeActor->actor->status = NMESTATE_DOG_IDLE;
				break;
			case NMETYPE_SNAPPER:
				AnimateActor(newItem->nmeActor->actor,Random(3)+1,NO,NO,0.75F);
				newItem->nmeActor->actor->status = NMESTATE_SNAPPER_IDLE;
				break;
			case NMETYPE_WASP:
				AnimateActor(newItem->nmeActor->actor,0,NO,NO,1.0F);
				newItem->nmeActor->actor->status = NMESTATE_WASP_MOVING;
				newItem->nmeActor->actor->scale.v[X] = 1.5F;
				newItem->nmeActor->actor->scale.v[Y] = 1.5F;
				newItem->nmeActor->actor->scale.v[Z] = 1.5F;
				break;
			case NMETYPE_SNAKE:
				AnimateActor(newItem->nmeActor->actor,3,YES,NO,2.0F);
				newItem->nmeActor->actor->status = NMESTATE_WASP_MOVING;
				break;
			case NMETYPE_CAR:
				AnimateActor(newItem->nmeActor->actor,0,YES,NO,1.0F);
				newItem->nmeActor->actor->status = NMESTATE_CAR_MOVING;
				break;
			case NMETYPE_TRUCK:
				AnimateActor(newItem->nmeActor->actor,0,YES,NO,1.0F);
				newItem->nmeActor->actor->status = NMESTATE_TRUCK_MOVING;
				break;
			case NMETYPE_SHARK:
				AnimateActor(newItem->nmeActor->actor,0,YES,NO,0.5F);
				newItem->nmeActor->actor->status = NMESTATE_SHARK_IDLE;
				break;
		}

		newItem->nmeActor->actor->type = enemyType;
		newItem->nmeActor->action.dead = 30;
	}
	
	newItem->speed			= eSpeed;
	newItem->startSpeed		= eSpeed;
	newItem->endSpeed		= eSpeed;
	newItem->accel			= 0.0F;

	newItem->path			= NULL;
	newItem->inTile			= &firstTile[pathIndex[startNode + 1]];

	// create the ptr to the linked list of world game tiles that define the enemy's path
	newItem->path = CreateEnemyPathFromTileList(pathIndex,offset,offset2);

	newItem->path->startNode = startNode;

	if(eFlags & ENEMY_PATHFORWARDS)
	{
		newItem->path->fromNode = startNode;
		if ( startNode == pathIndex[0] )
		{
			newItem->path->toNode	= startNode;
		}
		else
		{
			newItem->path->toNode	= newItem->path->fromNode + 1;
		}
		// ENDIF
	}
	
	if(eFlags & ENEMY_PATHBACKWARDS)
	{
		newItem->path->fromNode = startNode;
		if ( startNode == 0 )
		{
			newItem->path->toNode	= 1;
			eFlags &= ~ENEMY_PATHBACKWARDS;
			eFlags |= ENEMY_PATHFORWARDS;
		}
		else
		{
			newItem->path->toNode	= newItem->path->fromNode - 1;
		}
		// ENDIF
	}

	// set enemy (actor) position and orientation
	SetVector(&newItem->nmeActor->actor->pos,&newItem->path->nodes[startNode].worldTile->centre);

	newItem->nmeActor->actor->pos.v[X] += (newItem->path->nodes[startNode].worldTile->normal.v[X] * newItem->path->nodes[startNode].offset);
	newItem->nmeActor->actor->pos.v[Y] += (newItem->path->nodes[startNode].worldTile->normal.v[Y] * newItem->path->nodes[startNode].offset);
	newItem->nmeActor->actor->pos.v[Z] += (newItem->path->nodes[startNode].worldTile->normal.v[Z] * newItem->path->nodes[startNode].offset);

	NormalToQuaternion(&newItem->nmeActor->actor->qRot,&newItem->path->nodes[startNode].worldTile->normal);

	GetPositionForPathNode(&toPosition,&newItem->path->nodes[newItem->path->toNode]);

	if ( eFlags & ENEMY_PATHFORWARD_BACK )
	{
		SubVector(&v1,&toPosition,&newItem->nmeActor->actor->pos);
		MakeUnit(&v1);
		Orientate(&newItem->nmeActor->actor->qRot,&v1,&vfd,&vup);
	}
	else
	{
	}

	newItem->flags			= eFlags;

	return newItem;
}


/*	--------------------------------------------------------------------------------
	Function		: CreateEnemyPathFromTileList
	Purpose			: creates and initialise a path for an enemy
	Parameters		: unsigned long *,float,float
	Returns			: PATH *
	Info			: 
*/
PATH *CreateEnemyPathFromTileList(unsigned long *pIndex,float offset,float offset2)
{
	int i;
	PATH *newPath = (PATH *)JallocAlloc(sizeof(PATH),YES,"epath");

	newPath->numNodes = pIndex[0];

	// allocate memory for path nodes
	newPath->nodes = (PATHNODE *)JallocAlloc(sizeof(PATHNODE) * newPath->numNodes,YES,"enode");

	// add the nodes to the path
	if(newPath->numNodes == 1)
	{
		newPath->nodes[0].worldTile	= &firstTile[pIndex[1]];
		newPath->nodes[0].offset	= offset;
		newPath->nodes[0].offset2	= offset2;
	}
	else
	{
		for(i=1; i<=newPath->numNodes; i++)
		{
			newPath->nodes[i - 1].worldTile	= &firstTile[pIndex[i]];
			newPath->nodes[i - 1].offset	= offset;
			newPath->nodes[i - 1].offset2	= offset2;
		}
	}

	return newPath;
}

/*	--------------------------------------------------------------------------------
	Function		: EnemyHasArrivedAtNode
	Purpose			: determines if an enemy has arrived at a given path node
	Parameters		: ENEMY *,int
	Returns			: BOOL
	Info			: 
*/
BOOL EnemyHasArrivedAtNode(ENEMY *nme, int toNodeID)
{
	VECTOR nodePos;

	if(!(nme->flags & ENEMY_HASPATH))
		return FALSE;

	GetPositionForPathNode(&nodePos,&nme->path->nodes[toNodeID]);
	if(DistanceBetweenPointsSquared(&nme->nmeActor->actor->pos,&nodePos) < ((nme->speed + 0.1) * (nme->speed + 0.1)))
	{
		return TRUE;
	}
	// ENDIF

	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function		: JumpedOnEnemy
	Purpose			: checks if frog has jumped onto an enemy
	Parameters		: ENEMY *
	Returns			: BOOL
	Info			: 
*/
static BOOL JumpedOnEnemy(ENEMY *nme)
{
	if(currTile[0] == nme->inTile)
		return TRUE;

	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function		: GetActiveGameTile
	Purpose			: determines which tile the enemy is currently 'in'
	Parameters		: ENEMY *
	Returns			: void
	Info			: 
*/
static void GetActiveTile(ENEMY *nme)
{
	VECTOR v1,v2,diff;
	float halfdist;

	if(nme->flags & ENEMY_HASPATH)
	{
		GetPositionForPathNode(&v1,&nme->path->nodes[nme->path->fromNode]);
		GetPositionForPathNode(&v2,&nme->path->nodes[nme->path->toNode]);

		halfdist = DistanceBetweenPoints(&v1,&v2) / 2.0F;

//		if(DistanceBetweenPoints(&v1,&nme->nmeActor->actor->pos) < halfdist)
		if(DistanceBetweenPointsSquared(&v1,&nme->nmeActor->actor->pos) < (halfdist * halfdist))
			nme->inTile = nme->path->nodes[nme->path->fromNode].worldTile;
		else
			nme->inTile = nme->path->nodes[nme->path->toNode].worldTile;
	}
	else
	{
		// no path associated with this enemy
		nme->inTile = nme->path->nodes[0].worldTile;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: UpdateEnemies
	Purpose			: updates all enemies and their states / movements, etc.
	Parameters		: 
	Returns			: void
	Info			: 
*/
void UpdateEnemies()
{
	QUATERNION q1,destQ;
	VECTOR toPosition;
	VECTOR fwd;
	ENEMY *cur,*next;
	float lowest,t;
	int lowval;
	int i,j;
	int newCamFacing,newFrogFacing;

	float m,n,qrspd = 0.0F;
	VECTOR fromPosition,atPosition;

	VECTOR vfd	= { 0,0,1 };
	VECTOR vup	= { 0,1,0 };
	VECTOR nmeup,swarmPos;
	VECTOR v1,v2,v3;
						
	if(enemyList.numEntries == 0)
		return;

	for(cur = enemyList.head.next; cur != &enemyList.head; cur = next)
	{
		next = cur->next;

		if(cur->flags & ENEMY_HASPATH)
		{
			GetPositionForPathNode(&toPosition,&cur->path->nodes[cur->path->toNode]);
			SubVector(&fwd,&toPosition,&cur->nmeActor->actor->pos);
			MakeUnit(&fwd);

			cur->nmeActor->actor->pos.v[X] += (cur->speed * fwd.v[X]);
			cur->nmeActor->actor->pos.v[Z] += (cur->speed * fwd.v[Z]);
			cur->nmeActor->actor->pos.v[Y] += (cur->speed * fwd.v[Y]);

			if(cur->flags & ENEMY_FLATLEVEL)
			{
				// orientate the actor to face the direction of movement....
				SubVector(&v1,&toPosition,&cur->nmeActor->actor->pos);
				MakeUnit(&v1);
				Orientate(&cur->nmeActor->actor->qRot,&v1,&vfd,&vup);
			}
			else
			{

				GetPositionForPathNode(&fromPosition,&cur->path->nodes[cur->path->fromNode]);
				m = DistanceBetweenPoints(&toPosition,&fromPosition);
				n = DistanceBetweenPoints(&toPosition,&cur->nmeActor->actor->pos);
				qrspd = (1.0F - (n / m)) * 0.5F;

				NormalToQuaternion(&q1,&cur->path->nodes[cur->path->toNode].worldTile->normal);
				QuatSlerp(&cur->nmeActor->actor->qRot,&q1,qrspd,&destQ);
				SetQuaternion(&cur->nmeActor->actor->qRot,&destQ);

				// orientate the actor to face the direction of movement....
				SubVector(&v1,&cur->nmeActor->actor->pos,&toPosition);
				MakeUnit(&v1);

				// Calculate nme up vector
				RotateVectorByQuaternion(&nmeup,&vup,&cur->nmeActor->actor->qRot);
				CrossProduct(&v2,&v1,&nmeup);
				CrossProduct(&v3,&v2,&nmeup);
				Orientate(&cur->nmeActor->actor->qRot,&v3,&vfd,&nmeup);
			}

			// check if a node has been reached with the current enemy
			if(EnemyHasArrivedAtNode(cur, cur->path->toNode))
			{
				if(cur->flags & ENEMY_PATHBOUNCE)
				{
					// enemy moving with 'ping-pong' motion
					if(cur->flags & ENEMY_PATHFORWARDS)
					{
						cur->path->fromNode++;
						cur->path->toNode++;

						if(cur->path->toNode > (cur->path->numNodes - 1))
						{
							cur->path->fromNode = (cur->path->numNodes - 1);
							cur->path->toNode	= cur->path->fromNode - 1;

							cur->flags &= ~ENEMY_PATHFORWARDS;
							cur->flags |= ENEMY_PATHBACKWARDS;
						}
					}
					else if(cur->flags & ENEMY_PATHBACKWARDS)
					{
						cur->path->fromNode--;
						cur->path->toNode--;

						if(cur->path->toNode < 0)
						{
							cur->path->fromNode = 0;
							cur->path->toNode	= cur->path->fromNode + 1;

							cur->flags &= ~ENEMY_PATHBACKWARDS;
							cur->flags |= ENEMY_PATHFORWARDS;
						}
					}
				}
				else if(cur->flags & ENEMY_PATHCYCLE)
				{
					// enemy moves with cyclic motion
					if(cur->flags & ENEMY_PATHFORWARDS)
					{
						cur->path->fromNode++;
						cur->path->toNode++;

						if(cur->path->toNode > (cur->path->numNodes - 1))
							cur->path->toNode = 0;
						if(cur->path->fromNode > (cur->path->numNodes - 1))
						{
							cur->path->fromNode = 0;
							cur->speed = cur->startSpeed;
						}
					}
					else if(cur->flags & ENEMY_PATHBACKWARDS)
					{
						cur->path->fromNode--;
						cur->path->toNode--;

						if(cur->path->toNode < 0)
							cur->path->toNode = (cur->path->numNodes - 1);
						if(cur->path->fromNode < 0)
						{
							cur->path->fromNode = (cur->path->numNodes - 1);
							cur->speed = cur->startSpeed;
						}
					}
				}
				else if(cur->flags & ENEMY_PATHEND2START)
				{
					// enemy returns immediately to start of path when path end reached
					if(cur->flags & ENEMY_PATHFORWARDS)
					{
						cur->path->fromNode++;
						cur->path->toNode++;

						if(cur->path->fromNode > (cur->path->numNodes - 2))
						{
							cur->path->fromNode = 0;
							cur->path->toNode = 1;
							SetVector(&cur->nmeActor->actor->pos,&cur->path->nodes[cur->path->fromNode].worldTile->centre);
							NormalToQuaternion(&cur->nmeActor->actor->qRot,&cur->path->nodes[cur->path->fromNode].worldTile->normal);
							cur->speed = cur->startSpeed;
						}
					}
					else if(cur->flags & ENEMY_PATHBACKWARDS)
					{
						cur->path->fromNode--;
						cur->path->toNode--;

						if(cur->path->fromNode < 1)
						{
							cur->path->fromNode = (cur->path->numNodes - 1);
							cur->path->toNode = (cur->path->numNodes - 2);
							SetVector(&cur->nmeActor->actor->pos,&cur->path->nodes[cur->path->fromNode].worldTile->centre);
							NormalToQuaternion(&cur->nmeActor->actor->qRot,&cur->path->nodes[cur->path->fromNode].worldTile->normal);
							cur->speed = cur->startSpeed;
						}
					}
				}
				else if(cur->flags & ENEMY_PATHFORWARD_BACK)
				{
					// enemy moving with 'ping-pong' motion
					if(cur->flags & ENEMY_PATHFORWARDS)
					{
						cur->path->fromNode++;
						cur->path->toNode++;

						if(cur->path->toNode > (cur->path->numNodes - 1))
						{
							cur->path->fromNode = (cur->path->numNodes - 1);
							cur->path->toNode	= cur->path->fromNode - 1;

							cur->flags &= ~ENEMY_PATHFORWARDS;
							cur->flags |= ENEMY_PATHBACKWARDS;
							cur->speed = cur->startSpeed;
						}
					}
					else if(cur->flags & ENEMY_PATHBACKWARDS)
					{
						cur->path->fromNode--;
						cur->path->toNode--;

						if(cur->path->toNode < 0)
						{
							cur->path->fromNode = 0;
							cur->path->toNode	= cur->path->fromNode + 1;

							cur->flags &= ~ENEMY_PATHBACKWARDS;
							cur->flags |= ENEMY_PATHFORWARDS;
							cur->speed = cur->startSpeed;
						}
					}
				}
			}
		}

		// check which world tile enemy is currently 'in'
		oldTile = currTile[0];
		GetActiveTile(cur);

		// check if frog has been 'killed' by current enemy - tile based collision
		if ( ( currTile[0] == cur->inTile ) && ( !frog[0]->action.dead ) &&
			(!frog[0]->action.safe) && (!(player[0].frogState & FROGSTATUS_ISSUPERHOPPING) || (cur->flags & ENEMY_NOJUMPOVER) ) &&
			 (! currPlatform ) && !(player[0].frogState & FROGSTATUS_ISFLOATING ) )
		{

			//osMotorStart ( &rumble );

//			frog->action.dead = 50;
			frog[0]->action.lives--;
			if(frog[0]->action.lives != 0)
			{
				cameraShake = 25;
				PlaySample(42,NULL,192,128);
				frog[0]->action.safe = 25;

				SetVector(&swarmPos,&frog[0]->actor->pos);
				swarmPos.v[Y] += 35;
				CreateAndAddFXSwarm(SWARM_TYPE_STARSTUN,&swarmPos,64,25);
				swarmPos.v[Y] += 10;
				CreateAndAddFXSwarm(SWARM_TYPE_STARSTUN,&swarmPos,64,35);

				switch(cur->nmeActor->actor->type)
				{
					case NMETYPE_CAR:
					case NMETYPE_TRUCK:
					case NMETYPE_FORK:
						cameraShake = 25;
						break;
				}
			}
			else
			{
				PlaySample(110,NULL,192,128);
				AnimateActor(frog[0]->actor,2,NO,NO,0.367);
				frog[0]->action.dead = 50;
				frog[0]->action.lives = 3;
			
				switch(cur->nmeActor->actor->type)
				{
					case NMETYPE_CAR:
					case NMETYPE_TRUCK:
					case NMETYPE_FORK:
						cameraShake = 50;
						frog[0]->action.deathBy = DEATHBY_RUNOVER;
						PlaySample(31,NULL,192,128);
						break;

					default:
						frog[0]->action.deathBy = DEATHBY_NORMAL;
				}
			
				player[0].frogState |= FROGSTATUS_ISDEAD;
			}
		}

		// process enemies (update anims, etc.)
		switch(cur->nmeActor->actor->type)
		{
			case NMETYPE_SNAPPER:
				ProcessNMESnapperPlant(cur->nmeActor);
				break;
			case NMETYPE_DOG:
				ProcessNMEDog ( cur->nmeActor );
				break;
			case NMETYPE_WASP:
				ProcessNMEWasp(cur->nmeActor);
				break;
			case NMETYPE_SNAKE:
				ProcessNMESnake(cur->nmeActor);
				break;
			case NMETYPE_CAR:
			case NMETYPE_FORK:
				ProcessNMECar(cur->nmeActor);
				break;
			case NMETYPE_TRUCK:
				ProcessNMETruck(cur->nmeActor);
				break;

			case NMETYPE_SHARK:
				ProcessNMEShark ( cur->nmeActor );
				break;
			case NMETYPE_MOWER:
				ProcessNMEMower(cur->nmeActor);
				break;
			case NMETYPE_ROLLER:
				ProcessNMERoller(cur->nmeActor);
				break;

		}
	}

	// go through enemy list and update accelerating / decelerating enemies - radius based collision
	for(cur = enemyList.head.next; cur != &enemyList.head; cur = next)
	{
		next = cur->next;

		if(cur->flags & ENEMY_ACCELERATECONST)
			cur->speed += cur->accel;
		else if(cur->flags & ENEMY_DECELERATECONST)
			cur->speed -= cur->accel;

		if(cur->flags & ENEMY_RADIUSBASEDCOLLISION)
		{
			cur->nmeActor->radius = 15;

			// perform radius collision check between frog and current enemy
			if(ActorsHaveCollided(frog[0],cur->nmeActor))
			{
				dprintf"COLLIDED !\n"));
			}
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function		: ProcessNMEMower
	Purpose			: process mower enemy
	Parameters		: ACTOR2 *
	Returns			: void
	Info			:
*/
void ProcessNMEMower(ACTOR2 *nme)
{
	if(nme->actor->animation->reachedEndOfAnimation)
		AnimateActor(nme->actor,0,YES,NO,1.5F);

	if((nme->distanceFromFrog < ACTOR_DRAWDISTANCEINNER) && (frameCount & 3) == 0)
		CreateAndAddFXSmoke(SMOKE_TYPE_NORMAL,&nme->actor->pos,128,1,1,8);
}

/*	--------------------------------------------------------------------------------
	Function		: ProcessNMERoller
	Purpose			: process roller enemy
	Parameters		: ACTOR2 *
	Returns			: void
	Info			:
*/
void ProcessNMERoller(ACTOR2 *nme)
{
	if(nme->actor->animation->reachedEndOfAnimation)
		AnimateActor(nme->actor,0,YES,NO,1.5F);
}

/*	--------------------------------------------------------------------------------
	Function			: ProcessNMEDog
	Purpose			   : 
	Parameters		 : 
	Returns				: void
	Info					: 
*/
void ProcessNMEDog ( ACTOR2 *nme )
{
	VECTOR vfd	= { 0,0,1 };
	VECTOR vup	= { 0,1,0 };
	VECTOR snapup;
	VECTOR v1,v2,v3;
	VECTOR cDir;
	float t;

	switch ( nme->actor->status )
	{
		case NMESTATE_DOG_IDLE:

			SubVector(&v1,&nme->actor->pos,&frog[0]->actor->pos);
			MakeUnit(&v1);

			// Calculate snapper dog up vector
			RotateVectorByQuaternion	( &snapup, &vup, &nme->actor->qRot );
			CrossProduct				( &v2, &v1, &snapup );
			CrossProduct				( &v3, &v2, &snapup );
			Orientate					( &nme->actor->qRot, &v3, &vfd, &snapup );

			snapPos = frog[0]->actor->pos;

			// check if frog is in snapping range
			if(DistanceBetweenPointsSquared(&snapPos,&nme->actor->pos) < 6400.0F)
			{
				// frog is in snapping range - prepare for him to snap at frog
				nme->actor->status	= NMESTATE_DOG_SNAPPING;
				AnimateActor ( nme->actor, 1, NO, NO, 1.5F );
			}
			else
			{
				if(nme->actor->animation->reachedEndOfAnimation)
				{
					// Choose an idle animation at random
					AnimateActor(nme->actor,0,NO,NO,1.0F);
				}
			}
							

			break;

		case NMESTATE_DOG_SNAPPING:

			if((nme->actor->animation->animTime > 200.0F) && (nme->actor->animation->animTime < 240.0F))
			{
				// Check if frog is still in snapping range
				if ( ( DistanceBetweenPoints(&snapPos,&nme->actor->pos) < 6400.0F ) && ( !frog[0]->action.dead ) )
				{
					if ( ( DistanceBetweenPointsSquared(&snapPos,&frog[0]->actor->pos) == 0.0F ) )
					{
						// kill frog
						AnimateActor(frog[0]->actor,2,NO,NO,0.35F);
						frog[0]->action.deathBy = DEATHBY_NORMAL;
						player[0].frogState |= FROGSTATUS_ISDEAD;
						frog[0]->action.dead = 50;
						PlaySample ( 75,NULL,192,128);
					}
					// ENDIF
				}
				else
				{
					if(nme->actor->animation->reachedEndOfAnimation)
					{
						nme->actor->status	= NMESTATE_DOG_YAP;
						AnimateActor(nme->actor,3,NO,NO,1.0F);
						PlaySample ( 75,NULL,192,128);
					}
				}
				// ENDIF
			}
			// ENDIF
			if(nme->actor->animation->reachedEndOfAnimation)
			{
				nme->actor->status	= NMESTATE_DOG_RETURN;
				AnimateActor(nme->actor,2,NO,NO,1.0F);
			}
						   
			break;
		case NMESTATE_DOG_YAP:
				if ( nme->actor->loopCount == 70 )
				{
					nme->actor->animation->loopAnimation = 0;
					nme->actor->loopCount = 0;
				}
				// ENDIF
				if(nme->actor->animation->reachedEndOfAnimation)
				{
					nme->actor->status	= NMESTATE_DOG_RETURN;
					AnimateActor(nme->actor,2,NO,NO,1.0F);
				}
			break;
		case NMESTATE_DOG_RETURN:
				if(nme->actor->animation->reachedEndOfAnimation)
				{
					nme->actor->status	= NMESTATE_DOG_IDLE;
					AnimateActor(nme->actor,0,NO,NO,1.0F);
				}	  
			break;

	}
}


/*	--------------------------------------------------------------------------------
	Function		: ProcessNMESnapperPlant
	Purpose			: process snapper plant enemy
	Parameters		: ACTOR2 *
	Returns			: void
	Info			:
*/
void ProcessNMESnapperPlant(ACTOR2 *nme)
{
	VECTOR vfd	= { 0,0,1 };
	VECTOR vup	= { 0,1,0 };
	VECTOR snapup;
	VECTOR v1,v2,v3;

/*switch ( nme->actor->status )
{
case NMESTATE_SNAPPER_IDLE:
	SubVector(&v1,&nme->actor->pos,&frog->actor->pos);
	snapPos = frog->actor->pos;
	MakeUnit(&v1);
	// Calculate snapper plant up vector
	RotateVectorByQuaternion(&snapup,&vup,&nme->actor->qRot);
	CrossProduct(&v2,&v1,&snapup);
	CrossProduct(&v3,&v2,&snapup);
	Orientate(&nme->actor->qRot,&v3,&vfd,&snapup);
	break;
case NMESTATE_SNAPPER_READYTOSNAP:
	//SubVector(&v1,&nme->actor->pos,&snapPos);
	break;
case NMESTATE_SNAPPER_SNAPPING:
//	SubVector(&v1,&nme->actor->pos,&snapPos);
	break;
};

*/	
	switch(nme->actor->status)
	{
		case NMESTATE_SNAPPER_IDLE:

			// check if frog is in snapping range
			if(DistanceBetweenPoints(&snapPos,&nme->actor->pos) < 3600.0F)
			{
				// frog is in snapping range - prepare for him to snap at frog
				nme->actor->status	= NMESTATE_SNAPPER_READYTOSNAP;
				nme->action.dead	= 5;
//				snapPos = frog->actor->pos;
			}
			else
			{
				if(nme->actor->animation->reachedEndOfAnimation)
				{
					// Choose an idle animation at random
					AnimateActor(nme->actor,Random(3)+1,NO,NO,0.75F);
				}
			}

			break;

		case NMESTATE_SNAPPER_READYTOSNAP:

			if(nme->action.dead)
				nme->action.dead--;
			else
			{
				nme->action.dead	= 5;
				nme->actor->status	= NMESTATE_SNAPPER_SNAPPING;
				AnimateActor(nme->actor,0,NO,NO,1.0F);
			}

			break;

		case NMESTATE_SNAPPER_SNAPPING:

			if((nme->actor->animation->animTime > 11.0F) && (nme->actor->animation->animTime < 13.0F))
			{
				// Check if frog is still in snapping range
				if(DistanceBetweenPoints(&snapPos,&nme->actor->pos) < 3600.0F && (!frog[0]->action.dead))
				{
					// kill frog
					AnimateActor(frog[0]->actor,2,NO,NO,0.35F);
					frog[0]->action.deathBy = DEATHBY_NORMAL;
					player[0].frogState |= FROGSTATUS_ISDEAD;
					frog[0]->action.dead = 50;
				}
			}

			if(nme->actor->animation->reachedEndOfAnimation)
			{
				nme->actor->status	= NMESTATE_SNAPPER_IDLE;
				AnimateActor(nme->actor,1,NO,NO,0.75F);
			}

			break;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: ProcessNMEWasp
	Purpose			: process wasp enemy
	Parameters		: ACTOR2 *
	Returns			: void
	Info			:
*/
void ProcessNMEWasp(ACTOR2 *nme)
{
	if(nme->actor->animation->reachedEndOfAnimation)
		AnimateActor(nme->actor,0,NO,NO,Random(3)+1.0F);
}

/*	--------------------------------------------------------------------------------
	Function		: ProcessNMESnake
	Purpose			: process snake enemy
	Parameters		: ACTOR2 *
	Returns			: void
	Info			:
*/
void ProcessNMESnake(ACTOR2 *nme)
{
	switch(nme->actor->status)
	{
		case NMESTATE_SNAKE_IDLE:
			break;

		case NMESTATE_SNAKE_MOVING:
			break;

		case NMESTATE_SNAKE_ATTACKING:
			break;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: ProcessNMECar
	Purpose			: process car enemy
	Parameters		: ACTOR2 *
	Returns			: void
	Info			:
*/
void ProcessNMECar(ACTOR2 *nme)
{
	VECTOR smokePos;

	if((frameCount & 1) == 0)
	{
		SetVector(&smokePos,&frog[0]->actor->pos);
		smokePos.v[Y] -= 10;
		CreateAndAddFXSmoke(SMOKE_TYPE_NORMAL,&smokePos,128,1,1,8);
	}
}

/*	--------------------------------------------------------------------------------
	Function		: ProcessNMETruck
	Purpose			: process truck enemy
	Parameters		: ACTOR2 *
	Returns			: void
	Info			:
*/
void ProcessNMETruck(ACTOR2 *nme)
{
}


/*	--------------------------------------------------------------------------------
	Function		: ProcessNMEShark
	Purpose			: process truck enemy
	Parameters		: ACTOR2 *
	Returns			: void
	Info			:
*/
void ProcessNMEShark(ACTOR2 *nme)
{
	switch(nme->actor->status)
	{
		case NMESTATE_SHARK_IDLE:
			break;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: InitEnemyLinkedList
	Purpose			: initialises the enemy linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void InitEnemyLinkedList()
{
	enemyList.numEntries = 0;
	enemyList.head.next = enemyList.head.prev = &enemyList.head;
}

/*	--------------------------------------------------------------------------------
	Function		: AddEnemy
	Purpose			: adds an enemy to the linked list
	Parameters		: ENEMY *
	Returns			: void
	Info			: 
*/
void AddEnemy(ENEMY *enemy)
{
	if(enemy->next == NULL)
	{
		enemyList.numEntries++;
		enemy->prev = &enemyList.head;
		enemy->next = enemyList.head.next;
		enemyList.head.next->prev = enemy;
		enemyList.head.next = enemy;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: SubEnemy
	Purpose			: removes an enemy from the linked list
	Parameters		: ENEMY *
	Returns			: void
	Info			: 
*/
void SubEnemy(ENEMY *enemy)
{
	if(enemy->next == NULL)
		return;

	if(enemy->path)
	{
		if(enemy->path->nodes)
			JallocFree((UBYTE**)&enemy->path->nodes);

		JallocFree((UBYTE**)&enemy->path);
	}

	enemy->prev->next = enemy->next;
	enemy->next->prev = enemy->prev;
	enemy->next = NULL;
	enemyList.numEntries--;

	JallocFree((UBYTE **)&enemy);
}


/*	--------------------------------------------------------------------------------
	Function		: FreeEnemyLinkedList
	Purpose			: frees the enemy linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeEnemyLinkedList()
{
	ENEMY *cur,*next;

	// check if any elements in list
	if(enemyList.numEntries == 0)
		return;

	dprintf"Freeing linked list : ENEMY : (%d elements)\n",enemyList.numEntries));

	// traverse enemy list and free elements
	for(cur = enemyList.head.next; cur != &enemyList.head; cur = next)
	{
		next = cur->next;

		SubEnemy(cur);
	}

	// initialise list for future use
	InitEnemyLinkedList();
}
