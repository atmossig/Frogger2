/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: Cam.h
	Programmer	: Andy Eder
	Date		: 15/04/99 15:01:33

----------------------------------------------------------------------------------------------- */

#ifndef CAM_H_INCLUDED
#define CAM_H_INCLUDED


#include "define.h"

#define STATIC_CAMERA	0
#define FIXED_SOURCE	1
#define LOOK_AT_FROG	2

//----- [ FUNCTION PROTOTYPES ] ----------------------------------------------------------------//

typedef struct TAGTRANSCAMERA
{
	GAMETILE		*tile;
	unsigned long		dirCamMustFace;
	unsigned long		flags;
	VECTOR			camOffset;
	float FOV;

	struct TAGTRANSCAMERA	*next;

} TRANSCAMERA;


//----- [ GLOBALS ] ----------------------------------------------------------------------------//

extern ACTOR	camera;

extern VECTOR	cameraUpVect;
extern VECTOR	camVect;
extern VECTOR	camDist;

extern float	camSpeed;
extern float	camSpeed2;
extern float	camSpeed3;
extern float	camLookOfs;
extern float	camLookOfsNew;
extern int		camFacing;
extern int		camFacing2;

extern VECTOR	camSource[4];
extern VECTOR	camTarget[4];
extern VECTOR	currCamSource[4];
extern VECTOR	currCamTarget[4];

extern VECTOR	actualCamSource[2][4];
extern VECTOR	actualCamTarget[2][4];

extern float	xValuesFOV[4];
extern float	yValuesFOV[4];
extern float	camValuesLookOfs[4];
extern VECTOR	camValuesDist[4];
extern short	currCamSetting;
extern float	xFOV;				// actually the SCREEN RATIO, i.e. 4/3
extern float	xFOVNew;
extern float	yFOV;				// THIS is the field of view
extern float	yFOVNew;
extern float	precScaleFactor;

extern char		camZoom;
extern float	scaleV;

extern short	cameraShake;

extern char		controlCamera;
extern char		fixedDir;
extern char		fixedPos;
extern char		firstPerson;

extern TRANSCAMERA *transCameraList;

//----- [ FUNCTION PROTOTYPES ] ----------------------------------------------------------------//

void InitCameraForLevel(unsigned long worldID,unsigned long levelID);

TRANSCAMERA *CreateAndAddTransCamera(GAMETILE *tile,unsigned long dirCamMustFace, VECTOR *v, unsigned long flags);
void CheckForDynamicCameraChange(GAMETILE *tile);
void FreeTransCameraList();

void ChangeCameraSetting();

#endif