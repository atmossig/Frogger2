#ifndef __NETGAME_H
#define __NETGAME_H

#ifdef __cplusplus
extern "C"
{
#endif

/*
extern DPID netPlayers[MAX_MULTIPLAYERS]; // Index is local player number. Zero -> local DPID.

extern long myLatency;

extern HRESULT SendUpdateMessage( ); // Called to send game updates - need format for data
extern void HandleUpdateMessage( LPDPLAYINFO lpDPInfo,LPMSG_UPDATEGAME lpMsg,DWORD dwMsgSize,DPID idFrom,DPID idTo );
extern void HandleSynchMessage( LPDPLAYINFO lpDPInfo,LPMSG_SYNCHGAME lpMsg,DWORD dwMsgSize,DPID idFrom,DPID idTo );

extern HRESULT HandlePingMessageServer( LPDPLAYINFO lpDPInfo,LPMSG_SYNCHGAME lpMsg,DWORD dwMsgSize,DPID idFrom,DPID idTo );
extern void HandlePingMessagePlayer( LPDPLAYINFO lpDPInfo,LPMSG_SYNCHGAME lpMsg,DWORD dwMsgSize,DPID idFrom,DPID idTo );

extern HRESULT SendSynchMessage(void);
extern HRESULT SendPingMessage(void);
extern void InitMPGameData();
extern HRESULT InitialServerSynch(void);
extern HRESULT InitialPlayerSynch(void);


extern void RefreshMPFrogs( );
extern int MPAddFrog( int i );
extern int MPRemoveFrog( int i );
extern long synchedFrameCount;
extern unsigned char synchedOK;
*/

#ifdef __cplusplus
}
#endif

#endif