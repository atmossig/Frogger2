/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: netrace.cpp
	Programmer	: David Swift
	Date		:

	Based on the race-mode part of Jim's multiplayer code with 'optimisations'
	(i.e. cludges that may or may not work) so only player one is updated - the
	other players are dutifully faked by the netgame code - ds

----------------------------------------------------------------------------------------------- */

#include "netrace.h"
#include "netgame.h"
#include "frogger.h"
#include "frogmove.h"
#include "cam.h"
#include "controll.h"
#include "game.h"
#include "layout.h"
#include "lang.h"
#include "pcaudio.h"
#include "islutil.h"

long started = 0;

int countdown = 4;

int NetRaceRun()
{
	static char txt[3];
	//RunGameLoop();

	UpdateCameraPosition();
	GameProcessController(0);                                      

	if (actFrameCount < gameStartTime)
	{
		if (gameStartTime)
		{
			FVECTOR fwd;
			SetVectorFF( &fwd, &currTile[0]->dirVector[(frogFacing[0]+camFacing[0])&3] );
			
			int count = (gameStartTime-actFrameCount+59)/60;

			if (count<4)
			{
				if( currTile[0]->state == TILESTATE_FROGGER1AREA )
					player[0].canJump = 0;

				if( player[0].canJump )
					HopFrogToTile( FindJoinedTileByDirection(currTile[0],&fwd), 0);
			}

			if (count<countdown)
			{
				itoa(count, txt, 10);
				countdown = count;
				
				netMessage->a = (char)255;
				netMessage->draw = 1;
				netMessage->text = txt;
				netMessage->scale = 8192;
				PlaySample( FindSample(utilStr2CRC("racehorn")), NULL, 0, SAMPLE_VOLUME, -1 );
			}
		}
	}
	else
	{
		if (lastActFrameCount < gameStartTime)
		{
			netMessage->text = GAMESTRING(STR_GO);
			PlaySample( FindSample(utilStr2CRC("racehorngo")), NULL, 0, SAMPLE_VOLUME, -1 );

			player[0].canJump = 1;
		}
	}

	if (netMessage->a > (gameSpeed>>10))
		netMessage->a -= (gameSpeed>>10);
	else
	{
		netMessage->a = 0;
		netMessage->draw = 0;
	}
		
	UpdateFroggerPos(0);

	return 0;
}

	/*	--------------------------------------------------------------------------------
	Function		: UpdateRace
	Purpose			: Do game mechanics for mulitplayer race mode
	Parameters		: 
	Returns			: 
	Info			:

int UpdateNetRace( )
{
	int i, j;//, score;

	// Wait for all players to be on the start/finish line
	// this includes a wait-for-synchronisation thang
	if( !started )
	{
		// Hop player 1 only to the front line - after we're sychronised

		FVECTOR fwd;
		SetVectorFF( &fwd, &currTile[i]->dirVector[(frogFacing[0]+camFacing[i])&3] );
		
		// Hop forward to starting line 
		if( player[0].canJump )
			HopFrogToTile( FindJoinedTileByDirection(currTile[0],&fwd), i );

		if( currTile[0]->state == TILESTATE_FROGGER1AREA )
			player[0].canJump = 0;
	}

	// When all players are ready, start a countdown
	if( multiTimer.time)
	{
		int oldTime = multiTimer.time;
		GTUpdate( &multiTimer, -1 );
		if(multiTimer.time==1)
		{
			if( started != 2 )
			{
				PlaySample( FindSample(utilStr2CRC("racehorngo")), NULL, 0, SAMPLE_VOLUME, -1 );
				started = 2;
				for( i=0; i<NUM_FROGS; i++ )
					player[i].canJump = 1;
			}
		}
		else if( !multiTimer.time )
			PlayVoice( Random(NUM_FROGS), "frogletsgo" );
		else if( multiTimer.time != oldTime && multiTimer.time < 5 )
			PlaySample( FindSample(utilStr2CRC("racehorn")), NULL, 0, SAMPLE_VOLUME, -1 );
	}
	else if( endTimer.time ) // If finished the race then wait before replaying
	{
		GTUpdate( &endTimer, -1 );

		if( !endTimer.time )
			StartMultiWinGame( );

		return;
	}

	// Mini-loop for respawning before race start
	if( started != 2 )
	{
		for( i=0; i<NUM_FROGS; i++ )
		{
			// If waiting to respawn, update timer and check for respawn timeout
			if( player[i].dead.time )
			{
				GTUpdate( &player[i].dead, -1 );

				if( !player[i].dead.time )
				{
					RaceRespawn(i);
					frog[i]->draw = 1;
					GTInit( &player[i].safe, 3 );
				}
			}
		}
		return;
	}

	// Proper game loop thing
	for( i=0; i<NUM_FROGS; i++ )
	{
		if(mpl[i].penalText->draw)
		{
			SlideTextOverlayToPos(mpl[i].penalText,backTextX[i],backTextY[i],mpl[i].scrX,mpl[i].scrY,40);	
			if((mpl[i].penalText->xPos == backTextX[i]) && (mpl[i].penalText->yPos == backTextY[i]))
				mpl[i].penalText->draw = 0;
		}


		if( !mpl[i].ready )
		{
			// Increase timer if not finished
			mpl[i].timer += actFrameCount - lastActFrameCount;

			// If waiting to respawn, update timer and check for respawn timeout
			if( player[i].dead.time )
			{
				GTUpdate( &player[i].dead, -1 );

				if( !player[i].dead.time )
				{
					RaceRespawn(i);
					frog[i]->draw = 1;
					GTInit( &player[i].safe, 3 );
				}
				else continue;
			}

			// Initialise quickhop for everyone but the lead player
			if( i != playerFocus )
			{
				GTInit( &player[i].quickhop, 3 );
			}

			// Kill frogs that have fallen off screen
			if( (frameCount > 50) && !(IsPointVisible(&frog[i]->actor->position)) )
			{
				KillMPFrog(i);
			}
			else if((currTile[i]->state >= TILESTATE_FROGGER1AREA && currTile[i]->state <= TILESTATE_FROGGER4AREA) || (currTile[i]->state == TILESTATE_FINISHLINE))
			{
				// If last tile state was type 4 and this is type 1 then we've got around another lap
				// sbond changed but seems to work!!!!!!!!!!!!!!
//				if( currTile[i]->state == TILESTATE_FROGGER1AREA && mpl[i].lasttile != TILESTATE_FROGGER1AREA )
				if( currTile[i]->state == TILESTATE_FINISHLINE && mpl[i].lasttile != TILESTATE_FINISHLINE )
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
		{
			gameWinner = MULTI_ROUND_DRAW;
		}
		else
		{
			PlayVoice( winner, "frogokay" );
			gameWinner = winner;
			mpl[winner].wins++;
		}

		GTInit( &endTimer, 2 );
		controlCamera = 1;
	}
}
*/