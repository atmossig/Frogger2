/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: mavis.cpp
	Programmer	: Matthew Cloy
	Date		: 

----------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <math.h>
#include "mgeReport.h"
#include "mdxDDraw.h"
#include "mdxD3D.h"
#include "mdxInfo.h"
#include "mdxTiming.h"
#include "mdxCRC.h"
#include "mdxTexture.h"
#include "mdxMath.h"
#include "mdxObject.h"
#include "mdxActor.h"
#include "mdxLandscape.h"
#include "mdxRender.h"
#include "mdxPoly.h"
#include "mdxDText.h"
#include "mdxProfile.h"
#include "mdxWindows.h"
#include "gelf.h"
#include "majikPR.h"

extern "C"
{

// Instance of FRAME_INFO, for storing... Surprisingly.. The frame information./
// Would be ideal to choose what to alloc when render hardware/software is chosen!

FRAME_INFO frameInfo[MA_MAX_FRAMES];
FRAME_INFO *cFInfo = &frameInfo[0], *oFInfo = &frameInfo[1];

unsigned long numSoftPolys,numSoftVertex;
SOFTPOLY softPolyBuffer[MA_MAX_FACES];
SOFTPOLY *softDepthBuffer[MA_SOFTWARE_DEPTH];
D3DTLVERTEX softV[MA_MAX_VERTICES];
long softScreen[640*480];

unsigned long numHaloPoints;
long limTex = 0;

short haloZVals[MA_MAX_HALOS];
MDX_VECTOR haloPoints[MA_MAX_HALOS];
float flareScales[MA_MAX_HALOS];
float flareScales2[MA_MAX_HALOS];
MDX_TEXENTRY *cTexture = NULL;

unsigned long lightingMapRS[] = 
{
	D3DRENDERSTATE_ZFUNC,D3DCMP_EQUAL,
	D3DRENDERSTATE_ZWRITEENABLE,FALSE,	
	//
	D3DRENDERSTATE_FORCE_DWORD,			NULL
};

unsigned long normalAlphaCmpRS[] = 
{
	D3DRENDERSTATE_ALPHAREF,			0,
	D3DRENDERSTATE_ALPHAFUNC,			D3DCMP_NOTEQUAL,
	//
	D3DRENDERSTATE_FORCE_DWORD,			NULL
};

unsigned long tightAlphaCmpRS[] = 
{
	D3DRENDERSTATE_ALPHAREF,			0x64,
	D3DRENDERSTATE_ALPHAFUNC,			D3DCMP_GREATER,
	//
	D3DRENDERSTATE_FORCE_DWORD,			NULL
};

unsigned long normalZRS[] = 
{
	D3DRENDERSTATE_ZFUNC,D3DCMP_LESSEQUAL,
	D3DRENDERSTATE_ZWRITEENABLE,TRUE,	
	D3DRENDERSTATE_ZENABLE,TRUE,
	//
	D3DRENDERSTATE_FORCE_DWORD,			NULL
};

unsigned long xluZRS[] = 
{
	D3DRENDERSTATE_ZFUNC,D3DCMP_LESSEQUAL,
	D3DRENDERSTATE_ZWRITEENABLE,FALSE,	
	D3DRENDERSTATE_ZENABLE,TRUE,
	//
	D3DRENDERSTATE_FORCE_DWORD,			NULL
};

unsigned long noZRS[] = 
{
	D3DRENDERSTATE_ZWRITEENABLE,FALSE,
	D3DRENDERSTATE_ZENABLE,FALSE,
	//
	D3DRENDERSTATE_FORCE_DWORD,			NULL
};

unsigned long xluSubRS[] = 
{
	D3DRENDERSTATE_SRCBLEND,D3DBLEND_ZERO,
	D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCCOLOR,
	//
	D3DRENDERSTATE_FORCE_DWORD,			NULL
};

unsigned long xluAddRS[] = 
{
	D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA,
	D3DRENDERSTATE_DESTBLEND,D3DBLEND_ONE,	
	//
	D3DRENDERSTATE_FORCE_DWORD,			NULL
};

unsigned long xluSemiRS[] = 
{
	D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA,
	D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA,
	//
	D3DRENDERSTATE_FORCE_DWORD,			NULL
};

unsigned long cullNoneRS[] = 
{
	D3DRENDERSTATE_CULLMODE,D3DCULL_NONE,
	//
	D3DRENDERSTATE_FORCE_DWORD,			NULL
};

unsigned long cullCWRS[] = 
{
	D3DRENDERSTATE_CULLMODE,D3DCULL_CW,
	//
	D3DRENDERSTATE_FORCE_DWORD,			NULL
};

unsigned long cullCCWRS[] = 
{
	D3DRENDERSTATE_CULLMODE,D3DCULL_CCW,
	//
	D3DRENDERSTATE_FORCE_DWORD,			NULL
};



long dPoly = 0;

void CopySoftScreenToSurface(LPDIRECTDRAWSURFACE7 srf)
{
	unsigned long mPitch;
	DDSURFACEDESC2		ddsd;
	unsigned long r,g,b,d;

	if (rHardware)
		return;
		
	DDINIT(ddsd);
	
	while (srf->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR,0)!=DD_OK);
	
	
	mPitch = ddsd.lPitch/2;
	for (int y=0; y<480; y++)
		for (int x=0; x<640; x++)
		{
			d = softScreen[x+y*640];
			r = (d>>16) & 0xff;
			g = (d>>8) & 0xff;
			b = (d) & 0xff;

			r >>= 3;
			g >>= 2;
			b >>= 3;


			((short *)ddsd.lpSurface)[x+y*mPitch] = (r<<11 | g<<5 | b);
		}
	srf->Unlock(NULL);
	dPoly = 1;
}

/*  --------------------------------------------------------------------------------
    Function      : DrawBatchedPolys
	Purpose       :	-
	Parameters    : -
	Returns       : -
	Info          : -
*/

void ClearSoftwareSortBuffer(void)
{
	numSoftPolys = 0;
	numSoftVertex = 0;
	for (int i=0; i<MA_SOFTWARE_DEPTH; i++)
		softDepthBuffer[i] = 0;
}

/*  --------------------------------------------------------------------------------
    Function      : DrawBatchedPolys
	Purpose       :	-
	Parameters    : -
	Returns       : -
	Info          : -
*/

void InitFrames(void)
{
	for (int i=0; i<MA_MAX_FRAMES; i++)
	{
		SwapFrame(i);
		BlankFrame;
	}
	SwapFrame(0);
	numHaloPoints = 0;
	ClearSoftwareSortBuffer();
}

/*  --------------------------------------------------------------------------------
    Function      : DrawBatchedPolys
	Purpose       :	-
	Parameters    : -
	Returns       : -
	Info          : -
*/

void PushPolys_Software( D3DTLVERTEX *v, int vC, short *fce, long fC, MDX_TEXENTRY *tEntry)
{
	long i,zVal;
	SOFTPOLY *m;

	for (i=0; i<fC; i+=3)
	{
		m = &softPolyBuffer[numSoftPolys++];
		m->f[0] = fce[i]+numSoftVertex;
		m->f[1] = fce[i+1]+numSoftVertex;
		m->f[2] = fce[i+2]+numSoftVertex;
		m->t = tSrf;
		m->tEntry = tEntry;

		zVal = v[fce[i]].sz * MA_SOFTWARE_DEPTH;		
		zVal += v[fce[i+1]].sz * MA_SOFTWARE_DEPTH;		
		zVal >>= 1;

		if (softDepthBuffer[zVal])
			m->next = softDepthBuffer[zVal];
		else
			m->next = NULL;
		
		softDepthBuffer[zVal] = m;
	}
}

/*  --------------------------------------------------------------------------------
    Function      : DrawBatchedPolys
	Purpose       :	-
	Parameters    : -
	Returns       : -
	Info          : -
*/

void PushPolys( D3DTLVERTEX *v, int vC, short *fce, long fC, MDX_TEXENTRY *tEntry )
{
	long cnt;
	short *mfce = fce;

	// Push for software/sorting...
	
	if (!rHardware)
	{
		PushPolys_Software(v,vC,fce,fC,tEntry);
		memcpy(&softV[numSoftVertex],v,vC*sizeof(D3DTLVERTEX));
		numSoftVertex+=vC;

		return;
	}

	// discard excess polys
	if ((cFInfo->nV + vC) > MA_MAX_VERTICES || (cFInfo->nF + fC) > MA_MAX_FACES)
		return;

	for (cnt=0;cnt<fC; cnt++)
	{
		*cFInfo->cF = (unsigned short)((*mfce) + cFInfo->nV);
		*cFInfo->cT = tSrf;
		cFInfo->cF++;
		cFInfo->cT++;
		mfce++;
	}

	memcpy(cFInfo->cV,v,vC*sizeof(D3DTLVERTEX));
	
	cFInfo->cV+=vC;
	cFInfo->nV+=vC;
	cFInfo->nF+=fC;
}

//#endif

/*  --------------------------------------------------------------------------------
    Function      : DrawBatchedPolys
	Purpose       :	-
	Parameters    : -
	Returns       : -
	Info          : -
*/

void AddHalo(MDX_VECTOR *point, float flareScaleA,float flareScaleB)
{
	flareScales[numHaloPoints] = flareScaleA;
	flareScales2[numHaloPoints] = flareScaleB;
	SetVector(&haloPoints[numHaloPoints++], point);
	
}

/*  --------------------------------------------------------------------------------
    Function      : DrawBatchedPolys
	Purpose       :	-
	Parameters    : -
	Returns       : -
	Info          : -
*/


void DrawSoftwarePolys (void)
{
	SOFTPOLY *cur;
	MPR_BITMAP32 thisTex;

	for (int i=MA_SOFTWARE_DEPTH-1; i>0; i--)
	{
		cur = (SOFTPOLY *)softDepthBuffer[i];
		while (cur)
		{
			MPR_POLY_VERT v[3];
			unsigned long f1,f2,f3;
			f1 = cur->f[0];
			f2 = cur->f[1];
			f3 = cur->f[2];

			v[0].x = softV[f1].sx;
			v[0].y = softV[f1].sy;
			v[0].rgba = softV[f1].color;
			
			v[1].x = softV[f2].sx;
			v[1].y = softV[f2].sy;
			v[1].rgba = softV[f2].color;

			v[2].x = softV[f3].sx;
			v[2].y = softV[f3].sy;
			v[2].rgba = softV[f3].color;
			
			if (cur->t)
			{
				thisTex.width = cur->tEntry->xSize;
				thisTex.height = cur->tEntry->ySize;
				thisTex.image = (unsigned long *)cur->tEntry->softData;
				
				v[0].u = softV[f1].tu * thisTex.width;
				v[0].v = softV[f1].tv * thisTex.height;

				v[1].u = softV[f2].tu * thisTex.width;
				v[1].v = softV[f2].tv * thisTex.height;
	
				v[2].u = softV[f3].tu * thisTex.width;
				v[2].v = softV[f3].tv * thisTex.height;

				if (v[0].u>thisTex.width)
					 v[0].u = thisTex.width;

				if (v[0].v>thisTex.height)
					 v[0].v = thisTex.height;

				if (v[1].u>thisTex.width)
					 v[1].u = thisTex.width;

				if (v[1].v>thisTex.height)
					 v[1].v = thisTex.height;

				if (v[2].u>thisTex.width)
					 v[2].u = thisTex.width;

				if (v[2].v>thisTex.height)
					 v[2].v = thisTex.height;

				if (v[0].u<0)
					 v[0].u = 0;

				if (v[0].v<0)
					 v[0].v = 0;

				if (v[1].u<0)
					 v[1].u = 0;

				if (v[1].v<0)
					 v[1].v = 0;

				if (v[2].u<0)
					 v[2].u = 0;

				if (v[2].v<0)
					 v[2].v = 0;

				f1 = MPR_DrawPoly((unsigned short *)softScreen,v,3,POLY_TEXTURE, &thisTex);
			
			}
			 
			/*pDirect3DDevice->SetTexture(0,cur->t);
		
			while(pDirect3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,softV,numSoftVertex,cur->f,3,D3DDP_WAIT)!=D3D_OK)
			{
			}

			pDirect3DDevice->SetTexture(0,0);
			*/
			cur = cur->next;
		}
	}
}

/*  --------------------------------------------------------------------------------
    Function      : DrawBatchedPolys
	Purpose       :	-
	Parameters    : -
	Returns       : -
	Info          : -
*/

unsigned long numSeperates;

void DrawBatchedPolys (void)
{
	unsigned long i;
	unsigned long nFace;
	LPDIRECTDRAWSURFACE7 lSurface;

	if (!rHardware)
	{
		DrawSoftwarePolys();
		return;
	}

	cFInfo->cF = cFInfo->f;
	cFInfo->cT = cFInfo->t;

	nFace = 3;
	i=0;

	numSeperates = 0;

	while (i<cFInfo->nF)
	{
		lSurface = *cFInfo->cT;
		nFace = 0;

		while ((((*(cFInfo->cT)) == lSurface) || (limTex)) && (i<cFInfo->nF)) 
		{
			cFInfo->cT+=3;
			nFace+=3;
			
			i+=3;
		}
		
		pDirect3DDevice->SetTexture(0,lSurface);
		
		numSeperates++;
		
		SampleGraph(nFace);
		
		while(pDirect3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,cFInfo->v,cFInfo->nV,cFInfo->cF,nFace,D3DDP_WAIT)!=D3D_OK)
		{
			// Idle Time
		}

		pDirect3DDevice->SetTexture(0,0);
		
		cFInfo->cF+=nFace;
	}

}


void SetTexture(MDX_TEXENTRY *me)
{
	if (rHardware)
	{
		if (me)
			pDirect3DDevice->SetTexture(0,me->surf);
		else
			pDirect3DDevice->SetTexture(0,0);
	}
	
	cTexture = me;
	
}

HRESULT DrawPoly(D3DPRIMITIVETYPE d3dptPrimitiveType,DWORD  dwVertexTypeDesc, LPVOID lpvVertices, DWORD  dwVertexCount, LPWORD lpwIndices, DWORD  dwIndexCount, DWORD  dwFlags)
{
	HRESULT res;
	MPR_BITMAP32 thisTex;
	MPR_POLY_VERT v[3];
	D3DTLVERTEX *verts;
	unsigned long f1,f2,f3;
	
	if (rHardware)
		res = pDirect3DDevice->DrawIndexedPrimitive(d3dptPrimitiveType,dwVertexTypeDesc,lpvVertices,dwVertexCount,lpwIndices,dwIndexCount,dwFlags);
	else
	{
		verts = (D3DTLVERTEX *)lpvVertices;
		for (int i=0; i<dwIndexCount; i+=3)
		{
			f1 = lpwIndices[i+0];
			f2 = lpwIndices[i+1];
			f3 = lpwIndices[i+2];

			v[0].x = verts[f1].sx;
			v[0].y = verts[f1].sy;
			v[0].rgba = verts[f1].color;
			
			v[1].x = verts[f2].sx;
			v[1].y = verts[f2].sy;
			v[1].rgba = verts[f2].color;

			v[2].x = verts[f3].sx;
			v[2].y = verts[f3].sy;
			v[2].rgba = verts[f3].color;
			
			if (cTexture)
			{
				thisTex.width = cTexture->xSize;
				thisTex.height = cTexture->ySize;
				thisTex.image = (unsigned long *)cTexture->softData;
				
				v[0].u = verts[f1].tu * thisTex.width;
				v[0].v = verts[f1].tv * thisTex.height;

				v[1].u = verts[f2].tu * thisTex.width;
				v[1].v = verts[f2].tv * thisTex.height;
	
				v[2].u = verts[f3].tu * thisTex.width;
				v[2].v = verts[f3].tv * thisTex.height;

				if (v[0].u>thisTex.width)
					 v[0].u = thisTex.width;

				if (v[0].v>thisTex.height)
					 v[0].v = thisTex.height;

				if (v[1].u>thisTex.width)
					 v[1].u = thisTex.width;

				if (v[1].v>thisTex.height)
					 v[1].v = thisTex.height;

				if (v[2].u>thisTex.width)
					 v[2].u = thisTex.width;

				if (v[2].v>thisTex.height)
					 v[2].v = thisTex.height;

				if (v[0].u<0)
					 v[0].u = 0;

				if (v[0].v<0)
					 v[0].v = 0;

				if (v[1].u<0)
					 v[1].u = 0;

				if (v[1].v<0)
					 v[1].v = 0;

				if (v[2].u<0)
					 v[2].u = 0;

				if (v[2].v<0)
					 v[2].v = 0;

				f1 = MPR_DrawPoly((unsigned short *)softScreen,v,3,POLY_TEXTURE, &thisTex);
			}			
		}
	}
	
	return res;
}

/*	--------------------------------------------------------------------------------
	Function		: DrawFlatRect
	Purpose			: draw a flat rectangle
	Parameters		: 
	Returns			: 
	Info			: 
*/

void DrawFlatRect(RECT r, D3DCOLOR colour)
{
	D3DTLVERTEX v[4] = {
		{
				r.left,r.top,0,0,
			colour,D3DRGBA(0,0,0,1),
			0,0
		},
		{
			r.left,r.bottom,0,0,
			colour,D3DRGBA(0,0,0,1),
			0,0
		},
		{
			r.right,r.bottom,0,0,
			colour,D3DRGBA(0,0,0,1),
			0,0
		},
		{
			r.right,r.top,0,0,
			colour,D3DRGBA(0,0,0,1),
			0,0
	}};

	if (rHardware)
	{
		pDirect3DDevice->SetTexture(0,NULL);
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,0);
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,0);

		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);

		if (pDirect3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN,D3DFVF_TLVERTEX,v,4,D3DDP_WAIT)!=D3D_OK)
			dp("Could not draw flat rectangle\n");
	}
	else
	{
			MPR_POLY_VERT sv[4];
			long alpha = (v[0].color & 0xff000000)>>(24+4);

			
			sv[0].x = v[0].sx;
			sv[0].y = v[0].sy;
			sv[0].rgba = v[0].color & 0x00ffffff;
			
			sv[1].x = v[1].sx;
			sv[1].y = v[1].sy;
			sv[1].rgba = v[1].color & 0x00ffffff;

			sv[2].x = v[2].sx;
			sv[2].y = v[2].sy;
			sv[2].rgba = v[2].color & 0x00ffffff;
			
			sv[3].x = v[3].sx;
			sv[3].y = v[3].sy;
			sv[3].rgba = v[3].color & 0x00ffffff;
			
			alpha = 32;

			sv[0].rgba |= alpha << 24;
			sv[1].rgba |= alpha << 24;
			sv[2].rgba |= alpha << 24;
			sv[3].rgba |= alpha << 24;

			MPR_DrawPoly((unsigned short *)softScreen,sv,4,POLY_GOURAUD | POLY_NOCLIP,NULL);
	}
	
//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
}

/*	--------------------------------------------------------------------------------
	Function		: DrawFlatRect
	Purpose			: draw a flat rectangle
	Parameters		: 
	Returns			: 
	Info			: 
*/

void DrawTexturedRect(RECT r, D3DCOLOR colour, LPDIRECTDRAWSURFACE7 tex, float u0, float v0, float u1, float v1)
{

	if ((r.left>clx1) || (r.top>cly1) || (r.right<clx0) || (r.bottom<cly0))
		return;
		
	if (r.left<clx0)
	{
		u0 += ((clx0 - r.left)/((float)r.right-r.left)) * (u1-u0);
		r.left = clx0;
		
	}

	if (r.top<cly0)
	{
		v0 += ((cly0 - r.top)/((float)r.bottom-r.top)) * (v1-v0);
		r.top = cly0;
		
	}

	if (r.right>clx1)
	{
		u1 += ((clx1 - r.right)/((float)r.right-r.left)) * (u1-u0);
		r.right = clx1;		
	}

	if (r.bottom>cly1)
	{
		v1 += ((cly1 - r.bottom)/((float)r.bottom-r.top))*(v1-v0);
		r.bottom = cly1;		
	}

	D3DLVERTEX v[4] = {
		{
			r.left,r.top,0,0,
			colour,D3DRGBA(0,0,0,0),
			u0,v0
		},
		{
			r.left,r.bottom,0,0,
			colour,D3DRGBA(0,0,0,0),
			u0,v1
			},
		{
			r.right,r.bottom,0,0,
			colour,D3DRGBA(0,0,0,0),
			u1,v1
		},
		{
			r.right,r.top,0,0,
			colour,D3DRGBA(0,0,0,0),
			u1,v0
	}};

	if (rHardware)
	{
		pDirect3DDevice->SetTexture(0,tex);
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,0);
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,0);

		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
	
		pDirect3DDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTFN_POINT);  
		pDirect3DDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTFN_POINT);

		while ((pDirect3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN,D3DFVF_TLVERTEX,v,4,D3DDP_WAIT)!=D3D_OK));
			
		pDirect3DDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTFN_LINEAR);  
		pDirect3DDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTFN_LINEAR);
	
	//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
		pDirect3DDevice->SetTexture(0,NULL);
	}
}

/*	--------------------------------------------------------------------------------
	Function		: DrawFlatRect
	Purpose			: draw a flat rectangle
	Parameters		: 
	Returns			: 
	Info			: 
*/

void DrawTexturedRectRotated(float x, float y, float width, float height, D3DCOLOR colour, LPDIRECTDRAWSURFACE7 tex, float u0, float v0, float u1, float v1, float ang)
{
	float widthover2 = width/2, heightover2 = height /2;
	float x1,x2,x3,x4,y1,y2,y3,y4;
	D3DTLVERTEX v[4] = {
		{
			0,0,0,0,
			colour,D3DRGBA(0,0,0,0),
			u0,v0
		},
		{
			0,0,0,0,
			colour,D3DRGBA(0,0,0,0),
			u0,v1
			},
		{
			0,0,0,0,
			colour,D3DRGBA(0,0,0,0),
			u1,v1
		},
		{
			0,0,0,0,
			colour,D3DRGBA(0,0,0,0),
			u1,v0
	}};

	// Can be optimised lot's... I know... I know...
    x1 = -widthover2;
	y1 = -heightover2;

	x2 = +widthover2;
	y2 = -heightover2;

	x3 = +widthover2;
	y3 = +heightover2;

	x4 = -widthover2;
	y4 = +heightover2;

	v[0].sx =x+ cosf(ang) * x1 - sinf(ang) * y1;
	v[0].sy =y+ sinf(ang) * x1 + cosf(ang) * y1;
	
	v[1].sx =x+ cosf(ang) * x2 - sinf(ang) * y2;
	v[1].sy =y+ sinf(ang) * x2 + cosf(ang) * y2;
	
	v[2].sx =x+ cosf(ang) * x3 - sinf(ang) * y3;
	v[2].sy =y+ sinf(ang) * x3 + cosf(ang) * y3;
	
	v[3].sx =x+ cosf(ang) * x4 - sinf(ang) * y4;
	v[3].sy =y+ sinf(ang) * x4 + cosf(ang) * y4;
	
	pDirect3DDevice->SetTexture(0,tex);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,0);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,0);

	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
	
	pDirect3DDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTFN_POINT);  
	pDirect3DDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTFN_POINT);

	while ((pDirect3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN,D3DFVF_TLVERTEX,v,4,D3DDP_WAIT)!=D3D_OK));
			
	pDirect3DDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTFN_LINEAR);  
	pDirect3DDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTFN_LINEAR);
	
//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
	pDirect3DDevice->SetTexture(0,NULL);
}


/*	--------------------------------------------------------------------------------
	Function		: DrawFlatRec
	Purpose			: draw a flat rectangle
	Parameters		: 
	Returns			: 
	Info			: 
*/

void BlankAllFrames(void)
{	
	for (int i=MA_FRAME_NORMAL; i<MA_MAX_FRAMES; i++)
	{
		SwapFrame(i);
		BlankFrame;
	}	
	numHaloPoints = 0;
	
	ClearSoftwareSortBuffer();
}

/*	--------------------------------------------------------------------------------
	Function		: DrawFlatRect
	Purpose			: draw a flat rectangle
	Parameters		: 
	Returns			: 
	Info			: 
*/

void StoreHaloPoints(void)
{
	DDSURFACEDESC2		ddsd;
	MDX_VECTOR v;

	DDINIT(ddsd);
	while (surface[ZBUFFER_SRF]->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR | DDLOCK_NOSYSLOCK,0)!=DD_OK);
	
	ddsd.lPitch /= sizeof(short);

	for (int i=0; i<numHaloPoints; i++)
	{
		XfmPoint(&v,&haloPoints[i],NULL);
		haloPoints[i].vx = (unsigned long)v.vx;
		haloPoints[i].vy = (unsigned long)v.vy;
		haloPoints[i].vz = v.vz;
		
		if (haloPoints[i].vz)
			haloZVals[i] = ((short *)ddsd.lpSurface)[(unsigned long)(haloPoints[i].vx+(haloPoints[i].vy*ddsd.lPitch))];
	}

	surface[ZBUFFER_SRF]->Unlock(NULL);
}

/*	--------------------------------------------------------------------------------
	Function		: DrawFlatRect
	Purpose			: draw a flat rectangle
	Parameters		: 
	Returns			: 
	Info			: 
*/

void CheckHaloPoints(void)
{
	DDSURFACEDESC2		ddsd;
	MDX_VECTOR v;
	unsigned long i;

	DDINIT(ddsd);
	while (surface[ZBUFFER_SRF]->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR| DDLOCK_NOSYSLOCK,0)!=DD_OK);
	
	ddsd.lPitch /= sizeof(short);

	for (i=0; i<numHaloPoints; i++)
		if (haloPoints[i].vz)
			if (haloZVals[i] == ((short *)ddsd.lpSurface)[(unsigned long)(haloPoints[i].vx+(haloPoints[i].vy*ddsd.lPitch))])
				haloPoints[i].vz = 0;
	
	surface[ZBUFFER_SRF]->Unlock(NULL);
}

/*	--------------------------------------------------------------------------------
	Function		: DrawFlatRect
	Purpose			: draw a flat rectangle
	Parameters		: 
	Returns			: 
	Info			: 
*/

LPDIRECTDRAWSURFACE7 haloS;
LPDIRECTDRAWSURFACE7 flareS;
LPDIRECTDRAWSURFACE7 flareS2;

void DrawHalos(void)
{
	RECT rec;
	D3DSetupRenderstates(xluAddRS);
	float r = 1, g = 80/256.0, b = 10/256.0;
	float r1 = 1, g1 = 130/256.0, b1 = 110/256.0;
	for (int i=0; i<numHaloPoints; i++)
	{
		if (haloPoints[i].vz)
		{
			rec.left = haloPoints[i].vx - 60;
			rec.right = haloPoints[i].vx + 60;

			rec.top = haloPoints[i].vy - 60;
			rec.bottom = haloPoints[i].vy + 60;

			DrawTexturedRect(rec,D3DRGBA(r1,g1,b1,1),haloS,FULL_TEXTURE);

			DrawTexturedRectRotated(haloPoints[i].vx,haloPoints[i].vy,fabs(200 * flareScales[i]),60 * (fabs(flareScales[i])),D3DRGBA(r,g,b,0.8-fabs(flareScales[i])*0.5),flareS,FULL_TEXTURE,1.57);
			DrawTexturedRectRotated(haloPoints[i].vx,haloPoints[i].vy,fabs(150 * flareScales2[i]),20,D3DRGBA(r,g,b,1-fabs(flareScales2[i])*0.5),flareS,FULL_TEXTURE,1);
			DrawTexturedRectRotated(haloPoints[i].vx,haloPoints[i].vy,30+fabs(80 * flareScales2[i]),30+fabs(80 * flareScales2[i]),D3DRGBA(r,g,b,1-fabs(flareScales2[i])),flareS2,FULL_TEXTURE,3*flareScales2[i]);
			DrawTexturedRectRotated(haloPoints[i].vx,haloPoints[i].vy,30+fabs(80 * flareScales[i]),30+fabs(80 * flareScales[i]),D3DRGBA(r,g,b,1-fabs(flareScales[i])),flareS2,FULL_TEXTURE,0);			
		}
	}

}

/*	--------------------------------------------------------------------------------
	Function		: DrawFlatRect
	Purpose			: draw a flat rectangle
	Parameters		: 
	Returns			: 
	Info			: 
*/

unsigned long drawLighting = 1;
unsigned long drawPhong = 1;

void DrawAllFrames(void)
{
/*	pDirect3DDevice->BeginScene();
	DrawSoftwarePolys();
	pDirect3DDevice->EndScene();
*/
	BeginDraw();
	
	// Draw Normal Polys
//D3DSetupRenderstates(xluAddRS);
	SwapFrame(MA_FRAME_NORMAL);
	DrawBatchedPolys();
	
	if (!rHardware)
	{
		EndDraw();
		return;
	}

	
/*	
	// Need to test the ZBuffer on these polys...
	StoreHaloPoints();

	pDirect3DDevice->BeginScene();

	SwapFrame(MA_FRAME_GLOW);
	DrawBatchedPolys();

	pDirect3DDevice->EndScene();

	CheckHaloPoints();
	
	pDirect3DDevice->BeginScene();
*/
	D3DSetupRenderstates(tightAlphaCmpRS);

	if (drawLighting || drawPhong)
	{
		// Draw Lightingmap polys
		D3DSetupRenderstates(lightingMapRS);
	
		if (drawLighting)
		{
			// First normal lighting
			D3DSetupRenderstates(xluSubRS);
			SwapFrame(MA_FRAME_LIGHTMAP);
			DrawBatchedPolys();
		}

		if (drawPhong)
		{
			// Then Phong
			D3DSetupRenderstates(xluAddRS);
			SwapFrame(MA_FRAME_PHONG);
			DrawBatchedPolys();
		}

		// Switch Back to normal
		
	}
	D3DSetupRenderstates(normalAlphaCmpRS);	
	D3DSetupRenderstates(xluZRS);
	D3DSetupRenderstates(xluSemiRS);

	SwapFrame(MA_FRAME_XLU);
	DrawBatchedPolys();	

	D3DSetupRenderstates(xluAddRS);	
	SwapFrame(MA_FRAME_ADDITIVE);
	DrawBatchedPolys();	
	
	D3DSetupRenderstates(normalZRS);
	D3DSetupRenderstates(normalAlphaCmpRS);	

	DrawHalos();
	D3DSetupRenderstates(xluSemiRS);

	pDirect3DDevice->EndScene();
}


}