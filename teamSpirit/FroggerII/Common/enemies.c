/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: enemies.c
	Programmer	: Andrew Eder
	Date		: 1/12/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


ENEMY *testEnemy			= NULL;			// test enemy

ENEMYLIST enemyList;						// the enemy linked list

VECTOR snapPos;


ENEMY *devNME1	= NULL;


/**************************************************************************************************************/
/******  GARDEN LAWN LEVEL  ***********************************************************************************/
/**************************************************************************************************************/


PATHNODE debug_lawnWaspNodes1[] =
{
	415,30,0,4,5,	416,30,0,4,0,	417,30,0,4,0,	428,30,0,4,0,
	441,30,0,2,5,	440,30,0,2,0,	439,30,0,2,0,	426,30,0,2,0
};

PATHNODE debug_lawnWaspNodes2[] =
{
	421,30,0,4,5,	422,30,0,4,0,	423,30,0,2,10,	434,30,0,2,0,
	447,30,0,8,5,	446,30,0,8,0,	445,30,0,2,10,	432,30,0,2,0
};

PATHNODE debug_lawnWaspNodes3[] =
{
	467,30,0,3,10,	468,30,0,3,0,	469,30,0,3,0,	482,30,0,3,0,
	493,30,0,6,5,	492,30,0,6,0,	491,30,0,3,0,	480,30,0,3,0
};

PATHNODE debug_lawnWaspNodes4[] =
{
	453,30,0,4,0,	454,30,0,4,0,	455,30,0,4,0,	466,30,0,4,0,
	479,30,0,4,0,	478,30,0,4,0,	477,30,0,4,0,	464,30,0,4,0
};

PATHNODE debug_lawnWaspNodes5[] =
{
	461,30,0,4,10,	462,30,0,4,0,	463,30,0,2,5,	476,30,0,2,0,
	487,30,0,4,10,	486,30,0,4,0,	485,30,0,3,5,	474,30,0,3,0
};

PATHNODE debug_lawnWaspNodes6[] =
{
	232,30,0,5,0,	233,30,0,5,0,	234,30,0,5,0,	247,30,0,5,0,
	258,30,0,5,0,	257,30,0,5,0,	256,30,0,5,0,	245,30,0,5,0
};

PATHNODE debug_lawnWaspNodes7[] =
{
	235,30,0,3,0,	236,30,0,3,0,	237,30,0,3,0,	250,30,0,3,0,
	261,30,0,3,0,	260,30,0,3,0,	259,30,0,3,0,	248,30,0,3,0
};

PATHNODE debug_lawnWaspNodes8[] =
{
	238,30,0,3,0,	239,30,0,3,0,	240,30,0,3,0,	253,30,0,3,0,
	264,30,0,3,0,	263,30,0,3,0,	262,30,0,3,0,	251,30,0,3,0
};

PATHNODE debug_lawnWaspNodes9[] =
{
	241,30,0,5,0,	242,30,0,5,0,	243,30,0,5,0,	294,30,0,5,0,
	295,30,0,5,0,	266,30,0,5,0,	265,30,0,5,0,	254,30,0,5,0
};

PATHNODE debug_lawnWaspNodes10[] =
{
	532,30,0,10,0,	533,30,0,10,0,	534,30,0,10,0,	535,30,0,10,0,	536,30,0,10,0,	541,30,0,10,0,
	546,30,0,10,0,	551,30,0,10,0,	571,30,0,10,0,	554,30,0,10,0,	553,30,0,10,0,	552,30,0,10,0,
	570,30,0,10,0
};

PATHNODE debug_lawnWaspNodes11[] =
{
	538,30,0,5,0,	539,30,0,5,0,	540,30,0,5,0,	545,30,0,5,0,
	550,30,0,5,0,	549,30,0,5,0,	548,30,0,5,0,	543,30,0,5,0
};

PATH debug_lawnWasp1 = { 8,0,0,0,0,debug_lawnWaspNodes1 };
PATH debug_lawnWasp2 = { 8,0,0,0,0,debug_lawnWaspNodes2 };
PATH debug_lawnWasp3 = { 8,0,0,0,0,debug_lawnWaspNodes3 };
PATH debug_lawnWasp4 = { 8,0,0,0,0,debug_lawnWaspNodes4 };
PATH debug_lawnWasp5 = { 8,0,0,0,0,debug_lawnWaspNodes5 };

PATH debug_lawnWasp6 = { 8,0,0,1,0,debug_lawnWaspNodes6 };
PATH debug_lawnWasp7 = { 8,0,0,3,0,debug_lawnWaspNodes7 };
PATH debug_lawnWasp8 = { 8,0,0,5,0,debug_lawnWaspNodes8 };
PATH debug_lawnWasp9 = { 8,0,0,6,0,debug_lawnWaspNodes9 };

PATH debug_lawnWasp10 = { 13,0,0,4,0,debug_lawnWaspNodes10 };
PATH debug_lawnWasp11 = { 8,0,0,4,0,debug_lawnWaspNodes11 };


PATHNODE debug_lawnRollNodes1[] =
{
	287,0,0,5,0,	284,0,0,5,0,	42,0,0,5,0,		43,0,0,5,0,		44,0,0,5,0,		45,0,0,5,0,
	46,0,0,5,0,		47,0,0,5,0,		48,0,0,5,0,		49,0,0,5,0,		50,0,0,5,0,		51,0,0,5,0,
	52,0,0,5,0,		53,0,0,5,0,		54,0,0,5,0,		278,0,0,5,0,	281,0,0,5,0
};

PATHNODE debug_lawnRollNodes2[] =
{
	288,0,0,5,0,	285,0,0,5,0,	68,0,0,5,0,		69,0,0,5,0,		70,0,0,5,0,		71,0,0,5,0,
	72,0,0,5,0,		73,0,0,5,0,		74,0,0,5,0,		75,0,0,5,0,		76,0,0,5,0,		77,0,0,5,0,
	78,0,0,5,0,		79,0,0,5,0,		80,0,0,5,0,		279,0,0,5,0,	282,0,0,5,0
};

PATHNODE debug_lawnRollNodes3[] =
{
	403,0,0,5,0,	414,0,0,5,0,	425,0,0,5,0,	438,0,0,5,0,
	449,0,0,5,0,	460,0,0,5,0,	473,0,0,5,0,	484,0,0,5,0
};

PATHNODE debug_lawnRollNodes4[] =
{
	581,0,0,3,0,	578,0,0,3,0,	514,0,0,3,0,	515,0,0,3,0,
	516,0,0,3,0,	572,0,0,3,0,	575,0,0,3,0
};

PATHNODE debug_lawnRollNodes5[] =
{
	582,0,0,3,0,	579,0,0,3,0,	520,0,0,3,0,	521,0,0,3,0,
	522,0,0,3,0,	573,0,0,3,0,	576,0,0,3,0
};

PATHNODE debug_lawnRollNodes6[] =
{
	583,0,0,3,0,	580,0,0,3,0,	526,0,0,3,0,	527,0,0,3,0,
	528,0,0,3,0,	574,0,0,3,0,	577,0,0,3,0
};


PATH debug_lawnRoll1a = { 17,0,0,5,0,debug_lawnRollNodes1 };
PATH debug_lawnRoll1b = { 17,0,0,12,0,debug_lawnRollNodes1 };

PATH debug_lawnRoll2a = { 17,0,0,5,0,debug_lawnRollNodes2 };
PATH debug_lawnRoll2b = { 17,0,0,10,0,debug_lawnRollNodes2 };
PATH debug_lawnRoll2c = { 17,0,0,15,0,debug_lawnRollNodes2 };

PATH debug_lawnRoll3 = { 8,0,0,5,0,debug_lawnRollNodes3 };

PATH debug_lawnRoll4 = { 7,0,0,2,0,debug_lawnRollNodes4 };
PATH debug_lawnRoll5 = { 7,0,0,3,0,debug_lawnRollNodes5 };
PATH debug_lawnRoll6 = { 7,0,0,2,0,debug_lawnRollNodes6 };



PATHNODE debug_lawnMowNodes1[] =
{
	289,0,0,10,0,	286,0,0,10,0,	94,0,0,10,0,	95,0,0,10,0,	96,0,0,10,0,	97,0,0,10,0,
	98,0,0,10,0,	99,0,0,10,0,	100,0,0,10,0,	101,0,0,10,0,	102,0,0,10,0,	103,0,0,10,0,
	104,0,0,10,0,	105,0,0,10,0,	106,0,0,10,0,	280,0,0,10,0,	283,0,0,10,0
};

PATHNODE debug_lawnMowNodes2[] =
{
	291,0,0,5,0,	290,0,0,5,0,	141,0,0,5,0,	142,0,0,5,0,	143,0,0,5,0,	144,0,0,5,0,
	145,0,0,5,0,	146,0,0,5,0,	147,0,0,5,0,	148,0,0,5,0,	149,0,0,5,0,	150,0,0,5,0,
	151,0,0,5,0,	152,0,0,5,0,	153,0,0,5,0,	166,0,0,8,0,	179,0,0,8,0,	178,0,0,8,0,
	177,0,0,8,0,	176,0,0,8,0,	175,0,0,8,0,	174,0,0,8,0,	173,0,0,8,0,	172,0,0,8,0,
	171,0,0,8,0,	170,0,0,8,0,	169,0,0,8,0,	168,0,0,8,0,	167,0,0,8,0,	180,0,0,10,0,
	193,0,0,10,0,	194,0,0,10,0,	195,0,0,10,0,	196,0,0,10,0,	197,0,0,10,0,	198,0,0,10,0,
	199,0,0,10,0,	200,0,0,10,0,	201,0,0,10,0,	202,0,0,10,0,	203,0,0,10,0,	204,0,0,10,0,
	205,0,0,10,0,	218,0,0,10,0,	231,0,0,12,0,	230,0,0,12,0,	229,0,0,12,0,	228,0,0,12,0,
	227,0,0,12,0,	226,0,0,12,0,	225,0,0,12,0,	224,0,0,12,0,	223,0,0,12,0,	222,0,0,12,0,
	221,0,0,12,0,	220,0,0,12,0,	219,0,0,12,0,	390,0,0,12,0,	391,0,0,12,0
};


PATH debug_lawnMow1a = { 17,0,0,2,0,debug_lawnMowNodes1 };
PATH debug_lawnMow1b = { 17,0,0,9,0,debug_lawnMowNodes1 };

PATH debug_lawnMow2a = { 59,0,0,3,0,debug_lawnMowNodes2 };
PATH debug_lawnMow2b = { 59,0,0,13,0,debug_lawnMowNodes2 };
PATH debug_lawnMow2c = { 59,0,0,23,0,debug_lawnMowNodes2 };
PATH debug_lawnMow2d = { 59,0,0,33,0,debug_lawnMowNodes2 };
PATH debug_lawnMow2e = { 59,0,0,43,0,debug_lawnMowNodes2 };


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
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

ENEMY *GetEnemyFromUID (long uid)
{
	ENEMY *cur,*next;

	for(cur = enemyList.head.next; cur != &enemyList.head; cur = cur->next)
		if (cur->uid == uid)
			return cur;

	return NULL;
}

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
/*			// wasps
			devNME1 = CreateAndAddEnemy("wasp.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS | ENEMY_NEW_CYCLE,&debug_lawnWasp1,PATH_MAKENODETILEPTRS);

			devNME1 = CreateAndAddEnemy("wasp.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS | ENEMY_NEW_CYCLE,&debug_lawnWasp2,PATH_MAKENODETILEPTRS);

			devNME1 = CreateAndAddEnemy("wasp.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS | ENEMY_NEW_CYCLE,&debug_lawnWasp3,PATH_MAKENODETILEPTRS);

			devNME1 = CreateAndAddEnemy("wasp.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS | ENEMY_NEW_CYCLE,&debug_lawnWasp4,PATH_MAKENODETILEPTRS);

			devNME1 = CreateAndAddEnemy("wasp.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS | ENEMY_NEW_CYCLE,&debug_lawnWasp5,PATH_MAKENODETILEPTRS);

			devNME1 = CreateAndAddEnemy("wasp.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS | ENEMY_NEW_CYCLE,&debug_lawnWasp6,PATH_MAKENODETILEPTRS);

			devNME1 = CreateAndAddEnemy("wasp.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS | ENEMY_NEW_CYCLE,&debug_lawnWasp7,PATH_MAKENODETILEPTRS);

			devNME1 = CreateAndAddEnemy("wasp.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS | ENEMY_NEW_CYCLE,&debug_lawnWasp8,PATH_MAKENODETILEPTRS);

			devNME1 = CreateAndAddEnemy("wasp.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS | ENEMY_NEW_CYCLE,&debug_lawnWasp9,PATH_MAKENODETILEPTRS);

			devNME1 = CreateAndAddEnemy("wasp.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_BACKWARDS | ENEMY_NEW_CYCLE,&debug_lawnWasp10,PATH_MAKENODETILEPTRS);

			devNME1 = CreateAndAddEnemy("wasp.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS | ENEMY_NEW_CYCLE,&debug_lawnWasp11,PATH_MAKENODETILEPTRS);


			// rollers
			devNME1 = CreateAndAddEnemy("roll.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS,&debug_lawnRoll1a,PATH_MAKENODETILEPTRS);
			devNME1 = CreateAndAddEnemy("roll.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS,&debug_lawnRoll1b,0);

			devNME1 = CreateAndAddEnemy("roll.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS,&debug_lawnRoll2a,PATH_MAKENODETILEPTRS);
			devNME1 = CreateAndAddEnemy("roll.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS,&debug_lawnRoll2b,0);
			devNME1 = CreateAndAddEnemy("roll.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS,&debug_lawnRoll2c,0);

			devNME1 = CreateAndAddEnemy("roll.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS | ENEMY_NEW_PINGPONG,&debug_lawnRoll3,PATH_MAKENODETILEPTRS);

			devNME1 = CreateAndAddEnemy("roll.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS | ENEMY_NEW_PINGPONG,&debug_lawnRoll4,PATH_MAKENODETILEPTRS);

			devNME1 = CreateAndAddEnemy("roll.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS | ENEMY_NEW_PINGPONG,&debug_lawnRoll5,PATH_MAKENODETILEPTRS);

			devNME1 = CreateAndAddEnemy("roll.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS | ENEMY_NEW_PINGPONG,&debug_lawnRoll6,PATH_MAKENODETILEPTRS);

			// mowers
			devNME1 = CreateAndAddEnemy("moa.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS,&debug_lawnMow1a,PATH_MAKENODETILEPTRS);
			devNME1 = CreateAndAddEnemy("moa.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS,&debug_lawnMow1b,0);

			devNME1 = CreateAndAddEnemy("moa.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS,&debug_lawnMow2a,PATH_MAKENODETILEPTRS);
			devNME1 = CreateAndAddEnemy("moa.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS,&debug_lawnMow2b,0);
			devNME1 = CreateAndAddEnemy("moa.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS,&debug_lawnMow2c,0);
			devNME1 = CreateAndAddEnemy("moa.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS,&debug_lawnMow2d,0);
			devNME1 = CreateAndAddEnemy("moa.ndo");
			AssignPathToEnemy(devNME1,ENEMY_NEW_FORWARDS,&debug_lawnMow2e,0);*/

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
	else if( (nme->flags & ENEMY_NEW_ROTATEPATH_XZ) ||
			(nme->flags & ENEMY_NEW_ROTATEPATH_XY) ||
			(nme->flags & ENEMY_NEW_ROTATEPATH_ZY) )
	{
		nme->inTile = FindNearestTile( nme->nmeActor->actor->pos );
	}
	else
	{
		// no path associated with this enemy
		nme->inTile = nme->path->nodes[0].worldTile;
	}
}
/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

GAMETILE *FindJoinedTileByDirection(GAMETILE *st,VECTOR *d)
{
	float distance = 100000, t;
	int i;
	VECTOR un;
	long closest = 0;

	SetVector (&un,d);
	MakeUnit (&un);

	for (i=0; i<4; i++)
	{
		t = DotProduct(&un,&(st->dirVector[i]));
		if (t<distance)
		{
			distance = t;
			closest = i;					
		}
	}

	return st->tilePtrs[closest];
}

/*	--------------------------------------------------------------------------------
	Function		: UpdateEnemies
	Purpose			: updates all enemies and their states / movements, etc.
	Parameters		: 
	Returns			: void
	Info			: 
*/

float snapRadius = 75;
long snapTime = 50;
long SNAPPER_TIME = 10;
extern float waitScale;

void UpdateEnemies()
{
	ENEMY *cur,*next;
	VECTOR fromPosition,toPosition;
	VECTOR fwd;
	VECTOR moveVec;
	float length;
	long i;

	float tileRadiusSquared = 
		snapRadius * snapRadius;

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

			if(actFrameCount > cur->path->startFrame)
				cur->isWaiting = 0;
			else
				continue;
		}

		if ( cur->isIdle )
			cur->isIdle--;
		// ENDIF

		if(cur->flags & ENEMY_NEW_FOLLOWPATH)
		{
			// process enemies that follow a path (>1 node in path)
			
			// first, update the enemy position
/*
			GetPositionForPathNode(&toPosition,&cur->path->nodes[cur->path->toNode]);
			SubVector(&fwd,&toPosition,&cur->nmeActor->actor->pos);
			MakeUnit(&fwd);

			length = (float)(actFrameCount-cur->path->startFrame)/(float)(cur->path->endFrame-cur->path->startFrame);
			
			ScaleVector(&fwd,length);
			AddVector (&cur->nmeActor->actor->pos,&fwd,&toPosition);
*/

			GetPositionForPathNode(&toPosition,&cur->path->nodes[cur->path->toNode]);
			GetPositionForPathNode(&fromPosition,&cur->path->nodes[cur->path->fromNode]);
			
			SubVector(&fwd,&toPosition,&fromPosition);
			
			length = (float)(actFrameCount - cur->path->startFrame)/(float)(cur->path->endFrame - cur->path->startFrame);
			
			ScaleVector(&fwd,length);
			AddVector(&cur->nmeActor->actor->pos,&fwd,&fromPosition);
			MakeUnit (&fwd);

			if (cur->flags & ENEMY_NEW_PUSHESFROG)
			{
				if (DistanceBetweenPointsSquared(&frog[0]->actor->pos,&cur->nmeActor->actor->pos)<(40*40))
				{
					VECTOR direction;
					VECTOR frogDir;
					GAMETILE *tile;
					float len;

					tile = FindJoinedTileByDirection(currTile[0],&fwd);
					
					SetVector(&direction,&fwd);
					MakeUnit(&direction);

					SetVector(&frogDir,&frog[0]->actor->pos);
					SubFromVector(&frogDir,&cur->nmeActor->actor->pos);
					
					len = Magnitude(&frogDir);

					MakeUnit(&frogDir);
										
					len = (len * DotProduct(&direction,&frogDir));
					
					ScaleVector(&direction,5);//len);	

					AddToVector(&frog[0]->actor->pos,&direction);
						
					if (tile)
						if (DistanceBetweenPointsSquared(&currTile[0]->centre,&frog[0]->actor->pos)>(40*40))
					{
						currTile[0] = tile;
						destTile[0] = currTile[0];
					}
				}
			}

//			if (cur->flags & ENEMY_NEW_RANDOMSPEED)
//				if (Random(100)>50)
//					cur->speed = (Random(100)/100.0)*cur->path->nodes[cur->path->fromNode].speed;
				
//			cur->nmeActor->actor->pos.v[X] += (cur->speed * fwd.v[X]);
//			cur->nmeActor->actor->pos.v[Y] += (cur->speed * fwd.v[Y]);
//			cur->nmeActor->actor->pos.v[Z] += (cur->speed * fwd.v[Z]);
			
			
//--------------------->

			AddToVector(&cur->currNormal,&cur->deltaNormal);

//			NormalToQuaternion(&cur->nmeActor->actor->qRot,&cur->currNormal);
//			or
			if (cur->flags & ENEMY_NEW_FORWARDS)
				Orientate(&cur->nmeActor->actor->qRot,&fwd,&inVec,&cur->currNormal);
			else
			{
				ScaleVector (&fwd,-1);
				Orientate(&cur->nmeActor->actor->qRot,&fwd,&inVec,&cur->currNormal);
			}


//--------------------->

			// check if this enemy has arrived at a path node
			if(EnemyHasArrivedAtNode(cur))
			{
				UpdateEnemyPathNodes(cur);

				cur->path->startFrame += cur->isWaiting * waitScale;
				cur->path->endFrame += cur->isWaiting * waitScale;
			}
		}
		else
			if(cur->flags & ENEMY_NEW_WATCHFROG)
			{
				VECTOR vfd	= { 0,0,1 };
				VECTOR vup	= { 0,1,0 };

				VECTOR v1,v2,v3,nmeup;
				
				// Face enemy towards frog
				SubVector(&v1,&cur->nmeActor->actor->pos,&frog[0]->actor->pos);
				MakeUnit(&v1);

				RotateVectorByQuaternion(&nmeup,&vup,&cur->nmeActor->actor->qRot);
				CrossProduct(&v2,&v1,&nmeup);
				CrossProduct(&v3,&v2,&nmeup);

				Orientate(&cur->nmeActor->actor->qRot,&v3,&vfd,&nmeup);

				// Test for snap range!
								
				{
					if (cur->flags & ENEMY_NEW_SNAPFROG)
					{	
						if (cur->isSnapping > 1)
							cur->isSnapping--;
							
						if (DistanceBetweenPointsSquared(&frog[0]->actor->pos,&cur->nmeActor->actor->pos) >= tileRadiusSquared)
							cur->isSnapping=0;
						
						if (cur->isSnapping == 0)
						{
							if (Random(1000)>980)
								AnimateActor(cur->nmeActor->actor,2,NO,NO,1, 0, 0);
						
							if (Random(1000)>950)
							{
								if (Random(1000)>950)
									AnimateActor(cur->nmeActor->actor,3,NO,YES,1, 0, 0);
								else
									AnimateActor(cur->nmeActor->actor,0,NO,YES,1, 0, 0);
							}
						}

						if (DistanceBetweenPointsSquared(&frog[0]->actor->pos,&cur->nmeActor->actor->pos) < tileRadiusSquared)
						{
							if (cur->isSnapping == SNAPPER_TIME)
								AnimateActor(cur->nmeActor->actor,1,NO,NO,1, 0, 0);
								
							if (cur->isSnapping == 0)
							{
								cur->isSnapping = cur->path->nodes[0].waitTime;
							}
							else
								if (cur->isSnapping == 1)
								{
									cur->isSnapping = 0;
									frog[0]->action.lives--;
									
									CreateAndAddFXSwarm(SWARM_TYPE_STARSTUN,&frog[0]->actor->pos,64,25,35);
									CreateAndAddFXSwarm(SWARM_TYPE_STARSTUN,&frog[0]->actor->pos,64,35,10);

									if(frog[0]->action.lives != 0)
									{
										cameraShake = 25;
										PlaySample(42,NULL,192,128);
										frog[0]->action.safe = 25;
									}
									else
									{
										PlaySample(110,NULL,192,128);
										AnimateActor(frog[0]->actor,21,NO,NO,0.3, 0, 0);
										frog[0]->action.dead = 50;
										frog[0]->action.lives = 3;
										frog[0]->action.deathBy = DEATHBY_NORMAL;
										player[0].frogState |= FROGSTATUS_ISDEAD;
									}
								}
						}
						else
							cur->isSnapping = 0;
					}
				}
			}
			else
			{
				// process enemies that are based on a single node
	
				if( (cur->flags & ENEMY_NEW_MOVEUP) || (cur->flags & ENEMY_NEW_MOVEDOWN) )
				{
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

				// Move around a single flag
				if( (cur->flags & ENEMY_NEW_ROTATEPATH_XZ ) ||
					(cur->flags & ENEMY_NEW_ROTATEPATH_XY) ||
					(cur->flags & ENEMY_NEW_ROTATEPATH_ZY) )
				{
					VECTOR v1,v2,v3,nmeup;
					fromPosition = cur->nmeActor->actor->pos;

					length = (float)(actFrameCount - cur->path->startFrame)/(float)(cur->path->endFrame - cur->path->startFrame);
					length *= PI2;

					if( cur->flags & ENEMY_NEW_ROTATEPATH_XZ )
					{
						toPosition.v[X] = cur->path->nodes->worldTile->centre.v[X] + (cur->nmeActor->radius * sinf(length));
						toPosition.v[Z] = cur->path->nodes->worldTile->centre.v[Z] + (cur->nmeActor->radius * cosf(length));
						toPosition.v[Y] = cur->path->nodes->worldTile->centre.v[Y] + cur->path->nodes->offset;
					}
					else if( cur->flags & ENEMY_NEW_ROTATEPATH_XY )
					{
						toPosition.v[X] = cur->path->nodes->worldTile->centre.v[X] + (cur->nmeActor->radius * sinf(length));
						toPosition.v[Y] = cur->path->nodes->worldTile->centre.v[Y] + (cur->nmeActor->radius * cosf(length)) + cur->path->nodes->offset;
						toPosition.v[Z] = cur->path->nodes->worldTile->centre.v[Z];
					}
					else if( cur->flags & ENEMY_NEW_ROTATEPATH_ZY )
					{
						toPosition.v[Y] = cur->path->nodes->worldTile->centre.v[Y] + (cur->nmeActor->radius * cosf(length)) + cur->path->nodes->offset;
						toPosition.v[Z] = cur->path->nodes->worldTile->centre.v[Z] + (cur->nmeActor->radius * sinf(length));
						toPosition.v[X] = cur->path->nodes->worldTile->centre.v[X];
					}

					if( cur->flags & ENEMY_NEW_FACEFORWARDS ) // Look in direction of travel
					{
						SubVector(&v1,&fromPosition,&toPosition);
						MakeUnit(&v1);
						RotateVectorByQuaternion(&nmeup,&upVec,&cur->nmeActor->actor->qRot);
						CrossProduct(&v2,&v1,&nmeup);
						CrossProduct(&v3,&v2,&nmeup);
						Orientate(&cur->nmeActor->actor->qRot,&v3,&inVec,&nmeup);
					}

					cur->nmeActor->actor->pos = toPosition;

					if( actFrameCount > cur->path->endFrame )
					{
						cur->path->startFrame = cur->path->endFrame;
						cur->path->endFrame = cur->path->startFrame+(60*cur->speed);
					}
				}
			}

		// determine which world tile the enemy is currently 'in'
		oldTile[0] = currTile[0];
		GetEnemyActiveTile(cur);

		// Enemy homes in on frog - no tiles!
		if( cur->flags & ENEMY_NEW_HOMING )
		{
			GAMETILE *chTile;
			VECTOR nmeup, tVec, v2, v3;
			float distance, best=-2;
			short bFlag = 0;
			SubVector( &moveVec, &frog[0]->actor->pos, &cur->nmeActor->actor->pos );
			MakeUnit( &moveVec );
			chTile = FindNearestTile( cur->nmeActor->actor->pos );

			// Do check for close direction vector from tile. If none match closely, do not move.
			for( i=0; i<4; i++ )
				if( chTile->tilePtrs[i] )
				{
					// Direction to tile
					SubVector( &tVec, &chTile->tilePtrs[i]->centre, &chTile->centre );
					MakeUnit( &tVec );
					// Cosine of angle between vectors
					distance = DotProduct(&tVec,&moveVec);
					if( distance > best )
						best = distance;
				}
				else
					bFlag = 1; // There is some invalid tile

			// If the best direction match is close enough we can carry on (approx 45 degrees)
			// Also check that we're over a tile.
			if( best > 0.7 )
			{
				ScaleVector( &moveVec, cur->speed );
				AddVector( &tVec, &moveVec, &cur->nmeActor->actor->pos );
				chTile = FindNearestTile( tVec );
				SubVector( &moveVec, &chTile->centre, &tVec );
				distance = abs(Magnitude( &moveVec ));
				if( distance < 35 || !bFlag )
					cur->nmeActor->actor->pos = tVec;
			}
		}

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
	
					CreateAndAddFXSwarm(SWARM_TYPE_STARSTUN,&frog[0]->actor->pos,64,25,35);
					CreateAndAddFXSwarm(SWARM_TYPE_STARSTUN,&frog[0]->actor->pos,64,35,10);
				}
				else
				{
					PlaySample(110,NULL,192,128);
					AnimateActor(frog[0]->actor,2,NO,NO,0.367, 0, 0);
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
				(!currPlatform[0]) && !(player[0].frogState & FROGSTATUS_ISFLOATING))
		{
			frog[0]->action.lives--;
			if(frog[0]->action.lives != 0)
			{
				cameraShake = 25;
				PlaySample(42,NULL,192,128);
				frog[0]->action.safe = 25;

				CreateAndAddFXSwarm(SWARM_TYPE_STARSTUN,&frog[0]->actor->pos,64,25,35);
				CreateAndAddFXSwarm(SWARM_TYPE_STARSTUN,&frog[0]->actor->pos,64,35,10);
			}
			else
			{
				PlaySample(110,NULL,192,128);
				AnimateActor(frog[0]->actor,2,NO,NO,0.367, 0, 0);
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
			case NMETYPE_MOLE:
				ProcessNMEMole(cur);
				break;
			case NMETYPE_MOWER:
				ProcessNMEMower(cur->nmeActor);
				break;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function		: ProcessNMEMole
	Purpose			: 
	Parameters		: ACTOR2 *
	Returns			: void
	Info			:
*/
void ProcessNMEMole(ENEMY *nme)
{
	switch ( nme->nmeActor->actor->status )
	{
		case NMESTATE_MOLE_IDLE:
			break;
		case NMESTATE_MOLE_SNAPPING:
			break;
		case NMESTATE_MOLE_UNDER_GROUND:
				if ( ( nme->isIdle ) && ( nme->nmeActor->actor->animation->currentAnimation == 0 ) )
					nme->nmeActor->actor->animation->animTime = 1;
				else
				{
					if ( nme->nmeActor->actor->animation->reachedEndOfAnimation )
					{	
						if ( Random(2) == 1 )
						{
							nme->nmeActor->actor->status = NMESTATE_MOLE_LOOK;
							AnimateActor ( nme->nmeActor->actor, 5, NO, NO, 0.01F, 10, 0 );
						}
						else
						{
							nme->nmeActor->actor->status = NMESTATE_MOLE_SCRATCH;
							AnimateActor ( nme->nmeActor->actor, 6, NO, NO, 0.01F, 10, 0 );
						}
						// ENDIF
					}
					// ENDIF
				}
				// ENDIF
			break;
		case NMESTATE_MOLE_LOOK:
				if ( nme->nmeActor->actor->animation->reachedEndOfAnimation )
				{	
					AnimateActor ( nme->nmeActor->actor, 2, NO, NO, 0.01F, 10, 1 );
//					AnimateActor ( nme->nmeActor->actor, 0, NO, YES, 0.5F, 5, 1 );
//					nme->nmeActor->actor->status = NMESTATE_MOLE_UNDER_GROUND;
//					nme->isIdle = Random(500);
				}
				// ENDIF
				
			break;

		case NMESTATE_MOLE_SCRATCH:
				if ( nme->nmeActor->actor->animation->reachedEndOfAnimation )
				{	
					AnimateActor ( nme->nmeActor->actor, 2, NO, NO, 0.01F,10, 0 );
//					AnimateActor ( nme->nmeActor->actor, 0, NO, YES, 0.5F, 5, 1 );
//					nme->nmeActor->actor->status = NMESTATE_MOLE_UNDER_GROUND;
//					nme->isIdle = Random(500);

				}
				// ENDIF
				
			break;

	}
	// ENDSWITCH
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
	if((nme->distanceFromFrog < ACTOR_DRAWDISTANCEINNER) && (frameCount & 3) == 0)
		CreateAndAddFXSmoke(SMOKE_TYPE_NORMAL,&nme->actor->pos,128,1,1,15);
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
				AnimateActor ( nme->actor, 1, NO, NO, 1.5F, 0, 0);
			}
			else
			{
				if(nme->actor->animation->reachedEndOfAnimation)
				{
					// Choose an idle animation at random
					AnimateActor(nme->actor,0,NO,NO,1.0F, 0, 0);
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
						AnimateActor(frog[0]->actor,2,NO,NO,0.35F, 0, 0);
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
						AnimateActor(nme->actor,3,NO,NO,1.0F, 0, 0);
						PlaySample ( 75,NULL,192,128);
					}
				}
				// ENDIF
			}
			// ENDIF
			if(nme->actor->animation->reachedEndOfAnimation)
			{
				nme->actor->status	= NMESTATE_DOG_RETURN;
				AnimateActor(nme->actor,2,NO,NO,1.0F, 0, 0);
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
					AnimateActor(nme->actor,2,NO,NO,1.0F, 0, 0);
				}
			break;
		case NMESTATE_DOG_RETURN:
				if(nme->actor->animation->reachedEndOfAnimation)
				{
					nme->actor->status	= NMESTATE_DOG_IDLE;
					AnimateActor(nme->actor,0,NO,NO,1.0F, 0, 0);
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
					AnimateActor(nme->actor,Random(3)+1,NO,NO,0.75F, 0, 0);
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
				AnimateActor(nme->actor,0,NO,NO,1.0F, 0, 0);
			}

			break;

		case NMESTATE_SNAPPER_SNAPPING:

			if((nme->actor->animation->animTime > 11.0F) && (nme->actor->animation->animTime < 13.0F))
			{
				// Check if frog is still in snapping range
				if(DistanceBetweenPoints(&snapPos,&nme->actor->pos) < 3600.0F && (!frog[0]->action.dead))
				{
					// kill frog
					AnimateActor(frog[0]->actor,2,NO,NO,0.35F, 0, 0);
					frog[0]->action.deathBy = DEATHBY_NORMAL;
					player[0].frogState |= FROGSTATUS_ISDEAD;
					frog[0]->action.dead = 50;
				}
			}

			if(nme->actor->animation->reachedEndOfAnimation)
			{
				nme->actor->status	= NMESTATE_SNAPPER_IDLE;
				AnimateActor(nme->actor,1,NO,NO,0.75F, 0, 0);
			}

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
		CreateAndAddFXSmoke(SMOKE_TYPE_NORMAL,&smokePos,128,1,1,15);
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

	enemyType = NMETYPE_WASP;

	
//stringChange ( eActorName );
	// check the actor name to determine the enemy type
	/*
	if(gstrcmp(eActorName,"roto.obe") == 0)
		enemyType = NMETYPE_MOWER;
	else if(gstrcmp(eActorName,"lomoa.obe") == 0)
		enemyType = NMETYPE_MOWER;
	else if(gstrcmp(eActorName,"moa.obe") == 0)
		enemyType = NMETYPE_MOWER;
	else if(gstrcmp(eActorName,"roll.obe") == 0)
		enemyType = NMETYPE_ROLLER;
	else if(gstrcmp(eActorName,"wasp.obe") == 0)
		enemyType = NMETYPE_WASP;
	else if(gstrcmp(eActorName,"b.obe") == 0)
		enemyType = NMETYPE_WASP;
	else if(gstrcmp(eActorName,"mole.obe") == 0)
		enemyType = NMETYPE_MOLE;
*/

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
			case NMETYPE_MOLE:
					AnimateActor(newItem->nmeActor->actor,3,NO,NO,0.1F, 0, 0);
					newItem->nmeActor->actor->status = NMESTATE_MOLE_IDLE;
					newItem->nmeActor->actor->scale.v[X] = 0.0075F;
					newItem->nmeActor->actor->scale.v[Y] = 0.0075F;
					newItem->nmeActor->actor->scale.v[Z] = 0.0075F;
					newItem->isIdle = Random(500);
				break;

			case NMETYPE_MOWER:
			case NMETYPE_ROLLER:
				AnimateActor(newItem->nmeActor->actor,0,YES,NO,1.5F, 0, 0);
				newItem->nmeActor->actor->status = NMESTATE_MOWER_IDLE;
				break;
			case NMETYPE_WASP:
				AnimateActor(newItem->nmeActor->actor,0,YES,NO,2.0F, 0, 0);
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
	newItem->isSnapping 	= 0;

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

	// set the start position for the enemy
	nme->path->fromNode	= nme->path->startNode;

	if(enemyFlags & ENEMY_NEW_FORWARDS)
	{
		// this enemy moves forward thru path nodes
		nme->flags				|= ENEMY_NEW_FOLLOWPATH;
		nme->path->toNode = nme->path->fromNode + 1;
		if(nme->path->toNode > GET_PATHLASTNODE(path))
			nme->path->toNode = 0;
	}
	else if(enemyFlags & ENEMY_NEW_BACKWARDS)
	{
		// this enemy moves backward thru path nodes
		nme->flags				|= ENEMY_NEW_FOLLOWPATH;
		nme->path->toNode = nme->path->fromNode - 1;
		if(nme->path->toNode < 0)
			nme->path->toNode = GET_PATHLASTNODE(path);
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

	nme->path->startFrame = actFrameCount;
	nme->path->endFrame = (actFrameCount+(60*nme->speed));

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

	if (actFrameCount>path->endFrame)
	{
		return TRUE;
	}

	// check if path node is reached
/*	GetPositionForPathNode(&nodePos,&path->nodes[path->toNode]);
	if(DistanceBetweenPointsSquared(&nme->nmeActor->actor->pos,&nodePos) <  ((nme->speed + 0.1F) * (nme->speed + 0.1F)))
	{
		dprintf""));
		return TRUE;
	}
*/
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
	path->startFrame = path->endFrame;
	path->endFrame = path->startFrame+(60*nme->speed);
	nme->isWaiting = 0;

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
