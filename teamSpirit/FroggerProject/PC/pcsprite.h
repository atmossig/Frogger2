/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: pcsprite.h
	Programmer	: Jim
	Date		: 16/03/00
	Purpose		: Sprite sorting and printing

----------------------------------------------------------------------------------------------- */

#ifndef PCSPRITE_H_INCLUDED
#define PCSPRITE_H_INCLUDED

#include "sprite.h"

#ifdef __cplusplus
extern "C"
{
#endif

// added by ANDYE to facilitate (<- ooh, good word !) Sutherland-Hodgman clipping algorhythmicallythingy
typedef struct TAGPOLYCLIP
{
	int numVerts;
	D3DTLVERTEX verts[10];

} POLYCLIP;

extern int numSprites;
extern short spriteIndices[];

void PrintSpriteOverlays(long);
void PrintSprites();
void PrintSprite(SPRITE *sprite);

// PC Sprite drawing stuff
void DrawALine (float x1, float y1, float x2, float y2, D3DCOLOR color);
void DrawASprite (float x, float y, float xs, float ys, float u1, float v1, float u2, float v2, MDX_TEXENTRY *tex,DWORD colour);
void DrawAlphaSprite (float x, float y, float z, float xs, float ys, float u1, float v1, float u2, float v2, MDX_TEXENTRY *tex, DWORD colour );
void DrawAlphaSpriteRotating(MDX_VECTOR *pos,float angle,float x, float y, float z, float xs, float ys, float u1, float v1, float u2, float v2, MDX_TEXENTRY *tex, DWORD colour );
//void AddObjectsSpritesToSpriteList(MDX_OBJECT *obj,short flags);


// PC Sprite sorting stuff
#define MAX_ARRAY_SPRITES		768

extern int numSortArraySprites;
extern SPRITE *spriteSortArray;

void InitSpriteSortArray( );
void FreeSpriteSortArray();
int SpriteZCompare(const void *arg1,const void *arg2);
void ZSortSpriteList();

#ifdef __cplusplus
}
#endif


#endif
