/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: water.c
	Programmer	: Andy Eder
	Date		: 20/10/99 16:47:47

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>
#include "incs.h"

#ifdef N64_VERSION


float waterFreq[2] = { 40, 21 };
float waterFactor[2] = { 0.008, 0.016 };
float waterF = 0.1;
float waterWaveHeight[2] = { 20, -10 };
VECTOR waterCentre[2] = { { 100,0,100 },{ -75,0,-15 } };

float waterWaveHeightBase[2] = { 60,40 };
float waterWaveHeightAmp[2] = { 20,10 };
float waterWaveHeightFreq[2] = { 201, 202 };
float watRot[2] = { 0,0 };

float dist[2];
VECTOR tempVect;

ACTOR2 *watActor = NULL;


/*	--------------------------------------------------------------------------------
	Function		: RunWaterDemo
	Purpose			: runs the water demo (development)
	Parameters		: 
	Returns			: void
	Info			: 
*/
void RunWaterDemo()
{
	static u16 button,lastbutton;
	static s16 stickX,stickY;
	int j;
	
	if(frameCount == 1)
	{
		float x,y,z;
		int i;
		TEXTOVERLAY *txtTmp;
		
		FreeMenuItems();
		LoadTextureBank(SYSTEM_TEX_BANK);
		LoadTextureBank(TITLESGENERIC_TEX_BANK);
		LoadObjectBank(INGAMEGENERIC_OBJ_BANK);

		CreateAndAddTextOverlay(30,20,"water stuff",NO,255,smallFont,0,0);
		txtTmp = CreateAndAddTextOverlay(32,22,"water stuff",NO,255,smallFont,0,0);
		txtTmp->r = 0;	txtTmp->g = 0;	txtTmp->b = 0;

		// add the water actor
//		watActor = CreateAndAddActor("aeder.obe",0,0,100,0,0,0);
		watActor = CreateAndAddActor("wat_stop.obe",0,0,100,0,0,0);
		watActor->flags = ACTOR_WATER;

		watActor->actor->qRot.x = -0.5F;
	}

	// read controller
	button = controllerdata[ActiveController].button;
	stickX = controllerdata[ActiveController].stick_x;
	stickY = controllerdata[ActiveController].stick_y;

	if((button & CONT_UP) && !(lastbutton & CONT_UP))
	{
	}

	if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
	{
	}

	if((button & CONT_LEFT) && !(lastbutton & CONT_LEFT))
	{
	}

	if((button & CONT_RIGHT) && !(lastbutton & CONT_RIGHT))
	{
	}

	if((button & CONT_A) && !(lastbutton & CONT_A))
	{
	}

	if((button & CONT_B) && !(lastbutton & CONT_B))
	{
	}

	if((button & CONT_L) && !(lastbutton & CONT_L))
	{
	}

	if((button & CONT_R) && !(lastbutton & CONT_R))
	{
	}

	if((button & CONT_G) && !(lastbutton & CONT_G))
	{
		FreeAllLists();
		developmentMode = 0;
		gameState.mode = MENU_MODE;
		gameState.menuMode = TITLE_MODE;

		frameCount = 0;
		lastbutton = 0;
		return;
	}

	lastbutton = button;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateWater
	Purpose			: updates the specified water actor / object
	Parameters		: ACTOR *
	Returns			: void
	Info			: 
*/
void UpdateWater(ACTOR *wAct)
{
	static Vtx *wv = NULL;
		
	// update the water - assumes drawlisted and skinned object....
	if(wAct)
	{
		int i,j;

		wv = wAct->objectController->vtx[wAct->objectController->vtxBuf];
		i = wAct->objectController->numVtx;
		while(i--)
		{
			for(j=0; j<2; j++)
			{
				tempVect.v[X] = wv->v.ob[X] + waterCentre->v[X];
				tempVect.v[Z] = wv->v.ob[Z] + waterCentre->v[Z];
				dist[j] = Magnitude2D(&tempVect);
			}

			wv->v.ob[Y] =	wv->v.ob[Y] * (1 - waterF) + 
					(	SineWave2(waterFreq[0],frameCount + dist[0] * waterFactor[0] * waterFreq[0]) * waterWaveHeight[0] + 
						SineWave2(waterFreq[1],frameCount + dist[1] * waterFactor[1] * waterFreq[1]) * waterWaveHeight[1]) * waterF;

			wv->v.cn[0] = 128 + (2 * wv->v.ob[Y]);
			wv->v.cn[1] = wv->v.cn[0];
			wv->v.cn[2] = wv->v.cn[0];
			wv->v.cn[3] = 255;

			wv++;
		}

		for(j = 0;j < 2;j++)
		{
			waterWaveHeight[j] = SineWave2(waterWaveHeightFreq[j],frameCount)*waterWaveHeightAmp[j] + waterWaveHeightBase[j];
			RotateVector2D(&waterCentre[j],&waterCentre[j],watRot[j]);
		}
	}
}


#endif
