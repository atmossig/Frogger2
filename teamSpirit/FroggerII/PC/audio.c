/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: audio.c
	Programmer	: Andy Eder
	Date		: 28/06/99 10:37:45

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


//***********************************
// Function Prototypes

SOUNDLIST soundList;

//***********************************
// Forward Dec

int Makebuffer ( SAMPLE *sample );

//***********************************
// Function Prototypes

SAMPLE *CreateAndAddSample ( LPSTR lpFile )
{
	SAMPLE *newItem = ( SAMPLE * ) JallocAlloc ( sizeof ( SAMPLE ), YES, "SAM" );

	newItem->lpDSound = lpDS;

	LoadWav		( lpFile, newItem );
//	Makebuffer	( newItem );

	AddSampleToList ( newItem );

//	newItem->lpdsBuffer->lpVtbl->Play ( newItem->lpdsBuffer, 0, 0, DSBPLAY_LOOPING );
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



int Makebuffer ( SAMPLE *sample )
{
	int result=0;
	LPVOID	pBlk1, pBlk2;
	unsigned long	dwSize1=0, dwSize2=0;
	DSBUFFERDESC	DSBD;

	if(sample->lpdsBuffer) {
		IDirectSoundBuffer_Release(sample->lpdsBuffer);
		sample->lpdsBuffer = NULL;
	}

	ZeroMemory(&DSBD, sizeof(DSBUFFERDESC));
	DSBD.dwSize = sizeof(DSBUFFERDESC);
	DSBD.dwFlags = DSBCAPS_STICKYFOCUS | DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2;
	DSBD.dwBufferBytes = sample->Len;
	DSBD.lpwfxFormat = sample->lpWavFmt;
	result=IDirectSound_CreateSoundBuffer ( sample->lpDSound, &DSBD, &sample->lpdsBuffer, NULL);

	// add error handling here...
	if(sample->lpdsBuffer) {
		IDirectSoundBuffer_Lock(sample->lpdsBuffer, 0,sample->Len, &pBlk1, &dwSize1, &pBlk2, &dwSize2, 0);
		CopyMemory(pBlk1, sample->Data, dwSize1);
		if(dwSize2) {
			CopyMemory(pBlk2, sample->Data, dwSize2);
		}
		IDirectSoundBuffer_Unlock ( sample->lpdsBuffer, pBlk1, dwSize1, pBlk2, dwSize2);

		return 1;
	}

	//DisplayDirectSoundError(result);

	//LogFile("UnFunky. Couldn't make DSoundBuffer. And Shit.");
	return 0;
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
int PlaySample(short num,VECTOR *pos,short tempVol,short pitch)
{
	return 0;
}

/*	--------------------------------------------------------------------------------
	Function		: PlaySampleRadius
	Purpose			: 
	Parameters		: short,VECTOR,short,short,float
	Returns			: int
	Info			: 
*/
int PlaySampleRadius(short num, VECTOR *pos, short vol,short pitch,float radius)
{
	return 0;
}

/*	--------------------------------------------------------------------------------
	Function		: PrepareSong
	Purpose			: prepares a song...and plays it...woohoo !
	Parameters		: char
	Returns			: void
	Info			: 
*/
void PrepareSong(char num)
{
	// play cd audio track here....
}

