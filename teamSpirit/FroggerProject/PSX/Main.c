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
#include "islpsi.h"


#include "Layout.h"
#include "World.h"
#include "Main.h"
#include "Sprite.h"
#include "frogger.h"
#include "frontend.h"
#include "Game.h"
//#include "Block.h"
#include "Actor2.h"

#include "timer.h"
#include "Water.h"
#include "BackDrop.h"
#include "menus.h"
#include "Eff_Draw.h"
#include "World_Eff.h"
//#include "sfx.h"//mmsound
#include "audio.h" //mmsfx
#include "ptexture.h"
#include "cr_lang.h"
#include "cam.h"
//#include "psxtongue.h"


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


//PC has this in their main
int drawLandscape = 1;
long textEntry = 0;	
char textString[255] = "";



//fixed gameSpeed = 4096;
//char quitMainLoop;
//unsigned long actFrameCount = 0;
//unsigned long lastActFrameCount = 0;
//ULONG	frame;
//VECTOR bbsrc, bbtar;


//char UseAAMode = 0;
//char UseZMode = 1;


int vsyncCounter = 0;

static void vsyncCallback()
{
	frame++;
	vsyncCounter++;
	SpuFlush(SPU_EVENT_ALL);
#if GOLDCD==0
	asm("break 1024");
#endif
}


TextureType *texture;


USHORT EXPLORE_black_CLUT;

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
//bbopt - lighting test
// 	GsSetLightMode(2);

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



unsigned long CRC;

TextureType *texture;

extern int polyCount;
//extern int countMakeUnit;
//extern int countQuatToPSXMatrix;

extern int rotatedObjects;
extern int scaledObjects;
extern int movedObjects;


int main ( )
{
	while ( 1 )
	{
		RAMstart = (unsigned long)__bss_orgend;


#if GOLDCD==1
		RAMsize = (0x1fff00 - RAMstart)-8192;
//		RAMsize = (0x7fff00 - RAMstart)-8192;
#else
		RAMsize = 6291264;
#endif

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

		MemCardInit(1);
		MemCardStart();
		padInitialise(1); // 0 = No multi tap support
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

		StartSound();//mmsfx


#define ENABLE_LANG_SEL 0
#if ENABLE_LANG_SEL==1
		languageInitialise();
		while(!DoneLangSel)
		{
			currentDisplayPage = (currentDisplayPage==displayPage)?(&displayPage[1]):(&displayPage[0]);
			ClearOTagR(currentDisplayPage->ot, 1024);
			currentDisplayPage->primPtr = currentDisplayPage->primBuffer;

			padHandler();
			languageFrame();

			DrawSync(0);
			VSync(2);
			PutDispEnv(&currentDisplayPage->dispenv);
			PutDrawEnv(&currentDisplayPage->drawenv);
			DrawOTag(currentDisplayPage->ot+(1024-1));
		}
#endif



		InitCam();

/*
#if GOLDCD==1
		{
			int bbRes;
			int endFrame = 877;
		//#if PALMODE==1
		//	StrDataType strInfo = { "\\VIDEO\\TOYFAIR.STR;1", STR_MODE16, STR_BORDERS_ON,  320, 0,32, 320,192, endFrame, 0, 127};
			StrDataType strInfo = { "\\TOYFAIR.STR;1", STR_MODE16, STR_BORDERS_ON,  320, 0,32, 320,192, endFrame, 0, 127};

			printf("Starting video..\n");
			bbRes = videoPlayStream(&strInfo, 0, &ZeroFunc);
			printf("%d\n",bbRes);
		}
#endif
*/



		actorInitialise();
		InitBackdrop ( "FROGGER2.RAW" );

		CommonInit();


//		InitWater();
//		LoadSfx(WORLDID_GENERIC);//mmsfx

		while ( !quitMainLoop )
		{
			//turn on/off timers + display
			if(padData.debounce[0] & PAD_SELECT)
				timerActive ^= 1;

			TIMER_START(TIMER_TOTAL);

			currentDisplayPage = (currentDisplayPage==displayPage)?(&displayPage[1]):(&displayPage[0]);
			ClearOTagR(currentDisplayPage->ot, 1024);
			currentDisplayPage->primPtr = currentDisplayPage->primBuffer;

			polyCount = 0;
			actorCount = 0;
//			countMakeUnit = 0;
//			countQuatToPSXMatrix = 0;

			rotatedObjects = 0;
			scaledObjects = 0;
			movedObjects = 0;


			
			//see if this does anything we need.
			//probabaly ok without it.
			//bb just found it in the psi libs
//			psiResetModelctrl();


			DrawBackDrop();


			//for timing optimised functions
			//remember to stop really timing GameLoop
/*			TIMER_START(TIMER_GAMELOOP);
			{
				int i;
// 				int res1, res2, res3, res4;
// 				SVECTOR sv1 = {10,10,10};
// 				SVECTOR sv2 = {23456,23456,23456};
// 				SVECTOR sv3 = {-10,-10,-10};
// 				SVECTOR sv4 = {-12345,-12345,-12345};
//  			VECTOR v1 = {10,10,10};
//  			VECTOR v2 = {23456,23456,23456};
//  			VECTOR v3 = {-10,-10,-10};
//  			VECTOR v4 = {-12345,-12345,-12345};
  				FVECTOR fv1 = {10<<12, 0, 23456<<12};
  				FVECTOR fv2 = {23456<<12, 0, -12345<<12};
  				FVECTOR fv3 = {-100<<12, 0, -12345<<12};
  				FVECTOR fv4 = {23456<<12, 0, 23456<<12};
//  				FVECTOR fv1 = {0, 0, 4096};
//  				FVECTOR fv2 = {4096, 0, };
//  				FVECTOR fv3 = {0, 0, -4096};
//  				FVECTOR fv4 = {-4096, 0, 0};
 				FVECTOR tempUp = {0,4096,0};
				MakeUnit(&fv1);
				MakeUnit(&fv2);
				MakeUnit(&fv3);
				MakeUnit(&fv4);
//			TIMER_START(TIMER_GAMELOOP);
				for(i=0; i<1000; i++)
				{
// 					res1 = Magnitude2DF(&v1);
// 					res2 = Magnitude2DF(&v2);
// 					res3 = Magnitude2DF(&v3);
// 					res4 = Magnitude2DF(&v4);
// 					MakeUnit(&fv1);
// 					MakeUnit(&fv2);
// 					MakeUnit(&fv3);
// 					MakeUnit(&fv4);
					Orientate(&frog[0]->actor->qRot, &fv1, &tempUp );
					Orientate(&frog[0]->actor->qRot, &fv2, &tempUp );
					Orientate(&frog[0]->actor->qRot, &fv3, &tempUp );
					Orientate(&frog[0]->actor->qRot, &fv4, &tempUp );
				}
			}
			TIMER_STOP(TIMER_GAMELOOP);
*/
			TIMER_START0(TIMER_GAMELOOP);
			GameLoop();
			TIMER_STOP0(TIMER_GAMELOOP);
			
			TIMER_START0(TIMER_UPDATE_WATER);
			UpdateWater();
			TIMER_STOP0(TIMER_UPDATE_WATER);

	//		if(spriteList.numEntries)
	//			AnimateSprites();
			

			TIMER_START0(TIMER_DRAW_WORLD);
			if(drawLandscape)
				DrawWorld();
			TIMER_STOP0(TIMER_DRAW_WORLD);

			TIMER_START0(TIMER_DRAW_SPECFX);
			DrawSpecialFX();
			TIMER_STOP0(TIMER_DRAW_SPECFX);

			TIMER_START0(TIMER_PRINT_SPRITES);
			PrintSprites();
			TIMER_STOP0(TIMER_PRINT_SPRITES);

			TIMER_START0(TIMER_DRAW_SCENICS);
			if(gameState.mode == INGAME_MODE || gameState.mode == FRONTEND_MODE)
				DrawScenicObjList();
			TIMER_STOP0(TIMER_DRAW_SCENICS);
			
			TIMER_START0(TIMER_DRAW_WATER);
			if(gameState.mode == INGAME_MODE || gameState.mode == FRONTEND_MODE)
				DrawWaterList();
			TIMER_STOP0(TIMER_DRAW_WATER);

			TIMER_START0(TIMER_ACTOR_DRAW);
			if(gameState.mode == INGAME_MODE || gameState.mode == FRONTEND_MODE)
				DrawActorList();
			TIMER_STOP0(TIMER_ACTOR_DRAW);

// 			if ( !( frameCount % 10 ) )
// 				utilPrintf ( "Poly Count : %d\n", polyCount );
			// ENDIF

			TIMER_START0(TIMER_PRINT_OVERS);
			PrintSpriteOverlays(1);
			PrintTextOverlays();
			PrintSpriteOverlays(0);
			TIMER_STOP0(TIMER_PRINT_OVERS);

			TIMER_START0(TIMER_PROCTEX);
			ProcessProcTextures( );
			TIMER_STOP0(TIMER_PROCTEX);


			if ( padData.digital[1] & PAD_DOWN )
			{
				camDist.vy += ( 20 * gameSpeed ) >> 12;
			}
			// ENDIF

			if ( padData.digital[1] & PAD_UP )
			{
				camDist.vy-=(20*gameSpeed)>>12;
			}
			// ENDIF

			if ( padData.digital[1] & PAD_LEFT )
			{
				camSideOfs+=20*gameSpeed;
			}
			// ENDIF
			if ( padData.digital[1] & PAD_RIGHT )
			{
				camSideOfs-=20*gameSpeed;
			}
			// ENDIF

			if ( padData.digital[1] & PAD_TRIANGLE )
			{
				camDist.vz+=(20*gameSpeed)>>12;
			}
			// ENDIF
			if ( padData.digital[1] & PAD_CROSS )
			{
				camDist.vz-=(20*gameSpeed)>>12;
			}
			// ENDIF


			timerDisplay();

//			utilPrintf("countMakeUnit %d\n", countMakeUnit);
//			utilPrintf("countQuatToPSXMatrix %d\n", countQuatToPSXMatrix);

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


			TIMER_START0(TIMER_DRAWSYNC);
			DrawSync(0);
			TIMER_STOP0(TIMER_DRAWSYNC);


			TIMER_STOP(TIMER_TOTAL);
			TIMER_ENDFRAME;
			TIMER_ZERO;

			VSync(2);
			PutDispEnv(&currentDisplayPage->dispenv);
			PutDrawEnv(&currentDisplayPage->drawenv);
			DrawOTag(currentDisplayPage->ot+(1024-1));

			if(gameState.mode!=PAUSE_MODE)
			{
				char tempText[64];

// 				actFrameCount += 3;//(GetTickCount()/(1000/60));
// 				gameSpeed=3<<12;

 				gameSpeed = vsyncCounter<<12;
 				actFrameCount += vsyncCounter;
 				vsyncCounter = 0;

 				sprintf(tempText, "% 2d frames  % 2d actors  % 4d polys",
 						gameSpeed>>12, actorCount, polyCount); 
 				fontPrint(fontSmall, -200,80, tempText, 200,128,128);

// 				sprintf(tempText, "% 4d rotated  % 4d scaled  % 4d moved",
// 						rotatedObjects, scaledObjects, movedObjects); 
// 				fontPrint(fontSmall, -200,80, tempText, 200,128,128);
			}

		}//end main loop


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




