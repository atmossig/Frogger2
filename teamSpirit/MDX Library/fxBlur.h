/*

	This file is part of the M libraries,

	File		: fxBlur.h
	Programmer	: Matthew Cloy
	Date		: 

----------------------------------------------------------------------------------------------- */

#ifndef FXBLUR_H_INCLUDED
#define FXBLUR_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

/*	--------------------------------------------------------------------------------
	Function	: D3DSetupRenderstates
	Purpose		: Setup Renderstates from a list
	Parameters	: a pointer to a pair of unsigned long values
	Returns		:
	Info		: 
*/

void fxBlurSurfaces(LPDIRECTDRAWSURFACE7 scr, LPDIRECTDRAWSURFACE7 bak, unsigned long is565);

/*	--------------------------------------------------------------------------------
	Function	: D3DSetupRenderstates
	Purpose		: Setup Renderstates from a list
	Parameters	: a pointer to a pair of unsigned long values
	Returns		:
	Info		: 
*/

void fxBlur(unsigned short *screen, unsigned short *oldscreen, unsigned long pitch1, unsigned long pitch2, unsigned short maskVal);

#ifdef __cplusplus
}
#endif

#endif