#ifndef __MULTI_H
#define __MULTI_H


#define MULTIMODE_RACE	1
#define MULTIMODE_CTF	2
#define MULTIMODE_DM	3


extern int multiplayerMode;

void RunMultiplayer( );

void PickupBabyFrogMulti( ENEMY *baby, int pl );
void KillMPFrog(int num);

void CalcMPCamera( VECTOR *target );

#endif