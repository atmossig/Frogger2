/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: block.c
	Programmer	: Andy Eder
	Date		: 28/06/99 10:19:37

----------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <math.h>
#include <islutil.h>
#include <islpad.h>

#include <anim.h>
#include <stdio.h>

#include "game.h"
//#include "netgame.h"
#include "types2d.h"
//#include "controll.h"
#include "frogger.h"
#include "levplay.h"
#include "frogmove.h"
#include "cam.h"
#include "tongue.h"
#include "babyfrog.h"
#include "hud.h"
#include "frontend.h"
//#include "objects.h"
#include "textover.h"
//#include "3dtext.h"
#include "multi.h"
#include "layout.h"
#include "platform.h"
#include "enemies.h"
//#include "audio.h"
//#include "ptexture.h"
#include "levprog.h"
#include "event.h"
//#include "newstuff.h"
#include "main.h"
#include "newpsx.h"
#include "Main.h"
#include "actor2.h"
#include "bbtimer.h"
#include "maths.h"

#include "editor.h"

#include "temp_pc.h"
#include "pcsprite.h"
#include "pcgfx.h"
#include "ptexture.h"

#include "controll.h"

#include "mdx.h"

extern "C"
{
	psFont *font = 0;
	psFont *fontSmall = 0;
	MDX_FONT *pcFont;
	MDX_FONT *pcFontSmall;
}

extern char baseDirectory[MAX_PATH] = "X:\\TeamSpirit\\pcversion\\";

char lButton = 0, rButton = 0;
int editorOk = 0;

float camY = 100,camZ = 100;
extern "C" {MDX_LANDSCAPE *world;}


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
				editorOk = !editorOk; keysEnabled = !keysEnabled; break;
			default:
				return 1;
			}
			break;

		case WM_CHAR:
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

void CalcViewMatrix(void)
{
	guLookAtF (vMatrix.matrix,
		currCamTarget.vx*CAMVECTSCALE, currCamTarget.vy*CAMVECTSCALE, currCamTarget.vz*CAMVECTSCALE,
		currCamSource.vx*CAMVECTSCALE, currCamSource.vy*CAMVECTSCALE, currCamSource.vz*CAMVECTSCALE,
		camVect.vx*CAMVECTSCALE, camVect.vy*CAMVECTSCALE, camVect.vz*CAMVECTSCALE);
}

long DrawLoop(void)
{
	POINT t;
	D3DSetupRenderstates(D3DDefaultRenderstates);
	
	// Just to get functionality... ;)
	StartTimer (2,"DrawActorList (old)");
	DrawActorList();
	EndTimer(2);

	CalcViewMatrix();

	BlankAllFrames();
	SwapFrame(MA_FRAME_NORMAL);

	D3DSetupRenderstates(cullCWRS);
//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);

	if (world)
		DrawLandscape(world);

	StartTimer(1,"Actors");
	ActorListDraw();
	EndTimer(1);

	BeginDraw();
	DrawAllFrames();
	BlankAllFrames();
	
//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_LINEAR);
	D3DSetupRenderstates(xluZRS);
	D3DSetupRenderstates(cullNoneRS);
	
	// Draw Sprites
	if(sprList.count)
		PrintSprites();

	// FX and shadows
	DrawSpecialFX();

	// Light halos
//	CheckHaloPoints();
//	DrawHalos();

	SwapFrame(0);
	DrawBatchedPolys();
	BlankFrame;

	D3DSetupRenderstates(xluAddRS);
	SwapFrame(3);
	DrawBatchedPolys();
	BlankFrame;
	D3DSetupRenderstates(xluSemiRS);
	SwapFrame(0);

	D3DSetupRenderstates(xluZRS); // Turn off z write enable
	D3DSetupRenderstates(noZRS);  // And z enable

	PrintSpriteOverlays(0);	
	PrintTextOverlays();
	PrintSpriteOverlays(1);	
	if (editorOk)
		DrawEditor();

	EndDraw();


	EndTimer(0);
	if (consoleDraw)
		PrintConsole();
	if (timerDraw)
		PrintTimers();
	ClearTimers();
	StartTimer(0,"Everything");
	
	DDrawFlip();
	D3DClearView();

	GetCursorPos(&t);
	camZ = t.x*8;
	camY = t.y*8;

	if( gameState.mode == INGAME_MODE )
		ProcessProcTextures( );
	
//	AnimateTexturePointers();

	return 0;
}

long LoopFunc(void)
{
	ACTOR2 *c;

	actFrameCount = timeInfo.frameCount;
	gameSpeed = 4096*timeInfo.speed;

	ProcessUserInput();

	GameLoop();

	for (c = actList; c; c = c->next)
	{
		if (c->actor->actualActor)
		{
			MDX_ACTOR *a = (MDX_ACTOR*)c->actor->actualActor;

			if (!c->draw)
			{
				a->visible = 0;
				continue;
			}
			else
				a->visible = 1;

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

	if (editorOk)
		RunEditor();

	UpdateWater();
	DrawLoop();

	return 0;
}

int PASCAL WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	MDX_TEXENTRY *t;
	char waterFile[MAX_PATH];

	// Init common controls
	InitCommonControls();

	// Init windows
	if (!WindowsInitialise(hInstance,"Frogger2",1))
		return 1;

	// Init DDraw Object
	if (!DDrawInitObject (NULL))
		return 1;

	// Setup our sufaces
	if (!DDrawCreateSurfaces (mdxWinInfo.hWndMain, 640, 480, 16,TRUE, 16))
		return 2;

	// Setup D3D
	if (!D3DInit())
		return 3;
	
	// Initialise the matrix stack
	MatrixStackInitialise();
	
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

	CommonInit();

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

	mdxSetUserWndProc(MainWndProc);

	RunWindowsLoop(&LoopFunc);

	// Byeeeeeeeeeee
	ShutdownEditor();
	DeInitInputDevices();
	D3DShutdown();
	DDrawShutdown();	
	gelfShutdown();

	return 0;
}
