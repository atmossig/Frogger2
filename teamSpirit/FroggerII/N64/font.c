/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: font.c
	Programmer	: Andrew Eder
	Date		: 10/28/98

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

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
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	 0, 39, 43,  0,  0,  0,  0,  0, 41, 47, 46, 42, 38, 45, 40, 44, 
	26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 37,  0,  0,  0,  0, 36,
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
		
	bigFont		= (FONT *)JallocAlloc(sizeof(FONT),YES,"FONT32");
	smallFont	= (FONT *)JallocAlloc(sizeof(FONT),YES,"FONT16");
	oldeFont	= (FONT *)JallocAlloc(sizeof(FONT),YES,"FONT8");

	// Initialise the 32x32 font ----------------------------------------------------

	bigFont->width		= 32;
	bigFont->height		= 32;
	bigFont->palOffset	= 32*32/2;	//20996;
	bigFont->palOffset	+= 4;

	for(i=0; i<MAX_FONT_CHARS; i++)
	{
		bigFont->offset[i] = FONT32_DATACHUNK_OFFSET + letters32_offset[i];
		
		// Set spacing characteristics for individual font chars
		bigFont->xSpacing[i] = letters_spacing[i];
		bigFont->ySpacing[i] = 48;
	}

	// Initialise the 16x16 font ----------------------------------------------------

	smallFont->width		= 16;
	smallFont->height		= 16;
	smallFont->palOffset	= 16*16/2;	//7172;
	smallFont->palOffset	+= 4;

	for(i=0; i<MAX_FONT_CHARS; i++)
	{
		smallFont->offset[i] = FONT16_DATACHUNK_OFFSET + letters16_offset[i];
		
		// Set spacing characteristics for individual font chars
		smallFont->xSpacing[i] = 11;
		smallFont->ySpacing[i] = 24;
	}

	// Initialise the 8x8 font ----------------------------------------------------

	oldeFont->width		= 16;
	oldeFont->height	= 16;
	oldeFont->palOffset	= 16*16/2;	//7172;
	oldeFont->palOffset	+= 4;

	for(i=0; i<MAX_FONT_CHARS; i++)
	{
		oldeFont->offset[i] = FONT8_DATACHUNK_OFFSET + letters8_offset[i];
		
		// Set spacing characteristics for individual font chars
		oldeFont->xSpacing[i] = 7;
		oldeFont->ySpacing[i] = 12;
	}

	// Make 16x16 font the current font
	currFont = smallFont;
}
