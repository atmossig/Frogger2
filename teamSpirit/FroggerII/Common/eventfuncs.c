/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.
	
	  
	File		: eventfuncs.c
	Programmer	: Jim Hubbard
	Date		: 30/06/99

	Info		: Trigger and event functions for the event system, and setups for 
					level specific events.

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>
#include "incs.h"



/*----- [ TRIGGER FUNCTIONS ] ------------------------------------------------------------------*/

/*	--------------------------------------------------------------------------------
	Function 	: EnemyOnTile
	Purpose 	: Check if an enemy in the trigger structure is on the given tile
	Parameters 	: Pointer to trigger structure
	Returns 	: Boolean
	Info 		:
*/
int EnemyOnTile( TRIGGER *trigger )
{
	ENEMY *nme = (ENEMY *)trigger->data[0];
	GAMETILE *tile = (GAMETILE *)trigger->data[1];

	if( nme->inTile == tile )
		return 1;

	return 0;
}


/*	--------------------------------------------------------------------------------
	Function 	: FrogOnTile
	Purpose 	: Check if a frog in the trigger structure is on the given tile
	Parameters 	: Pointer to trigger structure
	Returns 	: Boolean
	Info 		:
*/
int FrogOnTile( TRIGGER *trigger )
{
	int frognum = *(int *)trigger->data[0];
	GAMETILE *tile = (GAMETILE *)trigger->data[1];

	if( currTile[frognum] == tile )
		return 1;

	return 0;
}


/*	--------------------------------------------------------------------------------
	Function 	: FrogOnPlatform
	Purpose 	: Check if a frog in the trigger structure is on the given platform
	Parameters 	: Pointer to trigger structure
	Returns 	: Boolean
	Info 		:
*/
int FrogOnPlatform( TRIGGER *trigger )
{
	ACTOR2 *frog = (ACTOR2 *)trigger->data[0];
	PLATFORM *plt = (PLATFORM *)trigger->data[1];

	if( plt->carrying == frog )
		return 1;

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
	float radius = *(float *)trigger->data[2];

	if( DistanceBetweenPoints(&actor->pos, pos) < radius )
		return 1;

	return 0;
}


/*	--------------------------------------------------------------------------------
	Function 	: OnTimeout
	Purpose 	: Trigger when counter reaches zero
	Parameters 	: Pointer to trigger structure
	Returns 	: Boolean
	Info 		:
*/
int OnTimeout( TRIGGER *trigger )
{
	int count = *(int *)trigger->data[0];  // Counter
	int start = *(int *)trigger->data[1];  // Count down from this

	if( count-- == -1 )
		count = start;
	else if( count-- == 0 )
		return 1;
	else
		count--;
}


/*	--------------------------------------------------------------------------------
	Function 	: OnTrigger
	Purpose 	: Trigger if another trigger fires
	Parameters 	: Pointer to trigger structure
	Returns 	: Boolean
	Info 		:
*/
int OnTrigger( TRIGGER *trigger )
{
	TRIGGER *t = (TRIGGER *)trigger->data[0];

	if( t->flags & TRIGGER_FIRED )
		return 1;

	return 0;
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


/*	--------------------------------------------------------------------------------
	Function 	: AssignFloatToFloat
	Purpose 	: Assigns a number to any float.
	Parameters 	: Pointer to event structure
	Returns 	: 
	Info 		:
*/
void AssignFloatToFloat( EVENT *event )
{
	float *ass = (float *)event->data[0];
	float val = *(float *)event->data[1];

	*ass = val;
}


/*	--------------------------------------------------------------------------------
	Function 	: AssignIntToInt
	Purpose 	: Assigns a number to any float.
	Parameters 	: Pointer to event structure
	Returns 	: 
	Info 		:
*/
void AssignIntToInt( EVENT *event )
{
	int *ass = (int *)event->data[0];
	int val = *(int *)event->data[1];

	*ass = val;
}


/*	--------------------------------------------------------------------------------
	Function 	: TogglePlatformMove
	Purpose 	: Make a floating platform halt or continue its path
	Parameters 	: Pointer to event structure
	Returns 	: 
	Info 		: Sets wait timer on next tile to achieve its purpose
*/
void TogglePlatformMove( EVENT *event )
{
	PLATFORM *plt = (PLATFORM *)event->data[0];

	plt->isWaiting = (plt->isWaiting) ? 0 : -1;
}


/*	--------------------------------------------------------------------------------
	Function 	: ToggleEnemyMove
	Purpose 	: Make an enemy halt or continue its path
	Parameters 	: Pointer to event structure
	Returns 	: 
	Info 		: Sets speed of enemies
*/
void ToggleEnemyMove( EVENT *event )
{
	ENEMY *nme = (ENEMY *)event->data[0];

	nme->speed = (nme->speed) ? 0 : nme->startSpeed;
}

/*	--------------------------------------------------------------------------------
	Function 	: ToggleTileLink
	Purpose 	: Change whether it is possible to go from one tile to another
	Parameters 	: Pointer to event structure
	Returns 	: 
	Info 		: Sets/unsets TILESTATE_BARRED flag in tile->state.
*/
void ToggleTileLink( EVENT *event )
{
	GAMETILE *tile = (GAMETILE *)event->data[0];

	tile->state = (tile->state==TILESTATE_BARRED) ? TILESTATE_NORMAL : TILESTATE_BARRED;
}



/*----- [ LEVEL SETUP ] ------------------------------------------------------------------------*/

/*	--------------------------------------------------------------------------------
	Function 	: InitEventsForLevel
	Purpose 	: Adds all triggers and events for the current level
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitEventsForLevel( unsigned long worldID, unsigned long levelID )
{
	void **args;
	TRIGGER *trigger;
	EVENT *event;
	VECTOR *pos = (VECTOR *)JallocAlloc( sizeof(VECTOR),YES,"Vector" );
	VECTOR *scale = (VECTOR *)JallocAlloc( sizeof(VECTOR),YES,"Vector" );
	float *fnum;
	int *inum;

	InitTriggerList( );

	if ( worldID == WORLDID_GARDEN )
	{
		if ( levelID == LEVELID_GARDENLAWN )
		{
			args = AllocArgs(2);
			args[0] = (void *)frog[0];
			args[1] = (void *)platformList.head.next;
			trigger = MakeTrigger( FrogOnPlatform, 2, args );

			args = AllocArgs(1);
			args[0] = (void *)platformList.head.next->next;
			event = MakeEvent( TogglePlatformMove, 1, args );

			AttachEvent( trigger, event, TRIGGER_DELAY, 100 );

		
			args = AllocArgs(1);
			args[0] = (void *)trigger;
			trigger = MakeTrigger( OnTrigger, 1, args );

			args = AllocArgs(1);
			args[0] = (void *)platformList.head.next->next->next;
			event = MakeEvent( TogglePlatformMove, 1, args );

			AttachEvent( trigger, event, TRIGGER_DELAY, 100 );

		}
		else if( levelID == LEVELID_GARDENMAZE )
		{
			fnum = (float *)JallocAlloc( sizeof(float),YES,"Float" );
			*fnum = 100;
			
			args = AllocArgs(3);
			args[0] = (void *)frog[0]->actor;
			args[1] = (void *)&enemyList.head.next->nmeActor->actor->pos;
			args[2] = (void *)fnum;
			trigger = MakeTrigger( ActorWithinRadius, 3, args );
			
			args = AllocArgs(1);
			args[0] = (void *)enemyList.head.next;
			event = MakeEvent( ToggleEnemyMove, 1, args );

			AttachEvent( trigger, event, TRIGGER_DELAY, 100 );
		}
		else if( levelID == LEVELID_GARDENVEGPATCH )
		{
			pos->v[0] = -75.0;
			pos->v[1] = 0.0;
			pos->v[2] = -375.0;
			
			fnum = (float *)JallocAlloc( sizeof(float),YES,"Float" );
			*fnum = 100;

			// Test trigger
			args = AllocArgs(3);//(void **)JallocAlloc( (sizeof(void *)*3),YES,"EventData" );
			args[0] = (void *)frog[0]->actor;
			args[1] = (void *)pos;
			args[2] = (void *)fnum;

			trigger = MakeTrigger( ActorWithinRadius, 3, args );

			scale->v[0] = 1.0;
			scale->v[1] = 1.0;
			scale->v[2] = 1.0;

			// Test event
			args = AllocArgs(2);//(void **)JallocAlloc( (sizeof(void *)*2),YES,"EventData" );
			args[0] = (void *)frog[0]->actor;
			args[1] = (void *)scale;

			event = MakeEvent( ChangeActorScale, 2, args );

			// Attach event to trigger
			AttachEvent( trigger, event, TRIGGER_ONCE, 0 );
		}
	} // etc
}