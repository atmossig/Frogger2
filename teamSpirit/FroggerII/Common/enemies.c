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


ENEMY *devNME1	= NULL;


PATHNODE debug_nmeNodes1[] =					// TEST PATH - ANDYE
{ 
	20,30,0,8,10,	21,30,0,8,0,	22,30,0,8,0,	23,30,0,2,10,
	15,30,0,6,5,	14,30,0,6,0,	13,30,0,6,0,	12,30,0,1,5
};


PATH debug_nme1 = { 8,0,0,0,debug_nmeNodes1 };


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


static void GetEnemyActiveTile(ENEMY *enemy);

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
			devNME1 = CreateAndAddEnemy("wasp.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS | ENEMY_NEW_CYCLE,&debug_nme1,PATH_MAKENODETILEPTRS);

/*
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
*/

		} else if ( levelID == LEVELID_GARDENMAZE )
		{
//			testEnemy = CreateAndAddEnemy("moa.ndo",mazePath1,0,0,3,7.0F, ENEMY_HASPATH | ENEMY_PATHFORWARDS | ENEMY_PATHCYCLE | ENEMY_FLATLEVEL);

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
	Function		: GetEnemyActiveGameTile
	Purpose			: determines which tile the enemy is currently 'in'
	Parameters		: ENEMY *
	Returns			: void
	Info			: 
*/
static void GetEnemyActiveTile(ENEMY *nme)
{
	VECTOR v1,v2,diff;
	float halfdist;

	if(nme->flags & ENEMY_NEW_FOLLOWPATH)
	{
		GetPositionForPathNode(&v1,&nme->path->nodes[nme->path->fromNode]);
		GetPositionForPathNode(&v2,&nme->path->nodes[nme->path->toNode]);

		halfdist = DistanceBetweenPoints(&v1,&v2) / 2.0F;

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
	ENEMY *cur,*next;
	VECTOR fromPosition,toPosition;
	VECTOR fwd,swarmPos;
	VECTOR moveVec;

	if(enemyList.numEntries == 0)
		return;
	
	for(cur = enemyList.head.next; cur != &enemyList.head; cur = next)
	{
		next = cur->next;

		// check if enemy is active
		if(!cur->active)
			continue;

		// check if this enemy is currently 'waiting' at a node
		if(cur->isWaiting)
		{
			if(cur->isWaiting == -1)
				continue;
			else
				cur->isWaiting--;
			
			continue;
		}

		if(cur->flags & ENEMY_NEW_FOLLOWPATH)
		{
			// process enemies that follow a path (>1 node in path)

			// first, update the enemy position
			GetPositionForPathNode(&toPosition,&cur->path->nodes[cur->path->toNode]);
			SubVector(&fwd,&toPosition,&cur->nmeActor->actor->pos);
			MakeUnit(&fwd);

			cur->nmeActor->actor->pos.v[X] += (cur->speed * fwd.v[X]);
			cur->nmeActor->actor->pos.v[Y] += (cur->speed * fwd.v[Y]);
			cur->nmeActor->actor->pos.v[Z] += (cur->speed * fwd.v[Z]);

//--------------------->

			AddToVector(&cur->currNormal,&cur->deltaNormal);

//			NormalToQuaternion(&cur->nmeActor->actor->qRot,&cur->currNormal);
//			or
			Orientate(&cur->nmeActor->actor->qRot,&fwd,&inVec,&cur->currNormal);


//--------------------->

			// check if this enemy has arrived at a path node
			if(EnemyHasArrivedAtNode(cur))
				UpdateEnemyPathNodes(cur);
		}
		else
		{
			// process enemies that are based on a single node

			// get up vector for this enemy
			SetVector(&moveVec,&cur->path->nodes[0].worldTile->normal);
			
			moveVec.v[X] *= cur->speed;
			moveVec.v[Y] *= cur->speed;
			moveVec.v[Z] *= cur->speed;

			// check if this enemy is moving up or down
			if(cur->flags & ENEMY_NEW_MOVEUP)
			{
				// enemy is moving up
				AddToVector(&cur->nmeActor->actor->pos,&moveVec);
			}
			else if(cur->flags & ENEMY_NEW_MOVEDOWN)
			{
				// enemy is moving down
				SubFromVector(&cur->nmeActor->actor->pos,&moveVec);
			}
	
			if(EnemyReachedTopOrBottomPoint(cur))
				UpdateEnemyPathNodes(cur);
		}

		// determine which world tile the enemy is currently 'in'
		oldTile[0] = currTile[0];
		GetEnemyActiveTile(cur);

		// check if frog has been 'killed' by current enemy - radius based collision
		if(cur->flags & ENEMY_NEW_RADIUSBASEDCOLLISION)
		{
			// perform radius collision check between frog and current enemy
			if((!frog[0]->action.dead) && (!frog[0]->action.safe) && ActorsHaveCollided(frog[0],cur->nmeActor))
			{
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
		}
		
		// check if frog has been 'killed' by current enemy - tile based collision
		else if((currTile[0] == cur->inTile) && (!frog[0]->action.dead) &&
				(!frog[0]->action.safe) && (!(player[0].frogState & FROGSTATUS_ISSUPERHOPPING) || (cur->flags & ENEMY_NEW_NOJUMPOVER)) &&
				(!currPlatform) && !(player[0].frogState & FROGSTATUS_ISFLOATING))
		{
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
			case NMETYPE_WASP:
				ProcessNMEWasp(cur->nmeActor);
				break;
			case NMETYPE_MOWER:
				ProcessNMEMower(cur->nmeActor);
				break;
			case NMETYPE_ROLLER:
				ProcessNMERoller(cur->nmeActor);
				break;
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
				AnimateActor ( nme->actor, 1, NO, NO, 1.5F);
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
/*				if ( nme->actor->loopCount == 70 )
				{
					nme->actor->animation->loopAnimation = 0;
					nme->actor->loopCount = 0;
				}
				// ENDIF*/

// Commented out for skinning


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




//------------------------------------------------------------------------------------------------
// NEW ENEMY CODE - UNDER DEVELOPMENT - ANDYE - NEW ENEMY CODE - UNDER DEVELOPMENT - ANDYE -
//------------------------------------------------------------------------------------------------

ENEMY *CreateAndAddEnemy(char *eActorName)
{
	int i,enemyType = 0,initFlags = 0;
	float shadowRadius = 0;

	ENEMY *newItem = (ENEMY *)JallocAlloc(sizeof(ENEMY),YES,"NME");
	AddEnemy(newItem);

	initFlags |= INIT_ANIMATION;

	// check the actor name to determine the enemy type
	if(gstrcmp(eActorName,"roto.ndo") == 0)
		enemyType = NMETYPE_MOWER;
	else if(gstrcmp(eActorName,"lomoa.ndo") == 0)
		enemyType = NMETYPE_MOWER;
	else if(gstrcmp(eActorName,"moa.ndo") == 0)
		enemyType = NMETYPE_MOWER;
	else if(gstrcmp(eActorName,"roll.ndo") == 0)
		enemyType = NMETYPE_ROLLER;
	else if(gstrcmp(eActorName,"wasp.ndo") == 0)
		enemyType = NMETYPE_WASP;

	// check nme type and assign shadow if necessary
	initFlags |= INIT_ANIMATION;
	if(enemyType == NMETYPE_WASP)
	{
		initFlags |= INIT_SHADOW;
		shadowRadius = 20;
	}

	// create and add the nme actor
	newItem->nmeActor = CreateAndAddActor(eActorName,0,0,0,initFlags,0,0);
	if(newItem->nmeActor->actor->objectController)
//	{
		InitActorAnim(newItem->nmeActor->actor);
//		AnimateActor(newItem->nmeActor->actor,0,YES,NO,1.0F);
//	}

	// set shadow radius (if applicable)
	if(shadowRadius)
		newItem->nmeActor->actor->shadow->radius = shadowRadius;

	// specify enemy radius if the enemy is radius based
	if(initFlags & ENEMY_NEW_RADIUSBASEDCOLLISION)
	{
		// set a default collision radius
		SetActorCollisionRadius(newItem->nmeActor,15.0F);
	}
	else
	{
		// set radius to zero - not used for collision detection
		SetActorCollisionRadius(newItem->nmeActor,0.0F);
	}

	// set animation depending on enemy type
	if(enemyType > NMETYPE_NONE)
	{

		switch(enemyType)
		{
			case NMETYPE_MOWER:
			case NMETYPE_ROLLER:
				AnimateActor(newItem->nmeActor->actor,0,YES,NO,1.5F);
				newItem->nmeActor->actor->status = NMESTATE_MOWER_IDLE;
				break;
			case NMETYPE_WASP:
				AnimateActor(newItem->nmeActor->actor,0,NO,NO,1.0F);
				newItem->nmeActor->actor->status = NMESTATE_WASP_MOVING;
				newItem->nmeActor->actor->scale.v[X] = 1.5F;
				newItem->nmeActor->actor->scale.v[Y] = 1.5F;
				newItem->nmeActor->actor->scale.v[Z] = 1.5F;
				break;
		}

		newItem->nmeActor->actor->type = enemyType;
		newItem->nmeActor->action.dead = 30;
	}
	
	// set the platform to be active (i.e. is updated)
	newItem->active			= 1;
	
	newItem->path			= NULL;
	newItem->inTile			= NULL;

	newItem->speed			= 1.0F;
	newItem->startSpeed		= 1.0F;
	newItem->accel			= 0.0F;

	return newItem;
}

void AssignPathToEnemy(ENEMY *nme,unsigned long enemyFlags,PATH *path,unsigned long pathFlags)
{
	int i;
	VECTOR enemyStartPos;

	// assign the path to this enemy
	nme->flags		|= enemyFlags;
	nme->path		= path;

	dprintf"Add enemy path : "));

	// check if pathnode indices need converting to game tile pointers
	if(pathFlags & PATH_MAKENODETILEPTRS)
	{
		for(i=0; i<path->numNodes; i++)
		{
			// convert integer to a valid game tile
			dprintf"%d, ",(unsigned long)path->nodes[i].worldTile));
			nme->path->nodes[i].worldTile = &firstTile[(unsigned long)path->nodes[i].worldTile];
		}
	}

	if(enemyFlags & ENEMY_NEW_FORWARDS)
	{
		// this enemy moves forward thru path nodes
		nme->flags				|= ENEMY_NEW_FOLLOWPATH;
		nme->path->fromNode		= 0;
		nme->path->toNode		= 1;
	}
	else if(enemyFlags & ENEMY_NEW_BACKWARDS)
	{
		// this enemy moves backward thru path nodes
		nme->flags				|= ENEMY_NEW_FOLLOWPATH;
		nme->path->fromNode		= GET_PATHLASTNODE(path);
		nme->path->toNode		= GET_PATHLASTNODE(path) - 1;
	}
	else if((enemyFlags & ENEMY_NEW_MOVEUP) ||
			(enemyFlags & ENEMY_NEW_MOVEDOWN))
	{
		// this enemy moves up or down
		nme->path->fromNode = nme->path->toNode = 0;
	}

	// set enemy position to relevant point on path
	GetPositionForPathNode(&enemyStartPos,&path->nodes[nme->path->fromNode]);
	SetVector(&nme->nmeActor->actor->pos,&enemyStartPos);
	NormalToQuaternion(&nme->nmeActor->actor->qRot,&path->nodes[nme->path->fromNode].worldTile->normal);

	// set enemy current 'in' tile and speeds and pause times
	nme->inTile		= path->nodes[nme->path->fromNode].worldTile;
	nme->speed		= path->nodes[nme->path->fromNode].speed;
	nme->startSpeed	= path->nodes[nme->path->fromNode].speed;
	nme->isWaiting	= path->nodes[nme->path->fromNode].waitTime;

	CalcEnemyNormalInterps(nme);

	dprintf"\n"));
}


/*	--------------------------------------------------------------------------------
	Function		: EnemyHasArrivedAtNode
	Purpose			: checks if an enemy has arrived at a node
	Parameters		: ENEMY *
	Returns			: BOOL - TRUE if node reached, else FALSE
	Info			: 
*/
BOOL EnemyHasArrivedAtNode(ENEMY *nme)
{
	VECTOR nodePos;
	PATH *path = nme->path;

	// check if path node is reached
	GetPositionForPathNode(&nodePos,&path->nodes[path->toNode]);
	if(DistanceBetweenPointsSquared(&nme->nmeActor->actor->pos,&nodePos) <  ((nme->speed + 0.1F) * (nme->speed + 0.1F)))
	{
		dprintf""));
		return TRUE;
	}

	return FALSE;
}


/*	--------------------------------------------------------------------------------
	Function		: EnemyReachedTopOrBottomPoint
	Purpose			: checks if an enemy has arrived at top or bottom point (1 node based)
	Parameters		: ENEMY *
	Returns			: BOOL - TRUE if node reached, else FALSE
	Info			: 
*/
BOOL EnemyReachedTopOrBottomPoint(ENEMY *nme)
{
	VECTOR toPos;
	PATH *path = nme->path;

	// check if path extreme point is reached
	if(nme->flags & ENEMY_NEW_MOVEUP)
	{
		// moving up
		GetPositionForPathNodeOffset2(&toPos,&path->nodes[0]);

		if(DistanceBetweenPointsSquared(&nme->nmeActor->actor->pos,&toPos) < (nme->speed * nme->speed))
			return TRUE;
	}
	else if(nme->flags & ENEMY_NEW_MOVEDOWN)
	{
		// moving down
		GetPositionForPathNode(&toPos,&path->nodes[0]);

		if(DistanceBetweenPointsSquared(&nme->nmeActor->actor->pos,&toPos) < (nme->speed * nme->speed))
			return TRUE;
	}

	return FALSE;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateEnemyPathNodes
	Purpose			: updates enemy path move status
	Parameters		: ENEMY *
	Returns			: void
	Info			: 
*/
void UpdateEnemyPathNodes(ENEMY *nme)
{
	VECTOR nmePos;
	int nextToNode,nextFromNode;
	PATH *path = nme->path;
	unsigned long flags = nme->flags;
	
	VECTOR *n1,*n2;
	
	// determine to/from nodes for the current enemy

	if(flags & ENEMY_NEW_FORWARDS)
	{
		// enemy moves forward through path nodes
		nextToNode = path->toNode + 1;

		if(nextToNode > GET_PATHLASTNODE(path))
		{
			// reached end of path nodes
			// check if this enemy has ping-pong movement
			if(flags & ENEMY_NEW_PINGPONG)
			{
				// reverse enemy path movement
				nme->flags		&= ~ENEMY_NEW_FORWARDS;
				nme->flags		|= ENEMY_NEW_BACKWARDS;
				path->fromNode	= GET_PATHLASTNODE(path);
				path->toNode	= GET_PATHLASTNODE(path) - 1;

				nme->speed		= GetSpeedFromIndexedNode(path,path->fromNode);
				nme->isWaiting	= GetWaitTimeFromIndexedNode(path,path->fromNode);
				CalcEnemyNormalInterps(nme);
				return;
			}
			else if(flags & ENEMY_NEW_CYCLE)
			{
				// enemy has cyclic movement
				path->fromNode	= GET_PATHLASTNODE(path);
				path->toNode	= 0;
				
				nme->speed		= GetSpeedFromIndexedNode(path,path->fromNode);
				nme->isWaiting	= GetWaitTimeFromIndexedNode(path,path->fromNode);
				CalcEnemyNormalInterps(nme);
				return;
			}

			path->fromNode	= 0;
			path->toNode	= 1;

			nme->speed		= GetSpeedFromIndexedNode(path,path->fromNode);
			nme->isWaiting	= GetWaitTimeFromIndexedNode(path,path->fromNode);
			CalcEnemyNormalInterps(nme);

			GetPositionForPathNode(&nmePos,&path->nodes[0]);
			SetVector(&nme->nmeActor->actor->pos,&nmePos);
		}
		else
		{
			nextFromNode = path->fromNode + 1;

			if((nme->flags & ENEMY_NEW_CYCLE) && (nextFromNode > GET_PATHLASTNODE(path)))
			{
				path->fromNode	= 0;
				path->toNode	= 1;

				nme->speed		= GetSpeedFromIndexedNode(path,path->fromNode);
				nme->isWaiting	= GetWaitTimeFromIndexedNode(path,path->fromNode);
				CalcEnemyNormalInterps(nme);
				return;
			}
			
			path->fromNode++;
			path->toNode++;

			nme->speed		= GetSpeedFromIndexedNode(path,path->fromNode);
			nme->isWaiting	= GetWaitTimeFromIndexedNode(path,path->fromNode);
			CalcEnemyNormalInterps(nme);
		}
	}
	else if(flags & ENEMY_NEW_BACKWARDS)
	{
		// enemy moves backwards through path nodes
		nextToNode = path->toNode - 1;

		if(nextToNode < 0)
		{
			// reached beginning of path nodes
			// check if this enemy has ping-pong movement
			if(flags & ENEMY_NEW_PINGPONG)
			{
				// reverse enenmy path movement
				nme->flags		&= ~ENEMY_NEW_BACKWARDS;
				nme->flags		|= ENEMY_NEW_FORWARDS;
				path->fromNode	= 0;
				path->toNode	= 1;

				nme->speed		= GetSpeedFromIndexedNode(path,path->fromNode);
				nme->isWaiting	= GetWaitTimeFromIndexedNode(path,path->fromNode);
				CalcEnemyNormalInterps(nme);
				return;
			}
			else if(flags & ENEMY_NEW_CYCLE)
			{
				// enenmy has cyclic movement
				path->fromNode	= 0;
				path->toNode	= GET_PATHLASTNODE(path);

				nme->speed		= GetSpeedFromIndexedNode(path,path->fromNode);
				nme->isWaiting	= GetWaitTimeFromIndexedNode(path,path->fromNode);
				CalcEnemyNormalInterps(nme);
				return;
			}

			path->fromNode	= GET_PATHLASTNODE(path);
			path->toNode	= GET_PATHLASTNODE(path) - 1;

			nme->speed		= GetSpeedFromIndexedNode(path,path->fromNode);
			nme->isWaiting	= GetWaitTimeFromIndexedNode(path,path->fromNode);
			CalcEnemyNormalInterps(nme);

			GetPositionForPathNode(&nmePos,&path->nodes[GET_PATHLASTNODE(path)]);
			SetVector(&nme->nmeActor->actor->pos,&nmePos);
		}
		else
		{
			nextFromNode = path->fromNode - 1;

			if((nme->flags & ENEMY_NEW_CYCLE) && (nextFromNode < 0))
			{
				path->fromNode	= GET_PATHLASTNODE(path);
				path->toNode	= GET_PATHLASTNODE(path) - 1;

				nme->speed		= GetSpeedFromIndexedNode(path,path->fromNode);
				nme->isWaiting	= GetWaitTimeFromIndexedNode(path,path->fromNode);
				CalcEnemyNormalInterps(nme);
				return;
			}

			path->fromNode--;
			path->toNode--;

			nme->speed		= GetSpeedFromIndexedNode(path,path->fromNode);
			nme->isWaiting	= GetWaitTimeFromIndexedNode(path,path->fromNode);
			CalcEnemyNormalInterps(nme);
		}
	}
	else if(flags & ENEMY_NEW_MOVEUP)
	{
		// path has reached 'top' of path
		// check if this enenmy has ping-pong movement
		if(flags & ENEMY_NEW_PINGPONG)
		{
			// reverse enemy movement
			nme->flags	&= ~ENEMY_NEW_MOVEUP;
			nme->flags	|= ENEMY_NEW_MOVEDOWN;

			nme->speed		= GetSpeedFromIndexedNode(path,path->fromNode);
			nme->isWaiting	= GetWaitTimeFromIndexedNode(path,path->fromNode);
			return;
		}
	}
	else if(flags & ENEMY_NEW_MOVEDOWN)
	{
		// path has reached 'bottom' of path
		// check if this enemy has ping-pong movement
		if(flags & ENEMY_NEW_PINGPONG)
		{
			// reverse enemy movement
			nme->flags	|= ENEMY_NEW_MOVEUP;
			nme->flags	&= ~ENEMY_NEW_MOVEDOWN;

			nme->speed		= GetSpeedFromIndexedNode(path,path->fromNode);
			nme->isWaiting	= GetWaitTimeFromIndexedNode(path,path->fromNode);
			return;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: CalcEnemyNormalInterps
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void CalcEnemyNormalInterps(ENEMY *nme)
{
	PATH *path;
	PATHNODE *fromNode,*toNode;
	float numSteps;
	VECTOR destNormal,fromPos,toPos;

	path = nme->path;
	if(path->numNodes < 2)
		return;

	fromNode	= &path->nodes[path->fromNode];
	toNode		= &path->nodes[path->toNode];

	// set the current enemy normal to that of the 'from' node and get the dest normal
	SetVector(&nme->currNormal,&fromNode->worldTile->normal);
	SetVector(&destNormal,&toNode->worldTile->normal);

	// calculate deltas for linear interpolation of enemy normal during movement
	SubVector(&nme->deltaNormal,&destNormal,&nme->currNormal);

	// determine number of 'steps' over which to interpolate
	GetPositionForPathNode(&fromPos,fromNode);
	GetPositionForPathNode(&toPos,toNode);
	
	numSteps = DistanceBetweenPoints(&fromPos,&toPos);
	numSteps /= nme->speed;

	nme->deltaNormal.v[X] /= numSteps;
	nme->deltaNormal.v[Y] /= numSteps;
	nme->deltaNormal.v[Z] /= numSteps;
}


//------------------------------------------------------------------------------------------------
