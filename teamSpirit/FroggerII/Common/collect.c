/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: collect.c
	Programmer	: Andrew Eder
	Date		: 11/27/98

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>

#include "incs.h"


unsigned long autoHop			= 0;
unsigned long longHop			= 0;
unsigned long superHop			= 0;
unsigned long longTongue		= 0;
unsigned long superFrog			= 0;
unsigned long croakFloat		= 0;
unsigned long babiesSaved		= 0;
extern long carryOnBabies;

unsigned long gardenLawnSpawn[] = { 149,	34,35,58,60,62,64,81,84,87,90,
									93,121,122,123,124,125,126,127,154,155,
									156,157,158,159,160,161,162,163,164,165,
									181,182,183,184,185,186,187,188,189,190,
									191,192,219,220,221,222,223,224,225,226,
									227,228,229,230,231,256,257,258,259,
									260,261,262,263,264,265,266,295,400,401,
									402,413,424,435,436,437,448,459,471,472,
									483,494,539,543,545,549,364,365,366,367,
									368,369,370,371,372,373,374,375,376,377,
									378,379,380,381,382,383,384,385,386,387,
									415,416,417,428,441,440,439,426,421,422,
									423,434,447,446,445,432,467,468,469,482,
									493,492,491,480,461,462,463,476,487,486,
									485,474,453,454,455,466,479,478,477,464,
									
									};

unsigned long gardenLawnWholeKey[] = { 1, 532};

unsigned long gardenMazeSpawn[] = { 117,	1,2,3,9,10,11,12,13,15,19,
										20,22,23,24,25,26,27,28,29,30,
										31,32,33,34,35,37,39,41,42,43,
										44,51,52,53,54,55,56,57,63,64,
										66,67,68,69,70,76,77,78,79,81,
										86,88,89,90,91,92,98,103,104,105,
										106,153,154,155,163,164,171,172,173,0,
										165,156,146,130,278,279,280,281,282,283,
										286,288,306,307,308,309,310,300,298,299,
										305,304,303,302,301,287,170,162,151,143,
										135,127,119,169,161,150,141,134,125,118,
										168,159,149,140,133,124,117,
										};
									
unsigned long gardenMazeWholeKey[] = { 1, 15};

unsigned long gardenVegPatch[] = { 67,	3,8,12,16,18,20,25,28,31,32,
										34,40,42,45,48,49,51,54,59,63,
										66,67,69,74,76,82,83,84,86,88,
										91,94,105,113,116,126,130,132,146,150,
										160,164,168,186,188,190,192,193,194,195,
										196,197,198,204,207,209,211,213,220,222,
										229,231,234,235,236,237,238	};
									
									
unsigned long gardenVegWholeKey[] = { 1, 15};
									
									
									

unsigned long cityDocks[] = { 1,	25 };


//----- [ TEMPLATES FOR GARIB SPRITE ANIMATIONS ] -----//

SPRITE_ANIMATION_TEMPLATE garibAnimation[NUM_GARIB_TYPES] =
{
	{ &spriteFrameList[SPAWN_ANIM],0,SPRITE_ANIM_CYCLE_RANDOM,255,255,GARIB_SCALE,GARIB_SCALE,SPRITE_TRANSLUCENT },
	{ &spriteFrameList[EXTRAHEALTH_ANIM],0,SPRITE_ANIM_CYCLE,255,255,GARIB_SCALE,GARIB_SCALE,0 },
	{ &spriteFrameList[EXTRALIFE_ANIM],0,SPRITE_ANIM_CYCLE,255,255,GARIB_SCALE,GARIB_SCALE,0 },
	{ &spriteFrameList[AUTOHOP_ANIM],0,SPRITE_ANIM_CYCLE,255,255,GARIB_SCALE,GARIB_SCALE,0 },
	{ &spriteFrameList[LONGHOP_ANIM],0,SPRITE_ANIM_CYCLE,255,255,GARIB_SCALE,GARIB_SCALE,0 },
	{ &spriteFrameList[LONGTONGUE_ANIM],0,SPRITE_ANIM_CYCLE,255,255,GARIB_SCALE,GARIB_SCALE,0 },
	{ &spriteFrameList[WHOLEKEY_ANIM],0,SPRITE_ANIM_CYCLE,255,255,GARIB_SCALE,GARIB_SCALE,0 },
	{ &spriteFrameList[HALFLKEY_ANIM],0,SPRITE_ANIM_CYCLE,255,255,GARIB_SCALE,GARIB_SCALE,0 },
	{ &spriteFrameList[HALFRKEY_ANIM],0,SPRITE_ANIM_CYCLE,255,255,GARIB_SCALE,GARIB_SCALE,0 },
};


/*	--------------------------------------------------------------------------------
	Function		: InitCollectablesForLevel
	Purpose			: Initialises the collectables / bonuses for a level
	Parameters		: unsigned long,unsigned long
	Returns			: void
	Info			: 
*/
void InitCollectablesForLevel(unsigned long worldID,unsigned long levelID)
{
	unsigned long randomTile;
	unsigned long i;
	ACTOR2 *act;
	float x,y,z;

	switch(worldID)
	{
		case WORLDID_GARDEN:
			// get relevant collectables / bonuses for city levels
			switch(levelID)
			{
				case LEVELID_GARDENLAWN:
//					CreateLevelCollectables (gardenLawnSpawn, SPAWN_GARIB);
//					CreateLevelCollectables (gardenLawnWholeKey, WHOLEKEY_GARIB);
					break;

				case LEVELID_GARDENMAZE:
					CreateLevelCollectables (gardenMazeSpawn, SPAWN_GARIB);
					CreateLevelCollectables (gardenMazeWholeKey, WHOLEKEY_GARIB);
					break;
				case LEVELID_GARDENVEGPATCH:
					CreateLevelCollectables (gardenVegPatch, SPAWN_GARIB);
					CreateLevelCollectables (gardenVegWholeKey, WHOLEKEY_GARIB);
					break;
			}
			break;
		case WORLDID_CITY:
			// get relevant collectables / bonuses for city levels
			switch(levelID)
			{
				case LEVELID_CITYDOCKS:
					CreateLevelCollectables (cityDocks, SPAWN_GARIB);
					x = firstTile[308].centre.v[X];
					y = firstTile[308].centre.v[Y];
					z = firstTile[308].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[286].centre.v[X];
					y = firstTile[286].centre.v[Y];
					z = firstTile[286].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[264].centre.v[X];
					y = firstTile[264].centre.v[Y];
					z = firstTile[264].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[242].centre.v[X];
					y = firstTile[242].centre.v[Y];
					z = firstTile[242].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					//x = firstTile[25].centre.v[X];
					//y = firstTile[25].centre.v[Y];
					//z = firstTile[25].centre.v[Z];
					//act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					//act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[172].centre.v[X];
					y = firstTile[172].centre.v[Y];
					z = firstTile[172].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[238].centre.v[X];
					y = firstTile[238].centre.v[Y];
					z = firstTile[238].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[260].centre.v[X];
					y = firstTile[260].centre.v[Y];
					z = firstTile[260].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[289].centre.v[X];
					y = firstTile[289].centre.v[Y];
					z = firstTile[289].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] =1.0;
					x = firstTile[267].centre.v[X];
					y = firstTile[267].centre.v[Y];
					z = firstTile[267].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[292].centre.v[X];
					y = firstTile[292].centre.v[Y];
					z = firstTile[292].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					
					x = firstTile[270].centre.v[X];
					y = firstTile[270].centre.v[Y];
					z = firstTile[270].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[248].centre.v[X];
					y = firstTile[248].centre.v[Y];
					z = firstTile[248].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[226].centre.v[X];
					y = firstTile[226].centre.v[Y];
					z = firstTile[226].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[204].centre.v[X];
					y = firstTile[204].centre.v[Y];
					z = firstTile[204].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;

					x = firstTile[184].centre.v[X];
					y = firstTile[184].centre.v[Y];
					z = firstTile[184].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[162].centre.v[X];
					y = firstTile[162].centre.v[Y];
					z = firstTile[162].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;

					x = firstTile[366].centre.v[X];
					y = firstTile[366].centre.v[Y];
					z = firstTile[366].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[367].centre.v[X];
					y = firstTile[367].centre.v[Y];
					z = firstTile[367].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[368].centre.v[X];
					y = firstTile[368].centre.v[Y];
					z = firstTile[368].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;

					x = firstTile[345].centre.v[X];
					y = firstTile[345].centre.v[Y];
					z = firstTile[345].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[347].centre.v[X];
					y = firstTile[347].centre.v[Y];
					z = firstTile[347].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;

					x = firstTile[326].centre.v[X];
					y = firstTile[326].centre.v[Y];
					z = firstTile[326].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[327].centre.v[X];
					y = firstTile[327].centre.v[Y];
					z = firstTile[327].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[328].centre.v[X];
					y = firstTile[328].centre.v[Y];
					z = firstTile[328].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					
					x = firstTile[362].centre.v[X];
					y = firstTile[362].centre.v[Y];
					z = firstTile[362].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[363].centre.v[X];
					y = firstTile[363].centre.v[Y];
					z = firstTile[363].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[364].centre.v[X];
					y = firstTile[364].centre.v[Y];
					z = firstTile[364].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[344].centre.v[X];
					y = firstTile[344].centre.v[Y];
					z = firstTile[344].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;

					x = firstTile[359].centre.v[X];
					y = firstTile[359].centre.v[Y];
					z = firstTile[359].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[360].centre.v[X];
					y = firstTile[360].centre.v[Y];
					z = firstTile[360].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;

					x = firstTile[313].centre.v[X];
					y = firstTile[313].centre.v[Y];
					z = firstTile[313].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[314].centre.v[X];
					y = firstTile[314].centre.v[Y];
					z = firstTile[314].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;

					x = firstTile[316].centre.v[X];
					y = firstTile[316].centre.v[Y];
					z = firstTile[316].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[317].centre.v[X];
					y = firstTile[317].centre.v[Y];
					z = firstTile[317].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;

					x = firstTile[55].centre.v[X];
					y = firstTile[55].centre.v[Y];
					z = firstTile[55].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[57].centre.v[X];
					y = firstTile[57].centre.v[Y];
					z = firstTile[57].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[76].centre.v[X];
					y = firstTile[76].centre.v[Y];
					z = firstTile[76].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[78].centre.v[X];
					y = firstTile[78].centre.v[Y];
					z = firstTile[78].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;

					x = firstTile[278].centre.v[X];
					y = firstTile[278].centre.v[Y];
					z = firstTile[278].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[279].centre.v[X];
					y = firstTile[279].centre.v[Y];
					z = firstTile[279].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[280].centre.v[X];
					y = firstTile[280].centre.v[Y];
					z = firstTile[280].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[256].centre.v[X];
					y = firstTile[256].centre.v[Y];
					z = firstTile[256].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[258].centre.v[X];
					y = firstTile[258].centre.v[Y];
					z = firstTile[258].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[234].centre.v[X];
					y = firstTile[234].centre.v[Y];
					z = firstTile[234].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[235].centre.v[X];
					y = firstTile[235].centre.v[Y];
					z = firstTile[235].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[236].centre.v[X];
					y = firstTile[236].centre.v[Y];
					z = firstTile[236].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;

					x = firstTile[245].centre.v[X];
					y = firstTile[245].centre.v[Y];
					z = firstTile[245].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[246].centre.v[X];
					y = firstTile[246].centre.v[Y];
					z = firstTile[246].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[247].centre.v[X];
					y = firstTile[247].centre.v[Y];
					z = firstTile[247].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					break;

				case LEVELID_CITYWAREHOUSE:
					x = firstTile[65].centre.v[X];
					y = firstTile[65].centre.v[Y];
					z = firstTile[65].centre.v[Z];
					act = CreateAndAddActor("longhop.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;


					x = firstTile[0].centre.v[X];
					y = firstTile[0].centre.v[Y];
					z = firstTile[0].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[6].centre.v[X];
					y = firstTile[6].centre.v[Y];
					z = firstTile[6].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[10].centre.v[X];
					y = firstTile[10].centre.v[Y];
					z = firstTile[10].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[16].centre.v[X];
					y = firstTile[16].centre.v[Y];
					z = firstTile[16].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[18].centre.v[X];
					y = firstTile[18].centre.v[Y];
					z = firstTile[18].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[26].centre.v[X];
					y = firstTile[26].centre.v[Y];
					z = firstTile[26].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[27].centre.v[X];
					y = firstTile[27].centre.v[Y];
					z = firstTile[27].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[35].centre.v[X];
					y = firstTile[35].centre.v[Y];
					z = firstTile[35].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[36].centre.v[X];
					y = firstTile[36].centre.v[Y];
					z = firstTile[36].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[40].centre.v[X];
					y = firstTile[40].centre.v[Y];
					z = firstTile[40].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[41].centre.v[X];
					y = firstTile[41].centre.v[Y];
					z = firstTile[41].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[42].centre.v[X];
					y = firstTile[42].centre.v[Y];
					z = firstTile[42].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[55].centre.v[X];
					y = firstTile[55].centre.v[Y];
					z = firstTile[55].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[56].centre.v[X];
					y = firstTile[56].centre.v[Y];
					z = firstTile[56].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[57].centre.v[X];
					y = firstTile[57].centre.v[Y];
					z = firstTile[57].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[59].centre.v[X];
					y = firstTile[59].centre.v[Y];
					z = firstTile[59].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[60].centre.v[X];
					y = firstTile[60].centre.v[Y];
					z = firstTile[60].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[70].centre.v[X];
					y = firstTile[70].centre.v[Y];
					z = firstTile[70].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[71].centre.v[X];
					y = firstTile[71].centre.v[Y];
					z = firstTile[71].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[74].centre.v[X];
					y = firstTile[74].centre.v[Y];
					z = firstTile[74].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[138].centre.v[X];
					y = firstTile[138].centre.v[Y];
					z = firstTile[138].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[139].centre.v[X];
					y = firstTile[139].centre.v[Y];
					z = firstTile[139].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[140].centre.v[X];
					y = firstTile[140].centre.v[Y];
					z = firstTile[140].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[151].centre.v[X];
					y = firstTile[151].centre.v[Y];
					z = firstTile[151].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[152].centre.v[X];
					y = firstTile[152].centre.v[Y];
					z = firstTile[152].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[160].centre.v[X];
					y = firstTile[160].centre.v[Y];
					z = firstTile[160].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[161].centre.v[X];
					y = firstTile[161].centre.v[Y];
					z = firstTile[161].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[162].centre.v[X];
					y = firstTile[162].centre.v[Y];
					z = firstTile[162].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[163].centre.v[X];
					y = firstTile[163].centre.v[Y];
					z = firstTile[163].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[168].centre.v[X];
					y = firstTile[168].centre.v[Y];
					z = firstTile[168].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[169].centre.v[X];
					y = firstTile[169].centre.v[Y];
					z = firstTile[169].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[170].centre.v[X];
					y = firstTile[170].centre.v[Y];
					z = firstTile[170].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[175].centre.v[X];
					y = firstTile[175].centre.v[Y];
					z = firstTile[175].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[176].centre.v[X];
					y = firstTile[176].centre.v[Y];
					z = firstTile[176].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[177].centre.v[X];
					y = firstTile[177].centre.v[Y];
					z = firstTile[177].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[183].centre.v[X];
					y = firstTile[183].centre.v[Y];
					z = firstTile[183].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[184].centre.v[X];
					y = firstTile[184].centre.v[Y];
					z = firstTile[184].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[185].centre.v[X];
					y = firstTile[185].centre.v[Y];
					z = firstTile[185].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[186].centre.v[X];
					y = firstTile[186].centre.v[Y];
					z = firstTile[186].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[192].centre.v[X];
					y = firstTile[192].centre.v[Y];
					z = firstTile[192].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[193].centre.v[X];
					y = firstTile[193].centre.v[Y];
					z = firstTile[193].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[194].centre.v[X];
					y = firstTile[194].centre.v[Y];
					z = firstTile[194].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[198].centre.v[X];
					y = firstTile[198].centre.v[Y];
					z = firstTile[198].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[199].centre.v[X];
					y = firstTile[199].centre.v[Y];
					z = firstTile[199].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[200].centre.v[X];
					y = firstTile[200].centre.v[Y];
					z = firstTile[200].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[201].centre.v[X];
					y = firstTile[201].centre.v[Y];
					z = firstTile[201].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[210].centre.v[X];
					y = firstTile[210].centre.v[Y];
					z = firstTile[210].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[212].centre.v[X];
					y = firstTile[212].centre.v[Y];
					z = firstTile[212].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[216].centre.v[X];
					y = firstTile[216].centre.v[Y];
					z = firstTile[216].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[219].centre.v[X];
					y = firstTile[219].centre.v[Y];
					z = firstTile[219].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[228].centre.v[X];
					y = firstTile[228].centre.v[Y];
					z = firstTile[228].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[229].centre.v[X];
					y = firstTile[229].centre.v[Y];
					z = firstTile[229].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[230].centre.v[X];
					y = firstTile[230].centre.v[Y];
					z = firstTile[230].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[234].centre.v[X];
					y = firstTile[234].centre.v[Y];
					z = firstTile[234].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[237].centre.v[X];
					y = firstTile[237].centre.v[Y];
					z = firstTile[237].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[252].centre.v[X];
					y = firstTile[252].centre.v[Y];
					z = firstTile[252].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[253].centre.v[X];
					y = firstTile[253].centre.v[Y];
					z = firstTile[253].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[254].centre.v[X];
					y = firstTile[254].centre.v[Y];
					z = firstTile[254].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[255].centre.v[X];
					y = firstTile[255].centre.v[Y];
					z = firstTile[255].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					break;
				case LEVELID_CITYSTREETS:
					break;
				case LEVELID_CITYTOWER:
					break;
				case LEVELID_CITYBONUS:
					for ( i = 0; i < 100; i++ )
					{
						randomTile = Random ( 427 );
						x = firstTile[randomTile].centre.v[X];
						y = firstTile[randomTile].centre.v[Y]+(Random ( 20 ) * 50 );
						z = firstTile[randomTile].centre.v[Z];
						act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
						act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					}
					// ENDFOR
					/*
					x = firstTile[427].centre.v[X];
					y = firstTile[427].centre.v[Y]+2950;
					z = firstTile[427].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[427].centre.v[X];
					y = firstTile[427].centre.v[Y]+2850;
					z = firstTile[427].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[427].centre.v[X];
					y = firstTile[427].centre.v[Y]+2750;
					z = firstTile[427].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[427].centre.v[X];
					y = firstTile[427].centre.v[Y]+2650;
					z = firstTile[427].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[427].centre.v[X];
					y = firstTile[427].centre.v[Y]+2050;
					z = firstTile[427].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[427].centre.v[X];
					y = firstTile[427].centre.v[Y]+1850;
					z = firstTile[427].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[427].centre.v[X];
					y = firstTile[427].centre.v[Y]+1750;
					z = firstTile[427].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[427].centre.v[X];
					y = firstTile[427].centre.v[Y]+1450;
					z = firstTile[427].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[427].centre.v[X];
					y = firstTile[427].centre.v[Y]+1350;
					z = firstTile[427].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[427].centre.v[X];
					y = firstTile[427].centre.v[Y]+1150;
					z = firstTile[427].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[427].centre.v[X];
					y = firstTile[427].centre.v[Y]+850;
					z = firstTile[427].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[427].centre.v[X];
					y = firstTile[427].centre.v[Y]+750;
					z = firstTile[427].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[427].centre.v[X];
					y = firstTile[427].centre.v[Y]+700;
					z = firstTile[427].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[427].centre.v[X];
					y = firstTile[427].centre.v[Y]+600;
					z = firstTile[427].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[427].centre.v[X];
					y = firstTile[427].centre.v[Y]+500;
					z = firstTile[427].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[427].centre.v[X];
					y = firstTile[427].centre.v[Y]+400;
					z = firstTile[427].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[427].centre.v[X];
					y = firstTile[427].centre.v[Y]+300;
					z = firstTile[427].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[427].centre.v[X];
					y = firstTile[427].centre.v[Y]+200;
					z = firstTile[427].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[427].centre.v[X];
					y = firstTile[427].centre.v[Y]+100;
					z = firstTile[427].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;
					x = firstTile[427].centre.v[X];
					y = firstTile[427].centre.v[Y];
					z = firstTile[427].centre.v[Z];
					act = CreateAndAddActor("garibstarb.ndo",x,y,z,INIT_ANIMATION,0,0);
					act->actor->scale.v[X] = act->actor->scale.v[Y] = act->actor->scale.v[Z] = 1.0;*/
					break;
				case LEVELID_CITYMULTI:
					break;
			}
			break;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: CreateLevelCollectables
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void CreateLevelCollectables(unsigned long *tileList, int type)
{
	unsigned long i;
	GARIB *garib = NULL;
	VECTOR v;

	for(i=0; i<tileList[0]; i++)
	{
		v = firstTile[tileList[i+1]].centre;
		v.v[Y] += 20;
		garib = CreateNewGarib(v, type);
	}
}



/*	--------------------------------------------------------------------------------
	Function		: GrapplePointInTOngueRange
	Purpose			: indicates if a grapple point is in range when tongueing
	Parameters		: 
	Returns			: GAMETILE *
	Info			: returns ptr to the nearest gametile (if in range)
*/
GAMETILE *GrapplePointInTongueRange( )
{
	VECTOR diff;
	
	if( DistanceBetweenPointsSquared(&frog[0]->actor->pos, &gTStart[0]->centre) < tongueRadius * tongueRadius )
	{
		return gTStart[0];
	}
	
	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function		: GaribIsInTongueRange
	Purpose			: indicates if a garib is in range when tongueing
	Parameters		: 
	Returns			: GARIB *
	Info			: returns ptr to the nearest garib (if in range)
*/
GARIB *GaribIsInTongueRange()
{
	GARIB *garib,*nearest;
	GARIB *inRange[8];
	VECTOR diff;
	float dist,mags[8];
	int i = 0,numInRange = 0;
		
	for(garib = garibCollectableList.head.next; garib != &garibCollectableList.head; garib = garib->next)
	{
		// only check for garibs in visual range
		if(garib->distanceFromFrog > (tongueRadius * tongueRadius))
			continue;

		if(/*(garib->distanceFromFrog < (tongueRadius * tongueRadius)) &&*/ (garib->active) && (numInRange < 8))
		{
			mags[numInRange]		= garib->distanceFromFrog;	//dist;
			inRange[numInRange++]	= garib;
		}
	}

	if(numInRange)
	{
		// return closest item
		dist	= mags[0];
		nearest	= inRange[0];
		for(i=1; i<numInRange; i++)
		{
			if(mags[i] < dist)
			{
				dist	= mags[i];
				nearest	= inRange[i];
			}
		}

		return nearest;
	}

	return NULL;
}

/*	--------------------------------------------------------------------------------
	Function		: BabyFrogIsInTongueRange
	Purpose			: indicates if a baby frog is in range when tongueing
	Parameters		: 
	Returns			: ACTOR2 *
	Info			: returns ptr to the nearest baby frog (if in range)
*/
ACTOR2 *BabyFrogIsInTongueRange()
{
	ACTOR2 *act,*nearest;
	ACTOR2 *inRange[4];
	VECTOR diff;
	float dist,mags[4];
	int i = 0,numInRange = 0;

	if(numBabies)
	{
		for(i=0; i<numBabies; i++)
		{
			if((!babies[i]->action.isSaved) && (babies[i]->distanceFromFrog < (tongueRadius * tongueRadius)))
			{
				mags[numInRange]		= babies[i]->distanceFromFrog;
				inRange[numInRange++]	= babies[i];
			}
		}

		if(numInRange)
		{
			// return closest baby frog
			dist	= mags[0];
			nearest	= inRange[0];
			for(i=1; i<numInRange; i++)
			{
				if(mags[i] < dist)
				{
					dist	= mags[i];
					nearest	= inRange[i];
				}
			}

			return nearest;
		}
	}

	// no baby frog in range
	return NULL;

/*		
	for(garib = garibCollectableList.head.next; garib != &garibCollectableList.head; garib = garib->next)
	{
		// only check for garibs in visual range
		if(garib->distanceFromFrog > ACTOR_DRAWDISTANCEINNER)
			continue;

		if((garib->distanceFromFrog < (tongueRadius * tongueRadius)) && (garib->active) && (numInRange < 8))
		{
			mags[numInRange]		= dist;
			inRange[numInRange++]	= garib;
		}
	}

	if(numInRange)
	{
		// return closest item
		dist	= mags[0];
		nearest	= inRange[0];
		for(i=1; i<numInRange; i++)
		{
			if(mags[i] < dist)
			{
				dist	= mags[i];
				nearest	= inRange[i];
			}
		}

		return nearest;
	}

	return NULL;
*/
}


/*	--------------------------------------------------------------------------------
	Function		: CheckTileForCollectable
	Purpose			: checks frogs position for a collectable item
	Parameters		: 
	Returns			: void
	Info			: used to check if Frogger has jumped on a collectable item
*/
void CheckTileForCollectable(GAMETILE *tile)
{
	GARIB *garib;
	int i;
	
	// check current tile for a garib
	for(garib = garibCollectableList.head.next; garib != &garibCollectableList.head; garib = garib->next)
	{
		// process only garibs in visual range
		if(garib->distanceFromFrog > ACTOR_DRAWDISTANCEINNER)
			continue;

		if(DistanceBetweenPointsSquared(&garib->sprite.pos,&frog[0]->actor->pos) < (PICKUP_RADIUS * PICKUP_RADIUS))
		{
			PickupCollectable(garib);
			return;
		}
	}

	// check current tile for a baby frog
	i = numBabies;
	while(i--)
	{
		if(DistanceBetweenPointsSquared(&babies[i]->actor->pos,&frog[0]->actor->pos) < (PICKUP_RADIUS * PICKUP_RADIUS))
		{
			PickupBabyFrog(babies[i]);
			return;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function		: ProcessCollectables
	Purpose			: processes collectables scattered across the many varied worldscapes...
	Parameters		: none
	Returns			: void
	Info			:
*/
void ProcessCollectables()
{
	UpdateGaribs();
	
	// update state of powerups
	if(autoHop)
		autoHop--;
	if(longHop)
		longHop--;
	if(superHop)
		superHop--;
	if(longTongue)
		longTongue--;
	else
		tongueRadius = TONGUE_RADIUSNORMAL;

	// update players (for spawn counters / score bonus)
	if(player[0].spawnTimer)
	{
		player[0].spawnTimer--;
		if(!player[0].spawnTimer)
			player[0].spawnScoreLevel = 1;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: PickupCollectable
	Purpose			: called when a collectable is picked up...
	Parameters		: GARIB *
	Returns			: void
	Info			:
*/
void PickupCollectable(GARIB *garib)
{
	FX_RIPPLE *rip;

	rip = CreateAndAddFXRipple(RIPPLE_TYPE_PICKUP,&garib->sprite.pos,&upVec,20,0,0,25);
	rip->r = 255;
	rip->g = 255;
	rip->b = 0;
	rip->yRotSpeed = 16;

	switch(garib->type)
	{
		case SPAWN_GARIB:
			if(player[0].spawnTimer)
			{
				// increase player score bonus
				if(player[0].spawnScoreLevel < 5)
					player[0].spawnScoreLevel++;
			}

			player[0].spawnTimer = SPAWN_SCOREUPTIMER;

			CreateAndAddSpawnScoreSprite(&garib->sprite.pos,player[0].spawnScoreLevel);
			player[0].score += (player[0].spawnScoreLevel * 10);
			player[0].numSpawn++;
			PlaySample(145,NULL,192,118 + (player[0].spawnScoreLevel * 10));
			break;

		case EXTRAHEALTH_GARIB:
			player[0].score += 100;
			break;

		case EXTRALIFE_GARIB:
			player[0].score += 500;
			break;

		case AUTOHOP_GARIB:
			autoHop	= 150;
			break;

		case LONGHOP_GARIB:
			longHop	= 150;
			break;

		case LONGTONGUE_GARIB:
			longTongue = 150;
			tongueRadius = TONGUE_RADIUSLONG;
			break;
		case WHOLEKEY_GARIB:
			wholeKeyText->draw = 1;
			keyFound = 85;
			break;
		case HALFLKEY_GARIB:
			break;
		case HALFRKEY_GARIB:
			break;
	}

	// remove the collected garib
	SubGarib(garib);
}


/*	--------------------------------------------------------------------------------
	Function		: PutFrogOnGrapplePoint
	Purpose			: Sets frog tile to gametile
	Parameters		: GAMETILE *
	Returns			: void
	Info			:
*/
void PutFrogOnGrapplePoint( GAMETILE *tile )
{
	currTile[0] = tile;
	frog[0]->actor->pos.v[0] = tile->centre.v[0];
	frog[0]->actor->pos.v[1] = tile->centre.v[1];
	frog[0]->actor->pos.v[2] = tile->centre.v[2];
}


/*	--------------------------------------------------------------------------------
	Function		: PickupBabyFrog
	Purpose			: called when a baby frog is picked up...
	Parameters		: ACTOR2
	Returns			: void
	Info			:
*/
void PickupBabyFrog(ACTOR2 *baby)
{
	int babyFrogIndex = numBabies;

	// check if this baby has been collected
	if(baby->action.isSaved)
		return;

	while(babyFrogIndex--)
		if(babies[babyFrogIndex] == baby)
			break;

	baby->action.isSaved	= 1;
	baby->flags				= ACTOR_DRAW_NEVER;
	baby->draw				= 0;

	lastBabySaved = baby;

	babyIcons[babyFrogIndex]->a = 255;
	babyIcons[babyFrogIndex]->animSpeed = 1.0F;
	babiesSaved++;
	
	// make baby position the new start position ?
	if(carryOnBabies)
		gTStart[0] = bTStart[babyFrogIndex];
							  
	player[0].score += (1500 * babiesSaved);
	babySaved = 30;
	PlaySample(147,NULL,192,128);
}

//-------------------------------------------------------------------------------------------------


//----- [ GARIB RELATED ] -----//

GARIBLIST garibCollectableList;


/*	--------------------------------------------------------------------------------
	Function		: InitGaribLinkedList
	Purpose			: initialises the garib linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void InitGaribLinkedList()
{
	garibCollectableList.numEntries = 0;
	garibCollectableList.head.next = garibCollectableList.head.prev = &garibCollectableList.head;
}

/*	--------------------------------------------------------------------------------
	Function		: AddGarib
	Purpose			: adds a garib to the garib list
	Parameters		: GARIB *
	Returns			: void
	Info			: 
*/
void AddGarib(GARIB *garib)
{
	if(garib->next == NULL)
	{
		garibCollectableList.numEntries++;
		garib->prev = &garibCollectableList.head;
		garib->next = garibCollectableList.head.next;
		garibCollectableList.head.next->prev = garib;
		garibCollectableList.head.next = garib;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: SubGarib
	Purpose			: removes a garib from the garib list
	Parameters		: GARIB *
	Returns			: void
	Info			: 
*/
void SubGarib(GARIB *garib)
{
	if(garib->next == NULL)
		return;

	SubSprite(&garib->sprite);

	garib->prev->next = garib->next;
	garib->next->prev = garib->prev;
	garib->next = NULL;
	garibCollectableList.numEntries--;

	JallocFree((UBYTE **)&garib);
}


/*	--------------------------------------------------------------------------------
	Function		: FreeGaribLinkedList
	Purpose			: frees the garib linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeGaribLinkedList()
{
	GARIB *cur,*next;

	if(garibCollectableList.numEntries == 0)
		return;

	dprintf"Freeing linked list : GARIB : (%d elements)\n",garibCollectableList.numEntries));
	for(cur = garibCollectableList.head.next; cur != &garibCollectableList.head; cur = next)
	{
		next = cur->next;

		SubGarib(cur);
	}
}


/*	--------------------------------------------------------------------------------
	Function		: InitGaribSprite
	Purpose			: initialises the sprite for the specified garib
	Parameters		: GARIB *
	Returns			: void
	Info			: 
*/
void InitGaribSprite(GARIB *garib)
{
	switch(garib->type)
	{
		case SPAWN_GARIB:
		case EXTRAHEALTH_GARIB:
		case EXTRALIFE_GARIB:
		case AUTOHOP_GARIB:
		case LONGHOP_GARIB:
		case LONGTONGUE_GARIB:
		case WHOLEKEY_GARIB:
		case HALFLKEY_GARIB:
		case HALFRKEY_GARIB:
			InitSpriteAnimation(&garib->sprite,&garibAnimation[garib->type],0);
			garib->sprite.r = 255;
			garib->sprite.g = 255;
			garib->sprite.b = 255;
			garib->sprite.a = 127;
			break;
	}

	garib->sprite.scaleX = garib->sprite.scaleY = 0;
}

/*	--------------------------------------------------------------------------------
	Function		: CreateNewGarib
	Purpose			: creates a new garib
	Parameters		: VECTOR,int
	Returns			: GARIB *
	Info			: 
*/
GARIB *CreateNewGarib(VECTOR pos,int type)
{
	GARIB *garib;

	garib = (GARIB *)JallocAlloc(sizeof(GARIB),YES,"garib");
	AddGarib(garib);

	SetVector(&garib->sprite.pos,&pos);

	garib->type = type;
	garib->active = 1;
	garib->scale = 0;
	garib->scaleAim = 1;

#ifndef PC_VERSION
	memcpy(&garib->shadow.vert,shadowVtx,sizeof(Vtx) * 4);
#endif
	garib->shadow.altitude	= 0;
	garib->shadow.radius	= 20;
	garib->shadow.alpha		= 192;

	InitGaribSprite(garib);
	
#ifndef PC_VERSION
	garib->sprite.offsetX = -garib->sprite.texture->sx / 2;
	garib->sprite.offsetY = -garib->sprite.texture->sy / 2;
	garib->sprite.flags &= -1 - SPRITE_TRANSLUCENT;
#else
	garib->sprite.offsetX = -16;
	garib->sprite.offsetY = -16;
	garib->sprite.flags &= -1 - SPRITE_TRANSLUCENT;
#endif
	if(garib->active)
		AddSprite(&garib->sprite,NULL);
	
	if(garib->type == SPAWN_GARIB)
	{
		garib->sprite.flags |= SPRITE_TRANSLUCENT;
		garib->sprite.a = 200;
	}
	else if(garib->type == WHOLEKEY_GARIB)
	{
		garib->sprite.flags |= SPRITE_TRANSLUCENT;
		garib->sprite.a = 200;
	}
	else if(garib->type == HALFLKEY_GARIB)
	{
		garib->sprite.flags |= SPRITE_TRANSLUCENT;
		garib->sprite.a = 200;
	}
	else if(garib->type == HALFRKEY_GARIB)
	{
		garib->sprite.flags |= SPRITE_TRANSLUCENT;
		garib->sprite.a = 200;
	}

	return garib;
}

/*	--------------------------------------------------------------------------------
	Function		: UpdateGaribs
	Purpose			: updates the garibs
	Parameters		: 
	Returns			: void
	Info			: 
*/
void UpdateGaribs()
{
	GARIB *garib = NULL;
	float radius;
	float scale;

	// update garib scales and calculate distance from Frog
	for(garib = garibCollectableList.head.next; garib != &garibCollectableList.head; garib = garib->next)
	{
		if(garib->active == NO)
			continue;
		else if(garib->active > 1)
			garib->active--;

		garib->distanceFromFrog = DistanceBetweenPointsSquared(&garib->sprite.pos,&frog[0]->actor->pos);
		radius = 10;
		scale = garib->scale;
		scale -= (scale - garib->scaleAim) / 5;
		garib->scale = scale;

		garib->sprite.scaleX = (64 + SineWave(2,frameCount + garib->type * 2,0) * 10) * scale;
		garib->sprite.scaleY = (64 + SineWave(2,frameCount + garib->type * 2,0) * 10) * scale;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: frog
	Info			: 
*/
void CreateAndAddSpawnScoreSprite(VECTOR *pos,char scoreType)
{
	char bmpBuff[16];

	SPRITE *sprite = (SPRITE *)JallocAlloc(sizeof(SPRITE),YES,"SPRITE");

	SetVector(&sprite->pos,pos);

	sprite->scaleX		= 64;
	sprite->scaleY		= 64;

	sprite->anim.type	= SPRITE_ANIM_SPAWNSCORE;

	sprite->r			= 255;
	sprite->g			= 255;
	sprite->b			= 255;
	sprite->a			= 255;

//	sprite->kill		= 0;	// commented out to matched glover 2 structures
	sprite->flags		= SPRITE_TRANSLUCENT;

	sprintf(bmpBuff,"spnu%d.bmp",(scoreType * 10));
	FindTexture(&sprite->texture,UpdateCRC(bmpBuff),YES,bmpBuff);
	
	sprite->offsetX = -16;
	sprite->offsetY = -16;

	AddSprite(sprite,NULL);
}

