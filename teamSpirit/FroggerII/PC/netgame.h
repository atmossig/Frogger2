#ifndef __NETGAME_H
#define __NETGAME_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
	DWORD			dwType;			// message type (APPMSG_UPDATEGAME)
	DWORD			data;			// controller data
	DWORD			tileNum;		// Emergency sync

} MSG_UPDATEGAME,*LPMSG_UPDATEGAME;


extern DPID netPlayers[MAX_MULTIPLAYERS]; // Index is local player number. Zero -> local DPID.


extern HRESULT SendUpdateMessage( ); // Called to send game updates - need format for data
extern void HandleUpdateMessage( LPDPLAYINFO lpDPInfo,LPMSG_UPDATEGAME lpMsg,DWORD dwMsgSize,DPID idFrom,DPID idTo );
extern void InitMPGameData( );

extern void RefreshMPFrogs( );
extern int MPAddFrog( int i );
extern int MPRemoveFrog( int i );


#ifdef __cplusplus
}
#endif

#endif