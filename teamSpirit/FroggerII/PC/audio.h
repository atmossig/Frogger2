/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: audio.h
	Programmer	: Andy Eder
	Date		: 28/06/99 10:37:57

----------------------------------------------------------------------------------------------- */

#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED


extern int PlaySample(short num, VECTOR *pos, short tempVol,short pitch);
extern int PlaySampleRadius(short num, VECTOR *pos, short vol,short pitch,float radius);
extern void PrepareSong(char num);


#endif
