/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: babyfrog.h
	Programmer	: James Healey
	Date		: 17/03/99

----------------------------------------------------------------------------------------------- */


#ifndef BABYFROGS_H_INCLUDED
#define BABYFROGS_H_INCLUDED

#define NUM_BABIES 5

extern ACTOR2 *babies[NUM_BABIES];
extern ACTOR2 *nearestBaby;
extern ACTOR2 *lastBabySaved;

extern SPRITEOVERLAY *babyIcons[6];
extern unsigned long babiesSaved;

extern void CreateBabies(unsigned long createActors,unsigned long createOverlays);
extern void RunBabySavedSequence(ACTOR2 *baby);

extern ACTOR2 *GetNearestBabyFrog();


#endif