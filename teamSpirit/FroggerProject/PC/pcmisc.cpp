/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: pcmisc.cpp
	Programmer	: Andy Eder
	Date		: 05/05/99 17:42:44

----------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <gelf.h>
#include <ddraw.h>
#include <d3d.h>
#include <mdx.h>
#include <islmem.h>

#include "Main.h"
#include "frogger.h"
#include "sprite.h"
#include "pcmisc.h"

LPDIRECTDRAWSURFACE7 LoadEditorTexture(const char* filename)
{
	int xDim, yDim;
	short *textureData;
	LPDIRECTDRAWSURFACE7 temp;

	textureData = (short *)gelfLoad_BMP((char*)filename, NULL, (void**)-1, 
		&xDim,&yDim,NULL,GELF_IFORMAT_16BPP555,GELF_IMAGEDATA_TOPDOWN);

	if (!textureData)
		return NULL;

	// Create surface and blit
	temp = D3DCreateTexSurface(xDim, yDim, 0xf81f, 0, 1);
	DDrawCopyToSurface(temp,(unsigned short *)textureData, 0, xDim, yDim);

	//res = D3DCreateTexSurface(xDim, yDim, 0xf81f, 0, 1);
	//res->BltFast(0,0,temp,NULL,DDBLTFAST_WAIT);

	return temp;
}

//----- [ SORTING STUFF ] -----------------------------------------------------------------------


/*	--------------------------------------------------------------------------------
	Function		: InitSpriteSortArray
	Purpose			: initialises the sprite sort array
	Parameters		: int
	Returns			: void
	Info			: 
*/
void InitSpriteSortArray(int numElements)
{
	if(spriteSortArray)
		FreeSpriteSortArray();

	spriteSortArray = (SPRITE *)MALLOC0( sizeof(SPRITE) * numElements );
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
