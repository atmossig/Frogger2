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

char pauseMode		= 0;

#define NUM_TRAIL_ELEMENTS 32

// Defines and data for the static sprite list
#define MAX_FXSPRITES	512
SPRITE *fxSpriteList = NULL;
short fxsCount=0;

// Defines and data for the sprite allocation stack
SPRITE *fxSpriteStack[MAX_FXSPRITES];
short fxsStackPtr=0;


//----- [ TEXTURES USED FOR SPECIAL FX ] -----//


TEXTURE *txtrRipple		= NULL;
TEXTURE *txtrStar		= NULL;
TEXTURE *txtrSolidRing	= NULL;
TEXTURE *txtrSmoke		= NULL;
TEXTURE *txtrSmoke2		= NULL;
TEXTURE *txtrRing		= NULL;
TEXTURE *txtrBubble		= NULL;
TEXTURE *txtrBlank		= NULL;
TEXTURE *txtrTrail		= NULL;
TEXTURE *txtrFlash		= NULL;
TEXTURE *txtrElectric	= NULL;
TEXTURE *txtrFlare		= NULL;


void UpdateFXRipple( SPECFX *fx );
void UpdateFXRing( SPECFX *fx );
void UpdateFXBolt( SPECFX *fx );
void UpdateFXSmoke( SPECFX *fx );
void UpdateFXSwarm( SPECFX *fx );
void UpdateFXExplode( SPECFX *fx );
void UpdateFXTrail( SPECFX *fx );
void UpdateFXLightning( SPECFX *fx );
void UpdateFXFly( SPECFX *fx );
void UpdateFXTwinkle( SPECFX *fx );

void CreateGloopEffects( SPECFX *parent );
void CreateBlastRing( );
void AddTrailElement( SPECFX *fx, int i );

SPRITE *AllocateSprites( int number );
void DeallocateSprites( SPRITE *head, int number );

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
	SPECFX *effect = NULL;
	SPRITE *s;
	VECTOR distance;
	long i,n;
	float life = lifetime * 60;

	SubVector( &distance, origin, &frog[0]->actor->pos );
	if( MagnitudeSquared(&distance) > ACTOR_DRAWDISTANCEOUTER )
		return NULL;

	effect = (SPECFX *)JallocAlloc( sizeof(SPECFX), YES, "FX" );

	effect->type = type;
	SetVector( &effect->origin, origin );
	SetVector( &effect->normal, normal );
	effect->scale.v[X] = size;
	effect->scale.v[Y] = size;
	effect->scale.v[Z] = size;
	effect->speed = speed;
	effect->accn = accn;
	effect->lifetime = actFrameCount + life;
	effect->startLife = lifetime;
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

		effect->origin.v[X] += (effect->normal.v[X] * 5);
		effect->origin.v[Y] += (effect->normal.v[Y] * 5);
		effect->origin.v[Z] += (effect->normal.v[Z] * 5);

		effect->tex = txtrStar;
		effect->Update = UpdateFXRipple;
		effect->Draw = DrawFXRipple;

		break;
	case FXTYPE_DECAL:
		effect->a = 200;
		effect->fade = effect->a / life;

		AddToVector(&effect->origin,&effect->normal);

		effect->tex = txtrRing;
		effect->Update = UpdateFXRipple;
		effect->Draw = DrawFXRipple;

		break;
	case FXTYPE_FROGSHIELD:
	case FXTYPE_POLYRING:
		if( !ringVtx )
			CreateBlastRing( );

		effect->fade = effect->a / life;

		AddToVector(&effect->origin,&effect->normal);

		effect->scale.v[Y] /= 8;
		effect->tilt = 0.9;

		if( effect->type == FXTYPE_FROGSHIELD )
			effect->tex = txtrFlash;
		else
			effect->tex = txtrBlank;

		effect->Update = UpdateFXRing;
		effect->Draw = DrawFXRing;

		break;

	case FXTYPE_LASER:
		if( !ringVtx )
			CreateBlastRing( );

		if( effect->type == FXTYPE_LASER )
		{
			effect->scale.v[X] = 2;
			effect->scale.v[Z] = 2;
			effect->tex = txtrBlank;
		}

		SetVector( &effect->vel, &effect->normal );
		ScaleVector( &effect->vel, effect->speed );
		effect->spin = 0.1;
		effect->tilt = 1;
		effect->a = 128;

		effect->Update = UpdateFXBolt;
		effect->Draw = DrawFXRing;

		break;
	case FXTYPE_TRAIL:
	case FXTYPE_BILLBOARDTRAIL:

#ifdef N64_VERSION
		break;		// TEMPORARY MEASURE - ANDYE
#endif

		effect->fade = effect->a / life;

		effect->numP = i = NUM_TRAIL_ELEMENTS;
		effect->particles = (PARTICLE *)JallocAlloc( sizeof(PARTICLE)*i, YES, "P" );
		effect->particles[0].bounce = 1;

		while( i-- )
		{
			effect->particles[i].poly = (VECTOR *)JallocAlloc( sizeof(VECTOR)*2, YES, "V" );
			effect->particles[i].a = (i+1)*(255/effect->numP);
			SetVector( &effect->particles[i].pos, &effect->origin );
			effect->particles[i].rMtrx = (float *)JallocAlloc( sizeof(float)*16, YES, "Mtx" );
		}

		effect->tex = txtrTrail;
		effect->Update = UpdateFXTrail;
		effect->Draw = DrawFXTrail;

		break;
	case FXTYPE_FROGSTUN:
		effect->numP = 6;
		i = effect->numP;

		if( !(effect->sprites = AllocateSprites( effect->numP )) )
		{
			SubSpecFX( effect );
			return NULL;
		}

		effect->particles = (PARTICLE *)JallocAlloc( sizeof(PARTICLE)*effect->numP, YES, "Particles" );

		effect->tex = txtrStar;

		s = effect->sprites;
		while( i-- )
		{
			s->texture = effect->tex;
			SetVector( &s->pos, &effect->origin );

			s->scaleX = effect->scale.v[X];
			s->scaleY = effect->scale.v[Y];
			s->r = effect->r;
			s->g = effect->g;
			s->b = effect->b;
			s->a = effect->a;

			s->offsetX	= -16;
			s->offsetY	= -16;
			s->flags = SPRITE_TRANSLUCENT;

			effect->particles[i].pos.v[X] = -8 + Random(16);
			effect->particles[i].pos.v[Y] = -6 + Random(12);
			effect->particles[i].pos.v[Z] = -8 + Random(16);
			s = s->next;
		}

		effect->Update = UpdateFXSwarm;
		effect->Draw = NULL;
		break;
	case FXTYPE_BATSWARM:
	case FXTYPE_BUTTERFLYSWARM:
	case FXTYPE_SPACETHING1:
		effect->numP = (int)lifetime; // Nasty Nasty Nasty
		i = effect->numP;

		effect->act = (ACTOR2 **)JallocAlloc( sizeof(ACTOR2 *)*effect->numP, YES, "Actor2s" );
		effect->particles = (PARTICLE *)JallocAlloc( sizeof(PARTICLE)*effect->numP, YES, "Particles" );

		while( i-- )
		{
			if( effect->type == FXTYPE_BATSWARM )
				effect->act[i] = CreateAndAddActor( "bat.obe", 0,0,0, INIT_ANIMATION);
			else if( effect->type == FXTYPE_BUTTERFLYSWARM )
				effect->act[i] = CreateAndAddActor( "bfly.obe", 0,0,0, INIT_ANIMATION);
			else if( effect->type == FXTYPE_SPACETHING1 )
			{
				SPECFX *fx;
				effect->act[i] = CreateAndAddActor( "xx_saus.obe", 0,0,0, INIT_ANIMATION);

				if( (fx = CreateAndAddSpecialEffect( FXTYPE_TRAIL, &effect->origin, &effect->normal, 2, 0.99, 0, 2 )) )
				{
					fx->follow = effect->act[i]->actor;
					SetFXColour( fx, 255, 100, 100 );
				}
			}

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
	case FXTYPE_HEALTHFLY:
		effect->numP = 1;
		effect->act = (ACTOR2 **)JallocAlloc( sizeof(ACTOR2 *), YES, "Actor2s" );

		effect->act[0] = CreateAndAddActor( "bfly.obe", 0,0,0, INIT_ANIMATION);

		if( effect->act[0]->actor->objectController )
			InitActorAnim( effect->act[0]->actor );

		AnimateActor( effect->act[0]->actor,0,YES,NO,1.0F, 0, 0);
		effect->act[0]->actor->scale.v[X] = effect->scale.v[X];
		effect->act[0]->actor->scale.v[Y] = effect->scale.v[Y];
		effect->act[0]->actor->scale.v[Z] = effect->scale.v[Z];

		effect->vel.v[X] = -8 + Random(16);
		effect->vel.v[Y] = -6 + Random(12);
		effect->vel.v[Z] = -8 + Random(16);

		AddVector( &effect->act[0]->actor->pos, &effect->origin, &effect->vel );

		SetVector( &effect->act[0]->actor->rotaim, &effect->normal );
		ScaleVector( &effect->act[0]->actor->rotaim, 50 );

		SetVector( &effect->act[0]->actor->vel, &inVec );
		ScaleVector( &effect->act[0]->actor->vel, effect->speed );

		effect->Update = UpdateFXFly;
		effect->Draw = NULL;

		break;
	case FXTYPE_SMOKE_STATIC:
	case FXTYPE_SMOKE_GROWS:
	case FXTYPE_BUBBLES:
		// Velocity is normal scaled by speed, plus a random offset scaled by speed
		SetVector( &effect->vel, &effect->normal );
		ScaleVector( &effect->vel, speed );
		effect->vel.v[X] += (Random(2)-1)*speed*0.2;
		effect->vel.v[Y] += (Random(2)-1)*speed*0.2;
		effect->vel.v[Z] += (Random(2)-1)*speed*0.2;
		effect->fade = 180 / life;

		if( effect->type == FXTYPE_BUBBLES )
			effect->numP = i = 1;
		else
			effect->numP = i = 2;

		if( !(effect->sprites = AllocateSprites( effect->numP )) )
		{
			SubSpecFX( effect );
			return NULL;
		}

		s = effect->sprites;
		while( i-- )
		{
			if( effect->type == FXTYPE_BUBBLES )
				s->texture = txtrBubble;
			else
				s->texture = txtrSmoke;

			SetVector( &s->pos, &effect->origin );
			s->scaleX = effect->scale.v[X] + Random(21)-10;
			s->scaleY = effect->scale.v[Y] + Random(21)-10;

			s->r = effect->r;
			s->g = effect->g;
			s->b = effect->b;
			s->a = effect->a;

			s->offsetX = -16;
			s->offsetY = -16;
			s->flags = SPRITE_TRANSLUCENT;

			if( effect->type == FXTYPE_SMOKE_GROWS || effect->type == FXTYPE_SMOKE_STATIC )
			{
				s->flags |= SPRITE_FLAGS_ROTATE;
				s->angle = 0.0f;
				s->angleInc = 1.0 / (float)(12 + (rand() % 16));
				if(i == 0)
					s->angleInc *= -1;
			}

			s = s->next;
		}

		effect->Update = UpdateFXSmoke;
		effect->Draw = NULL;
		
		break;
	case FXTYPE_SMOKEBURST:
	case FXTYPE_SPLASH:
	case FXTYPE_SPARKBURST:
	case FXTYPE_SPARKLYTRAIL:
	case FXTYPE_FIERYSMOKE:
		effect->numP = 5;
		i = effect->numP;

		if( !(effect->sprites = AllocateSprites( effect->numP )) )
		{
			SubSpecFX( effect );
			return NULL;
		}
		effect->particles = (PARTICLE *)JallocAlloc( sizeof(PARTICLE)*effect->numP, YES, "Particles" );

		effect->rebound = (PLANE2 *)JallocAlloc( sizeof(PLANE2), YES, "Rebound" );
		SetVector( &effect->rebound->point, &effect->origin );
		SetVector( &effect->rebound->normal, &effect->normal );

		if( effect->type == FXTYPE_SPLASH )
			effect->tex = txtrBubble;
		else if( effect->type == FXTYPE_SPARKBURST )
			effect->tex = txtrSolidRing;
		else if( effect->type == FXTYPE_SPARKLYTRAIL )
			effect->tex = txtrFlash;
		else if( effect->type == FXTYPE_SMOKEBURST || effect->type == FXTYPE_FIERYSMOKE )
			effect->tex = txtrSmoke;

		effect->fade = (255/life)*2;

		s = effect->sprites;
		while( i-- )
		{
			SetVector( &s->pos, &effect->origin );

			s->r = effect->r;
			s->g = effect->g;
			s->b = effect->b;
			s->a = effect->a;

			if( effect->type == FXTYPE_FIERYSMOKE )
			{
				s->g = 180;
				s->b = 0;
			}

			s->scaleX = effect->scale.v[X] + Random(21)-10;
			s->scaleY = effect->scale.v[Y] + Random(21)-10;

			s->offsetX = -16;
			s->offsetY = -16;
			s->flags = SPRITE_TRANSLUCENT;

			if( effect->type == FXTYPE_SPARKBURST || effect->type == FXTYPE_SPARKLYTRAIL )
				effect->particles[i].bounce = 1;
			else
				effect->particles[i].bounce = 0;

			// Velocity is normal scaled by speed, plus a random offset scaled by speed
			SetVector( &effect->particles[i].vel, &effect->normal );
			ScaleVector( &effect->particles[i].vel, effect->speed );
			effect->particles[i].vel.v[X] += (Random(2)-1)*effect->speed*0.3;
			effect->particles[i].vel.v[Y] += (Random(2)-1)*effect->speed*0.3;
			effect->particles[i].vel.v[Z] += (Random(2)-1)*effect->speed*0.3;

			if( (effect->type == FXTYPE_SMOKEBURST || effect->type == FXTYPE_FIERYSMOKE) && !(Random(4)) )
			{
				s->texture = (Random(2))?txtrSmoke:txtrSmoke2;
				s->flags |= SPRITE_FLAGS_ROTATE;
				s->angle = 0.0f;
				s->angleInc = 1.0 / (float)(8 + (rand() % 12));
				if(!(actFrameCount & 1))
					effect->sprites->angleInc *= -1;
			}
			else
			{
				s->texture = effect->tex;
			}

			s = s->next;
		}

		effect->Update = UpdateFXExplode;
		effect->Draw = NULL;
		
		break;
	case FXTYPE_LIGHTNING:
		effect->numP = speed/24;
		i = effect->numP;

		effect->particles = (PARTICLE *)JallocAlloc( sizeof(PARTICLE)*effect->numP, YES, "P" );
		effect->tex = txtrBlank;

		while(i--)
		{
			SetVector( &effect->particles[i].pos, &effect->origin );

			effect->particles[i].poly = (VECTOR *)JallocAlloc( sizeof(VECTOR)*2, YES, "V" );

			effect->particles[i].r = effect->r;
			effect->particles[i].g = effect->g;
			effect->particles[i].b = effect->b;
			effect->particles[i].a = effect->a;
		}

		effect->Update = UpdateFXLightning;
		effect->Draw = DrawFXLightning;
		break;

	case FXTYPE_GLOW:
	case FXTYPE_TWINKLE:
		effect->numP = 1;

		if( !(effect->sprites = AllocateSprites( effect->numP )) )
		{
			SubSpecFX( effect );
			return NULL;
		}

		if( effect->type == FXTYPE_GLOW )
			effect->sprites->texture = txtrFlare;
		else
			effect->sprites->texture = txtrFlash;

		effect->sprites->r = 255;
		effect->sprites->g = 255;
		effect->sprites->b = 255;
		effect->sprites->a = 128;

		effect->sprites->offsetX = -16;
		effect->sprites->offsetY = -16;
		effect->sprites->flags = SPRITE_TRANSLUCENT;

		effect->sprites->scaleX = size;
		effect->sprites->scaleY = size;
		SetVector( &effect->sprites->pos, &effect->origin );

		AddSprite( effect->sprites, NULL );

		effect->fade = effect->sprites->a / life;

		effect->Update = UpdateFXTwinkle;
		break;

	case FXTYPE_GREENGLOOP:
		// Create a bubble, but the effect persists after the bubbles are gone, to create a wait time
		// Also create several actual bubbles, but they do not spawn all the shit that this does
		SetVector( &effect->vel, &effect->normal );
		ScaleVector( &effect->vel, speed );
		effect->vel.v[X] += (-1 + Random(3))*speed*0.4;
		effect->vel.v[Y] += (-1 + Random(3))*speed*0.4;
		effect->vel.v[Z] += (-1 + Random(3))*speed*0.4;
		effect->fade = 255 / life;

		effect->numP = 1;
		if( !(effect->sprites = AllocateSprites( effect->numP )) )
		{
			SubSpecFX( effect );
			return NULL;
		}
		effect->sprites->texture = txtrBubble;

		SetVector( &effect->sprites->pos, &effect->origin );
		effect->sprites->scaleX = 22;
		effect->sprites->scaleY = 22;

		effect->sprites->r = effect->r;
		effect->sprites->g = effect->g;
		effect->sprites->b = effect->b;
		effect->sprites->a = effect->a;
		effect->sprites->offsetX = -16;
		effect->sprites->offsetY = -16;
		effect->sprites->flags = SPRITE_TRANSLUCENT;

		AddSprite( effect->sprites, NULL );

		effect->Update = UpdateFXSmoke;
		effect->Draw = NULL;

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
	int fo;

	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX(fx);
			return;
		}

	if( fx->follow )
		SetVector( &fx->origin, &fx->follow->pos );

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

	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX(fx);
			return;
		}

	if( fx->follow )
		SetVector( &fx->origin, &fx->follow->pos );

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

	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX(fx);
			return;
		}

	if( fx->follow )
		SetVector( &fx->origin, &fx->follow->pos );

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
	int fo, i = fx->numP;
	float dist, vS = 1;
	SPRITE *s;

	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			// Spawn some bubbles, some smoke and a splash
			if( fx->type == FXTYPE_GREENGLOOP )
				CreateGloopEffects( fx );

			SubSpecFX(fx);
			return;
		}

	if( fx->follow )
		SetVector( &fx->origin, &fx->follow->pos );
	
	s = fx->sprites;
	while(i--)
	{
		fo = fx->fade * gameSpeed;
		if( s->a > fo ) s->a -= fo;
		else s->a = 0;

		s->pos.v[X] += fx->vel.v[X] * gameSpeed;
		s->pos.v[Y] += fx->vel.v[Y] * gameSpeed;
		s->pos.v[Z] += fx->vel.v[Z] * gameSpeed;

		// Slow down gameSpeed times
		vS = 1-(0.02*gameSpeed);
		ScaleVector( &fx->vel, vS );

		if(s->flags & SPRITE_FLAGS_ROTATE)
			s->angle += (s->angleInc * gameSpeed);

		if( fx->type == FXTYPE_SMOKE_GROWS )
		{
			s->scaleX += fx->accn*gameSpeed;
			s->scaleY += fx->accn*gameSpeed;
		}
		else if( fx->type == FXTYPE_BUBBLES )
		{
			if( fx->rebound )
			{
				fx->rebound->J = -DotProduct( &fx->rebound->point, &fx->rebound->normal );
				dist = -(DotProduct(&s->pos, &fx->rebound->normal) + fx->rebound->J);

				if(dist > 0 && dist < 10)
				{
					CreateAndAddSpecialEffect( FXTYPE_DECAL, &s->pos, &fx->rebound->normal, 5, 0.4, 0.05, 0.3 );
					JallocFree( (UBYTE **)&fx->rebound );
					fx->rebound = NULL;
				}
			}
		}
		s = s->next;
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
	SPRITE *s;
	int i = fx->numP;
	float dist;

	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX(fx);
			return;
		}

	if( fx->follow )
		SetVector( &fx->origin, &fx->follow->pos );

	if( fx->type == FXTYPE_FROGSTUN )
	{
		SetVector( &up, &currTile[0]->normal );
		ScaleVector( &up, 20 );
		AddVector( &fx->origin, &up, &frog[0]->actor->pos );
	}

	s = fx->sprites;
	while(i--)
	{
		// Set world check position from either sprite or actor
		if( !fx->act )
			SetVector( &pos, &s->pos );
		else
			SetVector( &pos, &fx->act[i]->actor->pos );

		// Fade out star stun
		if( fx->type == FXTYPE_FROGSTUN )
			if( s->a > 7 ) s->a -= 8;
			else s->a = 0;

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
		if( fx->act )
		{
			AddVector( &fx->act[i]->actor->pos, &fx->origin, &fx->particles[i].pos );
			SetVector( &pos, &fx->act[i]->actor->pos );
		}
		else
		{
			AddVector( &s->pos, &fx->origin, &fx->particles[i].pos );
			SetVector( &pos, &s->pos );
			s = s->next;
		}

		if( fx->rebound )
		{
			fx->rebound->J = -DotProduct( &fx->rebound->point, &fx->rebound->normal );
			dist = -(DotProduct(&pos, &fx->rebound->normal) + fx->rebound->J);

			if(dist > 0 && dist < 3)
				CreateAndAddSpecialEffect( FXTYPE_DECAL, &pos, &fx->rebound->normal, 5, 0.5, 0.1, 0.3 );
		}
	}

	if( fx->type == FXTYPE_FROGSTUN )
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
	float dist, vS;
	int i = fx->numP, j, fo, ele;
	VECTOR up;
	SPRITE *s;

	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX(fx);
			return;
		}

	if( fx->follow )
		SetVector( &fx->origin, &fx->follow->pos );

	// Slow down gameSpeed times
	vS = 1-(0.02*gameSpeed);

	s = fx->sprites;
	while(i--)
	{
		if( fx->particles[i].bounce == 2 )
			continue;

		ScaleVector( &fx->particles[i].vel, vS );

		if( fx->gravity != 0 )
		{
			SetVector( &up, &fx->normal );
			ScaleVector( &up, fx->gravity*gameSpeed );
			SubFromVector( &fx->particles[i].vel, &up );
		}

		if( fx->rebound )
		{
			fx->rebound->J = -DotProduct( &fx->rebound->point, &fx->rebound->normal );
			dist = -(DotProduct(&s->pos, &fx->rebound->normal) + fx->rebound->J);

			// check if particle has hit (or passed through) the plane
			if(dist > 0)
			{
				if( fx->particles[i].bounce )
					fx->particles[i].vel.v[Y] *= -0.95; // Should be relative to the normal, but it'll do for now
				else
					SetVector( &fx->particles[i].vel, &zero );

				// check if this exploding particle type triggers some other effect or event
				if( fx->type == FXTYPE_SPLASH )
					if( dist < 5 )
						CreateAndAddSpecialEffect( FXTYPE_DECAL, &s->pos, &fx->rebound->normal, 5, 0.2, 0.1, 0.3 );
			}
		}

		if(s->flags & SPRITE_FLAGS_ROTATE)
			s->angle += (s->angleInc * gameSpeed);

		s->pos.v[X] += fx->particles[i].vel.v[X] * gameSpeed;
		s->pos.v[Y] += fx->particles[i].vel.v[Y] * gameSpeed;
		s->pos.v[Z] += fx->particles[i].vel.v[Z] * gameSpeed;

		fo = (Random(4) + fx->fade) * gameSpeed;

		// For fiery (of whatever colour) smoke, fade to black then fade out
		if( fx->type == FXTYPE_FIERYSMOKE && (s->r || s->g || s->b) )
		{
			if( s->r > fo/2 ) s->r -= fo/2;
			else s->r = 0;
			if( s->g > fo ) s->g -= fo;
			else s->g = 0;
		}
		else
		{
			if( s->a > fo ) s->a -= fo;
			else s->a = 0;

			if( s->a < 16 )
			{
				s->scaleX	= 0;
				s->scaleY	= 0;
				s->a		= 0;
				fx->particles[i].bounce = 2;
			}
		}

		s = s->next;
	}

	if( (actFrameCount > fx->lifetime) && !fx->deadCount )
		fx->deadCount = 5;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFXTrails
	Purpose			: 
	Parameters		: 
	Returns			: void
	Info			: 
*/
void UpdateFXTrail( SPECFX *fx )
{
	unsigned long i = fx->start;
	short diff;
	float fo;
	VECTOR dist;

	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX(fx);
			return;
		}

	if( fx->follow )
	{
		// Flag for first time through
		if( fx->particles[i].bounce )
		{
			fx->particles[i].bounce = 0;
			AddTrailElement( fx, i );
		}
		else 
		{
			// If space, add a new particle after end (wrapped)
			if( fx->end < fx->start ) 
				diff = fx->start - fx->end - 1;
			else
				diff = ((fx->numP-1)-fx->end)+fx->start;

			if( diff > 0 && fx->particles[(fx->end+1)%fx->numP].a < 16 )
			{
				if( ++fx->end >= fx->numP ) fx->end = 0;
				AddTrailElement( fx, fx->end );
			}
		}
	}

	// Update particles that are alive - fade, shrink by speed, move by velocity
	// Note - polys stored in object space
	do
	{
		ScaleVector( &fx->particles[i].poly[0], fx->speed );
		ScaleVector( &fx->particles[i].poly[1], fx->speed );
		AddToVector( &fx->particles[i].pos, &fx->particles[i].vel );

		if( ++i >= fx->numP ) i=0;

	} while( i != ((fx->end+1)%fx->numP) );

	// Fade out *all* particles, to get correct delay
	for( i=0; i<fx->numP; i++ )
	{
		fo = fx->fade * gameSpeed ;
		if( fx->particles[i].a > fo ) fx->particles[i].a -= fo;
		else fx->particles[i].a = 0;
	}

	// If oldest particle has died, exclude it from the list
	if( fx->particles[fx->start].a < 16 || DistanceBetweenPointsSquared(&fx->particles[fx->start].poly[0],&fx->particles[fx->start].poly[1])<2 )
	{
		fx->particles[fx->start].a = 0;
		if( ++fx->start >= fx->numP )
			fx->start = 0;
		// If no more particles in list, time to die
		if( (fx->start == fx->end-1) && !fx->follow && !fx->deadCount )
			fx->deadCount = 5;
	}
}

// Check the follow actor for quaternion and rotate the points about it. If no actor, don't add.
void AddTrailElement( SPECFX *fx, int i )
{
	float t;
	QUATERNION cross;
	VECTOR offset;

	fx->particles[i].r = fx->r;
	fx->particles[i].g = fx->g;
	fx->particles[i].b = fx->b;
	fx->particles[i].a = fx->a;
	fx->particles[i].poly[0].v[X] = fx->scale.v[X];
	fx->particles[i].poly[1].v[X] = -fx->scale.v[X];

	// Distance of this particle from the origin
	SubVector( &fx->particles[i].pos, &fx->follow->pos, &fx->origin );

	// Hack to make effect lower for platforms
	if( fx->gravity )
	{
		SetVector( &offset, &fx->normal );
		ScaleVector( (VECTOR *)&offset, fx->gravity );
		AddToVector( &fx->particles[i].pos, (VECTOR *)&offset );
	}

	// Amount of drift - distance between this and the last particle scaled. Doesn't work for first one
	if( i != fx->start )
	{
		int j = i-1;
		if( j < 0 ) j = fx->numP-1;
		SubVector( &fx->particles[i].vel, &fx->particles[i].pos, &fx->particles[j].pos );
		ScaleVector( &fx->particles[i].vel, fx->accn );
	}

	if( fx->type != FXTYPE_BILLBOARDTRAIL )
		QuaternionToMatrix( &fx->follow->qRot, (MATRIX *)fx->particles[i].rMtrx );
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFXLightning
	Purpose			: Emperors hands effect
	Parameters		: 
	Returns			: void
	Info			: 
*/
void UpdateFXLightning( SPECFX *fx )
{
	VECTOR target, aim, to;
	VECTOR ran, source, cross;
	float scale, fr, r;
	long i, h=fx->numP*0.25;

	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX(fx);
			return;
		}

	// Find out where the lightning is being fired at
	if( fx->follow )
	{
		SetVector( &target, &fx->follow->pos );
		SubVector( &aim, &target, &fx->origin );
	}
	else
	{
		SetVector( &aim, &fx->normal );
		ScaleVector( &aim, fx->speed );
		AddVector( &target, &aim, &fx->origin );
	}

	// Find a route through space
	for( i=0; i<fx->numP; i++ )
	{
		SetVector( &source, (!i)?(&fx->origin):(&fx->particles[i-1].pos) );
		scale = 1/(float)(fx->numP-i);
		fr = 1-((float)i/(float)fx->numP);
		// Get direction from last particle to target
		SubVector( &to, &target, &source );
		ScaleVector( &to, scale );

		// Get a random direction, and then modify by the "unit" aim direction (general movement in direction of target)
		ran.v[X] = Random(21)-10;
		ran.v[Y] = Random(21)-10;
		ran.v[Z] = Random(21)-10;

		MakeUnit( &ran );
		r = fx->accn * ( (i>fx->numP*0.5)?(fx->numP-i):(i) * Magnitude(&to) );
		ScaleVector( &ran, r );
		AddToVector( &to, &ran );
		AddVector( &fx->particles[i].pos, &source, &to );

		// Push the polys out from the position
		MakeUnit( &to );
		CrossProduct( &cross, &to, &upVec );
		MakeUnit( &cross );
		ScaleVector( &cross, fx->scale.v[X] );
		AddVector( &fx->particles[i].poly[0], &fx->particles[i].pos, &cross );
		SubVector( &fx->particles[i].poly[1], &fx->particles[i].pos, &cross );

		// Randomly fork a new lightning strand, but not if we're near the end or we're more than 2 layers of forking deep
/*		if( (Random(100)>(100-fx->tilt)) && (i<fx->numP-h && i>h) && fx->fade < 4 )
		{
			VECTOR dir;
			SPECFX *effect;
			SubVector( &dir, &fx->particles[i].pos, &source );
			MakeUnit( &dir );
			effect = CreateAndAddSpecialEffect( FXTYPE_LIGHTNING, &fx->particles[i].pos,
												&dir, fx->scale.v[X]-Random(3), fx->speed * min(fr*1.5, 1),
												fx->accn, fx->startLife * min(fr*1.5, 1) );
			effect->fade = ++fx->fade;
			SetFXColour( effect, fx->r, fx->g, fx->b );
		}
*/	}

	if( (actFrameCount > fx->lifetime) && !fx->deadCount )
		fx->deadCount = 5;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFXFly
	Purpose			: Only for health fles at the moment, but could be anything
	Parameters		: 
	Returns			: 
	Info			: Uses fx->vel as a local space position, which isn't nice.
*/
void UpdateFXFly( SPECFX *fx )
{
	VECTOR fwd, down;
	QUATERNION q1, q2, q3;
	float t, best = 10000000, speed;
	ACTOR *act = fx->act[0]->actor;
	unsigned long i;

	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX(fx);
			return;
		}

	// Randomly go to places
	if( DistanceBetweenPointsSquared( &act->rotaim, &fx->vel ) < 100 )
	{
		act->rotaim.v[X] = Random(50)-25;
		act->rotaim.v[Y] = Random(50)-25;
		act->rotaim.v[Z] = Random(50)-25;
	}

	SetQuaternion( &q1, &act->qRot );
	
	SubVector(&fwd,&act->rotaim,&fx->vel);
	MakeUnit(&fwd);

	CrossProduct((VECTOR *)&q3,&inVec,&fwd);
	MakeUnit( (VECTOR *)&q3 );
	t = DotProduct(&inVec,&fwd);
	if (t<-0.999)
		t=-0.999;
	if (t>0.999)
		t = 0.999;
	if(t<0.001 && t>-0.001)
		t = 0.1;
	q3.w=acos(t);
	GetQuaternionFromRotation(&q2,&q3);

	speed = fx->accn * gameSpeed;
	if( speed > 0.999 ) speed = 0.999;
	QuatSlerp( &q1, &q2, speed, &act->qRot );

	// Forward motion
	RotateVectorByQuaternion( &fwd, &inVec, &act->qRot );
	ScaleVector( &fwd, fx->speed*gameSpeed );
	AddToVector( &fx->vel, &fwd );

	// Gravity
	SetVector( &down, &fx->normal );
	ScaleVector( &down, fx->gravity );
	AddToVector( &fx->vel, &down );

	// World coordinates
	AddVector( &act->pos, &fx->vel, &fx->origin );
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFXTwinkle
	Purpose			: Glows around fire, candles etc and twinkles from gold
	Parameters		: 
	Returns			: 
	Info			: Need a method of specifying, not just affix to nmes
*/
void UpdateFXTwinkle( SPECFX *fx )
{
	int fo;

	if( fx->follow )
	{
		SetVector( &fx->origin, &fx->follow->pos );
		SetVector( &fx->sprites->pos, &fx->origin );
	}

	if( fx->type == FXTYPE_TWINKLE )
	{
		if( fx->deadCount )
			if( !(--fx->deadCount) )
			{
				SubSpecFX(fx);
				return;
			}

		fo = Random((int)fx->fade) * gameSpeed;
		if( fx->sprites->a > fo ) fx->sprites->a -= fo;
		else fx->sprites->a = 0;

		if( (actFrameCount > fx->lifetime) && !fx->deadCount )
			fx->deadCount = 5;
	}

	// Use tilt to specify twinkliness - urgh
	if( fx->tilt >= 2 )
	{
		int size = Random(5)-2;
		fx->sprites->scaleX += size;
		fx->sprites->scaleY += size;
		if( fx->sprites->scaleX > fx->scale.v[X]*2 ) fx->sprites->scaleX = fx->scale.v[X]*2;
		else if( fx->sprites->scaleX < fx->scale.v[X]*0.5 ) fx->sprites->scaleX = fx->scale.v[X]*0.5;
		if( fx->sprites->scaleY > fx->scale.v[Y]*2 ) fx->sprites->scaleY = fx->scale.v[Y]*2;
		else if( fx->sprites->scaleY < fx->scale.v[Y]*0.5 ) fx->sprites->scaleY = fx->scale.v[Y]*0.5;
	}
	if( fx->tilt )
	{
		fx->sprites->a += Random(50)-25;
		if( fx->sprites->a > 200 ) fx->sprites->a = 200;
		else if( fx->sprites->a < 50 ) fx->sprites->a = 50;
	}
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
	int i;

	specFXList.head.next = specFXList.head.prev = &specFXList.head;
	specFXList.numEntries = 0;

	// get the textures used for the various special effects
	FindTexture(&txtrRipple,UpdateCRC("ai_ripple.bmp"),YES);
	FindTexture(&txtrStar,UpdateCRC("ai_star.bmp"),YES);
	FindTexture(&txtrSolidRing,UpdateCRC("ai_circle.bmp"),YES);
	FindTexture(&txtrSmoke,UpdateCRC("ai_smoke.bmp"),YES);
	FindTexture(&txtrSmoke2,UpdateCRC("ai_smoke2.bmp"),YES);
	FindTexture(&txtrRing,UpdateCRC("ai_ring.bmp"),YES);
	FindTexture(&txtrBubble,UpdateCRC("watdrop.bmp"),YES);
	FindTexture(&txtrBlank,UpdateCRC("ai_fullwhite.bmp"),YES);
	FindTexture(&txtrTrail,UpdateCRC("ai_trail.bmp"),YES);
	FindTexture(&txtrFlash,UpdateCRC("ai_flash.bmp"),YES);
	FindTexture(&txtrElectric,UpdateCRC("ai_electric.bmp"),YES);
	FindTexture(&txtrFlare,UpdateCRC("ai_flare.bmp"),YES);

	// Allocate a big bunch of sprites
	if( !fxSpriteList ) fxSpriteList = (SPRITE *)JallocAlloc( sizeof(SPRITE)*MAX_FXSPRITES, YES, "FXSprites" );
	fxsCount = 0;

	// Initially, all sprites are available
	for( i=0; i<MAX_FXSPRITES; i++ )
		fxSpriteStack[i] = &fxSpriteList[i];

	fxsStackPtr = i-1;
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

	if( fxSpriteList )
	{
		int i;
		// Remove all sprites in array from sprite list so they don't get removed after deallocation
		for( i=0; i<MAX_FXSPRITES; i++ )
			SubSprite( &fxSpriteList[i] );

		JallocFree( (UBYTE **)&fxSpriteList );
		fxSpriteList = NULL;
		fxsCount = 0;
		fxsStackPtr = 0;
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
		DeallocateSprites( fx->sprites, fx->numP );

	if( fx->particles )
	{
		if(fx->numP)
		{
			i = fx->numP;
			while(i--)
			{
				JallocFree( (UBYTE **)&fx->particles[i].poly );
				JallocFree( (UBYTE **)&fx->particles[i].rMtrx );
			}
		}

		JallocFree( (UBYTE **)&fx->particles );
	}

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
	SPRITE *s;

	if( !fx ) return;

	fx->r = r;
	fx->g = g;
	fx->b = b;

	if( fx->sprites )
		for( i=0,s=fx->sprites; i<fx->numP; i++,s=s->next )
		{
			s->r = r;
			s->g = g;
			s->b = b;
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
void CreateTeleportEffect( VECTOR *pos, VECTOR *normal, unsigned char r, unsigned char g, unsigned char b )
{
	VECTOR telePos;
	SPECFX *fx;
	short i = 8;

	SetVector(&telePos,pos);
	while(i--)
	{
		fx = CreateAndAddSpecialEffect(FXTYPE_DECAL,&telePos,&upVec,30 - (i * 2),0,0,1);
		SetFXColour(fx,r,g,b);
		telePos.v[Y] += 5;
	}
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
	float r;
	VECTOR rPos, up, normal;
	SPECFX *fx = NULL;
	float fxDist;
	ACTOR2 *act;
	GAMETILE *tile;
	unsigned long flags, t;
	PATH *path;

#ifdef NO_ATTACHEDEFFECTS
	return;
#endif

	// Nasty casting - should use generic structure for enemies and platforms"
	if( type == ENTITY_ENEMY )
	{
		ENEMY *nme = (ENEMY *)entity;

		act = nme->nmeActor;
		SetVector( &normal, &nme->inTile->normal );
		tile = nme->inTile;
		flags = nme->flags;
		path = nme->path;
	}
	else if( type == ENTITY_PLATFORM )
	{
		PLATFORM *plt = (PLATFORM *)entity;

		act = plt->pltActor;
		SetVector( &normal, &plt->inTile[0]->normal );
		tile = plt->inTile[0];
		flags = plt->flags;
		path = plt->path;
	}

	fxDist = DistanceBetweenPointsSquared(&frog[0]->actor->pos,&act->actor->pos);

	if( fxDist < ACTOR_DRAWDISTANCEOUTER && actFrameCount > act->fxCount )
	{
		// Restart effect timer
		if( type == ENTITY_ENEMY && (flags & ENEMY_NEW_BABYFROG) )
			r = 57;
		else if( (int)act->value1 )
		{
			if( act->effects & EF_RANDOMCREATE )
				r = 60/(Random((int)act->value1)+1);
			else
				r = 60/act->value1;
		}
		else r = 60;

		act->fxCount = actFrameCount + r;

		if( act->effects & EF_RIPPLE_RINGS )
		{
			SetVector( &rPos, &act->actor->pos );
			rPos.v[Y] = tile->centre.v[Y];
			if( act->effects & EF_FAST )
				fx = CreateAndAddSpecialEffect( FXTYPE_DECAL, &rPos, &normal, 10, 0.3, 0.1, 0.5 );
			else if( act->effects & EF_SLOW )
				fx = CreateAndAddSpecialEffect( FXTYPE_DECAL, &rPos, &normal, 20, 0.1, 0.05, 0.5 );
			else
				fx = CreateAndAddSpecialEffect( FXTYPE_DECAL, &rPos, &normal, 20, 0.2, 0.1, 0.5 );

			SetAttachedFXColour( fx, act->effects );
		}
		if( act->effects & EF_SMOKE_STATIC )
		{
			if( act->effects & EF_FAST )
				fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_STATIC, &act->actor->pos, &normal, 64, 1.5, 0, 1.5 );
			else if( act->effects & EF_SLOW )
				fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_STATIC, &act->actor->pos, &normal, 64, 0.2, 0, 1.5 );
			else // EF_MEDIUM
				fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_STATIC, &act->actor->pos, &normal, 64, 0.8, 0, 1.5 );

			SetAttachedFXColour( fx, act->effects );
		}
		if( act->effects & EF_SMOKE_GROWS )
		{
			float gs = (path->nodes[path->fromNode].offset2==0)?1:path->nodes[path->fromNode].offset2;

			if( act->effects & EF_FAST )
				fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_GROWS, &act->actor->pos, &normal, 42, 1.5, gs, 1.5 );
			else if( act->effects & EF_SLOW )
				fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_GROWS, &act->actor->pos, &normal, 42, 0.2, gs, 1.5 );
			else // EF_MEDIUM
				fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_GROWS, &act->actor->pos, &normal, 42, 0.8, gs, 1.5 );

			SetAttachedFXColour( fx, act->effects );
		}
		if( act->effects & EF_SPARKLYTRAIL )
		{
			if( act->effects & EF_FAST )
				fx = CreateAndAddSpecialEffect( FXTYPE_SPARKLYTRAIL, &act->actor->pos, &normal, 20, 4, 0, 5 );
			else if( act->effects & EF_SLOW )
				fx = CreateAndAddSpecialEffect( FXTYPE_SPARKLYTRAIL, &act->actor->pos, &normal, 20, 0.5, 0, 5 );
			else // EF_MEDIUM
				fx = CreateAndAddSpecialEffect( FXTYPE_SPARKLYTRAIL, &act->actor->pos, &normal, 20, 2, 0, 5 );

			if( fx )
			{
				SetVector( &fx->rebound->point, &tile->centre );
				SetVector( &fx->rebound->normal, &tile->normal );
				fx->gravity = act->radius;

				SetAttachedFXColour( fx, act->effects );
			}
		}
		if( act->effects & EF_SPARKBURST )
		{
			if( act->effects & EF_FAST )
				fx = CreateAndAddSpecialEffect( FXTYPE_SPARKBURST, &act->actor->pos, &normal, 10, 4, 0, 5 );
			else if( act->effects & EF_SLOW )
				fx = CreateAndAddSpecialEffect( FXTYPE_SPARKBURST, &act->actor->pos, &normal, 10, 0.5, 0, 5 );
			else // EF_MEDIUM
				fx = CreateAndAddSpecialEffect( FXTYPE_SPARKBURST, &act->actor->pos, &normal, 10, 2, 0, 5 );

			if( fx )
			{
//				SetVector( &fx->rebound->point, &tile->centre );
//				SetVector( &fx->rebound->normal, &tile->normal );
				fx->gravity = act->radius;

				SetAttachedFXColour( fx, act->effects );
			}
		}
		if( act->effects & EF_SMOKEBURST )
		{
			if( act->effects & EF_FAST )
				fx = CreateAndAddSpecialEffect( FXTYPE_SMOKEBURST, &act->actor->pos, &normal, 50, 5, 0, 1.7 );
			else if( act->effects & EF_SLOW )
				fx = CreateAndAddSpecialEffect( FXTYPE_SMOKEBURST, &act->actor->pos, &normal, 50, 0.6, 0, 1.7 );
			else // EF_MEDIUM
				fx = CreateAndAddSpecialEffect( FXTYPE_SMOKEBURST, &act->actor->pos, &normal, 50, 2, 0, 1.7 );

			SetAttachedFXColour( fx, act->effects );
		}
		if( act->effects & EF_FIERYSMOKE )
		{
			if( act->effects & EF_FAST )
				fx = CreateAndAddSpecialEffect( FXTYPE_FIERYSMOKE, &act->actor->pos, &normal, 50, 4, 0, 2.5 );
			else if( act->effects & EF_SLOW )
				fx = CreateAndAddSpecialEffect( FXTYPE_FIERYSMOKE, &act->actor->pos, &normal, 50, 1, 0, 2.5 );
			else // EF_MEDIUM
				fx = CreateAndAddSpecialEffect( FXTYPE_FIERYSMOKE, &act->actor->pos, &normal, 50, 2.5, 0, 2.5 );

//				SetAttachedFXColour( fx, act->effects );
		}
		if( act->effects & EF_BUBBLES )
		{
			if( act->effects & EF_FAST )
				fx = CreateAndAddSpecialEffect( FXTYPE_BUBBLES, &act->actor->pos, &normal, 8, 1.5, 0, 0.5 );
			else if( act->effects & EF_SLOW )
				fx = CreateAndAddSpecialEffect( FXTYPE_BUBBLES, &act->actor->pos, &normal, 8, 0.3, 0, 0.5 );
			else // EF_MEDIUM
				fx = CreateAndAddSpecialEffect( FXTYPE_BUBBLES, &act->actor->pos, &normal, 8, 0.7, 0, 0.5 );

			if( fx )
			{
				fx->rebound = (PLANE2 *)JallocAlloc( sizeof(PLANE2), YES, "Rebound" );
				SetVector( &up, &path->nodes[0].worldTile->normal );
				SetVector( &fx->rebound->normal, &up );
				ScaleVector( &up, act->radius );
				AddVector( &fx->rebound->point, &act->actor->pos, &up );

				SetAttachedFXColour( fx, act->effects );
			}
		}
		if( act->effects & EF_TWINKLE )
		{
			fx = CreateAndAddSpecialEffect( FXTYPE_TWINKLE, &act->actor->pos, &normal, 20, 0, 0, Random(3)+1 );
			SetAttachedFXColour( fx, act->effects );
			fx->follow = act->actor;

			if( act->effects & EF_FAST )
				fx->tilt = 3;
			else if( act->effects & EF_MEDIUM )
				fx->tilt = 2;
			else if( act->effects & EF_SLOW )
				fx->tilt = 1;
		}
		if( act->effects & EF_GREENGLOOP )
		{
			if( act->effects & EF_FAST )
				fx = CreateAndAddSpecialEffect( FXTYPE_GREENGLOOP, &act->actor->pos, &normal, act->radius, 1.5, 0, 0.5 );
			else if( act->effects & EF_SLOW )
				fx = CreateAndAddSpecialEffect( FXTYPE_GREENGLOOP, &act->actor->pos, &normal, act->radius, 0.3, 0, 0.5 );
			else // EF_MEDIUM
				fx = CreateAndAddSpecialEffect( FXTYPE_GREENGLOOP, &act->actor->pos, &normal, act->radius, 0.7, 0, 0.5 );

			if( fx )
			{
				fx->rebound = (PLANE2 *)JallocAlloc( sizeof(PLANE2), YES, "Rebound" );
				SetVector( &up, &path->nodes[0].worldTile->normal );
				SetVector( &fx->rebound->normal, &up );
				ScaleVector( &up, act->radius );
				AddVector( &fx->rebound->point, &act->actor->pos, &up );

				SetAttachedFXColour( fx, act->effects );
			}
		}
	}

	if( act->effects & EF_GLOW )
	{
		fx = CreateAndAddSpecialEffect( FXTYPE_GLOW, &act->actor->pos, &normal, 120, 0, 0, 0 );
		SetAttachedFXColour( fx, act->effects );
		fx->follow = act->actor;

		if( act->effects & EF_FAST )
			fx->tilt = 2;
		else if( act->effects & EF_MEDIUM )
			fx->tilt = 1;
		else if( act->effects & EF_SLOW )
			fx->tilt = 0;

		if( fx ) act->effects &= ~EF_GLOW;
	}

	if( (act->effects & EF_BUTTERFLYSWARM) || (act->effects & EF_BATSWARM) || (act->effects & EF_SPACETHING1) )
	{
		if( act->effects & EF_BATSWARM )
		{
			fx = CreateAndAddSpecialEffect( FXTYPE_BATSWARM, &act->actor->pos, &normal, act->radius, 0, 0, act->value1 );
			act->effects &= ~EF_BATSWARM;
		}
		else if( act->effects & EF_BUTTERFLYSWARM )
		{
			fx = CreateAndAddSpecialEffect( FXTYPE_BUTTERFLYSWARM, &act->actor->pos, &normal, act->radius, 0, 0, act->value1 );
			act->effects &= ~EF_BUTTERFLYSWARM;
			if( fx && type == ENTITY_ENEMY && (flags & ENEMY_NEW_FLAPPYTHING) )
			{
				fx->rebound = (PLANE2 *)JallocAlloc( sizeof(PLANE2), YES, "Rebound" );
				GetPositionForPathNode( &rPos, &path->nodes[0] );
				SetVector( &fx->rebound->point, &rPos );
				SetVector( &fx->rebound->normal, &path->nodes[0].worldTile->normal );
			}
		}
		else
		{
			fx = CreateAndAddSpecialEffect( FXTYPE_SPACETHING1, &act->actor->pos, &normal, act->radius, 0, 0, act->value1 );
			act->effects &= ~EF_SPACETHING1;
		}

		if( fx ) fx->follow = act->actor;
	}

	if( act->effects & EF_TRAIL )
	{
		if( act->effects & EF_FAST )
			fx = CreateAndAddSpecialEffect( FXTYPE_TRAIL, &act->actor->pos, &normal, act->value1, 0.95, 0.00, 0.6 );
		else if( act->effects & EF_SLOW )
			fx = CreateAndAddSpecialEffect( FXTYPE_TRAIL, &act->actor->pos, &normal, act->value1, 0.95, 0.00, 3 );
		else
			fx = CreateAndAddSpecialEffect( FXTYPE_TRAIL, &act->actor->pos, &normal, act->value1, 0.95, 0.00, 2 );

		if( fx )
		{
			if( type == ENTITY_PLATFORM )
				fx->gravity = -30;

			fx->follow = act->actor;
			SetAttachedFXColour( fx, act->effects );
			act->effects &= ~EF_TRAIL;
		}
	}
	else if( act->effects & EF_BILLBOARDTRAIL )
	{
		if( act->effects & EF_FAST )
			fx = CreateAndAddSpecialEffect( FXTYPE_BILLBOARDTRAIL, &act->actor->pos, &normal, act->value1, 0.95, 0.05, 0.6 );
		else if( act->effects & EF_SLOW )
			fx = CreateAndAddSpecialEffect( FXTYPE_BILLBOARDTRAIL, &act->actor->pos, &normal, act->value1, 0.95, 0.05, 3 );
		else
			fx = CreateAndAddSpecialEffect( FXTYPE_BILLBOARDTRAIL, &act->actor->pos, &normal, act->value1, 0.95, 0.05, 2 );

		if( fx )
		{
			fx->follow = act->actor;
			SetAttachedFXColour( fx, act->effects );
			act->effects &= ~EF_BILLBOARDTRAIL;
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
	Function		: CreateGloopEffects
	Purpose			: Some bubbles, some smoke and a splash
	Parameters		: 
	Returns			: void
	Info			: 
*/
void CreateGloopEffects( SPECFX *parent )
{
	SPECFX *fx;
	VECTOR up, surface;
	int i;

	SetVector( &up, &parent->normal );

	SetVector( &surface, &up );
	ScaleVector( &surface, parent->scale.v[X] );
	AddToVector( &surface, &parent->origin );

	// Create the second wave of bubbles
	if( (fx = CreateAndAddSpecialEffect( FXTYPE_BUBBLES, &parent->origin, &up, 22, 0.7, 0, 0.5 )) )
	{
		fx->rebound = (PLANE2 *)JallocAlloc( sizeof(PLANE2), YES, "Rebound" );
		SetVector( &fx->rebound->normal, &up );
		SetVector( &fx->rebound->point, &surface );

		SetFXColour( fx, parent->r, parent->g, parent->b );
	}
	// Green smoke created at the _surface_ of the water
	fx = CreateAndAddSpecialEffect( FXTYPE_SMOKEBURST, &surface, &up, 50, 1, 0, 1.7 );
	SetFXColour( fx, parent->r, parent->g, parent->b );

	// Ditto for splash
	if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPLASH, &surface, &up, 14, 2.5, 0, 2 ) ))
	{
		fx->gravity = 0.1;
		fx->rebound = (PLANE2 *)JallocAlloc( sizeof(PLANE2), YES, "Rebound" );
		SetVector( &fx->rebound->normal, &up );
		SetVector( &fx->rebound->point, &surface );
		SetFXColour( fx, parent->r, parent->g, parent->b );
	}
}


void CreateLightningEffect( VECTOR *p1, VECTOR *p2, unsigned long effects, long life )
{
	VECTOR dir;
	long distance;
	short rn, c, i;
	SPECFX *fx;

	SubVector( &dir, p1, p2 );
	distance = Magnitude( &dir );
	MakeUnit( &dir );

	if( effects & EF_SLOW ) i=2;
	else if( effects & EF_MEDIUM ) i=3;
	else if( effects & EF_FAST ) i=4;
	else i=1;

	for( ; i; i-- )
	{
		if( (fx = CreateAndAddSpecialEffect( FXTYPE_LIGHTNING, p2, &dir, 5, distance, 25.0/(float)distance, life/60 ) ))
		{
			SetAttachedFXColour( fx, effects );

			// Randomise colours a bit
			rn = fx->r * 0.5;
			rn += Random(rn+1)-(rn*0.5);
			if( rn > 255 ) fx->r = 255;
			else if( rn < 0 ) fx->r = 0;
			else fx->r = rn;

			rn = fx->g * 0.5;
			rn += Random(rn+1)-(rn*0.5);
			if( rn > 255 ) fx->g = 255;
			else if( rn < 0 ) fx->g = 0;
			else fx->g = rn;

			rn = fx->b * 0.5;
			rn += Random(rn+1)-(rn*0.5);
			if( rn > 255 ) fx->b = 255;
			else if( rn < 0 ) fx->b = 0;
			else fx->b = rn;
		}
	}
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
#else
void CreateBlastRing()
{
}
#endif

/*	--------------------------------------------------------------------------------
	Function		: AllocateSprites
	Purpose			: Find a number of sprites and return a sublist
	Parameters		: number of sprites
	Returns			: pointer to first one
	Info			: 
*/
SPRITE *AllocateSprites( int number )
{
	// Return if allocation is impossible for any reason
	if( (number <= 0) || (fxsStackPtr-number < 0) || (number >= MAX_FXSPRITES-fxsCount) ) return NULL;

	// Now we can go and allocate sprites with gay abandon
	while( number-- )
	{
		AddSprite( fxSpriteStack[fxsStackPtr--], NULL );
		fxsCount++;
	}

	return fxSpriteStack[fxsStackPtr+1];
}


/*	--------------------------------------------------------------------------------
	Function		: DeallocateSprites
	Purpose			: Remove sprites from list and flag as unused
	Parameters		: number of sprites
	Returns			: 
	Info			: 
*/
void DeallocateSprites( SPRITE *head, int number )
{
	SPRITE *s=head, *t;

	if( !head ) return;

	if( (number<=0) || (fxsStackPtr+number >= MAX_FXSPRITES) )
	{
		dprintf"Couldn't deallocate %i sprites to the stack!\n",number));
		return;
	}

	while( number-- )
	{
		t = s->next;

		SubSprite( s );
		fxsCount--;
		fxSpriteStack[++fxsStackPtr] = s;

		s = t;
	}
}

