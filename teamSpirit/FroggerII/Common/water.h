/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: water.h
	Programmer	: Andy Eder
	Date		: 20/10/99 16:46:53

----------------------------------------------------------------------------------------------- */

#ifndef WATER_H_INCLUDED
#define WATER_H_INCLUDED


#ifdef N64_VERSION

extern float waterFreq[2];
extern float waterFactor[2];
extern float waterF;
extern float waterWaveHeight[2];
extern VECTOR waterCentre[2];

extern float waterWaveHeightBase[2];
extern float waterWaveHeightAmp[2];
extern float waterWaveHeightFreq[2];
extern float watRot[2];

extern int waterMode;
extern int waterX;
extern int waterY;

extern float dist[2];
extern VECTOR tempVect;

extern ACTOR2 *watActor;


#endif


#endif
