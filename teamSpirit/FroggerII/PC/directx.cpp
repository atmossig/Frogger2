/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: directx.cpp
	Programmer	: Matthew Cloy
	Date		: 

----------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <dsound.h>
#include <dplay.h>
#include <dplobby.h>
#include "stdlib.h"
#include "stdio.h"
#include "directx.h"
#include "..\resource.h"
#include <crtdbg.h>
#include "commctrl.h"
#include "network.h"
#include "math.h"

#include "mdxDDraw.h"
#include "mdxD3D.h"
#include "mdxInfo.h"

#include <windowsx.h>
#include <mmsystem.h>
#include <memory.h>
#include <winbase.h>
#include <fstream.h>

#define SCREEN_BITS		16

extern "C"
{
#include <ultra64.h>
#include "incs.h"
#include "software.h"
#include "mavis.h"

long SCREEN_WIDTH=640;	//320
long SCREEN_HEIGHT=480;	//240
long HALF_WIDTH = 320;
long HALF_HEIGHT = 240;
float RES_DIFF = 1;
float RES_DIFF2 = 2;

float fStart = 0.3;
float fEnd = 0.6;
LPDIRECTDRAWSURFACE *screenTextureList;
LPDIRECTDRAWSURFACE *screenTextureList2;
D3DTLVERTEX *screenVtxList;

long is565;
void GrabScreenTextures(LPDIRECTDRAWSURFACE from, LPDIRECTDRAWSURFACE *to,LPDIRECTDRAWSURFACE *to2);
void DrawScreenOverlays(void);

extern long numPixelsDrawn; 

#define FOGADJ(x) (1.0-((x-fStart)*fEnd))
#define FOGVAL(y) (((unsigned long)(255*y) << 24))

//#define TriangleArea(x1,y1,x2,y2,x3,y3) fabs((fabs(x3-x1)*fabs(y3-y2) - fabs(y3-y1)*fabs(x3-x2))*0.5)
extern long numFacesDrawn; 

float TriangleArea(float x1,float y1,float x2,float y2,float x3,float y3)
{
	float height;
	float ang;
	float lA,lB,lC;
	float lAs,lBs,lCs;
	float lAsr,lBsr,lCsr;
	float dp;

	VECTOR a,b,c;
	a.v[X] = (int)(x1-x3);
	a.v[Y] = (int)(y1-y3); 
	b.v[X] = (int)(x2-x3);
	b.v[Y] = (int)(y2-y3); 
	c.v[X] = (int)(x1-x2);
	c.v[Y] = (int)(y1-y2);
	a.v[Z] = b.v[Z] = c.v[Z] = 0;
	
	if (numFacesDrawn==130)
		numFacesDrawn=130;

	if (fabs(a.v[X]+a.v[Y])<1)
		return 0;
	if (fabs(b.v[X]+b.v[Y])<1)
		return 0;
	if (fabs(c.v[X]+c.v[Y])<1)
		return 0;

	lAs = MagnitudeSquared(&a);
	lBs = MagnitudeSquared(&b);
	lCs = MagnitudeSquared(&c);

	lAsr = sqrtf(lAs);
	lBsr = sqrtf(lBs);
	lCsr = sqrtf(lCs);

	lA = fabs(lAsr);
	lB = fabs(lBsr);
	lC = fabs(lCsr);

	if (lA<1)
		return 0;
	if (lB<1)
		return 0;
	if (lC<1)
		return 0;

	MakeUnit(&a);
	MakeUnit(&b);
	MakeUnit(&c);

	// A is longest side
	if ((lA>=lB) && (lA>=lC))
	{
		dp = DotProduct(&a,&c);
		
		if (dp>0.9999)
			dp = 0.9999;
		
		if (dp<-0.9999)
			dp = -0.9999;

		ang = acos(dp);
	
		if (ang>PI_OVER_2)
			ang=PI-ang;

		height = (lC * sin(ang)) * (lA/2);
	}

	// B is longest side
	if ((lB>=lA) && (lB>=lC))
	{
		dp = DotProduct(&b,&a);
		
		if (dp>0.9999)
			dp = 0.9999;
		
		if (dp<-0.9999)
			dp = -0.9999;

		ang = acos(dp);

		if (ang>PI_OVER_2)
			ang=PI-ang;

		height = (lA * sin(ang)) * (lB/2);

	}
	
	// C is longest side
	if ((lC>=lA) && (lC>=lB))
	{
		dp = DotProduct(&c,&a);
		
		if (dp>0.9999)
			dp = 0.9999;
		
		if (dp<-0.9999)
			dp = -0.9999;

		ang = acos(dp);

		if (ang>PI_OVER_2)
			ang=PI-ang;

		height = (lA * sin(ang)) * (lC/2);
	}
	
	return height;
}

HWND win;

//LPDIRECTDRAW			pDirectDraw;
//LPDIRECTDRAWSURFACE		primarySrf;
//LPDIRECTDRAWSURFACE		hiddenSrf;
//LPDIRECTDRAWSURFACE		srfZBuffer;
//LPDIRECT3D2				pDirect3D;
//LPDIRECT3DDEVICE2		pDirect3DDevice;
//LPDIRECT3DVIEWPORT2		pDirect3DViewport;
//LPDIRECTDRAW4			pDirectDraw4;
//LPDIRECTDRAWCLIPPER		pClipper;

extern long winMode;
extern long scaleMode;
long runHardware;

struct dxDevice
{
	GUID *guid;
	DDCAPS caps;
	char *desc;
	char *name;
	long idx;
};

dxDevice dxDeviceList[100];
unsigned long dxNumDevices = 0;
long selIdx = 0;
long a565Card = 0;

extern float clx1,cly1;


#define DEBUG_FILE "C:\\frogger2.log"

long isHardware = 1;

int dumpScreen = 0;
int prim = 0;

extern long numFacesDrawn;

GUID guID;

// TODO: Tidy tidy tidy!
char videoCardName[256];	// which card we want
int foundDesiredVideo = 0;

//static GUID     guID;

void ScreenShot ( DDSURFACEDESC ddsd );

/*  --------------------------------------------------------------------------------
    Function      : EnumDDDevices
	Purpose       :	-
	Parameters    : (GUID FAR* lpGUID, LPSTR lpDriverDesc, LPSTR lpDriverName, LPVOID lpContext))
	Returns       : static BOOL FAR PASCAL 
	Info          : -
*/

static BOOL FAR PASCAL EnumDDDevices(GUID FAR* lpGUID, LPSTR lpDriverDesc, LPSTR lpDriverName, LPVOID lpContext)
{
    LPDIRECTDRAW	lpDD;
    DDCAPS			ddCaps;
	LPDIRECTDRAW4	pDD4;
	DDDEVICEIDENTIFIER ddId;

	if (DirectDrawCreate(lpGUID, &lpDD, NULL)!=DD_OK)				// Couldn't create DirectDraw device..
		return DDENUMRET_OK;										// so continue on to the next one

	DDINIT(ddCaps);													// Init caps struct
	
 	HRESULT capsResult;
	capsResult = lpDD->GetCaps(&ddCaps, NULL);					// Get the caps for the device
 	if (capsResult!=DD_OK)										// Get the caps for the device 	if (lpDD->GetCaps(&ddCaps, NULL)!=DD_OK)						// Get the caps for the device
	{
		lpDD->Release();											// Oops couldn't get it, release...
		return DDENUMRET_OK;										// And continue looking.
	}
	
	lpDD->QueryInterface(IID_IDirectDraw4, (LPVOID *)&pDD4);
	pDD4->GetDeviceIdentifier(&ddId,0);

	/* Pick desired card if at all possible!

	if (videoCardName[0] && (strcmp(ddId.szDescription, videoCardName) == 0))
	{
		if (lpGUID)
			memcpy(lpContext, lpGUID, sizeof(GUID));
		else
			memset(lpContext, 0, sizeof(GUID));
		foundDesiredVideo = 1;
		lpDD->Release();
		return DDENUMRET_CANCEL;	// yay, we found it
	}*/

	dxDeviceList[dxNumDevices].desc = new char [strlen (ddId.szDescription)];
	dxDeviceList[dxNumDevices].name = new char [strlen (ddId.szDriver)];
	
	strcpy(dxDeviceList[dxNumDevices].desc, ddId.szDescription);
	strcpy(dxDeviceList[dxNumDevices].name, ddId.szDriver);

	dxDeviceList[dxNumDevices].caps = ddCaps;						// Implicit structure copy.

    if (lpGUID)													// This is NULL for the primary display device
	{
	    dxDeviceList[dxNumDevices].guid = new GUID;
		memcpy(dxDeviceList[dxNumDevices].guid, lpGUID, sizeof(GUID));		
	}
	else
		dxDeviceList[dxNumDevices].guid = NULL;

	dxNumDevices++;

	lpDD->Release();

    return DDENUMRET_OK;
}

/*  --------------------------------------------------------------------------------
    Function      : enumDXObjects 
	Purpose       :	-
	Parameters    : (void))
	Returns       : bool 
	Info          : -
*/

void EnumDXObjects (GUID *guid)
{
	DirectDrawEnumerate(EnumDDDevices, guid);
}

char col0txt[] = "Name";
char col1txt[] = "Description";
char col2txt[] = "Caps";
char hwText[] = "3D Hardware Accelerated";
char swText[] = "Software Renderer";
char swName[] = "ISL Software Engine";
char swDesc[] = "Always3D Software Renderer";


BOOL CALLBACK DSEnumProc( LPGUID lpGUID, LPSTR lpszDesc,
				LPSTR lpszDrvName, LPVOID lpContext )
    {
    HWND   hCombo = *(HWND *)lpContext;
    LPGUID lpTemp = NULL;

    if( lpGUID != NULL )
	{
//		lpTemp = LocalAlloc ( LMEM_FIXED | LMEM_ZEROINIT , sizeof ( GUID ) );
		if ( ( lpTemp = (GUID*)LocalAlloc ( LPTR , sizeof ( GUID ) ) ) == NULL )
			return( TRUE );

		memcpy( lpTemp, lpGUID, sizeof(GUID));
	}

    ComboBox_AddString( hCombo, lpszDesc );
    ComboBox_SetItemData( hCombo,
			ComboBox_FindString( hCombo, 0, lpszDesc ),
			lpTemp );
    return( TRUE );
    }

void DisplayReadme(HWND hwnd)
{
	HRESULT res;
	
	res = (HRESULT)ShellExecute(hwnd, "open", "Readme.html", NULL, baseDirectory, 0);

	if (!res)
	{
		MessageBox(hwnd, "Couldn't open README.html", "Frogger", MB_OK | MB_ICONEXCLAMATION);
	}
	else
	{
		ShowWindow(winInfo.hWndMain, SW_MINIMIZE);
	}
}

BOOL CALLBACK HardwareProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	long i;
	HWND list;

    static HWND   hCombo;
    static LPGUID lpGUID;
    LPGUID        lpTemp;

    switch(uMsg)
	{
		case WM_INITDIALOG:
		{
			RECT meR;
			LV_COLUMN clm;
			LV_ITEM itm;
				
			GetWindowRect(hwndDlg, &meR);
			
			clm.mask= LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
			clm.fmt = LVCFMT_LEFT;
			clm.cx = 400;
			clm.pszText = col2txt;
			clm.cchTextMax = 255; 
			clm.iSubItem = 0; 

			list = GetDlgItem(hwndDlg,IDC_LIST2);

			SendMessage (list,LVM_INSERTCOLUMN,0,(long)&clm);
			clm.mask= LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			clm.pszText = col1txt;
			clm.cx = 120;
			clm.iSubItem = 1; 
			SendMessage (list,LVM_INSERTCOLUMN,0,(long)&clm);
			
			clm.mask= LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			clm.pszText = col0txt;
			clm.cx = 200;
			clm.iSubItem = 2; 
			SendMessage (list,LVM_INSERTCOLUMN,0,(long)&clm);
			SendMessage (GetDlgItem(hwndDlg,IDC_EDIT),WM_SETTEXT,0,(long)baseDirectory);

			for (i=0; i<dxNumDevices; i++)
			{
				
				itm.mask = LVIF_TEXT ;
				itm.iItem = i; 
				itm.iSubItem = 0;
				itm.state = 0;
				itm.stateMask = 0; 
				
				itm.pszText = dxDeviceList[i].desc;

				itm.cchTextMax = 255; 
				itm.iImage = NULL; 
				itm.lParam = i; 
				
				dxDeviceList[i].idx = SendMessage (list,LVM_INSERTITEM,0,(long)&itm);

				itm.mask = LVIF_TEXT ;
				itm.iItem = i; 
				itm.state = 0;
				itm.stateMask = 0; 
				itm.cchTextMax = 255; 
				itm.iImage = NULL; 
				itm.lParam = i; 
				itm.iSubItem = 1;
				itm.pszText = dxDeviceList[i].name;

				SendMessage (list,LVM_SETITEM,0,(long)&itm);
				
				itm.mask = LVIF_TEXT ;
				itm.iItem = i; 
				itm.state = 0;
				itm.stateMask = 0; 
				itm.cchTextMax = 255; 
				itm.iImage = NULL; 
				itm.lParam = i; 
				itm.iSubItem = 2;

				itm.pszText = (dxDeviceList[i].caps.dwCaps & DDCAPS_3D)?hwText:swText;

				SendMessage (list,LVM_SETITEM,0,(long)&itm);
				
			}
			///////////////////////////////////////////////////////////////////////////////



				itm.mask = LVIF_TEXT ;
				itm.iItem = i; 
				itm.iSubItem = 0;
				itm.state = 0;
				itm.stateMask = 0; 
				
				itm.pszText = swDesc;

				itm.cchTextMax = 255; 
				itm.iImage = NULL; 
				itm.lParam = i; 
				
				dxDeviceList[i].idx = SendMessage (list,LVM_INSERTITEM,0,(long)&itm);

				itm.mask = LVIF_TEXT ;
				itm.iItem = i; 
				itm.state = 0;
				itm.stateMask = 0; 
				itm.cchTextMax = 255; 
				itm.iImage = NULL; 
				itm.lParam = i; 
				itm.iSubItem = 1;
				itm.pszText = swName;

				SendMessage (list,LVM_SETITEM,0,(long)&itm);
				
				itm.mask = LVIF_TEXT ;
				itm.iItem = i; 
				itm.state = 0;
				itm.stateMask = 0; 
				itm.cchTextMax = 255; 
				itm.iImage = NULL; 
				itm.lParam = i; 
				itm.iSubItem = 2;

				itm.pszText = swText;

				SendMessage (list,LVM_SETITEM,0,(long)&itm);
				


			///////////////////////////////////////////////////////////////////////////////

			ListView_SetItemState(list, 0, LVIS_SELECTED | LVIS_FOCUSED, 0x00FF);

			SetWindowPos(hwndDlg, HWND_TOPMOST, (GetSystemMetrics(SM_CXSCREEN)-(meR.right-meR.left))/2,(GetSystemMetrics(SM_CYSCREEN)-(meR.bottom-meR.top))/2, 0,0,SWP_NOSIZE);

			hCombo = GetDlgItem( hwndDlg, IDC_SOUNDCOMBO );
			lpGUID = (LPGUID)lParam;

			if( DirectSoundEnumerate( (LPDSENUMCALLBACK)DSEnumProc, &hCombo ) != DS_OK )
			{
				EndDialog( hwndDlg, TRUE );
				return( TRUE );
			}
			if( ComboBox_GetCount( hCombo ))
				ComboBox_SetCurSel( hCombo, 0 );
			else
			{
				EndDialog( hwndDlg, TRUE );
				return( TRUE );
			}

			hCombo = GetDlgItem( hwndDlg, IDC_COMBO1);

		    ComboBox_AddString( hCombo, "320 x 240");
		    ComboBox_AddString( hCombo, "640 x 480");
		    ComboBox_AddString( hCombo, "800 x 600");
		    ComboBox_AddString( hCombo, "1024 x 768");
		    ComboBox_AddString( hCombo, "1280 x 1024");
			ComboBox_SetCurSel( hCombo, 1 );

 			return TRUE;
		}
        case WM_CLOSE:
			PostQuitMessage(0);
			runQuit = 1;
            return TRUE;
			break;
		
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_README:
				DisplayReadme(hwndDlg);
				break;

			case IDCANCEL:
				PostQuitMessage(0);
				runQuit = 1;
				break;
			case IDC_MPLAYER:
				InitMPDirectPlay(winInfo.hInstance); // Pop up multiplayer select dialogue box
				break;

			case IDC_CONTROLS:
				SetupControllers(hwndDlg);
				break;
/*
			case IDC_KEYMAP:
				DialogBoxParam(winInfo.hInstance, MAKEINTRESOURCE(IDD_KEYMAPBOX),winInfo.hWndMain,(DLGPROC)DLGKeyMapDialogue, (LPARAM)&DPInfo );
				break;
*/
			case IDOK:
			{
				if (!winMode)
					ShowCursor(0);
				
				for (i=0; i<SendMessage (GetDlgItem(hwndDlg,IDC_LIST2),LVM_GETITEMCOUNT,0,0); i++)
					if (SendMessage (GetDlgItem(hwndDlg,IDC_LIST2),LVM_GETITEMSTATE,i,LVIS_SELECTED))
						selIdx = i;
				
				SendMessage (GetDlgItem(hwndDlg,IDC_EDIT),WM_GETTEXT,MAX_PATH,(long)baseDirectory);

				if (selIdx == LB_ERR)
				{
					PostQuitMessage(0);
					runQuit = 1;
					return TRUE;
				}

				// Get selected sound driver........


				for ( i = 0; i < ComboBox_GetCount ( hCombo ); i++ )
				{
					(GUID*) lpTemp = ( LPGUID ) ComboBox_GetItemData ( hCombo, i );
					if ( i == ComboBox_GetCurSel ( hCombo ) )
					{
						if ( lpTemp != NULL )
							memcpy ( lpGUID, lpTemp, sizeof ( GUID ) );
						else
							lpGUID = NULL;
					}
					if ( lpTemp )
						LocalFree ( lpTemp );
				}
				// ENDFOR

				// If we got the NULL GUID, then we want to open the default
				// sound driver, so return with an error and the init code
				// will know not to pass in the guID and will send NULL
				// instead.
				if ( lpGUID == NULL )
					EndDialog ( hwndDlg, TRUE );
				else
					EndDialog( hwndDlg, FALSE );

				switch(ComboBox_GetCurSel(GetDlgItem( hwndDlg, IDC_COMBO1)))
				{
					case 0:
						SCREEN_WIDTH = 320;
						SCREEN_HEIGHT = 240;
						break;
					case 1:
						SCREEN_WIDTH = 640;
						SCREEN_HEIGHT = 480;
						break;
					case 2:
						SCREEN_WIDTH = 800;
						SCREEN_HEIGHT = 600;
						break;
					case 3:
						SCREEN_WIDTH = 1024;
						SCREEN_HEIGHT = 768;
						break;
					case 4:
						SCREEN_WIDTH = 1280;
						SCREEN_HEIGHT = 1024;
						break;
				}
			HALF_WIDTH = SCREEN_WIDTH/2;
				HALF_HEIGHT = SCREEN_HEIGHT/2;
				clx1 = SCREEN_WIDTH-1;
				cly1 = SCREEN_HEIGHT-1;


//					EndDialog(hwndDlg,0);
				runQuit = 0;
				break;
			}

		}
		break;
	}
	return FALSE;
}


// It's DirectX, innit?
long DirectXInit(HWND window, long hardware )
{
	D3DVIEWPORT				viewport;
	D3DFINDDEVICERESULT		result;
	D3DFINDDEVICESEARCH		search;
	DDSURFACEDESC			ddsd;
	DDSURFACEDESC			sdesc;
	DDCAPS					ddCaps;
	HRESULT					res;
	int						l;

	win = window;

	GUID guID;
	GUID videoguid, *vguid;

	EnumDXObjects(&videoguid);
	if (!videoguid.Data1)
		vguid = NULL; // todo: do this a LOT better

	if (!foundDesiredVideo)
	{
		DialogBoxParam(winInfo.hInstance, MAKEINTRESOURCE(IDD_DIALOG1),window,(DLGPROC)HardwareProc, ( LPARAM ) &guID );
		
		if (dxDeviceList[selIdx].guid)
		{
			memcpy(&videoguid, dxDeviceList[selIdx].guid, sizeof(GUID));
			vguid = &videoguid;
		}
		else
			vguid = NULL;

		strcpy(videoCardName, dxDeviceList[selIdx].desc);
	}

	if (runQuit)
		return 0;

	// Initialise DirectDraw
	DDrawInitObject (vguid);
	
	// Setup our sufaces
	DDrawCreateSurfaces (window, SCREEN_WIDTH, SCREEN_HEIGHT, 16, TRUE, 16); 

	// Setup D3D
	D3DInit();
	D3DSetupRenderstates(D3DDefaultRenderstates);

	a565Card = r565;

	runHardware = hardware;

	//SetupRenderstates(); 
	RES_DIFF = SCREEN_WIDTH/640.0;	

	// Initialise DSound!
	InitDirectSound ( &guID, winInfo.hInstance, winInfo.hWndMain, prim );

	return TRUE;
}



/*
long DirectXInit(HWND window, long hardware )
{
	D3DVIEWPORT				viewport;
	D3DFINDDEVICERESULT		result;
	D3DFINDDEVICESEARCH		search;
	DDSURFACEDESC			ddsd;
	DDSURFACEDESC			sdesc;
	DDCAPS					ddCaps;
	HRESULT					res;
	int						l;

	win = window;

	GUID guID;

	EnumDXObjects();

	DialogBoxParam(winInfo.hInstance, MAKEINTRESOURCE(IDD_DIALOG1),window,(DLGPROC)HardwareProc, ( LPARAM ) &guID );

	if (runQuit)
		return 0;

	InitDirectSound ( &guID, winInfo.hInstance, winInfo.hWndMain, prim );

	isHardware = (dxDeviceList[selIdx].caps.dwCaps & DDCAPS_3D);
	hardware = isHardware;

	if ((res = DirectDrawCreate(dxDeviceList[selIdx].guid, &pDirectDraw, NULL)) != DD_OK)
	{
		dp("Failed creating DirectDraw object: %s\n", ddError2String(res));
		return FALSE;
	}

	if ((res = pDirectDraw->QueryInterface(IID_IDirectDraw4, (LPVOID *)&pDirectDraw4)) != S_OK)
	{
		dp("Failed getting DirectDraw4 interface: %s\n", ddError2String(res));
		return FALSE;
	}

	dp ("--------------------------------------------------------------------------------------------------------------------------------------------------\n");
	dp ("%s - %s \n",dxDeviceList[selIdx].name,dxDeviceList[selIdx].desc);
	dp ("--------------------------------------------------------------------------------------------------------------------------------------------------\n");
	memset (&ddCaps,0,sizeof(DDCAPS));
	ddCaps.dwSize = sizeof(DDCAPS);

	if ((res = pDirectDraw4->GetCaps(&ddCaps,NULL)) != S_OK)
	{
		dp("Failed getting DirectDraw4 caps: %s\n", ddError2String(res));
		return FALSE;
	}

	PrintCaps (&ddCaps);	
	dp ("--------------------------------------------------------------------------------------------------------------------------------------------------\n");

	if (winMode)
	{
		if ((res = pDirectDraw->SetCooperativeLevel(window, DDSCL_NORMAL)) != DD_OK)
		{
			dp("Failed setting cooperative level: %s\n", ddError2String(res));
			return FALSE;
		}
	}
	else
	{
		if ((res = pDirectDraw->SetCooperativeLevel(window, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWMODEX)) != DD_OK)
		{
			dp("Failed setting cooperative level: %s\n", ddError2String(res));
			return FALSE;
		}

		if ((res = pDirectDraw->SetDisplayMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BITS)) != DD_OK)
		{
			dp("Failed setting display mode: %s\n", ddError2String(res));
			return FALSE;
		}
	}

	// Get the primary display surface
	DDINIT(ddsd);
	ddsd.dwFlags = DDSD_CAPS;

	if (hardware)
	 ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_VIDEOMEMORY;
	else
	 ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_SYSTEMMEMORY;
	
	if (pDirectDraw->CreateSurface(&ddsd, &surface[PRIMARY_SRF], NULL) != DD_OK)
	 return FALSE;

	// Create a back buffer and attach it to the primary display surface to make a flippable surface
	DDINIT(ddsd);
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.dwWidth = SCREEN_WIDTH;
	ddsd.dwHeight = SCREEN_HEIGHT;

	if (hardware)
	 ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_3DDEVICE;// | DDSCAPS_VIDEOMEMORY;
	else
	 ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE | DDSCAPS_SYSTEMMEMORY;
	
	if ((res = pDirectDraw->CreateSurface(&ddsd, &surface[RENDER_SRF], NULL)) != DD_OK)
	{
		dp("Error creating backbuffer: %s\n", ddError2String(res));
		return FALSE;
	}

	if (!winMode)
		if ((res = surface[PRIMARY_SRF]->AddAttachedSurface(surface[RENDER_SRF])) != DD_OK)
		{
			dp("Error attaching backbuffer: %s\n", ddError2String(res));
			return FALSE;
		}

	RES_DIFF = SCREEN_WIDTH/640.0;
	RES_DIFF2 = 2*RES_DIFF;

	if (hardware)
	{
		// Create a z-buffer and attach it to the backbuffer
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_ZBUFFERBITDEPTH;
		ddsd.dwWidth = SCREEN_WIDTH;
		ddsd.dwHeight = SCREEN_HEIGHT;
		ddsd.dwZBufferBitDepth = 16;
		if (hardware)
			ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
		else
			ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_SYSTEMMEMORY;
		if ((res = pDirectDraw->CreateSurface(&ddsd, &surface[ZBUFFER_SRF], NULL)) != DD_OK)
		{
			dp("Error creating Z-buffer: %s\n", ddError2String(res));
			return FALSE;
		}
		if ((res = surface[RENDER_SRF]->AddAttachedSurface(surface[ZBUFFER_SRF])) != DD_OK)
		{
			dp("Error attaching Z-buffer: %s\n", ddError2String(res));
			return FALSE;
		}
	}

	if (pDirectDraw->QueryInterface(IID_IDirect3D2, (LPVOID *)&pDirect3D) != S_OK)
		return FALSE;

	// Find a device we can use

	DDINIT(search);
	DDINIT(result);

	search.dwFlags = D3DFDS_HARDWARE | D3DFDS_COLORMODEL;
	search.bHardware = hardware;
	search.dcmColorModel = D3DCOLOR_RGB;
	//search.dcmColorModel = D3DCOLOR_MONO;  

	if (pDirect3D->FindDevice(&search, &result) != D3D_OK) 
		return FALSE;

	// Create the D3D device
	if (pDirect3D->CreateDevice(result.guid, surface[RENDER_SRF], &pDirect3DDevice) != D3D_OK)
	 return FALSE;

	// Create a viewport
	DDINIT (viewport);

	viewport.dwWidth = SCREEN_WIDTH;
	viewport.dwHeight = SCREEN_HEIGHT;
	viewport.dvScaleX = (SCREEN_WIDTH / 2.0f);
	viewport.dvScaleY = (SCREEN_HEIGHT / 2.0f);
	viewport.dvMaxX = D3DVAL(1.0);
	viewport.dvMaxY = D3DVAL(1.0);
	viewport.dvMinZ = D3DVAL(0.0);
	viewport.dvMaxZ = D3DVAL(1.0);
	if (pDirect3D->CreateViewport(&pDirect3DViewport, NULL) != D3D_OK) 
		return FALSE;
	if (pDirect3DDevice->AddViewport(pDirect3DViewport) != D3D_OK) 
		return FALSE;
	if (pDirect3DViewport->SetViewport(&viewport) != D3D_OK) 
		return FALSE;
	if (pDirect3DDevice->SetCurrentViewport(pDirect3DViewport) != D3D_OK) 
		return FALSE;
	

	if (winMode)
	{
		if (pDirectDraw->CreateClipper (0,&pClipper,NULL))
			return FALSE;
	
		if (pClipper->SetHWnd (0,win)!=DD_OK) 
			return FALSE;
	
		if (surface[PRIMARY_SRF]->SetClipper (pClipper)!=DD_OK)
			return FALSE;
	}

	memset (&sdesc,0,sizeof (sdesc));
	sdesc.dwSize=sizeof (sdesc);

	surface[PRIMARY_SRF]->GetSurfaceDesc(&sdesc);
	l = (int)sdesc.ddpfPixelFormat.dwGBitMask;
	while ((l&1) == 0)
		  l >>= 1;
	
	a565Card = !(l == 31);

	runHardware = hardware;

	if (hardware)
		SetupRenderstates(); 
	
	return TRUE;
}
*/

float bRed = 0, bGreen = 0, bBlue = 0;

extern short *loadScr;
short backScr[1280*1024];
unsigned long sTime = (1000 * 5);
unsigned long screenGrabbed = 0;

void ShowLoadScreen(void)
{
	long i,j,sTicks,nTicks;
	long numRequired = (SCREEN_WIDTH/SCREENTEX_SIZE) * (SCREEN_HEIGHT/SCREENTEX_SIZE);
	DDSURFACEDESC ddsd;
	long startTicks;
	long curTicks;
	float sVal,fVal;

	dp("--ShowLoadScreen---------------------------------------\n");
	PrintTextureInfo();
	dp("-------------------------------------------------------\n");
	
	GrabScreenTextures(surface[PRIMARY_SRF], screenTextureList, screenTextureList2);
	
	startTicks = GetTickCount();
	do
	{
		curTicks = GetTickCount();
		DDINIT(ddsd);
		
		while (surface[RENDER_SRF]->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR,0)!=DD_OK);
		
		for (i=0,j=0; i<SCREEN_HEIGHT*(ddsd.lPitch/2); i+=(ddsd.lPitch/2),j+=SCREEN_WIDTH)
			memcpy (&((short *)ddsd.lpSurface)[i],&loadScr[j],SCREEN_WIDTH*2);
	
		
		surface[RENDER_SRF]->Unlock(NULL);

		for (int i=0; i<numRequired * 4; i++)
		{
			sVal = (curTicks-startTicks)/100.0;
			sVal = sinf(sVal+i)+1;
			sVal /=2;
			fVal = (1-(float)(curTicks-(startTicks+1000))/1000.0);
			
			if (fVal>1)
			{
				fVal -= 1;
				sVal = fVal+(sVal*(1-fVal));
				
			}
			else
				if (fVal>0)
					sVal *= fVal;
				else
					sVal = 0;
			
			screenVtxList[i].color = D3DRGBA(1,1,1,sVal);
		}

		DrawScreenOverlays();
		DDrawFlip();	
	
	} while (curTicks<startTicks+1999);

	
	FreeScreenTextures(screenTextureList,screenTextureList2,screenTexList);
	
	dp("--ShowLoadScreen---------------------------------------\n");
	PrintTextureInfo();
	dp("-------------------------------------------------------\n");
	
	while (surface[RENDER_SRF]->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR,0)!=DD_OK);
	for (i=0,j=0; i<SCREEN_HEIGHT*(ddsd.lPitch/2); i+=(ddsd.lPitch/2),j+=SCREEN_WIDTH)
		memcpy (&((short *)ddsd.lpSurface)[i],&loadScr[j],SCREEN_WIDTH*2);
	surface[RENDER_SRF]->Unlock(NULL);
	DDrawFlip();	

	GrabScreenTextures(surface[PRIMARY_SRF], screenTextureList, screenTextureList2);

	dp("--ShowLoadScreen---------------------------------------\n");
	PrintTextureInfo();
	dp("-------------------------------------------------------\n");
	
	screenGrabbed = 1;
}

void ShowDesignScreen(char *filename)
{
	short *image;
	char file[MAX_PATH];
	long i,j;
	DDSURFACEDESC ddsd;
	long startTicks;
	
	if (SCREEN_WIDTH!=640)
	{
		dprintf"Not correct Mode ------------------------------------------\n"));
		return;
	}

	sprintf (file,"%s%sDesign\\%s.bmp",baseDirectory,TEXTURE_BASE,filename);
	
	if ((image = GetGelfBmpDataAsShortPtr(file,0))==NULL)
	{
		dprintf"Not correct Image ---------------------%s\n",file));
		return;
	}
	
	startTicks = GetTickCount();
	DDINIT(ddsd);

	while (surface[RENDER_SRF]->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR,0)!=DD_OK);
		
	for (i=0,j=0; i<SCREEN_HEIGHT*(ddsd.lPitch/2); i+=(ddsd.lPitch/2),j+=SCREEN_WIDTH)
		memcpy (&((short *)ddsd.lpSurface)[i],&image[j],SCREEN_WIDTH*2);
	
		
	surface[RENDER_SRF]->Unlock(NULL);

	DDrawFlip();	
	while ((GetTickCount()<startTicks+5000))
	{
	    MSG msg;
	    while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
			if(msg.message == WM_KEYDOWN)
				return;		
	}	
}


void PrintTextureInfo(void)
{
	DDSCAPS2 ddCaps;
	unsigned long dwVidMemTotal, dwVidMemFree;

	ddCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE;
	pDirectDraw7->GetAvailableVidMem(&ddCaps, &dwVidMemTotal, &dwVidMemFree);					// Get the caps for the device
	//pDirectDraw->GetCaps(&ddCaps, NULL);					// Get the caps for the device
	dp ( "Total Mem : %du : - Total Free : %du :\n",dwVidMemTotal, dwVidMemFree );
}

#ifdef TEXTURE_DEBUG
int CheckTexHDL( unsigned long hdl )
{
	// Hack for fonts
	if( hdl && hdl <3 ) return TRUE;

	if( hdl && hdl < MAX_HDLCHECKS && hdlCheck[hdl] )
		return TRUE;

	dprintf"Invalid texture handle on visible polygon!\n"));

	return FALSE;
}
#endif
 
void DirectXFlip(void)
{
	D3DRECT rect;
	RECT r,a;
	DDBLTFX m;

	DDSURFACEDESC			ddsd;

// lock hiddenSrf and run screengrab

	DDINIT(ddsd);
	if ( dumpScreen )
	{
		while ( surface[RENDER_SRF]->Lock ( NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR, 0 ) != DD_OK );

		// run screen grab
		ScreenShot ( ddsd );


		surface[RENDER_SRF]->Unlock (NULL);
		dumpScreen = 0;
	}
	// ENDIF
	
	if (!runHardware)
	{
		long i,j;
		
		DDINIT(ddsd);
		while (surface[RENDER_SRF]->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR,0)!=DD_OK);
	
		for (i=0,j=0; i<SCREEN_HEIGHT*(ddsd.lPitch/2); i+=(ddsd.lPitch/2),j+=SCREEN_WIDTH)
			memcpy (&((short *)ddsd.lpSurface)[i],&screen[j],SCREEN_WIDTH*2);
	
		surface[RENDER_SRF]->Unlock(NULL);
	}

	// Flip the back buffer to the primary surface
	if (!winMode)
		while (surface[PRIMARY_SRF]->Blt(NULL,surface[RENDER_SRF],NULL,NULL,NULL)!=DD_OK);
		//surface[PRIMARY_SRF]->Flip(NULL,0);
	else
	{
		RECT clientR,windowR;

		GetClientRect(win,&clientR);
		GetWindowRect(win,&windowR);
		
		if (!scaleMode)
		{
			if (clientR.right>SCREEN_WIDTH)
				clientR.right = SCREEN_HEIGHT;

			if (clientR.bottom>SCREEN_WIDTH)
				clientR.bottom = SCREEN_HEIGHT;
		
			windowR.top+=GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYSIZEFRAME);
			windowR.left+=GetSystemMetrics(SM_CXSIZEFRAME);
			windowR.bottom = clientR.bottom+windowR.top;
			windowR.right = clientR.right+windowR.left;
			while (surface[PRIMARY_SRF]->Blt(&windowR,surface[RENDER_SRF],&clientR,NULL,NULL)!=DD_OK);
		
		}
		else
		{
			windowR.top+=GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYSIZEFRAME);
			windowR.left+=GetSystemMetrics(SM_CXSIZEFRAME);

			windowR.bottom-=GetSystemMetrics(SM_CYSIZEFRAME);
			windowR.right-=GetSystemMetrics(SM_CXSIZEFRAME);
			
			clientR.right = SCREEN_WIDTH;
			clientR.bottom = SCREEN_HEIGHT;
			while (surface[PRIMARY_SRF]->Blt(&windowR,surface[RENDER_SRF],&clientR,NULL,NULL)!=DD_OK);
		
		}

		
	}

	
	if (runHardware)
	{
		long r,g,b;
		DDINIT(m);
		if (a565Card)
		{
			r = (fog.r*0x1f)/256.0 ;
			g = (fog.g*0x3f)/256.0 ;
			b = (fog.b*0x1f)/256.0 ;

			
			m.dwFillColor = r<<11 | g<<5 | b;
		}
		else
		{
			r = (fog.r*0x1f)/256.0 ;
			g = (fog.g*0x1f)/256.0 ;
			b = (fog.b*0x1f)/256.0 ;

			
			m.dwFillColor = r<<10 | g<<5 | b;
		}
		//D3DRGB((bRed/(float)0xff),(bGreen/(float)0xff),(bBlue/(float)0xff));
		while (surface[RENDER_SRF]->Blt(NULL,NULL,NULL,DDBLT_WAIT | DDBLT_COLORFILL,&m)!=DD_OK);
	
		DDINIT(m);
		m.dwFillDepth = -1;//D3DRGB(0,0,0);
		while (surface[ZBUFFER_SRF]->Blt(NULL,NULL,NULL,DDBLT_WAIT | DDBLT_DEPTHFILL,&m)!=DD_OK);
	}

	numFacesDrawn = 0;
}

void SetupRenderstates(void)
{

	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,NULL);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE,FALSE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND,D3DTBLEND_MODULATEALPHA);
	
	
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE,FALSE);

	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE,TRUE);

//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_LINEAR);
//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMIN,D3DFILTER_LINEAR);

//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_SUBPIXEL,FALSE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE,D3DSHADE_GOURAUD);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_CW);
//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE,TRUE);
	
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,TRUE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,TRUE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC,D3DCMP_LESSEQUAL);

//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_FOGCOLOR, D3DRGBA((float)fog.r/256.0,(float)fog.g/256.0,(float)fog.b/256.0,0) );

	//Pixel Fog
//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_NONE);//
//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_FOGTABLESTART, *(DWORD *)&fStart );
//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_FOGTABLEEND, *(DWORD *)&fEnd );

	// Vertex Fog
 //   pDirect3DDevice->SetLightState(D3DLIGHTSTATE_FOGMODE, D3DFOG_LINEAR);
//    pDirect3DDevice->SetLightState(D3DLIGHTSTATE_FOGSTART, *(DWORD *)(&fStart));
   // pDirect3DDevice->SetLightState(D3DLIGHTSTATE_FOGEND,   *(DWORD *)(&fEnd));
}

// Split this out into two functions (CreateTextureSurface and CopyToSurface)
LPDIRECTDRAWSURFACE CreateTextureSurface(long xs,long ys, short *data, BOOL hardware, long cKey, long aiSurf)
{
	LPDIRECTDRAWSURFACE pSurface,pTSurface;

	pSurface = D3DCreateTexSurface(xs,ys,cKey, aiSurf,0);
	pTSurface = D3DCreateTexSurface(xs,ys,cKey, aiSurf,1);

	if (!pSurface || !pTSurface)
	{
		if (pSurface) pSurface->Release();
		if (pTSurface) pTSurface->Release();
		return NULL;
	}
	
	DDrawCopyToSurface(pSurface,(unsigned short *)data, aiSurf, xs, ys);

	pTSurface->Blt(NULL,pSurface,NULL,DDBLT_WAIT,0);

	RELEASE(pSurface);

	return pTSurface;
}

/*	HRESULT capsResult;
    DDCAPS			ddCaps;

//	DDINIT(ddCaps);													// Init caps struct
//	capsResult = pDirectDraw->GetCaps(&ddCaps, NULL);					// Get the caps for the device
//	dp ( "Before Total Mem : %d : - Total Free : %d :\n",ddCaps.dwVidMemTotal, ddCaps.dwVidMemFree );

	LPDIRECTDRAWSURFACE pSurface,pTSurface = NULL;
	DDSURFACEDESC ddsd;
	HRESULT me;
	hardware = isHardware;
	//Create the surface
	DDINIT(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | (aiSurf?DDSD_PIXELFORMAT:0);
	ddsd.dwWidth = xs;
	ddsd.dwHeight = ys;
	ddsd.ddpfPixelFormat.dwSize = sizeof (DDPIXELFORMAT);

	if (!aiSurf)
	{
		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
		ddsd.ddpfPixelFormat.dwRBitMask=5<<11;
	    ddsd.ddpfPixelFormat.dwGBitMask=6<<5;
	    ddsd.ddpfPixelFormat.dwBBitMask=5;
	}
	else
    {
		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
		ddsd.ddpfPixelFormat.dwRBitMask=0x0f00;
	    ddsd.ddpfPixelFormat.dwGBitMask=0x00f0;
	    ddsd.ddpfPixelFormat.dwBBitMask=0x000f;
	    ddsd.ddpfPixelFormat.dwRGBAlphaBitMask=0xf000;
	}
	
	ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_TEXTURE;
//	ddsd.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY;

	if ((me = pDirectDraw->CreateSurface(&ddsd, &pSurface, NULL)) != DD_OK)
	{
		dp ("Failed NSurf\n");
		dp (ddError2String(me));
		RELEASE(pSurface); 
		return NULL;
	}

	//Create the surface
	DDINIT(ddsd);
	
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | (aiSurf?DDSD_PIXELFORMAT:0);
	ddsd.dwWidth = xs;
	ddsd.dwHeight = ys;
	ddsd.ddpfPixelFormat.dwSize = sizeof (DDPIXELFORMAT);

/*	if (!aiSurf)
	{
		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
		ddsd.ddpfPixelFormat.dwRBitMask=5<<11;
	    ddsd.ddpfPixelFormat.dwGBitMask=6<<5;
	    ddsd.ddpfPixelFormat.dwBBitMask=5;
	}
	else*/
/*
	if (aiSurf)
    {
		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
		ddsd.ddpfPixelFormat.dwRBitMask=0x0f00;
	    ddsd.ddpfPixelFormat.dwGBitMask=0x00f0;
	    ddsd.ddpfPixelFormat.dwBBitMask=0x000f;
	    ddsd.ddpfPixelFormat.dwRGBAlphaBitMask=0xf000;
	}
	
	if (hardware)
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_VIDEOMEMORY;
	else
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY;
	
	if ((me = pDirectDraw->CreateSurface(&ddsd, &pTSurface, NULL)) != DD_OK)
	{
		DDINIT(ddCaps);													// Init caps struct
		capsResult = pDirectDraw->GetCaps(&ddCaps, NULL);					// Get the caps for the device
		dp ( "Total Mem : %d : - Total Free : %d :\n",ddCaps.dwVidMemTotal, ddCaps.dwVidMemFree );

//dwVidMemTotal;    DWORD    dwVidMemFree;

 		/*if (capsResult!=DD_OK)										// Get the caps for the device 	if (lpDD->GetCaps(&ddCaps, NULL)!=DD_OK)						// Get the caps for the device
		{
			ddError2String (capsResult);
			pDirectDraw->Release();											// Oops couldn't get it, release...
			return NULL;										// And continue looking.
		}*/
/*
		dp ("Failed TSurf\n");
		dp (ddError2String(me));

		RELEASE(pSurface); 
		return NULL;
	}

	if (!aiSurf)
	{
		DDCOLORKEY cK;
		cK.dwColorSpaceLowValue = cKey;
		cK.dwColorSpaceHighValue = cKey;
		pTSurface->SetColorKey (DDCKEY_SRCBLT,&cK);
	}
	
	// Copy the data into the surface

	DDINIT(ddsd);
	while (pSurface->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR,0)!=DD_OK);
	
	if (aiSurf)
	{
		for (int y=0;y<ys;y++)
			for (int x=0;x<xs;x++)
				((short *)ddsd.lpSurface)[x+y*(ddsd.lPitch/2)] = (data[x+y*xs] & (0x1f << 11)) | 0x0fff;
	}
	else
	{
		for (int y=0;y<ys;y++)
			for (int x=0;x<xs;x++)
				((short *)ddsd.lpSurface)[x+y*(ddsd.lPitch/2)] = data[x+y*xs];
	}

	pSurface->Unlock(ddsd.lpSurface);

	pTSurface->Blt(NULL,pSurface,NULL,DDBLT_WAIT,0);
	RELEASE(pSurface);
	//dp ("Tex = %x\n",pTSurface);
//	DDINIT(ddCaps);													// Init caps struct
//	capsResult = pDirectDraw->GetCaps(&ddCaps, NULL);					// Get the caps for the device
//	dp ( "After Total Mem : %d : - Total Free : %d :\n",ddCaps.dwVidMemTotal, ddCaps.dwVidMemFree );
	return pTSurface;
}
*/

void ReleaseSurface(LPDIRECTDRAWSURFACE surf)
{
	RELEASE(surf);
}

D3DTEXTUREHANDLE lastH = NULL;

////////////////////////////////////////////////////////////////////////////////////////////
//PolyBatchStuff

void DrawAHardwarePoly (D3DTLVERTEX *v,long vC, short *fce, long fC, D3DTEXTUREHANDLE h)
{
	PushPolys(v,vC,fce,fC,h);
}


////////////////////////////////////////////////////////////////////////////////////////////

void BeginDrawHardware (void)
{
	pDirect3DDevice->BeginScene();
	BlankFrame(meatballs);
	
}

void EndDrawHardware (void)
{
	pDirect3DDevice->EndScene();
}

extern long drawTimers;
/*void DrawAHardwarePoly (D3DTLVERTEX *v,long vC, short *fce, long fC, D3DTEXTUREHANDLE h)
{
	int i, j, v1, v2, v3, e;
	
	if (h!=lastH)
	{
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,h);
		lastH = h;
	}

	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,3);

	//pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,1);
	//pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,1);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC,D3DCMP_LESS);
	
	if (drawTimers>1)
		for (i=0; i<fC; i+=3)
		{
			float t = TriangleArea( v[fce[i+0]].sx,v[fce[i+0]].sy,
									v[fce[i+1]].sx,v[fce[i+1]].sy,
									v[fce[i+2]].sx,v[fce[i+2]].sy);
			numPixelsDrawn += t;
		}
//	return;

	if (pDirect3DDevice->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST,
		D3DVT_TLVERTEX,
		v,
		vC,
		(unsigned short *)fce,
		fC,
			D3DDP_DONOTCLIP |
			D3DDP_DONOTLIGHT |
			D3DDP_DONOTUPDATEEXTENTS) !=D3D_OK)
	{
		dp("BUGGER !!!!! CAN'T DRAW POLY JOBBY\n");
	}
}*/

void DrawALine (float x1, float y1, float x2, float y2, D3DCOLOR color)
{
	D3DTLVERTEX v[2] = {
		{
			x1,y1,0,0,
			color, D3DRGBA(0,0,0,1.0),
			0.0, 0.0
		},
		{
			x2,y2,0,0,
			color, D3DRGBA(0,0,0,1.0),
			0.0, 0.0
		}
	};

	if (pDirect3DDevice->DrawPrimitive(
		D3DPT_LINESTRIP,
		D3DVT_TLVERTEX,
		v,
		2,
		D3DDP_DONOTCLIP 
			| D3DDP_DONOTLIGHT 
			| D3DDP_DONOTUPDATEEXTENTS 
			/*| D3DDP_WAIT*/)!=D3D_OK)
	{
		dp("COULDN'T DRAW LINE");
	}
}

#define SPRITECLIPLEFT		0
#define SPRITECLIPTOP		0
#define SPRITECLIPRIGHT		(SCREEN_WIDTH)
#define SPRITECLIPBOTTOM	(SCREEN_HEIGHT)

void DrawASprite (float x, float y, float xs, float ys, float u1, float v1, float u2, float v2, D3DTEXTUREHANDLE h,DWORD colour)
{
	float x2 = (x+xs), y2 = (y+ys);

	if (x < SPRITECLIPLEFT)
	{
		if (x2 < SPRITECLIPLEFT) return;
		u1 += (u2-u1) * (SPRITECLIPLEFT-x)/xs;	// clip u
		xs += x-SPRITECLIPLEFT; x = SPRITECLIPLEFT;
	}
	if (x2 > SPRITECLIPRIGHT)
	{
		if (x > SPRITECLIPRIGHT) return;
		u2 += (u2-u1) * (SPRITECLIPRIGHT-x2)/xs;	// clip u
		xs -= (x-SPRITECLIPRIGHT);
		x2 = SPRITECLIPRIGHT;
	}

	if (y < SPRITECLIPTOP)
	{
		if (y2 < SPRITECLIPTOP) return;
		v1 += (v2-v1) * (SPRITECLIPTOP-y)/ys;	// clip v
		ys += y-SPRITECLIPTOP; y = SPRITECLIPTOP;
	}
	if (y2 > SPRITECLIPBOTTOM)
	{
		if (y > SPRITECLIPBOTTOM) return;
		v2 += (v2-v1) * (SPRITECLIPBOTTOM-y2)/ys;	// clip v
		ys -= (y-SPRITECLIPBOTTOM);
		y2 = SPRITECLIPBOTTOM;
	}
	
	D3DTLVERTEX v[4] = {
		{
			x,y,0,0,
			colour,D3DRGBA(0,0,0,1),
			u1,v1
		},
		{
			x2,y,0,0,
			colour,D3DRGBA(0,0,0,1),
			u2,v1
		},
		{
			x2,y2,0,0,
			colour,D3DRGBA(0,0,0,1),
			u2,v2
		},
		{
			x,y2,0,0,
			colour,D3DRGBA(0,0,0,1),
			u1,v2
	}};

	if (h!=lastH)
	{
#ifdef TEXTURE_DEBUG
		if( CheckTexHDL(h) )
#endif
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,h);
		lastH = h;
	}

	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,0);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,0);


	if (pDirect3DDevice->DrawPrimitive(
		D3DPT_TRIANGLEFAN,
		D3DVT_TLVERTEX,
		v,
		4,
		D3DDP_DONOTCLIP 
			| D3DDP_DONOTLIGHT 
			| D3DDP_DONOTUPDATEEXTENTS 
			/*| D3DDP_WAIT*/)!=D3D_OK)
	{
		dp("Could not print sprite\n");
		// BUGGER !!!!! CAN'T DRAW POLY JOBBY !
	}
}

void DrawAlphaSprite (float x, float y, float z, float xs, float ys, float u1, float v1, float u2, float v2, D3DTEXTUREHANDLE h, DWORD colour )
{
	float x2 = (x+xs), y2 = (y+ys);
	float fogAmt;

	if (x < SPRITECLIPLEFT)
	{
		if (x2 < SPRITECLIPLEFT) return;
		u1 += (u2-u1) * (SPRITECLIPLEFT-x)/xs;	// clip u
		xs += x-SPRITECLIPLEFT; x = SPRITECLIPLEFT;
	}
	if (x2 > SPRITECLIPRIGHT)
	{
		if (x > SPRITECLIPRIGHT) return;
		u2 += (u2-u1) * (SPRITECLIPRIGHT-x2)/xs;	// clip u
		xs -= (x-SPRITECLIPRIGHT);
		x2 = SPRITECLIPRIGHT;
	}

	if (y < SPRITECLIPTOP)
	{
		if (y2 < SPRITECLIPTOP) return;
		v1 += (v2-v1) * (SPRITECLIPTOP-y)/ys;	// clip v
		ys += y-SPRITECLIPTOP; y = SPRITECLIPTOP;
	}
	if (y2 > SPRITECLIPBOTTOM)
	{
		if (y > SPRITECLIPBOTTOM) return;
		v2 += (v2-v1) * (SPRITECLIPBOTTOM-y2)/ys;	// clip v
		ys -= (y-SPRITECLIPBOTTOM);
		y2 = SPRITECLIPBOTTOM;
	}
	
	fogAmt = FOGADJ(z);
	if (fogAmt<0)
		fogAmt=0;
	if (fogAmt>1)
		fogAmt=1;
	
	D3DTLVERTEX v[4] = {
		{
			x,y,z,0,
			/*D3DRGBA(1,1,1,alpha)*/colour,FOGVAL(fogAmt),
			u1,v1
		},
		{
			x2,y,z,0,
			colour,FOGVAL(fogAmt),
			u2,v1
		},
		{
			x2,y2,z,0,
			colour,FOGVAL(fogAmt),
			u2,v2
		},
		{
			x,y2,z,0,
			colour,FOGVAL(fogAmt),
			u1,v2
	}};

	if (h!=lastH)
	{
#ifdef TEXTURE_DEBUG
		if( CheckTexHDL(h) )
#endif
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,h);
		lastH = h;
	}

	if ((z>0.01) || (z<-0.01))
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,1);
	else
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,0);

	if (pDirect3DDevice->DrawPrimitive(
		D3DPT_TRIANGLEFAN,
		D3DVT_TLVERTEX,
		v,
		4,
		D3DDP_DONOTCLIP 
			| D3DDP_DONOTLIGHT 
			| D3DDP_DONOTUPDATEEXTENTS 
			/*| D3DDP_WAIT*/)!=D3D_OK)
	{
		dp("Could not print sprite\n");
		// BUGGER !!!!! CAN'T DRAW POLY JOBBY !
	}
}
/*	--------------------------------------------------------------------------------
	Function		: DrawFlatRect
	Purpose			: draw a flat rectangle
	Parameters		: 
	Returns			: 
	Info			: 
*/

void DrawFlatRect(RECT r, D3DCOLOR colour)
{
	D3DLVERTEX v[4] = {
		{
			r.left,r.top,0,0,
			colour,D3DRGBA(0,0,0,1),
			0,0
		},
		{
			r.left,r.bottom,0,0,
			colour,D3DRGBA(0,0,0,1),
			0,0
		},
		{
			r.right,r.bottom,0,0,
			colour,D3DRGBA(0,0,0,1),
			0,0
		},
		{
			r.right,r.top,0,0,
			colour,D3DRGBA(0,0,0,1),
			0,0
	}};

	if (lastH) {
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,NULL);
		lastH = NULL;
	}

	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,0);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,0);

	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_BOTHSRCALPHA);
//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_BOTHSRCALPHA);

	if (pDirect3DDevice->DrawPrimitive(
		D3DPT_TRIANGLEFAN,
		D3DVT_TLVERTEX,
		v,
		4,
		D3DDP_DONOTCLIP 
			| D3DDP_DONOTLIGHT 
			| D3DDP_DONOTUPDATEEXTENTS 
			| D3DDP_WAIT)!=D3D_OK)
	{
		dp("Could not draw flat rectangle\n");
		// ARSE! Can't draw flat rect
	}

	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
}

D3DTEXTUREHANDLE ConvertSurfaceToTexture(LPDIRECTDRAWSURFACE srf)
{
	D3DTEXTUREHANDLE textureHandle;
	LPDIRECT3DTEXTURE2 texture;

	if (srf->QueryInterface(IID_IDirect3DTexture2, (LPVOID *)&texture) != S_OK)
		return NULL;

	texture->GetHandle(pDirect3DDevice, &textureHandle);
	texture->Release();
	return textureHandle;
}

}

void ScreenShot ( DDSURFACEDESC ddsd )
{
	static int picnum = 0;
	char fileName[16];
	FILE *fp;
	int x, y, linePos;
	unsigned short pixel;
	unsigned char col;
	unsigned char line [ 1280 * 4 ];

	sprintf ( fileName, "c:\\pc-%04d.raw", picnum++);

	fp = fopen ( fileName, "w" );
	if ( fp == NULL )
		return;

	y = 0;
	for ( y = 0; y < SCREEN_HEIGHT; y++ )
	{
		linePos = 0;
		for(x = 0; x < SCREEN_WIDTH; x++)
		{
			pixel = ((short*)ddsd.lpSurface)[x + y * (ddsd.lPitch/2)];

			col = (int)(255.0/31.0 * ((pixel >> 11) & 0x1f));
			line[linePos++] = col;
		
			col = (int)(255.0/63.0 * ((pixel >> 5) & 0x3f));
			line[linePos++] = col;

			col = (int)(255.0/31.0 * (pixel & 0x1f));
			line[linePos++] = col;
		}
		fwrite ( line, SCREEN_WIDTH*3, 1, fp );			
	}


	fclose ( fp );
}


/*	--------------------------------------------------------------------------------
	Function 	: PTSurfaceBlit
	Purpose 	: Copy data into texture surface from procedural texture
	Parameters 	: Target texture, source data
	Returns 	: 
	Info 		:
*/
void PTSurfaceBlit( LPDIRECTDRAWSURFACE to, unsigned char *buf, unsigned short *pal )
{
	DDSURFACEDESC ddsd;
	HRESULT res;
	long i;
	DDINIT(ddsd);

	static LPDIRECTDRAWSURFACE pSurface = D3DCreateTexSurface(32,32,0xf81f, 0,0);
	
	while( (res = pSurface->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR | DDLOCK_WRITEONLY,0)) != DD_OK )
		ddShowError(res);

	i=928;

	while( i-- ) ((unsigned short *)ddsd.lpSurface)[i] = (unsigned short)pal[(unsigned char)buf[i]];

	pSurface->Unlock(NULL);

	if ((res = to->BltFast(0,0,pSurface,NULL,0))!=DD_OK)
		ddShowError(res);
}

/*	--------------------------------------------------------------------------------
	Function 	: InitScreenTextureList
	Purpose 	: Initialise for screen textures
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

LPDIRECTDRAWSURFACE *InitScreenTextureList(void)
{
	long numRequired = (SCREEN_WIDTH/SCREENTEX_SIZE) * (SCREEN_HEIGHT/SCREENTEX_SIZE);
	LPDIRECTDRAWSURFACE *me = (LPDIRECTDRAWSURFACE *)JallocAlloc(numRequired * sizeof(LPDIRECTDRAWSURFACE),0,"ScrSrf");
	for (int i=0; i<numRequired; i++)
		me[i] = NULL;
	return me;
}

/*	--------------------------------------------------------------------------------
	Function 	: InitScreenTextureList
	Purpose 	: Initialise for screen textures
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

D3DTLVERTEX *InitScreenVertexList(void)
{
	long numRequired = (SCREEN_WIDTH/SCREENTEX_SIZE) * (SCREEN_HEIGHT/SCREENTEX_SIZE);
	RECT rect;
	D3DTLVERTEX *me = (D3DTLVERTEX *)JallocAlloc(numRequired * sizeof(D3DTLVERTEX) * 4,0,"ScrSrf");
	for (int i=0; i<numRequired * 4; i+=4)
	{
		me[i].tu = 0;
		me[i].tv = 0;
		
		me[i+1].tu = 1;
		me[i+1].tv = 0;

		me[i+2].tu = 1;
		me[i+2].tv = 1;

		me[i+3].tu = 0;
		me[i+3].tv = 1;
		
		me[i].color = D3DRGBA(1,1,1,1);
		me[i+1].color = D3DRGBA(1,1,1,1);
		me[i+2].color = D3DRGBA(1,1,1,1);
		me[i+3].color = D3DRGBA(1,1,1,1);

		me[i].specular = D3DRGB(0,0,0);
		me[i+1].specular = D3DRGB(0,0,0);
		me[i+2].specular = D3DRGB(0,0,0);
		me[i+3].specular = D3DRGB(0,0,0);
		
	}

	i = 0;

	for (rect.top = 0, rect.bottom = SCREENTEX_SIZE; rect.top < SCREEN_HEIGHT; rect.top += SCREENTEX_SIZE, rect.bottom += SCREENTEX_SIZE)
		for (rect.left = 0, rect.right = SCREENTEX_SIZE; rect.left < SCREEN_WIDTH; rect.left += SCREENTEX_SIZE, rect.right += SCREENTEX_SIZE)
		{
			me[i+0].sx = rect.left;
			me[i+0].sy = rect.top;
			me[i+1].sx = rect.right;
			me[i+1].sy = rect.top;
			me[i+2].sx = rect.right;
			me[i+2].sy = rect.bottom;
			me[i+3].sx = rect.left;
			me[i+3].sy = rect.bottom;

			i+=4;
		}

	return me;
}

unsigned long screenTexList[1000];

void DrawScreenOverlays(void)
{
	unsigned short faces[] = {0,1,2,0,2,3};
	long numRequired = (SCREEN_WIDTH/SCREENTEX_SIZE) * (SCREEN_HEIGHT/SCREENTEX_SIZE);
	BeginDrawHardware();

	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,FALSE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,FALSE);

	for (int i=0; i<numRequired; i++)
	{
#ifdef TEXTURE_DEBUG
		if( CheckTexHDL(screenTexList[i]) )
#endif
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,screenTexList[i]);

		if (pDirect3DDevice->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			D3DVT_TLVERTEX,
			&screenVtxList[i*4],
			4,
			faces,
			6,
			D3DDP_DONOTCLIP |
			D3DDP_DONOTLIGHT |
			D3DDP_DONOTUPDATEEXTENTS) !=D3D_OK) dp("!");
	}

	EndDrawHardware();

}

/*	--------------------------------------------------------------------------------
	Function 	: FreeScreenTextures
	Purpose 	: Ready for next use
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FreeScreenTextures(LPDIRECTDRAWSURFACE *where,LPDIRECTDRAWSURFACE *where2, unsigned long *tex)
{
	long numRequired = (SCREEN_WIDTH/SCREENTEX_SIZE) * (SCREEN_HEIGHT/SCREENTEX_SIZE);
	for (int i=0; i<numRequired; i++)
	{
		if (where[i])
			where[i]->Release();		
		if (where2[i])
			where2[i]->Release();		
		
		where[i] = NULL;
		where2[i] = NULL;
		tex[i] = NULL;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: GrabScreenTextures
	Purpose 	: Grab the screen from a surface into a series of square textures
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void GrabScreenTextures(LPDIRECTDRAWSURFACE from, LPDIRECTDRAWSURFACE *to,LPDIRECTDRAWSURFACE *to2)
{
	int i, j,y,sY,dY;
	LPDIRECTDRAWSURFACE surface, *dds, surface2, *dds2;
	DDSURFACEDESC ddsd,ddsd2;
	
	unsigned long *hdl;
	HRESULT res;
	RECT rect;

	dds = &to[0];
	dds2 = &to2[0];
	hdl = screenTexList;
	
	for (rect.top = 0, rect.bottom = SCREENTEX_SIZE; rect.top < SCREEN_HEIGHT; rect.top += SCREENTEX_SIZE, rect.bottom += SCREENTEX_SIZE)
	{
		for (rect.left = 0, rect.right = SCREENTEX_SIZE; rect.left < SCREEN_WIDTH; rect.left += SCREENTEX_SIZE, rect.right += SCREENTEX_SIZE)
		{
			if (!*dds2)
				*dds2 = surface =  D3DCreateTexSurface2(SCREENTEX_SIZE,SCREENTEX_SIZE,1,0);
			else
				surface = *dds2;

			if (!*dds)
				*dds = surface2 = D3DCreateTexSurface2(SCREENTEX_SIZE,SCREENTEX_SIZE,1,1);
			else
				surface2 = *dds;

			//res = surface->Blt(NULL, from, &rect, DDBLT_WAIT,NULL);
			res = surface->BltFast(0,0,from,&rect,DDBLTFAST_WAIT);
			
		/*	DDINIT(ddsd);
			DDINIT(ddsd2);
			
			while (surface->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR,0)!=DD_OK);
			while (surface2->Lock(NULL,&ddsd2,DDLOCK_SURFACEMEMORYPTR,0)!=DD_OK);

			surface->Unlock(NULL);
			surface2->Unlock(NULL);
			
			memcpy(ddsd2.lpSurface,ddsd.lpSurface,32*32*sizeof(short));
*/
			res = surface2->BltFast(0,0,surface,NULL,DDBLTFAST_WAIT);
			
			if (!*hdl)
			{
				*hdl = ConvertSurfaceToTexture(surface2);

#ifdef TEXTURE_DEBUG
			if( *hdl && *hdl < MAX_HDLCHECKS )
				hdlCheck[*hdl] = 1;
			else
				dprintf"Invalid texture handle %d\n", *hdl));
#endif
			}
			
			hdl++;
			dds++;
			dds2++;
		}
	}
}



/*	--------------------------------------------------------------------------------
	Function		: DrawAlphaSpriteRotating
	Purpose			: clips and draws a rotating sprite
	Parameters		: 
	Returns			: void
	Info			: 
*/

typedef struct POLYCLIP
{
	int numVerts;
	D3DTLVERTEX verts[8];
};

POLYCLIP tempPoly;

void SpriteClip_Do(POLYCLIP *polyIn,POLYCLIP *polyOut);
void SpriteClip_Left(POLYCLIP *poly,D3DTLVERTEX *v0,D3DTLVERTEX *v1);
void SpriteClip_Right(POLYCLIP *poly,D3DTLVERTEX *v0,D3DTLVERTEX *v1);
void SpriteClip_Top(POLYCLIP *poly,D3DTLVERTEX *v0,D3DTLVERTEX *v1);
void SpriteClip_Bottom(POLYCLIP *poly,D3DTLVERTEX *v0,D3DTLVERTEX *v1);


void DrawAlphaSpriteRotating(float *pos,float angle,float x, float y, float z, float xs, float ys, float u1, float v1, float u2, float v2, D3DTEXTUREHANDLE h, DWORD colour )
{
	POLYCLIP p2d,drawPoly;
	float x2 = (x+xs), y2 = (y+ys);
	float fogAmt;

	float sine,cosine;
	float newX,newY;
	int i,j;

	fogAmt = FOGADJ(z);
	if (fogAmt<0)
		fogAmt=0;
	if (fogAmt>1)
		fogAmt=1;

	// populate our structure ready for transforming and clipping the sprite
	p2d.numVerts = 4;
	p2d.verts[0].sx = x - pos[X];
	p2d.verts[1].sx = x2 - pos[X];
	p2d.verts[2].sx = x2 - pos[X];
	p2d.verts[3].sx = x - pos[X];

	p2d.verts[0].sy = y - pos[Y];
	p2d.verts[1].sy = y - pos[Y];
	p2d.verts[2].sy = y2 - pos[Y];
	p2d.verts[3].sy = y2 - pos[Y];

	p2d.verts[0].tu = u1;	p2d.verts[0].tv = v1;
	p2d.verts[1].tu = u2;	p2d.verts[1].tv = v1;
	p2d.verts[2].tu = u2;	p2d.verts[2].tv = v2;
	p2d.verts[3].tu = u1;	p2d.verts[3].tv = v2;

	// populate remaining data members
	i = p2d.numVerts;
	while(i--)
	{
		p2d.verts[i].sz			= z;
		p2d.verts[i].rhw		= 0;
		p2d.verts[i].color		= colour;
		p2d.verts[i].specular	= FOGVAL(fogAmt);
	}

	// get rotation angle
	cosine	= cosf(angle);
	sine	= sinf(angle);

	// rotate the vertices comprising the sprite
	i = p2d.numVerts;
	while(i--)
	{
		newX = (p2d.verts[i].sx * cosine) + (p2d.verts[i].sy * sine);
		newY = (p2d.verts[i].sy * cosine) - (p2d.verts[i].sx * sine);

		p2d.verts[i].sx = newX + pos[X];
		p2d.verts[i].sy = newY + pos[Y];
	}

	// clip the rotated sprite here...
	SpriteClip_Do(&p2d,&drawPoly);

	if (h!=lastH)
	{
#ifdef TEXTURE_DEBUG
		if( CheckTexHDL(h) )
#endif
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,h);
		lastH = h;
	}

	if ((z>0.01) || (z<-0.01))
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,1);
	else
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,0);

	if (pDirect3DDevice->DrawPrimitive(
		D3DPT_TRIANGLEFAN,
		D3DVT_TLVERTEX,

		drawPoly.verts,
		drawPoly.numVerts,

		D3DDP_DONOTCLIP | D3DDP_DONOTLIGHT | D3DDP_DONOTUPDATEEXTENTS 
		)!=D3D_OK)
	{
		dp("Poo-poo!\n");
	}
	
	for(i=0; i<drawPoly.numVerts; i++)
	{
		j = i + 1;
		if(j == drawPoly.numVerts)
			j = 0;

		DrawALine(drawPoly.verts[i].sx,drawPoly.verts[i].sy,drawPoly.verts[j].sx,drawPoly.verts[j].sy,0xffffffff);
	}
}

// use Sutherland - Hodgman edge clipping algorithm thingyjob - ANDYE

void SpriteClip_Do(POLYCLIP *polyIn,POLYCLIP *polyOut)
{
	int v,d;

	polyOut->numVerts = 0;
	tempPoly.numVerts = 0;

	for(v=0; v<polyIn->numVerts; v++)
	{
		d = v + 1;
		if(d == polyIn->numVerts) d = 0;
		SpriteClip_Left(&tempPoly,&polyIn->verts[v],&polyIn->verts[d]);
	}

	for(v=0; v<tempPoly.numVerts; v++)
	{
		d = v + 1;
		if(d == tempPoly.numVerts) d = 0;
		SpriteClip_Right(polyOut,&tempPoly.verts[v],&tempPoly.verts[d]);
	}

	tempPoly.numVerts = 0;
	for(v=0; v<polyOut->numVerts; v++)
	{
		d = v + 1;
		if(d == polyOut->numVerts) d = 0;
		SpriteClip_Top(&tempPoly,&polyOut->verts[v],&polyOut->verts[d]);
	}

	polyOut->numVerts = 0;
	for(v=0; v<tempPoly.numVerts; v++)
	{
		d = v + 1;
		if(d == tempPoly.numVerts) d = 0;
		SpriteClip_Bottom(polyOut,&tempPoly.verts[v],&tempPoly.verts[d]);
	}
}

void SpriteClip_Left(POLYCLIP *poly,D3DTLVERTEX *v0,D3DTLVERTEX *v1)
{
	float dx,dy,m,segLen;
	float du,dv;

	dx		= v1->sx - v0->sx;
	dy		= v1->sy - v0->sy;
	du		= v1->tu - v0->tu;
	dv		= v1->tv - v0->tv;
	segLen	= SPRITECLIPLEFT - v0->sx;
	m		= segLen / dx;

	// check if polygon edge is in viewport
	if((v0->sx >= SPRITECLIPLEFT) && (v1->sx >= SPRITECLIPLEFT))
	{
		poly->verts[poly->numVerts] = *(v1);
		poly->numVerts++;
	}

	// check if polygon edge is leaving viewport
	if((v0->sx >= SPRITECLIPLEFT) && (v1->sx < SPRITECLIPLEFT))
	{
		poly->verts[poly->numVerts] = *(v0);
		poly->verts[poly->numVerts].sx = SPRITECLIPLEFT;
		poly->verts[poly->numVerts].sy = v0->sy + (dy * m);
		poly->verts[poly->numVerts].tu = v0->tu + (du * m);
		poly->verts[poly->numVerts].tv = v0->tv + (dv * m);
		poly->numVerts++;
	}

	// check if polygon edge is entering viewport
	if((v0->sx < SPRITECLIPLEFT) && (v1->sx >= SPRITECLIPLEFT))
	{
		poly->verts[poly->numVerts] = *(v0);
		poly->verts[poly->numVerts].sx = SPRITECLIPLEFT;
		poly->verts[poly->numVerts].sy = v0->sy + (dy * m);
		poly->verts[poly->numVerts].tu = v0->tu + (du * m);
		poly->verts[poly->numVerts].tv = v0->tv + (dv * m);
		poly->numVerts++;

		poly->verts[poly->numVerts] = *(v1);
		poly->numVerts++;
	}
}

void SpriteClip_Right(POLYCLIP *poly,D3DTLVERTEX *v0,D3DTLVERTEX *v1)
{
	float dx,dy,m,segLen;
	float du,dv;

	dx		= v1->sx - v0->sx;
	dy		= v1->sy - v0->sy;
	du		= v1->tu - v0->tu;
	dv		= v1->tv - v0->tv;
	segLen	= SPRITECLIPRIGHT - 1 - v0->sx;
	m		= segLen / dx;

	// check if polygon edge is in viewport
	if((v0->sx < SPRITECLIPRIGHT) && (v1->sx < SPRITECLIPRIGHT))
	{
		poly->verts[poly->numVerts] = *(v1);
		poly->numVerts++;
	}

	// check if polygon edge is leaving viewport
	if((v0->sx < SPRITECLIPRIGHT) && (v1->sx >= SPRITECLIPRIGHT))
	{
		poly->verts[poly->numVerts] = *(v0);
		poly->verts[poly->numVerts].sx = SPRITECLIPRIGHT - 1;
		poly->verts[poly->numVerts].sy = v0->sy + (dy * m);
		poly->verts[poly->numVerts].tu = v0->tu + (du * m);
		poly->verts[poly->numVerts].tv = v0->tv + (dv * m);
		poly->numVerts++;
	}

	// check if polygon edge is entering viewport
	if((v0->sx >= SPRITECLIPRIGHT) && (v1->sx < SPRITECLIPRIGHT))
	{
		poly->verts[poly->numVerts] = *(v0);
		poly->verts[poly->numVerts].sx = SPRITECLIPRIGHT - 1;
		poly->verts[poly->numVerts].sy = v0->sy + (dy * m);
		poly->verts[poly->numVerts].tu = v0->tu + (du * m);
		poly->verts[poly->numVerts].tv = v0->tv + (dv * m);
		poly->numVerts++;

		poly->verts[poly->numVerts] = *(v1);
		poly->numVerts++;
	}
}

void SpriteClip_Top(POLYCLIP *poly,D3DTLVERTEX *v0,D3DTLVERTEX *v1)
{
	float dx,dy,m,segLen;
	float du,dv;

	dx		= v1->sx - v0->sx;
	dy		= v1->sy - v0->sy;
	du		= v1->tu - v0->tu;
	dv		= v1->tv - v0->tv;
	segLen	= SPRITECLIPTOP - v0->sy;
	m		= segLen / dy;

	// check if polygon edge is in viewport
	if((v0->sy >= SPRITECLIPTOP) && (v1->sy >= SPRITECLIPTOP))
	{
		poly->verts[poly->numVerts] = *(v1);
		poly->numVerts++;
	}

	// check if polygon edge is leaving viewport
	if((v0->sy >= SPRITECLIPTOP) && (v1->sy < SPRITECLIPTOP))
	{
		poly->verts[poly->numVerts] = *(v0);
		poly->verts[poly->numVerts].sx = v0->sx + (dx * m);
		poly->verts[poly->numVerts].sy = SPRITECLIPTOP;
		poly->verts[poly->numVerts].tu = v0->tu + (du * m);
		poly->verts[poly->numVerts].tv = v0->tv + (dv * m);
		poly->numVerts++;
	}

	// check if polygon edge is entering viewport
	if((v0->sy < SPRITECLIPTOP) && (v1->sy >= SPRITECLIPTOP))
	{
		poly->verts[poly->numVerts] = *(v0);
		poly->verts[poly->numVerts].sx = v0->sx + (dx * m);
		poly->verts[poly->numVerts].sy = SPRITECLIPTOP;
		poly->verts[poly->numVerts].tu = v0->tu + (du * m);
		poly->verts[poly->numVerts].tv = v0->tv + (dv * m);
		poly->numVerts++;

		poly->verts[poly->numVerts] = *(v1);
		poly->numVerts++;
	}
}

void SpriteClip_Bottom(POLYCLIP *poly,D3DTLVERTEX *v0,D3DTLVERTEX *v1)
{
	float dx,dy,m,segLen;
	float du,dv;

	dx		= v1->sx - v0->sx;
	dy		= v1->sy - v0->sy;
	du		= v1->tu - v0->tu;
	dv		= v1->tv - v0->tv;
	segLen	= SPRITECLIPBOTTOM - 1 - v0->sy;
	m		= segLen / dy;

	// check if polygon edge is in viewport
	if((v0->sy < SPRITECLIPBOTTOM) && (v1->sy < SPRITECLIPBOTTOM))
	{
		poly->verts[poly->numVerts] = *(v1);
		poly->numVerts++;
	}

	// check if polygon edge is leaving viewport
	if((v0->sy < SPRITECLIPBOTTOM) && (v1->sy >= SPRITECLIPBOTTOM))
	{
		poly->verts[poly->numVerts] = *(v0);
		poly->verts[poly->numVerts].sx = v0->sx + (dx * m);
		poly->verts[poly->numVerts].sy = SPRITECLIPBOTTOM - 1;
		poly->verts[poly->numVerts].tu = v0->tu + (du * m);
		poly->verts[poly->numVerts].tv = v0->tv + (dv * m);
		poly->numVerts++;
	}

	// check if polygon edge is entering viewport
	if((v0->sy >= SPRITECLIPBOTTOM) && (v1->sy < SPRITECLIPBOTTOM))
	{
		poly->verts[poly->numVerts] = *(v0);
		poly->verts[poly->numVerts].sx = v0->sx + (dx * m);
		poly->verts[poly->numVerts].sy = SPRITECLIPBOTTOM - 1;
		poly->verts[poly->numVerts].tu = v0->tu + (du * m);
		poly->verts[poly->numVerts].tv = v0->tv + (dv * m);
		poly->numVerts++;

		poly->verts[poly->numVerts] = *(v1);
		poly->numVerts++;
	}
}
