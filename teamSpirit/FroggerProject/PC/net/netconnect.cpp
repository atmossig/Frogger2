/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: netconnect.cpp
	Programmer	: 
	Date		:

	calls functions from Microsoft's dpconnect.cpp support file

----------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <dplay.h>
#include <dplobby.h>

#include <ddraw.h>
#include <d3d.h>
#include <mdx.h>		// because MDX is evil and all-pervasive

#include "islutil.h"
#include "main.h"
#include "../resource.h"
#include "netconnect.h"
#include "network.h"

#define MAX_PLAYER_NAME     14
#define MAX_SESSION_NAME    256

LPDIRECTPLAY4A dplay		= NULL;

DPID	dpidLocalPlayer;
HANDLE	hLocalPlayerEvent;

char	sessionName[256]	= "Frogger2";
char	playerName[32]		= "Player";

// {D0D2A940-5803-11d4-8832-00A0244E2381}
static const GUID Frogger2_GUID =
{ 0xd0d2a940, 0x5803, 0x11d4, { 0x88, 0x32, 0x0, 0xa0, 0x24, 0x4e, 0x23, 0x81 } };

// ------ Local functions ---------

bool SetupNetworking(HWND hwnd);
bool SelectProvider(HWND hwnd);
bool FindNetGame(HWND hwnd);
bool CreateNetGame(HWND hDlg);
bool CheckLobby(HWND hwnd);

/*	--------------------------------------------------------------------------------
	Function	: SetupNetworking
	Purpose		: sets up DirectPlay
	Parameters	: 
	Returns		: 
*/
bool SetupNetworking()
{
	HRESULT res;

	// init COM library
	if ((res = CoInitialize(NULL)) != S_OK)
	{
		utilPrintf("SetupNetworking(): Failed initialising COM!\n");
		return false;
	}

	if ((res = CoCreateInstance(CLSID_DirectPlay, NULL, CLSCTX_ALL,
		IID_IDirectPlay4A, (void**)&dplay)) != S_OK)
	{
		utilPrintf("SetupNetworking(): Failed creating DirectPlay4A interface\n");
		return false;
	}

	// ... set any other network-spiffic stuff up too ...

	return true;
}

/*	--------------------------------------------------------------------------------
	Function	: ShutdownNetworkGame
	Purpose		: 
	Parameters	: 
	Returns		: 
*/

void ShutdownNetworkGame()
{
	// .. shut down anything networky ..

	if (dplay) dplay->Release();

}

/*	--------------------------------------------------------------------------------
	Function	: StartNetworkGame
	Purpose		: sets up and configures a network game
	Parameters	: flags
	Returns		: success
*/
int StartNetworkGame(HWND hwnd, int flag)
{
	bool connected = false;

	if (!SetupNetworking())	// if we can't set networking up at all, fail
		return 0;

	// if the flag's set, we're definitely running networked
	if (flag)
	{
		// .. so we need to get LAN/modem/etc. settings
		
		if (SelectProvider(hwnd))
			if (FindNetGame(hwnd))
				connected = true;
	}
	else
	{
		if (CheckLobby(hwnd))
			connected = true;
	}

	if (!connected)
	{
		ShutdownNetworkGame();
		return 0;
	}
	else
		return 1;
}

/*	--------------------------------------------------------------------------------
	Function	: CheckLobby
	Purpose		: checks if we're running through a lobby and connects if we are
	Parameters	: 
	Returns		: success
*/

bool CheckLobby(HWND hwnd)
{
	return false;	// ds - not currently implemented
}

/*	--------------------------------------------------------------------------------
	Function	: SelectProvider
	Purpose		: Enumerate and select a DirectPlay provider
	Parameters	: 
	Returns		: success
*/
BOOL CALLBACK EnumConnections(LPCGUID lpguidSP, LPVOID connection, DWORD connectionSize,
							  const DPNAME* name, DWORD flags, LPVOID context)
{
	int index;
	HRESULT res;
	HWND list = (HWND)context;
	LPDIRECTPLAY4A dp;

	if ((res = CoCreateInstance(CLSID_DirectPlay, NULL, CLSCTX_ALL,
		IID_IDirectPlay4A, (void**)&dp)) != S_OK)
		return FALSE;
	
	res = dp->InitializeConnection(connection, 0);
	dp->Release();	// don't need any more..

	if (res != DP_OK) return TRUE;

	index = (int)SendMessage(list, LB_ADDSTRING, 0, (LPARAM)name->lpszShortNameA);

	BYTE* data = new BYTE[connectionSize];
	memcpy(data, connection, connectionSize);

	SendMessage(list, LB_SETITEMDATA, index, (LPARAM)data);

	return TRUE;
}

BOOL CALLBACK dlgProvider(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int count, index;

	switch (msg)
	{
	case WM_INITDIALOG:
		{
			HWND hctrl = GetDlgItem(hdlg, IDC_CONNECTIONLIST);
			dplay->EnumConnections(&Frogger2_GUID, EnumConnections, (void*)hctrl,
				DPCONNECTION_DIRECTPLAY | DPCONNECTION_DIRECTPLAYLOBBY);

			SendMessage(GetDlgItem(hdlg, IDC_CONNECTIONLIST), LB_SETCURSEL, 0, 0);

			hctrl = GetDlgItem(hdlg, IDC_PLAYERNAME);
			SetWindowText(hctrl, playerName);
			SendMessage(hctrl, EM_SETSEL, 0, -1);
		}		
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			{
				HWND hctrl = GetDlgItem(hdlg, IDC_CONNECTIONLIST);
				index = SendMessage(hctrl, LB_GETCURSEL, 0, 0);

				void *connection = (void*)SendMessage(hctrl, LB_GETITEMDATA, index, 0);

				if (CoCreateInstance(CLSID_DirectPlay, NULL, CLSCTX_ALL,
					IID_IDirectPlay4A, (void**)&dplay) == S_OK)
				{
					dplay->InitializeConnection(connection, 0);
				}

				EndDialog(hdlg, true);
				return true;
			}

		case IDCANCEL:
			EndDialog(hdlg, false);
			return true;

		case IDC_PLAYERNAME:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				int len = SendMessage((HWND)lParam, WM_GETTEXT, 32, (LPARAM)&playerName);
				EnableWindow(GetDlgItem(hdlg, IDOK), (len>0));
				return true;
			}
		}
		break;

	case WM_DESTROY:
		{
			// clean up connection info
			HWND hctrl = GetDlgItem(hdlg, IDC_CONNECTIONLIST);
			count = SendMessage(hctrl, LB_GETCOUNT, 0, 0);
			for(index=0; index<count; index++)
			{
				void *ptr = (VOID*)SendMessage(hctrl, LB_GETITEMDATA, index, 0 );
				if (ptr) delete [] ptr;
			}
		}
		return true;
	}
	return false;
}

bool SelectProvider(HWND hwnd)
{
	return DialogBox(mdxWinInfo.hInstance, MAKEINTRESOURCE(IDD_MULTI_CONNECT), hwnd, dlgProvider);
}


/*	--------------------------------------------------------------------------------
	Function	: FindNetGame
	Purpose		: Find a network game of Frogger2
	Parameters	: 
	Returns		: success
*/

BOOL CALLBACK EnumSessions(const DPSESSIONDESC2 *lpdpsd, LPDWORD lptimeout, DWORD flags, LPVOID context)
{
	if (flags & DPESC_TIMEDOUT)
		return FALSE;

	HWND hlist = (HWND)context;
	int item = SendMessage(hlist, LB_ADDSTRING, 0, (LPARAM)lpdpsd->lpszSessionNameA);

	return TRUE;
}

bool ShowNetGames(HWND hdlg, bool start)
{
	DPSESSIONDESC2 dpsd;
	HWND hlist = GetDlgItem(hdlg, IDC_GAMELIST);

	if (start)
		SendMessage(hlist, LB_RESETCONTENT, 0, 0);

	ZeroMemory(&dpsd, sizeof(DPSESSIONDESC2));
	dpsd.dwSize = sizeof(DPSESSIONDESC2);
	dpsd.guidApplication = Frogger2_GUID;

	dplay->EnumSessions(&dpsd, 0, &EnumSessions, GetDlgItem(hdlg, IDC_GAMELIST),
		(start)?DPENUMSESSIONS_ASYNC|DPENUMSESSIONS_ALL:DPENUMSESSIONS_STOPASYNC);

	return true;
}

BOOL CALLBACK dlgSession( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    HRESULT hr;
	static searching;

    switch( msg )
    {
        case WM_INITDIALOG:
            {
                searching = FALSE;
                SetDlgItemText( hDlg, IDC_SEARCH, "Start Search" );
//				DPConnect_SessionsDlgInitListbox( hDlg );
            }
            break;

        case WM_TIMER:
			ShowNetGames(hDlg, true);
            break;

        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
                case IDC_SEARCH:
                    searching = !searching;


                    if (searching)
                    {
                        // Start the timer, and start the async enumeratation
                        SetDlgItemText( hDlg, IDC_SEARCH, "Searching..." );
                        SetTimer( hDlg, 1, 250, NULL );

						ShowNetGames(hDlg, true);
                    }
                    else
                    {
                        // Stop the timer, and stop the async enumeration
                        KillTimer( hDlg, 1 );
						ShowNetGames(hDlg, false);
                        SetDlgItemText( hDlg, IDC_SEARCH, "Start Search" );
                    }
                    break;


                case IDC_GAMELIST:
                    if( HIWORD(wParam) != LBN_DBLCLK )
                        break;
                    // Fall through

                case IDC_JOIN:
/*                    if( FAILED( hr = DPConnect_SessionsDlgJoinGame( hDlg ) ) )
                    {
                        MessageBox( hDlg, TEXT("Unable to join game."),
                                    TEXT("DirectPlay Sample"), 
                                    MB_OK | MB_ICONERROR );
                    }    
*/					
                    break;

                case IDC_CREATE:
					CreateNetGame(hDlg);
                    break;

                case IDCANCEL: // The close button was press
                    EndDialog(hDlg, -1);
                    break;
/*
                case IDC_BACK: // "Cancel" button was pressed
                    EndDialog(hDlg, 1);
                    break;
*/
                default:
                    return FALSE; // Message not handled
            }
            break;
        
        case WM_DESTROY:
            KillTimer(hDlg, 1);
            //DPConnect_SessionsDlgCleanup();
            break;

        default:
            return FALSE; // Message not handled
    }

    // Message was handled
    return TRUE; 
}


bool FindNetGame(HWND hwnd)
{
	return (DialogBox(mdxWinInfo.hInstance, MAKEINTRESOURCE(IDD_MULTI_GAMES), hwnd, dlgSession) == 0);
}

/*	--------------------------------------------------------------------------------
	Function	: CreateNetGame
	Purpose		: Creates a network game of Frogger2
	Parameters	: 
	Returns		: success
*/


BOOL CALLBACK dlgCreate(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int len;

	switch (msg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_NAME, "Frogger2");
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			if ((len = GetDlgItemText(hDlg, IDC_NAME, sessionName, 256)) > 0)
				EndDialog(hDlg, 0);
			break;

		case IDCANCEL:
			EndDialog(hDlg, -1);
			break;

		default:
			return FALSE;
		}
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

bool CreateNetGame(HWND hwnd)
{
    DPSESSIONDESC2	dpsd;
    DPNAME			dpname;
	HRESULT			res;

	if (DialogBox(mdxWinInfo.hInstance, MAKEINTRESOURCE(IDD_MULTI_CREATE), hwnd, dlgCreate) != 0)
		return false;

    ZeroMemory( &dpsd, sizeof(dpsd) );
    dpsd.dwSize           = sizeof(dpsd);
    dpsd.guidApplication  = Frogger2_GUID;
    dpsd.lpszSessionNameA = sessionName;
    dpsd.dwMaxPlayers     = 4;
    dpsd.dwFlags          = DPSESSION_KEEPALIVE|DPSESSION_MIGRATEHOST|DPSESSION_DIRECTPLAYPROTOCOL;

	if (dplay->Open(&dpsd, DPOPEN_CREATE) != DP_OK)
	{
		utilPrintf("CreateNetGame(): dplay->Open() failed\n");
		return false;
	}

	ZeroMemory(&dpname, sizeof(dpname));
	dpname.dwSize			= sizeof(dpname);
	dpname.lpszShortNameA	= playerName;

	if (dplay->CreatePlayer(&dpidLocalPlayer, &dpname, 0, NULL, 0, DPPLAYER_SERVERPLAYER) != DP_OK)
	{
		dplay->Close();
		utilPrintf("CreateNetGame(): Failed creating player\n");
		return false;
	}

	return true;
}