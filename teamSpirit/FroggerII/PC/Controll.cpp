/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: Controll.c
	Programmer	: Andrew Eder
	Date		: 11/9/98

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>

#include "incs.h"

extern "C"
{

OSContPad controllerdata[4];


//----- [ PC RELATED ] -------------------------------------------------------------------------//


LPDIRECTINPUT lpDI			= NULL;
LPDIRECTINPUTDEVICE lpKeyb	= NULL;
LPDIRECTINPUTDEVICE lpMouse	= NULL;

BYTE keyTable[256];
DIMOUSESTATE mouseState;

unsigned long	playerInputPause;
unsigned long	playerInputPause2;


/*	--------------------------------------------------------------------------------
	Function	: InitInputDevices
	Purpose		: initialises input devices
	Parameters	: 
	Returns		: BOOL - TRUE on success, else FALSE
	Info		: 
*/
BOOL InitInputDevices()
{
	HRESULT hRes;

	hRes = DirectInputCreate(winInfo.hInstance,DIRECTINPUT_VERSION,&lpDI,NULL);
	if(FAILED(hRes))
		return FALSE;

	if(!InitKeyboardControl())
	{
		DeInitInputDevices();
		return FALSE;
	}

	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function	: DeInitInputDevices
	Purpose		: de-initialises input devices
	Parameters	: 
	Returns		: void
	Info		: 
*/
void DeInitInputDevices()
{
	DeInitKeyboardControl();
	DeInitMouseControl();

	if(lpDI)
	{
		lpDI->Release();
		lpDI = NULL;
	}
}

/*	--------------------------------------------------------------------------------
	Function	: InitKeyboardControl();
	Purpose		: initialises DirectInput keyboard control
	Parameters	: none
	Returns		: TRUE on success - else FALSE
	Info		: 
*/

BOOL InitKeyboardControl()
{
	HRESULT hRes;

	hRes = lpDI->CreateDevice(GUID_SysKeyboard,&lpKeyb,NULL);
	if(FAILED(hRes))
		return FALSE;

	hRes = lpKeyb->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hRes))
		return FALSE;

	hRes = lpKeyb->SetCooperativeLevel(winInfo.hWndMain,DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	if(FAILED(hRes))
		return FALSE;

	hRes = lpKeyb->Acquire();
	if(FAILED(hRes))
		return FALSE;

	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function	: DeInitKeyboardControl()
	Purpose		: cleans up DirectInput
	Parameters	: none
	Returns		: none
	Info		: 
*/
void DeInitKeyboardControl()
{
	if(lpKeyb)
	{
		lpKeyb->Unacquire();
		lpKeyb->Release();
		lpKeyb = NULL;
	}
}

/*	--------------------------------------------------------------------------------
	Function	: InitMouseControl();
	Purpose		: initialises DirectInput mouse control
	Parameters	: none
	Returns		: TRUE on success - else FALSE
	Info		: 
*/

BOOL InitMouseControl()
{
	HRESULT hRes;

	hRes = lpDI->CreateDevice(GUID_SysMouse,&lpMouse,NULL);
	if(FAILED(hRes))
		return FALSE;

	hRes = lpMouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(hRes))
		return FALSE;

	hRes = lpMouse->SetCooperativeLevel(winInfo.hWndMain,DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	if(FAILED(hRes))
		return FALSE;

	hRes = lpMouse->Acquire();
	if(FAILED(hRes))
		return FALSE;

	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function	: DeInitMouseControl()
	Purpose		: cleans up DirectInput
	Parameters	: none
	Returns		: none
	Info		: 
*/
void DeInitMouseControl()
{
	if(lpMouse)
	{
		lpMouse->Unacquire();
		lpMouse->Release();
		lpMouse = NULL;
	}
}

/*	--------------------------------------------------------------------------------
	Function	: ProcessUserInput
	Purpose		: processes keyboard input / mouse
	Parameters	: HWND
	Returns		: void
	Info		: 
*/

void ProcessUserInput(HWND hWnd)
{
	HRESULT hRes;

	// read keyboard data
	hRes = lpKeyb->GetDeviceState(sizeof(keyTable),&keyTable);
	if(FAILED(hRes))
		return;

	//----- [ KEYBOARD CONTROL ] -----//

	if(KEYPRESS(DIK_F12))
	{
		// Quit
		PostMessage(hWnd,WM_CLOSE,0,0);
	}

	if(KEYPRESS(DIK_ESCAPE))
	{
		// Quit
		PostMessage(hWnd,WM_CLOSE,0,0);
	}
	controllerdata[0].button = 0;

	if (KEYPRESS(DIK_UP))
		controllerdata[0].button |= CONT_UP;

	if (KEYPRESS(DIK_DOWN))
		controllerdata[0].button |= CONT_DOWN;

	if (KEYPRESS(DIK_LEFT))
		controllerdata[0].button |= CONT_LEFT;

	
	if (KEYPRESS(DIK_RIGHT))
		controllerdata[0].button |= CONT_RIGHT;
	
	if (KEYPRESS(DIK_A))
		controllerdata[0].button |= CONT_A;
	
	if (KEYPRESS(DIK_B))
		controllerdata[0].button |= CONT_B;
	
	if (KEYPRESS(DIK_S))
		controllerdata[0].button |= CONT_START;

	if (KEYPRESS(DIK_NUMPAD4))
		controllerdata[0].button |= CONT_C;

	if (KEYPRESS(DIK_NUMPAD6))
		controllerdata[0].button |= CONT_F;

	if (KEYPRESS(DIK_NUMPAD8))
		controllerdata[0].button |= CONT_E;

	if (KEYPRESS(DIK_NUMPAD2))
		controllerdata[0].button |= CONT_D;

	if (KEYPRESS(DIK_L))
		controllerdata[0].button |= CONT_L;

	if (KEYPRESS(DIK_R))
		controllerdata[0].button |= CONT_R;

}

}	// extern "C" end