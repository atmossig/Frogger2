/************************************************************************************
	ISL PSX LIBRARY	(c) 1999 Interactive Studios Ltd.

	isltex.c			Texture and VRAM management

************************************************************************************/
   
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "isltex.h"
#include "islutil.h"
#include "islfont.h"

extern psFont	*font;

#define MAXTEXBANKS			50

static TextureBankType *texBank[MAXTEXBANKS];

USHORT			*bitmapPtr,*twiddlePtr;

static int textureSetSPRPointers(NSPRITE *pHeader, short *maxWidth, short *maxHeight)
{
	int n = 0;
	
	*maxWidth = *maxHeight = 0;
	while(1)
	{
		pHeader[n].pal = (unsigned short *)((unsigned long)pHeader+((unsigned long)pHeader[n].pal));
		pHeader[n].image = (unsigned char *)((unsigned long)pHeader+((unsigned long)pHeader[n].image));
		if(*maxWidth < pHeader[n].w)
			*maxWidth = pHeader[n].w;
		if(*maxHeight < pHeader[n].h)
			*maxHeight = pHeader[n].h;
		if(pHeader[n].flags & NLASTSPR)
			return(++n);
		n++;
	}
}


/**************************************************************************
	FUNCTION:	textureLoadBank()
	PURPOSE:	Load texture bank
	PARAMETERS:	Filename
	RETURNS:	Ptr to texture bank info
**************************************************************************/

TextureBankType *textureLoadBank(char *sFile)
{
 	TextureBankType	*newBank;
	int				loop,j;
	ULONG			tempCRC;
	int				filelength;
	int				num,width,height;
	NSPRITE			*pNSprite;
	int				numTextures;
	short			maxWidth,maxHeight;

	
/*	newBank = Align32Malloc(sizeof(TextureBankType));	

	for(loop=0;loop<MAXTEXBANKS;loop++)
	{
		if(texBank[loop]==NULL)
		{
			texBank[loop] = newBank;
			break;
		}
	}
	if(loop==MAXTEXBANKS)
		printf("**** WARNING: OUT OF TEXTURE BANKS\n");
*/
	// change to the textures directory
	gdFsChangeDir("\\");
	gdFsChangeDir("textures");

	pNSprite = (NSPRITE *)fileLoad(sFile, &filelength);
	numTextures = textureSetSPRPointers(pNSprite,&maxWidth,&maxHeight);

	bitmapPtr = Align32Malloc(maxWidth*maxHeight*sizeof(USHORT));
	twiddlePtr = Align32Malloc(maxWidth*maxHeight*sizeof(USHORT));

	// convert and load textures
	for(loop=0;loop<numTextures;loop++)
	{
		if(pNSprite[loop].flags & NSPLIT)
		{
			num = 0;
			width = pNSprite[loop].w;
			height = pNSprite[loop].h;
			while(pNSprite[loop+num].flags & NSPLIT)		
			{
				width += pNSprite[loop+num].w;
				num++;
			}
			num++;
//			loadBigNSPRITEIntoSurface(newBank, width, height, num, loop);
			loop += (num-1);			
		}
		else
		{
/*			if((utilStr2CRC("AI_CIRCLE") == pNSprite[loop].crc)||
			(utilStr2CRC("FLASH") == pNSprite[loop].crc)||
			(utilStr2CRC("FLASH2") == pNSprite[loop].crc)||
			(utilStr2CRC("AI_RING") == pNSprite[loop].crc)||
			(utilStr2CRC("AI_VOLUME") == pNSprite[loop].crc)||
			(utilStr2CRC("AI_WARPFLASH") == pNSprite[loop].crc)||
			(utilStr2CRC("AI_RIPPLE2") == pNSprite[loop].crc))
				pNSprite[loop].flags |= NALPHA;

*/			if(utilStr2CRC("FROGWATCH01") == pNSprite[loop].crc)
			{
				j=8;			
			}
				
			loadNSPRITEIntoSurface(&pNSprite[loop]);
		}
	}

	Align32Free(pNSprite);	
	Align32Free(bitmapPtr);
	Align32Free(twiddlePtr);

/*	newBank->pNSprite = (NSPRITE *)fileLoad(sFile, &filelength);
	newBank->numTextures = textureSetSPRPointers(newBank->pNSprite);

	newBank->CRC = (unsigned long *)Align32Malloc(newBank->numTextures*4+(newBank->numTextures/8)+1);
	for(loop=0;loop<newBank->numTextures;loop++)
	{
		tempCRC = newBank->pNSprite[loop].crc;
		newBank->CRC[loop] = newBank->pNSprite[loop].crc;
	}

	newBank->used = (unsigned char *)(newBank->CRC+newBank->numTextures);
	memset(newBank->used, 0, (newBank->numTextures/8)+1);

	newBank->texture = (TextureType *)Align32Malloc(sizeof(TextureType)*newBank->numTextures);
	memset(newBank->texture, 0, sizeof(TextureType)*newBank->numTextures);

	// convert and load textures
	for(loop=0;loop<newBank->numTextures;loop++)
	{
		if(newBank->pNSprite[loop].flags & NSPLIT)
		{
			num = 0;
			width = newBank->pNSprite[loop].w;
			height = newBank->pNSprite[loop].h;
			while(newBank->pNSprite[loop+num].flags & NSPLIT)		
			{
				width += newBank->pNSprite[loop+num].w;
				num++;
			}
			num++;
//			loadBigNSPRITEIntoSurface(newBank, width, height, num, loop);
			loop += (num-1);			
		}
		else
		{
			if((utilStr2CRC("AI_CIRCLE") == newBank->pNSprite[loop].crc)||
			(utilStr2CRC("FLASH") == newBank->pNSprite[loop].crc)||
			(utilStr2CRC("FLASH2") == newBank->pNSprite[loop].crc)||
			(utilStr2CRC("AI_RING") == newBank->pNSprite[loop].crc)||
			(utilStr2CRC("AI_VOLUME") == newBank->pNSprite[loop].crc)||
			(utilStr2CRC("AI_RIPPLE2") == newBank->pNSprite[loop].crc))
				newBank->pNSprite[loop].flags |= NALPHA;
				
			loadNSPRITEIntoSurface(&newBank->pNSprite[loop]);
		}
	}
*/
	return newBank;
}

/**************************************************************************
	FUNCTION:	textureDownloadFromBank()
	PURPOSE:	Upload/find given texture in bank
	PARAMETERS:	Texture bank info ptr, index of texture
	RETURNS:	Sprite info ptr
**************************************************************************/

TextureType *textureDownloadFromBank(TextureBankType *bank, int idx)
{
	long	b,y;
	NSPRITE	*pNSprite = bank->pNSprite;

	b = 1<<(idx & 7);
	y = idx>>3;
	if(!(bank->used[y] & b))
	{
		if(pNSprite != NULL)
		{
			bank->used[y] |= b;
//			textureDownLoad(pNSprite+idx, (bank->texture)+idx);
		}
		else
			return NULL;
	}
	return ((bank->texture)+idx);
}


/**************************************************************************
	FUNCTION:	textureDownloadBank()
	PURPOSE:	Download entire texture bank to VRAM
	PARAMETERS:	Texture bank info ptr
	RETURNS:	
**************************************************************************/

void textureDownloadBank(TextureBankType *bank)
{
	int	loop;

	for(loop=0; loop<bank->numTextures; loop++)
		textureDownloadFromBank(bank, loop);
}


/**************************************************************************
	FUNCTION:	textureDestroyBank()
	PURPOSE:	Free system RAM data for given texture bank
	PARAMETERS:	Texture bank info ptr
	RETURNS:	
**************************************************************************/

void textureDestroyBank(TextureBankType *bank)
{
	if(bank==NULL)
		return;

	Align32Free(bank->pNSprite);
	bank->pNSprite = NULL;
}


/**************************************************************************
	FUNCTION:	textureUnloadBank()
	PURPOSE:	Unload given texture bank from VRAM
	PARAMETERS:	Texture bank info ptr
	RETURNS:	
**************************************************************************/

void textureUnloadBank(TextureBankType *bank)
{
	unsigned long	loop, y,b, numTex;
	
	if (bank==NULL)
		return;

	// free up texture space in vram
	numTex = bank->numTextures;
	for(loop=0; loop<numTex; loop++)
	{
		b = 1<<(loop & 7);
		y = loop>>3;
		if (bank->used[y] & b)
			textureUnload(bank->texture+loop);
	}

	// remove internal reference to bank

	for(loop=0; loop<MAXTEXBANKS; loop++)
		if (texBank[loop]==bank)
			texBank[loop] = NULL;

	// free up ram space
	syFree(bank->texture);
	syFree(bank->CRC);
	syFree(bank);
}


/**************************************************************************
	FUNCTION:	textureFindCRCInBank()
	PURPOSE:	Find given texture in bank
	PARAMETERS:	Texture bank info ptr, Texture name CRC
	RETURNS:	Texture info ptr
**************************************************************************/

TextureType *textureFindCRCInBank(TextureBankType *bank, unsigned long crc)
{
	unsigned long	numTextures = bank->numTextures;
	unsigned long	loop;

	for(loop=0; loop<numTextures; loop++)
	{
		if (bank->CRC[loop] == crc)
			return textureDownloadFromBank(bank, loop);
	}
	return NULL;
}


/**************************************************************************
	FUNCTION:	textureFindCRCInAllBanks()
	PURPOSE:	Find given texture in all loaded banks
	PARAMETERS:	Texture name CRC
	RETURNS:	Texture info ptr
**************************************************************************/

TextureType *textureFindCRCInAllBanks(unsigned long crc)
{
	int			loop;
	TextureType	*txPtr = NULL;

	for(loop=0; loop<MAXTEXBANKS; loop++)
	{
		if (texBank[loop]!=NULL)
		{
			txPtr = textureFindCRCInBank(texBank[loop], crc);
			if (txPtr!=NULL)
				return txPtr;
		}
	}
	return txPtr;
}


/**************************************************************************
	FUNCTION:	textureReallocTextureBank()
	PURPOSE:	Reallocate texture in system RAM (e.g. to defragement)
	PARAMETERS:	Texture bank info ptr
	RETURNS:	New texture bank info ptr
**************************************************************************/

TextureBankType *textureReallocTextureBank(TextureBankType *txBank)
{
	TextureBankType	*textureBank;
	int				loop;
	TextureType		*txPtr;

	textureBank = Align32Malloc(sizeof(TextureBankType));
	textureBank->numTextures = txBank->numTextures;
	
	txPtr = Align32Malloc(sizeof(TextureType) * txBank->numTextures);
	memcpy(txPtr, txBank->texture, sizeof(TextureType)*txBank->numTextures);
	textureBank->texture = txPtr;

	textureBank->CRC = (unsigned long *)Align32Malloc(textureBank->numTextures*4+(textureBank->numTextures/8)+1);
	memcpy(textureBank->CRC, txBank->CRC, textureBank->numTextures*4+(textureBank->numTextures/8)+1);

	textureBank->pNSprite = txBank->pNSprite;

	textureBank->used = (unsigned char *)(textureBank->CRC+textureBank->numTextures);
	memcpy(textureBank->used, txBank->used, (textureBank->numTextures/8)+1);

	syFree(txBank->texture);
	syFree(txBank->CRC);
	syFree(txBank);

	for(loop = 0; loop < MAXTEXBANKS; loop ++)
	{
		if(texBank[loop] == txBank)
		{
			texBank[loop] = textureBank;
			break;
		}
	}

	return textureBank;
}


/**************************************************************************
	FUNCTION:	textureCreateAnimation
	PURPOSE:	Create an animated texture
	PARAMETERS:	pointer to dummy texture, pointer to array of animated textures, number of frames
	RETURNS:	pointer to animated texture
**************************************************************************/

TextureAnimType *textureCreateAnimation(TextureType *dummy, TextureType **anim, int numFrames)
{
	TextureAnimType	*texAnim;
	int				loop;

	texAnim = Align32Malloc(sizeof(TextureAnimType));

	texAnim->dest = dummy;

	texAnim->anim = Align32Malloc(sizeof(TextureType) * numFrames);

	for(loop = 0; loop < numFrames; loop ++)
	{
		texAnim->anim[loop] = anim[loop];
	}

	return texAnim;
}


/**************************************************************************
	FUNCTION:	textureSetAnimation
	PURPOSE:	Set frame of an animated texture
	PARAMETERS:	Animated texture, frame number
	RETURNS:	
**************************************************************************/

void textureSetAnimation(TextureAnimType *texAnim, int frameNum)
{
/*	DR_MOVE *siMove;
	RECT	moveRect;

	moveRect.x = VRAM_CALCVRAMX(texAnim->anim[frameNum]->handle);
	moveRect.y = VRAM_CALCVRAMY(texAnim->anim[frameNum]->handle);
	moveRect.w = (texAnim->dest->w + 3) / 4;
	moveRect.h = texAnim->dest->h;

	// check for 256 colour mode
	if(texAnim->dest->tpage & (1 << 7))
		moveRect.w *= 2;

	// copy bit of vram
	BEGINPRIM(siMove, DR_MOVE);
	SetDrawMove(siMove, &moveRect, VRAM_CALCVRAMX(texAnim->dest->handle),VRAM_CALCVRAMY(texAnim->dest->handle));
	ENDPRIM(siMove, 1023, DR_MOVE);
*/	
}


/**************************************************************************
	FUNCTION:	textureDestroyAnimation
	PURPOSE:	Destroy an animated texture
	PARAMETERS:	Animated texture
	RETURNS:	
**************************************************************************/

void textureDestroyAnimation(TextureAnimType *texAnim)
{
	syFree(texAnim->anim);
	syFree(texAnim);
}
