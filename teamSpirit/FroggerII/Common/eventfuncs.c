/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.
	
	  
	File		: eventfuncs.c
	Programmer	: Jim Hubbard
	Date		: 30/06/99

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>
#include "incs.h"



/*----- [ TRIGGER FUNCTIONS ] ------------------------------------------------------------------*/

/*	--------------------------------------------------------------------------------
	Function 	: ActorOnTile
	Purpose 	: Check if the actor in the trigger structure is on the given tile
	Parameters 	: Pointer to trigger structure
	Returns 	: Boolean
	Info 		:
*/
int ActorOnTile( TRIGGER *trigger )
{
	/*
	* DO THIS LATER
	*/
	return 0;
}


/*	--------------------------------------------------------------------------------
	Function 	: ActorWithinRadius
	Purpose 	: Check if the actor in the trigger structure is within a distance of a point
	Parameters 	: Pointer to trigger structure
	Returns 	: Boolean
	Info 		:
*/
int ActorWithinRadius( TRIGGER *trigger )
{
	ACTOR *actor = (ACTOR *)trigger->data[0];
	VECTOR *pos = (VECTOR *)trigger->data[1];
	float *radius = (float *)trigger->data[2];

	if( DistanceBetweenPoints(&actor->pos, pos) < *radius ) return 1;
	else return 0;
}



/*----- [ EVENT FUNCTIONS ] --------------------------------------------------------------------*/

/*	--------------------------------------------------------------------------------
	Function 	: ChangeActorScale
	Purpose 	: Alters the scale property of the actor
	Parameters 	: Pointer to event structure
	Returns 	: 
	Info 		:
*/
void ChangeActorScale( EVENT *event )
{
	ACTOR *actor = (ACTOR *)event->data[0];
	VECTOR *scale = (VECTOR *)event->data[1];

	actor->scale = *scale;
}
