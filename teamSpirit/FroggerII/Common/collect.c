/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: collect.c
	Programmer	: Andrew Eder
	Date		: 11/27/98

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>

#include "incs.h"

SCREENSPAWN *spawnList = NULL;

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
	Function		: GrapplePointInTongueRange
	Purpose			: indicates if a grapple point is in range when tongueing
	Parameters		: 
	Returns			: GAMETILE *
	Info			: returns ptr to the nearest gametile (if in range)
*/
GAMETILE *GrapplePointInTongueRange( )
{
	VECTOR diff;
	GAMETILE *cur, *best = NULL;
	float min = 100000, dist;
	cur = firstTile;

	for( ;cur; cur=cur->next )
	{
		if( cur->state == TILESTATE_GRAPPLE )
		{
			dist = DistanceBetweenPointsSquared(&frog[0]->actor->pos, &cur->centre);
			if( dist < min ) 
			{
				min = dist; 
				best = cur;
			}
		}
	}


	if( best && (min < tongueRadius * tongueRadius))
		return best;
	
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

void AddScreenSpawn(long x, long y, long xa, long ya)
{
	SCREENSPAWN *me ;
	
	me = JallocAlloc(sizeof(SCREENSPAWN),"sspa",0);

	me->x = x;
	me->y = y;

	me->ax = xa;
	me->ay = ya;
	
	me->next = spawnList;

	me->sp = CreateAndAddSpriteOverlay(x,y,"rdfrg001.bmp",16,16,255,255,255,91,ANIMATION_FORWARDS | ANIMATION_CYCLE);

	if (spawnList)
		spawnList->prev = me;

	spawnList = me;

}

float spawnSpeed = 0.5;

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

void UpdateScreenSpawn(void)
{
	SCREENSPAWN *cur = spawnList;

	while (cur)
	{
		cur->x -= (cur->x - cur->ax) * spawnSpeed;
		cur->y -= (cur->y - cur->ay) * spawnSpeed;
		
		if (cur->x==cur->ax)
			if (cur->y==cur->ay)
			{
				SubSpriteOverlay(cur->sp);

				if (cur->prev)
					cur->prev->next = cur->next;
				if (cur->next)
					cur->next->prev = cur->prev;

				JallocFree(cur);
			}

		cur = cur->next;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
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
				VECTOR m;
				// increase player score bonus
				if(player[0].spawnScoreLevel < 5)
					player[0].spawnScoreLevel++;

				XfmPoint (&m,&garib->sprite.pos);
				
			//	if (m.v[3]>0)
			//		AddScreenSpawn(m.v[0], m.v[1],0,0);
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

