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

#define FIXED_TO_RADS				0.001525879

#define MAX_ARRAY_SPRITES			768
#define SPRITE_ZSORT_DRAWDISTANCE	450

int numSortArraySprites = 0;
SPRITE *spriteSortArray[MAX_ARRAY_SPRITES];

int SpriteZCompare(const void *arg1,const void *arg2);
void ZSortSpriteList();

// PC Sprite drawing stuff
void PrintSprite(SPRITE *sprite);

void DrawAlphaSprite (float x, float y, float z, float xs, float ys, float u1, float v1, float u2, float v2, MDX_TEXENTRY *tex, DWORD colour );
void DrawAlphaSpriteRotating(MDX_VECTOR *pos,float angle,float x, float y, float z, float xs, float ys, float u1, float v1, float u2, float v2, MDX_TEXENTRY *tex, DWORD colour );
void DrawSpriteOverlay( float x, float y, float z, float xs, float ys, float u1, float v1, float u2, float v2, MDX_TEXENTRY *tex, DWORD colour );


//----- [ PRINTING STUFF ] -----------------------------------------------------------------------

long	SPRITECLIPLEFT		=10;
long	SPRITECLIPTOP		=10;
long	SPRITECLIPRIGHT		=630;
long	SPRITECLIPBOTTOM	=470;

short spriteIndices[] = {0,1,2,2,3,0};
//short spriteIndices[] = {2,1,0,0,3,2};
float OVERLAY_X;
float OVERLAY_Y;

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
	while(i--) if( spriteSortArray[i]->draw ) 
		PrintSprite(spriteSortArray[i]);
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

	if( !drawOverlays || !spriteOverlayList.numEntries ) return;

	cur = spriteOverlayList.head.next;
	while(cur!=&spriteOverlayList.head)
	{
		if (cur->num!=num)
		{
			cur = cur->next;
			continue;
		}

//		cur->xPos = cur->xPosTo;
//		cur->yPos = cur->yPosTo;

		// update the sprite animation if an animated sprite overlay
		if(cur->draw)
		{
			// Go to destination, if specified
			float spd = (float)FMul(cur->speed,gameSpeed)/4096;
			
			if (Fabs(spd)>0)
			{
				if (Fabs(cur->xPosTo-cur->xPos) > 0)
				{
					cur->speed += FMul(gameSpeed,819);
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

						spd = (float)FMul(cur->speed,gameSpeed)/4096;
//						spd = cur->speed * gs;
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

							spd = (float)FMul(cur->speed, gameSpeed)/4096;
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
			
			if (cur->numFrames)
			{
				if (!(tEntry = ((MDX_TEXENTRY *)cur->frames[cur->currFrame])))
				{
					cur = cur->next;
					continue;
				}
			}
			else
				tEntry = NULL;	// untextured sprite (rect)
			
			if (cur->flags & SPRITE_ADDITIVE)
				D3DSetupRenderstates(xluAddRS);
			else if (cur->flags & SPRITE_SUBTRACTIVE)
				D3DSetupRenderstates(xluSubRS);
			else
				D3DSetupRenderstates(xluSemiRS);
			
			DrawSpriteOverlay( cur->xPos*OVERLAY_X, cur->yPos*OVERLAY_Y, 0, cur->width*OVERLAY_X, cur->height*OVERLAY_Y, 0, 0, 1, 1, 
				tEntry,D3DRGBA(cur->r/255.0,cur->g/255.0,cur->b/255.0,cur->a/255.0) );

			D3DSetupRenderstates(xluSemiRS);
			numSprites++;
		}

		cur = cur->next;
	}
}


/*	--------------------------------------------------------------------------------
    Function		: DrawAlphaSprite
	Parameters		: 
	Returns			: 
	Purpose			: Draw an alpha-ed, non-rotating sprite
*/
void DrawSpriteOverlay( float x, float y, float z, float xs, float ys, float u1, float v1, float u2, float v2, MDX_TEXENTRY *tex, DWORD colour )
{
	D3DTLVERTEX v[4];
	float x2 = (x+xs), y2 = (y+ys);

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

	SetTexture(tex);

	DrawPoly( D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX,
		v, 4, spriteIndices, 6,
		D3DDP_DONOTCLIP | D3DDP_DONOTLIGHT | D3DDP_DONOTUPDATEEXTENTS );

	SetTexture(NULL);
}


/*	--------------------------------------------------------------------------------
    Function		: PrintSprite
	Parameters		: Sprite
	Returns			: 
	Purpose			: Choose which sprite drawing function to use and set render states
*/
void PrintSprite(SPRITE *sprite)
{
	MDX_VECTOR m, sc, s = {1,1,0};
	float distx,disty;
	MDX_TEXENTRY *tEntry;

	if((!sprite->texture) || (sprite->scaleX == 0) || (sprite->scaleY == 0))
		return;

	SetVector( &sc, &sprite->sc );
	
	tEntry = ((MDX_TEXENTRY *)sprite->texture);

	if( sc.vz )
	{
		distx = disty = (FOV)/(sc.vz+DIST);
		distx *= (sprite->scaleX/(64.0));
		disty *= (sprite->scaleY/(64.0));
		numSprites++;

		if (sprite->flags & SPRITE_ADDITIVE)
		{
			D3DSetupRenderstates(xluAddRS);
			SwapFrame(3);
		}
		else
			SwapFrame(0);

		if(sprite->flags & SPRITE_FLAGS_ROTATE)
		{
			// rotate the little blighters
			sprite->angle += (sprite->angleInc * gameSpeed)>>12;
			if( sprite->angle >= 4096 ) 
				sprite->angle -= 4096;
			else if( sprite->angle < 0 ) 
				sprite->angle += 4096;

			DrawAlphaSpriteRotating( &sc,(float)sprite->angle*FIXED_TO_RADS,sc.vx+sprite->offsetX*distx,sc.vy+sprite->offsetY*disty,sc.vz*0.00025,32*distx,32*disty,
				0,0,1,1,tEntry,D3DRGBA(sprite->r/255.0,sprite->g/255.0,sprite->b/255.0,sprite->a/255.0) );
		}
		else
		{
			DrawAlphaSprite(sc.vx+sprite->offsetX*distx,sc.vy+sprite->offsetY*disty,sc.vz*0.00025,32*distx,32*disty,
				0,0,1,1,tEntry,D3DRGBA(sprite->r/255.0,sprite->g/255.0,sprite->b/255.0,sprite->a/255.0) );
		}

		D3DSetupRenderstates(xluSemiRS);
	}
}


/*	--------------------------------------------------------------------------------
    Function		: DrawAlphaSprite
	Parameters		: 
	Returns			: 
	Purpose			: Draw an alpha-ed, non-rotating sprite
void DrawAlphaSprite (float x, float y, float z, float xs, float ys, float u1, float v1, float u2, float v2, MDX_TEXENTRY *tex, DWORD colour )
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

	PushPolys(v,4,spriteIndices,6,tex);
}


void DrawAlphaSpriteRotating(MDX_VECTOR *pos,float angle,float x, float y, float z, float xs, float ys, float u1, float v1, float u2, float v2, MDX_TEXENTRY *tex, DWORD colour )
{
	D3DTLVERTEX v[5];
	float x2 = (x+xs), y2 = (y+ys), sine, cosine, newX, newY;
	int i,j;

	if( !tex ) return;

	// populate our structure ready for transforming and clipping the sprite
	v[0].sx = x - pos->vx;
	v[1].sx = x2 - pos->vx;
	v[2].sx = x2 - pos->vx;
	v[3].sx = x - pos->vx;

	v[0].sy = y - pos->vy;
	v[1].sy = y - pos->vy;
	v[2].sy = y2 - pos->vy;
	v[3].sy = y2 - pos->vy;

	v[0].tu = u1;	v[0].tv = v1;
	v[1].tu = u2;	v[1].tv = v1;
	v[2].tu = u2;	v[2].tv = v2;
	v[3].tu = u1;	v[3].tv = v2;

	// get rotation angle
	cosine	= (float)rcos(angle*4096)/4096;
	sine	= (float)rsin(angle*4096)/4096;

	// populate remaining data members and rotate the vertices comprising the sprite
	i = 4;
	while(i--)
	{
		v[i].sz			= z;
		v[i].rhw		= 0;
		v[i].color		= colour;
		v[i].specular	= D3DRGBA(0,0,0,1);

		newX = (v[i].sx * cosine) + (v[i].sy * sine);
		newY = (v[i].sy * cosine) - (v[i].sx * sine);

		v[i].sx = newX + pos->vx;
		v[i].sy = newY + pos->vy;
	}

	memcpy( &v[4], &v[0], sizeof(D3DTLVERTEX) );

	Clip3DPolygon( v, tex );
	Clip3DPolygon( &v[2], tex );
}	
*/

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
	int i=MAX_ARRAY_SPRITES;
	
	while( i-- ) spriteSortArray[i] = NULL;
	numSortArraySprites = 0;
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
	SPRITE *s1 = *(SPRITE **)arg1;
	SPRITE *s2 = *(SPRITE **)arg2;

	if(s1->sc.vz < s2->sc.vz)
		return -1;
	
	if(s1->sc.vz == s2->sc.vz)
		return 0;

	return 1;
}


/*	--------------------------------------------------------------------------------
	Function		: ZSortSpriteList
	Purpose			: sorts the sprites based on z-distance
	Parameters		: 
	Returns			: void
	Info			: list to sort is specified in srcList
*/
void ZSortSpriteList()
{
	SPRITE *cur;
	MDX_VECTOR frogXfm;
		
	if( !sprList.count )
		return;

	XfmPoint(&frogXfm,(MDX_VECTOR *)&frog[0]->actor->position,NULL);

	// traverse through sprite list and create the quick sort array
	numSortArraySprites = 0;
	for(cur = sprList.head.next; cur != &sprList.head && numSortArraySprites < MAX_ARRAY_SPRITES; cur = cur->next)
		if((cur->sc.vz - frogXfm.vz) < farClip )
			spriteSortArray[numSortArraySprites++] = cur;

	if( sprList.count > 1 )
		qsort( (void *)spriteSortArray, numSortArraySprites, sizeof(SPRITE*), SpriteZCompare );
}

