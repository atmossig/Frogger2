/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: pcsprite.c
	Programmer	: Jim
	Date		: 16/03/00
	Purpose		: Sprite sorting and drawing functions

----------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <mdx.h>

#include <islmem.h>

#include "frogger.h"
#include "sprite.h"
#include "types2d.h"
#include "overlays.h"
#include "Main.h"
#include "pcmisc.h"
#include "pcsprite.h"
#include "layout.h"


//----- [ PRINTING STUFF ] -----------------------------------------------------------------------

#define SPRITECLIPLEFT		10
#define SPRITECLIPTOP		10
#define SPRITECLIPRIGHT		630
#define SPRITECLIPBOTTOM	470

#define ROTSPRITECLIPLEFT	0
#define ROTSPRITECLIPTOP	0
#define ROTSPRITECLIPRIGHT	640
#define ROTSPRITECLIPBOTTOM	480

POLYCLIP tempPoly;

void SpriteClip_Do(POLYCLIP *polyIn,POLYCLIP *polyOut);
void SpriteClip_Left(POLYCLIP *poly,D3DTLVERTEX *v0,D3DTLVERTEX *v1);
void SpriteClip_Right(POLYCLIP *poly,D3DTLVERTEX *v0,D3DTLVERTEX *v1);
void SpriteClip_Top(POLYCLIP *poly,D3DTLVERTEX *v0,D3DTLVERTEX *v1);
void SpriteClip_Bottom(POLYCLIP *poly,D3DTLVERTEX *v0,D3DTLVERTEX *v1);

int numSprites;

void AddObjectsSpritesToSpriteList(MDX_OBJECT *obj,short flags)
{
	SPRITE *sprite;
	int i;

	if(obj->numSprites > 0)
	{
		for(i=0; i<obj->numSprites; i++)
		{
			sprite = AllocateSprites( 1 );

			sprite->texture = (TextureType *)obj->sprites[i].textureID;
			SetVectorSV(&sprite->pos,&zero);

			if((obj->sprites[i].flags & SPRITE_DONE) == 0)
				obj->sprites[i].flags |= SPRITE_DONE;

			sprite->r = sprite->g = sprite->b = 255;
			sprite->a = 128;
			if(obj->xlu < 255)
				sprite->flags = SPRITE_TRANSLUCENT;
			else
				sprite->flags = 0;

			sprite->flags |= flags;

			sprite->offsetX = -32 / 2;
			sprite->offsetY = -32 / 2;

			//AddSprite(sprite,NULL);
			obj->sprites[i].sprite = (MDX_SPRITE *)sprite;
		}
	}

	if(obj->children)
		AddObjectsSpritesToSpriteList(obj->children,flags);

	if(obj->next)
		AddObjectsSpritesToSpriteList(obj->next,flags);
}
/*	--------------------------------------------------------------------------------
    Function		: PrintSprites
	Parameters		: 
	Returns			: 
	Purpose			: Sort and draw all sprites
*/
void PrintSprites()
{
	int i;
	SPRITE *cur;
	MDX_VECTOR tVect;
	
	// transform sprites to screen coords ready for sorting
	for(cur = sprList.head.next; cur != &sprList.head && numSortArraySprites < MAX_ARRAY_SPRITES; cur = cur->next)
	{
		SetVectorRS( &tVect, &cur->pos );
		ScaleVector( &tVect, 0.1 );

		XfmPoint(&cur->sc,&tVect,NULL);
	}
	ZSortSpriteList();

	// draw from the newly sorted static array
	i = numSortArraySprites;
	while(i--) if( spriteSortArray[i].draw ) 
		PrintSprite(&spriteSortArray[i]);
}


/*	--------------------------------------------------------------------------------
    Function		: PrintSpriteOverlays
	Parameters		: 
	Returns			: 
	Purpose			: Draw all sprite overlays
*/
void PrintSpriteOverlays(long num)
{
	SPRITEOVERLAY *cur;
	short x,y;
	MDX_TEXENTRY *tEntry;
	float gs = gameSpeed>>12;

	if( !drawOverlays || !spriteOverlayList.numEntries ) return;

	cur = spriteOverlayList.head.next;
	while(cur!=&spriteOverlayList.head)
	{
		if (cur->num!=num)
		{
			cur = cur->next;
			continue;
		}

		cur->xPos = cur->xPosTo;
		cur->yPos = cur->yPosTo;

		// update the sprite animation if an animated sprite overlay
		if(cur->draw)
		{
			// Go to destination, if specified
			float spd = (cur->speed>>12) * gs;
			
			if (Fabs(spd)>0)
			{
				if (Fabs(cur->xPosTo-cur->xPos) > 0)
				{
					cur->speed += ToFixed(gs*0.2);
					cur->xPos += (cur->xPosTo > cur->xPos)?spd:-spd;
					
					if( Fabs(cur->xPosTo-cur->xPos) < Fabs(spd) )
					{
						if (fabs(cur->speed)>2)
							cur->speed = -cur->speed * 0.3;
						else
						{
							cur->speed = 0;
							cur->xPos = cur->xPosTo;
						}

						spd = cur->speed * gs;
					}
			
					
				}
				else
					if (Fabs(cur->yPosTo-cur->yPos) > 0)
					{
						cur->yPos += (cur->yPosTo > cur->yPos)?spd:-spd;
						cur->speed += gameSpeed*0.2;					

						if( Fabs(cur->yPosTo-cur->yPos) < Fabs(spd) )
						{
							if (fabs(cur->speed)>2)
							{
								cur->speed = -cur->speed * 0.3;
							}
							else
							{
								cur->speed = 0;
								cur->yPos = cur->yPosTo;
							}

							spd = cur->speed * gameSpeed;
						}
				
						
					}
			}
			
			//if( Fabs(cur->yPosTo-cur->yPos) < spd )
			//	cur->yPos = cur->yPosTo;
			//else
			//	cur->yPos += (cur->yPosTo > cur->yPos)?spd:-spd;
		


			// check animation playback type - cycle,ping-pong, etc.
			if(cur->flags & ANIMATION_CYCLE)
			{
				// sprite is cycling through frames - check if cycling forwards or backwards
				if(cur->flags & ANIMATION_FORWARDS)
				{
					// cycling forwards
					cur->currFrame = (short)cur->animTime;
					cur->animTime += cur->animSpeed * 0.4 * gameSpeed;
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
					cur->animTime -= cur->animSpeed * 0.4 * gameSpeed;
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
			
			if( !(tEntry = ((MDX_TEXENTRY *)cur->frames[cur->currFrame])) )
			{
				cur = cur->next;
				continue;
			}
			
			numSprites++;
			if (cur->flags & SPRITE_ADDITIVE)
			{
				pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
				pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_ONE);
			}

			DrawAlphaSprite( cur->xPos*OVERLAY_X, cur->yPos*OVERLAY_Y, 0, cur->width*OVERLAY_X, cur->height*OVERLAY_Y, 0, 0, 1, 1, 
				tEntry->surf,D3DRGBA(cur->r/255.0,cur->g/255.0,cur->b/255.0,cur->a/255.0) );

			if (cur->flags & SPRITE_ADDITIVE)
			{
				pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
				pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
			}
		}

		cur = cur->next;
	}
}


/*	--------------------------------------------------------------------------------
    Function		: PrintSprite
	Parameters		: Sprite
	Returns			: 
	Purpose			: Choose which sprite drawing function to use and set render states
*/
void PrintSprite(SPRITE *sprite)
{
	MDX_VECTOR m, sc;
	MDX_VECTOR s = {1,1,0};
	float distx,disty;
	MDX_TEXENTRY *tEntry;

	if((!sprite->texture) || (sprite->scaleX == 0) || (sprite->scaleY == 0))
		return;

	SetVector( &sc, (MDX_VECTOR *)&sprite->sc );
	
	tEntry = ((MDX_TEXENTRY *)sprite->texture);

	if( sc.vz )
	{
		distx = disty = (FOV)/(sc.vz+DIST);
		distx *= (sprite->scaleX/(64.0));
		disty *= (sprite->scaleY/(64.0));
		numSprites++;

		if (sprite->flags & SPRITE_ADDITIVE)
		{
			pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
			pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_ONE);
		}

/*		if(sprite->flags & SPRITE_FLAGS_ROTATE)
		{
			DrawAlphaSpriteRotating( &sc,(float)sprite->angle/57.6,sc.vx+sprite->offsetX*distx,sc.vy+sprite->offsetY*disty,sc.vz*0.00025,32*distx,32*disty,
				0,0,1,1,tEntry->surf,D3DRGBA(sprite->r/255.0,sprite->g/255.0,sprite->b/255.0,sprite->a/255.0) );
		}
		else*/
		{
			DrawAlphaSprite(sc.vx+sprite->offsetX*distx,sc.vy+sprite->offsetY*disty,sc.vz*0.00025,32*distx,32*disty,
				0,0,1,1,tEntry->surf,D3DRGBA(sprite->r/255.0,sprite->g/255.0,sprite->b/255.0,sprite->a/255.0) );
		}

		if (sprite->flags & SPRITE_ADDITIVE)
		{
			pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
			pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
		}
	}
}


/*	--------------------------------------------------------------------------------
    Function		: DrawALine
	Parameters		: point1, point2, colour
	Returns			: 
	Purpose			: Draw a line in screen space
*/
void DrawALine (float x1, float y1, float x2, float y2, D3DCOLOR color)
{
	D3DTLVERTEX v[2] = {
		{
			x1,y1,0,0,
			color, D3DRGBA(0,0,0,1.0),
			0.0, 0.0
		},
		{
			x2,y2,0,0,
			color, D3DRGBA(0,0,0,1.0),
			0.0, 0.0
		}
	};

	if (pDirect3DDevice->lpVtbl->DrawPrimitive(pDirect3DDevice,
		D3DPT_LINESTRIP,
		D3DFVF_TLVERTEX,
		v,
		2,
		D3DDP_DONOTCLIP 
			| D3DDP_DONOTLIGHT 
			| D3DDP_DONOTUPDATEEXTENTS 
			/*| D3DDP_WAIT*/)!=D3D_OK)
	{
		dp("COULDN'T DRAW LINE");
	}
}


/*	--------------------------------------------------------------------------------
    Function		: DrawASprite
	Parameters		: 
	Returns			: 
	Purpose			: Draw a non-alpha, non-rotating sprite
*/
void DrawASprite(float x, float y, float xs, float ys, float u1, float v1, float u2, float v2, LPDIRECTDRAWSURFACE7 h,DWORD colour)
{
	float x2 = (x+xs), y2 = (y+ys);
	D3DTLVERTEX v[4];

	if (x < SPRITECLIPLEFT)
	{
		if (x2 < SPRITECLIPLEFT) return;
		u1 += (u2-u1) * (SPRITECLIPLEFT-x)/xs;	// clip u
		xs += x-SPRITECLIPLEFT; x = SPRITECLIPLEFT;
	}
	if (x2 > SPRITECLIPRIGHT)
	{
		if (x > SPRITECLIPRIGHT) return;
		u2 += (u2-u1) * (SPRITECLIPRIGHT-x2)/xs;	// clip u
		xs -= (x-SPRITECLIPRIGHT);
		x2 = SPRITECLIPRIGHT;
	}

	if (y < SPRITECLIPTOP)
	{
		if (y2 < SPRITECLIPTOP) return;
		v1 += (v2-v1) * (SPRITECLIPTOP-y)/ys;	// clip v
		ys += y-SPRITECLIPTOP; y = SPRITECLIPTOP;
	}
	if (y2 > SPRITECLIPBOTTOM)
	{
		if (y > SPRITECLIPBOTTOM) return;
		v2 += (v2-v1) * (SPRITECLIPBOTTOM-y2)/ys;	// clip v
		ys -= (y-SPRITECLIPBOTTOM);
		y2 = SPRITECLIPBOTTOM;
	}
	
	v[0].sx = x; v[0].sy = y; v[0].sz = 0; v[0].rhw = 0;
	v[0].color = colour; v[0].specular = D3DRGBA(0,0,0,1);
	v[0].tu = u1; v[0].tv = v1;

	v[1].sx = x2; v[1].sy = y; v[1].sz = 0; v[1].rhw = 0;
	v[1].color = v[0].color; v[1].specular = v[0].specular;
	v[1].tu = u2; v[1].tv = v1;
	
	v[2].sx = x2; v[2].sy = y2; v[2].sz = 0; v[2].rhw = 0;
	v[2].color = v[0].color; v[2].specular = v[0].specular;
	v[2].tu = u2; v[2].tv = v2;

	v[3].sx = x; v[3].sy = y2; v[3].sz = 0; v[3].rhw = 0;
	v[3].color = v[0].color; v[3].specular = v[0].specular;
	v[3].tu = u1; v[3].tv = v2;

	pDirect3DDevice->lpVtbl->SetTexture(pDirect3DDevice,0,h);

	if (pDirect3DDevice->lpVtbl->DrawPrimitive(pDirect3DDevice,
		D3DPT_TRIANGLEFAN,
		D3DFVF_TLVERTEX,
		v,
		4,
		D3DDP_DONOTCLIP 
			| D3DDP_DONOTLIGHT 
			| D3DDP_DONOTUPDATEEXTENTS 
			/*| D3DDP_WAIT*/)!=D3D_OK)
	{
		dp("Could not print sprite\n");
		// BUGGER !!!!! CAN'T DRAW POLY JOBBY !
	}

	pDirect3DDevice->lpVtbl->SetTexture(pDirect3DDevice,0,0);
}


/*	--------------------------------------------------------------------------------
    Function		: DrawAlphaSprite
	Parameters		: 
	Returns			: 
	Purpose			: Draw an alpha-ed, non-rotating sprite
*/
void DrawAlphaSprite (float x, float y, float z, float xs, float ys, float u1, float v1, float u2, float v2, LPDIRECTDRAWSURFACE7 h, DWORD colour )
{
	D3DTLVERTEX v[4];
	float x2 = (x+xs), y2 = (y+ys);
//	float fogAmt;

	if (x < SPRITECLIPLEFT)
	{
		if (x2 < SPRITECLIPLEFT) return;
		u1 += (u2-u1) * (SPRITECLIPLEFT-x)/xs;	// clip u
		xs += x-SPRITECLIPLEFT; x = SPRITECLIPLEFT;
	}
	if (x2 > SPRITECLIPRIGHT)
	{
		if (x > SPRITECLIPRIGHT) return;
		u2 += (u2-u1) * (SPRITECLIPRIGHT-x2)/xs;	// clip u
		xs -= (x-SPRITECLIPRIGHT);
		x2 = SPRITECLIPRIGHT;
	}

	if (y < SPRITECLIPTOP)
	{
		if (y2 < SPRITECLIPTOP) return;
		v1 += (v2-v1) * (SPRITECLIPTOP-y)/ys;	// clip v
		ys += y-SPRITECLIPTOP; y = SPRITECLIPTOP;
	}
	if (y2 > SPRITECLIPBOTTOM)
	{
		if (y > SPRITECLIPBOTTOM) return;
		v2 += (v2-v1) * (SPRITECLIPBOTTOM-y2)/ys;	// clip v
		ys -= (y-SPRITECLIPBOTTOM);
		y2 = SPRITECLIPBOTTOM;
	}
	
/*	fogAmt = FOGADJ(z);
	if (fogAmt<0)
		fogAmt=0;
	if (fogAmt>1)
		fogAmt=1;
*/	
	v[0].sx = x; v[0].sy = y; v[0].sz = z; v[0].rhw = 0;
	v[0].color = colour; v[0].specular = D3DRGBA(0,0,0,1);
	v[0].tu = u1; v[0].tv = v1;

	v[1].sx = x2; v[1].sy = y; v[1].sz = z; v[1].rhw = 0;
	v[1].color = v[0].color; v[1].specular = v[0].specular;
	v[1].tu = u2; v[1].tv = v1;
	
	v[2].sx = x2; v[2].sy = y2; v[2].sz = z; v[2].rhw = 0;
	v[2].color = v[0].color; v[2].specular = v[0].specular;
	v[2].tu = u2; v[2].tv = v2;

	v[3].sx = x; v[3].sy = y2; v[3].sz = z; v[3].rhw = 0;
	v[3].color = v[0].color; v[3].specular = v[0].specular;
	v[3].tu = u1; v[3].tv = v2;

	if (v[0].sx>1000)
		dp("p{%f,%f   %f,%f   %f,%f   %f,%f}\n",v[0].sx,v[0].sy,v[1].sx,v[1].sy,v[2].sx,v[2].sy,v[3].sx,v[3].sy);
	pDirect3DDevice->lpVtbl->SetTexture(pDirect3DDevice,0,h);

	if ((z>0.01) || (z<-0.01))
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZENABLE,1);
	else
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZENABLE,0);

	if (pDirect3DDevice->lpVtbl->DrawPrimitive(pDirect3DDevice,
		D3DPT_TRIANGLEFAN,
		D3DFVF_TLVERTEX,
		v,
		4,
		D3DDP_DONOTCLIP 
			| D3DDP_DONOTLIGHT 
			| D3DDP_DONOTUPDATEEXTENTS 
			/*| D3DDP_WAIT*/)!=D3D_OK)
	{
		dp("Could not print sprite\n");
		// BUGGER !!!!! CAN'T DRAW POLY JOBBY !
	}

	pDirect3DDevice->lpVtbl->SetTexture(pDirect3DDevice,0,0);
}


/*	--------------------------------------------------------------------------------
	Function		: DrawAlphaSpriteRotating
	Purpose			: clips and draws a rotating sprite
	Parameters		: 
	Returns			: void
	Info			: 
*/
void DrawAlphaSpriteRotating(MDX_VECTOR *pos,float angle,float x, float y, float z, float xs, float ys, float u1, float v1, float u2, float v2, LPDIRECTDRAWSURFACE7 h, DWORD colour )
{
	POLYCLIP p2d,drawPoly;
	float x2 = (x+xs), y2 = (y+ys);
//	float fogAmt;

	float sine,cosine;
	float newX,newY;
	int i,j;
/*
	fogAmt = FOGADJ(z);
	if (fogAmt<0)
		fogAmt=0;
	if (fogAmt>1)
		fogAmt=1;
*/
	// populate our structure ready for transforming and clipping the sprite
	p2d.numVerts = 4;
	p2d.verts[0].sx = x - pos->vx;
	p2d.verts[1].sx = x2 - pos->vx;
	p2d.verts[2].sx = x2 - pos->vx;
	p2d.verts[3].sx = x - pos->vx;

	p2d.verts[0].sy = y - pos->vy;
	p2d.verts[1].sy = y - pos->vy;
	p2d.verts[2].sy = y2 - pos->vy;
	p2d.verts[3].sy = y2 - pos->vy;

	p2d.verts[0].tu = u1;	p2d.verts[0].tv = v1;
	p2d.verts[1].tu = u2;	p2d.verts[1].tv = v1;
	p2d.verts[2].tu = u2;	p2d.verts[2].tv = v2;
	p2d.verts[3].tu = u1;	p2d.verts[3].tv = v2;

	// populate remaining data members
	i = p2d.numVerts;
	while(i--)
	{
		p2d.verts[i].sz			= z;
		p2d.verts[i].rhw		= 0;
		p2d.verts[i].color		= colour;
		p2d.verts[i].specular	= D3DRGBA(0,0,0,1);//FOGVAL(fogAmt);
	}

	// get rotation angle
	cosine	= cosf(angle);
	sine	= sinf(angle);

	// rotate the vertices comprising the sprite
	i = p2d.numVerts;
	while(i--)
	{
		newX = (p2d.verts[i].sx * cosine) + (p2d.verts[i].sy * sine);
		newY = (p2d.verts[i].sy * cosine) - (p2d.verts[i].sx * sine);

		p2d.verts[i].sx = newX + pos->vx;
		p2d.verts[i].sy = newY + pos->vy;
	}

	// clip the rotated sprite here...
	SpriteClip_Do(&p2d,&drawPoly);

	// return if we have less than 3 vertices
	if(drawPoly.numVerts < 3)
		return;

	pDirect3DDevice->lpVtbl->SetTexture(pDirect3DDevice,0,h);

	if ((z>0.01) || (z<-0.01))
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZENABLE,1);
	else
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZENABLE,0);

	if (pDirect3DDevice->lpVtbl->DrawPrimitive(pDirect3DDevice,
		D3DPT_TRIANGLEFAN,
		D3DFVF_TLVERTEX,

		drawPoly.verts,
		drawPoly.numVerts,

		D3DDP_DONOTCLIP | D3DDP_DONOTLIGHT | D3DDP_DONOTUPDATEEXTENTS 
		)!=D3D_OK)
	{
		dp("Poo-poo!\n");
	}

	pDirect3DDevice->lpVtbl->SetTexture(pDirect3DDevice,0,0);

	// for testing...this draws the wireframe outline of the sprite polygon ---------------------
/*
	for(i=0; i<drawPoly.numVerts; i++)
	{
		j = i + 1;
		if(j == drawPoly.numVerts)
			j = 0;

		DrawALine(drawPoly.verts[i].sx,drawPoly.verts[i].sy,drawPoly.verts[j].sx,drawPoly.verts[j].sy,0xffffffff);
	}
*/
	//-------------------------------------------------------------------------------------------
}

// use Sutherland - Hodgman edge clipping algorithm thingyjob - ANDYE

void SpriteClip_Do(POLYCLIP *polyIn,POLYCLIP *polyOut)
{
	int v,d;

	polyOut->numVerts = 0;
	tempPoly.numVerts = 0;

	for(v=0; v<polyIn->numVerts; v++)
	{
		d = v + 1;
		if(d == polyIn->numVerts) d = 0;
		SpriteClip_Left(&tempPoly,&polyIn->verts[v],&polyIn->verts[d]);
	}

	for(v=0; v<tempPoly.numVerts; v++)
	{
		d = v + 1;
		if(d == tempPoly.numVerts) d = 0;
		SpriteClip_Right(polyOut,&tempPoly.verts[v],&tempPoly.verts[d]);
	}

	tempPoly.numVerts = 0;
	for(v=0; v<polyOut->numVerts; v++)
	{
		d = v + 1;
		if(d == polyOut->numVerts) d = 0;
		SpriteClip_Top(&tempPoly,&polyOut->verts[v],&polyOut->verts[d]);
	}

	polyOut->numVerts = 0;
	for(v=0; v<tempPoly.numVerts; v++)
	{
		d = v + 1;
		if(d == tempPoly.numVerts) d = 0;
		SpriteClip_Bottom(polyOut,&tempPoly.verts[v],&tempPoly.verts[d]);
	}
}

void SpriteClip_Left(POLYCLIP *poly,D3DTLVERTEX *v0,D3DTLVERTEX *v1)
{
	float dx,dy,m,segLen;
	float du,dv;

	dx		= v1->sx - v0->sx;
	dy		= v1->sy - v0->sy;
	du		= v1->tu - v0->tu;
	dv		= v1->tv - v0->tv;
	segLen	= ROTSPRITECLIPLEFT - v0->sx;
	m		= segLen / dx;

	// check if polygon edge is in viewport
	if((v0->sx >= ROTSPRITECLIPLEFT) && (v1->sx >= ROTSPRITECLIPLEFT))
	{
		poly->verts[poly->numVerts] = *(v1);
		poly->numVerts++;
	}

	// check if polygon edge is leaving viewport
	if((v0->sx >= ROTSPRITECLIPLEFT) && (v1->sx < ROTSPRITECLIPLEFT))
	{
		poly->verts[poly->numVerts] = *(v0);
		poly->verts[poly->numVerts].sx = ROTSPRITECLIPLEFT;
		poly->verts[poly->numVerts].sy = v0->sy + (dy * m);
		poly->verts[poly->numVerts].tu = v0->tu + (du * m);
		poly->verts[poly->numVerts].tv = v0->tv + (dv * m);
		poly->numVerts++;
	}

	// check if polygon edge is entering viewport
	if((v0->sx < ROTSPRITECLIPLEFT) && (v1->sx >= ROTSPRITECLIPLEFT))
	{
		poly->verts[poly->numVerts] = *(v0);
		poly->verts[poly->numVerts].sx = ROTSPRITECLIPLEFT;
		poly->verts[poly->numVerts].sy = v0->sy + (dy * m);
		poly->verts[poly->numVerts].tu = v0->tu + (du * m);
		poly->verts[poly->numVerts].tv = v0->tv + (dv * m);
		poly->numVerts++;

		poly->verts[poly->numVerts] = *(v1);
		poly->numVerts++;
	}
}

void SpriteClip_Right(POLYCLIP *poly,D3DTLVERTEX *v0,D3DTLVERTEX *v1)
{
	float dx,dy,m,segLen;
	float du,dv;

	dx		= v1->sx - v0->sx;
	dy		= v1->sy - v0->sy;
	du		= v1->tu - v0->tu;
	dv		= v1->tv - v0->tv;
	segLen	= ROTSPRITECLIPRIGHT - v0->sx;
	m		= segLen / dx;

	// check if polygon edge is in viewport
	if((v0->sx < ROTSPRITECLIPRIGHT) && (v1->sx < ROTSPRITECLIPRIGHT))
	{
		poly->verts[poly->numVerts] = *(v1);
		poly->numVerts++;
	}

	// check if polygon edge is leaving viewport
	if((v0->sx < ROTSPRITECLIPRIGHT) && (v1->sx >= ROTSPRITECLIPRIGHT))
	{
		poly->verts[poly->numVerts] = *(v0);
		poly->verts[poly->numVerts].sx = ROTSPRITECLIPRIGHT;
		poly->verts[poly->numVerts].sy = v0->sy + (dy * m);
		poly->verts[poly->numVerts].tu = v0->tu + (du * m);
		poly->verts[poly->numVerts].tv = v0->tv + (dv * m);
		poly->numVerts++;
	}

	// check if polygon edge is entering viewport
	if((v0->sx >= ROTSPRITECLIPRIGHT) && (v1->sx < ROTSPRITECLIPRIGHT))
	{
		poly->verts[poly->numVerts] = *(v0);
		poly->verts[poly->numVerts].sx = ROTSPRITECLIPRIGHT;
		poly->verts[poly->numVerts].sy = v0->sy + (dy * m);
		poly->verts[poly->numVerts].tu = v0->tu + (du * m);
		poly->verts[poly->numVerts].tv = v0->tv + (dv * m);
		poly->numVerts++;

		poly->verts[poly->numVerts] = *(v1);
		poly->numVerts++;
	}
}

void SpriteClip_Top(POLYCLIP *poly,D3DTLVERTEX *v0,D3DTLVERTEX *v1)
{
	float dx,dy,m,segLen;
	float du,dv;

	dx		= v1->sx - v0->sx;
	dy		= v1->sy - v0->sy;
	du		= v1->tu - v0->tu;
	dv		= v1->tv - v0->tv;
	segLen	= ROTSPRITECLIPTOP - v0->sy;
	m		= segLen / dy;

	// check if polygon edge is in viewport
	if((v0->sy >= ROTSPRITECLIPTOP) && (v1->sy >= ROTSPRITECLIPTOP))
	{
		poly->verts[poly->numVerts] = *(v1);
		poly->numVerts++;
	}

	// check if polygon edge is leaving viewport
	if((v0->sy >= ROTSPRITECLIPTOP) && (v1->sy < ROTSPRITECLIPTOP))
	{
		poly->verts[poly->numVerts] = *(v0);
		poly->verts[poly->numVerts].sx = v0->sx + (dx * m);
		poly->verts[poly->numVerts].sy = ROTSPRITECLIPTOP;
		poly->verts[poly->numVerts].tu = v0->tu + (du * m);
		poly->verts[poly->numVerts].tv = v0->tv + (dv * m);
		poly->numVerts++;
	}

	// check if polygon edge is entering viewport
	if((v0->sy < ROTSPRITECLIPTOP) && (v1->sy >= ROTSPRITECLIPTOP))
	{
		poly->verts[poly->numVerts] = *(v0);
		poly->verts[poly->numVerts].sx = v0->sx + (dx * m);
		poly->verts[poly->numVerts].sy = ROTSPRITECLIPTOP;
		poly->verts[poly->numVerts].tu = v0->tu + (du * m);
		poly->verts[poly->numVerts].tv = v0->tv + (dv * m);
		poly->numVerts++;

		poly->verts[poly->numVerts] = *(v1);
		poly->numVerts++;
	}
}

void SpriteClip_Bottom(POLYCLIP *poly,D3DTLVERTEX *v0,D3DTLVERTEX *v1)
{
	float dx,dy,m,segLen;
	float du,dv;

	dx		= v1->sx - v0->sx;
	dy		= v1->sy - v0->sy;
	du		= v1->tu - v0->tu;
	dv		= v1->tv - v0->tv;
	segLen	= ROTSPRITECLIPBOTTOM - v0->sy;
	m		= segLen / dy;

	// check if polygon edge is in viewport
	if((v0->sy < ROTSPRITECLIPBOTTOM) && (v1->sy < ROTSPRITECLIPBOTTOM))
	{
		poly->verts[poly->numVerts] = *(v1);
		poly->numVerts++;
	}

	// check if polygon edge is leaving viewport
	if((v0->sy < ROTSPRITECLIPBOTTOM) && (v1->sy >= ROTSPRITECLIPBOTTOM))
	{
		poly->verts[poly->numVerts] = *(v0);
		poly->verts[poly->numVerts].sx = v0->sx + (dx * m);
		poly->verts[poly->numVerts].sy = ROTSPRITECLIPBOTTOM;
		poly->verts[poly->numVerts].tu = v0->tu + (du * m);
		poly->verts[poly->numVerts].tv = v0->tv + (dv * m);
		poly->numVerts++;
	}

	// check if polygon edge is entering viewport
	if((v0->sy >= ROTSPRITECLIPBOTTOM) && (v1->sy < ROTSPRITECLIPBOTTOM))
	{
		poly->verts[poly->numVerts] = *(v0);
		poly->verts[poly->numVerts].sx = v0->sx + (dx * m);
		poly->verts[poly->numVerts].sy = ROTSPRITECLIPBOTTOM;
		poly->verts[poly->numVerts].tu = v0->tu + (du * m);
		poly->verts[poly->numVerts].tv = v0->tv + (dv * m);
		poly->numVerts++;

		poly->verts[poly->numVerts] = *(v1);
		poly->numVerts++;
	}
}


//----- [ SORTING STUFF ] -----------------------------------------------------------------------

/*	--------------------------------------------------------------------------------
	Function		: InitSpriteSortArray
	Purpose			: initialises the sprite sort array
	Parameters		: int
	Returns			: void
	Info			: 
*/
void InitSpriteSortArray( )
{
	if(spriteSortArray)
		FreeSpriteSortArray();

	spriteSortArray = (SPRITE *)MALLOC0( sizeof(SPRITE) * MAX_ARRAY_SPRITES );
	numSortArraySprites = 0;
}


/*	--------------------------------------------------------------------------------
	Function		: FreeSpriteSortArray
	Purpose			: frees the sprite sort array
	Parameters		: 
	Returns			: 
	Info			: 
*/
void FreeSpriteSortArray()
{
	if(spriteSortArray)
		FREE( spriteSortArray );

	spriteSortArray = NULL;
}


/*	--------------------------------------------------------------------------------
	Function		: SpriteZCompare
	Purpose			: function to compare transformed sprite z-values for sorting
	Parameters		: const void *,const void *
	Returns			: int
	Info			: -1 if less than, 0 if equal to, 1 if greater than
*/
int SpriteZCompare(const void *arg1,const void *arg2)
{
	SPRITE *s1 = (SPRITE *)arg1;
	SPRITE *s2 = (SPRITE *)arg2;

	if(s1->sc.vz < s2->sc.vz)
		return -1;
	else if(s1->sc.vz == s2->sc.vz)
		return 0;
	else
		return 1;
}


/*	--------------------------------------------------------------------------------
	Function		: ZSortSpriteList
	Purpose			: sorts the sprites based on z-distance
	Parameters		: 
	Returns			: void
	Info			: list to sort is specified in srcList
*/

#define SPRITE_ZSORT_DRAWDISTANCE	450

void ZSortSpriteList()
{
	SPRITE *cur;
	MDX_VECTOR frogXfm;
		
	if(sprList.count < 2)
		return;

	XfmPoint(&frogXfm,(MDX_VECTOR *)&frog[0]->actor->position,NULL);

	// uses a quick sort

	// traverse through sprite list and create the sort array
	numSortArraySprites = 0;
	for(cur = sprList.head.next; cur != &sprList.head && numSortArraySprites < MAX_ARRAY_SPRITES; cur = cur->next)
	{
		// the static array should be large enough to hold sprites
		if((cur->sc.vz - frogXfm.vz) < farClip )
		{
			spriteSortArray[numSortArraySprites] = *(cur);
			numSortArraySprites++;
		}
	}

	qsort(spriteSortArray,numSortArraySprites,sizeof(SPRITE),SpriteZCompare);
}

