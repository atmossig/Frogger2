/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: frogger.c
	Programmer	: James Healey
	Date		: 23/03/99

----------------------------------------------------------------------------------------------- */


#define F3DEX_GBI

#include <ultra64.h>

#include "incs.h"



//----------------------------------------------------------------------------//
//----- GLOBALS --------------------------------------------------------------//
//----------------------------------------------------------------------------//

PLAYER player[4];

ACTOR2 *frog					= NULL;
ACTOR2 *frog2					= NULL;
SPRITEOVERLAY *sprHeart[3]		= { NULL,NULL,NULL };

float CROAK_SOUND_RANGE			= 400;

float croakRadius				= 15;
float croakVelocity				= 1.1F;
char croakFade					= 255;
char croakFadeDec				= 16;
char croakR						= 255;
char croakG						= 255;
char croakB						= 255;

//----------------------------------------------------------------------------//
//----- FUNCTIONS ------------------------------------------------------------//
//----------------------------------------------------------------------------//

char me[100] = "frogger.ndo";

void CreateFrogger(unsigned long createFrogActor,unsigned long createFrogOverlays,
					unsigned long createBabyActors,unsigned long createBabyOverlays)
{
	int i;

	if ( createFrogActor )
	{
		frog		 = CreateAndAddActor ("frogger.ndo",0,0,200.0,INIT_ANIMATION | INIT_SHADOW,0, 0);
		frog->actor->shadow->radius = 30;
		frog->actor->shadow->alpha = 191;
		frog->flags	|= ACTOR_DRAW_ALWAYS;
		tongueState	 = TONGUE_NONE | TONGUE_IDLE;
		InitActorAnim ( frog->actor );
		AnimateActor  ( frog->actor, 0, YES, NO, 0.667 );
		frog->actor->scale.v[0] = 0.09;
		frog->actor->scale.v[1] = 0.09;
		frog->actor->scale.v[2] = 0.09;
		CreateBabies(createBabyActors, createBabyOverlays );
		SetFroggerStartPos ( gTStart[0], frog );
		frog->action.lives		= 3;
		frog->action.isOnFire	= 0;
		frog->radius = 20;

		frog2		 = CreateAndAddActor (me,0,0,200.0,INIT_ANIMATION | INIT_SHADOW,0, 0);
		frog2->actor->shadow->radius = 30;
		frog2->actor->shadow->alpha = 191;
		frog2->flags |= ACTOR_DRAW_ALWAYS;
		frog2->draw = 1;
		InitActorAnim ( frog2->actor );
		AnimateActor  ( frog2->actor, 0, YES, NO, 0.667 );
		frog2->actor->scale.v[0] = 0.09;
		frog2->actor->scale.v[1] = 0.09;
		frog2->actor->scale.v[2] = 0.09;
		SetFroggerStartPos ( gTStart[0], frog2);
		frog2->action.lives = 3;
		frog2->radius = 20;
	}
	// ENIF

	if ( createFrogOverlays )
	{

		// get health icons ready
		i = 3;
		while(i--)
		{
			sprHeart[i] = CreateAndAddSpriteOverlay(20+(i*10),205,"heart001.bmp",16,16,255,255,255,192,ANIMATION_FORWARDS | ANIMATION_CYCLE);
			AddFrameToSpriteOverlay(sprHeart[i],"heart002.bmp");
			AddFrameToSpriteOverlay(sprHeart[i],"heart003.bmp");
			AddFrameToSpriteOverlay(sprHeart[i],"heart004.bmp");
			AddFrameToSpriteOverlay(sprHeart[i],"heart005.bmp");
			AddFrameToSpriteOverlay(sprHeart[i],"heart006.bmp");
			AddFrameToSpriteOverlay(sprHeart[i],"heart007.bmp");
			AddFrameToSpriteOverlay(sprHeart[i],"heart008.bmp");
			AddFrameToSpriteOverlay(sprHeart[i],"heart009.bmp");
			AddFrameToSpriteOverlay(sprHeart[i],"heart010.bmp");
			SetSpriteOverlayAnimSpeed(sprHeart[i],1.0F);
			sprHeart[i]->animTime = (i * 3);
		}
	}
	// ENDIF 
}
