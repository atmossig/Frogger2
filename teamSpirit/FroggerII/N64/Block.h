/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: Block.h
	Programmer	: Andy Eder
	Date		: 15/04/99 16:22:24

----------------------------------------------------------------------------------------------- */

#ifndef BLOCK_H_INCLUDED
#define BLOCK_H_INCLUDED


#include "define.h" 

#define MAX_LOOKATS	64
#define MAX_HILITES 64

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
extern char *_texBank12SegmentRomStart;
extern char *_texBank12SegmentRomEnd;
extern char *_texBank13SegmentRomStart;
extern char *_texBank13SegmentRomEnd;
extern char *_texBank14SegmentRomStart;
extern char *_texBank14SegmentRomEnd;
extern char *_texBank15SegmentRomStart;
extern char *_texBank15SegmentRomEnd;
extern char *_texBank16SegmentRomStart;
extern char *_texBank16SegmentRomEnd;
extern char *_texBank17SegmentRomStart;
extern char *_texBank17SegmentRomEnd;
extern char *_texBank18SegmentRomStart;
extern char *_texBank18SegmentRomEnd;




extern char *_levData1SegmentRomStart;
extern char *_levData1SegmentRomEnd;
extern char *_levData2SegmentRomStart;
extern char *_levData2SegmentRomEnd;
extern char *_levData3SegmentRomStart;
extern char *_levData3SegmentRomEnd;
extern char *_levData4SegmentRomStart;
extern char *_levData4SegmentRomEnd;

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
#define MAXTASKS		20

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

//#define RDP_OUTPUT_BUF_LEN	((4096*8)*2)
#define RDP_OUTPUT_BUF_LEN	(4096)

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

extern unsigned long		actFrameCount;

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

//extern char					UseTextureMode;
//extern char					UseAAMode;
//extern char					UseZMode;
extern char					UseWireframeMode;

//extern char					transparentSurf;
//extern char					xluSurf;
//extern char					aiSurf;

extern char					mirrorTextures;

extern short				farPlaneDist;
extern short				nearPlaneDist;

extern						Vtx font_vtx[];

extern OSMesgQueue			dmaMessageQ, rspMessageQ, rdpMessageQ, retraceMessageQ;
extern OSMesg				dmaMessageBuf, rspMessageBuf, rdpMessageBuf, retraceMessageBuf;
extern OSMesg				dummyMessage;
extern OSIoMesg				dmaIOMessageBuf;
extern OSMesg				dummyMesg;
extern OSIoMesg				dmaIOMessageBuf;

extern OSMesgQueue			main_msgQ;

extern unsigned int			TicksPerFrame;

extern int gfxTasks;

extern void					ApplyGlobalTransformations();
extern void					ClearViewing();
extern void					DrawGraphics(void *arg);
extern void					SetRenderMode();
extern void					SetupViewing();

extern int dontClearScreen;

extern short dontDoAnims;

extern char codeRunning;

void ControllerProc(void *arg);
extern void InitDisplayLists(void);

#endif	/* _LANGUAGE_C */

#endif	/* __BLOCK_H__ */
