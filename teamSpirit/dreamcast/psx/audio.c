/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: audio.c
	Programmer	: Random Healey
	Date		: 28/06/99 10:37:45

----------------------------------------------------------------------------------------------- */

// #include <ultra64.h>
// #include "..\resource.h"
// #include "incs.h"
#include <islmem.h>

#include <audio.h>

#include "main.h"

#include "memload.h"
#include "types.h"
#include "frogger.h"
#include "layout.h"

#include "am_audio.h"
#include <ac.h>
#include <a64thunk.h>
#include <am.h>

#include "am_audio.h"
#include "bpamsetup.h"
#include "bpamstream.h"
#include "bpbuttons.h"
#include "bpprintf.h"
#include "bpamsfx.h"
#include "strsfx.h"
#include "menus.h"
#include "game.h"
#include "cam.h"

int lastSound = -1;

char *musicNames[] = {	"CDA.XA",
						"CDB.XA",
					 };

int musicList[] = { 0,//
					1,//
					2,//
					6,//
					3,//
					4,//
					5,//
					6,//
					7,
					0,
					1,
					2,
};//

#define SFX_BASE		"SFX\\"

 
//#define MAX_AMBIENT_SFX		50
#define DEFAULT_SFX_DIST	700*SCALE

//This value means the pitch can go up to 100000Hz like the PC. This might be a 
//problem for the PSX but I don't think it goes much above 20000Hz, which seems alright. May
//cause problems in the future though, so check!!
#define PITCH_STEP		190 //(DSBFREQUENCY_MAX/256)   //MM NEEDS CHANGING!!!!!

 
//#define VOLUME_MIN		-5000
//#define VOLUME_PERCENT (VOLUME_MIN/100)
//#define PITCH_STEP		(DSBFREQUENCY_MAX/256)

SAMPLE voiceArray[4][MAX_VOICES];
 
SAMPLE *genSfx[NUM_GENERIC_SFX];
// SAMPLE **sfx_anim_map = NULL;

//XAFileType	*xaFileData[NUM_CD_TRACKS];
 
// UINT mciDevice = 0;
 
SOUNDLIST soundList;					// Actual Sound Samples List
AMBIENT_SOUND_LIST	ambientSoundList;

int GetSoundVols(SVECTOR *pos,int *vl,int *vr,long radius,unsigned long vol);

int UpdateLoopingSample(AMBIENT_SOUND *sample)
{
	int vl,vr,vs;
	short pitch;

	if (!sample)
	{
		utilPrintf("Sample Not Valid!!!!!!\n");
		return;
	}
	
	if(GetSoundVols(&sample->pos,&vl,&vr,sample->radius,sample->volume) == -1)
	{
		if(sample->handle >= 0)
		{
			sfxStopChannel(sample->handle);
			sample->handle = -1;
		}
		return;
	}
	
	if(sample->pitch == -1)
	{
		pitch = 0;
	}
	else
	{
		pitch = sample->pitch * PITCH_STEP;
	}


//	if((sample->sample->handle == -1) && (sample->sample) && (sample->sample->snd))
	if((sample->handle == -1) && (sample->sample) && (sample->sample->snd))
	{
//		vs = VSync(1);
//		while((lastSound>=0) && (SpuGetKeyStatus(1<<lastSound)==SPU_ON_ENV_OFF) && (VSync(1)<vs+3));

 		lastSound = sample->handle = sfxPlaySample(sample->sample->snd, vl,vr, pitch);
		return;
	}

	if(pitch)
		sfxSetChannelPitch(sample->handle,pitch);
	sfxSetChannelVolume(sample->handle, vl, vr);
	
}

/*	--------------------------------------------------------------------------------
	Function		:StartSound
	Purpose		    :  Initialises the PSX sound chip and the soundlibraries
	Parameters		: none
	Returns			: 
	Info			: Should only be called once at the start of the game loop. 
*/
void StartSound()
{
	sfxInitialise(0);
 	sfxStartSound();
 	sfxOn();
 	sfxSetGlobalVolume(255);// set to max volume 
}

/*	--------------------------------------------------------------------------------
	Function		:StopSound
	Purpose		    :  Turns off the PSX sound chip and the soundlibraries
	Parameters		: none
	Returns			: 
	Info			: Should only be called once at the end of the game loop. 
*/
void StopSound()
{
	FreeSampleList();
	
	sfxOff();
	sfxStopSound();
	sfxDestroy();
}

/*	--------------------------------------------------------------------------------
	Function		: InitSampleList
	Purpose			: Initialises the sample list
	Parameters		: 
	Returns			: 
	Info			: 
*/
void InitSampleList( )
{
	//Init the sample list for the real samples.

	int i;
/*
	if(soundList.genericBank)
	{
		sfxUnloadSampleBank(soundList.genericBank);
		sfxRemoveSampleBank(soundList.genericBank,1);
		soundList.genericBank = NULL;
	}
	if(soundList.levelBank)
	{
		sfxUnloadSampleBank(soundList.levelBank);
		sfxRemoveSampleBank(soundList.levelBank,1);
		soundList.levelBank = NULL;
	}
	if(soundList.loopBank)
	{
		sfxUnloadSampleBank(soundList.loopBank);
		sfxRemoveSampleBank(soundList.loopBank,1);
		soundList.loopBank = NULL;
	}
	for(i = 0;i < 4;i++)
	{
		if(soundList.voiceBank[i])
		{
			sfxUnloadSampleBank(soundList.voiceBank[i]);
			sfxRemoveSampleBank(soundList.voiceBank[i],1);
			soundList.voiceBank[i] = NULL;
		}
	}
*/
	soundList.count = 0;
}

/*	--------------------------------------------------------------------------------
	Function		: LoadSfxSet
	Purpose			: Loads the soundbank and adds samples to list
	Parameters		: path to be loaded, adn whether the bank is the generic bank
	Returns			: Success
	Info			: 
*/

#define MAX_SAMPLE_BANKS	6

SfxBankType			sampleBanks[MAX_SAMPLE_BANKS];
AM_BANK_PTR			audio64Banks[MAX_SAMPLE_BANKS];

int	numSoundBanks = 0;

CurrentData			current[24];

int					sfxGlobalVolume,sfxOutputOn;

int LoadSfxSet(char *path, SfxBankType **sfxBank,int flags,SAMPLE *array,short *count)
{
	int 			i;
	SAMPLE 			*sfx;
	SfxBankType		*bank;
	AM_BANK_PTR		am_bank;
	SAMPLE 			*newItem;
	int				numSamples,loop,bytesRead;
	AM_SOUND		soundInfo;
	char			filename[256],*fPtr;
	char			*sampleList,*sListPtr;
	SfxSampleType 	*snd;
	
	sprintf(filename,"%s.kat",path);
	if((am_bank = bpAmLoadBank(filename))==KTNULL)
		return 0;

	bank = &sampleBanks[numSoundBanks];
	audio64Banks[numSoundBanks] = am_bank;
	
	//Allocate memory for sample data and grab data from .kat file
	amBankGetNumberOfAssets(am_bank,(KTU32 *)&numSamples);

	//malloc the bank all in one then fill it in
	bank->numSamples = numSamples;
	bank->sample = (SfxSampleType *) syMalloc(sizeof(SfxSampleType)*numSamples);

	for(loop=0; loop<bank->numSamples; loop++)
	{
		amSoundFetchSample(audio64Banks[numSoundBanks],loop,&soundInfo);
		bank->sample[loop].sampleSize	= soundInfo.sizeInBytes;
		bank->sample[loop].sampleRate	= soundInfo.sampleRate;
		bank->sample[loop].bankNumber	= numSoundBanks;
		bank->sample[loop].sampleNumber	= loop;
		bank->sample[loop].inSPURam		= 1;//Always loaded on DC
		bank->sample[loop].pad			= 0;
	}

	//Need name CRC's unfortunately so grab these from a seperate file
	sprintf(filename,"sfx\\%s.txt",path);
	
	sampleList = (char*)fileLoad(filename,&bytesRead);
	sListPtr = sampleList;

	for(loop=0; loop<bank->numSamples; loop++)
	{
		for(fPtr = filename; ((*sListPtr)!='\0') && ((*sListPtr)!='\n'); fPtr++)
		{
			*fPtr = *(sListPtr++);
		}
		fPtr -= 5;//Take off the .wav extension and the carriage return which seems to have squeezed in
		*fPtr = '\0';
		while(*sListPtr =='\n') sListPtr++;

		bank->sample[loop].nameCRC = utilStr2CRC(filename);
		strcpy(bank->sample[loop].name,filename);
	}

	Align32Free(sampleList);

	snd = (SfxSampleType *) bank->sample;

	for(i=0;i<bank->numSamples;i++)
	{
		if(!snd)
		{
			utilPrintf("oops...can't find sample %i in bank %s\n",i,path);
			continue;
		}
		newItem = array + *count;
		newItem->uid = snd->nameCRC;
		newItem->snd = snd;	
		newItem->flags = flags;
		newItem->bankPtr = am_bank;
		newItem->sampleNumber = *count;
		strcpy(newItem->idName,snd->name);
		
		(*count)++;
			
		*snd++;
	}

	numSoundBanks++;

	return 1;
}

/*	--------------------------------------------------------------------------------
	Function		: LoadSfx
	Purpose			: Loads a sound bank, and puts all the samples in the sample list
	Parameters		: number of world sound bank to load
	Returns			: Success?
	Info			: 
*/
short voiceCount[4];
int LoadSfx(long worldID )
{
	char path[256];
	int len,j;	


	if(worldID != -1)
	{
		for(j = 0;j < NUM_FROGS;j++)
		{
			path[len] = '\0';
			voiceCount[j] = 0;
			LoadSfxSet(frogPool[player[j].character].fileName, &soundList.voiceBank[j],0,&voiceArray[j][0],&voiceCount[j]);
		}
	}

	if ( worldID == 0xffff )
	{
		return 0;
	}
	else if(gameState.multi == SINGLEPLAYER)
	{
		switch( worldID )
		{
			case WORLDID_GARDEN: sprintf( path, "GARDEN" ); break;
			case WORLDID_ANCIENT: sprintf( path, "ANCIENTS" ); break;
			case WORLDID_SPACE: sprintf( path, "SPACE" ); break;
			case WORLDID_CITY: sprintf( path, "CITY" ); break;
			case WORLDID_SUBTERRANEAN: sprintf( path, "SUB" ); break;
			case WORLDID_LABORATORY: sprintf( path, "LAB" ); break;
			case WORLDID_HALLOWEEN: sprintf( path, "HALLOWEEN" ); break;
			case WORLDID_SUPERRETRO: sprintf( path, "SUPERRETRO" ); break;
			case WORLDID_FRONTEND: sprintf( path, "FRONTEND" ); break;
		}
	}
	else
		sprintf( path, "MULTI" );

	LoadSfxSet(path, &soundList.levelBank,0,&soundList.array[0],&soundList.count);

	genSfx[GEN_FROG_HOP] = FindSample(utilStr2CRC("hopongrass"));
	genSfx[GEN_SUPER_HOP] = FindSample(utilStr2CRC("hop2"));
	genSfx[GEN_DOUBLE_HOP] = FindSample(utilStr2CRC("doublehop"));
	genSfx[GEN_COLLECT_BABY] = FindSample(utilStr2CRC("getbabyfrog"));
	genSfx[GEN_COLLECT_COIN] = FindSample(utilStr2CRC("pickupcoin"));
	genSfx[GEN_BABYHAPPY] = FindSample(utilStr2CRC("babyhappy"));
	genSfx[GEN_BABYSAD] = FindSample(utilStr2CRC("babysad"));
	genSfx[GEN_BABYCRY] = FindSample(utilStr2CRC("babycry"));
	genSfx[GEN_BABYREPLY] = FindSample(utilStr2CRC("babyreply"));
	genSfx[GEN_TELEPORT] = FindSample(utilStr2CRC("teleport"));

	genSfx[GEN_POWERUP] = FindSample(utilStr2CRC("powerup"));

	genSfx[GEN_CLOCKTICK] = FindSample(utilStr2CRC("clocktick"));
	genSfx[GEN_POWERTICK] = FindSample(utilStr2CRC("puptick"));

	genSfx[GEN_DEATHDROWN] = FindSample(utilStr2CRC("frogdrown1"));
	genSfx[GEN_DEATHCRUSH] = FindSample(utilStr2CRC("frogcrush"));
	genSfx[GEN_DEATHEXPLODE] = FindSample(utilStr2CRC("frogexplode"));
	genSfx[GEN_DEATHFALL] = FindSample(utilStr2CRC("frogfall"));
	genSfx[GEN_DEATHMOWED] = FindSample(utilStr2CRC("frogmowed"));
	genSfx[GEN_DEATHGIB] = FindSample(utilStr2CRC("frog_gib"));
	genSfx[GEN_DEATHCHOP] = FindSample(utilStr2CRC("frogchop"));
	genSfx[GEN_DEATHELECTRIC] = FindSample(utilStr2CRC("electrocute"));
	genSfx[GEN_DEATHFIRE] = FindSample(utilStr2CRC("burnbum"));

	return 1;
}



/*	--------------------------------------------------------------------------------
	Function		:CreateAndAddSamples
	Purpose			: Adds a sample to the sample list
	Parameters		: The sound effect to be added to the sample structure
	Returns			: 
	Info			: 
*/
SAMPLE *CreateAndAddSample(SfxSampleType *snd,int flags,SAMPLE *array,short *count)
{
	SAMPLE *newItem;
	
	if(( soundList.count >= MAX_SAMPLES ) || ( !( newItem = array + *count)))
		return NULL;

	//sfx = (SAMPLE *)MALLOC0(sizeof(SAMPLE));

	newItem->uid = snd->nameCRC;
	newItem->snd = snd;
	newItem->flags = flags;

	(*count)++;

	return newItem;
}

/*	--------------------------------------------------------------------------------
	Function		: FindSample
	Purpose			: Search the list for the required sound
	Parameters		: ID
	Returns			: Sample or NULL
	Info			: 
*/

SAMPLE *FindSample( unsigned long uid )
{
	int i;

	if (!uid)
		return NULL;

	for(i = 0;i < soundList.count;i++)
	{
		if(soundList.array[i].uid == uid)
			return &soundList.array[i];
	}

	return NULL;	
}

void FreeSampleBlock( )
{
	soundList.count = 0;
}

/*	--------------------------------------------------------------------------------
	Function		:FreeSampleList
	Purpose			:Clears the whole sample list and the PSX sound memory
	Parameters		: 
	Returns			: 
	Info			: 
*/

void FreeSampleList( )
{
	int		i;
	SAMPLE 	*cur,*next;
	KTBOOL	ktflag;
	KTU32	memfreeBefore,memfreeAfter;

	amHeapGetFree(&memfreeBefore);
	
	FreeSampleBlock( );

	for(i=(numSoundBanks-1);i>=0;i--)
	{
		ktflag = amHeapFree((unsigned long*)audio64Banks[i]);
		syFree(sampleBanks[i].sample);
	}
	numSoundBanks = 0;

	amHeapGetFree(&memfreeAfter);

	// initialise list for future use
	InitSampleList();					
}

/*	--------------------------------------------------------------------------------
	Function		: PlaySample
	Purpose			: plays a sample
	Parameters		: ID, position, radius, volume, pitch
	Returns			: success?
	Info			: Pass in a valid vector to get attenuation, and a radius to override the default
*/

int sfxwaittime = 5;
int PlaySample( SAMPLE *sample, SVECTOR *pos, long radius, short volume, short pitch )
{
	int vl,vr;
	int vs;

	if (!sample)
	{
		utilPrintf("Sample Not Valid!!!!!!\n");
		return 0;
	}


//Stuff to ensure calls to PlaySample can be the same for PC and PSX
	if (pitch ==-1)
	{
		pitch = 0;
	}
	else
	{
		pitch *= PITCH_STEP;
	}
//end of stuff

	if( pos )
	{
		if(GetSoundVols(pos,&vl,&vr,radius,volume) == -1)
			return -1;
	}
	else
	{
		vr = vl = (volume*globalSoundVol)/MAX_SOUND_VOL;
	}

	if ( !sample->snd )
		return 0;

//	vs = VSync(1);
//	while((lastSound>=0) && (SpuGetKeyStatus(1<<lastSound)==SPU_ON_ENV_OFF) && (VSync(1)<vs+sfxwaittime));

//	if(sample->flags)
//		utilPrintf("playing looping sample the wrong way!!!!!!!!\n");
	
	sample->handle = lastSound = sfxPlaySample( sample->snd, vl,vr, pitch);
	if(lastSound<0)
		utilPrintf("SOUND NOT WORKED (%i RETURNED)\n",lastSound);
		
	return lastSound;	
}

 /*	--------------------------------------------------------------------------------
 	Function 	: AddAmbientSound
 	Purpose 	: Create an ambient sound
 	Parameters 	: 
 	Returns 	: 
 	Info 		:
 */

 AMBIENT_SOUND *AddAmbientSound(SAMPLE *sample, SVECTOR *pos, long radius, short vol, short pitch, float freq, float randFreq, ACTOR *follow )
 {
 	AMBIENT_SOUND *ptr;
 	AMBIENT_SOUND *ambientSound;
 
 	if( !sample )
 		return NULL;
// 	if(ambientSoundList.numEntries != 0)
 //		return NULL;
 
 	ambientSound = (AMBIENT_SOUND *)MALLOC0(sizeof(AMBIENT_SOUND));
 	ptr = &ambientSoundList.head;
 
 	if( pos ) SetVectorSS( &ambientSound->pos, pos );
 	if( follow ) ambientSound->follow = follow;
 
 	ambientSound->volume = vol;
 	ambientSound->sample = sample;
 	ambientSound->pitch = pitch;
 	ambientSound->radius = radius;// * SCALE;//does this need scaling????
 
 	ambientSound->freq = freq*60;
 	ambientSound->randFreq = randFreq*60;
 
 	ambientSound->prev = ptr;
 	ambientSound->next = ptr->next;
 	ptr->next->prev = ambientSound;
 	ptr->next = ambientSound;
 	ambientSoundList.numEntries++;

	ambientSound->handle = -1;

 	return ambientSound;
 }
 
 
 /*	--------------------------------------------------------------------------------
 	Function 	: UpdateAmbientSounds
 	Purpose 	: Start sound effect if close enough, if timeout, etc
 	Parameters 	: 
 	Returns 	: 
 	Info 		:
 */
 void UpdateAmbientSounds()
 {
 	AMBIENT_SOUND *amb,*amb2;
 	SVECTOR *pos;

//	return;
	
 	// Update each ambient in turn
 	for( amb = ambientSoundList.head.next; amb != &ambientSoundList.head; amb = amb2 )
 	{
 		amb2 = amb->next;
 
 		// Timeout, so play a new sound
 		if( actFrameCount < amb->counter )
 			continue;
 
 		// If it is attached to a platform, make ambient follow that platform
 		if( amb->follow )
 			SetVectorSS( &amb->pos, &amb->follow->position );
 
 		// If sound doesn't have a source
 //		if( MagnitudeSquared(&amb->pos) )	//PUT BACK IN?!?!
 //			pos = NULL;	//PUT BACK IN?!?!
 //		else	//PUT BACK IN?!?!
 			pos = &amb->pos;
 
		if(amb->sample->snd->pad & 1)
			UpdateLoopingSample(amb);
		else
		{		
			if(PlaySample(amb->sample, &amb->pos, amb->radius, amb->volume, amb->pitch ) != -1)
			{
				amb->sample->snd->pad = 1;
				amb->handle = amb->sample->handle;
			}
		}
		 
 		// Freq and randFreq are cunningly pre-multiplied by 60
 		amb->counter = actFrameCount + amb->freq + ((amb->randFreq)?Random(amb->randFreq):0);
 	} 	
 }
 
 
 
 
 
 void InitAmbientSoundList()
 {
 	ambientSoundList.head.next = ambientSoundList.head.prev = &ambientSoundList.head;
 	ambientSoundList.numEntries = 0;
 }
 
 
 
 
 
 void SubAmbientSound(AMBIENT_SOUND *ambientSound)
 {
 	if(ambientSound->handle != -1)
		sfxStopChannel(ambientSound->handle);
 
 	ambientSound->prev->next = ambientSound->next;
 	ambientSound->next->prev = ambientSound->prev;
 	ambientSoundList.numEntries--;
 
 	FREE( ambientSound );
 }
 
 
 void FreeAmbientSoundList( )
 {
 	AMBIENT_SOUND *cur,*next;

	utilPrintf("Freeing Schmambient sound list\n");
 
 	// check if any elements in list
 	if( !ambientSoundList.numEntries )
 		return;
 
 	// traverse enemy list and free elements

 	for( cur = ambientSoundList.head.next; cur != &ambientSoundList.head; cur = next )
 	{
 		next = cur->next;
 
 		SubAmbientSound( cur );
	 	if(cur->handle != -1)
			sfxStopChannel(cur->handle);
 	}
 

 	// initialise list for future use
 	InitAmbientSoundList();
}

/*	--------------------------------------------------------------------------------
	Function		: PrepareSongForLevel
	Purpose			: loads and starts playback of song for specified level
	Parameters		: short,short
	Returns			: void
	Info			: 
*/
void PrepareSong(short worldID,int loop)
{
	KTU32	memfreeBefore,memfreeAfter;
	int 	chan;
	int 	xaNum = 0;
	char	buffer[32];

//	return;
	
	if(!bpAmStreamDone(gStream))
		StopSong();
	
	amHeapGetFree(&memfreeBefore);
	
	switch ( worldID )
	{
		case AUDIOTRK_GAMEOVER:				worldID = 10; xaNum = 1; break;
		case AUDIOTRK_LEVELCOMPLETE:		worldID = 9; xaNum = 1; break;
		case AUDIOTRK_LEVELCOMPLETELOOP:	worldID = 11; xaNum = 1; break;
	}

	chan = musicList  [ worldID ] + 1;

	if(chan < 10)
		sprintf(buffer,"track0%d.str",chan);
	else
		sprintf(buffer,"track%d.str",chan);
	
	// Set up the buffers etc for the stream
	if(StreamSetup(buffer,1)==KTFALSE)
	{
		acASEBRK(KTTRUE);
	}
			
	// Start the stream
	bpAmStreamStart(gStream);	
	
	amHeapGetFree(&memfreeAfter);	
}

void StopSong( )
{
	KTU32	memfreeBefore,memfreeAfter;

	amHeapGetFree(&memfreeBefore);
	
	bpAmStreamDestroy(gStream);
	StreamDestroy();
	
	amHeapGetFree(&memfreeAfter);
}

long PAN_MAX = 4096*10;
int GetSoundVols(SVECTOR *pos,int *vl,int *vr,long radius,unsigned long vol)
{
	fixed att, dist;
	SVECTOR diff;
	long pan = 0;
	SVECTOR m;
	SVECTOR tempSvect;
	
	vol = (vol * globalSoundVol)/MAX_SOUND_VOL;

	if(vol == 0)
		return -1;

	att = (radius)?radius/*/SCALE*/:DEFAULT_SFX_DIST;
	att = att<<12;

	SubVectorSSF(&diff, pos, &currCamSource);
//	SubVectorSSS( &diff, pos, &frog[0]->actor->position );
	// Volume attenuation - check also for radius != 0 and use instead of default
	dist = MagnitudeS(&diff);
	if( dist > (att) )
		return -1;

	vol = (FDiv(att-dist,att)*vol)>>12;

	tempSvect.vx = -pos->vx;
	tempSvect.vy = -pos->vy;
	tempSvect.vz = pos->vz;

	gte_ldv0(&tempSvect);
	gte_rtps();
	gte_stsxy((long *)&m.vx);
//	gte_stsz(&m.vz);	//screen z/4 as otz
	m.vz = (screenxy[2].vz >> 2);
//	m.vz >>= 2;

	dist = MagnitudeS(&m);
	if(dist)
	{
		pan = (m.vx*PAN_MAX)/dist;
		pan *= 4096;
	}
	else
		pan = 0;

	if(pan >= 0)
	{
		*vl = ((PAN_MAX - pan)*vol)/PAN_MAX;
		*vr = vol;
	}
	else
	{
		*vl = vol;
		*vr = ((PAN_MAX + pan)*vol)/PAN_MAX;
	}
	
	return TRUE;
}


SAMPLE *FindVoice( unsigned long uid, int pl )
{
	int j;

	for(j = 0;j < voiceCount[pl];j++)
		if(voiceArray[pl][j].uid == uid)
			return &voiceArray[pl][j];

	return NULL;
}

void StopSample(SAMPLE *sample)
{
	if((sample) && (sample->handle != -1))
	{
		sfxStopChannel(sample->handle);
		sample->handle = -1;
	}
}

void PauseAudio( )
{
	// CD Pause: Possibly should check return value... Nah, can't be bothered.
//	SpuSetCommonCDVolume(0, 0);

	// Mute sound
//	SsSetMute( 1 );
	bpAmStreamSetVolume(gStream, 0);
}

void UnPauseAudio( )
{
	// CD Resume
//	SpuSetCommonCDVolume((0x7fff*globalMusicVol)/MAX_SOUND_VOL, (0x7fff*globalMusicVol)/MAX_SOUND_VOL);
	
	// Unmute sound
//	SsSetMute( 0 );
	bpAmStreamSetVolume(gStream, 127);
}

void SpuSetCommonCDVolume(int volume, int volume2)
{
	float	newVolume;
	
	newVolume = (float)globalMusicVol * (2.55);
	bpAmStreamSetVolume(gStream, (int)newVolume);
}

int IsSongPlaying()
{
	if(bpAmStreamDone(gStream))
		return TRUE;
		
	return	FALSE;
}

/**************************************************************************
	FUNCTION:	sfxPlaySample()
	PURPOSE:	Plays the requested sample
	PARAMETERS:	pointer to sample, left & right volume (0 - 255), pitch (Hz, 0 for default)
	RETURNS:	channel number, or -1 if failure
**************************************************************************/

int sfxPlaySample(SfxSampleType *sample, int volL, int volR, int pitch)
{
	AM_SOUND	*sound;
	int			volAverage,volPan,channel;
	
	for(channel=0; channel<24; channel++)
	{
		if(!current[channel].sound.isPlaying)
			break;
	}

	if(channel==24) return;

	if(volL||volR)
	{
		volPan = (127 * volL)/(volL+volR);
		volAverage = volL > volR ? volL : volR;
	}
	else
	{
		volPan = 64;
		volAverage = 0;
	}

	current[channel].sample = sample;
	current[channel].volume = (volAverage*sfxGlobalVolume)/512;

//	volAverage = sfxOutputOn ? current[channel].volume : 0;

	sound = bpAmPlaySoundEffect(audio64Banks[sample->bankNumber],&current[channel].sound,sample->sampleNumber,volAverage,volPan);

	if((pitch>0) && sound) 
		amSoundSetCurrentPlaybackRate(sound,pitch);

	return channel;
}

/**************************************************************************
	FUNCTION:	sfxInitialise()
	PURPOSE:	Initialise the sound library
	PARAMETERS:	reverb mode (not supported in this version)
	RETURNS:	none
**************************************************************************/

void sfxInitialise(int reverbMode)
{
	int loop;

	utilInitCRC();//TG: Temp fix

//	if(!bpAmSetup(AC_DRIVER_DA, KTFALSE, KTNULL))
//		for(;;);

//	acSetTransferMode(AC_TRANSFER_DMA);
//	acSetTransferMode(AC_TRANSFER_CPU);
		
//	acSystemDelay(500000);

//	acErr = acErrorGetLast();
//	amErr = amErrorGetLast();

	//Initialise all sample banks to null so we know they aren't in use
	for(loop=0; loop<MAX_SAMPLE_BANKS; loop++)
	{
		sampleBanks[loop].used = 0;
		audio64Banks[loop] = NULL;
	}

	for(loop=0; loop<24; loop++)
	{
		current[loop].sound.isPlaying = 0;
		current[loop].sample = NULL;
	}

	amSystemSetVolumeMode(USELINEAR);

	sfxGlobalVolume = 255;
	sfxOutputOn = 1;
		
//	a64FileInit();
}


/**************************************************************************
	FUNCTION:	sfxDestroy()
	PURPOSE:	Shutdown the sound library
	PARAMETERS:	none
	RETURNS:	none
**************************************************************************/

void sfxDestroy()
{
//	acSystemShutdown();
//	amShutdown();
}


/**************************************************************************
	FUNCTION:	sfxUpdate()
	PURPOSE:	update the sound queue
	PARAMETERS:	none
	RETURNS:	none
**************************************************************************/

void sfxUpdate()
{
}

/**************************************************************************
	FUNCTION:	sfxOn()
	PURPOSE:	Turn sound output on (emulated on DC)
	PARAMETERS:	none
	RETURNS:	none
**************************************************************************/

void sfxOn()
{
	int channel;

	sfxOutputOn = 1;

	for(channel=0; channel<24; channel++)
	{
		if(current[channel].sound.isPlaying)
			amSoundSetVolume(&current[channel].sound,current[channel].volume);
	}
}


/**************************************************************************
	FUNCTION:	sfxOff()
	PURPOSE:	Turn sound output off (emulated on DC)
	PARAMETERS:	none
	RETURNS:	none
**************************************************************************/

void sfxOff()
{
	int channel;

	for(channel=0; channel<24; channel++)
	{
		if(current[channel].sound.isPlaying)
			amSoundSetVolume(&current[channel].sound,0);
	}

	sfxOutputOn = 0;
}

/**************************************************************************
	FUNCTION:	sfxSetGlobalVolume()
	PURPOSE:	Set global sample volume
	PARAMETERS:	volume
	RETURNS:	none
**************************************************************************/

void sfxSetGlobalVolume(int vol)
{
	if(vol<0) vol = 0;
	if(vol>255) vol = 255;
	sfxGlobalVolume = vol;
}

/**************************************************************************
	FUNCTION:	sfxStopChannel
	PURPOSE:	Stop a channel from playing
	PARAMETERS:	channel number (0 - 23), or -1 for all
	RETURNS:	none
**************************************************************************/

void sfxStopChannel(int channel)
{
	if(current[channel].sound.isPlaying)
		amSoundStop(&current[channel].sound);
}

/**************************************************************************
	FUNCTION:	sfxSetChannelPitch()
	PURPOSE:	set pitch of channel
	PARAMETERS:	channel number, pitch (Hz)
	RETURNS:	none
**************************************************************************/

void sfxSetChannelPitch(int channel, int pitch)
{
	if(current[channel].sound.isPlaying) 
		amSoundSetCurrentPlaybackRate(&current[channel].sound,pitch);
}


/**************************************************************************
	FUNCTION:	sfxSetChannelVolume()
	PURPOSE:	set volume of channel
	PARAMETERS:	channel number, left vol, right vol (0 - 255)
	RETURNS:	none
**************************************************************************/

void sfxSetChannelVolume(int channel, int volL, int volR)
{
	int			volAverage,volPan;

	if(!current[channel].sound.isPlaying) return;
	
	if(volL||volR)
	{
		volPan = (127 * volL)/(volL+volR);
		volAverage = volL > volR ? volL : volR;
	}
	else
	{
		volPan = 0;
		volAverage = 0;
	}

	current[channel].volume = (volAverage*sfxGlobalVolume)/512;

	volAverage = sfxOutputOn ? current[channel].volume : 0;

	amSoundSetPan(&current[channel].sound,volPan);

	amSoundSetVolume(&current[channel].sound,volAverage);
}

/**************************************************************************
	FUNCTION:	sfxStartSound()
	PURPOSE:	Start sound DMA processing
	PARAMETERS:	none
	RETURNS:	none
**************************************************************************/

void sfxStartSound()
{
}

/**************************************************************************
	FUNCTION:	sfxStopSound()
	PURPOSE:	Stop sound DMA processing (DC just cuts all samples)
	PARAMETERS:	none
	RETURNS:	none
**************************************************************************/

void sfxStopSound()
{
	int channel;

	for(channel=0; channel<24; channel++)
	{
		if(current[channel].sound.isPlaying)
			amSoundSetVolume(&current[channel].sound,0);
	}
}