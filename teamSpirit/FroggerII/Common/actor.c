/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: actor.c
	Programmer	: Matthew Cloy
	Date		: 11/11/98

----------------------------------------------------------------------------------------------- */


#define F3DEX_GBI_2

#include <ultra64.h>

#ifndef PC_VERSION
#include <PR/ramrom.h>					// Needed for argument passing into the app
#include <PR/gbi.h>
#include <PR/os.h>
#endif

#include <assert.h>

#include "incs.h"


#define MAX_UNIQUE_ACTORS	50

#ifdef PC_VERSION

float ACTOR_DRAWDISTANCEINNER = 80000.0F;
float ACTOR_DRAWDISTANCEOUTER = 150000.0F;
float ACTOR_DRAWDISTANCESTART = 300000.0F;

#else

float ACTOR_DRAWDISTANCEINNER = 100000.0F;
float ACTOR_DRAWDISTANCEOUTER = 125000.0F;

#endif

#define WATER_XLU 70
long waterObject = 0;
int objectMatrix = 0;

ACTOR2 *actList = NULL;				// entire actor list
ACTOR2 *globalLevelActor = NULL;	// ptr to actor representing level

//used to keep a count of how many of each enemy are present at the same time
//char uniqueEnemyCount[20];

int uniqueActorCRC[MAX_UNIQUE_ACTORS];
char numUniqueActors = 0;

/* --------------------------------------------------------------------------------	
	Programmer	: Matthew Cloy
	Function    : DrawActorList

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/
void XformActor(ACTOR *ptr);
float texSlideSpeed = 40;
void SlideObjectTextures(OBJECT *obj)
{
	int i;
	for (i=0; i<obj->mesh->numFaces*3; i++)
	{
		obj->mesh->faceTC[i].v[Y] -= (gameSpeed * texSlideSpeed);
	}
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void XformActorList()
{
	ACTOR2 *cur;
	
	cur = actList;
	while(cur)
	{
		if (frontEndState.mode != OBJVIEW_MODE)
		{
			// calculate the distance between the camera and this actor
			cur->distanceFromFrog = DistanceBetweenPointsSquared(&cur->actor->pos,&frog[0]->actor->pos);

			// transform actor
//			if(cur->distanceFromFrog < ACTOR_DRAWDISTANCEINNER)
			if(cur->distanceFromFrog < ACTOR_DRAWDISTANCEOUTER || cur->flags & ACTOR_DRAW_ALWAYS)
				XformActor(cur->actor);

//			if(cur->flags & ACTOR_DRAW_ALWAYS)
//			{
				// always draw this actor
//				cur->draw = 1;
//			}
		}
		else
		{
			// transform actor
			XformActor(cur->actor);
		
//			cur->draw = 0;
//			if(cur->flags & ACTOR_DRAW_ALWAYS)
//			{
				// always draw this actor
//				cur->draw = 1;
//			}
		}


		cur = cur->next;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: DrawActorList
	Purpose			: draws the actors in the actor list....doh !!
	Parameters		: 
	Returns			: void
	Info			: 
*/
void DrawActorList()
{
	/****************************************************************************************/
	// IMPORTANT NOTE FROM SHARKY...
	// I HAVE USED #ifdef's TO SPLIT UP THE DRAWING OF ACTORS BASED ON THE HARDWARE BEING
	//	USED...I.E. PC OR N64...
	//	THIS IS BECAUSE IT'S BEING A PAIN IN THE ARSE ON THE N64 AND I'M GONNA KICK IT'S
	//	COCK OFF AND FIX IT AS SOON AS I CAN.
	/****************************************************************************************/
	
	ACTOR2 *cur;
//	float ACTOR_DRAWFADERANGE = sqrtf(ACTOR_DRAWDISTANCEOUTER) - sqrtf(ACTOR_DRAWDISTANCEINNER);
	float ACTOR_DRAWFADERANGE = sqrtf(ACTOR_DRAWDISTANCEOUTER - ACTOR_DRAWDISTANCEINNER);

	/****************************************************************************************/
	/* PC SPECIFIC VERSION																	*/
	/****************************************************************************************/

#ifdef PC_VERSION
	
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);

	cur = actList;
	waterObject = 0;
	while(cur)
	{
		float slideSpeed = 0;

		if ((cur->flags & ACTOR_SLIDYTEX))
		{
			if ((cur->flags & ACTOR_SLIDYTEX2))
				slideSpeed = 64;
			else
				slideSpeed = 16;
		}
		else
			if ((cur->flags & ACTOR_SLIDYTEX2))
				slideSpeed = 32;

	
		if (slideSpeed>1)
		{
			texSlideSpeed = slideSpeed;
			if (cur->actor->objectController)
				SlideObjectTextures(cur->actor->objectController->object);
		}

		if(((cur->flags & ACTOR_WATER)) || (!cur->actor->objectController))
		{
			cur = cur->next;
			continue;
		}
		
		if((cur->flags & ACTOR_DRAW_CULLED) && (cur->distanceFromFrog > ACTOR_DRAWDISTANCEINNER) && !(cur->flags & ACTOR_DRAW_ALWAYS) )
		{
			if( cur->distanceFromFrog < ACTOR_DRAWDISTANCEOUTER )
			{
				float fadeValue =  1.0-(sqrtf(((float)(cur->distanceFromFrog - ACTOR_DRAWDISTANCEINNER)))/ACTOR_DRAWFADERANGE);

				cur->actor->objectController->object->flags |= OBJECT_FLAGS_XLU;
				cur->actor->xluOverride = 95;//fadeValue*100;				
			}
			else
			{
				cur->actor->objectController->object->flags &= ~OBJECT_FLAGS_XLU;
			}
		}
		else
		{
			if( cur->flags & ACTOR_DRAW_CULLED )
			{
				cur->actor->xluOverride = 100;
				cur->actor->objectController->object->flags &= ~OBJECT_FLAGS_XLU;
			}

			if(gameState.mode == GAME_MODE || gameState.mode == OBJVIEW_MODE || 
			   gameState.mode == RECORDKEY_MODE || gameState.mode == LEVELPLAYING_MODE ||
			   gameState.mode == FRONTEND_MODE  || gameState.mode == CAMEO_MODE || gameState.mode == PAUSE_MODE )
			{
				if( cur->draw )
				if( !(cur->actor->objectController->object->flags & OBJECT_FLAGS_XLU) )
				{
					DrawActor(cur->actor);
				}
			}
		}
	
		cur = cur->next;
	}
	
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);

	waterObject = 1;
	cur = actList;
	while(cur)
	{
		if((!(cur->flags & ACTOR_WATER)) || (!cur->actor->objectController))
		{
			cur = cur->next;
			continue;
		}

		if( gameState.mode == GAME_MODE || gameState.mode == OBJVIEW_MODE || 
			gameState.mode == RECORDKEY_MODE || gameState.mode == LEVELPLAYING_MODE ||
			gameState.mode == FRONTEND_MODE  || gameState.mode == CAMEO_MODE || gameState.mode == PAUSE_MODE )
		{
			DrawActor(cur->actor);
		}
		
		cur = cur->next;
	}

	waterObject = 0;
	cur = actList;
	while(cur)
	{
		if( ((cur->flags & ACTOR_WATER)) || (!cur->actor->objectController))
		{
			cur = cur->next;
			continue;
		}

		if( cur->actor->objectController->object->flags & OBJECT_FLAGS_XLU )
		if( !((cur->flags & ACTOR_DRAW_CULLED) && (cur->distanceFromFrog > ACTOR_DRAWDISTANCEOUTER)) )
		if( gameState.mode == GAME_MODE || gameState.mode == OBJVIEW_MODE || 
			gameState.mode == RECORDKEY_MODE || gameState.mode == LEVELPLAYING_MODE ||
			gameState.mode == FRONTEND_MODE  || gameState.mode == CAMEO_MODE || gameState.mode == PAUSE_MODE )
		{
			DrawActor(cur->actor);
		}
		
		cur = cur->next;
	}

#endif



	/****************************************************************************************/
	/* N64 SPECIFIC VERSION																	*/
	/****************************************************************************************/
	
#ifdef N64_VERSION
	
	vtxPtr = &(objectsVtx[draw_buffer][0]);

	// fogging ?
	if(fog.mode)
	{
	   gDPSetFogColor(glistp++,fog.r,fog.g,fog.b,255);
	   gSPFogPosition(glistp++,fog.min,fog.max);
	}

	// draw non-xlu objects and calculate xlu values for actor fading
	cur = actList;
	waterObject = 0;
	while(cur)
	{
		if((cur->flags & ACTOR_DRAW_NEVER) || (cur->flags & ACTOR_WATER))
		{
			// do not display this object yet, if at all
			cur = cur->next;
			continue;
		}

		// slide texture coordinates here for water objects ??? AndyE
	
		if((!cur->actor->objectController) || (cur->flags & ACTOR_DRAW_LAST))
		{
			// don't draw object yet
			cur = cur->next;
			continue;
		}

		if(cur->flags & ACTOR_DRAW_ALWAYS)
		{
			// always draw this actor without "xlu-culling"
			DrawActor(cur->actor);
		}
		else
		{
			// check if distance from frog is outside inner draw distance radius
			if(cur->distanceFromFrog > ACTOR_DRAWDISTANCEINNER)
			{
				// do not draw for now...
			}
			else
			{
				// draw for now...
				DrawActor(cur->actor);
			}
		}

		cur = cur->next;
	}

	// go ahead and draw the xlu water objects
	waterObject = 1;
	cur = actList;
	while(cur)
	{
		// only draw xlu water objects
		if(cur->flags & ACTOR_WATER)
		{
			int i;
			OBJECT *obj = cur->actor->objectController->object;
			SetXluInDrawList(obj->drawList,WATER_XLU);
			DrawActor(cur->actor);
		}
		
		cur = cur->next;
	}

/*
	// now draw the xlu objects that aren't water objects
	waterObject = 0;
	cur = actList;
	while(cur)
	{
		// don't draw the water objects
		if((cur->flags & ACTOR_WATER) || !(cur->flags & ACTOR_DRAW_CULLED))
		{
			cur = cur->next;
			continue;
		}

		if(cur->actor->objectController->object->flags & OBJECT_FLAGS_XLU)
		{
			if(cur->distanceFromFrog < ACTOR_DRAWDISTANCEOUTER)
			{
				DrawActor(cur->actor);
			}
		}
		
		cur = cur->next;
	}
*/
	// now draw non-transformed actors
	ClearViewing();

	cur = actList;
	while(cur)
	{
		if(cur->flags & ACTOR_DRAW_LAST)
		{
			DrawActor(cur->actor);
		}

		cur = cur->next;
	}

#endif
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

	dprintf"Freeing linked list : ACTOR2\n"));
	cur = actList;
	while (cur)
	{
		next = cur->next;

		if((cur->actor->objectController) && (cur->actor->objectController->object))
		{
		 	FreeObjectSprites(cur->actor->objectController->object);

			// NEW
			if(cur->actor->objectController->drawList)
			{
				JallocFree((UBYTE **)&cur->actor->objectController->vtx[0]);
				JallocFree((UBYTE **)&cur->actor->objectController->drawList);
			}

			// NEW
			RemoveUniqueObject(cur->actor->objectController->object);
			JallocFree((UBYTE **)&cur->actor->objectController);
		}

		if(cur->actor->LODObjectController)
			JallocFree((UBYTE **)&cur->actor->LODObjectController);

		if(cur->actor->matrix)
			JallocFree((UBYTE **)&cur->actor->matrix);

		if(cur->actor->animation)
			JallocFree((UBYTE **)&cur->actor->animation);

		if(cur->actor->shadow)
			JallocFree((UBYTE **)&cur->actor->shadow);

		JallocFree((UBYTE**)&cur->actor);
		JallocFree((UBYTE**)&cur);
		
		cur = next;
	}
	
	actList = NULL;
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

	newItem			= (ACTOR2 *)JallocAlloc(sizeof(ACTOR2),YES,"ACTOR2");
	newItem->actor	= (ACTOR *)JallocAlloc(sizeof(ACTOR),YES,"ACTOR");

	InitActor(newItem->actor,name,cx,cy,cz,initFlags);
	MakeUniqueActor(newItem->actor,0);

	newItem->actor->oldpos.v[X]	= cx;
	newItem->actor->oldpos.v[Y]	= cy;
	newItem->actor->oldpos.v[Z]	= cz;

	newItem->flags = 0;
	newItem->draw	= 1;
	newItem->radius	= 0.0F;
	newItem->animSpeed = 1.0F;
	newItem->value1 = 0.0F;
	newItem->effects = 0;

	// add actor object sprites to sprite list
	if((newItem->actor->objectController) && (newItem->actor->objectController->object))
		AddObjectsSpritesToSpriteList(newItem->actor->objectController->object,0);

	if(name[0] != 'x' && name[1] != 'x')
		newItem->flags = ACTOR_DRAW_CULLED;
	else
		newItem->flags = ACTOR_DRAW_ALWAYS;

	if (name[0] == 's')
		if (name[1] == 'p')
		{
			switch (name[2])
			{
				case 'l':
					newItem->flags |= ACTOR_SLIDYTEX;
					break;
				case 'm':
					newItem->flags |= ACTOR_SLIDYTEX2;
					break;
				case 'f':
					newItem->flags |= ACTOR_SLIDYTEX & ACTOR_SLIDYTEX2;
					break;
			}
		}

	newItem->speed				= 18.0;
	newItem->offset				= 0.0;
	newItem->distanceFromFrog	= 0.0F;

	newItem->next = actList;
	newItem->prev = NULL;
	if (actList) actList->prev = newItem;
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
/*				switch(sprite->texture->sx)
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
*/			}

			sprite->r = sprite->g = sprite->b = 255;
			sprite->a = 128;
			if(obj->xlu < 255)
				sprite->flags = SPRITE_TRANSLUCENT;
			else
				sprite->flags = 0;

			sprite->flags |= flags;

			sprite->offsetX = -16; // -sprite->texture->sx / 2;
			sprite->offsetY = -16; // -sprite->texture->sy / 2;

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
void RemoveObjectSprites(OBJECT *obj,BOOL f)
{
	int i;

	if(obj->sprites)
	{
		for(i=0; i<obj->numSprites; i++)
		{
			if(obj->sprites[i].sprite)
				SubSprite(obj->sprites[i].sprite);
			if(f)
				JallocFree((UBYTE**)&obj->sprites[i].sprite);
		}
	}

	if(obj->children)
		RemoveObjectSprites(obj->children, f);
	
	if(obj->next)
		RemoveObjectSprites(obj->next, f);
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
	Function 	: MakeUniqueDrawList
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
	Function 	: CopyDrawList
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CopyDrawlist(u8 *dest, u8 *source)
{
	while(*source != 0xDF)
	{
		*(Gfx *)dest = *(Gfx *)source;
		dest+=8;
		source+=8;
	}
	//must copy last entry (end drawlist)
	*(Gfx *)dest = *(Gfx *)source;
}


/*	--------------------------------------------------------------------------------
	Function 	: CalculateSizeOfDrawList
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int CalculateSizeOfDrawlist(Gfx *dl)
{
	u8	*temp = (u8 *)dl; 
	int size = 0;

	while(*temp != 0xDF)
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
	Function		: MakeUniqueObject
	Purpose			: 
	Parameters		: 
	Returns			: 
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
	OBJECT_CONTROLLER	*objCont;
	short	unique = TRUE;
	short	i;
	int		CRC;
	
	if (!actor->objectController) return;
		
	CRC = UpdateCRC(actor->objectController->object->name);

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

	objCont = actor->LODObjectController;
	if(objCont)
	{
		actor->LODObjectController = (OBJECT_CONTROLLER *)JallocAlloc(sizeof(OBJECT_CONTROLLER), YES, "UniqObjC");
		memcpy(actor->LODObjectController, objCont, sizeof(OBJECT_CONTROLLER));
		actor->LODObjectController->object = MakeUniqueObject(actor->LODObjectController->object);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: ResetUniqueActorList
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void ResetUniqueActorList()
{
	int i;

	for(i=0; i<MAX_UNIQUE_ACTORS; i++)
		uniqueActorCRC[i] = 0;
}



/*	--------------------------------------------------------------------------------
	Function 	: RemoveUniqueObject
	Purpose 	: Frees all the unique stuff from an object
	Parameters 	: OBJECT *object
	Returns 	: OBJECT *
	Info 		:
*/
void RemoveUniqueObject(OBJECT *object)
{
	OBJECT	*obj;	
	OBJECTSPRITE **spr;
	int		i;
		
	obj = object;

	if(object->numSprites)
	{
		spr = &object->sprites;
		JallocFree((UBYTE**)spr);
	}

	if(object->children)
		RemoveUniqueObject(object->children);

	if(object->next)
		RemoveUniqueObject(object->next);
		
	JallocFree((UBYTE**)&obj);
}

/*	--------------------------------------------------------------------------------
	Function 	: RemoveUniqueActor
	Purpose 	: removes unique actors
	Parameters 	: ACTOR *,int
	Returns 	: void
	Info 		:
*/
void ActorLookAt( ACTOR *act, VECTOR *at, long flags )
{
	VECTOR forward, dir;
	QUATERNION q,q2,q3;
	float a,b;
	
	SubVector( &dir, &act->pos, at );

	SetVector( &forward, &dir );
	forward.v[Y] = 0;
	MakeUnit(&forward);
	a = -DotProduct(&forward,&inVec);
	q.x = 0;
	q.y = 1;
	q.z = 0;
	if (forward.v[X]<0)
		q.w = acos(a);
	else
		q.w = -acos(a);

	if( flags == LOOKAT_ANYWHERE )
	{
		GetQuaternionFromRotation(&q3,&q);

		SetVector( &forward, &dir );
		b = sqrtf(forward.v[X]*forward.v[X]+forward.v[Z]*forward.v[Z]);
		
		forward.v[X] = 0;
		forward.v[Z] = b;
		MakeUnit(&forward);

		a = DotProduct(&forward,&upVec);
		q.x = 1;
		q.y = 0;
		q.z = 0;
		if (forward.v[Y]<0)
			q.w = (acos(a)-PI_OVER_2);
		else
			q.w = (PI_OVER_2-acos(a));
		
		GetQuaternionFromRotation(&q2,&q);

		QuaternionMultiply (&act->qRot,&q3,&q2);
	}
	else
	{
		GetQuaternionFromRotation(&act->qRot,&q);
	}
}


void Orientate(QUATERNION *me, VECTOR *fd, VECTOR *mfd, VECTOR *up)
{
	VECTOR dirn;
	QUATERNION rotn,q;
	float dp,m;
	
	CalculateQuatForPlane2( 0, me, up);
	RotateVectorByQuaternion( &dirn, mfd, me);
	dp = DotProduct( fd, &dirn );
	CrossProduct( (VECTOR *)&rotn, &dirn, fd );
	if(dp > -0.99)
	{
		m = Magnitude( (VECTOR *)&rotn );
		if(m > 0.0001)
		{
			ScaleVector( (VECTOR *)&rotn, 1/m );

			if (dp<0.99)
				rotn.w = acos(dp);
			else
				rotn.w = 0;
			
			GetQuaternionFromRotation( &q, &rotn );
			QuaternionMultiply( me, &q, me );
		}
	}
	else
	{
		vertQ.w = PI;
		GetQuaternionFromRotation(&q,&vertQ);
		QuaternionMultiply(me,me,&q);
	}
}


void SitAndFace(ACTOR2 *me, GAMETILE *tile, long fFacing)
{
	VECTOR fwdVec = { 0,0,1 };
	VECTOR dirn2;
	QUATERNION rotn,q;
	float frogMatrix[4][4];
	float frogMatrix2[4][4];
	float dp,m;
	
	CalculateQuatForPlane2(0,&me->actor->qRot,&tile->normal);
	RotateVectorByQuaternion(&dirn2,&fwdVec,&me->actor->qRot);
	dp = DotProduct(&tile->dirVector[fFacing],&dirn2);
	CrossProduct((VECTOR *)&rotn,&dirn2,&tile->dirVector[fFacing]);
	if(dp > -0.99)
	{
		m = Magnitude((VECTOR *)&rotn);
		if(m > 0.0001)
		{
			ScaleVector((VECTOR *)&rotn,1/m);
			rotn.w = acos(dp);
			GetQuaternionFromRotation(&q,&rotn);
			QuaternionMultiply(&me->actor->qRot,&q,&me->actor->qRot);
		}
	}
	else
	{
		vertQ.w = PI;
		GetQuaternionFromRotation(&q,&vertQ);
		QuaternionMultiply(&me->actor->qRot,&me->actor->qRot,&q);
	}
}






/*

void RemoveUniqueActor(ACTOR *actor,int type)
{
	if((actor->objectController) && (actor->objectController->object))
	{
		if((type < 0) || ((type >= 0) && (type < CAMEO_ACTOR)))
		{
			SubActor(actor);
			FreeObjectSprites(actor->objectController->object);

			if(actor->objectController->drawList)
			{
				JallocFree((UBYTE **)&actor->objectController->Vtx[0]);
				JallocFree((UBYTE **)&actor->objectController->drawList);
			}


			RemoveUniqueObject(actor->objectController->object);
			JallocFree((UBYTE **)&actor->objectController);
		
			if((actor->LODObjectController) && (actor->LODObjectController->object))
			{
				RemoveUniqueObject(actor->LODObjectController->object);
				JallocFree((UBYTE **)&actor->LODObjectController);
			}
		}
	}
}
*/
