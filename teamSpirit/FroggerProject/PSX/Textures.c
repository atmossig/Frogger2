#include "Ultra64.h"
#include <stdio.h>

#include <isltex.h>
#include <islutil.h>

#include "LayOut.h"
#include "Textures.h"

#include "Main.h"

int numTextureBanks = 0;

TextureBankType *textureBanks [ MAX_TEXTURE_BANKS ] = { NULL, NULL, NULL, NULL, NULL };

void LoadTextureBank ( int textureBank )
{
	char fileName[256];

	switch ( textureBank )
	{
		case GARDEN_TEX_BANK:
				sprintf ( fileName, "TEXTURES\\GARDEN.SPT" );
			break;

		case ANCIENT_TEX_BANK:
				sprintf ( fileName, "TEXTURES\\ANCIENTS.SPT" );
			break;

		case SPACE_TEX_BANK:
				sprintf ( fileName, "TEXTURES\\SPACE.SPT" );
			break;

		case CITY_TEX_BANK:
				sprintf ( fileName, "TEXTURES\\CITY.SPT" );
			break;

		case SUBTERRANEAN_TEX_BANK:
				sprintf ( fileName, "TEXTURES\\SUB.SPT" );
			break;

		case LABORATORY_TEX_BANK:
				sprintf ( fileName, "TEXTURES\\LAB.SPT" );
			break;

		case HALLOWEEN_TEX_BANK:
				sprintf ( fileName, "TEXTURES\\HALLOWEEN.SPT" );
			break;

		case FRONTEND_TEX_BANK:
				sprintf ( fileName, "TEXTURES\\HUB.SPT" );
			break;

		case INGAMEGENERIC_TEX_BANK:
				sprintf ( fileName, "TEXTURES\\GENERIC.SPT" );
			break;

		case TITLES_TEX_BANK:
				sprintf ( fileName, "TEXTURES\\TITLES.SPT" );
			break;
	}
	// ENDSWITCH - textureBank

	if ( numTextureBanks < MAX_TEXTURE_BANKS )
	{
		textureBanks [ numTextureBanks ] = textureLoadBank ( fileName );

		textureDownloadBank ( textureBanks [ numTextureBanks ]   );
		textureDestroyBank  ( textureBanks [ numTextureBanks++ ] );
	}
	else
	{
		utilPrintf("Error Loading: %s, Max Texture Banks Reached", fileName );
	}
	// ENDELSEIF - 	if ( numTextureBanks < MAX_TEXTURE_BANKS )
}


void FreeTextureBank ( TextureBankType *textureBank )
{
	textureDestroyBank	( textureBank );
	textureUnloadBank		( textureBank );
}

void FreeAllTextureBanks ( void )
{
	short c;

	utilPrintf("Freeing All Texture Banks : TEXTURE\n");

	for ( c = 0; c < MAX_TEXTURE_BANKS; c++ )
	{
		if ( textureBanks[c] )
			FreeTextureBank ( textureBanks [c] );
		// ENDIF - if ( textureBanks[c] )
		textureBanks[c] = NULL;
	}
	// ENDIF - 	for ( c = o; c < MAX_TEXTURE_BANKS; c++ )
	numTextureBanks = 0;
	//textureDestroy();
}

