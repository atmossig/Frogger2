#ifndef _TEXTURES_H_INCLUDE
#define _TEXTURES_H_INCLUDE

#include <isltex.h>
#include "sonylibs.h"

#define MAX_TEXTURE_BANKS		5

typedef struct _LOADPAL_24BitRGBStr
{
	unsigned char r, g, b;
} LOADPAL_24BitRGBStr;
//////////////////////////////////////////////////////////////////
typedef struct _LOADPAL_PCPalette16Str
{
	LOADPAL_24BitRGBStr component[16];
} LOADPAL_PCPalette16Str;

extern TextureBankType *textureBanks [ MAX_TEXTURE_BANKS ];


extern char *palNames [ 5 ];

// For PSX, no need to add .bmp extension
#define FindTexture(x) textureFindCRCInAllBanks(utilStr2CRC(x))

void LoadTextureBank			( int textureBank );
void FreeTextureBank			( TextureBankType *textureBank );
void FreeAllTextureBanks	( void );

#endif