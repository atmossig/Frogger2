/*
	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: specfx.c
	Programmer	: Andy Eder
	Date		: 14/05/99 09:31:42

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>
#include "incs.h"


FX_RIPPLELIST rippleFXList;
FX_SMOKELIST smokeFXList;
FX_EXPLODEPARTICLELIST explodeParticleFXList;

char doScreenFade	= 0;
char fadeDir		= FADE_OUT;
short fadeOut		= 255;
short fadeStep		= 4;

char pauseMode		= 0;
UBYTE testR			= 0;
UBYTE testG			= 0;
UBYTE testB			= 0;
UBYTE testA			= 170;


//----- [ TEXTURES USED FOR SPECIAL FX ] -----//

TEXTURE *txtrRipple		= NULL;
TEXTURE *txtrStar		= NULL;
TEXTURE *txtrSolidRing	= NULL;
TEXTURE *txtrSmoke		= NULL;
TEXTURE *txtrWaterDrop	= NULL;
TEXTURE *txtrBlastRing	= NULL;

/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddFXRipple
	Purpose			: creates and initialises a ripple based effect
	Parameters		: char,VECTOR *,VECTOR *,float,float,float,float
	Returns			: FX_RIPPLE *
	Info			: 
*/
FX_RIPPLE *CreateAndAddFXRipple(char rippleType,VECTOR *origin,VECTOR *normal,float size,float velocity,float acceleration,float lifetime)
{
	FX_RIPPLE *ripple;
	
	ripple = (FX_RIPPLE *)JallocAlloc(sizeof(FX_RIPPLE),YES,"FX_RIP");
	AddFXRipple(ripple);

	// set centre of ripple
	SetVector(&ripple->origin,origin);

	if(normal)
	{
		// use normal passed to function
		SetVector(&ripple->normal,normal);
	}
	else
	{
		// use camera vector
		SubVector(&ripple->normal,&currCamTarget[0],&currCamSource[0]);
		MakeUnit(&ripple->normal);
	}

	ripple->rippleType		= rippleType;
	ripple->deadCount		= 0;

	ripple->alpha			= 255;
	
	ripple->radius			= size;
	ripple->velocity		= velocity;
	ripple->accel			= acceleration;
	
	ripple->lifetime		= lifetime;

	ripple->r = 255;
	ripple->g = 255;
	ripple->b = 255;

	AddToVector(&ripple->origin,&ripple->normal);

	switch(rippleType)
	{
		case RIPPLE_TYPE_WATER:
			ripple->txtr = txtrRipple;
			break;
		
		case RIPPLE_TYPE_PICKUP:
			ripple->origin.v[X] += (ripple->normal.v[X] * 10);
			ripple->origin.v[Y] += (ripple->normal.v[Y] * 10);
			ripple->origin.v[Z] += (ripple->normal.v[Z] * 10);
			ripple->txtr = txtrStar;
			break;

		case RIPPLE_TYPE_RING:
		case RIPPLE_TYPE_TELEPORT:
		case RIPPLE_TYPE_CROAK:
		case RIPPLE_TYPE_BLASTRING:
			ripple->rippleType = RIPPLE_TYPE_BLASTRING;
			ripple->txtr = txtrBlastRing;
			ripple->alpha = 128;
			break;

		case RIPPLE_TYPE_SOLIDCROAK:
			ripple->alpha = 200;
			ripple->txtr = txtrSolidRing;
			break;
	}

	ripple->alphaSpeed		= ripple->alpha / lifetime;

	AddFXRipple(ripple);

	return ripple;
}


/*	--------------------------------------------------------------------------------
	Function		: FreeFXRippleLinkedList
	Purpose			: frees the fx linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeFXRippleLinkedList()
{
	FX_RIPPLE *cur,*next;

	if(rippleFXList.numEntries == 0)
		return;

	dprintf"Freeing linked list : FX_RIPPLE : (%d elements)\n",rippleFXList.numEntries));
	for(cur = rippleFXList.head.next; cur != &rippleFXList.head; cur = next)
	{
		next = cur->next;

		SubFXRipple(cur);
	}
}

/*	--------------------------------------------------------------------------------
	Function		: AddFXRipple
	Purpose			: adds a ripple fx type to the list
	Parameters		: FX_RIPPLE *
	Returns			: 
	Info			: 
*/
void AddFXRipple(FX_RIPPLE *ripple)
{
	if(ripple->next == NULL)
	{
		rippleFXList.numEntries++;
		ripple->prev = &rippleFXList.head;
		ripple->next = rippleFXList.head.next;
		rippleFXList.head.next->prev = ripple;
		rippleFXList.head.next = ripple;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: SubFXRipple
	Purpose			: removes a fx ripple from the list
	Parameters		: FX_RIPPLE *
	Returns			: 
	Info			: 
*/
void SubFXRipple(FX_RIPPLE *ripple)
{
	if(ripple->next == NULL)
		return;

	ripple->prev->next = ripple->next;
	ripple->next->prev = ripple->prev;
	ripple->next = NULL;
	rippleFXList.numEntries--;

	JallocFree((UBYTE **)&ripple);
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFXRipples
	Purpose			: updates the ripples based fx
	Parameters		: 
	Returns			: void
	Info			: 
*/
void UpdateFXRipples()
{
	FX_RIPPLE *ripple,*ripple2;
	short i;
	float temp;

	// go through ripple fx list and remove 'dead' effects
	for(ripple = rippleFXList.head.next; ripple != &rippleFXList.head; ripple = ripple2)
	{
		ripple2 = ripple->next;
		if(ripple->deadCount)
		{
			ripple->deadCount--;
			if(!ripple->deadCount)
			{
				SubFXRipple(ripple);
				continue;
			}
		}
	}

	// update ripple fx
	for(ripple = rippleFXList.head.next; ripple != &rippleFXList.head; ripple = ripple2)
	{
		ripple2 = ripple->next;

		if(ripple->lifetime)
		{
			ripple->lifetime--;

			ripple->alpha		-= ripple->alphaSpeed;
			ripple->velocity	+= ripple->accel;
			ripple->radius		+= ripple->velocity;

			if(!(ripple->lifetime) || (ripple->alpha < (ripple->alphaSpeed + 1)))
				ripple->deadCount = 5;
		}
	}
}



/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddFXSmoke
	Purpose			: creates and initialises a smoke based effect
	Parameters		: char,VECTOR *,VECTOR *,short,float
	Returns			: FX_SMOKE *
	Info			: 
*/
FX_SMOKE *CreateAndAddFXSmoke(VECTOR *origin,short size,float lifetime)
{
	FX_SMOKE *smoke;

	smoke = (FX_SMOKE *)JallocAlloc(sizeof(FX_SMOKE),YES,"FX_SMK");
	AddFXSmoke(smoke);

	SetVector(&smoke->sprite.pos,origin);

	smoke->alphaSpeed		= 255 / lifetime;
	smoke->lifetime			= lifetime;
	
	smoke->sprite.texture	= txtrSmoke;
	smoke->sprite.scaleX	= size;
	smoke->sprite.scaleY	= size;
	smoke->sprite.r			= 255;
	smoke->sprite.g			= 255;
	smoke->sprite.b			= 255;
	smoke->sprite.a			= 255;

	smoke->velocity.v[X]	= (-2 + Random(5)); //* up->v[X];
	smoke->velocity.v[Y]	= (Random(6) + 8); //* up->v[Y];
	smoke->velocity.v[Z]	= (-2 + Random(5));	//* up->v[Z];

#ifndef PC_VERSION
	smoke->sprite.offsetX	= -smoke->sprite.texture->sx / 2;
	smoke->sprite.offsetY	= -smoke->sprite.texture->sy / 2;
#else
	smoke->sprite.offsetX	= -16;
	smoke->sprite.offsetY	= -16;
#endif

	smoke->sprite.flags		= SPRITE_TRANSLUCENT;

	AddSprite(&smoke->sprite,NULL);

	return smoke;
}

/*	--------------------------------------------------------------------------------
	Function		: FreeFXSmokeLinkedList
	Purpose			: frees the fx linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeFXSmokeLinkedList()
{
	FX_SMOKE *cur,*next;

	if(smokeFXList.numEntries == 0)
		return;

	dprintf"Freeing linked list : FX_SMOKE : (%d elements)\n",smokeFXList.numEntries));
	for(cur = smokeFXList.head.next; cur != &smokeFXList.head; cur = next)
	{
		next = cur->next;

		SubFXSmoke(cur);
	}
}

/*	--------------------------------------------------------------------------------
	Function		: AddFXSmoke
	Purpose			: adds smoke fx element to the list
	Parameters		: FX_SMOKE *
	Returns			: void
	Info			: 
*/
void AddFXSmoke(FX_SMOKE *smoke)
{
	if(smoke->next == NULL)
	{
		smokeFXList.numEntries++;
		smoke->prev = &smokeFXList.head;
		smoke->next = smokeFXList.head.next;
		smokeFXList.head.next->prev = smoke;
		smokeFXList.head.next = smoke;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: SubFXSmoke
	Purpose			: removes a smoke fx element from the list
	Parameters		: FX_SMOKE *
	Returns			: void
	Info			: 
*/
void SubFXSmoke(FX_SMOKE *smoke)
{
	if(smoke->next == NULL)
		return;

	SubSprite(&smoke->sprite);

	smoke->prev->next = smoke->next;
	smoke->next->prev = smoke->prev;
	smoke->next = NULL;
	smokeFXList.numEntries--;

	JallocFree((UBYTE **)&smoke);
}

/*	--------------------------------------------------------------------------------
	Function		: UpdateFXSmoke
	Purpose			: updates the smoke based fx
	Parameters		: 
	Returns			: void
	Info			: 
*/
void UpdateFXSmoke()
{
	FX_SMOKE *smoke,*smoke2;

	// go through smoke fx list and remove 'dead' effects
	for(smoke = smokeFXList.head.next; smoke != &smokeFXList.head; smoke = smoke2)
	{
		smoke2 = smoke->next;
		if(smoke->deadCount)
		{
			smoke->deadCount--;
			if(!smoke->deadCount)
			{
				SubFXSmoke(smoke);
				continue;
			}
		}
	}

	// update smoke fx
	for(smoke = smokeFXList.head.next; smoke != &smokeFXList.head; smoke = smoke2)
	{
		smoke2 = smoke->next;

		if(smoke->lifetime)
		{
			smoke->lifetime--;

			smoke->sprite.a -= smoke->alphaSpeed;
			if(smoke->sprite.a < smoke->alphaSpeed)
			{
				smoke->lifetime	= 0;
				smoke->sprite.a = 0;
				smoke->deadCount = 5;
			}

			AddToVector(&smoke->sprite.pos,&smoke->velocity);
			smoke->velocity.v[X] *= 0.95;
			smoke->velocity.v[Y] *= 0.85;
			smoke->velocity.v[Z] *= 0.95;

			smoke->sprite.scaleX += 4;
			smoke->sprite.scaleY += 4;

			if(!smoke->lifetime)
				smoke->deadCount = 5;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddFXExplodeParticle
	Purpose			: creates and initialises a particle explosion based effect
	Parameters		: char,VECTOR *,VECTOR *,float,short,PLANE2 *,float
	Returns			: FX_EXPLODEPARTICLE *
	Info			: 
*/
FX_EXPLODEPARTICLE *CreateAndAddFXExplodeParticle(char explodeType,VECTOR *origin,VECTOR *normal,float maxSpeed,short size,PLANE2 *reboundPlane,float lifetime)
{
	int i = MAX_EXPLODE_PARTICLES;
	FX_EXPLODEPARTICLE *explode;
	float speed;
		
	explode = (FX_EXPLODEPARTICLE *)JallocAlloc(sizeof(FX_EXPLODEPARTICLE),YES,"FX_XPLTCLE");
	AddFXExplodeParticle(explode);

	// set up vectors for explosion
	SetVector(&explode->origin,origin);
	SetVector(&explode->normal,normal);

	SetVector(&explode->reboundPlane.point,&reboundPlane->point);
	SetVector(&explode->reboundPlane.normal,&reboundPlane->normal);

	explode->explodeType = explodeType;

	while(i--)
	{
		SetVector(&explode->sprite[i].pos,origin);
		explode->hasHitPlane[i]		= 0;
		explode->alphaDecay[i]		= Random(4) + 4;

		explode->sprite[i].r		= 255;
		explode->sprite[i].g		= 255;
		explode->sprite[i].b		= 255;

		explode->sprite[i].scaleX	= size;
		explode->sprite[i].scaleY	= size;

		if(explodeType == EXPLODEPARTICLE_TYPE_SMOKEBURST)
		{
			explode->sprite[i].texture	= txtrSmoke;
			explode->alphaDecay[i]		*= 2;
		}
		else if(explodeType == EXPLODEPARTICLE_TYPE_SPLASH)
		{
			explode->sprite[i].texture	= txtrWaterDrop;
		}
		else
		{
			explode->sprite[i].texture	= txtrStar;
		}

		explode->sprite[i].a		= 255;

		explode->sprite[i].offsetX	= -16;
		explode->sprite[i].offsetY	= -16;
		explode->sprite[i].flags	= SPRITE_TRANSLUCENT;
		
		AddSprite(&explode->sprite[i],NULL);

		// set up the explosion particle velocities, etc...
		speed = Random(8) + maxSpeed;
		explode->velocity[i].v[X]	= (speed * normal->v[X]) + (-2 + Random(4));
		explode->velocity[i].v[Y]	= (speed * normal->v[Y]) + (-2 + Random(4));
		explode->velocity[i].v[Z]	= (speed * normal->v[Z]) + (-2 + Random(4));

		explode->initialSpeed[i]	= speed;
	}

	explode->lifetime = lifetime;

	return explode;
}

/*	--------------------------------------------------------------------------------
	Function		: FreeFXSExplodeParticleLinkedList
	Purpose			: frees the fx linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeFXExplodeParticleLinkedList()
{
	FX_EXPLODEPARTICLE *cur,*next;

	if(explodeParticleFXList.numEntries == 0)
		return;

	dprintf"Freeing linked list : FX_EXPLODEPARTICLE : (%d elements)\n",explodeParticleFXList.numEntries));
	for(cur = explodeParticleFXList.head.next; cur != &explodeParticleFXList.head; cur = next)
	{
		next = cur->next;

		SubFXExplodeParticle(cur);
	}
}

/*	--------------------------------------------------------------------------------
	Function		: AddFXExplodeParticle
	Purpose			: adds explode particle fx element to the list
	Parameters		: FX_EXPLODEPARTICLE *
	Returns			: void
	Info			: 
*/
void AddFXExplodeParticle(FX_EXPLODEPARTICLE *explode)
{
	if(explode->next == NULL)
	{
		explodeParticleFXList.numEntries++;
		explode->prev = &explodeParticleFXList.head;
		explode->next = explodeParticleFXList.head.next;
		explodeParticleFXList.head.next->prev = explode;
		explodeParticleFXList.head.next = explode;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: SubFXSExplodeParticle
	Purpose			: removes an explode particle fx element from the list
	Parameters		: FX_EXPLODEPARTICLE *
	Returns			: void
	Info			: 
*/
void SubFXExplodeParticle(FX_EXPLODEPARTICLE *explode)
{
	int i = MAX_EXPLODE_PARTICLES;

	if(explode->next == NULL)
		return;

	while(i--)
		SubSprite(&explode->sprite[i]);

	explode->prev->next = explode->next;
	explode->next->prev = explode->prev;
	explode->next = NULL;
	explodeParticleFXList.numEntries--;

	JallocFree((UBYTE **)&explode);
}

/*	--------------------------------------------------------------------------------
	Function		: UpdateFXSExplodeParticle
	Purpose			: updates the explode particle based fx
	Parameters		: 
	Returns			: void
	Info			: 
*/
void UpdateFXExplodeParticle()
{
	int i;
	FX_EXPLODEPARTICLE *explode,*explode2;
	FX_RIPPLE *rip;
	float dist;

	// go through explode particle fx list and remove 'dead' effects
	for(explode = explodeParticleFXList.head.next; explode != &explodeParticleFXList.head; explode = explode2)
	{
		explode2 = explode->next;
		if(explode->deadCount)
		{
			explode->deadCount--;
			if(!explode->deadCount)
			{
				SubFXExplodeParticle(explode);
				continue;
			}
		}
	}

	// update explode particle fx
	for(explode = explodeParticleFXList.head.next; explode != &explodeParticleFXList.head; explode = explode2)
	{
		explode2 = explode->next;

		if(explode->lifetime)
		{
			explode->lifetime--;

			i = MAX_EXPLODE_PARTICLES;
			while(i--)
			{
				if(explode->hasHitPlane[i])
					continue;

				// update particle velocities
				explode->velocity[i].v[X] *= 0.95F;
				explode->velocity[i].v[Y] += -1.1F;
				explode->velocity[i].v[Z] *= 0.95F;

				explode->sprite[i].pos.v[X] += explode->velocity[i].v[X];
				explode->sprite[i].pos.v[Y] += explode->velocity[i].v[Y];
				explode->sprite[i].pos.v[Z] += explode->velocity[i].v[Z];

				explode->reboundPlane.J = -DotProduct(&explode->reboundPlane.point,&explode->reboundPlane.normal);
				dist = -(DotProduct(&explode->sprite[i].pos,&explode->reboundPlane.normal) + explode->reboundPlane.J);						

				// check if particle has hit (or passed through) the plane
				if(dist > 0)
				{
					SubFromVector(&explode->sprite[i].pos,&explode->velocity[i]);

					// check if this exploding particle type triggers some other effect or event
					if(explode->explodeType == EXPLODEPARTICLE_TYPE_SPLASH)
					{
						explode->hasHitPlane[i] = 1;
						explode->sprite[i].a = 1;

						rip = CreateAndAddFXRipple(RIPPLE_TYPE_BLASTRING,&explode->sprite[i].pos,&explode->reboundPlane.normal,1,0.1,0.1,Random(20)+10);
						rip->r = 255;
						rip->g = 255;
						rip->b = 255;
						continue;
					}
					else if(explode->explodeType == EXPLODEPARTICLE_TYPE_SMOKEBURST)
					{
						explode->hasHitPlane[i] = 1;
						explode->sprite[i].a = 1;
						continue;
					}
					else
					{
						CalcBounce(&explode->velocity[i],&explode->reboundPlane.normal);
					}
				}

				explode->sprite[i].a -= explode->alphaDecay[i];
				if(explode->sprite[i].a < 16)
				{
					explode->sprite[i].scaleX	= 0;
					explode->sprite[i].scaleY	= 0;
					explode->sprite[i].a		= 0;
					explode->hasHitPlane[i]		= 1;
				}

				// update specific effects
				if(explode->explodeType == EXPLODEPARTICLE_TYPE_SMOKEBURST)
				{
					explode->sprite[i].scaleX	+= 2;
					explode->sprite[i].scaleY	= explode->sprite[i].scaleX;
				}
			}

			if(!explode->lifetime)
			{
				i = MAX_EXPLODE_PARTICLES;
				while(i--)
					explode->sprite[i].a = 0;
				explode->deadCount = 5;
			}
		}
	}
}






/*	--------------------------------------------------------------------------------
	Function		: InitFXLists
	Purpose			: initialises the special fx lists and textures used
	Parameters		: 
	Returns			: void
	Info			: 
*/
void InitFXLinkedLists()
{
	// initialise the ripple fx list
	rippleFXList.numEntries = 0;
	rippleFXList.head.next = rippleFXList.head.prev = &rippleFXList.head;

	// initialise the smoke fx list
	smokeFXList.numEntries = 0;
	smokeFXList.head.next = smokeFXList.head.prev = &smokeFXList.head;

	// initialise the explode particle fx list
	explodeParticleFXList.numEntries = 0;
	explodeParticleFXList.head.next = explodeParticleFXList.head.prev = &explodeParticleFXList.head;

	// get the textures used for the various special effects
	FindTexture(&txtrRipple,UpdateCRC("ai_ripple.bmp"),YES);
	FindTexture(&txtrStar,UpdateCRC("ai_star.bmp"),YES);
	FindTexture(&txtrSolidRing,UpdateCRC("ai_circle.bmp"),YES);
	FindTexture(&txtrSmoke,UpdateCRC("ai_smoke.bmp"),YES);
	FindTexture(&txtrWaterDrop,UpdateCRC("watdrop.bmp"),YES);
	FindTexture(&txtrBlastRing,UpdateCRC("ai_blastring.bmp"),YES);
}

/*	--------------------------------------------------------------------------------
	Function		: UpdateSpecialFX
	Purpose			: updates the special fx
	Parameters		: 
	Returns			: void
	Info			: 
*/
void UpdateSpecialFX()
{
	if(rippleFXList.numEntries)
		UpdateFXRipples();

	if(smokeFXList.numEntries)
		UpdateFXSmoke();

	if(explodeParticleFXList.numEntries)
		UpdateFXExplodeParticle();
}


void CalcBounce(VECTOR *vel,VECTOR *normal)
{
	float NdotL;

	NdotL = -DotProduct(vel,normal);
	AddToOneScaledVector(vel,normal,NdotL * 2);
}

