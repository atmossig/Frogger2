/*

	This file is part of the M libraries,

	File		: mliReport.h
	Programmer	: Matthew Cloy
	Date		: 

----------------------------------------------------------------------------------------------- */

#ifndef MDXDDRAW_H_INCLUDED
#define MDXDDRAW_H_INCLUDED

#define RELEASE(x) if (x) {x->Release(); x=NULL;}
#define DDINIT(x) {memset(&x,0,sizeof(x)); x.dwSize = sizeof(x);}

enum 
{
	PRIMARY_SRF,
	RENDER_SRF,
	ZBUFFER_SRF,
	NUM_SRF
};

extern LPDIRECTDRAW			pDirectDraw;
extern LPDIRECTDRAW4		pDirectDraw4;
extern LPDIRECTDRAWCLIPPER	pClipper;
extern LPDIRECTDRAWSURFACE	surface[NUM_SRF];
extern unsigned long		rXRes, rYRes, rBitDepth, r565 ,rHardware;

/*	--------------------------------------------------------------------------------
	Function	: DDrawInitObject
	Purpose		: Initialise Directdraw objects
	Parameters	: Guid of device
	Returns		: success
	Info		: 
*/

unsigned long DDrawInitObject (GUID *guid);

/*	--------------------------------------------------------------------------------
	Function	: DDrawCreateSurfaces
	Purpose		: Create all our surfaces
	Parameters	: xRes,yRes & Bitdepth, whether we will want a 3D capable render surface, and whether we would want a zbuffer
	Returns		: success
	Info		: 
*/

unsigned long DDrawCreateSurfaces(unsigned long xRes, unsigned long yRes, unsigned long bitDepth, unsigned long want3D = 0, unsigned long zBits = 0);

/*	--------------------------------------------------------------------------------
	Function	: DDrawAttachSurface
	Purpose		: Attach two surfaces
	Parameters	: A is surface that B attaches to.
	Returns		: success
	Info		: 
*/

unsigned long DDrawAttachSurface(unsigned long srfA, unsigned long srfB);

/*	--------------------------------------------------------------------------------
	Function	: DDrawSetupWindow
	Purpose		: Setup for widowing
	Parameters	: The window that will contain the view of the primary surface
	Returns		: success
	Info		: 
*/

unsigned long DDrawSetupWindow(HWND window);

//	-------------------------- Example
//
//	DDrawInitObject (guid);
//	DDrawCreateSurfaces (640, 480, 16, TRUE, zBuffer?16:0); 
//	DDrawSetupWindow (window);
//
//	DDrawAttachSurface (PRIMARY_SRF,RENDER_SRF);
//	if (zBuffer)
//	DDrawAttachSurface (RENDER_SRF,ZBUFFER_SRF);
//
//	-------------------------- 

#endif

