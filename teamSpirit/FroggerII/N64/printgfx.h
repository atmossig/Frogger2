/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: printgfx.h
	Programmer	: Andy Eder
	Date		: 19/04/99 11:12:30

----------------------------------------------------------------------------------------------- */

#ifndef PRINTGFX_H_INCLUDED
#define PRINTGFX_H_INCLUDED


/*#define G_CC_DECALRGB_MODULATEPRIMA 0,0,0,TEXEL0,TEXEL0,0,PRIMITIVE,0
#define G_CC_MODULATEPRIMRGBA TEXEL0,0,PRIMITIVE,0,TEXEL0,0,PRIMITIVE,0
#define G_CC_MODULATERGBPRIMA TEXEL0,0,SHADE,0,0,0,0,PRIMITIVE

#define G_CC_COLOURBLEND_SPRITE		ENVIRONMENT,TEXEL0,ENV_ALPHA,TEXEL0,TEXEL0,0,PRIMITIVE,0
#define G_CC_COLOURBLEND_SPRITE2	COMBINED,0,PRIMITIVE,0,0,0,0,COMBINED

#define G_CC_COLOURBLEND_SPRITE_AFTERLIGHT	TEXEL0,0,PRIMITIVE,0,TEXEL0,0,PRIMITIVE,0
#define G_CC_COLOURBLEND_SPRITE_AFTERLIGHT2	ENVIRONMENT,COMBINED,ENV_ALPHA,COMBINED,0,0,0,COMBINED
*/

#define	G_CC_MYTRILERP		TEXEL1, TEXEL0, LOD_FRACTION, TEXEL0, 0, 0, 0, PRIMITIVE

#define G_CC_MODULATERGB_MODULATEPRIMA  TEXEL0,0,SHADE,0,TEXEL0,0,PRIMITIVE,0
#define G_CC_MODULATERGB_MODULATEPRIMA2 COMBINED,0,SHADE,0,COMBINED,0,PRIMITIVE,0

#define G_CC_MODULATEPRIMRGBA TEXEL0,0,PRIMITIVE,0,TEXEL0,0,PRIMITIVE,0
#define G_CC_MODULATERGBPRIMA TEXEL0, 0, SHADE, 0, 0, 0, 0, PRIMITIVE

#define G_CC_TEXTUREBLEND TEXEL1, TEXEL0, SHADE_ALPHA, TEXEL0, 0, 0, 0, PRIMITIVE
#define G_CC_TEXTUREBLENDENV TEXEL1, TEXEL0, ENV_ALPHA, TEXEL0, 0, 0, 0, PRIMITIVE
#define G_CC_TEXTUREBLEND2 COMBINED, 0, SHADE, 0, 0, 0, 0, PRIMITIVE

#define G_CC_MODULATEPRIM	PRIMITIVE,0,SHADE,0,PRIMITIVE,0,SHADE,0
#define G_CC_MODULATEPRIM_ENVALPHA	PRIMITIVE,0,SHADE,0,ENVIRONMENT,0,SHADE,0

#define G_CC_HILITEFACECOLOUR1	SHADE,0,PRIMITIVE,0,TEXEL0,0,ENVIRONMENT,0
#define G_CC_HILITEFACECOLOUR2	ENVIRONMENT, COMBINED, TEXEL0, COMBINED, 0, 0, 0, COMBINED

#define G_CC_MODULATERGBPRIMA2 COMBINED, 0, SHADE, 0, 0, 0, 0, PRIMITIVE

#define G_CC_HILITERGB_PRIMA	PRIMITIVE, SHADE, TEXEL0, SHADE, TEXEL0, 0, PRIMITIVE,0
#define G_CC_HILITERGB_PRIMA2	ENVIRONMENT, COMBINED, TEXEL0, COMBINED, 0, 0, 0, COMBINED

#define G_CC_COLOURBLEND  ENVIRONMENT,TEXEL0,ENV_ALPHA,TEXEL0,TEXEL0,0,PRIMITIVE,0
#define G_CC_COLOURBLEND2 COMBINED,0,SHADE,0,0,0,0,COMBINED

#define G_CC_COLOURBLEND_AFTERLIGHT  TEXEL0,0,SHADE,0,TEXEL0,0,PRIMITIVE,0
#define G_CC_COLOURBLEND_AFTERLIGHT2 ENVIRONMENT,COMBINED,ENV_ALPHA,COMBINED,0,0,0,COMBINED

#define G_CC_COLOURBLEND_SPRITE  ENVIRONMENT,TEXEL0,ENV_ALPHA,TEXEL0,TEXEL0,0,PRIMITIVE,0
#define G_CC_COLOURBLEND_SPRITE2 COMBINED,0,PRIMITIVE,0,0,0,0,COMBINED

#define G_CC_COLOURBLEND_SPRITE_AFTERLIGHT2 ENVIRONMENT,COMBINED,ENV_ALPHA,COMBINED,0,0,0,COMBINED
#define G_CC_COLOURBLEND_SPRITE_AFTERLIGHT  TEXEL0,0,PRIMITIVE,0,TEXEL0,0,PRIMITIVE,0

#define G_CC_FACECOLOURBLEND  ENVIRONMENT,PRIMITIVE,ENV_ALPHA,PRIMITIVE,0,0,0,PRIMITIVE
#define G_CC_FACECOLOURBLEND2 COMBINED,0,SHADE,0,0,0,0,PRIMITIVE

#define G_CC_FACECOLOURBLEND_AFTERLIGHT  PRIMITIVE,0,SHADE,0,0,0,0,PRIMITIVE
#define G_CC_FACECOLOURBLEND_AFTERLIGHT2 ENVIRONMENT,COMBINED,ENV_ALPHA,COMBINED,0,0,0,PRIMITIVE

#define G_CC_DECALI_MODULATEA_PRIM    TEXEL0, 0, 1, SHADE, TEXEL0, 0, PRIMITIVE, 0


#define CALC_VTX			(1<<0)
#define DYNAMIC_VTX			(1<<1)

#define MOTION_BLUR			(1<<5)
#define VERTEX_WODGE		(1<<6)
#define TILE_SHRINK_HORZ	(1<<7)
#define TILE_SHRINK_VERT	(1<<8)
#define SHRINK_TO_POINT		(1<<9)
#define MEZZOTINT			(1<<10)

#define BLUR_HEAVY			(1<<20)
#define BLUR_MEDIUM			(1<<21)
#define BLUR_LIGHT			(1<<22)

#define BLUR_CENTRE			(1<<23)
#define BLUR_INWARD			(1<<24)
#define BLUR_OUTWARD		(1<<25)

#define TINT_RED			(1<<26)
#define TINT_GREEN			(1<<27)
#define TINT_BLUE			(1<<28)

#define NEW_FLAGS			(1<<31)

typedef struct TAGGRABSTRUCT
{
	long tSize, maxTSize,  // size and max size of texture tiles
		xOff, yOff, zOff;  // Offset of mesh on screen
	char vR, vG, vB,       // Vertex rgb
		pR, pG, pB,        // Primitive rgb
		eR, eG, eB,        // environment rgb
		alpha;             // Translucency of mesh
	unsigned long flags;
	// For vertex wodging
	float sinAmt;          // Amount to wodge
	float sinSpeed;        // Speed of wodge
	// For tile scaling operations
	float maxScale;        // Max scale of tiles
	float speedScale;      // Speed of scaling
	float scale;           // Current scale

} GRABSTRUCT;

extern Vtx shadowVtx[4];
extern Sprite template_sprite;

extern short drawScreenGrab;
extern short grabFlag;

extern GRABSTRUCT grabData;

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

extern void ScreenShot();
extern void Screen2Texture();
extern void DrawScreenGrab( unsigned long flags );
extern void SetGrabData( );

#endif
