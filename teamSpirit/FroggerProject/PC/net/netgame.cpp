
// PC headers
#include "network.h"
#include "netgame.h"
#include "main.h"
#include "islpad.h"
#include "islutil.h"

// Common headers
#include "frontend.h"
#include "game.h"
#include "frogger.h"
#include "frogmove.h"
#include "cam.h"

#define BIT(x)	(1<<(x))

#define NETUPD_SUPERHOP		BIT(0)
#define NETUPD_DOUBLEJUMP	BIT(1)

#define UPDATE_PERIOD		5		// in 60ths of a second (actFrameCounts)
#define PING_PERIOD			20

unsigned long nextUpdate = 0, nextPing = 0;

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
	DWORD	appmsg_ping;
	unsigned long time;
};

struct	MSG_PINGREPLY
{
	DWORD	appmsg_pingreply;
	unsigned long reply, time0;
};

typedef struct
{
	unsigned char	type;
	DWORD			actTickCount;
} MSG_SYNCHGAME, *LPMSG_SYNCHGAME;

#define GetTileNo(t) (((DWORD)tile-(DWORD)firstTile)/sizeof(GAMETILE))

int NetgameMessageDispatch(void *data, unsigned long size, NETPLAYER *player);
int SendUpdateMessage();
void SendPing();

/*	--------------------------------------------------------------------------------
	Function		: HandleSynchMessage
	Purpose			: interpret a game update from the network
	Parameters		: 
	Returns			: 
	Info			: 

void HandleSynchMessage( LPDPLAYINFO lpDPInfo,LPMSG_SYNCHGAME lpMsg,DWORD dwMsgSize,DPID idFrom,DPID idTo )
{
	unsigned int i;
	
	if (lpMsg->actTickCount == 0)
	{
		if (DPInfo.bIsHost)
		{
			for( i=1; i<MAX_MULTIPLAYERS; i++ )
				if( netPlayers[i] == idFrom )
					playersReady[i] = 1;
		}
		else
		{
			serverReady = 1;
			serverPlayer = idFrom;
		}
	}
	else
		synchedFrameCount = timeInfo.tickCount - (lpMsg->actTickCount + myLatency);	
}
*/

/*	--------------------------------------------------------------------------------
	Function		: HandleSynchMessage
	Purpose			: interpret a game update from the network
	Parameters		: 
	Returns			: 
	Info			: 

HRESULT HandlePingMessageServer( LPDPLAYINFO lpDPInfo,LPMSG_SYNCHGAME lpMsg,DWORD dwMsgSize,DPID idFrom,DPID idTo )
{
	LPMSG_SYNCHGAME lpMessage = NULL;
	HRESULT	hRes;
	
	// Check the DPlay device
	if(!DPInfo.lpDP4A )
		return DPERR_ABORTED;
	
	// Alloc the message
	if(!(lpMessage = (LPMSG_SYNCHGAME)GlobalAllocPtr(GHND,sizeof(MSG_SYNCHGAME))))
		return DPERR_OUTOFMEMORY;
	
	// build message	
	lpMessage->dwType = APPMSG_SYNCHPING;
	lpMessage->actTickCount = lpMsg->actTickCount;
	
	// send this data to all other players
	hRes = DPInfo.lpDP4A->Send(DPInfo.dpidPlayer,idFrom, 0,lpMessage,sizeof(MSG_SYNCHGAME));	//DPSEND_GUARANTEED

	// Free the message.
	GlobalFreePtr(lpMessage);

	return hRes;
}
*/

/*	--------------------------------------------------------------------------------
	Function		: HandleSynchMessage
	Purpose			: interpret a game update from the network
	Parameters		: 
	Returns			: 
	Info			: 

void HandlePingMessagePlayer( LPDPLAYINFO lpDPInfo,LPMSG_SYNCHGAME lpMsg,DWORD dwMsgSize,DPID idFrom,DPID idTo )
{
	myLatency = (timeInfo.tickCount - lpMsg->actTickCount)/2;
}
*/

/*	--------------------------------------------------------------------------------
	Function		: InitialPlayerSynch
	Purpose			: Tell the server we are ready and then wait until we get a go message.
	Parameters		: 
	Returns			: 
	Info			: 

HRESULT InitialPlayerSynch(void)
{
	// *NB* Should wait for the server to send a ready message, for now we assume the server enters the game first. (Shouldn't really just wait)
	LPMSG_SYNCHGAME lpMessage = NULL;
	HRESULT	hRes;
	
	// Check the DPlay device
	if(!DPInfo.lpDP4A )
		return DPERR_ABORTED;
	
	// Alloc the message
	if(!(lpMessage = (LPMSG_SYNCHGAME)GlobalAllocPtr(GHND,sizeof(MSG_SYNCHGAME))))
		return DPERR_OUTOFMEMORY;
	
	// build message	
	lpMessage->dwType = APPMSG_SYNCHGAME;
	lpMessage->actTickCount = 0;
	
	// send this data to all other players
	hRes = DPInfo.lpDP4A->Send(DPInfo.dpidPlayer,DPID_ALLPLAYERS,DPSEND_GUARANTEED,lpMessage,sizeof(MSG_SYNCHGAME));
	
	// Free the message
	GlobalFreePtr(lpMessage);
	
	// *NB* Wait for the GO message! (Again shouldn't really just wait, but still!) (Also add a timeout!)
	while (!serverReady);

	Sleep(600);

	return hRes;
}
*/

/*	--------------------------------------------------------------------------------
	Function		: InitialServerSynch
	Purpose			: Wait for everyone to be ready, synch clocks, and agree a start time
	Parameters		: 
	Returns			: 
	Info			: 

HRESULT SendPingMessage(void)
{
	LPMSG_SYNCHGAME lpMessage = NULL;
	HRESULT	hRes;
	
	// Check the DPlay device
	if(!DPInfo.lpDP4A )
		return DPERR_ABORTED;
	
	// Alloc the message
	if(!(lpMessage = (LPMSG_SYNCHGAME)GlobalAllocPtr(GHND,sizeof(MSG_SYNCHGAME))))
		return DPERR_OUTOFMEMORY;
	
	// build message	
	lpMessage->dwType = APPMSG_SYNCHPING;
	lpMessage->actTickCount = timeInfo.tickCount;
	
	// send this data to all other players
	hRes = DPInfo.lpDP4A->Send(DPInfo.dpidPlayer,serverPlayer,0,lpMessage,sizeof(MSG_SYNCHGAME));

	// Free the message.
	GlobalFreePtr(lpMessage);

	return hRes;		
}
*/

/*	--------------------------------------------------------------------------------
	Function		: InitialServerSynch
	Purpose			: Wait for everyone to be ready, synch clocks, and agree a start time
	Parameters		: 
	Returns			: 
	Info			: 

HRESULT SendSynchMessage(void)
{
	LPMSG_SYNCHGAME lpMessage = NULL;
	HRESULT	hRes;
	
	// Check the DPlay device
	if(!DPInfo.lpDP4A)
		return DPERR_ABORTED;
		
	// Alloc the message
	if(!(lpMessage = (LPMSG_SYNCHGAME)GlobalAllocPtr(GHND,sizeof(MSG_SYNCHGAME))))
		return DPERR_OUTOFMEMORY;
	
	// build message	
	lpMessage->dwType = APPMSG_SYNCHGAME;
	lpMessage->actTickCount = timeInfo.tickCount;
	
	// send this data to all other players
	hRes = DPInfo.lpDP4A->Send(DPInfo.dpidPlayer,DPID_ALLPLAYERS,0,lpMessage,sizeof(MSG_SYNCHGAME));	//DPSEND_GUARANTEED

	// Free the message.
	GlobalFreePtr(lpMessage);
	
	return hRes;		
}
*/

/*	--------------------------------------------------------------------------------
	Function		: InitialServerSynch
	Purpose			: Wait for everyone to be ready, synch clocks, and agree a start time
	Parameters		: 
	Returns			: 
	Info			: 

HRESULT InitialServerSynch(void)
{
	LPMSG_SYNCHGAME lpMessage = NULL;
	HRESULT	hRes;
	unsigned long everyoneReady = 0;
	int i;
	
	// Check the DPlay device
	if(!DPInfo.lpDP4A )
		return DPERR_ABORTED;

	// *NB* Wait for all the players to be ready to synch (Need a timeout!)
	while (!everyoneReady)
	{
		everyoneReady = TRUE;
		for (i=1; i<NUM_FROGS; i++)
		if (!playersReady[i])
			everyoneReady = 0;
	}

	// Lets try just starting them all!
	//------
	
	// Alloc the message
	if(!(lpMessage = (LPMSG_SYNCHGAME)GlobalAllocPtr(GHND,sizeof(MSG_SYNCHGAME))))
		return DPERR_OUTOFMEMORY;
	
	// build message	
	lpMessage->dwType = APPMSG_SYNCHGAME;
	lpMessage->actTickCount = 0;
	
	// send this data to all other players
	hRes = DPInfo.lpDP4A->Send(DPInfo.dpidPlayer,DPID_ALLPLAYERS,DPSEND_GUARANTEED,lpMessage,sizeof(MSG_SYNCHGAME));

	// Free the message.
	GlobalFreePtr(lpMessage);

	return hRes;		
}
*/

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


void NetgameStartGame()
{
	int pl;
	const int temp_netchars[4] = { FROG_FROGGER, FROG_LILLIE, FROG_HOPPER, FROG_TWEE };
	int players[4];

	nextUpdate = 0;

	gameState.mode = INGAME_MODE;
	gameState.multi = SINGLEPLAYER;
	gameState.single = ARCADE_MODE;
	gameState.difficulty = DIFFICULTY_NORMAL;

	for (pl=1; pl<MAX_FROGS; pl++)
	{
		if (!netPlayerList[pl].dpid)
			break;

		netPlayerList[pl].lastUpdateMsg = 0;
	}
	NUM_FROGS = pl;

	for (pl=0; pl<NUM_FROGS; pl++)
	{
		player[pl].character = FROG_FROGGER; //(netPlayerList[pl].isHost)?FROG_FROGGER:FROG_LILLIE;
	}

	NetInstallMessageHandler(NetgameMessageDispatch);

	GTInit( &modeTimer, 1 );
	InitLevel(9, 0);

}

void NetgameRun()
{
	if (!dplay) return;

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

	NetProcessMessages();

	if (gameState.mode == INGAME_MODE)
	{
		//RunGameLoop();

		UpdateCameraPosition();
		GameProcessController(0);                                      
		UpdateFroggerPos(0);

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
	else
	{
		utilPrintf("PANIC PANIC PANIC PANIC PANIC!!!!!\n");
	}
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
}


int NetgameMessageDispatch(void *data, unsigned long size, NETPLAYER *player)
{
	switch (*(unsigned char*)data)
	{
	case APPMSG_UPDATE:
		HandleUpdateMessage((LPMSG_UPDATEGAME)data, player);
		return 0;

	case APPMSG_PING:
		OnPing((MSG_PING*)data, player);
		return 0;

	case APPMSG_PINGREPLY:
		OnPingReply((MSG_PINGREPLY*)data, player);
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