#include <isltex.h>

#ifdef __cplusplus
extern "C"
{
#endif

TextureBankType *textureLoadBank(char *sFile)
{
	return NULL;
}

TextureType *textureFindCRCInAllBanks(unsigned long crc)						
{
	return NULL;
}

void textureDownloadBank(TextureBankType *bank)									
{
	return;
}

void textureDestroyBank(TextureBankType *bank)									
{
	return;
}

TextureType *textureFindCRCInBank(TextureBankType *bank, unsigned long crc)		
{
	return NULL;
}

TextureAnimType *textureCreateAnimation(TextureType *dummy, TextureType **anim, int numFrames) 
{
	return NULL;
}

#ifdef __cplusplus
}
#endif
