/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: actor.c
	Programmer	: Matthew Cloy
	Date		: 11/11/98

----------------------------------------------------------------------------------------------- */


#define F3DEX_GBI

#include <ultra64.h>

#ifndef PC_VERSION
#include <PR/ramrom.h>					// Needed for argument passing into the app
#include <PR/gbi.h>
#include <PR/os.h>
#endif

#include <assert.h>

#include "incs.h"


#define MAX_UNIQUE_ACTORS	50

float ACTOR_DRAWDISTANCEINNER = 100000.0F;
float ACTOR_DRAWDISTANCEOUTER = 125000.0F;

int objectMatrix = 0;

ACTOR2 *actList				= NULL;			// entire actor list

//used to keep a count of how many of each enemy are present at the same time
char uniqueEnemyCount[20];

//used to keep a count of how many of each enemy are present at the same time
//char uniqueEnemyCount[CAMEO_ACTOR];
int uniqueActorCRC[MAX_UNIQUE_ACTORS];
char numUniqueActors = 0;

long environmentMapped = 0;

/* --------------------------------------------------------------------------------	
	Programmer	: Matthew Cloy
	Function    : DrawActorList

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/
float meMod;

void XformActor(ACTOR *actor);

void XformActorList()
{
	ACTOR2	*cur;

	cur = actList;
	while ( cur )
	{
		if (frontEndState.mode != OBJVIEW_MODE)
		{
			// calculate the distance between the camera and this actor
			cur->distanceFromFrog = DistanceBetweenPointsSquared ( &cur->actor->pos, &frog[0]->actor->pos );

			// transform actor
			XformActor ( cur->actor );

			// determine this actor's visibility
#ifdef PC_VERSION
			cur->draw = 1;
#else
			cur->draw = 0;
#endif
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
			XformActor ( cur->actor );
			cur->draw = 0;
			if ( cur->flags & ACTOR_DRAW_ALWAYS )
			{
				// always draw this actor
				cur->draw = 1;
			}
			// ENDIF
		}
		// ENDIF

		cur = cur->next;
	}
	// ENDWHILE



/*	for(ptr = actorList.head.next;ptr != &actorList.head;ptr = ptr->next)
	{
		TransformActor(ptr);
	}
  */

/*	ACTOR2 *cur,*next;
		
	objectMatrix = 0;
	cur = actList;
	while(cur)
	{
		if (frontEndState.mode != OBJVIEW_MODE)
		{
			// calculate the distance between the camera and this actor
			cur->distanceFromFrog = DistanceBetweenPointsSquared(&cur->actor->pos,&frog[0]->actor->pos);

			// transform actor
			XformActor(cur->actor);

			// determine this actor's visibility
#ifdef PC_VERSION
			cur->draw = 1;
#else
			cur->draw = 0;
#endif
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
	}*/
}

void DrawActorList()
{
	ACTOR2	*cur;


	InitDisplayLists();
	SetRenderMode();
	SetupViewing();

/*	ACTOR2 *cur,*next;
	float distance;
		
	objectMatrix = 0;
//	SetRenderMode();

*/	cur = actList;
	while(cur)
	{
		if(gameState.mode == GAME_MODE || gameState.mode == OBJVIEW_MODE || 
		   gameState.mode == RECORDKEY_MODE || gameState.mode == LEVELPLAYING_MODE ||
		   gameState.mode == FRONTEND_MODE  || gameState.mode == CAMEO_MODE || gameState.mode == PAUSE_MODE )
		{
			if(cur->draw && !(cur->flags & ACTOR_DRAW_LAST) )
			{
				environmentMapped = 0;

				DrawActor(cur->actor);
			}
		}
	
		cur = cur->next;
	} 
}

void DrawCameraSpaceActorList()
{
	ACTOR2	*cur;

	cur = actList;
	while(cur)
	{
		if(gameState.mode == GAME_MODE || gameState.mode == OBJVIEW_MODE || 
		   gameState.mode == RECORDKEY_MODE || gameState.mode == LEVELPLAYING_MODE ||
		   gameState.mode == FRONTEND_MODE  || gameState.mode == CAMEO_MODE || gameState.mode == PAUSE_MODE )
		{
			if(cur->draw && (cur->flags & ACTOR_DRAW_LAST) )
			{
				if( cur->flags & ACTOR_LEVEL_TROPHY )
				{
					static TEXTURE *tex;
					static short lastA = -1;

					environmentMapped = 1;
					gDPSetTextureLUT(glistp++,G_TT_NONE);

					if( lastA != award )
					{
						lastA = award;

						switch( award )
						{
						case 0:
							FindTexture( &tex, UpdateCRC("chrome.bmp"), YES, "chrome.bmp" );
							break;
						case 1:
							FindTexture( &tex, UpdateCRC("chrome.bmp"), YES, "chrome.bmp" );
							break;
						case 2:
							FindTexture( &tex, UpdateCRC("chrome.bmp"), YES, "chrome.bmp" );
							break;
						}
					}

					gSPTexture(glistp++,32<<6,32<<6,0,G_TX_RENDERTILE, G_ON);
					gDPLoadTextureBlock(glistp++,tex->data,G_IM_FMT_RGBA,G_IM_SIZ_16b,tex->sx,tex->sy,
											0,G_TX_WRAP,G_TX_WRAP,5,5,G_TX_NOLOD,G_TX_NOLOD);
				}

				DrawActor(cur->actor);
			}
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
ACTOR2 *CreateAndAddActor(char *name,float cx,float cy,float cz,int initFlags,float offset,int startNode)
{
	ACTOR2 *newItem;
	newItem			= (ACTOR2 *)JallocAlloc(sizeof(ACTOR2), 1, "A2");
	newItem->actor	= (ACTOR *)JallocAlloc(sizeof(ACTOR), 1, "A");


	InitActor(newItem->actor,name,cx,cy,cz,initFlags);


	if ( gstrcmp ( name, "wasp.obe" ) == 0 )
		MakeUniqueActor(newItem->actor,0);
	// ENDIF
	if ( gstrcmp ( name, "treetrnk.obe" ) == 0 )
		MakeUniqueActor(newItem->actor,0);
	// ENDIF
	if ( gstrcmp ( name, "a_rushes.obe" ) == 0 )
		MakeUniqueActor(newItem->actor,0);
	// ENDIF
	if ( gstrcmp ( name, "conetreeleav.obe" ) == 0 )
		MakeUniqueActor(newItem->actor,0);
	// ENDIF
	if ( gstrcmp ( name, "froglet.obe" ) == 0 )
		MakeUniqueActor(newItem->actor,0);
	// ENDIF
	if ( gstrcmp ( name, "frogger.obe" ) == 0 )
		MakeUniqueActor(newItem->actor,0);
	// ENDIF
	if ( gstrcmp ( name, "mole.obe" ) == 0 )
		MakeUniqueActor(newItem->actor,0);
	// ENDIF
	if ( gstrcmp ( name, "roto.obe" ) == 0 )
		MakeUniqueActor(newItem->actor,0);
	// ENDIF
	if ( gstrcmp ( name, "pltlilly.obe" ) == 0 )
		MakeUniqueActor(newItem->actor,0);
	// ENDIF
	if ( gstrcmp ( name, "appltree.obe" ) == 0 )
		MakeUniqueActor(newItem->actor,0);
	// ENDIF
		
	newItem->actor->oldpos.v[X]	= cx;
	newItem->actor->oldpos.v[Y]	= cy;
	newItem->actor->oldpos.v[Z]	= cz;

	newItem->draw	= 0;
	newItem->flags	|= ACTOR_DRAW_CULLED;
	newItem->radius	= 0.0F;

	// add actor object sprites to sprite list
	if((newItem->actor->objectController) && (newItem->actor->objectController->object))
		AddObjectsSpritesToSpriteList(newItem->actor->objectController->object,0);
	
	newItem->speed				= 18.0;
	newItem->offset				= 0.0;
	newItem->distanceFromFrog	= 0.0F;

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


/* --------------------------------------------------------------------------------
	Function	: ActorsHaveCollided
	Purpose		: determines is two actors have collided
	Parameters	: ACTOR2 *,ACTOR2 *
	Returns		: BOOL 
	Info		:
*/
BOOL ActorsHaveCollided(ACTOR2 *act1,ACTOR2 *act2)
{
	VECTOR result;
	float crad = (act1->radius + act2->radius);

	SubVector(&result,&act1->actor->pos,&act2->actor->pos);

	if ((Fabs(result.v[0]) < crad) &&
		(Fabs(result.v[1]) < crad) &&
		(Fabs(result.v[2]) < crad))
		return TRUE;

	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function		: SetActorCollisionRadius
	Purpose			: sets the specified actor's collision radius
	Parameters		: ACTOR2 *,float
	Returns			: void
	Info			: 
*/
void SetActorCollisionRadius(ACTOR2 *act,float radius)
{
	act->radius = radius;
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CopyDrawlist(u8 *dest, u8 *source)
{

	while(*source != 0xB8)
	{
		*(Gfx *)dest = *(Gfx *)source;
		dest+=8;
		source+=8;
	}
	//must copy last entry (end drawlist)
	*(Gfx *)dest = *(Gfx *)source;

}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int CalculateSizeOfDrawlist(Gfx *dl)
{
	u8	*temp = (u8 *)dl; 
	int size = 0;

	while(*temp != 0xB8)
	{
		temp += 8;
		size++;
	}
	size++;
	return size;	

}



/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void MakeUniqueVtx(OBJECT_CONTROLLER *objC)
{
/*	short i;
	Vtx *vtxa;
	Vtx *vtxb;
	Vtx *oldVtxa, *oldVtxb;
	int offset;

//	dprintf"make unique vtx %s\n", objC->object->name));

	oldVtxa = objC->vtx[0];
	oldVtxb = objC->vtx[1];

	vtxa = (Vtx *)JallocAlloc(sizeof(Vtx) * objC->numVtx * 2, NO, "unqVtx");
	vtxb = vtxa + objC->numVtx;

	memcpy(vtxa, oldVtxa, sizeof(Vtx) * objC->numVtx);
	memcpy(vtxb, oldVtxb, sizeof(Vtx) * objC->numVtx);

	//controller now references new vtx's, must also make sure that drawlist is updated
	objC->vtx[0] = vtxa;
	objC->vtx[1] = vtxb;
	offset = (int)oldVtxa - (int)vtxa;// - objC->Vtx[0];
	AddOffsetToVertexLoads(-offset, objC->drawList); */
	
	short i;
	Vtx *vtxa;
	Vtx *vtxb;
	Vtx *oldVtxa, *oldVtxb;
	int offset;


	oldVtxa = objC->vtx[0];
	oldVtxb = objC->vtx[1];

	vtxa = (Vtx *)JallocAlloc(sizeof(Vtx) * objC->numVtx * 2, NO, "unqVtx");
	vtxb = vtxa + objC->numVtx;

	memcpy(vtxa, oldVtxa, sizeof(Vtx) * objC->numVtx);
	memcpy(vtxb, oldVtxb, sizeof(Vtx) * objC->numVtx);


	//controller now references new vtx's, must also make sure that drawlist is updated
	objC->vtx[0] = vtxa;
	objC->vtx[1] = vtxb;


	
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void MakeUniqueDrawlist(OBJECT_CONTROLLER *objC)
{
	Gfx *newDl;
	int size;

	size = CalculateSizeOfDrawlist(objC->drawList);
	
	newDl = (Gfx *)JallocAlloc(size * sizeof(Gfx), NO, "uniqDl");

	CopyDrawlist((u8 *)newDl, (u8 *)objC->drawList);
	objC->drawList = newDl;
}

/*	--------------------------------------------------------------------------------
	Function		: SetActorCollisionRadius
	Purpose			: sets the specified actor's collision radius
	Parameters		: ACTOR2 *,float
	Returns			: void
	Info			: 
*/

OBJECT *MakeUniqueObject(OBJECT *object)
{

	OBJECT	*obj;	
	OBJECTSPRITE **spr, *tempSpr;
	int		i;
		
	obj = object;
	object = (OBJECT *)JallocAlloc(sizeof(OBJECT), YES, "UniqObj");
	memcpy(object, obj, sizeof(OBJECT));

	if(obj->numSprites)
	{
		spr = &object->sprites;
		tempSpr = (OBJECTSPRITE *)JallocAlloc(sizeof(OBJECTSPRITE) * obj->numSprites, YES, "UniqSpr");
		memcpy(tempSpr, *spr, sizeof(OBJECTSPRITE) * obj->numSprites);
		*spr = tempSpr;
/*		for(i = 0; i < obj->numSprites; i++)
		{
			spr = &object->sprites;
			spr += i;
			tempSpr = (OBJECTSPRITE *)JallocAlloc(sizeof(OBJECTSPRITE), YES, "UniqSpr");
			memcpy(tempSpr, *spr, sizeof(OBJECTSPRITE));
			*spr = tempSpr;
		}
 */
	}


	if(object->children)
		object->children = MakeUniqueObject(object->children);

	if(object->next)
		object->next = MakeUniqueObject(object->next);
		
	return object;


/*	OBJECT	*obj;	
	OBJECTSPRITE **spr, *tempSpr;
	int		i;
		
	obj = object;
	object = (OBJECT *)JallocAlloc(sizeof(OBJECT), YES, "UniqObj");
	memcpy(object, obj, sizeof(OBJECT));

/*	if(obj->numSprites)
	{
		spr = &object->sprites;
		tempSpr = (OBJECTSPRITE *)JallocAlloc(sizeof(OBJECTSPRITE) * obj->numSprites, YES, "UniqSpr");
		memcpy(tempSpr, *spr, sizeof(OBJECTSPRITE) * obj->numSprites);
		*spr = tempSpr;
/*		for(i = 0; i < obj->numSprites; i++)
		{
			spr = &object->sprites;
			spr += i;
			tempSpr = (OBJECTSPRITE *)JallocAlloc(sizeof(OBJECTSPRITE), YES, "UniqSpr");
			memcpy(tempSpr, *spr, sizeof(OBJECTSPRITE));
			*spr = tempSpr;
		}
 */
/*	}*/


/*	if(object->children)
		object->children = MakeUniqueObject(object->children);

	if(object->next)
		object->next = MakeUniqueObject(object->next);
		
	return object;*/ 
}


/*	--------------------------------------------------------------------------------
	Function 	: MakeUniqueActor
	Purpose 	: Makes an actor unique by giving it its own object controller, and calls
        		  make unique object, to sort out the object (including children)
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void MakeUniqueActor(ACTOR *actor,int type)
{

/*	OBJECT_CONTROLLER	*objCont;

	objCont = actor->objectController;
	actor->objectController = (OBJECT_CONTROLLER *)JallocAlloc(sizeof(OBJECT_CONTROLLER), YES, "UniqObjC");
	memcpy(actor->objectController, objCont, sizeof(OBJECT_CONTROLLER));
	actor->objectController->object = MakeUniqueObject(actor->objectController->object);

	//if actor is skinned, duplicate Vtx's
	if(actor->objectController->drawList)
	{
		MakeUniqueDrawlist(actor->objectController);
		MakeUniqueVtx(actor->objectController);
		XformActor(actor);
		SwapVtxReferencesInDrawlist(actor->objectController);
	}		  */

	OBJECT_CONTROLLER	*objCont;
	short	unique = TRUE;
	short	i;
	int		CRC = UpdateCRC(actor->objectController->object->name);

	//check all crc's to see if actor is among them
	for(i = 0; i < numUniqueActors; i++)
	{
		if(uniqueActorCRC[i] == CRC)
		{
			//if it is, actor is not unique and must have seperate stuff
			dprintf"found duplicate actor %s\n", actor->objectController->object->name));
			unique = FALSE;
			break;
		}
	}
	//if actor is not in list
	if(unique == TRUE)
	{
		uniqueActorCRC[numUniqueActors++] = CRC;
	}	

	objCont = actor->objectController;
	actor->objectController = (OBJECT_CONTROLLER *)JallocAlloc(sizeof(OBJECT_CONTROLLER), YES, "UniqObjC");
	memcpy(actor->objectController, objCont, sizeof(OBJECT_CONTROLLER));
	actor->objectController->object = MakeUniqueObject(actor->objectController->object);

	if(unique == FALSE)
	{
		//if actor is skinned, duplicate Vtx's
		if(actor->objectController->drawList)
		{
			MakeUniqueVtx(actor->objectController);
			XformActor(actor);
		}
	}

/*	objCont = actor->LODObjectController;
	if(objCont)
	{
		actor->LODObjectController = (OBJECT_CONTROLLER *)JallocAlloc(sizeof(OBJECT_CONTROLLER), YES, "UniqObjC");
		memcpy(actor->LODObjectController, objCont, sizeof(OBJECT_CONTROLLER));
		actor->LODObjectController->object = MakeUniqueObject(actor->LODObjectController->object);
	} */

}


