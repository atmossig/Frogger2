/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: anim.c
	Programmer	: Andy Eder
	Date		: 29/04/99 10:30:56

----------------------------------------------------------------------------------------------- */


#define F3DEX_GBI

#include <ultra64.h>

#include "incs.h"


/*	--------------------------------------------------------------------------------
	Function 	  : 
	Purpose 	   : 
	Parameters 	: 
	Returns 	   : 
	Info 		     :
*/
void InitActorAnim(ACTOR *tempActor)
{
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
}

/*	--------------------------------------------------------------------------------
	Function 	  : 
	Purpose 	   : 
	Parameters 	: 
	Returns    	: 
	Info 		     :
*/
void AnimateActor(ACTOR *actor, int animNum, char loop, char queue, float speed)
{
	ACTOR_ANIMATION *actorAnim = actor->animation;
	animation *anim;
	float	actualSpeed;

	if(actor->animation == NULL)
		return;
	if(actor->animation->anims == NULL)
		return;

	if((actor->animation->currentAnimation == animNum) && (queue == NO) && (actor->animation->reachedEndOfAnimation == FALSE) && (((speed == 0) && (actorAnim->animationSpeed == actor->animation->anims[animNum].speed)) || (speed == actorAnim->animationSpeed)))
	{
		actorAnim->loopAnimation = loop;
		return;
	}

	if(animNum > actorAnim->numAnimations)
	{
//		dprintf"Trying to access illegal animation\n"));
		animNum = 0;
	}

	if(speed == 0)
		actualSpeed = actor->animation->anims[animNum].speed;
	else
		actualSpeed = speed;

	if(queue == NO)
	{
		actorAnim->currentAnimation = animNum;
		actorAnim->loopAnimation = loop;
		actor->loopCount = 0;
		actorAnim->animationSpeed = actualSpeed;
		actorAnim->reachedEndOfAnimation = FALSE;
		anim = actorAnim->anims + actorAnim->currentAnimation;
		actorAnim->animTime = anim->animStart;
		FlushQueue(actor);
		if(actualSpeed < 0)
			actorAnim->animTime = anim->animEnd;
	}
	else
	{
		if(actorAnim->currentAnimation == -1)
		{
			actorAnim->currentAnimation = animNum;
			actorAnim->loopAnimation = loop;
			actorAnim->animationSpeed = actualSpeed;
		}
		else
		{
			if(actorAnim->numberQueued == 5)
				return;
			actorAnim->queueAnimation[actorAnim->numberQueued] = animNum;
			actorAnim->queueLoopAnimation[actorAnim->numberQueued] = loop;
			actorAnim->queueAnimationSpeed[actorAnim->numberQueued] = actualSpeed;
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
	
//reject all objects which have no animation
	if(actorAnim == NULL)
		return;

	if(actorAnim->anims == NULL)
		return;
	anim = actorAnim->anims + actorAnim->currentAnimation;

	//reset all the stuff

	//tick the clock
	if(actorAnim->currentAnimation < 0)
		return;

	actorAnim->reachedEndOfAnimation = FALSE;
	actorAnim->animTime += actorAnim->animationSpeed;

	
	if(((actorAnim->animTime > anim->animEnd) || (actorAnim->animTime < anim->animStart)) && (actorAnim->loopAnimation == NO))
	{
		actorAnim->animTime = Bound(actorAnim->animTime,anim->animStart,anim->animEnd);
		actorAnim->reachedEndOfAnimation = actorAnim->currentAnimation+1;

		//queue stuff - do this later
		if(actorAnim->queueAnimation[0] != -1)
		{
//			SetAnimCallBack(temp, actorAnim->anims[actorAnim->queueAnimation[0]].animationSet);


			actorAnim->currentAnimation = actorAnim->queueAnimation[0];//actorAnim;
			actorAnim->loopAnimation = actorAnim->queueLoopAnimation[0];//loop;
			actorAnim->animationSpeed = actorAnim->queueAnimationSpeed[0];//speed;
			if(actorAnim->animationSpeed < 0)
				actorAnim->animTime = actorAnim->anims[actorAnim->currentAnimation].animEnd;
			else
				actorAnim->animTime = actorAnim->anims[actorAnim->currentAnimation].animStart;


			for(i = 0;i < ANIM_QUEUE_LENGTH - 1;i++)
			{
				actorAnim->queueAnimation[i] = actorAnim->queueAnimation[i + 1];
				actorAnim->queueLoopAnimation[i] = actorAnim->queueLoopAnimation[i + 1];
				actorAnim->queueAnimationSpeed[i] = actorAnim->queueAnimationSpeed[i + 1];
			}
			actorAnim->queueAnimation[ANIM_QUEUE_LENGTH - 1] = -1;
			actorAnim->queueLoopAnimation[ANIM_QUEUE_LENGTH - 1] = -1;
			actorAnim->queueAnimationSpeed[ANIM_QUEUE_LENGTH - 1] = -1;
			if(actorAnim->numberQueued >0) 
				actorAnim->numberQueued--;
		}
	}
	else
	{
		if(actorAnim->animTime > anim->animEnd)
			actorAnim->animTime = anim->animStart;
		else if(actorAnim->animTime < anim->animStart)
			actorAnim->animTime = anim->animEnd;
//			actorAnim->animTime -= (anim->animEnd - anim->animStart);
 		actor->loopCount++;
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


