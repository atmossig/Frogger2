/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: font.h
	Programmer	: Andrew Eder
	Date		: 10/28/98

----------------------------------------------------------------------------------------------- */

#ifndef FONT_H_INCLUDED
#define FONT_H_INCLUDED


#include "types2dgfx.h"


//----- [ DATA STRUCTURES ] --------------------------------------------------------------------//

typedef struct _TEXTLIST2
{
	const char *text;
	unsigned long x,y;
	unsigned long sinAmp,sinSpd,sinStart,sinEnd;
	float flashSpd;
	unsigned long kill;
	struct _TEXTLIST2 *next;
} TEXTLIST2;

extern TEXTLIST2 *textList;

extern FONT *bigFont;
extern FONT *smallFont;
extern FONT *oldeFont;
extern FONT	*currFont;

// ----- [ FONT PRINTING DATA ] ----- //

extern unsigned char characterMap[256];

extern unsigned long letters32_offset[];
extern unsigned long letters16_offset[];
extern unsigned long letters8_offset[];
extern unsigned char letters_spacing[];

// ----- [ FUNCTION PROTOTYPES ] ----- //

void InitFont();


#endif
