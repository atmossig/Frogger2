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
}	// extern "C" end

#include <fstream.h>

#include "..\resource.h"

/*	------------------------------------------------------------------------
	Global stuff
*/

OSContPad controllerdata[4];
BYTE keyTable[256];


/*	------------------------------------------------------------------------
	Controll-specific stuff
*/


// Gamepad

long DEAD_ZONE = 50;

char keyFileName[] = "frogkeys.map";

char *controlDesc[] = 
{
	"Up",
	"Down",
	"Left",
	"Right",
	"A",
	"B",
	"Start",
	"Camera Left",
	"Camera Down",
	"Camera Up",
	"Camera Right",
	"Trigger",
	"Left Shoulder",
	"Right Shoulder"
};

LPDIRECTINPUT			lpDI		= NULL;
LPDIRECTINPUTDEVICE		lpKeyb		= NULL;
LPDIRECTINPUTDEVICE2	lpJoystick[4] = { NULL, NULL, NULL, NULL };
LPDIRECTINPUTDEVICE		lpMouse		= NULL;

extern "C"
{
char rKeyFile[MAX_PATH] = "";
unsigned long rKeying = 0;
unsigned long rKeyOK = 0;
unsigned long rPlaying = 0;
unsigned long rPlayOK = 0;
}

int numJoypads = 0;

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

	{ 1, CONT_UP, DIK_W },
	{ 1, CONT_DOWN, DIK_S },
	{ 1, CONT_LEFT, DIK_A },
	{ 1, CONT_RIGHT, DIK_D },
	{ 1, CONT_A, DIK_C },
	{ 1, CONT_B, DIK_E },
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
	{ 2, CONT_A, DIK_N },
	{ 2, CONT_B, DIK_U },
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

unsigned int joymap[MAXBUTTONS] = { CONT_A, CONT_B, CONT_L, CONT_SHIFT, CONT_C, CONT_F };

// controller type for each player
#define KEYBOARD 0x200
#define GAMEPAD	 0x100		// controller # in low byte

struct CONTROLLERINFO
{
	char name[80];
	unsigned short id;
};

CONTROLLERINFO *controllerInfo;	// allocated and deallocated by controller dialog

// used by controller setup to decide which thing is what
unsigned short controllers[4] = { GAMEPAD, KEYBOARD, KEYBOARD, KEYBOARD };	

unsigned long	playerInputPause;
unsigned long	playerInputPause2;

BOOL keysEnabled = TRUE;

/*	------------------------------------------------------------------------
	Forward declarations
*/

BOOL InitKeyboardControl();
BOOL InitJoystickControl();
void DeInitKeyboardControl();
void DeInitJoystick();
BOOL SetupControllerDlg(HWND hdlg);
BOOL CALLBACK DLGKeyMapDialogue(HWND hDlg,UINT msg,WPARAM wParam,LPARAM lParam);

/*	------------------------------------------------------------------------ */

const char* DIErrorStr(HRESULT err)
{
	switch(err)
	{
	case DIERR_INPUTLOST:
		return "Access to the input device has been lost";
	case DIERR_NOTACQUIRED:
		return "The operation cannot be performed unless the device is acquired.";
	case DIERR_NOTINITIALIZED:
		return "This object has not been initialized.";
	}

	return "Unrecognised error code";
}


/*	--------------------------------------------------------------------------------
	Function	: InitInputDevices
	Purpose		: initialises input devices
	Parameters	: 
	Returns		: BOOL - TRUE on success, else FALSE
	Info		: 
*/

void RecordKeyInit(unsigned long worldNum, unsigned long levelNum)
{
	FILE *fp;
	
	if (rKeyOK)
		return;

	sprintf(rKeyFile,"%srecord-%lu-%lu.key",baseDirectory,worldNum,levelNum);
	fp = fopen(rKeyFile,"wb");
	if (fp)
	{
		rKeyOK = 1;
		fclose(fp);
	}
}

/*	--------------------------------------------------------------------------------
	Function	: InitInputDevices
	Purpose		: initialises input devices
	Parameters	: 
	Returns		: BOOL - TRUE on success, else FALSE
	Info		: 
*/
unsigned long playKeyCount;
unsigned long curPlayKey;
unsigned long *playKeyList;
void PlayKeyInit(unsigned long worldNum, unsigned long levelNum)
{
	FILE *fp;
	if (rPlayOK)
		return;

	sprintf(rKeyFile,"%srecord-%lu-%lu.key",baseDirectory,worldNum,levelNum);
	fp = fopen(rKeyFile,"rb");
	if (fp)
	{
		playKeyCount = 0;
		curPlayKey = 0;

		while (!feof(fp))
		{
			fread(&curPlayKey,4,1,fp);
			fread(&curPlayKey,4,1,fp);
			fread(&curPlayKey,4,1,fp);
			playKeyCount++;
		}
		
		fseek(fp,0,SEEK_SET);

		playKeyList = new unsigned long [playKeyCount*3];
		
		for (curPlayKey=0; curPlayKey<playKeyCount*3; curPlayKey++)
			fread(&(playKeyList[curPlayKey]),4,1,fp);			
		
		curPlayKey = 0;
		rPlayOK = 1;

		fclose(fp);
	}
}

/*	--------------------------------------------------------------------------------
	Function	: InitInputDevices
	Purpose		: initialises input devices
	Parameters	: 
	Returns		: BOOL - TRUE on success, else FALSE
	Info		: 
*/

void PlayKeyDone(void)
{
	playKeyCount = 0;
	curPlayKey = 0;
	delete playKeyList;
	rPlayOK = 0;
	rPlaying = 0;
}

/*	--------------------------------------------------------------------------------
	Function	: InitInputDevices
	Purpose		: initialises input devices
	Parameters	: 
	Returns		: BOOL - TRUE on success, else FALSE
	Info		: 
*/

void RecordButtons(unsigned long b0,unsigned long pnum)
{
	FILE *fp = fopen(rKeyFile,"a+b");
	if (fp)
	{
		fwrite(&actFrameCount,4,1,fp);
		fwrite(&b0,4,1,fp);
		fwrite(&pnum,4,1,fp);
		fflush(fp);
		fclose(fp);
	}
}

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
		DeInitJoystick();
	}

	// Read control map
/*
	ifstream in;

	in.open( keyFileName, ios::nocreate, filebuf::sh_read );
	if( in.is_open() )
	{
		for( int i=0; i<56; i++ )
			in >> keymap[i].key;

		in.close();
	}
*/	
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
	Function	: EnumJoypadProc
	Purpose		: Enumerates DI devices to find a gamepad (for now gets the first instance)
	Parameters	: 
	Returns		:
*/
BOOL CALLBACK EnumJoypadProc(LPCDIDEVICEINSTANCE dev, LPVOID foundDev)
{
	LPCDIDEVICEINSTANCE found;

	dprintf"Found device: %s\n", dev->tszInstanceName));

	found = ((LPCDIDEVICEINSTANCE)foundDev) + numJoypads;

	memcpy((void*)found, dev, sizeof(DIDEVICEINSTANCE));
	
	numJoypads++;

	return DIENUM_CONTINUE;
}
 

/*	--------------------------------------------------------------------------------
	Function	: InitJoystickControl();
	Purpose		: initialises DirectInput joystick/pad control
	Parameters	: none
	Returns		: TRUE on success - else FALSE
	Info		: 
*/

BOOL InitJoystickControl()
{
	HRESULT hRes;
	DIDEVICEINSTANCE dev[4];
	LPDIRECTINPUTDEVICE lpJoy1;
	LPDIRECTINPUTDEVICE2 lpJoy;
	int j;

	ZeroMemory(&dev, sizeof(dev));
	
	lpDI->EnumDevices(DIDEVTYPE_JOYSTICK, EnumJoypadProc, (void*)&dev, DIEDFL_ATTACHEDONLY);

	for (j = 0; j < numJoypads; j++)
	{
		if (!dev[j].dwSize) break;

		hRes = lpDI->CreateDevice(dev[j].guidInstance, &lpJoy1, NULL);
		if(FAILED(hRes))
			return FALSE;

		hRes = lpJoy1->QueryInterface( IID_IDirectInputDevice2,(LPVOID *)&lpJoy);
		if (FAILED(hRes))
		{
			dprintf"Error retrieving DirectInputDevice2 interface\n"));
			return FALSE;
		}

		lpJoy1->Release();

		hRes = lpJoy->SetDataFormat(&c_dfDIJoystick);
		if(FAILED(hRes))
		{
			dprintf"Error setting data format\n"));
			return FALSE;
		}

		hRes = lpJoy->SetCooperativeLevel(winInfo.hWndMain,DISCL_EXCLUSIVE | DISCL_FOREGROUND);
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
		hRes = lpJoy->SetProperty( DIPROP_RANGE, &diprg.diph );
		if ( FAILED(hRes) ) 
			return FALSE;

		diprg.diph.dwObj = DIJOFS_Y;    // set the y-axis range
		hRes = lpJoy->SetProperty( DIPROP_RANGE, &diprg.diph );
		if ( FAILED(hRes) ) 
			return FALSE;

		lpJoystick[j] = lpJoy;
	}

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
	Function	: DeInitJoystick
	Purpose		: Releases joystick(s)
	Parameters	: none
	Returns		: none
	Info		: 
*/
void DeInitJoystick()
{
	int j;
	
	for (j=0; j<MAXJOYPADS; j++)
		if(lpJoystick[j])
		{
			lpJoystick[j]->Unacquire();
			lpJoystick[j]->Release();
			lpJoystick[j] = NULL;
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

	if (KEYPRESS(DIK_X))
		ThrowFrogAtScreen(0);

	if (KEYPRESS(DIK_Z))
		ShowJalloc();


	if (rPlayOK)
	{
		while ((curPlayKey < playKeyCount) && (playKeyList[curPlayKey*3]<actFrameCount))
		{
			controllerdata[playKeyList[(curPlayKey*3)+2]].button = playKeyList[(curPlayKey*3)+1];
			curPlayKey++;
		}
	}
	else
	{
		for (i = 0; i<NUM_FROGS * 14; i++)
				if( keymap[i].key > 0 && KEYPRESS(keymap[i].key) )
					controllerdata[keymap[i].player].button |= keymap[i].button;

		for( i=0; i < NUM_FROGS; i++ )
		{
			
			if ((rKeyOK) && (controllerdata[i].button != controllerdata[i].lastbutton))
				RecordButtons(controllerdata[i].button,i);

			if (controllers[i] & GAMEPAD)
			{
				LPDIRECTINPUTDEVICE2 lpJoy = lpJoystick[controllers[i] & 0xFF];
				DIJOYSTATE joy;

				if (!lpJoy) break;

				lpJoy->Acquire();

				hRes = lpJoy->Poll();
				if (hRes == DD_OK || hRes == DI_NOEFFECT)
				{
					hRes = lpJoy->GetDeviceState(sizeof(joy), &joy);
					if (FAILED(hRes))
					{
						dprintf"GetDeviceState() failed\n"));
						return;
					}

					unsigned long b = 0;

					for (int m=0; m < MAXBUTTONS; m++)
						if (joy.rgbButtons[m]) b |= joymap[m];
					
					if (joy.lX < -DEAD_ZONE)
						b |= (b & CONT_SHIFT) ? CONT_C : CONT_LEFT;		// if shift rotate L else hop L
					else if (joy.lX > DEAD_ZONE)
						b |= (b & CONT_SHIFT) ? CONT_F : CONT_RIGHT;	// if shift rotate R else hop R

					//if ((b & (CONT_LEFT|CONT_RIGHT)) && (b & (CONT_UP|CONT_DOWN)))
					//	b &= ~(CONT_LEFT|CONT_RIGHT|CONT_DOWN|CONT_UP);	// diagonals do nothing

					if (joy.lY < -DEAD_ZONE)
						b |= (b & CONT_SHIFT) ? 0 : CONT_UP;
					else if (joy.lY > DEAD_ZONE)
						b |= (b & CONT_SHIFT) ? 0 : CONT_DOWN;

					controllerdata[i].button |= b;

					//lpJoystick->UnAcquire();
				}
			}
		}
	}
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


BOOL SetupControllerDlg(HWND hdlg)
{
	int n = numJoypads + 1;
	HWND combo;

	controllerInfo = (CONTROLLERINFO*)malloc(sizeof(CONTROLLERINFO) * n);

	strcpy(controllerInfo[0].name, "Keyboard");
	controllerInfo[0].id = KEYBOARD;

	for (int c = 0; c < numJoypads && lpJoystick[c]; c++)
	{
		DIDEVICEINSTANCE di;
		di.dwSize = sizeof(DIDEVICEINSTANCE);

		lpJoystick[c]->GetDeviceInfo(&di);

		strcpy(controllerInfo[c+1].name, di.tszInstanceName);
		controllerInfo[c+1].id = GAMEPAD + c;
	}

	for (int player = 0; player < 4; player++)
	{
		combo = GetDlgItem(hdlg, IDC_PLAYER1 + player);
		for (int i = 0; i < n; i++)
			SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)controllerInfo[i].name);

		SendMessage(combo, CB_SETCURSEL, 0, 0);
	}

	return TRUE;
}


/*	--------------------------------------------------------------------------------
	Function	: ControllerDlgProc
	Purpose		: Dialog procedure for controller setup
	Parameters	: 
	Returns		: 
	Info		: 
*/

BOOL CALLBACK ControllerDlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		return SetupControllerDlg(hdlg);

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			// Set appropriate controller data
			{
				for (int player = 0; player < 4; player++)
				{
					HWND combo = GetDlgItem(hdlg, IDC_PLAYER1 + player);
					int sel = SendMessage(combo, CB_GETCURSEL, 0, 0);
					controllers[player] = controllerInfo[sel].id;
				}
			}

			// fall through...
		case IDCANCEL:
			EndDialog(hdlg, wParam);
			free(controllerInfo);
			return TRUE;

		case ID_SETUP:
			// Run Jim's keymap dialog
			DialogBox(winInfo.hInstance, MAKEINTRESOURCE(IDD_KEYMAPBOX), hdlg,
				(DLGPROC)DLGKeyMapDialogue);
			return TRUE;
		}

		// When selection changes, make sure no other combo boxes have this controller
		// selected (unless KEYBOARD is selected)
		case CBN_SELENDOK:
			{
				HWND combo;
				int cb, controller, id;
				
				cb = LOWORD(wParam) - IDC_PLAYER1;
				controller = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
				id = controllerInfo[controller].id;

				if (id == KEYBOARD)
				{
					EnableWindow(GetDlgItem(hdlg, ID_SETUP), 1);
					return TRUE;
				}
				else
					EnableWindow(GetDlgItem(hdlg, ID_SETUP), 0);

				for (int player=0; player<4; player++)
				{
					if (player == cb) continue;
					combo = GetDlgItem(hdlg, IDC_PLAYER1 + player);
					int s = SendMessage(combo, CB_GETCURSEL, 0, 0);

					if (id == controllerInfo[s].id)
						SendMessage(combo, CB_SETCURSEL, 0, 0);
				}

				return TRUE;
			}

	}

	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function	: SetupControllers
	Purpose		: Set up control devices
	Parameters	: 
	Returns		: TRUE unless something goes horribly wrong
	Info		: 
*/
BOOL SetupControllers(HWND hwnd)
{
	return DialogBox(winInfo.hInstance, MAKEINTRESOURCE(IDD_CONTROLS), hwnd,
		(DLGPROC)ControllerDlgProc);
}

BOOL CALLBACK KeypressDlg(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hDlg, wParam);
			return TRUE;
		}
	}

	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function		: MakeKeyMap
	Purpose			: Make a dialogue that maps command to keys
	Parameters		: 
	Returns			: 
	Info			: 
*/
char listText1[] = "Command";
char listText2[] = "Key";

BOOL CALLBACK DLGKeyMapDialogue(HWND hDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{
	long i;
	HRESULT hRes;
	char itmTxt[64];
	MSG pMsg;
	HWND list;
	static DWORD keyIndex = 0;
	static long kMapSet = 0;

    switch(msg)
	{
		case WM_INITDIALOG:
		{
			RECT meR;
			
			GetWindowRect(hDlg, &meR);
			list = GetDlgItem(hDlg,IDC_KEYMAPLIST);

			for( i=0; i<14; i++ )
			{
				strcpy( itmTxt, controlDesc[i] );
				strcat( itmTxt, " -> " );
				strcat( itmTxt, DIKStrings[keymap[keyIndex+i].key] );
				SendMessage( list,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)itmTxt );
			}

			SetWindowPos(hDlg,HWND_TOPMOST,(GetSystemMetrics(SM_CXSCREEN)-(meR.right-meR.left))/2,(GetSystemMetrics(SM_CYSCREEN)-(meR.bottom-meR.top))/2, 0,0,SWP_NOSIZE);

 			return TRUE;
		}

        case WM_CLOSE:
			keyIndex = 0;
			kMapSet = 0;
			EndDialog(hDlg,TRUE);
            return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_KEYMAPLIST:
					switch(HIWORD(wParam))
					{
						case LBN_SELCHANGE:
							// Get index of new selection
							i = SendDlgItemMessage(hDlg,IDC_KEYMAPLIST,LB_GETCURSEL,(WPARAM)0,(LPARAM)0);
							if(i == LB_ERR || i < 0 || i > 14 )
								break;

							kMapSet = i;
							i=256;

							// Wait for a key to be pressed
							while( i==256 && IDirectInputDevice_GetDeviceState(lpKeyb, sizeof(keyTable),&keyTable) == DI_OK )
							{
								for( i=1; i<256; i++ )
								{
									if( KEYPRESS( i ) )
									{
										break;
									}
								}
							}

							// Dispense with windows message cack - we don't care
							while( PeekMessage(&pMsg, hDlg, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE) );
        
							// Set DInput key in keymap
							keymap[keyIndex+kMapSet].key = i;

							// Reset and remake the key list
							list = GetDlgItem(hDlg,IDC_KEYMAPLIST);
							SendMessage( list,LB_RESETCONTENT,(WPARAM)0,(LPARAM)0 );

							for( i=0; i<14; i++ )
							{
								strcpy( itmTxt, controlDesc[i] );
								strcat( itmTxt, " -> " );
								strcat( itmTxt, DIKStrings[keymap[keyIndex+i].key] );
								SendMessage( list,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)itmTxt );
							}

							SendDlgItemMessage(hDlg,IDC_KEYMAPLIST,LB_SETCURSEL,(WPARAM)-1,(LPARAM)0);

							kMapSet = 0;
							break;
					}
					break;

				case IDC_CONTROLLER1:
					// Set pointer to correct part of keymap and refresh display
					keyIndex = 0;
					kMapSet = 0;
					break;
				case IDC_CONTROLLER2:
					keyIndex = 14;
					kMapSet = 0;
					break;
				case IDC_CONTROLLER3:
					keyIndex = 28;
					kMapSet = 0;
					break;
				case IDC_CONTROLLER4:
					keyIndex = 42;
					kMapSet = 0;
					break;

				case IDCANCEL:
				{
					ifstream in;
					in.open( keyFileName, ios::nocreate, filebuf::sh_read );
					if( in.is_open() )
					{
						for( i=0; i<56; i++ )
							in >> keymap[i].key;

						in.close();
					}

					keyIndex = 0;
					kMapSet = 0;
					EndDialog(hDlg,FALSE);
					break;
				}

				case IDOK:
				{
					ofstream out;
					out.open( keyFileName, ios::out, filebuf::sh_read );
					if( out.is_open() )
					{
						for( i=0; i<56; i++ )
							out << keymap[i].key << ' ';

						out.close( );
					}

					keyIndex = 0;
					kMapSet = 0;
					EndDialog(hDlg,TRUE);
					break;
				}
			}

			return TRUE;
	}

	return FALSE;
}
