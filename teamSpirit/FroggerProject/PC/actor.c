/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: actor.c
	Programmer	: Matthew Cloy
	Date		: 11/11/98

----------------------------------------------------------------------------------------------- */


#define F3DEX_GBI_2

//#include "ultra64.h"
//#include <assert.h>
//#include <stdio.h>

//#ifdef PC_VERSION
//	#include "mdxDDraw.h"
//	#include "mdxD3D.h"
//	#include "mavis.h"
//#endif
//#include "crc32.h"

#include "actor.h"
#include "frogger.h"
#include "block.h"
#include "game.h"

#include "Types.h"

#include "maths.h"
//#include "subcube.h"
#include "cam.h"
//#include "jalloc.h"
#include "sprite.h"
#include <islmem.h>


#include <islutil.h>


//#define MAX_UNIQUE_ACTORS	50

//unsigned long ACTOR_DRAWDISTANCEINNER = (250000 * SCALE * SCALE);
//unsigned long ACTOR_DRAWDISTANCEOUTER = (500000 * SCALE * SCALE);
//bb
//unsigned long BBACTOR_DRAWDISTANCEINNER = sqrtf(250000)*SCALE;
//unsigned long BBACTOR_DRAWDISTANCEOUTER = sqrtf(500000)*SCALE;
//bb
//unsigned long BBACTOR_DRAWDISTANCEINNER = sqrt(250000)*SCALE;
//unsigned long BBACTOR_DRAWDISTANCEOUTER = sqrt(500000)*SCALE;
//unsigned long BBACTOR_DRAWDISTANCEINNER = 5000;
//unsigned long BBACTOR_DRAWDISTANCEOUTER = 5000;

 
//long waterObject = 0;
//long modgyObject = 0;
//int objectMatrix = 0;

//ACTOR2 *actList = NULL;				// entire actor list
//ACTOR2 *backGnd = NULL;
//ACTOR2 *globalLevelActor = NULL;	// ptr to actor representing level

//used to keep a count of how many of each enemy are present at the same time
//char uniqueEnemyCount[20];

//int uniqueActorCRC[MAX_UNIQUE_ACTORS];
//char numUniqueActors = 0;
//extern ACTOR2 *hat[MAX_FROGS];

/* --------------------------------------------------------------------------------	
	Programmer	: Matthew Cloy
	Function    : DrawActorList

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/
//void XformActor(ACTOR *ptr);
//bb
//fixed texSlideSpeed =ToFixed(40);
//fixed texSlideSpeed =163840;

// COMMENT:

//void SlideObjectTextures(PSIOBJECT *obj)
//{
// 	int i;
// 	for (i=0; i<obj->mesh->numFaces*3; i++)
// 	{
// 		obj->mesh->faceTC[i].vy -= FMul(gameSpeed, texSlideSpeed)>>12;
// 	}
//}

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
		if (gameState.mode == INGAME_MODE)
		{
			// calculate the distance between the camera and this actor
			cur->distanceFromFrog = DistanceBetweenPointsSS(&cur->actor->position,&frog[0]->actor->position);

			// transform actor

		// COMMENT:

		//		if(cur->distanceFromFrog < ToFixed(BBACTOR_DRAWDISTANCEOUTER) || cur->flags & ACTOR_DRAW_ALWAYS)

//				XformActor(cur->actor);
		}
		else
		{
			// transform actor
//			XformActor(cur->actor);
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
//mm fixed bFOV = ToFixed(450.0);
fixed bFOV = 1843200;

//extern fixed fStart, fEnd;
extern long FOV;
//extern long noClipping;

/*void DrawActorList()
{
	
	ACTOR2 *cur;
//bb
//	fixed ACTOR_DRAWFADERANGE = 0;//ToFixed(sqrtf(BBACTOR_DRAWDISTANCEOUTER - BBACTOR_DRAWDISTANCEINNER));
	fixed ACTOR_DRAWFADERANGE = Fsqrt(BBACTOR_DRAWDISTANCEOUTER - BBACTOR_DRAWDISTANCEINNER);
	
//	BlankFrame(_);

	if (backGnd)
	{

		// COMMENT:
//		fixed oFs = fStart, oFe = fEnd;
		
		waterObject = 0;
		modgyObject = 0;

//mm		fStart = ToFixed(7000.0*SCALE);
//		fStart = 28672000*SCALE;
//mm		fEnd = ToFixed(7001.0*SCALE);
//		fEnd = 28676096*SCALE;

		backGnd->actor->visible = 1;
		
//		noClipping = 1;

		SetVectorSF(&(backGnd->actor->position), &(currCamSource[0]));
//		XformActor(backGnd->actor);
		DrawActor(backGnd->actor);

	//	noClipping = 0;

//		fStart = oFs;
//		fEnd = oFe;

	}
	cur = actList;
	waterObject = 0;

//	BlankFrame(_);

	while(cur)
	{
		fixed slideSpeed = ToFixed(0);


		waterObject = 0;
		modgyObject = 0;


// COMMENT:

/* 		if ((cur->flags & ACTOR_SLIDYTEX))
 		{
 			if ((cur->flags & ACTOR_SLIDYTEX2))
 				slideSpeed = ToFixed(64);
 			else
 				slideSpeed = ToFixed(16);
 		}
 		else if((cur->flags & ACTOR_SLIDYTEX2))
 			slideSpeed = ToFixed(32);*/
 
 	
/* 		if (slideSpeed>ToFixed(1))
 		{
 			texSlideSpeed = slideSpeed;
// 			if (cur->actor->objectController)
// 				SlideObjectTextures(cur->actor->objectController->object);
 		}
 
// 		if(((cur->flags & ACTOR_WATER)) || (!cur->actor->objectController))
// 		{
// 			cur = cur->next;
// 			continue;
// 		}
		
// COMMENT:
/*		if (cur->flags & ACTOR_MODGETEX)
			modgyObject = 1;
		else
			modgyObject = 0;*/

/*		if( (cur->flags & ACTOR_DRAW_CULLED) &&
			(cur->distanceFromFrog > ToFixed(BBACTOR_DRAWDISTANCEINNER)) &&
			!(cur->flags & ACTOR_DRAW_ALWAYS) )
		{
			if( cur->distanceFromFrog < ToFixed(BBACTOR_DRAWDISTANCEOUTER) )
			{
// //mm					fixed fadeValue =  ToFixed(1.0-(sqrtf(FDiv
// //mm						((((cur->distanceFromFrog - ToFixed(BBACTOR_DRAWDISTANCEINNER)))),ACTOR_DRAWFADERANGE)/4096.0F)));
// //bb
// /*
// 					fixed fadeValue = 4096-(
// 												(sqrtf
// 													(FDiv
// 														(
// 															(
// 																(
// 																	(cur->distanceFromFrog - ToFixed(BBACTOR_DRAWDISTANCEINNER))
// 																)
// 															),ACTOR_DRAWFADERANGE
// 														)
// 													)
// 												)/64
// 											);
// */
/* 					fixed fadeValue = 4096-(
												FsqrtF
 												(
 													FDiv
 													(
 														(
 															(
 																(cur->distanceFromFrog - ToFixed(BBACTOR_DRAWDISTANCEINNER))
 															)
 														)
 														,ACTOR_DRAWFADERANGE
 													)
 												)
 											);
 
 
 
// 				cur->actor->objectController->object->flags |= OBJECT_FLAGS_XLU;//mm make a new flag variable

					// COMMENT:
 	//			cur->actor->xluOverride = (fadeValue*100)>>12;				
			}
			else
			{
// 				cur->actor->objectController->object->flags &= ~OBJECT_FLAGS_XLU;//mm make a new flag variable
			}
		}
		else
		{
			if( cur->flags & ACTOR_DRAW_CULLED )
			{
// 				cur->actor->xluOverride = 100;
// 				cur->actor->objectController->object->flags &= ~OBJECT_FLAGS_XLU;//mm make a new flag variable
			}

			if(gameState.mode == INGAME_MODE || gameState.mode == OBJVIEW_MODE || 
			   gameState.mode == RECORDKEY_MODE || gameState.mode == LEVELPLAYING_MODE ||
			   gameState.mode == FRONTEND_MODE  || gameState.mode == CAMEO_MODE || gameState.mode == PAUSE_MODE )
			{
				if( cur->draw )
				{
// 					if( !(cur->actor->objectController->object->flags & OBJECT_FLAGS_XLU) )//mm make a new flag variable
// 					{
// //						DrawActor(cur->actor);
// 					}
				}
			}
		}
	
		cur = cur->next;
	}
	

	if (hat[0])
	{
//		XformActor(hat[0]->actor);
		DrawActor(hat[0]->actor);	
	}

	//DrawBatchedPolys();
	//BlankFrame();
	
	waterObject = 1;
	cur = actList;
	while(cur)
	{
// 		if((!(cur->flags & ACTOR_WATER)) || (!cur->actor->objectController))
// 		{
// 			cur = cur->next;
// 			continue;
// 		}

		if( gameState.mode == INGAME_MODE || gameState.mode == OBJVIEW_MODE || 
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
// 		if( ((cur->flags & ACTOR_WATER)) || (!cur->actor->objectController))
// 		{
// 			cur = cur->next;
// 			continue;
// 		}

// 		if( cur->actor->objectController->object->flags & OBJECT_FLAGS_XLU )
// //mm		if( !((cur->flags & ACTOR_DRAW_CULLED) && (cur->distanceFromFrog > BBACTOR_DRAWDISTANCEOUTER)) )
// 		if( !((cur->flags & ACTOR_DRAW_CULLED) && (cur->distanceFromFrog > ToFixed(BBACTOR_DRAWDISTANCEOUTER))) )
 		if( gameState.mode == INGAME_MODE || gameState.mode == OBJVIEW_MODE || 
 			gameState.mode == RECORDKEY_MODE || gameState.mode == LEVELPLAYING_MODE ||
 			gameState.mode == FRONTEND_MODE  || gameState.mode == CAMEO_MODE || gameState.mode == PAUSE_MODE )
 		{
 				
 	/*	if (cur->flags & ACTOR_MODGETEX)
 			modgyObject = 1;
 		else
 			modgyObject = 0;*/
 
// //			DrawActor(cur->actor);
/* 		}
		
		cur = cur->next;
	}	
}*/


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: FreeActorList

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

void FreeActor(ACTOR2 *c)
{
 	ACTOR2 *cur = c;
 
 	if (!cur)
 		return;
 
// 	if((cur->actor->objectController) && (cur->actor->objectController->object))
// 	{
// 	 	FreeObjectSprites(cur->actor->objectController->object);//mm sprites are dealt with in the PSIMODELCTRL structure
// 
// 		// NEW
// 		if(cur->actor->objectController->drawList)
// 		{
// 			MALLOC0((UBYTE **)&cur->actor->objectController->vtx[0]);
// 			MALLOC0((UBYTE **)&cur->actor->objectController->drawList);
// 		}
// 
// 		// NEW
// 		RemoveUniqueObject(cur->actor->objectController->object);
// 		MALLOC0((UBYTE **)&cur->actor->objectController);
// 	}
// 
// 	if(cur->actor->LODObjectController)
// 		FREE((UBYTE **)&cur->actor->LODObjectController);
// 
// 	if(cur->actor->matrix)
// 		FREE((UBYTE **)&cur->actor->matrix);
// 
// 	if(cur->actor->animation)
// 		FREE((UBYTE **)&cur->actor->animation);
// 
// 	if(cur->actor->shadow)
// 		FREE((UBYTE **)&cur->actor->shadow);
// 
// 	FREE((UBYTE**)&cur->actor);
// 	FREE((UBYTE**)&cur);
}


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: FreeActorList

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/
/*void FreeActorList()
{
	ACTOR2 *cur,*next;
	
//	dprintf"Freeing linked list : ACTOR2\n"));
	
	cur = actList;
	while (cur)
	{
		next = cur->next;
		FreeActor(cur);		
		cur = next;
	}
	actList = NULL;

	FreeActor(backGnd);		
	backGnd = NULL;

	FreeActor(hat[0]);		
	hat[0] = NULL;
}*/

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: CreateAndAddActor 
	Purpose		:
	Parameters	: 
	Returns		: ACTOR2 *
*/
/*ACTOR2 *CreateAndAddActor(char *name,fixed cx,fixed cy,fixed cz,int initFlags,fixed offset,int startNode)
{
	ACTOR2 *newItem;
	
	newItem			= (ACTOR2 *)MALLOC0(sizeof(ACTOR2));
	newItem->actor	= (ACTOR *)MALLOC0(sizeof(ACTOR));

//mm	InitActor(newItem->actor,name,cx/4096.0F,cy/4096.0F,cz/4096.0F,initFlags);
	InitActor(newItem->actor,name,cx>>12,cy>>12,cz>>12,initFlags);

 	MakeUniqueActor(newItem->actor,0);


	newItem->actor->oldPosition.vx	= cx>>12;
	newItem->actor->oldPosition.vy	= cy>>12;
	newItem->actor->oldPosition.vz	= cz>>12;


	newItem->flags = 0;
	newItem->draw	= 1;
//mm	newItem->radius	= ToFixed(0.0F);
	newItem->radius	= 0;
//mm	newItem->animSpeed = ToFixed(1.0F);
	newItem->animSpeed = 4096;
//mm	newItem->value1 = ToFixed(0.0F);
	newItem->value1 = 0;
	newItem->effects = 0;

	// add actor object sprites to sprite list
// 	if((newItem->actor->objectController) && (newItem->actor->objectController->object))
// 		AddObjectsSpritesToSpriteList(newItem->actor->objectController->object,0);

	if(name[0] != 'x' && name[1] != 'x')
		newItem->flags = ACTOR_DRAW_CULLED;
	else
		newItem->flags = ACTOR_DRAW_ALWAYS;

	if(name[0] != 'x' && name[1] != 'x')
		newItem->flags = ACTOR_DRAW_CULLED;
	else
	{
/*		if (name[3]=='g')
		{
			newItem->flags = ACTOR_DRAW_ALWAYS | ACTOR_MODGETEX;

#ifdef N64_VERSION
			// add support for modgy objects
			AddN64ModgyTexObjectResource(newItem->actor);
#endif
		}
		else
		{
			if (name[2]=='a')
			{
				newItem->flags = ACTOR_DRAW_ALWAYS | ACTOR_MODGETEX | ACTOR_SLIDYTEX;

#ifdef N64_VERSION
				// add support for modgy objects
				AddN64ModgyTexObjectResource(newItem->actor);
#endif
			}
			else
			{
				newItem->flags = ACTOR_DRAW_ALWAYS;
			}
		}*/
/*	}

	/*if (name[0] == 's')
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
					newItem->flags |= (ACTOR_SLIDYTEX | ACTOR_SLIDYTEX2);
					break;
			}
		}*/

//mm	newItem->speed				= ToFixed(18.0);
//mm	newItem->offset				= ToFixed(0.0);
//mm	newItem->distanceFromFrog	= ToFixed(0.0F);
/*	newItem->speed				= 73728;
	newItem->offset				= 0;
	newItem->distanceFromFrog	= 0;



	newItem->next = actList;
	newItem->prev = NULL;
	
	if (actList) actList->prev = newItem;
	actList = newItem;
	
	return newItem;
}*/


/*	--------------------------------------------------------------------------------
	Function		: AddObjectsSpritesToSpriteList
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
/*void AddObjectsSpritesToSpriteList(PSIOBJECT *obj,short flags)
{
// 	SPRITE *sprite;
// 	int i;
// 
// 	if(obj->numSprites > 0)
// 	{
// 		for(i=0; i<obj->numSprites; i++)
// 		{
// 			sprite = (SPRITE *)MALLOC(sizeof(SPRITE));
// 
// 			sprite->texture = obj->sprites[i].textureID;
// 			SetVectorSV(&sprite->pos,&zero);
// 
// 			if((obj->sprites[i].flags & SPRITE_DONE) == 0)
// 			{
// 				obj->sprites[i].flags |= SPRITE_DONE;
// 
// #ifdef N64_VERSION
// 				switch(sprite->texture->sx)
// 				{
// 					case 8:
// //						obj->sprites[i].sx /= 2;
// 						break;
// 					case 16:
// 						obj->sprites[i].sx *= 2;
// 						break;
// 					case 32:
// 						obj->sprites[i].sx *= 3;
// 						break;
// 				}
// 				switch(sprite->texture->sy)
// 				{
// 					case 8:
// //						obj->sprites[i].sy /= 2;
// 						break;
// 					case 16:
// 						obj->sprites[i].sy *= 2;
// 						break;
// 					case 32:
// 						obj->sprites[i].sy *= 3;
// 						break;
// 				}
// #endif
// 			}
// 
// 			sprite->r = sprite->g = sprite->b = 255;
// 			sprite->a = 128;
// 			if(obj->xlu < 255)
// 				sprite->flags = SPRITE_TRANSLUCENT;
// 			else
// 				sprite->flags = 0;
// 
// 			sprite->flags |= flags;
// 
// 			sprite->offsetX = -sprite->texture->sx / 2;
// 			sprite->offsetY = -sprite->texture->sy / 2;
// 
// 			AddSprite(sprite,NULL);
// 			obj->sprites[i].sprite = sprite;
// 		}
// 	}
// 
// 	if(obj->children)
// 		AddObjectsSpritesToSpriteList(obj->children,flags);
// 
// 	if(obj->next)
// 		AddObjectsSpritesToSpriteList(obj->next,flags);
}*/

/*	--------------------------------------------------------------------------------
	Function		: RemoveObjectSprites
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
/*void RemoveObjectSprites(PSIOBJECT *obj,BOOL f)
{
// 	int i;
// 
// 	if(obj->sprites)
// 	{
// 		for(i=0; i<obj->numSprites; i++)
// 		{
// 			if(obj->sprites[i].sprite)
// 				SubSprite(obj->sprites[i].sprite);
// 			if(f)
// 				FREE((UBYTE**)&obj->sprites[i].sprite);
// 		}
// 	}
// 
// 	if(obj->children)
// 		RemoveObjectSprites(obj->children, f);
// 	
// 	if(obj->next)
// 		RemoveObjectSprites(obj->next, f);
}*/

/*	--------------------------------------------------------------------------------
	Function		: FreeObjectSprites
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
/*void FreeObjectSprites(PSIOBJECT *obj)
{
// 	int i;
// 
// 	if(obj->sprites)
// 	{
// 		for(i=0; i<obj->numSprites; i++)
// 		{
// 			if(obj->sprites[i].sprite)
// 			{
// 				SubSprite(obj->sprites[i].sprite);
// 				FREE((UBYTE**)&obj->sprites[i].sprite);
// 			}
// 		}
// 	}
// 
// 	if(obj->children)
// 		FreeObjectSprites(obj->children);
// 	
// 	if(obj->next)
// 		FreeObjectSprites(obj->next);
}*/


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

// void MakeUniqueVtx(OBJECT_CONTROLLER *objC)
// {
// 	short i;
// 	Vtx *vtxa;
// 	Vtx *vtxb;
// 	Vtx *oldVtxa, *oldVtxb;
// 	int offset;
// 
// 	oldVtxa = objC->vtx[0];
// 	oldVtxb = objC->vtx[1];
// 
// 	vtxa = (Vtx *)MALLOC(sizeof(Vtx) * objC->numVtx * 2, NO, "unqVtx");
// 	vtxb = vtxa + objC->numVtx;
// 
// 	memcpy(vtxa, oldVtxa, sizeof(Vtx) * objC->numVtx);
// 	memcpy(vtxb, oldVtxb, sizeof(Vtx) * objC->numVtx);
// 
// 	//controller now references new vtx's, must also make sure that drawlist is updated
// 	objC->vtx[0] = vtxa;
// 	objC->vtx[1] = vtxb;
// }

/*	--------------------------------------------------------------------------------
	Function		: MakeUniqueObject
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

// PSIOBJECT *MakeUniqueObject(PSIOBJECT *object)
// {
// 	PSIOBJECT	*obj;	
// 	OBJECTSPRITE **spr, *tempSpr;
// 	int		i;
// 		
// 	obj = object;
// 	object = (PSIOBJECT *)MALLOC(sizeof(PSIOBJECT), YES, "UniqObj");
// 	memcpy(object, obj, sizeof(PSIOBJECT));
// 
// 	if(obj->numSprites)
// 	{
// 		spr = &object->sprites;
// 		tempSpr = (OBJECTSPRITE *)MALLOC(sizeof(OBJECTSPRITE) * obj->numSprites, YES, "UniqSpr");
// 		memcpy(tempSpr, *spr, sizeof(OBJECTSPRITE) * obj->numSprites);
// 		*spr = tempSpr;
// 		for(i = 0; i < obj->numSprites; i++)
// 		{
// 			spr = &object->sprites;
// 			spr += i;
// 			tempSpr = (OBJECTSPRITE *)MALLOC(sizeof(OBJECTSPRITE), YES, "UniqSpr");
// 			memcpy(tempSpr, *spr, sizeof(OBJECTSPRITE));
// 			*spr = tempSpr;
// 		}
//  
// 	}
// 
// 	if(object->children)
// 		object->children = MakeUniqueObject(object->children);
// 
// 	if(object->next)
// 		object->next = MakeUniqueObject(object->next);
// 		
// 	return object;
// }


/*	--------------------------------------------------------------------------------
	Function 	: MakeUniqueActor
	Purpose 	: Makes an actor unique by giving it its own object controller, and calls
        		  make unique object, to sort out the object (including children)
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
// void MakeUniqueActor(ACTOR *actor,int type)
// {
// 	OBJECT_CONTROLLER *objCont;
// 	short	unique = TRUE;
// 	short	i;
// 	int		CRC;
// 	
// 	if(!actor->objectController)
// 		return;
// 		
// //bb
// //	CRC = utilStr2CRC((char*)actor->objectController->object->name);
// 	CRC = utilStr2CRC((char*)actor->objectController->object->name);
// 
// 	//check all crc's to see if actor is among them
// 	for(i = 0; i < numUniqueActors; i++)
// 	{
// 		if(uniqueActorCRC[i] == CRC)
// 		{
// 			//if it is, actor is not unique and must have seperate stuff
// 			//dprintf"found duplicate actor %s\n", actor->objectController->object->name));
// 			unique = FALSE;
// 			break;
// 		}
// 	}
// 	//if actor is not in list
// 	if(unique == TRUE)
// 	{
// 		uniqueActorCRC[numUniqueActors++] = CRC;
// 	}	
// 
// 	objCont = actor->objectController;
// 	actor->objectController = (OBJECT_CONTROLLER *)MALLOC(sizeof(OBJECT_CONTROLLER), YES, "UniqObjC");
// 	memcpy(actor->objectController, objCont, sizeof(OBJECT_CONTROLLER));
// 	actor->objectController->object = MakeUniqueObject(actor->objectController->object);
// 
// 	if(unique == FALSE)
// 	{
// 		//if actor is skinned, duplicate Vtx's
// 		if(actor->objectController->drawList)
// 		{
// 			MakeUniqueVtx(actor->objectController);
// //			XformActor(actor);
// 		}
// 
// 		// if actor is a water object, duplicate Vtx's
// 		if(type == ACTOR_WATER)
// 		{
// //			dprintf"Adding water object...\n"));
// 			MakeUniqueVtx(actor->objectController);
// //			XformActor(actor);
// 		}
// 	}
// 
// 	objCont = actor->LODObjectController;
// 	if(objCont)
// 	{
// 		actor->LODObjectController = (OBJECT_CONTROLLER *)MALLOC(sizeof(OBJECT_CONTROLLER), YES, "UniqObjC");
// 		memcpy(actor->LODObjectController, objCont, sizeof(OBJECT_CONTROLLER));
// 		actor->LODObjectController->object = MakeUniqueObject(actor->LODObjectController->object);
// 	}
// }*/


/*	--------------------------------------------------------------------------------
	Function 	: ResetUniqueActorList
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
/*void ResetUniqueActorList()
{
	int i;

	for(i=0; i<MAX_UNIQUE_ACTORS; i++)
		uniqueActorCRC[i] = 0;
}*/



/*	--------------------------------------------------------------------------------
	Function 	: RemoveUniqueObject
	Purpose 	: Frees all the unique stuff from an object
	Parameters 	: PSIOBJECT *object
	Returns 	: PSIOBJECT *
	Info 		:
*/
// void RemoveUniqueObject(PSIOBJECT *object)
// {
// 	PSIOBJECT	*obj;	
// 	OBJECTSPRITE **spr;
// 	int		i;
// 		
// 	obj = object;
// 
// 	if(object->numSprites)
// 	{
// 		spr = &object->sprites;
// 		FREE((UBYTE**)spr);
// 	}
// 
// 	if(object->children)
// 		RemoveUniqueObject(object->children);
// 
// 	if(object->next)
// 		RemoveUniqueObject(object->next);
// 		
// 	FREE((UBYTE**)&obj);
// }

/*	--------------------------------------------------------------------------------
	Function 	: RemoveUniqueActor
	Purpose 	: removes unique actors
	Parameters 	: ACTOR *,int
	Returns 	: void
	Info 		:
*/
/*void ActorLookAt( ACTOR *act, SVECTOR *at, long flags )
{
// 	FVECTOR forward;
// 	SVECTOR dir;
// 	QUATERNION q,q2,q3;
// 	fixed a,b;
// 	
// 	SubVectorSSS(&dir, &act->pos, at);
// 
// 	SetVectorFS(&forward, &dir);
// 	forward.vy = ToFixed(0);
// 	MakeUnit(&forward);
// 	a = -DotProduct(&forward,&inVec);
// 	q.x = 0;
// 	q.y = 1;
// 	q.z = 0;
// 	if (forward.vx<ToFixed(0))
// 		q.w = acos(a/4096.0F);
// 	else
// 		q.w = -acos(a/4096.0F);
// 
// 	if( flags == LOOKAT_ANYWHERE )
// 	{
// 		GetQuaternionFromRotation(&q3,&q);
// 
// 		SetVectorFS(&forward, &dir);
// 		b = Magnitude2DF(&forward);
// 		forward.vx = ToFixed(0);
// 		forward.vz = (fixed)b;
// 		MakeUnit(&forward);
// 
// 		a = DotProduct(&forward,&upVec);
// 		q.x = 1;
// 		q.y = 0;
// 		q.z = 0;
// 		if (forward.vy<ToFixed(0))
// 			q.w = (acos(a/4096.0F)-PI_OVER_2);
// 		else
// 			q.w = (PI_OVER_2-acos(a/4096.0F));
// 		
// 		GetQuaternionFromRotation(&q2,&q);
// 
// 		QuaternionMultiply (&act->qRot,&q3,&q2);
// 	}
// 	else
// 	{
// 		GetQuaternionFromRotation(&act->qRot,&q);
// 	}
}*/


/*void Orientate(QUATERNION *me, FVECTOR *fd, FVECTOR *mfd, FVECTOR *up)
{
	FVECTOR dirn;
	QUATERNION rotn,q;
	fixed dp,m;
	
	CalculateQuatForPlane2( ToFixed(0), me, up);
	RotateVectorByQuaternionFF(&dirn, mfd, me);
	dp = DotProduct( fd, &dirn );
	CrossProductFlFF((FLVECTOR *)&rotn, &dirn, fd);
//mm	if(dp > -ToFixed(0.99))
	if(dp > -4055)
	{
		m = ToFixed(fl_Magnitude( (FLVECTOR *)&rotn ));
//mm		if(m > ToFixed(0.0003))
		if(m > 1)
		{
			ScaleVector( (FLVECTOR *)&rotn, 1.0F/(m/4096.0F) );

//mm			if (dp<ToFixed(0.99))				
			if (dp<4055)				
				rotn.w = acos(dp/4096.0F);
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
}*/


/*void SitAndFace(ACTOR2 *me, GAMETILE *tile, long fFacing)
{
	MATRIXI tmpMtx;

	IQUATERNION qRot;

   	FVECTOR dirn2;
   	IQUATERNION rotn,q;
   	fixed dp,m;

   	FVECTOR fwdVec = {0,0,4096};
   

//	FntPrint(FntStream,"FFACING %d\n",fFacing);


   	
   	CalculateQuatForPlane2(0,&qRot,&tile->normal);
// 	FVECTOR invNorm = tile->normal;
// 	invNorm.vy = -invNorm.vy;
//    	CalculateQuatForPlane2(0,&qRot,&invNorm);



//   	fixedRotateVectorByQuaternionFFQ(&dirn2,&fwdVec,&qRot);
   	fixedRotateVectorByRotation(&dirn2,&fwdVec,&qRot);
   	dp = DotProductFF(&tile->dirVector[fFacing],&dirn2);
   	CrossProductFFF((FVECTOR *)&rotn,&dirn2,&tile->dirVector[fFacing]);
   	if(dp > -4055)
   	{
   		m = MagnitudeF((FVECTOR *)&rotn);
   
   		if(m > 1) //smallest fixed
   		{
//   			ScaleVector((FVECTOR *)&rotn,1/(m/4096.0f));
   			ScaleVector((FVECTOR *)&rotn, 4096/m);//bbxx
   
//   			rotn.w = (float)acos(dp/4096.0f);
   			rotn.w = arccos(dp);
   
   			fixedGetQuaternionFromRotation(&q,&rotn);
   			fixedQuaternionMultiply(&qRot,&q,&qRot);
   		}
   	}
   	else
   	{
   		vertQ.w = 2048;
   		fixedGetQuaternionFromRotation(&q,&vertQ);
   		fixedQuaternionMultiply(&qRot,&qRot,&q);
   	}

//	fixedGetVectorFromQuaternion(&me->actor->psiData.object->rotate, &qRot);
//	fixedQuaternionToMatrix(&qRot, &tmpMtx)
//	fixedMatrixToEuler(&tmpMtx, &me->actor->psiData.object->rotate);




	Eul_FromQuat(&me->actor->psiData.object->rotate, &qRot);

//	me->actor->psiData.object->rotate.vz += 1024;
// 	FVECTOR fwdVec = MakeFVECTOR(ToFixed(0),ToFixed(0),ToFixed(1));
// 	FVECTOR dirn2;
// 	QUATERNION rotn,q;
// 	fixed frogMatrix[4][4];
// 	fixed frogMatrix2[4][4];
// 	fixed dp,m;
// 	
// 	CalculateQuatForPlane2(ToFixed(0),&me->actor->qRot,&tile->normal);
// 	RotateVectorByQuaternionFF(&dirn2, &fwdVec, &me->actor->qRot);
// 	dp = DotProduct(&tile->dirVector[fFacing],&dirn2);
// 	CrossProductFlFF((FLVECTOR *)&rotn, &dirn2, &tile->dirVector[fFacing]);
// //mm if dp > -ToFixed(0.99))
// 	if (dp > -4055)
// 	{
// 		m = ToFixed(fl_Magnitude((FLVECTOR *)&rotn));
// //mm		if(m > ToFixed(0.0003))
// 		if(m > 1)
// 		{
// 
// 			ScaleVector((FLVECTOR *)&rotn,1.0F/(m/4096.0F));
// 			rotn.w = acos(dp/4096.0F);
// 			GetQuaternionFromRotation(&q,&rotn);
// 			QuaternionMultiply(&me->actor->qRot,&q,&me->actor->qRot);
// 		}
// 	}
// 	else
// 	{
// 		vertQ.w = PI;
// 		GetQuaternionFromRotation(&q,&vertQ);
// 		QuaternionMultiply(&me->actor->qRot,&me->actor->qRot,&q);
// 	}
}*/






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
				FREE((UBYTE **)&actor->objectController->Vtx[0]);
				FREE((UBYTE **)&actor->objectController->drawList);
			}


			RemoveUniqueObject(actor->objectController->object);
			FREE((UBYTE **)&actor->objectController);
		
			if((actor->LODObjectController) && (actor->LODObjectController->object))
			{
				RemoveUniqueObject(actor->LODObjectController->object);
				FREE((UBYTE **)&actor->LODObjectController);
			}
		}
	}
}
*/


/*void SubActor(ACTOR2 *actor)
{
	if (!actor) return;

	if (actor->next)
		actor->next->prev = actor->prev;

	if (actor->prev)
		actor->prev->next = actor->next;
	else
		actList = actor->next;

	if(actor->actor)
	{
 		if(actor->actor->animation)
 		{
 			// free any animation associated with ACTOR type
 			FREE(actor->actor->animation);
 		}

// 		if(actor->actor->shadow)
// 		{
// 			// free any shadow associated with ACTOR type
// 			FREE((UBYTE**)&actor->actor->shadow);
// 		}

// 		if((actor->actor->objectController) && (actor->actor->objectController->object))
// 		{
// 			// free any object sprites for this actor
// 			FreeObjectSprites(actor->actor->objectController->object);
// 		}

 		// free associated ACTOR type
 		FREE(actor->actor);
	}

	FREE(actor);
}*/


