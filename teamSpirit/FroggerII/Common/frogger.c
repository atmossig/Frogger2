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

char me[100] = "frogger.obe";

void CreateFrogActor (GAMETILE *where, char *name,long p)
{
	ACTOR2 **me = &frog[p];
	if ( p == 0 )
		(*me)		 = CreateAndAddActor ("frogger.obe",0,0,200.0,INIT_ANIMATION | INIT_SHADOW,0, 0);
	else
		(*me)		 = CreateAndAddActor ("froglet.obe",0,0,200.0,INIT_ANIMATION | INIT_SHADOW,0, 0);

	(*me)->actor->shadow->radius = 30;
	(*me)->actor->shadow->alpha = 191;
	(*me)->flags	|= ACTOR_DRAW_ALWAYS;
	
	tongueState	 = TONGUE_NONE | TONGUE_IDLE;
	
	if ( p == 0 )
	{
		InitActorAnim ( (*me)->actor );
		AnimateActor  ( (*me)->actor, 0,YES, NO, 0.7f, 100, 0 );
	}
	
	(*me)->actor->scale.v[0] = 0.0075;
	(*me)->actor->scale.v[1] = 0.0075;
	(*me)->actor->scale.v[2] = 0.0075;
	
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

	if(createFrogActor)
	{
		CreateBabies(createBabyActors, createBabyOverlays );
		for (i=0; i<MAX_FROGS; i++)
			CreateFrogActor (gTStart[0],"frogger.ndo",i);
		
		for (i=4; i>NUM_FROGS; i--)
			frog[i-1]->actor->xluOverride = 0;
	}

	if(createFrogOverlays)
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
}
