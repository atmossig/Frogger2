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

TUNE_DATA_BANK gameSongs[] = 
{
	0,0,"no track",												//	NOTRACK			0 
	_mBank1SegmentRomStart,_mBank1SegmentRomEnd,"atlantis 1",	//	TEST1_TRACK		1
	_mBank2SegmentRomStart,_mBank2SegmentRomEnd,"atlantis 2",	//	TEST2_TRACK		2
	_mBank3SegmentRomStart,_mBank3SegmentRomEnd,"atlantis 3",	//	TEST3_TRACK		3
	_mBank4SegmentRomStart,_mBank4SegmentRomEnd,"carnival 1",	//	TEST4_TRACK		4
	_mBank5SegmentRomStart,_mBank5SegmentRomEnd,"carnival 2",	//	TEST5_TRACK		5
	_mBank6SegmentRomStart,_mBank6SegmentRomEnd,"carnival 3",	//	TEST6_TRACK		6
	_mBank7SegmentRomStart,_mBank7SegmentRomEnd,"fear 1",		//	TEST7_TRACK		7
	_mBank8SegmentRomStart,_mBank8SegmentRomEnd,"fear 2",		//	TEST8_TRACK		8
	_mBank9SegmentRomStart,_mBank9SegmentRomEnd,"fear 3",		//	TEST9_TRACK		9
	_mBank10SegmentRomStart,_mBank10SegmentRomEnd,"pirate 1",	//	TEST10_TRACK	10
	_mBank11SegmentRomStart,_mBank11SegmentRomEnd,"pirate 2",	//	TEST11_TRACK	11
	_mBank12SegmentRomStart,_mBank12SegmentRomEnd,"pirate 3",	//	TEST12_TRACK	12
	_mBank13SegmentRomStart,_mBank13SegmentRomEnd,"prehist 1",	//	TEST13_TRACK	13
	_mBank14SegmentRomStart,_mBank14SegmentRomEnd,"prehist 2",	//	TEST14_TRACK	14
	_mBank15SegmentRomStart,_mBank15SegmentRomEnd,"prehist 3",	//	TEST15_TRACK	15
	_mBank16SegmentRomStart,_mBank16SegmentRomEnd,"space 1",	//	TEST16_TRACK	16
	_mBank17SegmentRomStart,_mBank17SegmentRomEnd,"space 2",	//	TEST17_TRACK	17
	_mBank18SegmentRomStart,_mBank18SegmentRomEnd,"space 3",	//	TEST18_TRACK	18

	_mBank19SegmentRomStart,_mBank19SegmentRomEnd,"title",		//	TEST19_TRACK	19
	_mBank20SegmentRomStart,_mBank20SegmentRomEnd,"hasbro"		//	TEST20_TRACK	20
};


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

void PrepareSong(char num,char slot)
{
	MusHandleSetReverb(audioCtrl.musicHandle[slot],0);
	MusHandleSetVolume(audioCtrl.musicHandle[slot],80);

	//stop existing audio track	
	if(num == audioCtrl.currentTrack[slot])
		return;

	audioCtrl.currentTrack[slot] = num;

	MusHandleStop(audioCtrl.musicHandle[slot], 1);
	while(MusHandleAsk(audioCtrl.musicHandle[slot]));

	if(num == NOTRACK)
		return;

	audioCtrl.musicBin[0] = audioCtrl.sfxPtr + (PBANK2_END - PBANK2_START);
	audioCtrl.musicBin[1] = audioCtrl.musicBin[0] + 20000;//audioCtrl.sfxPtr + (PBANK2_END - PBANK2_START);

	DMAMemory(audioCtrl.musicBin[slot], (u32)gameSongs[num].bankStart, (u32)gameSongs[num].bankEnd);

	audioCtrl.musicHandle[slot] = MusBankStartSong(audioCtrl.musicPtr, audioCtrl.musicBin[slot]);
	while(!(MusHandleAsk(audioCtrl.musicHandle[slot])));

	MusHandleSetVolume(audioCtrl.musicHandle[slot],80);

	if((num == INTRO_TRACK) && (osTvType == 0))
	{
		MusHandleSetTempo(audioCtrl.musicHandle[slot], 0x6a);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PlaySample
	Purpose 	: Play a sound sample at a given point with no distance attenuation
	Parameters 	: Index of sample, position, volume, pitch
	Returns 	: 
	Info 		:
*/
int PlaySample(short num,VECTOR *pos,short tempVol,short pitch)
{
	int result;
	float	dist, scale = 1, vol = tempVol;
	VECTOR	tempVect;
//	MusHandleStop(musresult, 0);

	if(!pos)
		return 0;
	
//work out volume
	dist = DistanceBetweenPointsSquared(&currCamSource[0],pos);
	if(dist > MAX_SFX_DIST*MAX_SFX_DIST)
		return 0;
	
	dist = sqrtf(dist);
	scale = MAX_SFX_DIST - dist;
	scale = scale / MAX_SFX_DIST;

	vol *= scale;

//work out pan
	SubVector2D(&tempVect, pos, &currCamSource[0]);
	dist = Aabs(atan2(tempVect.v[X], tempVect.v[Z]));
	scale = FindShortestAngle(Aabs(frog[0]->actor->rot.v[Y]+PI/2),dist);

	scale *= 255/PI;

	if(num != FX_NULL)
	{

		vol *= 1.8;
		if(vol > 255)
			vol = 255;

		result = MusStartEffect2(num, (u8)vol, scale, 0, -1);
		MusHandleSetFreqOffset(result,(float)(pitch-128)/10.0);
		MusHandleSetTempo(result,pitch);
	}
	else	result = 0;
  
	if(reverbOn)
		MusHandleSetReverb(result,30);
	return result;
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
	float	dist, scale = 1, vol = tempVol;
	VECTOR	tempVect;

//work out volume
/*
	dist = Max(0,DistanceBetweenPoints(&currCamSource[0], pos) - radius);
	if(dist > MAX_SFX_DIST)
		return 0;

	scale = MAX_SFX_DIST - dist;
	scale = scale / MAX_SFX_DIST;

	vol *= scale;
*/
//work out pan
//	SubVector2D(&tempVect, pos, &currCamSource[0]);
//	dist = Aabs(atan2(tempVect.v[X], tempVect.v[Z]));
//	scale = FindShortestAngle(Aabs(frog[0]->actor->rot.v[Y]+PI/2),dist);

//	scale *= 255/PI;


	if(num != FX_NULL)
	{
		vol *= 1.8;
		if(vol > 255)
			vol = 255;

		result = MusStartEffect2(num, vol, scale, 0, -1);
		MusHandleSetFreqOffset(result,(float)(pitch-128)/10.0);
		MusHandleSetTempo(result,pitch);
	}
	else	result = 0;

	if(reverbOn)
		MusHandleSetReverb(result, 30);

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
		dist = Max(0,DistanceBetweenPoints(&currCamSource[0], sfx->pos) - sfx->radius);
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
		dist = DistanceBetweenPointsSquared(&currCamSource[0], sfx->pos);
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
//	SubVector2D(&tempVect, sfx->pos, &frog[0]->actor->pos);
//	dist = Aabs(atan2(tempVect.v[X], tempVect.v[Z]));
//	scale = FindShortestAngle(Aabs(frog[0]->actor->rot.v[Y]+PI/2),dist);

//	scale *= 255/PI;

	if(sfx->handle == 0)
	{
		sfx->handle = MusStartEffect2(sfx->sampleNum, sfx->actualVolume, scale, 0, -1);
		if(reverbOn)
			MusHandleSetReverb(sfx->handle, 30);
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


/*	--------------------------------------------------------------------------------
	Function		: PlayActorBasedSample
	Purpose			: play a sample based on actor rotation, rather than camera
	Parameters		: short,ACTOR *,short,short
	Returns			: int
	Info			: 
*/
int PlayActorBasedSample(short num,ACTOR *act,short tempVol,short pitch)
{
	int result;
	float	dist, scale = 1, vol = tempVol;
	VECTOR	tempVect;

//work out volume
	dist = DistanceBetweenPointsSquared(&currCamSource[0],&act->pos);
	if(dist > MAX_SFX_DIST*MAX_SFX_DIST)
		return 0;
	
	dist = sqrtf(dist);
	scale = MAX_SFX_DIST - dist;
	scale = scale / MAX_SFX_DIST;

	vol *= scale;

//work out pan
	SubVector2D(&tempVect, &act->pos, &currCamSource[0]);
	dist = Aabs(atan2(tempVect.v[X], tempVect.v[Z]));
	scale = FindShortestAngle(Aabs(act->rot.v[Y]+PI/2),dist);

	scale *= 255/PI;

	if(num != FX_NULL)
	{

		vol *= 1.8;
		if(vol > 255)
			vol = 255;

		result = MusStartEffect2(num, (u8)vol, scale, 0, -1);
		MusHandleSetFreqOffset(result,(float)(pitch-128)/10.0);
		MusHandleSetTempo(result,pitch);
	}
	else	result = 0;
  
	if(reverbOn)
		MusHandleSetReverb(result,30);
	return result;
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
			case LEVELID_GARDENLEV1:
				theToon = TEST1_TRACK;
				break;

			case LEVELID_GARDENLEV2:
				theToon = TEST2_TRACK;
				break;

			case LEVELID_GARDENLEV3:
				theToon = TEST3_TRACK;
				break;

			default:
				theToon = NOTRACK;
				break;
		}
	}
	else if(worldID == WORLDID_ANCIENT)
	{
		switch(levelID)
		{
			case LEVELID_ANCIENTLEV1:
				theToon = TEST13_TRACK;
				break;

			case LEVELID_ANCIENTLEV2:
				theToon = TEST14_TRACK;
				break;

			case LEVELID_ANCIENTLEV3:
				theToon = TEST15_TRACK;
				break;

			default:
				theToon = NOTRACK;
				break;
		}
	}
	else if(worldID == WORLDID_SPACE)
	{
		switch(levelID)
		{
			case LEVELID_SPACELEV1:
				theToon = TEST16_TRACK;
				break;

			case LEVELID_SPACELEV2:
				theToon = TEST17_TRACK;
				break;

			case LEVELID_SPACELEV3:
				theToon = TEST18_TRACK;
				break;

			default:
				theToon = NOTRACK;
				break;
		}
	}
	else if(worldID == WORLDID_CITY)
	{
		switch(levelID)
		{
			default:
				theToon = NOTRACK;
				break;
		}
	}
	else if(worldID == WORLDID_SUBTERRANEAN)
	{
		switch(levelID)
		{
			default:
				theToon = NOTRACK;
				break;
		}
	}
	else if(worldID == WORLDID_LABORATORY)
	{
		switch(levelID)
		{
			default:
				theToon = NOTRACK;
				break;
		}
	}
	else if(worldID == WORLDID_TOYSHOP)
	{
		switch(levelID)
		{
			default:
				theToon = NOTRACK;
				break;
		}
	}
	else if(worldID == WORLDID_HALLOWEEN)
	{
		switch(levelID)
		{
			default:
				theToon = NOTRACK;
				break;
		}
	}
	else if(worldID == WORLDID_SUPERRETRO)
	{
		switch(levelID)
		{
			case LEVELID_SUPERRETROLEV1:
				theToon = TEST12_TRACK;
				break;

			default:
				theToon = NOTRACK;
				break;
		}
	}
	else if(worldID == WORLDID_FRONTEND)
	{
		switch(levelID)
		{
			case LEVELID_FRONTEND1:
				theToon = TEST12_TRACK;
				break;

			case LEVELID_FRONTEND2:
				theToon = TEST19_TRACK;
				break;

			case LEVELID_FRONTEND3:
				theToon = TEST12_TRACK;
				break;

			case LEVELID_FRONTEND4:
				theToon = TEST12_TRACK;
				break;

			case LEVELID_FRONTEND5:
				theToon = TEST12_TRACK;
				break;

			default:
				theToon = NOTRACK;
				break;
		}
	}

	PrepareSong(theToon,0);
}

