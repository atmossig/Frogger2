/*

	This file is part of the M libraries,

	File		: 
	Programmer	: Matthew Cloy
	Date		: 

----------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <crtdbg.h>
#include <stdio.h>

#include "mdxTiming.h"
#include "mgeReport.h"

MDX_TIMING_VAL timeInfo;

/*	--------------------------------------------------------------------------------
	Function	: 
	Purpose		: 
	Parameters	: 
	Returns		: 
	Info		: 
*/

void InitTiming(float frameSpeed)
{
	timeInfo.firstTicks = GetTickCount();
	timeInfo.tickCount = 0;
	timeInfo.frameCount = 0;
	timeInfo.frameSpeed = frameSpeed;
}

/*	--------------------------------------------------------------------------------
	Function	: 
	Purpose		: 
	Parameters	: 
	Returns		: 
	Info		: 
*/

void UpdateTiming(void)
{
	unsigned long currentTicks = GetTickCount() - timeInfo.firstTicks;
	unsigned long intervalTicks = currentTicks - timeInfo.tickCount;
	float divVal = (1000.0F/timeInfo.frameSpeed);
	
	timeInfo.speed = intervalTicks / divVal;
	timeInfo.frameCount = (unsigned long)currentTicks / divVal;
	timeInfo.tickCount = currentTicks;	
}

#ifdef __cplusplus
}
#endif
