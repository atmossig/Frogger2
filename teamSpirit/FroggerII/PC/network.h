#ifndef __NETWORK_H
#define __NETWORK_H

#ifdef __cplusplus
extern "C"
{
#endif
//----- [ MACROS / DEFINES / CONSTANTS ] ---------------------------------------------------------

// {37097B21-5635-11d3-8FA8-00A0C9DB6D13}
DEFINE_GUID(DPCHAT_GUID, 0x37097b21, 0x5635, 0x11d3, 0x8f, 0xa8, 0x0, 0xa0, 0xc9, 0xdb, 0x6d, 0x13);

#define INIT(X) { memset(&X,0,sizeof(X)); (X).dwSize = sizeof(X); }

#define TIMERID					1				// timer ID used for refreshing of connection list
#define TIMERINTERVAL			250				// timer update value

#define MAX_MULTIPLAYERS		4				// max players allowed
#define MAX_NAMELEN				200				// max player / session name length

extern const DWORD APPMSG_UPDATEGAME;			// message type for game update
extern const DWORD APPMSG_GAMECHAT;				// message type for in game chat
extern const DWORD APPMSG_SYNCHGAME;			// message type for game synch
extern const DWORD APPMSG_SYNCHPING;			// message type for ping message
extern const DWORD MAX_STRLEN;					// max size of a temporary string


//----- [ DATA TYPES ] ---------------------------------------------------------------------------

typedef struct
{
	LPDIRECTPLAY4A	lpDP4A;			// IDirectPlay4A interface pointer
	HANDLE			hPlayerEvent;	// player event to use
	DPID			dpidPlayer;		// ID of player created
	BOOL			bIsHost;		// TRUE if we are hosting the session

} DPLAYINFO,*LPDPLAYINFO;


extern DPLAYINFO DPInfo;

//----- [ FUNCTION PROTOTYPES ] ------------------------------------------------------------------

extern int InitMPDirectPlay( HINSTANCE hInstance );
extern void UnInitMPDirectPlay( );

BOOL CALLBACK DLGChooseServiceProvider(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
int ChooseServiceProvider(HINSTANCE hInst,HWND hWndMain);

HRESULT CreateDirectPlayInterface(LPDIRECTPLAY4A *lplpDirectPlay4A);
HRESULT DestroyDirectPlayInterface(HWND hWnd,LPDIRECTPLAY4A lpDirectPlay4A);

BOOL FAR PASCAL DirectPlayEnumConnectionsCallback(LPCGUID lpguidSP,LPVOID lpConnection,DWORD dwConnectionSize,LPCDPNAME lpName,DWORD dwFlags,LPVOID lpContext);
BOOL WINAPI EnumPlayersCallback( DPID dpId, DWORD dwPlayerType, LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext );

void DeleteConnectionList(HWND hWnd);
void DeleteSessionInstanceList(HWND hWnd);

HRESULT GetConnection(HWND hWnd,LPVOID *lplpConnection);
HRESULT HostSession(LPDIRECTPLAY4A lpDirectPlay4A,LPSTR lpszSessionName,LPSTR lpszPlayerName,LPDPLAYINFO lpDPInfo);
HRESULT JoinSession(LPDIRECTPLAY4A lpDirectPlay4A,LPGUID lpguidSessionInstance,LPSTR lpszPlayerName,LPDPLAYINFO lpDPInfo);
HRESULT EnumSessions(HWND hWnd,LPDIRECTPLAY4A lpDirectPlay4A);
BOOL FAR PASCAL EnumSessionsCallback(LPCDPSESSIONDESC2 lpSessionDesc,LPDWORD lpdwTimeOut,DWORD dwFlags,LPVOID lpContext);
HRESULT GetSessionInstanceGuid(HWND hWnd,LPGUID lpguidSessionInstance);
void SelectSessionInstance(HWND hWnd,LPGUID lpguidSessionInstance);

void EnableDlgButton(HWND hDlg,int nIDDlgItem,BOOL bEnable);

//------------------------------------------------------------------------------------------------

HRESULT SetupMPConnection(HINSTANCE hInstance,LPDPLAYINFO lpDPInfo);
HRESULT ShutdownMPConnection(LPDPLAYINFO lpDPInfo);

DWORD WINAPI ReceiveThread(LPVOID lpThreadParameter);
HRESULT ReceiveMessage(LPDPLAYINFO lpDPInfo);

void HandleApplicationMessage(LPDPLAYINFO lpDPInfo,LPDPMSG_GENERIC lpMsg,DWORD dwMsgSize,DPID idFrom,DPID idTo);
void HandleSystemMessage(LPDPLAYINFO lpDPInfo,LPDPMSG_GENERIC lpMsg,DWORD dwMsgSize,DPID idFrom,DPID idTo);

void ErrorBox(LPSTR lpszErrorStr,HRESULT hRes);
char *GetDirectPlayErrStr(HRESULT hRes);

HRESULT ConnectUsingLobby(LPDPLAYINFO lpDPInfo);
HRESULT ConnectUsingDialog(HINSTANCE hInstance,LPDPLAYINFO lpDPInfo);

#ifdef __cplusplus
}
#endif

#endif