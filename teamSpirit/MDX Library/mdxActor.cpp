/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: mdxReport.cpp
	Programmer	: Matthew Cloy
	Date		: 

----------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <crtdbg.h>
#include <stdio.h>

#include "mdxDDraw.h"
#include "mdxD3D.h"
#include "mdxMath.h"
#include "mdxTexture.h"
#include "mdxObject.h"
#include "mdxActor.h"
#include "mdxCRC.h"
#include "mdxTiming.h"
#include "mgeReport.h"
#include "gelf.h"
#include "mdxLandscape.h"
#include "mdxRender.h"
#include "mdxPoly.h"
#include "mdxProfile.h"

#ifdef __cplusplus
extern "C"
{
#endif

MDX_VECTOR	parentScaleStack[10];
int		parentScaleStackLevel = 0;
MDX_VECTOR  *actorScale;

MDX_ACTOR *currentDrawActor;

MDX_ACTOR *actorList = NULL;

#define MAX_UNIQUE_ACTORS	100
int uniqueActorCRC[MAX_UNIQUE_ACTORS];
long numActorsUniqe = 0;
void (*StartAnim)(MDX_ACTOR *me) = NULL;


unsigned long AddActorToList(MDX_ACTOR *me)
{
	if (!me)
		return 0;

	if (actorList)
	{
		actorList->prev = me;
		me->prev = NULL;
		me->next = actorList;
		actorList = me;
	}
	else
	{
		me->prev = me->next = NULL;
		actorList = me;
	}

	return 1;
}

void ActorListDraw(void)
{
	MDX_ACTOR *cur = actorList;	
	MDX_VECTOR where;

	while (cur)
	{
		drawThisObjectsSprites = cur->draw;
//		XfmPoint(&where,&cur->pos,NULL);
	//f (((where.vx > -50) && (where.vx<640+50)) &&
	//((where.vy > -50) && (where.vy<480+50)))
		{	
			XformActor(cur);		
			if (cur->draw)
				DrawActor(cur);
		}

		cur = cur->next;
	}
}

void DrawActor(MDX_ACTOR *actor)
{
	// Optimisation.
	MDX_OBJECT_CONTROLLER *objectC = actor->objectController;


	// I can't see you...
	if(actor->visible == FALSE || !objectC)
		return;

	// xluOverride is used in the sprite engine, and xl is used within the main polygon engine.
	
	globalXLU = (float)actor->xluOverride/100.0F;

	// If we are skinned then XForm all the vertices first and then draw the skin, otherwise draw it as we transform it.
	overrideTex = actor->overrideTex;

	if (objectC->isSkinned)
	{
		globalXLU2 = (((float)objectC->object->xlu) / ((float)0xff)) * globalXLU;
		if (objectC->object->flags & OBJECT_FLAGS_CLIPPED)
			return;
	
		if (objectC->object->phong)
		{
			//phong = objectC->object->phong;
			DrawObject(objectC->object, 2, objectC->object->mesh);
			PCRenderObjectPhong(objectC->object);
		}
		else
		{
			DrawObject(objectC->object, TRUE, objectC->object->mesh);
			PCRenderObject(objectC->object);
//			DrawObject(objectC->object, 3, objectC->object->mesh);
//			PCRenderObjectOutline(objectC->object);
		}
	}
	else
		DrawObject(objectC->object, FALSE, objectC->object->mesh);		
}

void QueueFlush(MDX_ACTOR *myActor)
{
	int i;

	myActor->animation->numberQueued = 0;
	for(i = 0;i < MDX_ANIM_QUEUE_LENGTH;i++)
		myActor->animation->queueAnimation[i] = -1;
}

void Animate(MDX_ACTOR *actor, int animNum, char loop, char queue, float speed)
{
	MDX_ACTOR_ANIMATION *actorAnim = actor->animation;
	MDX_ANIMATION *anim;
	float	actualSpeed;

	if(actorAnim == NULL)
		return;
	if(actorAnim->anims == NULL)
		return;


	if(animNum > actorAnim->numAnimations)
	{
		animNum = 0;
	}

	if(speed == 0)
		actualSpeed = actorAnim->anims[animNum].speed;
	else
		actualSpeed = speed;

	if(queue == 0)
	{
		actorAnim->currentAnimation = animNum;
		anim = actorAnim->anims + actorAnim->currentAnimation;
		actorAnim->loopAnimation = loop;
		actorAnim->animationSpeed = actualSpeed;
		actorAnim->reachedEndOfAnimation = FALSE;
		if(actualSpeed < 0)
			actorAnim->animTime = anim->animEnd;
		else
			actorAnim->animTime = anim->animStart;
		QueueFlush(actor);

		if (StartAnim)
			StartAnim(actor);
//		if( actorAnim->sfxMapping && actorAnim->sfxMapping[actorAnim->currentAnimation] )
//			PlaySfxMappedSample( actor, 500, SAMPLE_VOLUME, -1/*128*/ );
	}
	else
	{
		if(actorAnim->currentAnimation == -1)
		{
			actorAnim->currentAnimation = animNum;
			actorAnim->loopAnimation = loop;
			actorAnim->animationSpeed = actualSpeed;			
		}
		else
		{
			if(actorAnim->numberQueued == MDX_ANIM_QUEUE_LENGTH)
				return;
			actorAnim->queueAnimation[actorAnim->numberQueued] = animNum;
			actorAnim->queueLoopAnimation[actorAnim->numberQueued] = loop;
			actorAnim->queueAnimationSpeed[actorAnim->numberQueued] = actualSpeed;
			actorAnim->numberQueued++;
		}
	}
}

void UpdateAnims(MDX_ACTOR *actor)
{
	MDX_ACTOR_ANIMATION *actorAnim = actor->animation;
	MDX_ANIMATION *anim;
	int i;

	if(actorAnim == NULL)
		return;

	if(actorAnim->anims == NULL)
		return;
	anim = actorAnim->anims + actorAnim->currentAnimation;

	if(actorAnim->currentAnimation < 0)
		return;

	actorAnim->reachedEndOfAnimation = FALSE;
	actorAnim->animTime += (actorAnim->animationSpeed) * timeInfo.speed;

	if( ((actorAnim->animTime > anim->animEnd) || (actorAnim->animTime < anim->animStart)) && (!actorAnim->loopAnimation || actorAnim->numberQueued) )
	{
		actorAnim->animTime = Bound(actorAnim->animTime,anim->animStart,anim->animEnd);
		actorAnim->reachedEndOfAnimation = 1;//actorAnim->currentAnimation+1;

		if(actorAnim->numberQueued)
		{
			actorAnim->currentAnimation = actorAnim->queueAnimation[0];//actorAnim;
			actorAnim->loopAnimation = actorAnim->queueLoopAnimation[0];//loop;
			actorAnim->animationSpeed = actorAnim->queueAnimationSpeed[0];//speed;
			
			if(actorAnim->animationSpeed < 0)
				actorAnim->animTime = actorAnim->anims[actorAnim->currentAnimation].animEnd;
			else
				actorAnim->animTime = actorAnim->anims[actorAnim->currentAnimation].animStart;


			if (--actorAnim->numberQueued)
			{
				for(i = 0;i < MDX_ANIM_QUEUE_LENGTH - 1;i++)
				{
					actorAnim->queueAnimation[i] = actorAnim->queueAnimation[i + 1];
					actorAnim->queueLoopAnimation[i] = actorAnim->queueLoopAnimation[i + 1];
					actorAnim->queueAnimationSpeed[i] = actorAnim->queueAnimationSpeed[i + 1];
				}

				actorAnim->queueAnimation[MDX_ANIM_QUEUE_LENGTH - 1] = -1;
				actorAnim->queueLoopAnimation[MDX_ANIM_QUEUE_LENGTH - 1] = -1;
				actorAnim->queueAnimationSpeed[MDX_ANIM_QUEUE_LENGTH - 1] = -1;
			}
			else
			{
				*actorAnim->queueAnimation = -1;
				*actorAnim->queueLoopAnimation = -1;
				*actorAnim->queueAnimationSpeed = -1;
			}

			if (StartAnim)
				StartAnim(actor);
			//if( actorAnim->sfxMapping )
			//	PlaySfxMappedSample( actor, 500, SAMPLE_VOLUME, -1/*128*/ );
		}
	}
	else
	{
		
		if(anim->animEnd == anim->animStart)
			actorAnim->animTime = anim->animEnd;			
		else if(actorAnim->animTime > anim->animEnd)
		{
			actorAnim->animTime -= (anim->animEnd - anim->animStart);

			if (StartAnim)
				StartAnim(actor);
//			if( actorAnim->sfxMapping )
//				PlaySfxMappedSample( actor, 500, SAMPLE_VOLUME, -1/*128*/ );
		}
		else if(actorAnim->animTime < anim->animStart)
			actorAnim->animTime += (anim->animEnd - anim->animStart);			
		
	}
}


unsigned long CheckBoundingBox(MDX_VECTOR *bBox,MDX_MATRIX *m)
{
	MDX_VECTOR t[8];
	MDX_VECTOR *r = t;
	unsigned long pointOn;

	unsigned long left,right,top,bottom;
	left = top = right = bottom = 0;
	
	for (int i=8; i; i--)
	{
		XfmPoint(r,bBox,m);

		if (r->vx>rXRes)// || r->vz<10)
			right++;

		if (r->vx<0)// || r->vz<10)		
			left++;			

		if (r->vy>rYRes)// || r->vz<10)
			bottom++;

		if (r->vy<0)// || r->vz<10)
			top++;				

		r++;
		bBox++;
	}
	
	if ((left|right|top|bottom) & (~7))
		return 1;
	//if ((left>7) || (right>7) || (top>7) || (bottom>7))
	
	return 0;
}
/*
MDX_TEXENTRY *temp = 0;

unsigned long CheckBoundingBox(MDX_VECTOR *bBox,MDX_MATRIX *m)
{
	MDX_VECTOR r[8];
	D3DTLVERTEX v[3];
	
	unsigned long left,right,top,bottom,in;
	left = top = right = bottom = in = 0;
	
	if (!temp)
		temp = GetTexEntryFromCRC(UpdateCRC("dock4.bmp"));


	for (int i=0; i<8; i++)
	{
		XfmPoint(&r[i],&bBox[i],m);

	//	if (r[i].vz < 10)
	//		in++;
	//	else
	//	{

		if (r[i].vx>rXRes || r[i].vz<10)
			right++;

		if (r[i].vx<0 || r[i].vz<10)		
			left++;			

		if (r[i].vy>rYRes || r[i].vz<10)
			bottom++;

		if (r[i].vy<0 || r[i].vz<10)
			top++;				
	//	}
	}
	
	if ((left>7) || (right>7) || (top>7) || (bottom>7) || (in>7))
		return 1;
	
	dp("left %lu   right %lu   up %lu   down %lu   z%lu   \n",left,right,top,bottom,in);
	for (i=0; i<24; i++)
	{
		long vals[] =	{
							2,1,0, 
							3,2,0,
							
							4,5,6,
							4,6,7,

							1,2,5, 
							2,6,5,

							4,3,0, 
							7,3,4,

							5,4,1, 
							1,4,0,

							7,6,3, 
							6,2,3,
				

							0,1,2, 
							0,2,3,
							
							6,5,4,
							7,6,4,

							5,2,1, 
							5,6,2,

							0,3,4, 
							4,3,7,

							1,4,5, 
							0,4,1,

							3,6,7, 
							3,2,6,				
						};
		v[0].sx = r[vals[i*3]].vx;
		v[0].sy = r[vals[i*3]].vy;
		v[0].sz = r[vals[i*3]].vz*0.00025;
		v[1].sx = r[vals[i*3+1]].vx;
		v[1].sy = r[vals[i*3+1]].vy;
		v[1].sz = r[vals[i*3+1]].vz*0.00025;
		v[2].sx = r[vals[i*3+2]].vx;
		v[2].sy = r[vals[i*3+2]].vy;
		v[2].sz = r[vals[i*3+2]].vz*0.00025;
		v[0].color = (((long)bBox)&0x00ffffff) | D3DRGBA(1,1,0,0.2);
		v[1].color = (((long)bBox)&0x00ffffff) | D3DRGBA(1,1,0,0.2);
		v[2].color = (((long)bBox)&0x00ffffff) | D3DRGBA(1,1,0,0.2);
		v[0].specular = 0;
		v[1].specular = 0;
		v[2].specular = 0;
		v[0].tu = 0;
		v[0].tv = 0;
		v[1].tu = 1;
		v[1].tv = 0;
		v[2].tu = 1;
		v[2].tv = 1;

		SwapFrame(MA_FRAME_XLU);

		if (v[0].sz && v[1].sz && v[2].sz)
			Clip3DPolygon(v,temp);
		
		SwapFrame(MA_FRAME_NORMAL);
	}
	return 0;
}
*/
void XformActor(MDX_ACTOR *actor)
{
	MDX_OBJECT_CONTROLLER *objectC = actor->objectController;
	float transmat[4][4];
	float rotmat[4][4];
	float animTime;

	if (!objectC)
		return;

	UpdateAnims(actor);

	if((actor->animation) && (actor->animation->anims))
		animTime = actor->animation->animTime;
	else
		animTime = 0;

	currentDrawActor = actor;
	actorScale = &actor->scale;

	parentScaleStackLevel = 0;
	parentScaleStack[parentScaleStackLevel].vx = parentScaleStack[parentScaleStackLevel].vy = parentScaleStack[parentScaleStackLevel].vz = 1;

	guTranslateF(transmat, actor->pos.vx, actor->pos.vy, actor->pos.vz);
	PushMatrix(transmat);

	QuaternionToMatrix(&actor->qRot,(MDX_MATRIX *)rotmat);
	PushMatrix(rotmat);

	
	
	if(objectC->object)
		TransformObject(objectC->object, animTime);


  	parentScaleStackLevel = 0;
	parentScaleStack[parentScaleStackLevel].vx = parentScaleStack[parentScaleStackLevel].vy = parentScaleStack[parentScaleStackLevel].vz = 1;
	
	matrixStack.stackPosition-=2;
}

void InitAnims(MDX_ACTOR *tempActor)
{

	if( !tempActor->objectController )
		return;
	
	memcpy(tempActor->animation, tempActor->objectController->animation, sizeof(MDX_ACTOR_ANIMATION));	

	tempActor->animation->currentAnimation = -1;
	tempActor->animation->queueAnimation[0] = -1;
	tempActor->animation->queueAnimation[1] = -1;
	tempActor->animation->queueAnimation[2] = -1;
	tempActor->animation->queueAnimation[3] = -1;
	tempActor->animation->queueAnimation[4] = -1;
	tempActor->animation->loopAnimation = 0;
	tempActor->animation->numberQueued = 0;
	tempActor->animation->animTime = 0;
	tempActor->animation->reachedEndOfAnimation = FALSE;	
}

void InitActorStructures(MDX_ACTOR *tempActor, int initFlags)
{
	tempActor->scale.vx = tempActor->scale.vy = tempActor->scale.vz = 1;

	if(initFlags & INIT_ANIMATION)
	{
		if((tempActor->objectController) && (tempActor->objectController->animation))
		{
			tempActor->animation = new MDX_ACTOR_ANIMATION;
			InitAnims(tempActor);
		}
	}

	if(initFlags & INIT_SHADOW)
	{
		tempActor->shadow = new MDX_ACTOR_SHADOW;
		tempActor->shadow->alpha = 255;
		tempActor->shadow->draw = 1;
	}

	tempActor->status = 1;
	tempActor->xluOverride = 100;
}

void InitActor(MDX_ACTOR *tempActor, char *name, float x, float y, float z, int initFlags)
{
	FindObject(&tempActor->objectController, UpdateCRC(name), name);
	tempActor->LODObjectController = NULL;
	
	if(!tempActor->objectController)
		return;

	InitActorStructures(tempActor, initFlags);

	tempActor->pos.vx = x;	tempActor->pos.vy = y;	tempActor->pos.vz = z;
	tempActor->scale.vx = tempActor->scale.vy = tempActor->scale.vz = 1;
	tempActor->flags = 0;
	tempActor->overrideTex = 0;
	ZeroQuaternion(&tempActor->qRot);
}

void FreeUniqueActorList(void)
{
	for (int i=0; i<numActorsUniqe; i++)
		uniqueActorCRC[i] = 0;
	numActorsUniqe = 0;

}


void FreeUniqueObject(MDX_OBJECT *object)
{
	if (object)
	{
		if (object->numSprites)
			delete object->sprites;

		if(object->children)
			FreeUniqueObject(object->children);

		if(object->next)
			FreeUniqueObject(object->next);	

		delete object;
	}
}



void FreeActor(MDX_ACTOR **toFree)
{
	MDX_ACTOR *a = *toFree;

	if (a->objectController)
		FreeUniqueObject(a->objectController->object);


	if (a->next) a->next->prev = a->prev;
	if (a->prev)
		a->prev->next = a->next;
	else
		actorList = a->next;

	delete a->animation;

	//MC - 0xcdcdcdcd - if( a->shadow ) delete a->shadow;
	delete a;

	*toFree = 0;
}

void MakeUniqueVtx(MDX_OBJECT_CONTROLLER *objC)
{
}

MDX_OBJECT *MakeUniqueObject(MDX_OBJECT *object)
{
	MDX_OBJECT	*obj;	
	MDX_OBJECTSPRITE **spr, *tempSpr;
		
	obj = object;
	object = new MDX_OBJECT;
	memcpy(object, obj, sizeof(MDX_OBJECT));

	if(obj->numSprites)
	{
		spr = &object->sprites;
		tempSpr = new MDX_OBJECTSPRITE[obj->numSprites];
		memcpy(tempSpr, *spr, sizeof(MDX_OBJECTSPRITE) * obj->numSprites);
		*spr = tempSpr;
	}

	if(object->children)
		object->children = MakeUniqueObject(object->children);

	if(object->next)
		object->next = MakeUniqueObject(object->next);
		
	return object;
}

void MakeUniqueActor(MDX_ACTOR *actor,int type)
{
	MDX_OBJECT_CONTROLLER *objCont;
	short	unique = TRUE;
	short	i;
	int		CRC;
	
	if(!actor->objectController)
		return;
		
	CRC = UpdateCRC((char *)actor->objectController->object->name);

	//check all crc's to see if actor is among them
	for(i = 0; i < numActorsUniqe; i++)
	{
		if(uniqueActorCRC[i] == CRC)
		{
			//if it is, actor is not unique and must have seperate stuff
			//dprintf"found duplicate actor %s\n", actor->objectController->object->name));
			unique = FALSE;
			break;
		}
	}
	//if actor is not in list
	if(unique == TRUE)
	{
		uniqueActorCRC[numActorsUniqe++] = CRC;
	}	

	objCont = actor->objectController;
	actor->objectController = new MDX_OBJECT_CONTROLLER;
	memcpy(actor->objectController, objCont, sizeof(MDX_OBJECT_CONTROLLER));
	actor->objectController->object = MakeUniqueObject(actor->objectController->object);
	
	if(unique == FALSE)
	{
		//if actor is skinned, duplicate Vtx's
		if(actor->objectController->drawList)
		{
//			MakeUniqueVtx(actor->objectController);
//			XformActor(actor);
		}
	}
}

MDX_ACTOR *CreateActor(char *name, unsigned long flags)
{
	MDX_ACTOR *t;
	t = new MDX_ACTOR;
	
	InitActor(t,name,0,0,0,flags);

	if (!t->objectController)
	{
		delete t;
		return NULL;
	}

	MakeUniqueActor(t,0);
	return t;
}

MDX_OBJECT *FindObj(MDX_OBJECT *me, char *name)
{
	MDX_OBJECT *t = NULL;
	
	if (strcmp((const char *)me->name,name)==0)
		return me;

	if (me->next)
		t = FindObj(me->next,name);
	if (me->children)
		t = FindObj(me->children,name);

	return t;
}

MDX_OBJECT *FindActorSubObject(MDX_ACTOR *me, char *subName)
{	
	return FindObj(me->objectController->object,subName);
}

void AttachActorToSubobject(MDX_OBJECT *dest, MDX_ACTOR *src)
{
	dest->attachedActor = src;
}

#ifdef __cplusplus
}
#endif
