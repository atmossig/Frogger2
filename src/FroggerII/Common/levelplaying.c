/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: levelplaying.c
	Programmer	: James Healey
	Date		: 17/03/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


//----------------------------------------------------------------------------//
//----- GLOBALS --------------------------------------------------------------//
//----------------------------------------------------------------------------//
LEVELPLAYING me;

LEVELPLAYING *levelPlaying = &me;

//----------------------------------------------------------------------------//
//----- FUNCTIONS ------------------------------------------------------------//
//----------------------------------------------------------------------------//

void AddPlayingActionToList ( unsigned long flags, unsigned long frameNum )
{
	PLAYINGACTIONS *cur;

	cur = ( PLAYINGACTIONS *) JallocAlloc ( sizeof ( PLAYINGACTIONS ), YES, "PLAYACT" );
	
	cur->frameNum	 = frameNum;
	cur->nextAction	 = NULL;
	cur->actions    |= flags;

	cur->nextAction  = levelPlaying->actions;

	levelPlaying->actions = cur;
}

u16 GetCurrentRecordKey ( void )
{
	PLAYINGACTIONS *cur;

	cur = levelPlaying->actions;

	while ( cur )
	{
		if ( cur->frameNum == frameCount )
		{
			if ( cur->actions & MOVEMENT_LEFT )
				return CONT_LEFT;
			if ( cur->actions & MOVEMENT_RIGHT )
				return CONT_RIGHT;
			if ( cur->actions & MOVEMENT_UP )
				return CONT_UP;
			if ( cur->actions & MOVEMENT_DOWN )
				return CONT_DOWN;
			if ( cur->actions & CAMERA_LEFT )
				return CONT_F;
			if ( cur->actions & CAMERA_RIGHT )
				return CONT_C;
			if ( cur->actions & CAMERA_IN )
				return CONT_E;
			if ( cur->actions & CAMERA_OUT )
				return CONT_D;
		}

		cur = cur->nextAction;
	}

	return 0;
}


void LevelPlayingProcessController ( void )
{
	static u16 button,lastbutton;
    
	button = controllerdata [ ActiveController ].button;

	if ( ( button & CONT_START ) && ! ( lastbutton & CONT_START ) )
	{
		FreeAllLists();
		levelPlaying = 0;

		frog[0] = NULL;
		gameState.mode = FRONTEND_MODE;	
		frameCount = 0;
		lastbutton = 0;

		ShowJalloc();
		return;
    }

	lastbutton = button;

}

void FreeLevelPlaying ( void )
{
	FreePlayingActions();

}

/*	--------------------------------------------------------------------------------
	Function		: FreePlayingActions
	Purpose			: frees resources assigned to the actions list
	Parameters		:
	Returns			: void
	Info			:
*/
void FreePlayingActions ( void )
{
	PLAYINGACTIONS *cur, *next;

	cur = levelPlaying->actions;

	while ( cur )
	{
		next = cur->nextAction;
		JallocFree ( (UBYTE**)&cur );
		cur = next;

	}
	// endwhile - cur

	levelPlaying->actions = NULL;
}


