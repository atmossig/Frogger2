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
#include <isltex.h>
#include <islutil.h>

#include "Main.h"
#include "pcmisc.h"
#include "ptexture.h"
#include "mdx.h"
#include "mdxTexture.h"

int drawOverlays = 1;

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


/*	--------------------------------------------------------------------------------
	Function 	: PTSurfaceBlit
	Purpose 	: Copy data into texture surface from procedural texture
	Parameters 	: Target texture, source data
	Returns 	: 
	Info 		:
*/
void PTSurfaceBlit( LPDIRECTDRAWSURFACE7 to, unsigned char *buf, unsigned short *pal )
{
	DDSURFACEDESC2 ddsd;
	HRESULT res;
	long i;
	DDINIT(ddsd);

	static LPDIRECTDRAWSURFACE7 pSurface = D3DCreateTexSurface( 32,32,0xf81f, 0,1);
	
	while( (res = pSurface->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR | DDLOCK_WRITEONLY,0)) != DD_OK )
		ddShowError(res);

	i=928;

	while( i-- ) ((unsigned short *)ddsd.lpSurface)[i] = (unsigned short)pal[(unsigned char)buf[i]];

	pSurface->Unlock(NULL);

	if ((res = to->BltFast(0,0,pSurface,NULL,0))!=DD_OK)
		ddShowError(res);
}


/*	--------------------------------------------------------------------------------
	Function 	: PTTextureLoad
	Purpose 	: Scan the texture list and convert any textures named "prc*" into procedurals
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void PTTextureLoad( )
{
	MDX_TEXENTRY *t;

	// List search
	for( t = texList; t; t = t->next )
	{
		if( t->name[0]=='p' && t->name[1]=='r' && t->name[2]=='c' )
			CreateProceduralTexture( (TextureType *)t, t->name );		
	}

	t = GetTexEntryFromCRC(UpdateCRC("00wate04.bmp"));
	if (t)
		testS = t->surf;

}


/*	--------------------------------------------------------------------------------
	Function 	: FindTexture
	Purpose 	: Add .bmp extension to filename and find in all banks
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
TextureType *FindTexture( char *name )
{
	char bmp[32];

	strcpy( bmp, name );
	strlwr( bmp );
	strcat( bmp, ".bmp" );

	return (TextureType *)textureFindCRCInAllBanks(utilStr2CRC(bmp));
}
