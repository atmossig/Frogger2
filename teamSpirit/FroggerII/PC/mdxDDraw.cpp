#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <crtdbg.h>
#include <stdio.h>

#include "mgeReport.h"
#include "mdxInfo.h"
#include "mdxDDraw.h"

LPDIRECTDRAW			pDirectDraw;		// Our Direct Draw Object
LPDIRECTDRAW4			pDirectDraw4;		// A modern view of our direct draw object
LPDIRECTDRAWCLIPPER		pClipper;
unsigned long			rXRes, rYRes, rBitDepth, r565 ,rHardware;

LPDIRECTDRAWSURFACE	surface[NUM_SRF] = {NULL,NULL,NULL};

/*	--------------------------------------------------------------------------------
	Function	: DDrawInitObject
	Purpose		: Initialise Directdraw objects
	Parameters	: Guid of device
	Returns		: success
	Info		: 
*/

unsigned long DDrawInitObject (GUID *guid)
{
	HRESULT		res;
	DDCAPS		ddCaps;
	
	// Create a base DirectDraw object
	if ((res = DirectDrawCreate(guid, &pDirectDraw, NULL)) != DD_OK)
	{
		dp("Failed creating DirectDraw object.\n");
		ddShowError(res);
		return 0;
	}

	// Get and interface to a DDraw4 object, NOTE: might change when we go to DX7
	if ((res = pDirectDraw->QueryInterface(IID_IDirectDraw4, (LPVOID *)&pDirectDraw4)) != S_OK)
	{
		dp("Failed getting DirectDraw4 interface.\n");
		ddShowError(res);
		return 0;
	}

	DDINIT(ddCaps);													
 	if ((res = pDirectDraw4->GetCaps(&ddCaps, NULL))!=DD_OK)			
	{
		dp("Failed getting DirectDraw4 caps.\n");
		ddShowError(res);
		return 0;
	}
	rHardware = (ddCaps.dwCaps & DDCAPS_3D);
	
	// We now have a valid object!
	return 1;
}

/*	--------------------------------------------------------------------------------
	Function	: DDrawCreateSurfaces
	Purpose		: Create all our surfaces
	Parameters	: xRes,yRes & Bitdepth, whether we will want a 3D capable render surface, and whether we would want a zbuffer
	Returns		: success
	Info		: 
*/

unsigned long DDrawCreateSurfaces(unsigned long xRes, unsigned long yRes, unsigned long bitDepth, unsigned long want3D, unsigned long zBits)
{
	DDSURFACEDESC	ddsd;
	HRESULT			res;
	unsigned long	l;
	
	rBitDepth =	bitDepth;

	// Create a primary surface
	DDINIT(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | (rHardware?DDSCAPS_VIDEOMEMORY:DDSCAPS_SYSTEMMEMORY);
	if (res = pDirectDraw->CreateSurface(&ddsd, &surface[PRIMARY_SRF], NULL) != DD_OK)
	{
		dp("Failed creating primary surface\n");
		ddShowError(res);
		return 0;
	}
	
	// Get Some info for the surface
	DDINIT(ddsd);
	if ((res = surface[PRIMARY_SRF]->GetSurfaceDesc(&ddsd)) != DD_OK)
	{
		dp("Failed creating primary surface description\n");
		ddShowError(res);
		return 0;
	}

	// Test the green mask to see how many bits it is.
	l = (int)ddsd.ddpfPixelFormat.dwGBitMask;
	while ( ((l&1) == 0) & (l) )
		  l >>= 1;
	r565 = (l != 31);
	
	// Create a render surface
	DDINIT(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.dwWidth = (rXRes = xRes);
	ddsd.dwHeight = (rYRes = yRes);
	ddsd.ddsCaps.dwCaps = (rHardware?DDSCAPS_BACKBUFFER | (want3D?DDSCAPS_3DDEVICE:0) : DDSCAPS_OFFSCREENPLAIN | (want3D?DDSCAPS_3DDEVICE:0) | DDSCAPS_SYSTEMMEMORY);
	if ((res = pDirectDraw->CreateSurface(&ddsd, &surface[RENDER_SRF], NULL)) != DD_OK)
	{
		dp("Failed creating render surface\n");
		ddShowError(res);
		return 0;
	}

	// Create a zbuffer if asked
	if (zBits)
	{
		DDINIT(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_ZBUFFERBITDEPTH;
		ddsd.dwWidth = rXRes;
		ddsd.dwHeight = rYRes;
		ddsd.dwZBufferBitDepth = zBits;
		ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | (rHardware?DDSCAPS_VIDEOMEMORY:DDSCAPS_SYSTEMMEMORY);
		if ((res = pDirectDraw->CreateSurface(&ddsd, &surface[ZBUFFER_SRF], NULL)) != DD_OK)
		{
			dp("Error creating Z-buffer surface\n");
			ddShowError(res);
			return 0;
		}
	}

}

/*	--------------------------------------------------------------------------------
	Function	: DDrawAttachSurface
	Purpose		: Attach two surfaces
	Parameters	: A is surface that B attaches to.
	Returns		: success
	Info		: 
*/

unsigned long DDrawAttachSurface(unsigned long srfA, unsigned long srfB)
{
	HRESULT res;
	if ((res = surface[srfA]->AddAttachedSurface(surface[srfB])) != DD_OK)
	{
		dp("Error attaching surface %lu to %lu\n");
		ddShowError(res);
		return FALSE;
	}
}

/*	--------------------------------------------------------------------------------
	Function	: DDrawSetupWindow
	Purpose		: Setup for widowing
	Parameters	: The window that will contain the view of the primary surface
	Returns		: success
	Info		: 
*/

unsigned long DDrawSetupWindow(HWND window)
{
	HRESULT res;
	if (!surface[0])
	{
		dp("Cannot setup window before you create at least one surface");
		return 0;
	}

	// To run fullscreen - exclusive, ensure bitdepth is nonzero
	if (rBitDepth)
	{
		// Fullscreen Exclusive
		if ((res = pDirectDraw->SetCooperativeLevel(window, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE)) != DD_OK)
		{
			dp("Failed setting cooperative level. (Fullscreen Mode)\n");
			ddShowError(res);
			return 0;
		}

		// Set display
		if ((res = pDirectDraw->SetDisplayMode(rXRes, rYRes, rBitDepth)) != DD_OK)
		{
			dp("Failed setting display mode. (Fullscreen Mode)\n");
			ddShowError(res);
			return 0;
		}
	}
	
	// Make a clipper
	if ((res = pDirectDraw->CreateClipper (0,&pClipper,NULL)) !=DD_OK)
	{
		dp("Failed creating clipper.\n");
		ddShowError(res);
		return 0;
	}

	// Set one end to the window
	if (pClipper->SetHWnd (0,window)!=DD_OK) 
	{
		dp("Failed setting cliiper to hwnd.\n");
		ddShowError(res);
		return 0;
	}

	// And the other to the surface!
	if (surface[PRIMARY_SRF]->SetClipper (pClipper)!=DD_OK)
	{
		dp("Failed setting clipper to surface.\n");
		ddShowError(res);
		return 0;
	}

	return 1;
}







