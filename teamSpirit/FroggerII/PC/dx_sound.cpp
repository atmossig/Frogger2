/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: dx_sound.cpp
	Programmer	: James Healey
	Date		: 16/08/99

----------------------------------------------------------------------------------------------- */

//***********************************
// System Includes

#include <ultra64.h>
#include <windowsx.h>
#include <stdio.h>

//***********************************
// User Includes

#include "..\resource.h"
#include "incs.h"


//***********************************
// User Includes

LPDIRECTSOUND           lpDS			= NULL;
LPDIRECTSOUNDBUFFER		lpdsbPrimary	= NULL;
LPDIRECTSOUND3DLISTENER lpds3DListener	= NULL;

//***********************************
// Function Definitions

static char *DSoundErrorToString ( int dxerror )
{
	switch ( dxerror )
		{
		case DS_OK:
			return "The request completed successfully.";

		case DSERR_ALLOCATED:
			return "The request failed because resources, such as a priority level, were already in use by another caller.";

		case DSERR_ALREADYINITIALIZED:
			return "The object is already initialized.";

		case DSERR_BADFORMAT:
			return "The specified wave format is not supported.";

		case DSERR_BUFFERLOST:
			return "The buffer memory has been lost and must be restored.";

		case DSERR_CONTROLUNAVAIL:
			return "The control (volume, pan, and so forth) requested by the caller is not available.";

		case DSERR_GENERIC:
			return "An undetermined error occurred inside the DirectSound subsystem.";

		case DSERR_INVALIDCALL:
			return "This function is not valid for the current state of this object.";

		case DSERR_INVALIDPARAM:
			return "An invalid parameter was passed to the returning function.";

		case DSERR_NOAGGREGATION:
			return "The object does not support aggregation.";

		case DSERR_NODRIVER:
			return "No sound driver is available for use.";

		case DSERR_OTHERAPPHASPRIO:
			return "Another application has a higher priority level, preventing this call from succeeding";

		case DSERR_OUTOFMEMORY:
			return "The DirectSound subsystem could not allocate sufficient memory to complete the caller's request.";

		case DSERR_PRIOLEVELNEEDED:
			return "The caller does not have the priority level required for the function to succeed.";

		case DSERR_UNINITIALIZED:
			return "The IDirectSound_Initialize method has not been called or has not been called successfully before other methods were called.";

		case DSERR_UNSUPPORTED:
			return "The function called is not supported at this time.";
		}

	return "Unknown Error";
}


/*int DSoundEnumerate ( LPGUID lpGUID, HINSTANCE hInst,  HWND hWndMain )
{
  /*  if ( DialogBoxParam ( hInst, MAKEINTRESOURCE ( IDD_SOUNDDRIVER ), hWndMain, (DLGPROC)DSEnumDlgProc, ( LPARAM ) lpGUID ))
		return TRUE;
	// ENDIF
*/
   /* return FALSE;
}*/


int InitDirectSound ( GUID *guid, HINSTANCE hInst,  HWND hWndMain, int prim )
{
	WAVEFORMATEX	wfx;
	DSBUFFERDESC	dsbdesc;
	HRESULT			dsrVal;

	if ( prim )
		dsrVal = DirectSoundCreate ( guid, &lpDS, NULL );
	else
		dsrVal = DirectSoundCreate ( NULL, &lpDS, NULL );

	if ( dsrVal != DS_OK )
	{
		dp("DirectSound Create failed - '%s'\n", DSoundErrorToString(dsrVal));
		lpDS = NULL;
		return 0;
	}

	dsrVal = lpDS->SetCooperativeLevel ( hWndMain, DSSCL_EXCLUSIVE );

	if ( dsrVal != DS_OK )
	{
		dp("Set Cooperative Level failed - '%s'\n", DSoundErrorToString(dsrVal));
		lpDS->Release();
		lpDS = NULL;
		return 0;
	}

	ZeroMemory ( &dsbdesc, sizeof ( DSBUFFERDESC ) );
	dsbdesc.dwSize	= sizeof ( DSBUFFERDESC );
	dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;

	
	dsrVal = lpDS->CreateSoundBuffer ( &dsbdesc, &lpdsbPrimary, NULL ) ;
	if ( dsrVal != DS_OK )
	{
		dp("Create Sound Buffer failed - '%s'\n", DSoundErrorToString(dsrVal));
		lpDS->Release();
		lpDS = NULL;
		return 0;
	}

	memset ( &wfx, 0, sizeof ( WAVEFORMATEX ) ); 
	wfx.wFormatTag		= WAVE_FORMAT_PCM; 
	wfx.nChannels		= 2; 
	wfx.nSamplesPerSec	= 22050; 
	wfx.wBitsPerSample	= 16; 
	wfx.nBlockAlign		= wfx.wBitsPerSample / 8 * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
 
	dsrVal = lpdsbPrimary->SetFormat ( &wfx );

	if ( dsrVal != DS_OK )
	{
		dp("Set Format failed - '%s'\n", DSoundErrorToString(dsrVal));
		return 0;
	}

    dsrVal = lpdsbPrimary->QueryInterface( IID_IDirectSound3DListener, ( void** ) &lpds3DListener );

	if ( dsrVal != DS_OK )
	{
		dp("Query Interface For 3d Listener Failed - '%s'\n", DSoundErrorToString(dsrVal));
		return 0;
	}

	return 1;
}


void ShutDownDirectSound ( void )
{
}


int LoadWav( SAMPLE *sample )
{
	HRESULT			dsrVal;
	HMMIO 			hwav;    // handle to wave file
	MMCKINFO		parent,  // parent chunk
					child;   // child chunk
	WAVEFORMATEX    wfmtx;   // wave format structure

	WAVEFORMATEX		pcmwf;          // generic waveformat structure

	UCHAR *snd_buffer,       // temporary sound buffer to hold voc data
		  *audio_ptr_1=NULL, // data ptr to first write buffer 
		  *audio_ptr_2=NULL; // data ptr to second write buffer

	unsigned long	audio_length_1=0,  // length of first write buffer
					audio_length_2=0;  // length of second write buffer

	DSBUFFERDESC		dsbd;           // directsound description

	// open the WAV file
	if( (hwav = mmioOpen( sample->idName, NULL, MMIO_READ | MMIO_ALLOCBUF )) == NULL )
	{
		dp("RETURNING : LoadWavFile - Opening Wav File.\n");
		return 0;
	}

	// descend into the RIFF 
	parent.fccType = mmioFOURCC ( 'W', 'A', 'V', 'E' );

	if ( mmioDescend ( hwav, &parent, NULL, MMIO_FINDRIFF ) )
    {
		// close the file
		mmioClose ( hwav, 0 );

		// return error, no wave section
		return 0; 	
    }

	// descend to the WAVEfmt 
	child.ckid = mmioFOURCC ( 'f', 'm', 't', ' ' );

	if ( mmioDescend ( hwav, &child, &parent, 0 ) )
    {
		// close the file
		mmioClose ( hwav, 0 );

		// return error, no format section
		return 0; 	
    }

	// now read the wave format information from file
	if ( mmioRead ( hwav, ( char * ) &wfmtx, sizeof ( wfmtx ) ) != sizeof ( wfmtx ) )
    {
		// close file
		mmioClose ( hwav, 0 );

		// return error, no wave format data
		return 0;
    }

	// make sure that the data format is PCM
	if ( wfmtx.wFormatTag != WAVE_FORMAT_PCM )
    {
		// close the file
		mmioClose ( hwav, 0 );

		// return error, not the right data format
		return 0; 
    }

	// now ascend up one level, so we can access data chunk
	if ( mmioAscend ( hwav, &child, 0 ) )
	{
		// close file
		mmioClose ( hwav, 0 );

		// return error, couldn't ascend
		return 0; 	
	}

	// descend to the data chunk 
	child.ckid = mmioFOURCC ( 'd', 'a', 't', 'a' );

	if ( mmioDescend ( hwav, &child, &parent, MMIO_FINDCHUNK ) )
    {
		// close file
		mmioClose ( hwav, 0 );

		// return error, no data
		return 0; 	
    }

	// finally!!!! now all we have to do is read the data in and
	// set up the directsound buffer

	// allocate the memory to load sound data
	snd_buffer = ( UCHAR * ) malloc ( child.cksize );

	// read the wave data 
	mmioRead ( hwav, ( char * ) snd_buffer, child.cksize );

	// close the file
	mmioClose ( hwav, 0 );

	// set up the format data structure
	memset(&pcmwf, 0, sizeof(WAVEFORMATEX));

	pcmwf.wFormatTag	  = WAVE_FORMAT_PCM;  // pulse code modulation
	pcmwf.nChannels		  = 2;                // stereo 
	pcmwf.nSamplesPerSec  = 22050;            // always this rate
	pcmwf.wBitsPerSample  = 16;
	pcmwf.nBlockAlign	  = pcmwf.wBitsPerSample / 8 * pcmwf.nChannels;                
	pcmwf.nAvgBytesPerSec = pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;

	// prepare to create sounds buffer
	ZeroMemory ( &dsbd, sizeof ( DSBUFFERDESC ) );
	dsbd.dwSize			= sizeof(DSBUFFERDESC);
	dsbd.dwFlags		= DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_STATIC;
	if ( sample->flags & FLAGS_3D_SAMPLE )
	{
		dsbd.dwFlags |= DSBCAPS_CTRL3D;
	}

	dsbd.dwBufferBytes	= child.cksize;
	dsbd.lpwfxFormat	= &pcmwf;

	// create the sound buffer

	dsrVal = lpDS->CreateSoundBuffer ( &dsbd, &sample->lpdsBuffer, NULL );
	dp("Createing Sample Buffer = (&%x) FileName : %s", sample->lpdsBuffer, sample->idName);
	if ( dsrVal != DS_OK )
	{
		dp("CreateSoundBuffer failed on file '%s' - '%s'\n", sample->idName, DSoundErrorToString(dsrVal));

		// release memory
		free ( snd_buffer );

		// return error
		return ( -1 );
	}

	// copy data into sound buffer
	if ( sample->lpdsBuffer->Lock ( 0, child.cksize, (void **) &audio_ptr_1, &audio_length_1, (void **)&audio_ptr_2, &audio_length_2, DSBLOCK_FROMWRITECURSOR ) != DS_OK )
		 return(0);

	// copy first section of circular buffer
	memcpy(audio_ptr_1, snd_buffer, audio_length_1);

	// copy last section of circular buffer
	memcpy(audio_ptr_2, (snd_buffer+audio_length_1),audio_length_2);

	// unlock the buffer
	if (sample->lpdsBuffer->Unlock(audio_ptr_1, 
											audio_length_1, 
											audio_ptr_2, 
											audio_length_2)!=DS_OK)
 									 return(0);

	// release the temp buffer
	free(snd_buffer);

	return 1;
}


void Update3DListener ( float sourceX, float sourceY, float sourceZ )
{
	lpds3DListener->SetPosition			( 0.0, 0.0, 0.0, DS3D_IMMEDIATE );
	lpds3DListener->SetDopplerFactor	( 1.0f, DS3D_IMMEDIATE );
	lpds3DListener->SetDistanceFactor	( 5.0f, DS3D_IMMEDIATE );

//	lpds3DListener->CommitDeferredSettings

}




void Get3DInterface ( LPDIRECTSOUNDBUFFER lpdsBuffer, LPDIRECTSOUND3DBUFFER lpds3DBuffer )
{
	lpdsBuffer->QueryInterface ( IID_IDirectSound3DBuffer, (void**)&lpds3DBuffer );
}


void ReleaseBuffer ( LPDIRECTSOUNDBUFFER lpdsBuffer )
{
	lpdsBuffer->Release();
}

void Set3DPosition ( LPDIRECTSOUND3DBUFFER lpds3DBuffer, float xPos, float yPos, float zPos )
{
	lpds3DBuffer->SetPosition ( 0.0f, 0.0f, 10.0f, DS3D_IMMEDIATE );
	lpds3DBuffer->SetMinDistance ( 4.0, DS3D_IMMEDIATE );
}
