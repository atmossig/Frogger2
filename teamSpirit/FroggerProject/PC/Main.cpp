/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: block.c
	Programmer	: Andy Eder
	Date		: 28/06/99 10:19:37

----------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <dplay.h>
#include <dplobby.h>
#include <math.h>
#include <islutil.h>
#include <islpad.h>

#include <anim.h>
#include <stdio.h>

#include "game.h"
#include "types2d.h"
#include "frogger.h"
#include "levplay.h"
#include "frogmove.h"
#include "cam.h"
#include "tongue.h"
#include "babyfrog.h"
#include "hud.h"
#include "frontend.h"
#include "menus.h"
#include "textover.h"
#include "multi.h"
#include "layout.h"
#include "platform.h"
#include "enemies.h"
#include "event.h"
#include "main.h"
#include "newpsx.h"
#include "Main.h"
#include "actor2.h"
#include "bbtimer.h"
#include "maths.h"

#include "editor.h"

#include "pcsprite.h"
#include "pcgfx.h"
#include "backdrop.h"
#include "ptexture.h"
#include "dx_sound.h"
#include "banks.h"
#include "controll.h"
#include "pcmisc.h"

#include "mdx.h"
#include "mdxException.h"
#include "..\resource.h"
#include "..\network.h"
#include "..\netchat.h"
#include "..\netgame.h"

psFont *font = 0;
psFont *fontSmall = 0;
MDX_FONT *pcFont;
MDX_FONT *pcFontSmall;
long drawLandscape = 1;
long drawGame = 1;
long textEntry = 0;	
char textString[255] = "";

char baseDirectory[MAX_PATH] = "X:\\TeamSpirit\\pcversion\\";

#define REGISTRY_KEY "Software\\Atari\\Frogger2"

char lButton = 0, rButton = 0;
int editorOk = 0;

float camY = 100,camZ = 100;
extern "C" {MDX_LANDSCAPE *world;}

unsigned long nextSynchAt;
unsigned long actTickCountModifier = 0;
unsigned long synchSpeed = 60 * 1;
unsigned long pingOffset = 40;
unsigned long synchRecovery = 1;

long resolution;
long slideSpeeds[4] = {0,16,32,64};

long fogEnable = 0;

void GetArgs(char *arglist);

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
		utilPrintf("Couldn't open registry key\n"); return 0;
	}
	else
	{
		if (RegQueryValueEx(hkey, "InstallDir", NULL, NULL, (unsigned char*)baseDirectory, &len) == ERROR_SUCCESS)
		{
			if (baseDirectory[strlen(baseDirectory) - 1] != '\\')
				strcat(baseDirectory, "\\");

			utilPrintf("Using base directory: %s\n", baseDirectory);
		}
		else
			utilPrintf("Couldn't read InstallDir value from registry\n");

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
		utilPrintf("Couldn't open registry kep for writing\n"); return 0;
	}
	else
	{
		RegSetValueEx(hkey, "InstallDir", NULL, REG_SZ, (unsigned char*)baseDirectory, strlen(baseDirectory) + 1);
//		RegSetValueEx(hkey, "VideoCard", NULL, REG_SZ, videoCardName, strlen(videoCardName) + 1);
		RegCloseKey(hkey);
	}

	return 1;
}

/*	--------------------------------------------------------------------------------
	Function		: GetArgs
	Purpose			: Process Cmd Line Args
	Parameters		: 
	Returns			: 
	Info			: 
*/
void GetArgs(char *arglist)
{
	char cmdMode;

	do
	{
		if (*arglist=='+' || *arglist=='-' || *arglist=='/')
		{
			cmdMode = 1;
			while (cmdMode)
				switch(*(++arglist))
				{
					case 'C': case 'c':
						swingCam = !swingCam;
						utilPrintf("Swinging camera %s\n",swingCam?"enabled":"disabled");
						break;

					case 'R': case 'r':
						rKeying = 1;
						break;

					case 'P': case 'p':
						rPlaying = 1;
						break;

					case 'a': case 'A':
						useAudio = !useAudio;
						utilPrintf("Audio %s\n",useAudio?"enabled":"disabled");
						break;

					case 'D': case 'd':
						playDemos = !playDemos;
						utilPrintf("Demos %s\n",playDemos?"enabled":"disabled");
						break;

					case 'K': case 'k':
						debugKeys = !debugKeys;
						displayDebugInfo = !displayDebugInfo;
						utilPrintf("Debug keys %s\n",debugKeys?"enabled":"disabled");
						break;

					case 'L': case 'l':
						levelAccess = !levelAccess;
						utilPrintf("All level access mode is %s\n",levelAccess?"enabled":"disabled");
						break;

					case 'S': case 's':
						sortMode = MA_SORTBACKFRONT;
						break;

					case 'I': case 'i':
						screenshotEnable = !screenshotEnable;
						utilPrintf("Screenshot mode is %s\n",screenshotEnable?"enabled":"disabled");
						break;

					case ' ':
					case 0:
						cmdMode = 0;
						break;
				}
		}
	} while ((*arglist)++);
}


/*	-------------------------------------------------------------------------------
	Function:	MainWndProc
	Params:		As WNDPROC
	returns:	0 for success, other to pass on to mdx default handler
*/

LRESULT CALLBACK MyInitProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WIN32_FIND_DATA fData;
	HANDLE			fHandle;
	char			fName[MAX_PATH];
	char			fPath[MAX_PATH];
	long			idx,data;
	FILE			*fp;

    switch(msg)
	{
		case WM_INITDIALOG:
		{
			strcpy (fPath,baseDirectory);
			strcpy (fName,fPath);
			strcat (fName,"*.fsg");

			fHandle = FindFirstFile (fName,&fData);

			if (fHandle != INVALID_HANDLE_VALUE)
			{
				long ret;
				char finalFile[MAX_PATH];
				char finalShort[MAX_PATH];

				do
				{
					strcpy (finalFile,fPath);
					strcat (finalFile,fData.cFileName);
					strcpy (finalShort,fData.cFileName);
					ret = FindNextFile (fHandle,&fData);
					
					for (int i=0; finalShort[i]!=0; i++)
						if (finalShort[i] == '.')
							finalShort[i] = 0;

					SendMessage ( GetDlgItem(hWnd,IDC_LIST3),CB_INSERTSTRING,0,(long)finalShort);
				}
				while (ret);
			
				FindClose (fHandle);

				SendMessage ( GetDlgItem(hWnd,IDC_640),BM_SETCHECK,BST_CHECKED,0);
			}
			else
				dp("No savegames",fName);

			SendMessage ( GetDlgItem(hWnd,IDC_LIST3),CB_SETCURSEL,0,0);
			
			strcpy (fName,fPath);
			strcat (fName,"setup.fsc");
			fp = fopen(fName,"rb");
			if (fp)
			{
				fread(&data,1,4,fp);
				SendMessage ( GetDlgItem(hWnd,IDC_640),BM_SETCHECK,data,0);
				fread(&data,1,4,fp);
				SendMessage ( GetDlgItem(hWnd,IDC_800),BM_SETCHECK,data,0);
				fread(&data,1,4,fp);
				SendMessage ( GetDlgItem(hWnd,IDC_1024),BM_SETCHECK,data,0);
				fread(&data,1,4,fp);
				SendMessage ( GetDlgItem(hWnd,IDC_1280),BM_SETCHECK,data,0);
				fread(&data,1,4,fp);
				SendMessage ( GetDlgItem(hWnd,IDC_WINDOW),BM_SETCHECK,data,0);				
				fclose(fp);
			}

			return FALSE;			
		}		

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDC_MULTI:
					InitMPDirectPlay(mdxWinInfo.hInstance);
					return TRUE;					
				case IDOK:
				{
					
					strcpy (fPath,baseDirectory);
					strcpy (fName,fPath);
					strcat (fName,"setup.fsc");
					fp = fopen(fName,"wb");
					if (fp)
					{
						data = SendMessage (GetDlgItem(hWnd,IDC_640),BM_GETCHECK,0,0);
						fwrite(&data,1,4,fp);
						if (data)
							resolution=1;
						data = SendMessage (GetDlgItem(hWnd,IDC_800),BM_GETCHECK,0,0);
						fwrite(&data,1,4,fp);
						if (data)
							resolution=2;
						data = SendMessage (GetDlgItem(hWnd,IDC_1024),BM_GETCHECK,0,0);
						fwrite(&data,1,4,fp);
						if (data)
							resolution=3;
						data = SendMessage (GetDlgItem(hWnd,IDC_1280),BM_GETCHECK,0,0);
						fwrite(&data,1,4,fp);
						if (data)
							resolution=4;
						data = SendMessage (GetDlgItem(hWnd,IDC_WINDOW),BM_GETCHECK,0,0);
						fwrite(&data,1,4,fp);
						fclose(fp);
					}

					SendMessage ( GetDlgItem(hWnd,IDC_LIST3),WM_GETTEXT,16,(long)saveName);

					return FALSE;				
				}
			}
		}
	}

	return FALSE;
}

void TextInput( char c )
{
	long numc = strlen(textString);
	if (c == 8) // backspace
	{
		if (numc>0)
			textString[numc-1] = 0;
	}
	else if (c == 13) // enter
	{
		textEntry = 0;
	}
	else
	{
		if (numc<textEntry)
		{
			textString[numc] = c;
			textString[numc+1] = 0;
		}
	}
}

/*	-------------------------------------------------------------------------------
	Function:	MainWndProc
	Params:		As WNDPROC
	returns:	0 for success, other to pass on to mdx default handler
*/

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
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

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_F10:
				if( debugKeys )
				{
					editorOk = !editorOk;
					keysEnabled = !keysEnabled;
					break;
				}
			default:
				return 1;
			}
			break;

		case WM_CHAR:

			if (textEntry)
			{
				TextInput((char)wParam);
			}
			if( chatFlags & CHAT_INPUT )
			{
				ChatInput((char)wParam);
				return 0;
			}
			if (editorOk)	// only when editor is set up to "grab" keyboard data
			{
				EditorKeypress((char)wParam);
				return 0;
			}

			return 1;
			break;

	default:
		return 1;
	}

	return 0;
}

#define CAMVECTSCALE (1.0f/40960.0f)

void CalcViewMatrix(long uDate)
{
	guLookAtF (vMatrix.matrix,
		currCamTarget.vx*CAMVECTSCALE, currCamTarget.vy*CAMVECTSCALE, currCamTarget.vz*CAMVECTSCALE,
		currCamSource.vx*CAMVECTSCALE, currCamSource.vy*CAMVECTSCALE, currCamSource.vz*CAMVECTSCALE,
		camVect.vx*CAMVECTSCALE, camVect.vy*CAMVECTSCALE, camVect.vz*CAMVECTSCALE,uDate);
}

void DrawBackground(void)
{
	
	float oF = farClip;
//	float oFs = fStart, oFe = fEnd;
	if (!drawGame)
		return;

	SwapFrame(0);
	backGnd->actor->visible = 1;
	
	noClipping = 1;
	farClip = 124000;
	((MDX_ACTOR *)backGnd->actor->actualActor)->pos.vx = currCamSource.vx / 40960.0;
	((MDX_ACTOR *)backGnd->actor->actualActor)->pos.vy = currCamSource.vy / 40960.0;
	((MDX_ACTOR *)backGnd->actor->actualActor)->pos.vz = currCamSource.vz / 40960.0;
		
	XformActor(((MDX_ACTOR *)backGnd->actor->actualActor));
	DrawActor(((MDX_ACTOR *)backGnd->actor->actualActor));

	noClipping = 0;

	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,FALSE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,FALSE);
	//pDirect3DDevice->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_CLAMP);	// clamp textures
	//pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);//D3DFILTER_LINEAR);

//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,TRUE);
//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF,0);
//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC,D3DCMP_NOTEQUAL);

	//D3DSetupRenderstates(xluAddRS);

	DrawBatchedPolys();
	//D3DSetupRenderstates(xluSemiRS);

	// Draw the second mavis frame set, Transparent objects (non water objects)
//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,FALSE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,TRUE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,TRUE);
	//pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_WRAP);	// wrap textures
	//pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);
	
	BlankFrame;
}


long DrawLoop(void)
{
	POINT t;
	D3DSetupRenderstates(D3DDefaultRenderstates);
	// Just to get functionality... ;)
	StartTimer (2,"DrawActorList (old)");
	DrawActorList();

	if (editorOk)
		CalcViewMatrix(0);
	else
		CalcViewMatrix(1);

	//	changedView = 1;

	SetupFogParams(fog.min,0,0,0,0);

	BeginDraw();
	DrawBackdrop();

	if (backGnd)
		DrawBackground();
	EndDraw();

	farClip = fog.max;
	if (fog.min>0)
	{
		SetupFogParams(fog.min,fog.r/255.0,fog.g/255.0,fog.b/255.0,1);
		fogEnable = 1;
	}
	
	BlankAllFrames();
	SwapFrame(MA_FRAME_NORMAL);
	EndTimer(2);

	if (drawGame)
	{
		D3DSetupRenderstates(cullCWRS);

	//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);

		StartTimer(14,"Landscape");
		if (world && drawLandscape)
			DrawLandscape(world);
		EndTimer(14);
		StartTimer(1,"Actors");
		
		ActorListDraw();
		EndTimer(1);

		
		StartTimer(15,"DAF");
		if (rHardware)
		{
			DrawAllFrames();
			BlankAllFrames();
		}
		EndTimer(15);

		BeginDraw();
		
		StartTimer(16,"Sprites,text & SpecFX");
	//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_LINEAR);
		D3DSetupRenderstates(xluZRS);
		D3DSetupRenderstates(cullNoneRS);
		
		// Draw Sprites
		if(sprList.count)
			PrintSprites();

		D3DSetupRenderstates(xluZRS);
		// FX and shadows
		DrawSpecialFX();

		// Light halos
	//	CheckHaloPoints();
	//	DrawHalos();
	}
	else
	{
		D3DSetupRenderstates(xluZRS);
		D3DSetupRenderstates(cullNoneRS);
	}

	if (rHardware)
	{
		SwapFrame(0);
		DrawBatchedPolys();
		BlankFrame;

		D3DSetupRenderstates(xluAddRS);
		SwapFrame(3);
		DrawBatchedPolys();
		BlankFrame;
		D3DSetupRenderstates(xluSemiRS);
		SwapFrame(0);

		D3DSetupRenderstates(noZRS);  // And z enable
	}
	else
	{
//		D3DSetupRenderstates(xluAddRS);
		
		DrawAllFrames();
		BlankAllFrames();
	}

	PrintSpriteOverlays(0);	
	
	PrintTextOverlays();
	PrintSpriteOverlays(1);	

	if (editorOk)
		DrawEditor();

	if( chatFlags && gameState.mode == INGAME_MODE )
		DrawChatBuffer( 100, 20, 540, 150 );

	EndDraw();
	EndTimer(16);

	//CopySoftScreenToSurface(surface[RENDER_SRF]);

	EndTimer(0);
	
	if (textureDraw)
		ShowTextures();
	if (consoleDraw)
		PrintConsole();
	if (timerDraw)
		PrintTimers();
	
	ClearTimers();
	StartTimer(0,"Everything");
	
	
	StartTimer(17,"Flip");
	BeginDraw();
	EndDraw();

	if( screenshotEnable )
		if (KEYPRESS(DIK_F9))
			ScreenShot();

	DDrawFlip();
	EndTimer(17);
	StartTimer(18,"Clear");
	D3DClearView();
	EndTimer(18);
	GetCursorPos(&t);
	camZ = t.x*8;
	camY = t.y*8;

	StartTimer(19,"PText");
	if( gameState.mode != PAUSE_MODE )
		ProcessProcTextures( );
	EndTimer(19);
	
//	AnimateTexturePointers();

	return 0;
}

long LoopFunc(void)
{
	ACTOR2 *c;

	lastActFrameCount = actFrameCount;
	actFrameCount = timeInfo.frameCount;
	gameSpeed = 4096*timeInfo.speed;

	StartTimer(10,"Controller");
	ProcessUserInput();
	EndTimer(10);

	StartTimer(5,"Gameloop");
	GameLoop();
	EndTimer(5);

	StartTimer(11,"UpdateStuff");
	for (c = actList; c; c = c->next)
	{
		if (c->actor->actualActor)
		{
			MDX_ACTOR *a = (MDX_ACTOR*)c->actor->actualActor;
			long slideVal;
			slideVal = ((c->flags>>5) & 3);
			if (slideVal)
				SlideObjectTextures(a->objectController->object,slideSpeeds[slideVal]);

			if( a->objectController->object->flags & OBJECT_FLAGS_CLIPPED )
				c->clipped = 1;
			else
				c->clipped = 0;

			if (!c->draw)
			{
				a->visible = 0;
				a->draw = 0;
				continue;
			}
			else
			{
				a->visible = 1;
				a->draw = 1;
			}

			a->pos.vx = c->actor->position.vx * 0.1f;
			a->pos.vy = c->actor->position.vy * 0.1f;
			a->pos.vz = c->actor->position.vz * 0.1f;

			a->scale.vx = c->actor->size.vx * (1.0f/4096.0f);
			a->scale.vy = c->actor->size.vy * (1.0f/4096.0f);
			a->scale.vz = c->actor->size.vz * (1.0f/4096.0f);
	
			
			if (c->actor->qRot.w || c->actor->qRot.x || c->actor->qRot.y || c->actor->qRot.z)
			{
				a->qRot.x = c->actor->qRot.x * (1.0f/4096.0f);
				a->qRot.y = c->actor->qRot.y * (1.0f/4096.0f);
				a->qRot.z = c->actor->qRot.z * (1.0f/4096.0f);
				a->qRot.w = c->actor->qRot.w  * (1.0f/4096.0f);
			}
		}
	}
	EndTimer(11);

	StartTimer(12,"Editor");
	if (editorOk)
		RunEditor();
	EndTimer(12);

	if( KEYPRESS(DIK_F7) && chatFlags )
	{
		if( chatFlags & CHAT_INPUT )
		{
			chatFlags &= ~CHAT_INPUT;
		}
		else
		{
			if( !chatInput.msg )
				chatInput.msg = new char[MAX_CSLENGTH];

			chatInput.msgLen = 0;
			chatFlags |= CHAT_INPUT;
		}
	}

	StartTimer(13,"WaterUpdate");
	UpdateWater();
	EndTimer(13);

	DrawLoop();
	
	if(networkPlay && (gameState.mode == INGAME_MODE))
	{
		
		if (!synchedOK)
		{
			if (DPInfo.bIsHost)
				InitialServerSynch();
			else
				InitialPlayerSynch();
			
			InitTiming(60);
			synchedOK = 1;
			nextSynchAt = synchSpeed;
		}
		else
		{
			SendUpdateMessage();

			if (DPInfo.bIsHost)
			{
				if (actFrameCount >	nextSynchAt)
				{
					nextSynchAt = timeInfo.frameCount + synchSpeed;
					SendSynchMessage();
				}
			}
			else
			{
				if ((actFrameCount+pingOffset) > nextSynchAt)
				{
					nextSynchAt = timeInfo.frameCount + synchSpeed;
					SendPingMessage();
				}

				if (synchedFrameCount>0)
				{
					synchedFrameCount-=synchRecovery;
					actTickCountModifier+=synchRecovery;
					timeInfo.tickModifier = -actTickCountModifier;
				}
				else
				{
					synchedFrameCount+=synchRecovery;
					actTickCountModifier-=synchRecovery;
					timeInfo.tickModifier = -actTickCountModifier;
				}
			}
		}
	}

	return 0;
}

int PASCAL WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	MDX_TEXENTRY *t;
	char waterFile[MAX_PATH];
	long xRes,yRes;
	
	SYSTEMTIME currTime;
	GetLocalTime(&currTime);
	utilPrintf("\n------------- Starting Frogger2 ----------------\n"
		"Session started %02d/%02d/%d %02d:%02d:%02d\n",
		currTime.wDay, currTime.wMonth, currTime.wYear,
		currTime.wHour, currTime.wMinute, currTime.wSecond);

	GetRegistryInformation();
	
	SetUserVideoProc(MyInitProc);
	// Init common controls
	InitCommonControls();

	// Init windows
	if (!WindowsInitialise(hInstance,"Frogger2",1))
		return 1;

	// Init DDraw Object
	if (!DDrawInitObject (NULL))
		return 1;

	switch(resolution)
	{
		case 1:
			xRes = 640; yRes = 480; break;
		case 2:
			xRes = 800; yRes = 600; break;
		case 3:
			xRes = 1024; yRes = 768; break;
		case 4:
			xRes = 1280; yRes = 1024; break;
		default:
			xRes = 640; yRes = 480; break;
	}

	OVERLAY_X = xRes/4096.0;
	OVERLAY_Y = yRes/4096.0;
	// Setup our sufaces
	if (!DDrawCreateSurfaces (mdxWinInfo.hWndMain, xRes, yRes, 16,TRUE, 16))
		return 2;

	// Setup the renderer
	SetupRenderer(xRes, yRes);

	// Setup the profiler
	InitProfile();

	// Setup D3D
	if (!D3DInit())
		return 3;

	// Command line arguments
	GetArgs(lpCmdLine);

	// Setup the sound device
	InitDirectSound( mdxWinInfo.hInstance, mdxWinInfo.hWndMain );

	// Initialise the matrix stack
	InitMaths();
	
	// Initialise Johns BMP loader
	gelfInit();

	// Init the CRC table
	InitCRCTable();

	// Init a table for 1/n (Optimisation)
	InitOneOverTable();

	// Init mavis frameset
	InitFrames();
	
	// Init the font system
	InitFontSystem();

	// Clear the spare suface (Ready for lotsa nice fx)
	//DDrawClearSurface(SPARE_SRF, 0, DDBLT_COLORFILL);

	// Clear the timers for the initial frame
	ClearTimers();

	InitInputDevices();

	pcFont = InitFont("FontA",baseDirectory);
	pcFontSmall = InitFont("FontB",baseDirectory);
	LoadTexBank("Phong",baseDirectory);

	if (t = GetTexEntryFromCRC(UpdateCRC("phong.bmp")))
		phong = t;

	if (t = GetTexEntryFromCRC(UpdateCRC("light.bmp")))
		lightMap = t;

	font = (psFont *)pcFont;
	fontSmall = (psFont *)pcFontSmall;

	sprintf(waterFile,"%stextures\\ProcData\\",baseDirectory);
	InitWater(waterFile);

	InitTiming(60.0);

	InitEditor();

	CommonInit();
	
	mdxSetUserWndProc(MainWndProc);
	SPRITECLIPLEFT = clx0;
	SPRITECLIPTOP = cly0;
	SPRITECLIPRIGHT	= clx1;
	SPRITECLIPBOTTOM = cly1;
	
	LoadGame();
	
	RunWindowsLoop(&LoopFunc);

	// Byeeeeeeeeeee
	ShutdownEditor();
	DeInitInputDevices();
	D3DShutdown();
	DDrawShutdown();	
	gelfShutdown();

	return 0;
}
