/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: pcgfx.h
	Programmer	: Jim
	Date		: 16/03/00
	Purpose		: Effect and shadow drawing functions

----------------------------------------------------------------------------------------------- */

#ifndef PCGFX_H_INCLUDED
#define PCGFX_H_INCLUDED

#include "specfx.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define ONEOVERFIXED 0.00024414

void DrawSpecialFX();
void DrawFXDecal( SPECFX *fx );
void DrawFXRing( SPECFX *fx );
void DrawFXTrail( SPECFX *fx );
void DrawFXLightning( SPECFX *fx );

void ProcessShadows();

#ifdef __cplusplus
}
#endif


#endif
