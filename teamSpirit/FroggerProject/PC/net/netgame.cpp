/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: netgame.cpp
	Programmer	: David Swift
	Date		:

----------------------------------------------------------------------------------------------- */

// Net headers
#include "network.h"
#include "netgame.h"
#include "netrace.h"

// PC headers
#include "main.h"
#include "islpad.h"
#include "islutil.h"
#include "fadeout.h"

// Common headers
#include "frontend.h"
#include "game.h"
#include "frogger.h"
#include "frogmove.h"
#include "cam.h"
#include "enemies.h"
#include "specfx.h"
#include "overlays.h"
#include "lang.h"

#define BIT(x)	(1<<(x))

#define NETUPD_SUPERHOP		BIT(0)
#define NETUPD_DOUBLEJUMP	BIT(1)

#define UPDATE_PERIOD		5		// in 60ths of a second (actFrameCounts)
#define PING_PERIOD			200		// how often we should ping once we're synchronised

#define STARTGAME_COUNT		(4*60)	// how much time we allow for the countdown

unsigned long	nextUpdate = 0, nextPing = 0, gameStartTime = 0;
bool			hostSync, hostReady, gameReady;
NETGAME_LOOP	netgameLoopFunc = NULL;
TEXTOVERLAY		*netMessage;

typedef struct
{
	unsigned char	type;
	DWORD			buttons;		// controller data
	short			fromTile, toTile;
	DWORD			tickCount;		// When in game frames this event happened.
	DWORD			flags;

} MSG_UPDATEGAME,*LPMSG_UPDATEGAME;

struct	MSG_PING
{
	UBYTE appmsg_ping;
	unsigned long time;
};

struct	MSG_PINGREPLY
{
	UBYTE appmsg_pingreply;
	unsigned long reply, time0;
};

struct	MSG_STARTGAME
{
	UBYTE appmsg_start;
	unsigned long	gameStartTime;		// when the game is due to start, according to actFrameCount
};

#define GetTileNo(t) (((DWORD)tile-(DWORD)firstTile)/sizeof(GAMETILE))

int NetgameMessageDispatch(void *data, unsigned long size, NETPLAYER *player);
int SendUpdateMessage();
void SendPing();

/*	--------------------------------------------------------------------------------
	Function		: HandleUpdateMessage
	Purpose			: interpret a game update from the network
	Parameters		: 
	Returns			: 
	Info			: 
*/
void HandleUpdateMessage(LPMSG_UPDATEGAME lpMsg, NETPLAYER *pl)
{
	if (lpMsg->tickCount < pl->lastUpdateMsg)
		return;
	
	int i = GetPlayerNumFromID(pl->dpid);
	GAMETILE *tile = &firstTile[lpMsg->toTile];

//	padData.digital[i] = lpMsg->buttons;
//	padData[i].tickOn = lpMsg->tickCount;

//	dp("Recieved message for tile %lu\n",lpMsg->tileNum);

	// Calculate frog jump if it's not 'in' the current tile
	if (tile != currTile[i])
	{
		FVECTOR fv;

		if (currTile[i])
		{
			SubVectorFSS(&fv, &tile->centre, &currTile[i]->centre);
			OrientateFS(&frog[i]->actor->qRot, &fv, &currTile[i]->normal);
		}

		//SetVector(&frog[i]->actor->position, &tile->centre);
		currTile[i] = tile;
		frog[i]->draw = 1;

		if (lpMsg->flags & NETUPD_SUPERHOP)
		{
			CalculateFrogJumpS(&frog[i]->actor->position, &tile->centre, &tile->normal, superhopHeight, superHopFrames, i);
			actorAnimate(frog[i]->actor, FROG_ANIM_SUPERHOP, NO, NO, (player[i].jumpSpeed*15)>>4, 0);
		}
		else if (lpMsg->flags & NETUPD_DOUBLEJUMP)
		{
			CalculateFrogJumpS(&frog[i]->actor->position, &tile->centre, &tile->normal, doublehopHeight, doubleHopFrames, i);
			actorAnimate(frog[i]->actor,FROG_ANIM_SOMERSAULT,NO,NO,75,0);
		}
		else
		{
			CalculateFrogJumpS(&frog[i]->actor->position, &tile->centre, &tile->normal, hopHeight, standardHopFrames, i);
			actorAnimate(frog[i]->actor,FROG_ANIM_HOP,NO,NO,frogAnimSpeed,0);
		}
	}

	pl->lastUpdateMsg = lpMsg->tickCount;
}

/*
{
	currTile[0] = &firstTile[lpMsg->tileNum];
	SetVector(&frog[0]->actor->position, &currTile[0]->centre);
}
*/

/*
*/

void SortOutPlayerNumbers()
{
	int s, pl, dpid;
	int start[4] = { -1, -1, -1, -1 };

	for (s=0; s<NUM_FROGS; s++)
	{
		dpid = 0xFFFFFFFF;

		for (pl=0; pl<NUM_FROGS; pl++)
		{
			if (start[pl] < 0 && netPlayerList[pl].dpid < dpid)
			{
				start[pl] = s;
				dpid = netPlayerList[pl].dpid;
			}
		}
	}

	for (pl=0; pl<NUM_FROGS; pl++)
	{
		UBYTE data[2] = { APPMSG_PLAYERNUM, start[pl] };
		dplay->Send(dpidLocalPlayer, netPlayerList[pl].dpid, DPSEND_GUARANTEED, data, 2);
	}
}


void NetgameStartGame()
{
	int pl;
	int players[4];

	nextUpdate = 0;
	hostSync = hostReady = (isServer);
	gameReady = false;

	gameState.mode = INGAME_MODE;
	gameState.multi = SINGLEPLAYER;
	gameState.single = ARCADE_MODE;
	gameState.difficulty = DIFFICULTY_NORMAL;

	for (pl=1; pl<MAX_FROGS; pl++)
	{
		if (!netPlayerList[pl].dpid)
			break;

		netPlayerList[pl].lastUpdateMsg = 0;
		netPlayerList[pl].isReady = false;
	}
	NUM_FROGS = pl;

	for (pl=0; pl<NUM_FROGS; pl++)
	{
		player[pl].character = pl;
		//FROG_FROGGER; //(netPlayerList[pl].isHost)?FROG_FROGGER:FROG_LILLIE;
	}

	NetInstallMessageHandler(NetgameMessageDispatch);

	GTInit( &modeTimer, 1 );
	InitLevel(9, 3);

	for (pl= isServer?1:0; pl<NUM_FROGS; pl++)
	{
		frog[pl]->draw = 0;
	}
	
	netMessage = CreateAndAddTextOverlay(2048, 2048, "Waiting for players", YES, (char)255, font, TEXTOVERLAY_SHADOW);

	if (isServer)
	{
		unsigned char msg;
		
		SortOutPlayerNumbers();

		msg = APPMSG_HOSTREADY;
		NetBroadcastUrgentMessage(&msg, 1);

		msg = APPMSG_READY;
		NetBroadcastUrgentMessage(&msg, 1);
	}

	netgameLoopFunc = NetRaceRun;
}

void NetgameRun()
{
	if (!dplay) return;

	bool wasSync = hostSync;
	bool wasReady = gameReady;

	NetProcessMessages();

	if (!hostReady)
		return;

	if (!hostSync)
	{
		SendPing();
		return;
	}
	else if (!wasSync)
	{
		// We've just become synchronised, so tell everybody else
		unsigned char msg = APPMSG_READY;
		NetBroadcastUrgentMessage(&msg, 1);

		UpdateAllEnemies();	// sync all our enemies with the host
	}

	if (!gameReady)
	{
		int pl;
		gameReady = true;
		for (pl=1; pl<NUM_FROGS; pl++)
		{
			if (!netPlayerList[pl].dpid) break;
			
			if (!netPlayerList[pl].isReady)
			{
				gameReady = false;
				break;
			}
		}

		if (!gameReady)
			return;
	}

	if (gameReady && !wasReady)
	{
		netMessage->draw = 0;

		if (isServer)
		{
			MSG_STARTGAME start;

			gameStartTime = actFrameCount + STARTGAME_COUNT;

			start.appmsg_start = APPMSG_START;
			start.gameStartTime = gameStartTime;	// game starts in five seconds

			NetBroadcastUrgentMessage(&start, sizeof(start));
		}
	}

	// ... otherwise ...

	// Make sure we don't send update messages EVERY frame, in case we're running at, like,
	// 8 million frames per second, or some junk
	if (actFrameCount >= nextUpdate)
	{
		SendUpdateMessage();
		nextUpdate += UPDATE_PERIOD;
	}

	if (actFrameCount > nextPing)
	{
		SendPing();
		nextPing += PING_PERIOD;
	}


	if (netgameLoopFunc)
		netgameLoopFunc();

	UpdateEnemies();
	UpdateSpecialEffects();

	int i;

	for (i=1; i<NUM_FROGS; i++)
	{
		FroggerHop(i);
		if (player[i].jumpTime > 4096)
		{
			player[i].jumpTime = -1;
			SetVectorSS(&frog[i]->actor->position, &currTile[i]->centre);
			actorAnimate(frog[i]->actor, FROG_ANIM_BREATHE, YES, YES, FROG_BREATHE_SPEED, NO);
		}
	}

	frameCount++;
	player[0].inputPause = 0;
}

// ------------------------------------------------------------------------
// OnPing()
// Replies to a PING message, returning the current time on THIS machine and
// the timestamp (unchanged) from the other machine.

void OnPing(MSG_PING*ping, NETPLAYER *player)
{
	MSG_PINGREPLY reply;

	reply.appmsg_pingreply = APPMSG_PINGREPLY;
	reply.reply = timeGetTime()-timeInfo.firstTicks;
	reply.time0 = ping->time;

	dplay->Send(dpidLocalPlayer, player->dpid, 0, &reply, sizeof(reply));
}


// ------------------------------------------------------------------------
// OnPingReply()
// Synchronises clocks with another machine; sets the timeinfo so that actFrameCount
// on this machine will be the same (give or take a bit) as the other machine

void OnPingReply(MSG_PINGREPLY* pingreply, NETPLAYER *player)
{
	unsigned long currTime = timeGetTime();
	long latency = ((long)currTime - (long)pingreply->time0) / 2;

	timeInfo.tickCount = (pingreply->reply + latency);
	timeInfo.firstTicks = (long)currTime - (long)timeInfo.tickCount; //

	hostSync = true;
}


int NetgameMessageDispatch(void *data, unsigned long size, NETPLAYER *player)
{
	switch (*(unsigned char*)data)
	{
	case APPMSG_PLAYERNUM:
		SetFroggerStartPos(gTStart[((UBYTE*)data)[1]], 0); // umm
		frog[0]->draw = 1;
		return 0;

	case APPMSG_UPDATE:
		HandleUpdateMessage((LPMSG_UPDATEGAME)data, player);
		return 0;

	case APPMSG_PING:
		OnPing((MSG_PING*)data, player);
		return 0;

	case APPMSG_PINGREPLY:
		OnPingReply((MSG_PINGREPLY*)data, player);
		return 0;

	case APPMSG_READY:
		player->isReady = true;
		return 0;

	case APPMSG_HOSTREADY:
		hostReady = true;
		return 0;

	case APPMSG_START:
		gameStartTime = ((MSG_STARTGAME*)data)->gameStartTime;
		return 0;
	}

	return -1;
}


/*	--------------------------------------------------------------------------------
	Function		: SendUpdateMessage
	Purpose			: sends an update message
	Parameters		: Player number
	Returns			: 
	Info			: 
*/

int SendUpdateMessage()
{
	MSG_UPDATEGAME updateMessage;
	HRESULT	hRes;
	GAMETILE *tile = (destTile[0]?destTile[0]:currTile[0]);

	//if (!padData.digital[0])
	//	return DPERR_ABORTED;

	// build message	
	updateMessage.type = APPMSG_UPDATE;
	updateMessage.buttons = padData.digital[0];
	updateMessage.toTile = GetTileNo(tile);
	updateMessage.tickCount = timeInfo.tickCount;
	updateMessage.flags = 0;
	
	if (player[0].hasDoubleJumped)
		updateMessage.flags |= NETUPD_DOUBLEJUMP;
	else if (player[0].isSuperHopping)
		updateMessage.flags |= NETUPD_SUPERHOP;

	return NetBroadcastMessage(&updateMessage, sizeof(updateMessage));
}



void SendPing()
{
	if (!isServer)
	{
		MSG_PING ping;

		ping.appmsg_ping = APPMSG_PING;
		ping.time = timeGetTime();

		dplay->Send(dpidLocalPlayer, DPID_SERVERPLAYER, 0, &ping, sizeof(ping));
	}
}
