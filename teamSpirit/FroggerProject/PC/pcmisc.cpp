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
