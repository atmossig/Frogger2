#ifndef _ISLTEX_H_INCLUDE
#define _ISLTEX_H_INCLUDE

#include "ultra64.h"

#define textureFindInAllBanks(n)	(0)

typedef struct
{
	int temp;
} TextureType;

typedef struct
{
	int numTextures;
	TextureType* texture;
} TextureBankType;

typedef struct
{
	int temp;
} TextureAnimType;


TextureBankType *textureLoadBank(char *sFile);
TextureType *textureFindCRCInAllBanks(unsigned long crc);
void textureDownloadBank(TextureBankType *bank);
void textureDestroyBank(TextureBankType *bank);
TextureType *textureFindCRCInBank(TextureBankType *bank, unsigned long crc);
TextureAnimType *textureCreateAnimation(TextureType *dummy, TextureType **anim, int numFrames);

#endif