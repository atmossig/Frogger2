/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: block.h
	Programmer	: Andy Eder
	Date		: 28/06/99 10:19:51

----------------------------------------------------------------------------------------------- */

#ifndef BLOCK_H_INCLUDED
#define BLOCK_H_INCLUDED


#define COLLISION_BASE	"collision\\"
#define SCENIC_BASE		"scenics\\"
#define OBJECT_BASE		"objects\\"
#define TEXTURE_BASE	"textures\\"
#define ENTITY_BASE		"maps\\"
#define SAVEDATA_BASE	"savedata\\"
#define SFX_BASE		"sfx\\"

#ifndef __cplusplus
#define sinf(x)			((float)sin(x))
#define cosf(x)			((float)cos(x))
#define sqrtf(x)		((float)sqrt(x))
#endif

typedef struct TAGWININFO
{
	HWND	hWndMain;
	HANDLE	hInstance;

} WININFO;

extern long winMode;
extern WININFO winInfo;
extern BYTE lButton, rButton;
extern char baseDirectory[MAX_PATH];
extern char outputMessageBuffer[256];
extern unsigned long actFrameCount, lastActFrameCount;
extern unsigned long currentFrameTime;
extern long keyInput;

extern char	transparentSurf;
extern char	xluSurf;
extern char	aiSurf;
extern char	UseAAMode;
extern char	UseZMode;
extern char	desiredFrameRate;
extern char	newDesiredFrameRate;
extern int runQuit;
extern float gameSpeed;

void debugPrintf(int num);
void Crash(char *mess);

int InitialiseWindows(HINSTANCE hInstance,int nCmdShow);
long FAR PASCAL WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

void DrawGraphics();



#endif
