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


CHARACTER *hubChar = NULL;
CHARACTER *characterList = NULL;


void FreeAICommandList( AICOMMAND *cl );
void FreeAIPathNodeList( AIPATHNODE *pl );
void SubAIPathNode( AIPATHNODE *p );
void NextAICommand( AICOMMAND **c );
void StartAICommand( CHARACTER *ch );

int BestFirst( AIPATHNODE *path, int depth, GAMETILE *previous, GAMETILE *tile, GAMETILE *goal, char fly );

// Generic for all critters
void CharFaceDir( CHARACTER *ch );		// Orientate to direction

// Creature can walk, so can't cross join tiles and generally does the path following kind of motion
void CharWalkToTile( CHARACTER *ch );
// Creature can fly, so skip join tiles and swoop round corners
void CharFlyToTile( CHARACTER *ch );

// For hub character specifically.
void CharHubPoint( CHARACTER *ch );		// Bounce in direction, like a jabbing kind of motion
void CharHubIdle( CHARACTER *ch );		// Just flit about a bit

// Path functions
void LocateAIPathNode( VECTOR *pos, AIPATHNODE *node, float offset );


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

	ch->next = characterList;
	characterList = ch;

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
		{
			AIPATHNODE *path = (AIPATHNODE *)JallocAlloc(sizeof(AIPATHNODE),YES,"AIPath");

			switch( ch->type )
			{
			case CHAR_HUB:
				if( BestFirst( path, 0, NULL, ch->command->target, ch->inTile, YES ) )
					ch->Update = CharFlyToTile;
				break;

			default:
				if( BestFirst( path, 0, NULL, ch->command->target, ch->inTile, NO ) )
					ch->Update = CharWalkToTile;
				break;
			}

			if( ch->Update )
				ch->path = path;
			else
			{
				JallocFree( (UBYTE **)path );
				ch->command->flags |= AICOMFLAG_FAILED;
			}
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

	ch->node = ch->path;
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
		if( c->command && ((c->command->flags & AICOMFLAG_COMPLETE) || (c->command->flags & AICOMFLAG_FAILED)) )
		{
			if( c->command->flags & AICOMFLAG_COMPLETE )
				dprintf"Type %i: Command %i completed\n",c->type, c->command->type));
			else if( c->command->flags & AICOMFLAG_FAILED )
				dprintf"Type %i: Command %i failed\n",c->type, c->command->type));

			FreeAIPathNodeList( c->path );
			NextAICommand( &c->command );
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
void CharWalkToTile( CHARACTER *ch )
{
	VECTOR fwd, from, to;

	LocateAIPathNode( &to, ch->node, ch->command->offset );

	// Set target node
	if( DistanceBetweenPointsSquared(&ch->act->actor->pos,&to) < 10 )
		ch->node = ch->node->next;

	// We have arrived - command is finished
	if( !ch->node )
	{
		ch->command->flags |= AICOMFLAG_COMPLETE;
		return;
	}
	else
		ch->inTile = ch->node->tile;

	LocateAIPathNode( &to, ch->node, ch->command->offset );

	SubVector( &fwd, &to, &ch->act->actor->pos );
	MakeUnit( &fwd );

	// TODO: Normal interpolation

	ActorLookAt( ch->act->actor, &to, LOOKAT_ANYWHERE );

	ScaleVector( &fwd, ch->command->speed*gameSpeed );
	AddToVector( &ch->act->actor->pos, &fwd );
}

// Creature can fly, so skip join tiles and swoop round corners
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
	Function		: LocateAIPathNode
	Purpose			: Desired point, including offset
	Parameters		: 
	Returns			: 
	Info			: 
*/
void LocateAIPathNode( VECTOR *pos, AIPATHNODE *node, float offset )
{
	pos->v[X] = node->tile->centre.v[X] + (node->tile->normal.v[X] * offset);
	pos->v[Y] = node->tile->centre.v[Y] + (node->tile->normal.v[Y] * offset);
	pos->v[Z] = node->tile->centre.v[Z] + (node->tile->normal.v[Z] * offset);
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

void NextAICommand( AICOMMAND **c )
{
	AICOMMAND *fr, *next = (*c)->next;

	fr = *c;
	JallocFree( (UBYTE **)&fr );
	*c = next;
}


void TestPath( )
{
	AIPATHNODE *node, *path = (AIPATHNODE *)JallocAlloc( sizeof(AIPATHNODE), YES, "AIPath" );

	if( BestFirst( path, 0, NULL, currTile[0], gTStart[0], YES ) )
	{
		SPECFX *fx;
		for( node = path; node; node = node->next )
			fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_STATIC, &node->tile->centre, &node->tile->normal, 64, 0, 0, 5 );
	}

	FreeAIPathNodeList( path );
}


/*	--------------------------------------------------------------------------------
	Function		: BestFirst
	Purpose			: Do a recursive search of a path to the target, using a heuristic to choose the branching direction
	Parameters		: Path to complete, depth counter, previous tile, current tile, goal tile
	Returns			: Path success
	Info			: 
*/
int BestFirst( AIPATHNODE *path, int depth, GAMETILE *previous, GAMETILE *tile, GAMETILE *goal, char fly )
{
	// Terminating condition, success - start the path building
	if( tile == goal )
	{
		path->tile = tile;
		return TRUE;
	}
	else if( depth > MAX_SEARCH_DEPTH ) // Terminating condition, failure, depth exceeded
	{
		return FALSE;
	}
	else
	{
		int j;
		short closed[4] = {0,0,0,0};

		for( j=0; j<4; j++ )
		{
			float dist, best = 9999999;
			int dir=-1, i, nc=0;

			// All closed
			if( nc > 3 ) break;

			// Check tile pointers, and find best direction match to search first
			for( i=0; i<4; i++ )
				if( tile->tilePtrs[i] == previous || !tile->tilePtrs[i] ||
					(!fly && (tile->tilePtrs[i]->state == TILESTATE_JOIN || tile->tilePtrs[i]->state == TILESTATE_DEADLY || tile->tilePtrs[i]->state == TILESTATE_BARRED)) )
				{
					closed[i] = 1;
					nc++;
				}
				else if( !closed[i] )
				{
					dist = DistanceBetweenPointsSquared( &goal->centre, &tile->tilePtrs[i]->centre );
					if( dist < best )
					{
						dir = i;
						best = dist;
					}
				}

			// May as well stop now if no directions
			if( dir == -1 ) break;

			// Search down this branch
			if( BestFirst(path, depth+1, tile, tile->tilePtrs[dir], goal, fly) )
			{
				AIPATHNODE *tail, *node = (AIPATHNODE *)JallocAlloc( sizeof(AIPATHNODE), YES, "AIPath" );

				// Find last entry in path
				for( tail = path; tail->next; tail = tail->next );

				node->tile = tile;
				SubVector( &node->to, &tail->tile->centre, &node->tile->centre );
				tail->next = node;
				node->prev = tail;

				return TRUE;
			}
			else
			{
				closed[dir] = 1;	// Mark failed branch as closed
				nc++;
			}
		}

		return FALSE;	// Dead end, return failure
	}
}
