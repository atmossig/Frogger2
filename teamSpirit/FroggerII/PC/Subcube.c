/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.
	
		File		: Subcube.c
		Programmer	: Andy Eder
		Date		: 15/09/99 14:43:27
		
		 ----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

#include <ultra64.h>

#include "incs.h"

#define LODDist (700 * 700)

void TransformObject(OBJECT *obj, float time);
void PCDrawObject(OBJECT *obj, float m[4][4]);

void PCPrepareObject(OBJECT *obj, MESH *mesh, float m[4][4]);
void PCRenderObject(OBJECT *obj);
void PCPrepareSkinnedObject(OBJECT *obj, MESH *mesh, float m[4][4]);

///////////////////////////////////////////////////////////////////////////////////////////

long TestDistanceFromFrog = 0;
float hedRotAmt = 0;

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

float nearClip = 10;
float farClip = 1100;

float horizClip = 600;
float vertClip = 550;

long DIST=0;
long FOV=512;

#define IsNotClipped(vTemp2) (((vTemp2->v[Z]+DIST)>nearClip) && ((noClipping) || (((vTemp2->v[Z]+DIST)<farClip) && ((vTemp2->v[X])>-horizClip) && ((vTemp2->v[X])<horizClip) && ((vTemp2->v[Y])>-vertClip) && ((vTemp2->v[Y])<vertClip))))

///////////////////////////////////////////////////////////////////////////////////////////

VECTOR tV[8000];
float mV[8000];

float sinSpeedWater = 0.2;
float sinSpeedWater2 = 0.1;
float sinSpeedVert = 0.4;
float sinSpeedVert2 = 0.4;
float scVC = 0.02;
float valWater = 2;
float rval = 50000;
float rval2 = 15;
float svall = 0.03;
float fmody = 0.4,fmody2 = 0.9;
float sclW = 1;
float thresh = 900;
float sofs = 5;

long noClipping = 0;

///////////////////////////////////////////////////////////////////////////////////////////

short	xluOverride = 0;
Mtx		*staticObjectMtx;

ACTOR	*currentDrawActor;
OBJECT_CONTROLLER *currentObjectController;
VECTOR	*actorScale;

VECTOR	parentScaleStack[10];
int		parentScaleStackLevel = 0;

float rotmat[4][4], scalemat[4][4], transmat[4][4];




// Outcode enum
enum {OUTCODE_LEFT, OUTCODE_RIGHT, OUTCODE_TOP, OUTCODE_BOTTOM};

// Outcode macros for clipping.
#define OUTCODEF_LEFT	(1<<OUTCODE_LEFT)
#define OUTCODEF_RIGHT	(1<<OUTCODE_RIGHT)
#define OUTCODEF_TOP	(1<<OUTCODE_TOP)
#define OUTCODEF_BOTTOM	(1<<OUTCODE_BOTTOM)

// Calculate the actual outcode
#define CALC_OUTCODE(x,y,x0,y0,x1,y1) \
						((((y)>=(y1))<<OUTCODE_BOTTOM)| \
						 (((y)<=(y0))<<OUTCODE_TOP)| \
						 (((x)>=(x1))<<OUTCODE_RIGHT)| \
						 (((x)<=(x0))<<OUTCODE_LEFT))

// The 2D clip Volume.
float clx0 = 1,
      cly0 = 1,
	  clx1 = 639,
	  cly1 = 479;

// Subtracted Square!
//#define sbsqr(x,y) ((x-y)*(x-y))

float xl = 1;

/*---------------------------------------------------------------------------------------------
	Function	: calcIntVertex
	Parameters	: (D3DTLVERTEX *vOut, int outcode, D3DTLVERTEX *v0,D3DTLVERTEX *v1, float cx0, float cy0, float cx1, float cy1)
	Returns		: inline int 
*/

float   rCol,gCol,bCol;

int calcIntVertex(D3DTLVERTEX *vOut, int outcode, D3DTLVERTEX *v0,D3DTLVERTEX *v1, float cx0, float cy0, float cx1, float cy1)
{
	float segLen, totalLen, vt;
	long r1,g1,b1,a1;
	long r2,g2,b2,a2;

	switch(outcode)
	{
		case OUTCODE_LEFT:
			segLen = cx0-v0->sx;
			totalLen = v1->sx-v0->sx;
			vt = segLen/totalLen;
			vOut->sx = cx0;
			vOut->sy = v0->sy+((v1->sy-v0->sy)*vt);
			break;
		case OUTCODE_RIGHT:
			segLen = cx1-v0->sx;
			totalLen = v1->sx-v0->sx;
			vt = segLen/totalLen;
			vOut->sx = cx1;
			vOut->sy = v0->sy+((v1->sy-v0->sy)*vt);
			break;
		case OUTCODE_TOP:
			segLen = cy0-v0->sy;
			totalLen = v1->sy-v0->sy;
			vt = segLen/totalLen;
			vOut->sx = v0->sx+((v1->sx-v0->sx)*vt);
			vOut->sy = cy0;
			break;
		case OUTCODE_BOTTOM:
			segLen = cy1-v0->sy;
			totalLen = v1->sy-v0->sy;
			vt = segLen/totalLen;
			vOut->sx = v0->sx+((v1->sx-v0->sx)*vt);
			vOut->sy = cy1;
			break;		
	}
			
	vOut->tu = (v0->tu+((v1->tu-v0->tu)*vt));
    vOut->tv = (v0->tv+((v1->tv-v0->tv)*vt));
	vOut->sz = (v0->sz+((v1->sz-v0->sz)*vt));
	
	a1 = RGBA_GETALPHA(v0->color);
	r1 = RGBA_GETRED(v0->color);
	g1 = RGBA_GETGREEN(v0->color);
	b1 = RGBA_GETBLUE(v0->color);

	a2 = RGBA_GETALPHA(v1->color);
	r2 = RGBA_GETRED(v1->color);
	g2 = RGBA_GETGREEN(v1->color);
	b2 = RGBA_GETBLUE(v1->color);

	vOut->color = RGBA_MAKE ((long)(r1+(r2-r1)*vt),(long)(g1+(g2-g1)*vt),(long)(b1+(b2-b1)*vt),(long)(a1+(a2-a1)*vt));

	vOut->specular = 0;
	return !((vOut->sx==v0->sx)&&(vOut->sy==v0->sy));
}

void Clip3DPolygon (D3DTLVERTEX in[3], long texture)
{
	D3DTLVERTEX		vList[10];
	int				out0, out1;
	D3DTLVERTEX 	*v0ptr, *v1ptr, vBuf1[8], vBuf2[8], *vIn, *vOut, *vTmp;
	int				vInCount, vOutCount;
	int				sideLp, vertLp, sideMask;
	short			faceList[8*3];
	long			numFaces,i,j;
	// Set up in & out buffers											

	vIn = vBuf1;
	
	memcpy (vIn,in,sizeof(D3DTLVERTEX)*3);
	//vIn[0].sx = x1;
	//vIn[0].sy = y1;
	//vIn[1].sx = x2;
	//vIn[1].sy = y2;
	//vIn[2].sx = x3;
	//vIn[2].sy = y3;
	
	//vIn[0].tu = u1;
	//vIn[0].tv = v1;
	//vIn[1].tu = u2;
	//vIn[1].tv = v2;
	//vIn[2].tu = u3;
	//vIn[2].tv = v3;
	
	vInCount = 3;
	vOut = vBuf2;
	
	vOutCount = 0;

	for(sideLp=0; sideLp<4; sideLp++)
	{
		sideMask = 1<<sideLp;
		for(vertLp=0; vertLp<vInCount; vertLp++)
		{
			v0ptr = &vIn[vertLp];
			if ((vertLp+1)<vInCount)
				v1ptr = &vIn[(vertLp+1)];
			else
				v1ptr = vIn;

			out0 = CALC_OUTCODE(v0ptr->sx,v0ptr->sy, clx0,cly0,clx1,cly1);
			out1 = CALC_OUTCODE(v1ptr->sx,v1ptr->sy, clx0,cly0,clx1,cly1);
			if ((out0 & sideMask)==0)		// v0 on
			{
				// add v0 to output
				memcpy(&vOut[vOutCount++], v0ptr, sizeof(D3DTLVERTEX));
				if (out1 & sideMask)		// v0 on, v1 off
				{
					// add intersection to output
					if (calcIntVertex(&vOut[vOutCount], sideLp, v0ptr, v1ptr, clx0,cly0,clx1,cly1))
						vOutCount++;
				}
			}
			else
			{
				if ((out1 & sideMask)==0)	// v0 off, v1 on
				{
					// add intersection to output
					if (calcIntVertex(&vOut[vOutCount], sideLp, v1ptr, v0ptr, clx0,cly0,clx1,cly1))
						vOutCount++;
				}
			}
		}
		vTmp = vIn;					// Swap in & out bufs
		vIn = vOut;
		vOut = vTmp;
		vInCount = vOutCount;
		vOutCount = 0;
	}

	numFaces = 0;
	j=0;
	for (i=1; i<(vInCount-1); i++, j+=3)
	{
		faceList[j] = 0;
		faceList[j+1] = i;
		faceList[j+2] = i+1;
		numFaces++;
	}
	
	if (numFaces)
	{
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZENABLE,1);
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,1);
		DrawAHardwarePoly(vIn,vInCount,faceList,j,texture);
	}

/*	xb = vIn[0].sx;
	yb = vIn[0].sy;
	ub = vIn[0].tu;
	vb = vIn[0].tv;

	for (int i=1; i<(vInCount-1); i++)
	{
		DrawPolygon (
			xb, yb,
				ub, vb,
			vIn[i].sx, vIn[i].sy,
				vIn[i].tu, vIn[i].tv,
			vIn[i+1].sx, vIn[i+1].sy, 
				vIn[i+1].tu, vIn[i+1].tv);
	}*/

}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

void DrawObject(OBJECT *obj, Gfx *drawList, int skinned, MESH *masterMesh)
{
	if (skinned)
	{
		PCPrepareSkinnedObject(obj, masterMesh,  obj->objMatrix.matrix);
	}
	else
	{
		if (obj->mesh)
		{
			PCPrepareObject(obj, obj->mesh,  obj->objMatrix.matrix);
			PCRenderObject(obj);
		}
	}

	if(obj->children)
		DrawObject(obj->children, obj->children->drawList, skinned, masterMesh);

	if(obj->next)
		DrawObject(obj->next, obj->next->drawList, skinned, masterMesh);
}




























































































































/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void FindToFromVKeys(VKEYFRAME *keys,short *from,short *to,float *interp,float time,int numKeys)
{
	int			i;
	int			low,high;
	short		fromKey = -1, toKey;
	float		tempFloat;

	low = 0;
	high = numKeys - 1;

	while(low <= high)
	{
		i = (low + high)>>1;
		if(keys[i].time == time)
		{
			if(i == numKeys - 1)
			{
				fromKey = -2;
			}
			else
			{
				fromKey = i;
				toKey = i + 1;
			}
			break;
		}
		else
		{
			if(keys[i].time > time)
			{
				high = i - 1;
			}
			else
			{
				low = i + 1;
			}
		}
	}

	if((fromKey == -1) && (high+1 < numKeys) && (keys[high + 1].time > time))
	{
		fromKey = high;
		toKey = high + 1;
	}

	if(fromKey >= 0)		//time is within valid keyframes
	{
		tempFloat = abs(keys[toKey].time - keys[fromKey].time);
		*interp = (time - (float)keys[fromKey].time) / tempFloat;
	}
	else					// must be between last and first keyframe
	{
		fromKey = numKeys-1;
		toKey = fromKey;
		*interp = 1;
	}
	*from = fromKey;
	*to = toKey;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void FindToFromQKeys(QKEYFRAME *keys,short *from,short *to,float *interp,float time,int numKeys)
{
	int			i;
	int			low,high;
	short		fromKey = -1, toKey;
	float		tempFloat;

	low = 0;
	high = numKeys - 1;

	while(low <= high)
	{
		i = (low + high)>>1;
		if(keys[i].time == time)
		{
			if(i == numKeys - 1)
			{
				fromKey = -2;
			}
			else
			{
				fromKey = i;
				toKey = i + 1;
			}
			break;
		}
		else
		{
			if(keys[i].time > time)
			{
				high = i - 1;
			}
			else
			{
				low = i + 1;
			}
		}
	}

	if((fromKey == -1) && (high+1 < numKeys) && (keys[high + 1].time > time))
	{
		fromKey = high;
		toKey = high + 1;
	}

	if(fromKey >= 0)		//time is within valid keyframes
	{
		tempFloat = abs(keys[toKey].time - keys[fromKey].time);
		*interp = (time - (float)keys[fromKey].time) / tempFloat;
	}
	else					// must be between last and first keyframe
	{
		fromKey = numKeys-1;
		toKey = fromKey;
		*interp = 1;
	}
	*from = fromKey;
	*to = toKey;
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

void XfmPoint (VECTOR *vTemp2, VECTOR *in)
{
	float c[4][4];
	guLookAtF(c,
			currCamTarget[screenNum].v[X],currCamTarget[screenNum].v[Y],currCamTarget[screenNum].v[Z],
			currCamSource[screenNum].v[X],currCamSource[screenNum].v[Y],currCamSource[screenNum].v[Z],
			camVect.v[X],camVect.v[Y],camVect.v[Z]);

	guMtxXFMF(c,in->v[X],in->v[Y],in->v[Z],
		&(vTemp2->v[X]),&(vTemp2->v[Y]),&(vTemp2->v[Z]));
	
		if  (((vTemp2->v[Z]+DIST)>nearClip) &&
			((vTemp2->v[Z]+DIST)<farClip) &&
			((vTemp2->v[X])>-horizClip) &&
			((vTemp2->v[X])<horizClip) &&
			((vTemp2->v[Y])>-vertClip) &&
			((vTemp2->v[Y])<vertClip))
		{
			float oozd = -1/(vTemp2->v[Z]+DIST);
			vTemp2->v[X] = 320+((vTemp2->v[X] * FOV) * oozd);
			vTemp2->v[Y] = 220+((vTemp2->v[Y] * FOV) * oozd);
		}
		else
			vTemp2->v[Z] = 0;
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
		//memcpy(tempActor->shadow->vert,shadowVtx,sizeof(Vtx) * 4);
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

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void XformActor(ACTOR *actor)
{
	OBJECT_CONTROLLER *objectC = actor->objectController;
	float animTime;

	if((actor->xluOverride == 0) || (objectC->object->xlu == 0))
	{
		actor->visible = 0;
		return;
	}

	if((actor->matrix) || (actor->objectController->objectSize == 1))
	{
		actor->visible = TRUE;
		return;
	}
	
	UpdateAnimations(actor);

	actor->visible = TRUE;

	xluOverride = actor->xluOverride;

//	numActorsXformed++;
	staticObjectMtx = actor->matrix;
	currentDrawActor = actor;
	currentObjectController = objectC;
	actorScale = &actor->scale;

	if((actor->animation) && (actor->animation->anims))
		animTime = actor->animation->animTime;
	else
		animTime = 0;

	parentScaleStackLevel = 0;
	parentScaleStack[parentScaleStackLevel].v[X] = parentScaleStack[parentScaleStackLevel].v[Y] = parentScaleStack[parentScaleStackLevel].v[Z] = 1;

/* MATT
	if(objectC->drawList)	//if object is skinned, transform mesh before aplying object TM
	{
		if(objectC->object)
		{
			if((actor->LODObjectController) && (actor->distanceFromCamera - actor->maxRadius > (LODDist)))
				TransformSkinnedObject(actor->LODObjectController->object, animTime);
			else
				TransformSkinnedObject(objectC->object, animTime);
		}
	}
*/
	guTranslateF(transmat, actor->pos.v[X], actor->pos.v[Y], actor->pos.v[Z]);
	PushMatrix(transmat);

	QuaternionToMatrix(&actor->qRot,(MATRIX *)rotmat);
	PushMatrix(rotmat);

	if(objectC->object)
		TransformObject(objectC->object, animTime);

/* MATT
	if(objectC->drawList == NULL)
	{
		if(objectC->object)
		{
			if((actor->LODObjectController) && (actor->distanceFromCamera - actor->maxRadius > (LODDist)))
				TransformObject(actor->LODObjectController->object, animTime);
			else
				TransformObject(objectC->object, animTime);
		}
	}
	
	else
	{
		//skinned objects need to store the TM, including the scale which can't be done at vertex xform stage due to interer inaccuracies
		guScaleF(scalemat,actor->scale.v[X],actor->scale.v[Y],actor->scale.v[Z]);
		PushMatrix(scalemat);
//MATT		guMtxF2L(matrixStack.stack[matrixStack.stackPosition], &objectC->object->objMatrix);
		PopMatrix();
	}
*/

  	parentScaleStackLevel = 0;
	parentScaleStack[parentScaleStackLevel].v[X] = parentScaleStack[parentScaleStackLevel].v[Y] = parentScaleStack[parentScaleStackLevel].v[Z] = 1;
	
	matrixStack.stackPosition-=2;
//	PopMatrix();
//	PopMatrix();
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
QUATERNION	tempQuat,morphFromQuat,morphToQuat,morphResultQuat;
void TransformObject(OBJECT *obj, float time)
{
	VECTOR translation,scale;
	VECTOR	morphFrom,morphTo;
	SPRITE	*sprite;
	float	tempFloat,interp;
	short i, j;
	short	fromKey, toKey;
	short	xluVal;

//handle position keyframes
	if((obj->numMoveKeys > 1) && (currentDrawActor->animation))
	{
		if(currentDrawActor->animation->numMorphFrames)
		{
			// Find from vector
			FindToFromVKeys(obj->moveKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphFrom,obj->numMoveKeys);
			LinearInterp(&morphFrom,&obj->moveKeys[fromKey].vect,&obj->moveKeys[toKey].vect,interp);

			// Find to vector
			FindToFromVKeys(obj->moveKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphTo,obj->numMoveKeys);
			LinearInterp(&morphTo,&obj->moveKeys[fromKey].vect,&obj->moveKeys[toKey].vect,interp);

			// Interp between from and to vectors
			tempFloat = (float)currentDrawActor->animation->currentMorphFrame/(float)(Fabs(currentDrawActor->animation->numMorphFrames));
			LinearInterp(&translation,&morphFrom,&morphTo,tempFloat);
		}
		else
		{
			FindToFromVKeys(obj->moveKeys,&fromKey,&toKey,&interp,time,obj->numMoveKeys);
			LinearInterp(&translation,&obj->moveKeys[fromKey].vect,&obj->moveKeys[toKey].vect,interp);
		}
	}
	else
	{
		SetVector(&translation,&obj->moveKeys[0].vect);
	}
	if((obj->numScaleKeys > 1) && (currentDrawActor->animation))
	{
		if(currentDrawActor->animation->numMorphFrames)
		{
			// Find from vector
			FindToFromVKeys(obj->scaleKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphFrom,obj->numScaleKeys);
			LinearInterp(&morphFrom,&obj->scaleKeys[fromKey].vect,&obj->scaleKeys[toKey].vect,interp);

			// Find to vector
			FindToFromVKeys(obj->scaleKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphTo,obj->numScaleKeys);
			LinearInterp(&morphTo,&obj->scaleKeys[fromKey].vect,&obj->scaleKeys[toKey].vect,interp);

			// Slerp between from and to vectors
			tempFloat = (float)currentDrawActor->animation->currentMorphFrame/(float)(Fabs(currentDrawActor->animation->numMorphFrames));
			LinearInterp(&scale,&morphFrom,&morphTo,tempFloat);
		}
		else
		{
			FindToFromVKeys(obj->scaleKeys,&fromKey,&toKey,&interp,time,obj->numScaleKeys);
			LinearInterp(&scale,&obj->scaleKeys[fromKey].vect,&obj->scaleKeys[toKey].vect,interp);
		}
	}
	else
	{
		SetVector(&scale,&obj->scaleKeys[0].vect);
	}
	// handle rotation keyframes
	if((obj->numRotateKeys > 1) && (currentDrawActor->animation))
	{
		if(currentDrawActor->animation->numMorphFrames)
		{
			// Find from vector
			FindToFromQKeys(obj->rotateKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphFrom,obj->numRotateKeys);
			QuatSlerp(&obj->rotateKeys[fromKey].quat, &obj->rotateKeys[toKey].quat, interp, &morphFromQuat);

			// Find to vector
			FindToFromQKeys(obj->rotateKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphTo,obj->numRotateKeys);
			QuatSlerp(&obj->rotateKeys[fromKey].quat, &obj->rotateKeys[toKey].quat, interp, &morphToQuat);

			// Slerp between from and to vectors
			tempFloat = (float)currentDrawActor->animation->currentMorphFrame/(float)(Fabs(currentDrawActor->animation->numMorphFrames));
			QuatSlerp(&morphFromQuat, &morphToQuat, tempFloat, &morphResultQuat);

			QuaternionToMatrix(&morphResultQuat, (MATRIX *)rotmat);
		}
		else
		{
			FindToFromQKeys(obj->rotateKeys,&fromKey,&toKey,&interp,time,obj->numRotateKeys);
			QuatSlerp(&obj->rotateKeys[fromKey].quat, &obj->rotateKeys[toKey].quat, interp, &tempQuat);

			// convert back to rotation matrix
			QuaternionToMatrix(&tempQuat, (MATRIX *)rotmat);
		}
	}
	else
	{
		QuaternionToMatrix(&obj->rotateKeys[0].quat, (MATRIX *)rotmat);
	}

	if (strncmp(obj->name,"fghed",5) == 0)
	{
		float rotmat2[4][4];
		QUATERNION quat, rot = {0,1,0,hedRotAmt};
		GetQuaternionFromRotation (&quat,&rot);
		QuaternionToMatrix(&quat, (MATRIX*)rotmat2);
		guMtxCatF(rotmat,rotmat2,rotmat);
	}

	rotmat[3][0] = translation.v[X] * actorScale->v[X] * parentScaleStack[parentScaleStackLevel].v[X];
	rotmat[3][1] = translation.v[Y] * actorScale->v[Y] * parentScaleStack[parentScaleStackLevel].v[Y];
	rotmat[3][2] = translation.v[Z] * actorScale->v[Z] * parentScaleStack[parentScaleStackLevel].v[Z];

	PushMatrix(rotmat);
	guScaleF(scalemat, scale.v[X] * actorScale->v[X], scale.v[Y] * actorScale->v[Y], scale.v[Z] * actorScale->v[Z]);

	PushMatrix(scalemat);
	

	//if the object has a collision sphere
/*	if(obj->collSphere)
	{
		SetVector(&obj->collSphere->oldOffset, &obj->collSphere->offset);
		guMtxXFMF(matrixStack.stack[matrixStack.stackPosition], 0, 0, 0,&obj->collSphere->offset.v[X], &obj->collSphere->offset.v[Y], &obj->collSphere->offset.v[Z]);
	}
*/
	//maintain posision of all objects sprites        
//	if(obj->numSprites > 0)
/*	{
		for(i = 0; i < obj->numSprites; i++)
		{
			sprite = obj->sprites[i].sprite;
			if(sprite)
			{
				if((currentDrawActor->stats) && (currentDrawActor->stats->inShadow))
					sprite->r = sprite->g = sprite->b = 255-currentDrawActor->stats->inShadow;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//				sprite->r = (float)obj->sprites[i].r*inShadow;
//				sprite->g = (float)obj->sprites[i].g*inShadow;
//				sprite->b = (float)obj->sprites[i].b*inShadow;
				if(obj->flags & OBJECT_FLAGS_COLOUR_BLEND)
				{
					sprite->red2 = obj->colour.r;
					sprite->green2 = obj->colour.g;
					sprite->blue2 = obj->colour.b;
					sprite->alpha2 = obj->colour.a;
					sprite->flags |= SPRITE_FLAGS_COLOUR_BLEND;
				}
				else
					sprite->flags &= -1 - SPRITE_FLAGS_COLOUR_BLEND;
				if(obj->flags & OBJECT_FLAGS_COLOUR_BLEND_AFTERLIGHT)
				{
					sprite->red2 = obj->colour.r;
					sprite->green2 = obj->colour.g;
					sprite->blue2 = obj->colour.b;
					sprite->alpha2 = obj->colour.a;
					sprite->flags |= SPRITE_FLAGS_COLOUR_BLEND_AFTERLIGHT;
				}
				else
					sprite->flags &= -1 - SPRITE_FLAGS_COLOUR_BLEND_AFTERLIGHT;
				obj->sprites[i].sprite->scaleX = (float)obj->sprites[i].sx * actorScale->v[X] * scale.v[X];
				obj->sprites[i].sprite->scaleY = (float)obj->sprites[i].sy * actorScale->v[Y] * scale.v[Y];
				xluVal = ((int)(obj->xlu * xluOverride))/100;
				if(xluOverride <= 10)
					xluVal = 0;
				if((currentDrawActor->objectType == BALL_TYPE_OBJECT_SPAWN) || (currentDrawActor->type == DEBRIS))
				{
					obj->sprites[i].sprite->r = 0;
					obj->sprites[i].sprite->g = 0;
					obj->sprites[i].sprite->b = 0;
					obj->sprites[i].sprite->a = 254;
				}
				else
					obj->sprites[i].sprite->a = Min(255,xluVal);
				if(obj->sprites[i].sprite->a >= 255)
					obj->sprites[i].sprite->flags &= -1 - SPRITE_TRANSLUCENT;
				else
					obj->sprites[i].sprite->flags |= SPRITE_TRANSLUCENT;
				guMtxXFMF(matrixStack.stack[matrixStack.stackPosition], obj->sprites[i].x, obj->sprites[i].y, obj->sprites[i].z,
												&sprite->pos.v[X], &sprite->pos.v[Y], &sprite->pos.v[Z]);
				if(renderMode.pixelOut)
				{
					sprite->flags |= SPRITE_FLAGS_PIXEL_OUT;
					sprite->a = (sprite->a * obj->pixelOutAlpha)/255;
				}
				else
					sprite->flags &= -1 - SPRITE_FLAGS_PIXEL_OUT;
			}
		}
	}
*/

		
//MATT, required
// guMtxF2L(matrixStack.stack[matrixStack.stackPosition], &obj->objMatrix);
	SetMatrix (&obj->objMatrix.matrix,&matrixStack.stack[matrixStack.stackPosition]);
	
	PopMatrix();


//transform children
	if(obj->children)
	{
		parentScaleStackLevel++;
		SetVector(&parentScaleStack[parentScaleStackLevel],&scale);
		TransformObject(obj->children, time);
		parentScaleStackLevel--;
	}

	PopMatrix();

	if(obj->next)
	{
		TransformObject(obj->next, time);
	}
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

void DrawActor(ACTOR *actor)
{
	OBJECT_CONTROLLER *objectC = actor->objectController;
	int col;
	float animTime;

	if(actor->visible == FALSE)
		return;

	staticObjectMtx = actor->matrix;

	xluOverride = actor->xluOverride;

	currentDrawActor = actor;
	actorScale = &actor->scale;

	if((actor->animation) && (actor->animation->anims))
		animTime = actor->animation->animTime;
	else
		animTime = 0;

	/* MATT
	//quick draw
	if(objectC->objectSize == 1)
	{
		SetupRenderModeForObject(objectC->object);
		gSPDisplayList(glistp++, objectC->object->drawList);
	}
	else
	{
		if(objectC->drawList)	//if object is skinned, draw mesh now
 		{
			gSPSegment(glistp++, 1, objectC->vtx[objectC->vtxBuf]);
			DrawObject(objectC->object, objectC->drawList, TRUE);
		}
		else
			DrawObject(objectC->object, objectC->object->drawList, FALSE);
	}
	*/

	if (objectC->vtxBuf)
	{
		DrawObject(objectC->object, objectC->object->drawList, TRUE, objectC->object->mesh);
		PCRenderObject(objectC->object);
	}
	else
		DrawObject(objectC->object, objectC->object->drawList, FALSE, objectC->object->mesh);

	//DrawObject(objectC->object, objectC->object->drawList, FALSE);
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

void PCDrawObject(OBJECT *obj, float m[4][4])
{
	short fce[3] = {0,1,2};		
	D3DTLVERTEX v[3],*vTemp;
	float *mTemp;
	VECTOR *in,*c1,*c2,*c3,*vTemp2;
	float c[4][4];
	float f[4][4];
	int i;
	long drawme;
	
	in = obj->mesh->vertices;

	//if ((xl<0.99) || (xluFade<0.99))
	{
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,FALSE);
	}

	guLookAtF(c,
			currCamTarget[screenNum].v[X],currCamTarget[screenNum].v[Y],currCamTarget[screenNum].v[Z],
			currCamSource[screenNum].v[X],currCamSource[screenNum].v[Y],currCamSource[screenNum].v[Z],
			//stx,sty,stz,
			//ctx,cty,ctz,
			//0,1,0);
			camVect.v[X],camVect.v[Y],camVect.v[Z]);
	
	guMtxCatF(m,c,f);
	
	vTemp2 = tV;
	
	for (i=0; i<obj->mesh->numVertices; i++)
	{
		if (xl<0.99)
		{
			if (player[0].frogState & FROGSTATUS_ISDEAD)
			{
				player[0].lives = 5;
				if (frog[0]->action.deathBy & DEATHBY_DROWNING)
				{
					float val;
					VECTOR me,me2;
					me.v[X] = in->v[X]*sclW;
					me.v[Y] = in->v[Y]*sclW;
					me.v[Z] = in->v[Z]*sclW;

					guMtxXFMF(m,me.v[X],me.v[Y],me.v[Z],
								&me2.v[0],&me2.v[1],&me2.v[2]); 
					
					val = 1+DistanceBetweenPoints2DSquared(&destTile[0]->centre,&me2);
					if (val<thresh)
						val = thresh;
					val = rval / (val/rval2);

					val = (1+sinf(val*fmody2+frameCount*fmody)) * val * svall;
					if (frog[0]->action.dead>sofs)
						val *= ((frog[0]->action.dead-sofs) / 75.0);
					else
						val*=0;

					val += (2+(sin(in->v[Z]*sinSpeedVert+frameCount*sinSpeedWater)+cos(in->v[X]*sinSpeedVert2+frameCount*sinSpeedWater2)))*valWater;
					
					mV[i] = (2+(sin(in->v[Z]*sinSpeedVert+frameCount*sinSpeedWater)+cos(in->v[X]*sinSpeedVert2+frameCount*sinSpeedWater2)))*valWater;

					guMtxXFMF(f,in->v[X],in->v[Y] - val, in->v[Z],

					&(vTemp2->v[X]),&(vTemp2->v[Y]),&(vTemp2->v[Z]));
				}
			}
			else
			{
				mV[i] =(	(2+(sin(in->v[Z]*sinSpeedVert+frameCount*sinSpeedWater)+cos(in->v[X]*sinSpeedVert2+frameCount*sinSpeedWater2))) + 
					(2+(sin((in->v[Z]+50)*sinSpeedVert+frameCount*sinSpeedWater)+cos((in->v[X]+50)*sinSpeedVert2+frameCount*sinSpeedWater2)))
					)*valWater/2;
				guMtxXFMF(f,in->v[X],in->v[Y]-
				mV[i]
				,in->v[Z],
				&(vTemp2->v[X]),&(vTemp2->v[Y]),&(vTemp2->v[Z]));
			}
		}
		else
		{
			guMtxXFMF(f,in->v[X],in->v[Y],in->v[Z],
			&(vTemp2->v[X]),&(vTemp2->v[Y]),&(vTemp2->v[Z]));
		}
	
		if (((vTemp2->v[Z]+DIST)>nearClip) &&
			((noClipping) ||   
			(((vTemp2->v[Z]+DIST)<farClip) &&
			((vTemp2->v[X])>-horizClip) &&
			((vTemp2->v[X])<horizClip) &&
			((vTemp2->v[Y])>-vertClip) &&
			((vTemp2->v[Y])<vertClip))))
		{
			float oozd = -1/(vTemp2->v[Z]+DIST);
			vTemp2->v[X] = 320+((vTemp2->v[X] * FOV) * oozd);
			vTemp2->v[Y] = 220+((vTemp2->v[Y] * FOV) * oozd);
		}
		else
			vTemp2->v[Z] = 0;

		vTemp2++;
		in++;
		mV[i]*=scVC;
	}
	
	for (i=0; i<obj->mesh->numFaces; i++)
	{
		long v0 = obj->mesh->faceIndex[i].v[0];
		long v1 = obj->mesh->faceIndex[i].v[1];
		long v2 = obj->mesh->faceIndex[i].v[2];
		long v0a,v1a,v2a;
		TEXENTRY *tex = (TEXENTRY *)(obj->mesh->textureIDs[i]);

		v0a = i*3;
		v1a = v0a+1;
		v2a = v1a+1;

		if (tV[v0].v[Z])
		if (tV[v1].v[Z])
		if (tV[v2].v[Z])
		if (tex)
		{
			c1 = &(((VECTOR *)obj->mesh->vertexNormals)[v0a]);
			c2 = &(((VECTOR *)obj->mesh->vertexNormals)[v1a]);
			c3 = &(((VECTOR *)obj->mesh->vertexNormals)[v2a]);
			
			drawme = 1;

			vTemp = v;
			
			vTemp->specular = 0;
			vTemp->sx = tV[v0].v[X];
			vTemp->sy = tV[v0].v[Y];
			vTemp->sz = (tV[v0].v[Z]+DIST)/2000;///2000;
			vTemp->tu = (obj->mesh->faceTC[v0a].v[0]*(1.0/1024.0));
			vTemp->tv = (obj->mesh->faceTC[v0a].v[1]*(1.0/1024.0));
			vTemp->color = D3DRGBA(1,1,1,1);//D3DRGBA(c1->v[2],c1->v[1],c1->v[0],xl);
			if (xl<0.99)
			{
				vTemp->specular = D3DRGB(mV[v0],mV[v0],mV[v0]);
			}
			else
			{
				vTemp->specular = D3DRGB(rCol,gCol,bCol);
			}

			vTemp++;

			vTemp->specular = 0;
			vTemp->sx = tV[v1].v[X];
			vTemp->sy = tV[v1].v[Y];
			vTemp->sz = (tV[v1].v[Z]+DIST)/2000;//2000;
			vTemp->tu = (obj->mesh->faceTC[v1a].v[0]*(1.0/1024.0));
			vTemp->tv = (obj->mesh->faceTC[v1a].v[1]*(1.0/1024.0));
			vTemp->color = D3DRGBA(1,1,1,1);//(c2->v[2],c2->v[1],c2->v[0],xl);
			if (xl<0.99)
			{
				vTemp->specular = D3DRGB(mV[v1],mV[v1],mV[v1]);
			}
			else
			{
				vTemp->specular = D3DRGB(rCol,gCol,bCol);
			}

			vTemp++;

			vTemp->specular = 0;
			vTemp->sx = tV[v2].v[X];
			vTemp->sy = tV[v2].v[Y];
			vTemp->sz = (tV[v2].v[Z]+DIST)/2000;///2000;
			vTemp->tu = (obj->mesh->faceTC[v2a].v[0]*(1.0/1024.0));
			vTemp->tv = (obj->mesh->faceTC[v2a].v[1]*(1.0/1024.0));
			vTemp->color = D3DRGBA(1,1,1,1);//D3DRGBA(c3->v[2],c3->v[1],c3->v[0],xl);
			if (xl<0.99)
			{
				vTemp->specular = D3DRGB(mV[v2],mV[v2],mV[v2]);
			}
			else
			{
				vTemp->specular = D3DRGB(rCol,gCol,bCol);
			}
		

			if (v[0].sx <0)
				if (v[1].sx<0)
					if (v[2].sx<0)
						drawme = 0;
			
			if (v[0].sy <0)
				if (v[1].sy<0)
					if (v[2].sy<0)
						drawme = 0;

			if (v[0].sx >640)
				if (v[1].sx>640)
					if (v[2].sx>640)
						drawme = 0;
			
			if (v[0].sy >480)
				if (v[1].sy>480)
					if (v[2].sy>480)
						drawme = 0;

			if (drawme)
				Clip3DPolygon(v,tex->hdl);
		
		/*	SetTexture (obj->mesh->textureIDs[i]);
			if (obj->mesh->textureIDs[i])
			{
				ClipPolygon(
					tV[v0].v[X],tV[v0].v[Y],
						obj->mesh->faceTC[v0a].v[0]>>2,
						obj->mesh->faceTC[v0a].v[1]>>2,
					tV[v1].v[X],tV[v1].v[Y],
						obj->mesh->faceTC[v1a].v[0]>>2,
						obj->mesh->faceTC[v1a].v[1]>>2,
					tV[v2].v[X],
					tV[v2].v[Y],
						obj->mesh->faceTC[v2a].v[0]>>2,
						obj->mesh->faceTC[v2a].v[1]>>2);
			}*/

		}
	}

//	if ((xl<0.99) || (xluFade<0.99))
	{
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,TRUE);
	}

	return;
}






























///////////////////////////////////////////////////////////////////////






/*
void PCPrepareObject(OBJECT *obj, MESH *mesh, float m[4][4])
{
	float c[4][4];
	float f[4][4];
	VECTOR *in;
	VECTOR *vTemp2;
	unsigned long i;

	in = mesh->vertices;
	vTemp2 = tV;
	
	guLookAtF(c,
			currCamTarget[screenNum].v[X],currCamTarget[screenNum].v[Y],currCamTarget[screenNum].v[Z],
			currCamSource[screenNum].v[X],currCamSource[screenNum].v[Y],currCamSource[screenNum].v[Z],
			camVect.v[X],camVect.v[Y],camVect.v[Z]);
	
	guMtxCatF(m,c,f);
	
	for (i=0; i<mesh->numVertices; i++)
	{
		guMtxXFMF(f,in->v[X],in->v[Y],in->v[Z],
		&(vTemp2->v[X]),&(vTemp2->v[Y]),&(vTemp2->v[Z]));

		if (IsNotClipped(vTemp2))
		{
			float oozd = -1/(vTemp2->v[Z]+DIST);
			vTemp2->v[X] = 320+((vTemp2->v[X] * FOV) * oozd);
			vTemp2->v[Y] = 220+((vTemp2->v[Y] * FOV) * oozd);
		}
		else
			vTemp2->v[Z] = 0;

		vTemp2++;
		in++;
	}
}
	*/


void PCPrepareSkinnedObject(OBJECT *obj, MESH *mesh, float m[4][4])
{
	float c[4][4];
	float f[4][4];
	VECTOR *in;
	VECTOR *vTemp2;
	unsigned long i,j;
	guLookAtF(c,
			currCamTarget[screenNum].v[X],currCamTarget[screenNum].v[Y],currCamTarget[screenNum].v[Z],
			currCamSource[screenNum].v[X],currCamSource[screenNum].v[Y],currCamSource[screenNum].v[Z],
			camVect.v[X],camVect.v[Y],camVect.v[Z]);
	
	guMtxCatF(m,c,f);

	// Go thru the affected vertices, and xfm the corresponding vertices
	for (i=0; i<obj->numVerts; i++)
	{
		j = ((unsigned long *)(obj->effectedVerts))[i];
		if (j==0)
			j=0;
		// Setup Our pointers
		vTemp2 = &tV[j];
		in = &mesh->vertices[j];

		// Transform by our matrix
		guMtxXFMF(f,in->v[X],in->v[Y],in->v[Z],
		&(vTemp2->v[X]),&(vTemp2->v[Y]),&(vTemp2->v[Z]));

		// Transform to screen space
		if (IsNotClipped(vTemp2))
		{
			float oozd = -1/(vTemp2->v[Z]+DIST);
			vTemp2->v[X] = 320+((vTemp2->v[X] * FOV) * oozd);
			vTemp2->v[Y] = 220+((vTemp2->v[Y] * FOV) * oozd);
		}
		else
		{
			vTemp2->v[Z] = 0;
		}

	}
}






























void PCPrepareObject (OBJECT *obj, MESH *me, float m[4][4])
{
	float c[4][4];
	float f[4][4];
	VECTOR *in;
	VECTOR *vTemp2;
	unsigned long i;

	in = obj->mesh->vertices;

	guLookAtF(c,
			currCamTarget[screenNum].v[X],currCamTarget[screenNum].v[Y],currCamTarget[screenNum].v[Z],
			currCamSource[screenNum].v[X],currCamSource[screenNum].v[Y],currCamSource[screenNum].v[Z],
			//stx,sty,stz,
			//ctx,cty,ctz,
			//0,1,0);
			camVect.v[X],camVect.v[Y],camVect.v[Z]);
	
	guMtxCatF(m,c,f);
	
	vTemp2 = tV;
	
	for (i=0; i<obj->mesh->numVertices; i++)
	{
		if (xl<0.99)
		{
			if (player[0].frogState & FROGSTATUS_ISDEAD)
			{
				player[0].lives = 5;
				if (frog[0]->action.deathBy & DEATHBY_DROWNING)
				{
					float val;
					VECTOR me,me2;
					me.v[X] = in->v[X]*sclW;
					me.v[Y] = in->v[Y]*sclW;
					me.v[Z] = in->v[Z]*sclW;

					guMtxXFMF(m,me.v[X],me.v[Y],me.v[Z],
								&me2.v[0],&me2.v[1],&me2.v[2]); 
					
					val = 1+DistanceBetweenPoints2DSquared(&destTile[0]->centre,&me2);
					if (val<thresh)
						val = thresh;
					val = rval / (val/rval2);

					val = (1+sinf(val*fmody2+frameCount*fmody)) * val * svall;
					if (frog[0]->action.dead>sofs)
						val *= ((frog[0]->action.dead-sofs) / 75.0);
					else
						val*=0;

					val += (2+(sin(in->v[Z]*sinSpeedVert+frameCount*sinSpeedWater)+cos(in->v[X]*sinSpeedVert2+frameCount*sinSpeedWater2)))*valWater;
					
					mV[i] = (2+(sin(in->v[Z]*sinSpeedVert+frameCount*sinSpeedWater)+cos(in->v[X]*sinSpeedVert2+frameCount*sinSpeedWater2)))*valWater;

					guMtxXFMF(f,in->v[X],in->v[Y] - val, in->v[Z],

					&(vTemp2->v[X]),&(vTemp2->v[Y]),&(vTemp2->v[Z]));
				}
			}
			else
			{
				mV[i] =(	(2+(sin(in->v[Z]*sinSpeedVert+frameCount*sinSpeedWater)+cos(in->v[X]*sinSpeedVert2+frameCount*sinSpeedWater2))) + 
					(2+(sin((in->v[Z]+50)*sinSpeedVert+frameCount*sinSpeedWater)+cos((in->v[X]+50)*sinSpeedVert2+frameCount*sinSpeedWater2)))
					)*valWater/2;
				guMtxXFMF(f,in->v[X],in->v[Y]-
				mV[i]
				,in->v[Z],
				&(vTemp2->v[X]),&(vTemp2->v[Y]),&(vTemp2->v[Z]));
			}
		}
		else
		{
			guMtxXFMF(f,in->v[X],in->v[Y],in->v[Z],
			&(vTemp2->v[X]),&(vTemp2->v[Y]),&(vTemp2->v[Z]));
		}
	
		if (((vTemp2->v[Z]+DIST)>nearClip) &&
			((noClipping) ||   
			(((vTemp2->v[Z]+DIST)<farClip) &&
			((vTemp2->v[X])>-horizClip) &&
			((vTemp2->v[X])<horizClip) &&
			((vTemp2->v[Y])>-vertClip) &&
			((vTemp2->v[Y])<vertClip))))
		{
			float oozd = -1/(vTemp2->v[Z]+DIST);
			vTemp2->v[X] = 320+((vTemp2->v[X] * FOV) * oozd);
			vTemp2->v[Y] = 220+((vTemp2->v[Y] * FOV) * oozd);
		}
		else
			vTemp2->v[Z] = 0;

		vTemp2++;
		in++;
		mV[i]*=scVC;
	}
}

void PCRenderObject (OBJECT *obj)
{
	unsigned long i;
	short fce[3] = {0,1,2};		
	VECTOR *c1,*c2,*c3;
	D3DTLVERTEX v[3],*vTemp;
	long drawme;
	
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,FALSE);

	for (i=0; i<obj->mesh->numFaces; i++)
	{
		long v0 = obj->mesh->faceIndex[i].v[0];
		long v1 = obj->mesh->faceIndex[i].v[1];
		long v2 = obj->mesh->faceIndex[i].v[2];
		long v0a,v1a,v2a;
		TEXENTRY *tex = (TEXENTRY *)(obj->mesh->textureIDs[i]);

		v0a = i*3;
		v1a = v0a+1;
		v2a = v1a+1;

		if (tV[v0].v[Z])
		if (tV[v1].v[Z])
		if (tV[v2].v[Z])
		if (tex)
		{
			c1 = &(((VECTOR *)obj->mesh->vertexNormals)[v0a]);
			c2 = &(((VECTOR *)obj->mesh->vertexNormals)[v1a]);
			c3 = &(((VECTOR *)obj->mesh->vertexNormals)[v2a]);
			
			drawme = 1;

			vTemp = v;
			
			vTemp->specular = 0;
			vTemp->sx = tV[v0].v[X];
			vTemp->sy = tV[v0].v[Y];
			vTemp->sz = (tV[v0].v[Z]+DIST)/2000;///2000;
			vTemp->tu = (obj->mesh->faceTC[v0a].v[0]*(1.0/1024.0));
			vTemp->tv = (obj->mesh->faceTC[v0a].v[1]*(1.0/1024.0));
			vTemp->color = D3DRGBA(c1->v[2],c1->v[1],c1->v[0],xl);
			if (xl<0.99)
			{
				vTemp->specular = D3DRGB(mV[v0],mV[v0],mV[v0]);
			}
			else
			{
				vTemp->specular = D3DRGB(rCol,gCol,bCol);
			}

			vTemp++;

			vTemp->specular = 0;
			vTemp->sx = tV[v1].v[X];
			vTemp->sy = tV[v1].v[Y];
			vTemp->sz = (tV[v1].v[Z]+DIST)/2000;//2000;
			vTemp->tu = (obj->mesh->faceTC[v1a].v[0]*(1.0/1024.0));
			vTemp->tv = (obj->mesh->faceTC[v1a].v[1]*(1.0/1024.0));
			vTemp->color = D3DRGBA(c2->v[2],c2->v[1],c2->v[0],xl);
			if (xl<0.99)
			{
				vTemp->specular = D3DRGB(mV[v1],mV[v1],mV[v1]);
			}
			else
			{
				vTemp->specular = D3DRGB(rCol,gCol,bCol);
			}

			vTemp++;

			vTemp->specular = 0;
			vTemp->sx = tV[v2].v[X];
			vTemp->sy = tV[v2].v[Y];
			vTemp->sz = (tV[v2].v[Z]+DIST)/2000;///2000;
			vTemp->tu = (obj->mesh->faceTC[v2a].v[0]*(1.0/1024.0));
			vTemp->tv = (obj->mesh->faceTC[v2a].v[1]*(1.0/1024.0));
			vTemp->color = D3DRGBA(c3->v[2],c3->v[1],c3->v[0],xl);
			if (xl<0.99)
			{
				vTemp->specular = D3DRGB(mV[v2],mV[v2],mV[v2]);
			}
			else
			{
				vTemp->specular = D3DRGB(rCol,gCol,bCol);
			}
		

			if (v[0].sx <0)
				if (v[1].sx<0)
					if (v[2].sx<0)
						drawme = 0;
			
			if (v[0].sy <0)
				if (v[1].sy<0)
					if (v[2].sy<0)
						drawme = 0;

			if (v[0].sx >640)
				if (v[1].sx>640)
					if (v[2].sx>640)
						drawme = 0;
			
			if (v[0].sy >480)
				if (v[1].sy>480)
					if (v[2].sy>480)
						drawme = 0;

			if (drawme)
				Clip3DPolygon(v,tex->hdl);
		
		/*	SetTexture (obj->mesh->textureIDs[i]);
			if (obj->mesh->textureIDs[i])
			{
				ClipPolygon(
					tV[v0].v[X],tV[v0].v[Y],
						obj->mesh->faceTC[v0a].v[0]>>2,
						obj->mesh->faceTC[v0a].v[1]>>2,
					tV[v1].v[X],tV[v1].v[Y],
						obj->mesh->faceTC[v1a].v[0]>>2,
						obj->mesh->faceTC[v1a].v[1]>>2,
					tV[v2].v[X],
					tV[v2].v[Y],
						obj->mesh->faceTC[v2a].v[0]>>2,
						obj->mesh->faceTC[v2a].v[1]>>2);
			}*/

		}
	}

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,TRUE);
}





















///////////////////////////////////////////////////////////////////////




































































































































































































#ifdef POOOO

/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.
	
	  
	File		: Subcube.c
	Programmer	: Andrew Eder
	Date		: 11/24/98
	
----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


#define LODDist (700 * 700)

short transform = 1;

Vtx	objectsVtx[2][2000];
Vtx *vtxPtr;

char changeRenderMode	= 1;

float unitMatrix[4][4] = {0, 0, 0, 0,
				  0, 0, 0, 0,
				  0, 0, 0, 0,
				  0, 0, 0, 0};

char	*tempObjectPtr, *tempObjectPtr2;

VECTOR	*actorScale;
VECTOR	parentScaleStack[10];
int		parentScaleStackLevel = 0;

int		matrixStackCount = 0;
short	xluFact = 0x8f;
short	xluOverride = 0;

ACTOR	*currentDrawActor;
Mtx		*staticObjectMtx;
char	calcMtx = FALSE;

char	staticObj;

float rotmat[4][4], scalemat[4][4], transmat[4][4];
OBJECT_CONTROLLER *currentObjectController;

char lookatNum = 0;
char hiliteNum = 0;

void SetRendermodeForEnviroment(void);

RENDER_MODE renderMode = 
{
	0,	//	UBYTE	transparentSurf;
	0,	//	UBYTE	aiSurf;         
	0,	//	UBYTE	xluSurf;        
	2,	//	UBYTE	useAAMode;      
	1,	//	UBYTE	useZMode;       
	1,	//	UBYTE	useTextureMode; 
	0,	//	UBYTE	mirrorTextures; 
	0,	//	UBYTE	phongMode;      
	0,	//	UBYTE	faceColour;     
	0,	//	UBYTE	objectColour;   
	0,	//	UBYTE	actorColour;    
	0,	//	UBYTE	pixelOut;       
	1,	//	UBYTE	phongAllowed;   
};


//void DrawObject(OBJECT *obj);


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

long TestDistanceFromFrog = 0;

extern float meMod;
float adjF = 0.3;
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void XformActor(ACTOR *actor)
{
	OBJECT_CONTROLLER *objectC = actor->objectController;
	float animTime;

	if((actor->xluOverride == 0) || (objectC->object->xlu == 0))
	{
		actor->visible = 0;
		return;
	}

	if((actor->matrix) || (actor->objectController->objectSize == 1))
	{
		actor->visible = TRUE;
		return;
	}
	
	UpdateAnimations(actor);

	actor->visible = TRUE;

	xluOverride = actor->xluOverride;

//	numActorsXformed++;
	staticObjectMtx = actor->matrix;
	currentDrawActor = actor;
	currentObjectController = objectC;
	actorScale = &actor->scale;

	if((actor->animation) && (actor->animation->anims))
		animTime = actor->animation->animTime;
	else
		animTime = 0;

	parentScaleStackLevel = 0;
	parentScaleStack[parentScaleStackLevel].v[X] = parentScaleStack[parentScaleStackLevel].v[Y] = parentScaleStack[parentScaleStackLevel].v[Z] = 1;

	if(objectC->drawList)	//if object is skinned, transform mesh before aplying object TM
	{
		if(objectC->object)
		{
			if((actor->LODObjectController) && (actor->distanceFromCamera - actor->maxRadius > (LODDist)))
				TransformSkinnedObject(actor->LODObjectController->object, animTime);
			else
				TransformSkinnedObject(objectC->object, animTime);
		}
	}

	guTranslateF(transmat, actor->pos.v[X], actor->pos.v[Y], actor->pos.v[Z]);
	PushMatrix(transmat);

	QuaternionToMatrix(&actor->qRot,(MATRIX *)rotmat);
	PushMatrix(rotmat);

	if(objectC->drawList == NULL)
	{
		if(objectC->object)
		{
			if((actor->LODObjectController) && (actor->distanceFromCamera - actor->maxRadius > (LODDist)))
				TransformObject(actor->LODObjectController->object, animTime);
			else
				TransformObject(objectC->object, animTime);
		}
	}
	else
	{
		//skinned objects need to store the TM, including the scale which can't be done at vertex xform stage due to interer inaccuracies
		guScaleF(scalemat,actor->scale.v[X],actor->scale.v[Y],actor->scale.v[Z]);
		PushMatrix(scalemat);
		guMtxF2L(matrixStack.stack[matrixStack.stackPosition], &objectC->object->objMatrix);
		PopMatrix();
	}

  	parentScaleStackLevel = 0;
	parentScaleStack[parentScaleStackLevel].v[X] = parentScaleStack[parentScaleStackLevel].v[Y] = parentScaleStack[parentScaleStackLevel].v[Z] = 1;
	
	matrixStack.stackPosition-=2;
//	PopMatrix();
//	PopMatrix();
}


/*---------------------------------------------------------------------------------------------
	Function	: DrawMyObject
	Parameters	: (OBJECT *obj)
	Returns		: void 
*/

void DrawObject(OBJECT *obj, Gfx *drawList, int skinned)
{
//	if(obj->textureAnim)
//		AnimateTexture(obj->textureAnim,obj->drawList);

	if(staticObjectMtx == NULL)
	{
		SetMatrix((float *)&dynamicp->modeling4[objectMatrix], (float *)&obj->objMatrix);

	    gSPMatrix(glistp++, 
		OS_K0_TO_PHYSICAL(&(dynamicp->modeling4[objectMatrix++])),
		G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
	}
	else
	{
		SetMatrix((float *)&dynamicp->modeling4[objectMatrix], (float *)staticObjectMtx);

	    gSPMatrix(glistp++, 
		OS_K0_TO_PHYSICAL(&(dynamicp->modeling4[objectMatrix++])),
		G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
	}
	
	SetupRenderModeForObject(obj);

	if(drawList)
	{
		gSPDisplayList(glistp++, drawList);
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
			if(obj->flags & OBJECT_FLAGS_PHONG)
				WriteObjectDisplayListGouraudPhong(obj);
			else
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

	if(skinned)
		return;

	if(obj->children)
		DrawObject(obj->children, obj->children->drawList, skinned);

	if(obj->next)
		DrawObject(obj->next, obj->next->drawList, skinned);
}


void DrawActor(ACTOR *actor)
{
	OBJECT_CONTROLLER *objectC = actor->objectController;
	int col;
	float animTime;

	if(actor->visible == FALSE)
		return;

	staticObjectMtx = actor->matrix;

	xluOverride = actor->xluOverride;
/*
	if((actor->stats) && (actor->stats->inShadow))
	{
		renderMode.actorColour = 1;
		col = 255-actor->stats->inShadow;
		ProcessBackgroundLightsWithCols(1,col,col,col);
	}
	else if(renderMode.actorColour)
	{
		renderMode.actorColour = 0;
		ProcessBackgroundLightsWithCols(0,0,0,0);
	}
*/
	currentDrawActor = actor;
	actorScale = &actor->scale;

	if((actor->animation) && (actor->animation->anims))
		animTime = actor->animation->animTime;
	else
		animTime = 0;

	//quick draw
	if(objectC->objectSize == 1)
	{
		SetupRenderModeForObject(objectC->object);
		gSPDisplayList(glistp++, objectC->object->drawList);
	}
	else
	{
		if(objectC->drawList)	//if object is skinned, draw mesh now
 		{
			gSPSegment(glistp++, 1, objectC->vtx[objectC->vtxBuf]);
			DrawObject(objectC->object, objectC->drawList, TRUE);
		}
		else
			DrawObject(objectC->object, objectC->object->drawList, FALSE);
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
/*	int i,j,plus;
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
	}*/
}




/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
QUATERNION	tempQuat,morphFromQuat,morphToQuat,morphResultQuat;
void TransformObject(OBJECT *obj, float time)
{
	VECTOR translation,scale;
	VECTOR	morphFrom,morphTo;
	SPRITE	*sprite;
	float	tempFloat,interp;
	short i, j;
	short	fromKey, toKey;
	short	xluVal;

//handle position keyframes
	if((obj->numMoveKeys > 1) && (currentDrawActor->animation))
	{
		if(currentDrawActor->animation->numMorphFrames)
		{
			// Find from vector
			FindToFromVKeys(obj->moveKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphFrom,obj->numMoveKeys);
			LinearInterp(&morphFrom,&obj->moveKeys[fromKey].vect,&obj->moveKeys[toKey].vect,interp);

			// Find to vector
			FindToFromVKeys(obj->moveKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphTo,obj->numMoveKeys);
			LinearInterp(&morphTo,&obj->moveKeys[fromKey].vect,&obj->moveKeys[toKey].vect,interp);

			// Interp between from and to vectors
			tempFloat = (float)currentDrawActor->animation->currentMorphFrame/(float)(Fabs(currentDrawActor->animation->numMorphFrames));
			LinearInterp(&translation,&morphFrom,&morphTo,tempFloat);
		}
		else
		{
			FindToFromVKeys(obj->moveKeys,&fromKey,&toKey,&interp,time,obj->numMoveKeys);
			LinearInterp(&translation,&obj->moveKeys[fromKey].vect,&obj->moveKeys[toKey].vect,interp);
		}
	}
	else
	{
		SetVector(&translation,&obj->moveKeys[0].vect);
	}
	if((obj->numScaleKeys > 1) && (currentDrawActor->animation))
	{
		if(currentDrawActor->animation->numMorphFrames)
		{
			// Find from vector
			FindToFromVKeys(obj->scaleKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphFrom,obj->numScaleKeys);
			LinearInterp(&morphFrom,&obj->scaleKeys[fromKey].vect,&obj->scaleKeys[toKey].vect,interp);

			// Find to vector
			FindToFromVKeys(obj->scaleKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphTo,obj->numScaleKeys);
			LinearInterp(&morphTo,&obj->scaleKeys[fromKey].vect,&obj->scaleKeys[toKey].vect,interp);

			// Slerp between from and to vectors
			tempFloat = (float)currentDrawActor->animation->currentMorphFrame/(float)(Fabs(currentDrawActor->animation->numMorphFrames));
			LinearInterp(&scale,&morphFrom,&morphTo,tempFloat);
		}
		else
		{
			FindToFromVKeys(obj->scaleKeys,&fromKey,&toKey,&interp,time,obj->numScaleKeys);
			LinearInterp(&scale,&obj->scaleKeys[fromKey].vect,&obj->scaleKeys[toKey].vect,interp);
		}
	}
	else
	{
		SetVector(&scale,&obj->scaleKeys[0].vect);
	}
	// handle rotation keyframes
	if((obj->numRotateKeys > 1) && (currentDrawActor->animation))
	{
		if(currentDrawActor->animation->numMorphFrames)
		{
			// Find from vector
			FindToFromQKeys(obj->rotateKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphFrom,obj->numRotateKeys);
			QuatSlerp(&obj->rotateKeys[fromKey].quat, &obj->rotateKeys[toKey].quat, interp, &morphFromQuat);

			// Find to vector
			FindToFromQKeys(obj->rotateKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphTo,obj->numRotateKeys);
			QuatSlerp(&obj->rotateKeys[fromKey].quat, &obj->rotateKeys[toKey].quat, interp, &morphToQuat);

			// Slerp between from and to vectors
			tempFloat = (float)currentDrawActor->animation->currentMorphFrame/(float)(Fabs(currentDrawActor->animation->numMorphFrames));
			QuatSlerp(&morphFromQuat, &morphToQuat, tempFloat, &morphResultQuat);

			QuaternionToMatrix(&morphResultQuat, (MATRIX *)rotmat);
		}
		else
		{
			FindToFromQKeys(obj->rotateKeys,&fromKey,&toKey,&interp,time,obj->numRotateKeys);
			QuatSlerp(&obj->rotateKeys[fromKey].quat, &obj->rotateKeys[toKey].quat, interp, &tempQuat);

			// convert back to rotation matrix
			QuaternionToMatrix(&tempQuat, (MATRIX *)rotmat);
		}
	}
	else
	{
		QuaternionToMatrix(&obj->rotateKeys[0].quat, (MATRIX *)rotmat);
	}

	rotmat[3][0] = translation.v[X] * actorScale->v[X] * parentScaleStack[parentScaleStackLevel].v[X];
	rotmat[3][1] = translation.v[Y] * actorScale->v[Y] * parentScaleStack[parentScaleStackLevel].v[Y];
	rotmat[3][2] = translation.v[Z] * actorScale->v[Z] * parentScaleStack[parentScaleStackLevel].v[Z];

	PushMatrix(rotmat);

	guScaleF(scalemat, scale.v[X] * actorScale->v[X], scale.v[Y] * actorScale->v[Y], scale.v[Z] * actorScale->v[Z]);

	PushMatrix(scalemat);


	//if the object has a collision sphere
	if(obj->collSphere)
	{
		SetVector(&obj->collSphere->oldOffset, &obj->collSphere->offset);
		guMtxXFMF(matrixStack.stack[matrixStack.stackPosition], 0, 0, 0,&obj->collSphere->offset.v[X], &obj->collSphere->offset.v[Y], &obj->collSphere->offset.v[Z]);
	}

	//maintain posision of all objects sprites        
//	if(obj->numSprites > 0)
/*	{
		for(i = 0; i < obj->numSprites; i++)
		{
			sprite = obj->sprites[i].sprite;
			if(sprite)
			{
				if((currentDrawActor->stats) && (currentDrawActor->stats->inShadow))
					sprite->r = sprite->g = sprite->b = 255-currentDrawActor->stats->inShadow;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//				sprite->r = (float)obj->sprites[i].r*inShadow;
//				sprite->g = (float)obj->sprites[i].g*inShadow;
//				sprite->b = (float)obj->sprites[i].b*inShadow;
				if(obj->flags & OBJECT_FLAGS_COLOUR_BLEND)
				{
					sprite->red2 = obj->colour.r;
					sprite->green2 = obj->colour.g;
					sprite->blue2 = obj->colour.b;
					sprite->alpha2 = obj->colour.a;
					sprite->flags |= SPRITE_FLAGS_COLOUR_BLEND;
				}
				else
					sprite->flags &= -1 - SPRITE_FLAGS_COLOUR_BLEND;
				if(obj->flags & OBJECT_FLAGS_COLOUR_BLEND_AFTERLIGHT)
				{
					sprite->red2 = obj->colour.r;
					sprite->green2 = obj->colour.g;
					sprite->blue2 = obj->colour.b;
					sprite->alpha2 = obj->colour.a;
					sprite->flags |= SPRITE_FLAGS_COLOUR_BLEND_AFTERLIGHT;
				}
				else
					sprite->flags &= -1 - SPRITE_FLAGS_COLOUR_BLEND_AFTERLIGHT;
				obj->sprites[i].sprite->scaleX = (float)obj->sprites[i].sx * actorScale->v[X] * scale.v[X];
				obj->sprites[i].sprite->scaleY = (float)obj->sprites[i].sy * actorScale->v[Y] * scale.v[Y];
				xluVal = ((int)(obj->xlu * xluOverride))/100;
				if(xluOverride <= 10)
					xluVal = 0;
				if((currentDrawActor->objectType == BALL_TYPE_OBJECT_SPAWN) || (currentDrawActor->type == DEBRIS))
				{
					obj->sprites[i].sprite->r = 0;
					obj->sprites[i].sprite->g = 0;
					obj->sprites[i].sprite->b = 0;
					obj->sprites[i].sprite->a = 254;
				}
				else
					obj->sprites[i].sprite->a = Min(255,xluVal);
				if(obj->sprites[i].sprite->a >= 255)
					obj->sprites[i].sprite->flags &= -1 - SPRITE_TRANSLUCENT;
				else
					obj->sprites[i].sprite->flags |= SPRITE_TRANSLUCENT;
				guMtxXFMF(matrixStack.stack[matrixStack.stackPosition], obj->sprites[i].x, obj->sprites[i].y, obj->sprites[i].z,
												&sprite->pos.v[X], &sprite->pos.v[Y], &sprite->pos.v[Z]);
				if(renderMode.pixelOut)
				{
					sprite->flags |= SPRITE_FLAGS_PIXEL_OUT;
					sprite->a = (sprite->a * obj->pixelOutAlpha)/255;
				}
				else
					sprite->flags &= -1 - SPRITE_FLAGS_PIXEL_OUT;
			}
		}
	}
*/

		
  	guMtxF2L(matrixStack.stack[matrixStack.stackPosition], &obj->objMatrix);

	PopMatrix();


//transform children
	if(obj->children)
	{
		parentScaleStackLevel++;
		SetVector(&parentScaleStack[parentScaleStackLevel],&scale);
		TransformObject(obj->children, time);
		parentScaleStackLevel--;
	}

	PopMatrix();

	if(obj->next)
	{
		TransformObject(obj->next, time);
	}
}



/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void TransformSkinnedObject(OBJECT *obj, float time)
{
	VECTOR translation,scale, rotation;
	VECTOR	morphFrom,morphTo;
	SPRITE	*sprite;
	float	tempFloat,interp;
	short i, j;
	short	fromKey, toKey;
	short	xluVal;
	Vtx		*vtx = currentObjectController->vtx[currentObjectController->vtxBuf];

//handle position keyframes
	if((obj->numMoveKeys > 1) && (currentDrawActor->animation))
	{
		if(currentDrawActor->animation->numMorphFrames)
		{
			// Find from vector
			FindToFromVKeys(obj->moveKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphFrom,obj->numMoveKeys);
			LinearInterp(&morphFrom,&obj->moveKeys[fromKey].vect,&obj->moveKeys[toKey].vect,interp);

			// Find to vector
			FindToFromVKeys(obj->moveKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphTo,obj->numMoveKeys);
			LinearInterp(&morphTo,&obj->moveKeys[fromKey].vect,&obj->moveKeys[toKey].vect,interp);

			// Interp between from and to vectors
			tempFloat = (float)currentDrawActor->animation->currentMorphFrame/(float)(Fabs(currentDrawActor->animation->numMorphFrames));
			LinearInterp(&translation,&morphFrom,&morphTo,tempFloat);
		}
		else
		{
			FindToFromVKeys(obj->moveKeys,&fromKey,&toKey,&interp,time,obj->numMoveKeys);
			LinearInterp(&translation,&obj->moveKeys[fromKey].vect,&obj->moveKeys[toKey].vect,interp);
		}
	}
	else
	{
		SetVector(&translation,&obj->moveKeys[0].vect);
	}
	if((obj->numScaleKeys > 1) && (currentDrawActor->animation))
	{
		if(currentDrawActor->animation->numMorphFrames)
		{
			// Find from vector
			FindToFromVKeys(obj->scaleKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphFrom,obj->numScaleKeys);
			LinearInterp(&morphFrom,&obj->scaleKeys[fromKey].vect,&obj->scaleKeys[toKey].vect,interp);

			// Find to vector
			FindToFromVKeys(obj->scaleKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphTo,obj->numScaleKeys);
			LinearInterp(&morphTo,&obj->scaleKeys[fromKey].vect,&obj->scaleKeys[toKey].vect,interp);

			// Slerp between from and to vectors
			tempFloat = (float)currentDrawActor->animation->currentMorphFrame/(float)(Fabs(currentDrawActor->animation->numMorphFrames));
			LinearInterp(&scale,&morphFrom,&morphTo,tempFloat);
		}
		else
		{
			FindToFromVKeys(obj->scaleKeys,&fromKey,&toKey,&interp,time,obj->numScaleKeys);
			LinearInterp(&scale,&obj->scaleKeys[fromKey].vect,&obj->scaleKeys[toKey].vect,interp);
		}
	}
	else
	{
		SetVector(&scale,&obj->scaleKeys[0].vect);
	}

	// handle rotation keyframes
	if((obj->numRotateKeys > 1) && (currentDrawActor->animation))
	{
		if(currentDrawActor->animation->numMorphFrames)
		{
			// Find from vector
			FindToFromQKeys(obj->rotateKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphFrom,obj->numRotateKeys);
			QuatSlerp(&obj->rotateKeys[fromKey].quat, &obj->rotateKeys[toKey].quat, interp, &morphFromQuat);

			// Find to vector
			FindToFromQKeys(obj->rotateKeys,&fromKey,&toKey,&interp,currentDrawActor->animation->morphTo,obj->numRotateKeys);
			QuatSlerp(&obj->rotateKeys[fromKey].quat, &obj->rotateKeys[toKey].quat, interp, &morphToQuat);

			// Slerp between from and to vectors
			tempFloat = (float)currentDrawActor->animation->currentMorphFrame/(float)(Fabs(currentDrawActor->animation->numMorphFrames));
			QuatSlerp(&morphFromQuat, &morphToQuat, tempFloat, &morphResultQuat);

			QuaternionToMatrix(&morphResultQuat, (MATRIX *)rotmat);
		}
		else
		{
			FindToFromQKeys(obj->rotateKeys,&fromKey,&toKey,&interp,time,obj->numRotateKeys);
			QuatSlerp(&obj->rotateKeys[fromKey].quat, &obj->rotateKeys[toKey].quat, interp, &tempQuat);

			// convert back to rotation matrix
			QuaternionToMatrix(&tempQuat, (MATRIX *)rotmat);
		}
	}
	else
	{
		QuaternionToMatrix(&obj->rotateKeys[0].quat, (MATRIX *)rotmat);
	}


	//transform the objects vertex normals ONLY if the object is lit.
	if((obj->flags & OBJECT_FLAGS_PRELIT) == 0)
	{
		PushRMatrix(rotmat);	//uses special matrix stack which only records the rotation
		
		for(i = 0; i < obj->numVerts; i++)
		{
			guMtxXFMF(rMatrixStack.stack[rMatrixStack.stackPosition], obj->originalVerts[i].n[X], obj->originalVerts[i].n[Y], obj->originalVerts[i].n[Z],
											&rotation.v[X], &rotation.v[Y], &rotation.v[Z]);

			for(j = 0; j < obj->effectedVerts[i].numVerts; j++)
			{
				vtx[obj->effectedVerts[i].verts[j]].n.n[X] = (s8)rotation.v[X];
				vtx[obj->effectedVerts[i].verts[j]].n.n[Y] = (s8)rotation.v[Y];
				vtx[obj->effectedVerts[i].verts[j]].n.n[Z] = (s8)rotation.v[Z];
			}
		}

	}


	rotmat[3][0] = translation.v[X] * parentScaleStack[parentScaleStackLevel].v[X];
	rotmat[3][1] = translation.v[Y] * parentScaleStack[parentScaleStackLevel].v[Y];
	rotmat[3][2] = translation.v[Z] * parentScaleStack[parentScaleStackLevel].v[Z];

	PushMatrix(rotmat);


	guScaleF(scalemat, scale.v[X], scale.v[Y], scale.v[Z]);

	PushMatrix(scalemat);


	//if the object has a collision sphere
	if(obj->collSphere)
	{
		SetVector(&obj->collSphere->oldOffset, &obj->collSphere->offset);
		guMtxXFMF(matrixStack.stack[matrixStack.stackPosition], 0, 0, 0,&obj->collSphere->offset.v[X], &obj->collSphere->offset.v[Y], &obj->collSphere->offset.v[Z]);
	}

	//maintain posision of all objects sprites        
//	if(obj->numSprites > 0)
/*	{
		for(i = 0; i < obj->numSprites; i++)
		{
			sprite = obj->sprites[i].sprite;
			if(sprite)
			{
				if((currentDrawActor->stats) && (currentDrawActor->stats->inShadow))
					sprite->r = sprite->g = sprite->b = 255-currentDrawActor->stats->inShadow;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//				sprite->r = (float)obj->sprites[i].r*inShadow;
//				sprite->g = (float)obj->sprites[i].g*inShadow;
//				sprite->b = (float)obj->sprites[i].b*inShadow;
				if(obj->flags & OBJECT_FLAGS_COLOUR_BLEND)
				{
					sprite->red2 = obj->colour.r;
					sprite->green2 = obj->colour.g;
					sprite->blue2 = obj->colour.b;
					sprite->alpha2 = obj->colour.a;
					sprite->flags |= SPRITE_FLAGS_COLOUR_BLEND;
				}
				else
					sprite->flags &= -1 - SPRITE_FLAGS_COLOUR_BLEND;
				if(obj->flags & OBJECT_FLAGS_COLOUR_BLEND_AFTERLIGHT)
				{
					sprite->red2 = obj->colour.r;
					sprite->green2 = obj->colour.g;
					sprite->blue2 = obj->colour.b;
					sprite->alpha2 = obj->colour.a;
					sprite->flags |= SPRITE_FLAGS_COLOUR_BLEND_AFTERLIGHT;
				}
				else
					sprite->flags &= -1 - SPRITE_FLAGS_COLOUR_BLEND_AFTERLIGHT;
				obj->sprites[i].sprite->scaleX = (float)obj->sprites[i].sx * actorScale->v[X] * scale.v[X];
				obj->sprites[i].sprite->scaleY = (float)obj->sprites[i].sy * actorScale->v[Y] * scale.v[Y];
				xluVal = ((int)(obj->xlu * xluOverride))/100;
				if((currentDrawActor->objectType == BALL_TYPE_OBJECT_SPAWN) || (currentDrawActor->type == DEBRIS))
				{
					obj->sprites[i].sprite->r = 0;
					obj->sprites[i].sprite->g = 0;
					obj->sprites[i].sprite->b = 0;
					obj->sprites[i].sprite->a = 254;
				}
				else
					obj->sprites[i].sprite->a = Min(255,xluVal);
				if(obj->sprites[i].sprite->a >= 255)
					obj->sprites[i].sprite->flags &= -1 - SPRITE_TRANSLUCENT;
				else
					obj->sprites[i].sprite->flags |= SPRITE_TRANSLUCENT;
				guMtxXFMF(matrixStack.stack[matrixStack.stackPosition], obj->sprites[i].x, obj->sprites[i].y, obj->sprites[i].z,
												&sprite->pos.v[X], &sprite->pos.v[Y], &sprite->pos.v[Z]);
				if(renderMode.pixelOut)
				{
					sprite->flags |= SPRITE_FLAGS_PIXEL_OUT;
					sprite->a = (sprite->a * obj->pixelOutAlpha)/255;
				}
				else
					sprite->flags &= -1 - SPRITE_FLAGS_PIXEL_OUT;
			}
		}
	}*/


	for(i = 0; i < obj->numVerts; i++)
	{
		//xform the vertex positions
		guMtxXFMF(matrixStack.stack[matrixStack.stackPosition], obj->originalVerts[i].v[X], obj->originalVerts[i].v[Y], obj->originalVerts[i].v[Z],
										&translation.v[X], &translation.v[Y], &translation.v[Z]);

		for(j = 0; j < obj->effectedVerts[i].numVerts; j++)
		{
			vtx[obj->effectedVerts[i].verts[j]].n.ob[X] = translation.v[X];
			vtx[obj->effectedVerts[i].verts[j]].n.ob[Y] = translation.v[Y];
			vtx[obj->effectedVerts[i].verts[j]].n.ob[Z] = translation.v[Z];
		}

	}

	PopMatrix();

//transform children
	if(obj->children)
	{
		parentScaleStackLevel++;
		SetVector(&parentScaleStack[parentScaleStackLevel],&scale);
		TransformSkinnedObject(obj->children, time);
		parentScaleStackLevel--;
	}

	PopMatrix();

	if((obj->flags & OBJECT_FLAGS_PRELIT) == 0)
	{
		PopRMatrix();
	}

	if(obj->next)
	{
		TransformSkinnedObject(obj->next, time);
	}
}





/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FindToFromVKeys(VKEYFRAME *keys,short *from,short *to,float *interp,float time,int numKeys)
{
	int			i;
	int			low,high;
	short		fromKey = -1, toKey;
	float		tempFloat;

	low = 0;
	high = numKeys - 1;

	while(low <= high)
	{
		i = (low + high)>>1;
		if(keys[i].time == time)
		{
			if(i == numKeys - 1)
			{
				fromKey = -2;
			}
			else
			{
				fromKey = i;
				toKey = i + 1;
			}
			break;
		}
		else
		{
			if(keys[i].time > time)
			{
				high = i - 1;
			}
			else
			{
				low = i + 1;
			}
		}
	}

	if((fromKey == -1) && (high+1 < numKeys) && (keys[high + 1].time > time))
	{
		fromKey = high;
		toKey = high + 1;
	}

	if(fromKey >= 0)		//time is within valid keyframes
	{
		tempFloat = abs(keys[toKey].time - keys[fromKey].time);
		*interp = (time - (float)keys[fromKey].time) / tempFloat;
	}
	else					// must be between last and first keyframe
	{
		fromKey = numKeys-1;
		toKey = fromKey;
		*interp = 1;
	}
	*from = fromKey;
	*to = toKey;
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FindToFromQKeys(QKEYFRAME *keys,short *from,short *to,float *interp,float time,int numKeys)
{
	int			i;
	int			low,high;
	short		fromKey = -1, toKey;
	float		tempFloat;

	low = 0;
	high = numKeys - 1;

	while(low <= high)
	{
		i = (low + high)>>1;
		if(keys[i].time == time)
		{
			if(i == numKeys - 1)
			{
				fromKey = -2;
			}
			else
			{
				fromKey = i;
				toKey = i + 1;
			}
			break;
		}
		else
		{
			if(keys[i].time > time)
			{
				high = i - 1;
			}
			else
			{
				low = i + 1;
			}
		}
	}

	if((fromKey == -1) && (high+1 < numKeys) && (keys[high + 1].time > time))
	{
		fromKey = high;
		toKey = high + 1;
	}

	if(fromKey >= 0)		//time is within valid keyframes
	{
		tempFloat = abs(keys[toKey].time - keys[fromKey].time);
		*interp = (time - (float)keys[fromKey].time) / tempFloat;
	}
	else					// must be between last and first keyframe
	{
		fromKey = numKeys-1;
		toKey = fromKey;
		*interp = 1;
	}
	*from = fromKey;
	*to = toKey;
}




/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void SetupRenderModeForObject ( OBJECT *obj )
{
	VECTOR	tempVect;
	float	tempFloat;
	Mtx		nomatrix;
	int		r,g,b;
	int		phongX,phongY;

//////////////////////////////////////////////////
//check to see if render mode needs to be changed
	changeRenderMode = 0;


	if(!renderMode.aiSurf)
	{
		/*if(obj->flags & OBJECT_FLAGS_OBJECT_COLOUR)
		{
			renderMode.objectColour = 1;
			if((currentDrawActor->stats) && (currentDrawActor->stats->inShadow > 0))
			{
				r = (float)(obj->colour.r*(255-currentDrawActor->stats->inShadow))/255.0;
				g = (float)(obj->colour.g*(255-currentDrawActor->stats->inShadow))/255.0;
				b = (float)(obj->colour.b*(255-currentDrawActor->stats->inShadow))/255.0;
				ProcessBackgroundLightsWithCols(1,r,g,b);
			}
			else
				ProcessBackgroundLightsWithCols(1,obj->colour.r,obj->colour.g,obj->colour.b);
		}
		else if(renderMode.objectColour)
		{
			renderMode.objectColour = 0;
			if(renderMode.actorColour)
			{
				r = 255-currentDrawActor->stats->inShadow;
				ProcessBackgroundLightsWithCols(1,r,r,r);
			}
			else
				ProcessBackgroundLightsWithCols(0,0,0,0);
		}*/

		if(obj->flags & OBJECT_FLAGS_TRANSPARENT)
		{
			if(renderMode.transparentSurf == FALSE)
			{
				renderMode.transparentSurf = TRUE;
				changeRenderMode = TRUE;
			}
		}
		else
		{
			if(renderMode.transparentSurf == TRUE)
			{
				renderMode.transparentSurf = FALSE;
				changeRenderMode = TRUE;
			}
		}

		if(obj->flags & OBJECT_FLAGS_XLU)
		{
			if(renderMode.xluSurf == FALSE)
			{
				renderMode.xluSurf = TRUE;
				changeRenderMode = 1;
			}
			if(obj->drawList == 0)
				renderMode.xluSurf = TRUE;

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
			if(renderMode.xluSurf == TRUE)
			{
				renderMode.xluSurf = FALSE;
				changeRenderMode = 1;
			}
		}

		if(obj->flags & OBJECT_FLAGS_FACE_COLOUR)
		{
			if((renderMode.faceColour == NO) && ((!(obj->flags & OBJECT_FLAGS_PHONG)) || (renderMode.phongAllowed == 0)))
			{
				renderMode.faceColour = YES;
				changeRenderMode = YES;
			}
		}
		else if(renderMode.faceColour)
		{
			renderMode.faceColour = NO;
			changeRenderMode = YES;
		}

		if((obj->pixelOutAlpha) && (obj->pixelOutAlpha < 250))
		{
	   		changeRenderMode = YES;
			renderMode.pixelOut = obj->pixelOutAlpha;
		    gDPSetAlphaCompare(glistp++, G_AC_DITHER);
			xluFact = obj->pixelOutAlpha;
		}
		else if(renderMode.pixelOut)
		{
			renderMode.pixelOut = 0;
			changeRenderMode = YES;
		    gDPSetAlphaCompare(glistp++, G_AC_NONE);
			if(!renderMode.xluSurf)
				xluFact = 255;
		}
		/*if((obj->phongTex) && (renderMode.phongAllowed))
		{
			if((lookatNum < MAX_LOOKATS) && (hiliteNum < MAX_HILITES))
			{
				phongX = obj->phongTex->sx;
				phongY = obj->phongTex->sy;

				if(changeRenderMode)
					SetRenderMode();
				changeRenderMode = 0;
				renderMode.useTextureMode = 1;//TEXTURE_MODE_NORMAL;
				renderMode.phongMode = ON;
				if(obj->flags & OBJECT_FLAGS_FACE_COLOUR)
				{
					/*if((currentDrawActor->objectType == BALL_TYPE_OBJECT_CRYSTAL_BALL) && (crystalBall))
					{
						SubVector(&tempVect,&crystalBall->pos,&camera->pos);
						MakeUnit(&tempVect);
						ScaleVector(&tempVect,-127);
						guLookAtHilite(&nomatrix, 
							&dynamicp->lookat[lookatNum], &dynamicp->hilite[hiliteNum],
							actualCamPos[draw_buffer].v[X],actualCamPos[draw_buffer].v[Y],actualCamPos[draw_buffer].v[Z],
							currentDrawActor->pos.v[X], currentDrawActor->pos.v[Y],currentDrawActor->pos.v[Z],
							camVect.v[X],camVect.v[Y],camVect.v[Z],
							0,1,0, 	// ignored 
							tempVect.v[X],tempVect.v[Y],tempVect.v[Z],
							phongX, phongY);	// texture width 
					}
					else
					{	*/
		/*				guLookAtHilite(&nomatrix, 
							&dynamicp->lookat[lookatNum], &dynamicp->hilite[hiliteNum],
							actualCamPos[draw_buffer].v[X],actualCamPos[draw_buffer].v[Y],actualCamPos[draw_buffer].v[Z],
							currentDrawActor->pos.v[X], currentDrawActor->pos.v[Y],currentDrawActor->pos.v[Z],
							camVect.v[X],camVect.v[Y],camVect.v[Z],
							0,1,0, 	// ignored 
							backgroundLightList.head.next->direction.v[X],backgroundLightList.head.next->direction.v[Y],backgroundLightList.head.next->direction.v[Z], 	// light 1 direction 
							phongX, phongY);	// texture width 
					//}
				}
				else
				{
					guLookAtHilite(&nomatrix, 
						&dynamicp->lookat[lookatNum], &dynamicp->hilite[hiliteNum],
						actualCamPos[draw_buffer].v[X],actualCamPos[draw_buffer].v[Y],actualCamPos[draw_buffer].v[Z],
						currentDrawActor->pos.v[X], currentDrawActor->pos.v[Y],currentDrawActor->pos.v[Z],
						camVect.v[X],camVect.v[Y],camVect.v[Z],
						backgroundLightList.head.next->direction.v[X],backgroundLightList.head.next->direction.v[Y],backgroundLightList.head.next->direction.v[Z], 	// light 1 direction 
						0,1,0, 	// light 2 direction 
						phongX, phongY);	// texture width 
				}

				gSPLookAt(glistp++, &dynamicp->lookat[lookatNum]);

				gDPPipeSync(glistp++);
				gSPSetGeometryMode(glistp++,G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH | 
					G_CULL_BACK | G_LIGHTING | G_TEXTURE_GEN);

				gDPSetCycleType(glistp++,G_CYC_2CYCLE);

				gSPTexture(glistp++,(phongX<<6),(phongY<<6), 0, G_TX_RENDERTILE+1, G_ON);

				gDPSetTexturePersp(glistp++,G_TP_PERSP);
				gDPSetTextureFilter(glistp++,G_TF_BILERP);

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Light 1 colour
				gDPSetPrimColor(glistp++,0, 0, 255, 255, 255, xluFact);


				LoadPhongTexture(obj->phongTex);

				if(obj->flags & OBJECT_FLAGS_FACE_COLOUR)
				{
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Light 1 colour

					if(renderMode.actorColour)
					{
						gDPSetEnvColor(glistp++,envCol.r*(255-currentDrawActor->stats->inShadow)/255, envCol.g*(255-currentDrawActor->stats->inShadow)/255, envCol.b*(255-currentDrawActor->stats->inShadow)/255, xluFact);
					}
					else
					{
						gDPSetEnvColor(glistp++,envCol.r, envCol.g, envCol.b, xluFact);
					}

					gDPSetCombineMode(glistp++,G_CC_HILITEFACECOLOUR1, G_CC_HILITEFACECOLOUR2);
					if(obj->phongTex->pixSize == G_IM_SIZ_4b)
					{
						gDPSetTile(glistp++,obj->phongTex->format, obj->phongTex->pixSize, (((obj->phongTex->sx)+7)>>4),
							0, G_TX_RENDERTILE+2, 0, G_TX_WRAP | G_TX_NOMIRROR,
							obj->phongTex->TCScaleX, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, obj->phongTex->TCScaleY, G_TX_NOLOD);
					}
					else if(obj->phongTex->pixSize == G_IM_SIZ_16b)
					{
						gDPSetTile(glistp++,obj->phongTex->format, obj->phongTex->pixSize, (((obj->phongTex->sx * obj->phongTex->pixSize)+7)>>3),
							256, G_TX_RENDERTILE+2, 0, G_TX_WRAP | G_TX_NOMIRROR,
							obj->phongTex->TCScaleX, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, obj->phongTex->TCScaleY, G_TX_NOLOD);
					}
					else
					{
						gDPSetTile(glistp++,obj->phongTex->format, obj->phongTex->pixSize, (((obj->phongTex->sx * obj->phongTex->pixSize)+7)>>3),
							0, G_TX_RENDERTILE+2, 0, G_TX_WRAP | G_TX_NOMIRROR,
							obj->phongTex->TCScaleX, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, obj->phongTex->TCScaleY, G_TX_NOLOD);
					}
					gDPSetHilite2Tile(glistp++,G_TX_RENDERTILE+2, &dynamicp->hilite[hiliteNum], obj->phongTex->sx,obj->phongTex->sy);

				}
				else
				{
					gDPSetCombineMode(glistp++,G_CC_HILITERGB_PRIMA, G_CC_PASS2);
				}
				if(renderMode.xluSurf)
				{
					if(renderMode.pixelOut)
					{
						if(renderMode.useZMode)
						{
							gDPSetRenderMode(glistp++, G_RM_PASS,G_RM_ZB_CLD_SURF2);      
						}
						else
						{
							gDPSetRenderMode(glistp++, G_RM_PASS,G_RM_CLD_SURF2);      
						}
					}
					else
					{
						if(renderMode.useZMode)
						{
							gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_AA_ZB_XLU_SURF2);
						}
						else
						{
							gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_AA_XLU_SURF2);
						}
					}
				}
				else
				{
					if(renderMode.transparentSurf)
					{
						if(renderMode.useZMode)
						{
							gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_AA_ZB_TEX_EDGE2);
						}
						else
						{
							gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_AA_TEX_EDGE2);
						}
					}
					else
					{
						if(renderMode.pixelOut)
						{
							if(renderMode.useZMode)
							{
								gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_AA_ZB_PCL_SURF2);
							}
							else
							{
								gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_AA_PCL_SURF2);
							}
						}
						else
						{
							if(renderMode.useZMode)
							{
								gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_AA_ZB_OPA_SURF2);
							}
							else
							{
								gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_AA_OPA_SURF2);
							}
						}
					}
				}
				gDPSetHilite1Tile(glistp++,G_TX_RENDERTILE+1, &dynamicp->hilite[hiliteNum], phongX,phongY);
				lookatNum++;
				hiliteNum++;
			}
			else
				dprintf"RUN OUT OF HILITES!!!!!!!!!!!!!!\n"));
		}*/
		/*else*/ if(renderMode.phongMode)
		{
			renderMode.phongMode = 0;
			changeRenderMode = YES;
			gDPPipeSync(glistp++);
		    gDPSetCycleType(glistp++, G_CYC_1CYCLE);
			gSPClearGeometryMode(glistp++,G_TEXTURE_GEN);
			gSPTexture(glistp++,0xffff, 0xffff, 0, G_TX_RENDERTILE, G_ON);
		}
		/*if(obj->flags & OBJECT_FLAGS_MIP_MAP)
		{
		    gDPPipeSync(glistp++);
			renderMode.useTextureMode = 2;//TEXTURE_MODE_MIP_MAP;
			SetRenderMode();
			gSPTexture(glistp++, 0xffff, 0xffff, MIP_MAP_NUMTILES, MIP_MAP_STARTTILE, G_ON);
			gDPSetTextureLOD(glistp++, G_TL_LOD);
//			gDPPipelineMode(glistp++, G_PM_1PRIMITIVE);
			gDPSetCombineMode(glistp++, G_CC_MYTRILERP, G_CC_MODULATERGB_MODULATEPRIMA2); 
			gDPSetCycleType(glistp++, G_CYC_2CYCLE);
		}*/
		/*else*/ if(obj->flags & OBJECT_FLAGS_TEXTURE_BLEND)
		{
		    gDPPipeSync(glistp++);
			renderMode.useTextureMode = 3;//TEXTURE_MODE_BLEND;
			SetRenderMode();
			gSPTexture(glistp++, 0xffff, 0xffff, 0, G_TX_RENDERTILE, G_ON);
			gDPSetTextureLOD(glistp++, G_TL_TILE);
			/*if(obj->flags & OBJECT_FLAGS_TEXTURE_BLEND_ENV)
			{
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//				gDPSetEnvColor(glistp++,0,0,0,obj->textureBlendAlpha);
				gDPSetEnvColor(glistp++,0,0,0,SineWave(80,frameCount)*127 + 127);
				gDPSetCombineMode(glistp++, G_CC_TEXTUREBLENDENV, G_CC_TEXTUREBLEND2); 
			}
			else
			{  */
				gDPSetCombineMode(glistp++, G_CC_TEXTUREBLEND, G_CC_TEXTUREBLEND2); 
			/*}*/
			gDPSetCycleType(glistp++, G_CYC_2CYCLE);
		}
		else if(obj->flags & OBJECT_FLAGS_COLOUR_BLEND)
		{
			gDPPipeSync(glistp++);
			renderMode.useTextureMode = 4;//TEXTURE_MODE_COLOUR_BLEND;
			SetRenderMode();
			gSPTexture(glistp++, 0xffff, 0xffff, 0, G_TX_RENDERTILE, G_ON);
			gDPSetTextureLOD(glistp++, G_TL_TILE);
			gDPSetEnvColor(glistp++,obj->colour.r,obj->colour.g,obj->colour.b,obj->colour.a);
			if(obj->flags & OBJECT_FLAGS_FACE_COLOUR)
			{
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//do only when xluFact for object changes to save time - need an oldxlu type thang
				if(obj->flags & OBJECT_FLAGS_XLU)
					SetXluInDrawList(obj->drawList,xluFact);
				gDPSetCombineMode(glistp++,G_CC_FACECOLOURBLEND,G_CC_FACECOLOURBLEND2);
			}
			else
			{
				gDPSetCombineMode(glistp++,G_CC_COLOURBLEND,G_CC_COLOURBLEND2);
			}
			gDPSetCycleType(glistp++, G_CYC_2CYCLE);
		}
		else if(obj->flags & OBJECT_FLAGS_COLOUR_BLEND_AFTERLIGHT)
		{
			gDPPipeSync(glistp++);
			renderMode.useTextureMode = 5;//TEXTURE_MODE_COLOUR_BLEND_AFTERLIGHT;
			SetRenderMode();
			gSPTexture(glistp++, 0xffff, 0xffff, 0, G_TX_RENDERTILE, G_ON);
			gDPSetTextureLOD(glistp++, G_TL_TILE);
			gDPSetEnvColor(glistp++,obj->colour.r,obj->colour.g,obj->colour.b,obj->colour.a);
			if(obj->flags & OBJECT_FLAGS_FACE_COLOUR)
			{
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//do only when xluFact for object changes to save time - need an oldxlu type thang
				if(obj->flags & OBJECT_FLAGS_XLU)
					SetXluInDrawList(obj->drawList,xluFact);
				gDPSetCombineMode(glistp++,G_CC_FACECOLOURBLEND_AFTERLIGHT,G_CC_FACECOLOURBLEND_AFTERLIGHT2);
			}
			else
			{
				gDPSetCombineMode(glistp++,G_CC_COLOURBLEND_AFTERLIGHT,G_CC_COLOURBLEND_AFTERLIGHT2);
			}
			gDPSetCycleType(glistp++, G_CYC_2CYCLE);
		}
		else if(renderMode.useTextureMode != 1/*TEXTURE_MODE_NORMAL*/)
		{
		    gDPPipeSync(glistp++);
			renderMode.useTextureMode = 1;//TEXTURE_MODE_NORMAL;
			gDPSetTextureLOD(glistp++, G_TL_TILE);
			gSPTexture(glistp++, 0xffff, 0xffff, 0, G_TX_RENDERTILE, G_ON);
			gDPSetCycleType(glistp++, G_CYC_1CYCLE);
			changeRenderMode = TRUE;
		}
		if(changeRenderMode)
			SetRenderMode();
	}
	else
	{

		if(obj->flags & OBJECT_FLAGS_XLU)
		{
			tempFloat = (float)xluOverride / 100;
			tempFloat *= (float)obj->xlu;
			if(tempFloat > 255)
				tempFloat = 255;
			xluFact = tempFloat;
		}
		gDPSetPrimColor(glistp++,0,0,/*r*/255,/*g*/255,/*b*/255,xluFact);
//		gDPSetPrimColor(glistp++,0,0,/*r*/255,/*g*/255,/*b*/255,obj->xlu);
	}

	SetRendermodeForEnviroment();
}


/*	--------------------------------------------------------------------------------
	Function		: XfmPoint
	Purpose			: transforms a point
	Parameters		: VECTOR *,VECTOR *
	Returns			: 
	Info			: 
*/

long DIST		= 0;
long FOV		= 512;
float horizClip	= 300;
float vertClip	= 225;

void XfmPoint(VECTOR *vTemp2,VECTOR *in)
{
	float c[4][4];
	guLookAtF(c,
			currCamTarget[screenNum].v[X],currCamTarget[screenNum].v[Y],currCamTarget[screenNum].v[Z],
			currCamSource[screenNum].v[X],currCamSource[screenNum].v[Y],currCamSource[screenNum].v[Z],
			//stx,sty,stz,
			//ctx,cty,ctz,
			//0,1,0);
			camVect.v[X],camVect.v[Y],camVect.v[Z]);

	guMtxXFMF(c,in->v[X],in->v[Y],in->v[Z],
		&(vTemp2->v[X]),&(vTemp2->v[Y]),&(vTemp2->v[Z]));
	
		if  (((vTemp2->v[Z]+DIST)>nearPlaneDist) &&
			((vTemp2->v[Z]+DIST)<farPlaneDist) &&
			((vTemp2->v[X])>-horizClip) &&
			((vTemp2->v[X])<horizClip) &&
			((vTemp2->v[Y])>-vertClip) &&
			((vTemp2->v[Y])<vertClip))
		{
			float oozd = -1/(vTemp2->v[Z]+DIST);
			vTemp2->v[X] = 320+((vTemp2->v[X] * FOV) * oozd);
			vTemp2->v[Y] = 220+((vTemp2->v[Y] * FOV) * oozd);
		}
		else
			vTemp2->v[Z] = 0;
}
























































































//#ifdef POOOO
//PC ONLY

/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.
	
	  
	File		: Subcube.c
	Programmer	: Andrew Eder
	Date		: 11/24/98
	
----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

#include <ultra64.h>

#include "incs.h"

//extern float xluFade;

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

short texture[256*256];
float nearClip = 10;
float farClip = 1100;

float horizClip = 600;
float vertClip = 550;


long InitTex(void)
{
	long i=0;
	for (i=0; i<256*256; i++)
		texture[i] = i;
	return 0;
}

VECTOR tV[4000];
float mV[4000];

long DIST=0;
long FOV=512;

float stx=124,sty=93, stz=864;
float ctx=124,cty=-224,ctz=451;
// Outcode enum
enum {OUTCODE_LEFT, OUTCODE_RIGHT, OUTCODE_TOP, OUTCODE_BOTTOM};

// Outcode macros for clipping.
#define OUTCODEF_LEFT	(1<<OUTCODE_LEFT)
#define OUTCODEF_RIGHT	(1<<OUTCODE_RIGHT)
#define OUTCODEF_TOP	(1<<OUTCODE_TOP)
#define OUTCODEF_BOTTOM	(1<<OUTCODE_BOTTOM)

// Calculate the actual outcode
#define CALC_OUTCODE(x,y,x0,y0,x1,y1) \
						((((y)>=(y1))<<OUTCODE_BOTTOM)| \
						 (((y)<=(y0))<<OUTCODE_TOP)| \
						 (((x)>=(x1))<<OUTCODE_RIGHT)| \
						 (((x)<=(x0))<<OUTCODE_LEFT))

// The 2D clip Volume.
float clx0 = 1,
      cly0 = 1,
	  clx1 = 639,
	  cly1 = 479;

// Subtracted Square!
#define sbsqr(x,y) ((x-y)*(x-y))

float xl = 1;

/*---------------------------------------------------------------------------------------------
	Function	: calcIntVertex
	Parameters	: (D3DTLVERTEX *vOut, int outcode, D3DTLVERTEX *v0,D3DTLVERTEX *v1, float cx0, float cy0, float cx1, float cy1)
	Returns		: inline int 
*/

float   rCol,gCol,bCol;

int calcIntVertex(D3DTLVERTEX *vOut, int outcode, D3DTLVERTEX *v0,D3DTLVERTEX *v1, float cx0, float cy0, float cx1, float cy1)
{
	float segLen, totalLen, vt;
	long r1,g1,b1,a1;
	long r2,g2,b2,a2;

	switch(outcode)
	{
		case OUTCODE_LEFT:
			segLen = cx0-v0->sx;
			totalLen = v1->sx-v0->sx;
			vt = segLen/totalLen;
			vOut->sx = cx0;
			vOut->sy = v0->sy+((v1->sy-v0->sy)*vt);
			break;
		case OUTCODE_RIGHT:
			segLen = cx1-v0->sx;
			totalLen = v1->sx-v0->sx;
			vt = segLen/totalLen;
			vOut->sx = cx1;
			vOut->sy = v0->sy+((v1->sy-v0->sy)*vt);
			break;
		case OUTCODE_TOP:
			segLen = cy0-v0->sy;
			totalLen = v1->sy-v0->sy;
			vt = segLen/totalLen;
			vOut->sx = v0->sx+((v1->sx-v0->sx)*vt);
			vOut->sy = cy0;
			break;
		case OUTCODE_BOTTOM:
			segLen = cy1-v0->sy;
			totalLen = v1->sy-v0->sy;
			vt = segLen/totalLen;
			vOut->sx = v0->sx+((v1->sx-v0->sx)*vt);
			vOut->sy = cy1;
			break;		
	}
			
	vOut->tu = (v0->tu+((v1->tu-v0->tu)*vt));
    vOut->tv = (v0->tv+((v1->tv-v0->tv)*vt));
	vOut->sz = (v0->sz+((v1->sz-v0->sz)*vt));
	
	a1 = RGBA_GETALPHA(v0->color);
	r1 = RGBA_GETRED(v0->color);
	g1 = RGBA_GETGREEN(v0->color);
	b1 = RGBA_GETBLUE(v0->color);

	a2 = RGBA_GETALPHA(v1->color);
	r2 = RGBA_GETRED(v1->color);
	g2 = RGBA_GETGREEN(v1->color);
	b2 = RGBA_GETBLUE(v1->color);

	vOut->color = RGBA_MAKE ((long)(r1+(r2-r1)*vt),(long)(g1+(g2-g1)*vt),(long)(b1+(b2-b1)*vt),(long)(a1+(a2-a1)*vt));

	vOut->specular = 0;
	return !((vOut->sx==v0->sx)&&(vOut->sy==v0->sy));
}

void Clip3DPolygon (D3DTLVERTEX in[3], long texture)
{
	D3DTLVERTEX		vList[10];
	int				out0, out1;
	D3DTLVERTEX 	*v0ptr, *v1ptr, vBuf1[8], vBuf2[8], *vIn, *vOut, *vTmp;
	int				vInCount, vOutCount;
	int				sideLp, vertLp, sideMask;
	short			faceList[8*3];
	long			numFaces,i,j;
	// Set up in & out buffers											

	vIn = vBuf1;
	
	memcpy (vIn,in,sizeof(D3DTLVERTEX)*3);
	//vIn[0].sx = x1;
	//vIn[0].sy = y1;
	//vIn[1].sx = x2;
	//vIn[1].sy = y2;
	//vIn[2].sx = x3;
	//vIn[2].sy = y3;
	
	//vIn[0].tu = u1;
	//vIn[0].tv = v1;
	//vIn[1].tu = u2;
	//vIn[1].tv = v2;
	//vIn[2].tu = u3;
	//vIn[2].tv = v3;
	
	vInCount = 3;
	vOut = vBuf2;
	
	vOutCount = 0;

	for(sideLp=0; sideLp<4; sideLp++)
	{
		sideMask = 1<<sideLp;
		for(vertLp=0; vertLp<vInCount; vertLp++)
		{
			v0ptr = &vIn[vertLp];
			if ((vertLp+1)<vInCount)
				v1ptr = &vIn[(vertLp+1)];
			else
				v1ptr = vIn;

			out0 = CALC_OUTCODE(v0ptr->sx,v0ptr->sy, clx0,cly0,clx1,cly1);
			out1 = CALC_OUTCODE(v1ptr->sx,v1ptr->sy, clx0,cly0,clx1,cly1);
			if ((out0 & sideMask)==0)		// v0 on
			{
				// add v0 to output
				memcpy(&vOut[vOutCount++], v0ptr, sizeof(D3DTLVERTEX));
				if (out1 & sideMask)		// v0 on, v1 off
				{
					// add intersection to output
					if (calcIntVertex(&vOut[vOutCount], sideLp, v0ptr, v1ptr, clx0,cly0,clx1,cly1))
						vOutCount++;
				}
			}
			else
			{
				if ((out1 & sideMask)==0)	// v0 off, v1 on
				{
					// add intersection to output
					if (calcIntVertex(&vOut[vOutCount], sideLp, v1ptr, v0ptr, clx0,cly0,clx1,cly1))
						vOutCount++;
				}
			}
		}
		vTmp = vIn;					// Swap in & out bufs
		vIn = vOut;
		vOut = vTmp;
		vInCount = vOutCount;
		vOutCount = 0;
	}

	numFaces = 0;
	j=0;
	for (i=1; i<(vInCount-1); i++, j+=3)
	{
		faceList[j] = 0;
		faceList[j+1] = i;
		faceList[j+2] = i+1;
		numFaces++;
	}
	
	if (numFaces)
	{
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZENABLE,1);
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,1);
		DrawAHardwarePoly(vIn,vInCount,faceList,j,texture);
	}

/*	xb = vIn[0].sx;
	yb = vIn[0].sy;
	ub = vIn[0].tu;
	vb = vIn[0].tv;

	for (int i=1; i<(vInCount-1); i++)
	{
		DrawPolygon (
			xb, yb,
				ub, vb,
			vIn[i].sx, vIn[i].sy,
				vIn[i].tu, vIn[i].tv,
			vIn[i+1].sx, vIn[i+1].sy, 
				vIn[i+1].tu, vIn[i+1].tv);
	}*/

}

void XfmPoint (VECTOR *vTemp2, VECTOR *in)
{
	float c[4][4];
	guLookAtF(c,
			currCamTarget[screenNum].v[X],currCamTarget[screenNum].v[Y],currCamTarget[screenNum].v[Z],
			currCamSource[screenNum].v[X],currCamSource[screenNum].v[Y],currCamSource[screenNum].v[Z],
			//stx,sty,stz,
			//ctx,cty,ctz,
			//0,1,0);
			camVect.v[X],camVect.v[Y],camVect.v[Z]);

	guMtxXFMF(c,in->v[X],in->v[Y],in->v[Z],
		&(vTemp2->v[X]),&(vTemp2->v[Y]),&(vTemp2->v[Z]));
	
		if  (((vTemp2->v[Z]+DIST)>nearClip) &&
			((vTemp2->v[Z]+DIST)<farClip) &&
			((vTemp2->v[X])>-horizClip) &&
			((vTemp2->v[X])<horizClip) &&
			((vTemp2->v[Y])>-vertClip) &&
			((vTemp2->v[Y])<vertClip))
		{
			float oozd = -1/(vTemp2->v[Z]+DIST);
			vTemp2->v[X] = 320+((vTemp2->v[X] * FOV) * oozd);
			vTemp2->v[Y] = 220+((vTemp2->v[Y] * FOV) * oozd);
		}
		else
			vTemp2->v[Z] = 0;
	
}

float sinSpeedWater = 0.2;
float sinSpeedWater2 = 0.1;
float sinSpeedVert = 0.4;
float sinSpeedVert2 = 0.4;
float scVC = 0.02;
float valWater = 2;
float rval = 50000;
float rval2 = 15;
float svall = 0.03;
float fmody = 0.4,fmody2 = 0.9;
float sclW = 1;
float thresh = 900;
float sofs = 5;

long noClipping = 0;

void PCDrawObject(OBJECT *obj, float m[4][4])
{
	short fce[3] = {0,1,2};		
	D3DTLVERTEX v[3],*vTemp;
	float *mTemp;
	VECTOR *in,*c1,*c2,*c3,*vTemp2;
	float c[4][4];
	float f[4][4];
	int i;
	long drawme;
	
	in = obj->mesh->vertices;

	//if ((xl<0.99) || (xluFade<0.99))
	{
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,FALSE);
	}

	guLookAtF(c,
			currCamTarget[screenNum].v[X],currCamTarget[screenNum].v[Y],currCamTarget[screenNum].v[Z],
			currCamSource[screenNum].v[X],currCamSource[screenNum].v[Y],currCamSource[screenNum].v[Z],
			//stx,sty,stz,
			//ctx,cty,ctz,
			//0,1,0);
			camVect.v[X],camVect.v[Y],camVect.v[Z]);
	
	guMtxCatF(m,c,f);
	
	vTemp2 = tV;
	
	for (i=0; i<obj->mesh->numVertices; i++)
	{
		if (xl<0.99)
		{
			if (player[0].frogState & FROGSTATUS_ISDEAD)
			{
				player[0].lives = 5;
				if (frog[0]->action.deathBy & DEATHBY_DROWNING)
				{
					float val;
					VECTOR me,me2;
					me.v[X] = in->v[X]*sclW;
					me.v[Y] = in->v[Y]*sclW;
					me.v[Z] = in->v[Z]*sclW;

					guMtxXFMF(m,me.v[X],me.v[Y],me.v[Z],
								&me2.v[0],&me2.v[1],&me2.v[2]); 
					
					val = 1+DistanceBetweenPoints2DSquared(&destTile[0]->centre,&me2);
					if (val<thresh)
						val = thresh;
					val = rval / (val/rval2);

					val = (1+sinf(val*fmody2+frameCount*fmody)) * val * svall;
					if (frog[0]->action.dead>sofs)
						val *= ((frog[0]->action.dead-sofs) / 75.0);
					else
						val*=0;

					val += (2+(sin(in->v[Z]*sinSpeedVert+frameCount*sinSpeedWater)+cos(in->v[X]*sinSpeedVert2+frameCount*sinSpeedWater2)))*valWater;
					
					mV[i] = (2+(sin(in->v[Z]*sinSpeedVert+frameCount*sinSpeedWater)+cos(in->v[X]*sinSpeedVert2+frameCount*sinSpeedWater2)))*valWater;

					guMtxXFMF(f,in->v[X],in->v[Y] - val, in->v[Z],

					&(vTemp2->v[X]),&(vTemp2->v[Y]),&(vTemp2->v[Z]));
				}
			}
			else
			{
				mV[i] =(	(2+(sin(in->v[Z]*sinSpeedVert+frameCount*sinSpeedWater)+cos(in->v[X]*sinSpeedVert2+frameCount*sinSpeedWater2))) + 
					(2+(sin((in->v[Z]+50)*sinSpeedVert+frameCount*sinSpeedWater)+cos((in->v[X]+50)*sinSpeedVert2+frameCount*sinSpeedWater2)))
					)*valWater/2;
				guMtxXFMF(f,in->v[X],in->v[Y]-
				mV[i]
				,in->v[Z],
				&(vTemp2->v[X]),&(vTemp2->v[Y]),&(vTemp2->v[Z]));
			}
		}
		else
		{
			guMtxXFMF(f,in->v[X],in->v[Y],in->v[Z],
			&(vTemp2->v[X]),&(vTemp2->v[Y]),&(vTemp2->v[Z]));
		}
	
		if (((vTemp2->v[Z]+DIST)>nearClip) &&
			((noClipping) ||   
			(((vTemp2->v[Z]+DIST)<farClip) &&
			((vTemp2->v[X])>-horizClip) &&
			((vTemp2->v[X])<horizClip) &&
			((vTemp2->v[Y])>-vertClip) &&
			((vTemp2->v[Y])<vertClip))))
		{
			float oozd = -1/(vTemp2->v[Z]+DIST);
			vTemp2->v[X] = 320+((vTemp2->v[X] * FOV) * oozd);
			vTemp2->v[Y] = 220+((vTemp2->v[Y] * FOV) * oozd);
		}
		else
			vTemp2->v[Z] = 0;

		vTemp2++;
		in++;
		mV[i]*=scVC;
	}
	
	for (i=0; i<obj->mesh->numFaces; i++)
	{
		long v0 = obj->mesh->faceIndex[i].v[0];
		long v1 = obj->mesh->faceIndex[i].v[1];
		long v2 = obj->mesh->faceIndex[i].v[2];
		long v0a,v1a,v2a;
		TEXENTRY *tex = (TEXENTRY *)(obj->mesh->textureIDs[i]);

		v0a = i*3;
		v1a = v0a+1;
		v2a = v1a+1;

		if (tV[v0].v[Z])
		if (tV[v1].v[Z])
		if (tV[v2].v[Z])
		if (tex)
		{
			c1 = &(((VECTOR *)obj->mesh->vertexNormals)[v0a]);
			c2 = &(((VECTOR *)obj->mesh->vertexNormals)[v1a]);
			c3 = &(((VECTOR *)obj->mesh->vertexNormals)[v2a]);
			
			drawme = 1;

			vTemp = v;
			
			vTemp->specular = 0;
			vTemp->sx = tV[v0].v[X];
			vTemp->sy = tV[v0].v[Y];
			vTemp->sz = (tV[v0].v[Z]+DIST)/2000;///2000;
			vTemp->tu = (obj->mesh->faceTC[v0a].v[0]*(1.0/1024.0));
			vTemp->tv = (obj->mesh->faceTC[v0a].v[1]*(1.0/1024.0));
			vTemp->color = D3DRGBA(c1->v[2],c1->v[1],c1->v[0],xl);
			if (xl<0.99)
			{
				vTemp->specular = D3DRGB(mV[v0],mV[v0],mV[v0]);
			}
			else
			{
				vTemp->specular = D3DRGB(rCol,gCol,bCol);
			}

			vTemp++;

			vTemp->specular = 0;
			vTemp->sx = tV[v1].v[X];
			vTemp->sy = tV[v1].v[Y];
			vTemp->sz = (tV[v1].v[Z]+DIST)/2000;//2000;
			vTemp->tu = (obj->mesh->faceTC[v1a].v[0]*(1.0/1024.0));
			vTemp->tv = (obj->mesh->faceTC[v1a].v[1]*(1.0/1024.0));
			vTemp->color = D3DRGBA(c2->v[2],c2->v[1],c2->v[0],xl);
			if (xl<0.99)
			{
				vTemp->specular = D3DRGB(mV[v1],mV[v1],mV[v1]);
			}
			else
			{
				vTemp->specular = D3DRGB(rCol,gCol,bCol);
			}

			vTemp++;

			vTemp->specular = 0;
			vTemp->sx = tV[v2].v[X];
			vTemp->sy = tV[v2].v[Y];
			vTemp->sz = (tV[v2].v[Z]+DIST)/2000;///2000;
			vTemp->tu = (obj->mesh->faceTC[v2a].v[0]*(1.0/1024.0));
			vTemp->tv = (obj->mesh->faceTC[v2a].v[1]*(1.0/1024.0));
			vTemp->color = D3DRGBA(c3->v[2],c3->v[1],c3->v[0],xl);
			if (xl<0.99)
			{
				vTemp->specular = D3DRGB(mV[v2],mV[v2],mV[v2]);
			}
			else
			{
				vTemp->specular = D3DRGB(rCol,gCol,bCol);
			}
		

			if (v[0].sx <0)
				if (v[1].sx<0)
					if (v[2].sx<0)
						drawme = 0;
			
			if (v[0].sy <0)
				if (v[1].sy<0)
					if (v[2].sy<0)
						drawme = 0;

			if (v[0].sx >640)
				if (v[1].sx>640)
					if (v[2].sx>640)
						drawme = 0;
			
			if (v[0].sy >480)
				if (v[1].sy>480)
					if (v[2].sy>480)
						drawme = 0;

			if (drawme)
				Clip3DPolygon(v,tex->hdl);
		
		/*	SetTexture (obj->mesh->textureIDs[i]);
			if (obj->mesh->textureIDs[i])
			{
				ClipPolygon(
					tV[v0].v[X],tV[v0].v[Y],
						obj->mesh->faceTC[v0a].v[0]>>2,
						obj->mesh->faceTC[v0a].v[1]>>2,
					tV[v1].v[X],tV[v1].v[Y],
						obj->mesh->faceTC[v1a].v[0]>>2,
						obj->mesh->faceTC[v1a].v[1]>>2,
					tV[v2].v[X],
					tV[v2].v[Y],
						obj->mesh->faceTC[v2a].v[0]>>2,
						obj->mesh->faceTC[v2a].v[1]>>2);
			}*/

		}
	}

//	if ((xl<0.99) || (xluFade<0.99))
	{
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,TRUE);
	}

	return;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

/*
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
*/
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
//		memcpy(tempActor->shadow->vert,shadowVtx,sizeof(Vtx) * 4);
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
	if( !(initFlags & INIT_SWARM) )
	{
		FindObject(&tempActor->objectController, UpdateCRC(name), name,YES);
		if(!tempActor->objectController)
			return;
	}
	else
		tempActor->objectController = NULL;

	InitActorStructures(tempActor, initFlags);

	tempActor->pos.v[X] = x;
	tempActor->pos.v[Y] = y;
	tempActor->pos.v[Z] = z;
	tempActor->scale.v[X] = tempActor->scale.v[Y] = tempActor->scale.v[Z] = 1;

//init actors rotation
	ZeroQuaternion(&tempActor->qRot);
	InitTex();

//	tempActor->currentcolor.v[0] = 0;
//	tempActor->currentcolor.v[1] = 0;
//	tempActor->currentcolor.v[2] = 0;
}

//extern ACTOR2* watert;
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
	return;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	:  
	Returns 	: 
	Info 		:
*/

long TestDistanceFromFrog = 0;
/*
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

		//store the static mtx
		SetMatrix ((float *)&obj->myMatrix,(float *)matrixStack.result);//&dynamicp->modeling4[objectMatrix]);
	}
	else
		matrixStackCount++;


	if((calcMtx == TRUE) || (staticObj == FALSE))
	{

		PopMatrix();

//transform children
		if(obj->children)
		{
			parentScaleStackLevel++;
			parentScaleStack[parentScaleStackLevel].v[X] = XScaleAmount
			parentScaleStack[parentScaleStackLevel].v[Y] = YScaleAmount
			parentScaleStack[parentScaleStackLevel].v[Z] = ZScaleAmount
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
*/
/*---------------------------------------------------------------------------------------------
	Function	: DrawMyObject
	Parameters	: (OBJECT *obj)
	Returns		: void 
*/
/*
void DrawObject(OBJECT *obj)
{
	float	tempFloat;

	changeRenderMode = 0;
	
	PushMatrix(&obj->myMatrix);
	ComputeResultMatrix();	
	PopMatrix();
	matrixStackCount++;
	
	if(obj->mesh->numFaces)
	{
		PCDrawObject(obj,(float *)matrixStack.result);
	}

	matrixStackCount--;

	if(obj->children)
		DrawObject(obj->children);
	
	if(obj->next)
		DrawObject(obj->next);
}
*/

void DrawActor(ACTOR *ptr)
{
	float rotmat[4][4], scalemat[4][4], transmat[4][4];
	float	animTime = 0;

	QUATERNION tempQ;
	
//	rCol = (((float)ptr->currentcolor.v[0])/256.0);
//	gCol = (((float)ptr->currentcolor.v[0])/256.0);
//	bCol = (((float)ptr->currentcolor.v[0])/256.0);

	ptr->matrix = NULL;
	if(dispFrameCount == 1)
	{
		//this is the first time objects are drawn, so store matrix if static object
//		if(ptr->type == LANDSCAPE_ACTOR)
//		{`
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
//		calcMtx = FALSE;	//use precalculated matrix

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
	{
		noClipping = 0;
		
		if (ptr->objectController->object->name[0] == '_')
			noClipping = 1;

	//	TransformAndDrawObject(ptr->objectController->object, animTime, 0, 0);
	}
	
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

/*
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
	VECTOR	tPos;

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

		tPos.v[0] = XTranslateAmount * actorScale.v[X] * parentScaleStack[parentScaleStackLevel].v[X];
		tPos.v[1] = YTranslateAmount * actorScale.v[Y] * parentScaleStack[parentScaleStackLevel].v[Y];
		tPos.v[2] = ZTranslateAmount * actorScale.v[Z] * parentScaleStack[parentScaleStackLevel].v[Z];

	    guTranslateF(transmat,tPos.v[0],tPos.v[1],tPos.v[2]);
					
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

	changeRenderMode = 0;
		xluFact = 0xff;
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
		}
		else
		{
		}


//here we need the result of our matrix stack 
	if((calcMtx == TRUE) || (staticObj == FALSE))
	{
		ComputeResultMatrix();
	

		//store the static mtx
		if(calcMtx == TRUE)
		{
			if(staticObjectMtx)
			{
			}
		}
		matrixStackCount++;
	}
	else
	{
		matrixStackCount++;

	}

	if(obj->mesh->numFaces)
	{
		if (1)//!TestDistanceFromFrog)
		{
			xl = xluFact / 256.0;
			PCDrawObject (obj,matrixStack.result);
		}
		else
		{
			if (DistanceBetweenPointsSquared ( &tPos, &frog[0]->actor->pos ) < 300000.0)
			{
				xl = xluFact / 256.0;
				PCDrawObject (obj,matrixStack.result);
			}
		}
	}

		
//->		gSPPopMatrix(glistp++, G_MTX_MODELVIEW);
		matrixStackCount--;

	if((calcMtx == TRUE) || (staticObj == FALSE))
	{

		PopMatrix();

//transform children
		if(obj->children)
		{
			parentScaleStackLevel++;
			parentScaleStack[parentScaleStackLevel].v[X] = XScaleAmount;
			parentScaleStack[parentScaleStackLevel].v[Y] = YScaleAmount;
			parentScaleStack[parentScaleStackLevel].v[Z] = ZScaleAmount;
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
*/

#endif