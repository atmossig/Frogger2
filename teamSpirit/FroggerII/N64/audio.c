/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: audio.c
	Programmer	: James Healey
	Date		: 20/04/99 14:42:36

----------------------------------------------------------------------------------------------- */


#define F3DEX_GBI_2
#define NNSCHED

#include <ultra64.h>
#include "incs.h"

AUDIOCONTROL	audioCtrl;
int MAX_SFX_DIST = 500;
BOOL reverbOn = 0;

AMBIENT_SOUND_LIST	ambientSoundList;
int numContinuousSamples = 0;

unsigned long sfxVol	= 100;
unsigned long musicVol	= 100;

short currentScriptEntry = 0;
int musresult;


/*	--------------------------------------------------------------------------------
	Function 	: SubAmbientSound
	Purpose 	: Remove ambient sound from playlist
	Parameters 	: Ambient sound to remove
	Returns 	: 
	Info 		:
*/
void SubAmbientSound(AMBIENT_SOUND *ambientSound)
{
	ambientSound->prev->next = ambientSound->next;
	ambientSound->next->prev = ambientSound->prev;
	ambientSoundList.numEntries--;
}


/*	--------------------------------------------------------------------------------
	Function 	: InitAmbientSoundList
	Purpose 	: Initialise the ambient list to empty.
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitAmbientSoundList()
{
	ambientSoundList.head.next = ambientSoundList.head.prev = &ambientSoundList.head;
	ambientSoundList.numEntries = 0;
}


/*	--------------------------------------------------------------------------------
	Function 	: PrepareSong()
	Purpose 	: dma's down all the bits of a song and sets up the music structure
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void PrepareSong(char num)
{
	u8	*tuneStart, *tuneEnd;


	MusHandleSetReverb(audioCtrl.musicHandle,0);
	MusHandleSetVolume(audioCtrl.musicHandle, 255);//0x20);

	//stop existing audio track	
	if(num == audioCtrl.currentTrack)
		return;



	audioCtrl.currentTrack = num;

	MusHandleStop(audioCtrl.musicHandle, 1);
	while(MusHandleAsk(audioCtrl.musicHandle));

	if(num == NOTRACK)
		return;

	switch(num)
	{
/*		case HUB1_TRACK:
			tuneStart = _mbank28SegmentRomStart;
			tuneEnd = _mbank28SegmentRomEnd;
			break;
		case HUB2_TRACK:
			tuneStart = _mbank29SegmentRomStart;
			tuneEnd = _mbank29SegmentRomEnd;
			break;
		case HUB3_TRACK:
			tuneStart = _mbank30SegmentRomStart;
			tuneEnd = _mbank30SegmentRomEnd;
			break;
		case HUB4_TRACK:
			tuneStart = _mbank31SegmentRomStart;
			tuneEnd = _mbank31SegmentRomEnd;
			break;
		case HUB5_TRACK:
			tuneStart = _mbank32SegmentRomStart;
			tuneEnd = _mbank32SegmentRomEnd;
			break;
		case HUB6_TRACK:
			tuneStart = _mbank33SegmentRomStart;
			tuneEnd = _mbank33SegmentRomEnd;
			break;
					*/
/*		case ATLANTIS_TRACK:
			tuneStart = _mBank2SegmentRomStart;
			tuneEnd = _mBank2SegmentRomEnd;
			break;
		case ATLANTIS2_TRACK:
			tuneStart = _mBank3SegmentRomStart;
			tuneEnd = _mBank3SegmentRomEnd;
			break;
		case ATLANTIS3_TRACK:
			tuneStart = _mBank4SegmentRomStart;
			tuneEnd = _mBank4SegmentRomEnd;
			break;
		case ATLANTISBOSS_TRACK:
			tuneStart = _mBank5SegmentRomStart;
			tuneEnd = _mBank5SegmentRomEnd;
			break;
						 */
		case CARNIVAL_TRACK:
			tuneStart = _mBank1SegmentRomStart;
			tuneEnd = _mBank1SegmentRomEnd;
			break;
/*		case CARNIVAL2_TRACK:
			tuneStart = _mbank6SegmentRomStart;
			tuneEnd = _mbank6SegmentRomEnd;
			break;
		case CARNIVAL3_TRACK:
			tuneStart = _mbank7SegmentRomStart;
			tuneEnd = _mbank7SegmentRomEnd;
			break;
		case CARNIVALBOSS_TRACK:
			tuneStart = _mbank27SegmentRomStart;
			tuneEnd = _mbank27SegmentRomEnd;
			break;


		case PIRATES_TRACK:
			tuneStart = _mbank8SegmentRomStart;
			tuneEnd = _mbank8SegmentRomEnd;
			break;
		case PIRATES2_TRACK:
			tuneStart = _mbank9SegmentRomStart;
			tuneEnd = _mbank9SegmentRomEnd;
			break;
		case PIRATES3_TRACK:
			tuneStart = _mbank26SegmentRomStart;
			tuneEnd = _mbank26SegmentRomEnd;
			break;
		case PIRATESBOSS_TRACK:
			tuneStart = _mbank10SegmentRomStart;
			tuneEnd = _mbank10SegmentRomEnd;
			break;

		case PREHISTORIC_TRACK:
			tuneStart = _mbank11SegmentRomStart;
			tuneEnd = _mbank11SegmentRomEnd;
			break;
		case PREHISTORIC2_TRACK:
			tuneStart = _mbank12SegmentRomStart;
			tuneEnd = _mbank12SegmentRomEnd;
			break;
		case PREHISTORIC3_TRACK:
			tuneStart = _mbank13SegmentRomStart;
			tuneEnd = _mbank13SegmentRomEnd;
			break;
		case PREHISTORICBOSS_TRACK:
			tuneStart = _mbank14SegmentRomStart;
			tuneEnd = _mbank14SegmentRomEnd;
			break;

		case FORTRESS_TRACK:
			tuneStart = _mbank15SegmentRomStart;
			tuneEnd = _mbank15SegmentRomEnd;
			break;
		case FORTRESS2_TRACK:
			tuneStart = _mbank16SegmentRomStart;
			tuneEnd = _mbank16SegmentRomEnd;
			break;
		case FORTRESS3_TRACK:
			tuneStart = _mbank17SegmentRomStart;
			tuneEnd = _mbank17SegmentRomEnd;
			break;
		case FORTRESSBOSS_TRACK:
			tuneStart = _mbank18SegmentRomStart;
			tuneEnd = _mbank18SegmentRomEnd;
			break;

		case OOTW_TRACK:
			tuneStart = _mbank19SegmentRomStart;
			tuneEnd = _mbank19SegmentRomEnd;
			break;
		case OOTW2_TRACK:
			tuneStart = _mbank20SegmentRomStart;
			tuneEnd = _mbank20SegmentRomEnd;
			break;
		case OOTW3_TRACK:
			tuneStart = _mbank21SegmentRomStart;
			tuneEnd = _mbank21SegmentRomEnd;
			break;
		case OOTWBOSS_TRACK:
			tuneStart = _mbank22SegmentRomStart;
			tuneEnd = _mbank22SegmentRomEnd;
			break;

		case TITLE_TRACK:
			tuneStart = _mbank34SegmentRomStart;
			tuneEnd = _mbank34SegmentRomEnd;
//			tuneStart = _mbank19SegmentRomStart;
//			tuneEnd = _mbank34SegmentRomEnd;
			break;
		case INTRO_TRACK:
			tuneStart = _mbank24SegmentRomStart;
			tuneEnd = _mbank24SegmentRomEnd;
			break;
		case OUTRO_TRACK:
			tuneStart = _mbank25SegmentRomStart;
			tuneEnd = _mbank25SegmentRomEnd;
			break;

		case PIRATES_KILL1_TRACK:
			tuneStart = _mbank47SegmentRomStart;
			tuneEnd = _mbank47SegmentRomEnd;
			break;
		case PIRATES_KILL2_TRACK:
			tuneStart = _mbank48SegmentRomStart;
			tuneEnd = _mbank48SegmentRomEnd;
			break;
		case PIRATES_WIN1_TRACK:
			tuneStart = _mbank49SegmentRomStart;
			tuneEnd = _mbank49SegmentRomEnd;
			break;
		case PIRATES_WIN2_TRACK:
			tuneStart = _mbank50SegmentRomStart;
			tuneEnd = _mbank50SegmentRomEnd;
			break;

		case ATLANTIS_KILL1_TRACK:
			tuneStart = _mbank51SegmentRomStart;
			tuneEnd = _mbank51SegmentRomEnd;
			break;
		case ATLANTIS_KILL2_TRACK:
			tuneStart = _mbank52SegmentRomStart;
			tuneEnd = _mbank52SegmentRomEnd;
			break;
		case ATLANTIS_WIN1_TRACK:
			tuneStart = _mbank53SegmentRomStart;
			tuneEnd = _mbank53SegmentRomEnd;
			break;
		case ATLANTIS_WIN2_TRACK:
			tuneStart = _mbank54SegmentRomStart;
			tuneEnd = _mbank54SegmentRomEnd;
			break;

		case CARNIVAL_KILL1_TRACK:
			tuneStart = _mbank55SegmentRomStart;
			tuneEnd = _mbank55SegmentRomEnd;
			break;
		case CARNIVAL_KILL2_TRACK:
			tuneStart = _mbank56SegmentRomStart;
			tuneEnd = _mbank56SegmentRomEnd;
			break;
		case CARNIVAL_WIN1_TRACK:
			tuneStart = _mbank57SegmentRomStart;
			tuneEnd = _mbank57SegmentRomEnd;
			break;
		case CARNIVAL_WIN2_TRACK:
			tuneStart = _mbank58SegmentRomStart;
			tuneEnd = _mbank58SegmentRomEnd;
			break;

		case PREHISTORIC_KILL1_TRACK:
			tuneStart = _mbank35SegmentRomStart;
			tuneEnd = _mbank35SegmentRomEnd;
			break;
		case PREHISTORIC_KILL2_TRACK:
			tuneStart = _mbank36SegmentRomStart;
			tuneEnd = _mbank36SegmentRomEnd;
			break;
		case PREHISTORIC_WIN1_TRACK:
			tuneStart = _mbank37SegmentRomStart;
			tuneEnd = _mbank37SegmentRomEnd;
			break;
		case PREHISTORIC_WIN2_TRACK:
			tuneStart = _mbank38SegmentRomStart;
			tuneEnd = _mbank38SegmentRomEnd;
			break;

		case FORTRESS_KILL1_TRACK:
			tuneStart = _mbank43SegmentRomStart;
			tuneEnd = _mbank43SegmentRomEnd;
			break;
		case FORTRESS_KILL2_TRACK:
			tuneStart = _mbank44SegmentRomStart;
			tuneEnd = _mbank44SegmentRomEnd;
			break;
		case FORTRESS_WIN1_TRACK:
			tuneStart = _mbank45SegmentRomStart;
			tuneEnd = _mbank45SegmentRomEnd;
			break;
		case FORTRESS_WIN2_TRACK:
			tuneStart = _mbank46SegmentRomStart;
			tuneEnd = _mbank46SegmentRomEnd;
			break;

		case OOTW_KILL1_TRACK:
			tuneStart = _mbank39SegmentRomStart;
			tuneEnd = _mbank39SegmentRomEnd;
			break;
		case OOTW_KILL2_TRACK:
			tuneStart = _mbank40SegmentRomStart;
			tuneEnd = _mbank40SegmentRomEnd;
			break;
		case OOTW_WIN1_TRACK:
			tuneStart = _mbank41SegmentRomStart;
			tuneEnd = _mbank41SegmentRomEnd;
			break;
		case OOTW_WIN2_TRACK:
			tuneStart = _mbank42SegmentRomStart;
			tuneEnd = _mbank42SegmentRomEnd;
			break;

		case BONUS_TRACK:
			tuneStart = _mbank59SegmentRomStart;
			tuneEnd = _mbank59SegmentRomEnd;
			break;
		case HASBRO_TRACK:
			tuneStart = _mbank60SegmentRomStart;
			tuneEnd = _mbank60SegmentRomEnd;
			break;*/
/*		case TITLE2_TRACK:
			tuneStart	= _mBank1SegmentRomStart;
			tuneEnd		= _mBank1SegmentRomEnd;
			break;
  */



		default:
			dprintf"Error, no song specified\n"));
			return;
			break;
	}

	audioCtrl.musicBin = audioCtrl.sfxPtr + (PBANK2_END - PBANK2_START);

	DMAMemory(audioCtrl.musicBin, (u32)tuneStart, (u32)tuneEnd);

	audioCtrl.musicHandle = MusBankStartSong(audioCtrl.musicPtr, audioCtrl.musicBin);
	while(!(MusHandleAsk(audioCtrl.musicHandle)));
//	audioCtrl.musicHandle = MusStartSong(audioCtrl.musicBin);

	MusHandleSetVolume(audioCtrl.musicHandle, 255);//0x20);

//	if((num == INTRO_TRACK) && (osTvType == 0))
//	if((num == INTRO_TRACK) && (osTvType == 0))
//	{
		//MusHandleSetTempo(audioCtrl.musicHandle, 0x6a);
//	}

}


/*	--------------------------------------------------------------------------------
	Function 	: PlaySample
	Purpose 	: Play a sound sample at a given point with no distance attenuation
	Parameters 	: Index of sample, position, volume, pitch
	Returns 	: 
	Info 		:
*/

int PlaySample(short num, VECTOR *pos, short tempVol,short pitch)
{
	float	dist, scale, vol = tempVol;
	VECTOR	tempVect;
//	MusHandleStop(musresult, 0);

//work out volume
//	dist = DistanceBetweenPointsSquared(&camera.pos, pos);
//	if(dist > MAX_SFX_DIST*MAX_SFX_DIST)
//		return 0;
	
//	dist = sqrtf(dist);
//	scale = MAX_SFX_DIST - dist;
//	scale = scale / MAX_SFX_DIST;

//	vol *= scale;

//work out pan
//	SubVector2D(&tempVect, pos, &camera.pos);
//	dist = Aabs(atan2(tempVect.v[X], tempVect.v[Z]));
//	scale = FindShortestAngle(Aabs(camera.rot.v[Y]+PI/2),dist);

//	scale *= 255/PI;


/*	if(num != FX_NULL)
	{

		vol *= 1.8;
		if(vol > 255)
			vol = 255;

		

//		result = MusBankStartEffect2(audioCtrl.sfxPtr, num, (u8)vol, scale, 0, -1);
*/
		vol *=2;
		if(vol > 255)
			vol = 255;
//vol = 255;
		scale = 255;

		musresult = MusStartEffect2(num, (u8)vol, scale, 0, -1);
		MusHandleSetFreqOffset(musresult,(float)(pitch-128)/10.0);
		MusHandleSetTempo(musresult,pitch);
/*	}
	else	result = 0;
  */
//	if((reverbOn) || (camera.stats->inWater))
//		MusHandleSetReverb(result, 30);
//	return result;

}


/*	--------------------------------------------------------------------------------
	Function 	: PlaySampleRadius
	Purpose 	: Play a sample at a given point using spherical volume attenuation
	Parameters 	: Index of sample, position, volume, pitch, attenuation radius
	Returns 	: 
	Info 		:
*/

int PlaySampleRadius(short num, VECTOR *pos, short tempVol,short pitch,float radius)
{
	int result;
	float	dist, scale, vol = tempVol;
	VECTOR	tempVect;

//work out volume
	dist = Max(0,DistanceBetweenPoints(&frog[0]->actor->pos, pos) - radius);
	if(dist > MAX_SFX_DIST)
		return 0;

	scale = MAX_SFX_DIST - dist;
	scale = scale / MAX_SFX_DIST;

	vol *= scale;

//work out pan
	SubVector2D(&tempVect, pos, &frog[0]->actor->pos);
	dist = Aabs(atan2(tempVect.v[X], tempVect.v[Z]));
	scale = FindShortestAngle(Aabs(frog[0]->actor->rot.v[Y]+PI/2),dist);

	scale *= 255/PI;


	if(num != FX_NULL)
	{
		//vol *= 1.8;
		if(vol > 255)
			vol = 255;

//		result = MusBankStartEffect2(audioCtrl.sfxPtr, num, vol, scale, 0, -1);
		result = MusStartEffect2(num, vol, scale, 0, -1);
		MusHandleSetFreqOffset(result,(float)(pitch-128)/10.0);
		MusHandleSetTempo(result,pitch);
	}
	else	result = 0;
/*
	if((reverbOn) || (camera.stats->inWater))
		MusHandleSetReverb(result, 30);
*/

	return result;
}


/*	--------------------------------------------------------------------------------
	Function 	: PlayContinuousSample
	Purpose 	: Loop sound effect???
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void PlayContinuousSample(SFX *sfx,short num,short vol,VECTOR *pos,short pitch)
{
	if(num == 0)
		return;
	if(sfx->handle)
		StopContinuousSample(sfx);		
	sfx->handle = PlaySampleRadius(num, pos, vol,pitch,sfx->radius);
	vol = (float)(vol)*1.8;
	if(vol > 255)
		vol = 255;

	sfx->volume = vol;
	sfx->pos = pos;
	sfx->sampleNum = num;
	sfx->pitch = pitch;

	if(sfx->handle)
		numContinuousSamples++;
}


/*	--------------------------------------------------------------------------------
	Function 	: StopContinuousSample
	Purpose 	: Interrupt a looping sample
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void StopContinuousSample(SFX *sfx)
{
	if(sfx->handle)
	{
		MusHandleStop(sfx->handle, 0);
		numContinuousSamples--;
	}
	sfx->actualVolume = sfx->volume;
	sfx->volume = 0;
	sfx->handle = 0;
  
}


/*	--------------------------------------------------------------------------------
	Function 	: UpdateContinuousSample
	Purpose 	: Recalculates distance attenuation for a continuous sample
	Parameters 	: Sound effect structure
	Returns 	: 
	Info 		: See also PlaySampleRadius
*/
void UpdateContinuousSample(SFX *sfx)
{
	float	dist, scale;
	VECTOR	tempVect;

	//if sfx is not playing return
	if((sfx->sampleNum == 0) || ((sfx->handle == 0) && (sfx->volume == 0)))
		return;

	if(sfx->radius)
	{
		dist = Max(0,DistanceBetweenPoints(&frog[0]->actor->pos, sfx->pos) - sfx->radius);
		if(dist > MAX_SFX_DIST)
		{
			if(sfx->handle)
			{
				MusHandleStop(sfx->handle, 0);
				numContinuousSamples--;
				sfx->handle = 0;
			}
			return;
		}
	}
	else
	{
		dist = DistanceBetweenPointsSquared(&frog[0]->actor->pos, sfx->pos);
		if(dist > MAX_SFX_DIST*MAX_SFX_DIST)
		{
			if(sfx->handle)
			{
				MusHandleStop(sfx->handle, 0);
				numContinuousSamples--;
				sfx->handle = 0;
			}
			return;
		}
		dist = sqrtf(dist);
	}

	scale = MAX_SFX_DIST - dist;
	scale = scale / MAX_SFX_DIST;

	sfx->actualVolume = sfx->volume * scale;

//	if(enemy->sfx[num].actualVolume == 0)
//	{
//		MusHandleStop(enemy->sfx[num].handle, 0);
//		enemy->sfx[num].handle = 0;
//		return;
//	}


//work out pan
	SubVector2D(&tempVect, sfx->pos, &frog[0]->actor->pos);
	dist = Aabs(atan2(tempVect.v[X], tempVect.v[Z]));
	scale = FindShortestAngle(Aabs(frog[0]->actor->rot.v[Y]+PI/2),dist);

	scale *= 255/PI;

	if(sfx->handle == 0)
	{
//		sfx->handle = MusBankStartEffect2(audioCtrl.sfxPtr, sfx->sampleNum, sfx->actualVolume, scale, 0, -1);
		sfx->handle = MusStartEffect2(sfx->sampleNum, sfx->actualVolume, scale, 0, -1);

/*
		if((reverbOn) || (camera.stats->inWater))
			MusHandleSetReverb(sfx->handle, 30);
*/
		numContinuousSamples++;
	}
	else
		MusHandleSetVolume(sfx->handle, sfx->actualVolume);

	MusHandleSetPan(sfx->handle, scale);
	MusHandleSetFreqOffset(sfx->handle, (float)(sfx->pitch-128)/10.0);
	MusHandleSetTempo(sfx->handle, sfx->pitch);
}


/*	--------------------------------------------------------------------------------
	Function 	: AddAmbientSfx
	Purpose 	: Put a new sourceless ambient effect into the list
	Parameters 	: Sample index, volume, pan
	Returns 	: 
	Info 		: eg. AddAmbientSfx( FX_RAIN_HEAVY, 255, 0 )
*/
void AddAmbientSfx(int num, int vol, int pan)
{
	AMBIENT_SOUND *ptr = ambientSoundList.head.next;
	AMBIENT_SOUND *ambientSound = (AMBIENT_SOUND *)JallocAlloc(sizeof(AMBIENT_SOUND),YES,"AmbSnd");
	
	ambientSound->prev = ptr;
	ambientSound->next = ptr->next;
	ptr->next->prev = ambientSound;
	ptr->next = ambientSound;
	ambientSoundList.numEntries++;


	vol = (float)(vol)*1.8;
	if(vol > 255)
		vol = 255;

//	ambientSound->sfx.handle = MusBankStartEffect2(audioCtrl.sfxPtr, num, vol, pan, 0, -1);
	ambientSound->sfx.handle = MusStartEffect2(num, vol, pan, 0, -1);
	numContinuousSamples++;
	ambientSound->sfx.pos = &zero;
	ambientSound->sfx.origVolume = vol;
	ambientSound->sfx.actualVolume = vol;
	ambientSound->sfx.sampleNum = num;
}


/*	--------------------------------------------------------------------------------
	Function 	: AddAmbientSfxAtPoint
	Purpose 	: As above, but has a definite source
	Parameters 	: Index, volume, positon, pitch, repeat frequency, random offset frequency, repeat timeperiod,
					platform to attach to(NOT IMPLEMENTED), tag???, radius of attenuation
	Returns 	: 
	Info 		:
*/
void AddAmbientSfxAtPoint(int num, int vol,VECTOR *pos,short pitch,short freq,short randFreq,short onTime,short platTag,short tag,float radius)
{
	AMBIENT_SOUND *ptr = ambientSoundList.head.next;
	AMBIENT_SOUND *ambientSound = (AMBIENT_SOUND *)JallocAlloc(sizeof(AMBIENT_SOUND),YES,"AmbSnd");
	
	ambientSound->prev = ptr;
	ambientSound->next = ptr->next;
	ptr->next->prev = ambientSound;
	ptr->next = ambientSound;
	ambientSoundList.numEntries++;
/*
	if(platTag)
	{
		SetVector(&ambientSound->offsetPos,pos);
		ambientSound->platform = TagToFirstPlatform(platTag);
	}
	else
	{*/
		SetVector(&ambientSound->pos,pos);
		ambientSound->platform = NULL;
	//}
	ambientSound->sfx.volume = vol;
	ambientSound->sfx.sampleNum = num;
	ambientSound->sfx.pitch = pitch;
//	if(onTime)
	if((onTime) || ((freq == 0) && (randFreq == 0)))
	{
		PlayContinuousSample(&ambientSound->sfx,num,vol,&ambientSound->pos,pitch);
		if((ambientSound->sfx.handle) && (ambientSound->sfx.sampleNum == FX_KLOSET_CALL))
			MusHandleSetReverb(ambientSound->sfx.handle, 40);
		ambientSound->counter = onTime;
	}
	else
	{
		ambientSound->counter = freq + Random(randFreq);
		ambientSound->sfx.pos = &ambientSound->pos;
	}

	ambientSound->sfx.radius = radius;

	ambientSound->origVol = vol;
	ambientSound->freq = freq;
	ambientSound->randFreq = randFreq;
	ambientSound->onTime = onTime;
	ambientSound->tag = tag;
}


/*	--------------------------------------------------------------------------------
	Function 	: ClearAmbientSfx
	Purpose 	: Stop all ambients
	Parameters 	: 
	Returns 	: 
	Info 		: Does not actually delete anything, just stops them
*/
void ClearAmbientSfx()
{
	AMBIENT_SOUND *ambientSound;

	for(ambientSound = ambientSoundList.head.next;ambientSound != &ambientSoundList.head;ambientSound = ambientSound->next)
	{
		if(ambientSound->sfx.handle)
			StopContinuousSample(&ambientSound->sfx);

		ambientSound->sfx.pos = &zero;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: KillAmbientSfx
	Purpose 	: Deletes all ambient sounds from the list, after stopping them
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void KillAmbientSfx()
{
	AMBIENT_SOUND *ambientSound, *tmp;

	for(ambientSound = ambientSoundList.head.next;ambientSound != &ambientSoundList.head; )
	{
		if( ambientSound != NULL )
		{
			if(ambientSound->sfx.handle)
				StopContinuousSample(&ambientSound->sfx);

			ambientSound->sfx.pos = &zero;
			SubAmbientSound( ambientSound );

			tmp = ambientSound->next;
			JallocFree( (UBYTE **)&ambientSound );
			ambientSound = tmp;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: UpdateAmbientSounds
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void UpdateAmbientSounds()
{
	AMBIENT_SOUND *ambientSound,*ambientSound2;
	float pitch;

	// Silence ambients if paused or level over?
	if((gameState.mode == PAUSE_MODE) || levelIsOver )
	{
		for(ambientSound = ambientSoundList.head.next;ambientSound != &ambientSoundList.head;ambientSound = ambientSound->next)
		{
			if(ambientSound->sfx.handle)
				MusHandleSetVolume(ambientSound->sfx.handle, (ambientSound->sfx.actualVolume*sfxVol)/32768);
		}
		return;
	}

	// Update each ambient in turn
	for(ambientSound = ambientSoundList.head.next;ambientSound != &ambientSoundList.head;ambientSound = ambientSound2)
	{
		ambientSound2 = ambientSound->next;

		// If sound doesn't have a source
		if(ambientSound->sfx.pos == &zero)
		{
			if(ambientSound->sfx.sampleNum == FX_EERIE_WIND)
			{
				// Oscillating tone for wind
//				pitch = 128 + SineWave(50,frameCount,0)*30;
				MusHandleSetFreqOffset(ambientSound->sfx.handle,(float)(pitch-128)/10.0);
				MusHandleSetTempo(ambientSound->sfx.handle,pitch);

//				pitch = (SineWave(60,frameCount,0) + 1)/4 + 0.5;
				pitch *= ambientSound->sfx.origVolume;
				MusHandleSetVolume(ambientSound->sfx.handle,pitch);
				ambientSound->sfx.actualVolume = pitch;
			}
			else if(ambientSound->sfx.handle)
				MusHandleSetVolume(ambientSound->sfx.handle, (ambientSound->sfx.actualVolume*sfxVol)/32768);
		}
		else // If ambient is sourced
		{
			// If it is attached to a platform, make ambient follow that platform
			if(ambientSound->platform)
			{
				RotateVectorByQuaternion(&ambientSound->pos,&ambientSound->offsetPos,&ambientSound->platform->pltActor->actor->qRot);
				AddToVector(&ambientSound->pos,&ambientSound->platform->pltActor->actor->pos);
			}
			if(ambientSound->sfx.volume)
				UpdateContinuousSample(&ambientSound->sfx);	

			// If ambient is on a timed loop, play on timeout
			if(ambientSound->onTime)
			{
				if(ambientSound->counter)
					ambientSound->counter--;
				if(ambientSound->counter == 0)
				{
					if(ambientSound->sfx.handle)
					{	
						if(ambientSound->sfx.sampleNum == FX_KLOSET_CALL)
							ambientSound->sfx.volume -= 2;
						else
							ambientSound->sfx.volume -= 20;
						if(ambientSound->sfx.volume <= 0)
						{
							StopContinuousSample(&ambientSound->sfx);
							if((ambientSound->freq) || (ambientSound->randFreq))
								ambientSound->counter = ambientSound->freq + Random(ambientSound->randFreq);
							else
							{
								SubAmbientSound(ambientSound);
								JallocFree((UBYTE **)&ambientSound);
								continue;
							}
						}
					}
					else
					{
						PlayContinuousSample(&ambientSound->sfx,ambientSound->sfx.sampleNum,ambientSound->origVol,&ambientSound->pos,ambientSound->sfx.pitch);
						ambientSound->counter = ambientSound->onTime;
						if((ambientSound->sfx.handle) && (ambientSound->sfx.sampleNum == FX_KLOSET_CALL))
							MusHandleSetReverb(ambientSound->sfx.handle, 40);
					}
				}
			}
			else if(ambientSound->freq) // Else if it plays after a randomly modified time
			{
				if(--ambientSound->counter == 0)
				{
					// If attenuated
					if(ambientSound->sfx.radius)
						PlaySampleRadius(ambientSound->sfx.sampleNum,&ambientSound->pos,ambientSound->sfx.volume,ambientSound->sfx.pitch,ambientSound->sfx.radius);
					else
						PlaySample(ambientSound->sfx.sampleNum,&ambientSound->pos,ambientSound->sfx.volume,ambientSound->sfx.pitch);
					ambientSound->counter = ambientSound->freq + Random(ambientSound->randFreq);
				}
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PlaySampleNot3D
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int PlaySampleNot3D(int num,UBYTE vol,UBYTE pan,UBYTE pitch)
{
	int handle;

//	handle = MusBankStartEffect2(audioCtrl.sfxPtr, num,vol,pan, 0, -1);
	handle = MusStartEffect2(num,vol,pan, 0, -1);
	MusHandleSetFreqOffset(handle,(float)(pitch-128)/10.0);
	MusHandleSetTempo(handle,pitch);
	return handle;
}


/*	--------------------------------------------------------------------------------
	Function 	: InitSFXScript
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
/*void InitSFXScript(SFX_SCRIPT *script)
{
	currentScriptEntry = 0;
	activeSFXScript = script;
}
*/

/*	--------------------------------------------------------------------------------
	Function 	: ScriptTriggerSound
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
/*BOOL ScriptTriggerSound(short second, short frame)
{
	float temp;
	short targetFrame;

	if(activeSFXScript == &introSfxScript[0])
	{
		if(second+4 == frameCount)
			return TRUE;
	}
	else
	{
		targetFrame = second * 20;
		temp = (float)frame * 0.8;
		targetFrame += temp;

		if(frameCount == targetFrame)
			return TRUE;
	}
	return FALSE;
}
*/

/*	--------------------------------------------------------------------------------
	Function 	: ProcessSFXScript
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
/*void ProcessSFXScript()
{
	SFX_SCRIPT *script = activeSFXScript;
	char reverb;

	if(script == NULL)
		return;

	while(script->triggerSecond >= 0)
	{
 		if((script->stopSecond != -1) || levelIsOver )
		{
			if( levelIsOver || (ScriptTriggerSound(script->stopSecond, script->stopFrame)))
			{
				MusHandleStop(script->handle, 0);
			}		
		}
		script++;
	}

	if( levelIsOver ) return;

	script = activeSFXScript;

	while(script->triggerSecond >= 0)
	{
		if(ScriptTriggerSound(script->triggerSecond, script->triggerFrame))
		{
			reverb = script->handle;
			script->handle = PlaySampleNot3D(script->sfxNum, script->vol, 128,script->pan ? script->pan : 128);		
			if(script->sfxNum == FX_KLOSET_CALL)
				MusHandleSetReverb(script->handle, 60);
				
			currentScriptEntry++;
			if(script->triggerSecond == -1)
			{
				activeSFXScript = NULL;
				return;
			}
 		}
		script++;
	}	
}
*/

