#include "shell.h"

//#include <memory.h>

#include <stddef.h>

#include <libgte.h>
#include <libsn.h>
#include <libgpu.h>
#include <libgs.h>
#include <libetc.h>
#include <libapi.h>
#include <libspu.h>

#include <inline_c.h>
#include <gtemac.h>

#include <islutil.h>
#include <islcard.h>
#include <isltex.h>
#include <islfont.h>
//mmsound #include <islsound.h>
#include <islpad.h>
#include <islmem.h>
#include <islfile.h>
//#include <islpsi.h>
#include "sonylibs.h"
#include "shell.h"
#include <islpsi.h>


#include "Layout.h"
#include "World.h"
#include "Main.h"
#include "Sprite.h"
#include "frogger.h"
#include "frontend.h"
#include "Game.h"
//#include "Block.h"
#include "Actor2.h"

#include "bbtimer.h"
#include "Water.h"
#include "BackDrop.h"
#include "menus.h"
#include "Eff_Draw.h"
#include "World_Eff.h"
//#include "sfx.h"//mmsound
#include "audio.h" //mmsfx
#include "ptexture.h"


void customDrawPrimitives2(int);
void customDrawSortedPrimitives(int);
void LSCAPE_DrawSortedPrimitives(int);

#define PROJECTION 0x220

extern USHORT EXPLORE_black_ref_palette[16];
extern USHORT EXPLORE_black_CLUT;

GsRVIEW2	camera;
unsigned long	RAMstart, RAMsize;
static RECT VRAMarea = {0,0,1024,512};
extern char __bss_orgend[];
displayPageType displayPage[2], *currentDisplayPage;
psFont *font;
psFont *fontSmall;



//fixed gameSpeed = 4096;
//char quitMainLoop;
//unsigned long actFrameCount = 0;
//unsigned long lastActFrameCount = 0;
//ULONG	frame;
//VECTOR bbsrc, bbtar;


//char UseAAMode = 0;
//char UseZMode = 1;




static void vsyncCallback()
{
	frame++;
	SpuFlush(SPU_EVENT_ALL);
#if GOLDCD==0
	asm("break 1024");
#endif
}


TextureType *texture;


USHORT EXPLORE_black_CLUT;

void DrawPoly ( void )
{
	POLY_FT4 	*si; 

	register PACKET*		packet;

	si = (POLY_FT4*)packet;

	BEGINPRIM(si, POLY_FT4);

	setPolyFT4(si);

	si->x0 = 0;
	si->y0 = 0;

	si->x1 = 0+32;
	si->y1 = 0;

	si->x2 = 0;
	si->y2 = 0-32;

	si->x3 = 0+32;
	si->y3 = 0-32;

	si->r0 = 100;
	si->g0 = 100;
	si->b0 = 100;

	si->u0 = texture->u0;
	si->v0 = texture->v0;
	si->u1 = texture->u1;
	si->v1 = texture->v1;
	si->u2 = texture->u2;
	si->v2 = texture->v2;
	si->u3 = texture->u3;
	si->v3 = texture->v3;

//	si->code = GPU_COM_TF4;
	si->tpage = texture->tpage;
	si->clut = texture->clut;

	si->code |= 2;
	si->tpage |= 32;

	ENDPRIM(si, 1 & 1023, POLY_FT4);


	BEGINPRIM(si, POLY_FT4);

	setPolyFT4(si);

	si->x0 = 0;
	si->y0 = 0;

	si->x1 = 0+32;
	si->y1 = 0;

	si->x2 = 0;
	si->y2 = 0-32;

	si->x3 = 0+32;
	si->y3 = 0-32;

	si->r0 = 100;
	si->g0 = 100;
	si->b0 = 100;


	si->clut = EXPLORE_black_CLUT;
	si->code |= 2;
	si->tpage = texture->tpage | 64;

	ENDPRIM(si, 1 & 1023, POLY_FT4);




//	PUTPACKETINTABLE(si, (PolyList->org+((MAXDEPTH)>>PolyList->shift)-1), POLYTF4_LEN);
//	si++;
//	(POLY_FT4*)GsOUT_PACKET_P = si;



/*	register PACKET*		packet;

#define si ((POLY_FT4*)packet)

				short		spritez;
				int		width, height;

				static int i;


				si = (POLY_FT4*)packet;
//				op = (TMD_P_FT4I*)(opcd);

				BEGINPRIM(si, POLY_FT4);

					setPolyFT4(si);


	// scaling-and-transform-in-one-go code from the Action Man people...
			//	width = 64;
			//	gte_SetLDDQB(0);			// clear offset control reg (C2_DQB)
//				gte_ldv0(&vp[op->v0]);		// Load centre point
			//	gte_SetLDDQA(width);		// shove sprite width into control reg (C2_DQA)
			//	gte_rtps();					// do the rtps
			//	gte_stsxy(&si->x0);			// get screen x and y
			//	gte_stsz(&spritez);		// get order table z
	// end of scaling-and-transform


	// tbd - make this ditch according to on-screen SIZE
	// limit to "max poly depth", and we can ditch the "MAXDEPTH" "and" below...


//				if (spritez < 20)
//					break;

			//	gte_stopz(&width);		// get scaled width of sprite
			//	width >>= 17;

// 				*(u_long *) & si->r0 = *(u_long *) & op->r0;			// Texture coords / colors
				si->r0 = 200;
				si->g0 = 200;
				si->b0 = 200;

				si->u0 = 0;
				si->v0 = 0;

				si->u1 = 32;
				si->v1 = 0;

				si->u2 = 0;
				si->v2 = 32;

				si->u3 = 32;
				si->v3 = 32;

				si->tpage = GetTPage ( 0, 0, 0, 0 );

		//		*(u_long *) & si->u1 = *(u_long *) & op->tu1;
		//		*(u_long *) & si->u2 = *(u_long *) & op->tu2;
		//		*(u_long *) & si->u3 = *(u_long *) & op->tu3;


		//		si->x1 = si->x3=si->x0+width;
		//		si->x0 = si->x2=si->x0-width;
			
 		 //		height = width>>1;//>>1;//(LONG)(width*(3))/spritez;
			
		//		si->y2 = si->y3=si->y0+height;
		//		si->y0 = si->y1=si->y0-height;


// Test
				si->x0 = 0;
				si->y0 = 0;

				si->x1 = si->x0+32;
				si->y1 = si->y0;

				si->x2 = si->x0;
				si->y2 = si->y0-32;

				si->x3 = si->x0+32;
				si->y3 = si->y0-32;
// Test

				si->code = GPU_COM_TF4;
		
//	gte_stsxy3_gt4(si);		//store in POLY_GT4 structure location

//				si->code = op->cd | modctrl->semitrans;

//				depth = 1;
//				gte_stsxy3_ft4(si);

	 				ENDPRIM(si, 1 & 1023, POLY_FT4);



				//	op = op->next;/*	register PACKET*		packet;

/*	#define si ((POLY_FT4*)packet)

	BEGINPRIM(si, POLY_FT4);

	*(u_long *)  (&si->u0) = 0;		// Texture coords

		
	*(u_long *)  (&si->u1) = 0;

	ENDPRIM(si, depth, POLY_FT4);*/
}





void *memoryAllocateZero(unsigned long size, char *file, int line)
{
	void *ptr;
	ptr=memoryAllocate(size, file, line);
	if(ptr)
		memset(ptr, 0, size);

	return ptr;
}

void videoInit(int otDepth, int maxPrims)
{
	if (maxPrims>0)
	{
//		displayPage[0].ot = MALLOC0(otDepth*4);
//		displayPage[1].ot = MALLOC0(otDepth*4);
//		displayPage[0].primPtr = displayPage[0].primBuffer = MALLOC0(maxPrims*sizeof(POLY_GT4));
//		displayPage[1].primPtr = displayPage[1].primBuffer = MALLOC0(maxPrims*sizeof(POLY_GT4));
		displayPage[0].ot = MALLOC0(otDepth*4);
		displayPage[1].ot = MALLOC0(otDepth*4);
		displayPage[0].primPtr = displayPage[0].primBuffer = MALLOC0(maxPrims*sizeof(POLY_GT4));
		displayPage[1].primPtr = displayPage[1].primBuffer = MALLOC0(maxPrims*sizeof(POLY_GT4));
	}

	VSync(0);
	ResetGraph(1);
	VSync(0);
#if PALMODE==1
	utilPrintf("Setting Mode For PAL\n");
	SetVideoMode(MODE_PAL);
//	GsInitGraph(512,273, 4,1,0);
	GsInitGraph(512,273, 4,1,0);
	GsInit3D();
	SetGeomOffset(512/2, 256/2);
#else
	utilPrintf("Setting Mode For NTSC\n");
	SetVideoMode(MODE_NTSC);
	GsInitGraph(512,240, 4,1,0);
	GsInit3D();
	SetGeomOffset(512/2, 240/2);
#endif
	SetDefDrawEnv(&displayPage[0].drawenv, 0,   0, 512,256);
	SetDefDrawEnv(&displayPage[1].drawenv, 0, 256, 512,256);
	SetDefDispEnv(&displayPage[0].dispenv, 0, 256, 512,256);
	SetDefDispEnv(&displayPage[1].dispenv, 0,   0, 512,256);

//	displayPage[0].dispenv.screen.y = displayPage[1].dispenv.screen.y = 8+PALMODE*12;
//	displayPage[0].dispenv.screen.h = displayPage[1].dispenv.screen.h = 240+PALMODE*16;

#if PALMODE==1
	displayPage[0].dispenv.screen.y = displayPage[1].dispenv.screen.y = 8+PALMODE*12;
	displayPage[0].dispenv.screen.h = displayPage[1].dispenv.screen.h =256;	// SetDefDispEnv always sets it to 240
#else
	displayPage[0].dispenv.screen.y = displayPage[1].dispenv.screen.y = 0;
	displayPage[0].dispenv.screen.h = displayPage[1].dispenv.screen.h =240;	// SetDefDispEnv always sets it to 240
#endif

//	displayPage[0].dispenv.screen.y = displayPage[1].dispenv.screen.y = 24;

  	displayPage[0].drawenv.ofs[0] = displayPage[1].drawenv.ofs[0] = 512/2;
  	displayPage[0].drawenv.ofs[1] = 120+PALMODE*8;
	displayPage[1].drawenv.ofs[1] = 256+120+PALMODE*8;
	displayPage[0].drawenv.isbg = displayPage[1].drawenv.isbg = 1;
	GsSetProjection(1000);
	SetDispMask(1);
	VSync(0);
}

void SetCam(VECTOR src, VECTOR tar)
{
	camera.vpx = src.vx;
	camera.vpy = src.vy;
	camera.vpz = src.vz;

	camera.vrx = tar.vx;
	camera.vry = tar.vy;
	camera.vrz = tar.vz;

	camera.rz  = 0;

	GsSetRefView2(&camera);

	gte_SetRotMatrix(&GsWSMATRIX);
	gte_SetTransMatrix(&GsWSMATRIX);
}

void InitCam(void)
{
	// Set up an ambient light
	GsSetAmbient(128*16, 128*16, 128*16);
 	GsSetLightMode(2);

// 	psiInitialise();
// 	psiInitLights();
// 	psiSetAmbient(128,128,128);



	// Set up a simple camera
	GsSetProjection(350);	//default distance of projection; alters perspective
	SetGeomOffset(0,0);				//camera focused on middle of screen

//	camera.vpx = 0; camera.vpy = -0x1000; camera.vpz = -0x1200;

//	camera.vpx = 0; camera.vpy = -500; camera.vpz = 8000;
//	camera.vrx = 0; camera.vry = 0; camera.vrz = 0;

//	camera.vpx = -500; camera.vpy = -1500; camera.vpz = 14000;
//	camera.vrx = -500; camera.vry = 0; camera.vrz = 9750;

// 	camera.vpx = 0; camera.vpy = 0; camera.vpz = 100;
// 	camera.vrx = 0; camera.vry = 0; camera.vrz = 0;


//    	bbsrc.vx = -50;
//    	bbsrc.vy = -600;
//    	bbsrc.vz = -1500;
//   
//    	bbtar.vx = -50;
//    	bbtar.vy = 0;
//    	bbtar.vz = -1000;

//     	bbsrc.vx = -2500;
//     	bbsrc.vy = -6000;
//     	bbsrc.vz = -15000;
//    
//     	bbtar.vx = -500;
//     	bbtar.vy = 0;
//     	bbtar.vz = -10000;

    	bbsrc.vx = 0;
    	bbsrc.vy = -6000;
    	bbsrc.vz = 18000;
   
    	bbtar.vx = 500;
    	bbtar.vy = 0;
    	bbtar.vz = 9750;


	SetCam(bbsrc,bbtar);


// 	camera.vpx = -50; camera.vpy = -600; camera.vpz = -1500;
// 	camera.vrx = -50; camera.vry = 0; camera.vrz = -1000;
// 	camera.rz  = 0;
// 
// 	GsSetRefView2(&camera);
// 
// 	gte_SetRotMatrix(&GsWSMATRIX);
// 	gte_SetTransMatrix(&GsWSMATRIX);
}


//bbtest
//int bbTime1;
BBTIMER tGameLoop, tDrawWorld, tPrintSprites, tDrawActorList, tDrawWaterList;

unsigned long CRC;

TextureType *texture;

extern int polyCount;

int main ( )
{
	while ( 1 )
	{
		RAMstart = (unsigned long)__bss_orgend;
		RAMsize = (0x1fff00 - RAMstart)-8192;
		utilPrintf("\nRAM start 0x%x  0x%x (%d)\n", RAMstart, RAMsize, RAMsize);
		memoryInitialise(RAMstart, RAMsize, 4096);

		utilPrintf ( "\n\nFROGGER2 PSX \n\n" );

		ResetCallback();
		memset((void *)0x1f8000,0,0x8000);

		utilInitCRC();
		utilSeedRandomInt(398623);

#if GOLDCD == NO
	fileInitialise("x:\\TEAMSPIRIT\\PSXVERSION\\CD\\");
		utilPrintf ( "\n\nFROGGER2 PSX \n\n" );
#else
	fileInitialise("\\FROGGER.DAT;1");
		utilPrintf ( "\n\nFROGGER2 PSX \n\n" );
#endif
//		fileInitialise("C:\\PSX\\FROGGER2\\CD\\");
//#if GOLDCD==0
//		XAenable = CdInit();
//#else
//		XAenable = 1;
//#endif

		Init_BB_AcosTable();

//		MemCardInit(1);
//		MemCardStart();
		padInitialise(0); // 0 = No multi tap support
		videoInit(1024, 3000);
		textureInitialise(500, 20);


//		sfxInitialise();
//		sfxStartSound();

		// SL: Right... here, I make up and store the index for an all black palette, used to do true transparency...
		EXPLORE_black_CLUT = textureAddCLUT16(EXPLORE_black_ref_palette);

		VSyncCallback(&vsyncCallback);
//		font = fontLoad("FONT12.FON");
// 		fontSmall = fontLoad("FONT12.FON");

		font = fontLoad("FONTL.FON");
		fontSmall = fontLoad("FONTS.FON");

		InitCam();
		actorInitialise();
		InitBackdrop ( "FROGGER2.RAW" );

		CommonInit();


		StartSound();//mmsfx
//		LoadSfx(WORLDID_GENERIC);//mmsfx



		while ( !quitMainLoop )
		{
			ZeroTime();
//			fontPrint(font, 20,20, "WELLO", 255,255,255);


			currentDisplayPage = (currentDisplayPage==displayPage)?(&displayPage[1]):(&displayPage[0]);
			ClearOTagR(currentDisplayPage->ot, 1024);
			currentDisplayPage->primPtr = currentDisplayPage->primBuffer;


			polyCount = 0;

			DrawBackDrop();

// 			bbTime1 = GetRCnt(1);
// 			ResetRCnt(1);

			TimerStart(&tGameLoop);
			GameLoop();
			TimerStop(&tGameLoop);
			
	//		if(spriteList.numEntries)
	//			AnimateSprites();
			

			TimerStart(&tDrawWorld);
//			DrawWorld();
			if(gameState.mode == INGAME_MODE || gameState.mode == FRONTEND_MODE)
				DrawWorld();
			TimerStop(&tDrawWorld);

			DrawSpecialFX();

			TimerStart(&tPrintSprites);
			PrintSprites();
			TimerStop(&tPrintSprites);

			TimerStart(&tDrawWaterList);
			if(gameState.mode == INGAME_MODE || gameState.mode == FRONTEND_MODE)
				DrawScenicObjList();
			TimerStop(&tDrawWaterList);
			
			TimerStart(&tDrawWaterList);
			if(gameState.mode == INGAME_MODE || gameState.mode == FRONTEND_MODE)
				DrawWaterList();
			TimerStop(&tDrawWaterList);

			TimerStart(&tDrawActorList);
			if(gameState.mode == INGAME_MODE || gameState.mode == FRONTEND_MODE)
				DrawActorList();
			TimerStop(&tDrawActorList);

//			utilPrintf ( "Poly Count : %d\n", polyCount );

			PrintSpriteOverlays(1);
			PrintTextOverlays();
			PrintSpriteOverlays(0);


			ProcessProcTextures( );

			gte_SetRotMatrix(&GsWSMATRIX);
			gte_SetTransMatrix(&GsWSMATRIX);

//			actorUpdateAnimations(frog[0]->actor);
//			actorSetAnimation(frog[0]->actor, frog[0]->actor->animation.frame);
//			actorDraw(frog[0]->actor);

#if GOLDCD==0
//			if (padData.debounce[0] & PAD_L1)
			if( (padData.debounce[0] & PAD_L1))
			{
//				textureShowVRAM(1); // 1 = PAL mode
				froggerShowVRAM(1);
				continue;
			}
#endif
			padHandler();
			if (padData.digital[4] == (PAD_SELECT|PAD_START|PAD_L1|PAD_R1|PAD_L2|PAD_R2))
				quitMainLoop = 1;

	//	DrawPoly();				Just testing....

			DrawSync(0);

			VSync(2);
			PutDispEnv(&currentDisplayPage->dispenv);
			PutDrawEnv(&currentDisplayPage->drawenv);
			DrawOTag(currentDisplayPage->ot+(1024-1));

//bb
//			actFrameCount += 1;//(GetTickCount()/(1000/60));
//			frameCount++;

//			actFrameCount += 3;//(GetTickCount()/(1000/60));
//			gameSpeed=3<<12;
			if(gameState.mode!=PAUSE_MODE)
			{
				actFrameCount += 3;//(GetTickCount()/(1000/60));
				gameSpeed=3<<12;
			}

		}
		// ENDWHILE

		StopSound();

		utilPrintf("\nFROGGER2 QUIT/RESET\n");
		DrawSync(0);
		ClearImage2(&VRAMarea, 0,0,0);

//		sfxDestroy();//mmsound
//		sfxStopSound();//mmsound

		StopCallback();

		ResetGraph(3);
		VSync(10);
	}
	// ENDWHILE

	return 0;
}




