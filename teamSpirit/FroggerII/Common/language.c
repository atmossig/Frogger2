/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: language.c
	Programmer	: James Healey
	Date		: 31/03/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"

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
		LoadTextureBank(GENERIC_TEX_BANK);

		currFont	= smallFont;
		british		= CreateAndAddTextOverlay(100,122,"english",YES,NO,200,255,200,91,currFont,TEXTOVERLAY_WAVECHARS,6,0);

		CreateOverlays();

		ResetParameters();

//		runningDevStuff = 1;

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
//			runningDevStuff = 0;
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
				british->a = 255 * fabs(sinf(frameCount/12.5));
				british->waveAmplitude = 6.0F;
				british->r = british->g = british->b = 255;
				break;
		}
	}

	lastbutton = button;
}


