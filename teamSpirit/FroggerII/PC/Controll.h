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

extern unsigned long	playerInputPause;
extern unsigned long	playerInputPause2;


//----- [ PC RELATED ] -------------------------------------------------------------------------//


#define KEYPRESS(keyDIK)	(keyTable[keyDIK] & 0x80)

extern BYTE keyTable[256];
extern DIMOUSESTATE mouseState;


extern LPDIRECTINPUT lpDI;
extern LPDIRECTINPUTDEVICE lpKeyb;
extern LPDIRECTINPUTDEVICE lpMouse;

//----- [ FUNCTION PROTOTYPES ] -----//

extern BOOL InitInputDevices();
extern void DeInitInputDevices();

extern BOOL InitKeyboardControl();
extern void DeInitKeyboardControl();

extern BOOL InitMouseControl();
extern void DeInitMouseControl();

extern void ProcessUserInput(HWND hWnd);

extern void ResetParameters();

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

typedef struct tagOSContPad 
{
	unsigned long button;
	char stick_x;
	char stick_y;
	unsigned char errno;
} OSContPad;

extern OSContPad controllerdata[4];

#ifdef __cplusplus
}
#endif


#endif
