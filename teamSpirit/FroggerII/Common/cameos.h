/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: cameos.h
	Programmer	: James Healey
	Date		: 21/05/99

----------------------------------------------------------------------------------------------- */

#ifndef CAMEOS_H_INCLUDED
#define CAMEOS_H_INCLUDED

//----------------------------------------------------------------------------//
//----- DEFINE ---------------------------------------------------------------//
//----------------------------------------------------------------------------//

#define CAMEO_START		( 1 << 0 )
#define CAMEO_FINISH	( 1 << 1 )
#define CAMEO_ANIMATE	( 1 << 2 )
#define CAMEO_CAMERA	( 1 << 3 )
#define CAMEO_FROGMOVE  ( 1 << 4 )
#define CAMEO_LEFT		( 1 << 5 )
#define CAMEO_RIGHT		( 1 << 6 )
#define CAMEO_UP		( 1 << 7 )
#define CAMEO_DOWN		( 1 << 8 )

//----------------------------------------------------------------------------//
//----- TYPDE DEFS -----------------------------------------------------------//
//----------------------------------------------------------------------------//

typedef struct _CAMEOOBJECT
{
	struct _CAMEOOBJECT *next;
	struct _CAMEOOBJECT *prev;

	ACTOR2	*cameoActor;

	char *objectName;

} CAMEOOBJECT;

typedef struct _CAMEO
{
	struct _CAMEO *next;
	struct _CAMEO *prev;

	CAMEOOBJECT *linkObject;

	unsigned long flags;
	unsigned long frameNum;
	unsigned long animNum;

	float x, y, z;

	float camSpeed;

	int camFacing;

} CAMEO;

typedef struct _CAMEO_SET
{
	unsigned long numCameos;

	CAMEO		  *cameos;

} CAMEO_SET;
  
//----------------------------------------------------------------------------//
//----- GLOBALS --------------------------------------------------------------//
//----------------------------------------------------------------------------//
extern CAMEOOBJECT *cameoObjectList;

extern CAMEO_SET *cameoSet;

//----------------------------------------------------------------------------//
//----- FUNCTIONS EXTERNS ----------------------------------------------------//
//----------------------------------------------------------------------------//
extern u16  GetCurrentCameoKey				( void );
extern void CameoProcessController			( void );
extern void InitCameosForLevel				( unsigned long worldID, unsigned long levelID );
extern void CreateAndAddCameoAction			( unsigned long cflags, unsigned long frameNum );

extern void CreateAndAddCameoCameraSource	( unsigned long cFlags, unsigned long frameNum, float x, float y, float z, float camSpeed );

extern void CreateAndAddCameoAnimation		( unsigned long cFlags, unsigned long frameNum, char *objectName, unsigned long animNum );
extern void CreateAndAddCameoFrogMovement	( unsigned long cflags, unsigned long frameNum );

extern void CreateAndAddCameoObject ( char *cameoObject, float x, float y, float z, int initFlags  );

extern void FreeCameoList ( void );

/*extern void CreateCutScene	( void );
extern void RunCutScene		( void );
extern void ProcessCutScene ( void );
extern void ProcessFrogger	( void );
extern void ProcessCamera	( void );
extern void AddCameraActionToList ( unsigned long flags, unsigned long frameNum );
extern void AddFroggerActionToList ( unsigned long flags, unsigned long frameNum );

// Free Functions
extern void FreeCutScene		( void );
extern void FreeCameraActions	( void );
extern void FreeFroggerActions	( void );


// FOR DEVELPOMENT ONLY
extern void ViewCutScene	( void );
*/
#endif