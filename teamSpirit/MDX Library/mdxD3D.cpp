
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <crtdbg.h>
#include <stdio.h>

#include "mgeReport.h"
#include "mdxDDraw.h"
#include "mdxD3D.h"
#include "mdxMath.h"
#include "mdxTexture.h"
#include "mdxObject.h"
#include "mdxActor.h"
#include "mdxCRC.h"
#include "mdxTiming.h"
#include "mgeReport.h"
#include "gelf.h"
#include "mdxLandscape.h"
#include "mdxRender.h"
#include "mdxPoly.h"
#include "mdxProfile.h"
#include "mdxiNFO.h"
#include "math.h"

#ifdef __cplusplus
extern "C"
{
#endif

LPDIRECT3D7				pDirect3D;
LPDIRECT3DDEVICE7		pDirect3DDevice;
LPDIRECT3DVIEWPORT2		pDirect3DViewport;

// Some default render states
	
unsigned long D3DDefaultRenderstates[] = 
{
	//
	D3DRENDERSTATE_CLIPPING,			FALSE,	
	D3DRENDERSTATE_LIGHTING,			FALSE,
	D3DRENDERSTATE_EXTENTS,				FALSE,
	D3DRENDERSTATE_TEXTUREPERSPECTIVE,	FALSE,
	D3DRENDERSTATE_ALPHABLENDENABLE,	TRUE,
	D3DRENDERSTATE_SRCBLEND,			D3DBLEND_SRCALPHA,
	D3DRENDERSTATE_DESTBLEND,			D3DBLEND_INVSRCALPHA,
	D3DRENDERSTATE_SPECULARENABLE,		FALSE,
	D3DRENDERSTATE_COLORKEYENABLE,		TRUE,
	D3DRENDERSTATE_SHADEMODE,			D3DSHADE_GOURAUD,
	D3DRENDERSTATE_CULLMODE,			D3DCULL_CW,
	D3DRENDERSTATE_ZENABLE,				TRUE,
	D3DRENDERSTATE_ZWRITEENABLE,		TRUE,
	D3DRENDERSTATE_ZFUNC,				D3DCMP_LESSEQUAL,
	D3DRENDERSTATE_ALPHABLENDENABLE,	TRUE,
	
	D3DRENDERSTATE_ALPHATESTENABLE,		FALSE,
	D3DRENDERSTATE_ALPHAREF,			0,
	D3DRENDERSTATE_ALPHAFUNC,			D3DCMP_NOTEQUAL,
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
	if (!rHardware)
	{
		SetSoftwareState(me);
		return;
	}

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
	HRESULT			res;
    D3DVIEWPORT7	vp = {0, 0, 640, 480, 0.0f, 1.0f};

	if (!rHardware)
		return TRUE;

	if ((res = pDirectDraw7->QueryInterface(IID_IDirect3D7, (LPVOID *)&pDirect3D)) != S_OK)
	{
		dp("Failed creating Direct3D Interface.\n");
		ddShowError(res);		
		return FALSE;
	}

	if ((res = pDirect3D->CreateDevice(IID_IDirect3DHALDevice, surface[RENDER_SRF], &pDirect3DDevice)) != D3D_OK)
	{
		ddShowError(res);			
		return FALSE;
	}
	// Create the viewport

    if ((res = pDirect3DDevice->SetViewport(&vp)) != D3D_OK)
        return FALSE;

	pDirect3DDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTFN_POINT);
	pDirect3DDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTFN_POINT);
	pDirect3DDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	pDirect3DDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	pDirect3DDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);

	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function	: D3DCreateTexSurface
	Purpose		: Dimentions, 
	Parameters	: 
	Returns		: 
	Info		: 
*/

void D3DClearView(void)
{
	if (rHardware)
		pDirect3DDevice->Clear(0,0,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER , 0,1, 0L );
	else
		memset(softScreen,0,640*480*sizeof(long));
}

/*	--------------------------------------------------------------------------------
	Function	: D3DCreateTexSurface
	Purpose		: Dimentions, 
	Parameters	: 
	Returns		: 
	Info		: 
*/

unsigned long D3DShutdown(void)
{
	if (rHardware)
	{
		pDirect3DDevice->Release();
		pDirect3D->Release();
	}
	return 1;
}

/*	--------------------------------------------------------------------------------
	Function	: D3DCreateTexSurface
	Purpose		: Dimentions, 
	Parameters	: 
	Returns		: 
	Info		: 
*/

LPDIRECTDRAWSURFACE7 D3DCreateTexSurface(long xs,long ys, long cKey, long alphaSurf, long videoRam)
{ 
	LPDIRECTDRAWSURFACE7 pSurface,pTSurface = NULL;
	DDSURFACEDESC2		ddsd;
	HRESULT				me;

	//Create the surface
	DDINIT(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;//(alphaSurf?DDSD_PIXELFORMAT:0);
	ddsd.dwWidth = xs;
	ddsd.dwHeight = ys;
	ddsd.ddpfPixelFormat.dwSize = sizeof (DDPIXELFORMAT);

	if (!alphaSurf)
	{
		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
		
		ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0x8000;
		ddsd.ddpfPixelFormat.dwRBitMask=0x1f<<10;
	    ddsd.ddpfPixelFormat.dwGBitMask=0x1f<<5;
	    ddsd.ddpfPixelFormat.dwBBitMask=0x1f;
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
	
	ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;//(videoRam?DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM:DDSCAPS_SYSTEMMEMORY) | DDSCAPS_TEXTURE;
	ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;

	if ((me = pDirectDraw7->CreateSurface(&ddsd, &pSurface, NULL)) != DD_OK)
	{
		dp (videoRam?"Failed doing something in video RAM\n":"Failed in system memory\n");
		ddShowError(me);
		RELEASE(pSurface); 
		return NULL;
	}

	if (!alphaSurf)
	{
		DDCOLORKEY cK;
		cK.dwColorSpaceLowValue = cKey;
		cK.dwColorSpaceHighValue = cKey;
		pSurface->SetColorKey (DDCKEY_SRCBLT,&cK);
	}
	return pSurface;
}	

LPDIRECTDRAWSURFACE7 D3DCreateTexSurface2(long xs,long ys,long videoRam, long texSrf)
{ 
	LPDIRECTDRAWSURFACE7 pSurface,pTSurface = NULL;
	DDSURFACEDESC2		ddsd;
	HRESULT				me;

	//Create the surface
	DDINIT(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.dwWidth = xs;
	ddsd.dwHeight = ys;

	if (videoRam)
	{	
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
		ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE | DDSCAPS2_HINTDYNAMIC;
	}
	else
	{
		ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_TEXTURE;		
	}


	if ((me = pDirectDraw7->CreateSurface(&ddsd, &pSurface, NULL)) != DD_OK)
	{
		dp (videoRam?"Failed doing something in video RAM\n":"Failed in system memory\n");
		ddShowError(me);
		RELEASE(pSurface); 
		return NULL;
	}

	
	return pSurface;
}	

/*	--------------------------------------------------------------------------------
	Function	: D3DCreateTexSurface
	Purpose		: Dimentions, 
	Parameters	: 
	Returns		: 
	Info		: 
*/

unsigned long DDrawExpandToSurface(LPDIRECTDRAWSURFACE7 pSurface, unsigned short *data, unsigned long IAlpha, unsigned long xs, unsigned long ys, unsigned long nxs, unsigned long nys)
{
	float dX,dY,cX,cY;
	unsigned int x,y,cXI,cYI;
	DDSURFACEDESC2		ddsd;
	DDINIT(ddsd);

	dX = (float)xs/(float)nxs;
	dY = (float)ys/(float)nys;
	cY = 0;
	
	while (pSurface->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR,0)!=DD_OK);

	for (y = 0; y<nxs; y++)
	{
		cX = 0;
		cYI = (unsigned int)cY;

		for (x = 0; x<nxs; x++)
		{
			cXI = (unsigned long)cX;
			((short *)ddsd.lpSurface)[x+y*(ddsd.lPitch/2)] = data[cXI+cYI*xs];
			cX+=dX;
		}
		cY+=dY;
	}
/*
	for (y = 1; y<nxs-1; y++)
		for (x = 1; x<nxs-1; x++)
		{
			unsigned short v1, v2, v3 ,v4 ,c;
			
			v1 = ((short *)ddsd.lpSurface)[x+1+y*(ddsd.lPitch/2)];
			v1 >>= 1;
			v1 &= 0x7BEF;
				
			v2 = ((short *)ddsd.lpSurface)[x-1+y*(ddsd.lPitch/2)];
			v2 >>= 1;
			v2 &= 0x7BEF;

			v3 = ((short *)ddsd.lpSurface)[x+(y-1)*(ddsd.lPitch/2)];
			v3 >>= 1;
			v3 &= 0x7BEF;
				
			v4 = ((short *)ddsd.lpSurface)[x+(y+1)*(ddsd.lPitch/2)];
			v4 >>= 1;
			v4 &= 0x7BEF;

			c = ((short *)ddsd.lpSurface)[x+y*(ddsd.lPitch/2)];
			c >>= 1;
			c &= 0x7BEF;
			
			v1 = v1+v2;
			v1 >>= 1;
			v1 &= 0x7BEF;
			
			v2 = v3+v4;
			v2 >>= 1;
			v2 &= 0x7BEF;
			
			v1 = v1+v2;
			v1 >>= 1;
			v1 &= 0x7BEF;
			
			((short *)ddsd.lpSurface)[x+y*(ddsd.lPitch/2)] = v1+c;
			
		}
*/	
	pSurface->Unlock(NULL);

	return 1;	
}


void BeginDraw(void)
{
	if (rHardware)
		pDirect3DDevice->BeginScene();

}

void EndDraw(void)
{
	if (rHardware)
		pDirect3DDevice->EndScene();
}

/*	--------------------------------------------------------------------------------
	Function	: DDrawCopyToSurface
	Purpose		: Dimentions
	Parameters	: 
	Returns		: 
	Info		: 
*/

unsigned long DDrawCopyToSurface(LPDIRECTDRAWSURFACE7 pSurface, unsigned short *data, unsigned long IAlpha, unsigned long xs, unsigned long ys)
{
	DDSURFACEDESC2		ddsd;
	short val;
	unsigned long texHasMagenta = 0;
	
	// Copy the data into the surface manually
	DDINIT(ddsd);
	while (pSurface->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR,0)!=DD_OK);

	// Could be faster
	if (IAlpha)
	{
		for (unsigned int y=0;y<ys;y++)
			for (unsigned int x=0;x<xs;x++)
				((short *)ddsd.lpSurface)[x+y*(ddsd.lPitch/2)] = (data[x+y*xs] & (0x1f << 11)) | 0x0fff;
	}
	else
	{
		for (unsigned int y=0;y<ys;y++)
			for (unsigned int x=0;x<xs;x++)
			{
				val  = data[x+y*xs];
				if (val!=(0x1f | 0x1f<<10))
					val |= 0x8000;
				else
				{
					texHasMagenta = 1;
				/*	// Stop magenta "halos"
					short c;
					long val2;
					c = 0;
					for (int i=-1; i<=1; i++)
						for (int j=-1; j<=1; j++)
						{
							val2 = (x+i)+(y+j)*xs;
							if (val2<0)
								val2 = 0;
							if (val2>xs*ys)
								val2 = 0;
							
							c = data[val2];
							if (c!=(0x1f | 0x1f<<10))
								val = c;
							c = 0;
						}					*/
				}

				((short *)ddsd.lpSurface)[x+y*(ddsd.lPitch/2)] = val;
			}
	}

	pSurface->Unlock(NULL);
	return texHasMagenta;
}


#ifdef __cplusplus
}
#endif
