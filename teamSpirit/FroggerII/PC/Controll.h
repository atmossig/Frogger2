/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: Controll.h
	Programmer	: Andrew Eder
	Date		: 12/1/98

----------------------------------------------------------------------------------------------- */

#ifndef CONTROLL_H_INCLUDED
#define CONTROLL_H_INCLUDED


#ifdef __cplusplus
extern "C"
{
#endif

/* -------------------------------------------------------------------- */

extern unsigned long	playerInputPause;
extern unsigned long	playerInputPause2;

extern BOOL keysEnabled;

#define CONT_UP     0x0001
#define CONT_DOWN   0x0002
#define CONT_LEFT   0x0004
#define CONT_RIGHT  0x0008

#define CONT_A		0x0010
#define CONT_B		0x0020
#define CONT_C		0x0040
#define CONT_D		0x0080
#define CONT_E		0x0100
#define CONT_F		0x0200
#define CONT_G		0x0400
#define CONT_L		0x0800
#define CONT_R		0x1000
#define CONT_START	0x2000
#define CONT_SHIFT	0x4000

typedef struct tagOSContPad 
{
	unsigned long button, lastbutton;
	char stick_x;
	char stick_y;
	unsigned char errornumber;
} OSContPad;

extern OSContPad controllerdata[4];

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
extern unsigned long joypadMap[MAXJOYPADS][MAXBUTTONS];

extern LPDIRECTINPUT lpDI;
extern LPDIRECTINPUTDEVICE lpKeyb;
extern LPDIRECTINPUTDEVICE lpMouse;

/* ---------- Function prototypes ------------------- */

extern BOOL InitInputDevices();
extern void DeInitInputDevices();
extern void ProcessUserInput(HWND hWnd);
extern void ResetParameters();
extern BOOL SetupControllers(HWND hwnd);
extern void MakeKeyMap( );

/* -------------------------------------------------- */

#ifdef __cplusplus
}
#endif


#endif
