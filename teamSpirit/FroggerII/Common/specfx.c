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
TEXTURE *txtrFly		= NULL;
TEXTURE *txtrBubble		= NULL;
TEXTURE *txtrFire		= NULL;
TEXTURE *txtrBlank		= NULL;


void UpdateFXRipple( SPECFX *fx );
void UpdateFXRing( SPECFX *fx );
void UpdateFXBolt( SPECFX *fx );
void UpdateFXSmoke( SPECFX *fx );
void UpdateFXSwarm( SPECFX *fx );
void UpdateFXExplode( SPECFX *fx );

void CreateBlastRing( );

// Used to store precalculated blast ring shape
#ifdef PC_VERSION
D3DTLVERTEX *ringVtx = NULL;
#endif

/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddSpecialEffect
	Purpose			: creates and initialises a special effect, either barebones or of a custom type
	Parameters		: 
	Returns			: Created effect
	Info			: 
*/
SPECFX *CreateAndAddSpecialEffect( short type, VECTOR *origin, VECTOR *normal, float size, float speed, float accn, float lifetime )
{
	SPECFX *effect = (SPECFX *)JallocAlloc( sizeof(SPECFX), YES, "FX" );
	long i;
	float life = lifetime*60;

	effect->type = type;
	SetVector( &effect->origin, origin );
	SetVector( &effect->normal, normal );
	effect->scale.v[X] = size;
	effect->scale.v[Y] = size;
	effect->scale.v[Z] = size;
	effect->speed = speed;
	effect->accn = accn;
	effect->lifetime = actFrameCount + life;
	effect->r = 255;
	effect->g = 255;
	effect->b = 255;
	effect->a = 255;

	switch( type )
	{
	case FXTYPE_WATERRIPPLE:
		effect->r = 200;
		effect->g = 200;
		effect->fade = effect->a / life;

		AddToVector(&effect->origin,&effect->normal);

		effect->tex = txtrRipple;
		effect->Update = UpdateFXRipple;
		effect->Draw = DrawFXRipple;

		break;
	case FXTYPE_GARIBCOLLECT:
		effect->b = 0;
		effect->spin = 0.15;
		effect->fade = effect->a / life;

		effect->origin.v[X] += (effect->normal.v[X] * 10);
		effect->origin.v[Y] += (effect->normal.v[Y] * 10);
		effect->origin.v[Z] += (effect->normal.v[Z] * 10);

		effect->tex = txtrStar;
		effect->Update = UpdateFXRipple;
		effect->Draw = DrawFXRipple;

		break;
	case FXTYPE_BASICRING:
		effect->a = 200;
		effect->fade = effect->a / life;

		AddToVector(&effect->origin,&effect->normal);

		effect->tex = txtrRing;
		effect->Update = UpdateFXRipple;
		effect->Draw = DrawFXRipple;

		break;
	case FXTYPE_POLYRING:
		if( !ringVtx )
			CreateBlastRing( );

		effect->fade = effect->a / life;

		AddToVector(&effect->origin,&effect->normal);

		effect->scale.v[Y] /= 8;
		effect->tilt = 0.9;

		effect->tex = txtrBlank;
		effect->Update = UpdateFXRing;
		effect->Draw = DrawFXRing;

		break;
	case FXTYPE_LASER:
		if( !ringVtx )
			CreateBlastRing( );

		effect->scale.v[X] = 3;
		effect->scale.v[Z] = 3;
		SetVector( &effect->vel, &effect->normal );
		ScaleVector( &effect->vel, effect->speed );
		effect->spin = 0.1;
		effect->tilt = 1;
		effect->a = 128;

		effect->tex = txtrBlank;
		effect->Update = UpdateFXBolt;
		effect->Draw = DrawFXRing;

		break;
	case FXTYPE_JUMPBLUR:
		effect->numP = 1;
		effect->sprites = (SPRITE *)JallocAlloc( sizeof(SPRITE), YES, "Sprite" );

		SetVector( &effect->sprites->pos, &effect->origin );
		effect->sprites->texture = txtrSolidRing;
		effect->sprites->scaleX = effect->scale.v[X];
		effect->sprites->scaleY = effect->scale.v[Y];
		effect->sprites->r = 100;
		effect->sprites->b = 100;
		effect->sprites->a = 200;
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
	case FXTYPE_FLYSWARM:
		effect->numP = 6;
		i = effect->numP;

		effect->sprites = (SPRITE *)JallocAlloc( sizeof(SPRITE)*effect->numP, YES, "Sprites" );
		effect->particles = (PARTICLE *)JallocAlloc( sizeof(PARTICLE)*effect->numP, YES, "Particles" );

		if( effect->type == FXTYPE_FLYSWARM )
			effect->tex = txtrFly;
		else
			effect->tex = txtrStar;

		while(i--)
		{
			effect->sprites[i].texture = effect->tex;
			SetVector( &effect->sprites[i].pos, &effect->origin );

			effect->sprites[i].scaleX = effect->scale.v[X];
			effect->sprites[i].scaleY = effect->scale.v[Y];
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

		effect->Update = UpdateFXSwarm;
		effect->Draw = NULL;
		break;
	case FXTYPE_BUTTERFLYSWARM:
		effect->numP = (int)lifetime; // Nasty Nasty Nasty
		i = effect->numP;

		effect->act = (ACTOR2 **)JallocAlloc( sizeof(ACTOR2 *)*effect->numP, YES, "Actor2s" );
		effect->particles = (PARTICLE *)JallocAlloc( sizeof(PARTICLE)*effect->numP, YES, "Particles" );

		while( i-- )
		{
			effect->act[i] = CreateAndAddActor( "bfly.obe", 0,0,0, INIT_ANIMATION, 0, 0 );
			if( effect->act[i]->actor->objectController )
				InitActorAnim( effect->act[i]->actor );

			AnimateActor( effect->act[i]->actor,0,YES,NO,1.0F, 0, 0);
			effect->act[i]->actor->scale.v[X] = effect->scale.v[X];
			effect->act[i]->actor->scale.v[Y] = effect->scale.v[Y];
			effect->act[i]->actor->scale.v[Z] = effect->scale.v[Z];

			effect->particles[i].pos.v[X] = -8 + Random(16);
			effect->particles[i].pos.v[Y] = -6 + Random(12);
			effect->particles[i].pos.v[Z] = -8 + Random(16);
		}

		effect->Update = UpdateFXSwarm;
		effect->Draw = NULL;

		break;
	case FXTYPE_SMOKE_STATIC:
	case FXTYPE_SMOKE_GROWS:
	case FXTYPE_BUBBLES:
		effect->vel.v[X] = (-2 + Random(4))*speed;
		effect->vel.v[Y] = (Random(4) + 2)*speed;
		effect->vel.v[Z] = (-2 + Random(4))*speed;
		effect->fade = 180 / life;

		effect->numP = 1;
		effect->sprites = (SPRITE *)JallocAlloc( sizeof(SPRITE), YES, "Sprite" );

		if( effect->type == FXTYPE_BUBBLES )
			effect->sprites->texture = txtrBubble;
		else
			effect->sprites->texture = txtrSmoke;

		SetVector( &effect->sprites->pos, &effect->origin );
		effect->sprites->scaleX = effect->scale.v[X];
		effect->sprites->scaleY = effect->scale.v[Y];

		effect->sprites->r = effect->r;
		effect->sprites->g = effect->g;
		effect->sprites->b = effect->b;
		effect->sprites->a = effect->a;

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
	case FXTYPE_SPARKBURST:
	case FXTYPE_FLAMES:
	case FXTYPE_FIERYSMOKE:
		effect->numP = 5;
		i = effect->numP;

		effect->sprites = (SPRITE *)JallocAlloc( sizeof(SPRITE)*effect->numP, YES, "Sprites" );
		effect->particles = (PARTICLE *)JallocAlloc( sizeof(PARTICLE)*effect->numP, YES, "Particles" );

		effect->rebound = (PLANE2 *)JallocAlloc( sizeof(PLANE2), YES, "Rebound" );
		SetVector( &effect->rebound->point, &effect->origin );
		SetVector( &effect->rebound->normal, &effect->normal );

		if( effect->type == FXTYPE_SPLASH )
			effect->tex = txtrBubble;
		else if( effect->type == FXTYPE_SPARKBURST )
			effect->tex = txtrSolidRing;
		else if( effect->type == FXTYPE_FLAMES )
			effect->tex = txtrFire;
		else if( effect->type == FXTYPE_SMOKEBURST || effect->type == FXTYPE_FIERYSMOKE )
			effect->tex = txtrSmoke;

		effect->fade = (255/life)*2;
		effect->speed = (float)Random(10) * speed;

		while(i--)
		{
			SetVector( &effect->sprites[i].pos, &effect->origin );

			effect->sprites[i].r = effect->r;
			effect->sprites[i].g = effect->g;
			effect->sprites[i].b = effect->b;
			effect->sprites[i].a = effect->a;

			if( effect->type == FXTYPE_FIERYSMOKE )
			{
				effect->sprites[i].g = 180;
				effect->sprites[i].b = 0;
			}

			effect->sprites[i].texture = effect->tex;

			effect->sprites[i].scaleX = effect->scale.v[X];
			effect->sprites[i].scaleY = effect->scale.v[Y];

			effect->sprites[i].offsetX = -16;
			effect->sprites[i].offsetY = -16;
			effect->sprites[i].flags = SPRITE_TRANSLUCENT;
			
			AddSprite( &effect->sprites[i], NULL );

			if( effect->type == FXTYPE_SPARKBURST )
				effect->particles[i].bounce = 1;
			else
				effect->particles[i].bounce = 0;

			effect->particles[i].vel.v[X] = (effect->speed * effect->normal.v[X]) + (speed*(-2 + Random(4)));
			effect->particles[i].vel.v[Y] = (effect->speed * effect->normal.v[Y]) + (speed*Random(2));
			effect->particles[i].vel.v[Z] = (effect->speed * effect->normal.v[Z]) + (speed*(-2 + Random(4)));
		}

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

		if( fx1->follow )
			SetVector( &fx1->origin, &fx1->follow->pos );

		if( fx1->deadCount )
			if( !(--fx1->deadCount) )
			{
				SubSpecFX(fx1);
				continue;
			}

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
	int fo;

	fo = fx->fade * gameSpeed;
	if( fx->a > fo ) fx->a -= fo;
	else fx->a = 0;

	fx->speed += fx->accn * gameSpeed;
	fx->scale.v[X] += fx->speed * gameSpeed;
	fx->scale.v[Z] += fx->speed * gameSpeed;
	fx->angle += fx->spin * gameSpeed;

	if( (actFrameCount > fx->lifetime) && !fx->deadCount )
		fx->deadCount = 5;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFXRing
	Purpose			: Grow and fade
	Parameters		: 
	Returns			: 
	Info			: 
*/
void UpdateFXRing( SPECFX *fx )
{
	int fo;
	float speed;

	fo = fx->fade * gameSpeed;
	if( fx->a > fo ) fx->a -= fo;
	else fx->a = 0;

	fx->speed += fx->accn * gameSpeed;
	speed = fx->speed * gameSpeed;
	fx->scale.v[X] += speed;
	fx->scale.v[Y] += speed/8;
	fx->scale.v[Z] += speed;
	fx->angle += fx->spin * gameSpeed;
	
	if( (actFrameCount > fx->lifetime) && !fx->deadCount )
		fx->deadCount = 5;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFXBolt
	Purpose			: Just move, not grow
	Parameters		: 
	Returns			: 
	Info			: 
*/
void UpdateFXBolt( SPECFX *fx )
{
	int fo;
	float accn = fx->accn * gameSpeed;

	fo = fx->fade * gameSpeed;
	if( fx->a > fo ) fx->a -= fo;
	else fx->a = 0;

	fx->vel.v[X] += accn;
	fx->vel.v[Y] += accn;
	fx->vel.v[Z] += accn;
	fx->origin.v[X] += fx->vel.v[X] * gameSpeed;
	fx->origin.v[Y] += fx->vel.v[Y] * gameSpeed;
	fx->origin.v[Z] += fx->vel.v[Z] * gameSpeed;
	fx->angle += fx->spin * gameSpeed;
	
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
	int fo;
	float dist;

	fo = fx->fade * gameSpeed;
	if( fx->sprites->a > fo ) fx->sprites->a -= fo;
	else
	{
		if( fx->type == FXTYPE_JUMPBLUR )
			fx->sprites->a = 255;
		else
			fx->sprites->a = 0;
	}

	fx->sprites->pos.v[X] += fx->vel.v[X] * gameSpeed;
	fx->sprites->pos.v[Y] += fx->vel.v[Y] * gameSpeed;
	fx->sprites->pos.v[Z] += fx->vel.v[Z] * gameSpeed;
	
	fx->vel.v[X] *= 0.95;
	fx->vel.v[Y] *= 0.85;
	fx->vel.v[Z] *= 0.95;

	if( fx->type == FXTYPE_SMOKE_GROWS )
	{
		fx->sprites->scaleX += gameSpeed;
		fx->sprites->scaleY += gameSpeed;
	}
	else if( fx->type == FXTYPE_JUMPBLUR )
	{
		fx->sprites->scaleX--;
		fx->sprites->scaleY--;
	}
	else if( fx->type == FXTYPE_BUBBLES )
	{
		if( fx->rebound )
		{
			fx->rebound->J = -DotProduct( &fx->rebound->point, &fx->rebound->normal );
			dist = -(DotProduct(&fx->sprites->pos, &fx->rebound->normal) + fx->rebound->J);

			if(dist < 0)
			{
				CreateAndAddSpecialEffect( FXTYPE_BASICRING, &fx->sprites->pos, &fx->rebound->normal, 5, 0.5, 0.1, 0.2 );
				JallocFree( (UBYTE **)&fx->rebound );
				fx->rebound = NULL;
			}
		}
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
	VECTOR up, pos;
	int i = fx->numP;
	float dist;

	if( fx->type == FXTYPE_FROGSTUN )
	{
		SetVector( &up, &currTile[0]->normal );
		ScaleVector( &up, 20 );
		AddVector( &fx->origin, &up, &frog[0]->actor->pos );
	}

	while(i--)
	{
		// Set world check position from either sprite or actor
		if( !fx->act )
			SetVector( &pos, &fx->sprites[i].pos );
		else
			SetVector( &pos, &fx->act[i]->actor->pos );

		// Fade out star stun
		if( fx->type == FXTYPE_FROGSTUN )
			if( fx->sprites[i].a > 7 ) fx->sprites[i].a -= 8;
			else fx->sprites[i].a = 0;

		// Update particle velocity to oscillate around the point
		if( pos.v[X] > fx->origin.v[X])
			fx->particles[i].vel.v[X] -= max(gameSpeed/3, 1);
		else
			fx->particles[i].vel.v[X] += max(gameSpeed/3, 1);
		if( pos.v[Y] > fx->origin.v[Y] )
			fx->particles[i].vel.v[Y] -= max(gameSpeed/3, 1);
		else
			fx->particles[i].vel.v[Y] += max(gameSpeed/3, 1);
		if( pos.v[Z] > fx->origin.v[Z])
			fx->particles[i].vel.v[Z] -= max(gameSpeed/3, 1);
		else
			fx->particles[i].vel.v[Z] += max(gameSpeed/3, 1);

		// Limit velocity of particles
		if( fx->particles[i].vel.v[X] > gameSpeed*2 )
			fx->particles[i].vel.v[X] = gameSpeed*2;
		else if( fx->particles[i].vel.v[X] < -gameSpeed*2 )
			fx->particles[i].vel.v[X] = -gameSpeed*2;
		if( fx->particles[i].vel.v[Y] > gameSpeed*2 )
			fx->particles[i].vel.v[Y] = gameSpeed*2;
		else if( fx->particles[i].vel.v[Y] < -gameSpeed*2 )
			fx->particles[i].vel.v[Y] = -gameSpeed*2;
		if( fx->particles[i].vel.v[Z] > gameSpeed*2 )
			fx->particles[i].vel.v[Z] = gameSpeed*2;
		else if( fx->particles[i].vel.v[Z] < -gameSpeed*2 )
			fx->particles[i].vel.v[Z] = -gameSpeed*2;

		// Add velocity to local particle position
		AddToVector( &fx->particles[i].pos, &fx->particles[i].vel );
		// Add local particle pos to swarm origin to get world coords for sprite or actor
		if( !fx->act )
		{
			AddVector( &fx->sprites[i].pos, &fx->origin, &fx->particles[i].pos );
		}
		else
		{
			AddVector( &fx->act[i]->actor->pos, &fx->origin, &fx->particles[i].pos );
		}

		if( !fx->act )
			SetVector( &pos, &fx->sprites[i].pos );
		else
			SetVector( &pos, &fx->act[i]->actor->pos );

		if( fx->rebound )
		{
			fx->rebound->J = -DotProduct( &fx->rebound->point, &fx->rebound->normal );
			dist = -(DotProduct(&pos, &fx->rebound->normal) + fx->rebound->J);

			if(dist > 0 && dist < 3.5)
				CreateAndAddSpecialEffect( FXTYPE_BASICRING, &pos, &fx->rebound->normal, 10, 1, 0.1, 0.3 );
		}
	}

	if( fx->type != FXTYPE_FLYSWARM && fx->type != FXTYPE_BUTTERFLYSWARM )
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
	int i = fx->numP, fo, ele;
	VECTOR up;

	while(i--)
	{
		if( fx->particles[i].bounce == 2 )
			continue;

		ScaleVector( &fx->particles[i].vel, 0.98 );

		if( fx->gravity != 0 )
		{
			SetVector( &up, &fx->normal );
			ScaleVector( &up, fx->gravity*gameSpeed );
			SubFromVector( &fx->particles[i].vel, &up );
		}

		if( fx->rebound )
		{
			fx->rebound->J = -DotProduct( &fx->rebound->point, &fx->rebound->normal );
			dist = -(DotProduct(&fx->sprites[i].pos, &fx->rebound->normal) + fx->rebound->J);

			// check if particle has hit (or passed through) the plane
			if(dist > 0)
			{
				if( fx->particles[i].bounce )
					fx->particles[i].vel.v[Y] *= -0.95; // Should be relative to the normal, but it'll do for now
				else
					SetVector( &fx->particles[i].vel, &zero );

				// check if this exploding particle type triggers some other effect or event
				if( fx->type == FXTYPE_SPLASH )
				{
					fx->sprites[i].a = 1;

					CreateAndAddSpecialEffect( FXTYPE_BASICRING, &fx->sprites[i].pos, &fx->rebound->normal, 10, 1, 0.1, 0.3 );
					continue;
				}
			}
		}

		AddToVector( &fx->sprites[i].pos, &fx->particles[i].vel );

		fo = (Random(4) + fx->fade) * gameSpeed ;

		// For fiery (of whatever colour) smoke, fade to black then fade out
		if( fx->type == FXTYPE_FIERYSMOKE && (fx->sprites[i].r || fx->sprites[i].g || fx->sprites[i].b) )
		{
			if( fx->sprites[i].r > fo/2 ) fx->sprites[i].r -= fo/2;
			else fx->sprites[i].r = 0;
			if( fx->sprites[i].g > fo ) fx->sprites[i].g -= fo;
			else fx->sprites[i].g = 0;
		}
		else
		{
			if( fx->sprites[i].a > fo ) fx->sprites[i].a -= fo;
			else fx->sprites[i].a = 0;

			if( fx->sprites[i].a < 16 )
			{
				fx->sprites[i].scaleX	= 0;
				fx->sprites[i].scaleY	= 0;
				fx->sprites[i].a		= 0;
				fx->particles[i].bounce = 2;
			}
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
	FindTexture(&txtrFly,UpdateCRC("fly1.bmp"),YES);
	FindTexture(&txtrBubble,UpdateCRC("watdrop.bmp"),YES);
	FindTexture(&txtrFire,UpdateCRC("ai_flame3.bmp"),YES);
	FindTexture(&txtrBlank,UpdateCRC("ai_fullwhite.bmp"),YES);
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

	if( fx->act )
	{
		for( i=fx->numP; i; i-- )
			SubActor(fx->act[i-1]);
		JallocFree( (UBYTE **)&fx->act );
	}

	JallocFree((UBYTE **)&fx);
}


/*	--------------------------------------------------------------------------------
	Function		: SetFXColour
	Purpose			: Sets the colour of the effect and any sprites
	Parameters		: 
	Returns			: 
	Info			: 
*/
void SetFXColour( SPECFX *fx, unsigned char r, unsigned char g, unsigned char b )
{
	int i;

	if( !fx ) return;

	fx->r = r;
	fx->g = g;
	fx->b = b;

	if( fx->sprites )
		for( i=0; i<fx->numP; i++ )
		{
			fx->sprites[i].r = r;
			fx->sprites[i].g = g;
			fx->sprites[i].b = b;
		}

	if( fx->particles )
		for( i=0; i<fx->numP; i++ )
		{
			fx->particles[i].r = r;
			fx->particles[i].g = g;
			fx->particles[i].b = b;
		}
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


/*	--------------------------------------------------------------------------------
	Function		: ProcessEnemyEffects
	Purpose			: Add special effects to the enemy
	Parameters		: 
	Returns			: void
	Info			: 
*/
void ProcessAttachedEffects( void *entity, int type )
{
	long r;
	VECTOR rPos, up, normal;
	SPECFX *fx = NULL;
	float fxDist;
	ACTOR2 *act;
	GAMETILE *tile;
	unsigned long flags, t;
	PATH *path;

	if( type == 1 ) // Thing is an enemy
	{
		ENEMY *nme = (ENEMY *)entity;

		act = nme->nmeActor;
		SetVector( &normal, &nme->inTile->normal );
		tile = nme->inTile;
		flags = nme->flags;
		path = nme->path;
	}
	else
	{
		PLATFORM *plt = (PLATFORM *)entity;

		act = plt->pltActor;
		SetVector( &normal, &plt->inTile->normal );
		tile = plt->inTile;
		flags = plt->flags;
		path = plt->path;
	}

	if( act->value1 )
	{
		if( flags & EF_RANDOMCREATE )
			r = Random(act->value1)+1;
		else
			r = act->value1;
	}
	else
		r = 10;

	fxDist = DistanceBetweenPointsSquared(&frog[0]->actor->pos,&act->actor->pos);

	if(fxDist < ACTOR_DRAWDISTANCEOUTER)
	{
		if( !(actFrameCount%r) )
		{
			if( act->effects & EF_RIPPLE_RINGS )
			{
				SetVector( &rPos, &act->actor->pos );
				rPos.v[Y] = tile->centre.v[Y];
				if( flags & ENEMY_NEW_FOLLOWPATH ) // More of a wake effect when moving
					fx = CreateAndAddSpecialEffect( FXTYPE_BASICRING, &rPos, &normal, 10, 0.3, 0.1, 0.5 );
				else // Gentle ripples
					fx = CreateAndAddSpecialEffect( FXTYPE_BASICRING, &rPos, &normal, 20, 0.1, 0.1, 0.8 );

				SetAttachedFXColour( fx, act->effects );
			}
			if( act->effects & EF_SMOKE_STATIC )
			{
				if( act->effects & EF_FAST )
					fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_STATIC, &act->actor->pos, &normal, 64, 0.4, 0, 1.5 );
				else if( act->effects & EF_SLOW )
					fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_STATIC, &act->actor->pos, &normal, 64, 0.1, 0, 1.5 );
				else // EF_MEDIUM
					fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_STATIC, &act->actor->pos, &normal, 64, 0.2, 0, 1.5 );

				SetAttachedFXColour( fx, act->effects );
			}
			if( act->effects & EF_SMOKE_GROWS )
			{
				if( act->effects & EF_FAST )
					fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_GROWS, &act->actor->pos, &normal, 42, 0.4, 0, 1.5 );
				else if( act->effects & EF_SLOW )
					fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_GROWS, &act->actor->pos, &normal, 42, 0.1, 0, 1.5 );
				else // EF_MEDIUM
					fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_GROWS, &act->actor->pos, &normal, 42, 0.2, 0, 1.5 );

				SetAttachedFXColour( fx, act->effects );
			}
			if( act->effects & EF_SPARKBURST )
			{
				if( act->effects & EF_FAST )
					fx = CreateAndAddSpecialEffect( FXTYPE_SPARKBURST, &act->actor->pos, &normal, 7, 2.0, 0, 2 );
				else if( act->effects & EF_SLOW )
					fx = CreateAndAddSpecialEffect( FXTYPE_SPARKBURST, &act->actor->pos, &normal, 7, 0.2, 0, 2 );
				else // EF_MEDIUM
					fx = CreateAndAddSpecialEffect( FXTYPE_SPARKBURST, &act->actor->pos, &normal, 7, 1.0, 0, 2 );

				SetVector( &fx->rebound->point, &tile->centre );
				SetVector( &fx->rebound->point, &tile->normal );
				fx->gravity = act->radius;

				SetAttachedFXColour( fx, act->effects );
			}
			if( act->effects & EF_SMOKEBURST )
			{
				if( act->effects & EF_FAST )
					fx = CreateAndAddSpecialEffect( FXTYPE_SMOKEBURST, &act->actor->pos, &normal, 50, 3, 0, 1.7 );
				else if( act->effects & EF_SLOW )
					fx = CreateAndAddSpecialEffect( FXTYPE_SMOKEBURST, &act->actor->pos, &normal, 50, 0.5, 0, 1.7 );
				else // EF_MEDIUM
					fx = CreateAndAddSpecialEffect( FXTYPE_SMOKEBURST, &act->actor->pos, &normal, 50, 1.5, 0, 1.7 );

				SetAttachedFXColour( fx, act->effects );
			}
			if( act->effects & EF_FIERYSMOKE )
			{
				if( act->effects & EF_FAST )
					fx = CreateAndAddSpecialEffect( FXTYPE_FIERYSMOKE, &act->actor->pos, &normal, 50, 1.0, 0, 2.5 );
				else if( act->effects & EF_SLOW )
					fx = CreateAndAddSpecialEffect( FXTYPE_FIERYSMOKE, &act->actor->pos, &normal, 50, 0.2, 0, 2.5 );
				else // EF_MEDIUM
					fx = CreateAndAddSpecialEffect( FXTYPE_FIERYSMOKE, &act->actor->pos, &normal, 50, 0.5, 0, 2.5 );

//				SetAttachedFXColour( fx, act->effects );
			}
			if( act->effects & EF_FLAMES )
			{
				if( act->effects & EF_FAST )
					fx = CreateAndAddSpecialEffect( FXTYPE_FLAMES, &act->actor->pos, &normal, 50, 2, 0, 0.7 );
				else if( act->effects & EF_SLOW )
					fx = CreateAndAddSpecialEffect( FXTYPE_FLAMES, &act->actor->pos, &normal, 50, 0.2, 0, 0.7 );
				else // EF_MEDIUM
					fx = CreateAndAddSpecialEffect( FXTYPE_FLAMES, &act->actor->pos, &normal, 50, 0.9, 0, 0.7 );

				SetAttachedFXColour( fx, act->effects );
			}
			if( act->effects & EF_BUBBLES )
			{
				if( act->effects & EF_FAST )
					fx = CreateAndAddSpecialEffect( FXTYPE_BUBBLES, &act->actor->pos, &normal, 8, 0.5, 0, 0.5 );
				else if( act->effects & EF_SLOW )
					fx = CreateAndAddSpecialEffect( FXTYPE_BUBBLES, &act->actor->pos, &normal, 8, 0.1, 0, 0.5 );
				else // EF_MEDIUM
					fx = CreateAndAddSpecialEffect( FXTYPE_BUBBLES, &act->actor->pos, &normal, 8, 0.3, 0, 0.5 );

				fx->rebound = (PLANE2 *)JallocAlloc( sizeof(PLANE2), YES, "Rebound" );
				SetVector( &up, &path->nodes[0].worldTile->normal );
				SetVector( &fx->rebound->normal, &up );
				ScaleVector( &up, act->radius );
				AddVector( &fx->rebound->point, &act->actor->pos, &up );

				SetAttachedFXColour( fx, act->effects );
			}
		}
		if( act->effects & EF_FLYSWARM )
		{
			fx = CreateAndAddSpecialEffect( FXTYPE_FLYSWARM, &act->actor->pos, &normal, 25, 0, 0, 0 );
			fx->follow = act->actor;
			if( type == 1 && (flags & ENEMY_NEW_FLAPPYTHING) )
			{
				fx->rebound = (PLANE2 *)JallocAlloc( sizeof(PLANE2), YES, "Rebound" );
				GetPositionForPathNode( &rPos, &path->nodes[0] );
				SetVector( &fx->rebound->point, &rPos );
				SetVector( &fx->rebound->normal, &path->nodes[0].worldTile->normal );
			}
			act->effects &= ~EF_FLYSWARM;
		}
		if( act->effects & EF_BUTTERFLYSWARM )
		{
			fx = CreateAndAddSpecialEffect( FXTYPE_BUTTERFLYSWARM, &act->actor->pos, &normal, act->radius, 0, 0, act->value1 );
			fx->follow = act->actor;
			if( type == 1 && (flags & ENEMY_NEW_FLAPPYTHING) )
			{
				fx->rebound = (PLANE2 *)JallocAlloc( sizeof(PLANE2), YES, "Rebound" );
				GetPositionForPathNode( &rPos, &path->nodes[0] );
				SetVector( &fx->rebound->point, &rPos );
				SetVector( &fx->rebound->normal, &path->nodes[0].worldTile->normal );
			}
			act->effects &= ~EF_BUTTERFLYSWARM;
		}
	}
}


void SetAttachedFXColour( SPECFX *fx, int effects )
{
	unsigned char r=60, g=60, b=60;

	if( effects & EF_TINTRED )
		r = 255;
	if( effects & EF_TINTGREEN )
		g = 255;
	if( effects & EF_TINTBLUE )
		b = 255;

	SetFXColour( fx, r, g, b );
}


/*	--------------------------------------------------------------------------------
	Function		: CreateBlastRing
	Purpose			: Make a generic ring of polys
	Parameters		: 
	Returns			: void
	Info			: 
*/
#ifdef PC_VERSION
void CreateBlastRing( )
{
	float tesa, tesb, teca, tecb, pB, arcStep = PI2 / NUM_RINGSEGS;
	unsigned long i, v;

	if( !ringVtx )
		ringVtx = (D3DTLVERTEX *)JallocAlloc(sizeof(D3DTLVERTEX)*NUM_RINGSEGS*4, NO, "D3DTLVERTEX");

	for( i=0; i<NUM_RINGSEGS; i++ )
	{
		pB = i*arcStep;
		v = i*4;

		tesa = sinf(pB);
		tesb = sinf(pB+arcStep);

		teca = cosf(pB);
		tecb = cosf(pB+arcStep);

		ringVtx[v+0].sx = tesa;
		ringVtx[v+0].sy = 0.5;
		ringVtx[v+0].sz = teca;
		ringVtx[v+0].color = D3DRGBA(1,1,1,1);
		ringVtx[v+0].specular = D3DRGB(0,0,0);
		ringVtx[v+0].tu = 1;
		ringVtx[v+0].tv = 1;
		ringVtx[v+1].sx = tesb;
		ringVtx[v+1].sy = 0.5;
		ringVtx[v+1].sz = tecb;
		ringVtx[v+1].color = D3DRGBA(1,1,1,1);
		ringVtx[v+1].specular = D3DRGB(0,0,0);
		ringVtx[v+1].tu = 0;
		ringVtx[v+1].tv = 1;
		ringVtx[v+2].sx = tesb;
		ringVtx[v+2].sy = -0.5;
		ringVtx[v+2].sz = tecb;
		ringVtx[v+2].color = D3DRGBA(1,1,1,1);
		ringVtx[v+2].specular = D3DRGB(0,0,0);
		ringVtx[v+2].tu = 0;
		ringVtx[v+2].tv = 0;
		ringVtx[v+3].sx = tesa;
		ringVtx[v+3].sy = -0.5;
		ringVtx[v+3].sz = teca;
		ringVtx[v+3].color = D3DRGBA(1,1,1,1);
		ringVtx[v+3].specular = D3DRGB(0,0,0);
		ringVtx[v+3].tu = 1;
		ringVtx[v+3].tv = 0;
	}
}
#endif