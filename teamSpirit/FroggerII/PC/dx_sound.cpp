/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: dx_sound.cpp
	Programmer	: James Healey
	Date		: 16/08/99

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <mmsystem.h>
#include <dsound.h>
#include <memory.h>
#include <winuser.h>
#include <commctrl.h>
#include <commdlg.h>
#include <cderr.h>
#include "..\resource.h"
#include "incs.h"


LPDIRECTSOUND           lpDS			= NULL;
LPDIRECTSOUNDBUFFER		lpdsbPrimary	= NULL;

static char *DSoundErrorToString(int dxerror)
{
	switch(dxerror)
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


int DSoundEnumerate ( LPGUID lpGUID, HINSTANCE hInst,  HWND hWndMain )
{
    if ( DialogBoxParam ( hInst, MAKEINTRESOURCE ( IDD_SOUNDDRIVER ), hWndMain, (DLGPROC)DSEnumDlgProc, ( LPARAM ) lpGUID ))
		return TRUE;
	// ENDIF

    return FALSE;
}


BOOL CALLBACK DSEnumProc( LPGUID lpGUID, LPSTR lpszDesc,
				LPSTR lpszDrvName, LPVOID lpContext )
    {
    HWND   hCombo = *(HWND *)lpContext;
    LPGUID lpTemp = NULL;

    if( lpGUID != NULL )
	{
//		lpTemp = LocalAlloc ( LMEM_FIXED | LMEM_ZEROINIT , sizeof ( GUID ) );
		if ( ( lpTemp = (GUID*)LocalAlloc ( LPTR , sizeof ( GUID ) ) ) == NULL )
			return( TRUE );

		memcpy( lpTemp, lpGUID, sizeof(GUID));
	}

    ComboBox_AddString( hCombo, lpszDesc );
    ComboBox_SetItemData( hCombo,
			ComboBox_FindString( hCombo, 0, lpszDesc ),
			lpTemp );
    return( TRUE );
    }


BOOL CALLBACK DSEnumDlgProc( HWND hDlg, UINT msg,
				WPARAM wParam, LPARAM lParam )
{
    static HWND   hCombo;
    static LPGUID lpGUID;
    LPGUID        lpTemp;
    int           i;

    switch( msg )
	{
	case WM_INITDIALOG:
	    hCombo = GetDlgItem( hDlg, IDC_SOUNDCOMBO );
	    lpGUID = (LPGUID)lParam;

	    if( DirectSoundEnumerate( (LPDSENUMCALLBACK)DSEnumProc, &hCombo ) != DS_OK )
		{
		EndDialog( hDlg, TRUE );
		return( TRUE );
		}
	    if( ComboBox_GetCount( hCombo ))
		ComboBox_SetCurSel( hCombo, 0 );
	    else
		{
		EndDialog( hDlg, TRUE );
		return( TRUE );
		}
	    return( TRUE );


	case WM_COMMAND:
	    switch( LOWORD( wParam ))
		{
		case ID_SOUNDOK:
		    for( i = 0; i < ComboBox_GetCount( hCombo ); i++ )
			{
			lpTemp = (LPGUID)ComboBox_GetItemData( hCombo, i );
			if( i == ComboBox_GetCurSel( hCombo ))
			    {
			    if( lpTemp != NULL )
				memcpy( lpGUID, lpTemp, sizeof(GUID));
			    else
				lpGUID = NULL;
			    }
			if( lpTemp )
			    LocalFree( lpTemp );
			}
		    // If we got the NULL GUID, then we want to open the default
		    // sound driver, so return with an error and the init code
		    // will know not to pass in the guID and will send NULL
		    // instead.
		    if( lpGUID == NULL )
			EndDialog( hDlg, TRUE );
		    else
			EndDialog( hDlg, FALSE );
		    return( TRUE );

		case ID_SOUNDCANCEL:
		    // Force a NULL GUID
		    EndDialog( hDlg, TRUE );
		    return( TRUE );
		}
	    break;


	default:
	    return( FALSE );
	}

    return( FALSE );
}


int InitDirectSound ( GUID *guid, HINSTANCE hInst,  HWND hWndMain, int prim )
{
	WAVEFORMATEX	wfx;
	DSBUFFERDESC	dsbdesc;
	HRESULT			dsrVal;

	if ( prim )
	{
		DirectSoundCreate ( guid, &lpDS, NULL );
	}
	else
	{
		DirectSoundCreate ( NULL, &lpDS, NULL );
	}
	// ENDELSIF

	dsrVal = lpDS->SetCooperativeLevel ( hWndMain, DSSCL_EXCLUSIVE );

	if ( dsrVal != DS_OK )
	{
		lpDS->Release();
		lpDS = NULL;
		return 0;
	}
	// ENDIF

	ZeroMemory ( &dsbdesc, sizeof ( DSBUFFERDESC ) );
	dsbdesc.dwSize	= sizeof ( DSBUFFERDESC );
	dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

	
	dsrVal = lpDS->CreateSoundBuffer ( &dsbdesc, &lpdsbPrimary, NULL ) ;
	if ( dsrVal != DS_OK )
	{
		lpDS->Release();
		lpDS = NULL;
		return 0;
	}
	// ENDIF


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
		return 0;
	}

	return 1;
}


DWORD LoadWavFile(char *fileName,  SAMPLE *sample)
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
	if ( ( hwav = mmioOpen ( fileName, NULL, MMIO_READ | MMIO_ALLOCBUF ) ) == NULL )
		return 0;
	// ENDIF

	// descend into the RIFF 
	parent.fccType = mmioFOURCC ( 'W', 'A', 'V', 'E' );

	if ( mmioDescend ( hwav, &parent, NULL, MMIO_FINDRIFF ) )
    {
		// close the file
		mmioClose ( hwav, 0 );

		// return error, no wave section
		return 0; 	
    }
	// ENDIF

	// descend to the WAVEfmt 
	child.ckid = mmioFOURCC ( 'f', 'm', 't', ' ' );

	if ( mmioDescend ( hwav, &child, &parent, 0 ) )
    {
		// close the file
		mmioClose ( hwav, 0 );

		// return error, no format section
		return 0; 	
    }
	// ENDIF

	// now read the wave format information from file
	if ( mmioRead ( hwav, ( char * ) &wfmtx, sizeof ( wfmtx ) ) != sizeof ( wfmtx ) )
    {
		// close file
		mmioClose ( hwav, 0 );

		// return error, no wave format data
		return 0;
    }
	// ENDIF

	// make sure that the data format is PCM
	if ( wfmtx.wFormatTag != WAVE_FORMAT_PCM )
    {
		// close the file
		mmioClose ( hwav, 0 );

		// return error, not the right data format
		return 0; 
    }
	// ENDIF

	// now ascend up one level, so we can access data chunk
	if ( mmioAscend ( hwav, &child, 0 ) )
	{
		// close file
		mmioClose ( hwav, 0 );

		// return error, couldn't ascend
		return 0; 	
	}
	// ENDIF

	// descend to the data chunk 
	child.ckid = mmioFOURCC ( 'd', 'a', 't', 'a' );

	if ( mmioDescend ( hwav, &child, &parent, MMIO_FINDCHUNK ) )
    {
		// close file
		mmioClose ( hwav, 0 );

		// return error, no data
		return 0; 	
    }
	// ENDIF


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
	pcmwf.nChannels		  = 2;                // mono 
	pcmwf.nSamplesPerSec  = 22050;            // always this rate
	pcmwf.wBitsPerSample  = 16;
	pcmwf.nBlockAlign	  = pcmwf.wBitsPerSample / 8 * pcmwf.nChannels;                
	pcmwf.nAvgBytesPerSec = pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;

	// prepare to create sounds buffer
	ZeroMemory ( &dsbd, sizeof ( DSBUFFERDESC ) );
	dsbd.dwSize			= sizeof(DSBUFFERDESC);
	dsbd.dwFlags		= DSBCAPS_CTRLDEFAULT | DSBCAPS_STATIC;
	dsbd.dwBufferBytes	= child.cksize;
	dsbd.lpwfxFormat	= &pcmwf;

	// create the sound buffer

	dsrVal = lpDS->CreateSoundBuffer ( &dsbd, &sample->lpdsBuffer, NULL );
	if ( dsrVal != DS_OK )
	{
		dp("CreateSoundBuffer failed on file '%s' - '%s'\n", fileName, DSoundErrorToString(dsrVal));

		// release memory
		free ( snd_buffer );

		// return error
		return ( -1 );
	}
	// ENDIF

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




	return 0;

} // LoadFile



int LoadWav ( LPSTR lpFile, SAMPLE *sample )
{
	LPWAVEFORMATEX	oldWavFmt=sample->lpWavFmt;
	BYTE			*oldData=sample->Data;
	
	sample->Len = LoadWavFile ( lpFile, sample );
	
	if(sample->Len) {
		if (oldWavFmt) {
			delete oldWavFmt;
		}

		if (oldData) {
			delete oldData;
		}

		return true;
	} else {
		sample->Data = oldData;
		sample->lpWavFmt = oldWavFmt;

		return false;
	}
}


