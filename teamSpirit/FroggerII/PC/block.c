/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: block.c
	Programmer	: Andy Eder
	Date		: 28/06/99 10:19:37

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>
#include "incs.h"
#include "editor.h"
#include <crtdbg.h>
#include <stdio.h>
#include "..\resource.h"

#define DEBUG_FILE "C:\\frogger2.log"

unsigned long actFrameCount, currentFrameTime;

WININFO winInfo;
BYTE lButton = 0, rButton = 0;
int runQuit = 0;

char baseDirectory[MAX_PATH] = "x:\\teamspirit\\pcversion\\";
char editorOk = 0;
long drawTimers = 0;
char keyDelay;

char outputMessageBuffer[256];

char	transparentSurf		= 0;
char	xluSurf				= 0;		
char	aiSurf				= 0;
char	UseAAMode			= 0;
char	UseZMode			= 1;

char	desiredFrameRate	= 2;
char	newDesiredFrameRate = 2;

int		appActive		= 0;

static GUID     guID;

/*	--------------------------------------------------------------------------------
	Function		: debugPrintf(int num)
	Purpose			: prints debug information to the file server output window
	Parameters		:
	Returns			: none
	Info			:
*/
void debugPrintf(int num)
{
	FILE *f;

	_CrtDbgReport(_CRT_WARN,NULL,NULL,"FroggerII",outputMessageBuffer);

	f = fopen(DEBUG_FILE, "a+");
	if (f)
	{
		fputs(outputMessageBuffer, f);
		fclose(f);
	}
}

/*	--------------------------------------------------------------------------------
	Function		: Crash(char *mess)
	Purpose			: prints a message in file server output window if system crash
	Parameters		:
	Returns			: none
	Info			:
*/
void Crash(char *mess)
{
	dprintf"\n\nCRASHED %s!!!!!!!!!!!!!!!\n",mess));
}


/*	--------------------------------------------------------------------------------
	Function		: WinMain
	Purpose			: entry point
	Parameters		: 
	Returns			: int
	Info			: 
*/
int PASCAL WinMain2(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow);
int PASCAL WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
//	__try
//	{
		WinMain2(hInstance,hPrevInstance,lpCmdLine,nCmdShow);
//	} 
/*	__except (1) 
	{
		dprintf"--------------------------------------------------------\n"));
		dprintf"- *** Unhandled exception ***                          -\n"));
		dprintf"--------------------------------------------------------\n"));
		///////////////////////////
		
	//	_exception_info();

		///////////////////////////
		dprintf"--------------------------------------------------------\n"));

	}*/
}

int PASCAL WinMain2(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	SYSTEMTIME currTime;
	ULONG memSizeInBytes = 0;
	UBYTE *memPtr = NULL;
    MSG msg;
	FILE *fpp;
	int ok = 1;
	long zero = 0;
	HRESULT	dsrVal;
	
	SAMPLE *test;

	GetLocalTime(&currTime);
//	fpp = fopen("c:\frog.ini","rt");
//	if (fpp)
//	{
//		fgets(fpp,&baseDirectory);
//		fclose(fpp);
//	}
	
	dprintf"\n------------- Starting Frogger II ----------------\n"
		"Session started %02d/%02d/%d %02d:%02d:%02d\n",
		currTime.wDay, currTime.wMonth, currTime.wYear,
		currTime.wHour, currTime.wMinute, currTime.wSecond));

	// create and initialise app window
	if(!InitialiseWindows(hInstance,nCmdShow))
		ok = 0;

	// create area for memory Jalloc's
	// 16MB () for now - ANDYE
	memSizeInBytes = (16 * 1024 * 1024);
	dprintf"\nAllocating %lu bytes memory.....",memSizeInBytes));
	memPtr = (UBYTE *)malloc(memSizeInBytes);
	if(memPtr == NULL)
	{
		dprintf"ERROR !\n"));
		ok = 0;
	}
	else
	{
		JallocInit((long)memPtr,memSizeInBytes);
		dprintf"DONE !\n"));
	}

	// initialisation...
	dprintf"Init CRC table...\n"));
	InitCRCTable();
	dprintf"Init matrix stack...\n"));
	InitMatrixStack();

	if(!InitInputDevices())
		ok = 0;
	
	/*if ( !DSoundEnumerate ( &guID, hInstance, winInfo.hWndMain ) )
	{
		InitDirectSound ( &guID, hInstance, winInfo.hWndMain, 1 );
	} 
	else 
	{
		InitDirectSound ( NULL, hInstance, winInfo.hWndMain, 0 );
	}
	// ENDELSIF
	
	InitSampleList();
	// TEST
	test = CreateAndAddSample ( "x:\\teamspirit\\pcversion\\test.wav" );

//	test
	PlaySample ( 0, 0, 0, 0 );
//	test->lpdsBuffer->lpVtbl->Play ( test->lpdsBuffer, 0, 0, 0 );
	//InitSaveData();

	PrepareSong ( 1 );
//	playCDTrack ( winInfo.hWndMain, 1 );

/*	InitSaveData();
	SaveGameData();*/
	gameState.mode		= FRONTEND_MODE;
	//frontEndState.mode	= TITLE_MODE;

	// initialise PC stuff and DirectX / Direct3D
	InitPCSpecifics();
	if(!DirectXInit(winInfo.hWndMain,1))
		ok = 0;
	if (runQuit)
		exit (0);

	InitSampleList();
	// TEST
	test = CreateAndAddSample ( "x:\\teamspirit\\pcversion\\test.wav" );

//	test
	PlaySample ( 0, 0, 0, 0 );
//	test->lpdsBuffer->lpVtbl->Play ( test->lpdsBuffer, 0, 0, 0 );
	//InitSaveData();

	//PrepareSong ( 1 );
//	playCDTrack ( winInfo.hWndMain, 1 );


	InitFont();
	InitEditor();
	
	gameState.mode		= FRONTEND_MODE;
	//frontEndState.mode	= DEMO_MODE;
	frameCount = 1;

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

		if(appActive)
		{
			if (KEYPRESS(DIK_F1))
				camDist.v[1]+=2;

			if (KEYPRESS(DIK_F2))
				camDist.v[1]-=2;

			if (KEYPRESS(DIK_F3))
				camDist.v[2]+=2;

			if (KEYPRESS(DIK_F4))
				camDist.v[2]-=2;

			if (!keyDelay)
			{

				if KEYPRESS(DIK_F10)
				{
					editorOk = !editorOk; 
					keyDelay = 20;
				}
			
				if (KEYPRESS(DIK_F5))
				{
					drawTimers = !drawTimers;;
					keyDelay = 20;
				}

			}
			else
				keyDelay--;
			
			if (editorOk)
				RunEditor();

			StartTimer(4,"GameLoop");
			GameLoop();
			EndTimer(4);

			keysEnabled = !editorOk;
			ProcessUserInput(winInfo.hWndMain);
			
			DrawGraphics();
			
			StartTimer(3,"Flip");
			DirectXFlip();
			EndTimer(3);
			actFrameCount = (GetTickCount()/(1000/60));
			
			// THIS IS A SPRITE OVERLAY. ASSIGNING A TIME TO IT IS BAD.
			//clock = timeGetTime();
		}
	}

	// clean up
	DeInitPCSpecifics();
	DeInitInputDevices();
	ShutdownEditor();

	free(memPtr);
	memPtr = NULL;

    return msg.wParam;
}


/*	--------------------------------------------------------------------------------
	Function		: InitialiseWindows
	Purpose			: initialises and creates application window
	Parameters		: HINSTANCE,int
	Returns			: int
	Info			: 
*/

HBITMAP appBackgnd;
HDC appBackgndDC;

int InitialiseWindows(HINSTANCE hInstance,int nCmdShow)
{
    WNDCLASS wc;
	
	// save instance handle
    winInfo.hInstance = hInstance;

    // set up and register window class
    wc.style			= 0;
    wc.lpfnWndProc		= WindowProc;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hInstance		= hInstance;
    wc.hIcon			= LoadIcon(NULL,(LPSTR)IDI_WINLOGO);
    wc.hCursor			= LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName		= NULL;
    wc.lpszClassName	= "Frogger2PC";
    if(!RegisterClass(&wc))
	{
		dprintf"RegisterClass failed\n"));
        return 0;
	}

	// create the window
    winInfo.hWndMain = CreateWindowEx(
		WS_EX_TOPMOST,
        "Frogger2PC",
        "Frogger2PC",
		WS_POPUP,
		//WS_OVERLAPPEDWINDOW | WS_POPUP | WS_VISIBLE,			// window style
        0,
		0,
		GetSystemMetrics(SM_CXSCREEN), // 640
		GetSystemMetrics(SM_CYSCREEN), // 480
        NULL,				// parent window
        NULL,				// menu handle
        hInstance,			// program handle
        NULL);				// create parms

	
    if(!winInfo.hWndMain)
	{
        dprintf"CreateWindowEx failed\n"));
        return 0;
	}

    appBackgnd=LoadBitmap(winInfo.hInstance,MAKEINTRESOURCE(IDB_BACKGROUND));
	
	appActive = 1;
	ShowWindow(winInfo.hWndMain,SW_SHOW);
	UpdateWindow(winInfo.hWndMain);
//	ShowCursor(0);
	
												
    return 1;
}


/*	--------------------------------------------------------------------------------
	Function		: WindowProc
	Purpose			: handles windows messages
	Parameters		: HWND,UINT,WPARAM,LPARAM
	Returns			: long
	Info			: 
*/


long FAR PASCAL WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	HDC dc;
	PAINTSTRUCT ps;

    switch(message)
	{
        case WM_DESTROY:
			PostQuitMessage(0);
			appActive = 0;
            break;

		case WM_PAINT:
			dc = BeginPaint(winInfo.hWndMain,&ps);
			EndPaint(winInfo.hWndMain,&ps);
			break;
		
		
		case WM_LBUTTONDOWN:
			lButton = 1;
			break;
		
		case WM_LBUTTONUP:
			lButton = 0;
			break;

		case WM_RBUTTONDOWN:
			rButton = 1;
			break;
		
		case WM_RBUTTONUP:
			rButton = 0;
			break;
/*
		case WM_CHAR:
			if (editorOk)	// only when editor is set up to "grab" keyboard data
			{
				EditorKeypress((char)wParam);
				return 0;
			}
			break;
*/
		case WM_KEYUP:
			switch( (int)wParam )
			{
			case VK_SHIFT: mod &= ~FRED_SHIFT; break;
			case VK_CONTROL: mod &= ~FRED_CONTROL; break;
			}
			break;

		case WM_KEYDOWN:
			switch( (int)wParam )
			{
			case VK_SHIFT: mod |= FRED_SHIFT; break;
			case VK_CONTROL: mod |= FRED_CONTROL; break;
			case VK_CAPITAL: 
				if( mod & FRED_CAPS ) mod &= ~FRED_CAPS;
				else mod |= FRED_CAPS;
				break; // Caps lock is a toggle
			}
			break;

		case WM_CHAR:
			if (editorOk)	// only when editor is set up to "grab" keyboard data
			{
				EditorKeypress((char)wParam);
				return 0;
			}
			break;

		case WM_ERASEBKGND:
		{
			RECT cRect;
			int i,j;

			GetClientRect (hWnd,&cRect);
			appBackgndDC=CreateCompatibleDC((HDC)wParam);
	
			if (SelectObject (appBackgndDC,appBackgnd)==NULL)
				dp("sharugar");

			for (i=0; i<cRect.right+128; i+=128)
				for (j=0; j<cRect.bottom+128; j+=128)
					if (!BitBlt((HDC)wParam,i,j,128,128,appBackgndDC,0,0,SRCCOPY))
										 dp("bugger");
			DeleteDC(appBackgndDC);
      			
			return TRUE;
		 }
	}

    return DefWindowProc(hWnd,message,wParam,lParam);
}


/*	--------------------------------------------------------------------------------
	Function		: DrawGraphics
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
VECTOR oldCCSource, oldCCTarget;

void DrawGraphics() 
{
	currentFrameTime = timeGetTime();
		
	StartTimer(0,"DrawGfx");

	XformActorList();

	
	if(spriteList.numEntries)
		AnimateSprites();


	// Actual stuff that draws
	BeginDrawHardware();
	
	StartTimer(1,"Draw Actor List");

//	XformActorList();
	DrawActorList();	
	
	EndTimer(1);
	StartTimer(2,"Draw Sprites");

	DrawSpecialFX();

	if(spriteList.numEntries)
		PrintSpritesOpaque();

	PrintSpriteOverlays();	
	PrintTextOverlays();
	
	EndTimer(2);

	if (editorOk)
		DrawEditor();
	
	EndTimer(0);

	/* CAMERA SPACE STUFF */
	// Back up currCamSource and currCamTarget
	oldCCSource = currCamSource[screenNum];
	oldCCTarget = currCamTarget[screenNum];
	// Set them so we don't do any modelling transforms
	currCamSource[screenNum] = zero;
	currCamTarget[screenNum] = inVec;
	
	if( (gameState.mode == GAME_MODE || frontEndState.mode == HISCORE_MODE) && text3DList.numEntries )
	{
		Calculate3DText( );
		Print3DText( );
	}

	// Restore currCam vectors
	currCamSource[screenNum] = oldCCSource;
	currCamTarget[screenNum] = oldCCTarget;
	/* END CAMERA SPACE STUFF */

	if (drawTimers)
		PrintTimers();

	if (KEYPRESS(DIK_F6))
		HoldTimers();

			
	ClearTimers();
	EndDrawHardware();
}
