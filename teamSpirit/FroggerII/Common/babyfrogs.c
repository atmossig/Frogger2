/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: babyfrog.c
	Programmer	: James Healey
	Date		: 17/03/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"

BABY babyList[NUM_BABIES];
ACTOR2 *babies[NUM_BABIES];
ACTOR *babyFollow[NUM_BABIES];

GAMETILE **bTStart;

int nearestBaby		= NULL;
int lastBabySaved	= NULL;

SPRITEOVERLAY *babyIcons[NUM_BABIES];

unsigned long numBabies = 0;

const char* baby_filenames[NUM_BABIES] = { "frog", "ylfrg", "blfrog", "prfrg", "rdfrg" };


void CreateBabies(unsigned long createActors,unsigned long createOverlays)
{
	unsigned long i,j;

	nearestBaby		= NULL;
	lastBabySaved	= NULL;
	babiesSaved		= 0;
	
	if ( createActors )
	{
		for (i=0; i<numBabies; i++)
		{
			babies[i] = CreateAndAddActor("froglet.obe",0,0,200.0,INIT_ANIMATION | INIT_SHADOW,0,0);

			babies[i]->actor->shadow->radius	= 15;
			babies[i]->actor->shadow->alpha		= 191;

			InitActorAnim(babies[i]->actor);
			AnimateActor(babies[i]->actor, BABY_ANIM_HOP, YES, NO, 0.5F, NO, NO);
			babyList[i].isSaved = 0;

			SetVector( &babies[i]->actor->pos, &bTStart[i]->centre );

			babyFollow[i] = NULL;

			// determine baby colour and set values accordingly
			switch(i)
			{
				case 0:
					// green frog
					babyList[i].fxColour[R] = 0;
					babyList[i].fxColour[G] = 255;
					babyList[i].fxColour[B] = 0;
					babyList[i].fxColour[A] = 255;
					break;
				case 1:
					// yellow frog
					babyList[i].fxColour[R] = 255;
					babyList[i].fxColour[G] = 255;
					babyList[i].fxColour[B] = 0;
					babyList[i].fxColour[A] = 255;
					break;
				case 2:
					// blue frog
					babyList[i].fxColour[R] = 0;
					babyList[i].fxColour[G] = 0;
					babyList[i].fxColour[B] = 255;
					babyList[i].fxColour[A] = 255;
					break;
				case 3:
					// purple frog
					babyList[i].fxColour[R] = 255;
					babyList[i].fxColour[G] = 0;
					babyList[i].fxColour[B] = 255;
					babyList[i].fxColour[A] = 255;
					break;
				case 4:
					// red frog
					babyList[i].fxColour[R] = 255;
					babyList[i].fxColour[G] = 0;
					babyList[i].fxColour[B] = 0;
					babyList[i].fxColour[A] = 255;
					break;
			}
		}
	}

	if ( createOverlays )
	{
		for(i=0; i<numBabies; i++)
		{
			char fn[14];

			sprintf(fn, "%s001.bmp", baby_filenames[i]);
			
			babyIcons[i] = CreateAndAddSpriteOverlay(
				(short)(280-(i*20)), 205,
				fn,
				16,16,255,255,255,91,
				(short)(ANIMATION_FORWARDS|ANIMATION_CYCLE));

			// Add animation frames

			for (j=2; j<=8; j++)
			{
				sprintf(fn, "%s%03d.bmp", baby_filenames[i], j);
				AddFrameToSpriteOverlay(babyIcons[i], fn);
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: RunBabySavedSequence
	Purpose			: runs sequence when baby is saved
	Parameters		: ACTOR2 *
	Returns			: void
	Info			: 
*/
void RunBabySavedSequence(int i)
{
	static VECTOR pos,pos2;		// yuck
	SPECFX *fx;

	babySaved--;
	if(!babySavedText->draw)
	{
		EnableTextOverlay(babySavedText);
		SetVector(&pos,&babies[i]->actor->pos);
		SetVector(&pos2,&pos);
		pos2.v[Y] += 48;
	}
	else
	{
		if(babySaved)
		{
			babySavedText->a -= 8;
		}
		else
		{
			DisableTextOverlay(babySavedText);
			babySavedText->a = 255;
		}

		if((babySaved & 3) == 0)
		{
			pos.v[Y] += 6;
			pos2.v[Y] -= 4;
			
			CreateAndAddSpecialEffect( FXTYPE_BASICRING, &pos, &upVec, 5, 1, 0.05, 0.6 );
			CreateAndAddSpecialEffect( FXTYPE_BASICRING, &pos, &upVec, 10, 0.8, 0.05, 0.3 );

			fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_GROWS, &babies[i]->actor->pos, &upVec, 64, 0, 0, 2 );
			fx->sprites->r = babyList[i].fxColour[R];
			fx->sprites->g = babyList[i].fxColour[G];
			fx->sprites->b = babyList[i].fxColour[B];
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: GetNearestBabyFrog
	Purpose			: determines the nearest baby frog
	Parameters		: 
	Returns			: ACTOR2 *
	Info			: used for, say, determining
*/
int GetNearestBabyFrog()
{
	int nearest = -1;
	float t,distance = 99999;
	int i;

	i = numBabies;
	while(i--)
	{
		if(!babyList[i].isSaved)
		{
			t = DistanceBetweenPoints(&frog[0]->actor->pos,&babies[i]->actor->pos);
			if(t < distance)
			{
				distance	= t;
				nearest		= i;
			}
		}
	}

	if(nearest && (distance < CROAK_SOUND_RANGE))
		PlaySample(218, &babies[nearest]->actor->pos, 255, (short)(128 + (128 - (distance / 4))));

	return i;
}

/*	--------------------------------------------------------------------------------
	Function		: FaceBabiesTowardsPlayer
	Purpose			: makes baby frogs face the frog
	Parameters		: long
	Returns			: void
	Info			: 
*/
void FaceBabiesTowardsFrog(long pl)
{
	VECTOR vfd	= { 0,0,1 };
	VECTOR babyup;
	VECTOR v1,v2,v3;
	unsigned long i;
		
	for(i=0; i<numBabies; i++)
	{
		if(babies[i])
		{
			if(!(babyList[i].isSaved))
			{
				if(bTStart[i])
				{
					babies[i]->actor->pos.v[X] = bTStart[i]->centre.v[X];
					babies[i]->actor->pos.v[Y] = bTStart[i]->centre.v[Y];
					babies[i]->actor->pos.v[Z] = bTStart[i]->centre.v[Z];
				}
				else
				{
					babies[i]->actor->pos.v[X] = 0;
					babies[i]->actor->pos.v[Y] = 0;
					babies[i]->actor->pos.v[Z] = 0;
				}
				babyList[i].isSaved = 0;
			}

			// face baby towards frog
			SubVector(&v1,&babies[i]->actor->pos,&frog[pl]->actor->pos);
			MakeUnit(&v1);

			// calculate baby up vector
			RotateVectorByQuaternion(&babyup,&upVec,&babies[i]->actor->qRot);
			CrossProduct(&v2,&v1,&babyup);
			CrossProduct(&v3,&v2,&babyup);
			Orientate(&babies[i]->actor->qRot,&v3,&vfd,&babyup);
		}
	}
}

