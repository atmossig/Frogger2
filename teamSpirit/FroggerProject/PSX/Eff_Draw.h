#ifndef EFF_DRAW_H_INCLUDED
#define	EFF_DRAW_H_INCLUDED

#include "game.h"
#include <isltex.h>
#include "specfx.h"

void DrawSpecialFX();
void ProcessShadows();
//void DrawShadow( SVECTOR, FVECTOR, float, float, short, long );
void DrawFXRipple( SPECFX * );
void DrawFXRing( SPECFX * );                           
void DrawFXTrail( SPECFX * );                        
void CalcTrailPoints( SVECTOR * , SPECFX * , int  ); 
void DrawFXLightning( SPECFX * );                    
/* void DrawRandomPolyList( )                           */
/* void TransformAndDrawPolygon( POLYGON );             */









#endif