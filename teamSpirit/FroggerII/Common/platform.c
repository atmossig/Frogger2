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

PLATFORM *lillyList1[7];

unsigned long lillyPath1[]		 = { 8,	273,115,123,132,139,148,158,167 };
unsigned long lillyHeightPath1[] = { 8,	0,0,0,0,0,0,0,0 };
float		  lillySpeedPath1[]  = { 8,	4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0 };

unsigned long lillyPath2[]		 = { 8,	272,114,122,131,138,147,157,166 };
unsigned long lillyHeightPath2[] = { 8,	0,0,0,0,0,0,0,0 };
float		  lillySpeedPath2[]  = { 8,	4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0 };

unsigned long lillyPath3[]		 = { 8,	186,188,189,190,191,192,193,194 };
unsigned long lillyHeightPath3[] = { 8,	0,0,0,0,0,0,0,0 };
float		  lillySpeedPath3[]  = { 8,	4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0 };

unsigned long lillyPath4[]		 = { 8,	207,208,209,210,211,212,213,214 };
unsigned long lillyHeightPath4[] = { 8,	0,0,0,0,0,0,0,0 };
float		  lillySpeedPath4[]  = { 8,	4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0 };

unsigned long lillyPath5[]		 = { 8,	216,218,219,220,221,222,223,224 };
unsigned long lillyHeightPath5[] = { 8,	0,0,0,0,0,0,0,0 };
float		  lillySpeedPath5[]  = { 8,	4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0 };

unsigned long lillyPath6[]		 = { 8,	226,228,229,230,231,232,233,234 };
unsigned long lillyHeightPath6[] = { 8,	0,0,0,0,0,0,0,0 };
float		  lillySpeedPath6[]  = { 8,	4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0 };

unsigned long lillyPath7[]		 = { 8,	 247,248,249,250,251,252,253,254};
unsigned long lillyHeightPath7[] = { 8,	0,0,0,0,0,0,0,0 };
float		  lillySpeedPath7[]  = { 8,	4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0 };

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

// test platforms
PLATFORM *locPlatform1		= NULL;
PLATFORM *locPlatform2		= NULL;
PLATFORM *locPlatform3		= NULL;

PLATFORM *locPlatform4		= NULL;
PLATFORM *locPlatform5		= NULL;
PLATFORM *locPlatform6		= NULL;
PLATFORM *locPlatform7		= NULL;
PLATFORM *locPlatform8		= NULL;
PLATFORM *locPlatform9		= NULL;

PLATFORM *testPlatform1		= NULL;
PLATFORM *testPlatform2		= NULL;
PLATFORM *testPlatform3		= NULL;
PLATFORM *testPlatform4		= NULL;
PLATFORM *testPlatform5		= NULL;
PLATFORM *testPlatform6		= NULL;
PLATFORM *testPlatform7		= NULL;
PLATFORM *testPlatform8		= NULL;

PLATFORM *testPlatform9		= NULL;
PLATFORM *testPlatform10	= NULL;
PLATFORM *testPlatform11	= NULL;
PLATFORM *testPlatform12	= NULL;
PLATFORM *testPlatform13	= NULL;
PLATFORM *testPlatform14	= NULL;
PLATFORM *testPlatform15	= NULL;
PLATFORM *testPlatform16	= NULL;
PLATFORM *testPlatform17	= NULL;
PLATFORM *testPlatform18	= NULL;


PLATFORM *plank1		= NULL;
PLATFORM *plank2		= NULL;
PLATFORM *plank3		= NULL;
PLATFORM *plank4		= NULL;

PLATFORM *bus1		= NULL;
PLATFORM *bus2		= NULL;
PLATFORM *bus3		= NULL;

PLATFORM *devPlat1	= NULL;
PLATFORM *devPlat2	= NULL;



unsigned long log1[]	= {	15,		184,181,55,56,57,58,59,60,61,62,63,64,65,178,187 };
unsigned long log2[]	= {	15,		185,182,66,67,68,69,70,71,72,73,74,75,76,179,188 };
unsigned long log3[]	= {	7,		81,82,83,84,85,86,87 };
unsigned long log4[]	= { 3,		77,78,79 };

unsigned long path1[]	= { 1,		446 };

unsigned long path2[]	= { 1,		26 };
unsigned long path3[]	= { 1,		27 };
unsigned long path4[]	= { 1,		28 };
unsigned long path5[]	= { 1,		29 };

unsigned long p1[] = { 1,		527};
unsigned long p2[] = { 1,		528};
unsigned long p3[] = { 1,		529};

unsigned long place1[]		= { 1,		300};
float		  placeSpeed1[] = { 1,		0.0};
unsigned long place2[]		= { 1,		301};
float		  placeSpeed2[] = { 1,		0.0};
unsigned long place3[]		= { 1,		302};
float		  placeSpeed3[]	= { 1,		0.0};
unsigned long place4[]		= { 1,		292};
float		  placeSpeed4[]	= { 1,		0.0};

unsigned long ware1[] = { 1,		38};
unsigned long ware2[] = { 1,		31};

unsigned long ppath1[]		= { 9,		45,46,47,48,49,50,51,52,53 };
unsigned long ppathHight1[]	= { 9,		50,50,50,50,75,100,116,132,150 };
float ppathSpeed1[]	= { 9,		2.0,2.0,2.0,2.0,2.0,2.0,2.0,2.0,2.0 };

unsigned long ppath2[]		= { 7,		99,311,100,101,102,103,104  };
unsigned long ppathHight2[]	= { 7,		200,200,200,187,174,162,150 };
float ppathSpeed2[]	= { 7,		2.0,2.0,2.0,2.0,2.0,2.0,2.0 };

unsigned long ppath3[]		= { 1,		88 };
unsigned long ppath4[]		= { 1,		95 };
unsigned long ppath5[]		= { 1,		109 };

unsigned long wwpath1[]			= { 15,		450,203,204,205,206,207,208,445,466,209,209,227,245,448,449 };
unsigned long wwpathHight1[]	= { 15,		50,50,50,50,50,50,50,50,0,0,50,10,50,25,50 };
float		  wwpathSpeed1[]	= { 15,		2.0,2.0,2.0,2.0,2.0,2.0,2.0,15.0,15.0,6.0,5.0,4.0,3.0,2.0,2.0 };

unsigned long wwpath2[]			= { 15,		451,221,222,223,224,225,226,446,467,227,227,245,448,449,470 };
unsigned long wwpathHight2[]	= { 15,		50,50,50,50,50,50,50,50,0,0,50,10,50,25,50 };
float		  wwpathSpeed2[]	= { 15,		2.0,2.0,2.0,2.0,2.0,2.0,2.0,15.0,15.0,6.0,5.0,4.0,3.0,2.0,2.0 };

unsigned long wwpath3[]			= { 15,		452,239,240,241,242,243,244,447,468,245,245,448,449,470,471 };
unsigned long wwpathHight3[]	= { 15,		50,50,50,50,50,50,50,50,0,0,50,10,50,25,50 };
float		  wwpathSpeed3[]	= { 15,		2.0,2.0,2.0,2.0,2.0,2.0,2.0,15.0,15.0,6.0,5.0,4.0,3.0,2.0,2.0 };

unsigned long busPath1[]    = { 30,		647,645,643,414,413,412,411,410,409,408,
										407,406,405,404,403,402,401,400,399,398,
										397,396,395,394,393,392,391,649,651,653 };
unsigned long busPathHight1[]	= { 20,		50,50,50,50,50,50,50,50,50,50,
											50,50,50,50,50,50,50,50,50,50,
											50,50,50,50,50,50,50,50,50,50 };
float		  busPathSpeed1[]	= { 20,		6.0,6.0,6.0,6.0,6.0,6.0,6.0,6.0,6.0,6.0,
											6.0,6.0,6.0,6.0,6.0,6.0,6.0,6.0,6.0,6.0,
											6.0,6.0,6.0,6.0,6.0,6.0,6.0,6.0,6.0,6.0 };

unsigned long	devPath[]		= {	6,		31,32,33,34,35,36 };
unsigned long	devPathHeight[]	= { 6,		15,15,15,15,15,15 };
float			devPathSpeed[]	= { 6,		4,4,4,4,4,4 };



PATHNODE debug_pathNodes1[] = { 20,20,0, 21,50,0, 22,20,0, 23,50,0, 15,20,0, 11,50,0, 10,20,0, 9,50,0, 8,20,0, 12,50,0 };
PATH debug_path1 = { 10,0,0,0,debug_pathNodes1 };

PATHNODE debug_pathNodes2[] = { 14,5,45 };
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

	if ( worldID == WORLDID_GARDEN)
	{
		if(levelID == LEVELID_GARDENLAWN)
		{
			devPlat1 = NEW_CreateAndAddPlatform("pltlilly.ndo");
			devPlat1->currSpeed = 6.0F;
//			NEW_AssignPathToPlatform(devPlat1,PLATFORM_NEW_FORWARDS | PLATFORM_NEW_PINGPONG,&debug_path1,PATH_MAKENODETILEPTRS);
			NEW_AssignPathToPlatform(devPlat1,PLATFORM_NEW_FORWARDS | PLATFORM_NEW_CYCLE,&debug_path1,PATH_MAKENODETILEPTRS);
//			NEW_AssignPathToPlatform(devPlat1,PLATFORM_NEW_BACKWARDS | PLATFORM_NEW_CYCLE,&debug_path1,PATH_MAKENODETILEPTRS);

			devPlat2 = NEW_CreateAndAddPlatform("pltlilly.ndo");
			devPlat2->currSpeed = 2.0F;
			NEW_AssignPathToPlatform(devPlat2,PLATFORM_NEW_MOVEUP | PLATFORM_NEW_STEPONACTIVATED,&debug_path2,PATH_MAKENODETILEPTRS);
		}

		if ( levelID == LEVELID_GARDENMAZE )
		{
			/*lillyList1[0] = CreateAndAddPlatform ( "pltlilly.ndo", lillyPath1, lillyHeightPath1,
													0, 0, 2, lillySpeedPath1, 0.0f,
													PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_PATHBOUNCE | PLATFORM_FLATLEVEL | PLATFORM_INWATER );
			lillyList1[1] = CreateAndAddPlatform ( "pltlilly.ndo", lillyPath2, lillyHeightPath2,
													0, 0, 2, lillySpeedPath2, 0.0f,
													PLATFORM_HASPATH | PLATFORM_PATHBACKWARDS | PLATFORM_PATHBOUNCE | PLATFORM_FLATLEVEL | PLATFORM_INWATER );
			lillyList1[2] = CreateAndAddPlatform ( "pltlilly.ndo", lillyPath3, lillyHeightPath3,
													0, 0, 2, lillySpeedPath3, 0.0f,
													PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_PATHBOUNCE | PLATFORM_FLATLEVEL | PLATFORM_INWATER );
			lillyList1[3] = CreateAndAddPlatform ( "pltlilly.ndo", lillyPath4, lillyHeightPath4,
													0, 0, 2, lillySpeedPath4, 0.0f,
													PLATFORM_HASPATH | PLATFORM_PATHBACKWARDS | PLATFORM_PATHBOUNCE | PLATFORM_FLATLEVEL | PLATFORM_INWATER );
			// local platforms 1,2,3,etc
			for(i=0; i<4; i++)
			{
				for ( c = 0; c < 4; c++ )
				{
					if ( lillyList1[i]->inTile->tilePtrs[c] )
						SubVector(&lillyList1[i]->initDirVector[c],&lillyList1[i]->inTile->tilePtrs[c]->centre,&lillyList1[i]->inTile->centre);
					// ENDIF
					FlipVector(&lillyList1[i]->initDirVector[c]);
					MakeUnit(&lillyList1[i]->initDirVector[c]);
					SetVector(&lillyList1[i]->dirVector[c],&lillyList1[i]->initDirVector[c]);
					RotateVectorByQuaternion(&lillyList1[i]->initDirVector[c],&lillyList1[i]->initDirVector[c],&lillyList1[i]->pltActor->actor->qRot);
				}
				// ENDFOR
			}  */
			// ENDFOR
		}	   
		// ENDIF
		if ( levelID == LEVELID_GARDENVEGPATCH )
		{
			lillyList2[0] = CreateAndAddPlatform ( "pltlilly.ndo", lillyPath8, lillyHeightPath8,
													0, 0, 2, lillySpeedPath8, 0.0f,
													PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_PATHCYCLE | PLATFORM_FLATLEVEL );
			lillyList2[1] = CreateAndAddPlatform ( "pltlilly.ndo", lillyPath8, lillyHeightPath8,
													0, 0, 6, lillySpeedPath8, 0.0f,
													PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_PATHCYCLE | PLATFORM_FLATLEVEL );
			// local platforms 1,2,3,etc
			for(i=0; i<2; i++)
			{
				for ( c = 0; c < 4; c++ )
				{
					if ( lillyList2[i]->inTile->tilePtrs[c] )
						SubVector(&lillyList2[i]->initDirVector[c],&lillyList2[i]->inTile->tilePtrs[c]->centre,&lillyList2[i]->inTile->centre);
					// ENDIF
					FlipVector(&lillyList2[i]->initDirVector[c]);
					MakeUnit(&lillyList2[i]->initDirVector[c]);
					SetVector(&lillyList2[i]->dirVector[c],&lillyList2[i]->initDirVector[c]);
					RotateVectorByQuaternion(&lillyList2[i]->initDirVector[c],&lillyList2[i]->initDirVector[c],&lillyList2[i]->pltActor->actor->qRot);
				}
				// ENDFOR
			}
			// ENDFOR  
		}
		// ENDIF
	}
	// ENDIF

	if ( worldID == WORLDID_SUPERRETRO)
	{
		if ( levelID == LEVELID_SUPERRETRO1 )
		{

		locPlatform1 = CreateAndAddPlatform (	"pltlogend.ndo", log1, 0, 0, 0, 3, 0, 0.0f,
	  PLATFORM_HASPATH | PLATFORM_PATHBACKWARDS | PLATFORM_PATHEND2START | PLATFORM_FLATLEVEL | PLATFORM_SEGMENT );
		locPlatform1->pltActor->action.isSaved = 1;

//		locPlatform2 = CreateAndAddPlatform (	"pltlogend.ndo", log1, 0, 0, 4, 3.0F,
//    PLATFORM_HASPATH | PLATFORM_PATHBACKWARDS | PLATFORM_PATHEND2START | PLATFORM_FLATLEVEL | PLATFORM_SEGMENT );
		locPlatform2 = CreateAndAddPlatform (	"logsqb.ndo", log1, 0, 0, 0, 4, 0, 0.0f,
    PLATFORM_HASPATH | PLATFORM_PATHBACKWARDS | PLATFORM_PATHEND2START | PLATFORM_FLATLEVEL | PLATFORM_SEGMENT );

		locPlatform3 = CreateAndAddPlatform (	"pltlogend.ndo", log1, 0, 0, 0, 5, 0, 0.0f,
    PLATFORM_HASPATH | PLATFORM_PATHBACKWARDS | PLATFORM_PATHEND2START | PLATFORM_FLATLEVEL | PLATFORM_SEGMENT );
		locPlatform3->pltActor->action.isSaved = 1;

		locPlatform4 = CreateAndAddPlatform (	"turtl.ndo", log2, 0, 2, 0, 3, 0, 0.0f,
    PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_PATHEND2START | PLATFORM_FLATLEVEL | PLATFORM_SEGMENT );

		locPlatform5 = CreateAndAddPlatform (	"turtl.ndo", log2, 0, 2, 0, 4, 0, 0.0f,
    PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_PATHEND2START | PLATFORM_FLATLEVEL | PLATFORM_SEGMENT );

		locPlatform6 = CreateAndAddPlatform (	"turtl.ndo", log2, 0, 2, 0, 7, 0, 0.0f,
    PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_PATHEND2START | PLATFORM_FLATLEVEL | PLATFORM_SEGMENT );

		locPlatform7 = CreateAndAddPlatform (	"turtl.ndo", log2, 0, 2, 0, 8, 0, 0.0f,
    PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_PATHEND2START | PLATFORM_FLATLEVEL | PLATFORM_SEGMENT );

		locPlatform8 = CreateAndAddPlatform (	"pltlilly.ndo", log3, 0, -5, 0, 0, 0, 0.0f,
    PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_PATHBOUNCE | PLATFORM_FLATLEVEL );

		locPlatform9 = CreateAndAddPlatform (	"pltlilly.ndo", log4, 0, -5, 0, 0, 0, 0.0f,
    PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_PATHBOUNCE | PLATFORM_FLATLEVEL );

	// updown only
	testPlatform1 = CreateAndAddPlatform (	"pltlogend.ndo", log1, 0, -5, -5, 9, 0, 0.0f,
    PLATFORM_HASPATH | PLATFORM_PATHBACKWARDS | PLATFORM_PATHEND2START | PLATFORM_FLATLEVEL );
/*
	testPlatform2 = CreateAndAddPlatform (	"pltlogend.ndo", path2, 0,0, /*25, 25,*//* 0, 1.0F,
    PLATFORM_SINKABLE );
	
	testPlatform3 = CreateAndAddPlatform (	"pltlogend.ndo", path3, 0,0, /*40, 40,*//* 0, 1.0F,
    PLATFORM_SINKABLE );
	
	testPlatform4 = CreateAndAddPlatform (	"pltlogend.ndo", path4, 0,0, /*60, 60,*//* 0, 1.0F,
    PLATFORM_SINKABLE );
	
	testPlatform5 = CreateAndAddPlatform (	"pltlogend.ndo", path5, 0,0, /*5, 5,*/ /*0, 1.0F,
    PLATFORM_SINKABLE );

	// temporarily alter game tiles to make raised gametile segments
	firstTile[25].centre.v[Y] = 10.0F;
	firstTile[26].centre.v[Y] = 25.0F;
	firstTile[27].centre.v[Y] = 40.0F;
	firstTile[28].centre.v[Y] = 60.0F;
	firstTile[29].centre.v[Y] = 5.0F;
*/
		// local platforms 1,2,3,etc
		for(i=0; i<4; i++)
		{
			SubVector(&locPlatform1->initDirVector[i],&locPlatform1->inTile->tilePtrs[i]->centre,&locPlatform1->inTile->centre);
			SubVector(&locPlatform2->initDirVector[i],&locPlatform2->inTile->tilePtrs[i]->centre,&locPlatform2->inTile->centre);
			SubVector(&locPlatform3->initDirVector[i],&locPlatform3->inTile->tilePtrs[i]->centre,&locPlatform3->inTile->centre);
			FlipVector(&locPlatform1->initDirVector[i]);
			FlipVector(&locPlatform2->initDirVector[i]);
			FlipVector(&locPlatform3->initDirVector[i]);

			SubVector(&locPlatform4->initDirVector[i],&locPlatform4->inTile->tilePtrs[i]->centre,&locPlatform4->inTile->centre);
			SubVector(&locPlatform5->initDirVector[i],&locPlatform5->inTile->tilePtrs[i]->centre,&locPlatform5->inTile->centre);
			SubVector(&locPlatform6->initDirVector[i],&locPlatform6->inTile->tilePtrs[i]->centre,&locPlatform6->inTile->centre);
			SubVector(&locPlatform7->initDirVector[i],&locPlatform7->inTile->tilePtrs[i]->centre,&locPlatform7->inTile->centre);
			FlipVector(&locPlatform4->initDirVector[i]);
			FlipVector(&locPlatform5->initDirVector[i]);
			FlipVector(&locPlatform6->initDirVector[i]);
			FlipVector(&locPlatform7->initDirVector[i]);

			SubVector(&locPlatform8->initDirVector[i],&locPlatform8->inTile->tilePtrs[i]->centre,&locPlatform8->inTile->centre);
			SubVector(&locPlatform9->initDirVector[i],&locPlatform9->inTile->tilePtrs[i]->centre,&locPlatform9->inTile->centre);
			FlipVector(&locPlatform8->initDirVector[i]);
			FlipVector(&locPlatform9->initDirVector[i]);

			SubVector(&testPlatform1->initDirVector[i],&testPlatform1->inTile->tilePtrs[i]->centre,&testPlatform1->inTile->centre);
	//		SubVector(&testPlatform2->initDirVector[i],&testPlatform2->inTile->tilePtrs[i]->centre,&testPlatform2->inTile->centre);
	//		SubVector(&testPlatform3->initDirVector[i],&testPlatform3->inTile->tilePtrs[i]->centre,&testPlatform3->inTile->centre);
	//		SubVector(&testPlatform4->initDirVector[i],&testPlatform4->inTile->tilePtrs[i]->centre,&testPlatform4->inTile->centre);
	//		SubVector(&testPlatform5->initDirVector[i],&testPlatform5->inTile->tilePtrs[i]->centre,&testPlatform5->inTile->centre);
			FlipVector(&testPlatform1->initDirVector[i]);
	//		FlipVector(&testPlatform2->initDirVector[i]);
	//		FlipVector(&testPlatform3->initDirVector[i]);
	//		FlipVector(&testPlatform4->initDirVector[i]);
	//		FlipVector(&testPlatform5->initDirVector[i]);

			MakeUnit(&locPlatform1->initDirVector[i]);
			MakeUnit(&locPlatform2->initDirVector[i]);
			MakeUnit(&locPlatform3->initDirVector[i]);
			SetVector(&locPlatform1->dirVector[i],&locPlatform1->initDirVector[i]);
			SetVector(&locPlatform2->dirVector[i],&locPlatform2->initDirVector[i]);
			SetVector(&locPlatform3->dirVector[i],&locPlatform3->initDirVector[i]);

			MakeUnit(&locPlatform4->initDirVector[i]);
			MakeUnit(&locPlatform5->initDirVector[i]);
			MakeUnit(&locPlatform6->initDirVector[i]);
			MakeUnit(&locPlatform7->initDirVector[i]);
			SetVector(&locPlatform4->dirVector[i],&locPlatform4->initDirVector[i]);
			SetVector(&locPlatform5->dirVector[i],&locPlatform5->initDirVector[i]);
			SetVector(&locPlatform6->dirVector[i],&locPlatform6->initDirVector[i]);
			SetVector(&locPlatform7->dirVector[i],&locPlatform7->initDirVector[i]);

			MakeUnit(&locPlatform8->initDirVector[i]);
			MakeUnit(&locPlatform9->initDirVector[i]);
			SetVector(&locPlatform8->dirVector[i],&locPlatform8->initDirVector[i]);
			SetVector(&locPlatform9->dirVector[i],&locPlatform9->initDirVector[i]);

			MakeUnit(&testPlatform1->initDirVector[i]);
	//		MakeUnit(&testPlatform2->initDirVector[i]);
	//		MakeUnit(&testPlatform3->initDirVector[i]);
	//		MakeUnit(&testPlatform4->initDirVector[i]);
	//		MakeUnit(&testPlatform5->initDirVector[i]);
			SetVector(&testPlatform1->dirVector[i],&testPlatform1->initDirVector[i]);
	//		SetVector(&testPlatform2->dirVector[i],&testPlatform2->initDirVector[i]);
	//		SetVector(&testPlatform3->dirVector[i],&testPlatform3->initDirVector[i]);
	//		SetVector(&testPlatform4->dirVector[i],&testPlatform4->initDirVector[i]);
	//		SetVector(&testPlatform5->dirVector[i],&testPlatform5->initDirVector[i]);

			RotateVectorByQuaternion(&locPlatform1->initDirVector[i],&locPlatform1->initDirVector[i],&locPlatform1->pltActor->actor->qRot);
			RotateVectorByQuaternion(&locPlatform2->initDirVector[i],&locPlatform2->initDirVector[i],&locPlatform2->pltActor->actor->qRot);
			RotateVectorByQuaternion(&locPlatform3->initDirVector[i],&locPlatform3->initDirVector[i],&locPlatform3->pltActor->actor->qRot);
			RotateVectorByQuaternion(&locPlatform4->initDirVector[i],&locPlatform4->initDirVector[i],&locPlatform4->pltActor->actor->qRot);
			RotateVectorByQuaternion(&locPlatform5->initDirVector[i],&locPlatform5->initDirVector[i],&locPlatform5->pltActor->actor->qRot);
			RotateVectorByQuaternion(&locPlatform6->initDirVector[i],&locPlatform6->initDirVector[i],&locPlatform6->pltActor->actor->qRot);
			RotateVectorByQuaternion(&locPlatform7->initDirVector[i],&locPlatform7->initDirVector[i],&locPlatform7->pltActor->actor->qRot);
			RotateVectorByQuaternion(&locPlatform8->initDirVector[i],&locPlatform8->initDirVector[i],&locPlatform8->pltActor->actor->qRot);
			RotateVectorByQuaternion(&locPlatform9->initDirVector[i],&locPlatform9->initDirVector[i],&locPlatform9->pltActor->actor->qRot);
		
			RotateVectorByQuaternion(&testPlatform1->initDirVector[i],&testPlatform1->initDirVector[i],&testPlatform1->pltActor->actor->qRot);
	//		RotateVectorByQuaternion(&testPlatform2->initDirVector[i],&testPlatform2->initDirVector[i],&testPlatform2->pltActor->actor->qRot);
	//		RotateVectorByQuaternion(&testPlatform3->initDirVector[i],&testPlatform3->initDirVector[i],&testPlatform3->pltActor->actor->qRot);
	//		RotateVectorByQuaternion(&testPlatform4->initDirVector[i],&testPlatform4->initDirVector[i],&testPlatform4->pltActor->actor->qRot);
	//		RotateVectorByQuaternion(&testPlatform5->initDirVector[i],&testPlatform5->initDirVector[i],&testPlatform5->pltActor->actor->qRot);
		}

		// create ptrs to surrounding local platforms

		for(i=0; i<4; i++)
		{
			locPlatform1->pltPtrs[i] = NULL;	//GetPlatformForTile ( locPlatform1->inTile->tilePtrs[i] );
			locPlatform2->pltPtrs[i] = NULL;	//GetPlatformForTile ( locPlatform2->inTile->tilePtrs[i] );
			locPlatform3->pltPtrs[i] = NULL;	//GetPlatformForTile ( locPlatform3->inTile->tilePtrs[i] );
			locPlatform4->pltPtrs[i] = NULL;	//GetPlatformForTile ( locPlatform5->inTile->tilePtrs[i] );
			locPlatform5->pltPtrs[i] = NULL;	//GetPlatformForTile ( locPlatform6->inTile->tilePtrs[i] );
			locPlatform6->pltPtrs[i] = NULL;	//GetPlatformForTile ( locPlatform7->inTile->tilePtrs[i] );
			locPlatform7->pltPtrs[i] = NULL;	//GetPlatformForTile ( locPlatform8->inTile->tilePtrs[i] );
			locPlatform8->pltPtrs[i] = NULL;	//GetPlatformForTile ( locPlatform9->inTile->tilePtrs[i] );
			locPlatform9->pltPtrs[i] = NULL;	//GetPlatformForTile ( locPlatform10->inTile->tilePtrs[i] );

			testPlatform1->pltPtrs[i] = NULL;
//			testPlatform2->pltPtrs[i] = NULL;
//			testPlatform3->pltPtrs[i] = NULL;
//			testPlatform4->pltPtrs[i] = NULL;
//			testPlatform5->pltPtrs[i] = NULL;
		}

			locPlatform1->pltPtrs[3] = locPlatform2;
			locPlatform2->pltPtrs[1] = locPlatform1;
			locPlatform2->pltPtrs[3] = locPlatform3;
			locPlatform3->pltPtrs[1] = locPlatform2;
			locPlatform4->pltPtrs[3] = locPlatform5;
			locPlatform5->pltPtrs[1] = locPlatform4;

			locPlatform6->pltPtrs[3] = locPlatform7;
			locPlatform7->pltPtrs[1] = locPlatform6;
		}
		else
		{
		}
		// ENDIF
	}
	else if(worldID == WORLDID_CITY)
	{
		if ( levelID == LEVELID_CITYDOCKS )
		{

			plank1 = CreateAndAddPlatform (	"crate.ndo", place1, 0, 158,158, 0, placeSpeed1, 0.0f,
			PLATFORM_BRIDGE);
			
			plank2 = CreateAndAddPlatform (	"crate.ndo", place2, 0, 158,158, 0, placeSpeed2, 0.0f,
			PLATFORM_BRIDGE);
			
			plank3 = CreateAndAddPlatform (	"crate.ndo", place3, 0, 158,158, 0, placeSpeed3, 0.0f,
			PLATFORM_BRIDGE);
			
			plank4 = CreateAndAddPlatform (	"crate.ndo", place4, 0, 208,208, 0, placeSpeed4, 0.0f,
			PLATFORM_BRIDGE);

				// local platforms 1,2,3,etc
				for(i=0; i<4; i++)
				{
					if ( ( i != 1 ) && ( i != 3 ) )
					{
					SubVector(&plank1->initDirVector[i],&plank1->inTile->tilePtrs[i]->centre,&plank1->inTile->centre);
					SubVector(&plank2->initDirVector[i],&plank2->inTile->tilePtrs[i]->centre,&plank2->inTile->centre);
					SubVector(&plank3->initDirVector[i],&plank3->inTile->tilePtrs[i]->centre,&plank3->inTile->centre);
					SubVector(&plank4->initDirVector[i],&plank4->inTile->tilePtrs[i]->centre,&plank4->inTile->centre);
					FlipVector(&plank1->initDirVector[i]);
					FlipVector(&plank2->initDirVector[i]);
					FlipVector(&plank3->initDirVector[i]);
					FlipVector(&plank4->initDirVector[i]);


					MakeUnit(&plank1->initDirVector[i]);
					MakeUnit(&plank2->initDirVector[i]);
					MakeUnit(&plank3->initDirVector[i]);
					MakeUnit(&plank4->initDirVector[i]);
					SetVector(&plank1->dirVector[i],&plank1->initDirVector[i]);
					SetVector(&plank2->dirVector[i],&plank2->initDirVector[i]);
					SetVector(&plank3->dirVector[i],&plank3->initDirVector[i]);
					SetVector(&plank4->dirVector[i],&plank4->initDirVector[i]);

				
					RotateVectorByQuaternion(&plank1->initDirVector[i],&plank1->initDirVector[i],&plank1->pltActor->actor->qRot);
					RotateVectorByQuaternion(&plank2->initDirVector[i],&plank2->initDirVector[i],&plank2->pltActor->actor->qRot);
					RotateVectorByQuaternion(&plank3->initDirVector[i],&plank3->initDirVector[i],&plank3->pltActor->actor->qRot);
					RotateVectorByQuaternion(&plank4->initDirVector[i],&plank4->initDirVector[i],&plank4->pltActor->actor->qRot);
					}
				}

				// create ptrs to surrounding local platforms

			for(i=0; i<4; i++)
			{
					plank1->pltPtrs[i] = NULL;
					plank2->pltPtrs[i] = NULL;
					plank3->pltPtrs[i] = NULL;
			}

		}
		else if ( levelID == LEVELID_CITYSTREETS )
		{

		}
		else if ( levelID == LEVELID_CITYWAREHOUSE )
		{
			testPlatform1 = CreateAndAddPlatform (	"crate.ndo", ppath1, ppathHight1, 50, 150, 1, ppathSpeed1, 2.0f,
			PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_FLATLEVEL | PLATFORM_PATHFORWARD_BACK );
			testPlatform2 = CreateAndAddPlatform (	"crate.ndo", ppath2, ppathHight2, 50, 150, 1, ppathSpeed2, 2.0f,
			PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_FLATLEVEL | PLATFORM_PATHFORWARD_BACK );

			testPlatform3 = CreateAndAddPlatform (	"crate.ndo", ppath3, 0, 50, 50, 0, 0, 2.0f,
			PLATFORM_SINKABLE );
			testPlatform4 = CreateAndAddPlatform (	"crate.ndo", ppath4, 0, 50, 50, 0, 0, 2.0f,
			PLATFORM_SINKABLE );
			testPlatform5 = CreateAndAddPlatform (	"crate.ndo", ppath5, 0, 50, 50, 0, 0, 2.0f,
			PLATFORM_SINKABLE );

			// Water fall create's
			testPlatform6 = CreateAndAddPlatform (	"crate.ndo", wwpath1, wwpathHight1, 50, 50, 1, wwpathSpeed1, 2.0f,
			PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_FLATLEVEL | PLATFORM_PATHEND2START | PLATFORM_NOORIENTATE );
			testPlatform7 = CreateAndAddPlatform (	"crate.ndo", wwpath1, wwpathHight1, 50, 50, 2, wwpathSpeed1, 2.0f,
			PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_FLATLEVEL | PLATFORM_PATHEND2START | PLATFORM_NOORIENTATE );
			testPlatform8 = CreateAndAddPlatform (	"crate.ndo", wwpath1, wwpathHight1, 50, 50, 5, wwpathSpeed1, 2.0f,
			PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_FLATLEVEL | PLATFORM_PATHEND2START | PLATFORM_NOORIENTATE );
			testPlatform9 = CreateAndAddPlatform (	"crate.ndo", wwpath1, wwpathHight1, 50, 50, 6, wwpathSpeed1, 2.0f,
			PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_FLATLEVEL | PLATFORM_PATHEND2START | PLATFORM_NOORIENTATE );

			testPlatform10 = CreateAndAddPlatform (	"crate.ndo", wwpath2, wwpathHight2, 50, 50, 3, wwpathSpeed2, 2.0f,
			PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_FLATLEVEL | PLATFORM_PATHEND2START | PLATFORM_NOORIENTATE );
			testPlatform11 = CreateAndAddPlatform (	"crate.ndo", wwpath2, wwpathHight2, 50, 50, 4, wwpathSpeed2, 2.0f,
			PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_FLATLEVEL | PLATFORM_PATHEND2START | PLATFORM_NOORIENTATE );
			testPlatform12 = CreateAndAddPlatform (	"crate.ndo", wwpath2, wwpathHight2, 50, 50, 5, wwpathSpeed2, 2.0f,
			PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_FLATLEVEL | PLATFORM_PATHEND2START | PLATFORM_NOORIENTATE );
			testPlatform13 = CreateAndAddPlatform (	"crate.ndo", wwpath2, wwpathHight2, 50, 50, 7, wwpathSpeed2, 2.0f,
			PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_FLATLEVEL | PLATFORM_PATHEND2START | PLATFORM_NOORIENTATE );

			testPlatform14 = CreateAndAddPlatform (	"crate.ndo", wwpath3, wwpathHight3, 50, 50, 1, wwpathSpeed3, 2.0f,
			PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_FLATLEVEL | PLATFORM_PATHEND2START | PLATFORM_NOORIENTATE );
			testPlatform15 = CreateAndAddPlatform (	"crate.ndo", wwpath3, wwpathHight3, 50, 50, 2, wwpathSpeed3, 2.0f,
			PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_FLATLEVEL | PLATFORM_PATHEND2START | PLATFORM_NOORIENTATE );
			testPlatform16 = CreateAndAddPlatform (	"crate.ndo", wwpath3, wwpathHight3, 50, 50, 3, wwpathSpeed3, 2.0f,
			PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_FLATLEVEL | PLATFORM_PATHEND2START | PLATFORM_NOORIENTATE );
			testPlatform17 = CreateAndAddPlatform (	"crate.ndo", wwpath3, wwpathHight3, 50, 50, 6, wwpathSpeed3, 2.0f,
			PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_FLATLEVEL | PLATFORM_PATHEND2START | PLATFORM_NOORIENTATE );
			testPlatform18 = CreateAndAddPlatform (	"crate.ndo", wwpath3, wwpathHight3, 50, 50, 7, wwpathSpeed3, 2.0f,
			PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_FLATLEVEL | PLATFORM_PATHEND2START | PLATFORM_NOORIENTATE );
			 
			for ( i = 0; i < 4; i++ )
			{
				SubVector(&testPlatform1->initDirVector[i],&testPlatform1->inTile->tilePtrs[i]->centre,&testPlatform1->inTile->centre);
				SubVector(&testPlatform2->initDirVector[i],&testPlatform2->inTile->tilePtrs[i]->centre,&testPlatform2->inTile->centre);
				SubVector(&testPlatform3->initDirVector[i],&testPlatform3->inTile->tilePtrs[i]->centre,&testPlatform3->inTile->centre);
				SubVector(&testPlatform4->initDirVector[i],&testPlatform4->inTile->tilePtrs[i]->centre,&testPlatform4->inTile->centre);
				SubVector(&testPlatform5->initDirVector[i],&testPlatform5->inTile->tilePtrs[i]->centre,&testPlatform5->inTile->centre);
				if ( testPlatform6->inTile->tilePtrs[i] )
					SubVector(&testPlatform6->initDirVector[i],&testPlatform6->inTile->tilePtrs[i]->centre,&testPlatform6->inTile->centre);
				if ( testPlatform7->inTile->tilePtrs[i] )
					SubVector(&testPlatform7->initDirVector[i],&testPlatform7->inTile->tilePtrs[i]->centre,&testPlatform7->inTile->centre);
				if ( testPlatform8->inTile->tilePtrs[i] )
					SubVector(&testPlatform8->initDirVector[i],&testPlatform8->inTile->tilePtrs[i]->centre,&testPlatform8->inTile->centre);
				if ( testPlatform9->inTile->tilePtrs[i] )
					SubVector(&testPlatform9->initDirVector[i],&testPlatform9->inTile->tilePtrs[i]->centre,&testPlatform9->inTile->centre);
				if ( testPlatform10->inTile->tilePtrs[i] )
					SubVector(&testPlatform10->initDirVector[i],&testPlatform10->inTile->tilePtrs[i]->centre,&testPlatform10->inTile->centre);
				if ( testPlatform11->inTile->tilePtrs[i] )
					SubVector(&testPlatform11->initDirVector[i],&testPlatform11->inTile->tilePtrs[i]->centre,&testPlatform11->inTile->centre);
				if ( testPlatform12->inTile->tilePtrs[i] )
					SubVector(&testPlatform12->initDirVector[i],&testPlatform12->inTile->tilePtrs[i]->centre,&testPlatform12->inTile->centre);
				if ( testPlatform13->inTile->tilePtrs[i] )
					SubVector(&testPlatform13->initDirVector[i],&testPlatform13->inTile->tilePtrs[i]->centre,&testPlatform13->inTile->centre);
				if ( testPlatform14->inTile->tilePtrs[i] )
					SubVector(&testPlatform14->initDirVector[i],&testPlatform14->inTile->tilePtrs[i]->centre,&testPlatform14->inTile->centre);
				if ( testPlatform15->inTile->tilePtrs[i] )
					SubVector(&testPlatform15->initDirVector[i],&testPlatform15->inTile->tilePtrs[i]->centre,&testPlatform15->inTile->centre);
				if ( testPlatform16->inTile->tilePtrs[i] )
					SubVector(&testPlatform16->initDirVector[i],&testPlatform16->inTile->tilePtrs[i]->centre,&testPlatform16->inTile->centre);
				if ( testPlatform17->inTile->tilePtrs[i] )
					SubVector(&testPlatform17->initDirVector[i],&testPlatform17->inTile->tilePtrs[i]->centre,&testPlatform17->inTile->centre);
				if ( testPlatform18->inTile->tilePtrs[i] )
					SubVector(&testPlatform18->initDirVector[i],&testPlatform18->inTile->tilePtrs[i]->centre,&testPlatform18->inTile->centre);

				FlipVector(&testPlatform1->initDirVector[i]);
				FlipVector(&testPlatform2->initDirVector[i]);
				FlipVector(&testPlatform3->initDirVector[i]);
				FlipVector(&testPlatform4->initDirVector[i]);
				FlipVector(&testPlatform5->initDirVector[i]);
				FlipVector(&testPlatform6->initDirVector[i]);
				FlipVector(&testPlatform7->initDirVector[i]);
				FlipVector(&testPlatform8->initDirVector[i]);

				MakeUnit(&testPlatform1->initDirVector[i]);
				MakeUnit(&testPlatform2->initDirVector[i]);
				MakeUnit(&testPlatform3->initDirVector[i]);
				MakeUnit(&testPlatform4->initDirVector[i]);
				MakeUnit(&testPlatform5->initDirVector[i]);
				MakeUnit(&testPlatform6->initDirVector[i]);
				MakeUnit(&testPlatform7->initDirVector[i]);
				MakeUnit(&testPlatform8->initDirVector[i]);

				RotateVectorByQuaternion(&testPlatform1->initDirVector[i],&testPlatform1->initDirVector[i],&testPlatform1->pltActor->actor->qRot);
				RotateVectorByQuaternion(&testPlatform2->initDirVector[i],&testPlatform2->initDirVector[i],&testPlatform2->pltActor->actor->qRot);
				RotateVectorByQuaternion(&testPlatform3->initDirVector[i],&testPlatform3->initDirVector[i],&testPlatform3->pltActor->actor->qRot);
				RotateVectorByQuaternion(&testPlatform4->initDirVector[i],&testPlatform4->initDirVector[i],&testPlatform4->pltActor->actor->qRot);
				RotateVectorByQuaternion(&testPlatform5->initDirVector[i],&testPlatform5->initDirVector[i],&testPlatform5->pltActor->actor->qRot);
				RotateVectorByQuaternion(&testPlatform6->initDirVector[i],&testPlatform6->initDirVector[i],&testPlatform6->pltActor->actor->qRot);
				RotateVectorByQuaternion(&testPlatform7->initDirVector[i],&testPlatform7->initDirVector[i],&testPlatform7->pltActor->actor->qRot);
				RotateVectorByQuaternion(&testPlatform8->initDirVector[i],&testPlatform8->initDirVector[i],&testPlatform8->pltActor->actor->qRot);
			}
			// ENDFOR
		}
		else if ( levelID == LEVELID_CITYBONUS )
		{
			bus1 = CreateAndAddPlatform (	"crate.ndo", busPath1, busPathHight1, 50, 150, 1, busPathSpeed1, 2.0f,
			PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_FLATLEVEL | PLATFORM_PATHBOUNCE );
			bus2 = CreateAndAddPlatform (	"crate.ndo", busPath1, busPathHight1, 50, 150, 2, busPathSpeed1, 2.0f,
			PLATFORM_HASPATH | PLATFORM_PATHFORWARDS | PLATFORM_FLATLEVEL | PLATFORM_PATHBOUNCE );

			 
			for ( i = 0; i < 4; i++ )
			{
				SubVector(&bus1->initDirVector[i],&bus1->inTile->tilePtrs[i]->centre,&bus1->inTile->centre);
				SubVector(&bus2->initDirVector[i],&bus2->inTile->tilePtrs[i]->centre,&bus2->inTile->centre);

				FlipVector(&bus1->initDirVector[i]);
				FlipVector(&bus2->initDirVector[i]);

				MakeUnit(&bus1->initDirVector[i]);
				MakeUnit(&bus2->initDirVector[i]);

				RotateVectorByQuaternion(&bus1->initDirVector[i],&bus1->initDirVector[i],&bus1->pltActor->actor->qRot);
				RotateVectorByQuaternion(&bus2->initDirVector[i],&bus2->initDirVector[i],&bus2->pltActor->actor->qRot);
			}
			// ENDFOR
			   
		}
		// ENDIF
	}
   // ENDIF
}


PLATFORM *CreateAndAddPlatform(char *pActorName,unsigned long *pathIndex, unsigned long *hightIndex, float offset,float offset2,int startNode,float *moveSpeed, float riseSpeed, unsigned long pFlags)
{
/*
	VECTOR toPosition;
	VECTOR vfd	= { 0,0,1 };
	VECTOR vup	= { 0,1,0 };
	VECTOR vTo,vFrom,v1;
	int i;

	PLATFORM *newItem = (PLATFORM *)JallocAlloc(sizeof(PLATFORM),YES,"plat");
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

	if(pFlags & PLATFORM_BRIDGE)
	{
		pFlags |= (PLATFORM_SINKABLE | PLATFORM_CANWALKUNDER);
		newItem->pltActor->flags |= ACTOR_DRAW_NEVER;
		offset2 = offset;
	}

	// set the platform active state
	if(pFlags & PLATFORM_INACTIVE)
	{
		// platform is inactive - still drawn but not updated
		newItem->active = 0;
	}
	else
	{
		// platform is active
		newItem->active = 1;
	}

	newItem->flags			= pFlags;
	newItem->originalFlags	= pFlags;
	newItem->moveSpeed		= moveSpeed;
	newItem->riseSpeed		= riseSpeed;
	
	newItem->path			= NULL;

	newItem->inTile			= &firstTile[pathIndex[startNode + 1]];

	// create the platform's path
	newItem->path				= CreatePlatformPathFromTileList(pathIndex,hightIndex,offset,offset2);
	newItem->path->startNode	= startNode;

	if(pFlags & PLATFORM_PATHFORWARDS)
	{
		newItem->path->fromNode = startNode;
		newItem->path->toNode	= newItem->path->fromNode + 1;
	}
	
	if(pFlags & PLATFORM_PATHBACKWARDS)
	{
		newItem->path->fromNode = startNode;
		newItem->path->toNode	= newItem->path->fromNode - 1;
	}

	// set platform (actor) position and orientation
	SetVector(&newItem->pltActor->actor->pos,&newItem->path->nodes[startNode].worldTile->centre);

	newItem->pltActor->actor->pos.v[X] += (newItem->path->nodes[startNode].worldTile->normal.v[X] * newItem->path->nodes[startNode].offset);
	newItem->pltActor->actor->pos.v[Y] += (newItem->path->nodes[startNode].worldTile->normal.v[Y] * newItem->path->nodes[startNode].offset);
	newItem->pltActor->actor->pos.v[Z] += (newItem->path->nodes[startNode].worldTile->normal.v[Z] * newItem->path->nodes[startNode].offset);

	NormalToQuaternion(&newItem->pltActor->actor->qRot,&newItem->path->nodes[startNode].worldTile->normal);

	GetPositionForPathNode(&toPosition,&newItem->path->nodes[newItem->path->toNode]);

	if ( pFlags & PLATFORM_PATHFORWARD_BACK )
	{
		SubVector(&v1,&toPosition,&newItem->pltActor->actor->pos);
		MakeUnit(&v1);
		Orientate(&newItem->pltActor->actor->qRot,&v1,&vfd,&vup);
	}
	else
	{
	}

	return newItem;
*/
}


/*	--------------------------------------------------------------------------------
	Function		: CreatePlatformPathFromTileList
	Purpose			: creates and initialise a path for a platform
	Parameters		: unsigned long *,float,float
	Returns			: PLATFORM_PATH *
	Info			: 
*/
PATH *CreatePlatformPathFromTileList(unsigned long *pIndex, unsigned long *hIndex, float offset,float offset2)
{
/*
	int i;
	PATH *newPath = (PATH *)JallocAlloc(sizeof(PATH),YES,"ppath");

	newPath->numNodes = pIndex[0];

	// allocate memory for path nodes
	newPath->nodes = (PATHNODE *)JallocAlloc(sizeof(PATHNODE) * newPath->numNodes,YES,"pnode");

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
			//newPath->nodes[i - 1].offset	= offset;
			newPath->nodes[i - 1].offset	= hIndex[i];
			newPath->nodes[i - 1].offset2	= offset2;
		}
	}

	return newPath;
*/
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
/*
	GAMETILE *nextTile = NULL,*fromTile,*toTile;
	QUATERNION q1,destQ;
	VECTOR fwd;
	VECTOR vup = { 0,1,0 };
	VECTOR vfd = { 0,0,1 };
	PLATFORM *cur,*next;
	float lowest,t;
	int lowval;
	int i,j;
	int newCamFacing,newFrogFacing;

	float m,n,qrspd = 0.0F;
	VECTOR toPosition,fromPosition,atPosition;

	VECTOR v1,v2,v3;
	VECTOR pltup;

	VECTOR pltUpVec,pltKnownVec,pltRightVec;
	QUATERNION q;

	FX_RIPPLE *rip;

	if(platformList.numEntries == 0)
		return;

	for(cur = platformList.head.next; cur != &platformList.head; cur = next)
	{
		next = cur->next;

		// check if this platform is active
		if(!cur->active)
			continue;

		// update any platforms that move along a specified path
		if(cur->flags & PLATFORM_HASPATH)
		{
			GetPositionForPathNode(&toPosition,&cur->path->nodes[cur->path->toNode]);
			SubVector(&fwd,&toPosition,&cur->pltActor->actor->pos);
			MakeUnit(&fwd);

			cur->pltActor->actor->pos.v[X] += (cur->moveSpeed[cur->path->toNode] * fwd.v[X]);
			cur->pltActor->actor->pos.v[Y] += (cur->moveSpeed[cur->path->toNode] * fwd.v[Y]);
			cur->pltActor->actor->pos.v[Z] += (cur->moveSpeed[cur->path->toNode] * fwd.v[Z]);

			// update ripples (if in water)
			if(cur->flags & PLATFORM_INWATER)
			{
				if((frameCount & 3) == 0)
				{
					rip = CreateAndAddFXRipple(RIPPLE_TYPE_WATER,&cur->pltActor->actor->pos,&upVec,10,0.1,0.1,25);
					rip->r = 255;
					rip->g = 255;
					rip->b = 255;
				}
			}

			if(cur->flags & PLATFORM_FLATLEVEL)
			{
				// orientate the actor to face the direction of movement....
				SubVector(&v1,&toPosition,&cur->pltActor->actor->pos);
				MakeUnit(&v1);
				Orientate(&cur->pltActor->actor->qRot,&v1,&vfd,&vup);
			}
			else
			{
				// platform moving over non-flat level
				GetPositionForPathNode(&fromPosition,&cur->path->nodes[cur->path->fromNode]);
				m = DistanceBetweenPoints(&toPosition,&fromPosition);
				n = DistanceBetweenPoints(&toPosition,&cur->pltActor->actor->pos);
				qrspd = (1.0F - (n / m)) * 0.5F;

				NormalToQuaternion(&q1,&cur->path->nodes[cur->path->toNode].worldTile->normal);
				QuatSlerp(&cur->pltActor->actor->qRot,&q1,qrspd,&destQ);
				SetQuaternion(&cur->pltActor->actor->qRot,&destQ);

				// orientate the actor to face the direction of movement....
				SubVector(&v1,&cur->pltActor->actor->pos,&toPosition);
				MakeUnit(&v1);

				// Calculate up vector
				RotateVectorByQuaternion(&pltup,&vup,&cur->pltActor->actor->qRot);
				CrossProduct(&v2,&v1,&pltup);
				CrossProduct(&v3,&v2,&pltup);
				Orientate(&cur->pltActor->actor->qRot,&v3,&vfd,&pltup);
			}

			// check if a node has been reached with the current platform
			if(PlatformHasArrivedAtNode(cur,cur->path->toNode))
			{
				if(cur->flags & PLATFORM_PATHBOUNCE)
				{
					// platform moving with 'ping-pong' motion
					if(cur->flags & PLATFORM_PATHFORWARDS)
					{
						cur->path->fromNode++;
						cur->path->toNode++;

						if(cur->path->toNode > (cur->path->numNodes - 1))
						{
							cur->path->fromNode = (cur->path->numNodes - 1);
							cur->path->toNode	= cur->path->fromNode - 1;

							cur->flags &= ~PLATFORM_PATHFORWARDS;
							cur->flags |= PLATFORM_PATHBACKWARDS;
						}
					}
					else if(cur->flags & PLATFORM_PATHBACKWARDS)
					{
						cur->path->fromNode--;
						cur->path->toNode--;

						if(cur->path->toNode < 0)
						{
							cur->path->fromNode = 0;
							cur->path->toNode	= cur->path->fromNode + 1;

							cur->flags &= ~PLATFORM_PATHBACKWARDS;
							cur->flags |= PLATFORM_PATHFORWARDS;
						}
					}
				}
				else if(cur->flags & PLATFORM_PATHCYCLE)
				{
					// platform moves with cyclic motion
					if(cur->flags & PLATFORM_PATHFORWARDS)
					{
						cur->path->fromNode++;
						cur->path->toNode++;

						if(cur->path->toNode > (cur->path->numNodes - 1))
							cur->path->toNode = 0;
						if(cur->path->fromNode > (cur->path->numNodes - 1))
							cur->path->fromNode = 0;
					}
					else if(cur->flags & PLATFORM_PATHBACKWARDS)
					{
						cur->path->fromNode--;
						cur->path->toNode--;

						if(cur->path->toNode < 0)
							cur->path->toNode = (cur->path->numNodes - 1);
						if(cur->path->fromNode < 0)
							cur->path->fromNode = (cur->path->numNodes - 1);
					}
				}
				else if(cur->flags & PLATFORM_PATHEND2START)
				{
					// enemy returns immediately to start of path when path end reached
					if(cur->flags & PLATFORM_PATHFORWARDS)
					{
						cur->path->fromNode++;
						cur->path->toNode++;

						if(cur->path->fromNode > (cur->path->numNodes - 2))
						{
							cur->path->fromNode = 0;
							cur->path->toNode = 1;
							SetVector(&cur->pltActor->actor->pos,&cur->path->nodes[cur->path->fromNode].worldTile->centre);
							NormalToQuaternion(&cur->pltActor->actor->qRot,&cur->path->nodes[cur->path->fromNode].worldTile->normal);
						}
					}
					else if(cur->flags & PLATFORM_PATHBACKWARDS)
					{
						cur->path->fromNode--;
						cur->path->toNode--;

						if(cur->path->fromNode < 1)
						{
							cur->path->fromNode = (cur->path->numNodes - 1);
							cur->path->toNode = (cur->path->numNodes - 2);
							SetVector(&cur->pltActor->actor->pos,&cur->path->nodes[cur->path->fromNode].worldTile->centre);
							NormalToQuaternion(&cur->pltActor->actor->qRot,&cur->path->nodes[cur->path->fromNode].worldTile->normal);
						}
					}
				}
				else if(cur->flags & PLATFORM_PATHFORWARD_BACK)
				{
					if(cur->flags & PLATFORM_PATHFORWARDS)
					{
						cur->path->fromNode++;
						cur->path->toNode++;

						if(cur->path->toNode > (cur->path->numNodes - 1))
						{
							cur->path->fromNode = (cur->path->numNodes - 1);
							cur->path->toNode	= cur->path->fromNode - 1;

							cur->flags &= ~PLATFORM_PATHFORWARDS;
							cur->flags |= PLATFORM_PATHBACKWARDS;
						}
					}
					else if(cur->flags & PLATFORM_PATHBACKWARDS)
					{
						cur->path->fromNode--;
						cur->path->toNode--;

						if(cur->path->toNode < 0)
						{
							cur->path->fromNode = 0;
							cur->path->toNode	= cur->path->fromNode + 1;

							cur->flags &= ~PLATFORM_PATHBACKWARDS;
							cur->flags |= PLATFORM_PATHFORWARDS;
						}
					}
				}
			}
		}

		// update platforms that have a single path node and have limited up/down movement
		else if(cur->flags & PLATFORM_UPDOWNONLY)
		{
			SetVector(&fwd,&cur->path->nodes[0].worldTile->normal);

			if(cur->flags & PLATFORM_MOVEUP)
			{
				cur->pltActor->actor->pos.v[X] += (cur->riseSpeed * fwd.v[X]);
				cur->pltActor->actor->pos.v[Y] += (cur->riseSpeed * fwd.v[Y]);
				cur->pltActor->actor->pos.v[Z] += (cur->riseSpeed * fwd.v[Z]);

				GetPositionForPathNodeOffset2(&toPosition,&cur->path->nodes[0]);

				if(DistanceBetweenPointsSquared(&cur->pltActor->actor->pos,&toPosition) < (cur->riseSpeed * cur->riseSpeed))
				{
					cur->flags &= ~PLATFORM_MOVEUP;
					cur->flags |= PLATFORM_MOVEDOWN;
				}
			}
			else if(cur->flags & PLATFORM_MOVEDOWN)
			{
				cur->pltActor->actor->pos.v[X] -= (cur->riseSpeed * fwd.v[X]);
				cur->pltActor->actor->pos.v[Y] -= (cur->riseSpeed * fwd.v[Y]);
				cur->pltActor->actor->pos.v[Z] -= (cur->riseSpeed * fwd.v[Z]);

				GetPositionForPathNode(&toPosition,&cur->path->nodes[0]);

				if(DistanceBetweenPointsSquared(&cur->pltActor->actor->pos,&toPosition) < (cur->riseSpeed * cur->riseSpeed))
				{
					cur->flags &= ~PLATFORM_MOVEDOWN;
					cur->flags |= PLATFORM_MOVEUP;
				}
			}
		}

		// update platforms that sink when frog is on it
		else if(cur->flags & PLATFORM_SINKABLE)
		{
			if(cur->flags & PLATFORM_CARRYINGFROG)
			{
				// frog is on sinking platform
				GetPositionForPathNode(&fromPosition,&cur->path->nodes[0]);
				GetPositionForPathNodeOffset2(&toPosition,&cur->path->nodes[0]);

				if(DistanceBetweenPointsSquared(&cur->pltActor->actor->pos,&fromPosition) < DistanceBetweenPointsSquared(&fromPosition,&toPosition))
				{
					// platform sinking
					cur->pltActor->actor->pos.v[X] -= (cur->riseSpeed * cur->inTile->normal.v[X]);
					cur->pltActor->actor->pos.v[Y] -= (cur->riseSpeed * cur->inTile->normal.v[Y]);
					cur->pltActor->actor->pos.v[Z] -= (cur->riseSpeed * cur->inTile->normal.v[Z]);
				}
				else
				{
					// check if over a deadly tile - if so then frog has drowned, sunk, etc.
					if(cur->inTile->state == TILESTATE_DEADLY && !frog[0]->action.dead)
					{
						player[0].frogState |= FROGSTATUS_ISDEAD;
						frog[0]->action.deathBy = DEATHBY_INSTANT;
						frog[0]->action.dead = 1;
					}
				}
			}
			else
			{
				// frog is NOT on sinking platform
				GetPositionForPathNode(&fromPosition,&cur->path->nodes[0]);
				if(DistanceBetweenPointsSquared(&cur->pltActor->actor->pos,&fromPosition) > 25.0F)
					AddToVector(&cur->pltActor->actor->pos,&cur->inTile->normal);
			}
		}

		// update platforms that have no path but rise when frog is on it
		else if(cur->flags & PLATFORM_RISABLE)
		{
			if(cur->flags & PLATFORM_CARRYINGFROG)
			{
				// frog is on rising platform
				GetPositionForPathNode(&fromPosition,&cur->path->nodes[0]);
				GetPositionForPathNodeOffset2(&toPosition,&cur->path->nodes[0]);

				if(DistanceBetweenPointsSquared(&cur->pltActor->actor->pos,&fromPosition) < DistanceBetweenPointsSquared(&fromPosition,&toPosition))
				{
					// platform raising
					cur->pltActor->actor->pos.v[X] += (cur->riseSpeed * cur->inTile->normal.v[X]);
					cur->pltActor->actor->pos.v[Y] += (cur->riseSpeed * cur->inTile->normal.v[Y]);
					cur->pltActor->actor->pos.v[Z] += (cur->riseSpeed * cur->inTile->normal.v[Z]);
				}
				else
				{
					// check if over a deadly tile - if so then frog has risen into seomthing deadly
					if(cur->inTile->state == TILESTATE_DEADLY && !frog[0]->action.dead)
					{
						player[0].frogState |= FROGSTATUS_ISDEAD;
						frog[0]->action.deathBy = DEATHBY_INSTANT;
						frog[0]->action.dead = 1;
					}
				}
			}
			else
			{
				// frog is NOT on rising platform
				GetPositionForPathNode(&fromPosition,&cur->path->nodes[0]);
				if(DistanceBetweenPointsSquared(&cur->pltActor->actor->pos,&fromPosition) > 25.0F)
					SubFromVector(&cur->pltActor->actor->pos,&cur->inTile->normal);
			}
		}

		// update platforms that fade out and disappear when frog is on it
//		else if(cur->flags & PLATFORM_DISAPPEARING)
		if(cur->flags & PLATFORM_DISAPPEARING)
		{
			if(cur->flags & PLATFORM_CARRYINGFROG)
			{
				// frog is on disappearing platform
				if(cur->pltActor->actor->xluOverride > 0)
				{
					cur->pltActor->actor->xluOverride -= (short)cur->moveSpeed[cur->path->toNode];
					if(cur->pltActor->actor->xluOverride < 0)
						cur->pltActor->actor->xluOverride = 0;
				}
			}
			else
			{
				// frog is NOT on disappearing platform
				if(cur->pltActor->actor->xluOverride < 99)
				{
					cur->pltActor->actor->xluOverride += (short)cur->moveSpeed[cur->path->toNode];
					if(cur->pltActor->actor->xluOverride > 99)
						cur->pltActor->actor->xluOverride = 99;
				}
			}
		}


		// check which world tile platform is currently 'in'
		oldTile = currTile[0];
		GetActiveTile(cur);

		if(cur->flags & PLATFORM_CARRYINGFROG)
		{
			currTile[0] = cur->inTile;

			SetVector(&frog[0]->actor->pos,&cur->pltActor->actor->pos);

			if ( !( cur->flags & PLATFORM_NOORIENTATE ) )
			{
				if(currTile[0] != oldTile)
				{
					// determine new camFacing
					lowest			= -999.0F;
					newCamFacing	= 0;
					for(i=0; i<4; i++)
					{
						t = DotProduct(&currTile[0]->dirVector[i],&oldTile->dirVector[camFacing]);
						if(t > lowest)
						{
							lowest			= t;
							newCamFacing	= i;
						}
					}
					camFacing = newCamFacing;

					// determine new frogFacing
					lowest			= -999.0F;
					newFrogFacing	= 0;
					for(i=0; i<4; i++)
					{
						t = DotProduct(&currTile[0]->dirVector[i],&oldTile->dirVector[frogFacing]);
						if(t > lowest)
						{
							lowest			= t;
							newFrogFacing	= i;
						}
					}
				}
			}
			// ENDIF
		}
	}
*/
	

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
			if(NEW_PlatformHasArrivedAtNode(cur))
				NEW_UpdatePlatformPathNodes(cur);
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
	
				if(NEW_PlatformReachedTopOrBottomPoint(cur))
					NEW_UpdatePlatformPathNodes(cur);
			}
		}

		// determine which world tile the platform is currently 'in'
		oldTile[0] = currTile[0];
		GetActiveTile(cur);

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
	Function		: PlatformHasArrivedAtNode
	Purpose			: checks if a platform (or segment) has arrived at a node
	Parameters		: PLATFORM *,int
	Returns			: BOOL - TRUE if node reached, else FALSE
	Info			: 
*/
BOOL PlatformHasArrivedAtNode(PLATFORM *pform,int nodeID)
{
	VECTOR nodePos;

	if(!(pform->flags & PLATFORM_HASPATH) && !(pform->flags & PLATFORM_PATH_MOVEUPDOWN))
		return FALSE;

	GetPositionForPathNode(&nodePos,&pform->path->nodes[nodeID]);
	if(DistanceBetweenPointsSquared(&pform->pltActor->actor->pos,&nodePos) <  ((pform->moveSpeed[pform->path->toNode] + 0.1) * (pform->moveSpeed[pform->path->toNode] + 0.1)))
		return TRUE;

	return FALSE;
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

PLATFORM *NEW_CreateAndAddPlatform(char *pActorName)
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

void NEW_AssignPathToPlatform(PLATFORM *pform,unsigned long platformFlags,PATH *path,unsigned long pathFlags)
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

	// set platform current 'in' tile
	pform->inTile = path->nodes[pform->path->fromNode].worldTile;

	dprintf"\n"));
}


/*	--------------------------------------------------------------------------------
	Function		: NEW_PlatformHasArrivedAtNode
	Purpose			: checks if a platform has arrived at a node
	Parameters		: PLATFORM *
	Returns			: BOOL - TRUE if node reached, else FALSE
	Info			: 
*/
BOOL NEW_PlatformHasArrivedAtNode(PLATFORM *pform)
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
	Function		: NEW_PlatformReachedTopOrBottomPoint
	Purpose			: checks if a platform has arrived at top or bottom point (1 node based)
	Parameters		: PLATFORM *
	Returns			: BOOL - TRUE if node reached, else FALSE
	Info			: 
*/
BOOL NEW_PlatformReachedTopOrBottomPoint(PLATFORM *pform)
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
	Function		: NEW_UpdatePlatformPathNodes
	Purpose			: updates platform path move status
	Parameters		: PLATFORM *
	Returns			: void
	Info			: 
*/
void NEW_UpdatePlatformPathNodes(PLATFORM *pform)
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
				return;
			}
			else if(flags & PLATFORM_NEW_CYCLE)
			{
				// platform has cyclic movement
				path->fromNode	= GET_PATHLASTNODE(path);
				path->toNode	= 0;
				return;
			}

			path->fromNode	= 0;
			path->toNode	= 1;

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
				return;
			}
			
			path->fromNode++;
			path->toNode++;
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
				return;
			}
			else if(flags & PLATFORM_NEW_CYCLE)
			{
				// platform has cyclic movement
				path->fromNode	= 0;
				path->toNode	= GET_PATHLASTNODE(path);
				return;
			}

			path->fromNode	= GET_PATHLASTNODE(path);
			path->toNode	= GET_PATHLASTNODE(path) - 1;

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
				return;
			}

			path->fromNode--;
			path->toNode--;
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
			return;
		}
	}
}


//------------------------------------------------------------------------------------------------
