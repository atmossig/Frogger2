extern "C" {

#include <windows.h>
#include <windowsx.h>
#include <wtypes.h>
#include <crtdbg.h>
#include <commctrl.h>
#include <cguid.h>
#include <ddraw.h>
#include <d3d.h>
#include <dsound.h>
#include <dinput.h>
#include <dplay.h>
#include <dplobby.h>

#include <ultra64.h>
#include "block.h"
#include "directx.h"
#include "controll.h"

}

#include "network.h"
#include "netchat.h"
#include "netgame.h"


DPID netPlayers[MAX_MULTIPLAYERS];


/*	--------------------------------------------------------------------------------
	Function		: HandleUpdateMessage
	Purpose			: interpret a game update from the network
	Parameters		: 
	Returns			: 
	Info			: 
*/
void HandleUpdateMessage( LPDPLAYINFO lpDPInfo,LPMSG_UPDATEGAME lpMsg,DWORD dwMsgSize,DPID idFrom,DPID idTo )
{
	long i;
	char tmp[256];

	wsprintf( tmp, "Receiving: Player %d does %d", idFrom, lpMsg->data );
	AddChatMessage( tmp );

	for( i=1; i<MAX_MULTIPLAYERS; i++ )
		if( netPlayers[i] == idFrom )
		{
			controllerdata[i].button = lpMsg->data;
			break;
		}
}


/*	--------------------------------------------------------------------------------
	Function		: SendUpdateMessage
	Purpose			: sends an update message
	Parameters		: Player number
	Returns			: 
	Info			: 
*/
HRESULT SendUpdateMessage( )
{
	LPMSG_UPDATEGAME lpUpdateMessage = NULL;
	HRESULT	hRes;

	char tmp[256];

	if( !controllerdata[0].button )
		return DPERR_ABORTED;

	lpUpdateMessage = (LPMSG_UPDATEGAME)GlobalAllocPtr(GHND,sizeof(MSG_UPDATEGAME));
	if(lpUpdateMessage == NULL)
	{
		hRes = DPERR_OUTOFMEMORY;
		goto FAILURE;
	}

	// build message	
	lpUpdateMessage->dwType = APPMSG_UPDATEGAME;
	lpUpdateMessage->data = controllerdata[0].button;

	wsprintf( tmp, "Sending: Player %d does %d", netPlayers[0], lpUpdateMessage->data );
	AddChatMessage( tmp );

	// send this data to all other players
	hRes = DPInfo.lpDP4A->Send(DPInfo.dpidPlayer,DPID_ALLPLAYERS,DPSEND_GUARANTEED,lpUpdateMessage,sizeof(MSG_UPDATEGAME));
	if(FAILED(hRes))
		goto FAILURE;

FAILURE:
	if(lpUpdateMessage)
		GlobalFreePtr(lpUpdateMessage);

	return hRes;
}


/*	--------------------------------------------------------------------------------
	Function		: InitMPGameData
	Purpose			: Clear player mappings and stuff
	Parameters		: 
	Returns			: 
	Info			: 
*/
void InitMPGameData( )
{
	long i;

	netPlayers[0] = DPInfo.dpidPlayer;

	for( i=1; i<MAX_MULTIPLAYERS; i++ )
		netPlayers[i] = -1;

	DPInfo.lpDP4A->EnumPlayers( NULL, EnumPlayersCallback, NULL, DPENUMPLAYERS_REMOTE );
}
