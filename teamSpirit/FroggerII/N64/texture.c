/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: texture.c
	Programmer	: Andy Eder
	Date		: 19/04/99 12:39:48

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

#include <ultra64.h>

#include "incs.h"


TEXTURE_BANK	textureBanks[MAX_TEXTURE_BANKS];		//final texture bank is for font
TEXTURE *tempTexture;

unsigned long numTextureBanks = 0;

char message[32];

/*	--------------------------------------------------------------------------------
	Function		: FreeAllTextureBanks
	Purpose			: frees up ALL texture banks
	Parameters		: 
	Returns			: void
	Info			: NOTE: leaves system texture bank alone !
*/
void FreeAllTextureBanks()
{
	unsigned long i;

	for(i=0; i<MAX_TEXTURE_BANKS; i++)
	{
		if(textureBanks[i].freePtr)
		{
			dprintf"Freeing TBank %d...",i));
			JallocFree((UBYTE**)&textureBanks[i].freePtr);
			dprintf"done !\n",i));
		}
	}
	numTextureBanks = 0;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitTextureBanks()
{
	int x;

	for(x = 0; x < MAX_TEXTURE_BANKS; x++)
	{
		textureBanks[x].freePtr = NULL;
		textureBanks[x].data = NULL;
		textureBanks[x].numTextures = 0;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: LoadTexureBank
	Purpose 	: dma's a texture bank from rom into ram.
	Parameters 	: char *texturebank
	Returns 	: none
	Info 		:
*/
void LoadTextureBank(int num)
{
	char	*textureBank;
	u32		bankRomStart, bankRomEnd, bankSize;
	short	x = 0, y = 0;
	TEXTURE *tex;
	char	*temp;

	switch(num)
	{
		case SYSTEM_TEX_BANK:
			bankRomStart	= (u32)&_texBank1SegmentRomStart;
			bankRomEnd		= (u32)&_texBank1SegmentRomEnd;
			sprintf(message,"SYS_TB");
			break;

		case GARDEN_TEX_BANK:
			bankRomStart	= (u32)&_texBank2SegmentRomStart;
			bankRomEnd		= (u32)&_texBank2SegmentRomEnd;
			sprintf(message,"GARD_TB");
			break;

		case SUPERRETRO_TEX_BANK:
			bankRomStart	= (u32)&_texBank3SegmentRomStart;
			bankRomEnd		= (u32)&_texBank3SegmentRomEnd;
			sprintf(message,"SUPER_TB");
			break;

		case HALLOWEEN_TEX_BANK:
			bankRomStart	= (u32)&_texBank4SegmentRomStart;
			bankRomEnd		= (u32)&_texBank4SegmentRomEnd;
			sprintf(message,"HOLLW_TB");
			break;

		case SPACE_TEX_BANK:
			bankRomStart	= (u32)&_texBank5SegmentRomStart;
			bankRomEnd		= (u32)&_texBank5SegmentRomEnd;
			sprintf(message,"SPACE_TB");
			break;

		case CITY_TEX_BANK:
			bankRomStart	= (u32)&_texBank6SegmentRomStart;
			bankRomEnd		= (u32)&_texBank6SegmentRomEnd;
			sprintf(message,"CITY_TB");
			break;

		case ANCIENT_TEX_BANK:
			bankRomStart	= (u32)&_texBank7SegmentRomStart;
			bankRomEnd		= (u32)&_texBank7SegmentRomEnd;
			sprintf(message,"ANCIENT_TB");
			break;

		case TITLES_TEX_BANK:
			bankRomStart	= (u32)&_texBank8SegmentRomStart;
			bankRomEnd		= (u32)&_texBank8SegmentRomEnd;
			sprintf(message,"TITLE_TB");
			break;

		case OLDEFROG_TEX_BANK:
			bankRomStart	= (u32)&_texBank9SegmentRomStart;
			bankRomEnd		= (u32)&_texBank9SegmentRomEnd;
			sprintf(message,"OLDE_TB");
			break;

		case FRONT_TEX_BANK:
			bankRomStart	= (u32)&_texBank10SegmentRomStart;
			bankRomEnd		= (u32)&_texBank10SegmentRomEnd;
			sprintf(message,"FRONT_TB");
			break;
		case SNDVIEW_TEX_BANK:
			bankRomStart	= (u32)&_texBank11SegmentRomStart;
			bankRomEnd		= (u32)&_texBank11SegmentRomEnd;
			sprintf(message,"SNDVIEW_TB");
			break;
		default:
			dprintf"Invalid texture bank!\n"));
			for(;;);
			break;
	}
	bankSize = bankRomEnd - bankRomStart;

	if(num != SYSTEM_TEX_BANK)
		textureBank = (char *)JallocAlloc(DMAGetSize(bankRomStart,bankRomEnd),YES,message);
	else
	{
		bigFont->data = (char *)JallocAlloc(DMAGetSize(bankRomStart,bankRomEnd),YES,"SYSTXTR");
		smallFont->data = &bigFont->data[0];
		oldeFont->data = &bigFont->data[0];
		textureBank = &bigFont->data[0];
	}

	DMAMemory(textureBank, bankRomStart, bankRomEnd);
	if(textureBank)
		dprintf"Loaded texture bank %s [%d]\n",message,bankSize));
	else
	{
		dprintf"Unable to load texture bank %s\n",message));
		return;
	}

	if(num != SYSTEM_TEX_BANK)
	{
		//store a pointer to the texture bank so it may be free'd at a later date	
		while(textureBanks[numTextureBanks].freePtr != 0);
		if(numTextureBanks >= MAX_TEXTURE_BANKS)
		{
			dprintf"ERROR:too many texture banks downloaded\n"));
			return;
		}
	}

	textureBanks[numTextureBanks].freePtr = textureBank;
	textureBanks[numTextureBanks].numTextures = *(int *)textureBank;
	textureBank += 4;
	textureBanks[numTextureBanks].data = textureBank;

	temp = textureBanks[numTextureBanks].data;
	for(y = 0; y < textureBanks[numTextureBanks].numTextures; y++)
	{
		tex = (TEXTURE *)temp;
		tex->data = (char *)((int)tex + sizeof(TEXTURE) );
		if(tex->palette)
			tex->palette = (short *)((int)tex->palette + (int)tex);
		temp += tex->size;
	}

	numTextureBanks++;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FindTexture(TEXTURE **texPtr, int texID, BOOL report, char *txtName)
{
	int x, y;
	TEXTURE *tex;
	char *temp;
			
	for(x = MAX_TEXTURE_BANKS - 1; x >= 0; x--)
//	for(x = 0; x < MAX_TEXTURE_BANKS; x++)
	{
		if(textureBanks[x].freePtr)
		{
			temp = textureBanks[x].data;
			for(y = 0; y < textureBanks[x].numTextures; y++)
			{
				tex = (TEXTURE *)temp;
				if(tex->texID == texID)
				{
					(*texPtr) = tex;
					
					return;
				}
				temp += tex->size;
			}
		}
	}
	*texPtr = NULL;
	if(report)
		dprintf"Whoops, texture '%X' not found, NAME: '%s' !\n", texID, txtName));

//	(*texPtr) = &fadeTexture;
	return;
}
