#ifndef __NETCHAT_H
#define __NETCHAT_H


#ifdef __cplusplus
extern "C"
{
#endif


#define MAX_CSLENGTH 256

#define CHAT_SHOW		(1<<0)
#define CHAT_INPUT		(1<<1)

typedef struct _CHATSTRING
{
	struct _CHATSTRING *next;
	char *msg;
	short msgLen;

} CHATSTRING;


typedef struct
{
	CHATSTRING head;
	short numEntries;
	short maxEntries;

} CHATBUFFER;


typedef struct
{
	DWORD			dwType;			// message type (APPMSG_CHATSTRING)
	char			szMsg[1];		// message string (variable length)

} MSG_CHATSTRING,*LPMSG_CHATSTRING;


extern CHATBUFFER chatBuffer;
extern unsigned char chatFlags;
extern CHATSTRING chatInput;        // Reuse of chatstring. Next is always NULL for this

HRESULT NewChatMessage(LPDIRECTPLAY4A lpDirectPlay4A,DPID dpidPlayer,LPSTR lpszMsg,LPSTR *lplpszStr);
HRESULT GetChatPlayerName(LPDIRECTPLAY4A lpDirectPlay4A,DPID dpidPlayer,LPDPNAME *lplpName);
HRESULT	SendChatMessage(LPSTR string, LONG lStrLen); // Called to send in game chat
void HandleChatMessage( LPDPLAYINFO lpDPInfo,LPMSG_CHATSTRING lpMsg,DWORD dwMsgSize,DPID idFrom,DPID idTo );

void InitChatBuffer( );
void FreeChatBuffer( );
void AddChatMessage( char *msg );
void DrawChatBuffer( int left, int top, int right, int bottom );
void ChatInput( char c );
void TruncateChatBuffer( );

#ifdef __cplusplus
}
#endif

#endif