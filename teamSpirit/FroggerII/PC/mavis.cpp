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

extern "C"
{

#include <ultra64.h>
#include "incs.h"
#include "software.h"
#include "mavis.h"

// Instance of FRAME_INFO, for storing... Surprisingly.. The frame information./
FRAME_INFO frameInfo;

#ifdef _DEBUG

void PushPolys( D3DTLVERTEX *v, int vC, short *fce, long fC, long h )
{
	unsigned long cnt;
	short *mfce = fce;

	// discard excess polys
	if ((frameInfo.nV + vC) > MA_MAX_VERTICES || (frameInfo.nF + fC) > MA_MAX_FACES)
		return;

	for (cnt=0;cnt<fC; cnt++)
	{
		*frameInfo.cF = (*mfce) + frameInfo.nV;
		*frameInfo.cH = h;
		frameInfo.cF++;
		frameInfo.cH++;
		mfce++;
	}

	memcpy(frameInfo.cV,v,vC*sizeof(D3DTLVERTEX));
	
	frameInfo.cV+=vC;
	frameInfo.nV+=vC;
	frameInfo.nF+=fC;
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

void DrawBatchedPolys (void)
{
	unsigned long i;
	unsigned long lHandle,nFace;

	frameInfo.cF = frameInfo.f;
	frameInfo.cH = frameInfo.h;
	StartTimer(4,"Mavis");
	

	nFace = 3;
	i=0;
	totalFacesDrawn+=frameInfo.nF/3;

	while (i<frameInfo.nF)
	{
		lHandle = *frameInfo.cH;
		nFace = 0;

		while (((*(frameInfo.cH)) == lHandle) && (i<frameInfo.nF))
		{

#ifdef SHOW_OUTLINES
			if (drawTimers==3)
			{
				pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,0);

				lx1 = frameInfo.v[frameInfo.f[i]].sx;
				ly1 = frameInfo.v[frameInfo.f[i]].sy;
				
				lx2 = frameInfo.v[frameInfo.f[i+1]].sx;
				ly2 = frameInfo.v[frameInfo.f[i+1]].sy;
				
				DrawALine (lx1,ly1,lx2,ly2, D3DRGB(1,1,1));

				lx1 = frameInfo.v[frameInfo.f[i+1]].sx;
				ly1 = frameInfo.v[frameInfo.f[i+1]].sy;
				
				lx2 = frameInfo.v[frameInfo.f[i+2]].sx;
				ly2 = frameInfo.v[frameInfo.f[i+2]].sy;
				
				DrawALine (lx1,ly1,lx2,ly2, D3DRGB(1,1,1));

				lx1 = frameInfo.v[frameInfo.f[i]].sx;
				ly1 = frameInfo.v[frameInfo.f[i]].sy;
				
				lx2 = frameInfo.v[frameInfo.f[i+2]].sx;
				ly2 = frameInfo.v[frameInfo.f[i+2]].sy;
				
				DrawALine (lx1,ly1,lx2,ly2, D3DRGB(1,1,1));
			}
	#endif

			frameInfo.cH+=3;
			nFace+=3;
			i+=3;
		}
		
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,lHandle);

		if (pDirect3DDevice->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			D3DVT_TLVERTEX,
			frameInfo.v,
			frameInfo.nV,
			frameInfo.cF,
			nFace,
				D3DDP_DONOTCLIP |
				D3DDP_DONOTLIGHT |
				D3DDP_DONOTUPDATEEXTENTS) !=D3D_OK) dp("!");
		
		
		frameInfo.cF+=nFace;
	}

	EndTimer(4);
}

}