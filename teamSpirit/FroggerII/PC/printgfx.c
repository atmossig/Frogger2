/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: printgfx.c
	Programmer	: Andy Eder
	Date		: 19/04/99 11:12:48

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>

#include "incs.h"
#include "software.h"

//GRABSTRUCT grabData;

short drawScreenGrab = 0;
short grabFlag = 0;
extern float RES_DIFF;
extern float RES_DIFF2;
extern long runHardware;
long numSprites;
void Clip3DPolygon (D3DTLVERTEX in[3], long texture);

/*	--------------------------------------------------------------------------------
	Function		: PrintBackdrops
	Purpose			: used to print the backdrops
	Parameters		: 
	Returns			: void
	Info			: 
*/
void PrintBackdrops()
{
	// TO BE IMPLEMENTED
}

/*	--------------------------------------------------------------------------------
	Function		: PrintTextAsOverlay
	Purpose			: prints text as an overlay
	Parameters		: none
	Returns			: void
	Info			:
*/
float wSeed = 0.0F;

void PrintTextAsOverlay(TEXTOVERLAY *tOver)
{
	unsigned int pos = 0,length;
	unsigned int x,y,width;
	unsigned char letter,letterCount;
	
	short u,v,letterID;
				
	x = tOver->xPos;
	y = tOver->yPos;

	if(tOver->centred)
	{
		// Centre text along screen x-axis
		length	= strlen(tOver->text);
		width	= length * (tOver->font->xSpacing[0]);
		x		= (320 >> 1) - (width >> 1);

		if((length & 1) != 0)
			x -= 5;
	}

	letterCount = 0;
	while(tOver->text[pos] != '\0')
	{
		letter = tOver->text[pos];
		letterID = characterMap[letter];

		
		if(tOver->flags & TEXTOVERLAY_WAVECHARS)
		{
			float t = sinf(wSeed);
			letterCount++;
			y = tOver->yPos + sinf((float)wSeed + (float)tOver->waveStart + ((float)letterCount * (PI_OVER_4))) * tOver->waveAmplitude;
		}

		if(letter == 32)
		{
			x += tOver->font->xSpacing[0];
		}
		else
		{
			u = tOver->font->offset[letterID].v[X];
			v = tOver->font->offset[letterID].v[Y];
			
			if (runHardware)
			{
				DrawAlphaSprite (x*RES_DIFF2,y*RES_DIFF2,0,tOver->font->height*RES_DIFF2,tOver->font->width*RES_DIFF2,
				(float)u/256.0,(float)v/256.0,
				((float)u+tOver->font->width-1)/256.0,
				((float)v+tOver->font->height-1)/256.0,tOver->font->hdl,D3DRGBA(1,1,1,tOver->a/255.0));
			}
	
			x += tOver->font->xSpacing[letterID];
		}

		pos++;
	}

	wSeed += 0.1;
}
void XfmPoint (VECTOR *vTemp2, VECTOR *in);

extern long FOV,DIST;

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

void PrintSprite(SPRITE *sprite)
{
	VECTOR m,sc;
	VECTOR s = {1,1,0};
	float distx,disty;
	TEXENTRY *tEntry;

	if((!sprite->texture) || (sprite->scaleX == 0) || (sprite->scaleY == 0))
		return;

	XfmPoint (&m,&sprite->pos);
	
	if (m.v[Z])
	{
		tEntry = ((TEXENTRY *)sprite->texture);
		distx = disty = (FOV)/(m.v[Z]+DIST);
		distx *= (sprite->scaleX/(64.0))*RES_DIFF;
		disty *= (sprite->scaleY/(64.0))*RES_DIFF;
		numSprites++;
		if (runHardware)
			DrawAlphaSprite (m.v[X]+sprite->offsetX*distx,m.v[Y]+sprite->offsetY*disty,m.v[Z]/2000.0,32*distx,32*disty,
				0,
				0,
				1,
				1,
				tEntry->hdl,D3DRGBA(sprite->r/255.0,sprite->g/255.0,sprite->b/255.0,sprite->a/255.0) );
	}
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

SPRITE *PrintSpritesOpaque()
{
	SPRITE *cur,*next;
	
	spriteList.lastTexture = NULL;
	spriteList.xluMode = NO;

//	if(!pauseMode)
//	{
		for(cur = spriteList.head.next; (cur != &spriteList.head); cur = next)
		{
			next = cur->next;
			PrintSprite(cur);
		}
//	}

	return cur;
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
	SPRITEOVERLAY *cur;
	short *texture,x,y;
	TEXENTRY *tEntry;


	cur = spriteOverlayList.head.next;
	if (spriteOverlayList.numEntries)
	while(cur!=&spriteOverlayList.head)
	{
		
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
					if(cur->animTime > cur->numFrames)
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
					if(cur->animTime < 0)
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
				cur->currFrame = rand() % cur->numFrames;
			}
			
			tEntry = ((TEXENTRY *)cur->frames[cur->currFrame]);
			
			if (tEntry)
				texture = tEntry->hdl;
			
			//tEntry->xo = 7 * (32.0/256.0);
			//tEntry->yo = 3 * (32.0/256.0);
			numSprites++;
			if (runHardware)
				DrawAlphaSprite (cur->xPos*RES_DIFF2,cur->yPos*RES_DIFF2,0,cur->width*RES_DIFF2,cur->height*RES_DIFF2,
				0,
				0,
				1,
				1,
				texture,D3DRGBA(cur->r/255.0,cur->g/255.0,cur->b/255.0,cur->a/255.0) );
		}

		cur = cur->next;
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
	if ( ( gameState.mode == GAME_MODE ) || ( gameState.mode == PAUSE_MODE ) ||
		 ( gameState.mode == CAMEO_MODE ) )
	{
		ProcessShadows();
		
		if( specFXList.numEntries )
		{
			SPECFX *fx;
			for( fx = specFXList.head.next; fx != &specFXList.head; fx = fx->next )
				if( fx->Draw )
					fx->Draw( fx );
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function		: ProcessShadows
	Purpose			: processes the shadows
	Parameters		: 
	Returns			: void
	Info			: 
*/
long tex;

void ProcessShadows()
{
	TEXTURE *theTexture;
	VECTOR vec;
	ENEMY *nme,*nme2;
	PLATFORM *plat;
	GARIB *garib;
	int i;
	TEXENTRY *tEntry;
	
	FindTexture(&theTexture,UpdateCRC("ai_circle.bmp"),YES);
	tEntry = ((TEXENTRY *)theTexture);
	tex = (long)tEntry->hdl;	

//	dprintf"%x",tex));
	if(frog[0]->actor->shadow)
	{
		vec.v[X] = frog[0]->actor->pos.v[X];
		vec.v[Y] = currTile[0]->centre.v[Y] + 1;
		vec.v[Z] = frog[0]->actor->pos.v[Z];
		DrawShadow(&vec,NULL,frog[0]->actor->shadow->radius,0,frog[0]->actor->shadow->alpha,frog[0]->actor->shadow->vert,NULL,0,0);
	}

	//------------------------------------------------------------------------------------------------

	i = numBabies;
	while(i--)
	{
		if ( babies[i] )
		{
			if(!babies[i]->action.isSaved)
			{
				if(babies[i]->actor->shadow && babies[i]->distanceFromFrog < ACTOR_DRAWDISTANCEINNER)
				{
					if ( bTStart[i] )
					{
						SetVector( &vec, &babies[i]->actor->pos );
					}
					else
					{
						vec.v[X] = 0;
						vec.v[Y] = 1;
						vec.v[Z] = 0;
					}
					// ENDIF
					DrawShadow(&vec,NULL,babies[i]->actor->shadow->radius,0,babies[i]->actor->shadow->alpha,babies[i]->actor->shadow->vert,NULL,0,0);
				}
			}
		}
		// ENDIF
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
			DrawShadow(&vec,NULL,nme->nmeActor->actor->shadow->radius,0,nme->nmeActor->actor->shadow->alpha,nme->nmeActor->actor->shadow->vert,NULL,0,0);
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
			DrawShadow(&vec,NULL,garib->shadow.radius,0,garib->shadow.alpha,garib->shadow.vert,NULL,0,0);
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

float sscale = 1.2;

void DrawShadow(VECTOR *pos,PLANE *plane,float size,float altitude,short alph,Vtx *vert,VECTOR *lightDir, float tu, float tv)
{
	VECTOR tempVect[4],m[4];
	D3DTLVERTEX vT[4];
	short f[6] = {0,1,2,0,2,3};
	long i;

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZENABLE,1);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,0);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_LINEAR);

	tempVect[0].v[X] = pos->v[X]-size*sscale;
	tempVect[0].v[Y] = pos->v[Y];
	tempVect[0].v[Z] = pos->v[Z]-size*sscale;

	tempVect[1].v[X] = pos->v[X]+size*sscale;
	tempVect[1].v[Y] = pos->v[Y];
	tempVect[1].v[Z] = pos->v[Z]-size*sscale;

	tempVect[2].v[X] = pos->v[X]+size*sscale;
	tempVect[2].v[Y] = pos->v[Y];
	tempVect[2].v[Z] = pos->v[Z]+size*sscale;

	tempVect[3].v[X] = pos->v[X]-size*sscale;
	tempVect[3].v[Y] = pos->v[Y];
	tempVect[3].v[Z] = pos->v[Z]+size*sscale;

	for(i=0; i<4; i++)
		XfmPoint (&m[i],&tempVect[i]);
	
	if (m[0].v[Z])
	if (m[1].v[Z])
	if (m[2].v[Z])
	if (m[3].v[Z])
	{
		vT[0].sx = m[1].v[X];
		vT[0].sy = m[1].v[Y];
		vT[0].sz = (m[1].v[Z]+DIST)/2000;///2000;
		vT[0].tu = tu+1;
		vT[0].tv = tv;
		vT[0].color = D3DRGBA(0,0,0,1);
		vT[0].specular = D3DRGB(0,0,0);

		vT[1].sx = m[0].v[X];
		vT[1].sy = m[0].v[Y];
		vT[1].sz = (m[0].v[Z]+DIST)/2000;///2000;
		vT[1].tu = tu;
		vT[1].tv = tv;
		vT[1].color = D3DRGBA(0,0,0,1);
		vT[1].specular = D3DRGB(0,0,0);

		vT[2].sx = m[2].v[X];
		vT[2].sy = m[2].v[Y];
		vT[2].sz = (m[2].v[Z]+DIST)/2000;///2000;
		vT[2].tu = tu+1;
		vT[2].tv = tv+1;
		vT[2].color = D3DRGBA(0,0,0,1);
		vT[2].specular = 0;
		vT[2].specular = D3DRGB(0,0,0);
		
		vT[3].sx = m[3].v[X];
		vT[3].sy = m[3].v[Y];
		vT[3].sz = (m[3].v[Z]+DIST)/2000;///2000;
		vT[3].tu = tu;
		vT[3].tv = tv+1;
		vT[3].color = D3DRGBA(0,0,0,1);
		vT[3].specular = 0;
		vT[3].specular = D3DRGB(0,0,0);

		if (runHardware)
		{
			//Clip3DPolygon (&vT[0],tex);
			//Clip3DPolygon (&vT[1],tex);
		}
	}

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,1);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);
}


/*	--------------------------------------------------------------------------------
	Function		: DrawTongue
	Purpose			: er, draw Frogger's tongue.....
	Parameters		:
	Returns			:
	Info			:
*/

void DrawTongue()
{
/*
	Vtx *tongueVtxPtr	= NULL;
	Vtx *tongueVtxPtr2	= NULL;
	unsigned long i = 0,tv = 0;
	int x,y,z;
	
	if(tongueSegment < 1)
		return;

	gDPPipeSync(glistp++);
	gSPClearGeometryMode(glistp++,G_CULL_BACK);

	tongueVtxPtr	= &tongueVtx[0];
	tongueVtxPtr2	= &tongueVtx2[0];
	for(i=0; i<tongueSegment-1; i++)
	{
		x = tongueCoords[i].v[X] + (2 * fu.v[X]);
		y = tongueCoords[i].v[Y] + (2 * fu.v[Y]);
		z = tongueCoords[i].v[Z] + (2 * fu.v[Z]);
		V((tongueVtxPtr),x,y,z,0,1024,1024,255,0,0,255);
		tongueVtxPtr++;

		x = tongueCoords[i].v[X] - (2 * fu.v[X]);
		y = tongueCoords[i].v[Y] - (2 * fu.v[Y]);
		z = tongueCoords[i].v[Z] - (2 * fu.v[Z]);
		V((tongueVtxPtr),x,y,z,0,1024,1024,255,0,0,255);
		tongueVtxPtr++;

		x = tongueCoords[i+1].v[X] + (2 * fu.v[X]);
		y = tongueCoords[i+1].v[Y] + (2 * fu.v[Y]);
		z = tongueCoords[i+1].v[Z] + (2 * fu.v[Z]);
		V((tongueVtxPtr),x,y,z,0,1024,1024,255,0,0,255);
		tongueVtxPtr++;

		x = tongueCoords[i+1].v[X] - (2 * fu.v[X]);
		y = tongueCoords[i+1].v[Y] - (2 * fu.v[Y]);
		z = tongueCoords[i+1].v[Z] - (2 * fu.v[Z]);
		V((tongueVtxPtr),x,y,z,0,1024,1024,255,0,0,255);
		tongueVtxPtr++;

//-----------------------------------------

		x = tongueCoords[i].v[X] + (2 * fr.v[X]);
		y = tongueCoords[i].v[Y] + (2 * fr.v[Y]);
		z = tongueCoords[i].v[Z] + (2 * fr.v[Z]);
		V((tongueVtxPtr2),x,y,z,0,1024,1024,255,0,0,255);
		tongueVtxPtr2++;

		x = tongueCoords[i].v[X] - (2 * fr.v[X]);
		y = tongueCoords[i].v[Y] - (2 * fr.v[Y]);
		z = tongueCoords[i].v[Z] - (2 * fr.v[Z]);
		V((tongueVtxPtr2),x,y,z,0,1024,1024,255,0,0,255);
		tongueVtxPtr2++;

		x = tongueCoords[i+1].v[X] + (2 * fr.v[X]);
		y = tongueCoords[i+1].v[Y] + (2 * fr.v[Y]);
		z = tongueCoords[i+1].v[Z] + (2 * fr.v[Z]);
		V((tongueVtxPtr2),x,y,z,0,1024,1024,255,0,0,255);
		tongueVtxPtr2++;

		x = tongueCoords[i+1].v[X] - (2 * fr.v[X]);
		y = tongueCoords[i+1].v[Y] - (2 * fr.v[Y]);
		z = tongueCoords[i+1].v[Z] - (2 * fr.v[Z]);
		V((tongueVtxPtr2),x,y,z,0,1024,1024,255,0,0,255);
		tongueVtxPtr2++;

//-----------------------------------------

		gSPVertex(glistp++,&(tongueVtx[tv]),4,0);
		gSP2Triangles(glistp++,0,1,2,0,0,3,2,0);

		gSPVertex(glistp++,&(tongueVtx2[tv]),4,0);
		gSP2Triangles(glistp++,0,1,2,0,0,3,2,0);

		tv += 4;
	}

	gDPPipeSync(glistp++);
	gSPSetGeometryMode(glistp++,G_CULL_BACK);
*/
}




//----- [ GLOBALS ] ----------------------------------------------------------------------------//

//BACKDROPLIST backdropList;


//----- [ BACKDROP HANDLING ] ------------------------------------------------------------------//


/*	--------------------------------------------------------------------------------
	Function 	: InitBackdropLinkedList
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitBackdropLinkedList()
{
//	backdropList.head.next = backdropList.head.prev = &backdropList.head;
//	backdropList.numEntries = 0;
}

/*	--------------------------------------------------------------------------------
	Function 	: AddBackdrop()
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void AddBackdrop(BACKDROP *backdrop)
{
/*	BACKDROP *ptr;	 

	if(backdrop->next == NULL)
	{
		for(ptr = backdropList.head.next;ptr != &backdropList.head;ptr = ptr->next)
		{
			if((ptr->zPos < backdrop->zPos) || ((ptr->texture == backdrop->texture) && (ptr->texture->format == backdrop->texture->format) && (ptr->zPos == backdrop->zPos)))
			{
				break;
			}
		}
		backdrop->next = ptr;
		backdrop->prev = ptr->prev;
		ptr->prev->next = backdrop;
		ptr->prev = backdrop;
		backdropList.numEntries++;
		backdrop->draw = 1;
	}*/
}

/*	--------------------------------------------------------------------------------
	Function 	: SubBackdrop
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void SubBackdrop(BACKDROP *backdrop)
{
/*	if(backdrop->next == NULL)
		return;

	backdrop->prev->next = backdrop->next;
	backdrop->next->prev = backdrop->prev;

	backdrop->next = NULL;
	backdropList.numEntries--;*/
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void FreeBackdropLinkedList()
{
/*	BACKDROP *cur,*next;	 

	if(backdropList.numEntries == 0)
		return;

	dprintf"Freeing linked list : BACKDROP : (%d elements)\n",backdropList.numEntries));
	for(cur = backdropList.head.next; cur != &backdropList.head; cur = next)
	{
		next = cur->next;

		SubBackdrop(cur);
	}*/
}

/*	--------------------------------------------------------------------------------
	Function 	: SetupBackdrop
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
BACKDROP *SetupBackdrop(BACKDROP *backdrop,int texID,int sourceX,int sourceY,int z,int destX,int destY,int destWidth,int destHeight,int scalex,int scaley,int flags)
{
	if(backdrop == NULL)
		backdrop = (BACKDROP *)JallocAlloc(sizeof(BACKDROP),YES,"backdrop");
	backdrop->scaleX = scalex;
	backdrop->scaleY = scaley;
	backdrop->xPos = sourceX;
	backdrop->yPos = sourceY;
//	backdrop->zPos = z;
	backdrop->draw = 1;
//	backdrop->flags = flags;
	backdrop->r = backdrop->g = backdrop->b = backdrop->a = 255;
	FindTexture(&backdrop->texture,texID,YES);
/*
	backdrop->background.s.imageX = sourceX<<5;
	backdrop->background.s.imageY = sourceY<<5;
	backdrop->background.s.imageW = (backdrop->texture->sx+1)<<2;
	backdrop->background.s.imageH = backdrop->texture->sy<<2;
	backdrop->background.s.frameX = destX<<2;
	backdrop->background.s.frameY = destY<<2;
	backdrop->background.s.frameW = destWidth<<2;
	backdrop->background.s.frameH = destHeight<<2;
	backdrop->background.s.imagePtr = (u64*)backdrop->texture->data;
	backdrop->background.s.imageLoad = G_BGLT_LOADTILE;
	backdrop->background.s.imageFmt = backdrop->texture->format;
	backdrop->background.s.imageSiz = backdrop->texture->pixSize;
	backdrop->background.s.imagePal = 0;
	backdrop->background.s.imageFlip = 0;
	backdrop->background.s.scaleW = (1024*1024)/scalex;
	backdrop->background.s.scaleH = (1024*1024)/scaley;
	backdrop->background.s.imageYorig = 0<<5;

	osWritebackDCache(&backdrop->background, sizeof(uObjBg));
*/
	AddBackdrop(backdrop);

	return backdrop;
}


void DrawFXRipple( SPECFX *ripple )
{
	TEXENTRY *tEntry;
	long i;
	D3DTLVERTEX vT[4], vT2[3];
	VECTOR tempVect[4], m[4];
	static short f[6] = {0,1,2,0,2,3};
	QUATERNION q;

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZENABLE,1);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,0);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_LINEAR);
	
	if(ripple->deadCount)
		return;

	tempVect[0].v[X] = -ripple->size;
	tempVect[0].v[Y] = 0;
	tempVect[0].v[Z] = ripple->size;

	tempVect[1].v[X] = ripple->size;
	tempVect[1].v[Y] = 0;
	tempVect[1].v[Z] = ripple->size;

	tempVect[2].v[X] = ripple->size;
	tempVect[2].v[Y] = 0;
	tempVect[2].v[Z] = -ripple->size;

	tempVect[3].v[X] = -ripple->size;
	tempVect[3].v[Y] = 0;
	tempVect[3].v[Z] = -ripple->size;

	if(	ripple->type == FXTYPE_GARIBCOLLECT )
	{
		SetVector( (VECTOR *)&q, &ripple->normal );
		q.w = ripple->angle;

		for( i=0; i<4; i++ )
			RotateVectorByRotation( &tempVect[i], &tempVect[i], &q );
	}

	for(i=0; i<4; i++)
	{
		AddToVector( &tempVect[i], &ripple->origin );
		XfmPoint (&m[i],&tempVect[i]);
	}
	
	if (m[0].v[Z])
	if (m[1].v[Z])
	if (m[2].v[Z])
	if (m[3].v[Z])
	{

		vT[0].sx = m[1].v[X];
		vT[0].sy = m[1].v[Y];
		vT[0].sz = (m[1].v[Z]+DIST)/2000;
		vT[0].tu = 0;
		vT[0].tv = 0;
		vT[0].color = D3DRGBA(ripple->r/256.0,ripple->g/256.0,ripple->b/256.0,ripple->a/256.0);
		vT[0].specular = D3DRGB(0,0,0);

		vT[1].sx = m[0].v[X];
		vT[1].sy = m[0].v[Y];
		vT[1].sz = (m[0].v[Z]+DIST)/2000;
		vT[1].tu = 1;
		vT[1].tv = 0;
		vT[1].color = vT[0].color;
		vT[1].specular = D3DRGB(0,0,0);

		vT[2].sx = m[2].v[X];
		vT[2].sy = m[2].v[Y];
		vT[2].sz = (m[2].v[Z]+DIST)/2000;
		vT[2].tu = 0;
		vT[2].tv = 1;
		vT[2].color = vT[0].color;
		vT[2].specular = D3DRGB(0,0,0);

		vT[3].sx = m[3].v[X];
		vT[3].sy = m[3].v[Y];
		vT[3].sz = (m[3].v[Z]+DIST)/2000;
		vT[3].tu = 1;
		vT[3].tv = 1;
		vT[3].color = vT[0].color;
		vT[3].specular = D3DRGB(0,0,0);

		tEntry = ((TEXENTRY *)ripple->tex);
		if (tEntry)
		{
			memcpy( &vT2[0], &vT[2], sizeof(D3DTLVERTEX) );
			memcpy( &vT2[1], &vT[1], sizeof(D3DTLVERTEX) );
			memcpy( &vT2[2], &vT[3], sizeof(D3DTLVERTEX) );
			Clip3DPolygon( vT, tEntry->hdl );
			Clip3DPolygon( vT2, tEntry->hdl );
		}
	}

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,1);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);
}
