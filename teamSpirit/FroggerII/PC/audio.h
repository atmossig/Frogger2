/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: audio.h
	Programmer	: Andy Eder
	Date		: 28/06/99 10:37:57

----------------------------------------------------------------------------------------------- */

#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED


//***********************************
// Type Defines

typedef struct _SAMPLE
{
	struct _SAMPLE		*next, *prev;
	LPWAVEFORMATEX		lpWavFmt;
	BYTE				*Data;
	DWORD				Len;
	LPDIRECTSOUND		lpDSound;
	LPDIRECTSOUNDBUFFER lpdsBuffer;	

} SAMPLE;

typedef struct _SOUNDLIST
{
	int				numEntries;
	SAMPLE			head;
} SOUNDLIST;

extern SOUNDLIST soundList;


//***********************************
// Function Prototypes

//***************
// Sound list functions.

//extern int Makebuffer ( SAMPLE *sample );


extern SAMPLE *CreateAndAddSample ( LPSTR lpFile );

extern void InitSampleList			( void );
extern void FreeSampleList			( void );

extern void AddSampleToList			( SAMPLE *sample );
extern void RemoveSampleFromList	( SAMPLE *sample );





extern int PlaySample(short num, VECTOR *pos, short tempVol,short pitch);
extern int PlaySampleRadius(short num, VECTOR *pos, short vol,short pitch,float radius);
extern void PrepareSong(char num);


#endif
