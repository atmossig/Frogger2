/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.
	
		File		: Subcube.c
		Programmer	: Andy Eder
		Date		: 15/09/99 14:43:27
		
		 ----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

#include <ultra64.h>
#include "incs.h"
#include "mavis.h"

#define LODDist (700 * 700)

#define FOGADJ(x) (1.0-((x-fStart)*fEnd))
#define FOGVAL(y) (((unsigned long)(255*y) << 24))

#define BETWEEN(x,a,b) ((x>a) && (x<b))

VECTOR pointVec = {0,0,1};
extern long waterObject;
extern long modgyObject;
extern long HALF_WIDTH,HALF_HEIGHT;
extern long runHardware;

extern long leafObject;

float hedSpeed = 0.2;
void TransformObject(OBJECT *obj, float time);
void PCDrawObject(OBJECT *obj, float m[4][4]);

long useNear = 0;

ACTOR2 *hat[MAX_FROGS];

#define SETALPHA(rgba, x) ((((long)(x)) << 24) | ((rgba & 0x00ffffff)))
#define MODALPHA(rgba, x) ((((long) ((x*(rgba>>24))>>8)) << 24) | ((rgba & 0x00ffffff)))

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

float nearClip = 1;
float farClip = 2500;

float horizClip = 3000;
float vertClip = 2500;

long DIST=-10;
long FOV=450;

#define IsNotClipped(vTemp2) (((vTemp2->v[Z]+DIST)>nearClip) && ((noClipping) || (((vTemp2->v[Z]+DIST)<farClip) && ((vTemp2->v[X])>-horizClip) && ((vTemp2->v[X])<horizClip) && ((vTemp2->v[Y])>-vertClip) && ((vTemp2->v[Y])<vertClip))))

///////////////////////////////////////////////////////////////////////////////////////////

VECTOR tV[8000];
VECTOR nV[8000];
float mV[8000];

//float sinSpeedWater = 0.2;
//float sinSpeedWater2 = 0.1;
//float sinSpeedVert = 0.4;
//float sinSpeedVert2 = 0.4;
//float scVC = 0.02;
//float valWater = 2;
float rval = 50000;
//float rval2 = 15;
//float svall = 0.03;
//float fmody = 0.4,fmody2 = 0.9;
//float sclW = 1;
//float thresh = 900;
//float sofs = 5;

long noClipping = 0;
long numFacesDrawn;
long numPixelsDrawn;
extern float RES_DIFF;

float oneOver[65535];
void InitOneOverTable(void)
{
	int i;
	for(i=1; i<65535; i++)
		oneOver[i] = (RES_DIFF)/(float)i;
}


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
	float segLen, totalLen, vt,fogAmt;
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
	if (vOut->sx > 6400000)
		vOut->sx += 0;
	if (vOut->sy > 4800000)
		vOut->sy += 0;

	vOut->tu = (v0->tu+((v1->tu-v0->tu)*vt));
    vOut->tv = (v0->tv+((v1->tv-v0->tv)*vt));
	vOut->sz = (v0->sz+((v1->sz-v0->sz)*vt));
	
	fogAmt = FOGADJ(vOut->sz);
	if (fogAmt<0)
		fogAmt=0;
	if (fogAmt>1)
		fogAmt=1;
	vOut->specular = FOGVAL(fogAmt);
	
	a1 = RGBA_GETALPHA(v0->color);
	r1 = RGBA_GETRED(v0->color);
	g1 = RGBA_GETGREEN(v0->color);
	b1 = RGBA_GETBLUE(v0->color);

	a2 = RGBA_GETALPHA(v1->color);
	r2 = RGBA_GETRED(v1->color);
	g2 = RGBA_GETGREEN(v1->color);
	b2 = RGBA_GETBLUE(v1->color);

	vOut->color = RGBA_MAKE ((long)(r1+(r2-r1)*vt),(long)(g1+(g2-g1)*vt),(long)(b1+(b2-b1)*vt),(long)(a1+(a2-a1)*vt));

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
		PushPolys(vIn,vInCount,faceList,j,texture);
	}
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

unsigned long worldObject = 0;
unsigned long thresh = 500*500;

#define DistanceFail(m1,m2) (DistanceBetweenPointsSquared(m1,m2)>thresh)
		
void DrawObject(OBJECT *obj, Gfx *drawList, int skinned, MESH *masterMesh)
{
	long drawme;

	drawme = 1;

	// If we are a skinned object then we just need to prepare all the skinned vertices, so do that for this sub-object.
	if (skinned)
		PCPrepareSkinnedObject(obj, masterMesh,  obj->objMatrix.matrix);
	else
	{ 
		//if (worldObject && DistanceFail(&frog[0]->actor->pos,obj->))
		//	drawme = 0;
		if ((obj->name[0] == 'g') && (obj->name[1] == 'l') && (obj->name[2] == '_'))
		{
			VECTOR v,cVect,oVect,cVect2,oVect2;
			float horiz,horiz2;

			SwapFrame(4);
			PCPrepareObject(obj, obj->mesh,  obj->objMatrix.matrix);
			PCRenderObject(obj);
			v.v[X] = obj->objMatrix.matrix[3][0];
			v.v[Y] = obj->objMatrix.matrix[3][1];
			v.v[Z] = obj->objMatrix.matrix[3][2];
						
			SubVector(&cVect,&currCamSource,&currCamTarget);
			SubVector(&oVect,&currCamSource,&v);
			cVect.v[1] = 0;
			oVect.v[1] = 0;
			MakeUnit(&cVect);
			MakeUnit(&oVect);
			
			horiz = fabs(0.7+acos(DotProduct(&cVect,&oVect))*2.5);
			horiz = 0.2+fabs(cosf(horiz))*0.8;
			horiz2 = fabs(1.2+acos(DotProduct(&cVect,&oVect))*2.5);
			horiz2 = 0.2+fabs(sinf(horiz2))*0.8;

			//AddHalo(&v,vMatrix[2][0],vMatrix[2][2],SETALPHA(*((long *)(&(((VECTOR *)obj->mesh->vertexNormals)->v[0]))),(long)(0.8*0xff)),obj->xlu);
			AddHalo(&v,horiz,horiz2,SETALPHA(*((long *)(&(((VECTOR *)obj->mesh->vertexNormals)->v[0]))),(long)(0.8*0xff)),obj->xlu);
			SwapFrame(0);
		}
		else
		{
			if (obj->mesh && drawme)
			{
				xl = (((float)obj->xlu) / ((float)0xff)) * xl;
				
				// We are a water object, if so modge the vertices like water
				if ((waterObject))
				{
					PCPrepareWaterObject(obj, obj->mesh,  obj->objMatrix.matrix);

					// Draw it, eventually water will need it's own optimised draw function!
					PCRenderObject(obj);
				}
				else // Or we can modge just the vertices
					if (modgyObject) 
					{
						PCPrepareModgyObject(obj, obj->mesh,  obj->objMatrix.matrix);
						// Same as for water objects
						PCRenderObject(obj);
					}
					else // Or maybe we can't
					{
						PCPrepareObject(obj, obj->mesh,  obj->objMatrix.matrix);
						PCRenderObject(obj);
					}
			}
		}
	}

	// Recurse.
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

	guMtxXFMF(vMatrix,in->v[X],in->v[Y],in->v[Z],

		
		&(vTemp2->v[X]),&(vTemp2->v[Y]),&(vTemp2->v[Z]));
	
		if  (((vTemp2->v[Z]+DIST)>nearClip) &&
			((vTemp2->v[Z]+DIST)<farClip) &&
			((vTemp2->v[X])>-horizClip) &&
			((vTemp2->v[X])<horizClip) &&
			((vTemp2->v[Y])>-vertClip) &&
			((vTemp2->v[Y])<vertClip))
		{
			long x = vTemp2->v[Z]+DIST;
			float oozd = -FOV * oneOver[x];///(vTemp2->v[Z]+DIST);
		//	float oozd = -1/(vTemp2->v[Z]+DIST);
						
			vTemp2->v[X] = HALF_WIDTH+(vTemp2->v[X] * oozd);
			vTemp2->v[Y] = HALF_HEIGHT+(vTemp2->v[Y] * oozd);			
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
		tempActor->shadow->alpha/* = tempActor->shadow->alphaAim */= 255;
		tempActor->shadow->draw = 1;
//		tempActor->shadow->alphaAimSpeed = 10;
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

	if (!objectC)
		return;

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
MATRIX hatMat[4];
ACTOR2 *currentDrawActor2;

void TransformObject(OBJECT *obj, float time)
{
	VECTOR translation,scale;
	VECTOR	morphFrom,morphTo;
	SPRITE	*sprite;
	float	tempFloat,interp;
	short i, j;
	short	fromKey, toKey;
	short	xluVal;
	unsigned long wasHed;

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

	// Handle Scale Keyframes
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

	wasHed = 0;
	if (strncmp(obj->name,"fghed",5) == 0)
	{
		float rotmat2[4][4];
		QUATERNION quat, rot = {0,1,0,0};
		VECTOR actVec = {0,0,0};
		wasHed = 1;

		if(player[0].canJump)
		{
			if (pointOfInterest)
			{
				SubVector (&actVec,pointOfInterest,&(frog[0]->actor->pos));
			}
			else
			{
				SetVector (&actVec,&(currTile[0]->dirVector[frogFacing[0]]));
			}

			MakeUnit (&actVec);

			pointVec.v[X] += (actVec.v[X] - pointVec.v[X]) * hedSpeed;
			pointVec.v[Y] += (actVec.v[Y] - pointVec.v[Y]) * hedSpeed;
			pointVec.v[Z] += (actVec.v[Z] - pointVec.v[Z]) * hedSpeed;
		
			MakeUnit (&pointVec);

			// Could cause problems if point pass through frog... Should never Happen.
	
			CrossProduct( (VECTOR *)&rot, &(currTile[0]->dirVector[frogFacing[0]]),&pointVec);
			rot.w = DotProduct(&pointVec,&(currTile[0]->dirVector[frogFacing[0]]));
			if (rot.w>1)
				rot.w = 0;
			else
				rot.w = acos(rot.w);

			GetQuaternionFromRotation (&quat,&rot);
			QuaternionToMatrix(&quat, (MATRIX*)rotmat2);
			guMtxCatF(rotmat,rotmat2,rotmat);
		}
		else
		{
			SetVector(&pointVec,&(currTile[0]->dirVector[frogFacing[0]]));
		}
	}


	rotmat[3][0] = translation.v[X] * actorScale->v[X] * parentScaleStack[parentScaleStackLevel].v[X];
	rotmat[3][1] = translation.v[Y] * actorScale->v[Y] * parentScaleStack[parentScaleStackLevel].v[Y];
	rotmat[3][2] = translation.v[Z] * actorScale->v[Z] * parentScaleStack[parentScaleStackLevel].v[Z];

	PushMatrix(rotmat);
	guScaleF(scalemat, scale.v[X] * actorScale->v[X], scale.v[Y] * actorScale->v[Y], scale.v[Z] * actorScale->v[Z]);

	PushMatrix(scalemat);

	if (wasHed)
	{
		if (hat[0])
		{
			guMtxXFMF(matrixStack.stack[matrixStack.stackPosition], 0, 0, 0,
									&(hat[0]->actor->pos.v[X]),&(hat[0]->actor->pos.v[Y]),&(hat[0]->actor->pos.v[Z]));
			SetVector(&(hat[0]->actor->scale),&scale);
			hat[0]->actor->scale.v[X] *= actorScale->v[X];
			hat[0]->actor->scale.v[Y] *= actorScale->v[Y];
			hat[0]->actor->scale.v[Z] *= actorScale->v[Z];
			MatrixToQuaternion (matrixStack.stack[matrixStack.stackPosition],&(hat[0]->actor->qRot));
		}
	}

	//if the object has a collision sphere
/*	if(obj->collSphere)
	{
		SetVector(&obj->collSphere->oldOffset, &obj->collSphere->offset);
		guMtxXFMF(matrixStack.stack[matrixStack.stackPosition], 0, 0, 0,&obj->collSphere->offset.v[X], &obj->collSphere->offset.v[Y], &obj->collSphere->offset.v[Z]);
	}
*/
//	maintain posision of all objects sprites        
	if(obj->numSprites > 0)
	{
		for(i = 0; i < obj->numSprites; i++)
		{
			sprite = obj->sprites[i].sprite;

			if (currentDrawActor2)
				sprite->draw = currentDrawActor2->draw;					 
						
			if(sprite)
			{
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
				obj->sprites[i].sprite->scaleX = 2*((float)obj->sprites[i].sx * actorScale->v[X] * scale.v[X]);
				obj->sprites[i].sprite->scaleY = 2*((float)obj->sprites[i].sy * actorScale->v[Y] * scale.v[Y]);
				xluVal = ((int)(obj->xlu * xluOverride))/100;
				if(xluOverride <= 10)
					xluVal = 0;

		/*		if((currentDrawActor->objectType == BALL_TYPE_OBJECT_SPAWN) || (currentDrawActor->type == DEBRIS))
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
			*/
				obj->sprites[i].sprite->a = xluVal;
				sprite->offsetX = -16;
				sprite->offsetY = -16;

				/*
					"TEMPORARY" SPRITE FIX (ahahaha..) - Dave

					I'm not sure why this works; clearly the sprite's z-value is the
					negative of what it should be. Where does this negative creep in? 
				*/
				guMtxXFMF(matrixStack.stack[matrixStack.stackPosition], obj->sprites[i].x, obj->sprites[i].y, -obj->sprites[i].z,
												&sprite->pos.v[X], &sprite->pos.v[Y], &sprite->pos.v[Z]);
			}
		}
	}


		
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
	// Optimisation.
	OBJECT_CONTROLLER *objectC = actor->objectController;

	// I can't see you...
	if(actor->visible == FALSE)
		return;

	// xluOverride is used in the sprite engine, and xl is used within the main polygon engine.
	xluOverride = actor->xluOverride;
	xl = (float)xluOverride/100.0F;

	// If we are skinned then XForm all the vertices first and then draw the skin, otherwise draw it as we transform it.
	if (objectC->vtxBuf)
	{
		DrawObject(objectC->object, objectC->object->drawList, TRUE, objectC->object->mesh);
		PCRenderObject(objectC->object);
	}
	else
		DrawObject(objectC->object, objectC->object->drawList, FALSE, objectC->object->mesh);
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
	float f[4][4];
	VECTOR *in;
	VECTOR *vTemp2;
	unsigned long i,j;
	
	guMtxCatF(m,vMatrix,f);

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
		vTemp2->v[X] = (f[0][0]*in->v[X])+(f[1][0]*in->v[Y])+(f[2][0]*in->v[Z])+(f[3][0]);
		vTemp2->v[Y] = (f[0][1]*in->v[X])+(f[1][1]*in->v[Y])+(f[2][1]*in->v[Z])+(f[3][1]);
		vTemp2->v[Z] = (f[0][2]*in->v[X])+(f[1][2]*in->v[Y])+(f[2][2]*in->v[Z])+(f[3][2]);

		// Transform to screen space
		if (IsNotClipped(vTemp2))
		{
			long x = vTemp2->v[Z]+DIST;
			float oozd = -FOV * oneOver[x];///(vTemp2->v[Z]+DIST);
			
			vTemp2->v[X] = HALF_WIDTH+(vTemp2->v[X] * oozd);
			vTemp2->v[Y] = HALF_HEIGHT+(vTemp2->v[Y] * oozd);
		}
		else
		{
			vTemp2->v[Z] = 0;
		}

	}
}

void PCPrepareObject (OBJECT *obj, MESH *me, float m[4][4])
{
	float f[4][4];
	VECTOR *in;
	VECTOR *vTemp2;
	unsigned long i;

	in = obj->mesh->vertices;

	guMtxCatF(m,vMatrix,f);
	
	vTemp2 = tV;
	
	for (i=0; i<obj->mesh->numVertices; i++)
	{
		vTemp2->v[X] = (f[0][0]*in->v[X])+(f[1][0]*in->v[Y])+(f[2][0]*in->v[Z])+(f[3][0]);
		vTemp2->v[Y] = (f[0][1]*in->v[X])+(f[1][1]*in->v[Y])+(f[2][1]*in->v[Z])+(f[3][1]);
		vTemp2->v[Z] = (f[0][2]*in->v[X])+(f[1][2]*in->v[Y])+(f[2][2]*in->v[Z])+(f[3][2]);

		if (((vTemp2->v[Z]+DIST)>nearClip) &&
			((noClipping) ||   
			(((vTemp2->v[Z]+DIST)<farClip) &&
			((vTemp2->v[X])>-horizClip) &&
			((vTemp2->v[X])<horizClip) &&
			((vTemp2->v[Y])>-vertClip) &&
			((vTemp2->v[Y])<vertClip))))
		{
			long x = vTemp2->v[Z]+DIST;
			float oozd = -FOV * oneOver[x];///(vTemp2->v[Z]+DIST);
			
			vTemp2->v[X] = HALF_WIDTH+(vTemp2->v[X] * oozd);
			vTemp2->v[Y] = HALF_HEIGHT+(vTemp2->v[Y] * oozd);
		}
		else
			vTemp2->v[Z] = 0;

		vTemp2++;
		in++;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/



/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

float modi1 = 0.05;
float modi2 = 1.29;
float modi3 = 2.5;
float modi4 = 0.08;
float modi5 = 0.1; // Small value for calculating normals.
float modi6 = 5; // Small value for calculating normals.

void PCPrepareWaterObject (OBJECT *obj, MESH *me, float m[4][4])
{
	float f[4][4];
	VECTOR *in;
	VECTOR *vTemp2,*vTemp4;
	float *vTemp3,t,t2,t2x,t2z,xval,zval;
	unsigned long i;

	in = obj->mesh->vertices;

	
	guMtxCatF(m,vMatrix,f);
	
	vTemp2 = tV;
	vTemp4 = nV;

	vTemp3 = mV;
	t = actFrameCount * modi1;
	for (i=0; i<obj->mesh->numVertices; i++)
	{
		xval=in->v[X]*modi2;
		zval=in->v[Z]*modi2;

		t2 = sinf(t+xval*zval*0.5) - cosf(t+xval*0.3*zval);
	//	t2x = sinf(t+(xval+modi5)+zval*0.5) * cosf(t+(xval+modi5)*0.3+zval);
	//	t2z = sinf(t+xval+(zval+modi5)*0.5) * cosf(t+xval*0.3+(zval+modi5));

		
	//	vTemp4->v[X] = (t2 - t2x)*modi6;
	//	vTemp4->v[Z] = (t2 - t2z)*modi6;
//		MakeUnit(vTemp4);
//		ScaleVector(vTemp4,modi6);
		*vTemp3 = t2 * modi4;

		vTemp2->v[X] = (f[0][0]*in->v[X])+(f[1][0]*in->v[Y])+(f[2][0]*in->v[Z])+(f[3][0]);
		vTemp2->v[Y] = (f[0][1]*in->v[X])+(f[1][1]*in->v[Y])+(f[2][1]*in->v[Z])+(f[3][1]);
		vTemp2->v[Z] = (f[0][2]*in->v[X])+(f[1][2]*in->v[Y])+(f[2][2]*in->v[Z])+(f[3][2]);
		vTemp2->v[Y]+=t2 * modi3;

		if (((vTemp2->v[Z]+DIST)>nearClip) &&
			((noClipping) ||   
			(((vTemp2->v[Z]+DIST)<farClip) &&
			((vTemp2->v[X])>-horizClip) &&
			((vTemp2->v[X])<horizClip) &&
			((vTemp2->v[Y])>-vertClip) &&
			((vTemp2->v[Y])<vertClip))))
		{
			long x = vTemp2->v[Z]+DIST;
			float oozd = -FOV * oneOver[x];///(vTemp2->v[Z]+DIST);
			
			vTemp2->v[X] = HALF_WIDTH+(vTemp2->v[X] * oozd);
			vTemp2->v[Y] = HALF_HEIGHT+(vTemp2->v[Y] * oozd);
		}
		else
			vTemp2->v[Z] = 0;

		vTemp2++;
		vTemp3++;
		vTemp4++;
		in++;
	}
}

void PCPrepareModgyObject (OBJECT *obj, MESH *me, float m[4][4])
{
	float f[4][4];
	VECTOR *in;
	VECTOR *vTemp2,*vTemp4;
	float *vTemp3,t,t2,t2x,t2z,xval,zval;
	unsigned long i;

	in = obj->mesh->vertices;
	
	guMtxCatF(m,vMatrix,f);
	
	vTemp2 = tV;
	vTemp4 = nV;

	vTemp3 = mV;
	t = actFrameCount * modi1 * 0.5;
	for (i=0; i<obj->mesh->numVertices; i++)
	{
		xval=in->v[X]*modi2;
		zval=in->v[Z]*modi2;

		t2 = sinf(t+xval*zval*0.2) - cosf(t+xval*0.4*zval);
	//	t2x = sinf(t+(xval+modi5)+zval*0.5) * cosf(t+(xval+modi5)*0.3+zval);
	//	t2z = sinf(t+xval+(zval+modi5)*0.5) * cosf(t+xval*0.3+(zval+modi5));

		
	//	vTemp4->v[X] = (t2 - t2x)*modi6;
	//	vTemp4->v[Z] = (t2 - t2z)*modi6;
//		MakeUnit(vTemp4);
//		ScaleVector(vTemp4,modi6);
		*vTemp3 = t2 * modi4;

		vTemp2->v[X] = (f[0][0]*in->v[X])+(f[1][0]*in->v[Y])+(f[2][0]*in->v[Z])+(f[3][0]);
		vTemp2->v[Y] = (f[0][1]*in->v[X])+(f[1][1]*in->v[Y])+(f[2][1]*in->v[Z])+(f[3][1]);
		vTemp2->v[Z] = (f[0][2]*in->v[X])+(f[1][2]*in->v[Y])+(f[2][2]*in->v[Z])+(f[3][2]);
		
		if (((vTemp2->v[Z]+DIST)>nearClip) &&
			((noClipping) ||   
			(((vTemp2->v[Z]+DIST)<farClip) &&
			((vTemp2->v[X])>-horizClip) &&
			((vTemp2->v[X])<horizClip) &&
			((vTemp2->v[Y])>-vertClip) &&
			((vTemp2->v[Y])<vertClip))))
		{
			long x = vTemp2->v[Z]+DIST;
			float oozd = -FOV * oneOver[x];///(vTemp2->v[Z]+DIST);
			
			vTemp2->v[X] = HALF_WIDTH+(vTemp2->v[X] * oozd);
			vTemp2->v[Y] = HALF_HEIGHT+(vTemp2->v[Y] * oozd);
		}
		else
			vTemp2->v[Z] = 0;

		vTemp2++;
		vTemp3++;
		vTemp4++;
		in++;
	}
}

















float fogMulSpeed = 0.05;
float fogMulAmt = 0.15;
float fogMulBase = 0.7;

short facesON[3] = {0,1,2};


float naddr = 0.25;
float nmult = 4.0;
extern unsigned long sludgeObject;

void PCRenderObject (OBJECT *obj)
{
	unsigned long i,j;
	unsigned short fce[3] = {0,1,2};		
	QUATERNION *c1,*c2,*c3;
	D3DTLVERTEX v[3],*vTemp;
	USHORTVECTOR *facesIdx;
	unsigned long drawme,x1on,x2on,x3on,y1on,y2on,y3on;
	unsigned long v0,v1,v2;
	unsigned long v0a,v1a,v2a;
	long alphaVal;
	TEXENTRY *tex;
	TEXTURE **tex2;
	VECTOR *tV0,*tV1,*tV2;
	QUATERNION *cols;
	float t1,t2,t3,fogAmt;

	if (xl<0.99)
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,FALSE);
	else
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,TRUE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZENABLE,TRUE);

	facesIdx = obj->mesh->faceIndex;
	tex2 = obj->mesh->textureIDs;
	cols = (VECTOR *)obj->mesh->vertexNormals;
	alphaVal = (xl*255.0);

	for (j=0, i=0; i<obj->mesh->numFaces; i++, j+=3)
	{
		// Get information from the mesh!
		v0 = facesIdx->v[0];
		v1 = facesIdx->v[1];
		v2 = facesIdx->v[2];
		
		
		tV0 = &tV[v0];
		tV1 = &tV[v1];
		tV2 = &tV[v2];

		tex = (TEXENTRY *)(*tex2);
		
		// If we are to be drawn.
		if (((tV0->v[Z]) && (tV1->v[Z]) && (tV2->v[Z])) && (tex))
		{
			// Get rest of info from mesh
			v0a = j;
			v1a = j+1;
			v2a = j+2;

			c1 = &(cols[v0a]);
			c2 = &(cols[v1a]);
			c3 = &(cols[v2a]);
			
			// Fill out D3DVertices...
			if (runHardware)
			{
				vTemp = v;
				
				vTemp->sx = tV0->v[X];
				vTemp->sy = tV0->v[Y];
				vTemp->sz = (tV[v0].v[Z]) * 0.00025F;///2000;
				
				fogAmt = FOGADJ(vTemp->sz);
				if (fogAmt<0)
					fogAmt=0;
				if (fogAmt>1)
					fogAmt=1;
				vTemp->specular = FOGVAL(fogAmt);
				
				//SETALPHA(*((long *)(c1->v)),alphaVal); //,c1->v[1],c1->v[2],xl);
		//		vTemp->color = D3DRGBA(1,1,1,1);
				if (waterObject)
				{
					if (!sludgeObject && !leafObject)
						vTemp->color = SETALPHA(*((long *)(&(c1->x))),alphaVal*c1->w*((naddr+mV[v0])*nmult));
					else
						vTemp->color = SETALPHA(*((long *)(&(c1->x))),alphaVal*c1->w);

					if (!leafObject)
					{
						vTemp->tu = (obj->mesh->faceTC[v0a].v[0]*0.000975F)+mV[v0];
						vTemp->tv = (obj->mesh->faceTC[v0a].v[1]*0.000975F)+mV[v0];
					}
					else
					{
						vTemp->tu = (obj->mesh->faceTC[v0a].v[0]*0.000975F);
						vTemp->tv = (obj->mesh->faceTC[v0a].v[1]*0.000975F);
					}
					
				}
				else
				{
					if (modgyObject)
					{
						vTemp->color = SETALPHA(*((long *)(&(c1->x))),alphaVal*c1->w);
						vTemp->tu = (obj->mesh->faceTC[v0a].v[0]*0.000975F)+mV[v0];
						vTemp->tv = (obj->mesh->faceTC[v0a].v[1]*0.000975F)+mV[v0];
					}
					else
					{
						vTemp->color = MODALPHA(*((unsigned long *)(&(c1->x))),alphaVal);
						vTemp->tu = (obj->mesh->faceTC[v0a].v[0]*0.000975F);
						vTemp->tv = (obj->mesh->faceTC[v0a].v[1]*0.000975F);
					}
				}

			
				vTemp++;

				vTemp->sx = tV1->v[X];
				vTemp->sy = tV1->v[Y];
				vTemp->sz = (tV1->v[Z]) * 0.00025F;//2000;
				fogAmt = FOGADJ(vTemp->sz);
				if (fogAmt<0)
					fogAmt=0;
				if (fogAmt>1)
					fogAmt=1;
				vTemp->specular = FOGVAL(fogAmt);
				//SETALPHA(*((long *)(c2->v)),alphaVal); //,c1->v[1],c1->v[2],xl);
//				vTemp->color = D3DRGBA(1,1,1,1);

				if (waterObject)
				{
					if (!sludgeObject && !leafObject)
						vTemp->color = SETALPHA(*((long *)(&(c2->x))),alphaVal*c2->w*((naddr+mV[v1])*nmult));
					else
						vTemp->color = SETALPHA(*((long *)(&(c2->x))),alphaVal*c2->w);
					
					if (!leafObject)
					{
						vTemp->tu = (obj->mesh->faceTC[v1a].v[0]*0.000975F)+mV[v1];
						vTemp->tv = (obj->mesh->faceTC[v1a].v[1]*0.000975F)+mV[v1];
					}
					else
					{
						vTemp->tu = (obj->mesh->faceTC[v1a].v[0]*0.000975F);
						vTemp->tv = (obj->mesh->faceTC[v1a].v[1]*0.000975F);
					}
					
				}
				else
				{
					if (modgyObject)
					{
						vTemp->color = SETALPHA(*((long *)(&(c2->x))),alphaVal*c2->w);
						vTemp->tu = (obj->mesh->faceTC[v1a].v[0]*0.000975F)+mV[v1];
						vTemp->tv = (obj->mesh->faceTC[v1a].v[1]*0.000975F)+mV[v1];
					}
					else
					{
						vTemp->color = MODALPHA(*((unsigned long *)(&(c2->x))),alphaVal);
						vTemp->tu = (obj->mesh->faceTC[v1a].v[0]*0.000975F);
						vTemp->tv = (obj->mesh->faceTC[v1a].v[1]*0.000975F);
					}
				}
				
				vTemp++;
				
				vTemp->sx = tV2->v[X];
				vTemp->sy = tV2->v[Y];
				vTemp->sz = (tV2->v[Z]) * 0.00025F;///2000;

				fogAmt = FOGADJ(vTemp->sz);
				if (fogAmt<0)
					fogAmt=0;
				if (fogAmt>1)
					fogAmt=1;
				vTemp->specular = FOGVAL(fogAmt);
				//SETALPHA(*((long *)(c3->v)),alphaVal); //,c1->v[1],c1->v[2],xl);
	//			vTemp->color = D3DRGBA(1,1,1,1);

				if (waterObject)
				{
					if (!sludgeObject && !leafObject)
						vTemp->color = SETALPHA(*((long *)(&(c3->x))),alphaVal*c3->w*((naddr+mV[v2])*nmult));
					else
						vTemp->color = SETALPHA(*((long *)(&(c3->x))),alphaVal*c3->w);

					if (!leafObject)
					{
						vTemp->tu = (obj->mesh->faceTC[v2a].v[0]*0.000975F)+mV[v2];
						vTemp->tv = (obj->mesh->faceTC[v2a].v[1]*0.000975F)+mV[v2];
					}
					else
					{
						vTemp->tu = (obj->mesh->faceTC[v2a].v[0]*0.000975F);
						vTemp->tv = (obj->mesh->faceTC[v2a].v[1]*0.000975F);
					}
					
				}
				else
				{
					if (modgyObject)
					{
						vTemp->color = SETALPHA(*((long *)(&(c3->x))),alphaVal*c3->w);
						vTemp->tu = (obj->mesh->faceTC[v2a].v[0]*0.000975F)+mV[v2];
						vTemp->tv = (obj->mesh->faceTC[v2a].v[1]*0.000975F)+mV[v2];
					}
					else
					{
						vTemp->color = MODALPHA(*((unsigned long *)(&(c3->x))),alphaVal);
						vTemp->tu = (obj->mesh->faceTC[v2a].v[0]*0.000975F);
						vTemp->tv = (obj->mesh->faceTC[v2a].v[1]*0.000975F);
					}
				}
				
				x1on = BETWEEN(v[0].sx,0,SCREEN_WIDTH);
				x2on = BETWEEN(v[1].sx,0,SCREEN_WIDTH);
				x3on = BETWEEN(v[2].sx,0,SCREEN_WIDTH);
				y1on = BETWEEN(v[0].sy,0,SCREEN_HEIGHT);
				y2on = BETWEEN(v[1].sy,0,SCREEN_HEIGHT);
				y3on = BETWEEN(v[2].sy,0,SCREEN_HEIGHT);
	
				if ((x1on || x2on || x3on) && (y1on || y2on || y3on))
				{
					if ((x1on && x2on && x3on) && (y1on && y2on && y3on))
					{
						numFacesDrawn++;
						PushPolys(v,3,facesON,3,tex->cFrame->hdl);
					}
					else
					{
						numFacesDrawn++;
						Clip3DPolygon(v,tex->cFrame->hdl);
					}
				}
			}
			else
			{
				numFacesDrawn++;
				DrawSoftwarePolygon (
					tV0->v[X],tV0->v[Y],
					(obj->mesh->faceTC[v0a].v[0]*0.000975F) * 32,
					(obj->mesh->faceTC[v0a].v[1]*0.000975F) * 32,
					
					tV1->v[X],tV1->v[Y],
					(obj->mesh->faceTC[v1a].v[0]*0.000975F) * 32,
					(obj->mesh->faceTC[v1a].v[1]*0.000975F) * 32,

					tV2->v[X],tV2->v[Y],
					(obj->mesh->faceTC[v2a].v[0]*0.000975F) * 32,
					(obj->mesh->faceTC[v2a].v[1]*0.000975F) * 32,
					tex->data);
							
			}
		}

		// Update our pointers
		facesIdx++;
		tex2++;
	}

	if (xl<0.99)
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,TRUE);
	
}






/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	/*
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
		PushPolys(vIn,vInCount,faceList,j,texture);
	}
*/

// Clip a set of three vertices to a set of n vertices -> face order, all share first vertex ie... [F1(0,1,2), F2(0,3,4), F3(0,5,6) ... Fn(0,3n-1,3n)
// Also, we need the destination to _always_ be able to hold 8 more vertices as the result,
// but that should be ok _if_ we are using mavis.... (32000000000 verts!! Grr)

unsigned long __fastcall ScreenClip (D3DTLVERTEX *vIn)
{
	int				out0, out1;									// Outcodes for the two vertices on the current edge
	D3DTLVERTEX 	*v0ptr, *v1ptr, vBuf2[8], *vOut, *vTmp;		// Pointers for the two buffers
	int				vInCount, vOutCount;						// Counts
	int				sideLp, vertLp, sideMask;					// Loop and temp variables

	// Setup for the first round!
	vInCount = 3;
	vOut = vBuf2;
	vOutCount = 0;

	// Step through each edge
	for(sideLp=0; sideLp<4; sideLp++)
	{
		// Create a bitmask for testing this side
		sideMask = 1<<sideLp;

		// Loop through all the vertices in the current n-gon, and perform the checks for the edge V(vertLp) -> V(vertLp+1)
		for(vertLp=0; vertLp<vInCount; vertLp++)
		{
			// Vertex 0 = vIn : Optimisation possibility, don't index... Increment
			v0ptr = &vIn[vertLp];

			// Vertex 1 (With wrapping back to 0) : Same optimisation applicable!
			if ((vertLp+1)<vInCount)
				v1ptr = &vIn[(vertLp+1)];
			else
				v1ptr = vIn;

			// Calculate a clip outcode, a bitfield for clipping on each side!
			out0 = CALC_OUTCODE(v0ptr->sx,v0ptr->sy, clx0,cly0,clx1,cly1);
			out1 = CALC_OUTCODE(v1ptr->sx,v1ptr->sy, clx0,cly0,clx1,cly1);

			// V0 is on
			if ((out0 & sideMask)==0)		
			{
				// add v0 to output
				memcpy(&vOut[vOutCount++], v0ptr, sizeof(D3DTLVERTEX));
				// v0 on, v1 off
				if (out1 & sideMask)		
				{
					// add intersection to output
					if (calcIntVertex(&vOut[vOutCount], sideLp, v0ptr, v1ptr, clx0,cly0,clx1,cly1))
						vOutCount++;
				}
			}
			else 
			{ 
				// v0 off, v1 on
				if ((out1 & sideMask)==0)	
				{
					// add intersection to output generating edge with next vertex! (So hence only add one!)
					if (calcIntVertex(&vOut[vOutCount], sideLp, v1ptr, v0ptr, clx0,cly0,clx1,cly1))
						vOutCount++;
				} // else this edge is completely offscreen
			}
		}

		// Swap in & out bufs
		vTmp = vIn;			
		vIn = vOut;
		vOut = vTmp;
		vInCount = vOutCount;
		vOutCount = 0;
	}

	// vIn, because of the last swap now actually holds vOut.
	return vInCount;
}


// Essentially the pushpoly macro optimised into the data filling algorithm - NEED TO ADD CLIPPING! (Arrrgh - noooooo)!
void PCRenderObjectFast (OBJECT *obj)
{
	MESH *mesh = obj->mesh;							// Pointer to the mesh to avoid multiple dereferencing.
	D3DTLVERTEX *vtxList = obj->renderData;			// Again avoiding multiple dereferencing of obj->renderdata.
	VECTOR *vecList = tV;							// A pointer we can modify for the vector list.
	unsigned long i,nClipV;							// Loop counter, the number of clipped vertices.
	unsigned short i1,i2,i3;						// Face indices.
	unsigned short *f1,*f2,*f3;						// Face pointers.
	unsigned short *startF, *fIndex;				// More face pointer.
	unsigned long x1on,x2on,x3on,y1on,y2on,y3on;	// Trivial face rejection!	
	unsigned long oldFc,vCount;
	TEXENTRY *tex;
	TEXTURE **tex2;
	VECTOR *v1,*v2,*v3;

	// Store the start so that we dont have to increment the count every frame.
	startF = cFInfo->cF;
	vecList = tV;
	fIndex = mesh->faceIndex;

	// For every face
	i = mesh->numFaces;
	oldFc = cFInfo->nF;
	// If there are no faces then we cannot draw this object!
	if (!i)
		return;
	
	tex2 = obj->mesh->textureIDs;
	vCount = 0;

	// Otherwise, we can!
	while (--i)
	{	
		// Copy the information from the list of VECTOR's to the list of D3DTLVERTEX's, the actual setting of x&y could go within the if statement, once it's working.
		v1 = (vecList+(i1 = *(fIndex  )));
		v2 = (vecList+(i1 = *(fIndex+1)));
		v3 = (vecList+(i1 = *(fIndex+2)));

		fIndex+=3;
		
		// If all the vertices can be drawn? IEEE normalised fp numbers store 0.0 as 0x00000000 (cool, since the co-pro will only generate normalised numbers)
		if (*(((unsigned long *)v1)+2) &&
			*(((unsigned long *)v2)+2) &&
			*(((unsigned long *)v3)+2))
		{
			SetVector((VECTOR *)(vtxList  ),v1);
			SetVector((VECTOR *)(vtxList+1),v2);
			SetVector((VECTOR *)(vtxList+2),v3);
		
			// Clip / Test clip (Could I check against each edge in turn?)
			x1on = BETWEEN((vtxList)->sx,0,SCREEN_WIDTH);
			x2on = BETWEEN((vtxList+1)->sx,0,SCREEN_WIDTH);
			x3on = BETWEEN((vtxList+2)->sx,0,SCREEN_WIDTH);
			y1on = BETWEEN((vtxList)->sy,0,SCREEN_HEIGHT);
			y2on = BETWEEN((vtxList+1)->sy,0,SCREEN_HEIGHT);
			y3on = BETWEEN((vtxList+2)->sy,0,SCREEN_HEIGHT);

			if ((x1on || x2on || x3on) && (y1on || y2on || y3on))
			{
				if (tex = ((TEXENTRY *)*tex2))
				{
					if ((x1on && x2on && x3on) && (y1on && y2on && y3on))
					{
						// Not Clipped! Add it to the face list! And update the pointer
						*(cFInfo->cF) = cFInfo->nV;
						*(cFInfo->cF+1) = cFInfo->nV+1;
						*(cFInfo->cF+2) = cFInfo->nV+2;
						*cFInfo->cH = tex->cFrame->hdl;
						cFInfo->cH+=3;
						cFInfo->cF += 3;
						cFInfo->nF += 3;
						
						*(cFInfo->cV) = *(vtxList);
						*(cFInfo->cV+1) = *(vtxList+1);
						*(cFInfo->cV+2) = *(vtxList+3);
						
						cFInfo->cV+=3;
						cFInfo->nV+=3;					
					} 
					else
					{
			//			nClipV = ScreenClip(vtxList); 
						// Do nothing for now!
					}
				}
			} // ...else completely clipped! (wooo)			
		}
		tex2++;		
		vtxList+=3;

	}

	// Copy the vertices onto the vertex list
	i = (vtxList - obj->renderData);
	memcpy (cFInfo->cV,obj->renderData,i*sizeof(D3DTLVERTEX));
	cFInfo->cV+=i;
	
	// All done!
}
































































/*
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
			vTemp->color = D3DRGBA(c1->v[0],c1->v[1],c1->v[2],xl);

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
			vTemp->color = D3DRGBA(c2->v[0],c2->v[1],c2->v[2],xl);
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
			vTemp->color = D3DRGBA(c3->v[0],c3->v[1],c3->v[2],xl);
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

			//if (drawme)
			//	Clip3DPolygon(v,tex->hdl);
		

		}
	}

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,TRUE);
}
*/