#ifndef __MULTI_H
#define __MULTI_H


#define MULTIMODE_RACE_NORMAL		1
#define MULTIMODE_RACE_KNOCKOUT		2
#define MULTIMODE_CTF				3
#define MULTIMODE_BATTLE			4


#define MULTI_BATTLE_TRAILLEN		4

#define MULTI_RACE_NUMLAPS 3

typedef struct
{
	union
	{
		char lap;
		char babyCount;
	};

	union
	{
		short check;
		short trail;
	};

	char ready;
	short lasttile;

} MPINFO;


extern int multiplayerMode;

void RunMultiplayer( );
void ResetMultiplayer( );

void RaceRespawn( int pl );
void PickupBabyFrogMulti( ENEMY *baby, int pl );
void KillMPFrog(int num);

void CalcMPCamera( VECTOR *target );

#endif