/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: printgfx.h
	Programmer	: Andy Eder
	Date		: 19/04/99 11:12:30

----------------------------------------------------------------------------------------------- */

#ifndef PRINTGFX_H_INCLUDED
#define PRINTGFX_H_INCLUDED


#define G_CC_DECALRGB_MODULATEPRIMA 0,0,0,TEXEL0,TEXEL0,0,PRIMITIVE,0
#define G_CC_MODULATEPRIMRGBA TEXEL0,0,PRIMITIVE,0,TEXEL0,0,PRIMITIVE,0
#define G_CC_MODULATERGBPRIMA TEXEL0,0,SHADE,0,0,0,0,PRIMITIVE

#define G_CC_COLOURBLEND_SPRITE		ENVIRONMENT,TEXEL0,ENV_ALPHA,TEXEL0,TEXEL0,0,PRIMITIVE,0
#define G_CC_COLOURBLEND_SPRITE2	COMBINED,0,PRIMITIVE,0,0,0,0,COMBINED

#define G_CC_COLOURBLEND_SPRITE_AFTERLIGHT	TEXEL0,0,PRIMITIVE,0,TEXEL0,0,PRIMITIVE,0
#define G_CC_COLOURBLEND_SPRITE_AFTERLIGHT2	ENVIRONMENT,COMBINED,ENV_ALPHA,COMBINED,0,0,0,COMBINED


extern Gfx rspInitForSprites_dl[];
extern Gfx rdpInitForSprites_dl[];


extern void PrintBackdrop(BACKDROP *bDrop);
extern void PrintTextAsOverlay(TEXTOVERLAY *tOver);
extern void PrintOverlays();
extern void PrintSpriteOverlays();

extern void DrawSpecialFX();
extern void DrawFXRipples();

extern void ProcessShadows();
extern void DrawShadow(VECTOR *pos,PLANE *plane,float size,float altitude,short alpha,Vtx *vert,VECTOR *lightDir);

extern void DrawPauseFX();
extern void ScreenFade(UBYTE dir,UBYTE step);

extern void DrawDarkenedLevel();


extern SPRITE *PrintSpritesOpaque();
extern void PrintSpritesTranslucent(SPRITE *sprite);
extern void TileRectangle(Gfx **glistp,SPRITE *sprite,f32 x0,f32 y0,int z,int scaleX,int scaleY);
extern void PrintSprite(SPRITE *sprite);


#endif
