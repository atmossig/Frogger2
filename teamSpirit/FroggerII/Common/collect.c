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
unsigned long longTongue		= 0;
unsigned long superFrog			= 0;
unsigned long croakFloat		= 0;

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
	{ &spriteFrameList[LONGTONGUE_ANIM],0,SPRITE_ANIM_CYCLE,255,255,GARIB_SCALE,GARIB_SCALE,0 },
	{ &spriteFrameList[QUICKHOP_ANIM],0,SPRITE_ANIM_CYCLE,255,255,GARIB_SCALE,GARIB_SCALE,0 },
	{ &spriteFrameList[INVULNERABILITY_ANIM],0,SPRITE_ANIM_CYCLE,255,255,GARIB_SCALE,GARIB_SCALE,0 },
};


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
	VECTOR *check;
	
	// check current tile for a garib
	for(garib = garibCollectableList.head.next, i = garibCollectableList.numEntries-1;
		garib != &garibCollectableList.head; garib = garib->next, i--)
	{
		// process only garibs in visual range
		if( DistanceBetweenPointsSquared(&garib->pos, &frog[0]->actor->pos ) > ACTOR_DRAWDISTANCEINNER)
			continue;

		// Also don't pickup garibs that the frog is trying to tongue
		if( garib == (GARIB *)tongue[pl].thing )
			continue;

		// Health garibs are buzzy flies
		if( garib->type == EXTRAHEALTH_GARIB )
			check = &garib->fx->act[0]->actor->pos;
		else
			check = &garib->pos;

		if( DistanceBetweenPointsSquared( check, &frog[0]->actor->pos ) < PICKUP_RADIUS_SQUARED)
		{
			garibStoreList[player[0].levelNum-3][i / 8] &= ~(1 << (i & 7));
			PickupCollectable(garib,pl);
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
	
	// update players (for spawn counters / score bonus)
	if(player[0].spawnTimer && gameState.multi == SINGLEPLAYER)
	{
		player[0].spawnTimer--;
		if(!player[0].spawnTimer)
			player[0].spawnScoreLevel = 1;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void PickupCollectable(GARIB *garib, int pl)
{
	SPECFX *fx;
	VECTOR seUp;
	int i;

	switch(garib->type)
	{
		case SPAWN_GARIB:
			if(player[pl].spawnTimer)
			{
				VECTOR m;
				// increase player score bonus
				if(player[pl].spawnScoreLevel < 5)
					player[pl].spawnScoreLevel++;

				XfmPoint (&m,&garib->pos);
			}

			player[pl].spawnTimer = SPAWN_SCOREUPTIMER;

			CreateAndAddSpawnScoreSprite(&garib->pos,player[pl].spawnScoreLevel);

			// we need to get the up vector for this collectable...
			SetVector(&seUp,&upVec);
			RotateVectorByQuaternion(&seUp,&seUp,&frog[pl]->actor->qRot);

			if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKLYTRAIL, &garib->pos, &seUp, 20, 2, 0, 2 )) )
			{
				SetFXColour(fx,255,255,255);
				SetVector(&fx->rebound->point,&garib->pos);
				SetVector(&fx->rebound->normal,&seUp);
				fx->gravity = 0.2;
			}
			if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKLYTRAIL, &garib->pos, &seUp, player[pl].spawnScoreLevel * 5, player[pl].spawnScoreLevel, 0, 3 )) )
			{
				SetFXColour(fx,255,255,0);
				SetVector(&fx->rebound->point,&garib->pos);
				SetVector(&fx->rebound->normal,&seUp);
				fx->gravity = 0.1;
			}
			if(player[pl].spawnScoreLevel == 5)
			{
				if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKLYTRAIL, &garib->pos, &seUp, 50, 4, 0, 6 )) )
				{
					SetFXColour(fx,0,255,255);
					SetVector(&fx->rebound->point,&garib->pos);
					SetVector(&fx->rebound->normal,&seUp);
					fx->gravity = 0.15;
				}
			}

			player[pl].score += (player[pl].spawnScoreLevel * 10);
			player[pl].numSpawn++;

			if (player[pl].numSpawn>100)
			{
				player[pl].numSpawn = 0;
				player[pl].numCredits++;
			}
			//PlaySample(0,&garib->pos,192,118 + (player[pl].spawnScoreLevel * 10));
			break;

		case EXTRAHEALTH_GARIB:
			if( player[pl].healthPoints < 3 )
				player[pl].healthPoints++;

			if( (fx = CreateAndAddSpecialEffect( FXTYPE_GARIBCOLLECT, &garib->fx->act[pl]->actor->pos, &upVec, 25, 0.0, 0.0, 2.0 )) )
			{
				SetFXColour( fx, 30, 240, 30 );
				SubSpecFX( garib->fx );
			}
			break;

		case EXTRALIFE_GARIB:
			if( player[pl].lives < 100 )
				player[pl].lives++;
			break;

		case AUTOHOP_GARIB:
			GTInit( &player[pl].autohop, 10 );
			break;

		case LONGTONGUE_GARIB:
			GTInit( &player[pl].longtongue, 30 );
			tongue[pl].radius = TONGUE_RADIUSLONG;
			break;

		case QUICKHOP_GARIB:
			GTInit( &player[pl].quickhop, 10 );
			break;

		case INVULNERABILITY_GARIB:
//			fx = CreateAndAddSpecialEffect( FXTYPE_FROGSHIELD, &frog[pl]->actor->pos, &currTile[pl]->normal, 150, 0, 0, 10 );
//			fx->follow = frog[pl]->actor;
			GTInit( &player[pl].safe, 10 );
			break;
	}

	// remove the collected garib
	SubGarib(garib);
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

	if( garib->type != EXTRAHEALTH_GARIB && garib->sprite )
		SubSprite( garib->sprite );

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
	}

	garib = (GARIB *)JallocAlloc(sizeof(GARIB),YES,"garib");
	AddGarib(garib);

	SetVector(&garib->pos,&pos);

//	garib->gameTile = gameTile;
//	garib->dropSpeed = dropSpeed;
	garib->type = type;
	garib->active = 1;
	garib->scale = 0;
	garib->scaleAim = 1;

//#ifndef PC_VERSION
//	memcpy(&garib->shadow.vert,shadowVtx,sizeof(Vtx) * 4);
//#endif
//	garib->shadow.altitude	= 0;
//	garib->shadow.radius	= 20;
//	garib->shadow.alpha		= 192;

	
	if( garib->type == EXTRAHEALTH_GARIB )
	{
		SPECFX *fx;
		if( (garib->fx = CreateAndAddSpecialEffect( FXTYPE_HEALTHFLY, &garib->pos, &upVec, 1, 1, 0.06, 0 )) )
		{
			garib->fx->gravity = -0.5;
		}
		if( (fx = CreateAndAddSpecialEffect( FXTYPE_TRAIL, &garib->fx->act[0]->actor->pos, &upVec, 5, 0.95, 0.00, 0.6 )) )
		{
			fx->follow = garib->fx->act[0]->actor;
			SetFXColour( fx, 0, 128, 255 );
		}
	}
	else
	{
		// initialise garib sprite
		garib->sprite = (SPRITE *)JallocAlloc(sizeof(SPRITE),YES,"garspr");
		SetVector(&garib->sprite->pos,&pos);
		InitSpriteAnimation( garib->sprite, &garibAnimation[garib->type], 0 );
		garib->sprite->r = 255;
		garib->sprite->g = 255;
		garib->sprite->b = 255;
		garib->sprite->a = 127;
		garib->sprite->scaleX = garib->sprite->scaleY = 0;

		// ok - make the spawn garib a rotating sprite - ANDYE
		garib->sprite->flags	|= SPRITE_FLAGS_ROTATE;
		garib->sprite->angle	= 1 / (1 + (rand() % 10));
		garib->sprite->angleInc = 0.1f;
		
#ifndef PC_VERSION
		garib->sprite->offsetX = -garib->sprite->texture->sx / 2;
		garib->sprite->offsetY = -garib->sprite->texture->sy / 2;
		garib->sprite->flags &= -1 - SPRITE_TRANSLUCENT;
#else
		garib->sprite->offsetX = -16;
		garib->sprite->offsetY = -16;
		garib->sprite->flags &= -1 - SPRITE_TRANSLUCENT;
#endif
		if(garib->active)
			AddSprite(garib->sprite,NULL);
		garib->sprite->flags |= SPRITE_TRANSLUCENT;
		garib->sprite->a = 200;
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
	VECTOR fwd;
	VECTOR actualPos;

	// update garib scales and calculate distance from Frog
	for(garib = garibCollectableList.head.next; garib != &garibCollectableList.head; garib = garib->next)
	{
		if(garib->active == NO)
			continue;
		else if(garib->active > 1)
			garib->active--;

		radius = 10;
		scale = garib->scale;
		scale -= (scale - garib->scaleAim) / 5;
		garib->scale = scale;

		if( garib->sprite )
		{
			garib->sprite->scaleX = 64;
			garib->sprite->scaleY = 64;

			// rotate the little blighters
			garib->sprite->angle += (garib->sprite->angleInc * gameSpeed);
		}

		// Drop Garibs.............

		if ( garib->gameTile )
		{			
			SetVector ( &actualPos, &garib->gameTile->centre );
			actualPos.v[Y] += 20;
			SubVector ( &fwd, &actualPos, &garib->pos );
			MakeUnit  ( &fwd );
			garib->pos.v[X] += ( fwd.v[X] * garib->dropSpeed );
			garib->pos.v[Y] += ( fwd.v[Y] * garib->dropSpeed );
			garib->pos.v[Z] += ( fwd.v[Z] * garib->dropSpeed );
			SetVector( &garib->sprite->pos, &garib->pos );
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void CreateAndAddSpawnScoreSprite(VECTOR *pos,char scoreType)
{
	char bmpBuff[16];
	SPRITE *sprite = (SPRITE *)JallocAlloc(sizeof(SPRITE),YES,"SPRITE");

	SetVector(&sprite->pos,pos);

	sprite->scaleX		= 16;
	sprite->scaleY		= 16;

	sprite->anim.type	= SPRITE_ANIM_SPAWNSCORE;

	sprite->r			= 255;
	sprite->g			= 255;
	sprite->b			= 255;
	sprite->a			= 255;

//	sprite->kill		= 0;	// commented out to matched glover 2 structures
	sprite->flags		= SPRITE_TRANSLUCENT | SPRITE_FLAGS_ROTATE;
	sprite->angle		= 0.0f;
	sprite->angleInc	= 0.2f;

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
