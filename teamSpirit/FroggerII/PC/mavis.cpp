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

// Instance of FRAME_INFO, for storing... Surprisingly.. The frame information./
FRAME_INFO frameInfo[MAX_FRAMES];
FRAME_INFO *cFInfo = &frameInfo[0];

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