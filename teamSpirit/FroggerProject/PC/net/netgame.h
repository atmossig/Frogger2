/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: netgame.h
	Programmer	: David Swift
	Date		:

----------------------------------------------------------------------------------------------- */

#ifndef __NETGAME_H
#define __NETGAME_H

#include "overlays.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern TEXTOVERLAY		*netMessage;
extern unsigned long	gameStartTime;

typedef int (*NETGAME_LOOP)(void);

void NetgameStartGame();
void NetgameRun();

#ifdef __cplusplus
}
#endif

#endif