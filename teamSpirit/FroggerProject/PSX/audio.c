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

#include "audio.h"

#include "main.h"

#include "memload.h"
#include "frogger.h"
#include "layout.h"
#include "menus.h"



char *musicNames[] = { "CD2.XA",//
					   "CD3.XA",//
					   "CD4.XA",//
					   "CD5.XA",//
					   "CD6.XA",//
					   "CD7.XA",//
					   "CD8.XA",//
					   "CD9.XA",//
					   "CD10.XA",//
					   "CD11.XA"};//


/*
char *musicNames[] = { "CD3.XA",//
					   "CD3.XA",
					   "CD4.XA",
					   "CD3.XA",//
					   "CD3.XA",//
					   "CD4.XA",//
					   "CD4.XA",
					   "CD4.XA",//
					   "CD4.XA",//
					   "CD3.XA"};//
*/




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
 
SAMPLE *genSfx[NUM_GENERIC_SFX];
// SAMPLE **sfx_anim_map = NULL;

//bbXAFileType	*xaFileData[NUM_CD_TRACKS];
XAFileType	*xaFileData[MUSIC_NUM_TRACKS];
 
// UINT mciDevice = 0;
 
SOUNDLIST soundList;					// Actual Sound Samples List
AMBIENT_SOUND_LIST	ambientSoundList;
//BUFFERLIST bufferList;					// Buffered Sound Samples List
 
 
// void AddSample( SAMPLE *sample );
// void RemoveSample( SAMPLE *sample );
// void AddBufSample( BUFSAMPLE *sample );
// void RemoveBufSample( BUFSAMPLE *sample );
 
// void SubAmbientSound(AMBIENT_SOUND *ambientSound);


int GetSoundVols(SVECTOR *pos,int *vl,int *vr,long radius,unsigned long vol);

int UpdateLoopingSample(AMBIENT_SOUND *sample)
{
	int vl,vr;
	short pitch;

	if ( !sample )
		return;
	
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

	if ( (sample->handle == -1) && ( sample->sample ) && (sample->sample->snd) )
	{
 		sample->handle = sfxPlaySample(sample->sample->snd, vl,vr, pitch);
		return;
	}

	if(pitch)
		sfxSetChannelPitch(sample->handle,pitch);
	sfxSetChannelVolume(sample->handle, vl, vr);
}
 
// SAMPLE *CreateAndAddSample( char *path, char *file );
 
// void SetSampleFormat ( SAMPLE *sample );
// void CleanBufferSamples( void );
 
// void PrepareSong( char num, char slot );
// void StopSong( );
// DWORD playCDTrack( HWND hWndNotify, BYTE bTrack );
// DWORD stopCDTrack( HWND hWndNotify );
 
 
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
//free the sfxBankType pointers
	FREE(soundList.genericBank);
	FREE(soundList.levelBank);
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

	soundList.array = NULL;
	soundList.count = 0;
	soundList.head.next = soundList.head.prev = &soundList.head;

}

void AllocSampleBlock ( int num )
{
	if ( !soundList.array )
	{
		utilPrintf ( "Mallocing a list of %d Sample Structures.\n", num );
		soundList.array = ( SAMPLE * ) MALLOC0 ( sizeof ( SAMPLE ) * num );
	}
	// ENDIF

	soundList.count		= 0;
	soundList.blocks	= num;
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
	int i;
	SfxBankType *sfxBank=0;
	SfxSampleType *snd;

	sfxBank = sfxLoadSampleBank(path);	
	if(!sfxBank)
	{
	 	utilPrintf("Can't load sound bank %s\n", path);
		return 0;
	}

	snd =(SfxSampleType *) sfxBank->sample;

	AllocSampleBlock ( sfxBank->numSamples );

	for  ( i=0; i<sfxBank->numSamples; i++)
	{
		if (!snd)
		{
			utilPrintf("oops...can't find sample %i in bank %s\n",i,path);
			continue;
		}
		snd = sfxDownloadSample(snd);
		CreateAndAddSample(snd,generic == -1 ? SFXFLAGS_LOOP : 0);

		*snd++;
	}
	if (generic > 0)
		soundList.genericBank = sfxBank;
	else
		soundList.levelBank = sfxBank;
 	sfxDestroySampleBank( sfxBank );		

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
	char *path;
	int len;
	
 	len = strlen(SFX_BASE);
 	path = (char *)MALLOC0( len+32 );

	strcat( path, SFX_BASE );

	// Load all generic samples first and put in array
	strcat( path, "GENERIC\\SFX" );

	LoadSfxSet(path, 1);

	genSfx[GEN_FROG_HOP] = FindSample(utilStr2CRC("hopongrass"));
	genSfx[GEN_SUPER_HOP] = FindSample(utilStr2CRC("hop2"));
	genSfx[GEN_DOUBLE_HOP] = FindSample(utilStr2CRC("doublehop"));
	genSfx[GEN_COLLECT_BABY] = FindSample(utilStr2CRC("getbabyfrog"));
	genSfx[GEN_FROG_TONGUE] = FindSample(utilStr2CRC("tongue"));
	genSfx[GEN_COLLECT_COIN] = FindSample(utilStr2CRC("pickupcoin"));
	genSfx[GEN_CROAK] = FindSample(utilStr2CRC("frogcroak"));
	genSfx[GEN_BABYHAPPY] = FindSample(utilStr2CRC("babyhappy"));
	genSfx[GEN_BABYSAD] = FindSample(utilStr2CRC("babysad"));
	genSfx[GEN_BABYCRY] = FindSample(utilStr2CRC("babycry"));
	genSfx[GEN_BABYREPLY] = FindSample(utilStr2CRC("babyreply"));
	genSfx[GEN_TELEPORT] = FindSample(utilStr2CRC("teleport"));

// JH: changed file name to some thing else coz we don't have this one yet....
	genSfx[GEN_POWERUP] = FindSample(utilStr2CRC("hopongrass"));

	//genSfx[GEN_POWERUP] = FindSample(utilStr2CRC("powerup"));

	genSfx[GEN_CLOCKTICK] = FindSample(utilStr2CRC("clocktick"));
	genSfx[GEN_POWERTICK] = FindSample(utilStr2CRC("puptick"));

	genSfx[GEN_FROGBELCH1] = FindSample(utilStr2CRC("frogbelch2"));
	genSfx[GEN_FROGBELCH2] = FindSample(utilStr2CRC("frogbelch2"));
	genSfx[GEN_FROGANNOYED] = FindSample(utilStr2CRC("frogannoyed"));
	genSfx[GEN_FROGSLIDE] = FindSample(utilStr2CRC("frogslide2"));
	genSfx[GEN_FROGHAPPY] = FindSample(utilStr2CRC("frogokay"));
	genSfx[GEN_FROGSCARED] = FindSample(utilStr2CRC("froguhoh"));
	genSfx[GEN_FROGBORED] = FindSample(utilStr2CRC("frogbored"));
	genSfx[GEN_FROGLETSGO] = FindSample(utilStr2CRC("frogletsgo"));

	genSfx[GEN_DEATHNORMAL] = FindSample(utilStr2CRC("frogdeath"));
	genSfx[GEN_DEATHDROWN1] = FindSample(utilStr2CRC("frogdrown1"));
	genSfx[GEN_DEATHDROWN2] = FindSample(utilStr2CRC("frogdrown2"));
	genSfx[GEN_DEATHCRUSH] = FindSample(utilStr2CRC("frogcrush"));
	genSfx[GEN_DEATHEXPLODE] = FindSample(utilStr2CRC("frogexplode"));
	genSfx[GEN_DEATHFALL] = FindSample(utilStr2CRC("frogfall"));
	genSfx[GEN_DEATHMOWED] = FindSample(utilStr2CRC("frogmowed"));
	genSfx[GEN_DEATHGIB] = FindSample(utilStr2CRC("frog_gib"));
	genSfx[GEN_DEATHCHOP] = FindSample(utilStr2CRC("frogchop"));
	genSfx[GEN_DEATHELECTRIC] = FindSample(utilStr2CRC("electrocute"));
	genSfx[GEN_DEATHFIRE] = FindSample(utilStr2CRC("burnbum"));

	path[len] = '\0';


	if ( worldID == 0xffff )
		return 0;
	// ENDIF

	utilPrintf ( "WorldID Is : %d\n", worldID );
//load the non-looping level samples
	switch( worldID )
	{
		case WORLDID_GARDEN: strcat( path, "GARDEN\\" ); break;
		case WORLDID_ANCIENT: strcat( path, "ANCIENTS\\" ); break;
		case WORLDID_SPACE: strcat( path, "SPACE\\" ); break;
		case WORLDID_CITY: strcat( path, "CITY\\" ); break;
		case WORLDID_SUBTERRANEAN: strcat( path, "SUB\\" ); break;
		case WORLDID_LABORATORY: strcat( path, "LAB\\" ); break;
		case WORLDID_HALLOWEEN: strcat( path, "HALLOWEEN\\" ); break;
//		case WORLDID_SWAMPYWORLD: strcat( path, "SWAMPYWORLD\\" ); break;
		case WORLDID_SUPERRETRO: strcat( path, "SUPERRETRO\\" ); break;
		case WORLDID_FRONTEND: strcat( path, "FRONTEND\\" ); break;
	}

 	len = strlen(path);

	strcat( path, "SFX" );

	LoadSfxSet( path, 0 );


// JH: Quite Possibly fix it!!!!!!!!!!!!!!!!!!!!!!!!!!
	path[len] = '\0';

/*	switch( worldID )
	{
		case WORLDID_GARDEN: strcat( path, "GARDEN\\" ); break;
		case WORLDID_ANCIENT: strcat( path, "ANCIENTS\\" ); break;
		case WORLDID_SPACE: strcat( path, "SPACE\\" ); break;
		case WORLDID_CITY: strcat( path, "CITY\\" ); break;
		case WORLDID_SUBTERRANEAN: strcat( path, "SUB\\" ); break;
		case WORLDID_LABORATORY: strcat( path, "LAB\\" ); break;
		case WORLDID_HALLOWEEN: strcat( path, "HALLOWEEN\\" ); break;
		case WORLDID_SWAMPYWORLD: strcat( path, "SWAMPYWORLD\\" ); break;
		case WORLDID_SUPERRETRO: strcat( path, "SUPERRETRO\\" ); break;
		case WORLDID_FRONTEND: strcat( path, "FRONTEND\\" ); break;
	}*/

//load the looping level samples
	//path[len] = '\0';

 	len = strlen(path);

	strcat( path, "LOOP" );

	LoadSfxSet( path, -1 );


	FREE( path );

	return 1;
}




/*	--------------------------------------------------------------------------------
	Function		:CreateAndAddSamples
	Purpose			: Adds a sample to the sample list
	Parameters		: The sound effect to be added to the sample structure
	Returns			: 
	Info			: 
*/
SAMPLE *CreateAndAddSample( SfxSampleType *snd, int flags )
{
	SAMPLE *newItem;
	
	if ( ( soundList.count >= soundList.blocks ) || ( !( newItem = &soundList.array [ soundList.count ] ) ) )
		return NULL;

	//sfx = (SAMPLE *)MALLOC0(sizeof(SAMPLE));

	newItem->uid = snd->nameCRC;
	newItem->snd = snd;
	newItem->flags = flags;

	AddSample( newItem );

	return newItem;
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
	SAMPLE *cur;

	if( !sample || sample->next )
		return;

	// If enemy is the first of its kind, add at head of list
	if( !sample->next )
	{
		sample->prev = &soundList.head;
		sample->next = soundList.head.next;
		soundList.head.next->prev = sample;
		soundList.head.next = sample;
	}

	soundList.count++;
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
	/*if( !sample->next )
		return;

	sample->prev->next	= sample->next;
	sample->next->prev	= sample->prev;
	sample->next		= NULL;
	soundList.numEntries--;

	FREE( sample );*/
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

	// WHY???
//	return FindSample ( utilStr2CRC ( "hopongrass" ) );

	return NULL;	
}
 

void FreeSampleBlock( )
{
	if( soundList.array )
		FREE( soundList.array );

	soundList.array = NULL;
	soundList.count = 0;
	soundList.blocks = 0;
}


void SubSample ( SAMPLE *sample )
{
	//n.b paths alloced and freed separately

	if(sample->next == NULL)
		return;

	sample->prev->next = sample->next;
	sample->next->prev = sample->prev;
	sample->next = NULL;
	soundList.count--;
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
	SAMPLE *cur,*next;

	if(soundList.count)
		utilPrintf("Freeing linked list : SAMPLE : (%d elements)\n",soundList.count);

	// traverse enemy list and free elements
	while( soundList.head.next && soundList.head.next != &soundList.head )
		SubSample ( soundList.head.next );

	FreeSampleBlock( );

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
 	int vl,vr;
	int i;

	if ( (!sample) )
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
		GetSoundVols(pos,&vl,&vr,radius,volume);
	}
	else
	{
		vr = vl = (volume*globalSoundVol)/MAX_SOUND_VOL;
	}

	if ( !sample->snd )
		return 0;

	i =	sfxPlaySample( sample->snd, vl,vr, pitch);
	if(i<0)
		utilPrintf("SOUND NOT WORKED (%i RETURNED)\n",i);
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
 
		if(amb->sample->flags & SFXFLAGS_LOOP)
			UpdateLoopingSample(amb);
		else
			PlaySample(amb->sample, &amb->pos, amb->radius, amb->volume, amb->pitch );
 
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
 	if( ambientSound->sample )
 		sfxStopSample(ambientSound->sample, -1);
 
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









void MAIN_PrintXAData(const XAFileType * const XATrack)
{
	if (XATrack->status)
	{
		utilPrintf("endpos = %d\n",XATrack->endPos);
		utilPrintf("loop = %d\n",XATrack->loop);
		utilPrintf("startpos = %d\n",XATrack->startPos);
		utilPrintf("status = %d\n",XATrack->status);
		utilPrintf("volume = %d\n",XATrack->vol);
		utilPrintf("cdpos %d, %d, %d, %d\n",XATrack->fileInfo.pos.minute,
										XATrack->fileInfo.pos.second,
										XATrack->fileInfo.pos.sector,
										XATrack->fileInfo.pos.track);
		utilPrintf("xa size = %d\n",XATrack->fileInfo.size);
		utilPrintf("xa name = %s\n",XATrack->fileInfo.name);
	}
	else
	{
		utilPrintf("No XA file currently being played.\n");
	}
}





void bb_InitXA(void)
{
#if GOLDCD==1
	int i=0;
	
	for(i=0; i<MUSIC_NUM_TRACKS; i++)
		xaFileData[i] = XAgetFileInfo(musicNames[i]);
#endif
}


/*	--------------------------------------------------------------------------------
	Function		: PrepareSongForLevel
	Purpose			: loads and starts playback of song for specified level
	Parameters		: short,short
	Returns			: void
	Info			: 
*/
void PrepareSong(short worldID)
{
/*
//	char cdTrack[16];
	int trackIndex;

	trackIndex = worldID + GARDEN_CDAUDIO;

//	xaFileData[trackIndex]=XAgetFileInfo(cdTrack);	// Not sure if you need to tell it its an xa file (looks like you do)
	xaFileData[trackIndex]=XAgetFileInfo("CD2.XA");	// Not sure if you need to tell it its an xa file (looks like you do)

	XAsetStatus(1);		// enable for playing
	XAstart(1);			// set for double speed playback
	XAplayChannelOffset( xaFileData[trackIndex],0,0,1,100); // file to play , offset from start , channel , loop , volume
	// volume goes from 0 to 100
*/


#if GOLDCD==1
	XAstart(1);
	utilPrintf ( "World ID : %d \n", worldID );
	XAplayChannel(xaFileData[worldID], 0,1,100);
	MAIN_PrintXAData ( xaFileData [ worldID ] );
#endif
}



void StopSong( )
{
#if GOLDCD==1
	XAstop();
#endif
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

	att = (radius)?radius/SCALE:DEFAULT_SFX_DIST;
	att = att<<12;

	SubVectorSSS( &diff, pos, &frog[0]->actor->position );
	// Volume attenuation - check also for radius != 0 and use instead of default
	dist = MagnitudeS( &diff );
	if( dist > (att) )
		return -1;

//bb	vol = FMul((vol<<12), FDiv(((att<<12)-(dist<<12)),att<<12))>>12;
	vol = (FDiv(att-dist,att)*vol)>>12;
//bb look out, this may have been badly optimised
//		vol = (vol * (((att-dist)<<12)/att)) >>12;

	tempSvect.vx = -pos->vx;
	tempSvect.vy = -pos->vy;
	tempSvect.vz = pos->vz;

	gte_ldv0(&tempSvect);
	gte_rtps();
	gte_stsxy((long *)&m.vx);
	gte_stsz(&m.vz);	//screen z/4 as otz
	m.vz >>= 2;

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
}

