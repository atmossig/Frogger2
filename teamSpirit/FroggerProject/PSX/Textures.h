#ifndef _TEXTURES_H_INCLUDE
#define _TEXTURES_H_INCLUDE

#include <isltex.h>
#include "sonylibs.h"

#define MAX_TEXTURE_BANKS		5

extern TextureBankType *textureBanks [ MAX_TEXTURE_BANKS ];


void LoadTextureBank			( int textureBank );
void FreeTextureBank			( TextureBankType *textureBank );
void FreeAllTextureBanks	( void );

#endif