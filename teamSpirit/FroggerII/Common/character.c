/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: character.c
	Programmer	: jim Hubbard
	Date		: 22/12/99 10:22

	Info		: I made this because we needed a hub character to respond to the frogs actions
					and be generally helpful to the novice.

					The problem is that enemies have fairly hardcoded paths, and hacking this stuff
					in would be a pain. They also need a queue of command rather than just one
					behaviour - adding this more complex behaviour to enemies would be impractical.

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>
#include "incs.h"



CHARACTER *characterList = NULL;


void FreeAICommandList( AICOMMAND *cl );
void FreeAIPathNodeList( AIPATHNODE *pl );
void SubAIPathNode( AIPATHNODE *p );
void NextAICommand( AICOMMAND *c );
void StartAICommand( CHARACTER *ch );

int BestFirst( AIPATHNODE *path, int depth, GAMETILE *previous, GAMETILE *tile, GAMETILE *goal );

// Generic for all critters
void CharFaceDir( CHARACTER *ch );		// Orientate to direction

// Creature can walk, so can't cross join tiles and generally does the path following kind of motion
void CharWalkToFrog( CHARACTER *ch );
void CharWalkToTile( CHARACTER *ch );

// Creature can fly, so skip join tiles and swoop round corners
void CharFlyToFrog( CHARACTER *ch );
void CharFlyToTile( CHARACTER *ch );

// For hub character specifically.
void CharHubPoint( CHARACTER *ch );		// Bounce in direction, like a jabbing kind of motion
void CharHubIdle( CHARACTER *ch );		// Just flit about a bit



/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddCharacter
	Purpose			: Make a character with basic attributes.
	Parameters		: Name of actor to create, GAMETILE to start on and the offset from it, and the type.
	Returns			: Created character
	Info			: 
*/
CHARACTER *CreateAndAddCharacter( char *name, GAMETILE *start, float offset, unsigned char type )
{
	CHARACTER *ch = (CHARACTER *)JallocAlloc( sizeof(CHARACTER), YES, "Character" );
	VECTOR pos;

	// Find position above worldTile
	SetVector( &pos, &start->normal );
	ScaleVector( &pos, offset );
	AddToVector( &pos, &start->centre );

	// create the actor
	ch->act = CreateAndAddActor( name, pos.v[X], pos.v[Y], pos.v[Z], INIT_ANIMATION | INIT_SHADOW );

	ch->type = type;
	ch->inTile = start;

	return ch;
}



/*	--------------------------------------------------------------------------------
	Function		: IssueAICommand
	Purpose			: Make a character do something
	Parameters		: Character and command
	Returns			: 
	Info			: 
*/
void IssueAICommand( CHARACTER *ch, AICOMMAND *command )
{
	if( !command || !ch ) return;

	if( command->flags & AICOMFLAG_QUEUED )
	{
		// If existing commands, append to queue
		if( ch->command )
		{
			AICOMMAND *c;
			for( c = ch->command; c->next; c = c->next );

			c->next = command;
		}
		else // Else this _is_ the queue.
		{
			ch->command = command;
			StartAICommand( ch );
		}
	}
	else if( command->flags & AICOMFLAG_INSTANT )
	{
		// If existing commands, replace the head of th queue (unfortunately I can't think of a valid way to preserve the current command)
		if( ch->command )
		{
			command->next = ch->command->next;
			ch->command = command;
		}
		else // This _is_ the queue
		{
			ch->command = command;
		}

		StartAICommand( ch );
	}
	else if( command->flags & AICOMFLAG_INTERRUPT )
	{
		// Free any existing command queue and start the new one
		FreeAICommandList( ch->command );
		ch->command = command;
		StartAICommand( ch );
	}
}


/*	--------------------------------------------------------------------------------
	Function		: SetupAICommand
	Purpose			: Start the command at the head of the queue
	Parameters		: Character
	Returns			: 
	Info			: 
*/
void StartAICommand( CHARACTER *ch )
{
	ch->Update = NULL;		// Reset to start with

	if( !ch->command ) return;

	// Assign update function
	switch( ch->command->type )
	{
	case AICOM_IDLE:
		if( ch->type == CHAR_HUB ) ch->Update = CharHubIdle;
		break;

	case AICOM_GOTO_TILE:
		switch( ch->type )
		{
		case CHAR_HUB:
			if( FindRoute( ch->path, ch->inTile, ch->command->target, YES ) )
				ch->Update = CharFlyToTile;
			break;

		default:
			if( FindRoute( ch->path, ch->inTile, ch->command->target, NO ) )
				ch->Update = CharWalkToTile;
			break;
		}
		break;

	case AICOM_GOTO_FROG:
		switch( ch->type )
		{
		case CHAR_HUB:
			if( FindRoute( ch->path, ch->inTile, currTile[0], YES ) )
				ch->Update = CharFlyToFrog;
			break;

		default:
			if( FindRoute( ch->path, ch->inTile, currTile[0], NO ) )
				ch->Update = CharWalkToFrog;
			break;
		}
		break;

	case AICOM_POINT_DIR:
		switch( ch->type )
		{
		case CHAR_HUB: ch->Update = CharHubPoint; break;
		default: ch->Update = CharFaceDir; break;
		}
		break;

	case AICOM_STOP:
		FreeAICommandList( ch->command );
		ch->Update = NULL;
		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: ProcessCharacters
	Purpose			: Check for command completed, trigger the next
	Parameters		: 
	Returns			: 
	Info			: 
*/
void ProcessCharacters( )
{
	CHARACTER *c;

	for( c = characterList; c; c = c->next )
	{
		if( c->command && (c->command->flags & AICOMFLAG_COMPLETE) )
		{
			dprintf"Type %i: Command %i completed\n",c->type, c->command->type));

			NextAICommand( c->command );
			StartAICommand( c );
		}

		if( c->Update )
			c->Update( c );
	}
}


/*	--------------------------------------------------------------------------------
	Function		: Generic Character action functions
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
// Orientate to direction
void CharFaceDir( CHARACTER *ch )
{

}

/*	--------------------------------------------------------------------------------
	Function		: Character Movement functions
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
// Creature can walk, so can't cross join tiles and generally does the path following kind of motion
void CharWalkToFrog( CHARACTER *ch )
{

}

void CharWalkToTile( CHARACTER *ch )
{

}

// Creature can fly, so skip join tiles and swoop round corners
void CharFlyToFrog( CHARACTER *ch )
{

}

void CharFlyToTile( CHARACTER *ch )
{

}

/*	--------------------------------------------------------------------------------
	Function		: Character Specific action functions
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
// For hub character specifically.
void CharHubPoint( CHARACTER *ch )		// Bounce in direction, like a jabbing kind of motion
{

}

void CharHubIdle( CHARACTER *ch )		// Just flit about a bit
{

}


/*	--------------------------------------------------------------------------------
	Function		: List functions, nothing exciting here.
	Purpose			: Manage the list of characters, the lists of command and pathnodes within characters
	Parameters		: 
	Returns			: 
	Info			: All singly linked except pathnode list, which is double.
*/
void FreeCharacterList( )
{
	CHARACTER *ch, *next;

	for( ch = characterList; ch; ch = next )
	{
		next = ch->next;
		SubCharacter( ch );
	}

	characterList = NULL;
}

void FreeAICommandList( AICOMMAND *cl )
{
	AICOMMAND *c, *next;

	for( c = cl; c; c = next )
	{
		next = c->next;
		JallocFree( (UBYTE **)&c );
	}

	cl = NULL;
}

void FreeAIPathNodeList( AIPATHNODE *pl )
{
	AIPATHNODE *p, *next;

	for( p = pl; p; p = next )
	{
		next = p->next;
		JallocFree( (UBYTE **)&p );
	}
}

void SubCharacter( CHARACTER *ch )
{
	FreeAICommandList( ch->command );
	FreeAIPathNodeList( ch->path );

	JallocFree( (UBYTE **)&ch );
}

void SubAIPathNode( AIPATHNODE *p )
{
	if( p->prev ) p->prev->next = p->next;
	if( p->next ) p->next->prev = p->prev;

	JallocFree( (UBYTE **)&p );
}

void NextAICommand( AICOMMAND *c )
{
	AICOMMAND *next = c->next;

	JallocFree( (UBYTE **)&c );
	c = next;
}







/*	--------------------------------------------------------------------------------
	Function		: FindRoute
	Purpose			: Find a consecutive sequence of tiles from start to target.
	Parameters		: Path to complete, start, end, fly flag
	Returns			: Confirm
	Info			: If fly is set then join tiles are ignored and the critter flies over them.
*/
int FindRoute( AIPATHNODE *path, GAMETILE *start, GAMETILE *target, char fly )
{
	if( !start || !target ) return FALSE;

	return( BestFirst(path, 0, NULL, start, target) );
}

/*	--------------------------------------------------------------------------------
	Function		: BestFirst
	Purpose			: Do a recursive search of a path to the target, using a heuristic to choose the branching direction
	Parameters		: Path to complete, depth counter, previous tile, current tile, goal tile
	Returns			: Confirm
	Info			: Needs a flag to not skip join tiles. I'll add it later, after this bugger's tested.

	********** WARNING! THIS FUNCTION IS EVIL! ***********
	** IT IS DOWN HERE TO AVOID SCARING SMALL CHILDREN! **
*/
int BestFirst( AIPATHNODE *path, int depth, GAMETILE *previous, GAMETILE *tile, GAMETILE *goal )
{
	// Terminating condition, success
	if( tile == goal )
	{
		path = (AIPATHNODE *)JallocAlloc( sizeof(AIPATHNODE), YES, "AIPath" );
		return TRUE;
	}
	else if( depth > MAX_SEARCH_DEPTH ) // Terminating condition, failure, depth exceeded
	{
		return FALSE;
	}
	else
	{
		unsigned char closed[4] = {0,0,0,0};
		int dir=-1, i;
		float dist, best = 9999999;

		// Check tile pointers, and find best direction match to search first
		for( i=0; i<4; i++ )
			if( tile->tilePtrs[i] == previous || !tile->tilePtrs[i] )
				closed[i] = 1;
			else
			{
				dist = DistanceBetweenPointsSquared( &goal->centre, &tile->tilePtrs[i]->centre );
				if( dist < best )
				{
					dir = i;
					best = dist;
				}
			}

		if( dir != -1 )
		{
			AIPATHNODE *node;
			// If the best choice branch got there, add this tile to the head of the path and return.
			if( BestFirst(path, depth+1, tile, tile->tilePtrs[dir], goal) )
			{
				node = (AIPATHNODE *)JallocAlloc( sizeof(AIPATHNODE), YES, "AIPath" );
				node->next = path;
				path->prev = node;
				path = node;
				return TRUE;
			}
			else closed[dir] = 1;	// Mark failed branch as closed

			best = 9999999;
			dir = -1;
			// Find the next best match
			for( i=0; i<4; i++ )
				if( !closed[i] ) // Only check needed since the first pass checked for backtracking and bad tile pointers
				{
					dist = DistanceBetweenPointsSquared( &goal->centre, &path->tile->tilePtrs[i]->centre );
					if( dist < best )
					{
						dir = i;
						best = dist;
					}
				}

			// Here we go again... check second branch for solution
			if( dir != -1 )
			{
				// If the second best choice branch got there, add this tile to the head of the path and return.
				if( BestFirst(path, depth+1, tile, tile->tilePtrs[dir], goal) )
				{
					node = (AIPATHNODE *)JallocAlloc( sizeof(AIPATHNODE), YES, "AIPath" );
					node->next = path;
					path->prev = node;
					path = node;
					return TRUE;
				}
				else closed[dir] = 1;	// Mark failed branch as closed

				best = 9999999;
				dir = -1;
				// Find the next best match
				for( i=0; i<4; i++ )
					if( !closed[i] ) // Only check needed since the first pass checked for backtracking and bad tile pointers
					{
						dist = DistanceBetweenPointsSquared( &goal->centre, &path->tile->tilePtrs[i]->centre );
						if( dist < best )
						{
							dir = i;
							best = dist;
						}
					}

				// Check last remaining branch for solution
				if( dir != -1 )
				{
					// If the third best choice branch got there, add this tile to the head of the path and return.
					if( BestFirst(path, depth+1, tile, tile->tilePtrs[dir], goal) )
					{
						node = (AIPATHNODE *)JallocAlloc( sizeof(AIPATHNODE), YES, "AIPath" );
						node->next = path;
						path->prev = node;
						path = node;
						return TRUE;
					}
				}
			}
		}
		
		return FALSE;	// Dead end, return failure
	}
}
