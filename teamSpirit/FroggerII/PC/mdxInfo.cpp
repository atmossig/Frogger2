/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: mdxReport.cpp
	Programmer	: Matthew Cloy
	Date		: 

----------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <crtdbg.h>
#include <stdio.h>

#include "mgeReport.h"

// General Device Capabilities
VALUESTRING dwCaps[] =
{
	DDCAPS_3D , 							"DDCAPS_3D								Indicates that the display hardware has 3-D acceleration. \n",
	DDCAPS_ALIGNBOUNDARYDEST ,              "DDCAPS_ALIGNBOUNDARYDEST				Indicates that DirectDraw will support only those overlay destination rectangles with the x-axis aligned to the dwAlignBoundaryDest boundaries of the surface. \n",
	DDCAPS_ALIGNBOUNDARYSRC ,               "DDCAPS_ALIGNBOUNDARYSRC				Indicates that DirectDraw will support only those overlay source rectangles with the x-axis aligned to the dwAlignBoundarySrc boundaries of the surface. \n",
	DDCAPS_ALIGNSIZEDEST ,                  "DDCAPS_ALIGNSIZEDEST					Indicates that DirectDraw will support only those overlay destination rectangles whose x-axis sizes, in pixels, are dwAlignSizeDest multiples. \n",
	DDCAPS_ALIGNSIZESRC ,                   "DDCAPS_ALIGNSIZESRC					Indicates that DirectDraw will support only those overlay source rectangles whose x-axis sizes, in pixels, are dwAlignSizeSrc multiples. \n",
	DDCAPS_ALIGNSTRIDE ,                    "DDCAPS_ALIGNSTRIDE						Indicates that DirectDraw will create display memory surfaces that have a stride alignment equal to the dwAlignStrideAlign value. \n",
	DDCAPS_ALPHA ,                          "DDCAPS_ALPHA							Indicates that the display hardware supports alpha-only surfaces. (See alpha channel) \n",
	DDCAPS_BANKSWITCHED ,                   "DDCAPS_BANKSWITCHED					Indicates that the display hardware is bank-switched and is potentially very slow at random access to display memory. \n",
	DDCAPS_BLT ,                            "DDCAPS_BLT								Indicates that display hardware is capable of blit operations. \n",
	DDCAPS_BLTCOLORFILL ,                   "DDCAPS_BLTCOLORFILL					Indicates that display hardware is capable of color filling with a blitter. \n",
	DDCAPS_BLTDEPTHFILL ,                   "DDCAPS_BLTDEPTHFILL					Indicates that display hardware is capable of depth filling z-buffers with a blitter. \n",
	DDCAPS_BLTFOURCC ,                      "DDCAPS_BLTFOURCC						Indicates that display hardware is capable of color-space conversions during blit operations. \n",
	DDCAPS_BLTQUEUE ,                       "DDCAPS_BLTQUEUE						Indicates that display hardware is capable of asynchronous blit operations. \n",
	DDCAPS_BLTSTRETCH ,                     "DDCAPS_BLTSTRETCH						Indicates that display hardware is capable of stretching during blit operations. \n",
	DDCAPS_CANBLTSYSMEM ,                   "DDCAPS_CANBLTSYSMEM					Indicates that display hardware is capable of blitting to or from system memory. \n",
	DDCAPS_CANCLIP ,                        "DDCAPS_CANCLIP							Indicates that display hardware is capable of clipping with blitting. \n",
	DDCAPS_CANCLIPSTRETCHED,                "DDCAPS_CANCLIPSTRETCHED				Indicates that display hardware is capable of clipping while stretch blitting. \n",
	DDCAPS_COLORKEY ,                       "DDCAPS_COLORKEY						Supports some form of color key in either overlay or blit operations. More specific color key capability information can be found in the dwCKeyCaps member. \n",
	DDCAPS_COLORKEYHWASSIST ,               "DDCAPS_COLORKEYHWASSIST				Indicates that the color key is partially hardware assisted. This means that other resources (CPU or video memory) might be used. If this bit is not set, full hardware support is in place. \n",
	DDCAPS_GDI ,                            "DDCAPS_GDI								Indicates that display hardware is shared with GDI. \n",
	DDCAPS_NOHARDWARE ,                     "DDCAPS_NOHARDWARE						Indicates that there is no hardware support. \n",
	DDCAPS_OVERLAY ,                        "DDCAPS_OVERLAY							Indicates that display hardware supports overlays. \n",
	DDCAPS_OVERLAYCANTCLIP ,                "DDCAPS_OVERLAYCANTCLIP					Indicates that display hardware supports overlays but cannot clip them. \n",
	DDCAPS_OVERLAYFOURCC ,                  "DDCAPS_OVERLAYFOURCC					Indicates that overlay hardware is capable of color-space conversions during overlay operations. \n",
	DDCAPS_OVERLAYSTRETCH ,                 "DDCAPS_OVERLAYSTRETCH					Indicates that overlay hardware is capable of stretching. The dwMinOverlayStretch and dwMaxOverlayStretch members contain valid data. \n",
	DDCAPS_PALETTE ,                        "DDCAPS_PALETTE							Indicates that DirectDraw is capable of creating and supporting DirectDrawPalette objects for more surfaces than only the primary surface. \n",
	DDCAPS_PALETTEVSYNC ,                   "DDCAPS_PALETTEVSYNC					Indicates that DirectDraw is capable of updating a palette synchronized with the vertical refresh. \n",
	DDCAPS_READSCANLINE ,                   "DDCAPS_READSCANLINE					Indicates that display hardware is capable of returning the current scan line. \n",
//	DDCAPS_STEREOVIEW ,                     "DDCAPS_STEREOVIEW						Indicates that display hardware has stereo vision capabilities. \n",
	DDCAPS_VBI ,                            "DDCAPS_VBI								Indicates that display hardware is capable of generating a vertical-blank interrupt. \n",
	DDCAPS_ZBLTS ,                          "DDCAPS_ZBLTS							Supports the use of z-buffers with blit operations. \n",
	DDCAPS_ZOVERLAYS ,                      "DDCAPS_ZOVERLAYS						Supports the use of the IDirectDrawSurface4::UpdateOverlayZOrder method as a z-value for overlays to control their layering. \n",
	0,0,
};

// Extended Device Capabilities
VALUESTRING dwCaps2[] =
{
	DDCAPS2_AUTOFLIPOVERLAY ,				"DDCAPS2_AUTOFLIPOVERLAY :				The overlay can be automatically flipped to the next surface in the flip chain each time a video port VSYNC occurs, allowing the video port and the overlay to double buffer the video without CPU overhead. This option is only valid when the surface is receiving data from a video port. If the video port data is non-interlaced or non-interleaved, it will flip on every VSYNC. If the data is being interleaved in memory, it will flip on every other VSYNC. \n",
	DDCAPS2_CANBOBHARDWARE ,				"DDCAPS2_CANBOBHARDWARE :				The overlay hardware can display each field of an interlaced video stream individually. \n",
	DDCAPS2_CANBOBINTERLEAVED ,				"DDCAPS2_CANBOBINTERLEAVED :			The overlay hardware can display each field individually of an interlaced video stream while it is interleaved in memory without causing any artifacts that might normally occur without special hardware support. This option is only valid when the surface is receiving data from a video port and is only valid when the video is zoomed at least two times in the vertical direction. \n",
	DDCAPS2_CANBOBNONINTERLEAVED ,			"DDCAPS2_CANBOBNONINTERLEAVED :			The overlay hardware can display each field individually of an interlaced video stream while it is not interleaved in memory without causing any artifacts that might normally occur without special hardware support. This option is only valid when the surface is receiving data from a video port and is only valid when the video is zoomed at least two times in the vertical direction. \n",
	DDCAPS2_CANCALIBRATEGAMMA ,				"DDCAPS2_CANCALIBRATEGAMMA :			The system has a calibrator installed that can automatically adjust the gamma ramp so that the result will be identical on all systems that have a calibrator. To invoke the calibrator when setting new gamma levels, use the DDSGR_CALIBRATE flag when calling the IDirectDrawGammaControl::SetGammaRamp method. Calibrating gamma ramps incurs some processing overhead, and should not be used frequently. \n",
	DDCAPS2_CANDROPZ16BIT ,					"DDCAPS2_CANDROPZ16BIT :				16-bit RGBZ values can be converted into sixteen-bit RGB values. (The system does not support eight-bit conversions.) \n",
	DDCAPS2_CANFLIPODDEVEN ,				"DDCAPS2_CANFLIPODDEVEN :				The driver is capable of performing odd and even flip operations, as specified by the DDFLIP_ODD and DDFLIP_EVEN flags used with the IDirectDrawSurface4::Flip method. \n",
	DDCAPS2_CANRENDERWINDOWED ,				"DDCAPS2_CANRENDERWINDOWED :			The driver is capable of rendering in windowed mode. \n",
	DDCAPS2_CERTIFIED ,						"DDCAPS2_CERTIFIED :					Indicates that display hardware is certified. \n",
	DDCAPS2_COLORCONTROLPRIMARY ,			"DDCAPS2_COLORCONTROLPRIMARY :			The primary surface contains color controls (for instance, gamma) \n",
	DDCAPS2_COLORCONTROLOVERLAY ,			"DDCAPS2_COLORCONTROLOVERLAY :			The overlay surface contains color controls (such as brightness, sharpness) \n",
	DDCAPS2_COPYFOURCC ,					"DDCAPS2_COPYFOURCC :					Indicates that the driver supports blitting any FOURCC surface to another surface of the same FOURCC. \n",
	DDCAPS2_FLIPINTERVAL ,					"DDCAPS2_FLIPINTERVAL :					Indicates that the driver will respond to the DDFLIP_INTERVAL* flags. (see IDirectDrawSurface4::Flip). \n",
	DDCAPS2_FLIPNOVSYNC ,					"DDCAPS2_FLIPNOVSYNC :					Indicates that the driver will respond to the DDFLIP_NOVSYNC flag (see IDirectDrawSurface4::Flip). \n",
	DDCAPS2_NO2DDURING3DSCENE ,				"DDCAPS2_NO2DDURING3DSCENE :			Indicates that 2-D operations such as IDirectDrawSurface4::Blt and IDirectDrawSurface4::Lock cannot be performed on any surfaces that Direct3D® is using between calls to the IDirect3DDevice3::BeginScene and IDirect3DDevice3::EndScene methods. \n",
	DDCAPS2_NONLOCALVIDMEM ,				"DDCAPS2_NONLOCALVIDMEM :				Indicates that the display driver supports surfaces in non-local video memory. \n",
	DDCAPS2_NONLOCALVIDMEMCAPS ,			"DDCAPS2_NONLOCALVIDMEMCAPS :			Indicates that blit capabilities for non-local video memory surfaces differ from local video memory surfaces. If this flag is present, the DDCAPS2_NONLOCALVIDMEM flag will also be present. \n",
	DDCAPS2_NOPAGELOCKREQUIRED ,			"DDCAPS2_NOPAGELOCKREQUIRED :			DMA blit operations are supported on system memory surfaces that are not page locked. \n",
	DDCAPS2_PRIMARYGAMMA ,					"DDCAPS2_PRIMARYGAMMA :					Supports dynamic gamma ramps for the primary surface. For more information, see Gamma and Color Controls. \n",
	DDCAPS2_VIDEOPORT ,						"DDCAPS2_VIDEOPORT :					Indicates that display hardware supports live video. \n",
	DDCAPS2_WIDESURFACES ,					"DDCAPS2_WIDESURFACES :					Indicates that the display surfaces supports surfaces wider than the primary surface. \n",
	0,0,
};

// Devices Color Keying Abilities
VALUESTRING dwCKeyCaps[] = 
{
	DDCKEYCAPS_DESTBLT ,					"DDCKEYCAPS_DESTBLT :					Supports transparent blitting with a color key that identifies the replaceable bits of the destination surface for RGB colors. \n",
	DDCKEYCAPS_DESTBLTCLRSPACE ,			"DDCKEYCAPS_DESTBLTCLRSPACE :			Supports transparent blitting with a color space that identifies the replaceable bits of the destination surface for RGB colors. \n",
	DDCKEYCAPS_DESTBLTCLRSPACEYUV ,			"DDCKEYCAPS_DESTBLTCLRSPACEYUV :		Supports transparent blitting with a color space that identifies the replaceable bits of the destination surface for YUV colors. \n",
	DDCKEYCAPS_DESTBLTYUV ,					"DDCKEYCAPS_DESTBLTYUV :				Supports transparent blitting with a color key that identifies the replaceable bits of the destination surface for YUV colors. \n",
	DDCKEYCAPS_DESTOVERLAY ,				"DDCKEYCAPS_DESTOVERLAY :				Supports overlaying with color keying of the replaceable bits of the destination surface being overlaid for RGB colors. \n",
	DDCKEYCAPS_DESTOVERLAYCLRSPACE ,		"DDCKEYCAPS_DESTOVERLAYCLRSPACE :		Supports a color space as the color key for the destination of RGB colors. \n",
	DDCKEYCAPS_DESTOVERLAYCLRSPACEYUV ,		"DDCKEYCAPS_DESTOVERLAYCLRSPACEYUV:		Supports a color space as the color key for the destination of YUV colors. \n",
	DDCKEYCAPS_DESTOVERLAYONEACTIVE ,		"DDCKEYCAPS_DESTOVERLAYONEACTIVE :		Supports only one active destination color key value for visible overlay surfaces . \n",
	DDCKEYCAPS_DESTOVERLAYYUV ,				"DDCKEYCAPS_DESTOVERLAYYUV :			Supports overlaying using color keying of the replaceable bits of the destination surface being overlaid for YUV colors. \n",
	DDCKEYCAPS_NOCOSTOVERLAY ,				"DDCKEYCAPS_NOCOSTOVERLAY :				Indicates there are no bandwidth trade-offs for using the color key with an overlay. \n",
	DDCKEYCAPS_SRCBLT ,						"DDCKEYCAPS_SRCBLT :					Supports transparent blitting using the color key for the source with this surface for RGB colors. \n",
	DDCKEYCAPS_SRCBLTCLRSPACE ,				"DDCKEYCAPS_SRCBLTCLRSPACE :			Supports transparent blitting using a color space for the source with this surface for RGB colors. \n",
	DDCKEYCAPS_SRCBLTCLRSPACEYUV ,			"DDCKEYCAPS_SRCBLTCLRSPACEYUV :			Supports transparent blitting using a color space for the source with this surface for YUV colors. \n",
	DDCKEYCAPS_SRCBLTYUV ,					"DDCKEYCAPS_SRCBLTYUV :					Supports transparent blitting using the color key for the source with this surface for YUV colors. \n",
	DDCKEYCAPS_SRCOVERLAY ,					"DDCKEYCAPS_SRCOVERLAY :				Supports overlaying using the color key for the source with this overlay surface for RGB colors. \n",
	DDCKEYCAPS_SRCOVERLAYCLRSPACE ,			"DDCKEYCAPS_SRCOVERLAYCLRSPACE :		Supports overlaying using a color space as the source color key for the overlay surface for RGB colors. \n",
	DDCKEYCAPS_SRCOVERLAYCLRSPACEYUV ,		"DDCKEYCAPS_SRCOVERLAYCLRSPACEYUV :		Supports overlaying using a color space as the source color key for the overlay surface for YUV colors. \n",
	DDCKEYCAPS_SRCOVERLAYONEACTIVE ,		"DDCKEYCAPS_SRCOVERLAYONEACTIVE :		Supports only one active source color key value for visible overlay surfaces. \n",
	DDCKEYCAPS_SRCOVERLAYYUV ,				"DDCKEYCAPS_SRCOVERLAYYUV :				Supports overlaying using the color key for the source with this overlay surface for YUV colors.\n",
	0,0,
};

// Device FX Capabilities
VALUESTRING dwFXCaps[] = 
{
	DDFXCAPS_BLTALPHA ,						"DDFXCAPS_BLTALPHA :					Supports alpha-blended blit operations. \n",
	DDFXCAPS_BLTARITHSTRETCHY ,				"DDFXCAPS_BLTARITHSTRETCHY :			Uses arithmetic operations, rather than pixel-doubling techniques, to stretch and shrink surfaces during a blit operation. Occurs along the y-axis (vertically). \n",
	DDFXCAPS_BLTARITHSTRETCHYN ,			"DDFXCAPS_BLTARITHSTRETCHYN :			Uses arithmetic operations, rather than pixel-doubling techniques, to stretch and shrink surfaces during a blit operation. Occurs along the y-axis (vertically), and works only for integer stretching (×1, ×2, and so on). \n",
	DDFXCAPS_BLTFILTER ,					"DDFXCAPS_BLTFILTER :					Driver can do surface-reconstruction filtering for warped blits. \n",
	DDFXCAPS_BLTMIRRORLEFTRIGHT ,			"DDFXCAPS_BLTMIRRORLEFTRIGHT :			Supports mirroring left to right in a blit operation. \n",
	DDFXCAPS_BLTMIRRORUPDOWN ,				"DDFXCAPS_BLTMIRRORUPDOWN :				Supports mirroring top to bottom in a blit operation. \n",
	DDFXCAPS_BLTROTATION ,					"DDFXCAPS_BLTROTATION :					Supports arbitrary rotation in a blit operation. \n",
	DDFXCAPS_BLTROTATION90 ,				"DDFXCAPS_BLTROTATION90 :				Supports 90-degree rotations in a blit operation. \n",
	DDFXCAPS_BLTSHRINKX ,					"DDFXCAPS_BLTSHRINKX :					Supports arbitrary shrinking of a surface along the x-axis (horizontally). This flag is valid only for blit operations. \n",
	DDFXCAPS_BLTSHRINKXN ,					"DDFXCAPS_BLTSHRINKXN :					Supports integer shrinking (×1, ×2, and so on) of a surface along the x-axis (horizontally). This flag is valid only for blit operations. \n",
	DDFXCAPS_BLTSHRINKY ,					"DDFXCAPS_BLTSHRINKY :					Supports arbitrary shrinking of a surface along the y-axis (vertically). This flag is valid only for blit operations. \n",
	DDFXCAPS_BLTSHRINKYN ,					"DDFXCAPS_BLTSHRINKYN :					Supports integer shrinking (×1, ×2, and so on) of a surface along the y-axis (vertically). This flag is valid only for blit operations. \n",
	DDFXCAPS_BLTSTRETCHX ,					"DDFXCAPS_BLTSTRETCHX :					Supports arbitrary stretching of a surface along the x-axis (horizontally). This flag is valid only for blit operations. \n",
	DDFXCAPS_BLTSTRETCHXN ,					"DDFXCAPS_BLTSTRETCHXN :				Supports integer stretching (×1, ×2, and so on) of a surface along the x-axis (horizontally). This flag is valid only for blit operations. \n",
	DDFXCAPS_BLTSTRETCHY ,					"DDFXCAPS_BLTSTRETCHY :					Supports arbitrary stretching of a surface along the y-axis (vertically). This flag is valid only for blit operations. \n",
	DDFXCAPS_BLTSTRETCHYN ,					"DDFXCAPS_BLTSTRETCHYN :				Supports integer stretching (×1, ×2, and so on) of a surface along the y-axis (vertically). This flag is valid only for blit operations. \n",
//	DDFXCAPS_BLTTRANSFORM ,					"DDFXCAPS_BLTTRANSFORM :				Supports geometric transformations (or warps) for blitted sprites. Transformations are not currently supported for explicit blit operations. \n",
	DDFXCAPS_OVERLAYALPHA ,					"DDFXCAPS_OVERLAYALPHA :				Supports alpha blending for overlay surfaces. \n",
	DDFXCAPS_OVERLAYARITHSTRETCHY ,			"DDFXCAPS_OVERLAYARITHSTRETCHY :		Uses arithmetic operations, rather than pixel-doubling techniques, to stretch and shrink overlay surfaces. Occurs along the y-axis (vertically). \n",
	DDFXCAPS_OVERLAYARITHSTRETCHYN ,		"DDFXCAPS_OVERLAYARITHSTRETCHYN :		Uses arithmetic operations, rather than pixel-doubling techniques, to stretch and shrink overlay surfaces. Occurs along the y-axis (vertically), and works only for integer stretching (×1, ×2, and so on). \n",
	DDFXCAPS_OVERLAYFILTER ,				"DDFXCAPS_OVERLAYFILTER :				Supports surface-reconstruction filtering for warped overlay sprites. Filtering is not currently supported for explicitly displayed overlay surfaces (those displayed with calls to IDirectDrawSurface4::UpdateOverlay). \n",
	DDFXCAPS_OVERLAYMIRRORLEFTRIGHT ,		"DDFXCAPS_OVERLAYMIRRORLEFTRIGHT :		Supports mirroring of overlays across the vertical axis. \n",
	DDFXCAPS_OVERLAYMIRRORUPDOWN ,			"DDFXCAPS_OVERLAYMIRRORUPDOWN :			Supports mirroring of overlays across the horizontal axis. \n",
	DDFXCAPS_OVERLAYSHRINKX ,				"DDFXCAPS_OVERLAYSHRINKX :				Supports arbitrary shrinking of a surface along the x-axis (horizontally). This flag is valid only for DDSCAPS_OVERLAY surfaces. This flag indicates only the capabilities of a surface; it does not indicate that shrinking is available. \n",
	DDFXCAPS_OVERLAYSHRINKXN ,				"DDFXCAPS_OVERLAYSHRINKXN :				Supports integer shrinking (×1, ×2, and so on) of a surface along the x-axis (horizontally). This flag is valid only for DDSCAPS_OVERLAY surfaces. This flag indicates only the capabilities of a surface; it does not indicate that shrinking is available. \n",
	DDFXCAPS_OVERLAYSHRINKY ,				"DDFXCAPS_OVERLAYSHRINKY :				Supports arbitrary shrinking of a surface along the y-axis (vertically). This flag is valid only for DDSCAPS_OVERLAY surfaces. This flag indicates only the capabilities of a surface; it does not indicate that shrinking is available. \n",
	DDFXCAPS_OVERLAYSHRINKYN ,				"DDFXCAPS_OVERLAYSHRINKYN :				Supports integer shrinking (×1, ×2, and so on) of a surface along the y-axis (vertically). This flag is valid only for DDSCAPS_OVERLAY surfaces. This flag indicates only the capabilities of a surface; it does not indicate that shrinking is available. \n",
	DDFXCAPS_OVERLAYSTRETCHX ,				"DDFXCAPS_OVERLAYSTRETCHX :				Supports arbitrary stretching of a surface along the x-axis (horizontally). This flag is valid only for DDSCAPS_OVERLAY surfaces. This flag indicates only the capabilities of a surface; it does not indicate that stretching is available. \n",
	DDFXCAPS_OVERLAYSTRETCHXN ,				"DDFXCAPS_OVERLAYSTRETCHXN :			Supports integer stretching (×1, ×2, and so on) of a surface along the x-axis (horizontally). This flag is valid only for DDSCAPS_OVERLAY surfaces. This flag indicates only the capabilities of a surface; it does not indicate that stretching is available. \n",
	DDFXCAPS_OVERLAYSTRETCHY ,				"DDFXCAPS_OVERLAYSTRETCHY :				Supports arbitrary stretching of a surface along the y-axis (vertically). This flag is valid only for DDSCAPS_OVERLAY surfaces. This flag indicates only the capabilities of a surface; it does not indicate that stretching is available. \n",
	DDFXCAPS_OVERLAYSTRETCHYN ,				"DDFXCAPS_OVERLAYSTRETCHYN :			Supports integer stretching (×1, ×2, and so on) of a surface along the y-axis (vertically). This flag is valid only for DDSCAPS_OVERLAY surfaces. This flag indicates only the capabilities of a surface; it does not indicate that stretching is available. \n",
//	DDFXCAPS_OVERLAYTRANSFORM ,				"DDFXCAPS_OVERLAYTRANSFORM :			Supports geometric transformations (or warps) for overlay sprites. Transformations are not currently supported for explicitly displayed overlay surfaces (those displayed with calls to IDirectDrawSurface4::UpdateOverlay). \n",
	0,0,
};

// Devices Alpha Capabilities
VALUESTRING dwFXAlphaCaps[] =
{
	DDFXALPHACAPS_BLTALPHAEDGEBLEND ,		"DDFXALPHACAPS_BLTALPHAEDGEBLEND :		Supports alpha blending around the edge of a source color-keyed surface. Used for blit operations. \n",
	DDFXALPHACAPS_BLTALPHAPIXELS ,			"DDFXALPHACAPS_BLTALPHAPIXELS :			Supports alpha information in pixel format. The bit depth of alpha information in the pixel format can be 1, 2, 4, or 8. The alpha value becomes more opaque as the alpha value increases. Regardless of the depth of the alpha information, 0 is always the fully transparent value. Used for blit operations. \n",
	DDFXALPHACAPS_BLTALPHAPIXELSNEG ,		"DDFXALPHACAPS_BLTALPHAPIXELSNEG :		Supports alpha information in pixel format. The bit depth of alpha information in the pixel format can be 1, 2, 4, or 8. The alpha value becomes more transparent as the alpha value increases. Regardless of the depth of the alpha information, 0 is always the fully opaque value. This flag can be used only if DDCAPS_ALPHA is set. Used for blit operations. \n",
	DDFXALPHACAPS_BLTALPHASURFACES ,		"DDFXALPHACAPS_BLTALPHASURFACES :		Supports alpha-only surfaces. The bit depth of an alpha-only surface can be 1, 2, 4, or 8. The alpha value becomes more opaque as the alpha value increases. Regardless of the depth of the alpha information, 0 is always the fully transparent value. Used for blit operations. \n",
	DDFXALPHACAPS_BLTALPHASURFACESNEG ,		"DDFXALPHACAPS_BLTALPHASURFACESNEG :	Indicates that the alpha channel becomes more transparent as the alpha value increases. The depth of the alpha channel data can be 1, 2, 4, or 8. Regardless of the depth of the alpha information, 0 is always the fully opaque value. This flag can be set only if DDCAPS_ALPHA has been set. Used for blit operations. \n",
	DDFXALPHACAPS_OVERLAYALPHAEDGEBLEND ,	"DDFXALPHACAPS_OVERLAYALPHAEDGEBLEND :	Supports alpha blending around the edge of a source color-keyed surface. Used for overlays. \n",
	DDFXALPHACAPS_OVERLAYALPHAPIXELS ,		"DDFXALPHACAPS_OVERLAYALPHAPIXELS :		Supports alpha information in pixel format. The bit depth of alpha information in pixel format can be 1, 2, 4, or 8. The alpha value becomes more opaque as the alpha value increases. Regardless of the depth of the alpha information, 0 is always the fully transparent value. Used for overlays. \n",
	DDFXALPHACAPS_OVERLAYALPHAPIXELSNEG ,	"DDFXALPHACAPS_OVERLAYALPHAPIXELSNEG :	Supports alpha information in pixel format. The bit depth of alpha information in pixel format can be 1, 2, 4, or 8. The alpha value becomes more transparent as the alpha value increases. Regardless of the depth of the alpha information, 0 is always the fully opaque value. This flag can be used only if DDCAPS_ALPHA has been set. Used for overlays. \n",
	DDFXALPHACAPS_OVERLAYALPHASURFACES ,	"DDFXALPHACAPS_OVERLAYALPHASURFACES :	Supports alpha-only surfaces. The bit depth of an alpha-only surface can be 1, 2, 4, or 8. The alpha value becomes more opaque as the alpha value increases. Regardless of the depth of the alpha information, 0 is always the fully transparent value. Used for overlays. \n",
	DDFXALPHACAPS_OVERLAYALPHASURFACESNEG ,	"DDFXALPHACAPS_OVERLAYALPHASURFACESNEG: Indicates that the alpha channel becomes more transparent as the alpha value increases. The depth of the alpha channel data can be 1, 2, 4, or 8. Regardless of the depth of the alpha information, 0 is always the fully opaque value. This flag can be used only if DDCAPS_ALPHA has been set. Used for overlays. \n",
	0,0,
};

// Devices Palette Capabilities
VALUESTRING dwPalCaps[] =
{
	DDPCAPS_1BIT,							"DDPCAPS_1BIT:							Supports palettes that contain 1 bit color entries (two colors). \n",
	DDPCAPS_2BIT,							"DDPCAPS_2BIT:							Supports palettes that contain 2 bit color entries (four colors). \n",
	DDPCAPS_4BIT,							"DDPCAPS_4BIT:							Supports palettes that contain 4 bit color entries (16 colors). \n",
	DDPCAPS_8BIT,							"DDPCAPS_8BIT:							Supports palettes that contain 8 bit color entries (256 colors). \n",
	DDPCAPS_8BITENTRIES,					"DDPCAPS_8BITENTRIES:					Specifies an index to an 8-bit color index. This field is valid only when used with the DDPCAPS_1BIT, DDPCAPS_2BIT, or DDPCAPS_4BIT capability and when the target surface is in 8 bits per pixel (bpp). Each color entry is 1 byte long and is an index to an 8-bpp palette on the destination surface. \n",
	DDPCAPS_ALPHA,							"DDPCAPS_ALPHA:							Supports palettes that include an alpha component. For alpha-capable palettes, the peFlags member of for each PALETTEENTRY structure the palette contains is to be interpreted as a single 8-bit alpha value (in addition to the color data in the peRed, peGreen, and peBlue members). A palette created with this flag can only be attached to a texture surface. \n",
	DDPCAPS_ALLOW256,						"DDPCAPS_ALLOW256:						Supports palettes that can have all 256 entries defined. \n",
	DDPCAPS_PRIMARYSURFACE,					"DDPCAPS_PRIMARYSURFACE:				Indicates that the palette is attached to the primary surface. Changing the palette has an immediate effect on the display unless the DDPCAPS_VSYNC capability is specified and supported. \n",
	DDPCAPS_PRIMARYSURFACELEFT,				"DDPCAPS_PRIMARYSURFACELEFT:			Indicates that the palette is attached to the primary surface on the left. Changing the palette has an immediate effect on the display unless the DDPCAPS_VSYNC capability is specified and supported. \n",
	DDPCAPS_VSYNC,							"DDPCAPS_VSYNC:							Indicates that the palette can be modified synchronously with the monitor's refresh rate. \n",
	0,0,
};

// DirectDraw Error strings
VALUESTRING ddErrors[] = 
{
	DD_OK,									"DD_OK:									No error.\0",
	DDERR_ALREADYINITIALIZED,				"DDERR_ALREADYINITIALIZED:				This object is already initialized.\0",
	DDERR_BLTFASTCANTCLIP,					"DDERR_BLTFASTCANTCLIP:					Return if a clipper object is attached to the source surface passed into a BltFast call.\0",
	DDERR_CANNOTATTACHSURFACE,				"DDERR_CANNOTATTACHSURFACE:				This surface can not be attached to the requested surface.\0",
	DDERR_CANNOTDETACHSURFACE,				"DDERR_CANNOTDETACHSURFACE:				This surface can not be detached from the requested surface.\0",
	DDERR_CANTCREATEDC,						"DDERR_CANTCREATEDC:					Windows can not create any more DCs.\0",
	DDERR_CANTDUPLICATE,					"DDERR_CANTDUPLICATE:					Can't duplicate primary & 3D surfaces, or surfaces that are implicitly created.\0",
	DDERR_CLIPPERISUSINGHWND,				"DDERR_CLIPPERISUSINGHWND:				An attempt was made to set a cliplist for a clipper object that is already monitoring an hwnd.\0",
	DDERR_COLORKEYNOTSET,					"DDERR_COLORKEYNOTSET:					No src color key specified for this operation.\0",
	DDERR_CURRENTLYNOTAVAIL,				"DDERR_CURRENTLYNOTAVAIL:				Support is currently not available.\0",
	DDERR_DIRECTDRAWALREADYCREATED,			"DDERR_DIRECTDRAWALREADYCREATED:		A DirectDraw object representing this driver has already been created for this process.\0",
	DDERR_EXCEPTION,						"DDERR_EXCEPTION:						An exception was encountered while performing the requested operation.\0",
	DDERR_EXCLUSIVEMODEALREADYSET,			"DDERR_EXCLUSIVEMODEALREADYSET:			An attempt was made to set the cooperative level when it was already set to exclusive.\0",
	DDERR_GENERIC,							"DDERR_GENERIC:							Generic failure.\0",
	DDERR_HEIGHTALIGN,						"DDERR_HEIGHTALIGN:						Height of rectangle provided is not a multiple of reqd alignment.\0",
	DDERR_HWNDALREADYSET,					"DDERR_HWNDALREADYSET:					The CooperativeLevel HWND has already been set. It can not be reset while the process has surfaces or palettes created.\0",
	DDERR_HWNDSUBCLASSED,					"DDERR_HWNDSUBCLASSED:					HWND used by DirectDraw CooperativeLevel has been subclassed, this prevents DirectDraw from restoring state.\0",
	DDERR_IMPLICITLYCREATED,				"DDERR_IMPLICITLYCREATED:				This surface can not be restored because it is an implicitly created surface.\0",
	DDERR_INCOMPATIBLEPRIMARY,				"DDERR_INCOMPATIBLEPRIMARY:				Unable to match primary surface creation request with existing primary surface.\0",
	DDERR_INVALIDCAPS,						"DDERR_INVALIDCAPS:						One or more of the caps bits passed to the callback are incorrect.\0",
	DDERR_INVALIDCLIPLIST,					"DDERR_INVALIDCLIPLIST:					DirectDraw does not support the provided cliplist.\0",
	DDERR_INVALIDDIRECTDRAWGUID,			"DDERR_INVALIDDIRECTDRAWGUID:			The GUID passed to DirectDrawCreate is not a valid DirectDraw driver identifier.\0",
	DDERR_INVALIDMODE,						"DDERR_INVALIDMODE:						DirectDraw does not support the requested mode.\0",
	DDERR_INVALIDOBJECT,					"DDERR_INVALIDOBJECT:					DirectDraw received a pointer that was an invalid DIRECTDRAW object.\0",
	DDERR_INVALIDPARAMS,					"DDERR_INVALIDPARAMS:					One or more of the parameters passed to the function are incorrect.\0",
	DDERR_INVALIDPIXELFORMAT,				"DDERR_INVALIDPIXELFORMAT:				The pixel format was invalid as specified.\0",
	DDERR_INVALIDPOSITION,					"DDERR_INVALIDPOSITION:					Returned when the position of the overlay on the destination is no longer legal for that destination.\0",
	DDERR_INVALIDRECT,						"DDERR_INVALIDRECT:						Rectangle provided was invalid.\0",
	DDERR_LOCKEDSURFACES,					"DDERR_LOCKEDSURFACES:					Operation could not be carried out because one or more surfaces are locked.\0",
	DDERR_NO3D,								"DDERR_NO3D:							There is no 3D present.\0",
	DDERR_NOALPHAHW,						"DDERR_NOALPHAHW:						Operation could not be carried out because there is no alpha accleration hardware present or available.\0",
	DDERR_NOBLTHW,							"DDERR_NOBLTHW:							No blitter hardware present.\0",
	DDERR_NOCLIPLIST,						"DDERR_NOCLIPLIST:						No cliplist available.\0",
	DDERR_NOCLIPPERATTACHED,				"DDERR_NOCLIPPERATTACHED:				No clipper object attached to surface object.\0",
	DDERR_NOCOLORCONVHW,					"DDERR_NOCOLORCONVHW:					Operation could not be carried out because there is no color conversion hardware present or available.\0",
	DDERR_NOCOLORKEY,						"DDERR_NOCOLORKEY:						Surface doesn't currently have a color key\0",
	DDERR_NOCOLORKEYHW,						"DDERR_NOCOLORKEYHW:					Operation could not be carried out because there is no hardware support of the destination color key.\0",
	DDERR_NOCOOPERATIVELEVELSET,			"DDERR_NOCOOPERATIVELEVELSET:			Create function called without DirectDraw object method SetCooperativeLevel being called.\0",
	DDERR_NODC,								"DDERR_NODC:							No DC was ever created for this surface.\0",
	DDERR_NODDROPSHW,						"DDERR_NODDROPSHW:						No DirectDraw ROP hardware.\0",
	DDERR_NODIRECTDRAWHW,					"DDERR_NODIRECTDRAWHW:					A hardware-only DirectDraw object creation was attempted but the driver did not support any hardware.\0",
	DDERR_NOEMULATION,						"DDERR_NOEMULATION:						Software emulation not available.\0",
	DDERR_NOEXCLUSIVEMODE,					"DDERR_NOEXCLUSIVEMODE:					Operation requires the application to have exclusive mode but the application does not have exclusive mode.\0",
	DDERR_NOFLIPHW,							"DDERR_NOFLIPHW:						Flipping visible surfaces is not supported.\0",
	DDERR_NOGDI,							"DDERR_NOGDI:							There is no GDI present.\0",
	DDERR_NOHWND,							"DDERR_NOHWND:							Clipper notification requires an HWND or no HWND has previously been set as the CooperativeLevel HWND.\0",
	DDERR_NOMIRRORHW,						"DDERR_NOMIRRORHW:						Operation could not be carried out because there is no hardware present or available.\0",
	DDERR_NOOVERLAYDEST,					"DDERR_NOOVERLAYDEST:					Returned when GetOverlayPosition is called on an overlay that UpdateOverlay has never been called on to establish a destination.\0",
	DDERR_NOOVERLAYHW,						"DDERR_NOOVERLAYHW:						Operation could not be carried out because there is no overlay hardware present or available.\0",
	DDERR_NOPALETTEATTACHED,				"DDERR_NOPALETTEATTACHED:				No palette object attached to this surface.\0",
	DDERR_NOPALETTEHW,						"DDERR_NOPALETTEHW:						No hardware support for 16 or 256 color palettes.\0",
	DDERR_NORASTEROPHW,						"DDERR_NORASTEROPHW:					Operation could not be carried out because there is no appropriate raster op hardware present or available.\0",
	DDERR_NOROTATIONHW,						"DDERR_NOROTATIONHW:					Operation could not be carried out because there is no rotation hardware present or available.\0",
	DDERR_NOSTRETCHHW,						"DDERR_NOSTRETCHHW:						Operation could not be carried out because there is no hardware support for stretching.\0",
	DDERR_NOT4BITCOLOR,						"DDERR_NOT4BITCOLOR:					DirectDrawSurface is not in 4 bit color palette and the requested operation requires 4 bit color palette.\0",
	DDERR_NOT4BITCOLORINDEX,				"DDERR_NOT4BITCOLORINDEX:				DirectDrawSurface is not in 4 bit color index palette and the requested operation requires 4 bit color index palette.\0",
	DDERR_NOT8BITCOLOR,						"DDERR_NOT8BITCOLOR:					DirectDrawSurface is not in 8 bit color mode and the requested operation requires 8 bit color.\0",
	DDERR_NOTAOVERLAYSURFACE,				"DDERR_NOTAOVERLAYSURFACE:				Returned when an overlay member is called for a non-overlay surface.\0",
	DDERR_NOTEXTUREHW,						"DDERR_NOTEXTUREHW:						Operation could not be carried out because there is no texture mapping hardware present or available.\0",
	DDERR_NOTFLIPPABLE,						"DDERR_NOTFLIPPABLE:					An attempt has been made to flip a surface that is not flippable.\0",
	DDERR_NOTFOUND,							"DDERR_NOTFOUND:						Requested item was not found.\0",
	DDERR_NOTLOCKED,						"DDERR_NOTLOCKED:						Surface was not locked.  An attempt to unlock a surface that was not locked at all, or by this process, has been attempted.\0",
	DDERR_NOTPALETTIZED,					"DDERR_NOTPALETTIZED:					The surface being used is not a palette-based surface.\0",
	DDERR_NOVSYNCHW,						"DDERR_NOVSYNCHW:						Operation could not be carried out because there is no hardware support for vertical blank synchronized operations.\0",
	DDERR_NOZBUFFERHW,						"DDERR_NOZBUFFERHW:						Operation could not be carried out because there is no hardware support for zbuffer blitting.\0",
	DDERR_NOZOVERLAYHW,						"DDERR_NOZOVERLAYHW:					Overlay surfaces could not be z layered based on their BltOrder because the hardware does not support z layering of overlays.\0",
	DDERR_OUTOFCAPS,						"DDERR_OUTOFCAPS:						The hardware needed for the requested operation has already been allocated.\0",
	DDERR_OUTOFMEMORY,						"DDERR_OUTOFMEMORY:						DirectDraw does not have enough memory to perform the operation.\0",
	DDERR_OUTOFVIDEOMEMORY,					"DDERR_OUTOFVIDEOMEMORY:				DirectDraw does not have enough memory to perform the operation.\0",
	DDERR_OVERLAYCANTCLIP,					"DDERR_OVERLAYCANTCLIP:					The hardware does not support clipped overlays.\0",
	DDERR_OVERLAYCOLORKEYONLYONEACTIVE,		"DDERR_OVERLAYCOLORKEYONLYONEACTIVE:	Can only have ony color key active at one time for overlays.\0",
	DDERR_OVERLAYNOTVISIBLE,				"DDERR_OVERLAYNOTVISIBLE:				Returned when GetOverlayPosition is called on a hidden overlay.\0",
	DDERR_PALETTEBUSY,						"DDERR_PALETTEBUSY:						Access to this palette is being refused because the palette is already locked by another thread.\0",
	DDERR_PRIMARYSURFACEALREADYEXISTS,		"DDERR_PRIMARYSURFACEALREADYEXISTS:		This process already has created a primary surface.\0",
	DDERR_REGIONTOOSMALL,					"DDERR_REGIONTOOSMALL:					Region passed to Clipper::GetClipList is too small.\0",
	DDERR_SURFACEALREADYATTACHED,			"DDERR_SURFACEALREADYATTACHED:			This surface is already attached to the surface it is being attached to.\0",
	DDERR_SURFACEALREADYDEPENDENT,			"DDERR_SURFACEALREADYDEPENDENT:			This surface is already a dependency of the surface it is being made a dependency of.\0",
	DDERR_SURFACEBUSY,						"DDERR_SURFACEBUSY:						Access to this surface is being refused because the surface is already locked by another thread.\0",
	DDERR_SURFACEISOBSCURED,				"DDERR_SURFACEISOBSCURED:				Access to surface refused because the surface is obscured.\0",
	DDERR_SURFACELOST,						"DDERR_SURFACELOST:						Access to this surface is being refused because the surface memory is gone. The DirectDrawSurface object representing this surface should have Restore called on it.\0",
	DDERR_SURFACENOTATTACHED,				"DDERR_SURFACENOTATTACHED:				The requested surface is not attached.\0",
	DDERR_TOOBIGHEIGHT,						"DDERR_TOOBIGHEIGHT:					Height requested by DirectDraw is too large.\0",
	DDERR_TOOBIGSIZE,						"DDERR_TOOBIGSIZE:						Size requested by DirectDraw is too large, but the individual height and width are OK.\0",
	DDERR_TOOBIGWIDTH,						"DDERR_TOOBIGWIDTH:						Width requested by DirectDraw is too large.\0",
	DDERR_UNSUPPORTED,						"DDERR_UNSUPPORTED:						Action not supported.\0",
	DDERR_UNSUPPORTEDFORMAT,				"DDERR_UNSUPPORTEDFORMAT:				FOURCC format requested is unsupported by DirectDraw.\0",
	DDERR_UNSUPPORTEDMASK,					"DDERR_UNSUPPORTEDMASK:					Bitmask in the pixel format requested is unsupported by DirectDraw.\0",
	DDERR_VERTICALBLANKINPROGRESS,			"DDERR_VERTICALBLANKINPROGRESS:			Vertical blank is in progress.\0",
	DDERR_WASSTILLDRAWING,					"DDERR_WASSTILLDRAWING:					Informs DirectDraw that the previous Blt which is transfering information to or from this Surface is incomplete.\0",
	DDERR_WRONGMODE,						"DDERR_WRONGMODE:						This surface can not be restored because it was created in a different mode.\0",
	DDERR_XALIGN,							"DDERR_XALIGN:							Rectangle provided was not horizontally aligned on required boundary.\0",
	0,0,
};



/*	--------------------------------------------------------------------------------
	Function	: ddShowCaps
	Purpose		: Prints out the capabilities held in a DDCAPS structure
	Parameters	: pointer to caps structure
	Returns		: nothing
	Info		: 
*/

void ddShowCaps(DDCAPS *me)
{
	rptShowBitfields (me->dwCaps, dwCaps);
	rptShowBitfields (me->dwCaps2, dwCaps2);
	rptShowBitfields (me->dwCKeyCaps, dwCKeyCaps);
	rptShowBitfields (me->dwFXCaps, dwFXCaps);
	rptShowBitfields (me->dwFXAlphaCaps, dwFXAlphaCaps);
	rptShowBitfields (me->dwPalCaps, dwPalCaps); 
}

/*	--------------------------------------------------------------------------------
	Function	: ddShowError
	Purpose		: Converts a DirectDraw Error to a string describing the error, and prints it
	Parameters	: error value
	Returns		: 
	Info		: 
*/

void ddShowError(HRESULT error)
{
	char *errSt;
	if (errSt = rptGetString (error, ddErrors))
		dp(errSt);
}
