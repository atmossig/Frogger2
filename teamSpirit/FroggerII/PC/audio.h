/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: audio.h
	Programmer	: Andy Eder
	Date		: 28/06/99 10:37:57

----------------------------------------------------------------------------------------------- */

#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED


//***********************************
// Defines

#define FLAGS_NONE			0
#define FLAGS_3D_SAMPLE		( 1 << 0 )


enum
{
	GARDEN_CDAUDIO,
	ANCIENTS_CDAUDIO,
	SPACE_CDAUDIO,
	CITY_CDAUDIO,
	SUBTERRANEAN_CDAUDIO,
	LABORATORY_CDAUDIO,
	TOYSHOP_CDAUDIO,
	HALLOWEEN_CDAUDIO,
	SUPERRETRO_CDAUDIO,

	NUM_CD_TRACKS,
};

// All generic sfx
enum
{
	GEN_FROG_HOP,
	GEN_SUPER_HOP,
	GEN_BABY_FROG,
	GEN_CLOCK_TOCK,
	GEN_CLOCK_TICK,
	GEN_TIME_OUT,
	GEN_TARGET_COM,
	GEN_LEVEL_COMP,

	NUM_GENERIC_SFX,
};

// All Garden sfx
enum
{
	GAR_MOWER = 100,
	END_GARDEN_SFX,
};

#define NUM_GARDEN_SFX ( END_GARDEN_SFX - 100 )

//***********************************
// Type Defines

typedef struct _SAMPLEMAP
{
	char			*sampleFileName;
	char			numChannels;
	int				sampleRate;
	char			bitsPerSample;
	unsigned char	sampleID;	
	unsigned long   flags;
} SAMPLEMAP;

typedef struct _SAMPLE
{
	struct _SAMPLE		*next, *prev;
	
	char				idName[32];
	char				numChannels;
	int					sampleRate;
	char				bitsPerSample;
	unsigned char		sampleID;
	unsigned long		flags;


	LPWAVEFORMATEX			lpWavFmt;
	BYTE					*Data;
	DWORD					Len;
	LPDIRECTSOUND			lpDSound;
	LPDIRECTSOUNDBUFFER		lpdsBuffer;	
    LPDIRECTSOUND3DBUFFER   lpds3DBuffer;	// DirectSound 3D buffer interface
} SAMPLE;

typedef struct _SOUNDLIST
{
	int				numEntries;
	SAMPLE			head;
} SOUNDLIST;

typedef struct _BUFSAMPLE
{
	struct _SAMPLE		*next, *prev;
	LPDIRECTSOUNDBUFFER lpdsBuffer;
} BUFSAMPLE;

typedef struct _BUFFERLIST
{
	int			numEntries;
	BUFSAMPLE	head;
} BUFFERLIST;

extern SOUNDLIST soundList;


extern SAMPLEMAP genericMapping [ NUM_GENERIC_SFX ];
extern SAMPLEMAP gardenMapping  [ NUM_GARDEN_SFX ];

//***********************************
// Function Prototypes

//***************
// Sound list functions.

extern void LoadSfx ( unsigned long worldID );

extern void InitSampleList				( void );
extern void FreeSampleList				( void );
extern void AddSampleToList				( SAMPLE *sample );
extern void RemoveSampleFromList		( SAMPLE *sample );

extern SAMPLE *CreateAndAddSample		( SAMPLEMAP sampleMap );
extern SAMPLE *GetEntryFromSampleList	( int num );


extern void FreeBufSampleList		( void );
extern void AddBufSampleToList		( BUFSAMPLE *bufSample );
extern void RemoveBufSampleFromList ( BUFSAMPLE *bufSample );


extern void CleanBufferSamples ( void );





extern int PlaySample ( short num, VECTOR *pos, short tempVol, short pitch );
extern int PlayActorBasedSample( short num, ACTOR* act, short tempVol, short pitch );
extern int PlaySampleNot3D( short num, UBYTE vol, short tempVol, short pitch );

extern void SetSampleFormat ( SAMPLE *sample );

extern int PlaySampleRadius(short num, VECTOR *pos, short vol,short pitch,float radius);
extern void PrepareSong(char num);

extern DWORD playCDTrack ( HWND hWndNotify, BYTE bTrack );

#endif
