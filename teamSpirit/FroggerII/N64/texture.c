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

		case GENERIC_TEX_BANK:
			bankRomStart	= (u32)&_texBank2SegmentRomStart;
			bankRomEnd		= (u32)&_texBank2SegmentRomEnd;
			sprintf(message,"GEN_TB");
			break;

		case GENERIC_GARDEN_TEX_BANK:
				bankRomStart	= (u32)&_texBank3SegmentRomStart;
				bankRomEnd		= (u32)&_texBank3SegmentRomEnd;
				sprintf ( message, "GEN_GAR_TB" );
			break;
		case LEVEL1_GARDEN_TEX_BANK:
				bankRomStart	= (u32)&_texBank4SegmentRomStart;
				bankRomEnd		= (u32)&_texBank4SegmentRomEnd;
				sprintf ( message, "LEV1_GAR_TB" );
			break;
		case LEVEL2_GARDEN_TEX_BANK:
				bankRomStart	= (u32)&_texBank5SegmentRomStart;
				bankRomEnd		= (u32)&_texBank5SegmentRomEnd;
				sprintf ( message, "LEV2_GAR_TB" );
			break;
		case LEVEL3_GARDEN_TEX_BANK:
				bankRomStart	= (u32)&_texBank6SegmentRomStart;
				bankRomEnd		= (u32)&_texBank6SegmentRomEnd;
				sprintf ( message, "LEV3_GAR_TB" );
			break;
		case BOSSA_GARDEN_TEX_BANK:
				bankRomStart	= (u32)&_texBank7SegmentRomStart;
				bankRomEnd		= (u32)&_texBank7SegmentRomEnd;
				sprintf ( message, "BOA_GAR_TB" );
			break;
		case BOSSB_GARDEN_TEX_BANK:
				bankRomStart	= (u32)&_texBank8SegmentRomStart;
				bankRomEnd		= (u32)&_texBank8SegmentRomEnd;
				sprintf ( message, "BOB_GAR_TB" );
			break;
		case BONUS_GARDEN_TEX_BANK:
				bankRomStart	= (u32)&_texBank9SegmentRomStart;
				bankRomEnd		= (u32)&_texBank9SegmentRomEnd;
				sprintf ( message, "BON_GAR_TB" );
			break;
		case MULTI_GARDEN_TEX_BANK:
				bankRomStart	= (u32)&_texBank10SegmentRomStart;
				bankRomEnd		= (u32)&_texBank10SegmentRomEnd;
				sprintf ( message, "MUL_GAR_TB" );
			break;



		case GENERIC_ANCIENT_TEX_BANK:
				bankRomStart	= (u32)&_texBank11SegmentRomStart;
				bankRomEnd		= (u32)&_texBank11SegmentRomEnd;
				sprintf ( message, "GEN_ANC_TB" );
			break;
		case LEVEL1_ANCIENT_TEX_BANK:
				bankRomStart	= (u32)&_texBank12SegmentRomStart;
				bankRomEnd		= (u32)&_texBank12SegmentRomEnd;
				sprintf ( message, "LEV1_ANC_TB" );
			break;
		case LEVEL2_ANCIENT_TEX_BANK:
				bankRomStart	= (u32)&_texBank13SegmentRomStart;
				bankRomEnd		= (u32)&_texBank13SegmentRomEnd;
				sprintf ( message, "LEV2_ANC_TB" );
			break;
		case LEVEL3_ANCIENT_TEX_BANK:
				bankRomStart	= (u32)&_texBank14SegmentRomStart;
				bankRomEnd		= (u32)&_texBank14SegmentRomEnd;
				sprintf ( message, "LEV3_ANC_TB" );
			break;
		case BOSSA_ANCIENT_TEX_BANK:
				bankRomStart	= (u32)&_texBank15SegmentRomStart;
				bankRomEnd		= (u32)&_texBank15SegmentRomEnd;
				sprintf ( message, "BOSA_ANC_TB" );
			break;
		case BOSSB_ANCIENT_TEX_BANK:
				bankRomStart	= (u32)&_texBank16SegmentRomStart;
				bankRomEnd		= (u32)&_texBank16SegmentRomEnd;
				sprintf ( message, "BOSB_ANC_TB" );
			break;
		case BOSSC_ANCIENT_TEX_BANK:
				sprintf ( message, "BOSC_ANC_TB" );
			break;
		case BOSSD_ANCIENT_TEX_BANK:
				sprintf ( message, "BOSD_ANC_TB" );
			break;
		case BOSSE_ANCIENT_TEX_BANK:
				sprintf ( message, "BOSE_ANC_TB" );
			break;
		case BONUS_ANCIENT_TEX_BANK:
				sprintf ( message, "BON_ANC_TB" );
			break;
		case MULTI_ANCIENT_TEX_BANK:
				sprintf ( message, "MUL_ANC_TB" );
			break;


		case GENERIC_SPACE_TEX_BANK:
				sprintf ( message, "GEN_SPA_TB" );
			break;
		case LEVEL1_SPACE_TEX_BANK:
				sprintf ( message, "LEV1_SPA_TB" );
			break;
		case LEVEL2_SPACE_TEX_BANK:
				sprintf ( message, "LEV2_SPA_TB" );
			break;
		case LEVEL3_SPACE_TEX_BANK:
				sprintf ( message, "LEV3_SPA_TB" );
			break;
		case BOSSA_SPACE_TEX_BANK:
				sprintf ( message, "BOSA_SPA_TB" );
			break;
		case BONUS_SPACE_TEX_BANK:
				sprintf ( message, "BON_SPA_TB" );
			break;
		case MULTI_SPACE_TEX_BANK:
				sprintf ( message, "MUL_SPA_TB" );
			break;


		case GENERIC_CITY_TEX_BANK:
				sprintf ( message, "GEN_CIT_TB" );
			break;
		case LEVEL1_CITY_TEX_BANK:
				sprintf ( message, "LEV1_CIT_TB" );
			break;
		case LEVEL2_CITY_TEX_BANK:
				sprintf ( message, "LEV2_CIT_TB" );
			break;
		case LEVEL3_CITY_TEX_BANK:
				sprintf ( message, "LEV3_CIT_TB" );
			break;
		case BOSSA_CITY_TEX_BANK:
				sprintf ( message, "BOSA_CIT_TB" );
			break;
		case BONUS_CITY_TEX_BANK:
				sprintf ( message, "BON_CIT_TB" );
			break;
		case MULTI_CITY_TEX_BANK:
				sprintf ( message, "MUL_CIT_TB" );
			break;


		case GENERIC_SUBTERRANEAN_TEX_BANK:
				sprintf ( message, "GEN_SUB_TB" );
			break;
		case LEVEL1_SUBTERRANEAN_TEX_BANK:
				sprintf ( message, "LEV1_SUB_TB" );
			break;
		case LEVEL2_SUBTERRANEAN_TEX_BANK:
				sprintf ( message, "LEV2_SUB_TB" );
			break;
		case LEVEL3_SUBTERRANEAN_TEX_BANK:
				sprintf ( message, "LEV3_SUB_TB" );
			break;
		case BOSSA_SUBTERRANEAN_TEX_BANK:
				sprintf ( message, "BOSA_SUB_TB" );
			break;
		case BONUS_SUBTERRANEAN_TEX_BANK:
				sprintf ( message, "BON_SUB_TB" );
			break;
		case MULTI_SUBTERRANEAN_TEX_BANK:
				sprintf ( message, "MUL_SUB_TB" );
			break;


		case GENERIC_LABORATORY_TEX_BANK:
				sprintf ( message, "GEN_LAB_TB" );
			break;
		case LEVEL1_LABORATORY_TEX_BANK:
				sprintf ( message, "LEV1_LAB_TB" );
			break;
		case LEVEL2_LABORATORY_TEX_BANK:
				sprintf ( message, "LEV2_LAB_TB" );
			break;
		case LEVEL3_LABORATORY_TEX_BANK:
				sprintf ( message, "LEV3_LAB_TB" );
			break;
		case BOSSA_LABORATORY_TEX_BANK:
				sprintf ( message, "BOSA_LAB_TB" );
			break;
		case BONUS_LABORATORY_TEX_BANK:
				sprintf ( message, "BON_LAB_TB" );
			break;
		case MULTI_LABORATORY_TEX_BANK:
				sprintf ( message, "MUL_LAB_TB" );
			break;


		case GENERIC_TOYSHOP_TEX_BANK:
				sprintf ( message, "GEN_TOY_TB" );
			break;
		case LEVEL1_TOYSHOP_TEX_BANK:
				sprintf ( message, "LEV1_TOY_TB" );
			break;
		case LEVEL2_TOYSHOP_TEX_BANK:
				sprintf ( message, "LEV2_TOY_TB" );
			break;
		case LEVEL3_TOYSHOP_TEX_BANK:
				sprintf ( message, "LEV3_TOY_TB" );
			break;
		case BOSSA_TOYSHOP_TEX_BANK:
				sprintf ( message, "BOSA_TOY_TB" );
			break;
		case BONUS_TOYSHOP_TEX_BANK:
				sprintf ( message, "BON_TOY_TB" );
			break;
		case MULTI_TOYSHOP_TEX_BANK:
				sprintf ( message, "MUL_TOY_TB" );
			break;


		case GENERIC_HALLOWEEN_TEX_BANK:
				sprintf ( message, "GEN_HAL_TB" );
			break;
		case LEVEL1_HALLOWEEN_TEX_BANK:
				sprintf ( message, "LEV1_HAL_TB" );
			break;
		case LEVEL2_HALLOWEEN_TEX_BANK:
				sprintf ( message, "LEV2_HAL_TB" );
			break;
		case LEVEL3_HALLOWEEN_TEX_BANK:
				sprintf ( message, "LEV3_HAL_TB" );
			break;
		case BOSSA_HALLOWEEN_TEX_BANK:
				sprintf ( message, "BOSA_HAL_TB" );
			break;
		case BONUS_HALLOWEEN_TEX_BANK:
				sprintf ( message, "BON_HAL_TB" );
			break;
		case MULTI_HALLOWEEN_TEX_BANK:
				sprintf ( message, "MUL_HAL_TB" );
			break;


		case GENERIC_RETRO_TEX_BANK:
				sprintf ( message, "GEN_RET_TB" );
			break;
		case LEVEL1_RETRO_TEX_BANK:
				sprintf ( message, "LEV1_RET_TB" );
			break;
		case LEVEL2_RETRO_TEX_BANK:
				sprintf ( message, "LEV2_RET_TB" );
			break;
		case LEVEL3_RETRO_TEX_BANK:
				sprintf ( message, "LEV3_RET_TB" );
			break;
		case BOSSA_RETRO_TEX_BANK:
				sprintf ( message, "BOSA_RET_TB" );
			break;
		case BONUS_RETRO_TEX_BANK:
				sprintf ( message, "BON_RET_TB" );
			break;
		case MULTI_RETRO_TEX_BANK:
				sprintf ( message, "MUL_RET_TB" );
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
