/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: netchat.cpp
	Programmer	: David Swift
	Date		: 

----------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <dplay.h>
#include <dplobby.h>
#include <richedit.h>

#include <ddraw.h>
#include <d3d.h>
#include <mdx.h>		// because MDX is evil and all-pervasive

#include "islutil.h"
#include "main.h"
#include "../resource.h"

#include "netchat.h"
#include "network.h"

COLORREF systemColor = RGB(0,0x80,0);	// green
COLORREF errorColor = RGB(0xFF,0,0);		// red
HWND hwndChatEdit;

int ChatHandler(int type, void *data, unsigned long size, NETPLAYER *player);

class ChatWindow
{
protected:
	HWND hwndDlg, hwndChatEdit;
	static BOOL CALLBACK dialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

	void UpdatePlayerList();

public:
	bool Run(HWND);
	~ChatWindow();

	void ShowMessage(const char* message, CHAT_FORMAT format);
};

ChatWindow *netChat;

void ChatWindow::ShowMessage(const char* string, CHAT_FORMAT f)
{
	if (!hwndChatEdit) return;

	CHARFORMAT fmt;
	fmt.cbSize = sizeof(fmt);
	fmt.dwMask = CFM_COLOR|CFM_BOLD;
	fmt.dwEffects = 0;
	
	switch (f)
	{
	case CHAT_NORMAL:	fmt.dwEffects |= CFE_AUTOCOLOR; break;
	case CHAT_SYSTEM:	fmt.dwEffects |= CFE_BOLD; fmt.crTextColor = systemColor; break;
	case CHAT_ERROR:	fmt.dwEffects |= CFE_BOLD; fmt.crTextColor = errorColor; break;
	}

	SendMessage(hwndChatEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&fmt);
	SendMessage(hwndChatEdit, EM_REPLACESEL, FALSE, (DWORD)string);
	SendMessage(hwndChatEdit, EM_REPLACESEL, FALSE, (DWORD)"\n");
}

void ChatWindow::UpdatePlayerList()
{
	int pl;
	HWND hlist = GetDlgItem(hwndDlg, IDC_PLAYERLIST);

	SendMessage(hlist, LB_RESETCONTENT, 0, 0);

	for (pl=0; pl<4; pl++)
	{
		if (netPlayerList[pl].player != -1)
		{
			char name[256];
			DWORD size = 256;

			if (DP_OK == dplay->GetPlayerName(netPlayerList[pl].dpid, &name, &size));
				SendMessage(hlist, LB_ADDSTRING, 0, (DWORD)((DPNAME*)name)->lpszShortNameA);
		}
	}
}


BOOL CALLBACK ChatWindow::dialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static ChatWindow *cw;

	switch (msg)
	{
	case WM_INITDIALOG:
		cw = (ChatWindow*)lParam;
		cw->hwndChatEdit = GetDlgItem(hDlg, IDC_RICHCHAT);
		cw->hwndDlg = hDlg;

		//SendDlgItemMessage(hDlg, IDC_PLAYERLIST, LB_ADDSTRING, 0, (DWORD)playerName);
		//SendDlgItemMessage(hDlg, IDC_RICHCHAT, EM_SETBKGNDCOLOR, 0, RGB(0xFF,0xFF,0xA0));
		
		cw->ShowMessage(isServer?"I'm the server!":"Joined a game!", CHAT_SYSTEM);

		SetTimer(hDlg, 1, 250, NULL);

		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_SEND:
			{
				char buffer[1024], text[1024];
				HWND hEdit = GetDlgItem(hDlg, IDC_CHATENTER);

				if (!GetWindowText(hEdit, text, 1024))
					break;

				SetWindowText(hEdit, "");
				strcpy(buffer, "<");
				strcat(buffer, playerName);
				strcat(buffer, "> ");
				strncat(buffer, text, 1024);

				netChat->ShowMessage(buffer, CHAT_NORMAL);

				*(int*)buffer = APPMSG_CHAT;
				strncpy(buffer+4, text, 1020);

				dplay->Send(dpidLocalPlayer, DPID_ALLPLAYERS, DPSEND_GUARANTEED, buffer, strlen(text)+4+1);
				return TRUE;
			}

		case IDCANCEL:
			dplay->Close();
			EndDialog(hDlg, 1);
			return TRUE;
		}
		break;

	case WM_TIMER:
		NetProcessMessages();
		
		netChat->UpdatePlayerList();
		return TRUE;

	case WM_DESTROY:
		KillTimer(hDlg, 1);
		return TRUE;
	}

	return FALSE;
}


bool ChatWindow::Run(HWND parent)
{
	int res;
	LoadLibrary("Riched32.dll");
	
	res = DialogBoxParam(mdxWinInfo.hInstance, MAKEINTRESOURCE(IDD_MULTI_START), parent, dialogProc, (LPARAM)this);

	return (res == 0);
}



ChatWindow::~ChatWindow()
{

}


void NetShowMessage(const char* str, CHAT_FORMAT fmt)
{
	if (netChat)
		netChat->ShowMessage(str, fmt);
}

void RunNetChatWindow(HWND parent)
{
	NetInstallMessageHandler(ChatHandler);
	netChat = new ChatWindow();
	netChat->Run(parent);

	delete netChat;	netChat = 0;
}

int ChatHandler(int type, void *data, unsigned long size, NETPLAYER *player)
{
	if (netChat)
	{
		switch (type)
		{
		case APPMSG_CHAT:
			{
				char buffer[1024], name[256];
				DWORD size = 256;

				dplay->GetPlayerName(player->dpid, &name, &size);
				
				strcpy(buffer, "<");
				strcat(buffer, ((DPNAME*)name)->lpszShortNameA);
				strcat(buffer, "> ");
				strncat(buffer, (char*)data, 1024);

				netChat->ShowMessage(buffer, CHAT_NORMAL);
			}
			return 0;
		}
	}

	return 1;
}
