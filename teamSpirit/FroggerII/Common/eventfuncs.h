/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.
	
	  
	File		: event.h
	Programmer	: Jim Hubbard
	Date		: 30/06/99
	
----------------------------------------------------------------------------------------------- */

#ifndef EVENTFUNCS_H_INCLUDED
#define EVENTFUNCS_H_INCLUDED


/*----- [ TRIGGER PROTOTYPES ] -----------------------------------------------------------------*/
extern int EnemyOnTile( TRIGGER *trigger );
extern int FrogOnTile( TRIGGER *trigger );
extern int ActorWithinRadius( TRIGGER *trigger );
extern int OnTimeout( TRIGGER *trigger );

/*----- [ EVENT PROTOTYPES ] -------------------------------------------------------------------*/
extern void ChangeActorScale( EVENT *event );
extern void AssignFloatToFloat( EVENT *event );
extern void AssignIntToInt( EVENT *event );

/*----- [ LEVEL SETUP PROTOTYPES ] -------------------------------------------------------------*/
extern void InitEventsForLevel( unsigned long worldID, unsigned long levelID );

#endif