#define SHOW_OUTLINES

/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: mavis.cpp
	Programmer	: Matthew Cloy
	Date		: 

----------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <dsound.h>
#include <dplay.h>
#include <dplobby.h>
#include "stdlib.h"
#include "stdio.h"
#include "directx.h"
#include "..\resource.h"
#include <crtdbg.h>
#include "commctrl.h"
#include "network.h"
#include "math.h"

#include <windowsx.h>
#include <mmsystem.h>
#include <memory.h>
#include <winbase.h>

float TriangleArea(float x1,float y1,float x2,float y2,float x3,float y3);

extern "C"
{

#include <ultra64.h>
#include "incs.h"
#include "software.h"
#include "mavis.h"
#include "crc32.h"
// Instance of FRAME_INFO, for storing... Surprisingly.. The frame information./
FRAME_INFO frameInfo[MAX_FRAMES];
FRAME_INFO *cFInfo = &frameInfo[0];

#define MA_MAX_HALOS 50

short haloZVals[MA_MAX_HALOS];
VECTOR haloPoints[MA_MAX_HALOS];
float flareScales[MA_MAX_HALOS];
float flareScales2[MA_MAX_HALOS];
unsigned long haloColor[MA_MAX_HALOS];

unsigned long numHaloPoints;
void XfmPoint (VECTOR *vTemp2, VECTOR *in);

unsigned long haloHandle = 0;
unsigned long haloHandle2 = 0;
unsigned long haloHandle3 = 0;

void DrawHalos(void)
{
	RECT rec;
	
	if (!haloHandle)
	{
		TEXENTRY *m;
		FindTexture((TEXTURE **)&m,UpdateCRC("glowtex.bmp"),1);
		haloHandle = m->hdl;
		FindTexture((TEXTURE **)&m,UpdateCRC("halotex2.bmp"),1);
		haloHandle2 = m->hdl;
		FindTexture((TEXTURE **)&m,UpdateCRC("halotex3.bmp"),1);
		haloHandle3 = m->hdl;
	}

	for (int i=0; i<numHaloPoints; i++)
	{
		if (haloPoints[i].v[2]>10)
		{
			unsigned long c,size,size2;
			DrawAlphaSprite(haloPoints[i].v[0] - 60,haloPoints[i].v[1] - 60, 0,120,120, 0,0,1,1,haloHandle,haloColor[i]);
			
			c = D3DRGB(0.7,0.7,0.7);//haloColor[i];
			c &= 0x00ffffff;
			
			/*
			DrawAlphaSpriteRotating(&(haloPoints[i].v[0]),3*flareScales2[i],haloPoints[i].v[0] - 30,haloPoints[i].v[1] - 30, 0,60,60, 0,0,1,1,haloHandle3,c | D3DRGBA(0,0,0,1-fabs(flareScales2[i])));
			DrawAlphaSpriteRotating(&(haloPoints[i].v[0]),1,haloPoints[i].v[0] - 30,haloPoints[i].v[1] - 30, 0,60,60, 0,0,1,1,haloHandle3,c | D3DRGBA(0,0,0,1-fabs(flareScales[i])));
			
			c = haloColor[i];
			c &= 0x00ffffff;
			
			size = fabs(400 * flareScales[i]);
			size2 = 40+fabs(20 * flareScales[i]);
			DrawAlphaSpriteRotating(&(haloPoints[i].v[0]),0,haloPoints[i].v[0] - size2/2,haloPoints[i].v[1] - size/2, 0,size2,size, 0,0,1,1,haloHandle2,c | D3DRGBA(0,0,0,1-fabs(flareScales[i])));
			size = fabs(200 * flareScales2[i]);
			DrawAlphaSpriteRotating(&(haloPoints[i].v[0]),0.5,haloPoints[i].v[0] - 20,haloPoints[i].v[1] - size/2, 0,40,size, 0,0,1,1,haloHandle2,c | D3DRGBA(0,0,0,1-fabs(flareScales2[i])));
			*/
		//DrawTexturedRectRotated(haloPoints[i].vx,haloPoints[i].vy,fabs(200 * flareScales[i]),60 * (fabs(flareScales[i])),D3DRGBA(r,g,b,0.8-fabs(flareScales[i])*0.5),flareS,FULL_TEXTURE,1.57);
		//DrawTexturedRectRotated(haloPoints[i].vx,haloPoints[i].vy,fabs(150 * flareScales2[i]),20,D3DRGBA(r,g,b,1-fabs(flareScales2[i])*0.5),flareS,FULL_TEXTURE,1);
		//DrawTexturedRectRotated(haloPoints[i].vx,haloPoints[i].vy,30+fabs(80 * flareScales2[i]),30+fabs(80 * flareScales2[i]),D3DRGBA(r,g,b,1-fabs(flareScales2[i])),flareS2,FULL_TEXTURE,3*flareScales2[i]);
		//DrawTexturedRectRotated(haloPoints[i].vx,haloPoints[i].vy,30+fabs(80 * flareScales[i]),30+fabs(80 * flareScales[i]),D3DRGBA(r,g,b,1-fabs(flareScales[i])),flareS2,FULL_TEXTURE,0);			
	
		}
	}

	numHaloPoints = 0;
}

/*	--------------------------------------------------------------------------------
	Function		: DrawFlatRect
	Purpose			: draw a flat rectangle
	Parameters		: 
	Returns			: 
	Info			: 
*/

void StoreHaloPoints(void)
{
	DDSURFACEDESC		ddsd;
	VECTOR v,t;

	DDINIT(ddsd);
	while (surface[ZBUFFER_SRF]->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR | DDLOCK_NOSYSLOCK,0)!=DD_OK);
	
	ddsd.lPitch /= sizeof(short);

	for (int i=0; i<numHaloPoints; i++)
	{
		XfmPoint(&v,&haloPoints[i]);
	
		haloPoints[i].v[2] = -1;
		
		if ((v.v[0]>0) && (v.v[0]<640))
			if ((v.v[1]>0) && (v.v[1]<480))
			{
				haloPoints[i].v[0] = (unsigned long)v.v[0];
				haloPoints[i].v[1] = (unsigned long)v.v[1];
				haloPoints[i].v[2] = v.v[2];
			}
					
		if (haloPoints[i].v[2]>10)
			haloZVals[i] = ((short *)ddsd.lpSurface)[(unsigned long)(haloPoints[i].v[0]+(haloPoints[i].v[1]*ddsd.lPitch))];
	}

	surface[ZBUFFER_SRF]->Unlock(ddsd.lpSurface);
}

/*	--------------------------------------------------------------------------------
	Function		: DrawFlatRect
	Purpose			: draw a flat rectangle
	Parameters		: 
	Returns			: 
	Info			: 
*/

void CheckHaloPoints(void)
{
	DDSURFACEDESC		ddsd;
	VECTOR v;
	unsigned long i;

	DDINIT(ddsd);
	while (surface[ZBUFFER_SRF]->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR | DDLOCK_NOSYSLOCK,0)!=DD_OK);
	
	ddsd.lPitch /= sizeof(short);

	for (i=0; i<numHaloPoints; i++)
		if (haloPoints[i].v[2]>10)
			if (haloZVals[i] == ((short *)ddsd.lpSurface)[(unsigned long)(haloPoints[i].v[0]+(haloPoints[i].v[1]*ddsd.lPitch))])
				haloPoints[i].v[Z] = 0;
	
	surface[ZBUFFER_SRF]->Unlock(ddsd.lpSurface);
}


void AddHalo(VECTOR *point, float flareScaleA,float flareScaleB, unsigned long color)
{
	flareScales[numHaloPoints] = flareScaleA;
	flareScales2[numHaloPoints] = flareScaleB;
	haloColor[numHaloPoints] = color;
	SetVector(&haloPoints[numHaloPoints++], point);	
}

#ifdef _DEBUG

void PushPolys( D3DTLVERTEX *v, int vC, short *fce, long fC, long h )
{
	unsigned long cnt;
	short *mfce = fce;

	// discard excess polys
	if ((cFInfo->nV + vC) > MA_MAX_VERTICES || (cFInfo->nF + fC) > MA_MAX_FACES)
		return;

	for (cnt=0;cnt<fC; cnt++)
	{
		*cFInfo->cF = (*mfce) + cFInfo->nV;
		*cFInfo->cH = h;
		cFInfo->cF++;
		cFInfo->cH++;
		mfce++;
	}

	memcpy(cFInfo->cV,v,vC*sizeof(D3DTLVERTEX));
	
	cFInfo->cV+=vC;
	cFInfo->nV+=vC;
	cFInfo->nF+=fC;
}

#endif

/*  --------------------------------------------------------------------------------
    Function      : DrawBatchedPolys
	Purpose       :	-
	Parameters    : -
	Returns       : -
	Info          : -
*/
extern long totalFacesDrawn;
float lx1,ly1,lx2,ly2;
extern long drawTimers;
extern long numPixelsDrawn; 

void DrawBatchedPolys (void)
{
	unsigned long i;
	unsigned long lHandle,nFace;

	cFInfo->cF = cFInfo->f;
	cFInfo->cH = cFInfo->h;
	StartTimer(4,"Mavis");
	

	nFace = 3;
	i=0;
	totalFacesDrawn+=cFInfo->nF/3;


	while (i<cFInfo->nF)
	{
		lHandle = *cFInfo->cH;
		nFace = 0;

		while (((*(cFInfo->cH)) == lHandle) && (i<cFInfo->nF))
		{
			numPixelsDrawn+=TriangleArea(cFInfo->v[cFInfo->f[i]].sx,cFInfo->v[cFInfo->f[i]].sy,
						 cFInfo->v[cFInfo->f[i+1]].sx,cFInfo->v[cFInfo->f[i+1]].sy,
						 cFInfo->v[cFInfo->f[i+2]].sx,cFInfo->v[cFInfo->f[i+2]].sy);
#ifdef SHOW_OUTLINES
			if (drawTimers==3)
			{
				pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,0);

				lx1 = cFInfo->v[cFInfo->f[i]].sx;
				ly1 = cFInfo->v[cFInfo->f[i]].sy;
				
				lx2 = cFInfo->v[cFInfo->f[i+1]].sx;
				ly2 = cFInfo->v[cFInfo->f[i+1]].sy;
				
				DrawALine (lx1,ly1,lx2,ly2, D3DRGB(1,1,1));

				lx1 = cFInfo->v[cFInfo->f[i+1]].sx;
				ly1 = cFInfo->v[cFInfo->f[i+1]].sy;
				
				lx2 = cFInfo->v[cFInfo->f[i+2]].sx;
				ly2 = cFInfo->v[cFInfo->f[i+2]].sy;
				
				DrawALine (lx1,ly1,lx2,ly2, D3DRGB(1,1,1));

				lx1 = cFInfo->v[cFInfo->f[i]].sx;
				ly1 = cFInfo->v[cFInfo->f[i]].sy;
				
				lx2 = cFInfo->v[cFInfo->f[i+2]].sx;
				ly2 = cFInfo->v[cFInfo->f[i+2]].sy;
				
				DrawALine (lx1,ly1,lx2,ly2, D3DRGB(1,1,1));
			}
	#endif

			cFInfo->cH+=3;
			nFace+=3;
			i+=3;
		}
		
#ifdef TEXTURE_DEBUG
		if( CheckTexHDL(lHandle) )
#endif
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,lHandle);

		if (pDirect3DDevice->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			D3DVT_TLVERTEX,
			cFInfo->v,
			cFInfo->nV,
			cFInfo->cF,
			nFace,
				D3DDP_DONOTCLIP |
				D3DDP_DONOTLIGHT |
				D3DDP_DONOTUPDATEEXTENTS) !=D3D_OK) dp("!");
		
		
		cFInfo->cF+=nFace;
	}

	EndTimer(4);
}

}