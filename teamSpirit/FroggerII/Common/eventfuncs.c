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


/*	--------------------------------------------------------------------------------
	Function 	: LogicalAND
	Purpose 	: Trigger if an arbitrary number of other triggers are all true
	Parameters 	: Pointer to trigger structure
	Returns 	: Boolean
	Info 		: First parameter is the number of other triggers, the rest are pointers to triggers
*/
int LogicalAND( TRIGGER *trigger )
{
	int numT = *(int *)trigger->data[0];
	long i;
	short fireFlag = 1;
	TRIGGER *t;

	for( i=0; i<numT; i++ )
	{
		t = (TRIGGER *)trigger->data[i+1];
		if( !t->func(t) )
			fireFlag = 0;
	}

	if( fireFlag )
		return 1;

	return 0;
}

/*	--------------------------------------------------------------------------------
	Function 	: LogicalOR
	Purpose 	: Trigger if any one of an arbitrary number of other triggers is true
	Parameters 	: Pointer to trigger structure
	Returns 	: Boolean
	Info 		: First parameter is the number of other triggers, the rest are 
					pointers to triggers
*/
int LogicalOR( TRIGGER *trigger )
{
	int numT = *(int *)trigger->data[0];
	long i;
	short fireFlag = 0;
	TRIGGER *t;

	for( i=0; i<numT; i++ )
	{
		t = (TRIGGER *)trigger->data[i+1];
		if( t->func(t) )
			fireFlag = 1;
	}

	if( fireFlag )
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

	nme->isWaiting = (nme->isWaiting) ? 0 : -1;
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


/*	--------------------------------------------------------------------------------
	Function 	: PlaySFX
	Purpose 	: Play a sound, either sourceless or at a point
	Parameters 	: Pointer to event structure
	Returns 	: 
	Info 		: 
*/
void PlaySFX( EVENT *event )
{
	int snum = *(int *)event->data[0];
	int vol = *(int *)event->data[1];
	int pitch = *(int *)event->data[2];
	VECTOR *point = (VECTOR *)event->data[3];
	float radius = *(float *)event->data[4];

	if( point )
		PlaySampleRadius( snum, point, vol, pitch, radius );
	else
		PlaySample( snum, point, vol, pitch );
}

/*	--------------------------------------------------------------------------------
	Function 	: EvAnimateActor
	Purpose 	: Animate an actor
	Parameters 	: Pointer to event structure
*/

void EvAnimateActor( EVENT *event )
{
	ACTOR *actor = (ACTOR *)event->data[0];
	int anim = *(int*)event->data[1];
	int flags = *(int*)event->data[2];
	float speed = *(float*)event->data[3];

	AnimateActor(actor, anim, (flags & 1) != 0, (flags & 2) != 0, speed);
}


/*	--------------------------------------------------------------------------------
	Function 	: ChangeLevel
	Purpose 	: Go to a new level in the game
	Parameters 	: Pointer to event structure
	Returns 	: 
	Info 		: I hope this deallocs everything...
*/
void ChangeLevel( EVENT *event )
{
	int wNum = *(int *)event->data[0],
		lNum = *(int *)event->data[1];

	player[0].worldNum = wNum;
	player[0].levelNum = lNum;

	levelIsOver = 400;
	showEndLevelScreen = 0;
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
			/*// This should change to level when you hop onto the first platform
			args = AllocArgs(2);
			args[0] = (void *)frog[0];
			args[1] = (void *)platformList.head.next;
			trigger = MakeTrigger( FrogOnPlatform, 2, args );

			args = AllocArgs(2);

			inum = (int *)JallocAlloc( sizeof(int),YES,"Int" );
			*inum = 0;
			args[0] = (void *)inum;

			inum = (int *)JallocAlloc( sizeof(int),YES,"Int" );
			*inum = 2;
			args[1] = (void *)inum;

			event = MakeEvent( ChangeLevel, 2, args );

			AttachEvent( trigger, event, TRIGGER_ONCE, 0 );
			*/
			
			/*	//This is an example of chained events, and some basic events and triggers
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
			*/
		}
		else if( levelID == LEVELID_GARDENMAZE )
		{
			/* // An example of toggling the motion of an enemy
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
			*/
		}
		else if( levelID == LEVELID_GARDENVEGPATCH )
		{
			/* // This triggers a sound when frog is near to baby frog
			fnum = (float *)JallocAlloc( sizeof(float),YES,"Float" );
			*fnum = 500;

			args = AllocArgs(3);
			args[0] = (void *)frog[0]->actor;
			args[1] = (void *)&bTStart[2]->centre;
			args[2] = (void *)fnum;

			trigger = MakeTrigger( ActorWithinRadius, 3, args );

			args = AllocArgs(5);
			inum = (int *)JallocAlloc( sizeof(int),YES,"Int" );
			*inum = FX_CHICKEN_BELCH;
			args[0] = (void *)inum;

			inum = (int *)JallocAlloc( sizeof(int),YES,"Int" );
			*inum = 200;
			args[1] = (void *)inum;

			inum = (int *)JallocAlloc( sizeof(int),YES,"Int" );
			*inum = 50;
			args[2] = (void *)inum;

			args[3] = (void *)&bTStart[2]->centre; // No position for sound

			fnum = (float *)JallocAlloc( sizeof(float),YES,"Float" );
			*fnum = 10;
			args[4] = (void *)fnum;

			event = MakeEvent( PlaySFX, 5, args );

			// Attach event to trigger
			AttachEvent( trigger, event, TRIGGER_DELAY, 100 );
			*/
		}
		//else if( levelID == LEVELID_GARDENTREETOPSA )
		//{
			/*// This should change to level when you hop onto the first platform
			args = AllocArgs(2);
			args[0] = (void *)frog[0];
			args[1] = (void *)platformList.head.next;
			trigger = MakeTrigger( FrogOnPlatform, 2, args );

			args = AllocArgs(2);

			inum = (int *)JallocAlloc( sizeof(int),YES,"Int" );
			*inum = 0;
			args[0] = (void *)inum;

			inum = (int *)JallocAlloc( sizeof(int),YES,"Int" );
			*inum = 2;
			args[1] = (void *)inum;

			event = MakeEvent( ChangeLevel, 2, args );

			AttachEvent( trigger, event, TRIGGER_ONCE, 0 );
			*/

		//}
	} // etc
}