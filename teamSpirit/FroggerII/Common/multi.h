#ifndef __MULTI_H
#define __MULTI_H


#define MULTIMODE_RACE_NORMAL		1
#define MULTIMODE_RACE_KNOCKOUT		2
#define MULTIMODE_CTF				3
#define MULTIMODE_BATTLE			4


typedef struct
{
	char lap;
	short check;
	short lasttile;

} RACEINFO;

#define MULTI_RACE_NUMLAPS 3


extern int multiplayerMode;

void RunMultiplayer( );
void ResetMultiplayer( );

void RaceRespawn( int pl );
void PickupBabyFrogMulti( ENEMY *baby, int pl );
void KillMPFrog(int num);

void CalcMPCamera( VECTOR *target );

#endif