/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: water.c
	Programmer	: Andy Eder
	Date		: 20/10/99 16:47:47

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>
#include "incs.h"

#ifdef N64_VERSION


float waterFreq[2] = { 80, 41 };
float waterFactor[2] = { 0.004, 0.008 };
float waterF = 0.1;
float waterWaveHeight[2] = { 20, -10 };
VECTOR waterCentre[2] = { { 25,0,25 },{ -15,0,0 } };

float waterWaveHeightBase[2] = { 10,8 };
float waterWaveHeightAmp[2] = { 20,10 };
float waterWaveHeightFreq[2] = { 101, 102 };
float watRot[2] = { 0,0 };

int waterMode = 1;
int waterX = 11;
int waterY = 11;

float dist[2];
VECTOR tempVect;

ACTOR2 *watActor = NULL;


#endif
