/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.
	
	  
	File		: develop.c
	Programmer	: Andrew Eder
	Date		: 22/2/99
	
----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


#define MAXMENUOPTIONS	2

unsigned long developmentMode = 0;

/*	--------------------------------------------------------------------------------
	Function		: RunDevelopmentRoutines
	Purpose			: for testing various development routines
	Parameters		: 
	Returns			: void
	Info			: 
*/
void RunDevelopmentMenu()
{
	static unsigned long currentSelection = 0;
	static u16 button, lastbutton;
	static TEXTOVERLAY *dev,*objView,*sndView;
	int i = 0, xPos, j;

	if(frameCount == 1)
	{
		StopDrawing("devmenu");
		FreeAllLists();

		LoadTextureBank(SYSTEM_TEX_BANK);
		LoadTextureBank(TITLESGENERIC_TEX_BANK);

		currFont = smallFont;

		dev		= CreateAndAddTextOverlay(20,20,"DEVELOPMENT MENU",NO,NO,255,255,255,255,currFont,0,0,0);
		objView	= CreateAndAddTextOverlay(40,60,"object viewer",NO,NO,255,255,255,255,currFont,0,0,0);
		sndView	= CreateAndAddTextOverlay(40,80,"sound player",NO,NO,255,255,255,255,currFont,0,0,0);

		ResetParameters();

		StartDrawing("devmenu");
	}

	button = controllerdata[ActiveController].button;

	if((button & CONT_UP) && !(lastbutton & CONT_UP))
    {
		if(currentSelection > 0)
		{
			currentSelection--;
			PlaySampleNot3D(237,255,128,128);
		}
	}
	    
	if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
    {
		if(currentSelection < (MAXMENUOPTIONS-1))
		{
			currentSelection++;
			PlaySampleNot3D(237,255,128,128);
		}
	}

	if(frameCount > 15)
	{
		if((button & CONT_B) && !(lastbutton & CONT_B))
		{
			FreeAllLists();
			frameCount = 0;
			lastbutton = 0;
			return;
		}

		if (((button & CONT_A) && !(lastbutton & CONT_A)) ||
		   ((button & CONT_START) && !(lastbutton & CONT_START)))
		{
			frameCount = 0;
			lastbutton = 0;

			switch(currentSelection)
			{
				case 0:   // object viewer
					developmentMode = OBJVIEW_MODE;
					FreeAllLists();
					return;
				case 1:  // sound player
					developmentMode = SNDVIEW_MODE;
					FreeAllLists();
					return;
			}
		}

		if ((button & CONT_G) && !(lastbutton & CONT_G))
		{
			frameCount = 0;
			lastbutton = 0;

			FreeAllLists();
			developmentMode = 0;
			gameState.mode = TITLE_MODE;
		}			

	
		switch(currentSelection)
		{
			case 0:
				objView->a = 255;
				sndView->a = 91;
				break;
			
			case 1:
				objView->a = 91;
				sndView->a = 255;
				break;
		}
	}

	lastbutton = button;
}
