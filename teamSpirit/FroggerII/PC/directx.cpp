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

long SCREEN_WIDTH=320;	//320
long SCREEN_HEIGHT=240;	//240
long HALF_WIDTH = 160;
long HALF_HEIGHT = 120;
float RES_DIFF = 1;
float RES_DIFF2 = 2;

float fStart = 0.3;
float fEnd = 0.6;

long is565;

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

LPDIRECTDRAW			pDirectDraw;
LPDIRECTDRAWSURFACE		primarySrf;
LPDIRECTDRAWSURFACE		hiddenSrf;
LPDIRECTDRAWSURFACE		srfZBuffer;
LPDIRECT3D2				pDirect3D;
LPDIRECT3DDEVICE2		pDirect3DDevice;
LPDIRECT3DVIEWPORT2		pDirect3DViewport;
LPDIRECTDRAW4			pDirectDraw4;
LPDIRECTDRAWCLIPPER		pClipper;

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

char *ddError2String(HRESULT error);
long isHardware = 1;

int dumpScreen = 0;
int prim = 0;

extern long numFacesDrawn;

GUID guID;

//static GUID     guID;

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

void PrintCaps(DDCAPS *me)
{
	// dwCaps
	dp ("dwCaps ----------------------------------------------------------------------------------------------------------------------------------------------\n");

	if (me->dwCaps & DDCAPS_3D )
		dp ("DDCAPS_3D							Indicates that the display hardware has 3-D acceleration. \n");
	if (me->dwCaps & DDCAPS_ALIGNBOUNDARYDEST )
		dp ("DDCAPS_ALIGNBOUNDARYDEST			Indicates that DirectDraw will support only those overlay destination rectangles with the x-axis aligned to the dwAlignBoundaryDest boundaries of the surface. \n");
	if (me->dwCaps & DDCAPS_ALIGNBOUNDARYSRC )
		dp ("DDCAPS_ALIGNBOUNDARYSRC			Indicates that DirectDraw will support only those overlay source rectangles with the x-axis aligned to the dwAlignBoundarySrc boundaries of the surface. \n");
	if (me->dwCaps & DDCAPS_ALIGNSIZEDEST )
		dp ("DDCAPS_ALIGNSIZEDEST				Indicates that DirectDraw will support only those overlay destination rectangles whose x-axis sizes, in pixels, are dwAlignSizeDest multiples. \n");
	if (me->dwCaps & DDCAPS_ALIGNSIZESRC )
		dp ("DDCAPS_ALIGNSIZESRC				Indicates that DirectDraw will support only those overlay source rectangles whose x-axis sizes, in pixels, are dwAlignSizeSrc multiples. \n");
	if (me->dwCaps & DDCAPS_ALIGNSTRIDE )
		dp ("DDCAPS_ALIGNSTRIDE					Indicates that DirectDraw will create display memory surfaces that have a stride alignment equal to the dwAlignStrideAlign value. \n");
	if (me->dwCaps & DDCAPS_ALPHA )
		dp ("DDCAPS_ALPHA						Indicates that the display hardware supports alpha-only surfaces. (See alpha channel) \n");
	if (me->dwCaps & DDCAPS_BANKSWITCHED )
		dp ("DDCAPS_BANKSWITCHED				Indicates that the display hardware is bank-switched and is potentially very slow at random access to display memory. \n");
	if (me->dwCaps & DDCAPS_BLT )
		dp ("DDCAPS_BLT							Indicates that display hardware is capable of blit operations. \n");
	if (me->dwCaps & DDCAPS_BLTCOLORFILL )
		dp ("DDCAPS_BLTCOLORFILL				Indicates that display hardware is capable of color filling with a blitter. \n");
	if (me->dwCaps & DDCAPS_BLTDEPTHFILL )
		dp ("DDCAPS_BLTDEPTHFILL				Indicates that display hardware is capable of depth filling z-buffers with a blitter. \n");
	if (me->dwCaps & DDCAPS_BLTFOURCC )
		dp ("DDCAPS_BLTFOURCC					Indicates that display hardware is capable of color-space conversions during blit operations. \n");
	if (me->dwCaps & DDCAPS_BLTQUEUE )
		dp ("DDCAPS_BLTQUEUE					Indicates that display hardware is capable of asynchronous blit operations. \n");
	if (me->dwCaps & DDCAPS_BLTSTRETCH )
		dp ("DDCAPS_BLTSTRETCH					Indicates that display hardware is capable of stretching during blit operations. \n");
	if (me->dwCaps & DDCAPS_CANBLTSYSMEM )
		dp ("DDCAPS_CANBLTSYSMEM				Indicates that display hardware is capable of blitting to or from system memory. \n");
	if (me->dwCaps & DDCAPS_CANCLIP )
		dp ("DDCAPS_CANCLIP						Indicates that display hardware is capable of clipping with blitting. \n");
	if (me->dwCaps & DDCAPS_CANCLIPSTRETCHED )
		dp ("DDCAPS_CANCLIPSTRETCHED			Indicates that display hardware is capable of clipping while stretch blitting. \n");
	if (me->dwCaps & DDCAPS_COLORKEY )
		dp ("DDCAPS_COLORKEY					Supports some form of color key in either overlay or blit operations. More specific color key capability information can be found in the dwCKeyCaps member. \n");
	if (me->dwCaps & DDCAPS_COLORKEYHWASSIST )
		dp ("DDCAPS_COLORKEYHWASSIST			Indicates that the color key is partially hardware assisted. This means that other resources (CPU or video memory) might be used. If this bit is not set, full hardware support is in place. \n");
	if (me->dwCaps & DDCAPS_GDI )
		dp ("DDCAPS_GDI							Indicates that display hardware is shared with GDI. \n");
	if (me->dwCaps & DDCAPS_NOHARDWARE )
		dp ("DDCAPS_NOHARDWARE					Indicates that there is no hardware support. \n");
	if (me->dwCaps & DDCAPS_OVERLAY )
		dp ("DDCAPS_OVERLAY						Indicates that display hardware supports overlays. \n");
	if (me->dwCaps & DDCAPS_OVERLAYCANTCLIP )
		dp ("DDCAPS_OVERLAYCANTCLIP				Indicates that display hardware supports overlays but cannot clip them. \n");
	if (me->dwCaps & DDCAPS_OVERLAYFOURCC )
		dp ("DDCAPS_OVERLAYFOURCC				Indicates that overlay hardware is capable of color-space conversions during overlay operations. \n");
	if (me->dwCaps & DDCAPS_OVERLAYSTRETCH )
		dp ("DDCAPS_OVERLAYSTRETCH				Indicates that overlay hardware is capable of stretching. The dwMinOverlayStretch and dwMaxOverlayStretch members contain valid data. \n");
	if (me->dwCaps & DDCAPS_PALETTE )
		dp ("DDCAPS_PALETTE						Indicates that DirectDraw is capable of creating and supporting DirectDrawPalette objects for more surfaces than only the primary surface. \n");
	if (me->dwCaps & DDCAPS_PALETTEVSYNC )
		dp ("DDCAPS_PALETTEVSYNC				Indicates that DirectDraw is capable of updating a palette synchronized with the vertical refresh. \n");
	if (me->dwCaps & DDCAPS_READSCANLINE )
		dp ("DDCAPS_READSCANLINE				Indicates that display hardware is capable of returning the current scan line. \n");
//	if (me->dwCaps & DDCAPS_STEREOVIEW )
//		dp ("DDCAPS_STEREOVIEW					Indicates that display hardware has stereo vision capabilities. \n");
	if (me->dwCaps & DDCAPS_VBI )
		dp ("DDCAPS_VBI							Indicates that display hardware is capable of generating a vertical-blank interrupt. \n");
	if (me->dwCaps & DDCAPS_ZBLTS )
		dp ("DDCAPS_ZBLTS						Supports the use of z-buffers with blit operations. \n");
	if (me->dwCaps & DDCAPS_ZOVERLAYS )
		dp ("DDCAPS_ZOVERLAYS					Supports the use of the IDirectDrawSurface4::UpdateOverlayZOrder method as a z-value for overlays to control their layering. \n");

	// dwCaps2
	dp ("dwCaps2 ----------------------------------------------------------------------------------------------------------------------------------------------\n");

	if (me->dwCaps2 & DDCAPS2_AUTOFLIPOVERLAY )
		dp ("DDCAPS2_AUTOFLIPOVERLAY :			The overlay can be automatically flipped to the next surface in the flip chain each time a video port VSYNC occurs, allowing the video port and the overlay to double buffer the video without CPU overhead. This option is only valid when the surface is receiving data from a video port. If the video port data is non-interlaced or non-interleaved, it will flip on every VSYNC. If the data is being interleaved in memory, it will flip on every other VSYNC. \n");
	if (me->dwCaps2 & DDCAPS2_CANBOBHARDWARE )
		dp ("DDCAPS2_CANBOBHARDWARE :			The overlay hardware can display each field of an interlaced video stream individually. \n");
	if (me->dwCaps2 & DDCAPS2_CANBOBINTERLEAVED )
		dp ("DDCAPS2_CANBOBINTERLEAVED :		The overlay hardware can display each field individually of an interlaced video stream while it is interleaved in memory without causing any artifacts that might normally occur without special hardware support. This option is only valid when the surface is receiving data from a video port and is only valid when the video is zoomed at least two times in the vertical direction. \n");
	if (me->dwCaps2 & DDCAPS2_CANBOBNONINTERLEAVED )
		dp ("DDCAPS2_CANBOBNONINTERLEAVED :		The overlay hardware can display each field individually of an interlaced video stream while it is not interleaved in memory without causing any artifacts that might normally occur without special hardware support. This option is only valid when the surface is receiving data from a video port and is only valid when the video is zoomed at least two times in the vertical direction. \n");
	if (me->dwCaps2 & DDCAPS2_CANCALIBRATEGAMMA )
		dp ("DDCAPS2_CANCALIBRATEGAMMA :		The system has a calibrator installed that can automatically adjust the gamma ramp so that the result will be identical on all systems that have a calibrator. To invoke the calibrator when setting new gamma levels, use the DDSGR_CALIBRATE flag when calling the IDirectDrawGammaControl::SetGammaRamp method. Calibrating gamma ramps incurs some processing overhead, and should not be used frequently. \n");
	if (me->dwCaps2 & DDCAPS2_CANDROPZ16BIT )
		dp ("DDCAPS2_CANDROPZ16BIT :			16-bit RGBZ values can be converted into sixteen-bit RGB values. (The system does not support eight-bit conversions.) \n");
	if (me->dwCaps2 & DDCAPS2_CANFLIPODDEVEN )
		dp ("DDCAPS2_CANFLIPODDEVEN :			The driver is capable of performing odd and even flip operations, as specified by the DDFLIP_ODD and DDFLIP_EVEN flags used with the IDirectDrawSurface4::Flip method. \n");
	if (me->dwCaps2 & DDCAPS2_CANRENDERWINDOWED )
		dp ("DDCAPS2_CANRENDERWINDOWED :		The driver is capable of rendering in windowed mode. \n");
	if (me->dwCaps2 & DDCAPS2_CERTIFIED )
		dp ("DDCAPS2_CERTIFIED :				Indicates that display hardware is certified. \n");
	if (me->dwCaps2 & DDCAPS2_COLORCONTROLPRIMARY )
		dp ("DDCAPS2_COLORCONTROLPRIMARY :		The primary surface contains color controls (for instance, gamma) \n");
	if (me->dwCaps2 & DDCAPS2_COLORCONTROLOVERLAY )
		dp ("DDCAPS2_COLORCONTROLOVERLAY :		The overlay surface contains color controls (such as brightness, sharpness) \n");
	if (me->dwCaps2 & DDCAPS2_COPYFOURCC )
		dp ("DDCAPS2_COPYFOURCC :				Indicates that the driver supports blitting any FOURCC surface to another surface of the same FOURCC. \n");
	if (me->dwCaps2 & DDCAPS2_FLIPINTERVAL )
		dp ("DDCAPS2_FLIPINTERVAL :				Indicates that the driver will respond to the DDFLIP_INTERVAL* flags. (see IDirectDrawSurface4::Flip). \n");
	if (me->dwCaps2 & DDCAPS2_FLIPNOVSYNC )
		dp ("DDCAPS2_FLIPNOVSYNC :				Indicates that the driver will respond to the DDFLIP_NOVSYNC flag (see IDirectDrawSurface4::Flip). \n");
	if (me->dwCaps2 & DDCAPS2_NO2DDURING3DSCENE )
		dp ("DDCAPS2_NO2DDURING3DSCENE :		Indicates that 2-D operations such as IDirectDrawSurface4::Blt and IDirectDrawSurface4::Lock cannot be performed on any surfaces that Direct3D® is using between calls to the IDirect3DDevice3::BeginScene and IDirect3DDevice3::EndScene methods. \n");
	if (me->dwCaps2 & DDCAPS2_NONLOCALVIDMEM )
		dp ("DDCAPS2_NONLOCALVIDMEM :			Indicates that the display driver supports surfaces in non-local video memory. \n");
	if (me->dwCaps2 & DDCAPS2_NONLOCALVIDMEMCAPS )
		dp ("DDCAPS2_NONLOCALVIDMEMCAPS :		Indicates that blit capabilities for non-local video memory surfaces differ from local video memory surfaces. If this flag is present, the DDCAPS2_NONLOCALVIDMEM flag will also be present. \n");
	if (me->dwCaps2 & DDCAPS2_NOPAGELOCKREQUIRED )
		dp ("DDCAPS2_NOPAGELOCKREQUIRED :		DMA blit operations are supported on system memory surfaces that are not page locked. \n");
	if (me->dwCaps2 & DDCAPS2_PRIMARYGAMMA )
		dp ("DDCAPS2_PRIMARYGAMMA :				Supports dynamic gamma ramps for the primary surface. For more information, see Gamma and Color Controls. \n");
	if (me->dwCaps2 & DDCAPS2_VIDEOPORT )
		dp ("DDCAPS2_VIDEOPORT :				Indicates that display hardware supports live video. \n");
	if (me->dwCaps2 & DDCAPS2_WIDESURFACES )
		dp ("DDCAPS2_WIDESURFACES :				Indicates that the display surfaces supports surfaces wider than the primary surface. \n");

	// dwCKeyCaps 
	dp ("dwCKeyCaps ----------------------------------------------------------------------------------------------------------------------------------------------\n");

	if (me->dwCKeyCaps & DDCKEYCAPS_DESTBLT )
		dp ("DDCKEYCAPS_DESTBLT :				Supports transparent blitting with a color key that identifies the replaceable bits of the destination surface for RGB colors. \n");
	if (me->dwCKeyCaps & DDCKEYCAPS_DESTBLTCLRSPACE )
		dp ("DDCKEYCAPS_DESTBLTCLRSPACE :		Supports transparent blitting with a color space that identifies the replaceable bits of the destination surface for RGB colors. \n");
	if (me->dwCKeyCaps & DDCKEYCAPS_DESTBLTCLRSPACEYUV )
		dp ("DDCKEYCAPS_DESTBLTCLRSPACEYUV :	Supports transparent blitting with a color space that identifies the replaceable bits of the destination surface for YUV colors. \n");
	if (me->dwCKeyCaps & DDCKEYCAPS_DESTBLTYUV )
		dp ("DDCKEYCAPS_DESTBLTYUV :			Supports transparent blitting with a color key that identifies the replaceable bits of the destination surface for YUV colors. \n");
	if (me->dwCKeyCaps & DDCKEYCAPS_DESTOVERLAY )
		dp ("DDCKEYCAPS_DESTOVERLAY :			Supports overlaying with color keying of the replaceable bits of the destination surface being overlaid for RGB colors. \n");
	if (me->dwCKeyCaps & DDCKEYCAPS_DESTOVERLAYCLRSPACE )
		dp ("DDCKEYCAPS_DESTOVERLAYCLRSPACE :	Supports a color space as the color key for the destination of RGB colors. \n");
	if (me->dwCKeyCaps & DDCKEYCAPS_DESTOVERLAYCLRSPACEYUV )
		dp ("DDCKEYCAPS_DESTOVERLAYCLRSPACEYUV: Supports a color space as the color key for the destination of YUV colors. \n");
	if (me->dwCKeyCaps & DDCKEYCAPS_DESTOVERLAYONEACTIVE )
		dp ("DDCKEYCAPS_DESTOVERLAYONEACTIVE :	Supports only one active destination color key value for visible overlay surfaces . \n");
	if (me->dwCKeyCaps & DDCKEYCAPS_DESTOVERLAYYUV )
		dp ("DDCKEYCAPS_DESTOVERLAYYUV :		Supports overlaying using color keying of the replaceable bits of the destination surface being overlaid for YUV colors. \n");
	if (me->dwCKeyCaps & DDCKEYCAPS_NOCOSTOVERLAY )
		dp ("DDCKEYCAPS_NOCOSTOVERLAY :			Indicates there are no bandwidth trade-offs for using the color key with an overlay. \n");
	if (me->dwCKeyCaps & DDCKEYCAPS_SRCBLT )
		dp ("DDCKEYCAPS_SRCBLT :				Supports transparent blitting using the color key for the source with this surface for RGB colors. \n");
	if (me->dwCKeyCaps & DDCKEYCAPS_SRCBLTCLRSPACE )
		dp ("DDCKEYCAPS_SRCBLTCLRSPACE :		Supports transparent blitting using a color space for the source with this surface for RGB colors. \n");
	if (me->dwCKeyCaps & DDCKEYCAPS_SRCBLTCLRSPACEYUV )
		dp ("DDCKEYCAPS_SRCBLTCLRSPACEYUV :		Supports transparent blitting using a color space for the source with this surface for YUV colors. \n");
	if (me->dwCKeyCaps & DDCKEYCAPS_SRCBLTYUV )
		dp ("DDCKEYCAPS_SRCBLTYUV :				Supports transparent blitting using the color key for the source with this surface for YUV colors. \n");
	if (me->dwCKeyCaps & DDCKEYCAPS_SRCOVERLAY )
		dp ("DDCKEYCAPS_SRCOVERLAY :			Supports overlaying using the color key for the source with this overlay surface for RGB colors. \n");
	if (me->dwCKeyCaps & DDCKEYCAPS_SRCOVERLAYCLRSPACE )
		dp ("DDCKEYCAPS_SRCOVERLAYCLRSPACE :	Supports overlaying using a color space as the source color key for the overlay surface for RGB colors. \n");
	if (me->dwCKeyCaps & DDCKEYCAPS_SRCOVERLAYCLRSPACEYUV )
		dp ("DDCKEYCAPS_SRCOVERLAYCLRSPACEYUV :	Supports overlaying using a color space as the source color key for the overlay surface for YUV colors. \n");
	if (me->dwCKeyCaps & DDCKEYCAPS_SRCOVERLAYONEACTIVE )
		dp ("DDCKEYCAPS_SRCOVERLAYONEACTIVE :	Supports only one active source color key value for visible overlay surfaces. \n");
	if (me->dwCKeyCaps & DDCKEYCAPS_SRCOVERLAYYUV )
		dp ("DDCKEYCAPS_SRCOVERLAYYUV :			Supports overlaying using the color key for the source with this overlay surface for YUV colors.\n");

	//dwFXCaps 
	dp ("dwFXCaps ----------------------------------------------------------------------------------------------------------------------------------------------\n");

	if (me->dwFXCaps & DDFXCAPS_BLTALPHA )
		dp ("DDFXCAPS_BLTALPHA :				Supports alpha-blended blit operations. \n");
	if (me->dwFXCaps & DDFXCAPS_BLTARITHSTRETCHY )
		dp ("DDFXCAPS_BLTARITHSTRETCHY :		Uses arithmetic operations, rather than pixel-doubling techniques, to stretch and shrink surfaces during a blit operation. Occurs along the y-axis (vertically). \n");
	if (me->dwFXCaps & DDFXCAPS_BLTARITHSTRETCHYN )
		dp ("DDFXCAPS_BLTARITHSTRETCHYN :		Uses arithmetic operations, rather than pixel-doubling techniques, to stretch and shrink surfaces during a blit operation. Occurs along the y-axis (vertically), and works only for integer stretching (×1, ×2, and so on). \n");
	if (me->dwFXCaps & DDFXCAPS_BLTFILTER )
		dp ("DDFXCAPS_BLTFILTER :				Driver can do surface-reconstruction filtering for warped blits. \n");
	if (me->dwFXCaps & DDFXCAPS_BLTMIRRORLEFTRIGHT )
		dp ("DDFXCAPS_BLTMIRRORLEFTRIGHT :		Supports mirroring left to right in a blit operation. \n");
	if (me->dwFXCaps & DDFXCAPS_BLTMIRRORUPDOWN )
		dp ("DDFXCAPS_BLTMIRRORUPDOWN :			Supports mirroring top to bottom in a blit operation. \n");
	if (me->dwFXCaps & DDFXCAPS_BLTROTATION )
		dp ("DDFXCAPS_BLTROTATION :				Supports arbitrary rotation in a blit operation. \n");
	if (me->dwFXCaps & DDFXCAPS_BLTROTATION90 )
		dp ("DDFXCAPS_BLTROTATION90 :			Supports 90-degree rotations in a blit operation. \n");
	if (me->dwFXCaps & DDFXCAPS_BLTSHRINKX )
		dp ("DDFXCAPS_BLTSHRINKX :				Supports arbitrary shrinking of a surface along the x-axis (horizontally). This flag is valid only for blit operations. \n");
	if (me->dwFXCaps & DDFXCAPS_BLTSHRINKXN )
		dp ("DDFXCAPS_BLTSHRINKXN :				Supports integer shrinking (×1, ×2, and so on) of a surface along the x-axis (horizontally). This flag is valid only for blit operations. \n");
	if (me->dwFXCaps & DDFXCAPS_BLTSHRINKY )
		dp ("DDFXCAPS_BLTSHRINKY :				Supports arbitrary shrinking of a surface along the y-axis (vertically). This flag is valid only for blit operations. \n");
	if (me->dwFXCaps & DDFXCAPS_BLTSHRINKYN )
		dp ("DDFXCAPS_BLTSHRINKYN :				Supports integer shrinking (×1, ×2, and so on) of a surface along the y-axis (vertically). This flag is valid only for blit operations. \n");
	if (me->dwFXCaps & DDFXCAPS_BLTSTRETCHX )
		dp ("DDFXCAPS_BLTSTRETCHX :				Supports arbitrary stretching of a surface along the x-axis (horizontally). This flag is valid only for blit operations. \n");
	if (me->dwFXCaps & DDFXCAPS_BLTSTRETCHXN )
		dp ("DDFXCAPS_BLTSTRETCHXN :			Supports integer stretching (×1, ×2, and so on) of a surface along the x-axis (horizontally). This flag is valid only for blit operations. \n");
	if (me->dwFXCaps & DDFXCAPS_BLTSTRETCHY )
		dp ("DDFXCAPS_BLTSTRETCHY :				Supports arbitrary stretching of a surface along the y-axis (vertically). This flag is valid only for blit operations. \n");
	if (me->dwFXCaps & DDFXCAPS_BLTSTRETCHYN )
		dp ("DDFXCAPS_BLTSTRETCHYN :			Supports integer stretching (×1, ×2, and so on) of a surface along the y-axis (vertically). This flag is valid only for blit operations. \n");
//	if (me->dwFXCaps & DDFXCAPS_BLTTRANSFORM )
//		dp ("DDFXCAPS_BLTTRANSFORM :			Supports geometric transformations (or warps) for blitted sprites. Transformations are not currently supported for explicit blit operations. \n");
	if (me->dwFXCaps & DDFXCAPS_OVERLAYALPHA )
		dp ("DDFXCAPS_OVERLAYALPHA :			Supports alpha blending for overlay surfaces. \n");
	if (me->dwFXCaps & DDFXCAPS_OVERLAYARITHSTRETCHY )
		dp ("DDFXCAPS_OVERLAYARITHSTRETCHY :	Uses arithmetic operations, rather than pixel-doubling techniques, to stretch and shrink overlay surfaces. Occurs along the y-axis (vertically). \n");
	if (me->dwFXCaps & DDFXCAPS_OVERLAYARITHSTRETCHYN )
		dp ("DDFXCAPS_OVERLAYARITHSTRETCHYN :	Uses arithmetic operations, rather than pixel-doubling techniques, to stretch and shrink overlay surfaces. Occurs along the y-axis (vertically), and works only for integer stretching (×1, ×2, and so on). \n");
	if (me->dwFXCaps & DDFXCAPS_OVERLAYFILTER )
		dp ("DDFXCAPS_OVERLAYFILTER :			Supports surface-reconstruction filtering for warped overlay sprites. Filtering is not currently supported for explicitly displayed overlay surfaces (those displayed with calls to IDirectDrawSurface4::UpdateOverlay). \n");
	if (me->dwFXCaps & DDFXCAPS_OVERLAYMIRRORLEFTRIGHT )
		dp ("DDFXCAPS_OVERLAYMIRRORLEFTRIGHT :	Supports mirroring of overlays across the vertical axis. \n");
	if (me->dwFXCaps & DDFXCAPS_OVERLAYMIRRORUPDOWN )
		dp ("DDFXCAPS_OVERLAYMIRRORUPDOWN :		Supports mirroring of overlays across the horizontal axis. \n");
	if (me->dwFXCaps & DDFXCAPS_OVERLAYSHRINKX )
		dp ("DDFXCAPS_OVERLAYSHRINKX :			Supports arbitrary shrinking of a surface along the x-axis (horizontally). This flag is valid only for DDSCAPS_OVERLAY surfaces. This flag indicates only the capabilities of a surface; it does not indicate that shrinking is available. \n");
	if (me->dwFXCaps & DDFXCAPS_OVERLAYSHRINKXN )
		dp ("DDFXCAPS_OVERLAYSHRINKXN :			Supports integer shrinking (×1, ×2, and so on) of a surface along the x-axis (horizontally). This flag is valid only for DDSCAPS_OVERLAY surfaces. This flag indicates only the capabilities of a surface; it does not indicate that shrinking is available. \n");
	if (me->dwFXCaps & DDFXCAPS_OVERLAYSHRINKY )
		dp ("DDFXCAPS_OVERLAYSHRINKY :			Supports arbitrary shrinking of a surface along the y-axis (vertically). This flag is valid only for DDSCAPS_OVERLAY surfaces. This flag indicates only the capabilities of a surface; it does not indicate that shrinking is available. \n");
	if (me->dwFXCaps & DDFXCAPS_OVERLAYSHRINKYN )
		dp ("DDFXCAPS_OVERLAYSHRINKYN :			Supports integer shrinking (×1, ×2, and so on) of a surface along the y-axis (vertically). This flag is valid only for DDSCAPS_OVERLAY surfaces. This flag indicates only the capabilities of a surface; it does not indicate that shrinking is available. \n");
	if (me->dwFXCaps & DDFXCAPS_OVERLAYSTRETCHX )
		dp ("DDFXCAPS_OVERLAYSTRETCHX :			Supports arbitrary stretching of a surface along the x-axis (horizontally). This flag is valid only for DDSCAPS_OVERLAY surfaces. This flag indicates only the capabilities of a surface; it does not indicate that stretching is available. \n");
	if (me->dwFXCaps & DDFXCAPS_OVERLAYSTRETCHXN )
		dp ("DDFXCAPS_OVERLAYSTRETCHXN :		Supports integer stretching (×1, ×2, and so on) of a surface along the x-axis (horizontally). This flag is valid only for DDSCAPS_OVERLAY surfaces. This flag indicates only the capabilities of a surface; it does not indicate that stretching is available. \n");
	if (me->dwFXCaps & DDFXCAPS_OVERLAYSTRETCHY )
		dp ("DDFXCAPS_OVERLAYSTRETCHY :			Supports arbitrary stretching of a surface along the y-axis (vertically). This flag is valid only for DDSCAPS_OVERLAY surfaces. This flag indicates only the capabilities of a surface; it does not indicate that stretching is available. \n");
	if (me->dwFXCaps & DDFXCAPS_OVERLAYSTRETCHYN )
		dp ("DDFXCAPS_OVERLAYSTRETCHYN :		Supports integer stretching (×1, ×2, and so on) of a surface along the y-axis (vertically). This flag is valid only for DDSCAPS_OVERLAY surfaces. This flag indicates only the capabilities of a surface; it does not indicate that stretching is available. \n");
//	if (me->dwFXCaps & DDFXCAPS_OVERLAYTRANSFORM )
//		dp ("DDFXCAPS_OVERLAYTRANSFORM :		Supports geometric transformations (or warps) for overlay sprites. Transformations are not currently supported for explicitly displayed overlay surfaces (those displayed with calls to IDirectDrawSurface4::UpdateOverlay). \n");

	//dwFXAlphaCaps
	dp ("dwFXAlphaCaps ----------------------------------------------------------------------------------------------------------------------------------------------\n");

	if (me->dwFXAlphaCaps & DDFXALPHACAPS_BLTALPHAEDGEBLEND )
		dp ("DDFXALPHACAPS_BLTALPHAEDGEBLEND :		Supports alpha blending around the edge of a source color-keyed surface. Used for blit operations. \n");
	if (me->dwFXAlphaCaps & DDFXALPHACAPS_BLTALPHAPIXELS )
		dp ("DDFXALPHACAPS_BLTALPHAPIXELS :			Supports alpha information in pixel format. The bit depth of alpha information in the pixel format can be 1, 2, 4, or 8. The alpha value becomes more opaque as the alpha value increases. Regardless of the depth of the alpha information, 0 is always the fully transparent value. Used for blit operations. \n");
	if (me->dwFXAlphaCaps & DDFXALPHACAPS_BLTALPHAPIXELSNEG )
		dp ("DDFXALPHACAPS_BLTALPHAPIXELSNEG :		Supports alpha information in pixel format. The bit depth of alpha information in the pixel format can be 1, 2, 4, or 8. The alpha value becomes more transparent as the alpha value increases. Regardless of the depth of the alpha information, 0 is always the fully opaque value. This flag can be used only if DDCAPS_ALPHA is set. Used for blit operations. \n");
	if (me->dwFXAlphaCaps & DDFXALPHACAPS_BLTALPHASURFACES )
		dp ("DDFXALPHACAPS_BLTALPHASURFACES :		Supports alpha-only surfaces. The bit depth of an alpha-only surface can be 1, 2, 4, or 8. The alpha value becomes more opaque as the alpha value increases. Regardless of the depth of the alpha information, 0 is always the fully transparent value. Used for blit operations. \n");
	if (me->dwFXAlphaCaps & DDFXALPHACAPS_BLTALPHASURFACESNEG )
		dp ("DDFXALPHACAPS_BLTALPHASURFACESNEG :	Indicates that the alpha channel becomes more transparent as the alpha value increases. The depth of the alpha channel data can be 1, 2, 4, or 8. Regardless of the depth of the alpha information, 0 is always the fully opaque value. This flag can be set only if DDCAPS_ALPHA has been set. Used for blit operations. \n");
	if (me->dwFXAlphaCaps & DDFXALPHACAPS_OVERLAYALPHAEDGEBLEND )
		dp ("DDFXALPHACAPS_OVERLAYALPHAEDGEBLEND :	Supports alpha blending around the edge of a source color-keyed surface. Used for overlays. \n");
	if (me->dwFXAlphaCaps & DDFXALPHACAPS_OVERLAYALPHAPIXELS )
		dp ("DDFXALPHACAPS_OVERLAYALPHAPIXELS :		Supports alpha information in pixel format. The bit depth of alpha information in pixel format can be 1, 2, 4, or 8. The alpha value becomes more opaque as the alpha value increases. Regardless of the depth of the alpha information, 0 is always the fully transparent value. Used for overlays. \n");
	if (me->dwFXAlphaCaps & DDFXALPHACAPS_OVERLAYALPHAPIXELSNEG )
		dp ("DDFXALPHACAPS_OVERLAYALPHAPIXELSNEG :	Supports alpha information in pixel format. The bit depth of alpha information in pixel format can be 1, 2, 4, or 8. The alpha value becomes more transparent as the alpha value increases. Regardless of the depth of the alpha information, 0 is always the fully opaque value. This flag can be used only if DDCAPS_ALPHA has been set. Used for overlays. \n");
	if (me->dwFXAlphaCaps & DDFXALPHACAPS_OVERLAYALPHASURFACES )
		dp ("DDFXALPHACAPS_OVERLAYALPHASURFACES :	Supports alpha-only surfaces. The bit depth of an alpha-only surface can be 1, 2, 4, or 8. The alpha value becomes more opaque as the alpha value increases. Regardless of the depth of the alpha information, 0 is always the fully transparent value. Used for overlays. \n");
	if (me->dwFXAlphaCaps & DDFXALPHACAPS_OVERLAYALPHASURFACESNEG )
		dp ("DDFXALPHACAPS_OVERLAYALPHASURFACESNEG: Indicates that the alpha channel becomes more transparent as the alpha value increases. The depth of the alpha channel data can be 1, 2, 4, or 8. Regardless of the depth of the alpha information, 0 is always the fully opaque value. This flag can be used only if DDCAPS_ALPHA has been set. Used for overlays. \n");

	//dwPalCaps 
	dp ("dwPalCaps----------------------------------------------------------------------------------------------------------------------------------------------\n");

	if (me->dwPalCaps & DDPCAPS_1BIT)
		dp ("DDPCAPS_1BIT:						Supports palettes that contain 1 bit color entries (two colors). \n");
	if (me->dwPalCaps & DDPCAPS_2BIT)
		dp ("DDPCAPS_2BIT:						Supports palettes that contain 2 bit color entries (four colors). \n");
	if (me->dwPalCaps & DDPCAPS_4BIT)
		dp ("DDPCAPS_4BIT:						Supports palettes that contain 4 bit color entries (16 colors). \n");
	if (me->dwPalCaps & DDPCAPS_8BIT)
		dp ("DDPCAPS_8BIT:						Supports palettes that contain 8 bit color entries (256 colors). \n");
	if (me->dwPalCaps & DDPCAPS_8BITENTRIES)
		dp ("DDPCAPS_8BITENTRIES:				Specifies an index to an 8-bit color index. This field is valid only when used with the DDPCAPS_1BIT, DDPCAPS_2BIT, or DDPCAPS_4BIT capability and when the target surface is in 8 bits per pixel (bpp). Each color entry is 1 byte long and is an index to an 8-bpp palette on the destination surface. \n");
	if (me->dwPalCaps & DDPCAPS_ALPHA)
		dp ("DDPCAPS_ALPHA:						Supports palettes that include an alpha component. For alpha-capable palettes, the peFlags member of for each PALETTEENTRY structure the palette contains is to be interpreted as a single 8-bit alpha value (in addition to the color data in the peRed, peGreen, and peBlue members). A palette created with this flag can only be attached to a texture surface. \n");
	if (me->dwPalCaps & DDPCAPS_ALLOW256)
		dp ("DDPCAPS_ALLOW256:					Supports palettes that can have all 256 entries defined. \n");
	if (me->dwPalCaps & DDPCAPS_PRIMARYSURFACE)
		dp ("DDPCAPS_PRIMARYSURFACE:			Indicates that the palette is attached to the primary surface. Changing the palette has an immediate effect on the display unless the DDPCAPS_VSYNC capability is specified and supported. \n");
	if (me->dwPalCaps & DDPCAPS_PRIMARYSURFACELEFT)
		dp ("DDPCAPS_PRIMARYSURFACELEFT:		Indicates that the palette is attached to the primary surface on the left. Changing the palette has an immediate effect on the display unless the DDPCAPS_VSYNC capability is specified and supported. \n");
	if (me->dwPalCaps & DDPCAPS_VSYNC)
		dp ("DDPCAPS_VSYNC:						Indicates that the palette can be modified synchronously with the monitor's refresh rate. \n");

}


char *ddError2String(HRESULT error)
{
    switch(error)
		{
        case DD_OK:
            return "DD_OK:			No error.\0";

        case DDERR_ALREADYINITIALIZED:
			return "DDERR_ALREADYINITIALIZED:			This object is already initialized.\0";

        case DDERR_BLTFASTCANTCLIP:
            return "DDERR_BLTFASTCANTCLIP:				Return if a clipper object is attached to the source surface passed into a BltFast call.\0";

        case DDERR_CANNOTATTACHSURFACE:
            return "DDERR_CANNOTATTACHSURFACE:			This surface can not be attached to the requested surface.\0";

        case DDERR_CANNOTDETACHSURFACE:
            return "DDERR_CANNOTDETACHSURFACE:			This surface can not be detached from the requested surface.\0";

        case DDERR_CANTCREATEDC:
            return "DDERR_CANTCREATEDC:					Windows can not create any more DCs.\0";

        case DDERR_CANTDUPLICATE:
            return "DDERR_CANTDUPLICATE:				Can't duplicate primary & 3D surfaces, or surfaces that are implicitly created.\0";

        case DDERR_CLIPPERISUSINGHWND:
            return "DDERR_CLIPPERISUSINGHWND:			An attempt was made to set a cliplist for a clipper object that is already monitoring an hwnd.\0";

        case DDERR_COLORKEYNOTSET:
            return "DDERR_COLORKEYNOTSET:				No src color key specified for this operation.\0";

        case DDERR_CURRENTLYNOTAVAIL:
            return "DDERR_CURRENTLYNOTAVAIL:			Support is currently not available.\0";

        case DDERR_DIRECTDRAWALREADYCREATED:
            return "DDERR_DIRECTDRAWALREADYCREATED:		A DirectDraw object representing this driver has already been created for this process.\0";

        case DDERR_EXCEPTION:
            return "DDERR_EXCEPTION:					An exception was encountered while performing the requested operation.\0";

        case DDERR_EXCLUSIVEMODEALREADYSET:
            return "DDERR_EXCLUSIVEMODEALREADYSET:		An attempt was made to set the cooperative level when it was already set to exclusive.\0";

        case DDERR_GENERIC:
            return "DDERR_GENERIC:						Generic failure.\0";

        case DDERR_HEIGHTALIGN:
            return "DDERR_HEIGHTALIGN:					Height of rectangle provided is not a multiple of reqd alignment.\0";

        case DDERR_HWNDALREADYSET:
            return "DDERR_HWNDALREADYSET:				The CooperativeLevel HWND has already been set. It can not be reset while the process has surfaces or palettes created.\0";

        case DDERR_HWNDSUBCLASSED:
            return "DDERR_HWNDSUBCLASSED:				HWND used by DirectDraw CooperativeLevel has been subclassed, this prevents DirectDraw from restoring state.\0";

        case DDERR_IMPLICITLYCREATED:
            return "DDERR_IMPLICITLYCREATED:			This surface can not be restored because it is an implicitly created surface.\0";

        case DDERR_INCOMPATIBLEPRIMARY:
            return "DDERR_INCOMPATIBLEPRIMARY:			Unable to match primary surface creation request with existing primary surface.\0";

        case DDERR_INVALIDCAPS:
            return "DDERR_INVALIDCAPS:					One or more of the caps bits passed to the callback are incorrect.\0";

        case DDERR_INVALIDCLIPLIST:
            return "DDERR_INVALIDCLIPLIST:				DirectDraw does not support the provided cliplist.\0";

        case DDERR_INVALIDDIRECTDRAWGUID:
            return "DDERR_INVALIDDIRECTDRAWGUID:		The GUID passed to DirectDrawCreate is not a valid DirectDraw driver identifier.\0";

        case DDERR_INVALIDMODE:
            return "DDERR_INVALIDMODE:					DirectDraw does not support the requested mode.\0";

        case DDERR_INVALIDOBJECT:
            return "DDERR_INVALIDOBJECT:				DirectDraw received a pointer that was an invalid DIRECTDRAW object.\0";

        case DDERR_INVALIDPARAMS:
            return "DDERR_INVALIDPARAMS:				One or more of the parameters passed to the function are incorrect.\0";

        case DDERR_INVALIDPIXELFORMAT:
            return "DDERR_INVALIDPIXELFORMAT:			The pixel format was invalid as specified.\0";

        case DDERR_INVALIDPOSITION:
            return "DDERR_INVALIDPOSITION:				Returned when the position of the overlay on the destination is no longer legal for that destination.\0";

        case DDERR_INVALIDRECT:
            return "DDERR_INVALIDRECT:					Rectangle provided was invalid.\0";

        case DDERR_LOCKEDSURFACES:
            return "DDERR_LOCKEDSURFACES:				Operation could not be carried out because one or more surfaces are locked.\0";

        case DDERR_NO3D:
            return "DDERR_NO3D:							There is no 3D present.\0";

        case DDERR_NOALPHAHW:
            return "DDERR_NOALPHAHW:					Operation could not be carried out because there is no alpha accleration hardware present or available.\0";

        case DDERR_NOBLTHW:
            return "DDERR_NOBLTHW:						No blitter hardware present.\0";

        case DDERR_NOCLIPLIST:
            return "DDERR_NOCLIPLIST:					No cliplist available.\0";

        case DDERR_NOCLIPPERATTACHED:
            return "DDERR_NOCLIPPERATTACHED:			No clipper object attached to surface object.\0";

        case DDERR_NOCOLORCONVHW:
            return "DDERR_NOCOLORCONVHW:				Operation could not be carried out because there is no color conversion hardware present or available.\0";

        case DDERR_NOCOLORKEY:
            return "DDERR_NOCOLORKEY:					Surface doesn't currently have a color key\0";

        case DDERR_NOCOLORKEYHW:
            return "DDERR_NOCOLORKEYHW:					Operation could not be carried out because there is no hardware support of the destination color key.\0";

        case DDERR_NOCOOPERATIVELEVELSET:
            return "DDERR_NOCOOPERATIVELEVELSET:		Create function called without DirectDraw object method SetCooperativeLevel being called.\0";

        case DDERR_NODC:
            return "DDERR_NODC:							No DC was ever created for this surface.\0";

        case DDERR_NODDROPSHW:
            return "DDERR_NODDROPSHW:					No DirectDraw ROP hardware.\0";

        case DDERR_NODIRECTDRAWHW:
            return "DDERR_NODIRECTDRAWHW:				A hardware-only DirectDraw object creation was attempted but the driver did not support any hardware.\0";

        case DDERR_NOEMULATION:
            return "DDERR_NOEMULATION:					Software emulation not available.\0";

        case DDERR_NOEXCLUSIVEMODE:
            return "DDERR_NOEXCLUSIVEMODE:				Operation requires the application to have exclusive mode but the application does not have exclusive mode.\0";

        case DDERR_NOFLIPHW:
            return "DDERR_NOFLIPHW:						Flipping visible surfaces is not supported.\0";

        case DDERR_NOGDI:
            return "DDERR_NOGDI:						There is no GDI present.\0";

        case DDERR_NOHWND:
            return "DDERR_NOHWND:						Clipper notification requires an HWND or no HWND has previously been set as the CooperativeLevel HWND.\0";

        case DDERR_NOMIRRORHW:
            return "DDERR_NOMIRRORHW:					Operation could not be carried out because there is no hardware present or available.\0";

        case DDERR_NOOVERLAYDEST:
            return "DDERR_NOOVERLAYDEST:				Returned when GetOverlayPosition is called on an overlay that UpdateOverlay has never been called on to establish a destination.\0";

        case DDERR_NOOVERLAYHW:
            return "DDERR_NOOVERLAYHW:					Operation could not be carried out because there is no overlay hardware present or available.\0";

        case DDERR_NOPALETTEATTACHED:
            return "DDERR_NOPALETTEATTACHED:			No palette object attached to this surface.\0";

        case DDERR_NOPALETTEHW:
            return "DDERR_NOPALETTEHW:					No hardware support for 16 or 256 color palettes.\0";

        case DDERR_NORASTEROPHW:
            return "DDERR_NORASTEROPHW:					Operation could not be carried out because there is no appropriate raster op hardware present or available.\0";

        case DDERR_NOROTATIONHW:
            return "DDERR_NOROTATIONHW:					Operation could not be carried out because there is no rotation hardware present or available.\0";

        case DDERR_NOSTRETCHHW:
            return "DDERR_NOSTRETCHHW:					Operation could not be carried out because there is no hardware support for stretching.\0";

        case DDERR_NOT4BITCOLOR:
            return "DDERR_NOT4BITCOLOR:					DirectDrawSurface is not in 4 bit color palette and the requested operation requires 4 bit color palette.\0";

        case DDERR_NOT4BITCOLORINDEX:
            return "DDERR_NOT4BITCOLORINDEX:			DirectDrawSurface is not in 4 bit color index palette and the requested operation requires 4 bit color index palette.\0";

        case DDERR_NOT8BITCOLOR:
            return "DDERR_NOT8BITCOLOR:					DirectDrawSurface is not in 8 bit color mode and the requested operation requires 8 bit color.\0";

        case DDERR_NOTAOVERLAYSURFACE:
            return "DDERR_NOTAOVERLAYSURFACE:			Returned when an overlay member is called for a non-overlay surface.\0";

        case DDERR_NOTEXTUREHW:
            return "DDERR_NOTEXTUREHW:					Operation could not be carried out because there is no texture mapping hardware present or available.\0";

        case DDERR_NOTFLIPPABLE:
            return "DDERR_NOTFLIPPABLE:					An attempt has been made to flip a surface that is not flippable.\0";

        case DDERR_NOTFOUND:
            return "DDERR_NOTFOUND:						Requested item was not found.\0";

        case DDERR_NOTLOCKED:
            return "DDERR_NOTLOCKED:					Surface was not locked.  An attempt to unlock a surface that was not locked at all, or by this process, has been attempted.\0";

        case DDERR_NOTPALETTIZED:
            return "DDERR_NOTPALETTIZED:				The surface being used is not a palette-based surface.\0";

        case DDERR_NOVSYNCHW:
            return "DDERR_NOVSYNCHW:					Operation could not be carried out because there is no hardware support for vertical blank synchronized operations.\0";

        case DDERR_NOZBUFFERHW:
            return "DDERR_NOZBUFFERHW:					Operation could not be carried out because there is no hardware support for zbuffer blitting.\0";

        case DDERR_NOZOVERLAYHW:
            return "DDERR_NOZOVERLAYHW:					Overlay surfaces could not be z layered based on their BltOrder because the hardware does not support z layering of overlays.\0";

        case DDERR_OUTOFCAPS:
            return "DDERR_OUTOFCAPS:					The hardware needed for the requested operation has already been allocated.\0";

        case DDERR_OUTOFMEMORY:
            return "DDERR_OUTOFMEMORY:					DirectDraw does not have enough memory to perform the operation.\0";

        case DDERR_OUTOFVIDEOMEMORY:
            return "DDERR_OUTOFVIDEOMEMORY:				DirectDraw does not have enough memory to perform the operation.\0";

        case DDERR_OVERLAYCANTCLIP:
            return "DDERR_OVERLAYCANTCLIP:				The hardware does not support clipped overlays.\0";

        case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
            return "DDERR_OVERLAYCOLORKEYONLYONEACTIVE:	Can only have ony color key active at one time for overlays.\0";

        case DDERR_OVERLAYNOTVISIBLE:
            return "DDERR_OVERLAYNOTVISIBLE:			Returned when GetOverlayPosition is called on a hidden overlay.\0";

        case DDERR_PALETTEBUSY:
            return "DDERR_PALETTEBUSY:					Access to this palette is being refused because the palette is already locked by another thread.\0";

        case DDERR_PRIMARYSURFACEALREADYEXISTS:
            return "DDERR_PRIMARYSURFACEALREADYEXISTS:	This process already has created a primary surface.\0";

        case DDERR_REGIONTOOSMALL:	
            return "DDERR_REGIONTOOSMALL:				Region passed to Clipper::GetClipList is too small.\0";

        case DDERR_SURFACEALREADYATTACHED:
            return "DDERR_SURFACEALREADYATTACHED:		This surface is already attached to the surface it is being attached to.\0";

        case DDERR_SURFACEALREADYDEPENDENT:
            return "DDERR_SURFACEALREADYDEPENDENT:		This surface is already a dependency of the surface it is being made a dependency of.\0";

        case DDERR_SURFACEBUSY:
            return "DDERR_SURFACEBUSY:					Access to this surface is being refused because the surface is already locked by another thread.\0";

        case DDERR_SURFACEISOBSCURED:
            return "DDERR_SURFACEISOBSCURED:			Access to surface refused because the surface is obscured.\0";

        case DDERR_SURFACELOST:
            return "DDERR_SURFACELOST:					Access to this surface is being refused because the surface memory is gone. The DirectDrawSurface object representing this surface should have Restore called on it.\0";

        case DDERR_SURFACENOTATTACHED:
            return "DDERR_SURFACENOTATTACHED:			The requested surface is not attached.\0";

        case DDERR_TOOBIGHEIGHT:
            return "DDERR_TOOBIGHEIGHT:					Height requested by DirectDraw is too large.\0";

        case DDERR_TOOBIGSIZE:
            return "DDERR_TOOBIGSIZE:					Size requested by DirectDraw is too large, but the individual height and width are OK.\0";

        case DDERR_TOOBIGWIDTH:
            return "DDERR_TOOBIGWIDTH:					Width requested by DirectDraw is too large.\0";

        case DDERR_UNSUPPORTED:
            return "DDERR_UNSUPPORTED:					Action not supported.\0";

        case DDERR_UNSUPPORTEDFORMAT:
            return "DDERR_UNSUPPORTEDFORMAT:			FOURCC format requested is unsupported by DirectDraw.\0";

        case DDERR_UNSUPPORTEDMASK:
            return "DDERR_UNSUPPORTEDMASK:				Bitmask in the pixel format requested is unsupported by DirectDraw.\0";

        case DDERR_VERTICALBLANKINPROGRESS:
            return "DDERR_VERTICALBLANKINPROGRESS:		Vertical blank is in progress.\0";

        case DDERR_WASSTILLDRAWING:
            return "DDERR_WASSTILLDRAWING:				Informs DirectDraw that the previous Blt which is transfering information to or from this Surface is incomplete.\0";

        case DDERR_WRONGMODE:
            return "DDERR_WRONGMODE:					This surface can not be restored because it was created in a different mode.\0";

        case DDERR_XALIGN:
            return "DDERR_XALIGN:						Rectangle provided was not horizontally aligned on required boundary.\0";

        default:
            return "Unrecognized error value.\0";
	    }
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


/*BOOL CALLBACK DSEnumProc( LPGUID lpGUID, LPSTR lpszDesc,
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
    }*/




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
	
	if ((res = pDirectDraw->CreateSurface(&ddsd, &hiddenSrf, NULL)) != DD_OK)
	{
		dp("Error creating backbuffer: %s\n", ddError2String(res));
		return FALSE;
	}

	if (!winMode)
		if ((res = primarySrf->AddAttachedSurface(hiddenSrf)) != DD_OK)
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
		if ((res = pDirectDraw->CreateSurface(&ddsd, &srfZBuffer, NULL)) != DD_OK)
		{
			dp("Error creating Z-buffer: %s\n", ddError2String(res));
			return FALSE;
		}
		if ((res = hiddenSrf->AddAttachedSurface(srfZBuffer)) != DD_OK)
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
	

	if (winMode)
	{
		if (pDirectDraw->CreateClipper (0,&pClipper,NULL))
			return FALSE;
	
		if (pClipper->SetHWnd (0,win)!=DD_OK) 
			return FALSE;
	
		if (primarySrf->SetClipper (pClipper)!=DD_OK)
			return FALSE;
	}

	memset (&sdesc,0,sizeof (sdesc));
	sdesc.dwSize=sizeof (sdesc);

	primarySrf->GetSurfaceDesc(&sdesc);
	l = (int)sdesc.ddpfPixelFormat.dwGBitMask;
	while ((l&1) == 0)
		  l >>= 1;
	
	a565Card = !(l == 31);

	runHardware = hardware;

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
	
	if (!runHardware)
	{
		long i,j;
		DDSURFACEDESC ddsd;
	
		DDINIT(ddsd);
		while (hiddenSrf->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR,0)!=DD_OK);
	
		for (i=0,j=0; i<SCREEN_HEIGHT*(ddsd.lPitch/2); i+=(ddsd.lPitch/2),j+=SCREEN_WIDTH)
			memcpy (&((short *)ddsd.lpSurface)[i],&screen[j],SCREEN_WIDTH*2);
	
		hiddenSrf->Unlock(ddsd.lpSurface);
	}

	// Flip the back buffer to the primary surface
	if (!winMode)
		primarySrf->Flip(NULL,DDFLIP_WAIT);
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
			while (primarySrf->Blt(&windowR,hiddenSrf,&clientR,NULL,NULL)!=DD_OK);
		
		}
		else
		{
			windowR.top+=GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYSIZEFRAME);
			windowR.left+=GetSystemMetrics(SM_CXSIZEFRAME);

			windowR.bottom-=GetSystemMetrics(SM_CYSIZEFRAME);
			windowR.right-=GetSystemMetrics(SM_CXSIZEFRAME);
			
			clientR.right = SCREEN_WIDTH;
			clientR.bottom = SCREEN_HEIGHT;
			while (primarySrf->Blt(&windowR,hiddenSrf,&clientR,NULL,NULL)!=DD_OK);
		
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
		while (hiddenSrf->Blt(NULL,NULL,NULL,DDBLT_WAIT | DDBLT_COLORFILL,&m)!=DD_OK);
	
		DDINIT(m);
		m.dwFillDepth = -1;//D3DRGB(0,0,0);
		while (srfZBuffer->Blt(NULL,NULL,NULL,DDBLT_WAIT | DDBLT_DEPTHFILL,&m)!=DD_OK);
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
	BlankFrame();
	
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
			/*| D3DDP_WAIT*/)!=D3D_OK)
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
	unsigned char line [ 480 * 4 ];

	char header[] =	   {0x42,0x4D,0x36,0x84,0x03,0x00,0x00,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00,
						0x00,0x00,0x40,0x01,0x00,0x00,0x1E0,0x00,0x00,0x00,0x01,0x00,0x18,0x00,0x00,0x00,
						0x00,0x00,0x00,0x84,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00};

	sprintf ( fileName, "c:\\pc%04d.raw", picnum++);

	fp = fopen ( fileName, "w" );
	if ( fp == NULL )
		return;
	// ENDIF

	//fwrite ( header, sizeof ( header ), 1, fp );

	y = 0;
	for ( y = 0; y < 480; y++ )
	{
		linePos = 0;
		for(x = 0; x < 640; x++)
		{
			pixel = ((short*)ddsd.lpSurface)[x + y * (ddsd.lPitch/2)];

/*			col = ((pixel>>1)<<3)&0xFF;
			line[linePos++] = col;
			col = ((pixel>>6)<<3)&0xFF;
			line[linePos++] = col;
			col = ((pixel>>11)<<3)&0xFF;
			line[linePos++] = col;*/

			col = (int)(255.0/31.0 * ((pixel >> 11) & 0x1f));
			line[linePos++] = col;
		
			col = (int)(255.0/63.0 * ((pixel >> 5) & 0x3f));
			line[linePos++] = col;

			col = (int)(255.0/31.0 * (pixel & 0x1f));
			line[linePos++] = col;
/*			col = pixel;
			col &= 0x1f;	
		//	col *= 0xff;
			col /= 0x1f;

			line[linePos++] = col;
		
			col = pixel>>5;
			col &= 0x3f;
			col *= 0xff;
			col /= 0x3f;

			line[linePos++] = col;
			col = pixel>>11;
			col &= 0x1f;
			col *= 0xff;
			col /= 0x1f;
			line[linePos++] = col;*/

		}
			fwrite ( line, sizeof ( line ), 1, fp );			
	}


	fclose ( fp );
}


