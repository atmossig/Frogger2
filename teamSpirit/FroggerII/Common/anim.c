/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: anim.c
	Programmer	: Andy Eder
	Date		: 29/04/99 10:30:56

----------------------------------------------------------------------------------------------- */


#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


float hedRotAmt = 0;

// Used to cue a sound effect when an animation is played
long *sfx_anim_map = NULL;

int *FindSfxMapping( unsigned long uid );


/*	--------------------------------------------------------------------------------
	Function 	  : 
	Purpose 	   : 
	Parameters 	: 
	Returns 	   : 
	Info 		     :
*/
void InitActorAnim(ACTOR *tempActor)
{
	if( !tempActor->objectController )
		return;
	
	memcpy(tempActor->animation, tempActor->objectController->animation, sizeof(ACTOR_ANIMATION));	
	tempActor->animation->currentAnimation = -1;

	tempActor->animation->queueAnimation[0] = -1;
	tempActor->animation->queueAnimation[1] = -1;
	tempActor->animation->queueAnimation[2] = -1;
	tempActor->animation->queueAnimation[3] = -1;
	tempActor->animation->queueAnimation[4] = -1;
	tempActor->animation->loopAnimation = NO;
	tempActor->animation->numberQueued = 0;
	tempActor->animation->animTime = 0;
	tempActor->animation->reachedEndOfAnimation = FALSE;

	// Cue sounds from animations
	tempActor->animation->sfxMapping = FindSfxMapping( tempActor->objectController->objectID );



	// HAAAACCCKKK! REMOVE ASAP!
	if( tempActor->animation->sfxMapping && tempActor->animation->sfxMapping[0] == GAR_MOWER )
		AddAmbientSound(GAR_MOWER, &tempActor->pos, 500, 255, 30, 1.02, 0, tempActor );
}

/*	--------------------------------------------------------------------------------
	Function	: AnimateActor
	Purpose		: Runs an animation on an actor EXCEPT when it's already running
	Parameters	: actor, anim, loop?, queue?, speed, morph?, keep proportion?
	Returns		: void
*/
void StartAnimateActor(ACTOR *actor, int animNum, char loop, char queue, float speed, BYTE morphs,BOOL keepProportion)
{
	if (actor->animation->currentAnimation != animNum)
		AnimateActor(actor, animNum, loop, queue, speed, morphs, keepProportion);
}

/*	--------------------------------------------------------------------------------
	Function	: AnimateActor
	Purpose		: Runs an animation on a specific ACTOR
	Parameters	: actor, anim, loop?, queue?, speed, morph?, keep proportion?
	Returns		: void
*/
void AnimateActor(ACTOR *actor, int animNum, char loop, char queue, float speed, BYTE morphs,BOOL keepProportion)
{
	ACTOR_ANIMATION *actorAnim = actor->animation;
	animation *anim;
	float	actualSpeed,proportion;

	if(actorAnim == NULL)
		return;
	if(actorAnim->anims == NULL)
		return;

//if((actorAnim->currentAnimation == animNum) && (queue == NO) && (actorAnim->reachedEndOfAnimation == FALSE) && (((speed == 0) && (actorAnim->animationSpeed == actorAnim->anims[animNum].speed)) || (speed == actorAnim->animationSpeed)))
//	{
//		actorAnim->loopAnimation = loop;
//		return;
//	}

	if(animNum > actorAnim->numAnimations)
	{
//		dprintf"Trying to access illegal animation\n"));
		animNum = 0;
	}

	if(speed == 0)
		actualSpeed = actorAnim->anims[animNum].speed;
	else
		actualSpeed = speed;

	if(keepProportion)
	{
		anim = actorAnim->anims + actorAnim->currentAnimation;
		proportion = (float)(actorAnim->animTime - anim->animStart)/((float)anim->animEnd - anim->animStart);
	}

	if(queue == NO)
	{
		if(morphs)
		{
			actorAnim->currentAnimation = animNum;
			anim = actorAnim->anims + actorAnim->currentAnimation;
			actorAnim->loopAnimation = loop;
			actorAnim->animationSpeed = actualSpeed;
			actorAnim->reachedEndOfAnimation = FALSE;
			actorAnim->numMorphFrames = morphs;
			actorAnim->currentMorphFrame = 0;
			actorAnim->morphTo = anim->animStart;
			actorAnim->morphFrom = actorAnim->animTime;
			if(actualSpeed < 0)
				actorAnim->animTime = anim->animEnd;
			else
				actorAnim->animTime = anim->animStart;
			FlushQueue(actor);
		}
		else
		{
			actorAnim->currentAnimation = animNum;
			anim = actorAnim->anims + actorAnim->currentAnimation;
			actorAnim->loopAnimation = loop;
			actorAnim->animationSpeed = actualSpeed;
			actorAnim->reachedEndOfAnimation = FALSE;
			actorAnim->numMorphFrames = morphs;
			if(actualSpeed < 0)
				actorAnim->animTime = anim->animEnd;
			else
				actorAnim->animTime = anim->animStart;
			FlushQueue(actor);
		}
		if(keepProportion)
			actorAnim->morphTo = actorAnim->animTime = anim->animStart + proportion*(float)(anim->animEnd-anim->animStart);

		if( actorAnim->sfxMapping && actorAnim->sfxMapping[actorAnim->currentAnimation] != -1 )
			PlaySample( actorAnim->sfxMapping[actorAnim->currentAnimation], &actor->pos, 500, 255, 48 );
	}
	else
	{
		if(actorAnim->currentAnimation == -1)
		{
			if (morphs)
			{
				actorAnim->currentAnimation = animNum;
				actorAnim->loopAnimation = loop;
				actorAnim->animationSpeed = actualSpeed;
				actorAnim->reachedEndOfAnimation = FALSE;
				actorAnim->numMorphFrames = morphs;
				actorAnim->currentMorphFrame = 0;
				actorAnim->morphTo = anim->animStart;
				actorAnim->morphFrom = actorAnim->animTime;
			}
			else
			{
				actorAnim->currentAnimation = animNum;
				actorAnim->loopAnimation = loop;
				actorAnim->animationSpeed = actualSpeed;
				actorAnim->numMorphFrames = morphs;
			}
		}
		else
		{
			if(actorAnim->numberQueued == ANIM_QUEUE_LENGTH)
				return;
			actorAnim->queueAnimation[actorAnim->numberQueued] = animNum;
			actorAnim->queueLoopAnimation[actorAnim->numberQueued] = loop;
			actorAnim->queueAnimationSpeed[actorAnim->numberQueued] = actualSpeed;
			actorAnim->queueNumMorphFrames[actorAnim->numberQueued] = morphs;
			actorAnim->numberQueued++;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: SetAnimationSpeed	
	Purpose 	: Sets the speed of an actor
	Parameters 	: actor, speed
	Returns 	: none
	Info 		:
*/
void SetAnimationSpeed(ACTOR *myActor, float speed)
{
	myActor->animation->animationSpeed = speed;
}

/*	--------------------------------------------------------------------------------
	Function 	: FlushQueue()
	Purpose 	: Flush the animation queue of the given actor
	Parameters 	: actor pointer
	Returns 	: none
	Info 		:
*/
void FlushQueue(ACTOR *myActor)
{
	int i;

	myActor->animation->numberQueued = 0;
	for(i = 0;i < ANIM_QUEUE_LENGTH;i++)
		myActor->animation->queueAnimation[i] = -1;
}

/*	--------------------------------------------------------------------------------
	Function 	: AnimateActor(ACTOR *actor)
	Purpose 	: handle animation for all charcters
	Parameters 	: ACTOR *
	Returns 	: none
	Info 		:
*/
void UpdateAnimations(ACTOR *actor)
{
	ACTOR_ANIMATION *actorAnim = actor->animation;
	animation *anim;
	int i;

	if(actorAnim == NULL)
		return;

	if(actorAnim->anims == NULL)
		return;
	anim = actorAnim->anims + actorAnim->currentAnimation;

	if(actorAnim->currentAnimation < 0)
		return;

	if(actorAnim->numMorphFrames > 0)
	{
		if (actorAnim->currentMorphFrame < actorAnim->numMorphFrames)
			actorAnim->currentMorphFrame++;
		else
			actorAnim->numMorphFrames = 0;
		return;
	}

	actorAnim->reachedEndOfAnimation = FALSE;
	actorAnim->animTime += (actorAnim->animationSpeed) * gameSpeed;	// * GAME_SPEED);

	if(((actorAnim->animTime > anim->animEnd) || (actorAnim->animTime < anim->animStart)) && (actorAnim->loopAnimation == NO))
	{
		actorAnim->animTime = Bound(actorAnim->animTime,anim->animStart,anim->animEnd);
		actorAnim->reachedEndOfAnimation = actorAnim->currentAnimation+1;

		if(actorAnim->numberQueued)
		{
			actorAnim->currentAnimation = actorAnim->queueAnimation[0];//actorAnim;
			actorAnim->loopAnimation = actorAnim->queueLoopAnimation[0];//loop;
			actorAnim->animationSpeed = actorAnim->queueAnimationSpeed[0];//speed;
			actorAnim->numMorphFrames = actorAnim->queueNumMorphFrames[0];
			if(actorAnim->numMorphFrames != 0)
			{
				actorAnim->morphTo = actorAnim->anims[actorAnim->currentAnimation].animStart;
				actorAnim->morphFrom = actorAnim->animTime;
				actorAnim->currentMorphFrame = 0;
			}
			if(actorAnim->animationSpeed < 0)
				actorAnim->animTime = actorAnim->anims[actorAnim->currentAnimation].animEnd;
			else
				actorAnim->animTime = actorAnim->anims[actorAnim->currentAnimation].animStart;


			if (--actorAnim->numberQueued)
			{
				for(i = 0;i < ANIM_QUEUE_LENGTH - 1;i++)
				{
					actorAnim->queueAnimation[i] = actorAnim->queueAnimation[i + 1];
					actorAnim->queueLoopAnimation[i] = actorAnim->queueLoopAnimation[i + 1];
					actorAnim->queueAnimationSpeed[i] = actorAnim->queueAnimationSpeed[i + 1];
					actorAnim->queueNumMorphFrames[i] = actorAnim->queueNumMorphFrames[i + 1];
				}

				actorAnim->queueAnimation[ANIM_QUEUE_LENGTH - 1] = -1;
				actorAnim->queueLoopAnimation[ANIM_QUEUE_LENGTH - 1] = -1;
				actorAnim->queueAnimationSpeed[ANIM_QUEUE_LENGTH - 1] = -1;
				actorAnim->queueNumMorphFrames[ANIM_QUEUE_LENGTH - 1] = 0;
			}
			else
			{
				*actorAnim->queueAnimation = -1;
				*actorAnim->queueLoopAnimation = -1;
				*actorAnim->queueAnimationSpeed = -1;
				*actorAnim->queueNumMorphFrames = 0;
			}
			if( actorAnim->sfxMapping && actorAnim->sfxMapping[actorAnim->currentAnimation] != -1 )
				PlaySample( actorAnim->sfxMapping[actorAnim->currentAnimation], &actor->pos, 500, 255, 128 );
		}
	}
	else
	{
		
		if(anim->animEnd == anim->animStart)
			actorAnim->animTime = anim->animEnd;			
		else if(actorAnim->animTime > anim->animEnd)
		{
			actorAnim->animTime -= (anim->animEnd - anim->animStart);

			if( actorAnim->sfxMapping && actorAnim->sfxMapping[actorAnim->currentAnimation] != -1 )
				PlaySample( actorAnim->sfxMapping[actorAnim->currentAnimation], &actor->pos, 500, 255, 128 );

		}
		else if(actorAnim->animTime < anim->animStart)
			actorAnim->animTime += (anim->animEnd - anim->animStart);			
		
	}
	if(actorAnim->numMorphFrames)
	{
		if(actorAnim->currentMorphFrame < -actorAnim->numMorphFrames)
			actorAnim->currentMorphFrame++;
		else
			actorAnim->numMorphFrames = 0;
		actorAnim->morphTo = actorAnim->animTime;
	}
}




/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
BOOL QueryAnimTime(ACTOR *actor, float time)
{
	if((actor->animation->animTime >= time) && (actor->animation->animTime < actor->animation->animationSpeed + time))
		return YES;

	return NO;
}


/*	--------------------------------------------------------------------------------
	Function 	: FindSfxMap
	Purpose 	: Return a pointer to an array of sample numbers for an actors animations
	Parameters 	: UID of actor
	Returns 	: Pointer to int
	Info 		:
*/
int *FindSfxMapping( unsigned long uid )
{
	unsigned long index=0;

	if( !sfx_anim_map ) return NULL;

	while( sfx_anim_map[index] && (unsigned long)sfx_anim_map[index] != uid )
	{
		switch( sfx_anim_map[index+1] )
		{
		case 0: index += NUM_FROG_ANIMS+2; break;
//		case 1: index += NUM_MULTI_ANIMS+2; break;   // NOT IMPLEMENTED YET!
		case 2: index += NUM_NME_ANIMS+2; break;
		}
	}

	if( !sfx_anim_map[index] )
		return NULL;

	return &sfx_anim_map[index+2];
}


/*	--------------------------------------------------------------------------------
	Function 	: Damage functions
	Purpose 	: Frog is hurt but not dead
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void DamageNormal( int pl )
{
	AnimateActor(frog[pl]->actor, FROG_ANIM_ASSONFIRE, NO, NO, 0.5F, 0, 0);
	CreateAndAddSpecialEffect( FXTYPE_FROGSTUN, &frog[pl]->actor->pos, &currTile[pl]->normal, 30, 0, 0, 3.0 );
}

void DamageRunOver( int pl )
{
	DamageNormal(pl);
}

void DamageDrowning( int pl )
{
	DamageNormal(pl);
}

void DamageSquashed( int pl )
{
	DamageNormal(pl);
}

void DamageFire( int pl )
{
	CreateAndAddSpecialEffect( FXTYPE_FIERYSMOKE, &frog[pl]->actor->pos, &currTile[pl]->normal, 50, 0.5, 0, 2 );
	AnimateActor(frog[pl]->actor, FROG_ANIM_ASSONFIRE, NO, NO, 0.5F, 0, 0);
}

void DamageElectric( int pl )
{
	DamageNormal(pl);
}

void DamageFalling( int pl )
{
	DamageNormal(pl);
}

void DamageWhacking( int pl )
{
	DamageNormal(pl);
}

void DamageVacuum( int pl )
{
	DamageNormal(pl);
}

void DamagePoison( int pl )
{
	DamageNormal(pl);
}


/*	--------------------------------------------------------------------------------
	Function 	: Death functions
	Purpose 	: Frog is dead
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void DeathNormal( int pl )
{
	player[pl].deathBy = DEATHBY_NORMAL;
	AnimateActor(frog[pl]->actor, FROG_ANIM_FWDSOMERSAULT, NO, NO, 0.5F, 0, 0);
	GTInit( &player[pl].dead, 3 );
}

void DeathRunOver( int pl )
{
	DeathNormal(pl);
}

void DeathDrowning( int pl )
{
	DeathNormal(pl);
}

void DeathSquashed( int pl )
{
	DeathNormal(pl);
}

void DeathFire( int pl )
{
	player[pl].deathBy = DEATHBY_FIRE;
	AnimateActor(frog[pl]->actor, FROG_ANIM_ASSONFIRE, NO, NO, 0.5F, 0, 0);

	BounceFrog( pl, 100, 50 );

	GTInit( &player[pl].dead, 3 );
}

void DeathElectric( int pl )
{
	player[pl].deathBy = DEATHBY_ELECTRIC;
	CreateAndAddSpecialEffect( FXTYPE_LIGHTNING, &frog[pl]->actor->pos, &currTile[pl]->normal, 5, 40, 0.25, 0.5 );
	AnimateActor(frog[pl]->actor, FROG_ANIM_FWDSOMERSAULT, NO, NO, 0.5F, 0, 0);
	GTInit( &player[pl].dead, 3 );
}

void DeathFalling( int pl )
{
	DeathNormal(pl);
}

void DeathWhacking( int pl )
{
	player[pl].deathBy = DEATHBY_WHACKING;
	ThrowFrogAtScreen( pl );
	GTInit( &player[pl].dead, 5 );
}

void DeathVacuum( int pl )
{
	DeathNormal(pl);
}

void DeathPoison( int pl )
{
	player[pl].deathBy = DEATHBY_POISON;
	AnimateActor(frog[pl]->actor, FROG_ANIM_LANDONHEAD, NO, NO, 0.5F, 0, 0);
	GTInit( &player[pl].dead, 3 );
}

