/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: fadeout.cpp
	Programmer	: David
	Date		: 
	Purpose		: Transition effects 'n shit

----------------------------------------------------------------------------------------------- */

#include "fadeout.h"
#include <mdx.h>

long fadeoutStart, fadeoutLength;
int fadingOut = 0;

int (*fadeProc)(void);



/*	-------------------------------------------------------------------------------
	Function:	ScreenFade
	Params:		start, end intensity (0-255), time in frames
	returns:	
*/
int DrawScreenFade(void)
{
	RECT r = { 0, 0, rxRes, ryRes };
	
	if (actFrameCount > (fadeoutStart+fadeoutLength)) return 1;

	int col = (actFrameCount-fadeOutStart)/fadeoutLength;

	D3DSetupRenderstates(xluSubRS);
	DrawFlatRect(r, RGB_MAKE(col, col, col));
	D3DSetupRenderstates(xluSemiRS);
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
}


void StopFadeout()
{
	fadeProc = NULL;
	fadingout = 0;
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
