/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: water.h
	Programmer	: Andy Eder
	Date		: 20/10/99 16:46:53

----------------------------------------------------------------------------------------------- */

#ifndef WATER_H_INCLUDED
#define WATER_H_INCLUDED


#ifdef N64_VERSION

#define MAX_N64_WATEROBJECTS	4

extern ACTOR2 *watActor;

extern short numN64WaterObjects;
extern short currN64WaterObject;
extern SHORT2DVECTOR *mTC[MAX_N64_WATEROBJECTS];


void RunWaterDemo();
void UpdateWater(ACTOR2 *wAct);

void AddN64WaterObjectResource(ACTOR *wAct);
void FreeN64WaterResources();


#endif


#endif
