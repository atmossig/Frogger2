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
		effect->r = 200;
		effect->g = 200;
		effect->b = 255;
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
		effect->spin = 0.15;
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
		effect->a = 200;
		
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
		effect->numP = 1;
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
	case FXTYPE_FLYSWARM:
		effect->numP = 6;
		i = effect->numP;

		effect->sprites = (SPRITE *)JallocAlloc( sizeof(SPRITE)*effect->numP, YES, "Sprites" );
		effect->particles = (PARTICLE *)JallocAlloc( sizeof(PARTICLE)*effect->numP, YES, "Particles" );

		effect->size = size;
		effect->r = 255;
		effect->g = 255;
		effect->b = 255;
		effect->a = 255;

		if( effect->type == FXTYPE_FLYSWARM )
			effect->tex = txtrFly;
		else
			effect->tex = txtrStar;

		while(i--)
		{
			effect->sprites[i].texture = effect->tex;
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
	case FXTYPE_SMOKE_STATIC:
	case FXTYPE_SMOKE_GROWS:
	case FXTYPE_BUBBLES:
		effect->lifetime = actFrameCount+life;
		effect->vel.v[X] = (-2 + Random(4))*speed;
		effect->vel.v[Y] = (Random(4) + 2)*speed;
		effect->vel.v[Z] = (-2 + Random(4))*speed;
		effect->fade = 180 / life;
		effect->size = size;

		effect->numP = 1;
		effect->sprites = (SPRITE *)JallocAlloc( sizeof(SPRITE), YES, "Sprite" );

		if( effect->type == FXTYPE_BUBBLES )
			effect->sprites->texture = txtrBubble;
		else
			effect->sprites->texture = txtrSmoke;

		SetVector( &effect->sprites->pos, &effect->origin );
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
	case FXTYPE_SPARKBURST:
	case FXTYPE_FLAMES:
	case FXTYPE_FIERYSMOKE:
		effect->numP = 10;
		i = effect->numP;

		effect->sprites = (SPRITE *)JallocAlloc( sizeof(SPRITE)*effect->numP, YES, "Sprites" );
		effect->particles = (PARTICLE *)JallocAlloc( sizeof(PARTICLE)*effect->numP, YES, "Particles" );

		effect->rebound = (PLANE2 *)JallocAlloc( sizeof(PLANE2), YES, "Rebound" );
		SetVector( &effect->rebound->point, &effect->origin );
		SetVector( &effect->rebound->normal, &effect->normal );

		if( effect->type == FXTYPE_SPLASH )
		{
			effect->tex = txtrBubble;
			effect->gravity = 1;
		}
		else if( effect->type == FXTYPE_SPARKBURST )
		{
			effect->tex = txtrSolidRing;
			effect->gravity = 1;
		}
		else if( effect->type == FXTYPE_FLAMES )
		{
			effect->tex = txtrFire;
			effect->gravity = 0;
		}
		else if( effect->type == FXTYPE_SMOKEBURST || effect->type == FXTYPE_FIERYSMOKE )
		{
			effect->tex = txtrSmoke;
			effect->gravity = 0;
		}

		effect->fade = 4;
		effect->speed = (float)Random(10) * speed;

		while(i--)
		{
			SetVector( &effect->sprites[i].pos, &effect->origin );

			effect->sprites[i].r = 255;
			effect->sprites[i].g = 255;
			effect->sprites[i].b = 255;
			effect->sprites[i].a = 255;

			effect->sprites[i].texture = effect->tex;

			effect->sprites[i].scaleX = size;
			effect->sprites[i].scaleY = size;

			effect->sprites[i].offsetX = -16;
			effect->sprites[i].offsetY = -16;
			effect->sprites[i].flags = SPRITE_TRANSLUCENT;
			
			AddSprite( &effect->sprites[i], NULL );

			if( effect->type == FXTYPE_SPARKBURST )
				effect->particles[i].bounce = 1;
			else
				effect->particles[i].bounce = 0;

			effect->particles[i].vel.v[X] = (effect->speed * effect->normal.v[X]) + (speed*(-2 + Random(4)));
			effect->particles[i].vel.v[Y] = (effect->speed * effect->normal.v[Y]) + (speed*(-2 + Random(4)));
			effect->particles[i].vel.v[Z] = (effect->speed * effect->normal.v[Z]) + (speed*(-2 + Random(4)));
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

		if( fx1->follow )
			SetVector( &fx1->origin, &fx1->follow->pos );

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

	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX(fx);
			return;
		}

	fo = fx->fade * gameSpeed;
	if( fx->a > fo ) fx->a -= fo;
	else fx->a = 0;

	fx->speed += fx->accn * gameSpeed;
	fx->size += fx->speed * gameSpeed;
	
	if( fx->type == FXTYPE_GARIBCOLLECT )
	{
		fx->angle += fx->spin * gameSpeed;
	}

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

	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX(fx);
			return;
		}

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
	VECTOR up;
	int i = fx->numP;
	float dist;

	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX( fx );
			return;
		}

	if( fx->type == FXTYPE_FROGSTUN )
	{
		SetVector( &up, &currTile[0]->normal );
		ScaleVector( &up, 20 );
		AddVector( &fx->origin, &up, &frog[0]->actor->pos );
	}

	while(i--)
	{
		if( fx->type == FXTYPE_FROGSTUN )
			if( fx->sprites[i].a > 7 ) fx->sprites[i].a -= 8;
			else fx->sprites[i].a = 0;

		if( fx->sprites[i].pos.v[X] > fx->origin.v[X])
			fx->particles[i].vel.v[X] -= max(gameSpeed/3, 1);
		else
			fx->particles[i].vel.v[X] += max(gameSpeed/3, 1);

		if( fx->sprites[i].pos.v[Y] > fx->origin.v[Y] )
			fx->particles[i].vel.v[Y] -= max(gameSpeed/3, 1);
		else
			fx->particles[i].vel.v[Y] += max(gameSpeed/3, 1);

		if( fx->sprites[i].pos.v[Z] > fx->origin.v[Z])
			fx->particles[i].vel.v[Z] -= max(gameSpeed/3, 1);
		else
			fx->particles[i].vel.v[Z] += max(gameSpeed/3, 1);

		if( fx->particles[i].vel.v[X] > gameSpeed*3 )
			fx->particles[i].vel.v[X] = gameSpeed*3;
		else if( fx->particles[i].vel.v[X] < -gameSpeed*3 )
			fx->particles[i].vel.v[X] = -gameSpeed*3;
		if( fx->particles[i].vel.v[Y] > gameSpeed*3 )
			fx->particles[i].vel.v[Y] = gameSpeed*3;
		else if( fx->particles[i].vel.v[Y] < -gameSpeed*3 )
			fx->particles[i].vel.v[Y] = -gameSpeed*3;
		if( fx->particles[i].vel.v[Z] > gameSpeed*3 )
			fx->particles[i].vel.v[Z] = gameSpeed*3;
		else if( fx->particles[i].vel.v[Z] < -gameSpeed*3 )
			fx->particles[i].vel.v[Z] = -gameSpeed*3;


		// Add velocity to local particle position
		AddToVector( &fx->particles[i].pos, &fx->particles[i].vel );
		// Add local particle pos to swarm origin to get world coords for sprite
		AddVector( &fx->sprites[i].pos, &fx->origin, &fx->particles[i].pos );

		if( fx->rebound )
		{
			fx->rebound->J = -DotProduct( &fx->rebound->point, &fx->rebound->normal );
			dist = -(DotProduct(&fx->sprites[i].pos, &fx->rebound->normal) + fx->rebound->J);

			if(dist > 0)
				CreateAndAddSpecialEffect( FXTYPE_BASICRING, &fx->sprites[i].pos, &fx->rebound->normal, 10, 1, 0.1, 0.3 );
		}
	}

	if( fx->type != FXTYPE_FLYSWARM )
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
	int i = fx->numP, fo;

	if( fx->deadCount)
		if( !(--fx->deadCount) )
		{
			SubSpecFX( fx );
			return;
		}

	while(i--)
	{
		if( fx->particles[i].bounce == 2 )
			continue;

		// update particle velocities
		fx->particles[i].vel.v[X] *= 0.95F;
		fx->particles[i].vel.v[Z] *= 0.95F;

		if( fx->gravity )
			fx->particles[i].vel.v[Y] += -1.1F;
		else
			fx->particles[i].vel.v[Y] *= 0.95F;

		fx->sprites[i].pos.v[X] += fx->particles[i].vel.v[X];
		fx->sprites[i].pos.v[Y] += fx->particles[i].vel.v[Y];
		fx->sprites[i].pos.v[Z] += fx->particles[i].vel.v[Z];

		if( fx->rebound )
		{
			fx->rebound->J = -DotProduct( &fx->rebound->point, &fx->rebound->normal );
			dist = -(DotProduct(&fx->sprites[i].pos, &fx->rebound->normal) + fx->rebound->J);

			// check if particle has hit (or passed through) the plane
			if(dist > 0)
			{
				SubFromVector( &fx->sprites[i].pos, &fx->particles[i].vel );

				if( fx->particles[i].bounce )
				{
					fx->particles[i].vel.v[Y] *= -0.95;
					AddToVector( &fx->sprites[i].pos, &fx->particles[i].vel );
				}

				// check if this exploding particle type triggers some other effect or event
				if( fx->type == FXTYPE_SPLASH )
				{
					fx->sprites[i].a = 1;

					CreateAndAddSpecialEffect( FXTYPE_BASICRING, &fx->sprites[i].pos, &fx->rebound->normal, 10, 1, 0.1, 0.3 );
					continue;
				}
			}
		}

		fo = (Random(4) + fx->fade) * gameSpeed ;

		// For fiery (of whatever colour) smoke, fade to black then fade out
		if( fx->type == FXTYPE_FIERYSMOKE && (fx->sprites[i].r || fx->sprites[i].g || fx->sprites[i].b) )
		{
			if( fx->sprites[i].r > fo ) fx->sprites[i].r -= fo;
			else fx->sprites[i].r = 0;
			if( fx->sprites[i].g > fo ) fx->sprites[i].g -= fo;
			else fx->sprites[i].g = 0;
			if( fx->sprites[i].b > fo ) fx->sprites[i].b -= fo;
			else fx->sprites[i].b = 0;
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
	unsigned long flags;
	PATH *path;

	if( type == 1 ) // Thing is an enemy
	{
		ENEMY *nme = (ENEMY *)entity;

		act = nme->nmeActor;
		SetVector( &normal, &nme->currNormal );
		tile = nme->inTile;
		flags = nme->flags;
		path = nme->path;
	}
	else
	{
		PLATFORM *plt = (PLATFORM *)entity;

		act = plt->pltActor;
		SetVector( &normal, &plt->currNormal );
		tile = plt->inTile;
		flags = plt->flags;
		path = plt->path;
	}

	if( act->value1 )
		r = Random(act->value1)+1;
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
					fx = CreateAndAddSpecialEffect( FXTYPE_BASICRING, &rPos, &normal, 30, 0.3, 0.1, 0.5 );
				else // Gentle ripples
					fx = CreateAndAddSpecialEffect( FXTYPE_BASICRING, &rPos, &normal, 40, 0.1, 0.1, 0.8 );

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
					fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_GROWS, &act->actor->pos, &normal, 32, 0.4, 0, 1.5 );
				else if( act->effects & EF_SLOW )
					fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_GROWS, &act->actor->pos, &normal, 32, 0.1, 0, 1.5 );
				else // EF_MEDIUM
					fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_GROWS, &act->actor->pos, &normal, 32, 0.2, 0, 1.5 );

				SetAttachedFXColour( fx, act->effects );
			}
			if( act->effects & EF_SPARKBURST )
			{
				if( act->effects & EF_FAST )
					fx = CreateAndAddSpecialEffect( FXTYPE_SPARKBURST, &act->actor->pos, &normal, 5, 1.5, 0, 0.5 );
				else if( act->effects & EF_SLOW )
					fx = CreateAndAddSpecialEffect( FXTYPE_SPARKBURST, &act->actor->pos, &normal, 5, 0.5, 0, 0.5 );
				else // EF_MEDIUM
					fx = CreateAndAddSpecialEffect( FXTYPE_SPARKBURST, &act->actor->pos, &normal, 5, 1.0, 0, 0.5 );

				SetVector( &fx->rebound->point, &tile->centre );
				SetVector( &fx->rebound->point, &tile->normal );

				SetAttachedFXColour( fx, act->effects );
			}
			if( act->effects & EF_SMOKEBURST )
			{
				if( act->effects & EF_FAST )
					fx = CreateAndAddSpecialEffect( FXTYPE_SMOKEBURST, &act->actor->pos, &normal, 50, 3, 0, 0.7 );
				else if( act->effects & EF_SLOW )
					fx = CreateAndAddSpecialEffect( FXTYPE_SMOKEBURST, &act->actor->pos, &normal, 50, 0.5, 0, 0.7 );
				else // EF_MEDIUM
					fx = CreateAndAddSpecialEffect( FXTYPE_SMOKEBURST, &act->actor->pos, &normal, 50, 1.5, 0, 0.7 );

				SetAttachedFXColour( fx, act->effects );
			}
			if( act->effects & EF_FIERYSMOKE )
			{
				if( act->effects & EF_FAST )
					fx = CreateAndAddSpecialEffect( FXTYPE_FIERYSMOKE, &act->actor->pos, &normal, 50, 2.5, 0, 2 );
				else if( act->effects & EF_SLOW )
					fx = CreateAndAddSpecialEffect( FXTYPE_FIERYSMOKE, &act->actor->pos, &normal, 50, 0.5, 0, 2 );
				else // EF_MEDIUM
					fx = CreateAndAddSpecialEffect( FXTYPE_FIERYSMOKE, &act->actor->pos, &normal, 50, 1.2, 0, 2 );

				SetAttachedFXColour( fx, act->effects );
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
