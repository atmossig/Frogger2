/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: audio.c
	Programmer	: Random Healey
	Date		: 28/06/99 10:37:45

----------------------------------------------------------------------------------------------- */

#include "..\resource.h"
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <dsound.h>
#include <mdx.h>

#include <islmem.h>
#include <islutil.h>

#include "enemies.h"
#include "frogger.h"
#include "frogmove.h"
#include "platform.h"
#include "memload.h"
#include "Main.h"
#include "dx_sound.h"
#include "pcaudio.h"
#include "pcmisc.h"


#define MAX_AMBIENT_SFX		50
#define DEFAULT_SFX_DIST	7000

#define VOLUME_MIN		-5000
#define VOLUME_PERCENT (VOLUME_MIN/100)
#define PITCH_STEP		(DSBFREQUENCY_MAX/256)

int AMBIENT_VOLUME = 255;
int SAMPLE_VOLUME = 255;

SAMPLE *genSfx[NUM_GENERIC_SFX];
SAMPLE **sfx_anim_map = NULL;

UINT mciDevice = 0;

SOUNDLIST soundList;					// Actual Sound Samples List
AMBIENT_SOUND_LIST	ambientSoundList;
BUFFERLIST bufferList;					// Buffered Sound Samples List


void AddSample( SAMPLE *sample );
void RemoveSample( SAMPLE *sample );
void AddBufSample( BUFSAMPLE *sample );
void RemoveBufSample( BUFSAMPLE *sample );

AMBIENT_SOUND *AddAmbientSound(SAMPLE *sample,SVECTOR *pos,long radius,short volume,short pitch,float freq,float rFreq,MDX_ACTOR *follow);
void SubAmbientSound(AMBIENT_SOUND *ambientSound);
int UpdateLoopingSample( AMBIENT_SOUND *sample );

SAMPLE *CreateAndAddSample( char *path, char *file );

void SetSampleFormat ( SAMPLE *sample );
void CleanBufferSamples( void );

DWORD playCDTrack( HWND hWndNotify, BYTE bTrack );
DWORD stopCDTrack( HWND hWndNotify );



/*	--------------------------------------------------------------------------------
	Function		: LoadSfx
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void LoadSfxSet( char *path )
{
	HANDLE h;
	WIN32_FIND_DATA fData;
	int numSfx=0;
	long ret;
	char filepath[128];

	strcpy( filepath, path );
	strcat( filepath, "*.wav" );

	h = FindFirstFile( filepath, &fData );

	if( h == INVALID_HANDLE_VALUE ) return;

	do
	{
		CreateAndAddSample( path, fData.cFileName );
		numSfx++;
	}
	while( (ret = FindNextFile( h, &fData )) );

	utilPrintf("Loaded %d Samples\n",numSfx );

	FindClose( h );
}


void LoadSfx( unsigned long worldID )
{
	char path[128];
	int len, load=1;
	
	len = strlen(baseDirectory) + strlen(SFX_BASE);

	strcpy( path, baseDirectory );
	strcat( path, SFX_BASE );

	// Load all generic samples first and put in array
	strcat( path, "generic\\" );
	LoadSfxSet(path);

	genSfx[GEN_FROG_HOP] = FindSample(UpdateCRC("hopongrass"));
	genSfx[GEN_SUPER_HOP] = FindSample(UpdateCRC("hop2"));
	genSfx[GEN_DOUBLE_HOP] = FindSample(UpdateCRC("doublehop"));
	genSfx[GEN_COLLECT_BABY] = FindSample(UpdateCRC("getbabyfrog"));
	genSfx[GEN_FROG_TONGUE] = FindSample(UpdateCRC("tongue"));
	genSfx[GEN_COLLECT_COIN] = FindSample(UpdateCRC("pickupcoin"));
	genSfx[GEN_CROAK] = FindSample(UpdateCRC("frogcroak"));
	genSfx[GEN_BABYHAPPY] = FindSample(UpdateCRC("babyhappy"));
	genSfx[GEN_BABYSAD] = FindSample(UpdateCRC("babysad"));
	genSfx[GEN_BABYCRY] = FindSample(UpdateCRC("babycry"));
	genSfx[GEN_BABYREPLY] = FindSample(UpdateCRC("babyreply"));
	genSfx[GEN_FROGBELCH1] = FindSample(UpdateCRC("frogbelch2"));
	genSfx[GEN_FROGBELCH2] = FindSample(UpdateCRC("frogbelch2"));
	genSfx[GEN_FROGANNOYED] = FindSample(UpdateCRC("frogannoyed"));
	genSfx[GEN_FROGSLIDE] = FindSample(UpdateCRC("frogslide2"));
	genSfx[GEN_FROGHAPPY] = FindSample(UpdateCRC("frogokay"));
	genSfx[GEN_FROGSCARED] = FindSample(UpdateCRC("froguhoh"));
	genSfx[GEN_FROGBORED] = FindSample(UpdateCRC("frogbored"));
	genSfx[GEN_FROGLETSGO] = FindSample(UpdateCRC("frogletsgo"));

	genSfx[GEN_DEATHNORMAL] = FindSample(UpdateCRC("frogdeath"));
	genSfx[GEN_DEATHDROWN1] = FindSample(UpdateCRC("frogdrown1"));
	genSfx[GEN_DEATHDROWN2] = FindSample(UpdateCRC("frogdrown2"));
	genSfx[GEN_DEATHCRUSH] = FindSample(UpdateCRC("frogcrush"));
	genSfx[GEN_DEATHEXPLODE] = FindSample(UpdateCRC("frogexplode"));
	genSfx[GEN_DEATHFALL] = FindSample(UpdateCRC("frogfall"));
	genSfx[GEN_DEATHGIB] = FindSample(UpdateCRC("frogmowed"));
	genSfx[GEN_DEATHCHOP] = FindSample(UpdateCRC("frogchop"));

	path[len] = '\0';

	switch( worldID )
	{
	case WORLDID_GARDEN: strcat( path, "garden\\" ); break;
	case WORLDID_ANCIENT: strcat( path, "ancients\\" ); break;
	case WORLDID_SPACE: strcat( path, "space\\" ); break;
	case WORLDID_CITY: strcat( path, "city\\" ); break;
	case WORLDID_SUBTERRANEAN: strcat( path, "sub\\" ); break;
	case WORLDID_LABORATORY: strcat( path, "lab\\" ); break;
	case WORLDID_HALLOWEEN: strcat( path, "halloween\\" ); break;
	case WORLDID_SWAMPYWORLD: strcat( path, "swampyworld\\" ); break;
	case WORLDID_SUPERRETRO: strcat( path, "superretro\\" ); break;
	case WORLDID_FRONTEND: strcat( path, "frontend\\" ); break;
	default: load = 0; break;
	}

	if( load )
		LoadSfxSet( path );
}


/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddSample
	Purpose			: Make a sample from a mapping
	Parameters		: 
	Returns			: 
	Info			: 
*/
SAMPLE *CreateAndAddSample( char *path, char *file )
{
	SAMPLE *sfx;
	int i=0;
	char name[32], *fileName;
	
	if( !lpDS )
	{
		utilPrintf("Could not create sample, no sound device\n");
		return NULL;
	}

	sfx = (SAMPLE *)MALLOC0(sizeof(SAMPLE));

	// Remove extension from filename for psx compatability
	while( file[i] != '\0' && file[i] != '.' )
	{
		name[i] = file[i];
		i++;
	}
	name[i] = '\0';
	strlwr(name);

	sfx->uid = UpdateCRC(name);

	sfx->idName = (char *)MALLOC0(strlen(name)+2);
	strcpy( sfx->idName, name );

	// Create full name
	fileName = (char *)MALLOC0( (strlen(path) + strlen(file))+5 );
	strcpy( fileName, path );
	strcat( fileName, file );

	if( !(LoadWav(sfx,fileName)) )
	{
		FREE( fileName );
		FREE( sfx->idName );
		FREE( sfx );
		utilPrintf("Could not load wave file\n");
		return NULL;
	}

	// Sample starts and don't stop
	if( file[0] == 'l' && file[1] == 'p' && file[2] == '_' )
		sfx->flags |= SFXFLAGS_LOOP;
	
	if( sfx->flags & SFXFLAGS_3D_SAMPLE )
	{
		utilPrintf("Getting 3D Interface\n");
//		Get3DInterface ( sfx->lpdsBuffer, sfx->lpds3DBuffer );
		sfx->lpdsBuffer->lpVtbl->QueryInterface ( sfx->lpdsBuffer, &IID_IDirectSound3DBuffer, (void**)&sfx->lpds3DBuffer );
		if ( sfx->lpds3DBuffer )
			utilPrintf("3D Buffer Ok\n");
		else
			utilPrintf("Error On Interface, No 3D Buffer\n");
	}

	SetSampleFormat( sfx );
	AddSample( sfx );

	FREE( fileName );

	return sfx;
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
	BUFSAMPLE *bufSample=NULL;
	unsigned long bufStatus, vol=volume;
	long pan;
	float att, dist;
	SVECTOR diff, check;
	unsigned long flags=0;
	LPDIRECTSOUNDBUFFER lpdsBuffer;

	if(!lpDS || !sample) return FALSE;	// No DirectSound object!

	if( sample->flags & SFXFLAGS_3D_SAMPLE )
	{
		if( sample->lpds3DBuffer )
		{
			sample->lpds3DBuffer->lpVtbl->SetMode ( sample->lpds3DBuffer, DS3DMODE_NORMAL, DS3D_IMMEDIATE );
			Set3DPosition ( sample->lpds3DBuffer, pos->vx, pos->vy, pos->vz );
		}
	}
	else if( pos )
	{
		att = (radius)?radius:DEFAULT_SFX_DIST;
		SetVectorSS( &check, currPlatform[0]?&currPlatform[0]->pltActor->actor->position:&currTile[0]->centre );

		SubVectorSSS( &diff, pos, &check );
		// Volume attenuation - check also for radius != 0 and use instead of default
		dist = (float)MagnitudeS( &diff )/4096.0;
		if( dist > att )
			return FALSE;

		vol *= (att-dist)/att;

		//work out pan
		dist = Aabs(atan2(diff.vx, diff.vz));
		pan = (255/PI) * (FindShortestAngle(Fabs(check.vy+PI/2)*4096,dist*4096)/4096.0);
	}

	if( sample->flags & SFXFLAGS_LOOP )
		flags |= DSBPLAY_LOOPING;

	// Now test if the sample is playing - if it is then make a buffered instance of it to play.
	sample->lpdsBuffer->lpVtbl->GetStatus( sample->lpdsBuffer, &bufStatus );

	//	What we need to do here is create an instance of the buffer and store it in the buffer list.
	//	Have a clean buffer function that will go though and check if the sample is playing or not,
	//	if the sample is not playing then remove it from the list.
	if( bufStatus & DSBSTATUS_PLAYING )
	{
		if( !(bufSample = (BUFSAMPLE *)MALLOC0(sizeof(BUFSAMPLE))) )
			return 0;

		lpDS->lpVtbl->DuplicateSoundBuffer( lpDS, sample->lpdsBuffer, &(bufSample->lpdsBuffer) );

		AddBufSample( bufSample );
		lpdsBuffer = bufSample->lpdsBuffer;
	}
	else
	{
		lpdsBuffer = sample->lpdsBuffer;
	}

	lpdsBuffer->lpVtbl->SetFrequency( lpdsBuffer, (pitch==-1)?(DSBFREQUENCY_ORIGINAL):(pitch*PITCH_STEP) );
	lpdsBuffer->lpVtbl->SetVolume( lpdsBuffer, VOLUME_MIN+(VOLUME_PERCENT*vol*-1) );
	lpdsBuffer->lpVtbl->SetPan( lpdsBuffer, pan );
	lpdsBuffer->lpVtbl->Play( lpdsBuffer, 0, 0, flags );

	// HAAACCK! Bwahahahahahah!
	return (int)lpdsBuffer;
}


/*	--------------------------------------------------------------------------------
	Function		: StopSample
	Purpose			: stops a playing sample
	Parameters		: sample
	Returns			: success?
	Info			: Only works for unique samples - any buffered copies will keep playing
*/
int StopSample( SAMPLE *sample )
{
	unsigned long bufStatus;

	sample->lpdsBuffer->lpVtbl->GetStatus( sample->lpdsBuffer, &bufStatus );

	if( bufStatus & DSBSTATUS_PLAYING )
	{
		sample->lpdsBuffer->lpVtbl->Stop( sample->lpdsBuffer );
		return TRUE;
	}

	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function		: PlaySample
	Purpose			: plays a sample
	Parameters		: ID, position, radius, volume, pitch
	Returns			: success?
	Info			: Pass in a valid vector to get attenuation, and a radius to override the default
*/
void PlaySfxMappedSample( MDX_ACTOR *act )
{
	SAMPLE **map, *sample;
	SVECTOR pos;

	if( !act || !act->animation )
		return;

	if( !(map = (SAMPLE **)act->animation->sfxMapping) )
		return;

	if( !(sample = map[act->animation->currentAnimation]) )
		return;

	SetVectorSR( &pos, &act->pos );
	ScaleVector( &pos, 10 );
	// If looping, add ambient sound and remove sample from mapping
	if( sample->flags & SFXFLAGS_LOOP )
	{
		if( !act->animation->loopFlags[act->animation->currentAnimation] )
		{
			AddAmbientSound( sample, &pos, DEFAULT_SFX_DIST, SAMPLE_VOLUME, -1, 0, 0, act );
			act->animation->loopFlags[act->animation->currentAnimation] = 1;
		}
		return;
	}

	PlaySample( sample, &pos, DEFAULT_SFX_DIST, SAMPLE_VOLUME, -1 );
}


/*	--------------------------------------------------------------------------------
	Function		: PlaySample
	Purpose			: plays a sample
	Parameters		: ID, position, radius, volume, pitch
	Returns			: success?
	Info			: Pass in a valid vector to get attenuation, and a radius to override the default
*/
int UpdateLoopingSample( AMBIENT_SOUND *sample )
{
	unsigned long bufStatus, vol=sample->volume;
	long pan;
	float att, dist;
	MDX_VECTOR diff;

	if(!lpDS || !sample) return FALSE;	// No DirectSound object!

	if( mdxMagnitudeSquared(&sample->pos) )
	{
		att = (sample->radius)?sample->radius:DEFAULT_SFX_DIST;

		SubVector( &diff, &sample->pos, &frog[0]->actor->position );
		// Volume attenuation - check also for radius != 0 and use instead of default
		dist = mdxMagnitude( &diff );
		if( dist > att )
			vol=0;

		vol *= (att-dist)/att;

		//work out pan
		dist = Aabs(atan2(diff.vx, diff.vz));
		pan = (255/PI) * FindShortestAngle(Aabs(frog[0]->actor->position.vy+PI/2),dist);
	}

	// Now test if the sample is playing - if it is then make a buffered instance of it to play.
	sample->lpdsBuffer->lpVtbl->GetStatus( sample->lpdsBuffer, &bufStatus );

	sample->lpdsBuffer->lpVtbl->SetFrequency( sample->lpdsBuffer, (sample->pitch==-1)?(DSBFREQUENCY_ORIGINAL):(sample->pitch*PITCH_STEP) );
	sample->lpdsBuffer->lpVtbl->SetVolume( sample->lpdsBuffer, VOLUME_MIN+(VOLUME_PERCENT*vol*-1) );
	sample->lpdsBuffer->lpVtbl->SetPan( sample->lpdsBuffer, pan );

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
	wfx.nChannels		= 1;
	wfx.nSamplesPerSec	= 22050;
	wfx.wBitsPerSample	= 16;
	wfx.nBlockAlign		= wfx.wBitsPerSample / 8 * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	sample->lpdsBuffer->lpVtbl->SetFormat ( sample->lpdsBuffer, &wfx );
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
		return;

	for ( cur = bufferList.head.next; cur != &bufferList.head; cur = next )
	{
		next = cur->next;

		// Check if sample is playing
		cur->lpdsBuffer->lpVtbl->GetStatus ( cur->lpdsBuffer, &bufStatus );

		if ( !( bufStatus & DSBSTATUS_PLAYING )	)
		{
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
AMBIENT_SOUND *AddAmbientSound(SAMPLE *sample, SVECTOR *pos, long radius, short vol, short pitch, float freq, float randFreq, MDX_ACTOR *follow )
{
	AMBIENT_SOUND *ptr;
	AMBIENT_SOUND *ambientSound;

	if( !sample ) return NULL;

	ambientSound = (AMBIENT_SOUND *)MALLOC0(sizeof(AMBIENT_SOUND));
	ptr = &ambientSoundList.head;

	if( pos ) SetVectorSS( &ambientSound->pos, pos );
	ambientSound->follow = follow;

	ambientSound->volume = vol;
	ambientSound->sample = sample;
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
		{
			SetVectorSR( &amb->pos, &amb->follow->pos );
			ScaleVector( &amb->pos, 10 );
		}

		// If sound doesn't have a source
		if( !mdxMagnitudeSquared(&amb->pos) )
			pos = NULL;
		else
			pos = &amb->pos;

		if( (amb->sample->flags & SFXFLAGS_LOOP) && amb->lpdsBuffer )
			UpdateLoopingSample( amb );
		else
			amb->lpdsBuffer = (LPDIRECTSOUNDBUFFER)PlaySample( amb->sample, &amb->pos, amb->radius, amb->volume, amb->pitch );

		// Freq and randFreq are cunningly pre-multiplied by 60
		amb->counter = actFrameCount + amb->freq + ((amb->randFreq)?Random(amb->randFreq):0);
	}
}


/*	--------------------------------------------------------------------------------
	Function		: PrepareSong
	Purpose			: prepares a song...and plays it...woohoo !
	Parameters		: char
	Returns			: void
	Info			: 
*/
void PrepareSong( short num )
{
	// play cd audio track here....
	playCDTrack ( mdxWinInfo.hWndMain, num + GARDEN_CDAUDIO );
}


void StopSong( )
{
	stopCDTrack( mdxWinInfo.hWndMain );
}


DWORD stopCDTrack ( HWND hWndNotify )
{
	MCI_GENERIC_PARMS parms;

	// Stop
	mciSendCommand(mciDevice, MCI_STOP, MCI_NOTIFY, (DWORD)(LPMCI_GENERIC_PARMS)&parms);

	// Close device
	mciSendCommand(mciDevice, MCI_CLOSE, MCI_NOTIFY, (DWORD)(LPMCI_GENERIC_PARMS)&parms);

	mciDevice = 0;

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

	if( !mciDevice )
	{
		// Open the CD audio device by specifying the device name.
		mciOpenParms.lpstrDeviceType = "cdaudio";

		if ( dwReturn = mciSendCommand( 0, MCI_OPEN, MCI_OPEN_TYPE, ( DWORD ) ( LPVOID ) &mciOpenParms ) )
		{
			utilPrintf("Failed to open device %s\n",mciOpenParms.lpstrDeviceType);
			// Failed to open device. Don't close it; just return error.
			return dwReturn;
		}

		// The device opened successfully; get the device ID.
		wDeviceID = mciOpenParms.wDeviceID;

		// Set the time format to track/minute/second/frame (TMSF).
		mciSetParms.dwTimeFormat = MCI_FORMAT_TMSF;

		if (dwReturn = mciSendCommand ( wDeviceID, MCI_SET,MCI_SET_TIME_FORMAT, ( DWORD )  ( LPVOID ) &mciSetParms ) )
		{
			utilPrintf("Failed to set time format for cd device\n");

			mciSendCommand( wDeviceID, MCI_CLOSE, 0, 0 );
			return dwReturn;
		}
	}
	else // Repeat performance
	{
		wDeviceID = mciDevice;
	}
	
	// Begin playback from the given track and play until the beginning 
    // of the next track. 
    mciPlayParms.dwTo		= 0L;
    mciPlayParms.dwFrom		= MCI_MAKE_TMSF ( bTrack, 0, 0, 0 );
    mciPlayParms.dwTo		= MCI_MAKE_TMSF ( bTrack + 1, 0, 0, 0 );
    mciPlayParms.dwCallback = ( DWORD ) hWndNotify;
    if ( dwReturn = mciSendCommand ( wDeviceID, MCI_PLAY,MCI_FROM | MCI_TO | MCI_NOTIFY, (DWORD)(LPVOID) &mciPlayParms ) )
    {
		utilPrintf("Couldn't play cd track %i\n",bTrack);

        mciSendCommand ( wDeviceID, MCI_CLOSE, 0, 0 );
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
SAMPLE *FindSample( unsigned long uid )
{
	SAMPLE *next, *cur;

	for( cur = soundList.head.next; cur != &soundList.head; cur = next )
	{
		next = cur->next;
		
		if( cur->uid == uid )
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

	// Setup animation callback
	StartAnim = PlaySfxMappedSample;

	sfx_anim_map = NULL;
}


void InitAmbientSoundList()
{
	ambientSoundList.head.next = ambientSoundList.head.prev = &ambientSoundList.head;
	ambientSoundList.numEntries = 0;
}


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


void AddBufSample( BUFSAMPLE *bufSample )
{
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

	if( sample->lpds3DBuffer )
		sample->lpds3DBuffer->lpVtbl->Release(sample->lpds3DBuffer);

	if( sample->lpdsBuffer )
		sample->lpdsBuffer->lpVtbl->Release(sample->lpdsBuffer);

	if( sample->idName )
		FREE( sample->idName );

	if( sample->data )
		FREE( sample->data );

	FREE( sample );
}


void RemoveBufSample( BUFSAMPLE *bufSample )
{
	if( !bufSample->next )
		return;

	bufSample->prev->next	= bufSample->next;
	bufSample->next->prev	= bufSample->prev;
	bufSample->next		= NULL;
	bufferList.numEntries--;

	if( bufSample->lpdsBuffer )
		bufSample->lpdsBuffer->lpVtbl->Release(bufSample->lpdsBuffer);

	FREE( bufSample );
}


void FreeSampleList( void )
{
	SAMPLE *cur,*next;
	unsigned long i;

	if( sfx_anim_map ) 
	{
		FREE( sfx_anim_map );
		sfx_anim_map = NULL;
	}

	FreeBufSampleList( );

	utilPrintf("Freeing linked list : SAMPLE : (%d elements)\n",soundList.numEntries);

	// traverse enemy list and free elements
	while( soundList.head.next && soundList.head.next != &soundList.head )
	{
		StopSample( soundList.head.next );
		RemoveSample( soundList.head.next );
	}

	for( i=0; i<NUM_GENERIC_SFX; i++ )
		genSfx[i] = NULL;

	dispSample = NULL;

	// initialise list for future use
	InitSampleList();
}


void FreeBufSampleList ( void )
{
	unsigned long stat;
	BUFSAMPLE *cur,*next;

	utilPrintf("Freeing linked list : BUFSAMPLE : (%d elements)\n",bufferList.numEntries);

	// traverse enemy list and free elements
	while( bufferList.head.next && bufferList.head.next != &bufferList.head )
	{
		bufferList.head.next->lpdsBuffer->lpVtbl->GetStatus( bufferList.head.next->lpdsBuffer, &stat );
		if( stat & DSBSTATUS_PLAYING )
			bufferList.head.next->lpdsBuffer->lpVtbl->Stop( bufferList.head.next->lpdsBuffer );

		RemoveBufSample( bufferList.head.next );
	}
}


void SubAmbientSound(AMBIENT_SOUND *ambientSound)
{
	if( !ambientSound->next )
		return;

	ambientSound->prev->next = ambientSound->next;
	ambientSound->next->prev = ambientSound->prev;
	ambientSoundList.numEntries--;

	FREE( ambientSound );
}


void FreeAmbientSoundList( )
{
	// traverse enemy list and free elements
	while( ambientSoundList.head.next && ambientSoundList.head.next != &ambientSoundList.head )
	{
		if( ambientSoundList.head.next->sample && ambientSoundList.head.next->lpdsBuffer )
			ambientSoundList.head.next->lpdsBuffer->lpVtbl->Stop( ambientSoundList.head.next->lpdsBuffer );

		SubAmbientSound( ambientSoundList.head.next );
	}

	// initialise list for future use
	InitAmbientSoundList();
}




/*	--------------------------------------------------------------------------------
	Function 	: LoadSfxMapping
	Purpose 	: Load a mapping of animations to sound effects for a list of actors
	Parameters 	: world and level ID's
	Returns 	: Pointer to mapping array
	Info 		: 
*/
void LoadSfxMapping( int world, int level )
{
	unsigned char *in;
	void *buffer;
	HANDLE h;
	char *filename, wnum[3], lnum[3];
	long num, size, read, index, type, i, count, uid;

	index = strlen(baseDirectory) + 24;
	filename = (char *)MALLOC0( index );

	// Directory and start of sfxmap filename
	strcpy( filename, baseDirectory );
	strcat( filename, "maps\\sfxanim" );
	// World and level ids are part of the filename
	_itoa( world, wnum, 10 );
	strcat( filename, wnum );
	_itoa( level, lnum, 10 );
	strcat( filename, lnum );
	// Extension
	strcat( filename, ".sam" );

	// Open input file
	h = CreateFile( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if (h == INVALID_HANDLE_VALUE)
	{
		utilPrintf("Couldn't load sfx map file %s\n", filename);
		FREE( filename );
		return;
	}

	// Get file info and data
	size = GetFileSize(h, NULL);
	buffer = MALLOC0( size );
	ReadFile( h, buffer, size, &read, NULL );
	CloseHandle(h);

	in = (unsigned char *)buffer;
	sfx_anim_map = (SAMPLE **)MALLOC0( size+8 );

	index=0;
	count=0;

	// Read data from buffer into mapping array
	while( count < size-4 )
	{
		// Actor uid
		uid = MEMGETINT(&in);
		count+=4;

		// Actor type
		type = MEMGETINT(&in);
		count+=4;

		// If a tile based sample, skip the normal mapping load and just make an ambient
		if( type == 4 && uid < tileCount )
		{
			AddAmbientSound( FindSample(MEMGETINT(&in)), &firstTile[uid].centre, 12000, AMBIENT_VOLUME, -1, 0, 0, NULL );
			count+=4;
			continue;
		}

		// If not tile based, store the previously loaded stuff in the sample map
		sfx_anim_map[index++] = (SAMPLE *)uid;
		sfx_anim_map[index++] = (SAMPLE *)type;

		switch( type )
		{
		case 0: num = NUM_FROG_ANIMS; break;
		case 1: num = MULTI_NUM_ANIMS; break;
		case 2: num = NUM_NME_ANIMS; break;
		case 3: num = 1; break; // NUM_SCENIC_ANIMS
		case 4: num = 1; break; // NUM_SCENIC_ANIMS
		default: num = 0; break;
		}

		for( i=0; i<num; i++ )
		{
			// Locate samples by their uids
			sfx_anim_map[index++] = FindSample( MEMGETINT(&in) );
			count+=4;
		}
	}

	// Signal end of list
	sfx_anim_map[index] = NULL;

	FREE( filename );
	FREE( buffer );
}


/*	--------------------------------------------------------------------------------
	Function 	: FindSfxMapping
	Purpose 	: Load a mapping of animations to sound effects for a list of actors
	Parameters 	: world and level ID's
	Returns 	: Pointer to mapping array
	Info 		: 
*/
void FindSfxMapping( unsigned long uid, ACTOR *actor )
{
	MDX_ACTOR *act = (MDX_ACTOR *)actor->actualActor;
	unsigned long actID, type, num, index=0, i;

	// Clear all loopflags
	for( i=0; i<NUM_NME_ANIMS; i++ )
		act->animation->loopFlags[i] = 0;

	// First actor uid - if none, return NULL
	if( !sfx_anim_map || !uid || !(actID = (unsigned long)sfx_anim_map[index++]) )
	{
		act->animation->sfxMapping = NULL;
		return;
	}

	do
	{
		// Advance cursor by number depending on actor type
		type = (unsigned long)sfx_anim_map[index++];

		if( actID == uid )
		{
			if( type == 3 )
			{
				// Make an ambient sound if we've attached a sound to a scenic
				AddAmbientSound( sfx_anim_map[index], &actor->position, 12000, AMBIENT_VOLUME, -1/*128*/, 0, 0, act );
				act->animation->sfxMapping = NULL;
				return;
			}
			else
			{
				// Return a run of samples if we've found the actor
				act->animation->sfxMapping = (void **)&sfx_anim_map[index];
				return;
			}
		}

		switch( type )
		{
		case 0: index += NUM_FROG_ANIMS; break;
		case 1: index += MULTI_NUM_ANIMS; break;
		case 2: index += NUM_NME_ANIMS; break;
		case 3: index ++; break; // Number of scenic animations
		}

		// Get next actor uid and stop if end of list
		actID = (unsigned long)sfx_anim_map[index++];

	} while( actID );

	// Not found owt
	act->animation->sfxMapping = NULL;
}
