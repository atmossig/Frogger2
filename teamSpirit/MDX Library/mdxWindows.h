/*

	This file is part of the M libraries,

	File		: 
	Programmer	: Matthew Cloy
	Date		: 

----------------------------------------------------------------------------------------------- */

#ifndef MDXWINDOWS_H_INCLUDED
#define MDXWINDOWS_H_INCLUDED

#ifdef __cplusplus

extern "C"
{
#endif

typedef struct TAG_MDX_WININFO
{
	HWND hWndMain;
	HINSTANCE hInstance;
} MDX_WININFO;

extern MDX_WININFO mdxWinInfo;
extern unsigned long consoleDraw;
extern unsigned long timerDraw;

int WindowsInitialise(HINSTANCE hInstance, char *appName, long debugMode);
void RunWindowsLoop(long (*AppLoop)());


#ifdef __cplusplus
}
#endif

#endif