/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: map.h
	Programmer	: Matthew Cloy
	Date		: 04/12/98

----------------------------------------------------------------------------------------------- */

#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

enum
{
	RETRO1_MAP_BANK,
	RETRO2_MAP_BANK,
	RETRO3_MAP_BANK,

	VENICE1_MAP_BANK,
};

extern char *mapBank;
extern unsigned long numSafe;
extern unsigned long numPwrups;

extern GAMETILE **bTSafe;
extern GAMETILE **pwrupTStart;


extern SCENIC *Sc_000;

#include "levbanks\\levext.h"

extern void LoadMapBank(int num);
extern void FreeMapBank(void);

extern void MakeTeleportTile(GAMETILE *fromTile,GAMETILE *toTile,char teleportType);
extern BOOL IsATeleportTile(GAMETILE *tile);
extern void TeleportActorToTile(ACTOR2 *act,GAMETILE *tile,long pl);

#endif
