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
#include <fstream.h>

extern "C"
{

#include <ultra64.h>
#include "incs.h"
#include "software.h"
#include "mavis.h"

// Instance of FRAME_INFO, for storing... Surprisingly.. The frame information./
FRAME_INFO frameInfo;

/*  --------------------------------------------------------------------------------
    Function      : DrawBatchedPolys
	Purpose       :	-
	Parameters    : -
	Returns       : -
	Info          : -
*/

void DrawBatchedPolys (void)
{
	unsigned long i;
	unsigned long lHandle,nFace;

	frameInfo.cF = frameInfo.f;
	frameInfo.cH = frameInfo.h;

	

	nFace = 3;
	i=0;

	while (i<frameInfo.nF)
	{
		lHandle = *frameInfo.cH;
		nFace = 0;

		while ((*(frameInfo.cH)) == lHandle)
		{
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
}

}