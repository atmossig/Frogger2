/*

	This file is part of the M libraries,

	File		: mliReport.h
	Programmer	: Matthew Cloy
	Date		: 

----------------------------------------------------------------------------------------------- */

#ifndef MDXD3D_H_INCLUDED
#define MDXD3D_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

extern	LPDIRECT3D7				pDirect3D;
extern	LPDIRECT3DDEVICE7		pDirect3DDevice;
extern	LPDIRECT3DVIEWPORT2		pDirect3DViewport;
extern unsigned long			D3DDefaultRenderstates[];

/*	--------------------------------------------------------------------------------
	Function	: D3DSetupRenderstates
	Purpose		: Setup Renderstates from a list
	Parameters	: a pointer to a pair of unsigned long values
	Returns		:
	Info		: 
*/

void D3DSetupRenderstates(unsigned long *me);

/*	--------------------------------------------------------------------------------
	Function	: D3DInit
	Purpose		: Setup D3D
	Parameters	: x, y pixel resolution
	Returns		: 
	Info		: 
*/

unsigned long D3DInit(int xRes, int yRes);

/*	--------------------------------------------------------------------------------
	Function	: D3DCreateTexSurface
	Purpose		: Dimentions, 
	Parameters	: 
	Returns		: 
	Info		: 
*/

unsigned long D3DShutdown(void);

/*	--------------------------------------------------------------------------------
	Function	: D3DCreateTexSurface
	Purpose		: Dimentions, 
	Parameters	: 
	Returns		: 
	Info		: 
*/

LPDIRECTDRAWSURFACE7 D3DCreateTexSurface(long xs,long ys, long cKey, long alphaSurf, long videoRam);
LPDIRECTDRAWSURFACE7 D3DCreateTexSurface2(long xs,long ys,long videoRam, long texSrf);

/*	--------------------------------------------------------------------------------
	Function	: DDrawCopyToSurface
	Purpose		: Dimentions
	Parameters	: 
	Returns		: 
	Info		: 
*/

unsigned long DDrawCopyToSurface(LPDIRECTDRAWSURFACE7 pSurface, unsigned short *data, unsigned long IAlpha, unsigned long xs, unsigned long ys, long convert);

/*	--------------------------------------------------------------------------------
	Function	: DDrawCopyToSurface
	Purpose		: Dimentions
	Parameters	: 
	Returns		: 
	Info		: 
*/

unsigned long DDrawExpandToSurface(LPDIRECTDRAWSURFACE7 pSurface, unsigned short *data, unsigned long IAlpha, unsigned long xs, unsigned long ys, unsigned long nxs, unsigned long nys);

/*	--------------------------------------------------------------------------------
	Function	: D3DCreateTexSurface
	Purpose		: Dimentions, 
	Parameters	: 
	Returns		: 
	Info		: 
*/

LPDIRECTDRAWSURFACE7 D3DCreateSurface(long xs,long ys, long cKey,long videoRam);
LPDIRECTDRAWSURFACE7 D3DCreateTexSurfaceScreen(long xs,long ys, long cKey, long alphaSurf, long videoRam);
unsigned long DDrawCopyToSurface2(LPDIRECTDRAWSURFACE7 pSurface, unsigned short *data, unsigned long xs, unsigned long ys);

void D3DClearView(void);
void BeginDraw(void);
void EndDraw(void);
void SetupFogParams(float fStart,float fR,float fG,float fB,long enable);


#ifdef __cplusplus
}
#endif

#endif