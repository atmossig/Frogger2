#ifndef __MULTI_H
#define __MULTI_H


#define MULTIMODE_RACE_NORMAL		1
#define MULTIMODE_RACE_KNOCKOUT		2
#define MULTIMODE_CTF				3
#define MULTIMODE_BATTLE			4


#define MULTI_BATTLE_MAXITEMS		15
#define MULTI_BATTLE_TRAILLENGTH	8

#define MULTI_RACE_NUMLAPS 3

typedef struct
{
	union
	{
		char lap;
		char wins;
	};

	char babyCount;

	union
	{
		short check;
		short score;
	};

	short trail;

	unsigned char r, g, b;

	char ready;
	short lasttile;

	AIPATHNODE *path;

} MPINFO;

extern MPINFO mpl[];
extern int multiplayerMode;
extern unsigned long numMultiItems;

void RunMultiplayer( );
void ResetMultiplayer( );

void RaceRespawn( int pl );
void PickupBabyFrogMulti( ENEMY *baby, int pl );
void KillMPFrog(int num);

void CalcMPCamera( VECTOR *target );

#endif