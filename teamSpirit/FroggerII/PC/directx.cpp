/*	A demo of i-MAGE (Written for E3'99)
	Written by Matthew Cloy (c)1999 Interactive Studios Ltd.
	
	directx.cpp		: All directx related gumph.

	Created			: Sat 01 May 1999 - Matthew Cloy - Chesham
	Modified		: Sat 01 May 1999 - Matthew Cloy - Chesham: r0.0.01 - Project started
					: Sun 02 May 1999 - Matthew Cloy - Chesham: r0.0.01 - Direct3D Working
					: Mon 03 May 1999 - Matthew Cloy - Train:	r0.0.01 - Adding texture support

********************************************************************************************/

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include "stdlib.h"
#include "directx.h"

#define SCREEN_WIDTH	640	//320
#define SCREEN_HEIGHT	480	//240
#define SCREEN_BITS		16


extern "C"
{

HWND win;

LPDIRECTDRAW			pDirectDraw;
LPDIRECTDRAWSURFACE		primarySrf;
LPDIRECTDRAWSURFACE		hiddenSrf;
LPDIRECTDRAWSURFACE		srfZBuffer;
LPDIRECT3D2				pDirect3D;
LPDIRECT3DDEVICE2		pDirect3DDevice;
LPDIRECT3DVIEWPORT2		pDirect3DViewport;

long a565Card = 0;

void dp(char *format, ...);

long DirectXInit(HWND window, long hardware)
{
	D3DVIEWPORT				viewport;
	D3DFINDDEVICERESULT		result;
	D3DFINDDEVICESEARCH		search;
	DDSURFACEDESC			ddsd;
	DDSURFACEDESC sdesc;
	int l;
	win = window;
	// Create main DirectDraw object
	if (DirectDrawCreate(NULL, &pDirectDraw, NULL) != DD_OK)
	 return FALSE;
	
	if (pDirectDraw->SetCooperativeLevel(window, DDSCL_NORMAL)!=DD_OK)
//	if (pDirectDraw->SetCooperativeLevel(window, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWMODEX) != DD_OK)
	 return FALSE;

//	if (pDirectDraw->SetDisplayMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BITS))
//	 return FALSE;

	// Get the primary display surface
	DDINIT(ddsd);
	ddsd.dwFlags = DDSD_CAPS;

	if (hardware)
	 ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	else
	 ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_SYSTEMMEMORY;
	
	if (pDirectDraw->CreateSurface(&ddsd, &primarySrf, NULL) != DD_OK)
	 return FALSE;

	// Create a back buffer and attach it to the primary display surface to make a flippable surface
	DDINIT(ddsd);
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.dwWidth = SCREEN_WIDTH;
	ddsd.dwHeight = SCREEN_HEIGHT;

	if (hardware)
	 ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE | DDSCAPS_VIDEOMEMORY;
	else
	 ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE | DDSCAPS_SYSTEMMEMORY;
	
	if (pDirectDraw->CreateSurface(&ddsd, &hiddenSrf, NULL) != DD_OK)
	 return FALSE;
	
	//if (primarySrf->AddAttachedSurface(hiddenSrf) != DD_OK)
	// return FALSE;

	if (hardware)
	{
		// Create a z-buffer and attach it to the backbuffer
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_ZBUFFERBITDEPTH;
		ddsd.dwWidth = SCREEN_WIDTH;
		ddsd.dwHeight = SCREEN_HEIGHT;
		ddsd.dwZBufferBitDepth = 16;
		if (hardware)
			ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
		else
			ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_SYSTEMMEMORY;
		if (pDirectDraw->CreateSurface(&ddsd, &srfZBuffer, NULL) != DD_OK) 
			return FALSE;
		if (hiddenSrf->AddAttachedSurface(srfZBuffer) != DD_OK) 
			return FALSE;
	}

	if (pDirectDraw->QueryInterface(IID_IDirect3D2, (LPVOID *)&pDirect3D) != S_OK)
		return FALSE;

	// Find a device we can use

	DDINIT(search);
	DDINIT(result);

	search.dwFlags = D3DFDS_HARDWARE | D3DFDS_COLORMODEL;
	search.bHardware = hardware;
	search.dcmColorModel = D3DCOLOR_RGB;
	//search.dcmColorModel = D3DCOLOR_MONO;  

	
	if (pDirect3D->FindDevice(&search, &result) != D3D_OK) 
		return FALSE;

	// Create the D3D device

	if (pDirect3D->CreateDevice(result.guid, hiddenSrf, &pDirect3DDevice) != D3D_OK)
	 return FALSE;

	// Create a viewport
	DDINIT (viewport);

	viewport.dwWidth = SCREEN_WIDTH;
	viewport.dwHeight = SCREEN_HEIGHT;
	viewport.dvScaleX = (SCREEN_WIDTH / 2.0f);
	viewport.dvScaleY = (SCREEN_HEIGHT / 2.0f);
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
	
	memset (&sdesc,0,sizeof (sdesc));
	sdesc.dwSize=sizeof (sdesc);

	primarySrf->GetSurfaceDesc(&sdesc);
	l = (int)sdesc.ddpfPixelFormat.dwGBitMask;
	while ((l&1) == 0)
		  l >>= 1;
	
	a565Card = !(l == 31);
	
	if (hardware)
		SetupRenderstates(); 
	
	return TRUE;
}

void DirectXFlip(void)
{
	D3DRECT rect;
	RECT r,a;
	// Flip the back buffer to the primary surface
	//primarySrf->Flip(NULL,DDFLIP_WAIT);
	GetClientRect (win,&r);
	GetWindowRect (win,&a);
	a.left+=GetSystemMetrics(SM_CXSIZEFRAME);
	a.top+=GetSystemMetrics(SM_CYSIZEFRAME);

	a.top+=GetSystemMetrics(SM_CYSIZE)+1;

	r.left+=a.left;
	r.top+=a.top;
	r.right+=a.left;
	r.bottom+=a.top;

	primarySrf->Blt(&r,hiddenSrf,NULL,DDBLT_WAIT,NULL);
	// Clear the z-buffer
	rect.x1 = 0;
	rect.y1 = 0;
	rect.x2 = SCREEN_WIDTH;
	rect.y2 = SCREEN_HEIGHT;
	pDirect3DViewport->Clear(1, &rect, D3DCLEAR_ZBUFFER);
}

void SetupRenderstates(void)
{
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,NULL);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE,FALSE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND,D3DTBLEND_COPY);

	//pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_LINEAR);
	//pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMIN,D3DFILTER_LINEAR);

	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_SUBPIXEL,TRUE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE,D3DSHADE_FLAT);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_CW);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE,FALSE);
	//pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,FALSE);
	//pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,FALSE);	
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,TRUE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,TRUE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC,D3DCMP_LESSEQUAL);
}

// Split this out into two functions (CreateTextureSurface and CopyToSurface)
LPDIRECTDRAWSURFACE CreateTextureSurface(long xs,long ys, short *data, BOOL hardware)
{ 
	LPDIRECTDRAWSURFACE pSurface = NULL;
	DDSURFACEDESC ddsd;

	//Create the surface
	DDINIT(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;// | DDSD_PIXELFORMAT;
	ddsd.dwWidth = xs;
	ddsd.dwHeight = ys;
	ddsd.ddpfPixelFormat.dwSize = sizeof (DDPIXELFORMAT);
	ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
    ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
	ddsd.ddpfPixelFormat.dwRBitMask=5<<11;
    ddsd.ddpfPixelFormat.dwGBitMask=6<<5;
    ddsd.ddpfPixelFormat.dwBBitMask=5;
    if (hardware)
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_VIDEOMEMORY;
	else
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY;
	
	if (pDirectDraw->CreateSurface(&ddsd, &pSurface, NULL) != DD_OK)
	{
		RELEASE(pSurface); 
		dp("Couldn't create surface\n");
		return NULL;
	}

	// Copy the data into the surface

	DDINIT(ddsd);
	while (pSurface->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR,0)!=DD_OK);
	
	for (int y=0;y<ys;y++)
		for (int x=0;x<xs;x++)
		{	
			((short *)ddsd.lpSurface)[x+y*(ddsd.lPitch/2)] = data[x+y*xs];
		}
		//memcpy (&((short *)ddsd.lpSurface)[y*(ddsd.lPitch/2)],&data[y+xs],xs*2);
		
	pSurface->Unlock(ddsd.lpSurface);

	return pSurface;
}

D3DTEXTUREHANDLE lastH = NULL;
void BeginDrawHardware (void)
{
	pDirect3DDevice->BeginScene();
	lastH = NULL;
}

void EndDrawHardware (void)
{
	pDirect3DDevice->EndScene();
}

void DrawAHardwarePoly (D3DTLVERTEX *v,long vC, short *fce, long fC, D3DTEXTUREHANDLE h)
{
	if (h!=lastH)
	{
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,h);
		lastH = h;
	}
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,1);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,1);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC,D3DCMP_LESS);
	//dp("Prim\n");mnlaskdjfhgdqpoieoiurewyytiuw
	if (pDirect3DDevice->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST,
		D3DVT_TLVERTEX,
		v,
		vC,
		(unsigned short *)fce,
		fC,
		D3DDP_DONOTCLIP 
			| D3DDP_DONOTLIGHT 
			| D3DDP_DONOTUPDATEEXTENTS 
			| D3DDP_WAIT)!=D3D_OK)
			dp("Bugger\n");	
}

D3DTEXTUREHANDLE ConvertSurfaceToTexture(LPDIRECTDRAWSURFACE srf)
{
	D3DTEXTUREHANDLE textureHandle;
	LPDIRECT3DTEXTURE2 texture;

	if (srf->QueryInterface(IID_IDirect3DTexture2, (LPVOID *)&texture) != S_OK)
		return NULL;

	texture->GetHandle(pDirect3DDevice, &textureHandle);
	return textureHandle;
}

}
