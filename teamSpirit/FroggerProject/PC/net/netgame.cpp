
// PC headers
#include "network.h"
#include "netgame.h"
#include "main.h"

#include "game.h"
#include "frogger.h"
#include "frogmove.h"
#include "islpad.h"

typedef struct
{
	unsigned char	type;
	DWORD			buttons;		// controller data
	DWORD			tileNum;		// Emergency sync
	DWORD			tickCount;		// When in game frames this event happened.

} MSG_UPDATEGAME,*LPMSG_UPDATEGAME;

typedef struct
{
	unsigned char	type;
	DWORD			actTickCount;
} MSG_SYNCHGAME, *LPMSG_SYNCHGAME;

/*
DPID netPlayers[MAX_MULTIPLAYERS];

unsigned char playersReady[MAX_MULTIPLAYERS] = {0,0,0,0};
unsigned char serverReady = 0;

unsigned char synchedOK = 0;
DPID serverPlayer;
*/


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
void HandleUpdateMessage(LPMSG_UPDATEGAME lpMsg, NETPLAYER *player)
{
	int i = GetPlayerNumFromID(player->dpid);

	padData.digital[i] = lpMsg->buttons;
//	padData[i].tickOn = lpMsg->tickCount;

//	dp("Recieved message for tile %lu\n",lpMsg->tileNum);

	// Re-centre frog on tile if it's not 'in' the current tile
	if( lpMsg->tileNum != (((DWORD)currTile[i] - (DWORD)firstTile) / sizeof(GAMETILE)) )
	{
		SetVector(&frog[i]->actor->position, &firstTile[lpMsg->tileNum].centre);
		currTile[i] = &firstTile[lpMsg->tileNum];
	}
}



int NetgameMessageDispatch(void *data, unsigned long size, NETPLAYER *player)
{
	switch (*(unsigned char*)data)
	{
	case APPMSG_UPDATE:
		HandleUpdateMessage((LPMSG_UPDATEGAME)data, player);
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

	if (!padData.digital[0])
		return DPERR_ABORTED;

	// build message	
	updateMessage.type = APPMSG_UPDATE;
	updateMessage.buttons = padData.digital[0];
	updateMessage.tileNum = ((DWORD)currTile[0] - (DWORD)firstTile) / sizeof(GAMETILE);
	updateMessage.tickCount = timeInfo.tickCount;

	return NetBroadcastMessage(&updateMessage, sizeof(updateMessage));
}
