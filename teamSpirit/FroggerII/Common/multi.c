#include <ultra64.h>
#include "incs.h"

#define BABY_UID	255

int multiplayerMode;
long started = 0;
unsigned long numMultiItems=0;
char matchWinner = -1;


TIMER powerupTimer;

void UpdateRace( );
void UpdateBattle( );

void CalcBattleCamera( VECTOR *target );
void CalcRaceCamera( VECTOR *target );

void BattleProcessController( int pl );
int AddBattleTrailNode( int i );
void FaceFrogInwards(int i);

MPINFO mpl[4];

TEXTOVERLAY *raceTimeOver[4];
char raceTimeText[4][20] = 
{
	"0000 0000 0000",
	"0000 0000 0000",
	"0000 0000 0000",
	"0000 0000 0000",
};


/*	--------------------------------------------------------------------------------
	Function		: UpdateRace
	Purpose			: Do game mechanics for mulitplayer race mode
	Parameters		: 
	Returns			: 
	Info			:
*/
void UpdateRace( )
{
	static TIMER endTimer, multiTimer;
	int i, j, score;

	// Wait for all players to be on the start/finish line
	if( !started )
	{
//		timeTextOver->text[0] = '\0';
//		scoreTextOver->text[0] = '\0';

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
	}

	// When all players are ready, start a countdown
	if( multiTimer.time )
	{
//		sprintf( timeTextOver->text, "%d", multiTimer.time );
		GTUpdate( &multiTimer, -1 );

		if( !multiTimer.time )
		{
//			sprintf( timeTextOver->text, "Go" );
			started = 2;

			for( i=0; i<NUM_FROGS; i++ )
				player[i].canJump = 1;
		}
	}
	else if( endTimer.time ) // If finished the race then wait before replaying
	{
		GTUpdate( &endTimer, -1 );

		if( !endTimer.time )
		{
			frameCount = 2; // Not 0, because 0 and 1 are used to trigger initialisation stuff elsewhere
			ResetMultiplayer( );
		}
		return;
	}

	if( started )
	{
//		sprintf( scoreTextOver->text, "%i %i %i %i", mpl[0].wins, mpl[1].wins, mpl[2].wins, mpl[3].wins );
	}

	if( started != 2 )
		return;

	for( i=0; i<NUM_FROGS; i++ )
	{
		raceTimeOver[i]->r = (raceTimeOver[i]->r < 250)?(raceTimeOver[i]->r + 3):255;
		raceTimeOver[i]->g = (raceTimeOver[i]->g < 250)?(raceTimeOver[i]->g + 3):255;
		raceTimeOver[i]->b = (raceTimeOver[i]->b < 250)?(raceTimeOver[i]->b + 3):255;

		if( !mpl[i].ready )
		{
			unsigned long total;

			// Increase timer if not finished
			mpl[i].timer += actFrameCount - lastActFrameCount;

			// Format and print players lap time
			total = mpl[i].timer + mpl[i].penalty;
			sprintf( raceTimeOver[i]->text, "%i %i %i", (total/3600), ((total%3600)/60), (total%60) );

			// Kill frogs that have fallen off screen
			if( (frameCount > 50) && !(IsPointVisible(&frog[i]->actor->pos)) )
			{
				KillMPFrog(i);
			}
			else if( currTile[i]->state >= TILESTATE_FROGGER1AREA && currTile[i]->state <= TILESTATE_FROGGER4AREA )
			{
				// If last tile state was type 4 and this is type 1 then we've got around another lap
				if( currTile[i]->state == TILESTATE_FROGGER1AREA && mpl[i].lasttile == TILESTATE_FROGGER4AREA )
				{
					mpl[i].lap++;
					mpl[i].lasttile = currTile[i]->state;
					mpl[i].check = 0;

					// Start of a new lap - if more then the defined number of maps for the race then this player is the winner
					if( mpl[i].lap >= 1)//MULTI_RACE_NUMLAPS )
					{
						mpl[i].ready = 1;
						player[i].canJump = 0;
					}
				}
				// Else if we've just got to another checkpoint (unlimited repetitions of 2,3,4. Tilestate 1 is used to signal lap changes)
				else if( currTile[i]->state == mpl[i].lasttile+1 || (mpl[i].lasttile == TILESTATE_FROGGER4AREA && currTile[i]->state == TILESTATE_FROGGER2AREA) )
				{
//					timeTextOver->text[0] = '\0';
					mpl[i].check++;
					mpl[i].lasttile = currTile[i]->state;
				}
			}
		}
	}

	// Check players for finish
	for( i=0; i<NUM_FROGS; i++ )
		if( !mpl[i].ready ) break;

	// If all players finished, check win conditions
	if( i==NUM_FROGS )
	{
		unsigned long best, time, winner, draw;
		// Find best time
		for( j=0,best=999999999; j<NUM_FROGS; j++ )
		{
			time = mpl[j].timer + mpl[j].penalty;
			if( time < best )
			{
				best = time;
				winner = j;
			}
		}
		// If more then one on best time, draw
		for( j=0,draw=0; j<NUM_FROGS; j++ )
		{
			time = mpl[j].timer + mpl[j].penalty;
			if( j!=winner && time == best )
				draw=1;
		}

		if( draw )
			draw = draw;
//			sprintf( timeTextOver->text, "Draw" );
		else
		{
			matchWinner = winner;
			ScaleVector( &camDist, 0.25 );
			mpl[winner].wins++;
//			sprintf( timeTextOver->text, "P%i won", winner );
		}

		GTInit( &endTimer, 5 );
	}
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateBattle
	Purpose			: Do game mechanics for lightcycles mode
	Parameters		: 
	Returns			: 
	Info			:
*/
void UpdateBattle( )
{
	static TIMER endTimer;
	int i, j, res, dead;

	if( !started )
	{
		int count=0;
//		sprintf( timeTextOver->text, "Press A" );

		for( i=0; i<NUM_FROGS; i++ )
		{
			if( (controllerdata[i].button & CONT_A) && !(controllerdata[i].lastbutton & CONT_A) ) mpl[i].ready = 1;
			if( mpl[i].ready ) count++;
			player[i].idleEnable = 0;
			// Face all frogs towards the centre of the map
			FaceFrogInwards(i);
		}

		if( count == NUM_FROGS )
		{
			GTInit( &endTimer, 0 );
			GTInit( &powerupTimer, Random(4)+3 );
			started = 1;
//			timeTextOver->text[0] = '\0';
		}
	}

	if( endTimer.time )
	{
		GTUpdate( &endTimer, -1 );

		if( !endTimer.time )
		{
			frameCount = 2;
			ResetMultiplayer( );
		}
		return;
	}

//	sprintf( scoreTextOver->text, "%i %i %i %i", mpl[0].wins, mpl[1].wins, mpl[2].wins, mpl[3].wins );
	if( started )
	{
//		sprintf( timeTextOver->text, "%i %i %i %i", mpl[0].score, mpl[1].score, mpl[2].score, mpl[3].score );
		GTUpdate( &powerupTimer, -1 );
		if( !powerupTimer.time )
		{
			if( numMultiItems < MULTI_BATTLE_MAXITEMS )
			{
				VECTOR pos;
				GARIB *g;
				int r;
				
				do{ r=Random(numTiles); } while( firstTile[r].state >= TILESTATE_FROGGER1AREA && firstTile[r].state <= TILESTATE_FROGGER4AREA );

				SetVector( &pos, &firstTile[r].normal );
				ScaleVector( &pos, 250 );
				AddToVector( &pos, &firstTile[r].centre );

				// TODO: Randomise garib type with preference for trail extension
				g = CreateNewGarib( pos, SPAWN_GARIB );
				DropGaribToTile( g, &firstTile[r], 10 );
				numMultiItems++;
			}

			GTInit( &powerupTimer, (Random(4)+3) );
		}
	}

	for( i=0; i<NUM_FROGS; i++ )
	{
		// Change desired frog facing
		BattleProcessController(i);

		if( started )
		{
			AIPATHNODE *node=NULL, *temp;
			// If the frog can move then continue in current direction
			if( player[i].canJump )
			{
				int tf=1;

				res = MoveToRequestedDestination( ((frogFacing[i] - camFacing[i]) & 3), i );
				player[i].canJump = 0;

				// Add a new node to the trail
				if( res )
				{
					tf = AddBattleTrailNode( i );
					AnimateFrogHop( frogFacing[i], i );
				}

				if( !tf || !res ) player[i].frogState |= FROGSTATUS_ISDEAD;
			}

			// Update trail. For 0->length, increase alpha to limit. 
			for( j=0, node=mpl[i].path; node && j<mpl[i].trail; j++,node = node->next )
			{
				int fo = 20*gameSpeed;
				if( node->fx && node->fx->a < 255-fo )
					node->fx->a += fo;
			}
			// For length->endoflist, fade out and make not deadly
			while( node )
			{
				int fo = 10*gameSpeed;

				temp = node->next;
				node->tile->state = TILESTATE_NORMAL;

				if( node->fx->a <= fo )
					SubAIPathNode( node );
				else
					node->fx->a -= fo;

				node = temp;
			}

			for( j=0; j<NUM_FROGS; j++ )
				if( i!=j && (currTile[j] == currTile[i] || (destTile[j] && destTile[i] && destTile[j] == destTile[i]) ) )
				{
					player[i].frogState |= FROGSTATUS_ISDEAD;
					player[j].frogState |= FROGSTATUS_ISDEAD;
				}
		}

		if( player[i].frogState & FROGSTATUS_ISDEAD )
		{
			player[i].deathBy = DEATHBY_NORMAL;
			AnimateActor(frog[i]->actor, FROG_ANIM_FWDSOMERSAULT, NO, NO, 0.5F, 0, 0);
			dead++;
		}
	}

	// The last player alive wins!
	if( NUM_FROGS > 1 && dead == NUM_FROGS-1 )
	{
		for( i=0; i<NUM_FROGS; i++ )
			if( !(player[i].frogState & FROGSTATUS_ISDEAD) )
				break;

		if( i!=NUM_FROGS )
		{
//			sprintf( timeTextOver->text, "P%i won", i );
			mpl[i].wins++;
			GTInit( &endTimer, 5 );
		}
	}
	else if( dead == NUM_FROGS ) // Or if everyone is dead, declare a draw
	{
		GTInit( &endTimer, 5 );
		fixedPos = fixedDir = 1;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: RunMultiplayer
	Purpose			: Call update function for each game type
	Parameters		: player
	Returns			: void
	Info			:
*/
void RunMultiplayer( )
{
	switch( multiplayerMode )
	{
	case MULTIMODE_RACE:
		UpdateRace( );
		break;
	case MULTIMODE_BATTLE:
		UpdateBattle( );
		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: RaceRespawn
	Purpose			: Teleport frog to a non-dead player
	Parameters		: 
	Returns			: 
	Info			: Oh dear. Respawn near the lead frog if available, or search for another if not
*/
void RaceRespawn( int pl )
{
	int j, respawn=0;
	GAMETILE *tile;
	VECTOR diff;
	SPECFX *fx;

	if( playerFocus != pl && player[playerFocus].healthPoints && !(player[playerFocus].frogState & FROGSTATUS_ISDEAD) && IsPointVisible(&frog[playerFocus]->actor->pos) )
		respawn = playerFocus;
	else
		for( j=0; j<NUM_FROGS; j++ )
			if( j != pl && j != playerFocus && player[j].healthPoints && !(player[j].frogState & FROGSTATUS_ISDEAD) && IsPointVisible(&frog[j]->actor->pos) )
			{
				respawn = j;
				break;
			}

	SubVector( &diff, &frog[pl]->actor->pos, &frog[respawn]->actor->pos );
	MakeUnit(&diff);

	// Find a tile to teleport to - first try a tile next to the target, then on the nearest "safe" tile, then on the target frog if that fails
	if( !(tile = FindJoinedTileByDirection( currTile[respawn], &diff )) )
	{
		GAMETILE *t, *target = NULL;
		long dist, best = 99999999;

		for( t = firstTile; t != NULL; t = t->next )
			if( t->state == TILESTATE_SAFE )
			{
				dist = DistanceBetweenPointsSquared( &frog[pl]->actor->pos, &t->centre );
				if( dist < best )
				{
					target = t;
					best = dist;
				}
			}

		if( target )
			tile = target;
		else
		{
			if( currPlatform[respawn] ) currPlatform[pl] = currPlatform[respawn];
			tile = currTile[respawn];
		}
	}

	if( (fx = CreateAndAddSpecialEffect( FXTYPE_POLYRING, &frog[pl]->actor->pos, &currTile[pl]->normal, 10, 2, 0.05, 0.6 )) )
	{
		fx->spin = 5;
		SetFXColour( fx, mpl[pl].r, mpl[pl].g, mpl[pl].b );
	}

	TeleportActorToTile(frog[pl],tile,pl);

	// Update progress info
	mpl[pl].check = mpl[respawn].check;
	mpl[pl].lap = mpl[respawn].lap;
	mpl[pl].lasttile = mpl[respawn].lasttile;

	camFacing[pl] = camFacing[respawn];
	frogFacing[pl] = frogFacing[respawn];
	prevCamFacing[pl] = prevCamFacing[respawn];

	lastTile[pl] = tile;
	destTile[pl] = tile;
}


/*	--------------------------------------------------------------------------------
	Function		: Battle process controller
	Purpose			: Just change direction, and maybe use tongue
	Parameters		: player
	Returns			: void
	Info			:
*/
void BattleProcessController( int pl )
{
	u16 button[4],lastbutton[4];

	button[pl] = controllerdata[pl].button;
	lastbutton[pl] = controllerdata[pl].lastbutton;

	// Change frog facing on direction keys
	player[pl].frogState &= ~FROGSTATUS_ALLHOPFLAGS;

	if( (button[pl] & CONT_UP) && !(lastbutton[pl] & CONT_UP))
		frogFacing[pl] = (camFacing[pl] + MOVE_UP) & 3;
	else if( (button[pl] & CONT_DOWN) && !(lastbutton[pl] & CONT_DOWN))
		frogFacing[pl] = (camFacing[pl] + MOVE_DOWN) & 3;
	else if( (button[pl] & CONT_LEFT) && !(lastbutton[pl] & CONT_LEFT))
		frogFacing[pl] = (camFacing[pl] + MOVE_LEFT) & 3;
	else if( (button[pl] & CONT_RIGHT) && !(lastbutton[pl] & CONT_RIGHT))
		frogFacing[pl] = (camFacing[pl] + MOVE_RIGHT) & 3;

	nextFrogFacing[pl] = frogFacing[pl];

	if((button[pl] & CONT_START) && !(lastbutton[pl] & CONT_START))
	{
		gameState.mode = PAUSE_MODE;
		pauseMode = 1;

		EnableTextOverlay ( continueText );
		EnableTextOverlay ( quitText );

//		timeTextOver->oa = timeTextOver->a;
//		timeTextOver->a = 0;
    }
}

/*	--------------------------------------------------------------------------------
	Function		: AddBattleTrailNode
	Purpose			: Add an aipathnode to the trail and put an effect on it
	Parameters		: player
	Returns			: void
	Info			:
*/
int AddBattleTrailNode( int i )
{
	AIPATHNODE *node = (AIPATHNODE *)JallocAlloc(sizeof(AIPATHNODE),YES,"battletrail");
	
	node->tile = destTile[i];

	if( node->tile->state >= TILESTATE_FROGGER1AREA && node->tile->state <= TILESTATE_FROGGER4AREA )
		return 0;

	node->tile->state = TILESTATE_FROGGER1AREA + i;
	// Create the effect that marks the trails
	if( (node->fx = CreateAndAddSpecialEffect( FXTYPE_DECAL, &currTile[i]->centre, &currTile[i]->normal, 25, 0, 0, 9999999 )) )
	{
		node->fx->fade = 0;
		node->fx->tex = txtrSolidRing;
		node->fx->a = 1;
		SetFXColour( node->fx, mpl[i].r, mpl[i].g, mpl[i].b );
	}

	// Add to list
	node->next = mpl[i].path;
	if( mpl[i].path ) mpl[i].path->prev = node;
	mpl[i].path = node;

	return 1;
}


/*	--------------------------------------------------------------------------------
	Function		: FaceFrogInwards
	Purpose			: make a player face the middle of the battle arena
	Parameters		: player
	Returns			: void
	Info			:
*/
void FaceFrogInwards(int pl)
{
	VECTOR dest, dir;
	float dp, best=-1;
	short facing=0, i;

	// Find direction to centre
	// NOTE: These can be done in one go, but I can't be bothered right now
	SetVector( &dir, &frog[pl]->actor->pos );
	MakeUnit( &dir );
	ScaleVector( &dir, -1 );

	for( i=0; i<4; i++ )
	{
		dp = DotProduct( &dir, &currTile[pl]->dirVector[i] );
		if( dp > best )
		{
			facing = i;
			best = dp;
		}
	}

	frogFacing[pl] = facing;
	Orientate( &frog[pl]->actor->qRot, &currTile[pl]->dirVector[frogFacing[pl]], &currTile[pl]->normal );
}


/*	--------------------------------------------------------------------------------
	Function		: KillMPFrog
	Purpose			: Subtract healthpoint or kill, or respawn if appropriate
	Parameters		: player
	Returns			: void
	Info			:
*/
void KillMPFrog(int num)
{
	if( multiplayerMode == MULTIMODE_RACE )
	{
		mpl[num].penalty += MULTI_RACE_TIMEPENALTY;
		raceTimeOver[num]->r = mpl[num].r;
		raceTimeOver[num]->g = mpl[num].g;
		raceTimeOver[num]->b = mpl[num].b;
		RaceRespawn(num);
	}
	else
	{
		int i=0;

		if( currTile[i]->state == TILESTATE_DEADLY )
			player[num].healthPoints = 0;

		GTInit( &player[num].stun, 2 );
		GTInit( &player[num].safe, 5 );
		
		player[num].healthPoints--;
		i=num*3+player[num].healthPoints;
	//	if( sprHeart[i] ) sprHeart[i]->draw = 0;

		if( !player[num].healthPoints )
		{
			player[num].deathBy = DEATHBY_NORMAL;
			player[num].frogState |= FROGSTATUS_ISDEAD;

			frog[num]->draw = 0;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: ReinitialiseMultiplayer
	Purpose			: Set mp data back to very initial values
	Parameters		: 
	Returns			: 
	Info			:
		MPINFO mpl[4] = 
		{
			{ 0, 0,	0, MULTI_BATTLE_TRAILLENGTH,	30,  230, 30,	0, TILESTATE_FROGGER1AREA, NULL },
			{ 0, 0,	0, MULTI_BATTLE_TRAILLENGTH,	230, 30,  30,	0, TILESTATE_FROGGER1AREA, NULL },
			{ 0, 0,	0, MULTI_BATTLE_TRAILLENGTH,	180, 180, 230,	0, TILESTATE_FROGGER1AREA, NULL },
			{ 0, 0,	0, MULTI_BATTLE_TRAILLENGTH,	30,  30,  230,	0, TILESTATE_FROGGER1AREA, NULL },
		};
*/
void ReinitialiseMultiplayer( )
{
	int i;

	memset( mpl, 0, sizeof( MPINFO )*4 );

	mpl[0].r = 30; mpl[0].g = 230; mpl[0].b = 30;
	mpl[1].r = 230; mpl[1].g = 30; mpl[1].b = 30;
	mpl[2].r = 180; mpl[2].g = 180; mpl[2].b = 230;
	mpl[3].r = 30; mpl[3].g = 30; mpl[3].b = 230;

	for( i=0; i<NUM_FROGS; i++ )
	{
		mpl[i].trail = MULTI_BATTLE_TRAILLENGTH;
		mpl[i].lasttile = TILESTATE_FROGGER1AREA;
	}

	switch( multiplayerMode )
	{
	case MULTIMODE_BATTLE:
		break;
	case MULTIMODE_RACE:
		raceTimeOver[0] = CreateAndAddTextOverlay( 20, 40, raceTimeText[0], 0, 255, smallFont, TEXTOVERLAY_NORMAL, 0 );
		raceTimeOver[1] = CreateAndAddTextOverlay( 260, 40, raceTimeText[1], 0, 255, smallFont, TEXTOVERLAY_NORMAL, 0 );
		raceTimeOver[2] = CreateAndAddTextOverlay( 20, 220, raceTimeText[2], 0, 255, smallFont, TEXTOVERLAY_NORMAL, 0 );
		raceTimeOver[3] = CreateAndAddTextOverlay( 260, 220, raceTimeText[3], 0, 255, smallFont, TEXTOVERLAY_NORMAL, 0 );

		for( i=0; i<NUM_FROGS; i++ ) sprintf(raceTimeOver[i]->text, "00 00 00");
		for( ; i<4; i++ ) DisableTextOverlay( raceTimeOver[i] );

		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: ResetMultiplayer
	Purpose			: Set multiplayer info to initial values EXCEPT the stuff that needs to be preserved between levels
	Parameters		: player
	Returns			: void
	Info			:
*/
void ResetMultiplayer( )
{
	int i;

	started = 0;
	playerFocus = 0;
	matchWinner = -1;
	GTInit( &powerupTimer, 0 );

	for( i=0; i<NUM_FROGS; i++ )
	{
		mpl[i].ready = 0;
		player[i].idleEnable = 1;
		player[i].canJump = 1;
	}

	switch( multiplayerMode )
	{
	case MULTIMODE_BATTLE:
	{
		AIPATHNODE *node, *temp;
		for( i=0; i<NUM_FROGS; i++ )
		{
			mpl[i].trail = MULTI_BATTLE_TRAILLENGTH;
			mpl[i].ready = 0;
			mpl[i].score = 0;
			node = mpl[i].path;
			while(node)
			{
				temp = node->next;
				node->tile->state = TILESTATE_NORMAL;
				SubAIPathNode(node);
				node = temp;
			}
			mpl[i].path = NULL;
			SetFroggerStartPos( gTStart[i], i );
		}

		FreeGaribLinkedList();
		InitGaribLinkedList();
		InitSpriteSortArray(MAX_ARRAY_SPRITES);
		numMultiItems = 0;
		break;
	}
	case MULTIMODE_RACE:
		for( i=0; i<NUM_FROGS; i++ )
		{
			mpl[i].check = 0;
			mpl[i].lap = 0;
			mpl[i].lasttile = TILESTATE_FROGGER1AREA;
			mpl[i].timer = 0;
			mpl[i].penalty = 0;

			SetFroggerStartPos( gTStart[i], i );
		}
		break;
	}

	ScaleVector( &camDist, 4 );
	InitCamera();
}


/*	--------------------------------------------------------------------------------
	Function		: CalcMPCamera
	Purpose			: Do different cameras for different game types
	Parameters		: target vector
	Returns			: 
	Info			:
*/
void CalcMPCamera( VECTOR *target )
{
	switch( multiplayerMode )
	{
	case MULTIMODE_RACE:
		CalcRaceCamera( target );
		break;

	case MULTIMODE_BATTLE:
		CalcBattleCamera( target );
		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: CalcRaceCamera
	Purpose			: Focus on the average point of all the lead players
	Parameters		: target vector
	Returns			: 
	Info			:
*/
void CalcRaceCamera( VECTOR *target )
{
	int i, bestLap=0, bestCheck=0, num;
	float t;

	ZeroVector( target );

	if( matchWinner != -1 )
	{
		playerFocus = matchWinner;
		SetVector( target, &frog[matchWinner]->actor->pos );
		return;
	}

	for( i=0; i<NUM_FROGS; i++ )
		if( mpl[i].lap >= bestLap && mpl[i].check >= bestCheck && player[i].healthPoints && !(player[i].frogState & FROGSTATUS_ISDEAD) )
		{
			bestLap = mpl[i].lap;
			bestCheck = mpl[i].check;
			playerFocus = i;
		}

	for( i=0,num=0; i<NUM_FROGS; i++ )
	{
		if( mpl[i].lap == bestLap && mpl[i].check == bestCheck && player[i].healthPoints && !(player[i].frogState & FROGSTATUS_ISDEAD) )
		{
			num++;
			t = player[i].jumpTime;
			if( t > 0 )	// jumping; calculate linear position
			{
				VECTOR v;
				SetVector(&v, &player[i].jumpFwdVector);
				ScaleVector(&v, t);
				AddToVector(target, &player[i].jumpOrigin);
				AddToVector(target, &v);
			}										
			else
			{
				AddToVector( target, &frog[i]->actor->pos );
			}
		}
	}

	ScaleVector( target, 1.0f/(float)(max(num,1)) );
}


/*	--------------------------------------------------------------------------------
	Function		: CalcBattleCamera
	Purpose			: Focus on average position of frogs and zoom depending on separation
	Parameters		: target vector
	Returns			: 
	Info			: Stupid really, since the battle area has fixed camera cases. Just the old ctf camera
*/
void CalcBattleCamera( VECTOR *target )
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




/*************** THE MULTIPLAYER MODE GRAVEYARD *****************/
/*
void UpdateDM( )
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
	
*/

/*	--------------------------------------------------------------------------------
	Function		: PickupBabyFrogMulti
	Purpose			: Player has collected baby frog in a multiplayer game
	Parameters		: ACTOR2
	Returns			: void
	Info			:
*/
/*
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
*/
