/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: collect.c
	Programmer	: Andrew Eder
	Date		: 11/27/98

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>

#include "incs.h"

#define storeListLength  16

SCREENSPAWN *spawnList = NULL;

unsigned long autoHop			= 0;
unsigned long longHop			= 0;
unsigned long longTongue		= 0;
unsigned long superFrog			= 0;
unsigned long croakFloat		= 0;
unsigned long babiesSaved		= 0;
extern long carryOnBabies;

unsigned char garibStoreList [4][storeListLength ];

unsigned char garibListPos = 0;

int reset = 0;

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
//		garib = CreateNewGarib(v, type);
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
	float dist,mags[4];
	int i = 0,numInRange = 0;

	if(numBabies)
	{
		for(i=0; i<numBabies; i++)
		{
			if ( babies[i] )
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
}


/*	--------------------------------------------------------------------------------
	Function		: ScenicIsInTongueRange
	Purpose			: indicates if a scenic is in range when tongueing
	Parameters		: 
	Returns			: ACTOR2 *
	Info			: returns ptr to the nearest scenic (if in range)
*/
ACTOR2 *ScenicIsInTongueRange()
{
	PLATFORM *cur,*next,*nearest;
	PLATFORM *inRange[4];
	float dist,mags[4];
	int i = 0,numInRange = 0;
		
	dprintf"<< ATTEMPTING TO TONGUE A SCENIC >>\n"));

	for(cur = platformList.head.next; cur != &platformList.head; cur = next)
	{
		next = cur->next;

		// only check for scenics in visual range
		dist = DistanceBetweenPointsSquared(&frog[0]->actor->pos,&cur->pltActor->actor->pos);
		if(dist > (tongueRadius * tongueRadius))
			continue;

		if((cur->flags & PLATFORM_NEW_SHAKABLESCENIC) && (numInRange < 4))
		{
			mags[numInRange]		= dist;
			inRange[numInRange++]	= cur;
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

		// we now have the nearest 'scenic' - return relevant actor
		return nearest->pltActor;
	}

	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function		: CheckTileForCollectable
	Purpose			: checks frogs position for a collectable item
	Parameters		: 
	Returns			: void
	Info			: used to check if Frogger has jumped on a collectable item
*/
void CheckTileForCollectable(GAMETILE *tile, long pl)
{
	GARIB *garib;
	int i;
	
	// check current tile for a garib
	for(garib = garibCollectableList.head.next, i = garibCollectableList.numEntries-1;
		garib != &garibCollectableList.head; garib = garib->next, i--)
	{
		// process only garibs in visual range
		if(garib->distanceFromFrog > ACTOR_DRAWDISTANCEINNER)
			continue;

		if(DistanceBetweenPointsSquared(&garib->sprite.pos,&frog[pl]->actor->pos) < (PICKUP_RADIUS * PICKUP_RADIUS))
		{
			garibStoreList[player[0].levelNum-3][i / 8] &= ~(1 << (i & 7));
			PickupCollectable(garib);
			return;
		}
	}

	// check current tile for a baby frog
	i = numBabies;
	while(i--)
	{
		if ( babies[i] )
			if(DistanceBetweenPointsSquared(&babies[i]->actor->pos,&frog[pl]->actor->pos) < (PICKUP_RADIUS * PICKUP_RADIUS))
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
	SCREENSPAWN *me = (SCREENSPAWN *)JallocAlloc(sizeof(SCREENSPAWN),0,"sspa");

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

				JallocFree((UBYTE**)&cur);
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
			PlaySample(0,NULL,192,118 + (player[0].spawnScoreLevel * 10)); //145
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
//	baby->flags	ACTOR2			= ACTOR_DRAW_NEVER;
	baby->actor->xluOverride = 0;
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
	PlaySample(GEN_TARGET_COM,0,0,0); // 147
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

	if ( !reset )
	{
		memset ( garibStoreList, 0xff, storeListLength*4 );
		reset = 1;
	}
	// ENDIF
	garibListPos = 0;
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
	static indexPos = 0;

	unsigned char value;

	GARIB *garib;

	if ( worldVisualData [ player[0].worldNum ].levelVisualData [ player[0].levelNum ].multiPartLevel == MULTI_PART )
	{
		value = garibStoreList[player[0].levelNum-3][garibListPos / 8] & (1 << (garibListPos & 7));   // position to retrive from
		garibListPos++;

		if ( value == 0 )
		{
			dprintf"Returned, did not  garib ( %d : %d : %d )\n",value, garibListPos-1, player[0].levelNum-3));
			return NULL;
		}
		// ENDIF

//		if ( 
//		{
//		}
		// ENDIF
	}
	// ENDIF

	garib = (GARIB *)JallocAlloc(sizeof(GARIB),YES,"garib");
	AddGarib(garib);

	SetVector(&garib->sprite.pos,&pos);

//	garib->gameTile = gameTile;
//	garib->dropSpeed = dropSpeed;
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

//	garibListPos++;

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
	VECTOR fwd;
	VECTOR actualPos;

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

		// Drop Garibs.............

		if ( garib->gameTile != NULL )
		{			
			SetVector ( &actualPos, &garib->gameTile->centre );
			actualPos.v[Y] += 20;
			SubVector ( &fwd, &actualPos, &garib->sprite.pos );
			MakeUnit  ( &fwd );
			garib->sprite.pos.v[X] += ( fwd.v[X] * garib->dropSpeed );
			garib->sprite.pos.v[Y] += ( fwd.v[Y] * garib->dropSpeed );
			garib->sprite.pos.v[Z] += ( fwd.v[Z] * garib->dropSpeed );

		}
		// ENDIF

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
	FindTexture(&sprite->texture,UpdateCRC(bmpBuff),YES);
	
	sprite->offsetX = -16;
	sprite->offsetY = -16;

	AddSprite(sprite,NULL);
}

void DropGaribToTile(GARIB *garib, GAMETILE *tile, float dropSpeed)
{
	garib->gameTile = tile;
	garib->dropSpeed = dropSpeed;
}
