/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: levelplaying.h
	Programmer	: David Swift
	Date		: 17/03/99

----------------------------------------------------------------------------------------------- */

#ifndef LEVELPLAYING_H_INCLUDED
#define LEVELPLAYING_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_DEMOS		3

extern int demoLevels[NUM_DEMOS][2];

extern int nextDemo;

extern unsigned long *playKeyList;

void InitDemoMode();
void RunDemoMode();

#ifdef __cplusplus
}
#endif
#endif