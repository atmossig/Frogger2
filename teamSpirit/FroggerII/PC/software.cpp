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

#include <fstream.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <memory.h>
#include <winbase.h>

#define SCREEN_BITS		16

extern "C"
{

#include <ultra64.h>
#include "incs.h"
#include "always3d.h"

long SoftwareInit(long sX, long sY, long IS555)
{
	long pKey[4] = { 10098, 7752, 1098, 4344 };

	if (A3D_Init(pKey)==A3DERR_INVALIDKEY)
		return 0;

	A3D_SetParameters (sX,sY, 0,0,sX-1,sY-1, IS555?A3D_16bit555:A3D_16bit565);
	
	return 1;
}


}