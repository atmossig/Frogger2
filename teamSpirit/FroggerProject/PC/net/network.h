#ifndef __NETWORK_H
#define __NETWORK_H

#include <dplay.h>

#ifdef __cplusplus
extern "C"
{
#endif

enum APPMSGTYPE
{
	APPMSG_CHAT,
	APPMSG_UPDATE,
};

typedef struct _NETPLAYER
{
	DPID	dpid;		// DirectPlay player identifier - passed to IDirectPlay to query address stuff
	bool	isHost;		// true if this player is the host, false otherwise
	
	struct _NETPLAYER *next, *prev;
} NETPLAYER;

extern LPDIRECTPLAY4A dplay;
extern DPID		dpidLocalPlayer;
extern HANDLE	hLocalPlayerEvent;
extern char		sessionName[256];
extern char		playerName[32];
extern bool		isServer;
extern NETPLAYER netPlayerList[4];

typedef int (*NET_MESSAGEHANDLER)(void *data, unsigned long size, NETPLAYER *player);

NET_MESSAGEHANDLER NetInstallMessageHandler(NET_MESSAGEHANDLER handler);
void NetProcessMessages();
void SetupNetPlayerList();

int GetPlayerNumFromID(DPID id);

int NetBroadcastMessage(void *data, unsigned long size);

#ifdef __cplusplus
}
#endif

#endif