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

long fadeoutStart, fadeoutLength;
int fadingOut = 0;

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
	
	if (actFrameCount > (fadeoutStart+fadeoutLength)) return 1;

	int col = startIntensity + ((endIntensity-startIntensity)*(actFrameCount-fadeoutStart))/fadeoutLength;

	D3DSetupRenderstates(xluSubRS);
	DrawFlatRect(r, RGB_MAKE(col, col, col));
	D3DSetupRenderstates(xluSemiRS);

	return 0;
}


/*	-------------------------------------------------------------------------------
	Function:	ScreenFade
	Params:		start, end intensity (0-255), time in frames
	returns:	
*/
void ScreenFade(int start, int end, long time, int stay)
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
	if (fadeProc)
		if (fadeProc())
			fadingOut = 0;
}
