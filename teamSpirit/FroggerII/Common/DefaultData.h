/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: LevelData.h
	Programmer	: James Healey
	Date		: 14/06/99

----------------------------------------------------------------------------------------------- */

#ifndef LEVELDATA_H_INCLUDED
#define LEVELDATA_H_INCLUDED

//************************************************************************************************
//	Defines

//#define MAX_LEVELHISCORE_SLOTS	3
#define MAX_HISCORE_SLOTS		5

//************************************************************************************************
//	Type Defines

typedef struct _HISCORE
{
	char	name[3];
	long	score;
	short	time;
	short	cup;
} HISCORE;
/*
typedef struct _LEVELHISCORES
{
	HISCORE	scoreData [ MAX_LEVELHISCORE_SLOTS ];
} LEVELHISCORES;

typedef struct _WORLDLEVELHISCORES
{
	HISCORE levelScoreData[ MAX_LEVELS ];
} WORLDLEVELHISCORES;
*/
//************************************************************************************************
//	Global Externs

extern HISCORE	worldHiScoreData	[ MAX_WORLDS ][ MAX_LEVELS ];
extern HISCORE	hiScoreData			[ MAX_HISCORE_SLOTS ];

#endif