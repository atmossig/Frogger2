/*	A demo of i-MAGE (Written for E3'99)
	Written by Matthew Cloy (c)1999 Interactive Studios Ltd.
	
	directx.cpp		: All directx related gumph.

	Created			: Sat 01 May 1999 - Matthew Cloy - Chesham
	Modified		: Sat 01 May 1999 - Matthew Cloy - Chesham: r0.0.01 - Project started
					: Sun 02 May 1999 - Matthew Cloy - Chesham: r0.0.01 - Direct3D Working
					: Mon 03 May 1999 - Matthew Cloy - Train:	r0.0.01 - Adding texture support

********************************************************************************************/

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <dsound.h>
#include "stdlib.h"
#include "stdio.h"
#include "directx.h"
#include "..\resource.h"
#include <crtdbg.h>
#include "commctrl.h"

#include <windowsx.h>
#include <mmsystem.h>
#include <memory.h>
#include <winbase.h>
//#include "incs.h"

#define SCREEN_WIDTH	640	//320
#define SCREEN_HEIGHT	480	//240
#define SCREEN_BITS		16


extern "C"
{
#include "block.h"

HWND win;

LPDIRECTDRAW			pDirectDraw;
LPDIRECTDRAWSURFACE		primarySrf;
LPDIRECTDRAWSURFACE		hiddenSrf;
LPDIRECTDRAWSURFACE		srfZBuffer;
LPDIRECT3D2				pDirect3D;
LPDIRECT3DDEVICE2		pDirect3DDevice;
LPDIRECT3DVIEWPORT2		pDirect3DViewport;
LPDIRECTDRAW4			pDirectDraw4;

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

#define DEBUG_FILE "C:\\frogger2.log"

char *ddError2String(HRESULT error);
long isHardware = 1;

int dumpScreen = 0;

void ScreenShot ( DDSURFACEDESC ddsd );


void dp(char *format, ...)
{
	va_list			argp;
	FILE *f;

	static char		debugprintfbuffer[2048];

	va_start(argp, format);
	vsprintf(debugprintfbuffer, format, argp);
	va_end(argp);

	_CrtDbgReport(_CRT_WARN, NULL, NULL, "A3Ddemo", debugprintfbuffer);

	f = fopen(DEBUG_FILE, "a+");
	if (f)
	{
		fputs(debugprintfbuffer, f);
		fclose(f);
	}
}


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
		ddError2String (capsResult);
		lpDD->Release();											// Oops couldn't get it, release...
		return DDENUMRET_OK;										// And continue looking.
	}
	
	lpDD->QueryInterface(IID_IDirectDraw4, (LPVOID *)&pDD4);
	pDD4->GetDeviceIdentifier(&ddId,0);
	
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

void EnumDXObjects (void)
{
	DirectDrawEnumerate(EnumDDDevices, NULL);
}

/* ---------------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------------- */

char *ddError2String(HRESULT error)
{
    switch(error)
		{
        case DD_OK:
            return "No error.\0";

        case DDERR_ALREADYINITIALIZED:
            return "This object is already initialized.\0";

        case DDERR_BLTFASTCANTCLIP:
            return "Return if a clipper object is attached to the source surface passed into a BltFast call.\0";

        case DDERR_CANNOTATTACHSURFACE:
            return "This surface can not be attached to the requested surface.\0";

        case DDERR_CANNOTDETACHSURFACE:
            return "This surface can not be detached from the requested surface.\0";

        case DDERR_CANTCREATEDC:
            return "Windows can not create any more DCs.\0";

        case DDERR_CANTDUPLICATE:
            return "Can't duplicate primary & 3D surfaces, or surfaces that are implicitly created.\0";

        case DDERR_CLIPPERISUSINGHWND:
            return "An attempt was made to set a cliplist for a clipper object that is already monitoring an hwnd.\0";

        case DDERR_COLORKEYNOTSET:
            return "No src color key specified for this operation.\0";

        case DDERR_CURRENTLYNOTAVAIL:
            return "Support is currently not available.\0";

        case DDERR_DIRECTDRAWALREADYCREATED:
            return "A DirectDraw object representing this driver has already been created for this process.\0";

        case DDERR_EXCEPTION:
            return "An exception was encountered while performing the requested operation.\0";

        case DDERR_EXCLUSIVEMODEALREADYSET:
            return "An attempt was made to set the cooperative level when it was already set to exclusive.\0";

        case DDERR_GENERIC:
            return "Generic failure.\0";

        case DDERR_HEIGHTALIGN:
            return "Height of rectangle provided is not a multiple of reqd alignment.\0";

        case DDERR_HWNDALREADYSET:
            return "The CooperativeLevel HWND has already been set. It can not be reset while the process has surfaces or palettes created.\0";

        case DDERR_HWNDSUBCLASSED:
            return "HWND used by DirectDraw CooperativeLevel has been subclassed, this prevents DirectDraw from restoring state.\0";

        case DDERR_IMPLICITLYCREATED:
            return "This surface can not be restored because it is an implicitly created surface.\0";

        case DDERR_INCOMPATIBLEPRIMARY:
            return "Unable to match primary surface creation request with existing primary surface.\0";

        case DDERR_INVALIDCAPS:
            return "One or more of the caps bits passed to the callback are incorrect.\0";

        case DDERR_INVALIDCLIPLIST:
            return "DirectDraw does not support the provided cliplist.\0";

        case DDERR_INVALIDDIRECTDRAWGUID:
            return "The GUID passed to DirectDrawCreate is not a valid DirectDraw driver identifier.\0";

        case DDERR_INVALIDMODE:
            return "DirectDraw does not support the requested mode.\0";

        case DDERR_INVALIDOBJECT:
            return "DirectDraw received a pointer that was an invalid DIRECTDRAW object.\0";

        case DDERR_INVALIDPARAMS:
            return "One or more of the parameters passed to the function are incorrect.\0";

        case DDERR_INVALIDPIXELFORMAT:
            return "The pixel format was invalid as specified.\0";

        case DDERR_INVALIDPOSITION:
            return "Returned when the position of the overlay on the destination is no longer legal for that destination.\0";

        case DDERR_INVALIDRECT:
            return "Rectangle provided was invalid.\0";

        case DDERR_LOCKEDSURFACES:
            return "Operation could not be carried out because one or more surfaces are locked.\0";

        case DDERR_NO3D:
            return "There is no 3D present.\0";

        case DDERR_NOALPHAHW:
            return "Operation could not be carried out because there is no alpha accleration hardware present or available.\0";

        case DDERR_NOBLTHW:
            return "No blitter hardware present.\0";

        case DDERR_NOCLIPLIST:
            return "No cliplist available.\0";

        case DDERR_NOCLIPPERATTACHED:
            return "No clipper object attached to surface object.\0";

        case DDERR_NOCOLORCONVHW:
            return "Operation could not be carried out because there is no color conversion hardware present or available.\0";

        case DDERR_NOCOLORKEY:
            return "Surface doesn't currently have a color key\0";

        case DDERR_NOCOLORKEYHW:
            return "Operation could not be carried out because there is no hardware support of the destination color key.\0";

        case DDERR_NOCOOPERATIVELEVELSET:
            return "Create function called without DirectDraw object method SetCooperativeLevel being called.\0";

        case DDERR_NODC:
            return "No DC was ever created for this surface.\0";

        case DDERR_NODDROPSHW:
            return "No DirectDraw ROP hardware.\0";

        case DDERR_NODIRECTDRAWHW:
            return "A hardware-only DirectDraw object creation was attempted but the driver did not support any hardware.\0";

        case DDERR_NOEMULATION:
            return "Software emulation not available.\0";

        case DDERR_NOEXCLUSIVEMODE:
            return "Operation requires the application to have exclusive mode but the application does not have exclusive mode.\0";

        case DDERR_NOFLIPHW:
            return "Flipping visible surfaces is not supported.\0";

        case DDERR_NOGDI:
            return "There is no GDI present.\0";

        case DDERR_NOHWND:
            return "Clipper notification requires an HWND or no HWND has previously been set as the CooperativeLevel HWND.\0";

        case DDERR_NOMIRRORHW:
            return "Operation could not be carried out because there is no hardware present or available.\0";

        case DDERR_NOOVERLAYDEST:
            return "Returned when GetOverlayPosition is called on an overlay that UpdateOverlay has never been called on to establish a destination.\0";

        case DDERR_NOOVERLAYHW:
            return "Operation could not be carried out because there is no overlay hardware present or available.\0";

        case DDERR_NOPALETTEATTACHED:
            return "No palette object attached to this surface.\0";

        case DDERR_NOPALETTEHW:
            return "No hardware support for 16 or 256 color palettes.\0";

        case DDERR_NORASTEROPHW:
            return "Operation could not be carried out because there is no appropriate raster op hardware present or available.\0";

        case DDERR_NOROTATIONHW:
            return "Operation could not be carried out because there is no rotation hardware present or available.\0";

        case DDERR_NOSTRETCHHW:
            return "Operation could not be carried out because there is no hardware support for stretching.\0";

        case DDERR_NOT4BITCOLOR:
            return "DirectDrawSurface is not in 4 bit color palette and the requested operation requires 4 bit color palette.\0";

        case DDERR_NOT4BITCOLORINDEX:
            return "DirectDrawSurface is not in 4 bit color index palette and the requested operation requires 4 bit color index palette.\0";

        case DDERR_NOT8BITCOLOR:
            return "DirectDrawSurface is not in 8 bit color mode and the requested operation requires 8 bit color.\0";

        case DDERR_NOTAOVERLAYSURFACE:
            return "Returned when an overlay member is called for a non-overlay surface.\0";

        case DDERR_NOTEXTUREHW:
            return "Operation could not be carried out because there is no texture mapping hardware present or available.\0";

        case DDERR_NOTFLIPPABLE:
            return "An attempt has been made to flip a surface that is not flippable.\0";

        case DDERR_NOTFOUND:
            return "Requested item was not found.\0";

        case DDERR_NOTLOCKED:
            return "Surface was not locked.  An attempt to unlock a surface that was not locked at all, or by this process, has been attempted.\0";

        case DDERR_NOTPALETTIZED:
            return "The surface being used is not a palette-based surface.\0";

        case DDERR_NOVSYNCHW:
            return "Operation could not be carried out because there is no hardware support for vertical blank synchronized operations.\0";

        case DDERR_NOZBUFFERHW:
            return "Operation could not be carried out because there is no hardware support for zbuffer blitting.\0";

        case DDERR_NOZOVERLAYHW:
            return "Overlay surfaces could not be z layered based on their BltOrder because the hardware does not support z layering of overlays.\0";

        case DDERR_OUTOFCAPS:
            return "The hardware needed for the requested operation has already been allocated.\0";

        case DDERR_OUTOFMEMORY:
            return "DirectDraw does not have enough memory to perform the operation.\0";

        case DDERR_OUTOFVIDEOMEMORY:
            return "DirectDraw does not have enough memory to perform the operation.\0";

        case DDERR_OVERLAYCANTCLIP:
            return "The hardware does not support clipped overlays.\0";

        case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
            return "Can only have ony color key active at one time for overlays.\0";

        case DDERR_OVERLAYNOTVISIBLE:
            return "Returned when GetOverlayPosition is called on a hidden overlay.\0";

        case DDERR_PALETTEBUSY:
            return "Access to this palette is being refused because the palette is already locked by another thread.\0";

        case DDERR_PRIMARYSURFACEALREADYEXISTS:
            return "This process already has created a primary surface.\0";

        case DDERR_REGIONTOOSMALL:
            return "Region passed to Clipper::GetClipList is too small.\0";

        case DDERR_SURFACEALREADYATTACHED:
            return "This surface is already attached to the surface it is being attached to.\0";

        case DDERR_SURFACEALREADYDEPENDENT:
            return "This surface is already a dependency of the surface it is being made a dependency of.\0";

        case DDERR_SURFACEBUSY:
            return "Access to this surface is being refused because the surface is already locked by another thread.\0";

        case DDERR_SURFACEISOBSCURED:
            return "Access to surface refused because the surface is obscured.\0";

        case DDERR_SURFACELOST:
            return "Access to this surface is being refused because the surface memory is gone. The DirectDrawSurface object representing this surface should have Restore called on it.\0";

        case DDERR_SURFACENOTATTACHED:
            return "The requested surface is not attached.\0";

        case DDERR_TOOBIGHEIGHT:
            return "Height requested by DirectDraw is too large.\0";

        case DDERR_TOOBIGSIZE:
            return "Size requested by DirectDraw is too large, but the individual height and width are OK.\0";

        case DDERR_TOOBIGWIDTH:
            return "Width requested by DirectDraw is too large.\0";

        case DDERR_UNSUPPORTED:
            return "Action not supported.\0";

        case DDERR_UNSUPPORTEDFORMAT:
            return "FOURCC format requested is unsupported by DirectDraw.\0";

        case DDERR_UNSUPPORTEDMASK:
            return "Bitmask in the pixel format requested is unsupported by DirectDraw.\0";

        case DDERR_VERTICALBLANKINPROGRESS:
            return "Vertical blank is in progress.\0";

        case DDERR_WASSTILLDRAWING:
            return "Informs DirectDraw that the previous Blt which is transfering information to or from this Surface is incomplete.\0";

        case DDERR_WRONGMODE:
            return "This surface can not be restored because it was created in a different mode.\0";

        case DDERR_XALIGN:
            return "Rectangle provided was not horizontally aligned on required boundary.\0";

        default:
            return "Unrecognized error value.\0";
	    }
}

char col0txt[] = "Name";
char col1txt[] = "Description";
char col2txt[] = "Caps";
char hwText[] = "3D Hardware Accelerated";
char swText[] = "Software Renderer";

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
				LV_ITEM itm;
				
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

			ListView_SetItemState(list, 0, LVIS_SELECTED | LVIS_FOCUSED, 0x00FF);

			SetWindowPos(hwndDlg, HWND_TOPMOST, (GetSystemMetrics(SM_CXSCREEN)-(meR.right-meR.left))/2,(GetSystemMetrics(SM_CYSCREEN)-(meR.bottom-meR.top))/2, 0,0,SWP_NOSIZE);

			// Direct Sound Setup part......

/*			hCombo = GetDlgItem ( hwndDlg, IDC_SOUNDSELECT );
			lpGUID = ( LPGUID ) lParam;

			if ( DirectSoundEnumerate ( ( LPDSENUMCALLBACK ) DSEnumProc, &hCombo ) != DS_OK )
			{
				EndDialog ( hwndDlg, TRUE );
				return( TRUE );
			}

		    if( ComboBox_GetCount ( hCombo ) )
				ComboBox_SetCurSel ( hCombo, 0 );
			else
			{
				EndDialog ( hwndDlg, TRUE );
				return( TRUE );
			}
			// ENDELSEIF*/

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
				case IDCANCEL:
					PostQuitMessage(0);
					runQuit = 1;
					break;
				case IDOK:
				{
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


//					EndDialog(hwndDlg,0);
					runQuit = 0;
					break;
				}
			}
			break;
	}
	return FALSE;
}


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






long DirectXInit(HWND window, long hardware)
{
	D3DVIEWPORT				viewport;
	D3DFINDDEVICERESULT		result;
	D3DFINDDEVICESEARCH		search;
	DDSURFACEDESC			ddsd;
	DDSURFACEDESC sdesc;
	int l;
	win = window;

	EnumDXObjects();

	DialogBox(winInfo.hInstance, MAKEINTRESOURCE(IDD_DIALOG1),window,(DLGPROC)HardwareProc);

	
	if (runQuit)
		return 0;
	
	isHardware = (dxDeviceList[selIdx].caps.dwCaps & DDCAPS_3D);
	hardware = isHardware;

//	while (0==0);
	// Create main DirectDraw object
	if (DirectDrawCreate(dxDeviceList[selIdx].guid, &pDirectDraw, NULL) != DD_OK)
	 return FALSE;

	if (pDirectDraw->QueryInterface(IID_IDirectDraw4, (LPVOID *)&pDirectDraw4) != S_OK)
		return FALSE;	

	if (pDirectDraw->SetCooperativeLevel(window, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWMODEX) != DD_OK)
	 return FALSE;

	if (pDirectDraw->SetDisplayMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BITS))
	 return FALSE;

	// Get the primary display surface
	DDINIT(ddsd);
	ddsd.dwFlags = DDSD_CAPS;

	if (hardware)
	 ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_VIDEOMEMORY;
	else
	 ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_SYSTEMMEMORY;
	
	if (pDirectDraw->CreateSurface(&ddsd, &primarySrf, NULL) != DD_OK)
	 return FALSE;

	// Create a back buffer and attach it to the primary display surface to make a flippable surface
	DDINIT(ddsd);
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.dwWidth = SCREEN_WIDTH;
	ddsd.dwHeight = SCREEN_HEIGHT;

	if (hardware)
	 ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_3DDEVICE /*| DDSCAPS_VIDEOMEMORY*/;
	else
	 ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE | DDSCAPS_SYSTEMMEMORY;
	
	if (pDirectDraw->CreateSurface(&ddsd, &hiddenSrf, NULL) != DD_OK)
	 return FALSE;
	
	if (primarySrf->AddAttachedSurface(hiddenSrf) != DD_OK)
	 return FALSE;


//	if (hardware)
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
		if (pDirectDraw->CreateSurface(&ddsd, &srfZBuffer, NULL) != DD_OK) 
			return FALSE;
		if (hiddenSrf->AddAttachedSurface(srfZBuffer) != DD_OK) 
			return FALSE;
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
	if (pDirect3D->CreateDevice(result.guid, hiddenSrf, &pDirect3DDevice) != D3D_OK)
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
	
	memset (&sdesc,0,sizeof (sdesc));
	sdesc.dwSize=sizeof (sdesc);

	primarySrf->GetSurfaceDesc(&sdesc);
	l = (int)sdesc.ddpfPixelFormat.dwGBitMask;
	while ((l&1) == 0)
		  l >>= 1;
	
	a565Card = !(l == 31);
	
	if (hardware)
		SetupRenderstates(); 
	
	return TRUE;
}

float bRed = 0, bGreen = 0, bBlue = 0;
 
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
		while ( hiddenSrf->Lock ( NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR, 0 ) != DD_OK );

		// run screen grab
		ScreenShot ( ddsd );


		hiddenSrf->Unlock ( ddsd.lpSurface );
		dumpScreen = 0;
	}
	// ENDIF

	// Flip the back buffer to the primary surface
	primarySrf->Flip(NULL,DDFLIP_WAIT);
//	while (primarySrf->Blt(NULL,hiddenSrf,NULL,NULL,NULL)!=DD_OK);

	
	DDINIT(m);
	m.dwFillColor = D3DRGB((bRed/(float)0xff),(bGreen/(float)0xff),(bBlue/(float)0xff));
	while (hiddenSrf->Blt(NULL,NULL,NULL,DDBLT_WAIT | DDBLT_COLORFILL,&m)!=DD_OK);
	DDINIT(m);
	m.dwFillDepth = -1;//D3DRGB(0,0,0);
	while (srfZBuffer->Blt(NULL,NULL,NULL,DDBLT_WAIT | DDBLT_DEPTHFILL,&m)!=DD_OK);
}

void SetupRenderstates(void)
{
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,NULL);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE,FALSE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND,D3DTBLEND_MODULATEALPHA);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE,TRUE);

//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_LINEAR);
//	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMIN,D3DFILTER_LINEAR);

	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_SUBPIXEL,FALSE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE,D3DSHADE_GOURAUD);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_CW);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE,TRUE);
	
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,TRUE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,TRUE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC,D3DCMP_LESSEQUAL);
}

// Split this out into two functions (CreateTextureSurface and CopyToSurface)
LPDIRECTDRAWSURFACE CreateTextureSurface(long xs,long ys, short *data, BOOL hardware, long cKey, long aiSurf)
{ 
	HRESULT capsResult;
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

void ReleaseSurface(LPDIRECTDRAWSURFACE surf)
{
	RELEASE(surf);
}

D3DTEXTUREHANDLE lastH = NULL;
void BeginDrawHardware (void)
{
	pDirect3DDevice->BeginScene();
	lastH = NULL;
}

void EndDrawHardware (void)
{
	pDirect3DDevice->EndScene();
}

void DrawAHardwarePoly (D3DTLVERTEX *v,long vC, short *fce, long fC, D3DTEXTUREHANDLE h)
{
	if (h!=lastH)
	{
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,h);
		lastH = h;
	}
	
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,1);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,1);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZFUNC,D3DCMP_LESS);
	
	if (pDirect3DDevice->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST,
		D3DVT_TLVERTEX,
		v,
		vC,
		(unsigned short *)fce,
		fC,
		D3DDP_DONOTCLIP 
			| D3DDP_DONOTLIGHT 
			| D3DDP_DONOTUPDATEEXTENTS 
			/*| D3DDP_WAIT*/)!=D3D_OK)
	{
		dp("BUGGER !!!!! CAN'T DRAW POLY JOBBY\n");
	}
}

void DrawALine (float x1, float y1, float x2, float y2, D3DCOLOR color)
{
	D3DTLVERTEX v[2] = {
		{
			x1,y1,0,0,
			color, 0.0,
			0.0, 0.0
		},
		{
			x2,y2,0,0,
			color, 0.0,
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

void DrawASprite (float x, float y, float xs, float ys, float u1, float v1, float u2, float v2, D3DTEXTUREHANDLE h)
{
	D3DTLVERTEX v[4] = {
		{
			x,y,0,0,
			D3DRGB(1,1,1),D3DRGB(0,0,0),
			u1,v1
		},
		{
			x+xs,y,0,0,
			D3DRGB(1,1,1),D3DRGB(0,0,0),
			u2,v1
		},
		{
			x+xs,y+ys,0,0,
			D3DRGB(1,1,1),D3DRGB(0,0,0),
			u2,v2
		},
		{
		x,y+ys,0,0,
			D3DRGB(1,1,1),D3DRGB(0,0,0),
			u1,v2
	}};

	if (h!=lastH)
	{
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
			| D3DDP_WAIT)!=D3D_OK)
	{
		dp("Could not print sprite\n");
		// BUGGER !!!!! CAN'T DRAW POLY JOBBY !
	}
}

void DrawAlphaSprite (float x, float y, float z, float xs, float ys, float u1, float v1, float u2, float v2, D3DTEXTUREHANDLE h, float alpha)
{
	D3DTLVERTEX v[4] = {
		{
			x,y,z,0,
			D3DRGBA(1,1,1,alpha),0,
			u1,v1
		},
		{
			x+xs,y,z,0,
			D3DRGBA(1,1,1,alpha),0,
			u2,v1
		},
		{
			x+xs,y+ys,z,0,
			D3DRGBA(1,1,1,alpha),0,
			u2,v2
		},
		{
			x,y+ys,z,0,
			D3DRGBA(1,1,1,alpha),0,
			u1,v2
	}};

	if (h!=lastH)
	{
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,h);
		lastH = h;
	}

	if ((z>0.01) | (z<-0.01))
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,1);
	else	
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,0);
	
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,0);
	
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
	
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_BOTHSRCALPHA);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_BOTHSRCALPHA);

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
		dp("Could not print sprite\n");
		// BUGGER !!!!! CAN'T DRAW POLY JOBBY !
	}

	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
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
			colour,0,
			0,0
		},
		{
			r.left,r.bottom,0,0,
			colour,0,
			0,0
		},
		{
			r.right,r.bottom,0,0,
			colour,0,
			0,0
		},
		{
			r.right,r.top,0,0,
			colour,0,
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
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_BOTHSRCALPHA);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_BOTHSRCALPHA);

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
	short pixel;
	unsigned char col;
	unsigned char line [ 640 * 4 ];

	char header[] =	   {0x42,0x4D,0x36,0x84,0x03,0x00,0x00,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00,
						0x00,0x00,0x40,0x01,0x00,0x00,0xF0,0x00,0x00,0x00,0x01,0x00,0x18,0x00,0x00,0x00,
						0x00,0x00,0x00,0x84,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00};

	sprintf ( fileName, "c:\\pc%04d.bmp", picnum++);

	fp = fopen ( fileName, "w" );
	if ( fp == NULL )
		return;
	// ENDIF

	fwrite ( header, sizeof ( header ), 1, fp );

	y = 480-1;
	while (y >= 0)
	{
		linePos = 0;
		for(x = 0; x < 640; x++)
		{
			pixel = ((short*)ddsd.lpSurface)[x + y * (ddsd.lPitch/2)];

			col = ((pixel>>1)<<3)&0xFF;
			line[linePos++] = col;
			col = ((pixel>>6)<<3)&0xFF;
			line[linePos++] = col;
			col = ((pixel>>11)<<3)&0xFF;
			line[linePos++] = col;


		}
		y--;

			fwrite ( line, sizeof ( linePos ), 1, fp );//(file, line, linePos);	
		
	}


	fclose ( fp );
}


