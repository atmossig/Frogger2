/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: printgfx.c
	Programmer	: Andy Eder
	Date		: 19/04/99 11:12:48

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

#include <ultra64.h>

#include "incs.h"


/*	--------------------------------------------------------------------------------
	Function		: PrintBackdrop
	Purpose			: prints a backdrop...
	Parameters		: ptr to a backdrop to display
	Returns			: void
	Info			: assumes relevant surface (back buffer) has been locked
*/
void PrintBackdrop(BACKDROP *bDrop)
{
//	SetRaster(NORMAL);
//	PrintSpriteSize(bDrop->xPos,bDrop->yPos,bDrop->data,640,480);
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
			
			DrawASprite (x*2,y*2,tOver->font->height*2,tOver->font->width*2,
				(float)u/256.0,(float)v/256.0,
				((float)u+tOver->font->width-1)/256.0,
				((float)v+tOver->font->height-1)/256.0,tOver->font->hdl );

			x += tOver->font->xSpacing[letterID];
		}

		pos++;
	}

	wSeed += 0.1;
}
void XfmPoint (VECTOR *vTemp2, VECTOR *in);

extern long FOV,DIST;

void PrintSprite(SPRITE *sprite)
{
	VECTOR m,sc;
	VECTOR s = {1,1,0};
	float distx,disty;
	
	if((!sprite->texture) || (sprite->scaleX == 0) || (sprite->scaleY == 0))
		return;

	XfmPoint (&m,&sprite->pos);
	
	if (m.v[Z])
	{
		distx = disty = (FOV)/(m.v[Z]+DIST);
		distx *= sprite->scaleX/64.0;
		disty *= sprite->scaleY/64.0;
		DrawASprite (m.v[X]+sprite->offsetX*distx,m.v[Y]+sprite->offsetY*disty,32*distx,32*disty,0,0,0.99,0.99,sprite->texture);
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

	if(!testPause)
	{
		for(cur = spriteList.head.next; (cur != &spriteList.head); cur = next)
		{
			next = cur->next;
			PrintSprite(cur);
		}
	}

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

			texture = cur->frames[cur->currFrame];
			DrawASprite (cur->xPos*2,cur->yPos*2,cur->width*2,cur->height*2,0,0,0.99,0.99,texture);

		}

		cur = cur->next;
	}

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
