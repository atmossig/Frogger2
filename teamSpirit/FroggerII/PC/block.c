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


WININFO winInfo;
long lButton = 0;

char baseDirectory[MAX_PATH] = "q:\\work\\froggerii\\pc\\";
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

/*	--------------------------------------------------------------------------------
	Function		: debugPrintf(int num)
	Purpose			: prints debug information to the file server output window
	Parameters		:
	Returns			: none
	Info			:
*/
void debugPrintf(int num)
{
	_CrtDbgReport(_CRT_WARN,NULL,NULL,"FroggerII",outputMessageBuffer);
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
int PASCAL WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	ULONG memSizeInBytes = 0;
	UBYTE *memPtr = NULL;
    MSG msg;
	int ok = 1;
	
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
		JallocInit(memPtr,memSizeInBytes);
		dprintf"DONE !\n"));
	}

	// initialisation...
	dprintf"Init CRC table...\n"));
	InitCRCTable();
	dprintf"Init matrix stack...\n"));
	InitMatrixStack();
	if(!InitInputDevices())
		ok = 0;

	gameState.mode		= FRONTEND_MODE;
	frontEndState.mode	= TITLE_MODE;

	// initialise PC stuff and DirectX / Direct3D
	InitPCSpecifics();
	if(!DirectXInit(winInfo.hWndMain,1))
		ok = 0;
	InitFont();
	InitEditor();
	
	gameState.mode		= FRONTEND_MODE;
	frontEndState.mode	= DEMO_MODE;

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
			
			StartTimer(4,"GameLoop");
			GameLoop();
			EndTimer(4);

			ProcessUserInput(winInfo.hWndMain);
			if (editorOk)
				RunEditor();

			
			DrawGraphics();
			
			StartTimer(3,"Flip");
			DirectXFlip();
			EndTimer(3);

		}
	}

	// clean up
	DeInitPCSpecifics();
	DeInitInputDevices();
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
		640,	//GetSystemMetrics(SM_CXSCREEN),
		480,	//GetSystemMetrics(SM_CYSCREEN),
        NULL,				// parent window
        NULL,				// menu handle
        hInstance,			// program handle
        NULL);				// create parms

	
    if(!winInfo.hWndMain)
	{
        dprintf"CreateWindowEx failed\n"));
        return 0;
	}

	appActive = 1;
	ShowWindow(winInfo.hWndMain,SW_SHOW);
	UpdateWindow(winInfo.hWndMain);
	ShowCursor(0);

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

void DrawGraphics() 
{
	StartTimer(0,"DrawGfx");

	XformActorList();

	
	if(spriteList.numEntries)
		AnimateSprites();


	// Actual stuff that draws
	BeginDrawHardware();
	
	StartTimer(1,"Draw Actor List");

	DrawActorList();	
	
	EndTimer(1);
	StartTimer(2,"Draw Sprites");

	if(spriteList.numEntries)
		PrintSpritesOpaque();
		
	PrintSpriteOverlays();	
	PrintTextOverlays();
	
	EndTimer(2);

	if (editorOk)
		DrawEditor();
	
	EndTimer(0);

	if (drawTimers)
		PrintTimers();

	if (KEYPRESS(DIK_F6))
		HoldTimers();

			
	ClearTimers();
	EndDrawHardware();
}
