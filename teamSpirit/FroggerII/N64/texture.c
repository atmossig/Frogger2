/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: texture.c
	Programmer	: Andy Eder
	Date		: 19/04/99 12:39:48

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


TEXTURE_BANK	textureBanks[MAX_TEXTURE_BANKS];		//final texture bank is for font
TEXTURE *tempTexture;

unsigned long numTextureBanks = 0;


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
			dprintf"SYSTEM TEXTURE BANK - "));
			break;
		case TITLESGENERIC_TEX_BANK:
			bankRomStart	= (u32)&_texBank2SegmentRomStart;
			bankRomEnd		= (u32)&_texBank2SegmentRomEnd;
			dprintf"GENERIC TITLES TEXTURE BANK - "));
			break;
		case INGAMEGENERIC_TEX_BANK:
			bankRomStart	= (u32)&_texBank3SegmentRomStart;
			bankRomEnd		= (u32)&_texBank3SegmentRomEnd;
			dprintf"GENERIC IN-GAME TEXTURE BANK - "));
			break;
		
		// GARDEN TEXTURES -----------------------------------------------------------------------
		case GENERIC_GARDEN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_1_0_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_1_0_SegmentRomEnd;
			dprintf"GENERIC GARDEN TEXTURE BANK - "));
			break;
		case LEVEL1_GARDEN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_1_1_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_1_1_SegmentRomEnd;
			dprintf"GARDEN LEVEL 1 TEXTURE BANK - "));
			break;
		case LEVEL2_GARDEN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_1_2_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_1_2_SegmentRomEnd;
			dprintf"GARDEN LEVEL 2 TEXTURE BANK - "));
			break;
		case LEVEL3_GARDEN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_1_3_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_1_3_SegmentRomEnd;
			dprintf"GARDEN LEVEL 3 TEXTURE BANK - "));
			break;
		case BOSSA_GARDEN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_1_4_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_1_4_SegmentRomEnd;
			dprintf"GARDEN BOSS A TEXTURE BANK - "));
			break;
		case BOSSB_GARDEN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_1_5_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_1_5_SegmentRomEnd;
			dprintf"GARDEN BOSS B TEXTURE BANK - "));
			break;
		case BONUS_GARDEN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_1_6_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_1_6_SegmentRomEnd;
			dprintf"GARDEN BONUS TEXTURE BANK - "));
			break;
		case MULTI_GARDEN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_1_7_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_1_7_SegmentRomEnd;
			dprintf"GARDEN MULTIPLAYER TEXTURE BANK - "));
			break;

		// ANCIENT TEXTURES -----------------------------------------------------------------------
		case GENERIC_ANCIENT_TEX_BANK:
			bankRomStart	= (u32)&_texBank_2_0_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_2_0_SegmentRomEnd;
			dprintf"GENERIC ANCIENT TEXTURE BANK - "));
			break;
		case LEVEL1_ANCIENT_TEX_BANK:
			bankRomStart	= (u32)&_texBank_2_1_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_2_1_SegmentRomEnd;
			dprintf"ANCIENT LEVEL 1 TEXTURE BANK - "));
			break;
		case LEVEL2_ANCIENT_TEX_BANK:
			bankRomStart	= (u32)&_texBank_2_2_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_2_2_SegmentRomEnd;
			dprintf"ANCIENT LEVEL 2 TEXTURE BANK - "));
			break;
		case LEVEL3_ANCIENT_TEX_BANK:
			bankRomStart	= (u32)&_texBank_2_3_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_2_3_SegmentRomEnd;
			dprintf"ANCIENT LEVEL 3 TEXTURE BANK - "));
			break;
		case BOSS_ANCIENT_TEX_BANK:
			bankRomStart	= (u32)&_texBank_2_4_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_2_4_SegmentRomEnd;
			dprintf"ANCIENT BOSS TEXTURE BANK - "));
			break;
		case BOSSA_ANCIENT_TEX_BANK:
			bankRomStart	= (u32)&_texBank_2_5_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_2_5_SegmentRomEnd;
			dprintf"ANCIENT BOSS A TEXTURE BANK - "));
			break;
		case BOSSB_ANCIENT_TEX_BANK:
			bankRomStart	= (u32)&_texBank_2_6_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_2_6_SegmentRomEnd;
			dprintf"ANCIENT BOSS B TEXTURE BANK - "));
			break;
		case BOSSC_ANCIENT_TEX_BANK:
			bankRomStart	= (u32)&_texBank_2_7_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_2_7_SegmentRomEnd;
			dprintf"ANCIENT BOSS C TEXTURE BANK - "));
			break;
		case BONUS_ANCIENT_TEX_BANK:
			bankRomStart	= (u32)&_texBank_2_8_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_2_8_SegmentRomEnd;
			dprintf"ANCIENT BONUS TEXTURE BANK - "));
			break;
		case MULTI_ANCIENT_TEX_BANK:
			bankRomStart	= (u32)&_texBank_2_9_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_2_9_SegmentRomEnd;
			dprintf"ANCIENT MULTIPLAYER TEXTURE BANK - "));
			break;

		// SPACE TEXTURES -----------------------------------------------------------------------
		case GENERIC_SPACE_TEX_BANK:
			bankRomStart	= (u32)&_texBank_3_0_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_3_0_SegmentRomEnd;
			dprintf"GENERIC SPACE TEXTURE BANK - "));
			break;
		case LEVEL1_SPACE_TEX_BANK:
			bankRomStart	= (u32)&_texBank_3_1_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_3_1_SegmentRomEnd;
			dprintf"SPACE LEVEL 1 TEXTURE BANK - "));
			break;
		case LEVEL2_SPACE_TEX_BANK:
			bankRomStart	= (u32)&_texBank_3_2_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_3_2_SegmentRomEnd;
			dprintf"SPACE LEVEL 2 TEXTURE BANK - "));
			break;
		case LEVEL3_SPACE_TEX_BANK:
			bankRomStart	= (u32)&_texBank_3_3_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_3_3_SegmentRomEnd;
			dprintf"SPACE LEVEL 3 TEXTURE BANK - "));
			break;
		case BOSSA_SPACE_TEX_BANK:
			bankRomStart	= (u32)&_texBank_3_4_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_3_4_SegmentRomEnd;
			dprintf"SPACE BOSS TEXTURE BANK - "));
			break;
		case BONUS_SPACE_TEX_BANK:
			bankRomStart	= (u32)&_texBank_3_5_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_3_5_SegmentRomEnd;
			dprintf"SPACE BONUS TEXTURE BANK - "));
			break;
		case MULTI_SPACE_TEX_BANK:
			bankRomStart	= (u32)&_texBank_3_6_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_3_6_SegmentRomEnd;
			dprintf"SPACE MULTIPLAYER TEXTURE BANK - "));
			break;

		// CITY TEXTURES -------------------------------------------------------------------------
		case GENERIC_CITY_TEX_BANK:
			bankRomStart	= (u32)&_texBank_4_0_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_4_0_SegmentRomEnd;
			dprintf"GENERIC CITY TEXTURE BANK - "));
			break;
		case LEVEL1_CITY_TEX_BANK:
			bankRomStart	= (u32)&_texBank_4_1_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_4_1_SegmentRomEnd;
			dprintf"CITY LEVEL 1 TEXTURE BANK - "));
			break;
		case LEVEL2_CITY_TEX_BANK:
			bankRomStart	= (u32)&_texBank_4_2_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_4_2_SegmentRomEnd;
			dprintf"CITY LEVEL 2 TEXTURE BANK - "));
			break;
		case LEVEL3_CITY_TEX_BANK:
			bankRomStart	= (u32)&_texBank_4_3_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_4_3_SegmentRomEnd;
			dprintf"CITY LEVEL 3 TEXTURE BANK - "));
			break;
		case BOSSA_CITY_TEX_BANK:
			bankRomStart	= (u32)&_texBank_4_4_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_4_4_SegmentRomEnd;
			dprintf"CITY BOSS TEXTURE BANK - "));
			break;
		case BONUS_CITY_TEX_BANK:
			bankRomStart	= (u32)&_texBank_4_5_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_4_5_SegmentRomEnd;
			dprintf"CITY BONUS TEXTURE BANK - "));
			break;
		case MULTI_CITY_TEX_BANK:
			bankRomStart	= (u32)&_texBank_4_6_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_4_6_SegmentRomEnd;
			dprintf"CITY MULTIPLAYER TEXTURE BANK - "));
			break;

		// SUBTERRANEAN TEXTURES -----------------------------------------------------------------
		case GENERIC_SUBTERRANEAN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_5_0_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_5_0_SegmentRomEnd;
			dprintf"GENERIC SUBTERRANEAN TEXTURE BANK - "));
			break;
		case LEVEL1_SUBTERRANEAN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_5_1_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_5_1_SegmentRomEnd;
			dprintf"SUBTERRANEAN LEVEL 1 TEXTURE BANK - "));
			break;
		case LEVEL2_SUBTERRANEAN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_5_2_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_5_2_SegmentRomEnd;
			dprintf"SUBTERRANEAN LEVEL 2 TEXTURE BANK - "));
			break;
		case LEVEL3_SUBTERRANEAN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_5_3_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_5_3_SegmentRomEnd;
			dprintf"SUBTERRANEAN LEVEL 3 TEXTURE BANK - "));
			break;
		case BOSSA_SUBTERRANEAN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_5_4_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_5_4_SegmentRomEnd;
			dprintf"SUBTERRANEAN BOSS TEXTURE BANK - "));
			break;
		case BONUS_SUBTERRANEAN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_5_5_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_5_5_SegmentRomEnd;
			dprintf"SUBTERRANEAN BONUS TEXTURE BANK - "));
			break;
		case MULTI_SUBTERRANEAN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_5_6_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_5_6_SegmentRomEnd;
			dprintf"SUBTERRANEAN MULTIPLAYER TEXTURE BANK - "));
			break;

		// LABORATORY TEXTURES -------------------------------------------------------------------
		case GENERIC_LABORATORY_TEX_BANK:
			bankRomStart	= (u32)&_texBank_6_0_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_6_0_SegmentRomEnd;
			dprintf"GENERIC LABORATORY TEXTURE BANK - "));
			break;
		case LEVEL1_LABORATORY_TEX_BANK:
			bankRomStart	= (u32)&_texBank_6_1_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_6_1_SegmentRomEnd;
			dprintf"LABORATORY LEVEL 1 TEXTURE BANK - "));
			break;
		case LEVEL2_LABORATORY_TEX_BANK:
			bankRomStart	= (u32)&_texBank_6_2_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_6_2_SegmentRomEnd;
			dprintf"LABORATORY LEVEL 2 TEXTURE BANK - "));
			break;
		case LEVEL3_LABORATORY_TEX_BANK:
			bankRomStart	= (u32)&_texBank_6_3_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_6_3_SegmentRomEnd;
			dprintf"LABORATORY LEVEL 3 TEXTURE BANK - "));
			break;
		case BOSSA_LABORATORY_TEX_BANK:
			bankRomStart	= (u32)&_texBank_6_4_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_6_4_SegmentRomEnd;
			dprintf"LABORATORY BOSS TEXTURE BANK - "));
			break;
		case BONUS_LABORATORY_TEX_BANK:
			bankRomStart	= (u32)&_texBank_6_5_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_6_5_SegmentRomEnd;
			dprintf"LABORATORY BONUS TEXTURE BANK - "));
			break;
		case MULTI_LABORATORY_TEX_BANK:
			bankRomStart	= (u32)&_texBank_6_6_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_6_6_SegmentRomEnd;
			dprintf"LABORATORY MULTIPLAYER TEXTURE BANK - "));
			break;

		// TOYSHOP TEXTURES ----------------------------------------------------------------------
		case GENERIC_TOYSHOP_TEX_BANK:
			bankRomStart	= (u32)&_texBank_7_0_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_7_0_SegmentRomEnd;
			dprintf"GENERIC TOYSHOP TEXTURE BANK - "));
			break;
		case LEVEL1_TOYSHOP_TEX_BANK:
			bankRomStart	= (u32)&_texBank_7_1_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_7_1_SegmentRomEnd;
			dprintf"TOYSHOP LEVEL 1 TEXTURE BANK - "));
			break;
		case LEVEL2_TOYSHOP_TEX_BANK:
			bankRomStart	= (u32)&_texBank_7_2_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_7_2_SegmentRomEnd;
			dprintf"TOYSHOP LEVEL 2 TEXTURE BANK - "));
			break;
		case LEVEL3_TOYSHOP_TEX_BANK:
			bankRomStart	= (u32)&_texBank_7_3_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_7_3_SegmentRomEnd;
			dprintf"TOYSHOP LEVEL 3 TEXTURE BANK - "));
			break;
		case BOSSA_TOYSHOP_TEX_BANK:
			bankRomStart	= (u32)&_texBank_7_4_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_7_4_SegmentRomEnd;
			dprintf"TOYSHOP BOSS TEXTURE BANK - "));
			break;
		case BONUS_TOYSHOP_TEX_BANK:
			bankRomStart	= (u32)&_texBank_7_5_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_7_5_SegmentRomEnd;
			dprintf"TOYSHOP BONUS TEXTURE BANK - "));
			break;
		case MULTI_TOYSHOP_TEX_BANK:
			bankRomStart	= (u32)&_texBank_7_6_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_7_6_SegmentRomEnd;
			dprintf"TOYSHOP MULTIPLAYER TEXTURE BANK - "));
			break;

		// HALLOWEEN TEXTURES --------------------------------------------------------------------
		case GENERIC_HALLOWEEN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_8_0_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_8_0_SegmentRomEnd;
			dprintf"GENERIC HALLOWEEN TEXTURE BANK - "));
			break;
		case LEVEL1_HALLOWEEN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_8_1_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_8_1_SegmentRomEnd;
			dprintf"HALLOWEEN LEVEL 1 TEXTURE BANK - "));
			break;
		case LEVEL2_HALLOWEEN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_8_2_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_8_2_SegmentRomEnd;
			dprintf"HALLOWEEN LEVEL 2 TEXTURE BANK - "));
			break;
		case LEVEL3_HALLOWEEN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_8_3_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_8_3_SegmentRomEnd;
			dprintf"HALLOWEEN LEVEL 3 TEXTURE BANK - "));
			break;
		case BOSSA_HALLOWEEN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_8_4_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_8_4_SegmentRomEnd;
			dprintf"HALLOWEEN BOSS TEXTURE BANK - "));
			break;
		case BONUS_HALLOWEEN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_8_5_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_8_5_SegmentRomEnd;
			dprintf"HALLOWEEN BONUS TEXTURE BANK - "));
			break;
		case MULTI_HALLOWEEN_TEX_BANK:
			bankRomStart	= (u32)&_texBank_8_6_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_8_6_SegmentRomEnd;
			dprintf"HALLOWEEN MULTIPLAYER TEXTURE BANK - "));
			break;

		// RETRO TEXTURES ------------------------------------------------------------------------
		case GENERIC_RETRO_TEX_BANK:
			bankRomStart	= (u32)&_texBank_9_0_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_9_0_SegmentRomEnd;
			dprintf"GENERIC RETRO TEXTURE BANK - "));
			break;
		case LEVEL1_RETRO_TEX_BANK:
			bankRomStart	= (u32)&_texBank_9_1_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_9_1_SegmentRomEnd;
			dprintf"RETRO LEVEL 1 TEXTURE BANK - "));
			break;
		case LEVEL2_RETRO_TEX_BANK:
			bankRomStart	= (u32)&_texBank_9_2_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_9_2_SegmentRomEnd;
			dprintf"RETRO LEVEL 2 TEXTURE BANK - "));
			break;
		case LEVEL3_RETRO_TEX_BANK:
			bankRomStart	= (u32)&_texBank_9_3_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_9_3_SegmentRomEnd;
			dprintf"RETRO LEVEL 3 TEXTURE BANK - "));
			break;
		case BOSSA_RETRO_TEX_BANK:
			bankRomStart	= (u32)&_texBank_9_4_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_9_4_SegmentRomEnd;
			dprintf"RETRO BOSS TEXTURE BANK - "));
			break;
		case BONUS_RETRO_TEX_BANK:
			bankRomStart	= (u32)&_texBank_9_5_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_9_5_SegmentRomEnd;
			dprintf"RETRO BONUS TEXTURE BANK - "));
			break;
		case MULTI_RETRO_TEX_BANK:
			bankRomStart	= (u32)&_texBank_9_6_SegmentRomStart;
			bankRomEnd		= (u32)&_texBank_9_6_SegmentRomEnd;
			dprintf"RETRO MULTIPLAYER TEXTURE BANK - "));
			break;
			

		default:
			dprintf"Invalid texture bank!\n"));
			for(;;);
			break;
	}
	bankSize = bankRomEnd - bankRomStart;

	if(num != SYSTEM_TEX_BANK)
		textureBank = (char *)JallocAlloc(DMAGetSize(bankRomStart,bankRomEnd),YES,"TXTRBANK");
	else
	{
		smallFont->data = (char *)JallocAlloc(DMAGetSize(bankRomStart,bankRomEnd),YES,"SYSTXTR");
		oldeFont->data = &smallFont->data[0];
		textureBank = &smallFont->data[0];
	}

	DMAMemory(textureBank, bankRomStart, bankRomEnd);
	if(textureBank)
		dprintf"LOADED OK (size: %d)\n",bankSize));
	else
	{
		dprintf"ERROR : UNABLE TO LOAD TEXTURE BANK\n"));
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
void FindTexture(TEXTURE **texPtr, int texID, BOOL report)
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
		dprintf"Whoops, texture '%X' not found!\n",texID));

//	(*texPtr) = &fadeTexture;
	return;
}


/*	--------------------------------------------------------------------------------
	Function 	: AnimateTexture
	Purpose 	: animates a texture for a drawlisted object
	Parameters 	: TEXTURE_ANIMATION *,Gfx *
	Returns 	: void
	Info 		:
*/
void AnimateTexture(TEXTURE_ANIMATION *anim,Gfx *dl)
{
	int lastFrame;
	float speed;

//	if(gamePaused)
//		return;

	lastFrame = anim->currentFrame;
	speed = 32 * GAME_SPEED;
	anim->counter -= speed;
	while(anim->counter <= 0)
	{
		if(anim->flags & TEX_ANIM_FLAGS_REVERSE)
		{
			anim->currentFrame--;
			if(anim->currentFrame >= anim->frameList->numFrames)
			{
				if(anim->flags & TEX_ANIM_FLAGS_FLIPFLOP)
				{
					anim->currentFrame = 1;
					anim->flags &= -1 - TEX_ANIM_FLAGS_REVERSE;
				}
				else
					anim->currentFrame = anim->frameList->numFrames-1;
			}
		}
		else
		{
			anim->currentFrame++;
			if(anim->currentFrame >= anim->frameList->numFrames)
			{
				if(anim->flags & TEX_ANIM_FLAGS_FLIPFLOP)
				{
					anim->currentFrame = anim->frameList->numFrames - 2;
					anim->flags |= TEX_ANIM_FLAGS_REVERSE;
				}
				else
		 			anim->currentFrame = 0;
			}
		}

		anim->counter += anim->delay;
	}
/*
	if(lastFrame != anim->currentFrame)
	{
		if(anim->frameList == &spriteFrameList[WATER_ANIM])
			ReplaceTextureInDrawList(dl,NULL,(u32)(*(anim->frameList->texture + anim->currentFrame))->data, YES);
		else
			ReplaceTextureInDrawList(dl,(u32)(*(anim->frameList->texture + lastFrame))->data,(u32)(*(anim->frameList->texture + anim->currentFrame))->data, NO);

	}
*/
}

