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

PLAYER player[MAX_FROGS];

ACTOR2 *frog[MAX_FROGS]					= {0,0,0,0};
ACTOR2 *frog2					= NULL;
SPRITEOVERLAY *sprHeart[3]		= { NULL,NULL,NULL };

long NUM_FROGS = 1;

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

void CreateFrogActor (GAMETILE *where, char *name,long p)
{
	ACTOR2 **me = &frog[p];
	(*me)		 = CreateAndAddActor ("frogger.ndo",0,0,200.0,INIT_ANIMATION | INIT_SHADOW,0, 0);
	(*me)->actor->shadow->radius = 30;
	(*me)->actor->shadow->alpha = 191;
	(*me)->flags	|= ACTOR_DRAW_ALWAYS;
	
	tongueState	 = TONGUE_NONE | TONGUE_IDLE;
	
	InitActorAnim ( (*me)->actor );
	AnimateActor  ( (*me)->actor, 0, YES, NO, 0.667, 10, 0 );
	
	(*me)->actor->scale.v[0] = 0.09;
	(*me)->actor->scale.v[1] = 0.09;
	(*me)->actor->scale.v[2] = 0.09;
	
	SetFroggerStartPos ( where, (*me),p);
	
	(*me)->action.lives		= 3;
	(*me)->action.isOnFire	= 0;
	(*me)->action.frogon	= -1;
	(*me)->action.frogunder = -1;

	(*me)->radius			= 37.0F;

}

void CreateFrogger(unsigned long createFrogActor,unsigned long createFrogOverlays,
					unsigned long createBabyActors,unsigned long createBabyOverlays)
{
	int i;

	if ( createFrogActor )
	{
		CreateBabies(createBabyActors, createBabyOverlays );
		for (i=0; i<MAX_FROGS; i++)
			CreateFrogActor (gTStart[0],"frogger.ndo",i);
		
		for (i=4; i>NUM_FROGS; i--)
			frog[i-1]->actor->xluOverride = 0;
		
/*
		frog[0]		 = CreateAndAddActor ("frogger.ndo",0,0,200.0,INIT_ANIMATION | INIT_SHADOW,0, 0);
		frog[0]->actor->shadow->radius = 30;
		frog[0]->actor->shadow->alpha = 191;
		frog[0]->flags	|= ACTOR_DRAW_ALWAYS;
		tongueState	 = TONGUE_NONE | TONGUE_IDLE;
		InitActorAnim ( frog[0]->actor );
		AnimateActor  ( frog[0]->actor, 0, YES, NO, 0.667 );
		frog[0]->actor->scale.v[0] = 0.09;
		frog[0]->actor->scale.v[1] = 0.09;
		frog[0]->actor->scale.v[2] = 0.09;
		CreateBabies(createBabyActors, createBabyOverlays );
		SetFroggerStartPos ( gTStart[0], frog[0] );
		frog[0]->action.lives		= 3;
		frog[0]->action.isOnFire	= 0;
		frog[0]->radius				= 37.0F;
*/
		frog2		 = CreateAndAddActor (me,0,0,200.0,INIT_ANIMATION | INIT_SHADOW,0, 0);
		frog2->actor->shadow->radius = 30;
		frog2->actor->shadow->alpha = 191;
		frog2->flags |= ACTOR_DRAW_ALWAYS;
		frog2->draw = 1;
		InitActorAnim ( frog2->actor );
		AnimateActor  ( frog2->actor, 0, YES, NO, 0.667, 10, 0 );
		frog2->actor->scale.v[0] = 0.09;
		frog2->actor->scale.v[1] = 0.09;
		frog2->actor->scale.v[2] = 0.09;
		//SetFroggerStartPos ( gTStart[0], frog2,);
		frog2->action.lives = 3;
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
