#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <crtdbg.h>
#include <stdio.h>

#include "mgeReport.h"
#include "mdxInfo.h"
#include "mdxDDraw.h"

LPDIRECT3D2				pDirect3D;
LPDIRECT3DDEVICE2		pDirect3DDevice;
LPDIRECT3DVIEWPORT2		pDirect3DViewport;

// Some default render states
unsigned long D3DRenderstates[] = 
{
	//
	D3DRENDERSTATE_TEXTUREHANDLE,		NULL,
	D3DRENDERSTATE_TEXTUREPERSPECTIVE,	FALSE,
	D3DRENDERSTATE_TEXTUREMAPBLEND,		D3DTBLEND_MODULATEALPHA,
	D3DRENDERSTATE_SRCBLEND,			D3DBLEND_SRCALPHA,
	D3DRENDERSTATE_DESTBLEND,			D3DBLEND_INVSRCALPHA,
	D3DRENDERSTATE_SPECULARENABLE,		FALSE,
	D3DRENDERSTATE_COLORKEYENABLE,		TRUE,
	D3DRENDERSTATE_SHADEMODE,			D3DSHADE_GOURAUD,
	D3DRENDERSTATE_CULLMODE,			D3DCULL_CW,
	D3DRENDERSTATE_ZENABLE,				TRUE,
	D3DRENDERSTATE_ZWRITEENABLE,		TRUE,
	D3DRENDERSTATE_ZFUNC,				D3DCMP_LESSEQUAL,
	//
	D3DRENDERSTATE_FORCE_DWORD,			NULL
};

/*	--------------------------------------------------------------------------------
	Function	: D3DSetupRenderstates
	Purpose		: Setup Renderstates from a list
	Parameters	: a pointer to a pair of unsigned long values
	Returns		:
	Info		: 
*/

void D3DSetupRenderstates(unsigned long *me)
{
	while (*me != D3DRENDERSTATE_FORCE_DWORD)
	{
		pDirect3DDevice->SetRenderState((D3DRENDERSTATETYPE)*me,*(me+1));
		me+=2;
	}
}

/*	--------------------------------------------------------------------------------
	Function	: D3DInit
	Purpose		: Setup D3D
	Parameters	: 
	Returns		: 
	Info		: 
*/

unsigned long D3DInit(void)
{
	D3DVIEWPORT				viewport;
	D3DFINDDEVICERESULT		result;
	D3DFINDDEVICESEARCH		search;

	if (pDirectDraw->QueryInterface(IID_IDirect3D2, (LPVOID *)&pDirect3D) != S_OK)
		return FALSE;

	// Find a device we can use

	DDINIT(search);
	DDINIT(result);
	search.dwFlags = /*D3DFDS_HARDWARE |*/ D3DFDS_COLORMODEL;
//	search.bHardware = TRUE;
	search.dcmColorModel = D3DCOLOR_RGB;
	
	if (pDirect3D->FindDevice(&search, &result) != D3D_OK) 
		return FALSE;

	// Create the D3D device
	if (pDirect3D->CreateDevice(result.guid, surface[RENDER_SRF], &pDirect3DDevice) != D3D_OK)
	 return FALSE;

	// Create a viewport onto the device
	DDINIT (viewport);
	viewport.dwWidth = rXRes;
	viewport.dwHeight = rYRes;
	viewport.dvScaleX = (rXRes / 2.0f);
	viewport.dvScaleY = (rYRes / 2.0f);
	viewport.dvMaxX = D3DVAL(1.0);
	viewport.dvMaxY = D3DVAL(1.0);
	viewport.dvMinZ = D3DVAL(0.0);
	viewport.dvMaxZ = D3DVAL(1.0);

	if (pDirect3D->CreateViewport(&pDirect3DViewport, NULL) != D3D_OK) 
		return FALSE;

	if (pDirect3DDevice->AddViewport(pDirect3DViewport) != D3D_OK) 
		return FALSE;

	if (pDirect3DViewport->SetViewport(&viewport) != D3D_OK) 
		return FALSE;

	if (pDirect3DDevice->SetCurrentViewport(pDirect3DViewport) != D3D_OK) 
		return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function	: D3DCreateTexSurface
	Purpose		: Dimentions, 
	Parameters	: 
	Returns		: 
	Info		: 
*/

LPDIRECTDRAWSURFACE D3DCreateTexSurface(long xs,long ys, long cKey, long alphaSurf, long videoRam)
{ 
	HRESULT				capsResult;
    DDCAPS				ddCaps;
	LPDIRECTDRAWSURFACE pSurface,pTSurface = NULL;
	DDSURFACEDESC		ddsd;
	HRESULT				me;

	//Create the surface
	DDINIT(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | (alphaSurf?DDSD_PIXELFORMAT:0);
	ddsd.dwWidth = xs;
	ddsd.dwHeight = ys;
	ddsd.ddpfPixelFormat.dwSize = sizeof (DDPIXELFORMAT);

	if (!alphaSurf)
	{
		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
		ddsd.ddpfPixelFormat.dwRBitMask=5<<11;
	    ddsd.ddpfPixelFormat.dwGBitMask=6<<5;
	    ddsd.ddpfPixelFormat.dwBBitMask=5;
	}
	else
    {
		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
		ddsd.ddpfPixelFormat.dwRBitMask=0x0f00;
	    ddsd.ddpfPixelFormat.dwGBitMask=0x00f0;
	    ddsd.ddpfPixelFormat.dwBBitMask=0x000f;
	    ddsd.ddpfPixelFormat.dwRGBAlphaBitMask=0xf000;
	}
	
	ddsd.ddsCaps.dwCaps = videoRam?DDSCAPS_VIDEOMEMORY:DDSCAPS_SYSTEMMEMORY | DDSCAPS_TEXTURE;

	if ((me = pDirectDraw->CreateSurface(&ddsd, &pSurface, NULL)) != DD_OK)
	{
		dp ("Failed NSurf\n");
		ddShowError(me);
		RELEASE(pSurface); 
		return NULL;
	}

	if (!alphaSurf)
	{
		DDCOLORKEY cK;
		cK.dwColorSpaceLowValue = cKey;
		cK.dwColorSpaceHighValue = cKey;
		pTSurface->SetColorKey (DDCKEY_SRCBLT,&cK);
	}
}	

unsigned long DDrawCopyToSurface(LPDIRECTDRAWSURFACE pSurface, unsigned short *data, unsigned long IAlpha, unsigned long xs, unsigned long ys)
{
	DDSURFACEDESC		ddsd;
		
	// Copy the data into the surface manually
	DDINIT(ddsd);
	while (pSurface->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR,0)!=DD_OK);

	// Could be faster
	if (IAlpha)
	{
		for (int y=0;y<ys;y++)
			for (int x=0;x<xs;x++)
				((short *)ddsd.lpSurface)[x+y*(ddsd.lPitch/2)] = (data[x+y*xs] & (0x1f << 11)) | 0x0fff;
	}
	else
	{
		for (int y=0;y<ys;y++)
			for (int x=0;x<xs;x++)
				((short *)ddsd.lpSurface)[x+y*(ddsd.lPitch/2)] = data[x+y*xs];
	}

	pSurface->Unlock(ddsd.lpSurface);
	return 1;
}
