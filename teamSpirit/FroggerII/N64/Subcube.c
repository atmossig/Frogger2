/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.
	
	  
	File		: Subcube.c
	Programmer	: Andrew Eder
	Date		: 11/24/98
	
----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

#include <ultra64.h>

#include "incs.h"


Vtx	objectsVtx[2][2000];
Vtx *vtxPtr;

char changeRenderMode	= 1;

float unitMatrix[4][4] = {0, 0, 0, 0,
				  0, 0, 0, 0,
				  0, 0, 0, 0,
				  0, 0, 0, 0};

char	*tempObjectPtr, *tempObjectPtr2;

VECTOR	actorScale;
VECTOR	parentScaleStack[10];
int		parentScaleStackLevel = 0;

int		matrixStackCount = 0;
short	xluFact = 0x8f;
short	xluOverride = 0;

ACTOR	*currentDrawActor;
Mtx		*staticObjectMtx;
char	calcMtx = FALSE;

char	staticObj;

void DrawObject(OBJECT *obj);


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int InitObject(OBJECT **objectPtr, short flags)//, char *tempPtr)
{
	int objSize;

	objSize = *(int *)tempObjectPtr;
	tempObjectPtr2 = tempObjectPtr;
	tempObjectPtr2 += 4;

	*objectPtr = (OBJECT *)tempObjectPtr2;

	if((*objectPtr)->active)
		return 0;

	(*objectPtr)->flags = flags;


	(*objectPtr)->mesh = (MESH *)((tempObjectPtr) + (int)(*objectPtr)->mesh);
	(*objectPtr)->scaleKeys = (KEYFRAME *)((tempObjectPtr) + (int)(*objectPtr)->scaleKeys);
	(*objectPtr)->moveKeys = (KEYFRAME *)((tempObjectPtr) + (int)(*objectPtr)->moveKeys);
	(*objectPtr)->rotateKeys = (KEYFRAME *)((tempObjectPtr) + (int)(*objectPtr)->rotateKeys);

	(*objectPtr)->mesh->vertices = (VECTOR *)((tempObjectPtr) + (int)(*objectPtr)->mesh->vertices);
	(*objectPtr)->mesh->faceIndex = (SHORTVECTOR *)((tempObjectPtr) + (int)(*objectPtr)->mesh->faceIndex);

	tempObjectPtr += objSize;

	if((*objectPtr)->children)	
	{
		InitObject(&((*objectPtr)->children), flags);//, tempPtr);
	}

	if((*objectPtr)->next)	
	{
		InitObject(&((*objectPtr)->next), flags);//, tempPtr);
	}

	(*objectPtr)->active = 1;
	return objSize;

}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitActorStructures(ACTOR *tempActor, int initFlags)
{
	int j;
	char temp[24];

	tempActor->scale.v[X] = tempActor->scale.v[Y] = tempActor->scale.v[Z] = 1;

	if(initFlags & INIT_ANIMATION)
	{
		if((tempActor->objectController) && (tempActor->objectController->animation))
		{
			tempActor->animation = (ACTOR_ANIMATION *)JallocAlloc(sizeof(ACTOR_ANIMATION), YES, "Act_Anim");
			InitActorAnim(tempActor);
		}
	}

	if(initFlags & INIT_SHADOW)
	{
		tempActor->shadow = (ACTOR_SHADOW *)JallocAlloc(sizeof(ACTOR_SHADOW),YES,"ACTSHAD");
		memcpy(tempActor->shadow->vert,shadowVtx,sizeof(Vtx) * 4);
		tempActor->shadow->alpha = tempActor->shadow->alphaAim = 255;
		tempActor->shadow->alphaAimSpeed = 10;
	}

	tempActor->status = ACTIVE;
	tempActor->xluOverride = 100;
}

/*	--------------------------------------------------------------------------------
	Function 	: InitActor()
	Purpose 	: inits structures etc for an actor with NO COLLISION DATA
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitActor(ACTOR *tempActor, char *name, float x, float y, float z, int initFlags)
{
	FindObject(&tempActor->objectController, UpdateCRC(name), name,YES);
	if(!tempActor->objectController)
		return;

	InitActorStructures(tempActor, initFlags);

	tempActor->pos.v[X] = x;
	tempActor->pos.v[Y] = y;
	tempActor->pos.v[Z] = z;
	tempActor->scale.v[X] = tempActor->scale.v[Y] = tempActor->scale.v[Z] = 1;

//init actors rotation
	ZeroQuaternion(&tempActor->qRot);
}

extern float meMod;
float adjF = 0.3;
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void XformActor(ACTOR *ptr)
{
	
	float rotmat[4][4], scalemat[4][4], transmat[4][4];
	float	animTime = 0;

	QUATERNION tempQ;
	UpdateAnimations(ptr);	
	vtxPtr = &(objectsVtx[draw_buffer][0]);	
	return;
/*		
	if(dispFrameCount == 1)
	{
		ptr->matrix = NULL;
	}
	else
	{
		calcMtx = FALSE;	//use precalculated matrix
	}


	staticObjectMtx = ptr->matrix;
	if((staticObjectMtx) && (calcMtx == FALSE))
		staticObj = TRUE;
	else
		staticObj = FALSE;

	UpdateAnimations(ptr);

	if(ptr->xluOverride == 0)
		return;

	matrixStackCount = 0;
	//create matrix for actors position rotation and scale

	// MATT!
	staticObj = FALSE;

	if((calcMtx == TRUE) || (staticObj == FALSE))
	{
	/*	guTranslateF(transmat, ptr->pos.v[X], ptr->pos.v[Y], ptr->pos.v[Z]);

		PushMatrix(&transmat[0][0]);

		QuaternionToMatrix(&ptr->qRot,(MATRIX *)rotmat);
		PushMatrix(&rotmat[0][0]);
*/
/*		
		actorScale.v[X] = ptr->scale.v[X];
		actorScale.v[Y] = ptr->scale.v[Y];
		actorScale.v[Z] = ptr->scale.v[Z];

		parentScaleStackLevel = 0;
		parentScaleStack[parentScaleStackLevel].v[X] = parentScaleStack[parentScaleStackLevel].v[Y] = parentScaleStack[parentScaleStackLevel].v[Z] = 1;

		if((ptr->animation) && (ptr->animation->anims))
			animTime = ptr->animation->animTime;
		else
			animTime = 0;
	}

	xluOverride = ptr->xluOverride;

	currentDrawActor = ptr;
	if(ptr->objectController->object)
	{
		TransformAndDrawObject(ptr->objectController->object, animTime, 0, 0);
	}
*/
	/*
	if((calcMtx == TRUE) || (staticObj == FALSE))
	{
		PopMatrix();
		PopMatrix();
	}*/
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	:  
	Returns 	: 
	Info 		:
*/
void TransformAndDrawObject2(OBJECT *obj, float time, short animStart, short animEnd)
{
	QUATERNION	tempQuat;

	float rotmat[4][4], scalemat[4][4], transmat[4][4];
	int i;
	float XTranslateAmount, YTranslateAmount, ZTranslateAmount;
	float XScaleAmount = 1, YScaleAmount = 1, ZScaleAmount = 1;
	SPRITE *sprite;

	short	fromKey = -1, toKey = -1, low, high;
	VECTOR	tempVect;
	float	tempFloat;
	int		xluVal;

//handle position keyframes

	if((calcMtx == TRUE) || (staticObj == FALSE))
	{

		if(obj->numMoveKeys > 1)
		{

			i = 0;
			fromKey = toKey = -1;

			low=0;
			high=obj->numMoveKeys-1;

			while(low<=high)
			{
				i= (low+high) >> 1;

				if (obj->moveKeys[i].time==time)
				{
					break;
				}
				else
				{
					if (obj->moveKeys[i].time>time)
					{
						high = i-1;
					}
					else
					{
						low = i+1;
					}
				}
			}
			
			if (low>0) low--;
			for(i = low; i < obj->numMoveKeys; i++)
			{
				if(obj->moveKeys[i].time > time)
				{
					toKey = i;
					fromKey = i - 1;
					break;
				}
			}						


			if(fromKey != -1)		//time is within valid keyframes
			{
				tempFloat = abs(obj->moveKeys[toKey].time - obj->moveKeys[fromKey].time);
				tempFloat = (time - (float)obj->moveKeys[fromKey].time) / tempFloat;

			}
			else					// must be between last and first keyframe
			{
				fromKey = obj->numMoveKeys-1;
				toKey = fromKey;
				tempFloat = 1;

			}

			tempVect.v[X] = obj->moveKeys[fromKey].u.vect.v[X] + ((obj->moveKeys[toKey].u.vect.v[X] - obj->moveKeys[fromKey].u.vect.v[X]) *  tempFloat);
			tempVect.v[Y] = obj->moveKeys[fromKey].u.vect.v[Y] + ((obj->moveKeys[toKey].u.vect.v[Y] - obj->moveKeys[fromKey].u.vect.v[Y]) *  tempFloat);
			tempVect.v[Z] = obj->moveKeys[fromKey].u.vect.v[Z] + ((obj->moveKeys[toKey].u.vect.v[Z] - obj->moveKeys[fromKey].u.vect.v[Z]) *  tempFloat);
			
			
			XTranslateAmount = tempVect.v[X];
			YTranslateAmount = tempVect.v[Y];
			ZTranslateAmount = tempVect.v[Z];

		}
		else
		{
			XTranslateAmount = (obj->moveKeys[0].u.vect.v)[X];
			YTranslateAmount = (obj->moveKeys[0].u.vect.v)[Y];
			ZTranslateAmount = (obj->moveKeys[0].u.vect.v)[Z];
		}

	// handle scale keyframes
		if(obj->numScaleKeys > 1)
		{

			i = 0;
			fromKey = toKey = -1;

			low=0;
			high=obj->numScaleKeys-1;

			while(low<=high)
			{
				i= (low+high) >> 1;

				if (obj->scaleKeys[i].time==time)
				{
					break;
				}
				else
				{
					if (obj->scaleKeys[i].time>time)
					{
						high = i-1;
					}
					else
					{
						low = i+1;
					}
				}
			}
			
			if (low>0) low--;
			for(i = low; i < obj->numScaleKeys; i++)
			{
				if(obj->scaleKeys[i].time > time)
				{
					toKey = i;
					fromKey = i - 1;
					break;
				}
			}						





			if(fromKey != -1)		//time is within valid keyframes
			{
				tempFloat = abs(obj->scaleKeys[toKey].time - obj->scaleKeys[fromKey].time);
				tempFloat = (time - (float)obj->scaleKeys[fromKey].time) / tempFloat;

			}
			else					// must be between last and first keyframe
			{

				fromKey = obj->numScaleKeys-1;
				toKey = fromKey;
				tempFloat = 1;

			}

			tempVect.v[X] = obj->scaleKeys[fromKey].u.vect.v[X] + ((obj->scaleKeys[toKey].u.vect.v[X] - obj->scaleKeys[fromKey].u.vect.v[X]) *  tempFloat);
			tempVect.v[Y] = obj->scaleKeys[fromKey].u.vect.v[Y] + ((obj->scaleKeys[toKey].u.vect.v[Y] - obj->scaleKeys[fromKey].u.vect.v[Y]) *  tempFloat);
			tempVect.v[Z] = obj->scaleKeys[fromKey].u.vect.v[Z] + ((obj->scaleKeys[toKey].u.vect.v[Z] - obj->scaleKeys[fromKey].u.vect.v[Z]) *  tempFloat);
			
			XScaleAmount = tempVect.v[0];
			YScaleAmount = tempVect.v[1];
			ZScaleAmount = tempVect.v[2];


		}
		else
		{
			XScaleAmount = obj->scaleKeys[0].u.vect.v[0];
			YScaleAmount = obj->scaleKeys[0].u.vect.v[1];
			ZScaleAmount = obj->scaleKeys[0].u.vect.v[2];
		}
	// handle rotation keyframes
		if(obj->numRotateKeys > 1)
		{
//			if(obj->flags & OBJECT_FLAGS_CONSTANIM)
//				time = 1 + (frameCount MOD 40);

			i = 0;
			fromKey = toKey = -1;

			low=0;
			high=obj->numRotateKeys-1;

			while(low<=high)
			{
				i= (low+high) >> 1;

				if (obj->rotateKeys[i].time==time)
				{
					break;
				}
				else
				{
					if (obj->rotateKeys[i].time>time)
					{
						high = i-1;
					}
					else
					{
						low = i+1;
					}
				}
			}
			
			if (low>0) low--;
			for(i = low; i < obj->numRotateKeys; i++)
			{
				if(obj->rotateKeys[i].time > time)
				{
					toKey = i;
					fromKey = i - 1;
					break;
				}
			}						


			if(fromKey != -1)		//time is within valid keyframes
			{
				tempFloat = abs(obj->rotateKeys[toKey].time - obj->rotateKeys[fromKey].time);
				tempFloat = (time - (float)obj->rotateKeys[fromKey].time) / tempFloat;

			}
			else					// must be between last and first keyframe
			{

				fromKey = obj->numRotateKeys-1;
				toKey = fromKey;
				tempFloat = 1;



			}

			QuatSlerp(&obj->rotateKeys[fromKey].u.quat, &obj->rotateKeys[toKey].u.quat, tempFloat, &tempQuat);

	//convert back to rotation matrix
			QuaternionToMatrix(&tempQuat, (MATRIX *)rotmat);
		}
		else
		{
			QuaternionToMatrix(&obj->rotateKeys[0].u.quat, (MATRIX *)rotmat);
		}

	
	    guTranslateF(transmat, XTranslateAmount * actorScale.v[X] * parentScaleStack[parentScaleStackLevel].v[X],
							   YTranslateAmount * actorScale.v[Y] * parentScaleStack[parentScaleStackLevel].v[Y],
							   ZTranslateAmount * actorScale.v[Z] * parentScaleStack[parentScaleStackLevel].v[Z]);

		//concatinate rotation and translate matrices quickly
		rotmat[3][0] = transmat[3][0];
		rotmat[3][1] = transmat[3][1];
		rotmat[3][2] = transmat[3][2];
		rotmat[3][3] = transmat[3][3];

		PushMatrix(&rotmat[0][0]);

//calculate objects scale matrix
	    guScaleF(scalemat, XScaleAmount * actorScale.v[X], YScaleAmount * actorScale.v[Y], ZScaleAmount * actorScale.v[Z]);
		PushMatrix(&scalemat[0][0]);
	}

//here we need the result of our matrix stack 
	if((calcMtx == TRUE) || (staticObj == FALSE))
	{
		
		ComputeResultMatrix();		
		guMtxF2L(matrixStack.result, &dynamicp->modeling4[objectMatrix]);
	

		//store the static mtx
		if(calcMtx == TRUE)
		{
			if(staticObjectMtx)
			{
//				SetMatrix((float *)staticObjectMtx, (float *)&dynamicp->modeling4[objectMatrix]);
				guMtxF2L(matrixStack.result, staticObjectMtx);
			}
		}
	
		SetMatrix ((float *)&obj->myMatrix,(float *)matrixStack.result);//&dynamicp->modeling4[objectMatrix]);

//		gSPMatrix(glistp++, 
//		 OS_K0_TO_PHYSICAL(&(dynamicp->modeling4[objectMatrix++])),
//		 G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
		
//		matrixStackCount++;
		

	}
	else
	{
//		guMtxF2L(unitMatrix, &dynamicp->modeling4[objectMatrix]);
		SetMatrix((float *)&dynamicp->modeling4[objectMatrix], (float *)staticObjectMtx);
//		memcpy(&dynamicp->modeling4[objectMatrix], staticObjectMtx, sizeof(Mtx));

	    gSPMatrix(glistp++, 
		OS_K0_TO_PHYSICAL(&(dynamicp->modeling4[objectMatrix++])),
		G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
		matrixStackCount++;

	}
	
//		/* --------------------------------------------------------/ *Hey.... Nevermind! */
/*--------------------------------------------

		if(obj->drawList)
		{
			gSPDisplayList(glistp++, obj->drawList);
		}
		else if(obj->mesh->numFaces)
		{
			if(obj->flags & OBJECT_FLAGS_GOURAUD_SHADED)
			{
				if(!(obj->flags & OBJECT_FLAGS_PRELIT))
				{
					gSPSetGeometryMode(glistp++, G_LIGHTING);
				}
				else
				{
				    gSPClearGeometryMode(glistp++, G_LIGHTING);
				}
				WriteObjectDisplayListGouraud(obj);
			}
			else
			{
				if(!(obj->flags & OBJECT_FLAGS_PRELIT))
				{
					gSPSetGeometryMode(glistp++, G_LIGHTING);
				}
				else
				{
				    gSPClearGeometryMode(glistp++, G_LIGHTING);
				}
				WriteObjectDisplayListFlat(obj);
			}
		}

		
		gSPPopMatrix(glistp++, G_MTX_MODELVIEW);
		matrixStackCount--;
-------------------------------------------------------------------------------------*/
	if((calcMtx == TRUE) || (staticObj == FALSE))
	{

		PopMatrix();

//transform children
		if(obj->children)
		{
			parentScaleStackLevel++;
			parentScaleStack[parentScaleStackLevel].v[X] = XScaleAmount/* * actorScale.v[X]*/;
			parentScaleStack[parentScaleStackLevel].v[Y] = YScaleAmount/* * actorScale.v[Y]*/;
			parentScaleStack[parentScaleStackLevel].v[Z] = ZScaleAmount/* * actorScale.v[Z]*/;
			TransformAndDrawObject(obj->children, time, animStart, animEnd);
			parentScaleStackLevel--;
		}

		PopMatrix();

		if(obj->next)
		{
			TransformAndDrawObject(obj->next, time, animStart, animEnd);
		}
	}
}

/*---------------------------------------------------------------------------------------------
	Function	: DrawMyObject
	Parameters	: (OBJECT *obj)
	Returns		: void 
*/

void DrawObject(OBJECT *obj)
{
	float	tempFloat;

	changeRenderMode = 0;
	if(!aiSurf)
	{
		if(obj->flags & OBJECT_FLAGS_TRANSPARENT)
		{
			if(transparentSurf == FALSE)
			{
				transparentSurf = TRUE;
				changeRenderMode = TRUE;
			}
		}
		else
		{
			if(transparentSurf == TRUE)
			{
				transparentSurf = FALSE;
				changeRenderMode = TRUE;
			}
		}
		if(obj->flags & OBJECT_FLAGS_XLU)
		{
			if(xluSurf == FALSE)
			{
				xluSurf = TRUE;
				changeRenderMode = 1;
			}
			if(obj->drawList == 0)
				xluSurf = TRUE;
			tempFloat = (float)xluOverride / 100;
			tempFloat *= (float)obj->xlu;
			if(tempFloat > 255)
				tempFloat = 255;
			if((int)tempFloat != xluFact)
				changeRenderMode = 1;
			xluFact = tempFloat;
		}
		else
		{
			if(xluSurf == TRUE)
			{
				xluSurf = FALSE;
				changeRenderMode = 1;
			}
		}

		if(changeRenderMode)
			SetRenderMode();
	}
	else
	{
		gDPSetPrimColor(glistp++,0,0,/*r*/255,/*g*/255,/*b*/255,obj->xlu);
	}

	
	PushMatrix(&obj->myMatrix);
		ComputeResultMatrix();	
		
		guMtxF2L(matrixStack.result, &dynamicp->modeling4[objectMatrix]);
		//	SetMatrix (&dynamicp->modeling4[objectMatrix],(float *)matrixStack.result);
	
	//	SetMatrix (&dynamicp->modeling4[objectMatrix],(float *)&obj->myMatrix);
	//	guMtxCatL(t, dynamicp->modeling4[objectMatrix], dynamicp->modeling4[objectMatrix]);
	
	PopMatrix();

	gSPMatrix(glistp++, 
		 OS_K0_TO_PHYSICAL(&(dynamicp->modeling4[objectMatrix++])),
		 G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
		
		matrixStackCount++;
	

	if(obj->drawList)
	{
		gSPDisplayList(glistp++, obj->drawList);
	}
	else if(obj->mesh->numFaces)
	{
		if(obj->flags & OBJECT_FLAGS_GOURAUD_SHADED)
		{
			if(!(obj->flags & OBJECT_FLAGS_PRELIT))
			{
				gSPSetGeometryMode(glistp++, G_LIGHTING);
			}
			else
			{
			    gSPClearGeometryMode(glistp++, G_LIGHTING);
			}
			WriteObjectDisplayListGouraud(obj);
		}
		else
		{
			if(!(obj->flags & OBJECT_FLAGS_PRELIT))
			{
				gSPSetGeometryMode(glistp++, G_LIGHTING);
			}
			else
			{
			    gSPClearGeometryMode(glistp++, G_LIGHTING);
			}
			WriteObjectDisplayListFlat(obj);
		}
	}

		
	gSPPopMatrix(glistp++, G_MTX_MODELVIEW);
	matrixStackCount--;

	if(obj->children)
		DrawObject(obj->children);
	
	if(obj->next)
		DrawObject(obj->next);
	
}


/*
void DrawActor(ACTOR *ptr)
{
	float rotmat[4][4], scalemat[4][4], transmat[4][4];
	float	animTime = 0;

	QUATERNION tempQ;

	vtxPtr = &(objectsVtx[draw_buffer][0]);			
	staticObj = FALSE;
	if(ptr->xluOverride == 0)
		return;
	matrixStackCount = 0;	

	if((calcMtx == TRUE) || (staticObj == FALSE))
	{
		guTranslateF(transmat, ptr->pos.v[X], ptr->pos.v[Y], ptr->pos.v[Z]);

		PushMatrix(&transmat[0][0]);

		QuaternionToMatrix(&ptr->qRot,(MATRIX *)rotmat);
		PushMatrix(&rotmat[0][0]);
	}

	xluOverride = ptr->xluOverride;
	currentDrawActor = ptr;
	if(ptr->objectController->object)
		DrawObject(ptr->objectController->object);		

	if((calcMtx == TRUE) || (staticObj == FALSE))
	{
		PopMatrix();
		PopMatrix();
	}

}
*/
void DrawActor(ACTOR *ptr)
{
	float rotmat[4][4], scalemat[4][4], transmat[4][4];
	float	animTime = 0;

	QUATERNION tempQ;
		
	if(dispFrameCount == 1)
	{
		//this is the first time objects are drawn, so store matrix if static object
//		if(ptr->type == LANDSCAPE_ACTOR)
//		{
//			if(ptr->objectController)
//				JallocFree((UBYTE **)&ptr->matrix);

//			ptr->matrix = (Mtx *)JallocAlloc(sizeof(Mtx), YES, "statMtx");
//			calcMtx = TRUE;		//work out the static objects mtx
//		}
//		else
		{
			ptr->matrix = NULL;
		}
	
	}
	else
	{
		calcMtx = FALSE;	//use precalculated matrix

//		if(IsActorVisible(ptr) == FALSE)
//			return;

	}


	staticObjectMtx = ptr->matrix;
	if((staticObjectMtx) && (calcMtx == FALSE))
		staticObj = TRUE;
	else
		staticObj = FALSE;


/*
	if((!gamePaused) && (animateActors) && ((ptr->stats == NULL) || (ptr->stats->enemyNum == -1) || (enemies[ptr->stats->enemyNum].spellCount[STUN] == 0)))
	{
//		if(IsActorVisible(ptr) == FALSE)
//			return;
		UpdateAnimations(ptr);
	}
*/


	if(ptr->xluOverride == 0)
		return;

	matrixStackCount = 0;
	//create matrix for actors position rotation and scale

	// MATT!
	staticObj = FALSE;

	if((calcMtx == TRUE) || (staticObj == FALSE))
	{
		guTranslateF(transmat, ptr->pos.v[X], ptr->pos.v[Y], ptr->pos.v[Z]);

		PushMatrix(&transmat[0][0]);

		/*
		if((ptr->stats) && (ptr->stats->bounceSpeed))
		{
			guScaleF(scalemat,ptr->stats->bounceScale.v[X],ptr->stats->bounceScale.v[Y],ptr->stats->bounceScale.v[X]);
			QuaternionToMatrix(&ptr->stats->bounceQ,(MATRIX *)rotmat);

			guMtxCatF(scalemat,rotmat,scalemat);

			PushMatrix(&scalemat[0][0]);
			tempQ = ptr->stats->bounceQ;
	 		tempQ.w = -tempQ.w;
			QuaternionToMatrix(&tempQ,(MATRIX *)transmat);
			PushMatrix(&transmat[0][0]);
		}
		*/

		QuaternionToMatrix(&ptr->qRot,(MATRIX *)rotmat);
		PushMatrix(&rotmat[0][0]);

		actorScale.v[X] = ptr->scale.v[X];
		actorScale.v[Y] = ptr->scale.v[Y];
		actorScale.v[Z] = ptr->scale.v[Z];

		parentScaleStackLevel = 0;
		parentScaleStack[parentScaleStackLevel].v[X] = parentScaleStack[parentScaleStackLevel].v[Y] = parentScaleStack[parentScaleStackLevel].v[Z] = 1;

		if((ptr->animation) && (ptr->animation->anims))
			animTime = ptr->animation->animTime;
		else
			animTime = 0;
	}

	xluOverride = ptr->xluOverride;

	currentDrawActor = ptr;
	if(ptr->objectController->object)
		TransformAndDrawObject(ptr->objectController->object, animTime, 0, 0);
	
	if((calcMtx == TRUE) || (staticObj == FALSE))
	{
		PopMatrix();
		PopMatrix();

		/*
		if((ptr->stats) && (ptr->stats->bounceSpeed))
		{
			PopMatrix();
			PopMatrix();
		}
		*/
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	:  
	Returns 	: 
	Info 		:
*/
void TransformAndDrawObject(OBJECT *obj, float time, short animStart, short animEnd)
{
	QUATERNION	tempQuat;

	float rotmat[4][4], scalemat[4][4], transmat[4][4];
	int i;
	float XTranslateAmount, YTranslateAmount, ZTranslateAmount;
	float XScaleAmount = 1, YScaleAmount = 1, ZScaleAmount = 1;
	SPRITE *sprite;

	short	fromKey = -1, toKey = -1, low, high;
	VECTOR	tempVect;
	float	tempFloat;
	int		xluVal;

//handle position keyframes

	if((calcMtx == TRUE) || (staticObj == FALSE))
	{

		if(obj->numMoveKeys > 1)
		{

			i = 0;
			fromKey = toKey = -1;

			low=0;
			high=obj->numMoveKeys-1;

			while(low<=high)
			{
				i= (low+high) >> 1;

				if (obj->moveKeys[i].time==time)
				{
					break;
				}
				else
				{
					if (obj->moveKeys[i].time>time)
					{
						high = i-1;
					}
					else
					{
						low = i+1;
					}
				}
			}
			
			if (low>0) low--;
			for(i = low; i < obj->numMoveKeys; i++)
			{
				if(obj->moveKeys[i].time > time)
				{
					toKey = i;
					fromKey = i - 1;
					break;
				}
			}						


			if(fromKey != -1)		//time is within valid keyframes
			{
				tempFloat = abs(obj->moveKeys[toKey].time - obj->moveKeys[fromKey].time);
				tempFloat = (time - (float)obj->moveKeys[fromKey].time) / tempFloat;

			}
			else					// must be between last and first keyframe
			{
				fromKey = obj->numMoveKeys-1;
				toKey = fromKey;
				tempFloat = 1;

			}

			tempVect.v[X] = obj->moveKeys[fromKey].u.vect.v[X] + ((obj->moveKeys[toKey].u.vect.v[X] - obj->moveKeys[fromKey].u.vect.v[X]) *  tempFloat);
			tempVect.v[Y] = obj->moveKeys[fromKey].u.vect.v[Y] + ((obj->moveKeys[toKey].u.vect.v[Y] - obj->moveKeys[fromKey].u.vect.v[Y]) *  tempFloat);
			tempVect.v[Z] = obj->moveKeys[fromKey].u.vect.v[Z] + ((obj->moveKeys[toKey].u.vect.v[Z] - obj->moveKeys[fromKey].u.vect.v[Z]) *  tempFloat);
			
			
			XTranslateAmount = tempVect.v[X];
			YTranslateAmount = tempVect.v[Y];
			ZTranslateAmount = tempVect.v[Z];

		}
		else
		{
			XTranslateAmount = (obj->moveKeys[0].u.vect.v)[X];
			YTranslateAmount = (obj->moveKeys[0].u.vect.v)[Y];
			ZTranslateAmount = (obj->moveKeys[0].u.vect.v)[Z];
		}

	// handle scale keyframes
		if(obj->numScaleKeys > 1)
		{

			i = 0;
			fromKey = toKey = -1;

			low=0;
			high=obj->numScaleKeys-1;

			while(low<=high)
			{
				i= (low+high) >> 1;

				if (obj->scaleKeys[i].time==time)
				{
					break;
				}
				else
				{
					if (obj->scaleKeys[i].time>time)
					{
						high = i-1;
					}
					else
					{
						low = i+1;
					}
				}
			}
			
			if (low>0) low--;
			for(i = low; i < obj->numScaleKeys; i++)
			{
				if(obj->scaleKeys[i].time > time)
				{
					toKey = i;
					fromKey = i - 1;
					break;
				}
			}						





			if(fromKey != -1)		//time is within valid keyframes
			{
				tempFloat = abs(obj->scaleKeys[toKey].time - obj->scaleKeys[fromKey].time);
				tempFloat = (time - (float)obj->scaleKeys[fromKey].time) / tempFloat;

			}
			else					// must be between last and first keyframe
			{

				fromKey = obj->numScaleKeys-1;
				toKey = fromKey;
				tempFloat = 1;

			}

			tempVect.v[X] = obj->scaleKeys[fromKey].u.vect.v[X] + ((obj->scaleKeys[toKey].u.vect.v[X] - obj->scaleKeys[fromKey].u.vect.v[X]) *  tempFloat);
			tempVect.v[Y] = obj->scaleKeys[fromKey].u.vect.v[Y] + ((obj->scaleKeys[toKey].u.vect.v[Y] - obj->scaleKeys[fromKey].u.vect.v[Y]) *  tempFloat);
			tempVect.v[Z] = obj->scaleKeys[fromKey].u.vect.v[Z] + ((obj->scaleKeys[toKey].u.vect.v[Z] - obj->scaleKeys[fromKey].u.vect.v[Z]) *  tempFloat);
			
			XScaleAmount = tempVect.v[0];
			YScaleAmount = tempVect.v[1];
			ZScaleAmount = tempVect.v[2];


		}
		else
		{
			XScaleAmount = obj->scaleKeys[0].u.vect.v[0];
			YScaleAmount = obj->scaleKeys[0].u.vect.v[1];
			ZScaleAmount = obj->scaleKeys[0].u.vect.v[2];
		}
	// handle rotation keyframes
		if(obj->numRotateKeys > 1)
		{
//			if(obj->flags & OBJECT_FLAGS_CONSTANIM)
//				time = 1 + (frameCount MOD 40);

			i = 0;
			fromKey = toKey = -1;

			low=0;
			high=obj->numRotateKeys-1;

			while(low<=high)
			{
				i= (low+high) >> 1;

				if (obj->rotateKeys[i].time==time)
				{
					break;
				}
				else
				{
					if (obj->rotateKeys[i].time>time)
					{
						high = i-1;
					}
					else
					{
						low = i+1;
					}
				}
			}
			
			if (low>0) low--;
			for(i = low; i < obj->numRotateKeys; i++)
			{
				if(obj->rotateKeys[i].time > time)
				{
					toKey = i;
					fromKey = i - 1;
					break;
				}
			}						


			if(fromKey != -1)		//time is within valid keyframes
			{
				tempFloat = abs(obj->rotateKeys[toKey].time - obj->rotateKeys[fromKey].time);
				tempFloat = (time - (float)obj->rotateKeys[fromKey].time) / tempFloat;

			}
			else					// must be between last and first keyframe
			{

				fromKey = obj->numRotateKeys-1;
				toKey = fromKey;
				tempFloat = 1;



			}

			QuatSlerp(&obj->rotateKeys[fromKey].u.quat, &obj->rotateKeys[toKey].u.quat, tempFloat, &tempQuat);

	//convert back to rotation matrix
			QuaternionToMatrix(&tempQuat, (MATRIX *)rotmat);
//			if((obj->collSphere) && (obj->collSphere->quatTag))
//				QuaternionMultiply(&TagToFirstPlatform(obj->collSphere->quatTag)->actor.qRot,&currentDrawActor->qRot,&tempQuat);
		}
		else
		{
			QuaternionToMatrix(&obj->rotateKeys[0].u.quat, (MATRIX *)rotmat);
//			if((obj->collSphere) && (obj->collSphere->quatTag))
//			{
//				SetQuaternion(&TagToFirstPlatform(obj->collSphere->tag)->actor.qRot,&obj->rotateKeys[0].u.quat);
//				QuaternionMultiply(&TagToFirstPlatform(obj->collSphere->quatTag)->actor.qRot,&currentDrawActor->qRot,&obj->rotateKeys[0].u.quat);
//			}
		}

	
	    guTranslateF(transmat, XTranslateAmount * actorScale.v[X] * parentScaleStack[parentScaleStackLevel].v[X],
							   YTranslateAmount * actorScale.v[Y] * parentScaleStack[parentScaleStackLevel].v[Y],
							   ZTranslateAmount * actorScale.v[Z] * parentScaleStack[parentScaleStackLevel].v[Z]);

		//concatinate rotation and translate matrices quickly
		rotmat[3][0] = transmat[3][0];
		rotmat[3][1] = transmat[3][1];
		rotmat[3][2] = transmat[3][2];
		rotmat[3][3] = transmat[3][3];

		PushMatrix(&rotmat[0][0]);

//calculate objects scale matrix
	    guScaleF(scalemat, XScaleAmount * actorScale.v[X], YScaleAmount * actorScale.v[Y], ZScaleAmount * actorScale.v[Z]);
		PushMatrix(&scalemat[0][0]);
	}

	// maintain position of all objects sprites
	if(obj->numSprites > 0)
	{
		for(i=0; i<obj->numSprites; i++)
		{
			sprite = obj->sprites[i].sprite;
			if(sprite)
			{
				obj->sprites[i].sprite->scaleX = (float)obj->sprites[i].sx * actorScale.v[X] * XScaleAmount;
				obj->sprites[i].sprite->scaleY = (float)obj->sprites[i].sy * actorScale.v[Y] * YScaleAmount;
				guMtxXFMF(matrixStack.stack[matrixStack.stackPosition],obj->sprites[i].x,obj->sprites[i].y,obj->sprites[i].z,
							&sprite->pos.v[X],&sprite->pos.v[Y],&sprite->pos.v[Z]);
			}
		}
	}
	
	changeRenderMode = 0;

		if(!aiSurf)
		{
			if(obj->flags & OBJECT_FLAGS_TRANSPARENT)
			{
				if(transparentSurf == FALSE)
				{
					transparentSurf = TRUE;
					changeRenderMode = TRUE;
				}
			}
			else
			{
				if(transparentSurf == TRUE)
				{
					transparentSurf = FALSE;
					changeRenderMode = TRUE;
				}
			}

			if(obj->flags & OBJECT_FLAGS_XLU)
			{
				if(xluSurf == FALSE)
				{
					xluSurf = TRUE;
					changeRenderMode = 1;
				}
				if(obj->drawList == 0)
					xluSurf = TRUE;

				tempFloat = (float)xluOverride / 100;
				tempFloat *= (float)obj->xlu;
				if(tempFloat > 255)
					tempFloat = 255;
				if((int)tempFloat != xluFact)
					changeRenderMode = 1;
				xluFact = tempFloat;
			}
			else
			{
				if(xluSurf == TRUE)
				{
					xluSurf = FALSE;
					changeRenderMode = 1;
				}
			}

			if(changeRenderMode)
				SetRenderMode();
		}
		else
		{
			gDPSetPrimColor(glistp++,0,0,/*r*/255,/*g*/255,/*b*/255,obj->xlu);
		}


//here we need the result of our matrix stack 
	if((calcMtx == TRUE) || (staticObj == FALSE))
	{
		ComputeResultMatrix();
		guMtxF2L(matrixStack.result, &dynamicp->modeling4[objectMatrix]);
	

		//store the static mtx
		if(calcMtx == TRUE)
		{
			if(staticObjectMtx)
			{
//				SetMatrix((float *)staticObjectMtx, (float *)&dynamicp->modeling4[objectMatrix]);
				guMtxF2L(matrixStack.result, staticObjectMtx);
			}
		}
	
  gSPMatrix(glistp++, 
		 OS_K0_TO_PHYSICAL(&(dynamicp->modeling4[objectMatrix++])),
		 G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
		
		matrixStackCount++;
	}
	else
	{
//		guMtxF2L(unitMatrix, &dynamicp->modeling4[objectMatrix]);
		SetMatrix((float *)&dynamicp->modeling4[objectMatrix], (float *)staticObjectMtx);
//		memcpy(&dynamicp->modeling4[objectMatrix], staticObjectMtx, sizeof(Mtx));

	    gSPMatrix(glistp++, 
		OS_K0_TO_PHYSICAL(&(dynamicp->modeling4[objectMatrix++])),
		G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
		matrixStackCount++;

	}

		if(obj->drawList)
		{
			gSPDisplayList(glistp++, obj->drawList);
		}
		else if(obj->mesh->numFaces)
		{
			if(obj->flags & OBJECT_FLAGS_GOURAUD_SHADED)
			{
				if(!(obj->flags & OBJECT_FLAGS_PRELIT))
				{
					gSPSetGeometryMode(glistp++, G_LIGHTING);
				}
				else
				{
				    gSPClearGeometryMode(glistp++, G_LIGHTING);
				}
				WriteObjectDisplayListGouraud(obj);
			}
			else
			{
				if(!(obj->flags & OBJECT_FLAGS_PRELIT))
				{
					gSPSetGeometryMode(glistp++, G_LIGHTING);
				}
				else
				{
				    gSPClearGeometryMode(glistp++, G_LIGHTING);
				}
				WriteObjectDisplayListFlat(obj);
			}
		}

		
		gSPPopMatrix(glistp++, G_MTX_MODELVIEW);
		matrixStackCount--;

	if((calcMtx == TRUE) || (staticObj == FALSE))
	{

		PopMatrix();

//transform children
		if(obj->children)
		{
			parentScaleStackLevel++;
			parentScaleStack[parentScaleStackLevel].v[X] = XScaleAmount/* * actorScale.v[X]*/;
			parentScaleStack[parentScaleStackLevel].v[Y] = YScaleAmount/* * actorScale.v[Y]*/;
			parentScaleStack[parentScaleStackLevel].v[Z] = ZScaleAmount/* * actorScale.v[Z]*/;
			TransformAndDrawObject(obj->children, time, animStart, animEnd);
			parentScaleStackLevel--;
		}

		PopMatrix();

		if(obj->next)
		{
			TransformAndDrawObject(obj->next, time, animStart, animEnd);
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: DeformTextureCoords
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void DeformTextureCoords(OBJECT *obj)
{
	int i,j,plus;
	USHORT2DVECTOR *texCoord, *oTexCoord;
	VECTOR	*vectP = obj->mesh->vertices;
	short	voffset = 0;
		
	texCoord = obj->mesh->faceTC;
	oTexCoord = obj->mesh->originalFaceTC;

	for(i = 0; i < obj->mesh->numFaces; i++)
	{
		for(j = 0;j < 3;j++)
		{
			voffset = obj->mesh->faceIndex[i].v[j];
			vectP = obj->mesh->vertices + voffset;
			plus = SineWave(10, (short)(vectP->v[X]+vectP->v[Y]+vectP->v[Z]),frameCount)*16;
			texCoord->v[X] += plus; 
			plus = SineWave(10, (short)(vectP->v[X]+vectP->v[Y]+vectP->v[Z])/2,frameCount)*16;
			texCoord->v[Y] += plus;
			texCoord++;
			oTexCoord++;
		}
	}
}
