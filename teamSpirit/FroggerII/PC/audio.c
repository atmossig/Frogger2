/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: audio.c
	Programmer	: James Healey
	Date		: 28/06/99 10:37:45

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>
#include "..\resource.h"
#include "incs.h"

//#define MYDEBUG	- stop printing all that CRAP, man!

SAMPLEMAP genericMapping [] = {
	"generic\\levelcomp.wav",		2, 22050, 16, GEN_LEVEL_COMP,	FLAGS_NONE,
	"generic\\targetcomplete.wav",	2, 22050, 16, GEN_TARGET_COM,	FLAGS_NONE,
	"generic\\timeout.wav",			2, 22050, 16, GEN_TIME_OUT,		FLAGS_NONE,
	"generic\\clocktock.wav",		2, 22050, 16, GEN_CLOCK_TOCK,	FLAGS_NONE,
	"generic\\clocktick.wav",		2, 22050, 16, GEN_CLOCK_TICK,	FLAGS_NONE,
	"generic\\froghop.wav",			2, 22050, 16, GEN_FROG_HOP,		FLAGS_NONE,
	"generic\\superhop.wav",		2, 22050, 16, GEN_SUPER_HOP,	FLAGS_NONE,
	"generic\\babyfrog.wav",		2, 22050, 16, GEN_BABY_FROG,	FLAGS_NONE };

SAMPLEMAP gardenMapping [] = {
	"generic\\babyfrog.wav", 2, 22050, 16, GAR_MOWER, FLAGS_NONE };


//***********************************
// Function Prototypes

SOUNDLIST soundList;					// Actual Sound Samples List
			
BUFFERLIST bufferList;					// Buffered Sound Samples List

//***********************************
// Function Prototypes

/*	--------------------------------------------------------------------------------
	Function		: LoadWorldSfx
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void LoadWorldSfx ( SAMPLEMAP mapping[], char numSfx  )
{
	int i;

#ifdef MYDEBUG
	dprintf"NUM_SAMPLE : %d\n", numSfx));
#endif
	for ( i = numSfx - 1; i >= 0; i-- )
	{
#ifdef MYDEBUG
		dprintf"numSfx : %d - i : %d \n", numSfx, i));
		dprintf"mapping[%d] - sampleFileName : %s\n", i, mapping[i].sampleFileName));
#endif
		CreateAndAddSample ( mapping [ i ] );
	}
	// ENDFOR
}


/*	--------------------------------------------------------------------------------
	Function		: LoadSfx
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void LoadSfx ( unsigned long worldID )
{

	LoadWorldSfx ( genericMapping, NUM_GENERIC_SFX );

	switch ( worldID )
	{
		case WORLDID_GARDEN:
				LoadWorldSfx ( gardenMapping, NUM_GARDEN_SFX );	
			break;
	}
	// ENDSWITCH
}


/*	--------------------------------------------------------------------------------
	Function		: InitDirectSound
	Purpose			: Set's up Direct Sound
	Parameters		: void
	Returns			: void
	Info			: 
*/
SAMPLE *CreateAndAddSample ( SAMPLEMAP sampleMap )
{
	SAMPLE *newItem;
	HRESULT dsrVal;
	
	if ( !lpDS )
	{
#ifdef MYDEBUG
		dprintf"Returned from Create Sample, because lpDS was NULL!!!!!!!!\n"));
#endif
		return NULL;
	}
	// ENDIF
	if ( ( newItem = ( SAMPLE * ) JallocAlloc ( sizeof ( SAMPLE ), YES, "SAM" ) ) == NULL )
	{
#ifdef MYDEBUG
		dprintf"CreateAndAddSample : newItem : NULL value from JallocAlloc\n"));
#endif
		return NULL;
	}
	// ENDIF

	newItem->flags = sampleMap.flags;

	sprintf ( newItem->idName, "%s%s%s", baseDirectory, SFX_BASE, sampleMap.sampleFileName );

	newItem->lpDSound = lpDS;



	if ( LoadWav		( newItem->idName, newItem ) == 0 )
	{
#ifdef MYDEBUG
		dprintf"Returned from LoadWav\n"));
#endif
		return NULL;
	}
	// ENDIF

	
	if ( newItem->flags & FLAGS_3D_SAMPLE )
	{
		dprintf"Getting 3D Interface\n"));
//		Get3DInterface ( newItem->lpdsBuffer, newItem->lpds3DBuffer );
		newItem->lpdsBuffer->lpVtbl->QueryInterface ( newItem->lpdsBuffer, &IID_IDirectSound3DBuffer, (void**)&newItem->lpds3DBuffer );
		if ( newItem->lpds3DBuffer )
			dprintf"3D Buffer Ok\n"));
		else
			dprintf"Error On Interface, No 3D Buffer\n"));


	}
	// ENDIF


	newItem->numChannels	= sampleMap.numChannels;
	newItem->sampleRate		= sampleMap.sampleRate;
	newItem->bitsPerSample	= sampleMap.bitsPerSample;
	newItem->sampleID		= sampleMap.sampleID;

	SetSampleFormat ( newItem );

#ifdef MYDEBUG
	dprintf"Calling : AddSampleToList\n"));
#endif
	AddSampleToList ( newItem );
}

/*	--------------------------------------------------------------------------------
	Function		: InitDirectSound
	Purpose			: Set's up Direct Sound
	Parameters		: void
	Returns			: void
	Info			: 
*/
void InitSampleList ( void )
{
	//Init the sample list for the real samples.
	soundList.numEntries	= 0;
	soundList.head.next		= soundList.head.prev = &soundList.head;

	// Init the buffer list for samples that are playing
	bufferList.numEntries	= 0;
	bufferList.head.next	= bufferList.head.prev = &bufferList.head;
}


/*	--------------------------------------------------------------------------------
	Function		: InitDirectSound
	Purpose			: Set's up Direct Sound
	Parameters		: void
	Returns			: void
	Info			: 
*/
void AddSampleToList ( SAMPLE *sample )
{
#ifdef MYDEBUG
	dprintf"Adding Sample To List - sample->next : (&%x)\n", sample->next));
#endif
	if ( sample->next == NULL )
	{
#ifdef MYDEBUG
		dprintf"Adding Sample To List\n"));
#endif
		soundList.numEntries++;
		sample->prev				= &soundList.head;
		sample->next				= soundList.head.next;
		soundList.head.next->prev	= sample;
		soundList.head.next			= sample;
	}
	// ENDIF
}


/*	--------------------------------------------------------------------------------
	Function		: InitDirectSound
	Purpose			: Set's up Direct Sound
	Parameters		: void
	Returns			: void
	Info			: 
*/
void AddBufSampleToList ( BUFSAMPLE *bufSample )
{
#ifdef MYDEBUG
	dprintf"Adding Buffer Sample To List - bufSample->next : (&%x)\n", bufSample->next));
#endif
	if ( bufSample->next == NULL )
	{
#ifdef MYDEBUG
		dprintf"Adding Buffer Sample To List\n"));
#endif
		bufferList.numEntries++;
		bufSample->prev				= &bufferList.head;
		bufSample->next				= bufferList.head.next;
		bufferList.head.next->prev	= bufSample;
		bufferList.head.next		= bufSample;
	}
	// ENDIF
}


/*	--------------------------------------------------------------------------------
	Function		: InitDirectSound
	Purpose			: Set's up Direct Sound
	Parameters		: void
	Returns			: void
	Info			: 
*/
void RemoveSampleFromList ( SAMPLE *sample )
{
	if ( sample->next == NULL )
		return;

	sample->prev->next	= sample->next;
	sample->next->prev	= sample->prev;
	sample->next		= NULL;
	soundList.numEntries--;

	JallocFree ( ( UBYTE ** ) &sample );
}


/*	--------------------------------------------------------------------------------
	Function		: InitDirectSound
	Purpose			: Set's up Direct Sound
	Parameters		: void
	Returns			: void
	Info			: 
*/
void RemoveBufSampleFromList ( BUFSAMPLE *bufSample )
{
	if ( bufSample->next == NULL )
		return;

	bufSample->prev->next	= bufSample->next;
	bufSample->next->prev	= bufSample->prev;
	bufSample->next		= NULL;
	bufferList.numEntries--;

	ReleaseBuffer ( bufSample->lpdsBuffer );

//	bufSample->lpdsBuffer->lpVtbl->Release ( bufSample->lpdsBuffer );

	JallocFree ( ( UBYTE ** ) &bufSample );
}


/*	--------------------------------------------------------------------------------
	Function		: InitDirectSound
	Purpose			: Set's up Direct Sound
	Parameters		: void
	Returns			: void
	Info			: 
*/
void FreeSampleList ( void )
{
	SAMPLE *cur,*next;

	// check if any elements in list
	if ( soundList.numEntries == 0 )
		return;

	dprintf"Freeing linked list : SAMPLE : (%d elements)\n",soundList.numEntries));

	// traverse enemy list and free elements
	for ( cur = soundList.head.next; cur != &soundList.head; cur = next )
	{
		next = cur->next;

		RemoveSampleFromList ( cur );
	}
	// ENDFOR

	// initialise list for future use
	InitSampleList();
}


/*	--------------------------------------------------------------------------------
	Function		: InitDirectSound
	Purpose			: Set's up Direct Sound
	Parameters		: void
	Returns			: void
	Info			: 
*/
void FreeBufSampleList ( void )
{
	BUFSAMPLE *cur,*next;

	// check if any elements in list
	if ( bufferList.numEntries == 0 )
		return;

	dprintf"Freeing linked list : BUFSAMPLE : (%d elements)\n",bufferList.numEntries));

	// traverse enemy list and free elements
	for ( cur = bufferList.head.next; cur != &bufferList.head; cur = next )
	{
		next = cur->next;

		RemoveBufSampleFromList ( cur );
	}
	// ENDFOR

	// initialise list for future use
	InitSampleList();
}


/*	--------------------------------------------------------------------------------
	Function		: InitDirectSound
	Purpose			: Set's up Direct Sound
	Parameters		: void
	Returns			: void
	Info			: 
*/
SAMPLE *GetEntryFromSampleList ( int num )
{
	SAMPLE *next, *cur;

	for ( cur = soundList.head.next; cur != &soundList.head; cur = next )
	{
		next = cur->next;
		
		if ( cur->sampleID == num )
		{
			dprintf"cur->sampleID %d - num %d\n", cur->sampleID, num));
			return cur;
		}
		// ENDIF
	}
	// ENDFOR

#ifdef MYDEBUG
	dprintf"sampleID : %d - numEntries : %d\n", cur->sampleID, soundList.numEntries));
#endif
	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function		: PlaySample
	Purpose			: plays a sample....doh !
	Parameters		: short,VETOR,short,short
	Returns			: int
	Info			: 
*/
int PlaySample ( short num, VECTOR *pos, short tempVol, short pitch )
{
	SAMPLE *sample;

	BUFSAMPLE *bufSample;

	unsigned long bufStatus;

	if (!lpDS) return 0;	// No DirectSound object!

	if (!(sample = GetEntryFromSampleList ( num ))) return 0;

	if (sample->flags & FLAGS_3D_SAMPLE)
	{
		if ( sample->lpds3DBuffer )
		{
			sample->lpds3DBuffer->lpVtbl->SetMode ( sample->lpds3DBuffer, DS3DMODE_NORMAL, DS3D_IMMEDIATE );
			Set3DPosition ( sample->lpds3DBuffer, pos->v[X], pos->v[Y], pos->v[Z] );
		}
	}

	// Now test if the sample is playing if it is then make an instance of it to play.

	sample->lpdsBuffer->lpVtbl->GetStatus ( sample->lpdsBuffer, &bufStatus );

	if ( bufStatus & DSBSTATUS_PLAYING	)
	{
		/*	What we need to do here is create an instance of the buffer and store it in the buffer list.
			Have a clean buffer function that will go though and check if the sample is playing or not,
			if the sample is not playing then remove it from the list.
		*/

		// Create the buffer sample.
		if ( !(bufSample = (BUFSAMPLE *) JallocAlloc(sizeof(BUFSAMPLE), YES, "BUFSAM" )) )
		{
			return NULL;
		}

		lpDS->lpVtbl->DuplicateSoundBuffer ( lpDS, sample->lpdsBuffer, &(bufSample->lpdsBuffer ) );

		AddBufSampleToList ( bufSample );

		bufSample->lpdsBuffer->lpVtbl->Play ( bufSample->lpdsBuffer, 0, 0, 0 );
	}
	else
	{
		sample->lpdsBuffer->lpVtbl->Play ( sample->lpdsBuffer, 0, 0, 0 );
	}
}


/*	--------------------------------------------------------------------------------
	Function		: PlayActorBasedSample
	Purpose			: 
	Parameters		: sample num, actor, volume, pitch
	Returns			: void
*/
int PlayActorBasedSample( short num, ACTOR* act, short tempVol, short pitch )
{
	dprintf"La la la la\n"));
}

/*	--------------------------------------------------------------------------------
	Function		: PlayActorBasedSample
	Purpose			: 
	Parameters		: sample num, playback volume, volume, pitch
	Returns			: void
*/
int PlaySampleNot3D( short num, UBYTE vol, short tempVol, short pitch )
{
	dprintf"Dooby dooby doo\n"));
}


/*	--------------------------------------------------------------------------------
	Function		: SetSampleFormat
	Purpose			: Changes the format of the sound
	Parameters		: SAMPLE *sample : Contains the sample to change the format for.
	Returns			: void
	Info			: 
*/
void SetSampleFormat ( SAMPLE *sample )
{
	WAVEFORMATEX	wfx;

	memset ( &wfx, 0, sizeof ( WAVEFORMATEX ) ); 
	wfx.wFormatTag		= WAVE_FORMAT_PCM; 
	wfx.nChannels		= sample->numChannels; 
	wfx.nSamplesPerSec	= sample->sampleRate; 
	wfx.wBitsPerSample	= sample->bitsPerSample; 
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
	Function		: PlaySampleRadius
	Purpose			: 
	Parameters		: short,VECTOR,short,short,float
	Returns			: int
	Info			: 
*/
int PlaySampleRadius ( short num, VECTOR *pos, short vol, short pitch, float radius )
{
/*	SAMPLE *sample;

	sample = GetEntryFromSampleList ( num );

	sample->lpdsBuffer->lpVtbl->Play ( sample->lpdsBuffer, 0, 0, 0 );*/
}



/*	--------------------------------------------------------------------------------
	Function		: PlaySampleRadius
	Purpose			: 
	Parameters		: short,VECTOR,short,short,float
	Returns			: int
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
	// ENDIF

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
//			cur->lpdsBuffer->lpVtbl->Release( cur->lpdsBuffer );
			RemoveBufSampleFromList ( cur );

			if ( bufferList.numEntries == 0 )
				return;
			// ENDIF
		}
		else
		{
#ifdef MYDEBUG
			dprintf"Sample Still Playing\n"));
#endif
		}
		// ENDIF
	}
	// ENDFOR


}

























/*	--------------------------------------------------------------------------------
	Function		: PrepareSong
	Purpose			: prepares a song...and plays it...woohoo !
	Parameters		: char
	Returns			: void
	Info			: 
*/
void PrepareSong ( char num )
{
	// play cd audio track here....
	playCDTrack ( winInfo.hWndMain, num );
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
	// ENDIF

    // The device opened successfully; get the device ID.
    wDeviceID = mciOpenParms.wDeviceID;

    // Set the time format to track/minute/second/frame (TMSF).
    mciSetParms.dwTimeFormat = MCI_FORMAT_TMSF;

    if (dwReturn = mciSendCommand ( wDeviceID, MCI_SET,MCI_SET_TIME_FORMAT, ( DWORD )  ( LPVOID ) &mciSetParms ) )
    {
        mciSendCommand ( wDeviceID, MCI_CLOSE, 0, NULL );
		return dwReturn;
    }
	// ENDIF
	
	// Begin playback from the given track and play until the beginning 
    // of the next track. The window procedure function for the parent 
    // window will be notified with an MM_MCINOTIFY message when 
    // playback is complete. Unless the play command fails, the window 
    // procedure closes the device.    mciPlayParms.dwFrom = 0L;
    mciPlayParms.dwTo		= 0L;
    mciPlayParms.dwFrom		= MCI_MAKE_TMSF ( bTrack, 0, 0, 0 );
    mciPlayParms.dwTo		= MCI_MAKE_TMSF ( bTrack + 1, 0, 0, 0 );
    mciPlayParms.dwCallback = ( DWORD ) hWndNotify;
    if ( dwReturn = mciSendCommand ( wDeviceID, MCI_PLAY,MCI_FROM | MCI_TO | MCI_NOTIFY, (DWORD)(LPVOID) &mciPlayParms ) )
    {
        mciSendCommand ( wDeviceID, MCI_CLOSE, 0, NULL );
        return dwReturn;
    }
	// ENDIF

    return 0L;
}



