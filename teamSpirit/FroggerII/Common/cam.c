/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.
	
	  
	File		: cam.c
	Programmer	: Andrew Eder
	Date		: 11/10/98
	
----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


ACTOR	camera;

VECTOR	cameraUpVect = { 0,1,0 };
VECTOR	camVect;

VECTOR	currCamSource[4] = {{ 0,0,-100 },{ 0,0,-100 },{ 0,0,-100 },{ 0,0,-100 }};
VECTOR	currCamTarget[4] = {{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 }};

VECTOR	actualCamSource[2][4];
VECTOR	actualCamTarget[2][4];

float	xValuesFOV[4]		= { 1.333333F,1.333333F,1.333333F,1.333333F };
float	yValuesFOV[4]		= { 45.0F,45.0F,45.0F,45.0F };
float	camValuesLookOfs[4]	= { 50.0F,50.0F,50.0F,25.0F};
VECTOR	camValuesDist[4]	= { { 0,680,192 }, { 0,480,192 }, { 0,280,192 }, { 0,90,100 } };
short	currCamSetting		= 0;
float	xFOVNew				= 320.0F / 240.0F;
float	yFOVNew				= 45.0F;
float	xFOV				= 320.0F / 240.0F;
float	yFOV				= 45.0F;
float	precScaleFactor		= 1.0F;

VECTOR camSource[4]			= {{ 0,569,-200 },{ 0,569,-200 },{ 0,569,-200 },{ 0,569,-200 }};
VECTOR camTarget[4];
float camLookOfs			= 50;
float camLookOfsNew			= 50;

VECTOR camDist				= { 0,680,192 };
float camSpeed				= 9;
float camSpeed2				= 9;
float camSpeed3				= 8;

int	camFacing				= 0;
int	camFacing2				= 0;
char camZoom				= 1;
float scaleV				= 1.1F;

short cameraShake			= 0;

char controlCamera			= 0;
char fixedDir = 0;
char fixedPos = 0;

TRANSCAMERA *transCameraList = NULL;


/*	--------------------------------------------------------------------------------
	Function		: InitCameraForLevel
	Purpose			: initialises the camera (and the special cases) for the specified level
	Parameters		: unsigned long,unsigned long
	Returns			: void
	Info			: 
*/
void InitCameraForLevel(unsigned long worldID,unsigned long levelID)
{
}


/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddTransCamera
	Purpose			: adds a dynamic camera angle change for a specified game tile
	Parameters		: GAMETILE *,unsigned long,float,float,float
	Returns			: TRANSCAMERA *
	Info			: 
*/
TRANSCAMERA *CreateAndAddTransCamera(GAMETILE *tile,unsigned long dirCamMustFace,float offsetX,float offsetY,float offsetZ, unsigned long flags)
{
	TRANSCAMERA *newItem = (TRANSCAMERA *)JallocAlloc(sizeof(TRANSCAMERA),YES,"TRNSCAM");

	newItem->tile			= tile;
	newItem->dirCamMustFace	= dirCamMustFace;
	newItem->camOffset.v[X]	= offsetX;
	newItem->camOffset.v[Y]	= offsetY;
	newItem->camOffset.v[Z]	= offsetZ;
	newItem->flags			= flags;

	newItem->next			= transCameraList;
	transCameraList			= newItem;

	return newItem;
}


/*	--------------------------------------------------------------------------------
	Function		: CheckForDynamicCameraChange
	Purpose			: checks current tile to see if a camera change is needed
	Parameters		: GAMETILE *
	Returns			: void
	Info			: 
*/
void CheckForDynamicCameraChange(GAMETILE *tile)
{
	char specialCaseTile = 0;
	TRANSCAMERA *cur;
	static VECTOR stdVector = { 0,480,192 };
	
	cur = transCameraList;
	while(cur)
	{
		if(cur->tile == tile)
		{
			switch (cur->flags)
			{
			case LOOK_AT_FROG:

				// frog on tile where camera change is needed
				// check if camera if facing right direction for the change
//				if(camFacing == cur->dirCamMustFace)
				// ok - move the camera to the new position
				SetVector(&camDist,&cur->camOffset);
				specialCaseTile = 1;
				fixedDir = 0;
				fixedPos = 0;
				break;

			case STATIC_CAMERA:
				SetVector(&camSource[0], &cur->camOffset);
				fixedDir = 1;
				fixedPos = 1;
				break;

			case FIXED_SOURCE:
				SetVector(&camSource[0], &cur->camOffset);
				fixedDir = 0;
				fixedPos = 1;
				break;
			}
			// ENDELSEIF

			if (cur->dirCamMustFace)
				camFacing = cur->dirCamMustFace - 1;
		}
		cur = cur->next;
	}

//	if(!specialCaseTile)
//		SetVector(&camDist,&stdVector);
}


/*	--------------------------------------------------------------------------------
	Function		: FreeTransCameraList
	Purpose			: frees the camera list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeTransCameraList()
{
	TRANSCAMERA *cur,*next;

	cur = transCameraList;
	while(cur)
	{
		next = cur->next;
		JallocFree((UBYTE**)&cur);
		cur = next;
	}

	transCameraList = NULL;
}


/*	--------------------------------------------------------------------------------
	Function		: ChangeCameraSetting
	Purpose			: changes camera position / view, etc.
	Parameters		: 
	Returns			: void
	Info			: 
*/

void ChangeCameraSetting()
{
	camZoom ^= 1;

	if(camZoom)
	{
#ifndef PC_VERSION
		ACTOR_DRAWDISTANCEINNER = 100000;
#else
		ACTOR_DRAWDISTANCEINNER = 200000;
#endif
		scaleV = 0.7F;
	}
	else
	{
#ifndef PC_VERSION
		ACTOR_DRAWDISTANCEINNER = 125000;
#else
		ACTOR_DRAWDISTANCEINNER = 250000;
#endif
		scaleV = 1.1F;
	}
}
