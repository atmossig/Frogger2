/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: network.cpp
	Programmer	: David Swift
	Date		: 

----------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <dplay.h>
#include <dplobby.h>

#include "main.h"
#include "islutil.h"
#include "resource.h"

#include "network.h"
#include "netchat.h"
#include "netgame.h"

#include "frogger.h"
#include "frogmove.h"
#include "multi.h"
#include "hud.h"

void *recieveBuffer;
DWORD recieveBufferSize = 1024;

LPDIRECTPLAY4A dplay		= NULL;

DPID	dpidLocalPlayer;
HANDLE	hLocalPlayerEvent;

char	sessionName[256]	= "Frogger2";
char	playerName[32]		= "Player";
bool	isHost			= false;

//void HandleApplicationMessage(LPDPMSG_GENERIC, DWORD, DPID, DPID);
void HandleSystemMessage(LPDPMSG_GENERIC, DWORD, DPID, DPID);

NET_MESSAGEHANDLER messageHandler;	// callback for interpreting game messages

NETPLAYER netPlayerList[MAX_FROGS];







NETPLAYER *GetPlayerFromID(DPID id)
{
	for (int p=0; p<MAX_FROGS; p++)
		if (netPlayerList[p].dpid == id)
			return &netPlayerList[p];

	return NULL;
}

int GetPlayerNumFromID(DPID id)
{
	for (int p=0; p<MAX_FROGS; p++)
		if (netPlayerList[p].dpid == id)
			return p;

	return NULL;
}

int AddNetPlayer(DPID id)
{
	int i;
	NETPLAYER *player;
	HRESULT res;
	DPCAPS dpc;

	dpc.dwSize = sizeof(dpc);
	res = dplay->GetPlayerCaps(id, &dpc, 0);

	for (i=0, player = &netPlayerList[0]; i<MAX_FROGS; i++, player++)
	{
		if (!player->dpid)
		{
			player->dpid = id;
			//player->isHost = ((dpc.dwFlags & DPCAPS_ISHOST) != 0);
			//player->player = i;
			NUM_FROGS++;
			return i;
		}
	}

	return -1;
}

int RemoveNetPlayer(DPID id)
{
	int p, q;
	NETPLAYER *player;

	for (p=0, player=&netPlayerList[0]; p<MAX_FROGS; p++, player++)
		if (netPlayerList[p].dpid == id)
		{
			if (frog[p])
				frog[p]->draw = 0;

			if (mpl[p].penalText)
				mpl[p].penalText->draw = 0;

			if (multiHud.backChars[p])
			{
				multiHud.backChars[p]->draw = false;
				multiHud.penaliseText[p]->draw = false;
				multiHud.penalOver[p]->draw = false;

				multiHud.trophies[p][0]->draw = false;
				multiHud.trophies[p][1]->draw = false;
				multiHud.trophies[p][2]->draw = false;
			}

			q = p+1;
			while (q<NUM_FROGS)
			{
				// here's a bit of comedy code... - ds

				multiHud.backChars[p] = multiHud.backChars[q];
				multiHud.penaliseText[p] = multiHud.penaliseText[q];
				multiHud.penalOver[p] = multiHud.penalOver[q];
				memcpy(multiHud.trophies[p], multiHud.trophies[q], sizeof(SPRITEOVERLAY*)*3);

				netPlayerList[p]	= netPlayerList[q];
				currTile[p]	= currTile[q];
				destTile[p]	= destTile[q];
				frog[p]		= frog[q];
				player[p]	= player[q];
				mpl[p]		= mpl[q];
				gTStart[p]	= gTStart[q];

				p++,q++;
			}

			
			netPlayerList[p].dpid = 0;
			NUM_FROGS--;
			return 1;
		}

	return 0;
}

BOOL CALLBACK PlayerListEnum(DPID id, DWORD type, LPCDPNAME lpname, DWORD flags, LPVOID context)
{
	if (!GetPlayerFromID(id))
		AddNetPlayer(id);

	return TRUE;
}

void SetupNetPlayerList()
{
	for (int i=0; i<MAX_FROGS; i++)
	{
		netPlayerList[i].dpid = 0;
		//netPlayerList[i].player = -1;
	}

	NUM_FROGS=0;

	if (dplay)
	{
		dplay->EnumPlayers(NULL, PlayerListEnum, 0, DPENUMPLAYERS_ALL);
	}
}

NET_MESSAGEHANDLER NetInstallMessageHandler(NET_MESSAGEHANDLER handler)
{
	NET_MESSAGEHANDLER old = messageHandler;

	messageHandler = handler;

	return old;
}

/*	--------------------------------------------------------------------------------
	Function		: ReceiveMessage
	Purpose			: receive a message from a player
	Parameters		: LPDPLAYINFO
	Returns			: HRESULT
	Info			: 
*/
HRESULT ReceiveMessages()
{
	DPID idFrom,idTo;
	HRESULT	hRes;
	DWORD recieveSize;

	if (!recieveBuffer)
		recieveBuffer = AllocMem(recieveBufferSize);

	// loop to read all msg in queue
	while(1)
	{
		// read msgs from any player, including system player
		idFrom = 0;
		idTo = 0;

		DWORD messageSize = recieveBufferSize;

		hRes = dplay->Receive(&idFrom, &idTo, DPRECEIVE_ALL, recieveBuffer, &messageSize);

		// not enough room - resize buffer
		if(hRes == DPERR_BUFFERTOOSMALL)
		{
			if (recieveBuffer) FreeMem(recieveBuffer);

			if (!(recieveBuffer = AllocMem(messageSize)))
				return DPERR_OUTOFMEMORY;
			
			recieveBufferSize = messageSize;
		}
		else
		if (SUCCEEDED(hRes)) /*&& (messageSize >= sizeof(DPMSG_GENERIC)))*/
		{
			// check for system message
			if (idFrom == DPID_SYSMSG)
				HandleSystemMessage((LPDPMSG_GENERIC)recieveBuffer, messageSize, idFrom, idTo);
			else
			{
				NETPLAYER *player = GetPlayerFromID(idFrom);

				if (messageHandler && player)
					messageHandler((VOID*)recieveBuffer, messageSize, player);
	
				//HandleApplicationMessage((LPDPMSG_GENERIC)recieveBuffer, messageSize, idFrom, idTo);
			}
		}
		else
			break;
	};

	return DP_OK;
}

void NetProcessMessages()
{
	if (dplay)
	{
		// handle pending messages
		ReceiveMessages();
	}
}

/*	--------------------------------------------------------------------------------
	Function		: HandleSystemMessage
	Purpose			: handles a system message for multiplay
	Parameters		: LPDPLAYINFO,LPDPMSG_GENERIC,DWORD,DPIP,DPID
	Returns			: void
	Info			: 
*/
void HandleSystemMessage(LPDPMSG_GENERIC lpMsg,DWORD dwMsgSize,DPID idFrom,DPID idTo)
{
	long i;
	LPSTR lpszStr = NULL;

	switch(lpMsg->dwType)
	{
		case DPSYS_CREATEPLAYERORGROUP:
		{
			LPDPMSG_CREATEPLAYERORGROUP lp = (LPDPMSG_CREATEPLAYERORGROUP) lpMsg;

			utilPrintf("NET: '%s' joined the game\n", lp->dpnName.lpszShortNameA);

			AddNetPlayer(lp->dpId);
		}
		break;

		case DPSYS_DESTROYPLAYERORGROUP:
		{
			LPDPMSG_DESTROYPLAYERORGROUP lp = (LPDPMSG_DESTROYPLAYERORGROUP) lpMsg;

			utilPrintf("NET: '%s' left the game\n", lp->dpnName.lpszShortNameA);

			RemoveNetPlayer(lp->dpId);
		}
		break;

		case DPSYS_SESSIONLOST:
		{
			LPDPMSG_SESSIONLOST lp = (LPDPMSG_SESSIONLOST)lpMsg;
			utilPrintf("NET: Lost connection.. shutting down\n");
		}
		break;

		case DPSYS_HOST:
		{
			utilPrintf("NET: This machine is now the host\n");
			// we are now the host
			isHost = true;

			NetgameHostGame();
		}
		break;
	}
}

// --------------------------------------------------------------------------
// NetBroadcastMessage & co
// Send broadcast messages (generally game-generated messages) asynchronously

// A send timeout of 500ms seems to work reliably enough over the net with a
// modem and everything. 
int NetBroadcastMessage(void *data, unsigned long size)
{
	HRESULT res;

	res = dplay->SendEx(dpidLocalPlayer, DPID_ALLPLAYERS,
		DPSEND_ASYNC|DPSEND_NOSENDCOMPLETEMSG,
		data, size, 0, 500, NULL, NULL);

	return res;
}

int NetBroadcastUrgentMessage(void *data, unsigned long size)
{
	HRESULT res;

	res = dplay->SendEx(dpidLocalPlayer, DPID_ALLPLAYERS,
		DPSEND_GUARANTEED|DPSEND_ASYNC,
		data, size, 0, 0, NULL, NULL);

	return res;
}

void NetStartGame()
{

	//.. net-specific stuff after the game's started!
	// (i.e. timer synchronisation and stuff)
}

/*	ds - more hacked network shit

const DWORD APPMSG_UPDATEGAME	= 0;			// message type for update of game
const DWORD APPMSG_GAMECHAT     = 1;			// message type for in game chat
const DWORD APPMSG_SYNCHGAME	= 2;			// message type for update of game
const DWORD APPMSG_SYNCHPING	= 3;			// message type for ping message

const DWORD	MAX_STRLEN			= 200;			// max size of a temporary string
unsigned long networkPlay = 0;
DPLAYINFO DPInfo;

// {37097B21-5635-11d3-8FA8-00A0C9DB6D13}
static const GUID DPCHAT_GUID = { 0x37097b21, 0x5635, 0x11d3, { 0x8f, 0xa8, 0x0, 0xa0, 0xc9, 0xdb, 0x6d, 0x13 } };

unsigned long nextSynchAt;
unsigned long actTickCountModifier = 0;
unsigned long synchSpeed = 60 * 1;
unsigned long pingOffset = 40;
unsigned long synchRecovery = 1;

	if(networkPlay && (gameState.mode == INGAME_MODE))
	{
		
		if (!synchedOK)
		{
			if (DPInfo.bIsHost)
				InitialServerSynch();
			else
				InitialPlayerSynch();
			
			InitTiming(60);
			synchedOK = 1;
			nextSynchAt = synchSpeed;
		}
		else
		{
			SendUpdateMessage();

			if (DPInfo.bIsHost)
			{
				if (actFrameCount >	nextSynchAt)
				{
					nextSynchAt = timeInfo.frameCount + synchSpeed;
					SendSynchMessage();
				}
			}
			else
			{
				if ((actFrameCount+pingOffset) > nextSynchAt)
				{
					nextSynchAt = timeInfo.frameCount + synchSpeed;
					SendPingMessage();
				}

				if (synchedFrameCount>0)
				{
					synchedFrameCount-=synchRecovery;
					actTickCountModifier+=synchRecovery;
					timeInfo.tickModifier = -(long)actTickCountModifier;
				}
				else
				{
					synchedFrameCount+=synchRecovery;
					actTickCountModifier-=synchRecovery;
					timeInfo.tickModifier = -(long)actTickCountModifier;
				}
			}
		}
	}
*/


