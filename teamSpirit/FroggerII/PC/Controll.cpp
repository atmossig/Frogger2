/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: Controll.c
	Programmer	: Andrew Eder
	Date		: 11/9/98

----------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <windowsx.h>
#include <wtypes.h>
#include <crtdbg.h>
#include <commctrl.h>
#include <cguid.h>
#include <dplay.h>
#include <dplobby.h>
#include "network.h"
#include "netgame.h"

extern "C"
{
#include <ultra64.h>
#include "stdio.h"
#include "incs.h"

OSContPad controllerdata[4];
long DEAD_ZONE = 50;
//----- [ PC RELATED ] -------------------------------------------------------------------------//


LPDIRECTINPUT lpDI			= NULL;
LPDIRECTINPUTDEVICE lpKeyb	= NULL;
LPDIRECTINPUTDEVICE lpJoystick = NULL;
LPDIRECTINPUTDEVICE2 lpJoystick2 = NULL;
LPDIRECTINPUTDEVICE lpMouse	= NULL;

// 14 buttons per controller, 4 controllers.
// Need to store DIK, controller number and controller command
KEYENTRY keymap[56] = 
{
	{ 0, CONT_UP, DIK_UP },
	{ 0, CONT_DOWN, DIK_DOWN },
	{ 0, CONT_LEFT, DIK_LEFT },
	{ 0, CONT_RIGHT, DIK_RIGHT },
	{ 0, CONT_A, DIK_RETURN },
	{ 0, CONT_B, DIK_RSHIFT },
	{ 0, CONT_START, DIK_ESCAPE },
	{ 0, CONT_C, DIK_NUMPAD4 },
	{ 0, CONT_D, DIK_NUMPAD2 },
	{ 0, CONT_E, DIK_NUMPAD8 },
	{ 0, CONT_F, DIK_NUMPAD6 },
	{ 0, CONT_G, DIK_NUMPAD0 },
	{ 0, CONT_L, DIK_RCONTROL },
	{ 0, CONT_R, DIK_R },

	{ 1, CONT_UP, DIK_T },
	{ 1, CONT_DOWN, DIK_G },
	{ 1, CONT_LEFT, DIK_F },
	{ 1, CONT_RIGHT, DIK_H },
	{ 1, CONT_A, DIK_Q },
	{ 1, CONT_B, 0 },
	{ 1, CONT_START, 0 },
	{ 1, CONT_C, 0 },
	{ 1, CONT_D, 0 },
	{ 1, CONT_E, 0 },
	{ 1, CONT_F, 0 },
	{ 1, CONT_G, 0 },
	{ 1, CONT_L, 0 },
	{ 1, CONT_R, 0 },

	{ 2, CONT_UP, DIK_I },
	{ 2, CONT_DOWN, DIK_K },
	{ 2, CONT_LEFT, DIK_J },
	{ 2, CONT_RIGHT, DIK_L },
	{ 2, CONT_A, DIK_M },
	{ 2, CONT_B, 0 },
	{ 2, CONT_START, 0 },
	{ 2, CONT_C, 0 },
	{ 2, CONT_D, 0 },
	{ 2, CONT_E, 0 },
	{ 2, CONT_F, 0 },
	{ 2, CONT_G, 0 },
	{ 2, CONT_L, 0 },
	{ 2, CONT_R, 0 },

	{ 3, CONT_UP, 0 },
	{ 3, CONT_DOWN, 0 },
	{ 3, CONT_LEFT, 0 },
	{ 3, CONT_RIGHT, 0 },
	{ 3, CONT_A, 0 },
	{ 3, CONT_B, 0 },
	{ 3, CONT_START, 0 },
	{ 3, CONT_C, 0 },
	{ 3, CONT_D, 0 },
	{ 3, CONT_E, 0 },
	{ 3, CONT_F, 0 },
	{ 3, CONT_G, 0 },
	{ 3, CONT_L, 0 },
	{ 3, CONT_R, 0 }
};

BYTE keyTable[256];
DIMOUSESTATE mouseState;
DIJOYSTATE joy;

unsigned long	playerInputPause;
unsigned long	playerInputPause2;

BOOL keysEnabled = TRUE;
long joyAvail = 1;
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

	if(!InitJoystickControl())
	{
		joyAvail = 0;
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
	if (joyAvail)
		DeInitJoystick();

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
	Function	: InitKeyboardControl();
	Purpose		: initialises DirectInput keyboard control
	Parameters	: none
	Returns		: TRUE on success - else FALSE
	Info		: 
*/

BOOL InitJoystickControl()
{
	HRESULT hRes;

	hRes = lpDI->CreateDevice(GUID_Joystick,&lpJoystick,NULL);
	if(FAILED(hRes))
		return FALSE;

	lpJoystick->QueryInterface( IID_IDirectInputDevice2,(LPVOID *)&lpJoystick2 );

	hRes = lpJoystick->SetDataFormat(&c_dfDIJoystick);
	if(FAILED(hRes))
		return FALSE;

	hRes = lpJoystick->SetCooperativeLevel(winInfo.hWndMain,DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	if(FAILED(hRes))
		return FALSE;

	hRes = lpJoystick->Acquire();
	if(FAILED(hRes))
		return FALSE;

	 // set the range of the joystick axis
    DIPROPRANGE diprg; 

    diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
    diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
    diprg.diph.dwHow        = DIPH_BYOFFSET; 
    diprg.lMin              = -1000; 
    diprg.lMax              = +1000; 

    diprg.diph.dwObj = DIJOFS_X;    // set the x-axis range
    hRes = lpJoystick->SetProperty( DIPROP_RANGE, &diprg.diph );
    if ( FAILED(hRes) ) 
        return FALSE;

    diprg.diph.dwObj = DIJOFS_Y;    // set the y-axis range
    hRes = lpJoystick->SetProperty( DIPROP_RANGE, &diprg.diph );
    if ( FAILED(hRes) ) 
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
	Function	: DeInitKeyboardControl()
	Purpose		: cleans up DirectInput
	Parameters	: none
	Returns		: none
	Info		: 
*/
void DeInitJoystick()
{
	if(lpJoystick)
	{
		lpJoystick->Unacquire();
		lpJoystick->Release();
		lpJoystick = NULL;
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
	long i,j;
	
	if (keyInput)
	{
		// read keyboard data
		hRes = lpKeyb->GetDeviceState(sizeof(keyTable),&keyTable);
		if(FAILED(hRes))
			return;
	}

//	if (joyAvail)
//	{
//		hRes = lpJoystick2->Poll();
//		if(FAILED(hRes))
//		{joyAvail = 0;
//		return;}
//		hRes = lpJoystick->GetDeviceState(sizeof(joy),&joy);
//		if(FAILED(hRes))
//		{joyAvail = 0;
//		return;}
//	}

	//----- [ KEYBOARD CONTROL ] -----//

	if(KEYPRESS(DIK_F12))
		PostMessage(hWnd,WM_CLOSE,0,0);
	
	controllerdata[0].lastbutton = controllerdata[0].button;
	controllerdata[1].lastbutton = controllerdata[1].button;
	controllerdata[2].lastbutton = controllerdata[2].button;
	controllerdata[3].lastbutton = controllerdata[3].button;

	controllerdata[0].button = 0;
	controllerdata[1].button = 0;
	controllerdata[2].button = 0;
	controllerdata[3].button = 0;

	if (!keysEnabled) return;
	
	if (KEYPRESS(DIK_NUMPAD7))
	{
		farClip*=1.2;
		horizClip*=1.2;
		vertClip*=1.2;
	}

	if (KEYPRESS(DIK_NUMPAD9))
	{
		farClip/=1.2;
		horizClip/=1.2;
		vertClip/=1.2;
	}


	if (KEYPRESS(DIK_Z))
		ShowJalloc();

	for( i=0; i < NUM_FROGS*14; i++ )
		if( keymap[i].key > 0 && KEYPRESS(keymap[i].key) )
			controllerdata[keymap[i].player].button |= keymap[i].button;
}


/*	--------------------------------------------------------------------------------
	Function		: ResetParameters
	Purpose			: resets parameters
	Parameters		: 
	Returns			: 
	Info			: 
*/
void ResetParameters()
{
}


}	// extern "C" end