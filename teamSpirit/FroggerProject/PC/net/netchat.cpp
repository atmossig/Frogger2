
#include <windows.h>
#include <math.h>
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

#include <islutil.h>
#include <islpad.h>

#include <anim.h>
#include <stdio.h>

#include "game.h"
#include "types2d.h"
#include "frogger.h"
#include "levplay.h"
#include "frogmove.h"
#include "cam.h"
#include "tongue.h"
#include "enemies.h"
#include "babyfrog.h"
#include "hud.h"
#include "frontend.h"
#include "textover.h"
#include "multi.h"
#include "layout.h"
#include "platform.h"
#include "event.h"
#include "main.h"
#include "newpsx.h"
#include "Main.h"
#include "actor2.h"
#include "bbtimer.h"
#include "maths.h"
#include "..\resource.h"
}


CHATBUFFER chatBuffer;
unsigned char chatFlags = 0;
CHATSTRING chatInput;

/*	--------------------------------------------------------------------------------
	Function		: HandleChatMessage
	Purpose			: receives a string and adds it to the buffer
	Parameters		: 
	Returns			: 
	Info			: 
*/
void HandleChatMessage( LPDPLAYINFO lpDPInfo,LPMSG_CHATSTRING lpMsg,DWORD dwMsgSize,DPID idFrom,DPID idTo )
{
	LPSTR lpszStr = NULL;
	HRESULT hRes;

	hRes = NewChatMessage(lpDPInfo->lpDP4A,idFrom,lpMsg->szMsg,&lpszStr);
	if(FAILED(hRes))
		return;

	AddChatMessage( lpszStr );
}


/*	--------------------------------------------------------------------------------
	Function		: NewChatMessage
	Purpose			: creates a new chat string for posting
	Parameters		: LPDIRECTPLAY4A,DPID,LPSTR,LPSTR *
	Returns			: HRESULT
	Info			: 
*/
HRESULT NewChatMessage(LPDIRECTPLAY4A lpDirectPlay4A,DPID dpidPlayer,LPSTR lpszMsg,LPSTR *lplpszStr)
{
	LPDPNAME lpName = NULL;
	LPSTR lpszStr = NULL;
	LPSTR lpszPlayerName;
	LPSTR szDisplayFormat = "%s:  %s";
	HRESULT	hRes;
	
	// get name of player
	hRes = GetChatPlayerName(lpDirectPlay4A,dpidPlayer,&lpName);
	if(FAILED(hRes))
		goto FAILURE;

	if(lpName->lpszShortNameA)
		lpszPlayerName = lpName->lpszShortNameA;
	else
		lpszPlayerName = "unknown";

	// allocate space for display string
	lpszStr = (LPSTR)GlobalAllocPtr(GHND,lstrlen(szDisplayFormat) + lstrlen(lpszPlayerName) + lstrlen(lpszMsg) + 1);
	if(lpszStr == NULL)
	{
		hRes = DPERR_OUTOFMEMORY;
		goto FAILURE;
	}

	// build string
	wsprintf(lpszStr,szDisplayFormat,lpszPlayerName,lpszMsg);

	*lplpszStr = lpszStr;
	lpszStr = NULL;

FAILURE:
	if(lpszStr)
		GlobalFreePtr(lpszStr);
	
	if(lpName)
		GlobalFreePtr(lpName);

	return hRes;
}


/*	--------------------------------------------------------------------------------
	Function		: GetChatPlayerName
	Purpose			: gets player name
	Parameters		: LPDIRECTPLAY4A,DPID,LPDPNAME *
	Returns			: HRESULT
	Info			: 
*/
HRESULT GetChatPlayerName(LPDIRECTPLAY4A lpDirectPlay4A,DPID dpidPlayer,LPDPNAME *lplpName)
{
	LPDPNAME lpName = NULL;
	unsigned long dwNameSize;
	HRESULT	hRes;

	// get size of player name data
	hRes = lpDirectPlay4A->GetPlayerName(dpidPlayer,NULL,&dwNameSize);
	if(hRes != DPERR_BUFFERTOOSMALL)
		goto FAILURE;

	// make room for it
	lpName = (LPDPNAME)GlobalAllocPtr(GHND,dwNameSize);
	if(lpName == NULL)
	{
		hRes = DPERR_OUTOFMEMORY;
		goto FAILURE;
	}

	// get player name data
	hRes = lpDirectPlay4A->GetPlayerName(dpidPlayer,lpName,&dwNameSize);
	if(FAILED(hRes))
		goto FAILURE;

	// return pointer to name structure
	*lplpName = lpName;

	return DP_OK;

FAILURE:
	if(lpName)
		GlobalFreePtr(lpName);

	return hRes;
}


/*	--------------------------------------------------------------------------------
	Function		: SendChatMessage
	Purpose			: sends a chat message
	Parameters		: HWND,LPDPLAYINFO
	Returns			: 
	Info			: 
*/
HRESULT SendChatMessage(LPSTR string, LONG lStrLen )
{
	LPSTR lpszChatStr = NULL;
	LPSTR lpszStr = NULL;
	LPMSG_CHATSTRING lpChatMessage = NULL;
	DWORD dwChatMessageSize;
	HRESULT	hRes;
	
	if( !string || lStrLen <= 0 || lStrLen > MAX_CSLENGTH || !DPInfo.lpDP4A )
	{
		hRes = DPERR_ABORTED;
		goto FAILURE;
	}

	// make room for it
	lpszChatStr = (LPSTR)GlobalAllocPtr(GHND,lStrLen);
	if(lpszChatStr == NULL)
	{
		hRes = DPERR_OUTOFMEMORY;
		goto FAILURE;
	}

	// get item text
	memcpy(lpszChatStr,string,lStrLen);

	// create string to display this text
	hRes = NewChatMessage( DPInfo.lpDP4A, DPInfo.dpidPlayer, lpszChatStr, &lpszStr );
	if(FAILED(hRes))
		goto FAILURE;

	AddChatMessage( lpszStr );

	// create space for message plus string (string length included in message header)
	dwChatMessageSize = sizeof(MSG_CHATSTRING) + lStrLen;
	lpChatMessage = (LPMSG_CHATSTRING)GlobalAllocPtr(GHND,dwChatMessageSize);
	if(lpChatMessage == NULL)
	{
		hRes = DPERR_OUTOFMEMORY;
		goto FAILURE;
	}

	// build message	
	lpChatMessage->dwType = APPMSG_GAMECHAT;
	memcpy(lpChatMessage->szMsg,lpszChatStr,lStrLen);
	
	// send this string to all other players
	hRes = DPInfo.lpDP4A->Send(DPInfo.dpidPlayer,DPID_ALLPLAYERS,DPSEND_GUARANTEED,lpChatMessage,dwChatMessageSize);
	if(FAILED(hRes))
		goto FAILURE;

FAILURE:
	if(lpszChatStr)
		GlobalFreePtr(lpszChatStr);

	if(lpszStr)
		GlobalFreePtr(lpszStr);

	if(lpChatMessage)
		GlobalFreePtr(lpChatMessage);

	return hRes;
}


/*	--------------------------------------------------------------------------------
	Function		: InitChatBuffer
	Purpose			: Initialise the buffer 
	Parameters		: Max number of chat entries
	Returns			: 
	Info			: 
*/
void InitChatBuffer( )
{
	chatBuffer.numEntries = 0;
	chatBuffer.maxEntries = 1;

	chatBuffer.head.next = NULL;
	chatBuffer.head.msg = NULL;
	chatBuffer.head.msgLen = 0;

	chatInput.next = NULL;
	chatInput.msg = NULL;
	chatInput.msgLen = 0;
}


/*	--------------------------------------------------------------------------------
	Function		: FreeChatBuffer
	Purpose			: Delete the list
	Parameters		: 
	Returns			: 
	Info			: 
*/
void FreeChatBuffer( )
{
	CHATSTRING *cs = chatBuffer.head.next;

	// Free all chatstrings by repeatedly removing the head
	while( cs )
	{
		chatBuffer.head.next = cs->next;
		cs->next = NULL;
		if( cs->msg )
			delete cs->msg;

		delete cs;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: AddChatMessage
	Purpose			: Put a new chat message into the list
	Parameters		: Pointer to string
	Returns			: 
	Info			: 
*/
void AddChatMessage( char *msg )
{
	int len;
	CHATSTRING *cstring;

	if( !msg )
		return;

	len = lstrlen( msg );

	if( len > MAX_CSLENGTH )
		len = MAX_CSLENGTH;

	// Alloc new chatstring and fill it with message
	cstring = new CHATSTRING;
	cstring->msgLen = len;
	cstring->msg = new char[len];

	memcpy( cstring->msg, msg, len );

	// Add new chatstring to head of list
	cstring->next = chatBuffer.head.next;
	chatBuffer.head.next = cstring;
	chatBuffer.numEntries++;

	TruncateChatBuffer( );
}


/*	--------------------------------------------------------------------------------
	Function		: DrawChatBuffer
	Purpose			: Print all chat messages
	Parameters		: screen coords of top left and bottom right
	Returns			: 
	Info			: 
*/
void DrawChatBuffer( int left, int top, int right, int bottom )
{
	CHATSTRING *cstring;
	RECT r, r2;
	HRESULT res;
	HDC hdc;
	int count, ht, num;
	
	r.left = left;
	r.top = top;
	r.right = right;
	r.bottom = bottom;

	DrawFlatRect(r, D3DRGBA(0.3, 0.3, 0.5, 0.6));

	if( chatFlags & CHAT_INPUT )
	{
		r2 = r;
		r2.top = r2.bottom+10;
		r2.bottom = r2.top+20;
		DrawFlatRect(r2, D3DRGBA(0.3, 0.3, 0.5, 0.6));
	}

	ht = bottom - top;
	num = ht / 20;
	if( num > 1 )
		chatBuffer.maxEntries = num;
	else
		chatBuffer.maxEntries = 1;

	TruncateChatBuffer( );

	res = IDirectDrawSurface4_GetDC(surface[RENDER_SRF], &hdc);
	if (res == DD_OK)
	{
		SetTextAlign( hdc, TA_LEFT );
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(255,255,255));

		for( count=0,cstring = chatBuffer.head.next; cstring; cstring = cstring->next,count++ )
			TextOut( hdc, r.left+5, r.bottom-5-(20*(count+1)), cstring->msg, cstring->msgLen );

		// Draw chat string being typed
		if( chatFlags & CHAT_INPUT )
			TextOut( hdc, r2.left+5, r2.top+5, chatInput.msg, chatInput.msgLen );

		IDirectDrawSurface4_ReleaseDC(surface[RENDER_SRF], hdc);
	}
}

/*	--------------------------------------------------------------------------------
	Function		: TruncateChatBuffer
	Purpose			: Limit number of chat strings to fit in chat window
	Parameters		: screen coords of top left and bottom right
	Returns			: 
	Info			: 
*/
void TruncateChatBuffer( )
{
	CHATSTRING *cstring;

	// Do size check on buffer
	while( chatBuffer.numEntries > chatBuffer.maxEntries )
	{
		for( cstring = chatBuffer.head.next; ; cstring = cstring->next )
		{
			if( !cstring->next->next ) // If next item is the last in the list
			{
				if( cstring->next->msg )
					delete cstring->next->msg;

				delete cstring->next;
				cstring->next = NULL;
				chatBuffer.numEntries--;
				break;
			}
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function		: ChatInput
	Purpose			: Process characters when typing a chat message
	Parameters		: char pressed
	Returns			: 
	Info			: 
*/
void ChatInput( char c )
{
	if (c == 8) // backspace
	{
		if( chatInput.msgLen )
			chatInput.msgLen--;
	}
	else if (c == 13) // enter
	{
		SendChatMessage( chatInput.msg, chatInput.msgLen );
		chatInput.msgLen = 0;
	}
	else
	{
		chatInput.msg[chatInput.msgLen++] = c;
	}
}
