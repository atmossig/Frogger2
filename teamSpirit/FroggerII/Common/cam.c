/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.
	
	  
	File		: cam.c
	Programmer	: Andrew Eder
	Date		: 11/10/98
	
----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

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

char cameraControl			= 0;

TRANSCAMERA *transCameraList = NULL;

TEXTOVERLAY *txtCamDist = NULL;


/*	--------------------------------------------------------------------------------
	Function		: InitCameraForLevel
	Purpose			: initialises the camera (and the special cases) for the specified level
	Parameters		: unsigned long,unsigned long
	Returns			: void
	Info			: 
*/

int tCam1[] = 
{
	6,7,8,9,12,14,17,19,22,23,24,25,
	-1
};

int tCam2[] = 
{
	41,601,602,603,
	-1
};

int tCam3[] = 
{
	48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,
	79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,
	107,108,109,110,113,114,115,116,135,136,137,157,158,159,179,180,181,
	129,130,131,132,
	154,153,152,151,
	176,175,174,173,172,
	198,197,196,195,
	220,
	242,
	264,
	286,
	308,
	125,126,147,148,169,170,
	-1
};

int tCam4[] = 
{
	188,189,190,191,192,
	210,211,212,213,214,
	232,233,234,235,236,237,238,
	254,255,256,257,258,259,260,
	276,277,278,279,280,
	-1
};

int tCam5[] = 
{
	298,299,303,304,
	300,301,302,
	-1
};

int tCam6[] = 
{
	305,306,307,
	283,284,285,
	261,262,263,
	239,240,241,
	-1
};

int tCam7[] = 
{
	215,216,217,218,219,
	193,194,
	171,
	149,150,
	127,128,
	-1
};

int tCam8[] = 
{
	117,118,119,
	140,141,
	-1
};

int tCam9[] = 
{
	163,
	185,
	206,207,
	228,229,
	-1
};

int tCam10[] = 
{
	291,292,293,294,295,
	271,272,273,
	249,250,251,
	-1
};

int tCam11[] = 
{
	289,290,
	267,268,269,
	245,246,247,
	223,224,225,
	201,202,203,
	-1
};


VECTOR dCam1 = { 0,290,330 };
VECTOR dCam2 = { 0,120,330 };
VECTOR dCam3 = { 0,560,200 };
VECTOR dCam4 = { 0,460,6 };
VECTOR dCam5 = { 0,250,250 };
VECTOR dCam6 = { 0,250,530 };
VECTOR dCam7 = { 0,415,132 };
VECTOR dCam8 = { 0,200,257 };
VECTOR dCam9 = { 0,350,7 };
VECTOR dCam10 = { 0,275,222 };
VECTOR dCam11 = { 0,540,7 };


// ----- [ CURRENT GARDEN LEVEL CAMERA SETTINGS ] ----- //

int gardenLawnSetting1[] = 
{
	12,13,14,15,108,109,110,111,112,113,114,115,116,292,



	-1
};

int gardenLawnSetting2[] = 
{
	94,95,96,97,98,99,100,101,102,103,104,105,106,107,
	167,168,169,170,171,172,173,174,175,176,178,179,180,
	181,182,183,184,185,186,187,189,190,191,192,193,194,
	195,196,197,198,199,200,201,202,203,204,205,206,207,
	208,209,210,211,212,213,214,215,216,217,218,219,220,
	221,222,223,224,225,226,227,228,229,230,231,232,233,
	234,235,236,237,238,239,240,241,242,243,245,246,247,
	248,249,250,251,252,253,254,255,256,257,258,259,260,
	261,262,263,264,265,266,294,295,
	497,498,
	528,527,526,

	-1
};

int gardenLawnSetting3[] = 
{
	81,82,83,84,85,86,87,88,89,90,91,92,93,
	154,155,156,157,158,159,160,161,162,163,164,165,166,
	494,495,496,
	529,530,531,
	-1
};

int gardenLawnSetting4[] = 
{
	16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
	32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
	48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
	64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,
	117,118,119,120,
	121,122,123,124,125,126,127,128,129,130,131,132,133,
	134,135,136,137,138,139,140,141,142,143,145,146,147,
	148,149,150,151,152,153,275,

	400,401,402,403,404,405,406,407,408,409,410,411,412,413,414,415,
	
	480,481,482,483,484,485,486,487,488,489,490,491,492,493,

	364,365,366,367,368,369,370,371,372,373,374,375,376,
	378,379,380,381,382,383,384,385,386,387,573,

	-1
};

VECTOR gardenLawnCam1 = { 0,270,272 };
VECTOR gardenLawnCam2 = { 0,470,272 };
VECTOR gardenLawnCam3 = { 0,600,272 };
VECTOR gardenLawnCam4 = { 0,640,82 };


void InitCameraForLevel(unsigned long worldID,unsigned long levelID)
{
	int i=0;
	TRANSCAMERA *tcam;

	txtCamDist = CreateAndAddTextOverlay(30,40,"************************************************",NO,NO,255,255,255,255,smallFont,0,0,0);
	DisableTextOverlay(txtCamDist);

	switch(worldID)
	{
		case WORLDID_CITY:
			switch(levelID)
			{
				case LEVELID_CITYDOCKS:
					while (tCam1[i]!=-1)
						CreateAndAddTransCamera(&firstTile[tCam1[i++]],0,dCam1.v[0],dCam1.v[1],dCam1.v[2]);
					i=0;
					while (tCam2[i]!=-1)
						CreateAndAddTransCamera(&firstTile[tCam2[i++]],0,dCam2.v[0],dCam2.v[1],dCam2.v[2]);
					i=0;
					while (tCam3[i]!=-1)
						CreateAndAddTransCamera(&firstTile[tCam3[i++]],0,dCam3.v[0],dCam3.v[1],dCam3.v[2]);
					i=0;
					while (tCam4[i]!=-1)
						CreateAndAddTransCamera(&firstTile[tCam4[i++]],0,dCam4.v[0],dCam4.v[1],dCam4.v[2]);
					i=0;
					while (tCam5[i]!=-1)
						CreateAndAddTransCamera(&firstTile[tCam5[i++]],0,dCam5.v[0],dCam5.v[1],dCam5.v[2]);
					i=0;
					while (tCam6[i]!=-1)
						CreateAndAddTransCamera(&firstTile[tCam6[i++]],0,dCam6.v[0],dCam6.v[1],dCam6.v[2]);
					i=0;
					while (tCam7[i]!=-1)
						CreateAndAddTransCamera(&firstTile[tCam7[i++]],0,dCam7.v[0],dCam7.v[1],dCam7.v[2]);
					i=0;
					while (tCam8[i]!=-1)
						CreateAndAddTransCamera(&firstTile[tCam8[i++]],0,dCam8.v[0],dCam8.v[1],dCam8.v[2]);
					i=0;
					while (tCam9[i]!=-1)
						CreateAndAddTransCamera(&firstTile[tCam9[i++]],0,dCam9.v[0],dCam9.v[1],dCam9.v[2]);
					i=0;
					while (tCam10[i]!=-1)
						CreateAndAddTransCamera(&firstTile[tCam10[i++]],0,dCam10.v[0],dCam10.v[1],dCam10.v[2]);
					i=0;
					while (tCam11[i]!=-1)
						CreateAndAddTransCamera(&firstTile[tCam11[i++]],0,dCam11.v[0],dCam11.v[1],dCam11.v[2]);

					break;

				case LEVELID_CITYSTREETS:
					break;

				case LEVELID_CITYWAREHOUSE:
					break;

				case LEVELID_CITYTOWER:
					break;

				case LEVELID_CITYBONUS:
					break;
			}
			break;

		case WORLDID_GARDEN:
			switch(levelID)
			{
				case LEVELID_GARDENLAWN:
					i=0;
					while(gardenLawnSetting1[i] != -1)
						CreateAndAddTransCamera(&firstTile[gardenLawnSetting1[i++]],0,gardenLawnCam1.v[0],gardenLawnCam1.v[1],gardenLawnCam1.v[2]);
					i=0;
					while(gardenLawnSetting2[i] != -1)
						CreateAndAddTransCamera(&firstTile[gardenLawnSetting2[i++]],0,gardenLawnCam2.v[0],gardenLawnCam2.v[1],gardenLawnCam2.v[2]);
					i=0;
					while(gardenLawnSetting3[i] != -1)
						CreateAndAddTransCamera(&firstTile[gardenLawnSetting3[i++]],0,gardenLawnCam3.v[0],gardenLawnCam3.v[1],gardenLawnCam3.v[2]);
					i=0;
					while(gardenLawnSetting4[i] != -1)
						CreateAndAddTransCamera(&firstTile[gardenLawnSetting4[i++]],0,gardenLawnCam4.v[0],gardenLawnCam4.v[1],gardenLawnCam4.v[2]);
					break;
			}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddTransCamera
	Purpose			: adds a dynamic camera angle change for a specified game tile
	Parameters		: GAMETILE *,unsigned long,float,float,float
	Returns			: TRANSCAMERA *
	Info			: 
*/
TRANSCAMERA *CreateAndAddTransCamera(GAMETILE *tile,unsigned long dirCamMustFace,float offsetX,float offsetY,float offsetZ)
{
	TRANSCAMERA *newItem = (TRANSCAMERA *)JallocAlloc(sizeof(TRANSCAMERA),YES,"TRNSCAM");

	newItem->tile			= tile;
	newItem->dirCamMustFace	= dirCamMustFace;
	newItem->camOffset.v[X]	= offsetX;
	newItem->camOffset.v[Y]	= offsetY;
	newItem->camOffset.v[Z]	= offsetZ;

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
	
	// display current camera position
	sprintf(txtCamDist->text,"cam at (%.0f,%.0f,%.0f) CF:%d",camDist.v[X],camDist.v[Y],camDist.v[Z],camFacing);

	cur = transCameraList;
	while(cur)
	{
		if(cur->tile == tile)
		{
			// frog on tile where camera change is needed
			// check if camera if facing right direction for the change
//			if(camFacing == cur->dirCamMustFace)
			{
				// ok - move the camera to the new position
				SetVector(&camDist,&cur->camOffset);
				specialCaseTile = 1;
			}
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
		ACTOR_DRAWDISTANCEINNER = 100000;
		scaleV = 0.7F;
	}
	else
	{
		ACTOR_DRAWDISTANCEINNER = 125000;
		scaleV = 1.1F;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: ShowCamInfo
	Purpose			: for debugging
	Parameters		: 
	Returns			: void
	Info			: CAN BE REMOVED BEFORE RELEASE
*/
void ShowCamInfo()
{
	txtCamDist->draw ^= 1;
}

