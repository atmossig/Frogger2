/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: block.c
	Programmer	: Andy Eder
	Date		: 28/06/99 10:19:37

----------------------------------------------------------------------------------------------- */

#include <windows.h>
//#include "directx.h"

//#include "ultra64.h"
//#include <stdio.h>
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

#include <temp_pc.h>

psFont *font;

extern char baseDirectory[MAX_PATH] = "X:\\TeamSpirit\\pcversion\\";

WININFO winInfo;

int PASCAL WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	CommonInit();

	winInfo.hInstance = hInstance;
	winInfo.hWndMain = NULL;

	while (1)
		GameLoop();
}
