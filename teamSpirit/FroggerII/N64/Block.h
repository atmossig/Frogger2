/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: Block.h
	Programmer	: Andy Eder
	Date		: 15/04/99 16:22:24

----------------------------------------------------------------------------------------------- */

#ifndef BLOCK_H_INCLUDED
#define BLOCK_H_INCLUDED


#include "define.h" 

extern char _staticSegmentStart[];
extern char __zbufferSegmentStart[];
extern char _cfbSegmentStart[];

extern char *_objBank1SegmentRomStart;
extern char *_objBank1SegmentRomEnd;
extern char *_objBank2SegmentRomStart;
extern char *_objBank2SegmentRomEnd;
extern char *_objBank3SegmentRomStart;
extern char *_objBank3SegmentRomEnd;
extern char *_objBank4SegmentRomStart;
extern char *_objBank4SegmentRomEnd;
extern char *_objBank5SegmentRomStart;
extern char *_objBank5SegmentRomEnd;
extern char *_objBank6SegmentRomStart;
extern char *_objBank6SegmentRomEnd;
extern char *_objBank7SegmentRomStart;
extern char *_objBank7SegmentRomEnd;
extern char *_objBank8SegmentRomStart;
extern char *_objBank8SegmentRomEnd;
extern char *_objBank9SegmentRomStart;
extern char *_objBank9SegmentRomEnd;
extern char *_objBank10SegmentRomStart;
extern char *_objBank10SegmentRomEnd;
extern char *_objBank11SegmentRomStart;
extern char *_objBank11SegmentRomEnd;
extern char *_objBank12SegmentRomStart;
extern char *_objBank12SegmentRomEnd;
extern char *_objBank13SegmentRomStart;
extern char *_objBank13SegmentRomEnd;
extern char *_objBank14SegmentRomStart;
extern char *_objBank14SegmentRomEnd;
extern char *_objBank15SegmentRomStart;
extern char *_objBank15SegmentRomEnd;
extern char *_objBank16SegmentRomStart;
extern char *_objBank16SegmentRomEnd;
extern char *_objBank17SegmentRomStart;
extern char *_objBank17SegmentRomEnd;
extern char *_objBank18SegmentRomStart;
extern char *_objBank18SegmentRomEnd;
extern char *_objBank19SegmentRomStart;
extern char *_objBank19SegmentRomEnd;
extern char *_objBank20SegmentRomStart;
extern char *_objBank20SegmentRomEnd;
extern char *_objBank21SegmentRomStart;
extern char *_objBank21SegmentRomEnd;
extern char *_objBank22SegmentRomStart;
extern char *_objBank22SegmentRomEnd;
extern char *_objBank23SegmentRomStart;
extern char *_objBank23SegmentRomEnd;
extern char *_objBank24SegmentRomStart;
extern char *_objBank24SegmentRomEnd;
extern char *_objBank25SegmentRomStart;
extern char *_objBank25SegmentRomEnd;
extern char *_objBank26SegmentRomStart;
extern char *_objBank26SegmentRomEnd;
extern char *_objBank27SegmentRomStart;
extern char *_objBank27SegmentRomEnd;
extern char *_objBank28SegmentRomStart;
extern char *_objBank28SegmentRomEnd;
extern char *_objBank29SegmentRomStart;
extern char *_objBank29SegmentRomEnd;
extern char *_objBank30SegmentRomStart;
extern char *_objBank30SegmentRomEnd;
extern char *_objBank31SegmentRomStart;
extern char *_objBank31SegmentRomEnd;
extern char *_objBank32SegmentRomStart;
extern char *_objBank32SegmentRomEnd;
extern char *_objBank33SegmentRomStart;
extern char *_objBank33SegmentRomEnd;
extern char *_objBank34SegmentRomStart;
extern char *_objBank34SegmentRomEnd;
extern char *_objBank35SegmentRomStart;
extern char *_objBank35SegmentRomEnd;
extern char *_objBank36SegmentRomStart;
extern char *_objBank36SegmentRomEnd;
extern char *_objBank37SegmentRomStart;
extern char *_objBank37SegmentRomEnd;
extern char *_objBank38SegmentRomStart;
extern char *_objBank38SegmentRomEnd;
extern char *_objBank39SegmentRomStart;
extern char *_objBank39SegmentRomEnd;
extern char *_objBank40SegmentRomStart;
extern char *_objBank40SegmentRomEnd;
extern char *_objBank41SegmentRomStart;
extern char *_objBank41SegmentRomEnd;
extern char *_objBank42SegmentRomStart;
extern char *_objBank42SegmentRomEnd;




extern char *_texBank1SegmentRomStart;
extern char *_texBank1SegmentRomEnd;
extern char *_texBank2SegmentRomStart;
extern char *_texBank2SegmentRomEnd;
extern char *_texBank3SegmentRomStart;
extern char *_texBank3SegmentRomEnd;
extern char *_texBank4SegmentRomStart;
extern char *_texBank4SegmentRomEnd;
extern char *_texBank5SegmentRomStart;
extern char *_texBank5SegmentRomEnd;
extern char *_texBank6SegmentRomStart;
extern char *_texBank6SegmentRomEnd;
extern char *_texBank7SegmentRomStart;
extern char *_texBank7SegmentRomEnd;
extern char *_texBank8SegmentRomStart;
extern char *_texBank8SegmentRomEnd;
extern char *_texBank9SegmentRomStart;
extern char *_texBank9SegmentRomEnd;
extern char *_texBank10SegmentRomStart;
extern char *_texBank10SegmentRomEnd;
extern char *_texBank11SegmentRomStart;
extern char *_texBank11SegmentRomEnd;

/*extern char *_pbank1SegmentRomStart;
extern char *_pbank1SegmentRomEnd;

extern char *_pbank2SegmentRomStart;
extern char *_pbank2SegmentRomEnd;

extern char *_wbank1SegmentRomStart;
extern char *_wbank1SegmentRomEnd;

extern char *_wbank2SegmentRomStart;
extern char *_wbank2SegmentRomEnd;

extern char *_mBank1SegmentRomStart;
extern char *_mBank1SegmentRomEnd;
								*/
//----- [ DEFINES ] -----------------------------------------------------------//


#define	STACKSIZE		0x4000
#define	SMALLSTACKSIZE	0x400
#define MAXTASKS		10

// microcode types
enum
{
	UCODE_NORMAL,
	UCODE_WIREFRAME,
	UCODE_SPRITE2D,
};


#define V(p, x, y, z, f, s, t, r, g, b, a) {                            \
        (p)->v.ob[0] = (x);                                             \
        (p)->v.ob[1] = (y);                                             \
        (p)->v.ob[2] = (z);                                             \
        (p)->v.flag  = (f);                                             \
        (p)->v.tc[0] = (s);                                             \
        (p)->v.tc[1] = (t);                                             \
        (p)->v.cn[0] = (r);                                             \
        (p)->v.cn[1] = (g);                                             \
        (p)->v.cn[2] = (b);                                             \
        (p)->v.cn[3] = (a);                                             \
}

#define	SCREEN_HT			240
#define SCREEN_WD_320		320
#define SCREEN_WD_MIN		320
#define SCREEN_WD_MAX		640
#define SCREEN_WD			SCREEN_WD_320

#define XSCALE_MAX			0x400
#define MAXCONTROLLERMODE	4

#define XMIN				-5
#define XMAX				315
#define YMIN				40
#define YMAX				230
#define ZMIN				0
#define ZMAX				600

#define EYEZPOS				475

#define MAXCLIPRATIO		6

#define RDP_OUTPUT_BUF		0x8004b400		// End of Z buffer
#define COLOR_BUFFER		0x8016a000

#define RDP_OUTPUT_BUF_LEN	((4096*8)*2)

#define RDP_OUTBUF_SIZE_MIN 0x100			// Min fifo buffer size

#define PRIMCOLOR			0
#define GOURAUD				1
#define LIGHTING			2

#define	GLIST_LEN			40000			// Large enough to handle enough polys

#define TXTR_NONE			0
#define TXTR_ON				1
#define TXTR_ON_MM			2

//#define MAX_MATRICES		128*4
#define MAX_MATRICES		128*8

#define FADE_IN				0
#define FADE_OUT			1


//-----------------------------------------------------------------------------//

/*
 * ifdef needed because this file is included by "spec"
 */
#ifdef _LANGUAGE_C

// Layout of dynamic data.
/*
  This structure holds the things which change per frame. It is advantageous
  to keep dynamic data together so that we may selectively write back dirty
  data cache lines to DRAM prior to processing by the RCP.
*/

typedef struct
{
	Mtx	projection[4];

	Mtx RotationX;
	Mtx RotationY;

	Mtx Translation;

	Mtx TranslateIn;
	Mtx TranslateOut;

	Mtx modelling;
	Mtx modeling4[MAX_MATRICES];
    
	Mtx viewing[4];
	Mtx noViewing;
	Mtx identity;
	Gfx glist[GLIST_LEN];

	LookAt lookat[4];
	Hilite hilite[4];

	Vp vp[4];

} Dynamic;

typedef union
{
	struct { short type; } gen;
    struct { short type; } done;
    
	NNScMsg		app;

} GFXMsg;

extern Dynamic				dynamic[];
extern Dynamic				*dynamicp;
extern int					actualGXFListLength[];

extern unsigned short		cfb_16_a[];
extern unsigned short		cfb_16_b[];
extern void					*cfb_ptrs[2];
extern unsigned short		zbuffer[];

extern char					draw_buffer;
extern char					codeDrawingRequest;
extern char					gfxIsDrawing;
extern char					disableGraphics;
extern char					onlyDrawGraphics;
extern char					onlyDrawBackdrops;
extern char					displayOverlays;

extern char					numtasks;
extern char					currentTask;
extern NNSched				sched;

extern OSThread				mainThread;
extern OSThread				drawGraphicsThread;
extern OSThread				ControllerThread;

extern u64					rdp_output[];
extern u64					rdp_output_len;

extern float				theta;

extern char					ActiveController;
extern char					ShadingMode;
extern int					UseGlobalTransforms;
extern int					ChangeVideoModes;

extern int					ScreenWidth;
extern int					ScreenWidthDelta;
extern int					UseUCode;
extern int					OutLen;

extern char					UseTextureMode;
extern char					UseAAMode;
extern char					UseZMode;
extern char					UseWireframeMode;

extern char					transparentSurf;
extern char					xluSurf;
extern char					aiSurf;

extern char					mirrorTextures;

extern int					frameCount,frameCount2,dispFrameCount;

extern short				farPlaneDist;
extern short				nearPlaneDist;

extern long					screenNum;

extern						Vtx font_vtx[];

extern OSMesgQueue			dmaMessageQ, rspMessageQ, rdpMessageQ, retraceMessageQ;
extern OSMesg				dmaMessageBuf, rspMessageBuf, rdpMessageBuf, retraceMessageBuf;
extern OSMesg				dummyMessage;
extern OSIoMesg				dmaIOMessageBuf;
extern OSMesg				dummyMesg;
extern OSIoMesg				dmaIOMessageBuf;

extern OSMesgQueue			main_msgQ;

extern unsigned int			TicksPerFrame;

extern char					doScreenFade;
extern char					fadeDir;
extern short				fadeOut;
extern short				fadeStep;


extern struct gameStateStruct gameState;

extern void					ApplyGlobalTransformations();
extern void					ClearViewing();
extern void					DrawGraphics(void *arg);
extern void					SetRenderMode();
extern void					SetupViewing();

extern int dontClearScreen;

void ControllerProc(void *arg);

#endif	/* _LANGUAGE_C */

#endif	/* __BLOCK_H__ */
