/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: frogmove.h
	Programmer	: Andrew Eder
	Date		: 11/13/98

----------------------------------------------------------------------------------------------- */

#ifndef FROGMOVE_H_INCLUDED
#define FROGMOVE_H_INCLUDED

#define NUM_PREV_TILES		5

#define FROG_SAFEDROPHEIGHT						60.0F
#define FROG_NORMALJUMPHEIGHT					25.0F
#define FROG_SUPERJUMPHEIGHT					50.0F


// frog movement / action flags
#define FROGSTATUS_NONE							0
#define FROGSTATUS_ISDEAD						(1 << 0)
#define FROGSTATUS_ISSTANDING					(1 << 1)
#define FROGSTATUS_ISWANTINGU					(1 << 2)
#define FROGSTATUS_ISWANTINGD					(1 << 3)
#define FROGSTATUS_ISWANTINGL					(1 << 4)
#define FROGSTATUS_ISWANTINGR					(1 << 5)
#define FROGSTATUS_ISWANTINGSUPERHOPU			(1 << 6)
#define FROGSTATUS_ISWANTINGSUPERHOPD			(1 << 7)
#define FROGSTATUS_ISWANTINGSUPERHOPL			(1 << 8)
#define FROGSTATUS_ISWANTINGSUPERHOPR			(1 << 9)
#define FROGSTATUS_ISJUMPINGTOTILE				(1 << 10)
#define FROGSTATUS_ISJUMPINGTOPLATFORM			(1 << 11)
#define FROGSTATUS_ISJUMPINGPLATFORMSLOCALLY	(1 << 12)
#define FROGSTATUS_ISSUPERHOPPING				(1 << 13)
#define FROGSTATUS_ISSINKING					(1 << 14)
#define FROGSTATUS_ISTONGUEING					(1 << 15)
#define FROGSTATUS_ISONMOVINGPLATFORM			(1 << 16)
#define FROGSTATUS_ISFALLINGTODEATH				(1 << 17)
#define FROGSTATUS_ISONICE						(1 << 18)

#define FROGSTATUS_ISWANTINGLONGHOPD			(1 << 19)
#define FROGSTATUS_ISWANTINGLONGHOPU			(1 << 20)
#define FROGSTATUS_ISWANTINGLONGHOPL			(1 << 21)
#define FROGSTATUS_ISWANTINGLONGHOPR			(1 << 22)
#define FROGSTATUS_ISLONGHOPPING				(1 << 23)

#define FROGSTATUS_ISFLOATING					(1 << 24)
#define FROGSTATUS_ISSPRINGING					(1 << 25)

#define FROGSTATUS_ISCROAKING					(1 << 26)
#define FROGSTATUS_ISTELEPORTING				(1 << 27)


enum
{
	MOVE_UP,
	MOVE_RIGHT,
	MOVE_DOWN,
	MOVE_LEFT,
};

enum
{
	JUMPING_TOTILE,
	JUMPING_TOPLATFORM,
};

// ENUM's for different ways of 'killing' Frogger

enum
{
	DEATHBY_NORMAL,
	DEATHBY_INSTANT,
	DEATHBY_RUNOVER,
	DEATHBY_DROWNING,
	DEATHBY_SQUASHED,
	DEATHBY_EXPLOSION,
	DEATHBY_FALLINGTOPLATFORM,
	DEATHBY_FALLINGTOTILE,
	DEATHBY_CHOCOLATE,
};


extern GAMETILE *currTile;
extern GAMETILE *destTile;
extern GAMETILE *prevTile;

extern GAMETILE *currTile2;
extern GAMETILE *destTile2;

extern unsigned long frogState;
extern unsigned long frogState2;

extern float landRadius;

extern unsigned long d;
extern unsigned long iceTileX;
extern unsigned long iceTileY;
extern unsigned long iceTileZ;
extern unsigned long ice[];

extern int			frogFacing;
extern int			frogFacing2;

extern float speedTest;

// ----- [ FUNCTION PROTOTYPES ] ---------- //

extern void SetFroggerStartPos(GAMETILE *startTile,ACTOR2 *act);
void UpdateFroggerPos();
void UpdateFroggerPos2();
extern BOOL MoveToRequestedDestination(int dir);
extern void GetNextTile(unsigned long direction);

extern void SlurpFroggerPosition(int whereTo);
extern void SlurpFroggerPosition2(int whereTo);

extern void CheckForFroggerLanding(int whereTo);
extern void CheckForFroggerLanding2(int whereTo);
extern void SlideFroggerPosition ( ACTOR2 *actor2 );

extern BOOL GameTileTooHigh(GAMETILE *tile);
extern BOOL GameTileTooLow(GAMETILE *tile);

extern BOOL KillFrog(ACTOR2 *frogAct);

extern void GetNextTileLongHop ( unsigned long direction );
extern void RotateFrog ( ACTOR2* frog, unsigned long fFacing );


#endif
