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

#include "controll.h"

#include "mdx.h"

extern "C"
{
	psFont *font = 0;
	MDX_FONT *pcFont;
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

		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_F2:
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

	pDirect3DDevice->BeginScene();
	BlankAllFrames();
	SwapFrame(MA_FRAME_NORMAL);

	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_CW);

	if (world)
		DrawLandscape(world);

	StartTimer(1,"Actors");
	ActorListDraw();
	EndTimer(1);

	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_LINEAR);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,FALSE);
	
	// Draw Sprites
	if(sprList.count)
		PrintSprites();

	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);

	// FX and shadows
	DrawSpecialFX();

/*	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_ONE);
	// Light halos
	CheckHaloPoints();
	DrawHalos();
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
*/
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,TRUE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);

	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_LINEAR);
	PrintSpriteOverlays(0);	
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);//D3DFILTER_LINEAR);
	PrintSpriteOverlays(1);	

/*	if( text3DList.numEntries )
	{
		Calculate3DText( );
		Print3DText( );
	}
*/
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);


	DrawAllFrames();
	PrintTextOverlays();

	if (editorOk)
		DrawEditor();

	pDirect3DDevice->EndScene();

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

//	if( gameState.mode == INGAME_MODE )
//		ProcessProcTextures( );
	
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

	c = actList;
	while (c)
	{
		if (c->actor->actualActor)
		{
			((MDX_ACTOR *)(c->actor->actualActor))->pos.vx = c->actor->position.vx / 10.0;
			((MDX_ACTOR *)(c->actor->actualActor))->pos.vy = c->actor->position.vy / 10.0;
			((MDX_ACTOR *)(c->actor->actualActor))->pos.vz = c->actor->position.vz / 10.0;

			if (c->actor->qRot.w || c->actor->qRot.x || c->actor->qRot.y || c->actor->qRot.z)
			{
				((MDX_ACTOR *)(c->actor->actualActor))->qRot.x = c->actor->qRot.x / 4096.0;
				((MDX_ACTOR *)(c->actor->actualActor))->qRot.y = c->actor->qRot.y / 4096.0;
				((MDX_ACTOR *)(c->actor->actualActor))->qRot.z = c->actor->qRot.z / 4096.0;
				((MDX_ACTOR *)(c->actor->actualActor))->qRot.w = c->actor->qRot.w / 4096.0;
			}
		}

		c = c->next;
	}

	if (editorOk)
		RunEditor();

	DrawLoop();
	return 0;
}

int PASCAL WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
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
	font = (psFont *)pcFont;

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
