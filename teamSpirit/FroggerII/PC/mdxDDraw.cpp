
LPDIRECTDRAW			pDirectDraw;		// Our Direct Draw Object
LPDIRECTDRAW4			pDirectDraw4;		// A modern view of our direct draw object
LPDIRECTDRAWCLIPPER		pClipper;
unsigned long			rXRes, rYRes, rBitDepth, r565;

enum 
{
	PRIMARY_SRF,
	RENDER_SRF,
	ZBUFFER_SRF,
	NUM_SRF
};

LPDIRECTDRAWSURFACE	surface[NUM_SRF] = {NULL,NULL,NULL};

unsigned long DDrawInitObject (GUID guid)
{
	HRESULT res;

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

	// We now have a valid object!
	return 1;
}


unsigned long DDrawCreateSurfaces(unsigned long xRes, unsigned long yRes, unsigned long bitDepth, unsigned long want3D = 0, unsigned long zBits = 0)
{
	unsigned long hardware = stupme
	DDSURFACEDESC			sdesc;
	
	rBitDepth = bitDepth;

	// Create a primary surface
	DDINIT(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | (hardware?DDSCAPS_VIDEOMEMORY | DDSCAPS_SYSTEMMEMORY);
	if (pDirectDraw->CreateSurface(&ddsd, &surface[PRIMARY_SRF], NULL) != DD_OK)
	{
		dp("Failed creating primary surface\n");
		ddShowError(res);
		return 0;
	}

	// What format would the screen be currently in?
	DDINIT(sdesc);
	surface->GetSurfaceDesc(&sdesc);
	l = (int)sdesc.ddpfPixelFormat.dwGBitMask;
	while ((l&1) == 0)
		  l >>= 1;
	r565 = !(l == 31);

	// Create a render surface
	DDINIT(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.dwWidth = (rXRes = xRes);
	ddsd.dwHeight = (rYRes = yRes);
	ddsd.ddsCaps.dwCaps = (hardware?DDSCAPS_BACKBUFFER | (want3D?DDSCAPS_3DDEVICE:0) : DDSCAPS_OFFSCREENPLAIN | (want3D?DDSCAPS_3DDEVICE:0) | DDSCAPS_SYSTEMMEMORY)
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
		ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | (hardware?DDSCAPS_VIDEOMEMORY:DDSCAPS_SYSTEMMEMORY);
		if ((res = pDirectDraw->CreateSurface(&ddsd, &surface[ZBUFFER_SRF], NULL)) != DD_OK)
		{
			dp("Error creating Z-buffer surface\n");
			ddShowError(res);
			return 0;
		}
	}

}

unsigned long DDrawAttachSurface(unsigned long srfA, unsigned long srfB)
{
	if ((res = surface[srfA]->AddAttachedSurface(surface[srfB])) != DD_OK)
	{
		dp("Error attaching surface %lu to %lu\n");
		ddShowError(res);
		return FALSE;
	}
}

unsigned long DDrawSetupWindow(HWND window)
{
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
		if ((res = pDirectDraw->SetDisplayMode(rXRes, rYRes, bitDepth)) != DD_OK)
		{
			dp("Failed setting display mode. (Fullscreen Mode)\n")
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

// NOTE: move this out, it's just here as an example

void SetupDDRAW(void)
{
	
	DDrawInitObject (guid);
	
	DDrawCreateSurfaces (640, 480, 16, TRUE, zBuffer?16:0); 
	DDrawSetupWindow (window);
	
	DDrawAttachSurface (PRIMARY_SRF,RENDER_SRF);
	if (zBuffer)
		DDrawAttachSurface (RENDER_SRF,ZBUFFER_SRF);


	
}






