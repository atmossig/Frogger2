/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: actor.c
	Programmer	: Matthew Cloy
	Date		: 11/11/98

----------------------------------------------------------------------------------------------- */
#include <math.h>
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>

#define F3DEX_GBI_2
#define LEAVEOUT_MATH

extern "C"
{


#include "actor.h"
#include "frogger.h"
#include "game.h"
#include "Types.h"
#include "maths.h"
#include "cam.h"
#include "sprite.h"
#include <islmem.h>
#include <islutil.h>
#include "mdx.h"


void actorAnimate(ACTOR *actor, int animNum, char loop, char queue, int speed, char skipendframe)
{
	if (actor->actualActor)
		Animate((MDX_ACTOR *)actor->actualActor,animNum,loop,queue,speed/256.0f);
}

void actorFree(ACTOR *actor)
{
	if( actor->shadow )
		FREE( actor->shadow );

	// Free the mdx_actor
	if (actor->actualActor)
		FreeActor((MDX_ACTOR **)&actor->actualActor);

	FREE(actor);
}

/*	--------------------------------------------------------------------------------
	Function 	: ChangeModel
	Purpose 	: Swap models within an actor
	Parameters 	: actor, name of new model
	Returns 	: void
	Info 		:
*/
void ChangeModel( ACTOR *actor, char *model )
{
	MDX_ACTOR *a = (MDX_ACTOR *)actor->actualActor;

	a->LODObjectController = a->objectController;
	FindObject( &a->objectController, UpdateCRC(model), model );
	InitAnims( a );
}


/*	--------------------------------------------------------------------------------
	Function 	: UndoChangeModel
	Purpose 	: Swap models back after a ChangeModel
	Parameters 	: actor
	Returns 	: void
	Info 		:
*/
int UndoChangeModel( ACTOR *actor )
{
	MDX_ACTOR *a = (MDX_ACTOR *)actor->actualActor;

	if( a->LODObjectController )
	{
		a->objectController = a->LODObjectController;
		a->LODObjectController = NULL;
		InitAnims( a );
		actorAnimate( actor, FROG_ANIM_BREATHE, YES, NO, 102, 0);
		return 1;
	}
	return 0;
}


/*	--------------------------------------------------------------------------------
	Function	: AnimateActor
	Purpose		: Runs an animation on an actor EXCEPT when it's already running
	Parameters	: actor, anim, loop?, queue?, speed, morph?, keep proportion?
	Returns		: void
*/
void StartAnimateActor(ACTOR *actor, int animNum, char loop, char queue, int speed, char skip)
{
	MDX_ACTOR *a = (MDX_ACTOR *)actor->actualActor;

 	if (a->animation->currentAnimation != animNum)
 		actorAnimate(actor, animNum, loop, queue, speed, skip);
}

}
