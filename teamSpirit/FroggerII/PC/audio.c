/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: audio.c
	Programmer	: Random Healey
	Date		: 28/06/99 10:37:45

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>
#include "..\resource.h"
#include "incs.h"

//#define MYDEBUG	- stop printing all that CRAP, man!
#define MAX_AMBIENT_SFX		50
#define DEFAULT_SFX_DIST	500

#define VOLUME_MIN		-5000
#define VOLUME_PERCENT (VOLUME_MIN/100)
#define PITCH_STEP		(DSBFREQUENCY_MAX/512)

SAMPLEMAP genericMapping[] = 
{
	"generic\\levelcomp.wav",		2, 11025, 16, GEN_LEVEL_COMP,	FLAGS_NONE,
	"generic\\targetcomplete.wav",	2, 11025, 16, GEN_TARGET_COM,	FLAGS_NONE,
	"generic\\timeout.wav",			2, 11025, 16, GEN_TIME_OUT,		FLAGS_NONE,
	"generic\\clocktock.wav",		2, 11025, 16, GEN_CLOCK_TOCK,	FLAGS_NONE,
	"generic\\clocktick.wav",		2, 11025, 16, GEN_CLOCK_TICK,	FLAGS_NONE,
	"generic\\hopongrass.wav",		2, 11025, 16, GEN_FROG_HOP,		FLAGS_NONE,
	"generic\\froggerB.wav",		2, 11025, 16, GEN_SUPER_HOP,	FLAGS_NONE,
	"generic\\babyfrog.wav",		2, 11025, 16, GEN_BABY_FROG,	FLAGS_NONE,
	"generic\\froggerF.wav",		2, 11025, 16, GEN_FROG_TONGUE,	FLAGS_NONE,
	"generic\\froggerD.wav",		2, 11025, 16, GEN_FROG_HURT,	FLAGS_NONE,
	"generic\\froggerE.wav",		2, 11025, 16, GEN_FROG_DEATH,	FLAGS_NONE,
	"generic\\froggerA.wav",		2, 11025, 16, GEN_FROG_DOUBLEHOP,FLAGS_NONE,

};


SAMPLEMAP gardenMapping[] =
{
	"generic\\babyfrog.wav", 2, 22050, 16, GAR_MOWER, FLAGS_NONE,
};

SAMPLEMAP spaceMapping[] =
{
	"space\\bbota.wav",		2, 22050, 16, GAR_MOWER,	FLAGS_NONE,
	"space\\bbotb.wav",		2, 22050, 16, SPC_BBOTB,	FLAGS_NONE,
	"space\\bbotc.wav",		2, 22050, 16, SPC_BBOTC,	FLAGS_NONE,
	"space\\bbotd.wav",		2, 22050, 16, SPC_BBOTD,	FLAGS_NONE,
	"space\\beastA.wav",	2, 22050, 16, SPC_BEASTA,	FLAGS_NONE,
	"space\\beastB.wav",	2, 22050, 16, SPC_BEASTB,	FLAGS_NONE,
	"space\\beastC.wav",	2, 22050, 16, SPC_BEASTC,	FLAGS_NONE,
	"space\\fisheadA.wav",	2, 22050, 16, SPC_FISHEADA, FLAGS_NONE,
	"space\\fisHeadB.wav",	2, 22050, 16, SPC_FISHEADB, FLAGS_NONE,
	"space\\muzapA.wav",	2, 22050, 16, SPC_MUZAPA,	FLAGS_NONE,
	"space\\muzapB.wav",	2, 22050, 16, SPC_MUZAPB,	FLAGS_NONE,
	"space\\muzapC.wav",	2, 22050, 16, SPC_MUZAPC,	FLAGS_NONE,
	"space\\muzapD.wav",	2, 22050, 16, SPC_MUZAPD,	FLAGS_NONE,
	"space\\seelA.wav",		2, 22050, 16, SPC_SEELA,	FLAGS_NONE,
	"space\\seyeballA.wav", 2, 22050, 16, SPC_SEYEBALLA,FLAGS_NONE,
	"space\\ssnail.wav",	2, 22050, 16, SPC_SSNAIL,	FLAGS_NONE,
	"space\\spropA.wav",	2, 22050, 16, SPC_SPROPA,	FLAGS_NONE,
	"space\\ssatA.wav",		2, 22050, 16, SPC_SSATA,	FLAGS_NONE,
	"space\\tankA.wav",		2, 22050, 16, SPC_TANKA,	FLAGS_NONE,
};


UINT mciDevice = 0;

SOUNDLIST soundList;					// Actual Sound Samples List
AMBIENT_SOUND_LIST	ambientSoundList;
BUFFERLIST bufferList;					// Buffered Sound Samples List


SAMPLE *FindSample( int num );
void AddSample( SAMPLE *sample );
void RemoveSample( SAMPLE *sample );
void AddBufSample( BUFSAMPLE *sample );
void RemoveBufSample( BUFSAMPLE *sample );

void SubAmbientSound(AMBIENT_SOUND *ambientSound);

SAMPLE *CreateAndAddSample( SAMPLEMAP *sampleMap );

void SetSampleFormat ( SAMPLE *sample );
void CleanBufferSamples( void );

void PrepareSong( char num, char slot );
void StopSong( );
DWORD playCDTrack( HWND hWndNotify, BYTE bTrack );
DWORD stopCDTrack( HWND hWndNotify );



/*	--------------------------------------------------------------------------------
	Function		: LoadSfx
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: MERGE THESE FUNCTIONS?
*/
void LoadSfx( unsigned long worldID )
{
	int numSfx, i;
	SAMPLEMAP *mapping;

	for( i = NUM_GENERIC_SFX-1; i>=0; i-- )
		CreateAndAddSample( &genericMapping[i] );

	switch ( worldID )
	{
		case WORLDID_GARDEN: 
			numSfx = NUM_GARDEN_SFX; 
			mapping = gardenMapping; 
			break;
		case WORLDID_SPACE: 
			numSfx = NUM_SPACE_SFX;
			mapping = spaceMapping;
			break;
		default: 
			numSfx = 0;
			break;
	}

	if( numSfx )
		for( i=numSfx-1; i>=0; i-- )
			CreateAndAddSample( &mapping[i] );
}


/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddSample
	Purpose			: Make a sample from a mapping
	Parameters		: 
	Returns			: 
	Info			: 
*/
SAMPLE *CreateAndAddSample ( SAMPLEMAP *sampleMap )
{
	SAMPLE *newItem;
	
	if( !lpDS )
	{
		dprintf"Returned from Create Sample, because lpDS was NULL!!!!!!!!\n"));
		return NULL;
	}

	if( (newItem = (SAMPLE *)JallocAlloc(sizeof(SAMPLE),YES,"SAM")) == NULL )
	{
		dprintf"CreateAndAddSample : newItem : NULL value from JallocAlloc\n"));
		return NULL;
	}

	newItem->map = sampleMap;

	// Create full name
	sprintf( newItem->idName, "%s%s%s", baseDirectory, SFX_BASE, sampleMap->sampleFileName );

	if( !LoadWav( newItem->idName, newItem ) )
	{
		dprintf"Returned from LoadWav\n"));
		return NULL;
	}
	
	if( sampleMap->flags & FLAGS_3D_SAMPLE )
	{
		dprintf"Getting 3D Interface\n"));
//		Get3DInterface ( newItem->lpdsBuffer, newItem->lpds3DBuffer );
		newItem->lpdsBuffer->lpVtbl->QueryInterface ( newItem->lpdsBuffer, &IID_IDirectSound3DBuffer, (void**)&newItem->lpds3DBuffer );
		if ( newItem->lpds3DBuffer )
			dprintf"3D Buffer Ok\n"));
		else
			dprintf"Error On Interface, No 3D Buffer\n"));
	}

	SetSampleFormat( newItem );

	AddSample( newItem );

	return newItem;
}


/*	--------------------------------------------------------------------------------
	Function		: PlaySample
	Purpose			: plays a sample
	Parameters		: ID, position, radius, volume, pitch
	Returns			: success?
	Info			: Pass in a valid vector to get attenuation, and a radius to override the default
*/
int PlaySample( short num, VECTOR *pos, long radius, short volume, short pitch )
{
	SAMPLE *sample;
	BUFSAMPLE *bufSample;
	unsigned long bufStatus, vol=volume;
	long pan;
	float att, dist;
	VECTOR diff;

	if(!lpDS) return FALSE;	// No DirectSound object!

	if( !(sample = FindSample(num)) ) return FALSE; // Sfx not in bank

	if( sample->map->flags & FLAGS_3D_SAMPLE )
	{
		if( sample->lpds3DBuffer )
		{
			sample->lpds3DBuffer->lpVtbl->SetMode ( sample->lpds3DBuffer, DS3DMODE_NORMAL, DS3D_IMMEDIATE );
			Set3DPosition ( sample->lpds3DBuffer, pos->v[X], pos->v[Y], pos->v[Z] );
		}
	}
	else if( pos )
	{
		att = (radius)?radius:DEFAULT_SFX_DIST;

		SubVector2D( &diff, pos, &frog[0]->actor->pos );
		// Volume attenuation - check also for radius != 0 and use instead of default
		dist = Magnitude( &diff );
		if( dist > att )
			return 0;
		
		vol *= (att-dist)/att;

		//work out pan
		dist = Aabs(atan2(diff.v[X], diff.v[Z]));
		pan = (255/PI) * FindShortestAngle(Aabs(frog[0]->actor->rot.v[Y]+PI/2),dist);
	}

	// Now test if the sample is playing if it is then make an instance of it to play.
	sample->lpdsBuffer->lpVtbl->GetStatus( sample->lpdsBuffer, &bufStatus );

	if( bufStatus & DSBSTATUS_PLAYING )
	{
		/*	What we need to do here is create an instance of the buffer and store it in the buffer list.
			Have a clean buffer function that will go though and check if the sample is playing or not,
			if the sample is not playing then remove it from the list.
		*/

		// Create the buffer sample.
		if( !(bufSample = (BUFSAMPLE *)JallocAlloc(sizeof(BUFSAMPLE), YES, "BUFSAM" )) ) return NULL;

		lpDS->lpVtbl->DuplicateSoundBuffer( lpDS, sample->lpdsBuffer, &(bufSample->lpdsBuffer) );

		AddBufSample( bufSample );

		bufSample->lpdsBuffer->lpVtbl->SetFrequency( bufSample->lpdsBuffer, pitch*PITCH_STEP );
		bufSample->lpdsBuffer->lpVtbl->SetVolume( bufSample->lpdsBuffer, VOLUME_MIN+(VOLUME_PERCENT*vol*-1) );
		bufSample->lpdsBuffer->lpVtbl->SetPan( bufSample->lpdsBuffer, pan );
		bufSample->lpdsBuffer->lpVtbl->Play( bufSample->lpdsBuffer, 0, 0, 0 );
	}
	else
	{
		sample->lpdsBuffer->lpVtbl->SetFrequency( sample->lpdsBuffer, pitch*PITCH_STEP );
		sample->lpdsBuffer->lpVtbl->SetVolume( sample->lpdsBuffer, VOLUME_MIN+(VOLUME_PERCENT*vol*-1) );
		sample->lpdsBuffer->lpVtbl->SetPan( sample->lpdsBuffer, pan );
		sample->lpdsBuffer->lpVtbl->Play( sample->lpdsBuffer, 0, 0, 0 );
	}

	return TRUE;
}


/*	--------------------------------------------------------------------------------
	Function		: SetSampleFormat
	Purpose			: Changes the format of the sound
	Parameters		: SAMPLE *sample : Contains the sample to change the format for.
	Returns			: void
	Info			: 
*/
void SetSampleFormat( SAMPLE *sample )
{
	WAVEFORMATEX	wfx;

	memset ( &wfx, 0, sizeof ( WAVEFORMATEX ) ); 
	wfx.wFormatTag		= WAVE_FORMAT_PCM; 
	wfx.nChannels		= sample->map->numChannels; 
	wfx.nSamplesPerSec	= sample->map->sampleRate; 
	wfx.wBitsPerSample	= sample->map->bitsPerSample; 
	wfx.nBlockAlign		= wfx.wBitsPerSample / 8 * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

#ifdef MYDEBUG
	dprintf"Setting Sample Format : Buffer = (&%x) FileName : %s\n", sample->lpdsBuffer, sample->idName));
#endif
	sample->lpdsBuffer->lpVtbl->SetFormat ( sample->lpdsBuffer, &wfx );
#ifdef MYDEBUG
	dprintf"Set Sample Format : Buffer = (&%x)\n", sample->lpdsBuffer));
#endif
}


/*	--------------------------------------------------------------------------------
	Function		: Clean Buffer Samples
	Purpose			: I have no idea
	Parameters		: 
	Returns			: 
	Info			: 
*/
void CleanBufferSamples ( void )
{
	BUFSAMPLE *cur, *next;

	unsigned long bufStatus;

	if ( bufferList.numEntries == 0)
	{
#ifdef MYDEBUG
		dprintf"No Samples in Buffer List\n"));
#endif
		return;
	}

	for ( cur = bufferList.head.next; cur != &bufferList.head; cur = next )
	{
		next = cur->next;

		// Check if sample is playing
		cur->lpdsBuffer->lpVtbl->GetStatus ( cur->lpdsBuffer, &bufStatus );

		if ( !( bufStatus & DSBSTATUS_PLAYING )	)
		{
#ifdef MYDEBUG
			dprintf"Releasing Buffered Sample\n"));
#endif
			RemoveBufSample( cur );

			if ( bufferList.numEntries == 0 )
				return;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: AddAmbientSound
	Purpose 	: Create an ambient sound
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
AMBIENT_SOUND *AddAmbientSound(short num, VECTOR *pos, long radius, short vol, short pitch, float freq, float randFreq, ACTOR *follow )
{
	AMBIENT_SOUND *ptr = &ambientSoundList.head;
	AMBIENT_SOUND *ambientSound = (AMBIENT_SOUND *)JallocAlloc(sizeof(AMBIENT_SOUND),YES,"AmbSnd");
	
	if( pos ) SetVector( &ambientSound->pos, pos );
	if( follow ) ambientSound->follow = follow;

	ambientSound->volume = vol;
	ambientSound->num = num;
	ambientSound->pitch = pitch;
	ambientSound->radius = radius;

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
	VECTOR *pos;

	// Update each ambient in turn
	for( amb = ambientSoundList.head.next; amb != &ambientSoundList.head; amb = amb2 )
	{
		amb2 = amb->next;

		// Timeout, so play a new sound
		if( actFrameCount < amb->counter )
			continue;

		// If it is attached to a platform, make ambient follow that platform
		if( amb->follow )
			SetVector( &amb->pos, &amb->follow->pos );

		// If sound doesn't have a source
		if( !MagnitudeSquared(&amb->pos) )
			pos = NULL;
		else
			pos = &amb->pos;

		PlaySample( amb->num, &amb->pos, amb->radius, amb->volume, amb->pitch );

		// Freq and randFreq are cunningly pre-multiplied by 60
		amb->counter = actFrameCount + amb->freq + ((amb->randFreq)?Random(amb->randFreq):0);
	}
}


/*	--------------------------------------------------------------------------------
	Function		: PrepareSongForLevel
	Purpose			: loads and starts playback of song for specified level
	Parameters		: short,short
	Returns			: void
	Info			: 
*/
void PrepareSongForLevel(short worldID,short levelID)
{
	int theToon = -1;

	if(worldID == WORLDID_GARDEN)
	{
		switch(levelID)
		{
			case LEVELID_GARDEN1:
				theToon = GARDEN_CDAUDIO;
				break;

			case LEVELID_GARDEN2:
				theToon = GARDEN_CDAUDIO;
				break;

			case LEVELID_GARDEN3:
				theToon = GARDEN_CDAUDIO;
				break;

			default:
				theToon = GARDEN_CDAUDIO;
				break;
		}
	}
	else if(worldID == WORLDID_ANCIENT)
	{
		switch(levelID)
		{
			case LEVELID_ANCIENT1:
				theToon = ANCIENTS_CDAUDIO;
				break;

			case LEVELID_ANCIENT2:
				theToon = ANCIENTS_CDAUDIO;
				break;

			case LEVELID_ANCIENT3:
				theToon = ANCIENTS_CDAUDIO;
				break;

			default:
				theToon = ANCIENTS_CDAUDIO;
				break;
		}
	}
	else if(worldID == WORLDID_SPACE)
	{
		switch(levelID)
		{
			case LEVELID_SPACE1:
				theToon = SPACE_CDAUDIO;
				break;

			case LEVELID_SPACE2:
				theToon = SPACE_CDAUDIO;
				break;

			case LEVELID_SPACE3:
				theToon = SPACE_CDAUDIO;
				break;

			default:
				theToon = SPACE_CDAUDIO;
				break;
		}
	}
	else if(worldID == WORLDID_CITY)
	{
		switch(levelID)
		{
			default:
				theToon = CITY_CDAUDIO;
				break;
		}
	}
	else if(worldID == WORLDID_SUBTERRANEAN)
	{
		switch(levelID)
		{
			default:
				theToon = SUBTERRANEAN_CDAUDIO;
				break;
		}
	}
	else if(worldID == WORLDID_LABORATORY)
	{
		switch(levelID)
		{
			default:
				theToon = LABORATORY_CDAUDIO;
				break;
		}
	}
	else if(worldID == WORLDID_TOYSHOP)
	{
		switch(levelID)
		{
			default:
				theToon = TOYSHOP_CDAUDIO;
				break;
		}
	}
	else if(worldID == WORLDID_HALLOWEEN)
	{
		switch(levelID)
		{
			default:
				theToon = HALLOWEEN_CDAUDIO;
				break;
		}
	}
	else if(worldID == WORLDID_SUPERRETRO)
	{
		switch(levelID)
		{
			case LEVELID_SUPERRETRO1:
				theToon = SUPERRETRO_CDAUDIO;
				break;

			default:
				theToon = SUPERRETRO_CDAUDIO;
				break;
		}
	}
	else if(worldID == WORLDID_FRONTEND)
	{
		switch(levelID)
		{
			case LEVELID_FRONTEND1:
				theToon = FRONTEND_CDAUDIO;
				break;

			case LEVELID_FRONTEND2:
				theToon = LEVELSELECT_CDAUDIO;
				break;

			case LEVELID_FRONTEND3:
				theToon = FRONTEND_CDAUDIO;
				break;

			case LEVELID_FRONTEND4:
				theToon = FRONTEND_CDAUDIO;
				break;

			case LEVELID_FRONTEND5:
				theToon = FRONTEND_CDAUDIO;
				break;

			default:
				theToon = FRONTEND_CDAUDIO;
				break;
		}
	}

	PrepareSong(theToon,0);
}


/*	--------------------------------------------------------------------------------
	Function		: PrepareSong
	Purpose			: prepares a song...and plays it...woohoo !
	Parameters		: char
	Returns			: void
	Info			: 
*/
void PrepareSong( char num, char slot )
{
	// play cd audio track here....
	playCDTrack ( winInfo.hWndMain, num );
}


void StopSong( )
{
	stopCDTrack( winInfo.hWndMain );
}


DWORD stopCDTrack ( HWND hWndNotify )
{
	MCI_GENERIC_PARMS parms;

	// Stop
	mciSendCommand(mciDevice, MCI_STOP, MCI_NOTIFY, (DWORD)(LPMCI_GENERIC_PARMS)&parms);

	// Close device
	mciSendCommand(mciDevice, MCI_CLOSE, MCI_NOTIFY, (DWORD)(LPMCI_GENERIC_PARMS)&parms);

	return TRUE;
}

// Plays a specified audio track using MCI_OPEN, MCI_PLAY. Returns as 
// soon as playback begins. The window procedure function for the 
// specified window will be notified when playback is complete. 
// Returns 0L on success; otherwise, returns an MCI error code.
DWORD playCDTrack ( HWND hWndNotify, BYTE bTrack )
{
    UINT wDeviceID;
    DWORD dwReturn;
    MCI_OPEN_PARMS mciOpenParms;
    MCI_SET_PARMS mciSetParms;
    MCI_PLAY_PARMS mciPlayParms;

    // Open the CD audio device by specifying the device name.
    mciOpenParms.lpstrDeviceType = "cdaudio";

    if ( dwReturn = mciSendCommand ( NULL, MCI_OPEN, MCI_OPEN_TYPE, ( DWORD ) ( LPVOID ) &mciOpenParms ) )
    {
        // Failed to open device. Don't close it; just return error.
        return dwReturn;
    }

    // The device opened successfully; get the device ID.
    wDeviceID = mciOpenParms.wDeviceID;

    // Set the time format to track/minute/second/frame (TMSF).
    mciSetParms.dwTimeFormat = MCI_FORMAT_TMSF;

    if (dwReturn = mciSendCommand ( wDeviceID, MCI_SET,MCI_SET_TIME_FORMAT, ( DWORD )  ( LPVOID ) &mciSetParms ) )
    {
        mciSendCommand ( wDeviceID, MCI_CLOSE, 0, NULL );
		return dwReturn;
    }
	
	// Begin playback from the given track and play until the beginning 
    // of the next track. 
    mciPlayParms.dwTo		= 0L;
    mciPlayParms.dwFrom		= MCI_MAKE_TMSF ( bTrack, 0, 0, 0 );
    mciPlayParms.dwTo		= MCI_MAKE_TMSF ( bTrack + 1, 0, 0, 0 );
    mciPlayParms.dwCallback = ( DWORD ) hWndNotify;
    if ( dwReturn = mciSendCommand ( wDeviceID, MCI_PLAY,MCI_FROM | MCI_TO | MCI_NOTIFY, (DWORD)(LPVOID) &mciPlayParms ) )
    {
        mciSendCommand ( wDeviceID, MCI_CLOSE, 0, NULL );
        return dwReturn;
    }

	mciDevice = wDeviceID;

    return 0L;
}


/*	--------------------------------------------------------------------------------
	Function		: FindSample
	Purpose			: Search the list for the required sound
	Parameters		: ID
	Returns			: Sample or NULL
	Info			: 
*/
SAMPLE *FindSample( int num )
{
	SAMPLE *next, *cur;

	for( cur = soundList.head.next; cur != &soundList.head; cur = next )
	{
		next = cur->next;
		
		if( cur->map->sampleID == num )
			return cur;
	}

	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function		: DULL LIST FUNCTIONS
	Purpose			: Init, Add, Sub, Free etc...
	Parameters		: 
	Returns			: 
	Info			: 
*/
void InitSampleList( )
{
	//Init the sample list for the real samples.
	soundList.numEntries	= 0;
	soundList.head.next		= soundList.head.prev = &soundList.head;

	// Init the buffer list for samples that are playing
	bufferList.numEntries	= 0;
	bufferList.head.next	= bufferList.head.prev = &bufferList.head;
}


void InitAmbientSoundList()
{
	ambientSoundList.head.next = ambientSoundList.head.prev = &ambientSoundList.head;
	ambientSoundList.numEntries = 0;
}


void AddSample( SAMPLE *sample )
{
#ifdef MYDEBUG
	dprintf"Adding Sample To List - sample->next : (&%x)\n", sample->next));
#endif
	if( !sample->next )
	{
		soundList.numEntries++;
		sample->prev				= &soundList.head;
		sample->next				= soundList.head.next;
		soundList.head.next->prev	= sample;
		soundList.head.next			= sample;
	}
}


void AddBufSample( BUFSAMPLE *bufSample )
{
#ifdef MYDEBUG
	dprintf"Adding Buffer Sample To List - bufSample->next : (&%x)\n", bufSample->next));
#endif
	if( !bufSample->next )
	{
		bufferList.numEntries++;
		bufSample->prev				= &bufferList.head;
		bufSample->next				= bufferList.head.next;
		bufferList.head.next->prev	= bufSample;
		bufferList.head.next		= bufSample;
	}
}


void RemoveSample( SAMPLE *sample )
{
	if( !sample->next )
		return;

	sample->prev->next	= sample->next;
	sample->next->prev	= sample->prev;
	sample->next		= NULL;
	soundList.numEntries--;

	JallocFree ( ( UBYTE ** ) &sample );
}


void RemoveBufSample( BUFSAMPLE *bufSample )
{
	if( !bufSample->next )
		return;

	bufSample->prev->next	= bufSample->next;
	bufSample->next->prev	= bufSample->prev;
	bufSample->next		= NULL;
	bufferList.numEntries--;

	ReleaseBuffer ( bufSample->lpdsBuffer );

	JallocFree ( ( UBYTE ** ) &bufSample );
}


void FreeSampleList( void )
{
	SAMPLE *cur,*next;

	// check if any elements in list
	if( !soundList.numEntries )
		return;

	dprintf"Freeing linked list : SAMPLE : (%d elements)\n",soundList.numEntries));

	// traverse enemy list and free elements
	for( cur = soundList.head.next; cur != &soundList.head; cur = next )
	{
		next = cur->next;

		RemoveSample( cur );
	}

	// initialise list for future use
	InitSampleList();
}


void FreeBufSampleList ( void )
{
	BUFSAMPLE *cur,*next;

	// check if any elements in list
	if( !bufferList.numEntries )
		return;

	dprintf"Freeing linked list : BUFSAMPLE : (%d elements)\n",bufferList.numEntries));

	// traverse enemy list and free elements
	for ( cur = bufferList.head.next; cur != &bufferList.head; cur = next )
	{
		next = cur->next;

		RemoveBufSample( cur );
	}

	// initialise list for future use
	InitSampleList();
}


void SubAmbientSound(AMBIENT_SOUND *ambientSound)
{
	ambientSound->prev->next = ambientSound->next;
	ambientSound->next->prev = ambientSound->prev;
	ambientSoundList.numEntries--;
}


void FreeAmbientSoundList( )
{
	AMBIENT_SOUND *cur,*next;

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

