
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <math.h>
#include "mgeReport.h"
#include "mdxDDraw.h"
#include "mdxD3D.h"
#include "mdxInfo.h"
#include "mdxTiming.h"
#include "mdxCRC.h"
#include "mdxTexture.h"
#include "mdxMath.h"
#include "mdxObject.h"
#include "mdxActor.h"
#include "mdxLandscape.h"
#include "mdxRender.h"
#include "mdxPoly.h"
#include "mdxDText.h"
#include "mdxProfile.h"
#include "mdxWindows.h"
#include "gelf.h"
#include "commctrl.h"
#include "majikPR.h"

#include "resource.h"

LPDIRECTDRAW7			pDirectDraw7;
LPDIRECTDRAWCLIPPER		pClipper;
unsigned long			rXRes, rYRes, rBitDepth, r565 ,rHardware,rFullscreen = 1, rScale = 1, rFlipOK = 1;
HWND					rWin;

LPDIRECTDRAWSURFACE7	surface[NUM_SRF] = {NULL,NULL,NULL};

LPDIRECTDRAWSURFACE7	backdrop = NULL;

WNDPROC userDlgProc = NULL;

WNDPROC SetUserVideoProc(WNDPROC proc)
{
	WNDPROC oldDlgProc = userDlgProc;
	userDlgProc = proc;

	return oldDlgProc;
}

MDX_DXDEVICE dxDeviceList[100];
unsigned long dxNumDevices = 0;

BOOL WINAPI  EnumDDDevices(GUID FAR* lpGUID, LPSTR lpDriverDesc, LPSTR lpDriverName, LPVOID lpContext, HMONITOR mon)
{
    LPDIRECTDRAW7	lpDD;
    DDCAPS			ddCaps;
	DDDEVICEIDENTIFIER2 ddId;

//	if (!_CrtCheckMemory())
//	{
//		dp("Heap is corrupt!\n");
//		__asm {int 3};
//	}

	if (DirectDrawCreateEx(lpGUID, (LPVOID *)&lpDD, IID_IDirectDraw7, NULL)!=DD_OK)				// Couldn't create DirectDraw device...
		return DDENUMRET_OK;										// so continue on to the next one

//	if (!_CrtCheckMemory())
//	{
//		dp("Heap is corrupt!\n");
//		__asm {int 3};
//	}

	DDINIT(ddCaps);													// Init caps struct
	
 	HRESULT capsResult;
	capsResult = lpDD->GetCaps(&ddCaps, NULL);					// Get the caps for the device
 	if (capsResult!=DD_OK)										// Get the caps for the device 	if (lpDD->GetCaps(&ddCaps, NULL)!=DD_OK)						// Get the caps for the device
	{
		lpDD->Release();											// Oops couldn't get it, release...
		return DDENUMRET_OK;										// And continue looking.
	}

//	if (!_CrtCheckMemory())
//	{
//		dp("Heap is corrupt!\n");
//		__asm {int 3};
//	}

	lpDD->GetDeviceIdentifier(&ddId,0);

	dxDeviceList[dxNumDevices].desc = new char [strlen (ddId.szDescription)+1];
	dxDeviceList[dxNumDevices].name = new char [strlen (ddId.szDriver)+1];
	
	strcpy(dxDeviceList[dxNumDevices].desc, ddId.szDescription);
	strcpy(dxDeviceList[dxNumDevices].name, ddId.szDriver);

	dxDeviceList[dxNumDevices].caps = ddCaps;						// Implicit structure copy.

//	if (!_CrtCheckMemory())
//	{
//		dp("Heap is corrupt!\n");
//		__asm {int 3};
//	}

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

char col0txt[] = "Description";
char col1txt[] = "Driver DLL";
unsigned long selIdx;
LV_ITEM i1 = {LVIF_TEXT,0,0,0,0,NULL,255};
LV_COLUMN c1 = {LVCF_FMT | LVCF_TEXT | LVCF_WIDTH,LVCFMT_LEFT,700,col1txt,255,0};
LV_COLUMN c2 = {LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM,LVCFMT_LEFT,500,col0txt,255,1};

#define NUM_LANGUAGES 5

char *languageText[NUM_LANGUAGES] = {"English","French","German","Italian","Hungarian"};

BOOL CALLBACK HardwareProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND list;
	int i,lastIdx;

	if (userDlgProc)
		if (userDlgProc(hwndDlg,uMsg,wParam, lParam))
			return TRUE;

    switch(uMsg)
	{
		case WM_INITDIALOG:
		{
			RECT meR;
			
			LV_ITEM itm;
	

			GetWindowRect(hwndDlg, &meR);
			ShowWindow(hwndDlg,SW_SHOW);
			SetWindowPos(hwndDlg, HWND_TOPMOST, (GetSystemMetrics(SM_CXSCREEN)-(meR.right-meR.left))/2,(GetSystemMetrics(SM_CYSCREEN)-(meR.bottom-meR.top))/2, 0,0,SWP_NOSIZE);
			list = GetDlgItem(hwndDlg,IDC_LIST2);
		
			for (i=NUM_LANGUAGES; i>0; i--)
				SendMessage ( GetDlgItem(hwndDlg,IDC_LANGUAGE),LB_INSERTSTRING,0,(unsigned int)languageText[i-1]);

			SendMessage ( GetDlgItem(hwndDlg,IDC_LANGUAGE),LB_SETCURSEL,0,0);

			SendMessage (list,LVM_INSERTCOLUMN,0,(long)&c1);
			SendMessage (list,LVM_INSERTCOLUMN,0,(long)&c2);
				
			for (i=0; i<dxNumDevices; i++)
			{
				
				if ((dxDeviceList[i].guid == (GUID *)-1) || (dxDeviceList[i].caps.dwCaps & DDCAPS_3D))
				{
					i1.iItem = i; 
					i1.pszText = dxDeviceList[i].desc;
					i1.lParam = i; 
					itm.iSubItem = 0;
								
					lastIdx = dxDeviceList[i].idx = SendMessage (list,LVM_INSERTITEM,0,(long)&i1);

					itm.iItem = i; 
					itm.lParam = i; 
					itm.pszText = dxDeviceList[i].name;
					itm.iSubItem = 1;
					
					SendMessage (list,LVM_SETITEM,lastIdx,(long)&itm);
				}
			}

	
			ListView_SetItemState(list, 0, LVIS_SELECTED | LVIS_FOCUSED, 0x00FF);

			return TRUE;
			break;
		}

		case WM_CLOSE:
			PostQuitMessage(0);
			EndDialog ( hwndDlg, FALSE );
            return TRUE;
			break;
		
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDCANCEL:
					PostQuitMessage(0);
					EndDialog ( hwndDlg, FALSE );
					break;
	
				case IDOK:
				{
					for (i=0; i<SendMessage (GetDlgItem(hwndDlg,IDC_LIST2),LVM_GETITEMCOUNT,0,0); i++)
						if (SendMessage (GetDlgItem(hwndDlg,IDC_LIST2),LVM_GETITEMSTATE,i,LVIS_SELECTED))
							selIdx = i;

					if (SendMessage (GetDlgItem(hwndDlg,IDC_WINDOW),BM_GETCHECK,0,0) == BST_CHECKED)
					{
						dp("Running Windowed!\n");
						rFullscreen = 0;
					}

					EndDialog ( hwndDlg, TRUE );
					break;
				}
			}
			break;		
	}

	return FALSE;
}


/*	--------------------------------------------------------------------------------
	Function	: DDrawInitObject
	Purpose		: Initialise Directdraw objects
	Parameters	: Guid of device
	Returns		: success
	Info		: 
*/

char *softwareString = "Software Rendering";
char *softwareDriver = "majikPR";


unsigned long DDrawInitObject (GUID *guid)
{
	HRESULT		res;
	DDCAPS		ddCaps;
	int i;

	// Create a base DirectDraw object
	DirectDrawEnumerateEx(EnumDDDevices, guid, DDENUM_ATTACHEDSECONDARYDEVICES | DDENUM_DETACHEDSECONDARYDEVICES | DDENUM_NONDISPLAYDEVICES);

	dxDeviceList[dxNumDevices].desc = new char [strlen (softwareString)+1];
	dxDeviceList[dxNumDevices].name = new char [strlen (softwareDriver)+1];
	
	strcpy(dxDeviceList[dxNumDevices].desc, softwareString);
	strcpy(dxDeviceList[dxNumDevices].name, softwareDriver);

	dxDeviceList[dxNumDevices++].guid = (GUID *)-1;


	if (!DialogBox(mdxWinInfo.hInstance, MAKEINTRESOURCE(IDD_VIDEODEVICE),NULL,(DLGPROC)HardwareProc))
		return 0;

	if (rFullscreen)
		ShowCursor(0);

	for (i=0; i<dxNumDevices; i++)
		if ((dxDeviceList[i].idx == selIdx) && ((dxDeviceList[i].caps.dwCaps & DDCAPS_3D) || (dxDeviceList[i].guid == (GUID *)-1)))
			break;
	
	dp ("%s\n%s\n",dxDeviceList[i].name,dxDeviceList[i].desc);

	if (dxDeviceList[i].guid == (GUID *)-1)
	{
		rHardware = 0;

		if ((res = DirectDrawCreateEx(NULL, (LPVOID *)&pDirectDraw7,IID_IDirectDraw7, NULL)) != DD_OK)
		{
			dp("Failed creating DirectDraw object.\n");
			ddShowError(res);
			return 0;
		}
		
//		MPR_Init();
	
	}
	else
	{
		rHardware = 1;

		if ((res = DirectDrawCreateEx(dxDeviceList[i].guid, (LPVOID *)&pDirectDraw7,IID_IDirectDraw7, NULL)) != DD_OK)
		{
			dp("Failed creating DirectDraw object.\n");
			ddShowError(res);
			return 0;
		}
	}

	DDINIT(ddCaps);													
 	if ((res = pDirectDraw7->GetCaps(&ddCaps, NULL))!=DD_OK)			
	{
		dp("Failed getting DirectDraw4 caps.\n");
		ddShowError(res);
		return 0;
	}
	
	// We now have a valid object!
	return 1;
}


/*	--------------------------------------------------------------------------------
	Function	: DDrawCreateSurfaces
	Purpose		: Create all our surfaces
	Parameters	: xRes,yRes & Bitdepth, whether we will want a 3D capable render surface, and whether we would want a zbuffer
	Returns		: success
	Info		: 
*/

unsigned long DDrawCreateSurfaces(HWND window, unsigned long xRes, unsigned long yRes, unsigned long bitDepth, unsigned long want3D, unsigned long zBits)
{
	DDSURFACEDESC2	ddsd;
	HRESULT			res;
	unsigned long	l;
	unsigned long	zMask = 0;

	rBitDepth =	bitDepth;
	rXRes = xRes;
	rYRes = yRes;

	// To run fullscreen - exclusive, ensure bitdepth is nonzero
	if (rFullscreen)
	{
		ShowWindow(mdxWinInfo.hWndMain,SW_SHOWMAXIMIZED);

		// Fullscreen Exclusive
		if ((res = pDirectDraw7->SetCooperativeLevel(window, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE)) != DD_OK)
		{
			dp("Failed setting cooperative level. (Fullscreen Mode)\n");
			ddShowError(res);
			return 0;
		}

		// Set display
		if ((res = pDirectDraw7->SetDisplayMode(rXRes, rYRes, rBitDepth,0,0)) != DD_OK)
		{
			dp("Failed setting display mode. (Fullscreen Mode)\n");
			ddShowError(res);
			return 0;
		}
	
		// Create a primary surface
		DDINIT(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_3DDEVICE | DDSCAPS_PRIMARYSURFACE | DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
		ddsd.dwBackBufferCount = 1;
		if ((res = pDirectDraw7->CreateSurface(&ddsd, &surface[PRIMARY_SRF], NULL))!= DD_OK)
		{
			dp("Failed creating primary surface\n");
			ddShowError(res);
			return 0;
		}
		
		
		DDINIT(ddsd);
		ddsd.ddsCaps.dwCaps =  DDSCAPS_BACKBUFFER | rHardware?DDSCAPS_3DDEVICE:0;
		if ((res = surface[PRIMARY_SRF]->GetAttachedSurface (&ddsd.ddsCaps, &surface[RENDER_SRF])) != DD_OK)
		{
			dp("Failed getting render surface\n");
			ddShowError(res);
			return 0;
		}
	}
	else
	{
			ShowWindow(mdxWinInfo.hWndMain,SW_SHOW);

		if ((res = pDirectDraw7->SetCooperativeLevel(window, DDSCL_NORMAL)) != DD_OK)
		{
			dp("Failed setting cooperative level. (Fullscreen Mode)\n");
			ddShowError(res);
			return 0;
		}

			// Create a primary surface
		DDINIT(ddsd);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
		if ((res = pDirectDraw7->CreateSurface(&ddsd, &surface[PRIMARY_SRF], NULL))!= DD_OK)
		{
			dp("Failed creating primary surface\n");
			ddShowError(res);
			return 0;
		}
	
			// Create a primary surface
		DDINIT(ddsd);
		ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		ddsd.dwWidth  = rXRes;
		ddsd.dwHeight = rYRes; 
		if (rHardware)
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE | DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
		else
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		if ((res = pDirectDraw7->CreateSurface(&ddsd, &surface[RENDER_SRF], NULL))!= DD_OK)
		{
			dp("Failed creating hidden surface\n");
			ddShowError(res);
			return 0;
		}
	
	}

	
	// Get Some info for the surface
	DDINIT(ddsd);
	if ((res = surface[PRIMARY_SRF]->GetSurfaceDesc(&ddsd)) != DD_OK)
	{
		dp("Failed creating primary surface description\n");
		ddShowError(res);
		return 0;
	}

	// Test the green mask to see how many bits it is.
 
	l = (int)ddsd.ddpfPixelFormat.dwGBitMask;
	while ( (!(l&1)) && (l) )
		  l >>= 1;
	r565 = (l != 31);
	
	// Create a render surface
	
	// Create a zbuffer if asked
	if (zBits && rHardware)
	{
		DDINIT(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
		ddsd.dwWidth = rXRes;
		ddsd.dwHeight = rYRes;
		
		ddsd.ddpfPixelFormat.dwSize = sizeof (DDPIXELFORMAT);
		ddsd.ddpfPixelFormat.dwFlags = DDPF_ZBUFFER;
		
		ddsd.ddpfPixelFormat.dwZBufferBitDepth = zBits;
		
		for (;zBits;zMask |= (1<<--zBits));
		
		ddsd.ddpfPixelFormat.dwZBitMask = zMask;
		ddsd.ddpfPixelFormat.dwRGBZBitMask = zMask;
		ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
		
		if ((res = pDirectDraw7->CreateSurface(&ddsd, &surface[ZBUFFER_SRF], NULL)) != DD_OK)
		{
			dp("Error creating Z-buffer surface\n");
			ddShowError(res);
			return 0;
		}
		
		DDrawAttachSurface (RENDER_SRF,ZBUFFER_SRF);
	}

/*	DDINIT(ddsd);
	ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	ddsd.dwWidth  = 640;
	ddsd.dwHeight = 480; 
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
	if ((res = pDirectDraw7->CreateSurface(&ddsd, &surface[SPARE_SRF], NULL))!= DD_OK)
	{
		dp("Failed creating spare surface\n");
		ddShowError(res);
		return 0;
	}
	
*/	
	DDrawSetupWindow (window,TRUE);

	return 1;
}

/*	--------------------------------------------------------------------------------
	Function	: DDrawInitObject
	Purpose		: Initialise Directdraw objects
	Parameters	: Guid of device
	Returns		: success
	Info		: 
*/

unsigned long DDrawShutdown (void)
{
	HRESULT res;
	if ((res = pDirectDraw7->SetCooperativeLevel(rWin, DDSCL_NORMAL)) != DD_OK)
	{
		dp("Failed setting cooperative level. (Fullscreen Mode)\n");
		ddShowError(res);
	}

	// Delete primary (And implicitly the render) surfaces
	surface[PRIMARY_SRF]->Release();
	
	if (surface[SPARE_SRF])
		surface[SPARE_SRF]->Release();
	if (surface[ZBUFFER_SRF])
		surface[ZBUFFER_SRF]->Release();
	pDirectDraw7->Release();
	return 1;
}


/*	--------------------------------------------------------------------------------
	Function	: DDrawAttachSurface
	Purpose		: Attach two surfaces
	Parameters	: A is surface that B attaches to.
	Returns		: success
	Info		: 
*/

unsigned long DDrawAttachSurface(unsigned long srfA, unsigned long srfB)
{
	HRESULT res;
	if ((res = surface[srfA]->AddAttachedSurface(surface[srfB])) != DD_OK)
	{
		dp("Error attaching surface %lu to %lu\n");
		ddShowError(res);
		return FALSE;
	}

	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function	: DDrawSetupWindow
	Purpose		: Setup for widowing
	Parameters	: The window that will contain the view of the primary surface
	Returns		: success
	Info		: 
*/

unsigned long DDrawSetupWindow(HWND window, unsigned long scaled)
{
	HRESULT res;
	
	if (!surface[0])
	{
		dp("Cannot setup window before you create at least one surface");
		return 0;
	}
	
	rWin = window;
	rScale = scaled;
	
	// Make a clipper
	if ((res = pDirectDraw7->CreateClipper (0,&pClipper,NULL)) !=DD_OK)
	{
		dp("Failed creating clipper.\n");
		ddShowError(res);
		return 0;
	}

	// Set one end to the window
	if (pClipper->SetHWnd (0,window)!=DD_OK) 
	{
		dp("Failed setting cliiper to hwnd.\n");
		ddShowError(res);
		return 0;
	}

	// And the other to the surface!
	if (surface[PRIMARY_SRF]->SetClipper (pClipper)!=DD_OK)
	{
		dp("Failed setting clipper to surface.\n");
		ddShowError(res);
		return 0;
	}

	return 1;
}

/*	--------------------------------------------------------------------------------
	Function	: DDrawSetupWindow
	Purpose		: Setup for widowing
	Parameters	: The window that will contain the view of the primary surface
	Returns		: success
	Info		: 
*/

void DDrawFlip(void)
{
	// Flip the back buffer to the primary surface
	if (rFullscreen)
	{
		if (rFlipOK)
			surface[PRIMARY_SRF]->Flip(NULL,DDFLIP_WAIT);
		else
			while (surface[PRIMARY_SRF]->Blt(NULL,surface[RENDER_SRF],NULL,NULL,NULL)!=DD_OK);
	}
	else
	{
		RECT clientR,windowR;

		// Or if we are windowed, copy it.. Since thay won't be attached
		GetClientRect(rWin,&clientR);
		GetWindowRect(rWin,&windowR);
		
		windowR.top+=GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYSIZEFRAME);
		windowR.left+=GetSystemMetrics(SM_CXSIZEFRAME);
			
		// Setup for scaling
		if (rScale)
		{
			windowR.bottom-=GetSystemMetrics(SM_CYSIZEFRAME);
			windowR.right-=GetSystemMetrics(SM_CXSIZEFRAME);
			clientR.right = rXRes;
			clientR.bottom = rYRes;
		}
		else
		{
			windowR.bottom = clientR.bottom+windowR.top;
			windowR.right = clientR.right+windowR.left;
			if (clientR.right>(long)rXRes) clientR.right = (long)rXRes;
			if (clientR.bottom>(long)rYRes) clientR.bottom = (long)rYRes;
		}

		// Blt it, I suppose if we aren't scaling I could used BltFast, but is it worth it? I dont think so
		while (surface[PRIMARY_SRF]->Blt(&windowR,surface[RENDER_SRF],&clientR,NULL,NULL)!=DD_OK);
	}
}

/*	--------------------------------------------------------------------------------
	Function	: ClearSurface
	Purpose		: Clear a surface (Depth or color)
	Parameters	: Surface Number, value to fill with, DDBLT_DEPTHFILL or DDBLT_COLORFILL
	Returns		: success
	Info		: 
*/

void DDrawClearSurface(unsigned long srfN, unsigned long value, unsigned long fillType)
{
	DDBLTFX			m;
	DDINIT(m);

	// Setup, fillDepth and fill color ar currently a union to the same four bytes, but it may not stay that way forever!
	m.dwFillDepth = m.dwFillColor = value;

	// Fill it, innefecient, I would recomend not waiting!
	while (surface[srfN]->Blt(NULL,NULL,NULL,DDBLT_WAIT | fillType,&m)!=DD_OK);	
}

/*	--------------------------------------------------------------------------------
	Function	: CopyDataToSurface
	Purpose		: Copy data (matching the screen format) from a buffer to the screen
	Parameters	: data to copy, surface to copy to
	Returns		: success
	Info		: 
*/
void CopyDataToSurface(void *data, LPDIRECTDRAWSURFACE7 surface)
{
	HRESULT res;
	DDSURFACEDESC2 ddsd;
	DDINIT(ddsd);

	res = surface->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT|DDLOCK_WRITEONLY|DDLOCK_NOSYSLOCK, 0);
	
	if (res == DD_OK)
	{
		long rows = ddsd.dwHeight;
		UCHAR *p = (UCHAR*)ddsd.lpSurface, *q = (UCHAR*)data;

		while (rows--)
		{
			memcpy(p, q, ddsd.dwWidth*2);
			
			p += ddsd.lPitch;
			q += ddsd.dwWidth*2;
		}

		surface->Unlock(NULL);
	}
	else
		ddShowError(res);
}

/*	--------------------------------------------------------------------------------
	Function	: mdxDrawBackdrop
	Purpose		: 
	Parameters	: 
	Returns		: 
	Info		: 
*/
void mdxDrawBackdrop()
{
	DDBLTFX	m; DDINIT(m);
	HRESULT res;
	if (backdrop)
		res = surface[RENDER_SRF]->Blt(NULL, backdrop, NULL, DDBLT_WAIT, &m);
}

void mdxSetBackdropToTex(MDX_TEXENTRY *t)
{
	if (t)
		backdrop = t->surf;
}

/*	--------------------------------------------------------------------------------
	Function	: mdxLoadBackdrop
	Purpose		: 
	Parameters	: 
	Returns		: 
	Info		: 
*/
void mdxLoadBackdrop(const char* filename)
{
	DDSURFACEDESC2 ddsd; DDINIT(ddsd);
	HRESULT res;

	int xDim,yDim;
	int pptr = -1;

	void* data = gelfLoad_BMP((char*)filename,NULL,(void**)&pptr,&xDim,&yDim,NULL,GELF_IFORMAT_16BPP565,GELF_IMAGEDATA_TOPDOWN);

	surface[RENDER_SRF]->GetSurfaceDesc(&ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
	ddsd.dwWidth = xDim;
	ddsd.dwHeight = yDim;
/*
	DDINIT(ddsd.ddpfPixelFormat);
	ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
	ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
*/
	/*	if (rHardware)
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
	else*/
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

	if ((res = pDirectDraw7->CreateSurface(&ddsd, &backdrop, NULL)) != DD_OK)
	{
		dp("Error creating backdrop surface\n");
		ddShowError(res);
		return;
	}

	CopyDataToSurface(data, backdrop);

	free(data);
}

/*	--------------------------------------------------------------------------------
	Function	: mdxFreeBackdrop
	Purpose		: 
	Parameters	: 
	Returns		: 
	Info		: 
*/
void mdxFreeBackdrop()
{
	if (backdrop)
		backdrop->Release();
}

#define REDVAL(x) ((((x>>11)&0x1f) * 0xff)/0x1f)
#define GREENVAL(x) ((((x>>5)&0x3f) * 0xff)/0x3f)
#define BLUEVAL(x) ((((x>>0)&0x1f) * 0xff)/0x1f)
int picnum = 0;

void ScreenShot()
{
	char	filename[MAX_PATH];
	FILE *file;
	short *screen;
	long pitch;
	int x, y;
	unsigned short pixel;
	unsigned char	col;
	unsigned char	line[1280 * 4];
	int	linePos;

	DDSURFACEDESC2		ddsd;
	DDINIT(ddsd);
	



	dp("==================\n");
	dp("Taking screen shot\n");


	sprintf(filename, "c:\\frogshot%04d_%lux%lu.raw", picnum++,rXRes,rYRes);
	file =	fopen(filename, "wb");
	
	if(!file)
	{
		dp("FILEERROR: could not open file:\n");
		return;
	}

	while (surface[RENDER_SRF]->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR,0)!=DD_OK);
	
	screen = (short *)ddsd.lpSurface;
	pitch = ddsd.lPitch/2;

	y = 0;
	while (y<rYRes)
	{
		linePos = 0;
		
		for(x = 0; x < rXRes; x++)
		{
			pixel = screen[x + pitch * y];

			col = REDVAL(pixel);
			line[linePos++] = col;
			col = GREENVAL(pixel);
			line[linePos++] = col;
			col = BLUEVAL(pixel);
			line[linePos++] = col;
		}

		y++;

		fwrite(line, rXRes*3,1,file);	
		
	}

	surface[RENDER_SRF]->Unlock(NULL);	

	fclose(file);	
}


