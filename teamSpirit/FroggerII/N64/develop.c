/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.
	
	  
	File		: develop.c
	Programmer	: Andrew Eder
	Date		: 22/2/99
	
----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

#include <ultra64.h>

#include "incs.h"


#define MAXMENUOPTIONS	5

TEXTOVERLAY *testTxt = NULL;

char runningDevStuff = 0;
char runningWaterStuff = 0;

Vtx verts[32];
Vtx *vtxp;

void RunTestRoutine1();
void DrawTestWater();

/*	--------------------------------------------------------------------------------
	Function		: RunDevelopmentRoutines
	Purpose			: for testing various development routines
	Parameters		: 
	Returns			: void
	Info			: 
*/
void RunDevelopmentMenu()
{
	static unsigned long currentSelection = 0;
	static u16 button, lastbutton;
	static TEXTOVERLAY *objView,*andysplay,*recordkey,*levelSel,*dev;
	SPRITEOVERLAY *sprOver;
	int i = 0, xPos, j;

	if ( frameCount == 1 )
	{
		StopDrawing("devmenu");
		FreeAllLists();

		LoadTextureBank(SYSTEM_TEX_BANK);
		LoadTextureBank(GENERIC_TEX_BANK);

		currFont = smallFont;

		objView		= CreateAndAddTextOverlay(0,122,"object viewer",YES,NO,127,255,127,91,currFont,TEXTOVERLAY_WAVECHARS,1,0);
		andysplay	= CreateAndAddTextOverlay(0,140,"sound player",YES,NO,127,255,127,91,currFont,TEXTOVERLAY_WAVECHARS,1,3.14/8);
		recordkey	= CreateAndAddTextOverlay(0,158,"record keying",YES,NO,127,255,127,91,currFont,TEXTOVERLAY_WAVECHARS,1,3.14/4);
		levelSel	= CreateAndAddTextOverlay(0,176,"world / level selection",YES,NO,127,255,127,91,currFont,TEXTOVERLAY_WAVECHARS,1,3.14/2);
		dev			= CreateAndAddTextOverlay(0,194,"testing area thingy",YES,NO,127,255,127,91,currFont,TEXTOVERLAY_WAVECHARS,1,3.14/3);

		// add the texture tiles that comprise the Frogger2 logo / Hasbro logo....
		sprOver = CreateAndAddSpriteOverlay(98,20,"flogo01.bmp",32,32,255,255,255,255,0);
		sprOver = CreateAndAddSpriteOverlay(130,20,"flogo02.bmp",32,32,255,255,255,255,0);
		sprOver = CreateAndAddSpriteOverlay(162,20,"flogo03.bmp",32,32,255,255,255,255,0);
		sprOver = CreateAndAddSpriteOverlay(194,20,"flogo04.bmp",32,32,255,255,255,255,0);
		sprOver = CreateAndAddSpriteOverlay(98,52,"flogo05.bmp",32,32,255,255,255,255,0);
		sprOver = CreateAndAddSpriteOverlay(130,52,"flogo06.bmp",32,32,255,255,255,255,0);
		sprOver = CreateAndAddSpriteOverlay(162,52,"flogo07.bmp",32,32,255,255,255,255,0);
		sprOver = CreateAndAddSpriteOverlay(194,52,"flogo08.bmp",32,32,255,255,255,255,0);
		sprOver = CreateAndAddSpriteOverlay(162,84,"flogo09.bmp",32,32,255,255,255,255,0);
		sprOver = CreateAndAddSpriteOverlay(194,84,"flogo10.bmp",32,32,255,255,255,255,0);

		sprOver = CreateAndAddSpriteOverlay(24,150,"haslg001.bmp",32,32,255,255,255,128,0);
		sprOver = CreateAndAddSpriteOverlay(56,150,"haslg002.bmp",32,32,255,255,255,128,0);
		sprOver = CreateAndAddSpriteOverlay(24,182,"haslg003.bmp",32,32,255,255,255,128,0);
		sprOver = CreateAndAddSpriteOverlay(56,182,"haslg004.bmp",32,32,255,255,255,128,0);

		sprOver = CreateAndAddSpriteOverlay(200,185,"isl1.bmp",32,32,255,255,255,128,0);
		sprOver = CreateAndAddSpriteOverlay(232,185,"isl2.bmp",32,32,255,255,255,128,0);
		sprOver = CreateAndAddSpriteOverlay(264,185,"isl3.bmp",32,32,255,255,255,128,0);
		
		ResetParameters();

		runningDevStuff = 1;
	
		StartDrawing("devmenu");
	}
	// ENDIF - frameCount == 1

	button = controllerdata[ActiveController].button;

	if((button & CONT_UP) && !(lastbutton & CONT_UP))
    {
		if(currentSelection > 0)
		{
			currentSelection--;
			PlaySample ( 237,NULL,255,128);
		}
	}
	    
	if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
    {
		if(currentSelection < (MAXMENUOPTIONS-1))
		{
			currentSelection++;
			PlaySample ( 237,NULL,255,128);
		}
	}

	if(frameCount > 15)
	{
		if((button & CONT_B) && !(lastbutton & CONT_B))
		{
			FreeAllLists();
			frontEndState.mode = TITLE_MODE;
			runningDevStuff = 0;
			frameCount = 0;
			lastbutton = 0;
			return;
		}

		if (((button & CONT_A) && !(lastbutton & CONT_A)) ||
		   ((button & CONT_START) && !(lastbutton & CONT_START)))
		{
			runningDevStuff = 0;
			PlaySample ( 2,NULL,255,128);
			frameCount = 0;
			lastbutton = 0;

			switch (currentSelection)
			{
				case 0:   // objviewer
					FreeAllLists();
					frontEndState.mode = OBJVIEW_MODE;
					FreeBackdrop(myBackdrop);
					return;
				case 1:  // andysplay pen
					FreeAllLists();
//					frontEndState.mode = SNDVIEW_MODE;
					FreeBackdrop(myBackdrop);
					return;
				case 2:  // record keying
					FreeAllLists();
					InitLevel ( 2,0 );
					gameState.mode = RECORDKEY_MODE;
//					recordKeying = 1;
					FreeBackdrop(myBackdrop);
					return;
				case 3:  // level selection
					FreeAllLists();
					gameState.mode = LEVELSELECT_MODE;
					FreeBackdrop(myBackdrop);
					return;
				case 4:  // test area
					FreeAllLists();
//					frontEndState.mode = DEVELOPMENT_MODE;
					FreeBackdrop(myBackdrop);
					return;
			}
		}			
	
		switch (currentSelection)
		{
			case 0:
				objView->a = 255 * fabs(sinf(frameCount/12.5));
				objView->waveAmplitude = 6.0F;
				andysplay->a = 91; //255;
				recordkey->a = 91; //255;				
				levelSel->a = 91; //255;				
				dev->a = 91; //255;				
				
				andysplay->r = andysplay->b = 127;
				andysplay->g = 255;
				recordkey->r = recordkey->b = 127;
				recordkey->g = 255;
				levelSel->r = levelSel->b = 127;
				levelSel->g = 255;
				dev->r = dev->b = 127;
				dev->g = 255;
				objView->r = objView->g = objView->b = 255;

				if(andysplay->waveAmplitude > 1)
					andysplay->waveAmplitude -= 0.10F;
				if(recordkey->waveAmplitude > 1)
					recordkey->waveAmplitude -= 0.10F;
				if(levelSel->waveAmplitude > 1)
					levelSel->waveAmplitude -= 0.10F;
				if(dev->waveAmplitude > 1)
					dev->waveAmplitude -= 0.10F;
				break;
			
			case 1:
				andysplay->a = 255 * fabs(sinf(frameCount/12.5));
				andysplay->waveAmplitude = 6.0F;
				objView->a = 91; //255;				
				recordkey->a = 91; //255;				
				levelSel->a = 91; //255;				
				dev->a = 91; //255;				

				objView->r = objView->b = 127;
				objView->g = 255;
				recordkey->r = recordkey->b = 127;
				recordkey->g = 255;
				levelSel->r = levelSel->b = 127;
				levelSel->g = 255;
				dev->r = dev->b = 127;
				dev->g = 255;
				andysplay->r = andysplay->g = andysplay->b = 255;

				if(objView->waveAmplitude > 1)
					objView->waveAmplitude -= 0.10F;
				if(recordkey->waveAmplitude > 1)
					recordkey->waveAmplitude -= 0.10F;
				if(levelSel->waveAmplitude > 1)
					levelSel->waveAmplitude -= 0.10F;
				if(dev->waveAmplitude > 1)
					dev->waveAmplitude -= 0.10F;
				break;

			case 2:
				recordkey->a = 255  * fabs(sinf(frameCount/12.5));				
				recordkey->waveAmplitude = 6.0F;
				objView->a = 91; //255;				
				andysplay->a = 91; //255;				
				levelSel->a = 91; //255;				
				dev->a = 91; //255;				

				objView->r = objView->b = 127;
				objView->g = 255;
				andysplay->r = andysplay->b = 127;
				andysplay->g = 255;
				levelSel->r = levelSel->b = 127;
				levelSel->g = 255;
				dev->r = dev->b = 127;
				dev->g = 255;
				recordkey->r = recordkey->g = recordkey->b = 255;

				if(objView->waveAmplitude > 1)
					objView->waveAmplitude -= 0.10F;
				if(andysplay->waveAmplitude > 1)
					andysplay->waveAmplitude -= 0.10F;
				if(levelSel->waveAmplitude > 1)
					levelSel->waveAmplitude -= 0.10F;
				if(dev->waveAmplitude > 1)
					dev->waveAmplitude -= 0.10F;
				break;

			case 3:
				levelSel->a = 255  * fabs(sinf(frameCount/12.5));				
				levelSel->waveAmplitude = 6.0F;
				objView->a = 91; //255;				
				andysplay->a = 91; //255;				
				recordkey->a = 91; //255;				
				dev->a = 91; //255;				

				objView->r = objView->b = 127;
				objView->g = 255;
				andysplay->r = andysplay->b = 127;
				andysplay->g = 255;
				recordkey->r = recordkey->b = 127;
				recordkey->g = 255;
				dev->r = dev->b = 127;
				dev->g = 255;
				levelSel->r = levelSel->g = levelSel->b = 255;

				if(andysplay->waveAmplitude > 1)
					andysplay->waveAmplitude -= 0.10F;
				if(recordkey->waveAmplitude > 1)
					recordkey->waveAmplitude -= 0.10F;
				if(objView->waveAmplitude > 1)
					objView->waveAmplitude -= 0.10F;
				if(dev->waveAmplitude > 1)
					dev->waveAmplitude -= 0.10F;
				break;

			case 4:
				dev->a = 255  * fabs(sinf(frameCount/12.5));				
				dev->waveAmplitude = 6.0F;
				objView->a = 91; //255;				
				andysplay->a = 91; //255;				
				recordkey->a = 91; //255;				
				levelSel->a = 91; //255;				

				objView->r = objView->b = 127;
				objView->g = 255;
				andysplay->r = andysplay->b = 127;
				andysplay->g = 255;
				recordkey->r = recordkey->b = 127;
				recordkey->g = 255;
				levelSel->r = levelSel->b = 127;
				levelSel->g = 255;
				dev->r = dev->g = dev->b = 255;

				if(andysplay->waveAmplitude > 1)
					andysplay->waveAmplitude -= 0.10F;
				if(recordkey->waveAmplitude > 1)
					recordkey->waveAmplitude -= 0.10F;
				if(objView->waveAmplitude > 1)
					objView->waveAmplitude -= 0.10F;
				if(levelSel->waveAmplitude > 1)
					levelSel->waveAmplitude -= 0.10F;
				break;
		}
	}

	lastbutton = button;
}



/*	--------------------------------------------------------------------------------
	Function		: RunDevelopmentRoutines
	Purpose			: for testing various development routines
	Parameters		: 
	Returns			: void
	Info			: 
*/
void RunDevelopmentRoutines()
{
	static u16 button,lastbutton;
	
	if(frameCount == 1)
	{
		StopDrawing("devlp");
		LoadTextureBank(SYSTEM_TEX_BANK);
		LoadTextureBank(GENERIC_TEX_BANK);
		myBackdrop	= CreateAndInitBackdrop("n64.bmp");

		ResetParameters();

		testTxt = CreateAndAddTextOverlay(20,20,"Developing",NO,NO,255,255,255,255,smallFont,TEXTOVERLAY_WAVECHARS,3,0);

		StartDrawing("devlp");
	}

	button = controllerdata[ActiveController].button;

	if(frameCount>15)
	{
		if((button & CONT_B) && !(lastbutton & CONT_B))
		{
			FreeAllLists();
			frameCount = 0;
			lastbutton = 0;
			gameState.mode = TITLE_MODE;
			return;
		}

		if((button & CONT_C) && !(lastbutton & CONT_C))
		{
		}
		if((button & CONT_D) && !(lastbutton & CONT_D))
		{
		}
		if((button & CONT_E) && !(lastbutton & CONT_E))
		{
		}
	}

	lastbutton = button;
}


long t1 = 0;
long t2 = 0;
long t3 = 1024;
long t4 = 0;
long t5 = 1024;
long t6 = 1024;
long t7 = 0;
long t8 = 1024;

long scalex = 0xffff;
long scaley = 0xffff;
long waterScalar = 14336;

long devX = 50,devY = 50;
float devPhaseInc = 0.02F;
long devTrans1 = 90;
long devTrans2 = 130;

float waterOffset = -274.0F;

long tOffset1 = 512;
long tOffset2 = 512;
long tOffset3 = 1024;
long tOffset4 = 1024;
long tOffset5 = 640;
long tOffset6 = 640;
long tOffset7 = 512;
long tOffset8 = 512;


void RunTestRoutine1()
{
	static float seed = 0.0F;
	TEXTURE *myTxtr1 = NULL;
	TEXTURE *myTxtr2 = NULL;
	float sinCalc,cosCalc;
	SetRenderMode();
			
	// set vertex ptr to start of vertices array
	vtxp = &verts[0];

	sinCalc = sinf(seed);
	cosCalc = cosf(seed);

	t1 = (sinCalc * tOffset1);
	t2 = (sinCalc * tOffset2);
	t3 = 2048 + (cosCalc * tOffset3);
	t4 = (sinCalc * tOffset4);
	t5 = 2048 + (sinCalc * tOffset5);
	t6 = 2048 + (cosCalc * tOffset6);
	t7 = (sinCalc * tOffset7);
	t8 = 2048 + (sinCalc * tOffset8);

	gDPSetTextureFilter(glistp++,G_TF_BILERP);
	gDPSetTexturePersp(glistp++,G_TP_PERSP);
	gDPSetColorDither(glistp++, G_CD_MAGICSQ);
	gSPTexture(glistp++,scalex,scaley,G_TX_RENDERTILE,0,G_ON);
	gDPSetTextureLUT(glistp++,G_TT_RGBA16);
	gSPClearGeometryMode(glistp++,G_CULL_BOTH | G_LIGHTING);
    gSPSetGeometryMode(glistp++,G_SHADE | G_SHADING_SMOOTH);	
	gDPSetCombineMode(glistp++,G_CC_MODULATERGBA,G_CC_MODULATERGBA);
	gDPSetRenderMode(glistp++,G_RM_ADD,G_RM_ADD2);

	FindTexture(&myTxtr1,UpdateCRC("water2.bmp"),YES,"water2.bmp");
	FindTexture(&myTxtr2,UpdateCRC("water1.bmp"),YES,"water1.bmp");
	LoadTexture(myTxtr1);

	gDPSetTextureLUT(glistp++,G_TT_RGBA16);
	gDPLoadTLUT_pal256(glistp++,myTxtr1->palette);
	gDPLoadTextureBlock(glistp++,myTxtr1->data,G_IM_FMT_IA,G_IM_SIZ_8b,myTxtr1->sx,myTxtr1->sy, 0,G_TX_WRAP/* | G_TX_MIRROR*/, G_TX_WRAP/* | G_TX_MIRROR*/,myTxtr1->TCScaleX,myTxtr1->TCScaleY,G_TX_NOLOD,G_TX_NOLOD);


	V((vtxp),-devX,-devY,0,0,t7,t8,255,255,255,255);
	vtxp++;
	V((vtxp),devX,-devY,0,0,t5,t6,255,255,255,255);
	vtxp++;
	V((vtxp),devX,devY,0,0,t3,t4,255,255,255,255);
	vtxp++;
	V((vtxp),-devX,devY,0,0,t1,t2,255,255,255,255);
	vtxp++;

	sinCalc = sinf(seed + PI_OVER_2);
	cosCalc = cosf(seed + PI_OVER_4);

	t1 = (sinCalc * tOffset1);
	t2 = (sinCalc * tOffset2);
	t3 = 2048 + (cosCalc * tOffset3);
	t4 = (sinCalc * tOffset4);
	t5 = 2048 + (sinCalc * tOffset5);
	t6 = 2048 + (cosCalc * tOffset6);
	t7 = (sinCalc * tOffset7);
	t8 = 2048 + (sinCalc * tOffset8);

	V((vtxp),-devX,-devY,1,0,t7,t8,255,255,255,255);
	vtxp++;
	V((vtxp),devX,-devY,1,0,t5,t6,255,255,255,255);
	vtxp++;
	V((vtxp),devX,devY,1,0,t3,t4,255,255,255,255);
	vtxp++;
	V((vtxp),-devX,devY,1,0,t1,t2,255,255,255,255);
	vtxp++;
	
	gDPSetPrimColor(glistp++,255,255,255,255,255,devTrans1);
	gDPSetFogColor(glistp++,255,255,255,devTrans1);

//	xluSurf = TRUE;

	gSPVertex(glistp++,&verts[0],8,0);
	gSP2Triangles(glistp++,2,1,0,0,0,3,2,0);

	gDPSetPrimColor(glistp++,255,255,255,255,255,devTrans2);
	gDPSetFogColor(glistp++,255,255,255,devTrans2);

	LoadTexture(myTxtr2);

	gSP2Triangles(glistp++,6,5,4,0,4,7,6,0);

	seed += devPhaseInc;
}


void DrawTestWater()
{
	static float seed = 0.0F;
	TEXTURE *myTxtr1 = NULL;
	TEXTURE *myTxtr2 = NULL;
	float sinCalc,cosCalc;

	SetRenderMode();
			
	// set vertex ptr to start of vertices array
	vtxp = &verts[0];

	sinCalc = sinf(seed);
	cosCalc = cosf(seed);

	t1 = (sinCalc * tOffset1);
	t2 = (sinCalc * tOffset2);
	t3 = waterScalar + (cosCalc * tOffset3);
	t4 = (sinCalc * tOffset4);
	t5 = waterScalar + (sinCalc * tOffset5);
	t6 = waterScalar + (cosCalc * tOffset6);
	t7 = (sinCalc * tOffset7);
	t8 = waterScalar + (sinCalc * tOffset8);

	gDPSetTextureFilter(glistp++,G_TF_BILERP);
	gDPSetTexturePersp(glistp++,G_TP_PERSP);
	gDPSetColorDither(glistp++, G_CD_MAGICSQ);
	gSPTexture(glistp++,scalex,scaley,G_TX_RENDERTILE,0,G_ON);
//	gDPSetCombineMode(glistp++,G_CC_SHADE,G_CC_SHADE);
	gDPSetCombineMode(glistp++,G_CC_MODULATERGBA,G_CC_MODULATERGBA);
	//gDPSetCombineMode(glistp++,G_CC_PRIMITIVE,G_CC_PRIMITIVE);
	gDPSetTextureLUT(glistp++,G_TT_RGBA16);
	gSPClearGeometryMode(glistp++,G_CULL_BOTH | G_LIGHTING);
    gSPSetGeometryMode(glistp++,G_SHADE | G_SHADING_SMOOTH);	
//	gDPSetRenderMode(glistp++,G_RM_AA_XLU_SURF,G_RM_AA_XLU_SURF);
	gDPSetRenderMode(glistp++,G_RM_ADD,G_RM_ADD2);
	gSPClearGeometryMode(glistp++, G_ZBUFFER);

	FindTexture(&myTxtr1,UpdateCRC("water2.bmp"),YES,"water2.bmp");
	FindTexture(&myTxtr2,UpdateCRC("water1.bmp"),YES,"water1.bmp");
	LoadTexture(myTxtr1);

	V((vtxp),-600,waterOffset,600,0,t1,t2,255,255,255,255);
	vtxp++;
	V((vtxp),600,waterOffset,600,0,t3,t4,255,255,255,255);
	vtxp++;
	V((vtxp),600,waterOffset,-600,0,t5,t6,255,255,255,255);
	vtxp++;
	V((vtxp),-600,waterOffset,-600,0,t7,t8,255,255,255,255);
	vtxp++;

	sinCalc = sinf(seed + PI_OVER_2);
	cosCalc = cosf(seed + PI_OVER_4);

	t1 = (sinCalc * tOffset1);
	t2 = (sinCalc * tOffset2);
	t3 = waterScalar + (cosCalc * tOffset3);
	t4 = (sinCalc * tOffset4);
	t5 = waterScalar + (sinCalc * tOffset5);
	t6 = waterScalar + (cosCalc * tOffset6);
	t7 = (sinCalc * tOffset7);
	t8 = waterScalar + (sinCalc * tOffset8);

	V((vtxp),-600,waterOffset+1,600,0,t1,t2,255,255,255,255);
	vtxp++;
	V((vtxp),600,waterOffset+1,600,0,t3,t4,255,255,255,255);
	vtxp++;
	V((vtxp),600,waterOffset+1,-600,0,t5,t6,255,255,255,255);
	vtxp++;
	V((vtxp),-600,waterOffset+1,-600,0,t7,t8,255,255,255,255);
	vtxp++;

	gDPSetPrimColor(glistp++,255,255,255,255,255,devTrans1);
	gDPSetFogColor(glistp++,255,255,255,devTrans1);

//	xluSurf = TRUE;

	gSPVertex(glistp++,&verts[0],8,0);
	gSP2Triangles(glistp++,2,1,0,0,0,3,2,0);

	gDPSetPrimColor(glistp++,255,255,255,255,255,devTrans2);
	gDPSetFogColor(glistp++,255,255,255,devTrans2);
	LoadTexture(myTxtr2);

	gSP2Triangles(glistp++,6,5,4,0,4,7,6,0);

	seed += devPhaseInc;
}
