/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: banks.h
	Programmer	: David Swift
	Date		: 21/3/00

----------------------------------------------------------------------------------------------- */

#ifndef BANKS_H_INCLUDED
#define BANKS_H_INCLUDED

#include <isltex.h>

#ifdef __cplusplus
extern "C"
{
#endif

//from psx textures.h
#define MAX_TEXTURE_BANKS		5

extern TextureBankType *textureBanks [ MAX_TEXTURE_BANKS ];
void LoadTextureBank(int textureBank);
void FreeTextureBank(TextureBankType *textureBank);
void FreeAllTextureBanks(void);
void LoadObjectBank(int objectBank);

#define FreeWaterObjectList()

#ifdef __cplusplus
}
#endif
#endif