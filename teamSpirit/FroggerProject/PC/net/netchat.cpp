/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: netchat.cpp
	Programmer	: David Swift
	Date		: 

	Does the chatting and level select bit at the start of the game

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
#include "netgame.h"

// common files
#include "layout.h"
#include "lang.h"
#include "frogger.h"

COLORREF systemColor = RGB(0,0x80,0);	// green
COLORREF errorColor = RGB(0xFF,0,0);		// red
HWND hwndChat, hwndChatEdit;

#define NUM_MULTI_LEVELS 10

struct MULTILEVEL { int world, level; };

MULTILEVEL multiLevels[NUM_MULTI_LEVELS] = {
	{ WORLDID_TEST, 0 },
	{ WORLDID_TEST, 1 },
	{ WORLDID_TEST, 2 },
	{ WORLDID_TEST, 3 },
	{ WORLDID_TEST, 4 },
	{ WORLDID_TEST, 5 },
	{ WORLDID_GARDEN, LEVELID_GARDEN1 },
	{ WORLDID_GARDEN, LEVELID_GARDEN1 },
	{ WORLDID_GARDEN, LEVELID_GARDEN1 },
	{ WORLDID_GARDEN, LEVELID_GARDEN1 },
};


struct	MSG_GAMESETUP
{
	UBYTE appmsg_gamesetup;
	UBYTE characters[4];
};


int ChatHandler(void *data, unsigned long size, NETPLAYER *player);
void ShowMessage(const char* string, CHAT_FORMAT f);

/*
void StartPlayers(HWND hDlg)
{
	int s, pl, dpid;
	int start[4] = { -1, -1, -1, -1 };

	struct {
		unsigned char msg;
		NETGAME_STARTUP info;
	} msg;

	for (s=0; s<4; s++)
	{
		dpid = 0xFFFFFFFF;

		for (pl=0; pl<4; pl++)
		{
			if (start[pl] < 0 && netPlayerList[pl].dpid && netPlayerList[pl].dpid < dpid)
			{
				start[pl] = s;
				dpid = netPlayerList[pl].dpid;
			}
		}
	}

	int level;
	HWND hctrl = GetDlgItem(hDlg, IDC_LEVEL);

	level = SendMessage(hctrl, CB_GETCURSEL, 0, 0);
	level = SendMessage(hctrl, CB_GETITEMDATA, level, 0);

	if (level<0 || level>=NUM_MULTI_LEVELS)
		level = 0;

	msg.msg = APPMSG_START;
	msg.info.level = multiLevels[level].level;
	msg.info.level = multiLevels[level].world;

	for (pl=0; pl<4 && start[pl]>=0; pl++)
	{
		msg.info.character = pl;
		msg.


//		UBYTE data[2] = { APPMSG_PLAYERNUM, start[pl] };
//		dplay->Send(dpidLocalPlayer, netPlayerList[pl].dpid, DPSEND_GUARANTEED, data, 2);
	}
}
*/

// -----------------------------------------------------------------------------------------


bool SetupChatDialog(HWND hdlg)
{
	int i;
	hwndChatEdit = GetDlgItem(hdlg, IDC_RICHCHAT);
	hwndChat = hdlg;

	ShowMessage(isHost?"I'm the server!":"Joined a game!", CHAT_SYSTEM);

	HWND hList = GetDlgItem(hdlg, IDC_LEVEL);

	for (i=0; i<NUM_MULTI_LEVELS; i++)
	{
		LEVEL_VISUAL_DATA *data;
		char *levelname;
		int index;

		// get level names from worldvisualdata

		data = &worldVisualData[multiLevels[i].world].levelVisualData[multiLevels[i].level];

		if (data->levelOpen)
		{
			levelname = GAMESTRING(data->description_str);
			index = SendMessage(hList, CB_ADDSTRING, 0, (DWORD)levelname);
			SendMessage(hList, CB_SETITEMDATA, index, i);
		}
	}

	SendMessage(hList, CB_SETCURSEL, 0, 0);

	// Timer 1 is used for checking the player list and footling through the message buffers
	SetTimer(hdlg, 1, 250, NULL);

	return true;
}

void ShowMessage(const char* string, CHAT_FORMAT f)
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

void UpdateChatWindow()
{
	int pl, count = 0;
	HWND hlist = GetDlgItem(hwndChat, IDC_PLAYERLIST);

	SendMessage(hlist, LB_RESETCONTENT, 0, 0);

	for (pl=0; pl<4; pl++)
	{
		if (netPlayerList[pl].dpid)
		{
			char name[256];
			DWORD size = 256;

			if (DP_OK == dplay->GetPlayerName(netPlayerList[pl].dpid, &name, &size));
				SendMessage(hlist, LB_ADDSTRING, 0, (DWORD)((DPNAME*)name)->lpszShortNameA);

			count++;
		}
	}

	EnableWindow(GetDlgItem(hwndChat, IDC_LEVEL), isHost);
	EnableWindow(GetDlgItem(hwndChat, IDC_START), isHost); // && count>=2);
}


BOOL CALLBACK dialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		SetupChatDialog(hDlg);
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

				ShowMessage(buffer, CHAT_NORMAL);

				*(int*)buffer = APPMSG_CHAT;
				strncpy(buffer+4, text, 1020);

				dplay->Send(dpidLocalPlayer, DPID_ALLPLAYERS, DPSEND_GUARANTEED, buffer, strlen(text)+4+1);
				return TRUE;
			}

		case IDCANCEL:
			dplay->Close();
			EndDialog(hDlg, 1);
			return TRUE;

		case IDC_START:
			if (isHost) {
				// ... drumroll please ..
				utilPrintf("NET: starting the game... drumroll please ...\n");

				DPSESSIONDESC2 dpsd;
				ZeroMemory(&dpsd, sizeof(dpsd));
				dpsd.dwSize = sizeof(dpsd);
				dpsd.dwFlags = DPSESSION_NEWPLAYERSDISABLED|DPSESSION_JOINDISABLED;

				dplay->SetSessionDesc(&dpsd, 0);

				unsigned char msg = APPMSG_START;

				NetBroadcastUrgentMessage(&msg, 1);

				EndDialog(hDlg, 0);
				return TRUE;
			}
		}
		break;

	case WM_TIMER:
		NetProcessMessages();
		UpdateChatWindow();
		return TRUE;

	case WM_DESTROY:
		KillTimer(hDlg, 1);
		return TRUE;
	}

	return FALSE;
}

void NetShowMessage(const char* str, CHAT_FORMAT fmt)
{
	ShowMessage(str, fmt);
}

int RunNetChatWindow(HWND parent)
{
	int res;
	LoadLibrary("Riched32.dll");

	NetInstallMessageHandler(ChatHandler);
	res = DialogBox(mdxWinInfo.hInstance, MAKEINTRESOURCE(IDD_MULTI_START), parent, dialogProc);
	return res == 0;
}

int ChatHandler(void *data, unsigned long size, NETPLAYER *player)
{
	switch (*(unsigned char*)data)
	{
	case APPMSG_CHAT:
		{
			char buffer[1024], name[256];
			DWORD size = 256;

			dplay->GetPlayerName(player->dpid, &name, &size);
			
			strcpy(buffer, "<");
			strcat(buffer, ((DPNAME*)name)->lpszShortNameA);
			strcat(buffer, "> ");
			strncat(buffer, ((char*)data) + 4, 1024);

			ShowMessage(buffer, CHAT_NORMAL);
		}
		return 0;

	case APPMSG_START:
		{
			utilPrintf("NET: starting the joined game... drumroll please ...\n");
			EndDialog(hwndChat, 0);
		}
		return 0;
	}

	return 1;
}
