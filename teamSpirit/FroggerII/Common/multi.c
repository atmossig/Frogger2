#include <ultra64.h>
#include "incs.h"

#define BABY_UID	255

int multiplayerMode;
long started = 0;

void UpdateCTF( );
void UpdateRace( );
void UpdateBattle( );

void CalcBattleCamera( VECTOR *target );
void CalcCTFCamera( VECTOR *target );
void CalcRaceCamera( VECTOR *target );

/*	--------------------------------------------------------------------------------
	Function		: UpdateCTF
	Purpose			: Do game mechanics for Capture the Frog multiplayer mode
	Parameters		: 
	Returns			: 
	Info			:
*/
void UpdateCTF( )
{
	unsigned long i;
	static TIMER endTimer, multiTimer;

	if( !started )
	{
		GTInit( &multiTimer, 90 );
		GTInit( &endTimer, 0 );
		started = 1;
	}

	if( endTimer.time )
	{
		GTUpdate( &endTimer, -1 );

		if( !endTimer.time )
		{
			StopDrawing("game over");
			FreeAllLists();

			InitLevel(player[0].worldNum,player[0].levelNum);
			gameState.mode = INGAME_MODE;

			started = frameCount = 0;
			fixedPos = fixedDir = 0;
			StartDrawing("game over");
		}
		return;
	}
	else // Is anyone still alive?
	{
		for( i=0; i<NUM_FROGS; i++ )
			if( player[i].healthPoints && !(player[i].frogState & FROGSTATUS_ISDEAD) ) break;

		if( i==NUM_FROGS )
		{
			GTInit( &endTimer, 10 );
			fixedPos = fixedDir = 1;
		}
	}

	GTUpdate( &multiTimer, -1 );

	if( !multiTimer.time )	// Check win conditions
	{
		GAMETILE *t;
		ENEMY *nme;
		short babyCount[4] = {0,0,0,0}, winner=-1, best=0;

		// If on a froggers area tile, check for babies on the tile
		for (t = firstTile; t != NULL; t = t->next)
			if( (t->state >= TILESTATE_FROGGER1AREA) && (t->state <= TILESTATE_FROGGER4AREA) )
				for( nme = enemyList.head.next; nme != &enemyList.head; nme = nme->next )
					if( (nme->flags & ENEMY_NEW_BABYFROG) && (t == nme->inTile) )
						babyCount[t->state-TILESTATE_FROGGER1AREA]++;

		for( i=0; i<NUM_FROGS; i++ )
			if( babyCount[i] > best )
			{
				best = babyCount[i];
				winner = i;
			}

		if( winner == -1 ) sprintf( timeTextOver->text, "No winner" );
		else sprintf( timeTextOver->text, "P%i won", winner );

		GTInit( &endTimer, 10 );
		return;
	}

	sprintf(timeTextOver->text,"%d",multiTimer.time);
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateRace
	Purpose			: Do game mechanics for mulitplayer race mode
	Parameters		: 
	Returns			: 
	Info			:
*/
RACEINFO racer[4];

void UpdateRace( )
{
	static TIMER endTimer, multiTimer;
	int i, j, dead=0;

	// Wait for all players to be on the start/finish line
	if( !started )
	{
		timeTextOver->text[0] = '\0';

		for( i=0,j=0; i<NUM_FROGS; i++ )
			if( currTile[i]->state == TILESTATE_FROGGER1AREA )
			{
				j++;
				player[i].canJump = 0;
			}

		if( j==NUM_FROGS )
		{
			GTInit( &endTimer, 0 );
			GTInit( &multiTimer, 3 );
			started = 1;
		}

		if( !started ) return;
	}

	// When all players are ready, start a countdown
	if( multiTimer.time )
	{
		sprintf( timeTextOver->text, "%d", multiTimer.time );
		GTUpdate( &multiTimer, -1 );

		if( !multiTimer.time )
		{
			sprintf( timeTextOver->text, "Go" );

			for( i=0; i<NUM_FROGS; i++ )
			{
				player[i].canJump = 1;
				racer[i].check = 0;
				racer[i].lap = 0;
				racer[i].lasttile = TILESTATE_FROGGER1AREA;
			}
		}
		else return;
	}
	else if( endTimer.time ) // If finished the race then wait before replaying
	{
		GTUpdate( &endTimer, -1 );

		if( !endTimer.time )
		{
			timeTextOver->text[0] = '\0';
			StopDrawing("game over");
			FreeAllLists();

			InitLevel(player[0].worldNum,player[0].levelNum);
			gameState.mode = INGAME_MODE;

			started = frameCount = 0;
			fixedPos = fixedDir = 0;
			StartDrawing("game over");
		}
		return;
	}
	
	for( i=0; i<NUM_FROGS; i++ )
	{
		// Check for frog off screen - death
		if( !(player[i].frogState & FROGSTATUS_ISDEAD) && (frameCount > 50) && !(IsPointVisible(&frog[i]->actor->pos)) )
		{
			KillMPFrog(i);

			for( j=0; j<NUM_FROGS; j++ )
				if( !(player[j].frogState & FROGSTATUS_ISDEAD) && IsPointVisible(&frog[j]->actor->pos) )
				{
					TeleportActorToTile(frog[i],currTile[j],i);
					racer[i].check = racer[j].check;
					racer[i].lap = racer[j].lap;
					racer[i].lasttile = racer[j].lasttile;
					destTile[i] = currTile[j];
				}
		}
		else if( currTile[i]->state >= TILESTATE_FROGGER1AREA && currTile[i]->state <= TILESTATE_FROGGER4AREA )
		{
			// If last tile state was type 4 and this is type 1 then we've got around another lap
			if( currTile[i]->state == TILESTATE_FROGGER1AREA && racer[i].lasttile == TILESTATE_FROGGER4AREA )
			{
				racer[i].lap++;
				racer[i].lasttile = currTile[i]->state;
				racer[i].check = 0;

				// Start of a new lap - if more then the defined number of maps for the race then this player is the winner
				if( racer[i].lap > MULTI_RACE_NUMLAPS )
				{
					sprintf( timeTextOver->text, "P%i won", i );
					GTInit( &endTimer, 5 );
					return;
				}
			}
			// Else if we've just got to another checkpoint (unlimited repetitions of 2,3,4. Tilestate 1 is used to signal lap changes)
			else if( currTile[i]->state > racer[i].lasttile || (racer[i].lasttile == TILESTATE_FROGGER4AREA && currTile[i]->state == TILESTATE_FROGGER2AREA) )
			{
				timeTextOver->text[0] = '\0';
				racer[i].check++;
				racer[i].lasttile = currTile[i]->state;
			}
		}

		if( !(player[i].healthPoints) || (player[i].frogState & FROGSTATUS_ISDEAD) ) dead++;
	}

	// Is anyone still alive?
	if( dead == NUM_FROGS )
	{
		GTInit( &endTimer, 5 );
		fixedPos = fixedDir = 1;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateRace
	Purpose			: Do game mechanics for mulitplayer race mode
	Parameters		: 
	Returns			: 
	Info			:
*/
void UpdateBattle( )
{
	static TIMER endTimer;
	int i, j;

	if( !started )
	{
		timeTextOver->text[0] = '\0';
		GTInit( &endTimer, 0 );
		started = 1;
	}

	if( endTimer.time )
	{
		GTUpdate( &endTimer, -1 );

		if( !endTimer.time )
		{
			StopDrawing("game over");
			FreeAllLists();

			InitLevel(player[0].worldNum,player[0].levelNum);
			gameState.mode = INGAME_MODE;

			started = frameCount = 0;
			fixedPos = fixedDir = 0;
			StartDrawing("game over");
		}
		return;
	}
	else // Is anyone still alive?
	{
		char dead[4] = {1,1,1,1};

		for( i=0,j=0; i<NUM_FROGS; i++ )
			if( player[i].healthPoints && !(player[i].frogState & FROGSTATUS_ISDEAD) ) 
			{
				dead[i] = 0;
				j++;
			}

		if( !j )
		{
			GTInit( &endTimer, 10 );
			fixedPos = fixedDir = 1;
		}
		else if( j==1 )
		{
			for( i=0,j=0; i<4; i++ )
				if( !dead[i] ) j = i;

			sprintf( timeTextOver->text, "P%i won", j+1 );

			GTInit( &endTimer, 10 );
		}
	}
}


void RunMultiplayer( )
{
	switch( player[0].worldNum )
	{
	case WORLDID_GARDEN:
		multiplayerMode = MULTIMODE_RACE_NORMAL;
		UpdateRace( );
		break;
	case WORLDID_ANCIENT:
		multiplayerMode = MULTIMODE_RACE_KNOCKOUT;
		UpdateRace( );
		break;
	case WORLDID_SUBTERRANEAN:
	case WORLDID_HALLOWEEN:
		multiplayerMode = MULTIMODE_CTF;
		UpdateCTF( );
		break;
	default:
		multiplayerMode = MULTIMODE_BATTLE;
		UpdateBattle( );
		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: PickupBabyFrogMulti
	Purpose			: Player has collected baby frog in a multiplayer game
	Parameters		: ACTOR2
	Returns			: void
	Info			:
*/
void PickupBabyFrogMulti( ENEMY *baby, int pl )
{
	int i;
	ENEMY *nme;
	PATH *path = baby->path, *path2;

	for( i=0; i<numBabies; i++ ) if( babyList[i].baby == baby->nmeActor ) break;

	if( i==numBabies ) return;

	lastBabySaved = i;

	// For different multiplayer modes do different things:
		// CTF - transport baby back to frogger start point - assumes babies are all randommove nmes

	if( multiplayerMode == MULTIMODE_CTF )
	{
		// Find placeholder enemy that holds the baby respawn path for this frog
		for(nme = enemyList.head.next; nme != &enemyList.head; nme = nme->next )
			if( nme->uid-BABY_UID == pl ) break;

		if( nme == &enemyList.head ) return;

		path2 = nme->path;

		if( baby->flags & ENEMY_NEW_RANDOMMOVE )
		{
			CreateTeleportEffect( &baby->nmeActor->actor->pos, &baby->inTile->normal, babyList[i].fxColour[R], babyList[i].fxColour[G], babyList[i].fxColour[B] );
			// Baby teleports to froggers base
			path->nodes[0].worldTile = path2->nodes[0].worldTile;
			path->nodes[1].worldTile = path2->nodes[1].worldTile;
			path->nodes[2].worldTile = path2->nodes[2].worldTile;
			baby->isSnapping = 0;

			// Teleport frog back to base too
			CreateTeleportEffect( &frog[pl]->actor->pos, &currTile[pl]->normal, 255, 255, 255 );
			TeleportActorToTile( frog[pl], gTStart[pl], pl );
			destTile[pl] = gTStart[pl];
		}
	}
}

void KillMPFrog(int num)
{
	int i=0;
	GTInit( &player[num].stun, 2 );
	GTInit( &player[num].safe, 3 );
	
	if (player[num].healthPoints > 0)
	{
		i=num*3+(--player[num].healthPoints);
		if( sprHeart[i] ) sprHeart[i]->draw = 0;
	}
	else
	{
		player[num].deathBy = DEATHBY_NORMAL;
		player[num].frogState |= FROGSTATUS_ISDEAD;

		frog[num]->draw = 0;
	}
}

void ResetMultiplayer( )
{
	int i;

	started = 0;

	switch( multiplayerMode )
	{
	case MULTIMODE_CTF:
		break;

	case MULTIMODE_BATTLE:
		break;

	case MULTIMODE_RACE_NORMAL:
	case MULTIMODE_RACE_KNOCKOUT:
		for( i=0; i<NUM_FROGS; i++ )
		{
			racer[i].check = -1;
			racer[i].lap = 0;
			racer[i].lasttile = TILESTATE_FROGGER1AREA;
		}
		break;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: CalcMPCamera
	Purpose			: Do different cameras for different game types
	Parameters		: target vector
	Returns			: void
	Info			:
*/
void CalcMPCamera( VECTOR *target )
{
	switch( multiplayerMode )
	{
	case MULTIMODE_RACE_NORMAL:
	case MULTIMODE_RACE_KNOCKOUT:
		CalcRaceCamera( target );
		break;
	case MULTIMODE_CTF:
		CalcCTFCamera( target );
		break;
	case MULTIMODE_BATTLE:
		CalcBattleCamera( target );
		break;
	}
}

void CalcRaceCamera( VECTOR *target )
{
	int lead=0, i, bestLap=0, bestCheck=0;
	float t;

	for( i=0; i<NUM_FROGS; i++ )
		if( racer[i].lap >= bestLap && racer[i].check >= bestCheck && player[i].healthPoints && !(player[i].frogState & FROGSTATUS_ISDEAD) )
		{
			bestLap = racer[i].lap;
			bestCheck = racer[i].check;
			lead = i;
		}

	t = player[lead].jumpTime;
	if( t > 0 )	// jumping; calculate linear position
	{
		VECTOR v;
		SetVector(target, &player[lead].jumpOrigin);
		SetVector(&v, &player[lead].jumpFwdVector);
		ScaleVector(&v, t);
		AddToVector(target, &v);
	}										
	else
	{
		SetVector( target, &frog[lead]->actor->pos );
	}
}

void CalcBattleCamera( VECTOR *target )
{
	CalcCTFCamera( target );
}

void CalcCTFCamera( VECTOR *target )
{
	VECTOR v;
	int i,l;
	float sc, t;

	ZeroVector( target );

	for( i=0,l=0; i<NUM_FROGS; i++ )
	{
		if( player[i].healthPoints && !(player[i].frogState & FROGSTATUS_ISDEAD) )
		{
			t = player[i].jumpTime;
			
			if (t > 0)	// jumping; calculate linear position
			{
				AddToVector(target, &player[i].jumpOrigin);

				SetVector(&v, &player[i].jumpFwdVector);
				ScaleVector(&v, t);
				AddToVector(target, &v);
			}										
			else
				AddToVector(target, &frog[i]->actor->pos);

			l++;
		}

		// Zoom in/out to keep multiplayer frogs in view
		sc = FindMaxInterFrogDistance( );
		if( sc != -1 ) scaleV = (sc*0.00115) + 0.6;
	}
	
	if (l > 1)
		ScaleVector(target, 1.0f/l);
}
