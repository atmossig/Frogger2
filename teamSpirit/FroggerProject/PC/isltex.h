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


TextureBankType *textureLoadBank(char *sFile)									{return NULL;}
TextureType *textureFindCRCInAllBanks(unsigned long crc)						{return NULL;}
void textureDownloadBank(TextureBankType *bank)									{return;}
void textureDestroyBank(TextureBankType *bank)									{return;}
TextureType *textureFindCRCInBank(TextureBankType *bank, unsigned long crc)		{return NULL;}
TextureAnimType *textureCreateAnimation(TextureType *dummy, TextureType **anim, int numFrames) {return NULL;}

#endif