/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: title.c
	Programmer	: Matthew Cloy
	Date		: 10/11/98

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"

//unsigned long levelPlayingTimer = 0;
//unsigned long numSecs			= 10;

SPRITEOVERLAY *sprOver = NULL;

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: RunTitleScreen 

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/
SPRITEOVERLAY *mySpr1 = NULL;
SPRITEOVERLAY *mySpr2 = NULL;





void RunTmpTitle()
{
	static unsigned long currentSelection = 0;
	static u16 button;
	static u16 lastbutton;
	static TEXTOVERLAY *availWorlds,*frontEnd;
	int i = 0,xPos,j;

	int posx = 64;
	int posy = 90;


	if ( frameCount == 1 )
	{
		StopDrawing ( "tmp title" );

		FreeAllLists();

		LoadTextureBank(SYSTEM_TEX_BANK);
		LoadTextureBank(TITLESGENERIC_TEX_BANK);
/*
		sprOver = CreateAndAddSpriteOverlay(posx,posy,"n64a.bmp",32,32,255,255,255,255,0);
		sprOver = CreateAndAddSpriteOverlay(posx+(32*1),posy,"n64b.bmp",32,32,255,255,255,255,0);
		sprOver = CreateAndAddSpriteOverlay(posx+(32*2),posy,"n64c.bmp",32,32,255,255,255,255,0);
		sprOver = CreateAndAddSpriteOverlay(posx+(32*3),posy,"n64d.bmp",32,32,255,255,255,255,0);

		sprOver = CreateAndAddSpriteOverlay(posx+(32*1),posy+(32*1),"n64e.bmp",32,32,255,255,255,255,0);
		sprOver = CreateAndAddSpriteOverlay(posx+(32*2),posy+(32*1),"n64f.bmp",32,32,255,255,255,255,0);
		sprOver = CreateAndAddSpriteOverlay(posx+(32*3),posy+(32*1),"n64g.bmp",32,32,255,255,255,255,0);

		sprOver = CreateAndAddSpriteOverlay(posx+(32*1),posy+(32*2),"n64h.bmp",32,32,255,255,255,255,0);
		sprOver = CreateAndAddSpriteOverlay(posx+(32*2),posy+(32*2),"n64i.bmp",32,32,255,255,255,255,0);
		sprOver = CreateAndAddSpriteOverlay(posx+(32*3),posy+(32*2),"n64j.bmp",32,32,255,255,255,255,0);
*/				 

		currFont	= smallFont;
		availWorlds	= CreateAndAddTextOverlay(100,122,"complete worlds",YES,NO,200,255,200,91,currFont,TEXTOVERLAY_WAVECHARS,1,0);
		frontEnd	= CreateAndAddTextOverlay(100,140,"the front end",YES,NO,200,255,200,91,currFont,TEXTOVERLAY_WAVECHARS,1,0);
			
		ResetParameters();

//		runningDevStuff = 1;

		StartDrawing ( "tmp title" );
	}

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
		if(currentSelection < 1)
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
				case 0:   // Complete Worlds/levels
					FreeAllLists();
					frameCount			= 0;
					lastbutton			= 0;
//					frontEndState.mode	= STARTLEVELS_MODE;
					PlaySample ( 2, NULL, 255, 128 );
					return;
				case 1:   // The Front End
					FreeAllLists();
					frameCount			= 0;
					lastbutton			= 0;
					//frontEndState.mode	= LANGUAGE_SELECT_MODE;
					PlaySample ( 2, NULL, 255, 128 );
					return;
			}
		}			
	
		switch (currentSelection)
		{
			case 0:
				availWorlds->a = 255 * Fabs(sinf(frameCount/12.5));
				availWorlds->waveAmplitude = 6.0F;

				frontEnd->a = 91;
				frontEnd->r = frontEnd->b = 200;
				frontEnd->g = 255;

				availWorlds->r = availWorlds->g = availWorlds->b = 255;

				if(frontEnd->waveAmplitude > 1)
					frontEnd->waveAmplitude -= 0.10F;
				break;
			case 1:
				frontEnd->a = 255 * Fabs(sinf(frameCount/12.5));
				frontEnd->waveAmplitude = 6.0F;

				availWorlds->a = 91; //255;
				availWorlds->r = availWorlds->b = 200;
				availWorlds->g = 255;

				frontEnd->r = frontEnd->g = frontEnd->b = 255;

				if(availWorlds->waveAmplitude > 1)
					availWorlds->waveAmplitude -= 0.10F;
				break;

		}
	}

	lastbutton = button;  
}

void CreateOverlays ( void )
{
	// add the texture tiles that comprise the Frogger2 logo / Hasbro logo....
	sprOver = CreateAndAddSpriteOverlay(98,20,"flogo01.bmp",32,32,255,255,255,255,0);
	sprOver = CreateAndAddSpriteOverlay(130,20,"flogo02.bmp",32,32,255,255,255,255,0);
	sprOver = CreateAndAddSpriteOverlay(162,20,"flogo03.bmp",32,32,255,255,255,255,0);
	sprOver = CreateAndAddSpriteOverlay(194,20,"flogo04.bmp",32,32,255,255,255,255,0);
	sprOver = CreateAndAddSpriteOverlay(98,52,"flogo05.bmp",32,32,255,255,255,255,0);
	sprOver = CreateAndAddSpriteOverlay(130,52,"flogo06.bmp",32,32,255,255,255,255,0);
	sprOver = CreateAndAddSpriteOverlay(162,52,"flogo07.bmp",32,32,255,255,255,255,0);
	sprOver = CreateAndAddSpriteOverlay(194,52,"flogo08.bmp",32,32,255,255,255,255,0);
	sprOver = CreateAndAddSpriteOverlay(162,84,"flogo09.bmp",32,32,255,255,255,255,0);
	sprOver = CreateAndAddSpriteOverlay(194,84,"flogo10.bmp",32,32,255,255,255,255,0);
	
	konami = CreateAndAddSpriteOverlay(240,35,"konami.bmp",32,32,255,255,255,192,0 );
	atari = CreateAndAddSpriteOverlay(40,35,"atari.bmp",32,32,255,255,255,192,0 );

/*	sprOver = CreateAndAddSpriteOverlay(24,150,"haslg001.bmp",32,32,255,255,255,127,0);
	sprOver = CreateAndAddSpriteOverlay(56,150,"haslg002.bmp",32,32,255,255,255,127,0);
	sprOver = CreateAndAddSpriteOverlay(24,182,"haslg003.bmp",32,32,255,255,255,127,0);
	sprOver = CreateAndAddSpriteOverlay(56,182,"haslg004.bmp",32,32,255,255,255,127,0);

	sprOver = CreateAndAddSpriteOverlay(200,185,"isl1.bmp",32,32,255,255,255,255,0);
	sprOver = CreateAndAddSpriteOverlay(232,185,"isl2.bmp",32,32,255,255,255,255,0);
	sprOver = CreateAndAddSpriteOverlay(264,185,"isl3.bmp",32,32,255,255,255,255,0);
*/
//	CreateAndAddTextOverlay(0,218,"milestone 5",YES,NO,255,255,255,91,smallFont,0,0,0);
}
