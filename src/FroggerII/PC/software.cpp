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

#include <windowsx.h>
#include <mmsystem.h>
#include <memory.h>
#include <winbase.h>

#define SCREEN_BITS		16

extern "C"
{

#include <ultra64.h>
#include "incs.h"
//#include "always3d.h"


short screen[640*480];
/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

long SoftwareInit(long sX, long sY, long IS555)
{
/*	long pKey[4] = { 10098, 7752, 1098, 4344 };

	if (A3D_Init(pKey)==A3DERR_INVALIDKEY)
		return 0;

	A3D_SetParameters (sX,sY, 0,0,sX-1,sY-1, IS555?A3D_16bit555:A3D_16bit565);
	*/
	return 1;
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

void SoftwareSetup(void)
{
		/*A3D_SetRaster (A3D_TEXTURE);
		A3D_SetCycle (A3D_1CYCLE);
		A3D_SetFlags (A3D_32X32);*/
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

void SoftwareBeginFrame(void)
{/*
	SoftwareSetup();
	memset (screen,0,640*480*2);
	A3D_BeginFrame (screen,SCREEN_WIDTH);
	A3D_Sync();*/
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

void SoftwareEndFrame(void)
{
	//A3D_EndFrame ();
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

void DrawSoftwarePolygon(float x1, float y1, float u1, float v1,
						 float x2, float y2, float u2, float v2,
						 float x3, float y3, float u3, float v3,
						 short *tex)
{
/*	A3D_Vertex v[3],*vT;
	short f[3] = {0,1,2};

	vT = v;
	vT->x =  x1; 
	vT->y =  y1; 
	vT->u =  u1; 
	vT->v =  v1; 
	vT++;
	vT->x =  x2; 
	vT->y =  y2; 
	vT->u =  u2; 
	vT->v =  v2; 	
	vT++;
	vT->x =  x3; 
	vT->y =  y3; 
	vT->u =  u3; 
	vT->v =  v3; 

	if (tex)
	{
		A3D_SetTexture(tex,NULL);
		A3D_DrawPolygon(v,3,f,3);
	}*/
}

}