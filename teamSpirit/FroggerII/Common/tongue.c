/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: tongue.c
	Programmer	: Andrew Eder
	Date		: 12/15/98

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


//float TONGUE_RADIUSNORMAL = 85.0;
//float TONGUE_RADIUSLONG	  = 175.0;

float tongueRadius			= 200.0;

unsigned long tongueState	= TONGUE_NONE | TONGUE_IDLE;

int tongueCoordIndex		= 0;
int tongueSegment			= 0;

unsigned int numTongueNodes	= MAX_TONGUENODES;

VECTOR tongueCoords[MAX_TONGUENODES];

VECTOR tongueUpVector;
VECTOR tongueCurrPos;

VECTOR ff;		// frog facing direction vector
VECTOR fu;		// frog up vector
VECTOR fr;		// frog right vector

char tongueToCollect		= 0;

GARIB *nearestColl			= NULL;
ACTOR2 *nearestBabyFrog		= NULL;
GAMETILE *nearestGrapple    = NULL;
ACTOR2 *nearestScenic		= NULL;

float tongueMag				= 0;
float tongueMagStep			= 0;

float tongueRotateAngle		= 0;
float tongueRotateStep		= 0;

SPRITE tongueSprite[MAX_TONGUENODES];

TEXTURE *txtrTongue = NULL;

/*	--------------------------------------------------------------------------------
	Function		: InitTongue
	Purpose			: initialises Frogger's tongue for picking up, throwing, etc.
	Parameters		: char
	Returns			: void
	Info			: contains initialisation data for shooting out the tongue
*/
void InitTongue(char toCollect)
{
	QUATERNION q;
	VECTOR v = { 0,0,1 };
	int i = MAX_TONGUENODES;
		
	tongueCoordIndex	= 0;
	tongueSegment		= 0;
	numTongueNodes		= MAX_TONGUENODES;

	tongueToCollect		= toCollect;

	FindTexture(&txtrTongue,UpdateCRC("tongue1.bmp"),YES);

	// Determine frog's forward vector (ff) and up vector (fu) and right vector (fr)
	RotateVectorByQuaternion(&ff,&v,&frog[0]->actor->qRot);
	SetVector(&fu,&currTile[0]->normal);
	CrossProduct(&fr,&ff,&fu);
}

/*	--------------------------------------------------------------------------------
	Function		: UpdateFrogTongue
	Purpose			: updates Frogger's tongue for collecting collectables
	Parameters		: none
	Returns			: 
	Info			:
*/
void UpdateFrogTongue()
{
	VECTOR p;				// vector from end of 1st tongue segment to the pickup
	VECTOR newVector;
	
	QUATERNION q;
	float dp;
	float angle;
	float mag;
	float x,y,z;
	int i;
	
	// Determine tongue state and perform relevant action / update

	if(tongueState & TONGUE_IDLE)
	{
		// Just to test grappling
		//gTStart[0]->next->state = TILESTATE_GRAPPLE;

		// Tongue idle
		return;
	}

	// Frogs tongue is ready for action
	if(tongueState & TONGUE_BEINGUSED)
	{
		// Check if tongue can actually be used
		if(tongueState & TONGUE_HASITEMINMOUTH)
		{
			return;
		}

		if(tongueState & TONGUE_OUTGOING)
		{
			if((tongueSegment < (tongueCoordIndex - 1)))
			{
				x = tongueCoords[tongueSegment].v[X];
				y = tongueCoords[tongueSegment].v[Y];
				z = tongueCoords[tongueSegment].v[Z];
				AddTongueSprite(tongueSegment,x,y,z);
				tongueSegment++;
			}
			else
			{
				tongueState = TONGUE_NONE | TONGUE_BEINGUSED | TONGUE_INCOMING | TONGUE_HASITEMONIT;
				if( tongueToCollect == TONGUE_GET_GRAPPLE ) tongueSegment = 0;
				return;
			}
		}
		
		if(tongueState & TONGUE_INCOMING)
		{
			if(tongueToCollect == TONGUE_GET_GARIB)
			{
				tongueSegment--;
				SetVector(&nearestColl->sprite.pos,&tongueCoords[tongueSegment]);
				nearestColl->scale *= 0.9;
				nearestColl->scale *= 0.9;
				nearestColl->scale *= 0.9;
			}
			else if( tongueToCollect == TONGUE_GET_BABY )
			{
				tongueSegment--;
				SetVector(&nearestBabyFrog->actor->pos,&tongueCoords[tongueSegment]);
				nearestBabyFrog->actor->scale.v[X] *= 0.9;
				nearestBabyFrog->actor->scale.v[Y] *= 0.9;
				nearestBabyFrog->actor->scale.v[Z] *= 0.9;
			}
			else if( tongueToCollect == TONGUE_GET_SCENIC )
			{
				tongueSegment--;
				nearestScenic->actor->pos.v[X] += (-3 + Random(7));
				nearestScenic->actor->pos.v[Y] += (-3 + Random(7));
				nearestScenic->actor->pos.v[Z] += (-3 + Random(7));
			}
			else // TONGUE_GET_GRAPPLE
			{
				SetVector(&frog[0]->actor->pos,&tongueCoords[tongueSegment]);
				tongueSegment++;
			}

			RemoveTongueSegment(tongueSegment);

			if(tongueSegment < 1 || tongueSegment > MAX_TONGUENODES )
			{
//				PlaySample(111,NULL,192,128);

				if(tongueToCollect == TONGUE_GET_GARIB)
					PickupCollectable(nearestColl,0);
				else if( tongueToCollect == TONGUE_GET_BABY )
					PickupBabyFrog(nearestBabyFrog);
				else if( tongueToCollect == TONGUE_GET_SCENIC )
					SetVector(&nearestScenic->actor->pos,&nearestScenic->actor->oldpos);
				else
				{
					currTile[0] = nearestGrapple;
					SetVector( &frog[0]->actor->pos, &nearestGrapple->centre );
				}

				// Check if frog has something in his mouth
				player[0].frogState &= ~FROGSTATUS_ISTONGUEING;
				tongueState = TONGUE_IDLE;

				// Set frog idle animation, and the speed
				frog[0]->actor->animation->animTime = 0;
				AnimateActor(frog[0]->actor,FROG_ANIM_BREATHE,YES,YES,0.5F,0,0);

				RemoveFrogTongue();
			}
		}
		
		if(tongueState & TONGUE_THROWING)
		{
		}
	}

	if(tongueState & TONGUE_SEARCHING)
	{
		// first priority - check if baby frog is in range
		if(nearestBabyFrog = BabyFrogIsInTongueRange())
		{
			InitTongue(TONGUE_GET_BABY);

			// Get first tongue position (tongueCoordIndex == 0)
			tongueCoords[tongueCoordIndex].v[X] = frog[0]->actor->pos.v[X] + (ff.v[X] * TONGUE_OFFSET_FORWARD) - (fu.v[X] * TONGUE_OFFSET_UP);
			tongueCoords[tongueCoordIndex].v[Y] = frog[0]->actor->pos.v[Y] + (ff.v[Y] * TONGUE_OFFSET_FORWARD) - (fu.v[Y] * TONGUE_OFFSET_UP);
			tongueCoords[tongueCoordIndex].v[Z] = frog[0]->actor->pos.v[Z] + (ff.v[Z] * TONGUE_OFFSET_FORWARD) - (fu.v[Z] * TONGUE_OFFSET_UP);
			tongueCoordIndex++;

			// Calculate the vector to the collectable item and its magnitude
			SubVector(&p,&nearestBabyFrog->actor->pos,&tongueCoords[0]);
			tongueMag = Magnitude(&p);
			MakeUnit(&p);

			// Calculate angle through which the tongue needs to rotate
			dp = DotProduct(&ff,&p);
			if(dp < TONGUE_WRAPAROUNDTHRESHOLD)
			{
				tongueState = TONGUE_NONE | TONGUE_IDLE;
			}
			else
			{
				tongueMagStep = tongueMag / numTongueNodes;

				// Determine the tongue up vector about which the tongue rotates
				CrossProduct(&tongueUpVector,&ff,&p);

				tongueRotateAngle = acos(dp);
				tongueRotateStep = tongueRotateAngle / numTongueNodes;

				// Create tongue segments
				while(numTongueNodes--)
					CreateTongueSegment(tongueCoordIndex);

				// Set frog mouth open animation, and the speed
				frog[0]->actor->animation->animTime = 0;
				AnimateActor(frog[0]->actor,FROG_ANIM_USINGTONGUE,NO,NO,0.5F,0,0);

				tongueState		= TONGUE_NONE | TONGUE_BEINGUSED | TONGUE_OUTGOING;
			}
		}
		else if(nearestColl = GaribIsInTongueRange())
		{
			InitTongue(TONGUE_GET_GARIB);

			// Get first tongue position (tongueCoordIndex == 0)
			tongueCoords[tongueCoordIndex].v[X] = frog[0]->actor->pos.v[X] + (ff.v[X] * TONGUE_OFFSET_FORWARD) - (fu.v[X] * TONGUE_OFFSET_UP);
			tongueCoords[tongueCoordIndex].v[Y] = frog[0]->actor->pos.v[Y] + (ff.v[Y] * TONGUE_OFFSET_FORWARD) - (fu.v[Y] * TONGUE_OFFSET_UP);
			tongueCoords[tongueCoordIndex].v[Z] = frog[0]->actor->pos.v[Z] + (ff.v[Z] * TONGUE_OFFSET_FORWARD) - (fu.v[Z] * TONGUE_OFFSET_UP);
			tongueCoordIndex++;

			// Calculate the vector to the collectable item and its magnitude
			if(tongueToCollect == TONGUE_GET_GARIB)
			{
				SubVector(&p,&nearestColl->sprite.pos,&tongueCoords[0]);
			}
			else
			{
				SubVector(&p,&nearestBabyFrog->actor->pos,&tongueCoords[0]);
			}

			tongueMag = Magnitude(&p);
			MakeUnit(&p);

			// Calculate angle through which the tongue needs to rotate
			dp = DotProduct(&ff,&p);
			if(dp < TONGUE_WRAPAROUNDTHRESHOLD)
			{
				tongueState = TONGUE_NONE | TONGUE_IDLE;
			}
			else
			{
				tongueMagStep = tongueMag / numTongueNodes;

				// Determine the tongue up vector about which the tongue rotates
				CrossProduct(&tongueUpVector,&ff,&p);

				tongueRotateAngle = acos(dp);
				tongueRotateStep = tongueRotateAngle / numTongueNodes;

				// Create tongue segments
				while(numTongueNodes--)
					CreateTongueSegment(tongueCoordIndex);

				// Set frog mouth open animation, and the speed
				frog[0]->actor->animation->animTime = 0;
				AnimateActor(frog[0]->actor,FROG_ANIM_USINGTONGUE,NO,NO,0.5F,0,0);
				
				tongueState		= TONGUE_NONE | TONGUE_BEINGUSED | TONGUE_OUTGOING;
			}
		}
		else if(nearestGrapple = GrapplePointInTongueRange())
		{
			// Grapple towards point
			InitTongue(TONGUE_GET_GRAPPLE);

			// Get first tongue position (tongueCoordIndex == 0)
			tongueCoords[tongueCoordIndex].v[X] = frog[0]->actor->pos.v[X] + (ff.v[X] * TONGUE_OFFSET_FORWARD) - (fu.v[X] * TONGUE_OFFSET_UP);
			tongueCoords[tongueCoordIndex].v[Y] = frog[0]->actor->pos.v[Y] + (ff.v[Y] * TONGUE_OFFSET_FORWARD) - (fu.v[Y] * TONGUE_OFFSET_UP);
			tongueCoords[tongueCoordIndex].v[Z] = frog[0]->actor->pos.v[Z] + (ff.v[Z] * TONGUE_OFFSET_FORWARD) - (fu.v[Z] * TONGUE_OFFSET_UP);
			tongueCoordIndex++;

			// Calculate the vector to the collectable item and its magnitude
			SubVector(&p,&nearestGrapple->centre,&tongueCoords[0]);

			tongueMag = Magnitude(&p);
			MakeUnit(&p);

			// Calculate angle through which the tongue needs to rotate
			dp = DotProduct(&ff,&p);
			if(dp < TONGUE_WRAPAROUNDTHRESHOLD)
			{
				tongueState = TONGUE_NONE | TONGUE_IDLE;
			}
			else
			{
				tongueMagStep = tongueMag / numTongueNodes;

				// Determine the tongue up vector about which the tongue rotates
				CrossProduct(&tongueUpVector,&ff,&p);

				tongueRotateAngle = acos(dp);
				tongueRotateStep = tongueRotateAngle / numTongueNodes;

				// Create tongue segments
				while(numTongueNodes--)
					CreateTongueSegment(tongueCoordIndex);

				// Set frog mouth open animation, and the speed
				frog[0]->actor->animation->animTime = 0;
				AnimateActor(frog[0]->actor,FROG_ANIM_USINGTONGUE,NO,NO,0.5F,0,0);

				tongueState		= TONGUE_NONE | TONGUE_BEINGUSED | TONGUE_OUTGOING;
			}
		}
		else if(nearestScenic = ScenicIsInTongueRange())
		{
			// tongue towards scenic
			InitTongue(TONGUE_GET_SCENIC);

			// Get first tongue position (tongueCoordIndex == 0)
			tongueCoords[tongueCoordIndex].v[X] = frog[0]->actor->pos.v[X] + (ff.v[X] * TONGUE_OFFSET_FORWARD) - (fu.v[X] * TONGUE_OFFSET_UP);
			tongueCoords[tongueCoordIndex].v[Y] = frog[0]->actor->pos.v[Y] + (ff.v[Y] * TONGUE_OFFSET_FORWARD) - (fu.v[Y] * TONGUE_OFFSET_UP);
			tongueCoords[tongueCoordIndex].v[Z] = frog[0]->actor->pos.v[Z] + (ff.v[Z] * TONGUE_OFFSET_FORWARD) - (fu.v[Z] * TONGUE_OFFSET_UP);
			tongueCoordIndex++;

			// Calculate the vector to the collectable item and its magnitude
			SubVector(&p,&nearestScenic->actor->pos,&tongueCoords[0]);

			tongueMag = Magnitude(&p);
			MakeUnit(&p);

			// Calculate angle through which the tongue needs to rotate
			dp = DotProduct(&ff,&p);
			if(dp < TONGUE_WRAPAROUNDTHRESHOLD)
			{
				tongueState = TONGUE_NONE | TONGUE_IDLE;
			}
			else
			{
				tongueMagStep = tongueMag / numTongueNodes;

				// Determine the tongue up vector about which the tongue rotates
				CrossProduct(&tongueUpVector,&ff,&p);

				tongueRotateAngle = acos(dp);
				tongueRotateStep = tongueRotateAngle / numTongueNodes;

				// Create tongue segments
				while(numTongueNodes--)
					CreateTongueSegment(tongueCoordIndex);

				// Set frog mouth open animation, and the speed
				frog[0]->actor->animation->animTime = 0;
				AnimateActor(frog[0]->actor,FROG_ANIM_USINGTONGUE,NO,NO,0.5F,0,0);

				tongueState		= TONGUE_NONE | TONGUE_BEINGUSED | TONGUE_OUTGOING;
			}
		}
		else
		{
			tongueState = TONGUE_NONE | TONGUE_IDLE;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function		: CreateTongueSegment
	Purpose			: creates a tongue segment
	Parameters		: char
	Returns			: void
	Info			:
*/
void CreateTongueSegment(char idx)
{
	QUATERNION q;
	VECTOR tongueVector;

	float dist;
	float angle;
	
	// Determine the direction the tongue needs to travel in
	angle = tongueRotateStep * idx;

	q.x = tongueUpVector.v[X];
	q.y = tongueUpVector.v[Y];
	q.z = tongueUpVector.v[Z];
	q.w = angle;

	RotateVectorByRotation(&tongueVector,&ff,&q);

	// Determine how far we need to travel away from the tongue origin
	dist = tongueMagStep * idx;

	// Update tongue coordinates with new values
	tongueCoords[tongueCoordIndex].v[X] = tongueCoords[0].v[X] + (tongueVector.v[X] * dist);
	tongueCoords[tongueCoordIndex].v[Y] = tongueCoords[0].v[Y] + (tongueVector.v[Y] * dist);
	tongueCoords[tongueCoordIndex].v[Z] = tongueCoords[0].v[Z] + (tongueVector.v[Z] * dist);
	
	tongueCoordIndex++;
}


/*	--------------------------------------------------------------------------------
	Function		: RemoveTongueSegment
	Purpose			: removes a tongue segment
	Parameters		: char
	Returns			: void
	Info			: 
*/
void RemoveTongueSegment(char idx)
{
	dprintf"Remove tongue segment %d\n",idx));
//	SubSprite(&tongueSprite[tongueSegment]);
	tongueSprite[idx].scaleX = tongueSprite[idx].scaleX = 0;
}


/*	--------------------------------------------------------------------------------
	Function		: AddTongueSprite
	Purpose			: adds a tongue sprite
	Parameters		: short index,float,float,float
	Returns			: void
	Info			: 
*/
void AddTongueSprite(short index,float x,float y,float z)
{
	tongueSprite[index].texture		= txtrTongue;

	tongueSprite[index].pos.v[X]	= x;
	tongueSprite[index].pos.v[Y]	= y;
	tongueSprite[index].pos.v[Z]	= z;

	tongueSprite[index].anim.type	= SPRITE_ANIM_NONE;
	
	tongueSprite[index].scaleX		= 32;	//48 - (index << 2);
	tongueSprite[index].scaleY		= tongueSprite[index].scaleX;
	tongueSprite[index].r			= 255;
	tongueSprite[index].g			= 0;
	tongueSprite[index].b			= 0;
	tongueSprite[index].a			= 255;

#ifndef PC_VERSION
	tongueSprite[index].offsetX		= -tongueSprite[index].texture->sx / 2;
	tongueSprite[index].offsetY		= -tongueSprite[index].texture->sy / 2;
#else
	tongueSprite[index].offsetX		= -16;
	tongueSprite[index].offsetY		= -16;
#endif

	AddSprite(&tongueSprite[index],NULL);
}


/*	--------------------------------------------------------------------------------
	Function		: RemoveFrogTongue
	Purpose			: removes the frog tongue
	Parameters		: 
	Returns			: 
	Info			: 
*/
void RemoveFrogTongue()
{
	int i = MAX_TONGUENODES;

	dprintf"Remove TONGUE\n"));

	while(i--)
	{
//		RemoveTongueSegment(i);
		tongueSprite[i].scaleX = tongueSprite[i].scaleY = 0;
	}

	tongueCoordIndex	= 0;
	tongueSegment		= 0;
	numTongueNodes		= MAX_TONGUENODES;

	player[0].frogState &= ~FROGSTATUS_ISTONGUEING;
	tongueState = TONGUE_IDLE;
}

