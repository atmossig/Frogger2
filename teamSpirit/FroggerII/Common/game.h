/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: game.h
	Programmer	: Matthew Cloy
	Date		: 12/11/98

----------------------------------------------------------------------------------------------- */

#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "font.h"
#include "sprite.h"
#include "babyfrogs.h"

// Tile status flags.
#define ATTACH (1<<0)
// -----------------

//extern long timeMin,timeSec;
//extern long score,lives;
extern long gameIsOver;
extern long levelIsOver;

extern unsigned long iceMoveDir;
extern unsigned long autoPlaying;
extern unsigned long recordKeying;

extern unsigned long num;

extern ACTOR2 *babies[NUM_BABIES];
extern ACTOR2 *demoTug;

extern char	scoreText[32];
extern char	timeText[32];
extern long hopAmt;

extern long babySaved;

extern void RunGameLoop (void);
extern void Orientate(QUATERNION *me, VECTOR *fd, VECTOR *mfd, VECTOR *up);
extern void RunLevelCompleteSequence();
extern void UpdateScoreTables ( void );

#endif