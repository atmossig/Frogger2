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

//void DrawRandomPolyList( );
//void TransformAndDrawPolygon( POLYGON *p );

void DrawSpecialFX();
void DrawFXDecal( SPECFX *fx );
void DrawFXRing( SPECFX *fx );
void DrawFXTrail( SPECFX *fx );
void DrawFXLightning( SPECFX *fx );

void ProcessShadows();

int InitBackdrop(const char *filename);
void FreeBackdrop(void);
void DrawBackdrop(void);

#ifdef __cplusplus
}
#endif


#endif
