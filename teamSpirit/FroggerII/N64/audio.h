/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: audio.h
	Programmer	: James Healey
	Date		: 20/04/99 14:40:24

----------------------------------------------------------------------------------------------- */

#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED




typedef struct
{

	u8	*sfxPtr;		//pointer table for level sfx
	u8	*sfxWbk;		//wave data for each level   

	u8	*musicPtr;		//pointer table for generic sfx
	u8	*musicWbk;		//wave data for generic sfx  

	u8	*musicBin;			//music data for current tune

	int	musicHandle;		//music handle
	int currentTrack;		//current track number

} AUDIOCONTROL;


typedef struct
{
	s16	triggerSecond,triggerFrame;
	s16	stopSecond,stopFrame;
	short	sfxNum;
	char	vol,pan;
	int		handle;
} SFX_SCRIPT;


typedef struct TAG_AMBIENT_SOUND
{
	struct TAG_AMBIENT_SOUND *next,*prev;
	SFX			sfx;
	VECTOR		offsetPos;
	VECTOR		pos;
	PLATFORM	*platform;
	short		freq;
	short		randFreq;
	short		counter;
	short		onTime;
	short		origVol;
	short		tag;
}AMBIENT_SOUND;


typedef struct
{
	AMBIENT_SOUND head;
	int numEntries;
}AMBIENT_SOUND_LIST;


#define MAX_AMBIENT_SFX	50


extern AMBIENT_SOUND_LIST	ambientSoundList;
extern AUDIOCONTROL audioCtrl;
extern int MAX_SFX_DIST;
extern int numberOfEnemies;
extern BOOL reverbOn;


extern unsigned long sfxVol;
extern unsigned long musicVol;

/*
extern SFX_SCRIPT	introSfxScript[];
extern SFX_SCRIPT	activeSfxScript[];
*/

enum
{
	NOTRACK,
//	ATLANTIS_TRACK,
//	ATLANTIS2_TRACK,
//	ATLANTIS3_TRACK,
//	ATLANTISBOSS_TRACK,
	CARNIVAL_TRACK,
	CARNIVAL2_TRACK,
	CARNIVAL3_TRACK,
	CARNIVALBOSS_TRACK,
	PIRATES_TRACK,
	PIRATES2_TRACK,
	PIRATES3_TRACK,
	PIRATESBOSS_TRACK,
	PREHISTORIC_TRACK,
	PREHISTORIC2_TRACK,
	PREHISTORIC3_TRACK,
	PREHISTORICBOSS_TRACK,
	FORTRESS_TRACK,
	FORTRESS2_TRACK,
	FORTRESS3_TRACK,
	FORTRESSBOSS_TRACK,
	OOTW_TRACK,
	OOTW2_TRACK,
	OOTW3_TRACK,
	OOTWBOSS_TRACK,
	TITLE_TRACK,
	INTRO_TRACK,
	OUTRO_TRACK,
	HUB1_TRACK,
	HUB2_TRACK,
	HUB3_TRACK,
	HUB4_TRACK,
	HUB5_TRACK,
	HUB6_TRACK,
	ATLANTIS_KILL1_TRACK,
	ATLANTIS_KILL2_TRACK,
	ATLANTIS_WIN1_TRACK,
	ATLANTIS_WIN2_TRACK,
	CARNIVAL_KILL1_TRACK,
	CARNIVAL_KILL2_TRACK,
	CARNIVAL_WIN1_TRACK,
	CARNIVAL_WIN2_TRACK,
	PIRATES_KILL1_TRACK,
	PIRATES_KILL2_TRACK,
	PIRATES_WIN1_TRACK,
	PIRATES_WIN2_TRACK,
	PREHISTORIC_KILL1_TRACK,
	PREHISTORIC_KILL2_TRACK,
	PREHISTORIC_WIN1_TRACK,
	PREHISTORIC_WIN2_TRACK,
	FORTRESS_KILL1_TRACK,
	FORTRESS_KILL2_TRACK,
	FORTRESS_WIN1_TRACK,
	FORTRESS_WIN2_TRACK,
	OOTW_KILL1_TRACK,
	OOTW_KILL2_TRACK,
	OOTW_WIN1_TRACK,
	OOTW_WIN2_TRACK,
	BONUS_TRACK,
	HASBRO_TRACK,
	TITLE2_TRACK,
	NUM_TRACKS,
};

extern int musresult;

int PlaySample(short num, VECTOR *pos, short vol,short pitch);
void UpdateContinuousSample(SFX *sfx);
void PrepareSong(char num);
void DmaRomToRam(char *src, char *dest, int len);
void ReInitMusicDriver(u8 *pointer, u8 *tune, u8 *wave);
//void SetMaxSfxDist(int dist);
void AddAmbientSfx(int num, int vol, int pan);
void ClearAmbientSfx();
void KillAmbientSfx();
void StopContinuousSample(SFX *sfx);
void PlayContinuousSample(SFX *sfx,short num, short vol, VECTOR *,short pitch);
void AddAmbientSfxAtPoint(int num, int vol,VECTOR *pos,short pitch,short freq,short randFreq,short onTime,short platTag,short tag,float radius);
void UpdateAmbientSounds();

void InitAmbientSoundList();
void SubAmbientSound(AMBIENT_SOUND *ambientSound);
//AMBIENT_SOUND *FindTaggedAmbientSound(AMBIENT_SOUND *startAmbientSound, int tag);
int PlaySampleNot3D(int num,UBYTE vol,UBYTE pan,UBYTE pitch);
int PlaySampleRadius(short num, VECTOR *pos, short vol,short pitch,float radius);

void InitSFXScript(SFX_SCRIPT *script);
BOOL ScriptTriggerSound(short second, short frame);
void ProcessSFXScript();
void InitMusicDriver(void);



#endif