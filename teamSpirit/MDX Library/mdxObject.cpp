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
#include "mgeReport.h"
#include "gelf.h"

#ifdef __cplusplus
extern "C"
{
#endif

MDX_OBJECT_BANK objectBanks[MAX_OBJECT_BANKS];
WORD fpuState;

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void SubdivideObject(MDX_OBJECT *me)
{
	MDX_MESH *mesh;
	MDX_VECTOR *newVerts,tVect;
	MDX_SHORTVECTOR *newFace;
	MDX_USHORT2DVECTOR *newFaceTC,tTC;
	MDX_QUATERNION *newGouraudColors;
	MDX_TEXENTRY **newTextures;
	unsigned long i,v0,v1,v2;
	float mag1,mag2,mag3,biggest,ut,vt;
		
	mesh = me->mesh;
	
	// Allocate enough vertices for double the current number of faces - too many at present, but enough just in case
	newVerts = new MDX_VECTOR[(mesh->numFaces*3) * 2];

	// The face structure should be the correct size.
	newFace = new MDX_SHORTVECTOR[mesh->numFaces * 2];
	newFaceTC = new MDX_USHORT2DVECTOR[(mesh->numFaces*3) * 2];
	
	// Gouraud colors!
	newGouraudColors = new MDX_QUATERNION[(mesh->numFaces*3) * 2];
	
	// Texture ID's
	newTextures = new MDX_TEXENTRY *[mesh->numFaces * 2];

	for (i=0; i<mesh->numFaces; i++)
	{
		// Need to reJig the face so we know which the hypotinuse is...
		SubVector(&tVect,&mesh->vertices[mesh->faceIndex[i].v[0]],&mesh->vertices[mesh->faceIndex[i].v[1]]);
		mag1 = mdxMagnitudeSquared(&tVect);

		SubVector(&tVect,&mesh->vertices[mesh->faceIndex[i].v[1]],&mesh->vertices[mesh->faceIndex[i].v[2]]);
		mag2 = mdxMagnitudeSquared(&tVect);

		SubVector(&tVect,&mesh->vertices[mesh->faceIndex[i].v[2]],&mesh->vertices[mesh->faceIndex[i].v[0]]);
		mag3 = mdxMagnitudeSquared(&tVect);

		// v0-v1 is hyp
		biggest = mag1;	
		v0 = 0; v1 = 1; v2 = 2;
		
		if (mag2>biggest)
		{
			biggest = mag2;
			v0 = 1; v1 = 2; v2 = 0;
		
		}

		if (mag3>biggest)
		{
			biggest = mag3;
			v0 = 2; v1 = 0; v2 = 1;
		}
		
		// Calculate the center vertex.
		SubVector(&tVect,&mesh->vertices[mesh->faceIndex[i].v[v1]],&mesh->vertices[mesh->faceIndex[i].v[v0]]);
		ScaleVector(&tVect,0.5);
		AddVector(&tVect,&mesh->vertices[mesh->faceIndex[i].v[v0]],&tVect);
		
		// Set the new Faces
		newFace[(i*2)].v[0] = (i*4)+0;
		newFace[(i*2)].v[1] = (i*4)+3;
		newFace[(i*2)].v[2] = (i*4)+2;
		newFace[1+(i*2)].v[0] = (i*4)+1;
		newFace[1+(i*2)].v[1] = (i*4)+2;
		newFace[1+(i*2)].v[2] = (i*4)+3;
		
		// Set the new Vectors
		SetVector(&newVerts[(i*4)],&mesh->vertices[mesh->faceIndex[i].v[v0]]);
		SetVector(&newVerts[(i*4)+1],&mesh->vertices[mesh->faceIndex[i].v[v1]]);
		SetVector(&newVerts[(i*4)+2],&mesh->vertices[mesh->faceIndex[i].v[v2]]);
		SetVector(&newVerts[(i*4)+3],&tVect);

		// Texture Coords
		tTC.v[0] = mesh->faceTC[(i*3) + v0].v[0] + (mesh->faceTC[(i*3) + v1].v[0] - mesh->faceTC[(i*3) + v0].v[0])/2;
		tTC.v[1] = mesh->faceTC[(i*3) + v0].v[1] + (mesh->faceTC[(i*3) + v1].v[1] - mesh->faceTC[(i*3) + v0].v[1])/2;

		newFaceTC[(i*6)+0] = mesh->faceTC[(i*3) + v0];
		newFaceTC[(i*6)+1] = tTC;
		newFaceTC[(i*6)+2] = mesh->faceTC[(i*3) + v2];
		newFaceTC[(i*6)+3] = mesh->faceTC[(i*3) + v1];
		newFaceTC[(i*6)+4] = mesh->faceTC[(i*3) + v2];
		newFaceTC[(i*6)+5] = tTC;
		
		// Texture IDs
		newTextures[(i*2)] = mesh->textureIDs[i];
		newTextures[(i*2)+1] = mesh->textureIDs[i];
	}

	if (me->flags & OBJECT_FLAGS_SUBDIVIDED)
	{
		delete mesh->vertices;
		delete mesh->faceIndex;
		delete mesh->faceTC;
		delete mesh->gouraudColors;
		delete mesh->textureIDs;
	}

	mesh->numVertices = mesh->numFaces*4;
	mesh->numFaces *= 2;	
	mesh->vertices = newVerts;
	mesh->faceIndex = newFace;
	mesh->faceTC = newFaceTC;
	mesh->gouraudColors = newGouraudColors;
	mesh->textureIDs = newTextures;
	
	me->flags |= OBJECT_FLAGS_SUBDIVIDED;
	
	if (me->next)
		SubdivideObject(me->next);

	if (me->children)
		SubdivideObject(me->children);
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void FindToFromVKeys(MDX_VKEYFRAME *keys,short *from,short *to,float *interp,float time,int numKeys)
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
		tempFloat = (float)abs(keys[toKey].time - keys[fromKey].time);
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

void FindToFromQKeys(MDX_QKEYFRAME *keys,short *from,short *to,float *interp,float time,int numKeys)
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
		tempFloat = (float)abs(keys[toKey].time - keys[fromKey].time);
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

void TransformObject(MDX_OBJECT *obj, float time)
{
	MDX_VECTOR translation,scale;
//	SPRITE	*sprite;
	float	interp;
	short	fromKey, toKey;
//	short	xluVal;
//	unsigned long wasHed;
	MDX_QUATERNION	tempQuat;
	float rotmat[4][4];
	float scalemat[4][4];

	//handle position keyframes
	if((obj->numMoveKeys > 1) && (currentDrawActor->animation))
	{
		FindToFromVKeys(obj->moveKeys,&fromKey,&toKey,&interp,time,obj->numMoveKeys);
		LinearInterp(&translation,&obj->moveKeys[fromKey].vect,&obj->moveKeys[toKey].vect,interp);
	}
	else
	{
		SetVector(&translation,&obj->moveKeys[0].vect);
	}

	// Handle Scale Keyframes
	if((obj->numScaleKeys > 1) && (currentDrawActor->animation))
	{
		FindToFromVKeys(obj->scaleKeys,&fromKey,&toKey,&interp,time,obj->numScaleKeys);
		LinearInterp(&scale,&obj->scaleKeys[fromKey].vect,&obj->scaleKeys[toKey].vect,interp);
	}
	else
	{
		SetVector(&scale,&obj->scaleKeys[0].vect);
	}

	// handle rotation keyframes
	if((obj->numRotateKeys > 1) && (currentDrawActor->animation))
	{
		FindToFromQKeys(obj->rotateKeys,&fromKey,&toKey,&interp,time,obj->numRotateKeys);
		QuatSlerp(&obj->rotateKeys[fromKey].quat, &obj->rotateKeys[toKey].quat, interp, &tempQuat);
			
		// convert back to rotation matrix
		QuaternionToMatrix(&tempQuat, (MDX_MATRIX *)rotmat);

	}
	else
	{
		QuaternionToMatrix(&obj->rotateKeys[0].quat, (MDX_MATRIX *)rotmat);		
	}

	
/* Frog Head Looking */

	rotmat[3][0] = translation.vx * actorScale->vx * parentScaleStack[parentScaleStackLevel].vx;
	rotmat[3][1] = translation.vy * actorScale->vy * parentScaleStack[parentScaleStackLevel].vy;
	rotmat[3][2] = translation.vz * actorScale->vz * parentScaleStack[parentScaleStackLevel].vz;

	PushMatrix(rotmat);
	guScaleF(scalemat, scale.vx * actorScale->vx, scale.vy * actorScale->vy, scale.vz * actorScale->vz);

	PushMatrix(scalemat);

	if (obj->attachedActor)
	{
		guMtxXFMF(matrixStack.stack[matrixStack.stackPosition], 0, 0, 0,
				&(obj->attachedActor->pos.vx),&(obj->attachedActor->pos.vy),&(obj->attachedActor->pos.vz));
			
		SetVector(&obj->attachedActor->scale,actorScale);
		MatrixToQuaternion ((MDX_MATRIX *)matrixStack.stack[matrixStack.stackPosition],&(obj->attachedActor->qRot));
	}
	
	MatrixSet (&obj->objMatrix.matrix,&matrixStack.stack[matrixStack.stackPosition]);

	if ((obj->bBox) && CheckBoundingBox(obj->bBox,&obj->objMatrix))
		obj->flags |= OBJECT_FLAGS_CLIPPED;
	else
		obj->flags &= ~OBJECT_FLAGS_CLIPPED;
	

	PopMatrix();


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

void FindObject(MDX_OBJECT_CONTROLLER **objCPtr, int objID, char *name)
{
	int x, y;
	char *temp;
			
	for(x = 0; x < MAX_OBJECT_BANKS; x++)
	{
		if(objectBanks[x].freePtr)
		{
			temp = objectBanks[x].data;
			for(y = 0; y < objectBanks[x].numObjects; y++)
			{
				if(objectBanks[x].objList[y].objectID == objID)
				{
					(*objCPtr) = objectBanks[x].objList[y].objCont;
					return;
				}

			}
		}
	}
	*objCPtr = NULL;

	return;
}

void RestoreObjectPointers(MDX_OBJECT *obj)
{
	int x,y;
	
	obj->attachedActor = NULL;
	
	if(obj->phong)
		obj->phong = GetTexEntryFromCRC ((long)obj->phong);

	if(obj->sprites)
		for(x = 0; x < obj->numSprites; x++)
			obj->sprites[x].textureID = GetTexEntryFromCRC ((long)obj->sprites[x].textureID);		
	
	if (obj->mesh)
	{
		float r,g,b,a;
		unsigned long dupCount = 0;
		
		// Sort object by texture for speedy drawing! OPTIMISE! Put in bankmanager!
		if (obj->mesh->numFaces)
		{
			unsigned long cTex = (unsigned long)obj->mesh->textureIDs[0];

			for (x=1; x<obj->mesh->numFaces; x++)
			{
				if ((unsigned long)obj->mesh->textureIDs[x] != cTex)
				{
					for (y=x; y<obj->mesh->numFaces; y++)
						if ((unsigned long)obj->mesh->textureIDs[y] == cTex)
							break;
					
					if (y!=obj->mesh->numFaces)
					{
						MDX_QUATERNION t;
						MDX_SHORTVECTOR t1;
						MDX_USHORT2DVECTOR t2;

						obj->mesh->textureIDs[y] = obj->mesh->textureIDs[x];
						obj->mesh->textureIDs[x] = (MDX_TEXENTRY *)cTex;
						
						t = ((MDX_QUATERNION *)obj->mesh->gouraudColors)[x*3];
						((MDX_QUATERNION *)obj->mesh->gouraudColors)[x*3] = ((MDX_QUATERNION *)obj->mesh->gouraudColors)[y*3];
						((MDX_QUATERNION *)obj->mesh->gouraudColors)[y*3] = t;

						t = ((MDX_QUATERNION *)obj->mesh->gouraudColors)[x*3+1];
						((MDX_QUATERNION *)obj->mesh->gouraudColors)[x*3+1] = ((MDX_QUATERNION *)obj->mesh->gouraudColors)[y*3+1];
						((MDX_QUATERNION *)obj->mesh->gouraudColors)[y*3+1] = t;

						t = ((MDX_QUATERNION *)obj->mesh->gouraudColors)[x*3+2];
						((MDX_QUATERNION *)obj->mesh->gouraudColors)[x*3+2] = ((MDX_QUATERNION *)obj->mesh->gouraudColors)[y*3+2];
						((MDX_QUATERNION *)obj->mesh->gouraudColors)[y*3+2] = t;
						
						t1 = obj->mesh->faceIndex[x];
						obj->mesh->faceIndex[x] = obj->mesh->faceIndex[y];
						obj->mesh->faceIndex[y] = t1;
						
						t2 = obj->mesh->faceTC[x*3];
						obj->mesh->faceTC[x*3] = obj->mesh->faceTC[y*3];
						obj->mesh->faceTC[y*3] = t2;
						
						t2 = obj->mesh->faceTC[x*3+1];
						obj->mesh->faceTC[x*3+1] = obj->mesh->faceTC[y*3+1];
						obj->mesh->faceTC[y*3+1] = t2;
						
						t2 = obj->mesh->faceTC[x*3+2];
						obj->mesh->faceTC[x*3+2] = obj->mesh->faceTC[y*3+2];
						obj->mesh->faceTC[y*3+2] = t2;
							
					}
				}

				cTex = (unsigned long)obj->mesh->textureIDs[x];
			}
		}

		for (x=0; x<obj->mesh->numFaces; x++)
		{
			obj->mesh->textureIDs[x] = GetTexEntryFromCRC( (long)obj->mesh->textureIDs[x]);
			if (obj->mesh->textureIDs[x])
			{
			/*	if (obj->mesh->textureIDs[x]->type == TEXTURE_NORMAL)
				{
					obj->mesh->faceTC[x*3+0].v[0]/=textureAdjustDivider;
					obj->mesh->faceTC[x*3+0].v[1]/=textureAdjustDivider;

					obj->mesh->faceTC[x*3+1].v[0]/=textureAdjustDivider;
					obj->mesh->faceTC[x*3+1].v[1]/=textureAdjustDivider;

					obj->mesh->faceTC[x*3+2].v[0]/=textureAdjustDivider;
					obj->mesh->faceTC[x*3+2].v[1]/=textureAdjustDivider;
	
					obj->mesh->faceTC[x*3+0].v[0]+=((obj->mesh->textureIDs[x]->xPos)*1024)/256;
					obj->mesh->faceTC[x*3+0].v[1]+=((obj->mesh->textureIDs[x]->yPos)*1024)/256;

					obj->mesh->faceTC[x*3+1].v[0]+=((obj->mesh->textureIDs[x]->xPos)*1024)/256;
					obj->mesh->faceTC[x*3+1].v[1]+=((obj->mesh->textureIDs[x]->yPos)*1024)/256;

					obj->mesh->faceTC[x*3+2].v[0]+=((obj->mesh->textureIDs[x]->xPos)*1024)/256;
					obj->mesh->faceTC[x*3+2].v[1]+=((obj->mesh->textureIDs[x]->yPos)*1024)/256;
				}*/
			} 
		}
		
		for (x=0; x<obj->mesh->numFaces*3; x++)
		{
			r = ((MDX_QUATERNION *)obj->mesh->gouraudColors)[x].x;
			g = ((MDX_QUATERNION *)obj->mesh->gouraudColors)[x].y;
			b = ((MDX_QUATERNION *)obj->mesh->gouraudColors)[x].z;
			a = ((MDX_QUATERNION *)obj->mesh->gouraudColors)[x].w;
			((long *)(& ((MDX_QUATERNION *)obj->mesh->gouraudColors)[x].x))[0] = D3DRGBA(r,g,b,a);			
		}

		
		
	}



	if(obj->children)
		RestoreObjectPointers(obj->children);
	if(obj->next)
		RestoreObjectPointers(obj->next);
}

BOOL MungeFPCW( WORD *pwOldCW )
{
    BOOL ret = FALSE;
    WORD wTemp, wSave; 
    __asm fstcw wSave    
	
	if (wSave & 0x300 ||            // Not single mode
        0x3f != (wSave & 0x3f) ||   // Exceptions enabled
        wSave & 0xC00)              // Not round to nearest mode    
	{
        __asm        
		{            
			mov ax, wSave
            and ax, not 300h    ;; single mode
            or  ax, 3fh         ;; disable all exceptions
            and ax, not 0xC00   ;; round to nearest mode
            mov wTemp, ax            
			fldcw   wTemp        
		}        
		ret = TRUE;
    }    
	*pwOldCW = wSave;    
	return ret;
} 

void LoadObjBank(char *bank, char *baseDir)
{
	char    file[MAX_PATH]; 
	char	*objectBank = NULL;
	long	x=-1,y;
    HINSTANCE handle = NULL;
	FARPROC func;

    strcpy (file,baseDir);	
    strcat (file,OBJECT_BASE);
	strcat (file,bank);
	strcat (file,".dll");

    handle = LoadLibrary(file);
	
	if (handle != NULL)
	{
		func = GetProcAddress(handle, "GetbankAddress");
		if (func != NULL)
		objectBank=(char *)func();
	}
	
	MungeFPCW(&fpuState);
	
	if( !objectBank )
	{
		dp("Unable to load object bank %s\n", bank);
		return;
	}

	while(objectBanks[++x].freePtr != 0);

	if(x >= MAX_OBJECT_BANKS)
	{
		dp("ERROR:too many object banks loaded\n");
		return;
	}

	objectBanks[x].freePtr = (char *)handle;
	objectBanks[x].objList = (MDX_OBJECT_DESCRIPTOR *)objectBank;

	y = 0;
	while(objectBanks[x].objList[y].objCont)
	{							   
		RestoreObjectPointers(objectBanks[x].objList[y].objCont->object);
		y++;
	}
	
	objectBanks[x].numObjects = y;	

	

	return;
}

void FreeObjectBank(long i)
{
	FreeLibrary((HINSTANCE)(objectBanks[i].freePtr));
	objectBanks[i].freePtr = NULL;
}

void FreeAllObjectBanks()
{
	unsigned long i;
	
	for(i=0; i<MAX_OBJECT_BANKS; i++)
		FreeObjectBank(i);
}

#ifdef __cplusplus
}
#endif
