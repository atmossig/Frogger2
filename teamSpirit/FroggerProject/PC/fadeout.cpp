/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: fadeout.cpp
	Programmer	: David
	Date		: 
	Purpose		: Transition effects 'n shit

----------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <d3d.h>
#include <mdx.h>
#include "fadeout.h"
#include "layout.h"

#include "controll.h"

long fadeoutStart, fadeoutLength;
int fadingOut = 0, keepFade = 0;

int (*fadeProc)(void);

int startIntensity = 0, endIntensity = 255;

/*	-------------------------------------------------------------------------------
	Function:	ScreenFade
	Params:		start, end intensity (0-255), time in frames
	returns:	
*/
int DrawScreenFade(void)
{
	RECT r = { 0, 0, rXRes, rYRes };
	int col;
	
	if (actFrameCount > (fadeoutStart+fadeoutLength))
		col = endIntensity;
	else
		col = startIntensity + ((endIntensity-startIntensity)*(long)(actFrameCount-fadeoutStart))/fadeoutLength;

//		col = startIntensity + ((endIntensity-startIntensity)*(actFrameCount-fadeoutStart))/fadeoutLength;

	D3DSetupRenderstates(xluSubRS);
	DrawFlatRect(r, RGBA_MAKE(col, col, col, 255));
	D3DSetupRenderstates(xluSemiRS);

	return (actFrameCount > (fadeoutStart+fadeoutLength));
}


/*	-------------------------------------------------------------------------------
	Function:	ScreenFade
	Params:		start, end intensity (0-255), time in frames
	returns:	
*/
void ScreenFade(int start, int end, long time)
{
	fadeProc = DrawScreenFade;
	fadeoutStart = actFrameCount;
	fadeoutLength = time;
	
	// cause we're using subtractive
	startIntensity = 255-start;
	endIntensity = 255-end;
}


void StopFadeout()
{
	fadeProc = NULL;
	fadingOut = 0;
}

/*	-------------------------------------------------------------------------------
	Function:	MainWndProc
	Params:		As WNDPROC
	returns:	0 for success, other to pass on to mdx default handler
*/
void DrawScreenTransition(void)
{
	if (KEYPRESS(DIK_F))
		ScreenFade(0, 255, 60);

	if (fadeProc)
		if (fadeProc())
		{
			fadingOut = 0;
			
			if (!keepFade)
				fadeProc = NULL;
		}
}
