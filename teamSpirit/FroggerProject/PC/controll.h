/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: Controll.h
	Programmer	: Andrew Eder
	Date		: 12/1/98

----------------------------------------------------------------------------------------------- */

#ifndef CONTROLL_H_INCLUDED
#define CONTROLL_H_INCLUDED

#include <dinput.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* -------------------------------------------------------------------- */

extern unsigned long	playerInputPause;
extern unsigned long	playerInputPause2;

extern BOOL keysEnabled;

typedef struct
{
	short player;
	DWORD button;
	DWORD key;

} KEYENTRY;

#define KEYPRESS(keyDIK)	(keyTable[keyDIK] & 0x80)

#define MAXJOYPADS 4
#define MAXBUTTONS 6

extern BYTE keyTable[256];
extern DIMOUSESTATE mouseState;

extern KEYENTRY keymap[56];

extern unsigned long rKeying;
extern unsigned long rPlaying;

extern LPDIRECTINPUT lpDI;
extern LPDIRECTINPUTDEVICE lpKeyb;
extern LPDIRECTINPUTDEVICE lpMouse;

/* ---------- Function prototypes ------------------- */

extern BOOL InitInputDevices();
extern void DeInitInputDevices();
extern void ProcessUserInput();
extern void ResetParameters();
extern BOOL SetupControllers(HWND hwnd);
extern void MakeKeyMap( );
extern void RecordKeyInit(unsigned long worldNum, unsigned long levelNum);
extern void PlayKeyInit(unsigned long worldNum, unsigned long levelNum);
extern void PlayKeyDone(void);

/* -------------------------------------------------- */

#ifdef __cplusplus
}
#endif


#endif
