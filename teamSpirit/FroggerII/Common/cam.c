/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.
	
	  
	File		: cam.c
	Programmer	: Andrew Eder
	Date		: 11/10/98
	
----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"

//ACTOR	camera;

//VECTOR	cameraUpVect = { 0,1,0 };
VECTOR	camVect;

VECTOR	currCamSource[4] = {{ 0,0,-100 },{ 0,0,-100 },{ 0,0,-100 },{ 0,0,-100 }};
VECTOR	currCamTarget[4] = {{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 }};

VECTOR	actualCamSource[2][4];
VECTOR	actualCamTarget[2][4];

VECTOR currCamDist = {0,0,10};

//float	xValuesFOV[4]		= { 1.333333F,1.333333F,1.333333F,1.333333F };
//float	yValuesFOV[4]		= { 45.0F,45.0F,45.0F,45.0F };
//float	camValuesLookOfs[4]	= { 50.0F,50.0F,50.0F,25.0F};
//VECTOR	camValuesDist[4]	= { { 0,680,192 }, { 0,480,192 }, { 0,280,192 }, { 0,90,100 } };
//short	currCamSetting		= 0;
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
//int	camFacing2				= 0;
char camZoom				= 1;
float scaleV				= 1.1F;

short cameraShake			= 0;

char controlCamera			= 0;
char fixedDir = 0;
char fixedPos = 0;
char firstPerson = 0;

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
TRANSCAMERA *CreateAndAddTransCamera(GAMETILE *tile,unsigned long dirCamMustFace, VECTOR *v, unsigned long flags)
{
	TRANSCAMERA *newItem = (TRANSCAMERA *)JallocAlloc(sizeof(TRANSCAMERA),YES,"TRNSCAM");

	newItem->tile			= tile;
	newItem->dirCamMustFace	= dirCamMustFace;
	SetVector(&newItem->camOffset, v);
	newItem->flags			= flags;
	newItem->FOV			= 45.0;

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

			yFOVNew = cur->FOV;

			break;
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
		scaleV = 0.7F;
	}
	else
	{
		scaleV = 1.1F;
	}
}


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: CameraLookAtFrog

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/
void CameraLookAtFrog(void)
{
	if(frog[0] && !fixedDir && !controlCamera)
	{
	
		float afx,afy,afz;
		int i,l;
		afx = afy = afz = 0;
		l = 0;
		for (i=0; i<NUM_FROGS; i++)
		{
			if (frog[i]->action.healthPoints > 0)
			{
				afx += frog[i]->actor->pos.v[0];
				afy += frog[i]->actor->pos.v[1];
				afz += frog[i]->actor->pos.v[2];
				l++;
			}
		}
		
		if (l)
		{
			afx/=l;
			afy/=l;
			afz/=l;
		}

		{
			camTarget[0].v[0] = afx+currTile[0]->dirVector[camFacing].v[0]*camLookOfs + currTile[0]->normal.v[0];	
			camTarget[0].v[1] = afy+currTile[0]->dirVector[camFacing].v[1]*camLookOfs + currTile[0]->normal.v[1];	
			camTarget[0].v[2] = afz+currTile[0]->dirVector[camFacing].v[2]*camLookOfs + currTile[0]->normal.v[2];
		}
		
	}
	
}


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: SlurpCamPosition

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

float fovSpd = 2;
float camSpeed4 = 8;

void SlurpCamPosition(long cam)
{
	float cam1 = camSpeed,
		cam2 = camSpeed3,
		cam3 = camSpeed4;

	while( lastActFrameCount < actFrameCount )
	{
		currCamSource[cam].v[0] -= (currCamSource[cam].v[0] - camSource[cam].v[0])/cam1;
		currCamSource[cam].v[1] -= (currCamSource[cam].v[1] - camSource[cam].v[1])/cam1;
		currCamSource[cam].v[2] -= (currCamSource[cam].v[2] - camSource[cam].v[2])/cam1;

		currCamTarget[cam].v[0] -= (currCamTarget[cam].v[0] - camTarget[cam].v[0])/cam2;
		currCamTarget[cam].v[1] -= (currCamTarget[cam].v[1] - camTarget[cam].v[1])/cam2;
		currCamTarget[cam].v[2] -= (currCamTarget[cam].v[2] - camTarget[cam].v[2])/cam2;

		currCamDist.v[0] -= (currCamDist.v[0] - camDist.v[0]*scaleV)/cam3;
		currCamDist.v[1] -= (currCamDist.v[1] - camDist.v[1]*scaleV)/cam3;
		currCamDist.v[2] -= (currCamDist.v[2] - camDist.v[2]*scaleV)/cam3;

		if ( gameState.mode != CAMEO_MODE )
		{
			VECTOR t = { 0,0,0 };
			int i;
		
			for (i=0; i<NUM_FROGS; i++)
			{
				if (frog[i]->action.healthPoints > 0)
				{
					t.v[0]+=currTile[i]->normal.v[0];
					t.v[1]+=currTile[i]->normal.v[1];
					t.v[2]+=currTile[i]->normal.v[2];
				}
			}

			MakeUnit (&t);
			
			camVect.v[0] -= (camVect.v[0] - t.v[0])/camSpeed2;
			camVect.v[1] -= (camVect.v[1] - t.v[1])/camSpeed2;
			camVect.v[2] -= (camVect.v[2] - t.v[2])/camSpeed2;
			
		}

		xFOV		-= (xFOV - xFOVNew) / (camSpeed*fovSpd);
		yFOV		-= (yFOV - yFOVNew) / (camSpeed*fovSpd);
		camLookOfs	-= (camLookOfs - camLookOfsNew) / camSpeed;

		if(cameraShake)
		{
			currCamSource[cam].v[0] += (-16 + Random(32));
			currCamSource[cam].v[1] += (-16 + Random(32));
			currCamSource[cam].v[2] += (-16 + Random(32));
			cameraShake--;
		}
		else
		{
	//		osMotorStop ( &rumble );
		}

		lastActFrameCount+=2;
	}
}


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: UpdateCameraPosition

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/
void UpdateCameraPosition(long cam)
{
	VECTOR result;

	if(!frog[0] || !currTile[0] || controlCamera)
		return;
	
	if ( gameState.mode != CAMEO_MODE && !fixedPos )
	{
		float afx,afy,afz;
		float afx2,afy2,afz2;

		int i,l;
		afx = afy = afz = 0;
		afx2 = afy2 = afz2 = 0;
		l=0;
		for (i=0; i<NUM_FROGS; i++)
		{
			if (frog[i]->action.healthPoints > 0)
			{

				afx += frog[i]->actor->pos.v[0];
				afy += frog[i]->actor->pos.v[1];
				afz += frog[i]->actor->pos.v[2];

				afx2 += currTile[i]->normal.v[0]*currCamDist.v[1];
				afy2 += currTile[i]->normal.v[1]*currCamDist.v[1];
				afz2 += currTile[i]->normal.v[2]*currCamDist.v[1];
			
				afx2 -= currTile[0]->dirVector[camFacing].v[0]*currCamDist.v[2];
				afy2 -= currTile[0]->dirVector[camFacing].v[1]*currCamDist.v[2];
				afz2 -= currTile[0]->dirVector[camFacing].v[2]*currCamDist.v[2];
				l++;
			}
		}
		
		if (l)
		{
		
			afx/=l;
			afy/=l;
			afz/=l;
		
			afx2/=l;
			afy2/=l;
			afz2/=l;
		}

		camSource[cam].v[0] = afx+afx2;
		camSource[cam].v[1] = afy+afy2;
		camSource[cam].v[2] = afz+afz2;
	}

	SlurpCamPosition(cam);
}
