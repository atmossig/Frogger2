/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: printgfx.c
	Programmer	: Andy Eder
	Date		: 19/04/99 11:12:48

----------------------------------------------------------------------------------------------- */


#define F3DEX_GBI_2

#include <ultra64.h>
#include "incs.h"

//----- [ GLOBALS ] ----------------------------------------------------------------------------//

Vtx shadowVtx[4] =
{
	{ 1,0,1,0,		0,0,63,63,63,255 },
	{ 1,0,-1,0,		0,1024,63,63,63,255 },
	{ -1,0,-1,0,	1024,1024,63,63,63,255 },
	{ -1,0,1,0,		1024,0,63,63,63,255 }
};

static Bitmap template_bm[NUM_template_BMS];
static Gfx template_dl[NUM_DL(NUM_template_BMS)];

Sprite template_sprite =
{
	0,0,						// xy position
	0,0,						// sprite size in xy (in texels)
	1.0,1.0,					// sprite scale in xy
	0,0,						// explosion spacing
	SP_TRANSPARENT,				// sprite attributes
	0x00,						// sprite depth in z
	255,255,255,255,			// sprite RGBA colour
	0,1,NULL,					// CLUT : start index, length, address
	0,1,						// sprite bitmap index: start index, step increment
	NUM_template_BMS,			// number of bitmaps
	NUM_DL(NUM_template_BMS),	// number of allocated dl locations
	128,208,					// sprite bm height: used height, physical height
	G_IM_FMT_RGBA,				// sprite image format
	G_IM_SIZ_16b,				// sprite bitmap texel size
	template_bm,				// ptr to bitmaps
	template_dl,				// dl memory
	NULL,						// HACK : dynamic_dl ptr
};



//----- [ RENDER STATES ] ----------------------------------------------------------------------//

Gfx rdpInitForBackdrops_dl[] = 
{
	gsDPSetCycleType(G_CYC_1CYCLE),    
	gsDPPipelineMode(G_PM_NPRIMITIVE), 
	gsDPSetTextureLOD(G_TL_TILE),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetTextureDetail(G_TD_CLAMP),
	gsDPSetTexturePersp(G_TP_NONE),
#ifdef NOFILTER
	gsDPSetTextureFilter(G_TF_POINT),
#else
	gsDPSetTextureFilter(G_TF_BILERP),
#endif
	gsDPSetTextureConvert(G_TC_FILT),
	gsDPSetCombineMode(G_CC_MODULATEPRIMRGBA, G_CC_MODULATEPRIMRGBA),
	gsDPSetCombineKey(G_CK_NONE),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsDPSetRenderMode(G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2),
	gsDPSetColorDither(G_CD_BAYER),
	gsDPPipeSync(),
	gsSPEndDisplayList(),
};

Gfx rspInitForBackdrops_dl[] = 
{
	gsSPClearGeometryMode(G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH |
			G_FOG | G_LIGHTING | G_TEXTURE_GEN |
			G_TEXTURE_GEN_LINEAR | G_LOD ),
	gsSPTexture(0, 0, 0, 0, G_OFF),
	gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH),
	gsSPEndDisplayList(),  
};

Gfx rdpInitForOverlays_dl[] = 
{
	gsDPSetTextureFilter(G_TF_BILERP),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetTextureLUT(G_TT_RGBA16),
//	gsDPSetRenderMode(G_RM_CLD_SURF,G_RM_CLD_SURF2),
	gsDPSetRenderMode(G_RM_XLU_SURF,G_RM_XLU_SURF2),
	gsDPSetCombineMode(G_CC_MODULATEPRIMRGBA,G_CC_MODULATEPRIMRGBA),
	gsSPEndDisplayList(),
};

Gfx rspInitForOverlays_dl[] = 
{
	gsSPClearGeometryMode(G_CULL_BOTH | G_LIGHTING),
    gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH),	
	gsSPTexture(0xffff,0xffff,0,G_TX_RENDERTILE,G_ON),
	gsSPEndDisplayList(),
};


Gfx rspInitForSprites_dl[] = 
{
//	gsSPClearGeometryMode(G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH |
//			G_FOG | G_LIGHTING | G_TEXTURE_GEN |
//			G_TEXTURE_GEN_LINEAR | G_LOD ),
//	gsSPTexture(0, 0, 0, 0, G_OFF),
//	gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH),
	gsSPEndDisplayList(),
};

Gfx rdpInitForSprites_dl[] =
{
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPPipelineMode(G_PM_NPRIMITIVE),
	gsDPSetTextureLOD(G_TL_TILE),
	gsDPSetTextureDetail(G_TD_CLAMP),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetTextureFilter(G_TF_BILERP),
	gsDPSetTextureConvert(G_TC_FILT),
	gsDPSetCombineKey(G_CK_NONE),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsDPSetColorDither(G_CD_DISABLE),
	gsDPSetDepthSource(G_ZS_PRIM),
	gsSPEndDisplayList(),
};


// display list for intensity alpha and the croak - ANDYE
// assumes 32x32 texture will be loaded (8bpp grayscale) into TMEM
Gfx croakRing_dl[] =
{
	gsDPPipeSync(),
	gsSPClearGeometryMode(G_CULL_BOTH | G_FOG),
	gsSPSetGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
	gsSPSetGeometryMode(G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),

	gsDPSetCombineMode(G_CC_MODULATERGBA,G_CC_MODULATERGBA),
//	gsDPSetRenderMode(G_RM_AA_ZB_XLU_SURF,G_RM_AA_ZB_XLU_SURF2),
	gsDPSetRenderMode(G_RM_ZB_XLU_SURF,G_RM_ZB_XLU_SURF2),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsSPTexture(0xffff,0xffff,0,G_TX_RENDERTILE,G_ON),
    gsDPPipelineMode(G_PM_NPRIMITIVE),
	gsDPSetColorDither(G_CD_DISABLE),
	gsDPSetTexturePersp(G_TP_PERSP),
	gsDPSetTextureLOD(G_TL_TILE),
	gsDPSetTextureFilter(G_TF_BILERP),
	gsDPSetTextureConvert(G_TC_FILT),
	gsDPSetTextureLUT(G_TT_NONE),
	gsSPEndDisplayList()
};

#define G_CC_MOTIONBLUR TEXEL0,ENVIRONMENT,PRIMITIVE,SHADE,  TEXEL0,0,SHADE,0

// For full screen blur fx and stuff
Gfx polyNoZ_dl[] =
{
	gsDPPipeSync(),
	gsSPClearGeometryMode( G_ZBUFFER | G_CULL_BOTH | G_FOG ),
	gsSPSetGeometryMode( G_SHADE | G_SHADING_SMOOTH ),
	
	gsDPSetCombineMode(G_CC_MOTIONBLUR,G_CC_MOTIONBLUR),

	gsDPSetRenderMode(G_RM_XLU_SURF,G_RM_XLU_SURF2),
	gsDPSetCycleType(G_CYC_1CYCLE),

	gsSPTexture(0xffff,0xffff,0,G_TX_RENDERTILE,G_ON),

    gsDPPipelineMode(G_PM_NPRIMITIVE),
	gsDPSetColorDither(G_CD_DISABLE),
	gsDPSetTexturePersp(G_TP_PERSP),
	gsDPSetTextureLOD(G_TL_TILE),
	gsDPSetTextureFilter(G_TF_POINT),
	gsDPSetTextureConvert(G_TC_FILT),
	gsDPSetTextureLUT(G_TT_NONE),
	gsSPEndDisplayList()
};

Gfx setrend_light_fog2[] =
{
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_2CYCLE),
	gsDPPipeSync(),
	gsSPEndDisplayList()
};


Vtx verts[32];
Vtx *fsVerts = NULL;
Vtx *vPtr = NULL;

unsigned short *grab = NULL;
unsigned short *scrTexGrab = NULL;

GRABSTRUCT grabData;

short drawScreenGrab = 0;
short grabFlag = 0;

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			:
*/
void PrintBackdrops()
{
	BACKDROP *backdrop;

	if(backdropList.numEntries == 0)
		return;

	gDPPipeSync(glistp++);
	gSPDisplayList(glistp++, rdpInitForBackdrops_dl);
	gSPObjRenderMode(glistp ++, G_OBJRM_XLU);
	for(backdrop = backdropList.head.next;backdrop != &backdropList.head;backdrop = backdrop->next)
	{
		if((backdrop->draw) && (backdrop->texture))
		{
			if(backdrop->flags & BACKDROP_FILTER)
			{
				gDPSetTextureFilter(glistp++,G_TF_BILERP);
			}
			else
			{
				gDPSetTextureFilter(glistp++,G_TF_POINT);
			}

			if((backdrop->a == 255) && (backdrop->texture->format != G_IM_FMT_IA))
			{
				if(backdrop->flags & BACKDROP_FILTER)
				{
					gSPObjRenderMode(glistp ++, G_OBJRM_BILERP);
				}
				else
				{
					gSPObjRenderMode(glistp ++, 0);
				}
				gDPSetRenderMode(glistp++, G_RM_SPRITE, G_RM_SPRITE2);
			}
			else
			{
				if(backdrop->flags & BACKDROP_FILTER)
				{
					gSPObjRenderMode(glistp ++, G_OBJRM_XLU | G_OBJRM_BILERP);
				}
				else
				{
					gSPObjRenderMode(glistp ++, G_OBJRM_XLU);
				}
				gDPSetRenderMode(glistp++, G_RM_XLU_SPRITE, G_RM_XLU_SPRITE2);
			}

			switch(backdrop->texture->pixSize)
			{
				case G_IM_SIZ_4b:
					gDPSetTextureLUT(glistp++,G_TT_RGBA16);
					gDPLoadTLUT_pal16(glistp++,0,backdrop->texture->palette);
					break;
				case G_IM_SIZ_8b:
					gDPSetTextureLUT(glistp++,G_TT_RGBA16);
					gDPLoadTLUT_pal256(glistp++,backdrop->texture->palette);
					break;
				case G_IM_SIZ_16b:
					gDPSetTextureLUT(glistp++,G_TT_NONE);
					break;
			}
			gDPSetPrimColor(glistp++,0,0,backdrop->r,backdrop->g,backdrop->b,backdrop->a);
			gSPBgRect1Cyc(glistp++,&backdrop->background);
		}
	}
	gDPPipeSync(glistp++);
}


/*	--------------------------------------------------------------------------------
	Function		: PrintTextAsOverlay
	Purpose			: prints text as an overlay
	Parameters		: none
	Returns			: void
	Info			:
*/
void PrintTextAsOverlay(TEXTOVERLAY *tOver)
{
	unsigned int pos = 0,length;
	unsigned int x,y,width;
	unsigned char letter,letterCount;
	static float wSeed = 0.0F;

	gSPDisplayList(glistp++,rdpInitForOverlays_dl);
	gSPDisplayList(glistp++,rspInitForOverlays_dl);
			
	x = tOver->xPos;
	y = tOver->yPos;

	if(tOver->centred)
	{
		// Centre text along screen x-axis
		length	= strlen(tOver->text);
		width	= length * (tOver->font->xSpacing[0]);
		x		= 160 - (width >> 1);

		if((length & 1) != 0)
			x -= 5;
	}

	gDPSetPrimColor(glistp++,0,0,tOver->r,tOver->g,tOver->b,tOver->a);

	letterCount = 0;
	while(tOver->text[pos] != '\0')
	{
		letter = tOver->text[pos];

		if(tOver->flags & TEXTOVERLAY_WAVECHARS)
		{
			letterCount++;
			y = tOver->yPos + sinf(wSeed + tOver->waveStart + (letterCount * (PI_OVER_4))) * tOver->waveAmplitude;
		}

		if(letter == 32)
		{
			x += tOver->font->xSpacing[0];
		}
		else
		{
			gDPLoadTLUT_pal16(glistp++,0,&tOver->font->data[tOver->font->offset[characterMap[letter]] + tOver->font->palOffset]);

			gDPLoadTextureBlock_4b(glistp++,&tOver->font->data[tOver->font->offset[characterMap[letter]]],G_IM_FMT_CI,
									tOver->font->width,tOver->font->height,
									0,G_TX_CLAMP|G_TX_NOMIRROR,G_TX_CLAMP|G_TX_NOMIRROR,0,0,G_TX_NOLOD,G_TX_NOLOD);

			gSPScisTextureRectangle(glistp++,x << 2,y << 2,(x + tOver->font->width) << 2,(y + tOver->font->height) << 2,
									G_TX_RENDERTILE,0,0,1<<10,1<<10);

			x += tOver->font->xSpacing[characterMap[letter]];
		}

		pos++;
	}

	wSeed += (0.05F * desiredFrameRate);
}


/*	--------------------------------------------------------------------------------
	Function		: PrintSpriteOverlays
	Purpose		.	: draws overlays - icons, etc....
	Parameters		: none
	Returns			: void
	Info			:
*/
void PrintSpriteOverlays()
{
	float tScaleX,tScaleY;
	SPRITEOVERLAY *cur,*next;
	TEXTURE *texture;

	if(spriteOverlayList.numEntries == 0)
		return;

	gSPDisplayList(glistp++,rdpInitForOverlays_dl);
	gSPDisplayList(glistp++,rspInitForOverlays_dl);

	for(cur = spriteOverlayList.head.next; cur != &spriteOverlayList.head; cur = next)
	{
		next = cur->next;

		// update the sprite animation if an animated sprite overlay
		if(cur->draw)
		{
			// check animation playback type - cycle,ping-pong, etc.
			if(cur->flags & ANIMATION_CYCLE)
			{
				// sprite is cycling through frames - check if cycling forwards or backwards
				if(cur->flags & ANIMATION_FORWARDS)
				{
					// cycling forwards
					cur->currFrame = (short)cur->animTime;
					cur->animTime += cur->animSpeed;
					if(cur->animTime > cur->numFrames)
					{
						cur->animTime = 0;
						cur->currFrame = 0;
					}
				}
				else if(cur->flags & ANIMATION_BACKWARDS)
				{
					// cycling backwards
					cur->currFrame = ROUND2SHORT(cur->animTime);
					cur->animTime -= cur->animSpeed;
					if(cur->animTime < 0)
					{
						cur->animTime = cur->numFrames-1;
						cur->currFrame = cur->numFrames-1;
					}
				}
			}
			else if(cur->flags & ANIMATION_PINGPONG)
			{
				// sprite ping-ponging through frames - check if currently forwards or backwards
				if(cur->flags & ANIMATION_FORWARDS)
				{
					// cycling forwards
					cur->currFrame = (short)cur->animTime;
					cur->animTime += cur->animSpeed;
					if(cur->animTime >= cur->numFrames)
					{
						cur->animTime = cur->numFrames-1;
						cur->currFrame = cur->numFrames-1;
						cur->flags &= ~ANIMATION_FORWARDS;
						cur->flags |= ANIMATION_BACKWARDS;
					}
				}
				else if(cur->flags & ANIMATION_BACKWARDS)
				{
					// cycling backwards
					cur->currFrame = ROUND2SHORT(cur->animTime);
					cur->animTime -= cur->animSpeed;
					if(cur->animTime <= 0)
					{
						cur->animTime = 0;
						cur->currFrame = 0;
						cur->flags &= ~ANIMATION_BACKWARDS;
						cur->flags |= ANIMATION_FORWARDS;
					}
				}
			}
			else if(cur->flags & ANIMATION_RANDOMFRAMES)
			{
				// choose a random frame and display it
				cur->currFrame = Random(cur->numFrames);
			}

			texture = cur->frames[cur->currFrame];

			// perform scaling
			tScaleX = ((float)cur->width  / (float)texture->sx) * 1024.0;
			tScaleY = ((float)cur->height / (float)texture->sy) * 1024.0;

			tScaleX = (1024.0 * 1024.0) / tScaleX;
			tScaleY = (1024.0 * 1024.0) / tScaleY;

			gDPSetPrimColor(glistp++,0,0,cur->r,cur->g,cur->b,cur->a);

			gDPLoadTLUT_pal256(glistp++,texture->palette);
			gDPLoadTextureBlock(glistp++,texture->data,G_IM_FMT_RGBA,G_IM_SIZ_8b,
								  texture->sx,texture->sy,0,
								  G_TX_CLAMP|G_TX_NOMIRROR,
								  G_TX_CLAMP|G_TX_NOMIRROR,
								  0,0,
								  G_TX_NOLOD,G_TX_NOLOD);

			gSPScisTextureRectangle(glistp++,cur->xPos << 2,cur->yPos << 2,
								(cur->xPos + cur->width) << 2,
								(cur->yPos + cur->height) << 2,
								G_TX_RENDERTILE,
								0,0,
								(short)tScaleX,
								(short)tScaleY);
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: DrawSpecialFX
	Purpose			: updates and draws the various special FX
	Parameters		: 
	Returns			: void
	Info			: 
*/
void DrawSpecialFX()
{
	if(specFXList.numEntries)
	{
		SPECFX *fx;
		for(fx = specFXList.head.next; fx!=&specFXList.head; fx=fx->next)
			if(fx->Draw)
				fx->Draw(fx);
	}
}


/*	--------------------------------------------------------------------------------
	Function		: DrawFXRipples
	Purpose			: draws the ripple based FX
	Parameters		: SPECFX *
	Returns			: void
	Info			: 
*/
void DrawFXRipple(SPECFX *ripple)
{
}


/*	--------------------------------------------------------------------------------
	Function		: ProcessShadows
	Purpose			: processes the shadows
	Parameters		: 
	Returns			: void
	Info			: 
*/
void ProcessShadows()
{
}


/*	--------------------------------------------------------------------------------
	Function		: DrawShadow
	Purpose			: draws a shadow
	Parameters		: VECTOR *,PLANE *,float,float,short,Vtx *,VECTOR *
	Returns			: 
	Info			: 
*/
void DrawShadow(VECTOR *pos,PLANE *plane,float size,float altitude,short alpha,Vtx *vert,VECTOR *lightDir)
{
}



float printSpritesProj[4][4][4];
float fm;

/*	--------------------------------------------------------------------------------
	Function		: PrintSpritesOpaque
	Purpose			: prints opaque sprites
	Parameters		: 
	Returns			: SPRITE *
	Info			: 
*/
SPRITE *PrintSpritesOpaque()
{
	SPRITE *cur,*next;
	Mtx temp;

	spriteList.lastTexture = NULL;

	gSPDisplayList(glistp++,rspInitForSprites_dl);
	gSPDisplayList(glistp++,rdpInitForSprites_dl);
	gDPSetScissor(glistp++,G_SC_NON_INTERLACE,0,0,SCREEN_WD,SCREEN_HT);

	fm = 128000/(fog.max - fog.min);	
	if(fog.mode)
	{
		gSPDisplayList(glistp++,setrend_light_fog2); 
	}

	guMtxCatL(&(dynamicp->viewing[screenNum]),&(dynamicp->projection[screenNum]),&temp);
	guMtxL2F(printSpritesProj[screenNum],&temp);

	spriteList.xluMode = NO;

	if(fog.mode)
	{
		gDPSetRenderMode(glistp++,G_RM_FOG_PRIM_A,G_RM_AA_ZB_TEX_EDGE2);
		gDPSetCombineMode(glistp++,G_CC_MODULATEPRIMRGBA,G_CC_PASS2);
	}
	else
	{
		gDPSetRenderMode(glistp++,G_RM_AA_ZB_TEX_EDGE,G_RM_AA_ZB_TEX_EDGE2);
		gDPSetCombineMode(glistp++,G_CC_MODULATEPRIMRGBA,G_CC_MODULATEPRIMRGBA);
	}

	if(!pauseMode)
	{
		for(cur = spriteList.head.next; (cur != &spriteList.head) && ((cur->flags & SPRITE_TRANSLUCENT) == 0); cur = next)
		{
			next = cur->next;

			PrintSprite(cur);
		}
	}

	return cur;
}

/*	--------------------------------------------------------------------------------
	Function		: PrintSpritesTranslucent
	Purpose			: prints translucent sprites
	Parameters		: SPRITE *
	Returns			: void
	Info			: 
*/
void PrintSpritesTranslucent(SPRITE *sprite)
{
	Mtx temp;
	
	spriteList.lastTexture = NULL;

	gSPDisplayList(glistp++,rspInitForSprites_dl);
	gSPDisplayList(glistp++,rdpInitForSprites_dl);
	gDPSetScissor(glistp++,G_SC_NON_INTERLACE,0,0,SCREEN_WD,SCREEN_HT);

	fm = 128000/(fog.max - fog.min);	
	if(fog.mode)
	{
		gSPDisplayList(glistp++,setrend_light_fog2); 
	}

	guMtxCatL(&(dynamicp->viewing[screenNum]),&(dynamicp->projection[screenNum]),&temp);
	guMtxL2F(printSpritesProj[screenNum],&temp);

	if(fog.mode)
	{
		gDPSetRenderMode(glistp++,G_RM_FOG_PRIM_A,G_RM_AA_ZB_TEX_EDGE2);
		gDPSetCombineMode(glistp++,G_CC_MODULATEPRIMRGBA,G_CC_PASS2);
	}
	else
	{
		gDPSetRenderMode(glistp++,G_RM_AA_ZB_TEX_EDGE,G_RM_AA_ZB_TEX_EDGE2);
		gDPSetCombineMode(glistp++,G_CC_MODULATEPRIMRGBA,G_CC_MODULATEPRIMRGBA);
	}

	if(!pauseMode)
	{
		for(; sprite != &spriteList.head; sprite = sprite->next)
		{
			PrintSprite(sprite);
		}
	}

	// IMPORTANT - MUST SET THESE STATES
	gDPSetDepthSource(glistp++,G_ZS_PIXEL);
	gDPSetTexturePersp(glistp++,G_TP_PERSP);
}

/*	--------------------------------------------------------------------------------
	Function		: TileRectangle
	Purpose			: displays a tile rectangle
	Parameters		: Gfx**,SPRITE *,f32,f32,int,int,int
	Returns			: void
	Info			: 
*/
void TileRectangle(Gfx **glistp,SPRITE *sprite,f32 x0,f32 y0,int z,int scaleX,int scaleY)
{
	Gfx *gfx = *glistp;
	int incx,incy;
	float tempx,tempy;
	int s,t;
	int sval,tval;

	gDPSetPrimDepth(gfx++,z,0);

	if(sprite->flags & SPRITE_FLIPX)
		incx = -scaleX;
	else
		incx = scaleX;

	if(sprite->flags & SPRITE_FLIPY)
		incy = -scaleY;
	else
		incy = scaleY;

	t = (scaleX < 512 ? -16 : -16 - (((int)y0) & 3) * 8);
	s = -16;

	if(sprite->texture != spriteList.lastTexture)
	{
		switch(sprite->texture->pixSize)
		{
			case G_IM_SIZ_4b:
				gDPSetTextureLUT(gfx++,G_TT_RGBA16);
				gDPLoadTLUT_pal16(gfx++,0,sprite->texture->palette);
				gDPLoadTextureTile_4b(gfx++,(void *)sprite->texture->data,sprite->texture->format,
					sprite->texture->sx,sprite->texture->sy,
					0,0,sprite->texture->sx - 1,sprite->texture->sy - 1,0,
					G_TX_NOMIRROR | G_TX_CLAMP,G_TX_NOMIRROR | G_TX_CLAMP,
					0,0,G_TX_NOLOD,G_TX_NOLOD);
				break;

			case G_IM_SIZ_8b:
				gDPSetTextureLUT(gfx++,G_TT_RGBA16);
				gDPLoadTLUT_pal256(gfx++,sprite->texture->palette);
				gDPLoadTextureTile(gfx++,(void *)sprite->texture->data,sprite->texture->format,
					G_IM_SIZ_8b,sprite->texture->sx,sprite->texture->sy,
					0,0,sprite->texture->sx - 1,sprite->texture->sy - 1,0,
					G_TX_NOMIRROR | G_TX_CLAMP,G_TX_NOMIRROR | G_TX_CLAMP,
					0,0,G_TX_NOLOD,G_TX_NOLOD);
				break;

			case G_IM_SIZ_16b:
				gDPSetTextureLUT(gfx++,G_TT_NONE);
				gDPLoadTextureTile(gfx++,(void *)sprite->texture->data,sprite->texture->format,
					G_IM_SIZ_16b,sprite->texture->sx,sprite->texture->sy,
					0,0,sprite->texture->sx - 1,sprite->texture->sy - 1,0,
					G_TX_NOMIRROR | G_TX_CLAMP,G_TX_NOMIRROR | G_TX_CLAMP,
					0,0,G_TX_NOLOD,G_TX_NOLOD);
				break;

			case G_IM_SIZ_32b:
				gDPLoadTextureTile(gfx++,(void *)sprite->texture->data,sprite->texture->format,
					G_IM_SIZ_32b,sprite->texture->sx,sprite->texture->sy,
					0,0,sprite->texture->sx - 1,sprite->texture->sy - 1,0,
					G_TX_NOMIRROR | G_TX_CLAMP,G_TX_NOMIRROR | G_TX_CLAMP,
					0,0,G_TX_NOLOD,G_TX_NOLOD);
				break;
		}
	}

	tempx = sprite->texture->sx;
	tempx *= (1 << 12);
	tempx /= (float)scaleX;
	tempx = x0 + tempx;

	tempy = sprite->texture->sy;
	tempy *= (1 << 12);
	tempy /= (float)scaleY;
	tempy = y0 + tempy;

	if(sprite->flags & SPRITE_FLIPX)
		sval = ((sprite->texture->sx) << 5) - s - 32;
	else
		sval = s;

	if(sprite->flags & SPRITE_FLIPY)
		tval = ((sprite->texture->sy) << 5) - t - 32;
	else
		tval = t;

	gSPScisTextureRectangle(gfx++,x0,y0,(int)tempx,(int)tempy,G_TX_RENDERTILE,sval,tval,incx,incy);
	*glistp = gfx;
}

/*	--------------------------------------------------------------------------------
	Function		: PrintSprite
	Purpose			: prints a sprite
	Parameters		: SPRITE *
	Returns			: void
	Info			: 
*/
void PrintSprite(SPRITE *sprite)
{
	float x,y,z,w,fogVal;
	float dist;
	int scaleX,scaleY;
	int sprScaleX,sprScaleY;

	if((!sprite->texture) || (sprite->scaleX == 0) || (sprite->scaleY == 0))
		return;

	dist = DistanceBetweenPointsSquared(&actualCamSource[draw_buffer][screenNum],&sprite->pos);
	if((dist > farPlaneDist * farPlaneDist) || (dist < nearPlaneDist * nearPlaneDist))
		return;

	w = printSpritesProj[screenNum][0][3] * sprite->pos.v[X] + 
		printSpritesProj[screenNum][1][3] * sprite->pos.v[Y] +
		printSpritesProj[screenNum][2][3] * sprite->pos.v[Z] +
		printSpritesProj[screenNum][3][3];

	if(w <= 0)
		return;

	z = printSpritesProj[screenNum][0][2] * sprite->pos.v[X] + 
		printSpritesProj[screenNum][1][2] * sprite->pos.v[Y] +
		printSpritesProj[screenNum][2][2] * sprite->pos.v[Z] +
		printSpritesProj[screenNum][3][2];

	if(z < 0)
		return;

	x = printSpritesProj[screenNum][0][0] * sprite->pos.v[X] +
		printSpritesProj[screenNum][1][0] * sprite->pos.v[Y] +
		printSpritesProj[screenNum][2][0] * sprite->pos.v[Z] +
		printSpritesProj[screenNum][3][0];

	y = printSpritesProj[screenNum][0][1] * sprite->pos.v[X] +
		printSpritesProj[screenNum][1][1] * sprite->pos.v[Y] +
		printSpritesProj[screenNum][2][1] * sprite->pos.v[Z] +
		printSpritesProj[screenNum][3][1];

	x = ((x * dynamicp->vp[screenNum].vp.vscale[0] / w) + dynamicp->vp[screenNum].vp.vtrans[0]);
	y = ((-y * dynamicp->vp[screenNum].vp.vscale[1] / w) + dynamicp->vp[screenNum].vp.vtrans[1]);
	z = 32 * ((z * dynamicp->vp[screenNum].vp.vscale[2] / w) + dynamicp->vp[screenNum].vp.vtrans[2]);

	dist = sqrtf(dist);

	sprScaleX = 33 * sprite->scaleX * ((float)dynamicp->vp[screenNum].vp.vscale[0] / (float)(SCREEN_WD * 2));
	x = (f32)x + (f32)(sprite->offsetX * sprScaleX << 4) / (dist * yFOV);
	if((x > SCREEN_WD * 4) || (x + (sprite->texture->sx * sprScaleX << 4) / (dist * yFOV) <= 0))
		return;

	sprScaleY = 33 * sprite->scaleY * ((float)dynamicp->vp[screenNum].vp.vscale[1] / (float)(SCREEN_HT * 2));
	y = (f32)y + (f32)(sprite->offsetY * sprScaleY << 4) / (dist * yFOV);
	if((y > SCREEN_HT * 4) || (y + (sprite->texture->sy * sprScaleY << 4) / (dist * yFOV) <= 0))
		return;

	gDPSetPrimColor(glistp++,0,0,sprite->r,sprite->g,sprite->b,sprite->a);

	if(sprite->flags & SPRITE_FLAGS_PIXEL_OUT)
	{
		gDPSetAlphaCompare(glistp++,G_AC_DITHER);
	}
	else
	{
		gDPSetAlphaCompare(glistp++,G_AC_NONE);
	}
		
	if(fog.mode)
	{
		fogVal = MAX(0,((z-16360)/16360)*fm + 255 - fm);
		if(fogVal >= 255)
			return;
		gDPSetFogColor(glistp++,fog.r,fog.g,fog.b,(unsigned int)fogVal);
		if(sprite->flags & SPRITE_TRANSLUCENT)
		{
			gDPSetRenderMode(glistp++,G_RM_FOG_PRIM_A,G_RM_ZB_CLD_SURF2);
		}
		else
		{
			gDPSetRenderMode(glistp++,G_RM_FOG_PRIM_A,G_RM_AA_ZB_TEX_EDGE2);
		}
		gDPSetCombineMode(glistp++,G_CC_MODULATEPRIMRGBA, G_CC_PASS2);
		gDPSetCycleType(glistp++,G_CYC_2CYCLE);
	}
	else
	{
		if(sprite->flags & SPRITE_FLAGS_COLOUR_BLEND)
		{
			if(sprite->flags & SPRITE_TRANSLUCENT)
			{
				gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_ZB_CLD_SURF2);
			}
			else
			{
				gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_AA_ZB_TEX_EDGE2);
			}
			gDPSetCombineMode(glistp++,G_CC_COLOURBLEND_SPRITE,G_CC_COLOURBLEND_SPRITE2);
			gDPSetEnvColor(glistp++,sprite->red2,sprite->green2,sprite->blue2,sprite->alpha2);
			gDPSetCycleType(glistp++,G_CYC_2CYCLE);
		}
		else if(sprite->flags & SPRITE_FLAGS_COLOUR_BLEND_AFTERLIGHT)
		{
			if(sprite->flags & SPRITE_TRANSLUCENT)
			{
				gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_ZB_CLD_SURF2);
			}
			else
			{
				gDPSetRenderMode(glistp++,G_RM_PASS,G_RM_AA_ZB_TEX_EDGE2);
			}
			gDPSetCombineMode(glistp++,G_CC_COLOURBLEND_SPRITE_AFTERLIGHT,G_CC_COLOURBLEND_SPRITE_AFTERLIGHT2);
			gDPSetEnvColor(glistp++,sprite->red2,sprite->green2,sprite->blue2,sprite->alpha2);
			gDPSetCycleType(glistp++,G_CYC_2CYCLE);
		}
		else
		{
			if(sprite->flags & SPRITE_TRANSLUCENT)
			{
				if(sprite->flags & SPRITE_FLAGS_NOZB)
				{
					gDPSetRenderMode(glistp++,G_RM_CLD_SURF,G_RM_CLD_SURF2);
				}
				else
				{
					gDPSetRenderMode(glistp++,G_RM_ZB_CLD_SURF,G_RM_ZB_CLD_SURF2);
				}
			}
			else
			{
				if(sprite->flags & SPRITE_FLAGS_NOZB)
				{
					gDPSetRenderMode(glistp++,G_RM_AA_TEX_EDGE,G_RM_AA_TEX_EDGE2);
				}
				else
				{
					gDPSetRenderMode(glistp++,G_RM_AA_ZB_TEX_EDGE,G_RM_AA_ZB_TEX_EDGE2);
				}
			}
			gDPSetCombineMode(glistp++,G_CC_MODULATEPRIMRGBA,G_CC_MODULATEPRIMRGBA);
			gDPSetCycleType(glistp++,G_CYC_1CYCLE);
		}
	}

/*
	gDPSetAlphaCompare(glistp++,G_AC_NONE);

	if(sprite->flags & SPRITE_TRANSLUCENT)
	{
		gDPSetRenderMode(glistp++,G_RM_ZB_CLD_SURF,G_RM_ZB_CLD_SURF2);
	}
	else
	{
		gDPSetRenderMode(glistp++,G_RM_AA_ZB_TEX_EDGE,G_RM_AA_ZB_TEX_EDGE2);
	}

	gDPSetCombineMode(glistp++,G_CC_MODULATEPRIMRGBA,G_CC_MODULATEPRIMRGBA);
	gDPSetCycleType(glistp++,G_CYC_1CYCLE);
*/
	

	scaleY = dist * yFOV;
	scaleX = (scaleY << 8) / sprScaleX;
	scaleY = (scaleY << 8) / sprScaleY;

	TileRectangle(&glistp,sprite,x,y,z,scaleX,scaleY);
	gDPPipeSync(glistp++);

	spriteList.lastTexture = sprite->texture;
}


/*	--------------------------------------------------------------------------------
	Function		: SetGrabData
	Purpose			: Update vars in grabData from new flags
	Parameters		: 
	Returns			: 
	Info			: 
*/
void SetGrabData( )
{
	grabData.calcVtx = 1;

	if( grabData.flags & MOTION_BLUR )
	{
		grabData.maxxTS = grabData.xTS = 72;
		grabData.maxyTS = grabData.yTS = 36;
		grabData.xOff = 35;
		grabData.yOff = 44;

		if( grabData.flags & BLUR_INWARD )
			grabData.zOff = 330;
		else if( grabData.flags & BLUR_OUTWARD )
			grabData.zOff = 302;
		else
			grabData.zOff = 316;

		if( grabData.flags & BLUR_HEAVY )
			grabData.alpha = 220;
		else if( grabData.flags & BLUR_LIGHT )
			grabData.alpha = 128;
		else if( grabData.flags & OVERLAY_SOLID )
			grabData.alpha = 255;
		else
			grabData.alpha = 170;

		grabData.vR = grabData.vG = grabData.vB = 0x80;

		if( grabData.flags & TINT_RED )
			grabData.vR = 0xB4;
		else if( grabData.flags & TINT_GREEN )
			grabData.vG = 0xB4;
		else if( grabData.flags & TINT_BLUE )
			grabData.vB = 0xB4;
		else if( grabData.flags & TINT_SHADE )
			grabData.vR = grabData.vG = grabData.vB = 0x50;

		grabData.pR = grabData.pG = grabData.pB = 0xff;
		grabData.eR = grabData.eG = grabData.eB = 0x80;
	}
	if( grabData.flags & VERTEX_WODGE )
	{
		grabData.sinAmt = 3;
		grabData.sinSpeed = 0.1;
		grabData.dynVtx = 1;
	}
	if( (grabData.flags & TILE_SHRINK_HORZ) || (grabData.flags & TILE_SHRINK_VERT) )
	{
		grabData.maxScale = 0.5;
		grabData.speedScale = 0.05;
		grabData.scale = 0;
		grabData.dynVtx = 1;
	}
	if( grabData.flags & MEZZOTINT ) // Requires one of the TILE_SHRINK defines to do anything
	{
		grabData.scale = 0.07;
		grabData.maxScale = 0;
		grabData.speedScale = 0;
		grabData.dynVtx = 1;
	}
	if( (grabData.flags & RECALC_VTX) ||
		(grabData.flags & SHRINK_TO_POINT) )
	{
		grabData.dynVtx = 1;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: DrawScreenGrab
	Purpose			: Draws the texture array made in Screen2Texture
	Parameters		: 
	Returns			: 
	Info			: 
*/
void DrawScreenGrab( unsigned long flags )
{
	QUATERNION q;
	float transMtx[4][4],rotMtx[4][4],tempMtx[4][4];
	long x, y, v, x1, y1, x2, y2, tileScale, cx, cy, vStep;
	if( !fsVerts )
	{
		fsVerts = (Vtx *)JallocAlloc( sizeof(Vtx)*160, NO, "Vtx Array" );
		vPtr = &fsVerts[0];
		grab = scrTexGrab;

		grabData.flags = flags;
		SetGrabData( );
	}
	else if( flags != grabData.flags )
	{
		grabData.flags = flags;
		SetGrabData( );
	}

	if( grabData.afterEffect == FROG_DEATH_OUT && grabData.fxTimer )
		grabData.fxTimer--;

	if( (grabData.flags & USE_GRAB_BUFFER) && !grab )
	{
		dprintf"No Grab Data!\n"));
		return;
	}
	// Recalc vertices every frame
	if( grabData.calcVtx )
	{
		if( (grabData.flags & TILE_SHRINK_HORZ) || (grabData.flags & TILE_SHRINK_VERT) )
		{
			tileScale = grabData.yTS * grabData.scale;

			if (grabData.scale<grabData.maxScale)
				grabData.scale += grabData.speedScale;
			else
				grabData.afterEffect = NO_EFFECT;
		}
		
		if( grabData.flags & SHRINK_TO_POINT )
		{
			if( grabData.yTS )
				grabData.yTS--;
			if( grabData.xTS )
				grabData.xTS--;

			if( !grabData.xTS || !grabData.yTS )
			{
				grabData.flags &= ~SHRINK_TO_POINT;
				grabData.afterEffect = NO_EFFECT;
			}
		}

		for (y=0; y<8; y++)
			for (x=0; x<5; x++)
			{
				v = (x+(y*5))<<2;

				x1 = ((2-x)*grabData.xTS)-grabData.xOff;
				y1 = ((4-y)*grabData.yTS)-grabData.yOff;
				
				x2 = ((3-x)*grabData.xTS)-grabData.xOff;
				y2 = ((5-y)*grabData.yTS)-grabData.yOff;
				
				if( (grabData.flags & TILE_SHRINK_HORZ) || (grabData.flags & TILE_SHRINK_VERT) )
				{
					if( grabData.flags & TILE_SHRINK_HORZ )
					{
						if( grabData.flags & BARS_VERT )
						{
							x2 -= tileScale;
							x1 += tileScale;
						}
						else
						{
							x2 -= tileScale<<1;
							x1 += tileScale<<1;
						}
					}
					if( grabData.flags & TILE_SHRINK_VERT )
					{
						if( grabData.flags & BARS_HORZ )
						{
							y2 -= tileScale>>1;
							y1 += tileScale>>1;
						}
						else
						{
							y2 -= tileScale;
							y1 += tileScale;
						}
					}
				}
				
				if( grabData.flags & VERTEX_WODGE )
				{
					if (x!=0)
						x2+=sinf((x2+frameCount*grabData.sinSpeed))*grabData.sinAmt;
					if (x!=4)
						x1+=sinf((x1+frameCount*grabData.sinSpeed))*grabData.sinAmt;
					if (y!=0)
						y2+=sinf((y2+frameCount*grabData.sinSpeed))*grabData.sinAmt;
					if (y!=7)
						y1+=sinf((y1+frameCount*grabData.sinSpeed))*grabData.sinAmt;
				}

				V((&vPtr[v+0]),x1,grabData.zOff,-y1,0,2048,1024,grabData.vR,grabData.vG,grabData.vB,grabData.alpha);
				V((&vPtr[v+1]),x2,grabData.zOff,-y1,0,0   ,1024,grabData.vR,grabData.vG,grabData.vB,grabData.alpha);
				V((&vPtr[v+2]),x2,grabData.zOff,-y2,0,0   ,0   ,grabData.vR,grabData.vG,grabData.vB,grabData.alpha);
				V((&vPtr[v+3]),x1,grabData.zOff,-y2,0,2048,0   ,grabData.vR,grabData.vG,grabData.vB,grabData.alpha);
			}

		if( !grabData.dynVtx )
			grabData.calcVtx = 0;
	}

	gDPPipeSync(glistp++);

	gSPDisplayList(glistp++,polyNoZ_dl);
	
	gDPSetPrimColor(glistp++,0,0,grabData.pR,grabData.pG,grabData.pB,255);
	gDPSetEnvColor(glistp++,grabData.eR,grabData.eG,grabData.eB,255);

	// draw screen-aligned quad
	NormalToQuaternion(&q,&inVec);
	QuaternionToMatrix(&q,(MATRIX *)rotMtx);

	guTranslateF(transMtx,-0.5,-0.5,10);
	guMtxCatF(rotMtx,transMtx,tempMtx);

	guMtxF2L(tempMtx,&dynamicp->modeling4[objectMatrix]);
	gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->modeling4[objectMatrix++])),
										G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
	
	for( x=0,cx=0,cy=0; x<160; x+=32 )
	{
		// Load in 32 vertices at a time
		gSPVertex(glistp++,&vPtr[x],32,0);
		
		if( grabData.flags & DES_REP_MODE )
		{
			cx=0;
			cy=0;
		}

		for( vStep=0; vStep<32; vStep+=4 )
		{
			if( !(grabData.flags & USE_GRAB_BUFFER) )
			{
				gDPLoadTextureTile( glistp++,&(((short *)cfb_ptrs[1-draw_buffer])[cx+cy]),G_IM_FMT_RGBA,G_IM_SIZ_16b,320,32,0,0,63,31,0,G_TX_CLAMP,G_TX_CLAMP,G_TX_NOMASK,G_TX_NOMASK,G_TX_NOLOD,G_TX_NOLOD );
			}
			else
			{
				gDPLoadTextureTile(glistp++,&grab[cx+cy],G_IM_FMT_RGBA,G_IM_SIZ_16b,320,32,0,0,63,31,0,G_TX_CLAMP,G_TX_CLAMP,G_TX_NOMASK,G_TX_NOMASK,G_TX_NOLOD,G_TX_NOLOD);
			}

			gSP2Triangles(glistp++,0+vStep,1+vStep,2+vStep,0,2+vStep,3+vStep,0+vStep,0);

			cx+=64;

			if( cx >= 320 )
			{
				cy += 10240;
				cx = 0;
			}
		}
	}
	
	gDPPipeSync(glistp++);

	gSPPopMatrix(glistp++,G_MTX_MODELVIEW);
}

/*	--------------------------------------------------------------------------------
	Function		: FreeGrabData
	Purpose			: Delete all the stuff that DrawScreenGrab creates
	Parameters		: 
	Returns			: 
	Info			: 
*/
Mtx	nomatrix;

void SetRendermodeForEnviroment(void)
{
	gSPClearGeometryMode(glistp++,G_TEXTURE_GEN);
}

/*	--------------------------------------------------------------------------------
	Function		: FreeGrabData
	Purpose			: Delete all the stuff that DrawScreenGrab creates
	Parameters		: 
	Returns			: 
	Info			: 
*/
void FreeGrabData()
{
	drawScreenGrab = 0;
	pauseMode = 0;
	grabData.afterEffect = 0;

	if( fsVerts )
	{
		JallocFree( (UBYTE **)&fsVerts );
		fsVerts = NULL;
		vPtr = NULL;
	}

	if( scrTexGrab )
	{
		JallocFree( (UBYTE **)&scrTexGrab );
		scrTexGrab = NULL;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: Screen2Texture
	Purpose			: Grabs primary frame buffer into a 2D array of 32x32 textures
	Parameters		: 
	Returns			: 
	Info			:	10240 is the number of pixels for one row of 32x32 textures.
						81920 is the number of pixels in the whole screen, including the unused
						half a row at the bottom.
*/

void Screen2Texture( )
{
	u16 *screen = cfb_ptrs[1-draw_buffer];
	unsigned long xTex = 0, yTex = 0, tStep = 0, x = 0, yPos = 0;
	long y = 76800;

	// Initialise grab buffer first time through
	if( !scrTexGrab )
	{
		scrTexGrab = (unsigned short *)JallocAlloc(163840,NO,"TGrab"); //sizeof(short)*81920

		grab = scrTexGrab;

		// Because the screen ends halfway down a row of textures, fill up the rest with blanks
		for( yTex=81919; yTex>71360; yTex-- )
			grab[yTex] = 0;

		yTex = 0;
	}

	lmemcpy( (unsigned long *)&grab[0], (unsigned long *)&screen[0], 38400 );
/*
	while( y >= 0 )
	{
		while( x < SCREEN_WD )
		{
			lmemcpy( (unsigned long *)&grab[xTex+yPos], (unsigned long *)&screen[x+y], 16 );
			x += 32;
			xTex += 1024;
		}

		y -= SCREEN_WD;
		tStep += 32;
		x = xTex = 0;

		if( tStep >= 1024 )
		{
			tStep = 0;
			yTex += 10240;
		}

		yPos = yTex + tStep;
	}*/
	
}

/*	--------------------------------------------------------------------------------
	Function		: IsPointVisible
	Purpose			: determine if a point in 3D space is visible in 2D screen space
	Parameters		: VECTOR *
	Returns			: char
	Info			: 
*/
char IsPointVisible(VECTOR *p)
{
	VECTOR m;
	XfmPoint (&m,p);
	if (m.v[3]==0)
		return 0;
	if (m.v[0]<0) 
		return 0;
	if (m.v[0]>640) 
		return 0;
	if (m.v[1]<0) 
		return 0;
	if (m.v[1]>480)
		return 0;
	return 1;
}

/*	--------------------------------------------------------------------------------
	Function		: ScreenShot
	Purpose			: performs a screen grab
	Parameters		: 
	Returns			: void
	Info			: 
*/
void ScreenShot()
{
	static int picnum = 0;
	char	filename[16];
	int	file;
	int x, y;
	u16 *screen = cfb_16_a;
	u16 pixel;
	u8	col;
	u8	line[SCREEN_WD * 4];
	int	linePos;
	char header[] =	   {0x42,0x4D,0x36,0x84,0x03,0x00,0x00,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00,
						0x00,0x00,0x40,0x01,0x00,0x00,0xF0,0x00,0x00,0x00,0x01,0x00,0x18,0x00,0x00,0x00,
						0x00,0x00,0x00,0x84,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00};


	StopDrawing("scchot");
	dontClearScreen = TRUE;
//help	disableGraphics = TRUE;

	dprintf"==================\n"));
	dprintf"Taking screen shot\n"));


	sprintf(filename, "c:\\n64%04d.bmp", picnum++);
	file = PCcreat(filename, 0);
	if(file == -1)
	{
		StartDrawing("scsho");
//help		disableGraphics = FALSE;
		dprintf"FILEERROR:could not open file:\n"));
		return;
	}

	PCwrite(file, header, sizeof(header));	

	y = SCREEN_HT-1;
	while (y >= 0)
	{
		linePos = 0;
		for(x = 0; x < SCREEN_WD; x++)
		{
			pixel = screen[x + SCREEN_WD * y];

			col = ((pixel>>1)<<3)&0xFF;
			line[linePos++] = col;
			col = ((pixel>>6)<<3)&0xFF;
			line[linePos++] = col;
			col = ((pixel>>11)<<3)&0xFF;
			line[linePos++] = col;


		}
		y--;

			PCwrite(file, line, linePos);	
		
	}


//	PCwrite(file, cfb_ptrs[1 - draw_buffer], SCREEN_WD*SCREEN_HT * 2);	
	PCclose(file);
	dontClearScreen = FALSE;
	StartDrawing("bum");
//help	disableGraphics = FALSE;

}

void LoadTextureForTrophy( TEXTURE *tex )
{
	gDPSetTextureLUT(glistp++,G_TT_NONE);

	gSPTexture(glistp++,32<<6,32<<6,0,G_TX_RENDERTILE, G_ON);
	gDPLoadTextureBlock(glistp++,tex->data,G_IM_FMT_RGBA,G_IM_SIZ_16b,tex->sx,tex->sy,
							0,G_TX_WRAP,G_TX_WRAP,5,5,G_TX_NOLOD,G_TX_NOLOD);
}