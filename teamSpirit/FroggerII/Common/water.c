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

ACTOR2 *watActor = NULL;

short numN64WaterObjects = 0;
short currN64WaterObject = 0;
SHORT2DVECTOR *mTC[MAX_N64_WATEROBJECTS];


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
		LoadObjectBank(INGAMEGENERIC_OBJ_BANK);

		CreateAndAddTextOverlay(30,20,"water stuff",NO,255,smallFont,0,0);
		txtTmp = CreateAndAddTextOverlay(32,22,"water stuff",NO,255,smallFont,0,0);
		txtTmp->r = 0;	txtTmp->g = 0;	txtTmp->b = 0;

		// add the water actor
		watActor = CreateAndAddActor("wat_fall.obe",0,0,100,0,0,0);
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
	Purpose			: updates the specified water actor / object - new method
	Parameters		: ACTOR *
	Returns			: void
	Info			: 
*/
float modi1 = 0.065;
float modi2 = 1.29;
float modi3 = 2.5;
float modi4 = 96.0;

int modc1 = 128;
int modc2 = 150;
int modc3 = 8;

void UpdateWater(ACTOR *wAct)
{
	// update the water - assumes drawlisted and skinned object....
	if(wAct)
	{
		float t,t2,xval,zval,mV;
		Vtx *in = NULL;
		int i,colMod;

		in = wAct->objectController->vtx[wAct->objectController->vtxBuf];
		i = wAct->objectController->numVtx;

		t = actFrameCount * modi1;
		while(i--)
		{
			xval = in->v.ob[X] * modi2;
			zval = in->v.ob[Z] * modi2;

			t2 = sinf(t + xval * zval * 0.5) - cosf(t + xval * 0.3 * zval);

			mV = t2 * modi4;

			in->v.ob[Y] += t2 * modi3;

			colMod = modc2 + (modc3 * (in->v.ob[Y] + 20));
			if(colMod > 255)
				colMod = 255;
			else if(colMod < modc1)
				colMod = modc1;

			in->v.cn[0] = colMod;
			in->v.cn[1] = colMod;
			in->v.cn[2] = colMod;
			in->v.cn[3] = colMod;

			in->v.tc[0] = mTC[currN64WaterObject][i].v[0] + mV;
			in->v.tc[1] = mTC[currN64WaterObject][i].v[1] + mV;

			in++;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: AddN64WaterObjectResource
	Purpose			: adds N64 specific water related stuff
	Parameters		: 
	Returns			: void
	Info			: 
*/
void AddN64WaterObjectResource(ACTOR *wAct)
{
	int i;
	Vtx *in;

	if(wAct && (numN64WaterObjects < MAX_N64_WATEROBJECTS))
	{
		// copy texture co-ords to an array
		in = wAct->objectController->vtx[wAct->objectController->vtxBuf];
		i = wAct->objectController->numVtx;

		dprintf"Allocating %d vertices for water object %d\n",i,numN64WaterObjects));
		mTC[numN64WaterObjects] = (SHORT2DVECTOR *)JallocAlloc(sizeof(SHORT2DVECTOR) * i,YES,"s2dv");

		while(i--)
		{
			mTC[numN64WaterObjects][i].v[0] = in->v.tc[0];
			mTC[numN64WaterObjects][i].v[1] = in->v.tc[1];
			in++;
		}

		numN64WaterObjects++;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: FreeN64WaterResources
	Purpose			: frees N64 specific water related stuff
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeN64WaterResources()
{
	int i;

	for(i=0; i<numN64WaterObjects; i++)
		JallocFree((UBYTE **)&mTC[i]);

	numN64WaterObjects = 0;
}


#endif
