/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: babyfrog.h
	Programmer	: James Healey
	Date		: 17/03/99

----------------------------------------------------------------------------------------------- */


#ifndef BABYFROGS_H_INCLUDED
#define BABYFROGS_H_INCLUDED

#define NUM_BABIES 5

extern GAMETILE **bTStart;

extern ACTOR2 *babies[NUM_BABIES];
extern ACTOR2 *nearestBaby;
extern ACTOR2 *lastBabySaved;
extern ACTOR *babyFollow[NUM_BABIES];

extern SPRITEOVERLAY *babyIcons[NUM_BABIES];
extern unsigned long babiesSaved;
extern unsigned long numBabies;

void CreateBabies(unsigned long createActors,unsigned long createOverlays);
void RunBabySavedSequence(ACTOR2 *baby);

ACTOR2 *GetNearestBabyFrog();
void FaceBabiesTowardsFrog(long pl);


#endif