#define VRAM_STARTX			512
#define VRAM_PAGECOLS		8
#define VRAM_PAGEROWS		2
#define VRAM_PAGES			16
#define VRAM_PAGEW			32
#define VRAM_PAGEH			32
#define VRAM_SETX(X)		(X)
#define VRAM_SETY(Y)		((Y)*VRAM_PAGEW)
#define VRAM_SETXY(X,Y)		((X)+((Y)*VRAM_PAGEW))
#define VRAM_SETPAGE(P)		((P)<<16)
#define VRAM_GETX(HND)		((HND) & (VRAM_PAGEW-1))
#define VRAM_GETY(HND)		(((HND)/VRAM_PAGEW) & (VRAM_PAGEW-1))
#define VRAM_GETXY(HND)		((HND) & 0xffff)
#define VRAM_GETPAGE(HND)	((HND)>>16)
#define VRAM_CALCVRAMX(HND)	(512+((VRAM_GETPAGE(HND)%(VRAM_PAGECOLS))*64)+(VRAM_GETX(HND)*2))
#define VRAM_CALCVRAMY(HND)	(((VRAM_GETPAGE(HND)/(VRAM_PAGECOLS))*256)+(VRAM_GETY(HND)*8))



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

//bb xa
#include <libcd.h>
#include <islxa.h>


#include "Layout.h"
#include "Main.h"
#include "Sprite.h"
#include "frogger.h"
#include "frontend.h"
#include "Game.h"
//#include "Block.h"
#include "Actor2.h"
#include "tongue.h"

#include "timer.h"
#include "Water.h"
#include "BackDrop.h"
#include "menus.h"
#include "Eff_Draw.h"
#include "World_Eff.h"
#include "World.h"
//#include "sfx.h"//mmsound
#include "audio.h" //mmsfx
#include "ptexture.h"
#include "cr_lang.h"
#include "cam.h"
#include "menus.h"
//#include "psxtongue.h"

#include "objviewer.h"
#include "platform.h"
#include "snapshot.h"
#include "psxsprite.h"
#include "memcard.h"
#include "lang.h"
#include "fadeout.h"

long turbo = 4096;

TextureAnimType* timerAnim = NULL;
int animFrame;

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
psFont *font = NULL;
psFont *fontSmall = NULL;


//PC has this in their main
int skipTextOverlaysSpecFX = NO;
int drawLandscape = 1;
long textEntry = 0;	
char textString[255] = "A--";

long drawGame = 1;

//fixed gameSpeed = 4096;
//char quitMainLoop;
//unsigned long actFrameCount = 0;
//unsigned long lastActFrameCount = 0;
//ULONG	frame;
//VECTOR bbsrc, bbtar;


//char UseAAMode = 0;
//char UseZMode = 1;

char objViewer = 0;


int vsyncCounter = 0;
char *saveicon = NULL;


void DisplayErrorMessage ( char *message )
{
 		currentDisplayPage = (currentDisplayPage==displayPage)?(&displayPage[1]):(&displayPage[0]);
 		ClearOTagR(currentDisplayPage->ot, 1024);
 		currentDisplayPage->primPtr = currentDisplayPage->primBuffer;


 		DrawSync(0);
 		VSync(2);
 		PutDispEnv(&currentDisplayPage->dispenv);
 		PutDrawEnv(&currentDisplayPage->drawenv);
 		DrawOTag(currentDisplayPage->ot+(1024-1));


		fontPrint(font, -80,0, message, 64,16,16);

 		currentDisplayPage = (currentDisplayPage==displayPage)?(&displayPage[1]):(&displayPage[0]);
 		ClearOTagR(currentDisplayPage->ot, 1024);
 		currentDisplayPage->primPtr = currentDisplayPage->primBuffer;


 		DrawSync(0);
 		VSync(2);
 		PutDispEnv(&currentDisplayPage->dispenv);
 		PutDrawEnv(&currentDisplayPage->drawenv);
 		DrawOTag(currentDisplayPage->ot+(1024-1));
}


#define SCREENOFF  SetDispMask(0);   
#define SCREENON   SetDispMask(1);     
//////////////////////////////////////////////////////////////////////
void MAIN_Flicker(const int num_flickers)
{
	int i;
	for (i=0;i<num_flickers;i++)
	{
		SCREENOFF;
		MAIN_Delay(100000);
		SCREENON;
		MAIN_Delay(100000);
	}

	// but now do a long pause
	MAIN_Delay(1000000);
}
//////////////////////////////////////////////////////////////////////
static void MAIN_Delay(const int t)
{
	int i=0;
	int j=0;

	while (i != t)
	{	// CS: do some shite to slow the PSX down
		j += i;
		j += utilSqrt(i);
		i++;
	}
}
//////////////////////////////////////////////////////////////////////




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

void videoInit ( int otDepth, int maxPrims, int flags)
{
	if ( maxPrims > 0 )
	{
		displayPage[0].ot = MALLOC0(otDepth*4);
		displayPage[1].ot = MALLOC0(otDepth*4);
		displayPage[0].primPtr = displayPage[0].primBuffer = MALLOC0(maxPrims*sizeof(POLY_GT4));
		displayPage[1].primPtr = displayPage[1].primBuffer = MALLOC0(maxPrims*sizeof(POLY_GT4));
	}

	if ( flags == VIDEO_INIT_AND_MALLOC )
	{
		VSync(0);
		ResetGraph(1);
		VSync(0);
		#if PALMODE==1
			utilPrintf("Setting Mode For PAL\n");
			SetVideoMode(MODE_PAL);
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
	}
	// ENDIF


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

//extern int rotatedObjects;
//extern int scaledObjects;
//extern int movedObjects;

int lastpolyCount=0;
int lastactorCount=0;
int maxInterpretTimer=0;

//used to save normal stack pointer,
//when using dchace for stack
char* oldStackPointer;

int main ( )
{
	static int frameAdvance = 0;
	TextureBankType *genBank;

	while ( 1 )
	{
		RAMstart = (unsigned long)__bss_orgend;


#if GOLDCD==1
		RAMsize = (0x1fff00 - RAMstart)-8192;
//		RAMsize = (0x7fff00 - RAMstart)-8192;
#else
		RAMsize = (0x1fff00 - RAMstart)-8192;
		//RAMsize = 6291264;
#endif

		memset((void *)0x1f8000,0,0x8000);

		utilPrintf("\nRAM start 0x%x  0x%x (%d)\n", RAMstart, RAMsize, RAMsize);
		memoryInitialise(RAMstart, RAMsize, 4096);
		utilPrintf ( "\n\nFROGGER2 PSX \n\n" );

		ResetCallback();

		utilInitCRC();
		utilSeedRandomInt(398623);


#if GOLDCD == NO
	fileInitialise("x:\\TEAMSPIRIT\\PSXVERSION\\CD\\");
	//fileInitialise("C:\\WORK\\FROGGERPROJECT\\PSX\\CODE\\CD\\");
	//XAsetStatus(CdInit());
#else
	fileInitialise("\\FROGGER.DAT;1");
	XAsetStatus(CdInit());
#endif



		//Init_BB_AcosTable();
		
		//bb - MUST INIT PAD BEFORE MEM CARD - SONY RULES!
		padInitialise(1); // 0 = No multi tap support
		MemCardInit(1);
		MemCardStart();
		videoInit ( 1024, 2400, VIDEO_INIT_AND_MALLOC );

		saveicon = fileLoad("saveicon.tim", NULL);
		if(!saveicon)
		{
			utilPrintf("ERROR! Could not load save icon\n");
			for(;;)
			{}
		}

//		fileInitialise("C:\\PSX\\FROGGER2\\CD\\");
//#if GOLDCD==0
//		XAenable = CdInit();
//#else
//		XAenable = 1;
//#endif

		textureInitialise ( 490, 50);

//		sfxInitialise();
//		sfxStartSound();

		// SL: Right... here, I make up and store the index for an all black palette, used to do true transparency...
		//EXPLORE_black_CLUT = textureAddCLUT16(EXPLORE_black_ref_palette);

		VSyncCallback(&vsyncCallback);

		StartSound();//mmsfx

		actFrameCount = 0;
#define ENABLE_LANG_SEL 0
#if ENABLE_LANG_SEL==1
		languageInitialise();
		while(!DoneLangSel)
		{
			currentDisplayPage = (currentDisplayPage==displayPage)?(&displayPage[1]):(&displayPage[0]);
			ClearOTagR(currentDisplayPage->ot, 1024);
			currentDisplayPage->primPtr = currentDisplayPage->primBuffer;

			padHandleInput();
			languageFrame();
			DrawScreenTransition();
			actFrameCount++;

			DrawSync(0);
			VSync(0);
			PutDispEnv(&currentDisplayPage->dispenv);
			PutDrawEnv(&currentDisplayPage->drawenv);
			DrawOTag(currentDisplayPage->ot+(1024-1));
		}
		actFrameCount = 0;
#endif
		LoadGame();
		
		if(saveInfo.saveFrame)
		{
			fontSmall = fontLoad("FONT12.FON");
			genBank = textureLoadBank("TEXTURES\\GENERIC.SPT");
			textureDownloadBank(genBank);
			textureDestroyBank(genBank);

			fontInitButtonSprites();
			gameTextInit("LANGUAGE.TXT", LANG_NUM_STRINGS, LANG_NUM_LANGUAGES, gameTextLang);
			InitTiledBackdrop("FROGTILE");
			memcpy(worldVisualData,origWorldVisualData,sizeof(worldVisualData));
			ScreenFade(0,255,20);
			keepFade = NO;
			while((saveInfo.saveFrame) || (fadingOut))
			{
				currentDisplayPage = (currentDisplayPage==displayPage)?(&displayPage[1]):(&displayPage[0]);
				ClearOTagR(currentDisplayPage->ot, 1024);
				currentDisplayPage->primPtr = currentDisplayPage->primBuffer;

				padHandleInput();

				if(actFrameCount > 20)
					ChooseLoadSave();
				DrawTiledBackdrop(NO);
				DrawScreenTransition();
				actFrameCount++;

				DrawSync(0);
				VSync(0);
				PutDispEnv(&currentDisplayPage->dispenv);
				PutDrawEnv(&currentDisplayPage->drawenv);
				DrawOTag(currentDisplayPage->ot+(1024-1));
				
				if((saveInfo.saveFrame == 0) && (keepFade == 0))
				{
					ScreenFade(255,0,20);
					keepFade = YES;
				}
			}
			FreeTiledBackdrop();
		}
	

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

//		drawGame = 0;

		actorInitialise();

		CommonInit();

//		loaded in CommonInit
//		LoadGame();

		//*****************//
		//*** MAIN LOOP ***//
		//*****************//

		while ( !quitMainLoop )
		{
			int i;

			//turn on/off timers + display
			if(padData.debounce[0] & PAD_SELECT)
			{
				timerActive ^= 1;
				frameAdvance ^= 1;
			}

			TIMER_START(TIMER_TOTAL);

			currentDisplayPage = (currentDisplayPage==displayPage)?(&displayPage[1]):(&displayPage[0]);
			ClearOTagR(currentDisplayPage->ot, 1024);
			currentDisplayPage->primPtr = currentDisplayPage->primBuffer;
			
			lastpolyCount =  polyCount;
			lastactorCount = actorCount;
			polyCount = 0;
			actorCount = 0;
			psiActorCount=0;
			fmaActorCount=0;


			worldPolyCount = 0;

			//restore the frog's depthshifts
			for(i=0; i<NUM_FROGS; i++)
			{
//				frog[i]->depthShift=0;
				frog[i]->depthShift=-10;
			}



//sbond - snapshot removed - triangle is now BACK on menus!!!!!!!!!!!!!!!!!!!
			if ( padData.debounce[1] & PAD_TRIANGLE )
			{
				SnapShot("C:\\");
			}

#if GOLDCD==0
			timerDisplay();
#endif

//		TIMER_START(TIMER_TOTAL);

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
TIMER_START(TIMER_GAMELOOP);
				for(i=0; i<1000; i++)
				{
// 					res1 = Magnitude2DF(&v1);
// 					res2 = Magnitude2DF(&v2);
// 					res3 = Magnitude2DF(&v3);
// 					res4 = Magnitude2DF(&v4);
				}
			}
TIMER_STOP(TIMER_GAMELOOP);
*/
			TIMER_START0(TIMER_GAMELOOP);
//			oldStackPointer = SetSp(0x1f800400);

			if ( !objViewer )
				GameLoop();
			else
				RunObjectViewer();
			// ENDIF


			if ( currPlatform[0] )
			{
				//bb - depthShift now zeroed each frame,
				//and added to as needed by plats/multiplayer frogon.
//				frog[0]->depthShift = 220;
//				frog[0]->depthShift += 10;
				frog[0]->depthShift -= 10;
			}
			// ENDIF

//			SetSp(oldStackPointer);
			TIMER_STOP0(TIMER_GAMELOOP);
			
			TIMER_START0(TIMER_UPDATE_WATER);
//			UpdateWater();
			TIMER_STOP0(TIMER_UPDATE_WATER);

	//		if(spriteList.numEntries)
	//			AnimateSprites();
			
//			textureSetAnimation ( timerAnim, animFrame++ );


/*	moveRect.x = VRAM_CALCVRAMX(timerAnim->anim[animFrame]->handle);
	moveRect.y = VRAM_CALCVRAMY(timerAnim->anim[animFrame]->handle);
	moveRect.w = (timerAnim->dest->w + 3) / 4;
	moveRect.h = timerAnim->dest->h;

	// check for 256 colour mode
	if(timerAnim->dest->tpage & (1 << 7))
		moveRect.w *= 2;

	// copy bit of vram
	BEGINPRIM(siMove, DR_MOVE);
	SetDrawMove(siMove, &moveRect, VRAM_CALCVRAMX(timerAnim->dest->handle),VRAM_CALCVRAMY(timerAnim->dest->handle));
	ENDPRIM(siMove, 1023, DR_MOVE);*/


			// JH:  Main Draw Function That Runs All The Draw Functions.
			if ( !objViewer )
				MainDrawFunction();


			TIMER_START0(TIMER_UPDATETEXANIM);
			UpdateTextureAnimations();
			TIMER_STOP0(TIMER_UPDATETEXANIM);

	
#if GOLDCD == NO
			if ( padData.digital[1] & PAD_L1 )
				DisplayOnScreenInfo();
			// ENDIF
#endif
			/*if ( ++animFrame == 8 )
				animFrame = 0;*/

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




#if GOLDCD == NO
	if ( gameState.multi == SINGLEPLAYER )
	{
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
			// ENDIF*/
			if ( padData.debounce[1] & PAD_SELECT )
			{
				if ( !objViewer )
					InitObjectViewer();
				else
					CommonInit();
				objViewer ^= 1;
			}
			// ENDIF
	}
	// ENDIF
#endif

	if(timerActive)
	{
		char tempText[128];
 		sprintf(tempText, "% 2d psiactors, %2d fmaactors",
 				psiActorCount, fmaActorCount); 
 		fontPrint(fontSmall, -200,40, tempText, 200,128,128);
	}

#if GOLDCD==0
			if ( padData.digital[1] & PAD_L1 )
			{
				char tempText[128];
 				sprintf(tempText, "% 2d frames  % 2d actors  % 4d polys  %d : TotalActors",
 						gameSpeed>>12, lastactorCount, lastpolyCount, actorsCount); 
 				fontPrint(fontSmall, -200,80, tempText, 200,128,128);

 				sprintf(tempText, "%2df", gameSpeed>>12); 
 				sprintf(tempText, "%df", totalObjs ); 
/* 				switch(gameSpeed>>12)
				{
					case 2:*/
						fontPrint(fontSmall, 0,-60, tempText, 64,255,64);
						/*break;
					case 3:
						fontPrint(fontSmall, 0,-60, tempText, 64,255,255);
						break;
					default:
						fontPrint(fontSmall, 0,-60, tempText, 255,64,64);
				}*/
				

			/*	sprintf(tempText, "% 2d frames  % 2d actors  % 4d maxintrpt",
 						gameSpeed>>12, lastactorCount, maxInterpretTimer); 
 				fontPrint(fontSmall, -200,80, tempText, 200,128,128);*/
			}

totalObjs = 0;
#endif

//			utilPrintf("countMakeUnit %d\n", countMakeUnit);
//			utilPrintf("countQuatToPSXMatrix %d\n", countQuatToPSXMatrix);

			gte_SetRotMatrix(&GsWSMATRIX);
			gte_SetTransMatrix(&GsWSMATRIX);

//			actorUpdateAnimations(frog[0]->actor);
//			actorSetAnimation(frog[0]->actor, frog[0]->actor->animation.frame);
//			actorDraw(frog[0]->actor);

#if GOLDCD==0
//			if (padData.debounce[0] & PAD_L1)
			if(camControlMode == 0)
			{
				if( (padData.debounce[0] & PAD_L1))
				{
					froggerShowVRAM(1);
				}
			}
#endif
			padHandleInput();
			if (padData.digital[4] == (PAD_SELECT|PAD_START|PAD_L1|PAD_R1|PAD_L2|PAD_R2))
				quitMainLoop = 1;



			if ( padData.digital[1] == PAD_L1 )
			{
				frameAdvance = 1;
			}

			if ( frameAdvance )
			{
				while ( frameAdvance )
				{
					padHandleInput();

					if ( padData.digital[0] == PAD_SELECT )
					{
						frameAdvance = 0;
					}

					if ( padData.debounce[0] == PAD_L2 )
					{
						break;
					}
				}
				// ENDIF
			}
			// ENDIF

			if((gameState.mode!=PAUSE_MODE) || (quittingLevel))
			{
				//bb
				lastActFrameCount = actFrameCount;

				gameSpeed = vsyncCounter<<12;

				if(gameSpeed > (5<<12))
					gameSpeed = (5<<12);

 				actFrameCount += gameSpeed>>12;
 				vsyncCounter = 0;
			}

		}//end main loop


		XAsetStatus(0);
		StopSound();

//		SaveGame();

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



void MainDrawFunction ( void )
{
	int i;

	TIMER_START0(TIMER_DRAW_WORLD);

	if ( drawLandscape && drawGame )
		DrawWorld();

	TIMER_STOP0(TIMER_DRAW_WORLD);

	if(!skipTextOverlaysSpecFX)
	{
		TIMER_START0(TIMER_DRAW_SPECFX);
		oldStackPointer = SetSp(0x1f800400);
		DrawSpecialFX();
		SetSp(oldStackPointer);
		TIMER_STOP0(TIMER_DRAW_SPECFX);
	}

	TIMER_START0(TIMER_PRINT_SPRITES);
	oldStackPointer = SetSp(0x1f800400);
	PrintSprites();
	SetSp(oldStackPointer);
	TIMER_STOP0(TIMER_PRINT_SPRITES);

	TIMER_START0(TIMER_DRAW_SCENICS);

	if ( drawLandscape && drawGame )
//	if ( /*( gameState.mode == INGAME_MODE || gameState.mode == FRONTEND_MODE ) &&*/ drawGame )
		DrawScenicObjList();

	TIMER_STOP0(TIMER_DRAW_SCENICS);
	
	//TIMER_START0(TIMER_DRAW_WATER);
	//if ( drawLandscape && drawGame )
//	if ( /*( gameState.mode == INGAME_MODE || gameState.mode == FRONTEND_MODE ) &&*/ drawGame )
		//DrawWaterList();
	//TIMER_STOP0(TIMER_DRAW_WATER);

	TIMER_START0(TIMER_ACTOR_DRAW);
	if ( /*( gameState.mode == INGAME_MODE || gameState.mode == FRONTEND_MODE ) &&*/ drawGame )
		DrawActorList();
	TIMER_STOP0(TIMER_ACTOR_DRAW);

	//bb - draw tongue.
	//Moved from DrawSpecFX because we need the frog
	//to have been drawn first, for latest position.
	for( i=0; i<NUM_FROGS; i++ )
		if( tongue[i].flags & TONGUE_BEINGUSED )
			DrawTongue( i );

	//if( gameState.multi == SINGLEPLAYER )
	{
		UpdateFrogTongue(0);
		UpdateFrogCroak(0);
	}

//	if ( !( frameCount % 10 ) )
//		utilPrintf ( "Poly Count : %d\n", polyCount );

	TIMER_START0(TIMER_PRINT_OVERS);

	if(tileTexture)
		DrawTiledBackdrop(saveInfo.saveFrame ? NO : YES);

//	PrintSpriteOverlays(1);
	//if ( padData.digital[0] == PAD_TRIANGLE )
	if(!skipTextOverlaysSpecFX)
	{
		PrintTextOverlays();
		PrintSpriteOverlays(0);
	}
	// ENDIF

	TIMER_STOP0(TIMER_PRINT_OVERS);

	TIMER_START0(TIMER_PROCTEX);
	//ProcessProcTextures( );
	TIMER_STOP0(TIMER_PROCTEX);

	DrawBackDrop(0, 0);

	DrawScreenTransition();
}


void SetCurrentDisplayPage ( int page1, int page2 )
{
	currentDisplayPage = ( currentDisplayPage == displayPage ) ?( &displayPage [ page1 ] ):( &displayPage [ page2 ] );
	ClearOTagR ( currentDisplayPage->ot, 1024 );
	currentDisplayPage->primPtr = currentDisplayPage->primBuffer;
}

void SendOTDisp ( void )
{
	DrawSync(0);
	VSync(2);
	PutDispEnv(&currentDisplayPage->dispenv);
	PutDrawEnv(&currentDisplayPage->drawenv);
	DrawOTag(currentDisplayPage->ot+(1024-1));
}

void ResetDrawingEnvironment ( void )
{
	SetGeomOffset(512/2, 256/2);
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
	displayPage[0].drawenv.isbg = displayPage[1].drawenv.isbg = 0;
	GsSetProjection(350);
	SetDispMask(1);
	VSync(0);
	SetGeomOffset(0,0);				//camera focused on middle of screen
}

void MainReset ( void )
{
		RAMstart = (unsigned long)__bss_orgend;


#if GOLDCD==1
		RAMsize = (0x1fff00 - RAMstart)-8192;
//		RAMsize = (0x7fff00 - RAMstart)-8192;
#else
	RAMsize = (0x1fff00 - RAMstart)-8192;
	//RAMsize = 6291264;
#endif

		utilPrintf("\nRAM start 0x%x  0x%x (%d)\n", RAMstart, RAMsize, RAMsize);
		memoryInitialise(RAMstart, RAMsize, 4096);

		utilPrintf ( "\n\nFROGGER2 PSX \n\n" );

		ResetCallback();
//		memset((void *)0x1f8000,0,0x8000);

		utilInitCRC();
		utilSeedRandomInt(398623);

#if GOLDCD == NO
	fileInitialise("x:\\TEAMSPIRIT\\PSXVERSION\\CD\\");
#else
	fileInitialise("\\FROGGER.DAT;1");
#endif

//		padInitialise(1); // 0 = No multi tap support
//		MemCardInit(1);
//		MemCardStart();
//		padInitialise(1); // 0 = No multi tap support
		videoInit ( 1024, 2400, 0 );
		textureInitialise ( 490, 50);
}


void DisplayOnScreenInfo ( void )
{
	static char whichSegment = 0;

	int i;
	char r, g, b;
	char tempText[128];

	FMA_MESH_HEADER **mesh;

	mesh = ADD2POINTER(fma_world,sizeof(FMA_WORLD));


	r = 255;
	g = 255;
	b = 255;

	fontPrint ( fontSmall, -220, -90, "WORLD POLY COUNT", r, g, b );

	r = 100;
	g = 100;
	b = 255;

 	sprintf ( tempText, "%d : Objs %d : %d", worldPolyCount, whichSegment, fma_world->n_meshes ); 

	fontPrint ( fontSmall, 20, -90, tempText, r, g, b );



	if ( padData.debounce[1] & PAD_R2 )
	{
		for ( i = fma_world->n_meshes; i != 0; i--, mesh++ )
		{
			(*mesh)->flags &= ~DRAW_SEGMENT;

			if ( whichSegment == i )
				(*mesh)->flags |= DRAW_SEGMENT;
		}
		// ENDFOR

		whichSegment++;

		if ( whichSegment > fma_world->n_meshes )
			whichSegment = fma_world->n_meshes;
		
	}
	// ENDIF

	if ( padData.debounce[1] & PAD_L2 )
	{
		whichSegment--;

		if ( whichSegment == 255 )
			whichSegment = 0;

		for ( i = fma_world->n_meshes; i != 0; i--, mesh++ )
		{
			(*mesh)->flags &= ~DRAW_SEGMENT;

			if ( whichSegment == i )
				(*mesh)->flags |= DRAW_SEGMENT;
		}
		// ENDFOR

	}
	// ENDIF

}



