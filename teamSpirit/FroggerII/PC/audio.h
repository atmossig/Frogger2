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

// Because CD playing pisses me off
#define USE_AUDIO

// Hmm - two flags. Worth it?
#define FLAGS_NONE			0
#define FLAGS_3D_SAMPLE		(1 << 0)


enum
{
	NOTRACK = 0,
	GARDEN_CDAUDIO = 2,
	ANCIENTS_CDAUDIO,
	SPACE_CDAUDIO,
	CITY_CDAUDIO,
	SUBTERRANEAN_CDAUDIO,
	LABORATORY_CDAUDIO,
	TOYSHOP_CDAUDIO,
	HALLOWEEN_CDAUDIO,
	SUPERRETRO_CDAUDIO,
	FRONTEND_CDAUDIO,
	LEVELSELECT_CDAUDIO,

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

// All Garden sfx - could this not be based on 100?
// e.g. Have enum for each world/level, which starts right after NUM_GENERIC_SFX.
// Depends how the samples are loaded and stored, I suppose. This seems a bit silly though
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

	struct _SAMPLEMAP	*map;
	char				idName[128];

	// Not keen on the windows specifics
	LPWAVEFORMATEX			lpWavFmt;
	BYTE					*data;
	DWORD					len;

//	LPDIRECTSOUND			lpDSound;
	LPDIRECTSOUNDBUFFER		lpdsBuffer;	
    LPDIRECTSOUND3DBUFFER   lpds3DBuffer;	// DirectSound 3D buffer interface
} SAMPLE;

typedef struct _SOUNDLIST
{
	int				numEntries;
	SAMPLE			head;

} SOUNDLIST;


// What is the point of these? A basic sample has a buffer anyway... Maybe I just don't understand
typedef struct _BUFSAMPLE
{
	struct _SAMPLE *next, *prev;
	LPDIRECTSOUNDBUFFER lpdsBuffer; // Is this a global? If so, why is it here?

} BUFSAMPLE;

typedef struct _BUFFERLIST
{
	int			numEntries;
	BUFSAMPLE	head;

} BUFFERLIST;


typedef struct TAG_AMBIENT_SOUND
{
	struct TAG_AMBIENT_SOUND *next,*prev;

	VECTOR		pos;
	ACTOR		*follow;

	int			num;

	short		volume;
	short		pitch;
	long		radius;

	long		freq;
	long		randFreq;
	long		counter;

}AMBIENT_SOUND;


typedef struct
{
	AMBIENT_SOUND head;
	int numEntries;

}AMBIENT_SOUND_LIST;


//***********************************
// Function Prototypes

extern void LoadSfx( unsigned long worldID );

extern void InitSampleList( );
extern void FreeSampleList( );

extern void InitAmbientSoundList( );
extern void FreeAmbientSoundList( );

extern void FreeBufSampleList( );


// What does this do?
extern void CleanBufferSamples ( void );
// Eh?
extern void SetSampleFormat ( SAMPLE *sample );


extern int PlaySample(short num,VECTOR *pos,long radius,short volume,short pitch);
extern AMBIENT_SOUND *AddAmbientSound(short num,VECTOR *pos,long radius,short volume,short pitch,float freq,float rFreq,ACTOR *follow);
void UpdateAmbientSounds();


extern void PrepareSongForLevel( short worldID, short levelID );

#endif


// Probably don't need to be available for other files
//extern AMBIENT_SOUND_LIST ambientSoundList;
//extern SOUNDLIST soundList;
//extern SAMPLEMAP genericMapping [ NUM_GENERIC_SFX ];
//extern SAMPLEMAP gardenMapping  [ NUM_GARDEN_SFX ];
//extern void AddBufSampleToList		( BUFSAMPLE *bufSample );
//extern void RemoveBufSampleFromList ( BUFSAMPLE *bufSample );
//extern void AddSample( SAMPLE *sample );
//extern void RemoveSample( SAMPLE *sample );
//extern SAMPLE *CreateAndAddSample( SAMPLEMAP sampleMap );
//extern SAMPLE *FindSample( int num );
