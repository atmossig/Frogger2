/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: audio.c
	Programmer	: Andy Eder
	Date		: 28/06/99 10:37:45

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>
#include "..\resource.h"
#include "incs.h"


//***********************************
// Function Prototypes

SOUNDLIST soundList;

//***********************************
// Forward Dec

int Makebuffer ( SAMPLE *sample );

//***********************************
// Function Prototypes

SAMPLE *CreateAndAddSample ( char *lpFile )
{
	SAMPLE *newItem = ( SAMPLE * ) JallocAlloc ( sizeof ( SAMPLE ), YES, "SAM" );

	sprintf ( newItem->idName, "%s", lpFile );

	newItem->lpDSound = lpDS;

	if ( lpDS )
		LoadWav		( lpFile, newItem );
	// ENDIF

	SetSampleFormat ( newItem, 2, 22050, 16 );

	AddSampleToList ( newItem );
}

void InitSampleList ( void )
{
	soundList.numEntries	= 0;
	soundList.head.next		= soundList.head.prev = &soundList.head;
}

void AddSampleToList ( SAMPLE *sample )
{
	if ( sample->next == NULL )
	{
		soundList.numEntries++;
		sample->prev				= &soundList.head;
		sample->next				= soundList.head.next;
		soundList.head.next->prev	= sample;
		soundList.head.next			= sample;
	}
	// ENDIF
}

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


SAMPLE *GetEntryFromSampleList ( int num )
{
	int i;
	SAMPLE *next, *cur;

	i = 0;
	for ( cur = soundList.head.next; cur != &soundList.head; cur = next )
	{
		next = cur->next;
		
		if ( i == num )
		{
			return cur;
		}
		// ENDIF
		i++;
	}
	// ENDFOR

	return NULL;
}



/*	--------------------------------------------------------------------------------
	Function		: InitDirectSound
	Purpose			: Set's up Direct Sound
	Parameters		: void
	Returns			: void
	Info			: 
*/
//void InitDirectSound ( void )//
//{
//}


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
//	return 0;

	sample = GetEntryFromSampleList ( num );

	if ( ( !sample ) || ( !lpDS ) )
	{
		if ( !sample )
		{
			dprintf"Could Not Find Sample : %d\n", num));	//  I will leave this, cos if you enter a
															// sample number and it's not there then you will not get a sound.
		}
		if ( !lpDS )
			dprintf"lpDS duh!!!!\n"));
		return 0;
	}
	// ENDIF


	sample->lpdsBuffer->lpVtbl->Play ( sample->lpdsBuffer, 0, 0, 0 );
	dprintf"Played Sample Ok - %d", num));
}


void SetSampleFormat ( SAMPLE *sample, char numChannels, int newSampleRate, char bitsPerSample )
{
	WAVEFORMATEX	wfx;

	memset ( &wfx, 0, sizeof ( WAVEFORMATEX ) ); 
	wfx.wFormatTag		= WAVE_FORMAT_PCM; 
	wfx.nChannels		= numChannels; 
	wfx.nSamplesPerSec	= newSampleRate; 
	wfx.wBitsPerSample	= bitsPerSample; 
	wfx.nBlockAlign		= wfx.wBitsPerSample / 8 * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	sample->lpdsBuffer->lpVtbl->SetFormat ( sample->lpdsBuffer, &wfx );
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
	SAMPLE *sample;

	sample = GetEntryFromSampleList ( num );

	sample->lpdsBuffer->lpVtbl->Play ( sample->lpdsBuffer, 0, 0, 0 );
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
