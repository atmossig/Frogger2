/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: audio.c
	Programmer	: James Healey
	Date		: 28/06/99 10:37:45

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>
#include "..\resource.h"
#include "incs.h"

#define MYDEBUG

SAMPLEMAP sampleMapping [] = {	"x:\\teamspirit\\pcversion\\sfx\\levelcomp.wav",		2, 22050, 16, GEN_LEVEL_COMP,
								"x:\\teamspirit\\pcversion\\sfx\\targetcomplete.wav",	2, 22050, 16, GEN_TARGET_COM,
								"x:\\teamspirit\\pcversion\\sfx\\timeout.wav",			2, 22050, 16, GEN_TIME_OUT,
								"x:\\teamspirit\\pcversion\\sfx\\clocktock.wav",		2, 22050, 16, GEN_CLOCK_TOCK,
								"x:\\teamspirit\\pcversion\\sfx\\clocktick.wav",		2, 22050, 16, GEN_CLOCK_TICK,
								"x:\\teamspirit\\pcversion\\sfx\\froghop.wav",			2, 22050, 16, GEN_FROG_HOP,
								"x:\\teamspirit\\pcversion\\sfx\\superhop.wav",			2, 22050, 16, GEN_SUPER_HOP,
								"x:\\teamspirit\\pcversion\\sfx\\babyfrog.wav",			2, 22050, 16, GEN_BABY_FROG };

//***********************************
// Function Prototypes

SOUNDLIST soundList;

BUFFERLIST bufferList;

//***********************************
// Function Prototypes

/*	--------------------------------------------------------------------------------
	Function		: InitDirectSound
	Purpose			: Set's up Direct Sound
	Parameters		: void
	Returns			: void
	Info			: 
*/
void LoadDemoSamples ( void )
{
	int i;

#ifdef MYDEBUG
	dprintf"NUM_SAMPLE : %d\n", NUM_SAMPLES));
#endif
	for ( i = NUM_SAMPLES - 1; i >= 0; i-- )
	{
#ifdef MYDEBUG
		dprintf"NUM_SAMPLE : %d - i : %d \n", NUM_SAMPLES, i));
		dprintf"sampleMapping[%d] - sampleFileName : %s\n", i, sampleMapping[i].sampleFileName));
#endif
		CreateAndAddSample ( sampleMapping [ i ] );
	}
	// ENDFOR
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
	
	if ( !lpDS )
	{
		dprintf"Returned from Create Sample, because lpDS was NULL!!!!!!!!"));
		return NULL;
	}
	// ENDIF
	if ( ( newItem = ( SAMPLE * ) JallocAlloc ( sizeof ( SAMPLE ), YES, "SAM" ) ) == NULL )
	{
		dprintf"CreateAndAddSample : newItem : NULL value from JallocAlloc\n"));
		return NULL;
	}
	// ENDIF

	sprintf ( newItem->idName, "%s", sampleMap.sampleFileName );

	newItem->lpDSound = lpDS;

	if ( LoadWav		( newItem->idName, newItem ) == 0 )
	{
		dprintf"Returned from LoadWav\n"));
		return NULL;
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

	bufSample->lpdsBuffer->lpVtbl->Release ( bufSample->lpdsBuffer );
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
			return cur;
		}
		// ENDIF
	}
	// ENDFOR

#ifdef MYDEBUG
	dprintf"sampleID : %d - numEntries : %d", cur->sampleID, soundList.numEntries));
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

	if ( !lpDS )
	{
		dprintf"Returned From PlaySample Because lpDS is NULL!!!!!!!!!\n"));
		return 0;
	}
	// ENDIF

#ifdef MYDEBUG
	dprintf"Getting Entry From Sample List - %d\n", num));
#endif
	sample = GetEntryFromSampleList ( num );

	if ( ( !sample ) )
	{
		if ( !sample )
		{
			dprintf"Could Not Find Sample : %d\n", num));	//  I will leave this, cos if you enter a
															// sample number and it's not there then you will not get a sound.
		}
		// ENDIF
		return 0;
	}
	// ENDIF


#ifdef MYDEBUG
	dprintf"About to Play Sample - %d\n", num));
#endif

	// Now test if the sample is playing if it is then make an instance of it to play.

	sample->lpdsBuffer->lpVtbl->GetStatus ( sample->lpdsBuffer, &bufStatus );

	if ( bufStatus & DSBSTATUS_PLAYING	)
	{
		/*	What we need to do here is create an instance of the buffer and store it in the buffer list.
			Have a clean buffer function that will go though and check if the sample is playing or not,
			if the sample is not playing then remove it from the list.
		*/
		
		dprintf"Buffering Sample %d\n", num));
		// Create the buffer sample.
		if ( ( bufSample = ( BUFSAMPLE * ) JallocAlloc ( sizeof ( BUFSAMPLE ), YES, "BUFSAM" ) ) == NULL )
		{
			dprintf"PlaySample : bufSample : NULL value from JallocAlloc\n"));
			return NULL;
		}
		// ENDIF

		lpDS->lpVtbl->DuplicateSoundBuffer ( lpDS, sample->lpdsBuffer, &(bufSample->lpdsBuffer ) );

		AddBufSampleToList ( bufSample );

		bufSample->lpdsBuffer->lpVtbl->Play ( bufSample->lpdsBuffer, 0, 0, 0 );


		dprintf"Sorry Sound Playing : %d\n", num));
	}
	else
	{
		sample->lpdsBuffer->lpVtbl->Play ( sample->lpdsBuffer, 0, 0, 0 );
	}
	// ENDIF



#ifdef MYDEBUG
	dprintf"Played Sample Ok - %d\n", num));
#endif
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
	dprintf"Set Sample Format : Buffer = (&%x)", sample->lpdsBuffer));
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



void CleanBufferSamples ( void )
{
	BUFSAMPLE *cur, *next;

	unsigned long bufStatus;

	if ( bufferList.numEntries == 0)
	{
		dprintf"No Samples in Buffer List\n"));
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
			dprintf"Releasing Buffered Sample\n"));
//			cur->lpdsBuffer->lpVtbl->Release( cur->lpdsBuffer );
			RemoveBufSampleFromList ( cur );

			if ( bufferList.numEntries == 0 )
				return;
			// ENDIF
		}
		else
		{
			dprintf"Sample Still Playing\n"));
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
