/*

	This file is part of the M libraries,

	File		: 
	Programmer	: Matthew Cloy
	Date		: 

----------------------------------------------------------------------------------------------- */
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <math.h>
#include "mgeReport.h"
#include "mdxDDraw.h"
#include "mdxD3D.h"
#include "mdxInfo.h"
#include "mdxTiming.h"
#include "mdxCRC.h"
#include "mdxTexture.h"
#include "mdxMath.h"
#include "mdxObject.h"
#include "mdxActor.h"
#include "mdxLandscape.h"
#include "mdxRender.h"
#include "mdxPoly.h"
#include "mdxDText.h"
#include "mdxProfile.h"
#include "mdxWindows.h"
#include "mdxFont.h"
#include "gelf.h"

#ifdef __cplusplus
extern "C"
{
#endif

MDX_WININFO mdxWinInfo;
WNDPROC userWndProc;

unsigned long consoleDraw = 0;
unsigned long timerDraw = 0;
char winAppName[128];
long windowActive;

long FAR PASCAL WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	if (userWndProc)
		if (userWndProc(hWnd, message, wParam, lParam) == 0)
			return 0;

    switch(message)
	{
		case WM_CREATE:
			cp(" %s console, build dated %s - Interactive Studios",winAppName,__DATE__);
			cp("");
			cp(">");
			break;
			
		case WM_SHOWWINDOW:
			dp("WM_SHOWWINDOW: %s\n", (wParam)?"showing":"hiding");
			break;

        case WM_DESTROY:
			PostQuitMessage(0);
            break;
		case WM_KEYDOWN:
		{
			HiliteUp(2);

			switch ((int)wParam)
			{
				case VK_F3:
					UpdateConsole((int)wParam);
					break;
				case VK_F5:
					consoleDraw = !consoleDraw;
					break;
				case VK_F6:
					timerDraw = !timerDraw;
					break;
			}
		
			break;
		}
		case WM_CHAR:
			if (consoleDraw)
				UpdateConsole((int)wParam);
			break;
		case WM_LBUTTONDOWN:
			break;
		
		case WM_LBUTTONUP:
			break;

		case WM_RBUTTONDOWN:
			break;
		
		case WM_RBUTTONUP:
			break;

		case WM_ACTIVATE:
			if (wParam == WA_INACTIVE)
				windowActive = 0;
			else
				windowActive = 1;
			break;

		default:
		    return DefWindowProc(hWnd,message,wParam,lParam);
	}

	return 0;
}

int WindowsInitialise(HINSTANCE hInstance, char *appName, long debugMode)
{
    WNDCLASS wc;
	
	strcpy (winAppName,appName);

	// save instance handle
    mdxWinInfo.hInstance = hInstance;

    // set up and register window class
    wc.style			= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc		= WindowProc;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hInstance		= hInstance;
    wc.hIcon			= NULL;
    wc.hCursor			= NULL;
    wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName		= NULL;
    wc.lpszClassName	= appName;
    if(!RegisterClass(&wc))
	{
		dp("RegisterClass failed\n");
        return 0;
	}

	// create the window
    mdxWinInfo.hWndMain = CreateWindowEx(
		0,
        appName,
        appName,
		WS_OVERLAPPED | WS_SYSMENU | WS_THICKFRAME,//_POPUP,
		0,
		0,
		640,//GetSystemMetrics(SM_CXSCREEN), 
		480,//GetSystemMetrics(SM_CYSCREEN), 
        NULL,				// parent window
        NULL,				// menu handle
        hInstance,			// program handle
        NULL);				// create parms

	
    if(!mdxWinInfo.hWndMain)
	{
        dp("CreateWindowEx failed\n");
        return 0;
	}

	ShowWindow(mdxWinInfo.hWndMain,SW_HIDE);
	UpdateWindow(mdxWinInfo.hWndMain);
	
	if (!debugMode)
	{
		SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
		SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
	}

    return 1;
}

void RunWindowsLoop(long (*AppLoop)())
{
	MSG msg;
	long ok = 1;

	while(ok)
	{
        while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				ok = 0;
				break;
			}
			
			TranslateMessage(&msg); 
			DispatchMessage(&msg);
		}

		if (ok)
		{
			if (AppLoop)
				if (AppLoop())
					ok = 0;
			UpdateTiming();
		}
	}
}

WNDPROC mdxSetUserWndProc(WNDPROC proc)
{
	WNDPROC oldWndProc = userWndProc;
	userWndProc = proc;

	return oldWndProc;
}


#ifdef __cplusplus
}
#endif
