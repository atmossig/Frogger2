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
#include "commctrl.h"
#include "gelf.h"
#include "stdio.h"

#ifdef __cplusplus
extern "C"
{
#endif

int Eval_Exception (int n_except,LPEXCEPTION_POINTERS exceptinf)
{
	MSG msg;
	
	ShowWindow(mdxWinInfo.hWndMain,SW_SHOWMINIMIZED);

    while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
	{
		TranslateMessage(&msg); 
		DispatchMessage(&msg);
	}
	pDirectDraw7->SetCooperativeLevel(mdxWinInfo.hWndMain, DDSCL_NORMAL);
	pDirectDraw7->Release();

	return EXCEPTION_CONTINUE_SEARCH;
}


#ifdef __cplusplus
}
#endif
