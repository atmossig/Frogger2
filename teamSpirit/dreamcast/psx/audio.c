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

#define SFX_BASE		"SFX\\"

 
//#define MAX_AMBIENT_SFX		50
#define DEFAULT_SFX_DIST	500*SCALE

//This value means the pitch can go up to 100000Hz like the PC. This might be a 
//problem for the PSX but I don't think it goes much above 20000Hz, which seems alright. May
//cause problems in the future though, so check!!
#define PITCH_STEP		390 //(DSBFREQUENCY_MAX/256)   //MM NEEDS CHANGING!!!!!

 
//#define VOLUME_MIN		-5000
//#define VOLUME_PERCENT (VOLUME_MIN/100)
//#define PITCH_STEP		(DSBFREQUENCY_MAX/256)
 
SAMPLE *genSfx[NUM_GENERIC_SFX];
// SAMPLE **sfx_anim_map = NULL;

//XAFileType	*xaFileData[NUM_CD_TRACKS];
 
// UINT mciDevice = 0;
 
SOUNDLIST soundList;					// Actual Sound Samples List
AMBIENT_SOUND_LIST	ambientSoundList;

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

char generic_names[][32] =
{
"frogfall",
"babyhappy",
"babyreply",
"babysad",
"doublehop",
"frogannoyed",
"frogbelch2",
"frogbored",
"frogchop",
"frogcroak",
"frogcrush",
"frogdeath",
"frogdrown1",
"frogdrown2",
"frogexplode",
"babycry",
"frogletsgo",
"frogmowed",
"frogokay",
"frogslide2",
"froguhoh",
"getbabyfrog",
"hop2",
"hopongrass",
"pickupcoin",
};
 
char garden_names[][32] =
{
"lp_area1",
"lp_mowrs",
"lp_wwheel",
};
 
/*	--------------------------------------------------------------------------------
	Function		:StartSound
	Purpose		    :  Initialises the PSX sound chip and the soundlibraries
	Parameters		: none
	Returns			: 
	Info			: Should only be called once at the start of the game loop. 
*/
void StartSound()
{
//ma	sfxInitialise(0);
//ma 	sfxStartSound();
//ma 	sfxOn();
//ma 	sfxSetGlobalVolume(255);// set to max volume 
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
//free the sfxBankType pointers
	FREE((UBYTE**)soundList.genericBank);
	FREE((UBYTE**)soundList.levelBank);
//ma	sfxOff();
//ma	sfxStopSound();
//ma	sfxDestroy();
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
	soundList.numEntries	= 0;
	soundList.head.next		= soundList.head.prev = &soundList.head;

/*ma
	if(!soundList.genericBank)//first time around
	{
		soundList.genericBank = (SfxBankType *)MALLOC0(sizeof(SfxBankType));
		soundList.levelBank = (SfxBankType *)MALLOC0(sizeof(SfxBankType));
	}
	else
	{
		soundList.genericBank = NULL;
		soundList.levelBank = NULL;
	}
*/
}

/*	--------------------------------------------------------------------------------
	Function		: LoadSfxSet
	Purpose			: Loads the soundbank and adds samples to list
	Parameters		: path to be loaded, adn whether the bank is the generic bank
	Returns			: Success
	Info			: 
*/

int LoadSfxSet( char *path, int generic )
{
	int 			i;
	SAMPLE 			*sfx;
	AM_BANK_PTR		*sfxBank;

	if((sfxBank = bpAmLoadBank(path))==KTNULL)
		return 0;

	if(generic)
	{
		for(i=0;i<25;i++)
		{
			sfx = (SAMPLE*)MALLOC0(sizeof(SAMPLE));
			
			sprintf(sfx->idName,"%s",generic_names[i]);		
			sfx->uid = utilStr2CRC(sfx->idName);
			sfx->flags = 0;
		
			sfx->bankPtr = sfxBank;
			sfx->sampleNumber = i;

			AddSample(sfx);
		}
	}
	else
	{
		for(i=0;i<3;i++)
		{
			sfx = (SAMPLE*)MALLOC0(sizeof(SAMPLE));
			
			sprintf(sfx->idName,"%s",garden_names[i]);		
			sfx->uid = utilStr2CRC(sfx->idName);
			sfx->flags = 0;
		
			sfx->bankPtr = sfxBank;
			sfx->sampleNumber = i;

			AddSample(sfx);
		}	
	}
	
/*	sfxBank = sfxLoadSampleBank(path);	
	if(!sfxBank)
	{
	 	utilPrintf("Can't load sound bank %s\n", path);
		return 0;
	}

	snd =(SfxSampleType *) sfxBank->sample;
	for  ( i=0; i<sfxBank->numSamples; i++)
	{
		if (!snd)
		{
			utilPrintf("oops...can't find sample %i in bank %s\n",i,path);
			continue;
		}
		snd = sfxDownloadSample(snd);
		CreateAndAddSample(snd);

		*snd++;
	}
*/	
	if(generic)
		soundList.genericBank = sfxBank;
	else
		soundList.levelBank = sfxBank;
		
// 	sfxDestroySampleBank( sfxBank );		

	return 1;
}

/*	--------------------------------------------------------------------------------
	Function		: LoadSfx
	Purpose			: Loads a sound bank, and puts all the samples in the sample list
	Parameters		: number of world sound bank to load
	Returns			: Success?
	Info			: 
*/
int LoadSfx( unsigned long worldID )
{
	int		len,i;
	char	path[256];
	
	// Load all generic samples first and put in array
	LoadSfxSet("GENERIC.KAT", 1);

	for(i=0;i<NUM_GENERIC_SFX;i++)
		genSfx[i] = NULL;
	
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

// JH: changed file name to some thing else coz we don't have this one yet....
//	genSfx[GEN_POWERUP] = FindSample(utilStr2CRC("hopongrass"));

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

	return;
	
	//load the non-looping level samples
	switch(worldID)
	{
		case WORLDID_GARDEN:
			strcat( path, "SFX\\GARDEN.KAT" );
			break;
		case WORLDID_ANCIENT:
			strcat( path, "SFX\\ANCIENTS" );
			break;
		case WORLDID_SPACE:
			strcat( path, "SFX\\SPACE.KAT" );
			break;
		case WORLDID_CITY:
			strcat( path, "SFX\\CITY.KAT" );
			break;
		case WORLDID_SUBTERRANEAN:
			strcat( path, "SFX\\SUB.KAT" );
			break;
		case WORLDID_LABORATORY:
			strcat( path, "SFX\\LAB.KAT" );
			break;
		case WORLDID_HALLOWEEN:
			strcat( path, "SFX\\HALLOWEEN.KAT" );
			break;
		case WORLDID_SUPERRETRO:
			strcat( path, "SFX\\SUPERRETRO.KAT" );
			break;
		case WORLDID_FRONTEND:
			strcat( path, "SFX\\FRONTEND.KAT" );
			break;
	}

	LoadSfxSet(path, 0);

/* 	len = strlen(path);

	strcat( path, "SFX" );

	LoadSfxSet( path, 0 );

	//load the looping level samples
	path[len] = '\0';

	strcat( path, "LOOP" );
	LoadSfxSet( path, 0 );
*/
	return 1;
}




/*	--------------------------------------------------------------------------------
	Function		:CreateAndAddSamples
	Purpose			: Adds a sample to the sample list
	Parameters		: The sound effect to be added to the sample structure
	Returns			: 
	Info			: 
*/
SAMPLE *CreateAndAddSample( SfxSampleType *snd )
{
	SAMPLE *sfx;
	
	sfx = (SAMPLE *)MALLOC0(sizeof(SAMPLE));

	sfx->uid = snd->nameCRC;
//ma	sfx->snd = snd;

	AddSample( sfx );

	return sfx;
}

/*	--------------------------------------------------------------------------------
	Function		: AddSample
	Purpose			: Adds a sample structure to the list
	Parameters		: The sample to be added 
	Returns			: 
	Info			: 
*/
void AddSample( SAMPLE *sample )
{
	if( !sample->next )
	{
		soundList.numEntries++;
		sample->prev				= &soundList.head;
		sample->next				= soundList.head.next;
		soundList.head.next->prev	= sample;
		soundList.head.next			= sample;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: RemoveSample
	Purpose			: Removes a sample from the list
	Parameters		: Sample to be removed
	Returns			: 
	Info			: 
*/

void RemoveSample( SAMPLE *sample )
{
	if( !sample->next )
		return;

	sample->prev->next	= sample->next;
	sample->next->prev	= sample->prev;
	sample->next		= NULL;
	soundList.numEntries--;

	FREE( (UBYTE **)sample );
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
	SAMPLE *next, *cur;

	if (!uid)
		return NULL;

	for( cur = soundList.head.next; cur != &soundList.head; cur = next )
	{
		next = cur->next;
		
		if( cur->uid == uid )
			return cur;
	}

	return NULL;	
}

/*	--------------------------------------------------------------------------------
	Function		: FindVoice
	Purpose			: Search the list for the required sound
	Parameters		: ID
	Returns			: Sample or NULL
	Info			: 
*/

SAMPLE *FindVoice( unsigned long uid, int pl )
{
	SAMPLE *next, *cur;

	if (!uid)
		return NULL;

	for( cur = soundList.head.next; cur != &soundList.head; cur = next )
	{
		next = cur->next;
		
		if( cur->uid == uid )
			return cur;
	}

	return NULL;	
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
	SAMPLE 	*cur,*next;
	KTBOOL	ktflag;
	KTU32	memfreeBefore,memfreeAfter;

	utilPrintf("Freeing linked list : SAMPLE : (%d elements)\n",soundList.numEntries);

	amHeapGetFree(&memfreeBefore);
	
	// check if any elements in list
	if( !soundList.numEntries )
		return;

	// traverse enemy list and free elements
	for( cur = soundList.head.next; cur != &soundList.head; cur = next )
	{
		next = cur->next;

		RemoveSample( cur );
	}

	amHeapGetFree(&memfreeBefore);
//	ktflag = amHeapFree((unsigned long*)soundList.genericBank);
	ktflag = amHeapFree(soundList.genericBank);
	amHeapGetFree(&memfreeAfter);

//	amHeapFree((unsigned long*)soundList.genericBank);

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

int PlaySample( SAMPLE *sample, SVECTOR *pos, long radius, short volume, short pitch )
{
//	unsigned long vol=volume;
	fixed att, dist;
	SVECTOR diff;
 	int vl,vr;
	int i;
	
/*	//Stuff to ensure calls to PlaySample can be the same for PC and PSX
	if (pitch ==-1)
		pitch = 0;
	else
		pitch *= PITCH_STEP;

	if( pos )
	{
		att = (radius)?radius:DEFAULT_SFX_DIST;
		att = att<<12;

		SubVectorSSS( &diff, pos, &frog[0]->actor->position );
		// Volume attenuation - check also for radius != 0 and use instead of default
		dist = MagnitudeS( &diff );
		if( dist > (att) )
			return FALSE;

		vol = (FDiv(att-dist,att)*vol)>>12;
	}

	vl = vol;
	vr = vol;
*/
	if(sample)
	{
		if(sample->bankPtr)
			bpAmPlaySoundEffect(sample->bankPtr,sample->sampleNumber, 127, AM_PAN_CENTER);
	}
	
	return i;
}

/*	--------------------------------------------------------------------------------
	Function		: PlayVoice
	Purpose			: plays a sample
	Parameters		: ID, position, radius, volume, pitch
	Returns			: success?
	Info			: Pass in a valid vector to get attenuation, and a radius to override the default
*/

int PlayVoice(int player, char *name)
{
	long 	vol = 127;
	fixed 	att, dist;
	SVECTOR diff;
 	int 	vl,vr;
	int 	i;
	SAMPLE	*sample;

	sample = FindSample(utilStr2CRC(name));
	if(sample == NULL)
		return 0;
		
	if(sample)
	{
		if(sample->bankPtr)
			bpAmPlaySoundEffect(sample->bankPtr,sample->sampleNumber, 127, AM_PAN_CENTER);
	}
				
	return i;
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
 
 	if( !sample ) return NULL;
 
 	ambientSound = (AMBIENT_SOUND *)MALLOC0(sizeof(AMBIENT_SOUND));
 	ptr = &ambientSoundList.head;
 
 	if( pos ) SetVectorSS( &ambientSound->pos, pos );
 	if( follow ) ambientSound->follow = follow;
 
 	ambientSound->volume = vol;
 	ambientSound->sample = sample;
 	ambientSound->pitch = pitch;
 	ambientSound->radius = radius * SCALE;//does this need scaling????
 
 	ambientSound->freq = freq*60;
 	ambientSound->randFreq = randFreq*60;
 
 	ambientSound->prev = ptr;
 	ambientSound->next = ptr->next;
 	ptr->next->prev = ambientSound;
 	ptr->next = ambientSound;
 	ambientSoundList.numEntries++;
 
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
 
 		PlaySample( amb->sample, &amb->pos, amb->radius, amb->volume, amb->pitch );
 
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
 //	if( ambientSound->sample )
// 		sfxStopSample(ambientSound->sample, -1);
 
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

}

int IsSongPlaying()
{
	if(bpAmStreamDone(gStream))
		return TRUE;
		
	return	FALSE;
}