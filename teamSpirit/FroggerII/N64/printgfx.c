/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: printgfx.c
	Programmer	: Andy Eder
	Date		: 19/04/99 11:12:48

----------------------------------------------------------------------------------------------- */


//************************************************************************************************
//	System Includes

#define F3DEX_GBI

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
	gsDPSetTextureFilter(G_TF_BILERP),
	gsDPSetTextureConvert(G_TC_FILT),
	gsDPSetCombineMode(G_CC_DECALRGB, G_CC_DECALRGB),
	gsDPSetCombineKey(G_CK_NONE),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsDPSetRenderMode(G_RM_OPA_SURF,G_RM_OPA_SURF2),
	gsDPPipeSync(),
	gsSPEndDisplayList(),
};

/*Gfx rdpInitForBackdrops_dl[] = 
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
};*/

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
	gsDPSetCombineMode(G_CC_MODULATERGBDECALA_PRIM,G_CC_MODULATERGBDECALA_PRIM),
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
	gsDPSetRenderMode(G_RM_AA_ZB_XLU_SURF,G_RM_AA_ZB_XLU_SURF2),
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

Vtx verts[32];
Vtx *fsVerts = NULL;
Vtx *vPtr = NULL;

unsigned short *grab = NULL;
unsigned short *scrTexGrab = NULL;

GRABSTRUCT grabData;

short drawScreenGrab = 0;
short grabFlag = 1;

/*	--------------------------------------------------------------------------------
	Function		: PrintBackdrop
	Purpose			: prints a backdrop...
	Parameters		: ptr to a backdrop to display
	Returns			: void
	Info			:
*/
void PrintBackdrop(BACKDROP *bDrop)
{
	gDPPipeSync(glistp++);

	gSPDisplayList(glistp++, rdpInitForBackdrops_dl);
	gSPDisplayList(glistp++, rspInitForBackdrops_dl);

	gDPSetPrimColor(glistp++,0,0,bDrop->r,bDrop->g,bDrop->b,bDrop->a);

	guSprite2DInit(&bDrop->N64Spr,(void *)bDrop->texture->data,bDrop->texture->palette, 
		bDrop->texture->sx,bDrop->texture->sx,bDrop->texture->sy,
		bDrop->texture->format,bDrop->texture->pixSize,0,0);

	gSPSprite2DBase(glistp++,OS_K0_TO_PHYSICAL(&bDrop->N64Spr));

	gSPSprite2DScaleFlip(glistp++,(1024*1024)/bDrop->scaleX,(1024*1024)/bDrop->scaleY,
		bDrop->flipX,bDrop->flipY);

	gSPSprite2DDraw(glistp++,bDrop->xPos,bDrop->yPos);

	gDPSetCycleType(glistp++,G_CYC_1CYCLE);

/*	gDPPipeSync(glistp++);
	gSPDisplayList(glistp++, rdpInitForBackdrops_dl);
	gSPObjRenderMode(glistp ++, G_OBJRM_XLU);
//	for(backdrop = backdropList.head.next;backdrop != &backdropList.head;backdrop = backdrop->next)
//	{
//		if((backdrop->draw) && (backdrop->texture))
//		{
//			if(backdrop->flags & BACKDROP_FILTER)
//			{
				gDPSetTextureFilter(glistp++,G_TF_BILERP);
//			}
//			else
//			{
//				gDPSetTextureFilter(glistp++,G_TF_POINT);
//			}

//			if((backdrop->a == 255) && (backdrop->texture->format != G_IM_FMT_IA))
//			{
//				if(backdrop->flags & BACKDROP_FILTER)
//				{
					gSPObjRenderMode(glistp ++, G_OBJRM_BILERP);
//				}
//				else
//				{
//					gSPObjRenderMode(glistp ++, 0);
//				}
				gDPSetRenderMode(glistp++, G_RM_SPRITE, G_RM_SPRITE2);
//			}
//			else
//			{
//				if(backdrop->flags & BACKDROP_FILTER)
//				{
//					gSPObjRenderMode(glistp ++, G_OBJRM_XLU | G_OBJRM_BILERP);
//				}
//				else
//				{
//					gSPObjRenderMode(glistp ++, G_OBJRM_XLU);
//				}
//				gDPSetRenderMode(glistp++, G_RM_XLU_SPRITE, G_RM_XLU_SPRITE2);
//			}

			switch(bDrop->texture->pixSize)
			{
				case G_IM_SIZ_4b:
					gDPSetTextureLUT(glistp++,G_TT_RGBA16);
					gDPLoadTLUT_pal16(glistp++,0,bDrop->texture->palette);
					break;
				case G_IM_SIZ_8b:
					gDPSetTextureLUT(glistp++,G_TT_RGBA16);
					gDPLoadTLUT_pal256(glistp++,bDrop->texture->palette);
					break;
				case G_IM_SIZ_16b:
					gDPSetTextureLUT(glistp++,G_TT_NONE);
					break;
			}
			gDPSetPrimColor(glistp++,0,0,bDrop->r,bDrop->g,bDrop->b,bDrop->a);
			gSPBgRect1Cyc(glistp++,&bDrop->background);
		//}
//	}
	gDPPipeSync(glistp++); */
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

//			gSPTextureRectangle(glistp++,x << 2,y << 2,(x + tOver->font->width) << 2,(y + tOver->font->height) << 2,
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

//			gSPTextureRectangle(glistp++,cur->xPos << 2,cur->yPos << 2,
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
	if(rippleFXList.numEntries)
		DrawFXRipples();

	if ( ( gameState.mode == GAME_MODE ) || ( gameState.mode == PAUSE_MODE ) ||
		 ( gameState.mode == CAMEO_MODE ) )
		ProcessShadows();
}


/*	--------------------------------------------------------------------------------
	Function		: DrawFXRipples
	Purpose			: draws the ripple based FX
	Parameters		: 
	Returns			: void
	Info			: 
*/
void DrawFXRipples()
{
	FX_RIPPLE *ripple,*ripple2;
	TEXTURE *theTexture;
	unsigned long i;
	float r;
	QUATERNION q;
	float transMtx[4][4],rotMtx[4][4],tempMtx[4][4];
	
	gSPDisplayList(glistp++,croakRing_dl);

	// go through list and draw ripples
	for(ripple = rippleFXList.head.next; ripple != &rippleFXList.head; ripple = ripple2)
	{
		ripple2 = ripple->next;
		if(ripple->deadCount)
			continue;

		// build matrices
		if(	ripple->rippleType == RIPPLE_TYPE_PICKUP ||
			ripple->rippleType == RIPPLE_TYPE_DUST)
		{
			guRotateF(rotMtx,ripple->yRot,ripple->normal.v[X],ripple->normal.v[Y],ripple->normal.v[Z]);
		}
		else
		{
			NormalToQuaternion(&q,&ripple->normal);
			QuaternionToMatrix(&q,(MATRIX *)rotMtx);
		}

		guTranslateF(transMtx,ripple->origin.v[X],ripple->origin.v[Y],ripple->origin.v[Z]);
		guMtxCatF(rotMtx,transMtx,tempMtx);

		guMtxF2L(tempMtx,&dynamicp->modeling4[objectMatrix]);
		gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->modeling4[objectMatrix++])),
					G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);

		r = ripple->radius;
		V((&ripple->verts[0]),-r,0,r,0,1024,0,ripple->r,ripple->g,ripple->b,ripple->alpha);
		V((&ripple->verts[1]),r,0,r,0,0,0,ripple->r,ripple->g,ripple->b,ripple->alpha);
		V((&ripple->verts[2]),r,0,-r,0,0,1024,ripple->r,ripple->g,ripple->b,ripple->alpha);
		V((&ripple->verts[3]),-r,0,-r,0,1024,1024,ripple->r,ripple->g,ripple->b,ripple->alpha);

		theTexture = ripple->txtr;
		if(!theTexture || !ripple->txtr)
		{
			dprintf"FECK FECK FECK !!!\n"));
			gSPPopMatrix(glistp++,G_MTX_MODELVIEW);
			return;
		}

		gDPLoadTextureBlock(glistp++,theTexture->data,G_IM_FMT_IA,G_IM_SIZ_16b,theTexture->sx,theTexture->sy,0,G_TX_WRAP,G_TX_WRAP,theTexture->TCScaleX,theTexture->TCScaleY,G_TX_NOLOD,G_TX_NOLOD);

		gSPVertex(glistp++,&ripple->verts[0],4,0);
		gSP2Triangles(glistp++,0,1,2,0,2,3,0,0);

		gSPPopMatrix(glistp++,G_MTX_MODELVIEW);
	}
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
	TEXTURE *theTexture;
	VECTOR vec;
	ENEMY *nme,*nme2;
	PLATFORM *plat;
	GARIB *garib;
	int i;
			
	gDPSetCycleType(glistp++,G_CYC_1CYCLE);

	gDPSetRenderMode(glistp++,G_RM_ZB_CLD_SURF,G_RM_ZB_CLD_SURF2);
	gDPSetCombineMode(glistp++,G_CC_MODULATEPRIMRGBA,G_CC_MODULATEPRIMRGBA);

	FindTexture(&theTexture,UpdateCRC("ai_circle.bmp"),YES,"ai_circle.bmp");
	gDPSetTextureLUT(glistp++,G_TT_NONE);
	gDPLoadTextureBlock(glistp++,theTexture->data,G_IM_FMT_IA,G_IM_SIZ_16b,theTexture->sx,theTexture->sy,0,
						G_TX_CLAMP,G_TX_CLAMP,theTexture->TCScaleX,theTexture->TCScaleY,G_TX_NOLOD,G_TX_NOLOD);

//------------------------------------------------------------------------------------------------
	// process frogs and babies shadows
/*
	i = number of players (1-4)
	while(i--)
	{
		if(frog[i]->actor->shadow)
		{
			vec.v[X] = frog[i]->actor->pos.v[X];
			vec.v[Y] = currTile[i]->centre.v[Y] + 1;
			vec.v[Z] = frog[i]->actor->pos.v[Z];
			DrawShadow(&vec,NULL,frog[i]->actor->shadow->radius,0,frog[i]->actor->shadow->alpha,frog[i]->actor->shadow->vert,NULL);
		}
	}
*/
	if(frog[0]->actor->shadow)
	{
		vec.v[X] = frog[0]->actor->pos.v[X];
		vec.v[Y] = currTile[0]->centre.v[Y] + 1;
		vec.v[Z] = frog[0]->actor->pos.v[Z];
		DrawShadow(&vec,NULL,frog[0]->actor->shadow->radius,0,frog[0]->actor->shadow->alpha,frog[0]->actor->shadow->vert,NULL);
	}
//------------------------------------------------------------------------------------------------

	i = numBabies;
	while(i--)
	{
		if(!babies[i]->action.isSaved)
		{
			if(babies[i]->actor->shadow && babies[i]->distanceFromFrog < ACTOR_DRAWDISTANCEINNER)
			{
				vec.v[X] = bTStart[i]->centre.v[X];
				vec.v[Y] = bTStart[i]->centre.v[Y] + 1;
				vec.v[Z] = bTStart[i]->centre.v[Z];
				DrawShadow(&vec,NULL,babies[i]->actor->shadow->radius,0,babies[i]->actor->shadow->alpha,babies[i]->actor->shadow->vert,NULL);
			}
		}
	}

	// process enemy shadows
	for(nme = enemyList.head.next; nme != &enemyList.head; nme = nme2)
	{
		nme2 = nme->next;

		if(nme->nmeActor->actor->shadow && nme->nmeActor->distanceFromFrog < ACTOR_DRAWDISTANCEINNER)
		{
			vec.v[X] = nme->nmeActor->actor->pos.v[X];
			vec.v[Y] = nme->inTile->centre.v[Y] + 1;
			vec.v[Z] = nme->nmeActor->actor->pos.v[Z];
			DrawShadow(&vec,NULL,nme->nmeActor->actor->shadow->radius,0,nme->nmeActor->actor->shadow->alpha,nme->nmeActor->actor->shadow->vert,NULL);
		}
	}

	// process platform shadows
	// to be done !!!

	// process garib shadows
	for(garib = garibCollectableList.head.next; garib != &garibCollectableList.head; garib = garib->next)
	{
		if(garib->distanceFromFrog < SPRITE_DRAWDISTANCE)
		{
			vec.v[X] = garib->sprite.pos.v[X];
			vec.v[Y] = garib->sprite.pos.v[Y] + garib->sprite.offsetY;
			vec.v[Z] = garib->sprite.pos.v[Z];
			DrawShadow(&vec,NULL,garib->shadow.radius,0,garib->shadow.alpha,garib->shadow.vert,NULL);
		}
	}
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
	QUATERNION q;
	float rotMtx[4][4],scaleMtx[4][4],transMtx[4][4],tempMtx[4][4];
	float scale;
	int i;
	VECTOR tempVect;
	VECTOR normal = { 0,1,0 };
	
	scale = size;
	gDPSetPrimColor(glistp++,0,0,0,0,0,alpha);
	if(vert != shadowVtx)
	{
		for(i=0; i<4; i++)
		{
			vert[i].v.cn[3] = alpha;
		}
	}

	NormalToQuaternion(&q,&normal);
	QuaternionToMatrix(&q,(MATRIX *)rotMtx);

	tempVect.v[X] = pos->v[X];
	tempVect.v[Y] = pos->v[Y];
	tempVect.v[Z] = pos->v[Z];

	guTranslateF(transMtx,tempVect.v[X],tempVect.v[Y],tempVect.v[Z]);
	guScaleF(scaleMtx,scale,1,scale);
	guMtxCatF(rotMtx,transMtx,tempMtx);
	guMtxCatF(scaleMtx,tempMtx,tempMtx);
	guMtxF2L(tempMtx,&dynamicp->modeling4[objectMatrix]);

	// push onto matrix stack
	gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->modeling4[objectMatrix++])),
				G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);

	gSPVertex(glistp++,vert,4,0);
	gSP2Triangles(glistp++,0,1,2,2,0,2,3,2);
	gSPPopMatrix(glistp++,G_MTX_MODELVIEW);
}


/*	--------------------------------------------------------------------------------
	Function		: DrawPauseFX
	Purpose			: draws the pause FX screen when the game is paused (obviously...)
	Parameters		: 
	Returns			: void
	Info			: 
*/

short testDim = 7;

void DrawPauseFX()
{
	TEXTURE *theTexture;
	float d;
	QUATERNION q;
	float transMtx[4][4],rotMtx[4][4],tempMtx[4][4];
	float newRotMtx[4][4];
	static float angle = 0;
						
	gSPDisplayList(glistp++,croakRing_dl);

	// draw screen-aligned quad
	NormalToQuaternion(&q,&inVec);
	QuaternionToMatrix(&q,(MATRIX *)rotMtx);

	guRotateF(newRotMtx,angle,0,0,1);

	guTranslateF(transMtx,0,0,10);
	guMtxCatF(rotMtx,newRotMtx,tempMtx);
	guMtxCatF(tempMtx,transMtx,tempMtx);

	guMtxF2L(tempMtx,&dynamicp->modeling4[objectMatrix]);
	gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->modeling4[objectMatrix++])),
				G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);

	d = testDim;
	V((&verts[0]),-d,0,d,0,1024,1024,testR,testG,testB,testA);
	V((&verts[1]),d,0,d,0,0,1024,testR,testG,testB,testA);
	V((&verts[2]),d,0,-d,0,0,0,testR,testG,testB,testA);
	V((&verts[3]),-d,0,-d,0,1024,0,testR,testG,testB,testA);

	FindTexture(&theTexture,UpdateCRC("ai_pausef.bmp"),YES,"ai_pausef.bmp");
	gDPLoadTextureBlock(glistp++,theTexture->data,G_IM_FMT_IA,G_IM_SIZ_16b,theTexture->sx,theTexture->sy,0,G_TX_WRAP,G_TX_WRAP,theTexture->TCScaleX,theTexture->TCScaleY,G_TX_NOLOD,G_TX_NOLOD);

	gSPVertex(glistp++,&verts[0],4,0);
	gSP2Triangles(glistp++,0,1,2,0,2,3,0,0);

	gSPPopMatrix(glistp++,G_MTX_MODELVIEW);

	angle += 2;
}


/*	--------------------------------------------------------------------------------
	Function		: ScreenFade
	Purpose			: fades the screen in or out
	Parameters		: UBYTE,UBYTE
	Returns			: void
	Info			: 
*/
void ScreenFade(UBYTE dir,UBYTE step)
{
	TEXTURE *theTexture;
	float d;
	QUATERNION q;
	float transMtx[4][4],rotMtx[4][4],tempMtx[4][4];
						
	gSPDisplayList(glistp++,croakRing_dl);

	// draw screen-aligned quad
	NormalToQuaternion(&q,&inVec);
	QuaternionToMatrix(&q,(MATRIX *)rotMtx);

	guTranslateF(transMtx,0,0,10);
	guMtxCatF(rotMtx,transMtx,tempMtx);

	guMtxF2L(tempMtx,&dynamicp->modeling4[objectMatrix]);
	gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->modeling4[objectMatrix++])),
				G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);

	d = testDim;
	V((&verts[0]),-d,0,d,0,1024,1024,testR,testG,testB,fadeOut);
	V((&verts[1]),d,0,d,0,0,1024,testR,testG,testB,fadeOut);
	V((&verts[2]),d,0,-d,0,0,0,testR,testG,testB,fadeOut);
	V((&verts[3]),-d,0,-d,0,1024,0,testR,testG,testB,fadeOut);

	FindTexture(&theTexture,UpdateCRC("ai_fullwhite.bmp"),YES,"ai_fullwhite.bmp");
	gDPLoadTextureBlock(glistp++,theTexture->data,G_IM_FMT_IA,G_IM_SIZ_16b,theTexture->sx,theTexture->sy,0,G_TX_WRAP,G_TX_WRAP,theTexture->TCScaleX,theTexture->TCScaleY,G_TX_NOLOD,G_TX_NOLOD);

	gSPVertex(glistp++,&verts[0],4,0);
	gSP2Triangles(glistp++,0,1,2,0,2,3,0,0);

	gSPPopMatrix(glistp++,G_MTX_MODELVIEW);

	if(dir)
	{
		fadeOut += step;
		if(fadeOut > 255)
			fadeOut = 255;
	}
	else
	{
		fadeOut -= step;
		if(fadeOut < 0)
			fadeOut = 0;
	}

	doScreenFade--;
}


/*	--------------------------------------------------------------------------------
	Function		: DrawDarkenedLevel
	Purpose			: draws alpha intensity 'darkness' over a level
	Parameters		: 
	Returns			: void
	Info			: 
*/
void DrawDarkenedLevel()
{
	TEXTURE *theTexture;
	float d;
	QUATERNION q;
	float transMtx[4][4],rotMtx[4][4],tempMtx[4][4];
						
	gSPDisplayList(glistp++,croakRing_dl);

	// draw screen-aligned quad
	NormalToQuaternion(&q,&inVec);
	QuaternionToMatrix(&q,(MATRIX *)rotMtx);

	guTranslateF(transMtx,-0.5,-0.5,10);
	guMtxCatF(rotMtx,transMtx,tempMtx);

	guMtxF2L(tempMtx,&dynamicp->modeling4[objectMatrix]);
	gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->modeling4[objectMatrix++])),
				G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);

	d = testDim;
	V((&verts[0]),-d,0,d,0,1024,1024,testR,testG,testB,lightIntensity);
	V((&verts[1]),d,0,d,0,0,1024,testR,testG,testB,lightIntensity);
	V((&verts[2]),d,0,-d,0,0,0,testR,testG,testB,lightIntensity);
	V((&verts[3]),-d,0,-d,0,1024,0,testR,testG,testB,lightIntensity);

	FindTexture(&theTexture,UpdateCRC("ai_bigcircle.bmp"),YES,"ai_bigcircle.bmp");
	gDPLoadTextureBlock(glistp++,theTexture->data,G_IM_FMT_IA,G_IM_SIZ_16b,theTexture->sx,theTexture->sy,0,G_TX_WRAP,G_TX_WRAP,theTexture->TCScaleX,theTexture->TCScaleY,G_TX_NOLOD,G_TX_NOLOD);

	gSPVertex(glistp++,&verts[0],4,0);
	gSP2Triangles(glistp++,0,1,2,0,2,3,0,0);

	gSPPopMatrix(glistp++,G_MTX_MODELVIEW);
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

	guMtxCatL(&(dynamicp->viewing[screenNum]),&(dynamicp->projection[screenNum]),&temp);
	guMtxL2F(printSpritesProj[screenNum],&temp);

	spriteList.xluMode = NO;

	gDPSetRenderMode(glistp++,G_RM_AA_ZB_TEX_EDGE,G_RM_AA_ZB_TEX_EDGE2);
	gDPSetCombineMode(glistp++,G_CC_MODULATEPRIMRGBA,G_CC_MODULATEPRIMRGBA);

	if(!testPause)
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

	guMtxCatL(&(dynamicp->viewing[screenNum]),&(dynamicp->projection[screenNum]),&temp);
	guMtxL2F(printSpritesProj[screenNum],&temp);

	gDPSetRenderMode(glistp++,G_RM_AA_ZB_TEX_EDGE,G_RM_AA_ZB_TEX_EDGE2);
	gDPSetCombineMode(glistp++,G_CC_MODULATEPRIMRGBA,G_CC_MODULATEPRIMRGBA);

	if(!testPause)
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
	float x,y,z,w;
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
	grabData.flags |= CALC_VTX;

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
		else
			grabData.alpha = 170;

		grabData.vR = grabData.vG = grabData.vB = 0x80;

		if( grabData.flags & TINT_RED )
			grabData.vR = 0xB4;
		else if( grabData.flags & TINT_GREEN )
			grabData.vG = 0xB4;
		else if( grabData.flags & TINT_BLUE )
			grabData.vB = 0xB4;

		grabData.pR = grabData.pG = grabData.pB = 0xff;
		grabData.eR = grabData.eG = grabData.eB = 0x80;
	}
	if( grabData.flags & VERTEX_WODGE )
	{
		grabData.sinAmt = 3;
		grabData.sinSpeed = 0.2;
		grabData.flags |= DYNAMIC_VTX;
	}
	if( (grabData.flags & TILE_SHRINK_HORZ) || (grabData.flags & TILE_SHRINK_VERT) )
	{
		grabData.maxScale = 0.5;
		grabData.speedScale = 0.0005;
		grabData.scale = 0;
		grabData.flags |= DYNAMIC_VTX;
	}
	if( grabData.flags & MEZZOTINT ) // Requires one of the TILE_SHRINK defines to do anything
	{
		grabData.scale = 0.07;
		grabData.maxScale = 0;
		grabData.speedScale = 0;
		grabData.flags |= DYNAMIC_VTX;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: DrawScreenGrab
	Purpose			: Draws the texture array made in Screen2Texture
	Parameters		: 
	Returns			: 
	Info			: 
*/
long newFlag = 0;
void DrawScreenGrab( unsigned long flags )
{
	QUATERNION q;
	float transMtx[4][4],rotMtx[4][4],tempMtx[4][4];
	long x, y, v, x1, y1, x2, y2, tileScale, cx, cy, vStep;

	if( newFlag == 1 || newFlag == 3 )
	{
		flags |= TILE_SHRINK_VERT;
		flags |= BARS_HORZ;
	}
	if( newFlag == 2 || newFlag == 3 )
	{
		flags |= TILE_SHRINK_HORZ;
		flags |= BARS_VERT;
	}

	if( !fsVerts )
	{
		fsVerts = (Vtx *)JallocAlloc( sizeof(Vtx)*320, NO, "Vtx Array" );
		vPtr = &fsVerts[0];
		grab = scrTexGrab;

		flags &= ~NEW_FLAGS;
		grabData.flags = flags;
		SetGrabData( );
	}
	else if( (flags & NEW_FLAGS) || newFlag )
	{
		flags &= ~NEW_FLAGS;
		newFlag = 0;
		grabData.flags = flags;
		SetGrabData( );
	}

	// Recalc vertices every frame
	if( grabData.flags & CALC_VTX )
	{
		if( (grabData.flags & TILE_SHRINK_HORZ) || (grabData.flags & TILE_SHRINK_VERT) )
			tileScale = grabData.yTS * grabData.scale;
		
		if( grabData.flags & SHRINK_TO_POINT )
		{
			if( grabData.yTS )
				grabData.yTS--;
			if( grabData.xTS )
				grabData.xTS--;

			if( !grabData.xTS && !grabData.yTS )
				grabData.flags &= ~SHRINK_TO_POINT;
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
					if (grabData.scale<grabData.maxScale)
						grabData.scale += grabData.speedScale;
				}
				
				if( grabData.flags & VERTEX_WODGE )
				{
					if (x!=0)
						x1+=sinf((x1+frameCount*grabData.sinSpeed))*grabData.sinAmt;
					if (x!=4)
						x2+=sinf((x2+frameCount*grabData.sinSpeed))*grabData.sinAmt;
					if (y!=0)
						y1+=sinf((y1+frameCount*grabData.sinSpeed))*grabData.sinAmt;
					if (y!=7)
						y2+=sinf((y2+frameCount*grabData.sinSpeed))*grabData.sinAmt;
				}

				V((&vPtr[v+0]),x1,grabData.zOff,-y1,0,2048,1024,grabData.vR,grabData.vG,grabData.vB,grabData.alpha);
				V((&vPtr[v+1]),x2,grabData.zOff,-y1,0,0   ,1024,grabData.vR,grabData.vG,grabData.vB,grabData.alpha);
				V((&vPtr[v+2]),x2,grabData.zOff,-y2,0,0   ,0   ,grabData.vR,grabData.vG,grabData.vB,grabData.alpha);
				V((&vPtr[v+3]),x1,grabData.zOff,-y2,0,2048,0   ,grabData.vR,grabData.vG,grabData.vB,grabData.alpha);
			}

		if( !(grabData.flags & DYNAMIC_VTX) )
			grabData.flags &= ~CALC_VTX;
	}

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
			if( !grabFlag )
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
	
	gSPPopMatrix(glistp++,G_MTX_MODELVIEW);
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
	static unsigned short *grab = NULL;
	unsigned long xTex = 0, yTex = 0, tStep = 0, x = 0, yPos = 0;
	long y = 76800;

	// Initialise grab buffer first time through
	if( !scrTexGrab )
	{
		grab = scrTexGrab = (unsigned short *)JallocAlloc(163840,NO,"TGrab"); //sizeof(short)*81920

		// Because the screen ends halfway down a row of textures, fill up the rest with blanks
		for( yTex=81919; yTex>71360; yTex-- )
			grab[yTex] = 0;

		yTex = 0;
		drawScreenGrab = 1;
		grabFlag = 0;
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
