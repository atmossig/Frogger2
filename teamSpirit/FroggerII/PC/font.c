/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: font.c
	Programmer	: Andrew Eder
	Date		: 10/28/98

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>

#include "incs.h"


// ----- [ FONT PRINTING DATA ] ----- //

TEXTLIST2 *textList = NULL;

FONT *bigFont		= NULL;
FONT *smallFont		= NULL;
FONT *oldeFont		= NULL;
FONT *currFont		= NULL;


// character mapping array
unsigned char characterMap[256] =
{
	 0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  
 	 0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0,  

	 0,  0,  'x'-'a',  0,  0,  0,  0,  0, 
	 
	 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,  0,  0,  0, 36,
	 0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,  0,  0,  0,  0,
	 0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// Offset of letters in font data
unsigned long letters32_offset[] =
{
	(0*(512+72)),	(1*(512+72)),	(2*(512+72)),	(3*(512+72)),
	(4*(512+72)),	(5*(512+72)),	(6*(512+72)),	(7*(512+72)),
	(8*(512+72)),	(9*(512+72)),	(10*(512+72)),	(11*(512+72)),
	(12*(512+72)),	(13*(512+72)),	(14*(512+72)),	(15*(512+72)),
	(16*(512+72)),	(17*(512+72)),	(18*(512+72)),	(19*(512+72)),
	(20*(512+72)),	(21*(512+72)),	(22*(512+72)),	(23*(512+72)),
	(24*(512+72)),	(25*(512+72)),	(26*(512+72)),	(27*(512+72)),
	(28*(512+72)),	(29*(512+72)),	(30*(512+72)),	(31*(512+72)),
	(32*(512+72)),	(33*(512+72)),	(34*(512+72)),	(35*(512+72)),
};
unsigned long letters16_offset[] =
{
	(0*(128+72)),	(1*(128+72)),	(2*(128+72)),	(3*(128+72)),
	(4*(128+72)),	(5*(128+72)),	(6*(128+72)),	(7*(128+72)),
	(8*(128+72)),	(9*(128+72)),	(10*(128+72)),	(11*(128+72)),
	(12*(128+72)),	(13*(128+72)),	(14*(128+72)),	(15*(128+72)),
	(16*(128+72)),	(17*(128+72)),	(18*(128+72)),	(19*(128+72)),
	(20*(128+72)),	(21*(128+72)),	(22*(128+72)),	(23*(128+72)),
	(24*(128+72)),	(25*(128+72)),	(26*(128+72)),	(27*(128+72)),
	(28*(128+72)),	(29*(128+72)),	(30*(128+72)),	(31*(128+72)),
	(32*(128+72)),	(33*(128+72)),	(34*(128+72)),	(35*(128+72)),
	(36*(128+72)),	(37*(128+72)),	(38*(128+72)),	(39*(128+72)),
	(40*(128+72)),	(41*(128+72)),	(42*(128+72)),	(43*(128+72)),
	(44*(128+72)),	(45*(128+72)),	(46*(128+72)),	(47*(128+72)),
};
unsigned long letters8_offset[] =
{
	(0*(128+72)),	(1*(128+72)),	(2*(128+72)),	(3*(128+72)),
	(4*(128+72)),	(5*(128+72)),	(6*(128+72)),	(7*(128+72)),
	(8*(128+72)),	(9*(128+72)),	(10*(128+72)),	(11*(128+72)),
	(12*(128+72)),	(13*(128+72)),	(14*(128+72)),	(15*(128+72)),
	(16*(128+72)),	(17*(128+72)),	(18*(128+72)),	(19*(128+72)),
	(20*(128+72)),	(21*(128+72)),	(22*(128+72)),	(23*(128+72)),
	(24*(128+72)),	(25*(128+72)),	(26*(128+72)),	(27*(128+72)),
	(28*(128+72)),	(29*(128+72)),	(30*(128+72)),	(31*(128+72)),
	(32*(128+72)),	(33*(128+72)),	(34*(128+72)),	(35*(128+72)),
};


// Spacing (width) of letters in the font
unsigned char letters_spacing[] =
{
	22,22,22,22,22,22,	// ABCDEF
	22,22,22,22,22,22,	// GHIJKL
	22,22,22,22,22,22,	// MNOPQR
	22,22,22,22,22,22,	// STUVWX
	22,22,22,22,22,22,	// YZ0123
	22,22,22,22,22,22,	// 456789
	22,22,22,22,22,22,	// ?:,!.(
	22,22,22,22,22,		// +"/-*
};



// ----- [ FUNCTION IMPLEMENTATIONS ] ----- //

void InitFont()
{
	unsigned int i,j,currTile;
	char filenameBuffer[MAX_PATH];
	
	bigFont		= (FONT *)JallocAlloc(sizeof(FONT),YES,"FONT32");
	smallFont	= (FONT *)JallocAlloc(sizeof(FONT),YES,"FONT16");
	oldeFont	= (FONT *)JallocAlloc(sizeof(FONT),YES,"FONT8");

	// Initialise the 32x32 font ----------------------------------------------------

	dprintf"Initialising 32x32 font..."));
	sprintf(filenameBuffer,"%s%sfont\\font32x32.bmp",baseDirectory,TEXTURE_BASE);

	bigFont->width		= 32;
	bigFont->height		= 32;
	bigFont->data		= GetGelfBmpDataAsShortPtr(filenameBuffer);

	for(i=0; i<MAX_FONT_CHARS; i++)
	{
		// set spacing for individual font chars
		bigFont->xSpacing[i] = 24;
		bigFont->ySpacing[i] = 24;
	}

	// determine offsets into bitmap data for each character in the font
	for(i=0; i<8; i++)
	{
		// characters A -> H inclusive
		bigFont->offset[i].v[X] = (i * bigFont->width);
		bigFont->offset[i].v[Y] = 0;

		// characters I -> P inclusive
		bigFont->offset[i+8].v[X] = (i * bigFont->width);
		bigFont->offset[i+8].v[Y] = bigFont->height;

		// charcaters Q -> X inclusive
		bigFont->offset[i+16].v[X] = (i * bigFont->width);
		bigFont->offset[i+16].v[Y] = (2 * bigFont->height);

		// characters Y -> Z inclusive
		if(i < 2)
		{
			bigFont->offset[i+24].v[X] = (i * bigFont->width);
			bigFont->offset[i+24].v[Y] = (3 * bigFont->height);
		}

		// numbers 0 -> 7 inclusive
		bigFont->offset[i+32].v[X] = (i * bigFont->width);
		bigFont->offset[i+32].v[Y] = (4 * bigFont->height);

		// numbers 8 -> 9 inclusive
		if(i < 2)
		{
			bigFont->offset[i+40].v[X] = (i * bigFont->width);
			bigFont->offset[i+40].v[Y] = (5 * bigFont->height);
		}
	}

	dprintf"OK !\n"));

/*	// Initialise the 16x16 font ----------------------------------------------------

	dprintf"Initialising 16x16 font..."));
	sprintf(filenameBuffer,"%s%sfont\\font16x16.bmp",baseDirectory,TEXTURE_BASE);

	smallFont->width	= 16;
	smallFont->height	= 16;
	smallFont->data		= GetGelfBmpDataAsShortPtr(filenameBuffer);

	for(i=0; i<MAX_FONT_CHARS; i++)
	{
		// set spacing for individual font chars
		smallFont->xSpacing[i] = 11;
		smallFont->ySpacing[i] = 11;
	}

	// determine offsets into bitmap data for each character in the font
	for(i=0; i<16; i++)
	{
		// characters A -> P inclusive
		smallFont->offset[i].v[X] = (i * smallFont->width);
		smallFont->offset[i].v[Y] = 0;

		// charcaters Q -> Z inclusive
		if(i < 10)
		{
			smallFont->offset[i+16].v[X] = (i * smallFont->width);
			smallFont->offset[i+16].v[Y] = smallFont->height;
		}

		// numbers 0 -> 9 inclusive
		if(i < 10)
		{
			smallFont->offset[i+32].v[X] = (i * smallFont->width);
			smallFont->offset[i+32].v[Y] = (2 * smallFont->height);
		}
	}

	dprintf"OK !\n"));

	// Initialise the 8x8 font ----------------------------------------------------

	dprintf"Initialising 8x8 font...(NOT USED IN PC VERSION YET !) "));

	dprintf"OK !\n"));
*/	
	dprintf"Initialising font texture surfaces\n"));

	bigFont->srf		= CreateTextureSurface (256,256,bigFont->data,1,0,0);
	bigFont->hdl		= ConvertSurfaceToTexture (bigFont->srf);

	//smallFont->srf		= CreateTextureSurface (256,256,smallFont->data,1,0,0);
	//smallFont->hdl	= ConvertSurfaceToTexture (smallFont->srf);

	smallFont = bigFont;
	
	// Make 16x16 font the current font
	currFont = smallFont;
}
