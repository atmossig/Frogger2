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
	unsigned long lastHandle,numFaceToSend;

	frameInfo.cF = frameInfo.f;
	frameInfo.cH = frameInfo.h;

	lastHandle = *(frameInfo.cH);
	numFaceToSend = 0;

	while (frameInfo.nF)
	{
		if (*(frameInfo.cH)!=lastHandle)
		{
			pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,lastHandle);
			
			if (pDirect3DDevice->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST,
				D3DVT_TLVERTEX,
				frameInfo.v,
				frameInfo.nV,
				frameInfo.cF,
				numFaceToSend,
					D3DDP_DONOTCLIP |
					D3DDP_DONOTLIGHT |
					D3DDP_DONOTUPDATEEXTENTS) !=D3D_OK)
			{
				dp("BUGGER !!!!! CAN'T DRAW POLY JOBBY\n");
			}

			numFaceToSend = 0;
			lastHandle = *frameInfo.cH;
		}
		else
		{
			frameInfo.cH++;
			frameInfo.cF++;
			frameInfo.nF--;
			numFaceToSend++;
		}
	}
}

}