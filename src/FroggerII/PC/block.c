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
#include <windows.h>
#include <windowsx.h>
#include <wtypes.h>
#include <crtdbg.h>
#include <commctrl.h>
#include <cguid.h>
#include <dplay.h>
#include <dplobby.h>
#include "..\resource.h"
#include "network.h"
#include "netchat.h"
#include "software.h"
#include "mavis.h"
#include "config.h"

#ifdef _DEBUG
#define USE_EDITOR
#endif

void SetupViewMatrix(void);
void AnimateTexturePointers(void);
float pauseGameSpeed = 1;
#define DEBUG_FILE "C:\\frogger2.log"

unsigned long actFrameCount,
			lastActFrameCount = 0,
			currentFrameTime,
			actTickCount;

unsigned long actTickCountModifier = 0;
unsigned long modFactor = 100;

unsigned long speedKill = 0;
float gameSpeed = 1, targetSpeed = 3;
char dprintbuf[255] = "---";

HBITMAP appBackgnd;

WININFO winInfo;
BYTE lButton = 0, rButton = 0;
int runQuit = 0;
long totalFacesDrawn;
extern long numFacesDrawn;
extern long numPixelsDrawn;
extern long runHardware;
extern unsigned long rKeying;
extern unsigned long rPlaying;
extern long synchedFrameCount;
extern float RES_DIFF;
extern float RES_DIFF2;
extern int drawOverlays;

void StopKeying(void);

float vMatrix[4][4];
long keyInput = 1;

extern long myLatency;

long winMode = 1;
long scaleMode = 0;

char baseDirectory[MAX_PATH] = "x:\\teamspirit\\pcversion\\";
char editorOk = 0;
long drawTimers = 0;
float keyDelay;

unsigned long nextSynchAt;

unsigned long synchSpeed = 60 * 1;
unsigned long pingOffset = 40;
unsigned long synchRecovery = 10;

char outputMessageBuffer[256];

char	transparentSurf		= 0;
char	xluSurf				= 0;		
char	aiSurf				= 0;
char	UseAAMode			= 0;
char	UseZMode			= 1;

// Screenshots needed?
char shotMode = 0;

char	desiredFrameRate	= 2;
char	newDesiredFrameRate = 2;

int		appActive		= 0;

#define REGISTRY_KEY "Software\\Atari\\Frogger2"

static GUID     guID;
extern TEXTURE *frogEyeOpen,*frogEyeClosed;
extern long showActorNames;


/*	--------------------------------------------------------------------------------
	Function		: debugPrintf(int num)
	Purpose			: prints debug information to the file server output window
	Parameters		:
	Returns			: none
	Info			:
*/
#ifndef FULL_BUILD

void debugPrintf(int num)
{
	FILE *f;

#ifdef _DEBUG
	_CrtDbgReport(_CRT_WARN,NULL,NULL,"FroggerII",outputMessageBuffer);
#endif
	f = fopen(DEBUG_FILE, "a+");
	if (f)
	{
		fputs(outputMessageBuffer, f);
		fclose(f);
	}
}

#endif

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
	Function		: GetRegistryInformation(void)
	Parameters		: 
	Returns			: int
	Info			: Gets setup stuff (e.g. install dir) from the registry
*/

int GetRegistryInformation(void)
{
	HKEY hkey;
	DWORD len = MAX_PATH;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGISTRY_KEY, 0, KEY_READ, &hkey) != ERROR_SUCCESS)
	{
		dprintf"Couldn't open registry key\n")); return 0;
	}
	else
	{
		if (RegQueryValueEx(hkey, "InstallDir", NULL, NULL, baseDirectory, &len) == ERROR_SUCCESS)
		{
			if (baseDirectory[strlen(baseDirectory) - 1] != '\\')
				strcat(baseDirectory, "\\");
		}
		else
			dprintf"Couldn't read InstallDir value from registry\n"));

		len = 255;

		RegCloseKey(hkey);
	}

	return 1;
}

int SetRegistryInformation(void)
{
	HKEY hkey;
	DWORD len = MAX_PATH;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGISTRY_KEY, 0, KEY_WRITE, &hkey) != ERROR_SUCCESS)
	{
		dprintf"Couldn't open registry kep for writing\n")); return 0;
	}
	else
	{
		RegSetValueEx(hkey, "InstallDir", NULL, REG_SZ, baseDirectory, strlen(baseDirectory) + 1);
//		RegSetValueEx(hkey, "VideoCard", NULL, REG_SZ, videoCardName, strlen(videoCardName) + 1);
		RegCloseKey(hkey);
	}

	return 1;
}

/*	--------------------------------------------------------------------------------
	Function		: WinMain
	Purpose			: entry point
	Parameters		: 
	Returns			: int
	Info			: 
*/

short *loadScr;
long startTicks;
long curTicks;
float turbo = 1;

int PASCAL WinMain2(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow);
int PASCAL WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	winMode = 0;
	scaleMode = 0;
	GetArgs(lpCmdLine);
	ReadConfigFile();
	WinMain2(hInstance,hPrevInstance,lpCmdLine,nCmdShow);
}

void InitOneOverTable(void);
extern float camSideOfs;
extern TEXTOVERLAY *polysText;

int PASCAL WinMain2(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	long numRequired;
	char filename[MAX_PATH];
	SYSTEMTIME currTime;
	ULONG memSizeInBytes = 0;
	UBYTE *memPtr = NULL;
    MSG msg;
	FILE *fpp;
	int ok = 1;
	long i;
	long zero = 0;
	HRESULT	dsrVal, hRes;
	unsigned long newTickCount;
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

	GetRegistryInformation();
	
	// create and initialise app window
	if(!InitialiseWindows(hInstance,nCmdShow))
		return 1;

	// initialise PC stuff and DirectX / Direct3D
	InitPCSpecifics();
	if(!InitInputDevices())
		return 1;

	if(!DirectXInit(winInfo.hWndMain,1) || runQuit)
		return 1;

#ifndef DONTUSEJALLOC
	// create area for memory Jalloc's
	// 16MB () for now - ANDYE
	memSizeInBytes = (16 * 1024 * 1024);
	dprintf"\nAllocating %lu bytes memory.....",memSizeInBytes));
	memPtr = (UBYTE *)malloc(memSizeInBytes);
	if(memPtr == NULL)
	{
		dprintf"ERROR !\n"));
		return 1;
	}
	else
	{
		JallocInit((long)memPtr,memSizeInBytes);
		dprintf"DONE !\n"));
	}
#endif

	// initialisation...
	dprintf"Init CRC table...\n"));
	InitCRCTable();
	dprintf"Init matrix stack...\n"));
	InitMatrixStack();

	gameState.mode		= INGAME_MODE;

#ifdef TEXTURE_DEBUG
	hdlCheck = (unsigned char *)JallocAlloc( MAX_HDLCHECKS, YES, "hdlcheck" );
#endif

	ShowWindow(winInfo.hWndMain,SW_SHOW);
	InitSampleList();

	InitFont();
#ifdef USE_EDITOR
	InitEditor();
#endif
	
	frameCount = 1;
	actTickCount = GetTickCount();

	lastActFrameCount = actFrameCount = (GetTickCount()*(60.0f/1000.0f));
	InitOneOverTable();	
	
    //strcpy (filename);	
	//strcat (filename,TEXTURE_BASE);
	
	sprintf (filename,"%s%sloading%lu.bmp",baseDirectory,TEXTURE_BASE,SCREEN_WIDTH);
	
	dprintf"Loading screen - %s\n",filename));
	loadScr = GetGelfBmpDataAsShortPtr(filename,0,0);
	
	screenTextureList = InitScreenTextureList();
	screenTextureList2 = InitScreenTextureList();
	screenVtxList = InitScreenVertexList();
	
	if (!runHardware)
		SoftwareInit(SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	RES_DIFF = SCREEN_WIDTH/640.0;
	RES_DIFF2 = 2*RES_DIFF;
	
	numRequired = (SCREEN_WIDTH/SCREENTEX_SIZE) * (SCREEN_HEIGHT/SCREENTEX_SIZE);

	for (i=0; i<numRequired; i++)
		screenTexList[i] = NULL;

	// Run-in to frontend load
	FreeAllLists();
	StartGameIntro();

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
			if( debugKeys )
			{
				if (KEYPRESS(DIK_F1))
					camDist.v[1]+=2*gameSpeed;

					
				if (KEYPRESS(DIK_F2))
					camDist.v[1]-=2*gameSpeed;

				/*if (KEYPRESS(DIK_F1))
					speedKill++;

				if (KEYPRESS(DIK_F2))
					if (speedKill>0)
						speedKill--;
	*/
				if (KEYPRESS(DIK_F3))
				{
					if (KEYPRESS(DIK_LCONTROL))
						camSideOfs+=2*gameSpeed;
					else
						camDist.v[2]+=2*gameSpeed;
				}
			
				if (KEYPRESS(DIK_F4))
				{
					if (KEYPRESS(DIK_LCONTROL))
						camSideOfs-=2*gameSpeed;
					else
						camDist.v[2]-=2*gameSpeed;
				}
			}
		
			if (KEYPRESS(DIK_F8))
			{
				if (KEYPRESS(DIK_LCONTROL))
					dumpScreen = 1;
				else
				{
					if (KEYPRESS(DIK_LSHIFT))
						showActorNames = 1;
					else
						displayingTile=!displayingTile;
				}
			}

			if (keyDelay<1)
			{
				if (KEYPRESS(DIK_F9))
				{
					
				}
			
				if (KEYPRESS(DIK_F11))
				{
					DisableHUD();
					polysText->draw = 0;
					for (i=0; i<numBabies; i++)
						babyIcons[i]->draw = 0;

					keyDelay = 30;
				}

				if( KEYPRESS(DIK_F7) && chatFlags )
				{
					if( chatFlags & CHAT_INPUT )
					{
						chatFlags &= ~CHAT_INPUT;
					}
					else
					{
						if( !chatInput.msg )
							chatInput.msg = (char *)JallocAlloc(MAX_CSLENGTH,YES,"string");

						chatInput.msgLen = 0;
						chatFlags |= CHAT_INPUT;
						keyDelay = 20;
					}
				}

//				if( KEYPRESS(DIK_F8) )
//				fog.mode = 1;//!(fog.mode);

				if( debugKeys )
				{
#ifdef USE_EDITOR
					if( KEYPRESS(DIK_F10) )
					{
						editorOk = !editorOk; 
						keyDelay = 20;
					}
#endif
				
					if (KEYPRESS(DIK_F5))
					{
						if (drawTimers<3)
							drawTimers++;
						else
							drawTimers = 0;

						keyDelay = 20;
					}

					if (KEYPRESS(DIK_F6))
					{
						drawOverlays = !drawOverlays;
						keyDelay = 20;
					}
				}
/*
				if( KEYPRESS(DIK_SPACE) )
				{
					AICOMMAND *com = (AICOMMAND *)JallocAlloc(sizeof(AICOMMAND), YES, "aicom");

					if( !hubChar )
						hubChar = CreateAndAddCharacter( "bfly.obe", gTStart[0], 40, CHAR_HUB );

					com->time = 60;
					com->flags = AICOMFLAG_QUEUED;
					com->type = AICOM_FACE_DIR;
					SubVector( &com->dir, &frog[0]->actor->pos, &hubChar->act->actor->pos );
					MakeUnit( &com->dir );

					IssueAICommand( hubChar, com );
				}
*/			}
			else
				if (gameSpeed)
					keyDelay-=gameSpeed;
				else
					keyDelay-=1;

			if( gameState.multi == MULTIREMOTE && gameState.mode == INGAME_MODE )
				RefreshMPFrogs( );

			GameLoop();

			keysEnabled = !(editorOk || (chatFlags & CHAT_INPUT));
			ProcessUserInput(winInfo.hWndMain);

#ifdef USE_EDITOR
			if (editorOk)
				RunEditor();
#endif

			DrawGraphics();

			//if (speedKill)
			//	Sleep(speedKill);

			if (actFrameCount>10)
			{
				// Screengrab needs old flip method
				if (screenGrabbed == 1)
				{
					startTicks = GetTickCount();
					screenGrabbed = 2;
				}

				if (screenGrabbed == 2)
				{
					float sVal,fVal;
					
					curTicks = GetTickCount();
					if (curTicks>startTicks+2000)
					{
						dp("--BeforeFinish---------------------------------------\n");
						PrintTextureInfo();
						dp("-------------------------------------------------------\n");
	
						FreeScreenTextures(screenTextureList,screenTextureList2,screenTexList);

						dp("--AfterFinish---------------------------------------\n");
						PrintTextureInfo();
						dp("-------------------------------------------------------\n");
	
						screenGrabbed = 0;
					}
					else
					{
						for (i=0; i<numRequired * 4; i++)
						{
							sVal = (curTicks-startTicks)/100.0;
							sVal = sinf(sVal+screenVtxList[i].sx)+cosf(sVal+screenVtxList[i].sy)+2;
							sVal /=4;
							fVal = (1-(float)(curTicks-(startTicks+1000))/1000.0);
							
							if (fVal>1)
							{
								fVal -= 1;
								sVal = fVal+(sVal*(1-fVal));
								
							}
							else
								if (fVal>0)
									sVal *= fVal;
								else
									sVal = 0;
							
							screenVtxList[i].color = D3DRGBA(1,1,1,sVal);
						}

						DrawScreenOverlays();
					}
				}

				/*if (pauseMode)
				{
					for (i=0; i<numRequired * 4; i++)
						screenVtxList[i].color = D3DRGBA(1,1,1,0.8);
					
					DrawScreenOverlays();
					GrabScreenTextures(surface[RENDER_SRF], screenTextureList, screenTextureList2);					
					//FreeScreenTextures(screenTextureList);
				}*/
				if( !shotMode )				
					DDrawFlip();
				else
					DirectXFlip();

				
			}
			
			DDrawClearSurface(RENDER_SRF, 0, DDBLT_COLORFILL);
			DDrawClearSurface(ZBUFFER_SRF, -1, DDBLT_DEPTHFILL);

			CleanBufferSamples();

			//Update3DListener ( currCamSource[0].v[X], currCamSource[0].v[Y], currCamSource[0].v[Z]);

			if ((frameCount == 1) && (gameState.mode == INGAME_MODE))
			{
				if (rKeying)
					RecordKeyInit(player[0].worldNum, player[0].levelNum);
				if (rPlaying)
					PlayKeyInit(player[0].worldNum, player[0].levelNum);

				if( gameState.multi == MULTIREMOTE )
				{
					if (DPInfo.bIsHost)
						InitialServerSynch();
					else
						InitialPlayerSynch();
					
					nextSynchAt = synchSpeed;
				}

				actTickCountModifier = GetTickCount()-1;
				gameSpeed = 1;
				actTickCount = actFrameCount = 1;
			}
			else
			{
				if( gameState.multi == MULTIREMOTE )
				{
					if (DPInfo.bIsHost)
					{
						if (actFrameCount >	nextSynchAt)
						{
							nextSynchAt = actFrameCount + synchSpeed;
							SendSynchMessage();
						}
					}
					else
					{
						if ((actFrameCount+pingOffset) > nextSynchAt)
						{
							nextSynchAt = actFrameCount + synchSpeed;
							SendPingMessage();
						}

						if (synchedFrameCount>0)
						{
							synchedFrameCount-=synchRecovery;
							actTickCountModifier+=synchRecovery;
						}
						else
						{
							synchedFrameCount+=synchRecovery;
							actTickCountModifier-=synchRecovery;
						}
					}
				}

				newTickCount = (GetTickCount()-actTickCountModifier)*turbo;

				if (gameState.mode == PAUSE_MODE)
				{
					pauseGameSpeed = (newTickCount-actTickCount)/(1000.0/60.0);
					actTickCount = newTickCount;
					gameSpeed = 0;					
				}
				else
				{
					lastActFrameCount = actFrameCount;
#ifdef FIX_FRAME_TIME
					actTickCount += gameSpeed*60;
					actFrameCount += gameSpeed;
#else
					gameSpeed = (newTickCount-actTickCount)*(60.0/1000.0);
					actTickCount = newTickCount;
					actFrameCount = actTickCount*(60.0/1000.0);
#endif
/*					if( gameSpeed < targetSpeed ) farClip+=10;
					else farClip-=10;

					if( farClip > 6000 ) farClip = 6000;
					else if( farClip < 1000 ) farClip = 1000;
*/				}
			}		
		}
	}

	// Save stuff in the registry
	SetRegistryInformation();

	// clean up
	FreeAllLists();
	DeInitPCSpecifics();
	DeInitInputDevices();
#ifdef USE_EDITOR
	ShutdownEditor();
#endif
	UnInitMPDirectPlay();

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
    wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FROGGER));
    wc.hCursor			= NULL;
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
		winMode?0:WS_EX_TOPMOST,
        "Frogger2PC",
        "Frogger2PC",
		winMode?WS_OVERLAPPEDWINDOW | WS_POPUP | WS_VISIBLE:WS_POPUP,
		//WS_OVERLAPPEDWINDOW | WS_POPUP | WS_VISIBLE,			// window style
        0,
		0,
		winMode?640:GetSystemMetrics(SM_CXSCREEN), // 640
		winMode?480:GetSystemMetrics(SM_CYSCREEN), // 480
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
	//ShowWindow(winInfo.hWndMain,SW_SHOW);
												
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
		case WM_SETCURSOR:
			SetCursor(NULL);
			break;

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

		case WM_KEYUP:
			switch( (int)wParam )
			{
			case VK_SHIFT: mod &= ~FRED_SHIFT; break;
			case VK_CONTROL: mod &= ~FRED_CONTROL; break;
			}
			break;

		case WM_KEYDOWN:
			if (rPlaying)
				StopKeying();

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
			if( chatFlags & CHAT_INPUT )
			{
				ChatInput((char)wParam);
				return 0;
			}
			else if (editorOk)	// only when editor is set up to "grab" keyboard data
			{
				EditorKeypress((char)wParam);
				return 0;
			}
			break;

		case WM_ERASEBKGND:
		{
/*			RECT cRect;
			int i,j;
			HDC appBackgndDC;

			GetClientRect (hWnd,&cRect);
			appBackgndDC=CreateCompatibleDC((HDC)wParam);
	
			if (SelectObject (appBackgndDC,appBackgnd)==NULL)
				dp("sharugar");

			for (i=0; i<cRect.right+128; i+=128)
				for (j=0; j<cRect.bottom+128; j+=128)
					if (!BitBlt((HDC)wParam,i,j,128,128,appBackgndDC,0,0,SRCCOPY))
										 dp("bugger");
			DeleteDC(appBackgndDC);
      			*/
			return TRUE;
		 }

		case WM_ACTIVATE:
		{
			switch (LOWORD(wParam))
			{
				

				case WA_ACTIVE:
				case WA_CLICKACTIVE:
					keyInput = 1;
					break;
				case WA_INACTIVE:
					keyInput = 0;
					break;
			}
			break;
		}

		case MM_MCINOTIFY:
		{
			// Loop cd track when it finishes
			switch( (int)wParam )
			{
			case MCI_NOTIFY_SUCCESSFUL:
				PrepareSongForLevel( player[0].worldNum, player[0].levelNum );
				break;
			}

			break;
		}
	}

    return DefWindowProc(hWnd,message,wParam,lParam);
}



VECTOR oldCCSource, oldCCTarget;
extern long numSprites;
extern long numPixelsDrawn;
char fR,fG,fB;
long useBilerpN = 0;
long useBilerpF = 0;
extern D3DTEXTUREHANDLE lastH;

unsigned long oldTH = 0;

unsigned long blinkTimer = 0;
unsigned long blinkTimer2 = 0;

unsigned long blinkSpeed = 5;
unsigned long blinkRand = 5;

unsigned long blinkSpeed2 = 10;
unsigned long blinkRand2 = 300;

/*	--------------------------------------------------------------------------------
	Function		: SetupViewMatrix
	Purpose			: Put together a view matrix.
	Parameters		: 
	Returns			: 
	Info			: 

*/

void SetupViewMatrix(void)
{
 	guLookAtF(vMatrix,
			currCamTarget.v[X],currCamTarget.v[Y],currCamTarget.v[Z],
			currCamSource.v[X],currCamSource.v[Y],currCamSource.v[Z],
			camVect.v[X],camVect.v[Y],camVect.v[Z]);	
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
	totalFacesDrawn = 0;

	StartTimer(1,"Draw Gfx");

	FindTexture(&frogEyeOpen,UpdateCRC("fgeye.bmp"),1);
	FindTexture(&frogEyeClosed,UpdateCRC("fgeye2.bmp"),1);

	if (frogEyeOpen)
	{
		
		if (oldTH==0)
		{
			if (actFrameCount>blinkTimer2)
			{
				unsigned long newH = ((TEXENTRY *)frogEyeClosed)->hdl;
				oldTH = ((TEXENTRY *)frogEyeOpen)->hdl;
				((TEXENTRY *)frogEyeOpen)->hdl = newH;
				blinkTimer = actFrameCount + blinkSpeed + Random(blinkRand);
			}
		}
		else
		{
			if (actFrameCount>blinkTimer)
			{
				((TEXENTRY *)frogEyeOpen)->hdl = oldTH;
				oldTH = 0;
				blinkTimer2 = actFrameCount + blinkSpeed2 + Random(blinkRand2);

			}
		}
	}

	/*if (useBilerpN)
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_LINEAR);
	else
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);

	if (useBilerpF)
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMIN,D3DFILTER_LINEAR);
	else
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMIN,D3DFILTER_NEAREST);
*/

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_FOGCOLOR, D3DRGBA(0,0,0,0));
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_FOGTABLESTART, *(DWORD *)&fStart );
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_FOGTABLEEND, *(DWORD *)&fEnd );

	numPixelsDrawn=0;
	numSprites = 0;
	
	SetupViewMatrix();

	currentFrameTime = timeGetTime();
	
	StartTimer(2,"XformActorList");
	XformActorList();
	EndTimer(2);

//	if(spriteList.numEntries)
//		AnimateSprites();

	// Actual stuff that draws
	if (runHardware)
		BeginDrawHardware();
	else
		SoftwareBeginFrame();

	
	
	StartTimer(3,"DrawActorList");
	DrawActorList();	
	EndTimer(3);

	DrawRandomPolyList( );

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
//	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
//	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,FALSE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZENABLE,TRUE);

	DrawBatchedPolys();
	BlankFrame(spaghetti);

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);
	
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,FALSE);
	if(sprList.count)
		PrintSprites();

	DrawBatchedPolys();
	BlankFrame(bolognese);

	//BeginDrawHardware();
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);

	DrawSpecialFX();
	DrawBatchedPolys();
	BlankFrame(parmesan);

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_ONE);
	SwapFrame(3);
	DrawBatchedPolys();
	BlankFrame(parmesan);
	SwapFrame(0);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);


	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_ONE);
	
	CheckHaloPoints();
	DrawHalos();
	
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_LINEAR);
	PrintSpriteOverlays(0);	
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);//D3DFILTER_LINEAR);
	lastH = -1;
	PrintTextOverlays();
	PrintSpriteOverlays(1);	
	
	//pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
	//pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_COLORKEYENABLE,TRUE);

#ifdef USE_EDITOR
	if (editorOk)
	{
		DrawBatchedPolys();
		BlankFrame(margerine);
	
		DrawEditor();
		DrawBatchedPolys();
		BlankFrame(basil);
	}
#endif

	if( chatFlags && gameState.mode == INGAME_MODE )
		DrawChatBuffer( 100, 20, 540, 150 );
	
	/* CAMERA SPACE STUFF */
	// Back up currCamSource and currCamTarget
	oldCCSource = currCamSource;
	oldCCTarget = currCamTarget;
	
	if( text3DList.numEntries )
	{
		Calculate3DText( );
		Print3DText( );
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
		DrawBatchedPolys();
		BlankFrame(kidneybeans);
		pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
	}

	// Set them so we don't do any modelling transforms
//	currCamSource = zero;
//	currCamTarget = inVec;
	
	// Restore currCam vectors
	currCamSource = oldCCSource;
	currCamTarget = oldCCTarget;
	/* END CAMERA SPACE STUFF */
	EndTimer(1);
	EndTimer(0);

	if (drawTimers)
	if (drawTimers<3)
	{
		PrintTimers();
		{	
			HDC hdc;
			HRESULT res = IDirectDrawSurface4_GetDC(surface[RENDER_SRF], &hdc);
			char speed[255];
					
			if (res == DD_OK)
			{
				sprintf(speed,"%i : %i : - : %.2f fps - %lu polys (%lu) - %lu sprites- %lu pixels",myLatency,synchedFrameCount,(60.0/gameSpeed),numFacesDrawn,totalFacesDrawn,numSprites,numPixelsDrawn);
				
				SetBkMode(hdc, TRANSPARENT);
				SetTextColor(hdc, RGB(255,0,0));
				TextOut(hdc,50,28, speed, strlen(speed));

				IDirectDrawSurface4_ReleaseDC(surface[RENDER_SRF], hdc);
			}
		}
	}
	
	if (KEYPRESS(DIK_F6))
		HoldTimers();
		
	ClearTimers();
	StartTimer(1,"Draw Gfx");
	StartTimer(0,"Everything");

	if( gameState.mode == INGAME_MODE )
		ProcessProcTextures( );

	if (runHardware)
		EndDrawHardware();
	else
		SoftwareEndFrame();

	AnimateTexturePointers();

	EndTimer(1);
}
