/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: cameos.c
	Programmer	: James Healey
	Date		: 21/05/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"

//----------------------------------------------------------------------------//
//----- GLOBALS --------------------------------------------------------------//
//----------------------------------------------------------------------------//

// Object list for objects that are used in cameos
CAMEOOBJECT *cameoObjectList = NULL;

// The cameo set that holds the entire cameo for the selected level
CAMEO_SET *cameoSet = NULL;

//----------------------------------------------------------------------------//
//----- FUNCTIONS ------------------------------------------------------------//
//----------------------------------------------------------------------------//

/*	--------------------------------------------------------------------------------
	Function		: InitCameosForLevel
	Purpose			: Initialises the cameos for a level
	Parameters		:	unsigned long worldID - the selected world, from the level select
						unsigned long levelID - the selected level, from the level select
	Returns			: void
	Info			: 
*/
void InitCameosForLevel ( unsigned long worldID, unsigned long levelID )
{
/*
	float x, y, z;

	cameoSet = ( CAMEO_SET * ) JallocAlloc ( sizeof ( CAMEO_SET ), YES, "CAM_SET" );

	cameoSet->numCameos = 0;
	cameoSet->cameos	= NULL;

	if ( worldID == WORLDID_GARDEN )
	{
	}
*/
}

/*	--------------------------------------------------------------------------------
	Function		: InitCameosForLevel
	Purpose			: Initialises the cameos for a level
	Parameters		:	unsigned long worldID - the selected world, from the level select
						unsigned long levelID - the selected level, from the level select
	Returns			: void
	Info			: 
*/
void FreeCameoList ( void )
{
	CAMEOOBJECT *curObj		= NULL,	*nextObj	= NULL;
	CAMEO		*curCameo	= NULL,	*nextCameo	= NULL;

	curObj = cameoObjectList;

	while ( curObj )
	{
		nextObj = curObj->next;
		JallocFree ( (UBYTE**)&curObj );
		curObj = nextObj;
	}
	// ENDWHILE

	cameoObjectList = NULL;

	if ( cameoSet )
		curCameo = cameoSet->cameos;
	// ENDIF - cameoSet

	while ( curCameo )
	{
		nextCameo = curCameo->next;
		JallocFree ( (UBYTE**)&curCameo );
		curCameo = nextCameo;
	}
	// ENDWHILE

	if ( cameoSet )
		cameoSet->cameos = NULL;
	// ENDIF - cameoSet

	JallocFree ( (UBYTE**)&cameoSet );

	cameoSet = NULL;
}


/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddCameoObject
	Purpose			: Creates and adds an object to the cameo list
	Parameters		:	char* cameoObject - the object name to enter into the list
						float x - the world x position
						float y - the y world position
						float z - the z world position
						int flags - any flags that need to be done
	Returns			: void
	Info			: 
*/
void CreateAndAddCameoObject ( char *cameoObject, float x, float y, float z, int initFlags  )
{
	CAMEOOBJECT *newItem = ( CAMEOOBJECT * ) JallocAlloc ( sizeof ( CAMEOOBJECT ), YES, "camobj" );

	newItem->cameoActor = CreateAndAddActor ( cameoObject, x, y, z, initFlags, 0, 0 );
	newItem->objectName = cameoObject;

	newItem->next	= cameoObjectList;
	cameoObjectList = newItem;
}

/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddCameoAction
	Purpose			: Creates and adds a cameo action to the cameoset list
	Parameters		:	unsigned long cFlags - any flags that need to be done						
						unsigned long frameNum - frame number to start this action on
	Returns			: void
	Info			: 
*/
void CreateAndAddCameoAction ( unsigned long cFlags, unsigned long frameNum )
{
	CAMEO *cameo 	 = ( CAMEO * ) JallocAlloc ( sizeof ( CAMEO ), YES, "CAMEO" );

	cameo->flags		= cFlags;
	cameo->frameNum		= frameNum;

	cameo->linkObject	= NULL;
	cameo->animNum		= 0;
	cameo->x			= 0;
	cameo->y			= 0;
	cameo->z			= 0;

	cameo->next		 = cameoSet->cameos;
	cameoSet->cameos = cameo;
}

/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddCameoCameraSource
	Purpose			: Creates and adds a cameo camera source position to the cameoset list
	Parameters		:	unsigned long cFlags - any flags that need to be done						
						unsigned long frameNum - frame number to start this action on
						float x - the world x position
						float y - the world y position
						float z - the world z position
						float camSpeed - the speed at which the camera will move at
	Returns			: void
	Info			: 
*/
void CreateAndAddCameoCameraSource ( unsigned long cFlags, unsigned long frameNum, float x, float y, float z, float camSpeed )
{
	CAMEO *cameo 	 = ( CAMEO * ) JallocAlloc ( sizeof ( CAMEO ), YES, "CAMEO" );

	cameo->flags	 = cFlags;
	cameo->frameNum	 = frameNum;
	cameo->x		 = x;
	cameo->y		 = y;
	cameo->z		 = z;
	cameo->camSpeed  = camSpeed;

	cameo->linkObject	= NULL;
	cameo->animNum		= 0;

	cameo->next		 = cameoSet->cameos;
	cameoSet->cameos = cameo;
}

/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddCameoAnimation
	Purpose			: Creates and adds a cameo animation to cameo set list
	Parameters		:	unsigned long cFlags - any flags that need to be done						
						unsigned long frameNum - frame number to start this action on
						char *objectName - the object name to enter into the list
						unsigned long animnum - the animation umber to run
	Returns			: void
	Info			: 
*/
void CreateAndAddCameoAnimation ( unsigned long cFlags, unsigned long frameNum, char *objectName, unsigned long animNum )
{
	CAMEOOBJECT *cur;

	CAMEO *cameo 	 = ( CAMEO * ) JallocAlloc ( sizeof ( CAMEO ), YES, "CAMEO" );

	cur = cameoObjectList;
	while ( cur )
	{
		if ( gstrcmp ( objectName, cur->objectName ) == 0 )
		{
			cameo->linkObject	= cur;
			cameo->animNum		= animNum;
			break;
		}
		// ENDIF
		cur = cur->next;
	}
	// ENDWHILE

	cameo->flags	 = cFlags;
	cameo->frameNum	 = frameNum;

	cameo->x		 = 0;
	cameo->y		 = 0;
	cameo->z		 = 0;

	cameo->next		 = cameoSet->cameos;
	cameoSet->cameos = cameo;
}

/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddCameoFrogMovement
	Purpose			: Creates and adds a cameo frog movement to the cameo set list
	Parameters		:	unsigned long cFlags - any flags that need to be done						
						unsigned long frameNum - frame number to start this action on
	Returns			: void
	Info			: 
*/
void CreateAndAddCameoFrogMovement ( unsigned long cFlags, unsigned long frameNum )
{
	CAMEOOBJECT *cur;

	CAMEO *cameo 	 = ( CAMEO * ) JallocAlloc ( sizeof ( CAMEO ), YES, "CAMEO" );

	cameo->flags	 = cFlags;
	cameo->frameNum	 = frameNum;

	cameo->linkObject	= NULL;
	cameo->animNum		= 0;
	cameo->x			= 0;
	cameo->y			= 0;
	cameo->z			= 0;

	cameo->next		 = cameoSet->cameos;
	cameoSet->cameos = cameo;
}

/*	--------------------------------------------------------------------------------
	Function		: UpdateCameos
	Purpose			: Update any of the cameo action, animations, frogger movements etc
	Parameters		: void
	Returns			: void
	Info			: 
*/
void UpdateCameos ( void )
{
	CAMEO *cur = NULL;

	if ( cameoSet )
		cur = cameoSet->cameos;
	// ENDIF cameoSet

	while ( cur )
	{
		if ( cur->frameNum == frameCount )
		{
			if ( ( cur->flags & CAMEO_START ) || ( gameState.mode = CAMEO_MODE ) )
			{
				gameState.mode = CAMEO_MODE;

				if ( cur->flags & CAMEO_ANIMATE )
				{
					AnimateActor ( cur->linkObject->cameoActor->actor, cur->animNum, NO, NO, 0.5F, 0, 0);
				}
				// ENDIF

				if ( cur->flags & CAMEO_CAMERA )
				{
					/*camDist.v[X] = cur->x;
					camDist.v[Y] = cur->y;
					camDist.v[Z] = cur->z;  */
					camSource[0].v[X] = cur->x;
					camSource[0].v[Y] = cur->y;
					camSource[0].v[Z] = cur->z;  
					camSpeed		  = cur->camSpeed;
//					camTarget[0].v[X] = cur->x+200;
//					camTarget[0].v[Y] = cur->y-100;
//					camTarget[0].v[Z] = cur->z-500;  
				}
				// ENDIF

				if ( cur->flags & CAMEO_FINISH )
				{
					gameState.mode	=  GAME_MODE;
					camSpeed		= 9;
//					frameCount = 400;
				}
				// ENDIF
			}
			// ENDIF
		}
		// ENDIF

		cur = cur->next;
	}
	// ENDWHILE
}


/*	--------------------------------------------------------------------------------
	Function		: CameoProcessController
	Purpose			: Checks to see if the user has pressed any buttons during the cameo
	Parameters		: void
	Returns			: void
	Info			: 
*/
void CameoProcessController ( void )
{
	static u16 button,lastbutton;
    
	button = controllerdata [ ActiveController ].button;

	if ( ( button & CONT_START ) && ! ( lastbutton & CONT_START ) )
	{
//		levelPlaying = 0;

//		frog = NULL;
		gameState.mode = GAME_MODE;	
//		frameCount = 0;
//		lastbutton = 0;

//		currCamSource.v[0] = 0;	currCamSource.v[1] = 0;	currCamSource.v[2] = -100;
//		currCamTarget.v[0] = 0;	currCamTarget.v[1] = 0;	currCamTarget.v[2] = 0;

//		ShowJalloc();
		return;
    }

	lastbutton = button;

}

/*	--------------------------------------------------------------------------------
	Function		: GetCurrentCameoKey
	Purpose			: Checks any record keys for cameos
	Parameters		: void
	Returns			: u16 - returns the button that needs to be processed
	Info			: 
*/
u16 GetCurrentCameoKey ( void )
{
	CAMEO *cur = NULL;

	if ( cameoSet )
		cur = cameoSet->cameos;
	// ENDIF - cameoSet

	while ( cur )
	{
		if ( cur->frameNum == frameCount )
		{
			if ( cur->flags & CAMEO_LEFT )
			{
				return CONT_LEFT;
			}
			// ENDIF
			if ( cur->flags & CAMEO_RIGHT )
			{
				return CONT_RIGHT;
			}
			// ENDIF
			if ( cur->flags & CAMEO_UP )
			{
				return CONT_UP;
			}
			// ENDIF
			if ( cur->flags & CAMEO_DOWN )
			{
				return CONT_DOWN;
			}
			// ENDIF
		}
		// ENDIF cur->frameNum == frameCount
		cur = cur->next;
	}
	// ENDWHILE - cur
}

