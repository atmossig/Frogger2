/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: Controll.c
	Programmer	: Andrew Eder
	Date		: 11/9/98

----------------------------------------------------------------------------------------------- */

#include <math.h>
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <stdio.h>
#include <fstream.h>
#include <islutil.h>
//#include "..\resource.h"

#include "Main.h"
#include "frogger.h"
#include "language.h"
#include "controll.h"
#include "islpad.h"
#include "mdx.h"
#include "layout.h"

/*	------------------------------------------------------------------------
	Global stuff
*/

BYTE keyTable[256];

PadDataType padData;	// PSX-Combatibuuule


/*	------------------------------------------------------------------------
	Controll-specific stuff
*/


// Gamepad

long DEAD_ZONE = 500;

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

void StopKeying(void);

// TODO: put these in a structure somewhere SENSIBLE
char rKeyFile[MAX_PATH] = "";
unsigned long rKeying = 0;
unsigned long rKeyOK = 0;				
unsigned long rPlaying = 0;
unsigned long rPlayOK = 0;
long rEndFrame;
// -------------------------------------------------

int numJoypads = 0;

// 14 buttons per controller, 4 controllers.
// Need to store DIK, controller number and controller command
KEYENTRY keymap[56] = 
{
	{ 0, PAD_UP, DIK_UP },
	{ 0, PAD_DOWN, DIK_DOWN },
	{ 0, PAD_LEFT, DIK_LEFT },
	{ 0, PAD_RIGHT, DIK_RIGHT },
	{ 0, PAD_CROSS, DIK_RETURN },
	{ 0, PAD_SQUARE, DIK_RSHIFT },
	{ 0, PAD_START, DIK_ESCAPE },
	{ 0, PAD_CIRCLE, DIK_NUMPAD4 },
	{ 0, PAD_TRIANGLE, DIK_NUMPAD2 },
	{ 0, PAD_L2, DIK_NUMPAD8 },
	{ 0, PAD_R2, DIK_NUMPAD6 },
	{ 0, PAD_SELECT, DIK_NUMPAD0 },
	{ 0, PAD_L1, DIK_RCONTROL },
	{ 0, PAD_R1, DIK_R },

	{ 1, PAD_UP, DIK_W },
	{ 1, PAD_DOWN, DIK_S },
	{ 1, PAD_LEFT, DIK_A },
	{ 1, PAD_RIGHT, DIK_D },
	{ 1, PAD_CROSS, DIK_C },
	{ 1, PAD_SQUARE, DIK_E },
	{ 1, PAD_START, 0 },
	{ 1, PAD_CIRCLE, 0 },
	{ 1, PAD_TRIANGLE, 0 },
	{ 1, PAD_L2, 0 },
	{ 1, PAD_R2, 0 },
	{ 1, PAD_SELECT, 0 },
	{ 1, PAD_L1, 0 },
	{ 1, PAD_R1, 0 },

	{ 2, PAD_UP, DIK_I },
	{ 2, PAD_DOWN, DIK_K },
	{ 2, PAD_LEFT, DIK_J },
	{ 2, PAD_RIGHT, DIK_L },
	{ 2, PAD_CROSS, DIK_N },
	{ 2, PAD_SQUARE, DIK_U },
	{ 2, PAD_START, 0 },
	{ 2, PAD_CIRCLE, 0 },
	{ 2, PAD_TRIANGLE, 0 },
	{ 2, PAD_L2, 0 },
	{ 2, PAD_R2, 0 },
	{ 2, PAD_SELECT, 0 },
	{ 2, PAD_L1, 0 },
	{ 2, PAD_R1, 0 },

	{ 3, PAD_UP, 0 },
	{ 3, PAD_DOWN, 0 },
	{ 3, PAD_LEFT, 0 },
	{ 3, PAD_RIGHT, 0 },
	{ 3, PAD_CROSS, 0 },
	{ 3, PAD_SQUARE, 0 },
	{ 3, PAD_START, 0 },
	{ 3, PAD_CIRCLE, 0 },
	{ 3, PAD_TRIANGLE, 0 },
	{ 3, PAD_L2, 0 },
	{ 3, PAD_R2, 0 },
	{ 3, PAD_SELECT, 0 },
	{ 3, PAD_L1, 0 },
	{ 3, PAD_R1, 0 }
};

unsigned int joymap[MAXBUTTONS] = { PAD_CROSS, PAD_SQUARE, PAD_CIRCLE, PAD_TRIANGLE, PAD_START };

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
int GetButtonDialog(LPDIRECTINPUTDEVICE lpDI, HWND hParent);
int SetupKeyboardDialog(int player, HWND hParent);

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
		rEndFrame = actFrameCount + (15 * 60);

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

	hRes = DirectInputCreate(mdxWinInfo.hInstance,DIRECTINPUT_VERSION,&lpDI,NULL);
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

	hRes = lpKeyb->SetCooperativeLevel(mdxWinInfo.hWndMain,DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
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

	utilPrintf("Found device: %s\n", dev->tszInstanceName);

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
			utilPrintf("Error retrieving DirectInputDevice2 interface\n");
			return FALSE;
		}

		lpJoy1->Release();

		hRes = lpJoy->SetDataFormat(&c_dfDIJoystick);
		if(FAILED(hRes))
		{
			utilPrintf("Error setting data format\n");
			return FALSE;
		}

		hRes = lpJoy->SetCooperativeLevel(mdxWinInfo.hWndMain,DISCL_EXCLUSIVE | DISCL_FOREGROUND);
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


#ifdef FULL_BUILD
// TODO: remove this timer
TIMER idletimer;
#endif

/*	--------------------------------------------------------------------------------
	Function	: ProcessUserInput
	Purpose		: processes keyboard input / mouse
	Parameters	: HWND
	Returns		: void
	Info		: 
*/
void ProcessUserInput()
{
	HRESULT hRes;
	long i;
	int pressed = 0;
	
	unsigned short oldDigital[8];

	if (consoleDraw)
		return;

	if (windowActive)
	{
		// read keyboard data
		hRes = lpKeyb->GetDeviceState(sizeof(keyTable),&keyTable);
		if(FAILED(hRes))
			return;
	}

	//----- [ KEYBOARD CONTROL ] -----//

	if(KEYPRESS(DIK_F12))
		PostMessage(mdxWinInfo.hWndMain, WM_CLOSE, 0, 0);
	
	// reset states
	for (i=0; i<8; i++)
	{
		oldDigital[i] = padData.digital[i];
		padData.digital[i] = 0;
	}

	if (!keysEnabled) return;

/*
#ifdef _DEBUG
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
#endif
*/

	if (rPlayOK)
	{
		while ((curPlayKey < playKeyCount) && (playKeyList[curPlayKey*3]<actFrameCount))
		{
			//controllerdata[playKeyList[(curPlayKey*3)+2]].button = playKeyList[(curPlayKey*3)+1];
			padData.digital[playKeyList[(curPlayKey*3)+2]] = playKeyList[(curPlayKey*3)+1];
			curPlayKey++;
		}
	}
	else
	{
		for (i = 0; i<NUM_FROGS * 14; i++)
				if( keymap[i].key > 0 && KEYPRESS(keymap[i].key) )
				{
					//controllerdata[keymap[i].player].button |= keymap[i].button;
					padData.digital[keymap[i].player] |= keymap[i].button;
					pressed = 1;
				}

		for( i=0; i < NUM_FROGS; i++ )
		{
			
			//if ((rKeyOK) && (controllerdata[i].button != controllerdata[i].lastbutton))
			//	RecordButtons(controllerdata[i].button,i);
			
			if ((rKeyOK) && (padData.digital[i] != oldDigital[i]))
				RecordButtons(padData.digital[i],i);

			if (controllers[i] & GAMEPAD)
			{
				LPDIRECTINPUTDEVICE2 lpJoy = lpJoystick[controllers[i] & 0xFF];
				DIJOYSTATE joy;

				if (!lpJoy) break;

				lpJoy->Acquire();

				hRes = lpJoy->Poll();
				if (hRes == DI_OK || hRes == DI_NOEFFECT)
				{
					hRes = lpJoy->GetDeviceState(sizeof(joy), &joy);
					if (FAILED(hRes))
					{
						utilPrintf("GetDeviceState() failed\n");
						return;
					}

					unsigned long b = 0;

					for (int m=0; m < MAXBUTTONS; m++)
						if (joy.rgbButtons[m]) b |= joymap[m];
					
					if (joy.lX < -DEAD_ZONE)		b |= PAD_LEFT;
					else if (joy.lX > DEAD_ZONE)	b |= PAD_RIGHT;

					//if ((b & (CONT_LEFT|CONT_RIGHT)) && (b & (CONT_UP|CONT_DOWN)))
					//	b &= ~(CONT_LEFT|CONT_RIGHT|CONT_DOWN|CONT_UP);	// diagonals do nothing

					if (joy.lY < -DEAD_ZONE)		b |= PAD_UP;
					else if (joy.lY > DEAD_ZONE)	b |= PAD_DOWN;

					if (b)
					{
						padData.digital[i] |= b;
						pressed = 1;

						//if (rPlaying) StopKeying();
					}

					//lpJoystick->UnAcquire();
				}
			}

		}
	}

	// Get "debounce" states

	for (i = 0; i<8; i++)
		padData.debounce[i] = (~oldDigital[i]) & padData.digital[i];

#ifdef FULL_BUILD
	if (pressed)
	{
		GTInit(&idletimer, 75);
	}
	else
	{
		GTUpdate(&idletimer, -1);
		if (!idletimer.time && (player[0].worldNum != 8))
		{
			player[0].worldNum = 8;	player[0].levelNum = 0;
			gameState.mode = LEVELCOMPLETE_MODE;
			gameState.multi = SINGLEPLAYER;
			GTInit( &modeTimer, 1 );
			showEndLevelScreen = 0;
		}
	}
#endif
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


//BOOL SetupControllerDlg(HWND hdlg)
//{
//	int n = numJoypads + 1;
//	HWND combo;
//
//	controllerInfo = (CONTROLLERINFO*)malloc(sizeof(CONTROLLERINFO) * n);
//
//	strcpy(controllerInfo[0].name, "Keyboard");
//	controllerInfo[0].id = KEYBOARD;
//
//	for (int c = 0; c < numJoypads && lpJoystick[c]; c++)
//	{
//		DIDEVICEINSTANCE di;
//		di.dwSize = sizeof(DIDEVICEINSTANCE);
//
//		lpJoystick[c]->GetDeviceInfo(&di);
//
//		strcpy(controllerInfo[c+1].name, di.tszInstanceName);
//		controllerInfo[c+1].id = GAMEPAD + c;
//	}
//
//	for (int player = 0; player < 4; player++)
//	{
//		combo = GetDlgItem(hdlg, IDC_PLAYER1 + player);
//		for (int i = 0; i < n; i++)
//			SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)controllerInfo[i].name);
//
//		SendMessage(combo, CB_SETCURSEL, 0, 0);
//	}
//
//	return TRUE;
//}
//
//
///*	--------------------------------------------------------------------------------
//	Function	: ControllerDlgProc
//	Purpose		: Dialog procedure for controller setup
//	Parameters	: 
//	Returns		: 
//	Info		: 
//*/
//
//BOOL CALLBACK ControllerDlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	static int selectedPlayer = -1;
//
//	switch (msg)
//	{
//	case WM_INITDIALOG:
//		return SetupControllerDlg(hdlg);
//
//	case WM_COMMAND:
//		switch (LOWORD(wParam))
//		{
//		case IDOK:
//			// Set appropriate controller data
//			{
//				for (int player = 0; player < 4; player++)
//				{
//					HWND combo = GetDlgItem(hdlg, IDC_PLAYER1 + player);
//					int sel = SendMessage(combo, CB_GETCURSEL, 0, 0);
//					controllers[player] = controllerInfo[sel].id;
//				}
//			}
//
//			// fall through...
//		case IDCANCEL:
//			EndDialog(hdlg, wParam);
//			free(controllerInfo);
//			return TRUE;
//
//		case ID_SETUP:
//			if (selectedPlayer >= 0)
//				SetupKeyboardDialog(selectedPlayer, hdlg);
//			return TRUE;
//		}
//
//		// When selection changes, make sure no other combo boxes have this controller
//		// selected (unless KEYBOARD is selected)
//		case CBN_SELENDOK:
//			{
//				HWND combo;
//				int cb, controller, id;
//				
//				selectedPlayer = cb = LOWORD(wParam) - IDC_PLAYER1;
//				controller = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
//				id = controllerInfo[controller].id;
//
//				if (id == KEYBOARD)
//				{
//					EnableWindow(GetDlgItem(hdlg, ID_SETUP), 1);
//					return TRUE;
//				}
//				else
//					EnableWindow(GetDlgItem(hdlg, ID_SETUP), 0);
//
//				for (int player=0; player<4; player++)
//				{
//					if (player == cb) continue;
//					combo = GetDlgItem(hdlg, IDC_PLAYER1 + player);
//					int s = SendMessage(combo, CB_GETCURSEL, 0, 0);
//
//					if (id == controllerInfo[s].id)
//						SendMessage(combo, CB_SETCURSEL, 0, 0);
//				}
//
//				return TRUE;
//			}
//
//	}
//
//	return FALSE;
//}
//
///*	--------------------------------------------------------------------------------
//	Function	: SetupControllers
//	Purpose		: Set up control devices
//	Parameters	: 
//	Returns		: TRUE unless something goes horribly wrong
//	Info		: 
//*/
//BOOL SetupControllers(HWND hwnd)
//{
//	return DialogBox(winInfo.hInstance, MAKEINTRESOURCE(IDD_CONTROLS), hwnd,
//		(DLGPROC)ControllerDlgProc);
//}
//
///*	--------------------------------------------------------------------------------
//	Function		: SetupKeyboardDialog
//	Purpose			: Make a dialogue that maps command to keys
//	Parameters		: 
//	Returns			: 
//	Info			: 
//*/
//char listText1[] = "Command";
//char listText2[] = "Key";
//BOOL CALLBACK DLGKeyMapDialogue(HWND hDlg,UINT msg,WPARAM wParam,LPARAM lParam);
//
//
//int SetupKeyboardDialog(int player, HWND hParent)
//{
//	return DialogBoxParam(winInfo.hInstance, MAKEINTRESOURCE(IDD_KEYMAPBOX), hParent,
//		(DLGPROC)DLGKeyMapDialogue, (LPARAM)player);
//}
//
//BOOL CALLBACK DLGKeyMapDialogue(HWND hDlg,UINT msg,WPARAM wParam,LPARAM lParam)
//{
//	long i;
//	char itmTxt[64];
//	HWND list;
//	static DWORD keyIndex = 0;
//	static long kMapSet = 0;
//
//    switch(msg)
//	{
//		case WM_INITDIALOG:
//		{
//			RECT meR;
//			int player = (int)lParam;
//			int tabstop = 80;
//
//			keyIndex = 14 * player;
//			
//			wsprintf(itmTxt, "Keyboard setup: Player %d", (player+1));
//			SetWindowText(hDlg, itmTxt);
//
//			GetWindowRect(hDlg, &meR);
//			SetWindowPos(hDlg,HWND_TOPMOST,(GetSystemMetrics(SM_CXSCREEN)-(meR.right-meR.left))/2,(GetSystemMetrics(SM_CYSCREEN)-(meR.bottom-meR.top))/2, 0,0,SWP_NOSIZE);
//
//			list = GetDlgItem(hDlg,IDC_KEYMAPLIST);
//
//			SendMessage(list, LB_SETTABSTOPS, 1, (LPARAM)&tabstop);
//
//			for( i=0; i<14; i++ )
//			{
//				strcpy( itmTxt, controlDesc[i] );
//				strcat( itmTxt, "\t" );
//				strcat( itmTxt, DIKStrings[keymap[keyIndex+i].key] );
//				SendMessage( list,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)itmTxt );
//			}
//
// 			return TRUE;
//		}
//
//        case WM_CLOSE:
//			keyIndex = 0;
//			kMapSet = 0;
//			EndDialog(hDlg,TRUE);
//            return TRUE;
//
//		case WM_COMMAND:
//			switch (LOWORD(wParam))
//			{
//				case IDC_KEYMAPLIST:
//					switch(HIWORD(wParam))
//					{
//						case LBN_SELCHANGE:
//							// Get index of new selection
//							i = SendDlgItemMessage(hDlg,IDC_KEYMAPLIST,LB_GETCURSEL,(WPARAM)0,(LPARAM)0);
//							if(i == LB_ERR || i < 0 || i > 14 )
//								break;
//
//							if ((i = GetButtonDialog(lpKeyb, hDlg)) == -1)
//								break;
//
//							// Set DInput key in keymap
//							keymap[keyIndex+kMapSet].key = i;
//
//							// Reset and remake the key list
//							list = GetDlgItem(hDlg,IDC_KEYMAPLIST);
//							SendMessage( list,LB_RESETCONTENT,(WPARAM)0,(LPARAM)0 );
//
//							for( i=0; i<14; i++ )
//							{
//								strcpy( itmTxt, controlDesc[i] );
//								strcat( itmTxt, "\t" );
//								strcat( itmTxt, DIKStrings[keymap[keyIndex+i].key] );
//								SendMessage( list,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)itmTxt );
//							}
//
//							SendDlgItemMessage(hDlg,IDC_KEYMAPLIST,LB_SETCURSEL,(WPARAM)-1,(LPARAM)0);
//
//							kMapSet = 0;
//							break;
//					}
//					break;
//
//				case IDCANCEL:
//				{
//					ifstream in;
//					in.open( keyFileName, ios::nocreate, filebuf::sh_read );
//					if( in.is_open() )
//					{
//						for( i=0; i<56; i++ )
//							in >> keymap[i].key;
//
//						in.close();
//					}
//
//					keyIndex = 0;
//					kMapSet = 0;
//					EndDialog(hDlg,FALSE);
//					break;
//				}
//
//				case IDOK:
//				{
//					ofstream out;
//					out.open( keyFileName, ios::out, filebuf::sh_read );
//					if( out.is_open() )
//					{
//						for( i=0; i<56; i++ )
//							out << keymap[i].key << ' ';
//
//						out.close( );
//					}
//
//					keyIndex = 0;
//					kMapSet = 0;
//					EndDialog(hDlg,TRUE);
//					break;
//				}
//			}
//
//			return TRUE;
//	}
//
//	return FALSE;
//}
//
///*	--------------------------------------------------------------------------------
//	Function		: GetButtonDialog
//	Purpose			: brings up a little dialog prompting to press a button; wait for a keypress and return immediately
//	Parameters		: LPDIRECTINPUT, HWND parent
//	Returns			: scan code of key
//	Info			: 
//*/
//
//BOOL CALLBACK ButtonDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	static int *button;
//
//	switch (msg)
//	{
//		case WM_INITDIALOG:
//			button = (int*)lParam;
//            
//			SetTimer( hDlg, 0, 100, NULL );	// poll every 100ms
//			return TRUE;
//
//		case WM_TIMER:
//			if (lpKeyb->GetDeviceState(sizeof(keyTable),&keyTable) == DI_OK)
//			{
//				int i;
//
//				for(i=1; i<256; i++ )
//					if( KEYPRESS( i ) )
//						break;
//
//				if (i < 256)
//				{
//					*button = i;
//                    KillTimer(hDlg, 0);
//					EndDialog(hDlg, TRUE);
//				}
//			}
//			break;
//
//		case WM_COMMAND:
//			switch (LOWORD(wParam))
//			{
//			case IDCANCEL:
//				DestroyWindow(hDlg);
//				break;
//			}
//			break;
//
//		default:
//			return FALSE;
//	}
//
//	return TRUE;
//}
//
//int GetButtonDialog(LPDIRECTINPUTDEVICE lpDID, HWND hParent)
//{
//	int button = -1;
//
//	DialogBoxParam(winInfo.hInstance, MAKEINTRESOURCE(IDD_KEYPRESS), hParent, ButtonDialogProc, (LPARAM)&button);
//
//	return button;
//}