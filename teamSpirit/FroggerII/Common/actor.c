/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: actor.c
	Programmer	: Matthew Cloy
	Date		: 11/11/98

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

#include <ultra64.h>
#include <PR/ramrom.h>					// Needed for argument passing into the app
#include <PR/gbi.h>
#include <assert.h>
#include <pr/os.h>

#include "incs.h"


float ACTOR_DRAWDISTANCEINNER = 100000.0F;
float ACTOR_DRAWDISTANCEOUTER = 125000.0F;

ACTOR2 *actList				= NULL;			// entire actor list

float globalVisOffset		= 0.4;


/* --------------------------------------------------------------------------------	
	Programmer	: Matthew Cloy
	Function    : DrawActorList

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/
float meMod;

void XformActor(ACTOR *ptr);

void XformActorList()
{
	ACTOR2 *cur,*next;
		
	objectMatrix = 0;
	cur = actList;
	while(cur)
	{
		if (frontEndState.mode != OBJVIEW_MODE)
		{
			// calculate the distance between the camera and this actor
			cur->distanceFromFrog = DistanceBetweenPointsSquared(&cur->actor->pos,&frog->actor->pos);

			// transform actor
			XformActor(cur->actor);

			// determine this actor's visibility
			cur->draw = 0;
			if(cur->flags & ACTOR_DRAW_ALWAYS)
			{
				// always draw this actor
				cur->draw = 1;
			}
			else if(cur->flags & ACTOR_DRAW_CULLED)
			{
				// determine if actor is visible
				if(cur->distanceFromFrog < ACTOR_DRAWDISTANCEINNER)
					cur->draw = 1;
			}
		}
		else
		{
			// transform actor
			XformActor(cur->actor);
			cur->draw = 0;
			if(cur->flags & ACTOR_DRAW_ALWAYS)
			{
				// always draw this actor
				cur->draw = 1;
			}
		}


		cur = cur->next;
	}
}

void DrawActorList()
{
	ACTOR2 *cur,*next;
	float distance;
		
	objectMatrix = 0;
	SetRenderMode();

	cur = actList;
	while(cur)
	{
		if(gameState.mode == GAME_MODE || gameState.mode == OBJVIEW_MODE || 
		   gameState.mode == RECORDKEY_MODE || gameState.mode == LEVELPLAYING_MODE ||
		   gameState.mode == FRONTEND_MODE  || gameState.mode == CAMEO_MODE || gameState.mode == PAUSE_MODE )
		{
			if(cur->draw)
				DrawActor(cur->actor);
		}
	
		cur = cur->next;
	}
}

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: FreeActorList

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/
void FreeActorList()
{
	ACTOR2 *next,*cur;
	cur = actList;
	while (cur)
	{
		next = cur->next;

		if(cur->actor)
		{
			if(cur->actor->animation)
			{
				// free any animation associated with ACTOR type
				JallocFree((UBYTE**)&cur->actor->animation);
			}

			if(cur->actor->shadow)
			{
				// free any shadow associated with ACTOR type
				JallocFree((UBYTE**)&cur->actor->shadow);
			}

			if((cur->actor->objectController) && (cur->actor->objectController->object))
			{
				// free any object sprites for this actor
				FreeObjectSprites(cur->actor->objectController->object);
			}

			// free associated ACTOR type
			JallocFree((UBYTE**)&cur->actor);
		}

		JallocFree((UBYTE**)&cur);
		cur = next;
	}
	actList				= NULL;
}
/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: CreateAndAddActor 
	Purpose		:
	Parameters	: 
	Returns		: ACTOR2 *
*/
ACTOR2 *CreateAndAddActor(char *name,float cx,float cy,float cz,int initFlags,int enemyType,float offset,int startNode)
{
	ACTOR2 *newItem;
	newItem			= (ACTOR2 *)JallocAlloc(sizeof(ACTOR2), 1, "A2");
	newItem->actor	= (ACTOR *)JallocAlloc(sizeof(ACTOR), 1, "A");
	InitActor(newItem->actor,name,cx,cy,cz,initFlags);

	newItem->actor->oldpos.v[X]	= cx;
	newItem->actor->oldpos.v[Y]	= cy;
	newItem->actor->oldpos.v[Z]	= cz;

	newItem->draw	= 0;
	newItem->flags	|= ACTOR_DRAW_CULLED;

	// add actor object sprites to sprite list
	if((newItem->actor->objectController) && (newItem->actor->objectController->object))
		AddObjectsSpritesToSpriteList(newItem->actor->objectController->object,0);

	// initialise enemy type dudes
	if(enemyType > NMETYPE_NONE)
	{
		// This actor is an enemy so add to enemy actor list
		InitActorAnim(newItem->actor);

		switch(enemyType)
		{
			case NMETYPE_MOWER:
			case NMETYPE_ROLLER:
				AnimateActor(newItem->actor,0,YES,NO,1.5F);
				newItem->actor->status = NMESTATE_MOWER_IDLE;
				break;
			case NMETYPE_DOG:
				AnimateActor(newItem->actor,0,NO,NO,0.75F);
				newItem->actor->status = NMESTATE_DOG_IDLE;
				break;
			case NMETYPE_SNAPPER:
				AnimateActor(newItem->actor,Random(3)+1,NO,NO,0.75F);
				newItem->actor->status = NMESTATE_SNAPPER_IDLE;
				break;
			case NMETYPE_WASP:
				AnimateActor(newItem->actor,0,NO,NO,1.0F);
				newItem->actor->status = NMESTATE_WASP_MOVING;
				break;
			case NMETYPE_SNAKE:
				AnimateActor(newItem->actor,3,YES,NO,2.0F);
				newItem->actor->status = NMESTATE_WASP_MOVING;
				break;
			case NMETYPE_CAR:
				AnimateActor(newItem->actor,0,YES,NO,1.0F);
				newItem->actor->status = NMESTATE_CAR_MOVING;
				break;
			case NMETYPE_TRUCK:
				AnimateActor(newItem->actor,0,YES,NO,1.0F);
				newItem->actor->status = NMESTATE_TRUCK_MOVING;
				break;
			case NMETYPE_SHARK:
				AnimateActor(newItem->actor,0,YES,NO,0.5F);
				newItem->actor->status = NMESTATE_SHARK_IDLE;
				break;
		}

		newItem->actor->type = enemyType;
		newItem->action.dead = 30;
	}
	
	newItem->speed				= 18.0;
	newItem->offset				= 0.0;
	newItem->distanceFromFrog	= 0.0F;

	//******************************************************************************//

	newItem->next = actList;
	actList = newItem;

	return newItem;
}


/*	--------------------------------------------------------------------------------
	Function		: AddObjectsSpritesToSpriteList
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void AddObjectsSpritesToSpriteList(OBJECT *obj,short flags)
{
	SPRITE *sprite;
	int i;

	if(obj->numSprites > 0)
	{
		for(i=0; i<obj->numSprites; i++)
		{
			sprite = (SPRITE *)JallocAlloc(sizeof(SPRITE),YES,"ObjSprite");

			sprite->texture = obj->sprites[i].textureID;
			SetVector(&sprite->pos,&zero);

			if((obj->sprites[i].flags & SPRITE_DONE) == 0)
			{
				obj->sprites[i].flags |= SPRITE_DONE;
				switch(sprite->texture->sx)
				{
					case 8:
						obj->sprites[i].sx /= 2;
						break;
					case 16:
						obj->sprites[i].sx *= 2;
						break;
					case 32:
						break;
				}
				switch(sprite->texture->sy)
				{
					case 8:
//						obj->sprites[i].sy /= 2;
						break;
					case 16:
						obj->sprites[i].sy *= 2;
						break;
					case 32:
						break;
				}
			}

			sprite->r = sprite->g = sprite->b = 255;
			sprite->a = 128;
			if(obj->xlu < 255)
				sprite->flags = SPRITE_TRANSLUCENT;
			else
				sprite->flags = 0;

			sprite->flags |= flags;

			sprite->offsetX = -sprite->texture->sx / 2;
			sprite->offsetY = -sprite->texture->sy / 2;

			AddSprite(sprite,NULL);
			obj->sprites[i].sprite = sprite;
		}
	}

	if(obj->children)
		AddObjectsSpritesToSpriteList(obj->children,flags);

	if(obj->next)
		AddObjectsSpritesToSpriteList(obj->next,flags);
}

/*	--------------------------------------------------------------------------------
	Function		: RemoveObjectSprites
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void RemoveObjectSprites(OBJECT *obj,BOOL free)
{
	int i;

	if(obj->sprites)
	{
		for(i=0; i<obj->numSprites; i++)
		{
			if(obj->sprites[i].sprite)
				SubSprite(obj->sprites[i].sprite);
			if(free)
				JallocFree((UBYTE**)&obj->sprites[i].sprite);
		}
	}

	if(obj->children)
		RemoveObjectSprites(obj->children,free);
	
	if(obj->next)
		RemoveObjectSprites(obj->next,free);
}

/*	--------------------------------------------------------------------------------
	Function		: FreeObjectSprites
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void FreeObjectSprites(OBJECT *obj)
{
	int i;

	if(obj->sprites)
	{
		for(i=0; i<obj->numSprites; i++)
		{
			if(obj->sprites[i].sprite)
			{
				SubSprite(obj->sprites[i].sprite);
				JallocFree((UBYTE**)&obj->sprites[i].sprite);
			}
		}
	}

	if(obj->children)
		FreeObjectSprites(obj->children);
	
	if(obj->next)
		FreeObjectSprites(obj->next);
}

