/*
	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: specfx.c
	Programmer	: Andy Eder
	Date		: 14/05/99 09:31:42

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>
#include "incs.h"


SPECFXLIST specFXList;

char doScreenFade	= 0;
char fadeDir		= FADE_OUT;
short fadeOut		= 255;
short fadeStep		= 4;

char pauseMode		= 0;

//----- [ TEXTURES USED FOR SPECIAL FX ] -----//


TEXTURE *txtrRipple		= NULL;
TEXTURE *txtrStar		= NULL;
TEXTURE *txtrSolidRing	= NULL;
TEXTURE *txtrSmoke		= NULL;
TEXTURE *txtrRing		= NULL;


void UpdateFXRipple( SPECFX *fx );
void UpdateFXSmoke( SPECFX *fx );
void UpdateFXSwarm( SPECFX *fx );
void UpdateFXExplode( SPECFX *fx );

/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddSpecialEffect
	Purpose			: creates and initialises a special effect, either barebones or of a custom type
	Parameters		: 
	Returns			: Created effect
	Info			: 
*/
SPECFX *CreateAndAddSpecialEffect( short type, VECTOR *origin, VECTOR *normal, int size, float speed, float accn, float lifetime )
{
	SPECFX *effect = (SPECFX *)JallocAlloc( sizeof(SPECFX), YES, "FX" );
	long i;
	float life = lifetime*60;

	effect->type = type;
	SetVector( &effect->origin, origin );
	SetVector( &effect->normal, normal );

	switch( type )
	{
	case FXTYPE_WATERRIPPLE:
		effect->r = 128;
		effect->g = 128;
		effect->b = 180;
		effect->a = 255;
		
		effect->size = size;
		effect->speed = speed;
		effect->accn = accn;
		effect->lifetime = actFrameCount + life;
		effect->fade = effect->a / life;

		AddToVector(&effect->origin,&effect->normal);

		effect->tex = txtrRipple;
		effect->Update = UpdateFXRipple;
		effect->Draw = DrawFXRipple;

		break;
	case FXTYPE_GARIBCOLLECT:
		effect->r = 255;
		effect->g = 255;
		effect->b = 0;
		effect->a = 255;
		
		effect->size = size;
		effect->speed = speed;
		effect->accn = accn;
		effect->lifetime = actFrameCount + life;
		effect->fade = effect->a / life;

		effect->origin.v[X] += (effect->normal.v[X] * 10);
		effect->origin.v[Y] += (effect->normal.v[Y] * 10);
		effect->origin.v[Z] += (effect->normal.v[Z] * 10);

		effect->tex = txtrStar;
		effect->Update = UpdateFXRipple;
		effect->Draw = DrawFXRipple;

		break;
	case FXTYPE_BASICRING:
		effect->r = 255;
		effect->g = 255;
		effect->b = 255;
		effect->a = 128;
		
		effect->size = size;
		effect->speed = speed;
		effect->accn = accn;
		effect->lifetime = actFrameCount + life;
		effect->fade = effect->a / life;

		AddToVector(&effect->origin,&effect->normal);

		effect->tex = txtrRing;
		effect->Update = UpdateFXRipple;
		effect->Draw = DrawFXRipple;
		break;
	case FXTYPE_JUMPBLUR:
		effect->sprites = (SPRITE *)JallocAlloc( sizeof(SPRITE), YES, "Sprite" );

		effect->size = size;
		SetVector( &effect->sprites->pos, &effect->origin );
		effect->sprites->texture = txtrSolidRing;
		effect->sprites->scaleX = effect->size;
		effect->sprites->scaleY = effect->size;
		effect->sprites->r = 100;
		effect->sprites->g = 255;
		effect->sprites->b = 100;
		effect->sprites->a = 200;

		effect->lifetime = actFrameCount + life;
		effect->fade = effect->sprites->a / life;

#ifndef PC_VERSION
		effect->sprites->offsetX = -effect->sprites->texture->sx / 2;
		effect->sprites->offsetY = -effect->sprites->texture->sy / 2;
#else
		effect->sprites->offsetX = -16;
		effect->sprites->offsetY = -16;
#endif

		effect->sprites->flags = SPRITE_TRANSLUCENT;

		AddSprite( effect->sprites, NULL );

		effect->Update = UpdateFXSmoke;
		effect->Draw = NULL;

		break;
	case FXTYPE_FROGSTUN:
		effect->numP = 6;
		i = effect->numP;

		effect->sprites = (SPRITE *)JallocAlloc( sizeof(SPRITE)*effect->numP, YES, "Sprites" );
		effect->particles = (PARTICLE *)JallocAlloc( sizeof(PARTICLE)*effect->numP, YES, "Particles" );

		effect->size = size;
		effect->r = 255;
		effect->g = 255;
		effect->b = 255;
		effect->a = 255;

		while(i--)
		{
			effect->sprites[i].texture = txtrStar;
			SetVector( &effect->sprites[i].pos, &effect->origin );
			SetVector( &effect->particles[i].pos, &zero );

			effect->sprites[i].scaleX = effect->size;
			effect->sprites[i].scaleY = effect->size;
			effect->sprites[i].r = effect->r;
			effect->sprites[i].g = effect->g;
			effect->sprites[i].b = effect->b;
			effect->sprites[i].a = effect->a;

			effect->sprites[i].offsetX	= -16;
			effect->sprites[i].offsetY	= -16;
			effect->sprites[i].flags = SPRITE_TRANSLUCENT;

			AddSprite( &effect->sprites[i], NULL );

			effect->particles[i].pos.v[X] = -8 + Random(16);
			effect->particles[i].pos.v[Y] = -6 + Random(12);
			effect->particles[i].pos.v[Z] = -8 + Random(16);
		}

		effect->lifetime = actFrameCount + life;
		effect->Update = UpdateFXSwarm;
		effect->Draw = NULL;
		
		break;
	case FXTYPE_EXHAUSTSMOKE:
		effect->lifetime = actFrameCount+life;
		effect->vel.v[X] = (-2 + Random(5));
		effect->vel.v[Y] = (Random(6) + 8);
		effect->vel.v[Z] = (-2 + Random(5));
		effect->fade = 255 / life;
		effect->size = size;
		
		effect->sprites = (SPRITE *)JallocAlloc( sizeof(SPRITE), YES, "Sprite" );
		effect->sprites->texture = txtrSmoke;
		effect->sprites->scaleX = effect->size;
		effect->sprites->scaleY = effect->size;
		effect->sprites->r = 255;
		effect->sprites->g = 255;
		effect->sprites->b = 255;
		effect->sprites->a = 255;

#ifndef PC_VERSION
		effect->sprites->offsetX = -effect->sprites->texture->sx / 2;
		effect->sprites->offsetY = -effect->sprites->texture->sy / 2;
#else
		effect->sprites->offsetX = -16;
		effect->sprites->offsetY = -16;
#endif
		effect->sprites->flags = SPRITE_TRANSLUCENT;

		AddSprite( effect->sprites, NULL );
		effect->Update = UpdateFXSmoke;
		effect->Draw = NULL;
		
		break;
	case FXTYPE_SMOKEBURST:
	case FXTYPE_SPLASH:
		effect->numP = 10;
		i = effect->numP;

		effect->sprites = (SPRITE *)JallocAlloc( sizeof(SPRITE)*effect->numP, YES, "Sprites" );
		effect->particles = (PARTICLE *)JallocAlloc( sizeof(PARTICLE)*effect->numP, YES, "Particles" );

		effect->rebound = (PLANE2 *)JallocAlloc( sizeof(PLANE2), YES, "Rebound" );
		SetVector( &effect->rebound->point, &effect->origin );
		SetVector( &effect->rebound->normal, &effect->normal );

		effect->fade = 4;
		effect->speed = Random(8) + speed;

		while(i--)
		{
			SetVector( &effect->sprites[i].pos, &effect->origin );

			effect->sprites[i].r = 255;
			effect->sprites[i].g = 255;
			effect->sprites[i].b = 255;
			effect->sprites[i].a = 255;

			effect->sprites[i].scaleX = size;
			effect->sprites[i].scaleY = size;

			if( effect->type == FXTYPE_SPLASH )
				effect->sprites[i].texture = txtrStar;
			else if( effect->type == FXTYPE_SMOKEBURST )
				effect->sprites[i].texture = txtrSmoke;

			effect->sprites[i].offsetX = -16;
			effect->sprites[i].offsetY = -16;
			effect->sprites[i].flags = SPRITE_TRANSLUCENT;
			
			AddSprite( &effect->sprites[i], NULL );

			effect->particles[i].vel.v[X] = (effect->speed * effect->normal.v[X]) + (-2 + Random(4));
			effect->particles[i].vel.v[Y] = (effect->speed * effect->normal.v[Y]) + (-2 + Random(4));
			effect->particles[i].vel.v[Z] = (effect->speed * effect->normal.v[Z]) + (-2 + Random(4));
		}

		effect->lifetime = actFrameCount + life;
		effect->Update = UpdateFXExplode;
		effect->Draw = NULL;
		
		break;
	default: // Custom type
		break;
	}

	AddSpecFX( effect );

	return effect;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateSpecialEffects
	Purpose			: Call update function for all effects in list
	Parameters		: 
	Returns			: 
	Info			: 
*/
void UpdateSpecialEffects( )
{
	SPECFX *fx1, *fx2;

	for( fx1 = specFXList.head.next; fx1 != &specFXList.head; fx1 = fx2 )
	{
		fx2 = fx1->next;

		if( fx1->Update )
			fx1->Update( fx1 );
	}
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFXRipples
	Purpose			: Grow and fade
	Parameters		: 
	Returns			: 
	Info			: 
*/
void UpdateFXRipple( SPECFX *fx )
{
	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX(fx);
			return;
		}

	fx->a -= fx->fade * gameSpeed;
	fx->speed += fx->accn;
	fx->size += fx->speed;

	if( (actFrameCount > fx->lifetime) && !fx->deadCount )
		fx->deadCount = 5;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFXSmoke
	Purpose			: updates the smoke based fx
	Parameters		: 
	Returns			: void
	Info			: 
*/
void UpdateFXSmoke( SPECFX *fx )
{
	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX(fx);
			return;
		}

	fx->sprites->a -= fx->fade * gameSpeed;

	AddToVector( &fx->sprites->pos,&fx->vel );
	fx->vel.v[X] *= 0.95;
	fx->vel.v[Y] *= 0.85;
	fx->vel.v[Z] *= 0.95;

	if( fx->type == FXTYPE_EXHAUSTSMOKE )
	{
		fx->sprites->scaleX += 4;
		fx->sprites->scaleY += 4;
	}
	else if( fx->type == FXTYPE_JUMPBLUR )
	{
		fx->sprites->scaleX--;
		fx->sprites->scaleY--;
	}

	if( (actFrameCount > fx->lifetime) && !fx->deadCount )
		fx->deadCount = 5;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFXSwarm
	Purpose			: updates the swarm based fx
	Parameters		: 
	Returns			: void
	Info			: 
*/
void UpdateFXSwarm( SPECFX *fx )
{
	int i = fx->numP;

	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX( fx );
			return;
		}

	if( fx->type == FXTYPE_FROGSTUN )
		SetVector( &fx->origin, &frog[0]->actor->pos );

	while(i--)
	{
		if( fx->type == FXTYPE_FROGSTUN )
			fx->sprites[i].a -= 8;

		if( fx->sprites[i].pos.v[X] > fx->origin.v[X])
			fx->particles[i].vel.v[X] -= 1.0F;
		else
			fx->particles[i].vel.v[X] += 1.0F;

		if( fx->sprites[i].pos.v[Y] > fx->origin.v[Y] )
			fx->particles[i].vel.v[Y] -= 1.0F;
		else
			fx->particles[i].vel.v[Y] += 1.0F;

		if( fx->sprites[i].pos.v[Z] > fx->origin.v[Z])
			fx->particles[i].vel.v[Z] -= 1.0F;
		else
			fx->particles[i].vel.v[Z] += 1.0F;

		fx->particles[i].pos.v[X] += fx->particles[i].vel.v[X];
		fx->particles[i].pos.v[Y] += fx->particles[i].vel.v[Y];
		fx->particles[i].pos.v[Z] += fx->particles[i].vel.v[Z];

		AddVector( &fx->sprites[i].pos, &fx->origin, &fx->particles[i].pos );
	}

	if( (actFrameCount > fx->lifetime) && !fx->deadCount )
		fx->deadCount = 5;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFXSExplodeParticle
	Purpose			: updates the explode particle based fx
	Parameters		: 
	Returns			: void
	Info			: 
*/
void UpdateFXExplode( SPECFX *fx )
{
	float dist;
	int i = fx->numP;
	SPECFX *ring;

	if( fx->deadCount)
		if( !(--fx->deadCount) )
		{
			SubSpecFX( fx );
			return;
		}

	while(i--)
	{
		if( fx->particles[i].bounce )
			continue;

		// update particle velocities
		fx->particles[i].vel.v[X] *= 0.95F;
		fx->particles[i].vel.v[Y] += -1.1F;
		fx->particles[i].vel.v[Z] *= 0.95F;

		fx->sprites[i].pos.v[X] += fx->particles[i].vel.v[X];
		fx->sprites[i].pos.v[Y] += fx->particles[i].vel.v[Y];
		fx->sprites[i].pos.v[Z] += fx->particles[i].vel.v[Z];

		fx->rebound->J = -DotProduct( &fx->rebound->point, &fx->rebound->normal );
		dist = -(DotProduct(&fx->sprites[i].pos, &fx->rebound->normal) + fx->rebound->J);						

		// check if particle has hit (or passed through) the plane
		if(dist > 0)
		{
			SubFromVector( &fx->sprites[i].pos, &fx->particles[i].vel );

			// check if this exploding particle type triggers some other effect or event
			if( fx->type == FXTYPE_SPLASH )
			{
				fx->sprites[i].a = 1;
				fx->particles[i].bounce = 1;

				ring = CreateAndAddSpecialEffect( FXTYPE_BASICRING, &fx->sprites[i].pos, &fx->rebound->normal, 10, 1, 0.1, 0.3 );
				continue;
			}
		}

		fx->sprites[i].a -= (Random(4) + fx->fade) * gameSpeed ;
		if( fx->sprites[i].a < 16 )
		{
			fx->sprites[i].scaleX	= 0;
			fx->sprites[i].scaleY	= 0;
			fx->sprites[i].a		= 0;
			fx->particles[i].bounce = 1;
		}
	}

	if( (actFrameCount > fx->lifetime) && !fx->deadCount )
		fx->deadCount = 5;
}


/*	--------------------------------------------------------------------------------
	Function		: FreeSpecFXList
	Purpose			: frees the fx linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void InitSpecFXList( )
{
	specFXList.head.next = specFXList.head.prev = &specFXList.head;
	specFXList.numEntries = 0;

	// get the textures used for the various special effects
	FindTexture(&txtrRipple,UpdateCRC("ai_ripple.bmp"),YES);
	FindTexture(&txtrStar,UpdateCRC("ai_star.bmp"),YES);
	FindTexture(&txtrSolidRing,UpdateCRC("ai_circle.bmp"),YES);
	FindTexture(&txtrSmoke,UpdateCRC("ai_smoke.bmp"),YES);
	FindTexture(&txtrRing,UpdateCRC("ai_ring.bmp"),YES);
}


/*	--------------------------------------------------------------------------------
	Function		: FreeFXRippleLinkedList
	Purpose			: frees the fx linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeSpecFXList()
{
	SPECFX *cur,*next;

	if( !specFXList.numEntries )
		return;

	for(cur = specFXList.head.next; cur != &specFXList.head; cur = next)
	{
		next = cur->next;
		SubSpecFX(cur);
	}
}

/*	--------------------------------------------------------------------------------
	Function		: AddSpecFX
	Purpose			: adds a special effect to the list
	Parameters		: 
	Returns			: 
	Info			: 
*/
void AddSpecFX( SPECFX *fx )
{
	if( !fx->next )
	{
		specFXList.numEntries++;
		fx->prev = &specFXList.head;
		fx->next = specFXList.head.next;
		specFXList.head.next->prev = fx;
		specFXList.head.next = fx;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: SubSpecFX
	Purpose			: removes a special effect from the list
	Parameters		: 
	Returns			: 
	Info			: 
*/
void SubSpecFX( SPECFX *fx )
{
	int i;

	if( !fx->next )
		return;

	fx->prev->next = fx->next;
	fx->next->prev = fx->prev;
	fx->next = NULL;
	specFXList.numEntries--;

	if( fx->sprites )
	{
		if( fx->numP )
			for( i=fx->numP; i; i-- )
				SubSprite( &fx->sprites[i-1] );
		else
			SubSprite( fx->sprites );

		JallocFree( (UBYTE **)&fx->sprites );
	}

	if( fx->particles )
		JallocFree( (UBYTE **)&fx->particles );

	if( fx->rebound )
		JallocFree( (UBYTE **)&fx->rebound );

	JallocFree((UBYTE **)&fx);
}


/*	--------------------------------------------------------------------------------
	Function		: CreateTeleportEffect
	Purpose			: Series of effect that are made when teleporting
	Parameters		: 
	Returns			: 
	Info			: 
*/
void CreateTeleportEffect( VECTOR *pos, VECTOR *normal )
{
	VECTOR telePos;
	SPECFX *fx;

	SetVector( &telePos, pos );
	fx = CreateAndAddSpecialEffect( FXTYPE_BASICRING, &telePos, &upVec, 30, 0, 0, 1 );
	telePos.v[Y] += 20;
	fx = CreateAndAddSpecialEffect( FXTYPE_BASICRING, &telePos, &upVec, 25, 0, 0, 1 );
	telePos.v[Y] += 40;
	fx = CreateAndAddSpecialEffect( FXTYPE_BASICRING, &telePos, &upVec, 20, 0, 0, 1 );
	telePos.v[Y] += 60;
	fx = CreateAndAddSpecialEffect( FXTYPE_BASICRING, &telePos, &upVec, 15, 0, 0, 1 );
	PlaySample(88,NULL,255,128);
}