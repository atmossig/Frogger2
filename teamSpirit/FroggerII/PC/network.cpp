#define INITGUID

#include <windows.h>
#include <windowsx.h>
#include <crtdbg.h>
#include <commctrl.h>
#include <cguid.h>
#include <ddraw.h>
#include <d3d.h>
#include <dsound.h>
#include <dinput.h>
#include <dplay.h>
#include <dplobby.h>

#include "network.h"
#include "netchat.h"
#include "netgame.h"

extern "C" {

#include <ultra64.h>
#include "block.h"
#include "directx.h"
#include "..\resource.h"
#include "types.h"
#include "define.h"
#include "font.h"
#include "actor.h"
#include "overlays.h"
#include "frogger.h"
#include "babyfrogs.h"
#include "game.h"

}


HANDLE ghReceiveThread			= NULL;			// handle of receive thread
DWORD gidReceiveThread			= 0;			// id of receive thread
HANDLE ghKillReceiveEvent		= NULL;			// event used to kill receive thread

const DWORD APPMSG_UPDATEGAME	= 0;			// message type for update of game
const DWORD APPMSG_GAMECHAT     = 1;			// message type for in game chat
const DWORD	MAX_STRLEN			= 200;			// max size of a temporary string

DPLAYINFO DPInfo;

//----- [ FUNCTION IMPLEMENTATIONS ] -------------------------------------------------------------

int InitMPDirectPlay( HINSTANCE hInstance )
{
	HRESULT hRes;
	int iResult = 0;

	// init COM library
	hRes = CoInitialize(NULL);
	if(FAILED(hRes))
		return 0;

	hRes = SetupMPConnection(hInstance,&DPInfo);
	if(FAILED(hRes))
		return 0;

	InitChatBuffer( );
	chatFlags = CHAT_SHOW;
	gameState.multi = MULTIREMOTE;

	return iResult;
}

void UnInitMPDirectPlay( )
{
	ShutdownMPConnection(&DPInfo);

	FreeChatBuffer( );
	chatFlags = 0;

	// de-init COM library
	CoUninitialize();
}

/*	--------------------------------------------------------------------------------
	Function		: EnableDlgButton
	Purpose			: enables / disables a button on a dialog
	Parameters		: HWND,int,BOOL
	Returns			: void
	Info			: 
*/
void EnableDlgButton(HWND hDlg,int nIDDlgItem,BOOL bEnable)
{
	EnableWindow(GetDlgItem(hDlg,nIDDlgItem),bEnable);
}

/*	--------------------------------------------------------------------------------
	Function		: DLGChooseServiceProvider
	Purpose			: dialog box to handle choosing of multiplay service provider
	Parameters		: HWND,UINT,WPARAM,LPARAM
	Returns			: BOOL
	Info			: 
*/
BOOL CALLBACK DLGChooseServiceProvider(HWND hDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{
	static LPDPLAYINFO lpDPInfo;
	static LPDIRECTPLAY4A lpDP4A;
	static UINT idTimer;
	GUID guidSessionInstance;
	char sessionName[MAX_NAMELEN];
	char playerName[MAX_NAMELEN];
	unsigned long dwNameSize;
	HRESULT hRes;
	LPVOID lpConnection = NULL;

    switch(msg)
	{
		case WM_INITDIALOG:
			// set up for network multiplay using DirectPlay
			// save connection info ptr
			lpDPInfo = (LPDPLAYINFO)lParam;
			lpDP4A = NULL;
			hRes = CreateDirectPlayInterface(&lpDP4A);
			if(FAILED(hRes))
				goto SETUP_FAILURE;

			// set first item in the connections combo box
			SendDlgItemMessage(hDlg,IDC_CONNECTION,CB_ADDSTRING,(WPARAM)0,(LPARAM)"<select service provider>");
			SendDlgItemMessage(hDlg,IDC_CONNECTION,CB_SETITEMDATA,(WPARAM)0,(LPARAM)0);
			SendDlgItemMessage(hDlg,IDC_CONNECTION,CB_SETCURSEL,(WPARAM)0,(LPARAM)0);

			// put any other available connections in the combo box
			lpDP4A->EnumConnections(&DPCHAT_GUID,DirectPlayEnumConnectionsCallback,hDlg,0);
		    
			EnableDlgButton(hDlg,IDC_HOSTBUTTON,FALSE);
			EnableDlgButton(hDlg,IDC_JOINBUTTON,FALSE);
			break;

		SETUP_FAILURE:
			MessageBox(NULL,"This application requires DirectX 5 or later.",NULL,MB_OK);
			EndDialog(hDlg,FALSE);
			break;

		case WM_DESTROY:
			// delete information stored along with the lists
			DeleteConnectionList(hDlg);
			DeleteSessionInstanceList(hDlg);
			break;

		case WM_TIMER:
			// refresh session list; guard against leftover timer msgs after timer has been killed
			if(idTimer)
			{
				hRes = EnumSessions(hDlg,lpDP4A);
				if(FAILED(hRes))
				{
					if(hRes == DPERR_USERCANCEL)
					{
						KillTimer(hDlg,idTimer);
						idTimer = 0;
					}
					else if(hRes != DPERR_CONNECTING)
					{
						if(MessageBox(NULL,"Session enumeration failed - retry ?","Continue ?",MB_OKCANCEL) == IDCANCEL)
						{
							KillTimer(hDlg,idTimer);
							idTimer = 0;
						}
					}
				}
			}
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_CONNECTION:
					switch(HIWORD(wParam))
					{
						case CBN_SELCHANGE:
							// service provider changed; rebuild display and
							// delete any existing DirectPlay interface
							if(idTimer)
							{
								KillTimer(hDlg,idTimer);
								idTimer = 0;
							}
			
							hRes = DestroyDirectPlayInterface(hDlg,lpDP4A);
							lpDP4A = NULL;

							// get pointer to the selected connection
							hRes = GetConnection(hDlg,&lpConnection);
							if(FAILED(hRes))
								goto SP_FAILURE;

							if(lpConnection)
							{
								// create new DPlay interface
								hRes = CreateDirectPlayInterface(&lpDP4A);
								if((FAILED(hRes)) || (NULL == lpDP4A))
									goto SP_FAILURE;

								// initialize the connection
								hRes = lpDP4A->InitializeConnection(lpConnection,0);
								if(FAILED(hRes))
									goto SP_FAILURE;

								// OK to host now
								EnableDlgButton(hDlg,IDC_HOSTBUTTON,TRUE);

								// start enumerating the sessions
								hRes = EnumSessions(hDlg,lpDP4A);
								if(FAILED(hRes))
									goto SP_FAILURE;

								// set a timer to refresh the session list
								idTimer = SetTimer(hDlg,TIMERID,TIMERINTERVAL,NULL);
							}
							else
							{
								// selected generic option "<Select a service provider>"
								EnableDlgButton(hDlg,IDC_HOSTBUTTON,FALSE);
								EnableDlgButton(hDlg,IDC_JOINBUTTON,FALSE);
							}
							break;
						}
					break;

				SP_FAILURE:
					if(hRes != DPERR_USERCANCEL)
						ErrorBox("Could not select service provider - %s",hRes);
					break;


				case IDC_HOSTBUTTON:
					// should have an interface by now
					if(lpDP4A == NULL)
						break;

					if(idTimer)
					{
						KillTimer(hDlg,idTimer);
						idTimer = 0;
					}
					// use computer name for session name
					dwNameSize = MAX_NAMELEN;
					if(!GetComputerName(sessionName,&dwNameSize))
						lstrcpy(sessionName,"Session");

					// use user name for player name
					dwNameSize = MAX_NAMELEN;
					if(!GetUserName(playerName,&dwNameSize))
						lstrcpy(playerName,"unknown");

					// host a new session on this service provider
					hRes = HostSession(lpDP4A,sessionName,playerName,lpDPInfo);
					if(FAILED(hRes))
						goto HOST_FAILURE;

					// dismiss dialog if we succeeded in hosting
					EndDialog(hDlg,TRUE);
					break;

				HOST_FAILURE:
					ErrorBox("Could not host session - %s",hRes);
					break;

				case IDC_JOINBUTTON:
					// should have an interface by now
					if(lpDP4A == NULL)
						break;

					if(idTimer)
					{
						KillTimer(hDlg,idTimer);
						idTimer = 0;
					}
					// get guid of selected session instance
					hRes = GetSessionInstanceGuid(hDlg,&guidSessionInstance);
					if(FAILED(hRes))
						goto JOIN_FAILURE;

					// use user name for player name
					dwNameSize = MAX_NAMELEN;
					if(!GetUserName(playerName,&dwNameSize))
						lstrcpy(playerName,"unknown");

					// join this session
					hRes = JoinSession(lpDP4A,&guidSessionInstance,playerName,lpDPInfo);

					if(FAILED(hRes))
						goto JOIN_FAILURE;

					// dismiss dialog if we succeeded in joining
					EndDialog(hDlg,TRUE);
					break;

				JOIN_FAILURE:
					ErrorBox("Could not join session - %s",hRes);
					break;


				case IDCANCEL:
					if(idTimer)
					{
						KillTimer(hDlg,idTimer);
						idTimer = 0;
					}
					// delete any interface created if cancelling
					hRes = DestroyDirectPlayInterface(hDlg,lpDP4A);
					lpDP4A = NULL;
					EndDialog(hDlg,FALSE);
					break;
			}

			break;
	}

    return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function		: ChooseServiceProvider
	Purpose			: function to initiate the selection of a service provider
	Parameters		: HINSTANCE,HWND
	Returns			: 
	Info			: 
*/
int ChooseServiceProvider(HINSTANCE hInst,HWND hWndMain)
{
	dp("\nChoose service provider...");
    if(DialogBox(hInst,MAKEINTRESOURCE(IDD_CONNECTION),hWndMain,(DLGPROC)DLGChooseServiceProvider))
		return 1;

    return 0;
}

/*	--------------------------------------------------------------------------------
	Function		: CreateDirectPlayInterface
	Purpose			: creates the DirectPlay interface
	Parameters		: LPDIRECTPLAY4A *
	Returns			: HRESULT
	Info			: 
*/
HRESULT CreateDirectPlayInterface(LPDIRECTPLAY4A *lplpDirectPlay4A)
{
	// create IDirectPlay interface
	return CoCreateInstance(CLSID_DirectPlay,NULL,CLSCTX_INPROC_SERVER, 
							IID_IDirectPlay4A,(LPVOID*)lplpDirectPlay4A);
}

/*	--------------------------------------------------------------------------------
	Function		: DestroyDirectPlayInterface
	Purpose			: destroys / frees the DirectPlay interface
	Parameters		: HWND,LPDIRECTPLAY4A
	Returns			: HRESULT
	Info			: 
*/
HRESULT DestroyDirectPlayInterface(HWND hWnd,LPDIRECTPLAY4A lpDirectPlay4A)
{
	HRESULT hRes = DP_OK;

	if(lpDirectPlay4A)
	{
		DeleteSessionInstanceList(hWnd);
		EnableDlgButton(hWnd,IDC_JOINBUTTON,FALSE);

		hRes = lpDirectPlay4A->Release();
	}

	return hRes;
}

/*	--------------------------------------------------------------------------------
	Function		: DirectPlayEnumConnectionsCallback
	Purpose			: enumerates connections for multiplay over network
	Parameters		: LPCGUID,LPVOID,DWORD,LPCDPNAME,DWORD,LPVOID
	Returns			: BOOL
	Info			: 
*/
BOOL FAR PASCAL DirectPlayEnumConnectionsCallback(LPCGUID lpguidSP,LPVOID lpConnection,DWORD dwConnectionSize,LPCDPNAME	lpName,DWORD dwFlags,LPVOID lpContext)
{
	HWND hWnd = (HWND)lpContext;
	LRESULT iIndex;
	LPVOID lpConnectionBuffer;

	// store service provider name in combo box
	iIndex = SendDlgItemMessage(hWnd,IDC_CONNECTION,CB_ADDSTRING,0,(LPARAM)lpName->lpszShortNameA);
	if(iIndex == CB_ERR)
		goto FAILURE;

	// make space for connection shortcut
	lpConnectionBuffer = GlobalAllocPtr(GHND,dwConnectionSize);
	if(lpConnectionBuffer == NULL)
		goto FAILURE;

	// store pointer to connection shortcut in combo box
	memcpy(lpConnectionBuffer,lpConnection,dwConnectionSize);
	SendDlgItemMessage(hWnd,IDC_CONNECTION,CB_SETITEMDATA,(WPARAM)iIndex,(LPARAM)lpConnectionBuffer);

FAILURE:
	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function		: DeleteSessionInstanceList
	Purpose			: deletes sessions from the session listbox
	Parameters		: HWND
	Returns			: void
	Info			: 
*/
void DeleteSessionInstanceList(HWND hWnd)
{
	WPARAM i;
	LONG lpData;
	
	// destroy the GUID's stored with each session name
	i = 0;
	while(TRUE)
	{
		// get data pointer stored with item
		lpData = SendDlgItemMessage(hWnd,IDC_SESSIONLIST,LB_GETITEMDATA,(WPARAM)i,(LPARAM)0);
		if(lpData == CB_ERR)			// error getting data
			break;

		if(lpData == 0)					// no data to delete
			continue;

		GlobalFreePtr((LPVOID)lpData);
		i += 1;
	}

	// delete all items in list
	SendDlgItemMessage(hWnd,IDC_SESSIONLIST,LB_RESETCONTENT,(WPARAM)0,(LPARAM)0);
}

/*	--------------------------------------------------------------------------------
	Function		: DeleteConnectionList
	Purpose			: deletes the connections in the connection list
	Parameters		: HWND
	Returns			: void
	Info			: 
*/
void DeleteConnectionList(HWND hWnd)
{
	WPARAM i;
	LONG lpData;
	
	// destroy the GUID's stored with each service provider name
	i = 0;
	while(TRUE)
	{
		// get data pointer stored with item
		lpData = SendDlgItemMessage(hWnd,IDC_CONNECTION,CB_GETITEMDATA,(WPARAM)i,(LPARAM)0);
		if(lpData == CB_ERR)			// error getting data
			break;

		if(lpData != 0)					// no data to delete
			GlobalFreePtr((LPVOID)lpData);

		i += 1;
	}

	// delete all items in combo box
	SendDlgItemMessage(hWnd,IDC_CONNECTION,CB_RESETCONTENT,(WPARAM)0,(LPARAM)0);
}

/*	--------------------------------------------------------------------------------
	Function		: GetConnection
	Purpose			: get selected connection
	Parameters		: HWND,LPVOID *
	Returns			: HRESULT
	Info			: 
*/
HRESULT GetConnection(HWND hWnd,LPVOID *lplpConnection)
{
	LONG iIndex;

	// get index of the item currently selected in the combobox
	iIndex = SendDlgItemMessage(hWnd,IDC_CONNECTION,CB_GETCURSEL,(WPARAM)0,(LPARAM)0);
	if(iIndex == CB_ERR)
		return DPERR_GENERIC;

	// get the pointer to the connection shortcut associated with the item
	iIndex = SendDlgItemMessage(hWnd,IDC_CONNECTION,CB_GETITEMDATA,(WPARAM)iIndex,(LPARAM)0);
	if(iIndex == CB_ERR)
		return DPERR_GENERIC;

	*lplpConnection = (LPVOID)iIndex;

	return DP_OK;
}

/*	--------------------------------------------------------------------------------
	Function		: HostSession
	Purpose			: prepare for hosting a multiplayer session
	Parameters		: LPDIRECTPLAY4A,LPSTR,LPSTR,LPDPLAYINFO
	Returns			: HRESULT
	Info			: 
*/
HRESULT HostSession(LPDIRECTPLAY4A lpDirectPlay4A,LPSTR lpszSessionName,LPSTR lpszPlayerName,LPDPLAYINFO lpDPInfo)
{
	DPID dpidPlayer;
	DPNAME dpName;
	DPSESSIONDESC2 sessionDesc;
	HRESULT hRes;

	// check for valid interface
	if(lpDirectPlay4A == NULL)
		return DPERR_INVALIDOBJECT;

	// host new session
	INIT(sessionDesc);
	sessionDesc.dwFlags = DPSESSION_MIGRATEHOST | DPSESSION_KEEPALIVE;
	sessionDesc.guidApplication = DPCHAT_GUID;
	sessionDesc.dwMaxPlayers = MAX_MULTIPLAYERS;
	sessionDesc.lpszSessionNameA = lpszSessionName;

	hRes = lpDirectPlay4A->Open(&sessionDesc,DPOPEN_CREATE);
	if(FAILED(hRes))
		goto OPEN_FAILURE;

	// fill out name structure
	INIT(dpName);
	dpName.lpszShortNameA = lpszPlayerName;
	dpName.lpszLongNameA = NULL;

	// create a player with this name
	hRes = lpDirectPlay4A->CreatePlayer(&dpidPlayer,&dpName,lpDPInfo->hPlayerEvent,NULL,0,0);
	if(FAILED(hRes))
		goto CREATEPLAYER_FAILURE;

	// return connection info
	lpDPInfo->lpDP4A = lpDirectPlay4A;
	lpDPInfo->dpidPlayer = dpidPlayer;
	lpDPInfo->bIsHost = TRUE;

	InitMPGameData( );

	return DP_OK;

CREATEPLAYER_FAILURE:
OPEN_FAILURE:
	lpDirectPlay4A->Close();
	return hRes;
}

/*	--------------------------------------------------------------------------------
	Function		: JoinSession
	Purpose			: prepare for joining a multiplayer session
	Parameters		: LPDIRECTPLAY4A,LPGUID,LPSTR,LPDPLAYINFO
	Returns			: HRESULT
	Info			: 
*/
HRESULT JoinSession(LPDIRECTPLAY4A lpDirectPlay4A,LPGUID lpguidSessionInstance,LPSTR lpszPlayerName,LPDPLAYINFO lpDPInfo)
{
	DPID dpidPlayer;
	DPNAME dpName;
	DPSESSIONDESC2 sessionDesc;
	HRESULT hRes;

	// check for valid interface
	if(lpDirectPlay4A == NULL)
		return DPERR_INVALIDOBJECT;

	// join existing session
	INIT(sessionDesc);
	sessionDesc.guidInstance = *lpguidSessionInstance;

	hRes = lpDirectPlay4A->Open(&sessionDesc,DPOPEN_JOIN);
	if(FAILED(hRes))
		goto OPEN_FAILURE;

	// fill out name structure
	INIT(dpName);
	dpName.lpszShortNameA = lpszPlayerName;
	dpName.lpszLongNameA = NULL;

	// create a player with this name
	hRes = lpDirectPlay4A->CreatePlayer(&dpidPlayer,&dpName,lpDPInfo->hPlayerEvent,NULL,0,0);
	if(FAILED(hRes))
		goto CREATEPLAYER_FAILURE;

	// return connection info
	lpDPInfo->lpDP4A = lpDirectPlay4A;
	lpDPInfo->dpidPlayer = dpidPlayer;
	lpDPInfo->bIsHost = FALSE;

	InitMPGameData( );

	return DP_OK;

CREATEPLAYER_FAILURE:
OPEN_FAILURE:
	lpDirectPlay4A->Close();
	return hRes;
}

/*	--------------------------------------------------------------------------------
	Function		: GetSessionInstanceGuid
	Purpose			: get GUID for instance of session
	Parameters		: HWND,LPGUID
	Returns			: HRESULT
	Info			: 
*/
HRESULT GetSessionInstanceGuid(HWND hWnd,LPGUID lpguidSessionInstance)
{
	LONG iIndex;

	// get guid for session
	iIndex = SendDlgItemMessage(hWnd,IDC_SESSIONLIST,LB_GETCURSEL,(WPARAM)0,(LPARAM)0);
	if(iIndex == LB_ERR)
		return DPERR_GENERIC;

	iIndex = SendDlgItemMessage(hWnd,IDC_SESSIONLIST,LB_GETITEMDATA,(WPARAM)iIndex,(LPARAM)0);
	if((iIndex == LB_ERR) || (iIndex == 0))
		return DPERR_GENERIC;

	*lpguidSessionInstance = *((LPGUID) iIndex);

	return DP_OK;
}

/*	--------------------------------------------------------------------------------
	Function		: SelectSessionInstance
	Purpose			: select instance of a particular session
	Parameters		: HWND,LPGUID
	Returns			: void
	Info			: 
*/
void SelectSessionInstance(HWND hWnd,LPGUID lpguidSessionInstance)
{
	WPARAM i,iIndex;
	LONG lpData;
	
	// loop thru GUID's stored with each session name to find one that matches what was passed in
	i = 0;
	iIndex = 0;
	while(TRUE)
	{
		// get data pointer stored with item
		lpData = SendDlgItemMessage(hWnd,IDC_SESSIONLIST,LB_GETITEMDATA,(WPARAM)i,(LPARAM)0);
		if(lpData == CB_ERR)			// error getting data
			break;

		if(lpData == 0)					// no data to compare to
			continue;

		// guid matches
		if(IsEqualGUID(*lpguidSessionInstance,*((LPGUID) lpData)))
		{
			iIndex = i;					// store index of this string
			break;
		}

		i += 1;
	}

	// select this item
	SendDlgItemMessage(hWnd,IDC_SESSIONLIST,LB_SETCURSEL,(WPARAM)iIndex,(LPARAM)0);
}

/*	--------------------------------------------------------------------------------
	Function		: EnumSessions
	Purpose			: enumerate current sessions
	Parameters		: HWND,LPDIRECTPLAY4A
	Returns			: HRESULT
	Info			: 
*/
HRESULT EnumSessions(HWND hWnd,LPDIRECTPLAY4A lpDirectPlay4A)
{
	DPSESSIONDESC2 sessionDesc;
	GUID guidSessionInstance;
	LONG iIndex;
	HRESULT hRes;

	// check for valid interface
	if(lpDirectPlay4A == NULL)
		return DPERR_INVALIDOBJECT;

	// get guid of currently selected session
	guidSessionInstance = GUID_NULL;
	hRes = GetSessionInstanceGuid(hWnd,&guidSessionInstance);

	// delete existing session list
	DeleteSessionInstanceList(hWnd);

	// add sessions to session list
	INIT(sessionDesc);
	sessionDesc.guidApplication = DPCHAT_GUID;

	hRes = lpDirectPlay4A->EnumSessions(&sessionDesc,0,EnumSessionsCallback,hWnd,DPENUMSESSIONS_AVAILABLE | DPENUMSESSIONS_ASYNC);

	// select the session that was previously selected
	SelectSessionInstance(hWnd,&guidSessionInstance);

	// hilite "Join" button only if there are sessions to join
	iIndex = SendDlgItemMessage(hWnd,IDC_SESSIONLIST,LB_GETCOUNT,(WPARAM)0,(LPARAM)0);
	EnableDlgButton(hWnd,IDC_JOINBUTTON,(iIndex > 0) ? TRUE : FALSE);

	return hRes;
}

/*	--------------------------------------------------------------------------------
	Function		: EnumSessionsCallback
	Purpose			: enumerate sessions callback function
	Parameters		: LPCDPSESSIONDESC2,LPDWORD,DWORD,LPVOID
	Returns			: BOOL
	Info			: 
*/
BOOL FAR PASCAL EnumSessionsCallback(LPCDPSESSIONDESC2 lpSessionDesc,LPDWORD lpdwTimeOut,DWORD dwFlags,LPVOID lpContext)
{
	HWND hWnd = (HWND)lpContext;
	LPGUID lpGuid;
	LONG iIndex;

	// see if last session has been enumerated
	if(dwFlags & DPESC_TIMEDOUT)
		return FALSE;

	// store session name in list
	iIndex = SendDlgItemMessage(hWnd,IDC_SESSIONLIST,LB_ADDSTRING,(WPARAM)0,(LPARAM)lpSessionDesc->lpszSessionNameA);

	if(iIndex == LB_ERR)
		goto FAILURE;

	// make space for session instance guid
	lpGuid = (LPGUID)GlobalAllocPtr(GHND,sizeof(GUID));
	if(lpGuid == NULL)
		goto FAILURE;

	// store pointer to guid in list
	*lpGuid = lpSessionDesc->guidInstance;
	SendDlgItemMessage(hWnd,IDC_SESSIONLIST,LB_SETITEMDATA,(WPARAM)iIndex,(LPARAM) lpGuid);

FAILURE:
	return TRUE;
}

//------------------------------------------------------------------------------------------------

/*	--------------------------------------------------------------------------------
	Function		: SetupMPConnection
	Purpose			: sets up for a multiplayer connection
	Parameters		: HINSTANCE,LPDPLAYINFO
	Returns			: HRESULT
	Info			: 
*/
HRESULT SetupMPConnection(HINSTANCE hInstance,LPDPLAYINFO lpDPInfo)
{
	HRESULT hRes;

	ZeroMemory(lpDPInfo,sizeof(DPLAYINFO));

	// create event used by DPlay to signal a msg has arrived
	lpDPInfo->hPlayerEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	if(lpDPInfo->hPlayerEvent == NULL)
	{
		hRes = DPERR_NOMEMORY;
		goto FAILURE;
	}

	// create event used to signal that receive thread should exit
	ghKillReceiveEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	if(ghKillReceiveEvent == NULL)
	{
		hRes = DPERR_NOMEMORY;
		goto FAILURE;
	}

	// create thread to receive player messages
	ghReceiveThread = CreateThread(NULL,0,ReceiveThread,lpDPInfo,0,&gidReceiveThread);
	if(ghReceiveThread == NULL)
	{
		hRes = DPERR_NOMEMORY;
		goto FAILURE;
	}

	// try to connect using the lobby
	hRes = ConnectUsingLobby(lpDPInfo);
	if(FAILED(hRes))
	{
		// if the error returned is DPERR_NOTLOBBIED, that means we
		// were not launched by a lobby and we should ask the user for
		// connection settings. If any other error is returned it means
		// we were launched by a lobby but there was an error making the
		// connection.
		if(hRes != DPERR_NOTLOBBIED)
			ErrorBox("Could not connect using lobby - %s",hRes);

		// if there is no lobby connection, ask the user for settings
		hRes = ConnectUsingDialog(hInstance,lpDPInfo);
		if(FAILED(hRes))
			goto FAILURE;
	}

	return DP_OK;	

FAILURE:
	ShutdownMPConnection(lpDPInfo);

	return hRes;
}

/*	--------------------------------------------------------------------------------
	Function		: ShutdownMPConnection
	Purpose			: shuts down a multiplayer connection
	Parameters		: LPDPLAYINFO
	Returns			: HRESULT
	Info			: 
*/
HRESULT ShutdownMPConnection(LPDPLAYINFO lpDPInfo)
{
	if(ghReceiveThread)
	{
		// wake up receive thread and wait for it to quit
		SetEvent(ghKillReceiveEvent);
		WaitForSingleObject(ghReceiveThread,INFINITE);

		CloseHandle(ghReceiveThread);
		ghReceiveThread = NULL;
	}

	if(ghKillReceiveEvent)
	{
		CloseHandle(ghKillReceiveEvent);
		ghKillReceiveEvent = NULL;
	}

	if(lpDPInfo->lpDP4A)
	{
		if(lpDPInfo->dpidPlayer)
		{
			lpDPInfo->lpDP4A->DestroyPlayer(lpDPInfo->dpidPlayer);
			lpDPInfo->dpidPlayer = 0;
		}
		lpDPInfo->lpDP4A->Close();
		lpDPInfo->lpDP4A->Release();
		lpDPInfo->lpDP4A = NULL;
	}

	if(lpDPInfo->hPlayerEvent)
	{
		CloseHandle(lpDPInfo->hPlayerEvent);
		lpDPInfo->hPlayerEvent = NULL;
	}

	return DP_OK;
}

/*	--------------------------------------------------------------------------------
	Function		: ReceiveThread
	Purpose			: handle player events on a thread
	Parameters		: LPVOID
	Returns			: DWORD
	Info			: 
*/
DWORD WINAPI ReceiveThread(LPVOID lpThreadParameter)
{
	LPDPLAYINFO	lpDPInfo = (LPDPLAYINFO)lpThreadParameter;
	HANDLE eventHandles[2];

	eventHandles[0] = lpDPInfo->hPlayerEvent;
	eventHandles[1] = ghKillReceiveEvent;

	// loop waiting for player events - if kill event signalled, thread will exit
	while(WaitForMultipleObjects(2,eventHandles,FALSE,INFINITE) == WAIT_OBJECT_0)
	{
		// receive any messages in queue
		ReceiveMessage(lpDPInfo);
	}

	ExitThread(0);

	return 0;
}

/*	--------------------------------------------------------------------------------
	Function		: ReceiveMessage
	Purpose			: receive a message from a player
	Parameters		: LPDPLAYINFO
	Returns			: HRESULT
	Info			: 
*/
HRESULT ReceiveMessage(LPDPLAYINFO lpDPInfo)
{
	DPID idFrom,idTo;
	LPVOID lpvMsgBuffer;
	unsigned long dwMsgBufferSize;
	HRESULT	hRes;

	lpvMsgBuffer = NULL;
	dwMsgBufferSize = 0;

	// loop to read all msg in queue
	do
	{
		// loop until single msg is successfully read
		do
		{
			// read msgs from any player, including system player
			idFrom = 0;
			idTo = 0;

			hRes = lpDPInfo->lpDP4A->Receive(&idFrom,&idTo,DPRECEIVE_ALL,lpvMsgBuffer,&dwMsgBufferSize);

			// not enough room - resize buffer
			if(hRes == DPERR_BUFFERTOOSMALL)
			{
				if(lpvMsgBuffer)
					GlobalFreePtr(lpvMsgBuffer);
				lpvMsgBuffer = GlobalAllocPtr(GHND,dwMsgBufferSize);
				if(lpvMsgBuffer == NULL)
					hRes = DPERR_OUTOFMEMORY;
			}
		}
		while(hRes == DPERR_BUFFERTOOSMALL);

		if((SUCCEEDED(hRes)) && (dwMsgBufferSize >= sizeof(DPMSG_GENERIC)))
		{
			// check for system message
			if(idFrom == DPID_SYSMSG)
				HandleSystemMessage(lpDPInfo,(LPDPMSG_GENERIC)lpvMsgBuffer,dwMsgBufferSize,idFrom,idTo);
			else
				HandleApplicationMessage(lpDPInfo,(LPDPMSG_GENERIC)lpvMsgBuffer,dwMsgBufferSize,idFrom,idTo);
		}
	}
	while(SUCCEEDED(hRes));

	// free any memory allocated
	if(lpvMsgBuffer)
		GlobalFreePtr(lpvMsgBuffer);

	return DP_OK;
}

/*	--------------------------------------------------------------------------------
	Function		: HandleApplicationMessage
	Purpose			: handles application message for multiplay
	Parameters		: LPDPLAYINFO,LPDPMSG_GENERIC,DWORD,DPID,DPID
	Returns			: void
	Info			: 
*/
void HandleApplicationMessage(LPDPLAYINFO lpDPInfo,LPDPMSG_GENERIC lpMsg,DWORD dwMsgSize,DPID idFrom,DPID idTo)
{
	HRESULT	hRes;

	switch(lpMsg->dwType)
	{
		case APPMSG_UPDATEGAME:
			HandleUpdateMessage( lpDPInfo, (LPMSG_UPDATEGAME)lpMsg, dwMsgSize, idFrom, idTo );
		break;

		case APPMSG_GAMECHAT:
			HandleChatMessage( lpDPInfo, (LPMSG_CHATSTRING)lpMsg, dwMsgSize, idFrom, idTo );
		break;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: HandleSystemMessage
	Purpose			: handles a system message for multiplay
	Parameters		: LPDPLAYINFO,LPDPMSG_GENERIC,DWORD,DPIP,DPID
	Returns			: void
	Info			: 
*/
void HandleSystemMessage(LPDPLAYINFO lpDPInfo,LPDPMSG_GENERIC lpMsg,DWORD dwMsgSize,DPID idFrom,DPID idTo)
{
	long i;
	LPSTR lpszStr = NULL;

	switch(lpMsg->dwType)
	{
		case DPSYS_CREATEPLAYERORGROUP:
		{
			LPDPMSG_CREATEPLAYERORGROUP lp = (LPDPMSG_CREATEPLAYERORGROUP) lpMsg;
			LPSTR lpszPlayerName;
			LPSTR szDisplayFormat = "\"%s\" has joined";

			for( i=1; i<MAX_MULTIPLAYERS; i++ )
				if( netPlayers[i] == -1 )
				{
					netPlayers[i] = lp->dpId;
					break;
				}

			if( i==MAX_MULTIPLAYERS )
				return;

			frog[i]->action.healthPoints = 3;
			player[i].lives = 3;

			RefreshMPFrogs( );

			// get pointer to player name
			if(lp->dpnName.lpszShortNameA)
				lpszPlayerName = lp->dpnName.lpszShortNameA;
			else
				lpszPlayerName = "unknown";

			// allocate space for string
			lpszStr = (LPSTR)GlobalAllocPtr(GHND,lstrlen(szDisplayFormat) + lstrlen(lpszPlayerName) + 1);
			if(lpszStr == NULL)
				break;

			// build string
			wsprintf(lpszStr,szDisplayFormat,lpszPlayerName);
		}
		break;

		case DPSYS_DESTROYPLAYERORGROUP:
		{
			LPDPMSG_DESTROYPLAYERORGROUP lp = (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
			LPSTR lpszPlayerName;
			LPSTR szDisplayFormat = "\"%s\" has left";

			for( i=1; i<MAX_MULTIPLAYERS; i++ )
				if( netPlayers[i] == lp->dpId )
				{
					netPlayers[i] = -1;
					break;
				}

			if( i==MAX_MULTIPLAYERS )
				return;

			RefreshMPFrogs( );

			// get pointer to player name
			if(lp->dpnName.lpszShortNameA)
				lpszPlayerName = lp->dpnName.lpszShortNameA;
			else
				lpszPlayerName = "unknown";

			// allocate space for string
			lpszStr = (LPSTR)GlobalAllocPtr(GHND,lstrlen(szDisplayFormat) + lstrlen(lpszPlayerName) + 1);
			if(lpszStr == NULL)
				break;

			// build string
			wsprintf(lpszStr,szDisplayFormat,lpszPlayerName);
		}
		break;

		case DPSYS_ADDPLAYERTOGROUP:
		{
			LPDPMSG_ADDPLAYERTOGROUP lp = (LPDPMSG_ADDPLAYERTOGROUP)lpMsg;
		}
		break;

		case DPSYS_DELETEPLAYERFROMGROUP:
		{
			LPDPMSG_DELETEPLAYERFROMGROUP lp = (LPDPMSG_DELETEPLAYERFROMGROUP)lpMsg;
		}
		break;

		case DPSYS_SESSIONLOST:
		{
			LPDPMSG_SESSIONLOST lp = (LPDPMSG_SESSIONLOST)lpMsg;
		}
		break;

		case DPSYS_HOST:
		{
			LPDPMSG_HOST lp = (LPDPMSG_HOST)lpMsg;
			LPSTR szDisplayFormat = "You have become the host";

			// allocate space for string
			lpszStr = (LPSTR)GlobalAllocPtr(GHND,lstrlen(szDisplayFormat) + 1);
			if(lpszStr == NULL)
				break;

			// build string
			lstrcpy(lpszStr,szDisplayFormat);

			// we are now the host
			lpDPInfo->bIsHost = TRUE;
		}
		break;

		case DPSYS_SETPLAYERORGROUPDATA:
		{
			LPDPMSG_SETPLAYERORGROUPDATA lp = (LPDPMSG_SETPLAYERORGROUPDATA)lpMsg;
		}
		break;

		case DPSYS_SETPLAYERORGROUPNAME:
		{
			LPDPMSG_SETPLAYERORGROUPNAME lp = (LPDPMSG_SETPLAYERORGROUPNAME)lpMsg;
		}
		break;
	}

	if( lpszStr )
		AddChatMessage( lpszStr );
}


/*	--------------------------------------------------------------------------------
	Function		: GetDirectPlayErrStr
	Purpose			: returns a string description of a DPlay error
	Parameters		: HRESULT
	Returns			: char *
	Info			: 
*/
char *GetDirectPlayErrStr(HRESULT hRes)
{
	static char szTempStr[12];

	switch(hRes)
	{
		case DP_OK: return ("DP_OK");
		case DPERR_ALREADYINITIALIZED: return ("DPERR_ALREADYINITIALIZED");
		case DPERR_ACCESSDENIED: return ("DPERR_ACCESSDENIED");
		case DPERR_ACTIVEPLAYERS: return ("DPERR_ACTIVEPLAYERS");
		case DPERR_BUFFERTOOSMALL: return ("DPERR_BUFFERTOOSMALL");
		case DPERR_CANTADDPLAYER: return ("DPERR_CANTADDPLAYER");
		case DPERR_CANTCREATEGROUP: return ("DPERR_CANTCREATEGROUP");
		case DPERR_CANTCREATEPLAYER: return ("DPERR_CANTCREATEPLAYER");
		case DPERR_CANTCREATESESSION: return ("DPERR_CANTCREATESESSION");
		case DPERR_CAPSNOTAVAILABLEYET: return ("DPERR_CAPSNOTAVAILABLEYET");
		case DPERR_EXCEPTION: return ("DPERR_EXCEPTION");
		case DPERR_GENERIC: return ("DPERR_GENERIC");
		case DPERR_INVALIDFLAGS: return ("DPERR_INVALIDFLAGS");
		case DPERR_INVALIDOBJECT: return ("DPERR_INVALIDOBJECT");
	//	case DPERR_INVALIDPARAM: return ("DPERR_INVALIDPARAM");	 dup value
		case DPERR_INVALIDPARAMS: return ("DPERR_INVALIDPARAMS");
		case DPERR_INVALIDPLAYER: return ("DPERR_INVALIDPLAYER");
		case DPERR_INVALIDGROUP: return ("DPERR_INVALIDGROUP");
		case DPERR_NOCAPS: return ("DPERR_NOCAPS");
		case DPERR_NOCONNECTION: return ("DPERR_NOCONNECTION");
	//	case DPERR_NOMEMORY: return ("DPERR_NOMEMORY");		dup value
		case DPERR_OUTOFMEMORY: return ("DPERR_OUTOFMEMORY");
		case DPERR_NOMESSAGES: return ("DPERR_NOMESSAGES");
		case DPERR_NONAMESERVERFOUND: return ("DPERR_NONAMESERVERFOUND");
		case DPERR_NOPLAYERS: return ("DPERR_NOPLAYERS");
		case DPERR_NOSESSIONS: return ("DPERR_NOSESSIONS");
		case DPERR_PENDING: return ("DPERR_PENDING");
		case DPERR_SENDTOOBIG: return ("DPERR_SENDTOOBIG");
		case DPERR_TIMEOUT: return ("DPERR_TIMEOUT");
		case DPERR_UNAVAILABLE: return ("DPERR_UNAVAILABLE");
		case DPERR_UNSUPPORTED: return ("DPERR_UNSUPPORTED");
		case DPERR_BUSY: return ("DPERR_BUSY");
		case DPERR_USERCANCEL: return ("DPERR_USERCANCEL");
		case DPERR_NOINTERFACE: return ("DPERR_NOINTERFACE");
		case DPERR_CANNOTCREATESERVER: return ("DPERR_CANNOTCREATESERVER");
		case DPERR_PLAYERLOST: return ("DPERR_PLAYERLOST");
		case DPERR_SESSIONLOST: return ("DPERR_SESSIONLOST");
		case DPERR_UNINITIALIZED: return ("DPERR_UNINITIALIZED");
		case DPERR_NONEWPLAYERS: return ("DPERR_NONEWPLAYERS");
		case DPERR_INVALIDPASSWORD: return ("DPERR_INVALIDPASSWORD");
		case DPERR_CONNECTING: return ("DPERR_CONNECTING");
		case DPERR_CONNECTIONLOST: return ("DPERR_CONNECTIONLOST");
		case DPERR_UNKNOWNMESSAGE: return ("DPERR_UNKNOWNMESSAGE");
		case DPERR_CANCELFAILED: return ("DPERR_CANCELFAILED");
		case DPERR_INVALIDPRIORITY: return ("DPERR_INVALIDPRIORITY");
		case DPERR_NOTHANDLED: return ("DPERR_NOTHANDLED");
		case DPERR_CANCELLED: return ("DPERR_CANCELLED");
		case DPERR_ABORTED: return ("DPERR_ABORTED");
		case DPERR_BUFFERTOOLARGE: return ("DPERR_BUFFERTOOLARGE");
		case DPERR_CANTCREATEPROCESS: return ("DPERR_CANTCREATEPROCESS");
		case DPERR_APPNOTSTARTED: return ("DPERR_APPNOTSTARTED");
		case DPERR_INVALIDINTERFACE: return ("DPERR_INVALIDINTERFACE");
		case DPERR_NOSERVICEPROVIDER: return ("DPERR_NOSERVICEPROVIDER");
		case DPERR_UNKNOWNAPPLICATION: return ("DPERR_UNKNOWNAPPLICATION");
		case DPERR_NOTLOBBIED: return ("DPERR_NOTLOBBIED");
		case DPERR_SERVICEPROVIDERLOADED: return ("DPERR_SERVICEPROVIDERLOADED");
		case DPERR_ALREADYREGISTERED: return ("DPERR_ALREADYREGISTERED");
		case DPERR_NOTREGISTERED: return ("DPERR_NOTREGISTERED");
		case DPERR_AUTHENTICATIONFAILED: return ("DPERR_AUTHENTICATIONFAILED");
		case DPERR_CANTLOADSSPI: return ("DPERR_CANTLOADSSPI");
		case DPERR_ENCRYPTIONFAILED: return ("DPERR_ENCRYPTIONFAILED");
		case DPERR_SIGNFAILED: return ("DPERR_SIGNFAILED");
		case DPERR_CANTLOADSECURITYPACKAGE: return ("DPERR_CANTLOADSECURITYPACKAGE");
		case DPERR_ENCRYPTIONNOTSUPPORTED: return ("DPERR_ENCRYPTIONNOTSUPPORTED");
		case DPERR_CANTLOADCAPI: return ("DPERR_CANTLOADCAPI");
		case DPERR_NOTLOGGEDIN: return ("DPERR_NOTLOGGEDIN");
		case DPERR_LOGONDENIED: return ("DPERR_LOGONDENIED");
	}

	// For errors not in the list, return HRESULT string
	wsprintf(szTempStr,"0x%08X",hRes);
	return szTempStr;
}


/*	--------------------------------------------------------------------------------
	Function		: ConnectUsingLobby
	Purpose			: connect for multiplay using lobby
	Parameters		: LPDPLAYINFO
	Returns			: HRESULT
	Info			: 
*/
HRESULT ConnectUsingLobby(LPDPLAYINFO lpDPInfo)
{
	LPDIRECTPLAY4A lpDirectPlay4A			= NULL;
	LPDIRECTPLAYLOBBY3A	lpDPlayLobby3A		= NULL;
	LPDPLCONNECTION	lpConnectionSettings	= NULL;
	DPID dpidPlayer;
	unsigned long dwSize;
	HRESULT hRes;

	// get an ANSI DirectPlayLobby interface
	hRes = CoCreateInstance(CLSID_DirectPlayLobby,NULL,CLSCTX_INPROC_SERVER,IID_IDirectPlayLobby3A,(LPVOID *)&lpDPlayLobby3A);
	if(FAILED(hRes))
		goto FAILURE;

	// get connection settings from the lobby
	// if this routine returns DPERR_NOTLOBBIED, then a lobby did not
	// launch this application and the user needs to configure the connection.

	// pass in a NULL pointer to just get the size of the connection setttings
	hRes = lpDPlayLobby3A->GetConnectionSettings(0,NULL,&dwSize);
	if(DPERR_BUFFERTOOSMALL != hRes)
		goto FAILURE;

	// allocate memory for the connection setttings
	lpConnectionSettings = (LPDPLCONNECTION)GlobalAllocPtr(GHND,dwSize);
	if(NULL == lpConnectionSettings)
	{
		hRes = DPERR_OUTOFMEMORY;
		goto FAILURE;
	}

	// get the connection settings
	hRes = lpDPlayLobby3A->GetConnectionSettings(0,lpConnectionSettings,&dwSize);
	if(FAILED(hRes))
		goto FAILURE;

	// before connecting, the game should configure the session description
	// with any settings it needs

	// set flags and max players used by the game
	lpConnectionSettings->lpSessionDesc->dwFlags = DPSESSION_MIGRATEHOST | DPSESSION_KEEPALIVE;
	lpConnectionSettings->lpSessionDesc->dwMaxPlayers = MAX_MULTIPLAYERS;

	// store the updated connection settings
	hRes = lpDPlayLobby3A->SetConnectionSettings(0,0,lpConnectionSettings);
	if(FAILED(hRes))
		goto FAILURE;

	// connect to the session - getting an ANSI IDirectPlay4A interface
	hRes = lpDPlayLobby3A->ConnectEx(0,IID_IDirectPlay4A,(LPVOID *)&lpDirectPlay4A,NULL);
	if(FAILED(hRes))
		goto FAILURE;

	// create a player with the name returned in the connection settings
	hRes = lpDirectPlay4A->CreatePlayer(&dpidPlayer,lpConnectionSettings->lpPlayerName,lpDPInfo->hPlayerEvent,NULL,0,0);
	if(FAILED(hRes))
		goto FAILURE;

	// return connection info
	lpDPInfo->lpDP4A = lpDirectPlay4A;
	lpDPInfo->dpidPlayer = dpidPlayer;
	if(lpConnectionSettings->dwFlags & DPLCONNECTION_CREATESESSION)
		lpDPInfo->bIsHost = TRUE;
	else
		lpDPInfo->bIsHost = FALSE;

	lpDirectPlay4A = NULL;	// set to NULL here so it won't release below

FAILURE:
	if(lpDirectPlay4A)
		lpDirectPlay4A->Release();

	if(lpDPlayLobby3A)
		lpDPlayLobby3A->Release();

	if(lpConnectionSettings)
		GlobalFreePtr(lpConnectionSettings);

	return hRes;
}

/*	--------------------------------------------------------------------------------
	Function		: ConnectUsingDialog
	Purpose			: connect for multiplay using dialog box
	Parameters		: HINSTANCE,LPDPLAYINFO
	Returns			: HRESULT
	Info			: 
*/
HRESULT ConnectUsingDialog(HINSTANCE hInstance,LPDPLAYINFO lpDPInfo)
{
	// ask user for connection settings 
	if(DialogBoxParam(hInstance,MAKEINTRESOURCE(IDD_CONNECTION),winInfo.hWndMain,(DLGPROC)DLGChooseServiceProvider,(LPARAM)lpDPInfo))
		return DP_OK;
	else
		return DPERR_USERCANCEL;
}

/*	--------------------------------------------------------------------------------
	Function		: ErrorBox
	Purpose			: displays a message box showing an error
	Parameters		: LPSTR,HRESULT
	Returns			: void
	Info			: 
*/
void ErrorBox(LPSTR lpszErrorStr,HRESULT hRes)
{
	char szStr[MAX_STRLEN];

	wsprintf(szStr,lpszErrorStr,GetDirectPlayErrStr(hRes));
	MessageBox(NULL,szStr,"Error",MB_OK);
}


/*	--------------------------------------------------------------------------------
	Function		: EnumPlayersCallback
	Purpose			: Called to enumerate players in current session
	Parameters		: 
	Returns			: void
	Info			: 
*/
BOOL WINAPI EnumPlayersCallback( DPID dpId, DWORD dwPlayerType, LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext )
{
	static int count = 1;

	if( dwPlayerType == DPPLAYERTYPE_PLAYER && count < MAX_MULTIPLAYERS )
		netPlayers[count++] = dpId;

	return TRUE;
}
