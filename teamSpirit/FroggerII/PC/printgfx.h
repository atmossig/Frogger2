/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: printgfx.h
	Programmer	: Andy Eder
	Date		: 19/04/99 11:12:30

----------------------------------------------------------------------------------------------- */

#ifndef PRINTGFX_H_INCLUDED
#define PRINTGFX_H_INCLUDED

#include "specfx.h"

#define G_CC_DECALRGB_MODULATEPRIMA 0,0,0,TEXEL0,TEXEL0,0,PRIMITIVE,0
#define G_CC_MODULATEPRIMRGBA TEXEL0,0,PRIMITIVE,0,TEXEL0,0,PRIMITIVE,0
#define G_CC_MODULATERGBPRIMA TEXEL0,0,SHADE,0,0,0,0,PRIMITIVE

#define G_CC_COLOURBLEND_SPRITE		ENVIRONMENT,TEXEL0,ENV_ALPHA,TEXEL0,TEXEL0,0,PRIMITIVE,0
#define G_CC_COLOURBLEND_SPRITE2	COMBINED,0,PRIMITIVE,0,0,0,0,COMBINED

#define G_CC_COLOURBLEND_SPRITE_AFTERLIGHT	TEXEL0,0,PRIMITIVE,0,TEXEL0,0,PRIMITIVE,0
#define G_CC_COLOURBLEND_SPRITE_AFTERLIGHT2	ENVIRONMENT,COMBINED,ENV_ALPHA,COMBINED,0,0,0,COMBINED


#define BACKDROP_FILTER	(1<<0)
#define BACKDROP_WRAP	(1<<1)

typedef struct TAGBACKDROP
{
	struct			TAGBACKDROP *next,*prev;
	struct			TAGBACKDROP *parent;
	TEXTURE			*texture;
	UBYTE			r,g,b,a;
	int				xPos,yPos,zPos;
	short			scaleX,scaleY;
	short			draw;
	short			flags;
//	uObjBg			background;

} BACKDROP;

typedef struct TAGBACKDROPLIST
{
	int				numEntries;
	BACKDROP		head;

} BACKDROPLIST;

//extern BACKDROPLIST backdropList;

extern Vtx shadowVtx[4];
extern Sprite template_sprite;


extern Gfx rspInitForSprites_dl[];
extern Gfx rdpInitForSprites_dl[];


void PrintBackdrops();
void PrintTextAsOverlay(TEXTOVERLAY *tOver);
void PrintOverlays();
void PrintSpriteOverlays();

void DrawRandomPolyList( );
void TransformAndDrawPolygon( POLYGON *p );

void DrawSpecialFX();
void DrawFXRipple( SPECFX *ripple );
void DrawFXRing( SPECFX *ring );
void DrawFXTrail( SPECFX *trail );
void DrawFXLightning( SPECFX *fx );

void ProcessShadows();

void DrawSwirlFX();
void ScreenFade(UBYTE dir,UBYTE step);

void DrawDarkenedLevel();


SPRITE *PrintSprites();
//void PrintSpritesTranslucent(SPRITE *sprite);
void TileRectangle(Gfx **glistp,SPRITE *sprite,f32 x0,f32 y0,int z,int scaleX,int scaleY);
void PrintSprite(SPRITE *sprite);

//void ScreenShot();
char IsPointVisible(VECTOR *p);

void InitBackdropLinkedList();
void FreeBackdropLinkedList();
void AddBackdrop(BACKDROP *bDrop);
void SubBackdrop(BACKDROP *bDrop);
BACKDROP *SetupBackdrop(BACKDROP *backdrop,int texID,int sourceX,int sourceY,int z,int destX,int destY,int destWidth,int destHeight,int scalex,int scaley,int flags);


#endif
