/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: language.c
	Programmer	: James Healey
	Date		: 31/03/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"

#ifdef PC_VERSION
char *DIKStrings[256] = 
{
	"",
	"Escape",			// 0
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"0",				// 10 DIK_UP
	"-",
	"=",
	"Backspace",
	"Tab",
	"q",
	"w",
	"e",
	"r",
	"t",
	"y",				// 20
	"u",
	"i",
	"o",
	"p",
	"[",
	"]",
	"Return",
	"Left Control",
	"a",
	"s",				// 30
	"d",
	"f",
	"g",
	"h",
	"j",
	"k",
	"l",
	";",
	"'",
	"Grave",
	"Left Shift",
	"Backslash",
	"z",
	"x",
	"c",
	"v",
	"b",
	"n",
	"m",
	",",				// 50
	".",
	"/",
	"Right Shift",
	"*",
	"Left Alt",
	"Space",
	"Caps Lock",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"NumLock",
	"Scroll Lock",
	"Numpad 7",
	"Numpad 8",
	"Numpad 9",
	"-",
	"Numpad 4",
	"Numpad 5",
	"Numpad 6",
	"+",
	"Numpad 1",
	"Numpad 2",
	"Numpad 3",
	"Numpad 0",
	".",
	"",
	"",
	"",
	"F11",
	"F12",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"F13",
	"F14",
	"F15",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"Kana",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"Convert",
	"",
	"No Convert",
	"",
	"Yen",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"=",
	"",
	"",
	"Circumflex",
	"@",
	":",
	"Underline",
	"Kanji",
	"Stop",
	"AX",
	"Unlabeled",		// 100
	"",
	"",
	"",
	"",
	"Numpad Enter",
	"Right Control",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	",",
	"",
	"/",
	"",
	"SysRq",
	"Right Alt",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"Home",
	"Up",
	"Page Up",
	"",
	"Left",
	"",
	"Right",
	"",
	"End",
	"Down",
	"Page Down",
	"Insert",
	"Delete",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"Left Win",
	"Right Win",
	"Apps",

	/* More languages to follow (Oh the joy of it all) */
};
#endif


void RunLanguageSelect()
{
	static unsigned long currentSelection = 0;
	static u16 button;
	static u16 lastbutton;

	static TEXTOVERLAY *british;

	int i = 0,xPos,j;

	if ( frameCount == 1 )
	{
		StopDrawing ( "language select" );

		FreeAllLists();

		LoadTextureBank(SYSTEM_TEX_BANK);
		LoadTextureBank(TITLESGENERIC_TEX_BANK);

		currFont	= smallFont;
		british		= CreateAndAddTextOverlay(100,122,"english",YES,NO,200,255,200,91,currFont,TEXTOVERLAY_WAVECHARS,6,0);

		CreateOverlays();

		ResetParameters();

		StartDrawing ( "language select" );
	}
	// ENDIF

	button = controllerdata [ ActiveController ].button;

	if((button & CONT_UP) && !(lastbutton & CONT_UP))
    {
		if(currentSelection > 0)
		{
			currentSelection--;
			PlaySample ( 237, NULL, 255, 128 );
		}
	}
	    
	if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
    {
		if(currentSelection < 0)
		{
			currentSelection++;
			PlaySample ( 237, NULL, 255, 128 );
		}
	}
	
	if(frameCount > 15)
	{
		if (((button & CONT_A) && !(lastbutton & CONT_A)) ||
		   ((button & CONT_START) && !(lastbutton & CONT_START)))
		{
			switch ( currentSelection )
			{
				case 0:   // British
					FreeAllLists();
					frameCount			= 0;
					lastbutton			= 0;
					frontEndState.mode	= TITLE_MODE;
					PlaySample ( 2, NULL, 255, 128 );
					return;
			}
		}			
	
		switch (currentSelection)
		{
			case 0:
				british->a = 255 * Fabs(sinf(frameCount/12.5));
				british->waveAmplitude = 6.0F;
				british->r = british->g = british->b = 255;
				break;
		}
	}

	lastbutton = button;
}


