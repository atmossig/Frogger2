#ifndef __NETCHAT_H
#define __NETCHAT_H

enum CHAT_FORMAT {
	CHAT_NORMAL,
	CHAT_SYSTEM,
	CHAT_ERROR,
};

int RunNetChatWindow(HWND);
void NetShowMessage(const char* string, CHAT_FORMAT f);

#endif