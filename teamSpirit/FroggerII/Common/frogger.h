/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: frogger.h
	Programmer	: James Healey
	Date		: 23/03/99

----------------------------------------------------------------------------------------------- */

#ifndef FROGGER_H_INCLUDED
#define FROGGER_H_INCLUDED


#define SPAWN_SCOREUPTIMER		15


typedef struct _PLAYER
{
	char			name[3];
	unsigned long	score;
	short			lives;
	short			numSpawn;
	short			timeSec;

	short			spawnTimer;
	short			spawnScoreLevel;

	unsigned long 	worldNum;
	unsigned long	levelNum;
	unsigned long	frogState;

} PLAYER;

extern PLAYER player[4];



extern float CROAK_SOUND_RANGE;

/**/
extern long isJump;
extern float jumpSpeed;
extern float jumpAmt;

extern float changeJumpSpeed;
extern float bounceJumpSpeed;
extern float finalJumpSpeed;
extern float startJumpSpeed;

/**/

extern long isLong;
extern float longSpeed;
extern float longAmt;

extern float changeLongSpeed;
extern float startLongSpeed;


extern ACTOR2	*frog[];
extern ACTOR2	*frog2;
extern SPRITEOVERLAY *sprHeart[3];

extern float	croakRadius;
extern float	croakVelocity;
extern char		croakFade;
extern char		croakFadeDec;
extern char		croakR,croakG,croakB;



extern void CreateFrogger ( unsigned long createFrogActor, unsigned long createFrogOverlays,
							 unsigned long createBabyActors, unsigned long createBabyOverlays );

#endif


