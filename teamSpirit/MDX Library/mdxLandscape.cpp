/*

	This file is part of the M libraries,

	File		: 
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
#include "mdxLandscape.h"
#include "gelf.h"

#ifdef __cplusplus
extern "C"
{
#endif

MDX_LANDSCAPE *ConvertObjectToLandscape(MDX_OBJECT *obj)
{
	MDX_LANDSCAPE *me = new MDX_LANDSCAPE;
	unsigned long i;
	
	// Copy the vertices and the faces
	if (me->numVertices = obj->mesh->numVertices)
	{
		me->vertices = new MDX_VECTOR[me->numVertices];
		for (int i=0; i<me->numVertices; i++)
		{
			guMtxXFMF(obj->objMatrix.matrix,obj->mesh->vertices[i].vx,obj->mesh->vertices[i].vy,obj->mesh->vertices[i].vz,
									&me->vertices[i].vx,&me->vertices[i].vy,&me->vertices[i].vz);
		}
	}

	memcpy (&me->objMatrix,&obj->objMatrix,sizeof(MDX_MATRIX));
	
	if (me->numFaces = obj->mesh->numFaces)
	{
		short *tFace;
		me->faceIndex = new short[me->numFaces * 3];
		me->xfmVert = new D3DTLVERTEX[me->numFaces * 3];
		me->textures = new LPDIRECTDRAWSURFACE7[me->numFaces];

		memcpy (me->faceIndex,obj->mesh->faceIndex,sizeof(short)*me->numFaces*3);

		tFace = me->faceIndex;

		for (i=0; i<obj->mesh->numFaces*3; i++)
		{
			me->xfmVert[i].tu = obj->mesh->faceTC[i].v[0] * 0.000975F;
			me->xfmVert[i].tv = obj->mesh->faceTC[i].v[1] * 0.000975F;
			me->xfmVert[i].rhw = 0;
			me->xfmVert[i].specular = D3DRGBA(0,0,0,0);
			me->xfmVert[i].color = *((long *)(&(obj->mesh->gouraudColors[i].x)));
			if (obj->mesh->textureIDs[i/3])
				me->textures[i/3] = obj->mesh->textureIDs[i/3]->surf;
			else
				me->textures[i/3] = NULL;
			tFace++;
		}
	}
	
	// Copy the untransformed bounding box for culling purposes
	if (obj->bBox)
		memcpy (&me->bBox,obj->bBox,sizeof(MDX_VECTOR)*8);
	
    me->next = me->children = NULL;		
	if (obj->next)
		me->next = ConvertObjectToLandscape(obj->next);

	if (obj->children)
		me->children = ConvertObjectToLandscape(obj->children);

	return me;
}

MDX_LANDSCAPE *ConvertActorToLandscape(MDX_ACTOR *actor)
{
	// Calculate the object matrices for each sub object (We'll pre-transform them)
	XformActor(actor);
	return ConvertObjectToLandscape(actor->objectController->object);
}

#ifdef __cplusplus
}
#endif
