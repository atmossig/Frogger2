/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: printgfx.c
	Programmer	: Andy Eder
	Date		: 19/04/99 11:12:48

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

#include <ultra64.h>

#include "incs.h"


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


Vtx verts[32];
Vtx *vPtr = NULL;


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
	Purpose			: draws overlays - icons, etc....
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

	// process frogs and babies shadows
	if(frog->actor->shadow)
	{
		vec.v[X] = frog->actor->pos.v[X];
		vec.v[Y] = currTile->centre.v[Y] + 1;
		vec.v[Z] = frog->actor->pos.v[Z];
		DrawShadow(&vec,NULL,frog->actor->shadow->radius,0,frog->actor->shadow->alpha,frog->actor->shadow->vert,NULL);
	}

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

