#define LOADPAL_PRINT_OUT_PALETTE

#include "Ultra64.h"
#include <stdio.h>

#include <isltex.h>
#include <islutil.h>
#include <islmem.h>

#include "LayOut.h"
#include "Textures.h"

#include "Main.h"

int numTextureBanks = 0;

//char *palNames [ 5 ] = { "BFG01", "BFG02", "BFG03", "BFG04", "BFG05"};
char *palNames [ 5 ] = { "GREENBABY.PAL", "YELLOWBABY.PAL", "BLUEBABY.PAL", "PINKBABY.PAL", "REDBABY.PAL"};

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

		case SUPERRETRO_TEX_BANK:
				sprintf ( fileName, "TEXTURES\\SUPER.SPT" );
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





static int LOADPAL_LoadPCPalette(char * const file, LOADPAL_PCPalette16Str * const pc_palette);
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
signed short LOADPAL_Load16(char * const file)
{
	signed short psx_palette[16];
	LOADPAL_PCPalette16Str pc_palette;
	int clut;

	// Load the palette into 24bit rgb components
	if (!LOADPAL_LoadPCPalette(file, &pc_palette))
		return 0;

	// Now convert the pc palette to the playstation version
	{
		int i;
		LOADPAL_24BitRGBStr *element;
		for (i=0;i<16;i++)
		{
			element = &pc_palette.component[i];

#ifdef LOADPAL_PRINT_OUT_PALETTE
			utilPrintf("pc rgb = %d, %d, %d\n", element->r, element->g, element->b);
#endif
			// Divide each element by 8 to get in the range 0-31
			// Remember the psx holds elements in the order b,g,r
			psx_palette[i] = ((element->r/8) << 0) +
							 ((element->g/8) << 5) +
							 ((element->b/8) << 10);
		}
	}

	// Finally try and load this PSX palette into VRAM
	clut = textureAddCLUT16(&psx_palette[0]);

	return clut;
}
//////////////////////////////////////////////////////////////////
//					Internal functions							//
//////////////////////////////////////////////////////////////////
static int LOADPAL_LoadPCPalette(char * const file, 
							   LOADPAL_PCPalette16Str * const pc_palette)
{
	unsigned char *palette = NULL;
	unsigned char *file_ptr;
	int file_length;
	int i;

	if (!file)
	{
		utilPrintf("ERROR: File address passed to palette load is NULL.\n");
		utilPrintf("Palette not loaded.\n");
		return 0;
	}

	palette = fileLoad(file, &file_length);

	if (!palette)
	{
		utilPrintf("ERROR: Could not load palette file %s.\n", file);
		utilPrintf("Palette not loaded.\n");
		return 0;
	}

	// Let's just check the 1st 3 letters for the right format
	if ( (palette[0] != 'J') ||
		 (palette[1] != 'A') ||
		 (palette[2] != 'S') )
	{
		utilPrintf("ERROR: Palette file is not in JASC format.\n");
		utilPrintf("Use Paint Shop Pro or Batchmaster.\n");
		utilPrintf("Palette not loaded.\n");
		return 0;
	}

	// go past the string and version number
	file_ptr = palette + 16;

	// Find out how many colours there are
	{
		unsigned char nb;			// next byte
		int nc;						// number of colours

		nb = *file_ptr;
		nc = nb - 0x30;
		file_ptr++;
		nb = *file_ptr;
		// Keep on readin in bytes until return is found
		while (nb != 0x0D)
		{
			nc *= 10;
			nc += (nb - 0x30);
			file_ptr++;
			nb = *file_ptr;
		}
		// If the number of colours is not equal to 16 throw an error
		if (nc != 16)
		{
			utilPrintf("ERROR: Palette has %d instead of 16 colours.\n");
			utilPrintf("Palette not loaded.\n");
			return 0;
		}
	}

	// increment again past the newline
	file_ptr+=2;

	// The components should be next
	for (i=0;i<16;i++)
	{
		unsigned char nb;		// next byte
		unsigned char r,g,b;

		// initialise these vars
		// get the r component (separated by a space from the green)
		nb = *file_ptr;
		r = (nb - 0x30);
		file_ptr++;
		nb = *file_ptr;
		while (nb != 0x20)
		{
			r *= 10;
			r += (nb - 0x30);
			file_ptr++;
			nb = *file_ptr;
		}
		// Go past the space
		file_ptr++;
		// get the g component (separated by a space from the green)
		nb = *file_ptr;
		g = (nb - 0x30);
		file_ptr++;
		nb = *file_ptr;
		while (nb != 0x20)
		{
			g *= 10;
			g += (nb - 0x30);
			file_ptr++;
			nb = *file_ptr;
		}
		// Go past the space
		file_ptr++;
		// get the b component (separated by a return and newline)
		nb = *file_ptr;
		b = (nb - 0x30);
		file_ptr++;
		nb = *file_ptr;
		while (nb != 0x0D)
		{
			b *= 10;
			b += (nb - 0x30);
			file_ptr++;
			nb = *file_ptr;
		}
		// Now get past the newline
		file_ptr += 2;

		// Finally store in the pc palette
		pc_palette->component[i].r = r;
		pc_palette->component[i].g = g;
		pc_palette->component[i].b = b;
	}

	FREE(palette);

	return file_length;
}
//////////////////////////////////////////////////////////////////
