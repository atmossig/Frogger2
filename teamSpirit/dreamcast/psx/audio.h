/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: audio.h
	Programmer	: Andy Eder
	Date		: 28/06/99 10:37:57

----------------------------------------------------------------------------------------------- */

#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED
 


#include <islsfx2.h> 
#include "types.h"
#include "actor2.h"
#include "am.h"

// //***********************************
// // Defines
 
// Hmm - three flags. Worth it?
#define SFXFLAGS_GENERIC		(1<<0)
#define SFXFLAGS_3D_SAMPLE		(1<<1)
#define SFXFLAGS_LOOP			(1<<2)

 
// #define AMBIENT_VOLUME	255
#define SAMPLE_VOLUME	255
 
 
enum
{
	NOTRACK = 0,
	GARDEN_CDAUDIO = 2,
	ANCIENTS_CDAUDIO,
	SPACE_CDAUDIO,
	CITY_CDAUDIO,
	SUBTERRANEAN_CDAUDIO,
	LABORATORY_CDAUDIO,
	HALLOWEEN_CDAUDIO,
	SUPERRETRO_CDAUDIO,
	FRONTEND_CDAUDIO,
	LEVELSELECT_CDAUDIO,

	NUM_CD_TRACKS,
};
 
// All generic sfx
enum
{
	/*GEN_FROG_HOP,
	GEN_SUPER_HOP,
	GEN_DOUBLE_HOP,
	GEN_COLLECT_BABY,
	GEN_COLLECT_COIN,
	GEN_BABYHAPPY,
	GEN_BABYSAD,
	GEN_BABYCRY,
	GEN_BABYREPLY,
	GEN_TELEPORT,
	GEN_POWERUP,

	GEN_DEATHDROWN,
	GEN_DEATHCRUSH,
	GEN_DEATHEXPLODE,
	GEN_DEATHFALL,
	GEN_DEATHGIB,
	GEN_DEATHCHOP,
	GEN_DEATHELECTRIC,

	NUM_GENERIC_SFX*/

	GEN_FROG_HOP,
	GEN_SUPER_HOP,
	GEN_DOUBLE_HOP,
	GEN_COLLECT_BABY,
	GEN_FROG_TONGUE,
	GEN_COLLECT_COIN,
	GEN_CROAK,
	GEN_BABYHAPPY,
	GEN_BABYSAD,
	GEN_BABYCRY,
	GEN_BABYREPLY,
	GEN_FROGBELCH1,
	GEN_FROGBELCH2,
	GEN_FROGANNOYED,
	GEN_FROGSLIDE,
	GEN_FROGHAPPY,
	GEN_FROGSCARED,
	GEN_FROGBORED,
	GEN_FROGLETSGO,
	GEN_TELEPORT,
	GEN_POWERUP,

	GEN_CLOCKTICK,
	GEN_POWERTICK,

	GEN_DEATHNORMAL,
	GEN_DEATHDROWN,
	GEN_DEATHDROWN1,
	GEN_DEATHDROWN2,
	GEN_DEATHCRUSH,
	GEN_DEATHEXPLODE,
	GEN_DEATHFALL,
	GEN_DEATHGIB,
	GEN_DEATHMOWED,
	GEN_DEATHCHOP,
	GEN_DEATHELECTRIC,
	GEN_DEATHFIRE,
	
	NUM_GENERIC_SFX,
};
 




// //***********************************
// // Type Defines
 
// typedef struct _SAMPLE
// {
// 	struct _SAMPLE			*next, *prev;
// 
// 	char					*idName;
// 	unsigned long			flags;
// 	unsigned long			uid;
// 
// 	LPWAVEFORMATEX			lpWavFmt;
// 	BYTE					*data;
// 	DWORD					len;
// 
// 	LPDIRECTSOUNDBUFFER		lpdsBuffer;	
//     LPDIRECTSOUND3DBUFFER   lpds3DBuffer;	// DirectSound 3D buffer interface
// 
// } SAMPLE;

typedef struct _SAMPLE
{
	struct _SAMPLE			*next, *prev;

	char					idName[32];
	unsigned long			uid;
	unsigned long			flags;

	AM_BANK_PTR				bankPtr;
	unsigned long			sampleNumber;
	
//ma	SfxSampleType			*snd;
} SAMPLE;

 
 
typedef struct _SOUNDLIST
{
	int				numEntries;
	AM_BANK_PTR		genericBank;
	AM_BANK_PTR		levelBank;
	SAMPLE			head;

} SOUNDLIST;
 
 
// typedef struct _BUFSAMPLE
// {
// 	struct _BUFSAMPLE *next, *prev;
// 	LPDIRECTSOUNDBUFFER lpdsBuffer;
// 
// } BUFSAMPLE;
 
 
//typedef struct _BUFFERLIST
// {
// 	int			numEntries;
// 	BUFSAMPLE	head;
// 
// } BUFFERLIST;
 
 
typedef struct TAG_AMBIENT_SOUND
{
	struct TAG_AMBIENT_SOUND *next,*prev;

	SVECTOR		pos;
	ACTOR		*follow;

	struct _SAMPLE *sample;

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
 
 extern SAMPLE *genSfx[];
 
//***********************************
// Function Prototypes
 
// extern void LoadSfxMapping( int world, int level );
// extern SAMPLE **FindSfxMapping( unsigned long uid, ACTOR *actor );
 
// extern void LoadSfx( unsigned long worldID );
 
// extern void InitSampleList( );
// extern void FreeSampleList( );
 
// extern void InitAmbientSoundList( );
// extern void FreeAmbientSoundList( );
 
// extern void FreeBufSampleList( );
 
// extern SAMPLE *FindSample( unsigned long uid );
 
// extern void CleanBufferSamples ( void );
// extern void SetSampleFormat ( SAMPLE *sample );
 
 
// extern int PlaySample(SAMPLE *sample,VECTOR *pos,long radius,short volume,short pitch);
// void PlaySfxMappedSample( ACTOR *act, long radius, short volume, short pitch );
// extern AMBIENT_SOUND *AddAmbientSound(SAMPLE *sample,VECTOR *pos,long radius,short volume,short pitch,float freq,float rFreq,ACTOR *follow);
// extern void UpdateAmbientSounds();
 
 
// extern void PrepareSongForLevel( short worldID, short levelID );


// #define UpdateAmbientSounds()
// //#define PTTextureLoad()
#define LoadSfxMapping(a,b)		0
// #define FreeAmbientSoundList()
// #define InitAmbientSoundList()


void StartSound();
void StopSound();
void InitSampleList( );
int LoadSfxSet( char *path, int generic );
int LoadSfx( unsigned long worldID );
SAMPLE *CreateAndAddSample( SfxSampleType *snd );
void AddSample( SAMPLE *sample );
void RemoveSample( SAMPLE *sample );
SAMPLE *FindSample( unsigned long uid );
void FreeSampleList( );
int PlaySample( SAMPLE *sample, SVECTOR *pos, long radius, short volume, short pitch );
int PlayVoice(int player, char *name);
AMBIENT_SOUND *AddAmbientSound(SAMPLE *sample, SVECTOR *pos, long radius, short vol, short pitch, float freq, float randFreq, ACTOR *follow );
void UpdateAmbientSounds();
void InitAmbientSoundList();
void SubAmbientSound(AMBIENT_SOUND *ambientSound);
void FreeAmbientSoundList( );
void PrepareSong(short worldID);
void StopSong( );

#endif
