/*
	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: specfx.c
	Programmer	: Andy Eder
	Date		: 14/05/99 09:31:42

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include "ultra64.h"
#include "specfx.h"
#include "sprite.h"
#include "particle.h"
#include "anim.h"
#include "frogmove.h"
#include "path.h"
#include "enemies.h"
#include "platform.h"
#include "collect.h"
#include "tongue.h"
#include "multi.h"
#include "babyfrog.h"

#ifdef PSX_VERSION
#include "textures.h"
#include "Eff_Draw.h"
#else
#include <pcgfx.h>
#include <pcmisc.h>
#include <editor.h>
#endif

#include <islmem.h>
#include <islutil.h>
#include "Main.h"
#include "layout.h"


#ifdef PC_VERSION
#define MAX_SPECFX	128
#define MAX_PLANES	64
#else
#define MAX_SPECFX	128
#define MAX_PLANES	64
#endif

#define FX_CLIPSTEP	(ACTOR_DRAWDISTANCEOUTER>>12)/MAX_SPECFX

SPECFXLIST sfxList;
PLANE2LIST p2List;

char pauseMode		= 0;

#define NUM_TRAIL_ELEMENTS 32



//----- [ TEXTURES USED FOR SPECIAL FX ] -----//

TextureType *txtrWaterRipples = NULL;
TextureType *txtrRipple		= NULL;
TextureType *txtrStar		= NULL;
TextureType *txtrSolidRing	= NULL;
TextureType *txtrSmoke		= NULL;
TextureType *txtrSmoke2		= NULL;
TextureType *txtrRing		= NULL;
TextureType *txtrBubble		= NULL;
TextureType *txtrBlank		= NULL;
TextureType *txtrTrail		= NULL;
TextureType *txtrFlash		= NULL;
TextureType *txtrElectric	= NULL;
TextureType *txtrFlare		= NULL;
TextureType *txtrCroak		= NULL;
TextureType *txtrWarpflash	= NULL;

enum
{
	FXUPDATE_DECAL,
	FXUPDATE_RING,
	FXUPDATE_BOLT,
	FXUPDATE_SMOKE,
	FXUPDATE_SWARM,
	FXUPDATE_EXPLODE,
	FXUPDATE_TRAIL,
	FXUPDATE_LIGHTNING,
	FXUPDATE_FLY,
	FXUPDATE_TWINKLE,

	FXUPDATE_PFX_JET,
	FXUPDATE_PFX_TELEIDLE,
	FXUPDATE_PFX_TELEACTIVE,

	NUM_FXUPDATES
};

void UpdateFXDecal( SPECFX *fx );
void UpdateFXRing( SPECFX *fx );
void UpdateFXBolt( SPECFX *fx );
void UpdateFXSmoke( SPECFX *fx );
void UpdateFXSwarm( SPECFX *fx );
void UpdateFXExplode( SPECFX *fx );
void UpdateFXTrail( SPECFX *fx );
void UpdateFXLightning( SPECFX *fx );
void UpdateFXFly( SPECFX *fx );
void UpdateFXTwinkle( SPECFX *fx );

void CreateBlastRing( );
void AddTrailElement( SPECFX *fx, int i );
void CreateGloopEffects( SPECFX *parent );

PLANE2 *AllocateP2( );
void DeallocateP2( PLANE2 *p );

short updateMapping[PTYPE_NUMTYPES] = 
{
	FXUPDATE_DECAL,
	FXUPDATE_DECAL,
	FXUPDATE_DECAL,
	FXUPDATE_EXPLODE,
	FXUPDATE_SWARM,
	FXUPDATE_SMOKE,
	FXUPDATE_SMOKE,
	FXUPDATE_DECAL,
	FXUPDATE_EXPLODE,
	FXUPDATE_EXPLODE,
	FXUPDATE_SMOKE,
	FXUPDATE_SWARM,
	FXUPDATE_SMOKE,
	FXUPDATE_EXPLODE,
	FXUPDATE_EXPLODE,
	FXUPDATE_RING,
	FXUPDATE_SWARM,
	FXUPDATE_BOLT,
	FXUPDATE_TRAIL,
	FXUPDATE_TRAIL,
	FXUPDATE_LIGHTNING,
	FXUPDATE_FLY,
	FXUPDATE_SWARM,
	FXUPDATE_RING,
	FXUPDATE_TWINKLE,
	FXUPDATE_TWINKLE,
	FXUPDATE_DECAL,
	FXUPDATE_RING,
	FXUPDATE_TWINKLE,

	FXUPDATE_PFX_JET,
	FXUPDATE_PFX_JET,
	FXUPDATE_PFX_TELEIDLE,
	FXUPDATE_PFX_TELEACTIVE,
};

// Used to store precalculated blast ring shape
FVECTOR *ringVtx = NULL;


/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddSpecialEffect
	Purpose			: creates and initialises a special effect, either barebones or of a custom type
	Parameters		: 
	Returns			: Created effect
	Info			: 
*/
SPECFX *CreateSpecialEffect( short type, SVECTOR *origin, FVECTOR *normal, fixed size, fixed speed, fixed accn, fixed lifetime )
{
	SVECTOR distance;

	SubVectorSSS( &distance, origin, &frog[0]->actor->position );
	if( MagnitudeS(&distance) > ACTOR_DRAWDISTANCEOUTER-((sfxList.count*FX_CLIPSTEP)<<12) )
		return NULL;

	return CreateSpecialEffectDirect( type, origin, normal, size, speed, accn, lifetime );
}

int specFXAlpha = 255;
SPECFX *CreateSpecialEffectDirect( short type, SVECTOR *origin, FVECTOR *normal, fixed size, fixed speed, fixed accn, fixed lifetime )
{
	SPECFX *effect = NULL;
	SPRITE *s;
	PARTICLE *p;
	long i, life = max( ((lifetime*60)>>12) ,1 ), tmp;
	static char rot = 0;

	if( !(effect = AllocateFX(1, type)) )
		return NULL;

	effect->type = type;
	SetVectorSS( &effect->origin, origin );
	SetVectorFF( &effect->normal, normal );
	effect->scale.vx = size*SCALE;
	effect->scale.vy = size*SCALE;
	effect->scale.vz = size*SCALE;
	effect->speed = speed*SCALE;
	effect->accn = accn*SCALE;
	effect->lifetime = actFrameCount + life;
	effect->r = 255;
	effect->g = 255;
	effect->b = 255;
	effect->a = specFXAlpha;	   

	#ifdef PSX_VERSION
	effect->zDepthOff = 0;			//make the default zDepthOff alteration 0
	#endif

	// Particle system type
	if( type >= FXTYPE_NUMTYPES )
		return CreateParticleEffect( effect );

	// If not a particle system then it's a standard effect
	switch( type )
	{
	case FXTYPE_WAKE:
	case FXTYPE_DECAL:
	case FXTYPE_GARIBCOLLECT:
 		effect->fade = effect->a/life;
		#ifdef PSX_VERSION
		effect->flags = SPRITE_TRANSLUCENT;//fading...needs to be translucent
		#endif

		effect->origin.vx += (effect->normal.vx * SCALE)>>12;
 		effect->origin.vy += (effect->normal.vy * SCALE)>>12;
 		effect->origin.vz += (effect->normal.vz * SCALE)>>12;

		if( effect->type == FXTYPE_WAKE )
			effect->tex = txtrRipple;
		else if( effect->type == FXTYPE_WATERRIPPLE )
		{
	 		effect->r = 200;
	 		effect->g = 200;
	 		effect->tex = txtrWaterRipples;
		}
		else if( effect->type == FXTYPE_GARIBCOLLECT )
		{
 			effect->b = 0;
 			effect->spin = 5;
	 		effect->tex = txtrStar;
		}
		else
			effect->tex = txtrSolidRing;

		effect->Update = UpdateFXDecal;
		effect->Draw = DrawFXDecal;

		break;	 
	case FXTYPE_POLYRING:
	case FXTYPE_FROGSHIELD:
	case FXTYPE_CROAK:
		if( !ringVtx )
			CreateBlastRing( );
 		effect->fade = (effect->a<<12)/life;
		effect->fadeAlpha = effect->a<<12;

		effect->origin.vx += (effect->normal.vx * SCALE)>>12;
 		effect->origin.vy += (effect->normal.vy * SCALE)>>12;
 		effect->origin.vz += (effect->normal.vz * SCALE)>>12;

		effect->scale.vy >>= 3;

 		effect->tilt = 3686;

		if( effect->type == FXTYPE_FROGSHIELD )
			effect->tex = txtrFlash;
		else if( effect->type == FXTYPE_CROAK )
			effect->tex = txtrCroak;
		else
			effect->tex = txtrBlank;
 		effect->Update = UpdateFXRing;
		effect->Draw = DrawFXRing;
		break;
	case FXTYPE_LASER:
		if( !ringVtx )
			CreateBlastRing( );
 
 		effect->scale.vx = 81920;
 		effect->scale.vz = 81920;
 		SetVectorFF(&effect->vel, &effect->normal);
 		ScaleVectorFF( &effect->vel, effect->speed );
 		effect->spin = 5;
 		effect->tilt = 4096;
 		effect->a = 128;
 
		effect->tex = txtrBlank;
 		effect->Update = UpdateFXBolt;
		effect->Draw = DrawFXRing;

		break;
//	case FXTYPE_TRAIL:
//	case FXTYPE_BILLBOARDTRAIL:
// 		effect->fade = effect->a/life;
//		#ifdef PSX_VERSION
//		effect->flags = SPRITE_TRANSLUCENT;//fading...needs to be translucent
//		#endif

//		effect->speed /= SCALE;
// 		effect->numP = i = NUM_TRAIL_ELEMENTS;
//		// We can get away with dynamic allocation here cos trails are usually persistant
// 		effect->particles = (PARTICLE *)MALLOC0( sizeof(PARTICLE)*i);
//		// Not to mention that the update function is a bitch if we use a linked list :)
// 		effect->particles[0].bounce = 1;
 
// 		while( i-- )
// 		{
// 			effect->particles[i].a = (i+1)*(255/effect->numP);
// 			SetVectorSS(&effect->particles[i].pos, &effect->origin);
// 			effect->particles[i].rMtrx = (fixed *)MALLOC0( sizeof(fixed)*16 );
// 		}
 
// 		effect->tex = txtrTrail;
// 		effect->Update = UpdateFXTrail;
//		effect->Draw = DrawFXTrail;

//		break;
	case FXTYPE_FROGSTUN:

 		effect->numP = 6;
 		i = effect->numP;

		effect->tex = txtrStar;

		if( !(effect->sprites = AllocateSprites(effect->numP)) )
		{
			DeallocateFX( effect,1 );
			return NULL;
		}

		if( !(effect->particles = AllocateParticles(i)) )
		{
			DeallocateFX( effect,1 );
			return NULL;
		}

		s = effect->sprites;
		p = effect->particles;
 
 		while(i--)
 		{
  			s->texture = effect->tex;
  			SetVectorSS( &s->pos, &effect->origin );
  
  			s->scaleX = (effect->scale.vx>>12)/SCALE;
  			s->scaleY = (effect->scale.vy>>12)/SCALE;
  			s->r = effect->r;
  			s->g = effect->g;
  			s->b = effect->b;
  			s->a = effect->a;
  
  			s->offsetX	= -16;
  			s->offsetY	= -40;//mm used to be -16...why does this work?
  			s->flags = SPRITE_TRANSLUCENT | SPRITE_ADDITIVE;
  			effect->particles[i].pos.vx = -80 + Random(160);
 			effect->particles[i].pos.vy = -60 + Random(120);
  			effect->particles[i].pos.vz = -80 + Random(160);

			s = s->next;
			p = p->next;
  		}
  
  		effect->Update = UpdateFXSwarm;
  		effect->Draw = NULL;
		break;
	case FXTYPE_BATSWARM:
	case FXTYPE_BUTTERFLYSWARM:
	case FXTYPE_SPACETHING1:
 		effect->numP = (lifetime>>12); // Nasty Nasty Nasty
 		i = effect->numP;
 
 		effect->act = (ACTOR2 **)MALLOC0( sizeof(ACTOR2 *)*effect->numP);
		if( !(effect->particles = AllocateParticles(i)) )
		{
			DeallocateFX( effect,1 );
			return NULL;
		}
		p = effect->particles;
 
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

				if( (fx = CreateSpecialEffect( FXTYPE_TRAIL, &effect->origin, &effect->normal, 8192, 4055, 0, 8092 )) )
				{
					fx->follow = effect->act[i]->actor;
					SetFXColour( fx, 255, 100, 100 );
				}
			}

//			if( effect->act[i]->actor->objectController )
//				InitActorAnim( effect->act[i]->actor );

 			actorAnimate( effect->act[i]->actor,0,YES,NO,255, 0);//bb??256?
 			effect->act[i]->actor->size.vx = effect->scale.vx;
 			effect->act[i]->actor->size.vy = effect->scale.vy;
 			effect->act[i]->actor->size.vz = effect->scale.vz;
 
			p->pos.vx = -80 + Random(160);
			p->pos.vy = -60 + Random(120);
			p->pos.vz = -80 + Random(160);
			p = p->next;
 		}
 
 		effect->Update = UpdateFXSwarm;
 		effect->Draw = NULL;

		break;
	case FXTYPE_HEALTHFLY:
		effect->numP = 1;
 		effect->act = (ACTOR2 **)MALLOC0( sizeof(ACTOR2 *));

		// create and add the nme actor
		if (!(effect->act[0] = CreateAndAddActor( pickupObjName, 0,0,0, INIT_ANIMATION)))
		{
			DeallocateFX( effect,1 );
			return NULL;
		}

		if( !(effect->sprites = AllocateSprites(effect->numP)) )
		{
			DeallocateFX( effect,1 );
			return NULL;
		}

		actorAnimate( effect->act[0]->actor,0,YES,NO,256, 0);
		effect->act[0]->actor->size.vx = size;
		effect->act[0]->actor->size.vy = size;
		effect->act[0]->actor->size.vz = size;

		effect->vel.vx = (-80 + Random(160))<<12;
		effect->vel.vy = (-60 + Random(120))<<12;
		effect->vel.vz = (-80 + Random(160))<<12;

		AddVectorSSF( &effect->act[0]->actor->position, &effect->origin, &effect->vel );

		SetVectorSF( &effect->act[0]->actor->rotaim, &effect->normal );
		ScaleVector( &effect->act[0]->actor->rotaim, 50 );

		SetVectorFF( &effect->act[0]->actor->vel, &inVec );
		ScaleVectorFF( &effect->act[0]->actor->vel, effect->speed );

		effect->Update = UpdateFXFly;

		s = effect->sprites;
		s->a = 128;
		s->texture = txtrFlare;

		effect->scale.vx /= SCALE;
		effect->scale.vy /= SCALE;
		effect->scale.vz /= SCALE;

		s->r = 255;
		s->g = 255;
		s->b = 255;

		s->offsetX = -20;
		s->offsetY = -20;
		s->flags = SPRITE_TRANSLUCENT | SPRITE_ADDITIVE;

		s->scaleX = 80;
		s->scaleY = 80;
		SetVectorSS( &s->pos, &effect->origin );

		effect->fade = 0;
		effect->tilt = 4096;
		effect->follow = effect->act[0]->actor;

		// bwahahahahahaha
		effect->Draw = UpdateFXTwinkle;

		break;
	case FXTYPE_SMOKE_STATIC:
	case FXTYPE_SMOKE_GROWS:
	case FXTYPE_BUBBLES:
		// Velocity is normal scaled by speed, plus a random offset scaled by speed
		effect->accn /= SCALE;
   		SetVectorFF( &effect->vel, &effect->normal );
   		ScaleVectorFF( &effect->vel, effect->speed );
		tmp = effect->speed>>5;
   		effect->vel.vx += (Random(30)-10)*tmp;
   		effect->vel.vy += (Random(30)-10)*tmp;
   		effect->vel.vz += (Random(30)-10)*tmp;

 		if (life == 0)
 			effect->fade = 0;
 		else				
	 		effect->fade = effect->a/life;

#ifdef PSX_VERSION
		effect->numP = i = 1;
#else
		if( effect->type == FXTYPE_BUBBLES )
			effect->numP = i = 1;
		else
			effect->numP = i = 2;
#endif
		if( !(effect->sprites = AllocateSprites( effect->numP )) )
		{
			DeallocateFX( effect,1 );
			return NULL;
		}

		s = effect->sprites;

		while( i-- )
		{
 			if( effect->type == FXTYPE_BUBBLES )
 				s->texture = txtrBubble;
 			else
 				s->texture = txtrSmoke;
 
 			SetVectorSS( &s->pos, &effect->origin );
 
 			s->r = effect->r;
 			s->g = effect->g;
 			s->b = effect->b;
 			s->a = effect->a;
 
 			s->offsetX = -16;
			s->offsetY = -16;
 			s->flags = SPRITE_TRANSLUCENT;
			if(type == FXTYPE_BUBBLES)
				s->flags |= SPRITE_ADDITIVE;
 
 			if( effect->type == FXTYPE_SMOKE_GROWS || effect->type == FXTYPE_SMOKE_STATIC )
 			{
 				s->scaleX = ((effect->scale.vx/SCALE)>>12) + (Random(21)-10);
 				s->scaleY = ((effect->scale.vy/SCALE)>>12) + (Random(21)-10);
 
 				s->flags |= (SPRITE_FLAGS_ROTATE | SPRITE_ADDITIVE);
 				s->angle = 0;
 				s->angleInc = 25 + Random(40);
				if( rot )
 					s->angleInc *= -1;
 			}
 			else
 			{
 				s->scaleX = (effect->scale.vx/SCALE)>>12;
 				s->scaleY = (effect->scale.vy/SCALE)>>12;
 			}

			s = s->next;
			rot = !rot;
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
		effect->accn /= SCALE;

		if( !(effect->sprites = AllocateSprites( effect->numP )) )
		{
			DeallocateFX( effect,1 );
			return NULL;
		}
		if( !(effect->particles = AllocateParticles(i)) )
		{
			DeallocateFX( effect,1 );
			return NULL;
		}
  		if( !(effect->rebound = AllocateP2()) )
		{
			DeallocateFX( effect,1 );
			return NULL;
		}

		SetVectorSS( &effect->rebound->point, &effect->origin );
 		SetVectorFF( &effect->rebound->normal, &effect->normal );
 
		if( effect->type == FXTYPE_SPLASH )
			effect->tex = txtrBubble;
		else if( effect->type == FXTYPE_SPARKBURST )
			effect->tex = txtrSolidRing;
		else if( effect->type == FXTYPE_SPARKLYTRAIL )
			effect->tex = txtrFlash;
		else if( effect->type == FXTYPE_SMOKEBURST || effect->type == FXTYPE_FIERYSMOKE )
			effect->tex = txtrSmoke;
 
 		effect->fade = effect->a/max(life,1);
		tmp = effect->speed / 30;

 		s = effect->sprites;
 		p = effect->particles;
		while(i--)
 		{
 			SetVectorSS( &s->pos, &effect->origin );
 
 			s->r = effect->r;
 			s->g = effect->g;
 			s->b = effect->b;
 			s->a = effect->a;
 
 			s->offsetX = -16;
 			s->offsetY = -16;
 			s->flags = SPRITE_TRANSLUCENT | SPRITE_ADDITIVE;
 
 			if( effect->type == FXTYPE_SPARKBURST || effect->type == FXTYPE_SPARKLYTRAIL )
 				p->bounce = 1;
 			else
 				p->bounce = 0;
 
 			// Velocity is normal scaled by speed, plus a random offset scaled by speed
 			SetVectorFF( &p->vel, &effect->normal );
 			ScaleVectorFF( &p->vel, effect->speed );
 
 			if( effect->type == FXTYPE_SMOKEBURST || effect->type == FXTYPE_FIERYSMOKE )
 			{
				s->flags |= SPRITE_FLAGS_ROTATE;

				s->scaleX = ((effect->scale.vx/SCALE)>>12) + Random(21)-10;
 				s->scaleY = ((effect->scale.vy/SCALE)>>12) + Random(21)-10;
 
 				p->vel.vx += (Random(30)-10)*(tmp>>1);
 				p->vel.vy += (Random(30)-10)*(tmp>>1);
 				p->vel.vz += (Random(30)-10)*(tmp>>1);

				s->texture = effect->tex;//Random(2)?txtrSmoke:txtrSmoke2;

				s->angle = 0;
				s->angleInc = 25 + Random(40);
 				if( i & 1 )
 					s->angleInc *= -1;
 			}
 			else
 			{
 				p->vel.vx += (Random(30)-10)*tmp;
 				p->vel.vy += (Random(30)-10)*tmp;
 				p->vel.vz += (Random(30)-10)*tmp;

				s->scaleX = (effect->scale.vx>>12)/SCALE;
 				s->scaleY = (effect->scale.vy>>12)/SCALE;
 				s->texture = effect->tex;
 			}
 
 			if( effect->type == FXTYPE_FIERYSMOKE )
 			{
				if( i & 1 )
				{
					s->flags &= ~SPRITE_ADDITIVE;
					s->texture = txtrSmoke2;
				}

 				s->g = 180;
 				s->b = 0;
 			}

			s = s->next;
			p = p->next;
 		}
			
 		effect->Update = UpdateFXExplode;
 		effect->Draw = NULL;
		break;
	case FXTYPE_LIGHTNING:
		effect->speed /= SCALE;
 		effect->numP = (effect->speed/240)>>12;
 		i = effect->numP;

		#ifdef PSX_VERSION
		effect->flags = SPRITE_TRANSLUCENT;//fading...needs to be translucent
		#endif
		
		if( !(effect->particles = AllocateParticles(i)) )
		{
			DeallocateFX( effect,1 );
			return NULL;
		}
		effect->tex = txtrElectric;

		p = effect->particles;
 		while(i--)
 		{
			SetVectorSS( &p->pos, &effect->origin );
 
			p->r = effect->r;
			p->g = effect->g;
			p->b = effect->b;
			p->a = effect->a;

			p = p->next;
 		}
 
 		effect->Update = UpdateFXLightning;
		effect->Draw = DrawFXLightning;
		break;
	case FXTYPE_GLOW:
	case FXTYPE_TWINKLE:
	case FXTYPE_FLASH:
		effect->numP = 1;

		if( !(effect->sprites = AllocateSprites( effect->numP )) )
		{
			DeallocateFX( effect,1 );
			return NULL;
		}
		s = effect->sprites;
		s->a = 200;

		if( effect->type == FXTYPE_GLOW )
		{
			s->texture = txtrFlare;
			s->a = 128;
		}
		else if( effect->type == FXTYPE_FLASH )
			s->texture = txtrWarpflash;
		else
			s->texture = txtrFlash;

		effect->scale.vx /= SCALE;
		effect->scale.vy /= SCALE;
		effect->scale.vz /= SCALE;

		s->r = 255;
		s->g = 255;
		s->b = 255;

		s->offsetX = -16;
		s->offsetY = -16;
		s->flags = SPRITE_TRANSLUCENT | SPRITE_ADDITIVE;

		s->scaleX = size>>12;
		s->scaleY = size>>12;
		SetVectorSS( &s->pos, &effect->origin );

		if (life != 0)
			effect->fade = effect->a/life;
		else
			effect->fade = 0;

		if( effect->type == FXTYPE_FLASH )
		{
			effect->tilt = actFrameCount;
			s->scaleY *= 5;
		}

		effect->Update = UpdateFXTwinkle;
		effect->Draw = NULL;
		break;
	case FXTYPE_GREENGLOOP:
		// Create a bubble, but the effect persists after the bubbles are gone, to create a wait time
		// Also create several actual bubbles, but they do not spawn all the shit that this does
		SetVectorFF( &effect->vel, &effect->normal );
		ScaleVectorFF( &effect->vel, speed );
		effect->vel.vx += FMul((-4096 + ToFixed(Random(30))),FMul(speed,1638));
		effect->vel.vy += FMul((-4096 + ToFixed(Random(30))),FMul(speed,1638));
		effect->vel.vz += FMul((-4096 + ToFixed(Random(30))),FMul(speed,1638));
 		effect->fade = effect->a/life;

		effect->numP = 1;
		if( !(effect->sprites = AllocateSprites( effect->numP )) )
		{
			DeallocateFX( effect,1 );
			return NULL;
		}
		s = effect->sprites;
		s->texture = txtrBubble;

		SetVectorSS( &s->pos, &effect->origin );
		s->scaleX = 22;
		s->scaleY = 22;

		s->r = effect->r;
		s->g = effect->g;
		s->b = effect->b;
		s->a = effect->a;
		s->offsetX = -16;
		s->offsetY = -16;
		s->flags = SPRITE_TRANSLUCENT;

		effect->Update = UpdateFXSmoke;
		effect->Draw = NULL;
		break;

	default:
		DeallocateFX( effect,1 );
		return NULL;
 	}

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

	for( fx1 = sfxList.head.next; fx1 != &sfxList.head; fx1 = fx2 )
	{
		fx2 = fx1->next;

		if( fx1->Update )
			fx1->Update( fx1 );
	}
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFXDecal
	Purpose			: Grow and fade
	Parameters		: 
	Returns			: 
	Info			: 
*/
void UpdateFXDecal( SPECFX *fx )
{
	int fo;
	fixed spd;

 	if( fx->follow )
 		SetVectorSS( &fx->origin, &fx->follow->position );

	fo = (fx->fade * gameSpeed)>>12;
	if( fx->a > fo ) fx->a -= fo;
	else fx->a = 0;

	fx->speed += FMul(fx->accn, gameSpeed);
	spd = FMul(fx->speed, gameSpeed);
	fx->scale.vx += spd;
	fx->scale.vz += spd;

	if( fx->spin )
		fx->angle += (fx->spin * gameSpeed)>>12;

	if( actFrameCount > fx->lifetime )
		DeallocateFX(fx,1);
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
	fixed speed;

 	if( fx->follow )
 		SetVectorSS( &fx->origin, &fx->follow->position );

	fo = (fx->fade * gameSpeed)>>12;
	if( fx->fadeAlpha > fo ) fx->fadeAlpha -= fo;
	else fx->fadeAlpha = 0;

	fx->a = fx->fadeAlpha>>12;

	fx->speed += FMul(fx->accn, gameSpeed);
	speed = FMul(fx->speed, gameSpeed);

	fx->scale.vx += speed;
	fx->scale.vy += speed>>2;
	fx->scale.vz += speed;
	fx->angle += (fx->spin * gameSpeed)>>12;
	
	if( actFrameCount > fx->lifetime )
		DeallocateFX(fx,1);
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
	fixed accn = FMul(fx->accn, gameSpeed);

 	if( fx->follow )
 		SetVectorSS( &fx->origin, &fx->follow->position );

	fo = (fx->fade * gameSpeed)>>12;
	if( fx->a > fo ) fx->a -= fo;
	else fx->a = 0;

	fx->vel.vx += accn;
	fx->vel.vy += accn;
	fx->vel.vz += accn;
	fx->origin.vx += (FMul(fx->vel.vx, gameSpeed)>>12);
	fx->origin.vy += (FMul(fx->vel.vy, gameSpeed)>>12);
	fx->origin.vz += (FMul(fx->vel.vz, gameSpeed)>>12);
	fx->angle += (fx->spin * gameSpeed)>>12;
	
	if( actFrameCount > fx->lifetime )
		DeallocateFX(fx,1);
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
	int fo, i = fx->numP, dead=0;
	fixed dist, vS;
	SPRITE *s;

	if( fx->follow )
 		SetVectorSS( &fx->origin, &fx->follow->position );

	s = fx->sprites;
	while(i--)
	{
		fo = (fx->fade * gameSpeed)>>12;
		if( s->a > fo ) s->a -= fo;
		else
		{
			s->draw = 0;
			s->a = 0;
			dead = 1;
		}
	 
	 	s->pos.vx += (FMul(fx->vel.vx, gameSpeed)>>12);
		s->pos.vy += (FMul(fx->vel.vy, gameSpeed)>>12);
		s->pos.vz += (FMul(fx->vel.vz, gameSpeed)>>12);
	 
		// Slow down gameSpeed times
		vS = ( 4096 - gameSpeed/50/*FMul(82,gameSpeed)*/);
		ScaleVectorFF( &fx->vel, vS );

		if( fx->type == FXTYPE_SMOKE_GROWS )
		{
			s->scaleX += (FMul(fx->accn,gameSpeed)>>12);
			s->scaleY += (FMul(fx->accn,gameSpeed)>>12);
		}
		else if( fx->type == FXTYPE_BUBBLES )
		{
			if( fx->rebound )
			{
				FVECTOR fRP;
				FVECTOR fSP;
				SetVectorFS(&fRP,&fx->rebound->point);
				fx->rebound->J = -DotProductFF( &fRP, &fx->rebound->normal );
				SetVectorFS(&fSP,&fx->sprites->pos);
				dist = -(DotProductFF(&fSP, &fx->rebound->normal) + fx->rebound->J);

				if(dist > 0 && dist < 409600)
				{
					CreateSpecialEffect( FXTYPE_WAKE, &s->pos, &fx->rebound->normal, 20480, 410, 205, 1280);
					DeallocateP2( fx->rebound );
					fx->rebound = NULL;
				}
			}
		}
		s = s->next;
	}

	if( actFrameCount > fx->lifetime || dead )
	{
		// Spawn some bubbles, some smoke and a splash
		if( fx->type == FXTYPE_GREENGLOOP )
			CreateGloopEffects( fx );

		DeallocateFX(fx,1);
	}
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
	FVECTOR up;
	SVECTOR pos;
	int i = fx->numP;
//bb	fixed dist, speed = max(FDiv(gameSpeed,12288), 4096)*SCALE, limit=gameSpeed*2*SCALE;
	fixed dist, speed = max((gameSpeed/3), 4096)*SCALE, limit=gameSpeed*2*SCALE;
	SPRITE *s;
	PARTICLE *p;
  
 	if( fx->follow )
 		SetVectorSS( &fx->origin, &fx->follow->position );

	if( fx->type == FXTYPE_FROGSTUN )
	{
		SetVectorFF( &up, &currTile[0]->normal );
		ScaleVectorFF( &up, 20<<12 );
		AddVectorSFS( &fx->origin, &up, &frog[0]->actor->position );
	}

	s = fx->sprites;
	p = fx->particles;
	while(i--)
	{
		// Set world check position from either sprite or actor
		if( !fx->act )
			SetVectorSS( &pos, &s->pos );
		else
			SetVectorSS( &pos, &fx->act[i]->actor->position );

		// Fade out star stun
		if( fx->type == FXTYPE_FROGSTUN )
		{		
			if( s->a > 7 ) s->a -= 8;
			else s->a = 0;
		}

		// Update particle velocity to oscillate around the point
		if( pos.vx > fx->origin.vx)
			p->vel.vx -= speed;
		else
			p->vel.vx += speed;
		if( pos.vy > fx->origin.vy )
			p->vel.vy -= speed;
		else
			p->vel.vy += speed;
		if( pos.vz > fx->origin.vz)
			p->vel.vz -= speed;
		else
			p->vel.vz += speed;

		// Limit velocity of particles
		if( p->vel.vx > limit )
			p->vel.vx = limit;
		else if( p->vel.vx < -limit )
			p->vel.vx = -limit;
		if( p->vel.vy > limit )
			p->vel.vy = limit;
		else if( p->vel.vy < -limit )
			p->vel.vy = -limit;
		if( p->vel.vz > limit )
			p->vel.vz = limit;
		else if( p->vel.vz < -limit )
			p->vel.vz = -limit;

		// Add velocity to local particle position
		AddToVectorSF( &p->pos, &p->vel );
		// Add local particle pos to swarm origin to get world coords for sprite or actor
		if( !fx->act )
		{
			AddVectorSSS( &s->pos, &fx->origin, &p->pos );
			SetVectorSS( &pos, &s->pos );
			s = s->next;
		}
		else
		{
			AddVectorSSS( &fx->act[i]->actor->position, &fx->origin, &p->pos );
			SetVectorSS( &pos, &fx->act[i]->actor->position );
		}

		if( fx->rebound )
		{
			FVECTOR fRP;
			FVECTOR fPos;
			SetVectorFS(&fRP,&fx->rebound->point);
			fx->rebound->J = -DotProductFF( &fRP, &fx->rebound->normal );
			SetVectorFS(&fPos,&pos);
			dist = -(DotProductFF(&fPos, &fx->rebound->normal) + fx->rebound->J);

			if(dist > 0 && dist < 122880)
				CreateSpecialEffect( FXTYPE_WAKE, &pos, &fx->rebound->normal, 20480, 2048, 410, 1229 );
		}

		p = p->next;
	}

	if( fx->type == FXTYPE_FROGSTUN )
		if( actFrameCount > fx->lifetime )
			DeallocateFX(fx, 1);
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
	fixed dist, vS;
//	int i = fx->numP, j, fo, ele, alive=0;
	int i = fx->numP, fo, alive=0;
	FVECTOR up;
	SPRITE *s;
	PARTICLE *p;

 	if( fx->follow )
 		SetVectorSS( &fx->origin, &fx->follow->position );

	// Slow down gameSpeed times
//bb	vS = 4096-FMul(82,gameSpeed);
	vS = 4096-gameSpeed/50;

	s = fx->sprites;
	p = fx->particles;
	while(i--)
	{
		if( p->bounce == 2 )
			continue;
		alive++;

		ScaleVectorFF( &p->vel, vS );

		if( fx->gravity != 0 )
		{
			SetVectorFF( &up, &fx->normal );
			ScaleVectorFF( &up, FMul(fx->gravity,gameSpeed) );
			SubFromVectorFF( &p->vel, &up );
		}

		if( fx->rebound )
		{
			FVECTOR fRP;
			FVECTOR fSP;
			SetVectorFS(&fRP,&fx->rebound->point);
			fx->rebound->J = -DotProductFF( &fRP, &fx->rebound->normal );
			SetVectorFS(&fSP,&s->pos);
			dist = -(DotProductFF(&fSP, &fx->rebound->normal) + fx->rebound->J);

			// check if particle has hit (or passed through) the plane
			if(dist > 0)
			{
				if( p->bounce )
					p->vel.vy = FMul(p->vel.vy,-3891); // Should be relative to the normal, but it'll do for now
				else
				{
					SetVectorFV( &p->vel, &zero );
					p->bounce = 2;
					s->draw = 0;
				}

				// check if this exploding particle type triggers some other effect or event
				if( fx->type == FXTYPE_SPLASH )
					if( dist < 204800 )
						CreateSpecialEffect( FXTYPE_WAKE, &s->pos, &fx->rebound->normal, 20480, 819, 410, 1229 );
			}
		}

		s->pos.vx += (FMul(p->vel.vx, gameSpeed)>>12);
		s->pos.vy += (FMul(p->vel.vy, gameSpeed)>>12);
		s->pos.vz += (FMul(p->vel.vz, gameSpeed)>>12);

		fo = ((Random(4) + fx->fade) * gameSpeed)>>12;

		if( fx->type == FXTYPE_SMOKEBURST || fx->type == FXTYPE_FIERYSMOKE )
		{
			s->scaleX += (FMul(fx->accn,gameSpeed)>>12);
			s->scaleY += (FMul(fx->accn,gameSpeed)>>12);
		}

		// For fiery smoke, fade to black then fade out if not the additive component
		if( fx->type == FXTYPE_FIERYSMOKE && (s->r || s->g || s->b) )//&& !(s->flags & SPRITE_ADDITIVE) )
		{
			if( s->r > fo ) s->r -= fo;
			else s->r = 0;
			if( s->g > fo ) s->g -= fo;
			else s->g = 0;
		}

		if( s->a > fo ) s->a -= fo;
		else s->a = 0;

		if( !s->a )
		{
			s->draw = 0;
			p->bounce = 2;
		}

		s = s->next;
		p = p->next;
	}

	if( actFrameCount > fx->lifetime || !alive )
		DeallocateFX(fx, 1);
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFXTrails
	Purpose			: 
	Parameters		: 
	Returns			: void
	Info			: 
*/
//void UpdateFXTrail( SPECFX *fx )
//{
//	unsigned long i = fx->start;
//	short diff;
//	fixed fo;
//	VECTOR dist;

//	if( fx->follow )
//	{
//		// Flag for first time through
//		if( fx->particles[i].bounce )
//		{
//			fx->particles[i].bounce = 0;
//			AddTrailElement( fx, i );
//		}
//		else 
//		{
//			// If space, add a new particle after end (wrapped)
//			if( fx->end < fx->start ) 
//				diff = fx->start - fx->end - 1;
//			else
//				diff = ((fx->numP-1)-fx->end)+fx->start;

//			if( diff > 0 && fx->particles[(fx->end+1)%fx->numP].a < 16 )
//			{
//				if( ++fx->end >= fx->numP ) fx->end = 0;
//				AddTrailElement( fx, fx->end );
//			}
//		}
//	}

	// Update particles that are alive - fade, shrink by speed, move by velocity
	// Note - polys stored in object space
//	do
//	{
//		ScaleVectorSF( &fx->particles[i].poly[0], fx->speed );
//		ScaleVectorSF( &fx->particles[i].poly[1], fx->speed );
//		AddToVectorSF( &fx->particles[i].pos, &fx->particles[i].vel );

//		if( ++i >= fx->numP ) i=0;

//	} while( i != ((fx->end+1)%fx->numP) );

	// Fade out *all* particles, to get correct delay
//	for( i=0; i<fx->numP; i++ )
//	{
//		fo = FMul(fx->fade<<12, gameSpeed) ;
//		fo = gameSpeed * fx->fade;
//		if( (fx->particles[i].a<<12) > fo ) fx->particles[i].a -= (fo>>12);
//		else fx->particles[i].a = 0;
//	}

	// If oldest particle has died, exclude it from the list
//	if( fx->particles[fx->start].a < 16 || DistanceBetweenPointsSS(&fx->particles[fx->start].poly[0],&fx->particles[fx->start].poly[1])<(2<<12) )
//	{
//		fx->particles[fx->start].a = 0;
//		if( ++fx->start >= fx->numP )
//			fx->start = 0;
//		// If no more particles in list, time to die
//		if( (fx->start == fx->end-1) && !fx->follow )
//			DeallocateFX(fx, 1);
//	}
//}


// Check the follow actor for quaternion and rotate the points about it. If no actor, don't add.
//void AddTrailElement( SPECFX *fx, int i )
//{
//	fixed t;
//	IQUATERNION cross;
//	SVECTOR offset;

//	fx->particles[i].r = fx->r;
//	fx->particles[i].g = fx->g;
//	fx->particles[i].b = fx->b;
//	fx->particles[i].a = fx->a;
// 	fx->particles[i].poly[0].vx = (fx->scale.vx>>12);
// 	fx->particles[i].poly[1].vx = (-fx->scale.vx>>12);

	// Distance of this particle from the origin
//	SubVectorSSS( &fx->particles[i].pos, &fx->follow->position, &fx->origin );

	// Hack to make effect lower for platforms
//	if( fx->gravity )
//	{
//		SetVectorSF( &offset, &fx->normal );
//		ScaleVectorSF( (SVECTOR *)&offset, fx->gravity );
//		AddToVectorSS( &fx->particles[i].pos, (SVECTOR *)&offset );
//	}

	// Amount of drift - distance between this and the last particle scaled. Doesn't work for first one
//	if( i != fx->start )
//	{
//		int j = i-1;
//		if( j < 0 ) j = fx->numP-1;
//		SubVectorFSS( &fx->particles[i].vel, &fx->particles[i].pos, &fx->particles[j].pos );
//		ScaleVectorFF( &fx->particles[i].vel, fx->accn );
//	}

//	if( fx->type != FXTYPE_BILLBOARDTRAIL )
//#ifdef PSX_VERSION
//	{
// 		QuatToPSXMatrix( &fx->follow->qRot, (MATRIX *)fx->particles[i].rMtrx );
//	}
//#else
//	{
//		QuatToPCMatrix( &fx->follow->qRot, (MATRIX *)fx->particles[i].rMtrx );
//	}
//#endif
//}



/*	--------------------------------------------------------------------------------
	Function		: UpdateFXLightning
	Purpose			: Emperors hands effect
	Parameters		: 
	Returns			: void
	Info			: 
*/
void UpdateFXLightning( SPECFX *fx )
{
	PARTICLE *p;
	SVECTOR target, source;
	FVECTOR to;
	FVECTOR aim, ran, cross;
	fixed scale, fr, r;
	long i;

	// Find out where the lightning is being fired at
	if( fx->follow )
	{
		SetVectorSS( &target, &fx->follow->position );
		SubVectorFSS( &aim, &target, &fx->origin );
	}
	else
	{
		SetVectorFF( &aim, &fx->normal );
		ScaleVectorFF( &aim, fx->speed );
		AddVectorSFS( &target, &aim, &fx->origin );
	}

	// Find a route through space
	for( i=0,p=fx->particles; i<fx->numP; i++,p=p->next )
	{
		if( i )
		{
			scale = 4096/(fx->numP-i);
			fr = 4096-((i<<12)/fx->numP);
			// Get direction from last particle to target
			SetVectorSS( &source, &p->prev->pos );
			SubVectorFSS( &to, &target, &source );
			ScaleVectorFF( &to, scale );

			// Get a random direction, and then modify by the "unit" aim direction (general movement in direction of target)
			ran.vx = (Random(201)-100);
			ran.vy = (Random(201)-100);
			ran.vz = (Random(201)-100);

			MakeUnit( &ran );
			r = fx->accn * ((i>(fx->numP>>1))?(fx->numP-i):(i) * (MagnitudeF(&to)>>12));
			ScaleVectorFF( &ran, r );
			AddToVectorFF( &to, &ran );
			AddVectorSSF( &p->pos, &source, &to );
		}
		else
		{
			// Get direction from last particle to target
			SetVectorSS( &source, &p->pos );
			SubVectorFSS( &to, &target, &source );
		}

		// Push the polys out from the position
		MakeUnit( &to );
		CrossProductFFF( &cross, &to, &upVec );
		MakeUnit( &cross );
		ScaleVectorFF( &cross, fx->scale.vx );
		AddVectorSSF( &p->poly[0], &p->pos, &cross );
		SubVectorSSF( &p->poly[1], &p->pos, &cross );
	}

	if( actFrameCount > fx->lifetime )
		DeallocateFX(fx, 1);
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
	FVECTOR fwd;//, down;
	IQUATERNION q1, q2, q3;
	fixed t, speed;
	ACTOR *act = fx->act[0]->actor;
//	unsigned long i;

	// Randomly go to places
	if( DistanceBetweenPointsSF( &act->rotaim, &fx->vel ) < (100<<12) )
	{
		act->rotaim.vx = Random(500)-250;
		act->rotaim.vy = Random(500)-250;
		act->rotaim.vz = Random(500)-250;
	}

	SetQuaternion( &q1, &act->qRot );
	
	SubVectorFSF(&fwd,&act->rotaim,&fx->vel);
	MakeUnit(&fwd);

	CrossProductFFF((FVECTOR *)&q3,&inVec,&fwd);
	MakeUnit( (FVECTOR *)&q3 );
	t = DotProductFF(&inVec,&fwd);
	if (t<-4091)
		t=-4091;
	if (t>4092)
		t = 4092;
	if(t<4 && t>-4)
		t = 4;
	q3.w=arccos(t);
	fixedGetQuaternionFromRotation(&q2,&q3);

	speed = FMul(fx->accn ,gameSpeed);
	if( speed > 4092 ) speed = 4092;
	IQuatSlerp( &q1, &q2, speed, &act->qRot );

	// Forward motion
//	RotateVectorByQuaternionFF( &fwd, &inVec, &act->qRot );
	ScaleVectorFF( &fwd, FMul(fx->speed,gameSpeed) );
	AddToVectorFF( &fx->vel, &fwd );

	// Gravity
//	SetVectorFF( &down, &fx->normal );
//	ScaleVectorFF( &down, fx->gravity );
//	AddToVectorFF( &fx->vel, &down );

	// World coordinates
	if( !(tongue[0].type == TONGUE_GET_PICKUP && ((GARIB *)tongue[0].thing)->fx == fx) )
		AddVectorSFS( &act->position, &fx->vel, &fx->origin );
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
	SPRITE *s = fx->sprites;
	int fo, dead=0;

	if( fx->follow )
		SetVectorSS( &s->pos, &fx->follow->position );

	if( fx->type == FXTYPE_FLASH )
	{

		if( (actFrameCount-fx->tilt) > (fx->lifetime-fx->tilt)/2 )
		{
			fx->speed += FMul( fx->accn, gameSpeed );

			s->scaleX -= fx->speed>>12;
			s->scaleY -= fx->speed>>12;
		}
		else
		{
			fx->speed -= FMul( fx->accn, gameSpeed );
			if( fx->speed < 0 ) fx->speed = 0;

			s->scaleX += fx->speed>>12;
			s->scaleY += fx->speed>>12;
		}

		if( actFrameCount > fx->lifetime )
			DeallocateFX(fx, 1);
	}
	else
	{
		if( fx->type == FXTYPE_TWINKLE )
		{
			fo = Random((int)fx->fade) * (gameSpeed>>12);
			if( s->a > fo ) s->a -= fo;
			else
			{
				s->a = 0;
				s->draw = 0;
				dead = 1;
			}

			if( actFrameCount > fx->lifetime || dead )
				DeallocateFX(fx, 1);
		}

		// Use tilt to specify twinkliness - urgh
		if( fx->tilt >= 8192 )
		{
			int size = Random(50)-20;
			s->scaleX += size;
			s->scaleY += size;
			if((s->scaleX<<12) > (fx->scale.vx<<1))
				s->scaleX = fx->scale.vx>>11;
			else if((s->scaleX<<12) < (fx->scale.vx>>1))
				s->scaleX = fx->scale.vx>>13;
			if((s->scaleY<<12) > (fx->scale.vy<<1))
				s->scaleY = fx->scale.vy>>11;
			else if((s->scaleY<<12) < (fx->scale.vy>>1))
				s->scaleY = fx->scale.vy>>13;
		}
		if( fx->tilt )
		{
			s->a += Random(50)-25;
			if( s->a > 200 ) s->a = 200;
			else if( s->a < 50 ) s->a = 50;
		}
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
	long i;

	sfxList.head.next = sfxList.head.prev = &sfxList.head;

	sfxList.lastAdded = (SPECFX **)MALLOC0( sizeof(SPECFX *)*NUM_FXUPDATES );
	// Allocate a big bunch of effects
	sfxList.array = (SPECFX *)MALLOC0( sizeof(SPECFX)*MAX_SPECFX );
	sfxList.stack = (SPECFX **)MALLOC0( sizeof(SPECFX*)*MAX_SPECFX );

	// Initially, all effects are available
	for( i=0; i<MAX_SPECFX; i++ )
		sfxList.stack[i] = &sfxList.array[i];

	sfxList.count = 0;
	sfxList.stackPtr = i-1;

	// Allocate a big bunch of PLANE2s
	p2List.array = (PLANE2 *)MALLOC0( sizeof(PLANE2)*MAX_PLANES );
	p2List.stack = (PLANE2 **)MALLOC0( sizeof(PLANE2*)*MAX_PLANES );
	for( i=0; i<MAX_PLANES; i++ )
		p2List.stack[i] = &p2List.array[i];

	p2List.count = 0;
	p2List.stackPtr = i-1;

	txtrWaterRipples = FindTexture("AI_RIPPLE");
	txtrRipple = FindTexture("AI_RIPPLE2");
	txtrStar = FindTexture("STAR_OUTLINE");
	txtrSolidRing = FindTexture("AI_CIRCLE");
	txtrSmoke =	FindTexture("00SMOK07");
	txtrSmoke2 = FindTexture("AI_SMOKE");
	txtrRing = FindTexture("AI_RING");
	txtrBubble = FindTexture("WATDROP");
	txtrBlank = FindTexture("AI_FULLWHITE");
	txtrTrail =	FindTexture("AI_TRAIL");
	txtrFlash = FindTexture("FLASH");
	txtrFlare =	FindTexture("FLASH2");
	txtrElectric = FindTexture("ELECTRIC");
	txtrCroak = FindTexture("AI_CROAK");
	txtrWarpflash = FindTexture("AI_WARPFLASH");
}


/*	--------------------------------------------------------------------------------
	Function		: FreeSpecFXList
	Purpose			: frees the fx linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeSpecFXList()
{
	if( sfxList.array )
	{
		DeallocateFX( sfxList.head.next, sfxList.count );

		FREE( sfxList.array );
		sfxList.array = NULL;
	}

	if( sfxList.stack )
	{
		FREE( sfxList.stack );
		sfxList.stack = NULL;
	}

	if( sfxList.lastAdded )
	{
		FREE( sfxList.lastAdded );
		sfxList.lastAdded = NULL;
	}

	sfxList.count = 0;
	sfxList.stackPtr = 0;


	if( p2List.array )
	{
		FREE( p2List.array );
		p2List.array = NULL;
	}

	if( p2List.stack )
	{
		FREE( p2List.stack );
		p2List.stack = NULL;
	}

	p2List.count = 0;
	p2List.stackPtr = 0;
}


/*	--------------------------------------------------------------------------------
	Function		: AllocateFX
	Purpose			: Find a number of effects and return a sublist
	Parameters		: number of fx
	Returns			: pointer to first one
	Info			: 
*/
SPECFX *AllocateFX( int number, int type )
{
	SPECFX *s, *ptr;
	short update;

	// Return if allocation is impossible for any reason
	if( (number <= 0) || (sfxList.stackPtr-number < 0) || (number >= MAX_SPECFX-sfxList.count) ) return NULL;

	update = updateMapping[type];

	// Now we can go and allocate effects with gay abandon
	while( number-- )
	{
		s = sfxList.stack[sfxList.stackPtr--];

		if( sfxList.lastAdded[update] )
			ptr = sfxList.lastAdded[update];
		else
			ptr = sfxList.head.next;

		sfxList.lastAdded[update] = s;
		s->updateType = update;

		s->prev = ptr->prev;
		s->next = ptr;
		ptr->prev->next = s;
		ptr->prev = s;

		sfxList.count++;
	}

	return sfxList.stack[sfxList.stackPtr+1];
}


PLANE2 *AllocateP2( )
{
	PLANE2 *p;

	// Return if allocation is impossible for any reason
	if( !p2List.stackPtr || p2List.count >= MAX_PLANES ) return NULL;

	p = p2List.stack[p2List.stackPtr--];
	p2List.count++;

	return p2List.stack[p2List.stackPtr+1];
}

/*	--------------------------------------------------------------------------------
	Function		: DeallocateFX
	Purpose			: Remove fx from list and flag as unused
	Parameters		: number of fx
	Returns			: 
	Info			: 
*/
void DeallocateFX( SPECFX *head, int number )
{
	SPECFX *fx=head, *t;
	int i;

	if( !fx || !fx->next || (number<=0) || (sfxList.stackPtr+number >= MAX_SPECFX) ) 
		return;

	while( number-- )
	{
		t = fx->next;

		// If this is the last added effect of its type then either set the last added to be this effects sucessor if the same type,
		// or NULL if this effect is also the last of its kind
		if( fx == sfxList.lastAdded[fx->updateType] )
		{
			if( fx->next->updateType == fx->updateType )
				sfxList.lastAdded[fx->updateType] = fx->next;
			else
				sfxList.lastAdded[fx->updateType] = NULL;
		}

		fx->prev->next = fx->next;
		fx->next->prev = fx->prev;

		if( fx->sprites )
			DeallocateSprites( fx->sprites, fx->numP );

		if( fx->particles )
			DeallocateParticles( fx->particles, fx->numP );
//		{
//			if( fx->updateType == FXUPDATE_TRAIL )
//			{
//				i = fx->numP;
//				while(i--)
//					FREE( fx->particles[i].rMtrx );

//				FREE( fx->particles );
//			}
//			else 
//		}

		if( fx->rebound )
			DeallocateP2( fx->rebound );

		if( fx->act )
		{
			for( i=0; i<fx->numP; i++ )
				actor2Free(fx->act[i]);

			FREE( fx->act );
		}

		sfxList.count--;
		sfxList.stack[++sfxList.stackPtr] = fx;

		memset( fx, 0, sizeof(SPECFX) );

		fx = t;
	}
}

void DeallocateP2( PLANE2 *p )
{
	if( !p || p2List.stackPtr >= MAX_PLANES ) 
		return;

	p2List.count--;
	p2List.stack[++p2List.stackPtr] = p;

	memset( p, 0, sizeof(PLANE2) );
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
	PARTICLE *p;

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
	{
		if( fx->updateType == FXUPDATE_TRAIL )
		{
			for( i=0; i<fx->numP; i++ )
			{
				fx->particles[i].r = r;
				fx->particles[i].g = g;
				fx->particles[i].b = b;
			}
		}
		else
		{
			for( i=0,p=fx->particles; i<fx->numP; i++,p=p->next )
			{
				p->r = r;
				p->g = g;
				p->b = b;
			}
		}
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
	int r;
	SVECTOR rPos;
	FVECTOR up, normal;
	SPECFX *fx = NULL;
	fixed fxDist;
	ACTOR2 *act;
	GAMETILE *tile;
	unsigned long flags;//, t;
	PATH *path;

#ifdef NO_ATTACHEDEFFECTS
	return;
#endif

	// Nasty casting - should use generic structure for enemies and platforms"
	if( type == ENTITY_ENEMY )
	{
		ENEMY *nme = (ENEMY *)entity;

		act = nme->nmeActor;
		SetVectorFF( &normal, &nme->inTile->normal );
		tile = nme->inTile;
		flags = nme->flags;
		path = nme->path;
	}
	else if( type == ENTITY_PLATFORM )
	{
		PLATFORM *plt = (PLATFORM *)entity;

		act = plt->pltActor;
		SetVectorFF( &normal, &plt->inTile[0]->normal );
		tile = plt->inTile[0];
		flags = plt->flags;
		path = plt->path;
	}

	fxDist = DistanceBetweenPointsSS(&frog[0]->actor->position,&act->actor->position);

	if( (fxDist < ACTOR_DRAWDISTANCEOUTER) && (actFrameCount > act->fxCount) )
	{
		int ran;
		// Restart effect timer
		if( type == ENTITY_ENEMY && (flags & ENEMY_NEW_BABYFROG) ) 
			r = 57;
		else if( act->value1 )
		{
			if( act->effects & EF_RANDOMCREATE )
			{
				ran = Random( act->value1?act->value1:4096 );
				r = FDiv(245760, max(ran,1))>>12;
			}
			else
				r = FDiv(245760, max(act->value1,1))>>12;
		}
		else r = 60;

		act->fxCount = actFrameCount + r;

		if( act->effects & EF_RIPPLE_RINGS )
		{
			SetVectorSS( &rPos, &act->actor->position );
			rPos.vy = tile->centre.vy;
			if( act->effects & EF_FAST )
				fx = CreateSpecialEffect( FXTYPE_WAKE, &rPos, &normal, 40960, 1229, 410, 2048 );
			else if( act->effects & EF_SLOW )
				fx = CreateSpecialEffect( FXTYPE_WAKE, &rPos, &normal, 81920, 410, 205, 2048 );
			else
				fx = CreateSpecialEffect( FXTYPE_WAKE, &rPos, &normal, 81920, 820, 410, 2048 );

			SetAttachedFXColour( fx, act->effects );
		}
		if( act->effects & EF_SMOKE_STATIC )
		{
			if( act->effects & EF_FAST )
				fx = CreateSpecialEffect( FXTYPE_SMOKE_STATIC, &act->actor->position, &normal, 262144, 6144, 0, 4096 );
			else if( act->effects & EF_SLOW )
				fx = CreateSpecialEffect( FXTYPE_SMOKE_STATIC, &act->actor->position, &normal, 262144, 2048, 0, 4096 );
			else // EF_MEDIUM
				fx = CreateSpecialEffect( FXTYPE_SMOKE_STATIC, &act->actor->position, &normal, 262144, 4915, 0, 4096 );

			SetAttachedFXColour( fx, act->effects );
		}
		if( act->effects & EF_SMOKE_GROWS )
		{
			fixed gs = (path->nodes[path->fromNode].offset2)?(path->nodes[path->fromNode].offset2/SCALE):4096;

			if( act->effects & EF_FAST )
				fx = CreateSpecialEffect( FXTYPE_SMOKE_GROWS, &act->actor->position, &normal, 172032, 6144, gs, 4096);
			else if( act->effects & EF_SLOW )
				fx = CreateSpecialEffect( FXTYPE_SMOKE_GROWS, &act->actor->position, &normal, 172032, 2048, gs, 4096);
			else // EF_MEDIUM
				fx = CreateSpecialEffect( FXTYPE_SMOKE_GROWS, &act->actor->position, &normal, 172032, 4915, gs, 4096);
																																
			SetAttachedFXColour( fx, act->effects );																			
		}																														
		if( act->effects & EF_SPARKLYTRAIL )
		{
			if( act->effects & EF_FAST )
				fx = CreateSpecialEffect( FXTYPE_SPARKLYTRAIL, &act->actor->position, &normal, 81920, 16384, 0, 20480 );
			else if( act->effects & EF_SLOW )
				fx = CreateSpecialEffect( FXTYPE_SPARKLYTRAIL, &act->actor->position, &normal, 81920, 2048, 0, 20480 );
			else // EF_MEDIUM
				fx = CreateSpecialEffect( FXTYPE_SPARKLYTRAIL, &act->actor->position, &normal, 81920, 8192, 0, 20480 );

			if( fx )
			{
				SetVectorSS( &fx->rebound->point, &tile->centre );
				SetVectorFF( &fx->rebound->normal, &tile->normal );
				fx->gravity = act->radius*SCALE;

				SetAttachedFXColour( fx, act->effects );
			}
		}
		if( act->effects & EF_SPARKBURST )
		{
			if( act->effects & EF_FAST )
				fx = CreateSpecialEffect( FXTYPE_SPARKBURST, &act->actor->position, &normal, 61440, 16384, 0, 20480 );

			if( act->effects & EF_SLOW )
				fx = CreateSpecialEffect( FXTYPE_SPARKBURST, &act->actor->position, &normal, 61440, 2048, 0, 20480 );

			else // EF_MEDIUM
				fx = CreateSpecialEffect( FXTYPE_SPARKBURST, &act->actor->position, &normal, 61440, 8192, 0, 20480 );

			if( fx )
			{
//				SetVector( &fx->rebound->point, &tile->centre );
//				SetVector( &fx->rebound->normal, &tile->normal );
				fx->gravity = act->radius*SCALE;

				SetAttachedFXColour( fx, act->effects );
			}
		}
		if( act->effects & EF_SMOKEBURST )
		{
			fixed gs = (path->nodes[path->fromNode].offset2)?(path->nodes[path->fromNode].offset2/SCALE):4096;

			if( act->effects & EF_FAST )
				fx = CreateSpecialEffect( FXTYPE_SMOKEBURST, &act->actor->position, &normal, 204800, 20480, gs, 6963 );
			else if( act->effects & EF_SLOW )
				fx = CreateSpecialEffect( FXTYPE_SMOKEBURST, &act->actor->position, &normal, 204800, 2458, gs, 6963 );
			else // EF_MEDIUM
				fx = CreateSpecialEffect( FXTYPE_SMOKEBURST, &act->actor->position, &normal, 204800, 8192, gs, 6963 );

			SetAttachedFXColour( fx, act->effects );
		}
		if( act->effects & EF_FIERYSMOKE )
		{
			fixed gs = (path->nodes[path->fromNode].offset2)?(path->nodes[path->fromNode].offset2/SCALE):4096;

			if( act->effects & EF_FAST )
				fx = CreateSpecialEffect( FXTYPE_FIERYSMOKE, &act->actor->position, &normal, 204800, 16384, gs, 10240 );
			else if( act->effects & EF_SLOW )
				fx = CreateSpecialEffect( FXTYPE_FIERYSMOKE, &act->actor->position, &normal, 204800, 4096, gs, 10240 );
			else // EF_MEDIUM
				fx = CreateSpecialEffect( FXTYPE_FIERYSMOKE, &act->actor->position, &normal, 204800, 10240, gs, 10240 );
		}
		if( act->effects & EF_BUBBLES )
		{
 			if( act->effects & EF_FAST )
 				fx = CreateSpecialEffect( FXTYPE_BUBBLES, &act->actor->position, &normal, 48192, 4096, 0, 2048 );
 			else if( act->effects & EF_SLOW )
 				fx = CreateSpecialEffect( FXTYPE_BUBBLES, &act->actor->position, &normal, 48192, 810, 0, 2048 );
 			else // EF_MEDIUM
 				fx = CreateSpecialEffect( FXTYPE_BUBBLES, &act->actor->position, &normal, 48192, 2048, 0, 2048 );

			if( fx )
			{
				fx->rebound = AllocateP2();
				SetVectorFF( &up, &path->nodes[0].worldTile->normal );
				SetVectorFF( &fx->rebound->normal, &up );
				ScaleVectorFF( &up, act->radius );
				AddVectorSSF( &fx->rebound->point, &act->actor->position, &up );

				SetAttachedFXColour( fx, act->effects );
			}
		}
		if( act->effects & EF_TWINKLE )
		{
			fx = CreateSpecialEffect( FXTYPE_TWINKLE, &act->actor->position, &normal, 81920, 0, 0, ToFixed(Random(3)+1) );
			SetAttachedFXColour( fx, act->effects );
			fx->follow = act->actor;

			if( act->effects & EF_FAST )
				fx->tilt = 12288;
			else if( act->effects & EF_MEDIUM )
				fx->tilt = 8192;
			else if( act->effects & EF_SLOW )
				fx->tilt = 4096;
		}
		if( act->effects & EF_GREENGLOOP )
		{
			if( act->effects & EF_FAST )
				fx = CreateSpecialEffect( FXTYPE_GREENGLOOP, &act->actor->position, &normal, act->radius, 6144, 0, 2048 );
			else if( act->effects & EF_SLOW )
				fx = CreateSpecialEffect( FXTYPE_GREENGLOOP, &act->actor->position, &normal, act->radius, 1229, 0, 2048 );
			else // EF_MEDIUM
				fx = CreateSpecialEffect( FXTYPE_GREENGLOOP, &act->actor->position, &normal, act->radius, 2867, 0, 2048 );

			if( fx )
			{
				fx->rebound = AllocateP2();
				SetVectorFF( &up, &path->nodes[0].worldTile->normal );
				SetVectorFF( &fx->rebound->normal, &up );
				ScaleVectorFF( &up, act->radius );
				AddVectorSSF( &fx->rebound->point, &act->actor->position, &up );

				SetAttachedFXColour( fx, act->effects );
			}
		}
	}						
	if( act->effects & EF_GLOW )
	{
		fx = CreateSpecialEffect( FXTYPE_GLOW, &act->actor->position, &normal, 491520, 0, 0, 0 );
		SetAttachedFXColour( fx, act->effects );
		fx->follow = act->actor;

		if( act->effects & EF_FAST )
			fx->tilt = 8192;
		else if( act->effects & EF_MEDIUM )
			fx->tilt = 4096;
		else if( act->effects & EF_SLOW )
			fx->tilt = 0;

		if( fx ) act->effects &= ~EF_GLOW;
	}
	if( (act->effects & EF_BUTTERFLYSWARM) || (act->effects & EF_BATSWARM) || (act->effects & EF_SPACETHING1) )
	{
		if( act->effects & EF_BATSWARM )
		{
			fx = CreateSpecialEffect( FXTYPE_BATSWARM, &act->actor->position, &normal, act->radius, 0, 0, act->value1 );
			act->effects &= ~EF_BATSWARM;
		}
		else if( act->effects & EF_BUTTERFLYSWARM )
		{
			fx = CreateSpecialEffect( FXTYPE_BUTTERFLYSWARM, &act->actor->position, &normal, act->radius, 0, 0, act->value1 );
			act->effects &= ~EF_BUTTERFLYSWARM;
			if( fx && type == ENTITY_ENEMY && (flags & ENEMY_NEW_FLAPPYTHING) )
			{
				fx->rebound = AllocateP2();
				GetPositionForPathNode( &rPos, &path->nodes[0] );
				SetVectorSS( &fx->rebound->point, &rPos );
				SetVectorFF( &fx->rebound->normal, &path->nodes[0].worldTile->normal );
			}
		}
		else
		{
			fx = CreateSpecialEffect( FXTYPE_SPACETHING1, &act->actor->position, &normal, act->radius, 0, 0, act->value1 );
			act->effects &= ~EF_SPACETHING1;
		}

		if( fx ) fx->follow = act->actor;
	}
	if( act->effects & EF_TRAIL )
	{
		if( act->effects & EF_FAST )
			fx = CreateSpecialEffect( FXTYPE_TRAIL, &act->actor->position, &normal, act->value1, 3891, 0, 2458 );
		else if( act->effects & EF_SLOW )
			fx = CreateSpecialEffect( FXTYPE_TRAIL, &act->actor->position, &normal, act->value1, 3891, 0, 12288 );
		else
			fx = CreateSpecialEffect( FXTYPE_TRAIL, &act->actor->position, &normal, act->value1, 3891, 0, 4096 );

		if( fx )
		{
			if( type == ENTITY_PLATFORM )
				fx->gravity = -1228800;

			fx->follow = act->actor;
			SetAttachedFXColour( fx, act->effects );
			act->effects &= ~EF_TRAIL;
		}
	}
	else if( act->effects & EF_BILLBOARDTRAIL )
	{
		if( act->effects & EF_FAST )
			fx = CreateSpecialEffect( FXTYPE_BILLBOARDTRAIL, &act->actor->position, &normal, act->value1, 3891, 0, 2458 );
		else if( act->effects & EF_SLOW )
			fx = CreateSpecialEffect( FXTYPE_BILLBOARDTRAIL, &act->actor->position, &normal, act->value1, 3891, 0, 12288 );
		else
			fx = CreateSpecialEffect( FXTYPE_BILLBOARDTRAIL, &act->actor->position, &normal, act->value1, 3891, 0, 8192 );

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
	FVECTOR up, surface;
//	int i;

	SetVectorFF( &up, &parent->normal );

	SetVectorFF( &surface, &up );
	ScaleVectorFF( &surface, parent->scale.vx );
	AddToVectorFS( &surface, &parent->origin );

	// Create the second wave of bubbles
	if( (fx = CreateSpecialEffect( FXTYPE_BUBBLES, &parent->origin, &up, 90112, 2867, 0, 2048 )) )
	{
		fx->rebound = AllocateP2();
		SetVectorFF( &fx->rebound->normal, &up );
		SetVectorSF( &fx->rebound->point, &surface );

		SetFXColour( fx, parent->r, parent->g, parent->b );
	}
	// Green smoke created at the _surface_ of the water
	fx = CreateSpecialEffect( FXTYPE_SMOKEBURST, (SVECTOR*)&surface, &up, 204800, 4096, 0, 6963 );
	SetFXColour( fx, parent->r, parent->g, parent->b );

	// Ditto for splash
	if( (fx = CreateSpecialEffect( FXTYPE_SPLASH, (SVECTOR*)&surface, &up, 57344, 10240, 0, 8192 ) ))
	{
		fx->gravity = 4100;
		fx->rebound = AllocateP2();
		SetVectorFF( &fx->rebound->normal, &up );
		SetVectorSF( &fx->rebound->point, &surface );
		SetFXColour( fx, parent->r, parent->g, parent->b );
	}
}


//void CreateLightningEffect( SVECTOR *p1, SVECTOR *p2, unsigned long effects, long life )
void CreateLightningEffect( SVECTOR *p1, SVECTOR *p2, unsigned long effects, long life, int zDepthOff)
{
	FVECTOR dir;
	fixed distance, accn, lifetime;
	short /*rn,*/ i, draw=0;
	SPECFX *fx;

	if( OutcodeCheck(p1,p2) )
		return;

	SubVectorFSS( &dir, p1, p2 );

	if( !(dir.vx || dir.vy || dir.vz) )
		return;

	// Do it this way cos we need distance later - already checked for a non-zero vector so that's ok
	distance = MagnitudeF( &dir );
	dir.vx = FDiv(dir.vx, distance);
	dir.vy = FDiv(dir.vy, distance);
	dir.vz = FDiv(dir.vz, distance);

	if( effects & EF_SLOW ) i=2;
	else if( effects & EF_MEDIUM ) i=3;
	else if( effects & EF_FAST ) i=4;
	else i=1;

	accn = FDiv(25<<12,distance);
	lifetime = (life<<12)/60;
	specFXAlpha = 180;

	for( ; i; i-- )
	{
		if( (fx = CreateSpecialEffectDirect(FXTYPE_LIGHTNING, p2, &dir, 20480, distance, accn, lifetime)) )
		{
			SetAttachedFXColour( fx, effects );

			// Randomise colours a bit
//			rn = fx->r>>1;
//			rn += Random(rn+1)-(rn>>1);
//			if( rn > 255 ) fx->r = 255;
//			else if( rn < 0 ) fx->r = 0;
//			else fx->r = rn;

//			rn = fx->g>>1;
//			rn += Random(rn+1)-(rn>>1);
//			if( rn > 255 ) fx->g = 255;
//			else if( rn < 0 ) fx->g = 0;
//			else fx->g = rn;

//			rn = fx->b>>1;
//			rn += Random(rn+1)-(rn>>1);
//			if( rn > 255 ) fx->b = 255;
//			else if( rn < 0 ) fx->b = 0;
//			else fx->b = rn;

#ifdef PSX_VERSION
			fx->zDepthOff = zDepthOff;
#endif
		}
	}

	specFXAlpha = 255;
}


/*	--------------------------------------------------------------------------------
	Function		: CreateBlastRing
	Purpose			: Make a generic ring of polys
	Parameters		: 
	Returns			: void
	Info			: 
*/
void CreateBlastRing( )
{
	fixed rs, rc, pB, arcStep = 4096 / NUM_RINGSEGS;
	unsigned long i, v;

	if( !ringVtx )
		ringVtx = (FVECTOR *)MALLOC0(sizeof(FVECTOR)*(NUM_RINGSEGS+1)*2);

	for( i=0,v=0,pB=0; i<NUM_RINGSEGS; i++,v+=2,pB+=arcStep )
	{
		rs = rsin(pB);
		rc = rcos(pB);

		ringVtx[v].vx = rs;
		ringVtx[v].vy = (i&1)?2048:-2048;
		ringVtx[v].vz = rc;
		ringVtx[v+1].vx = rs;
		ringVtx[v+1].vy = (i&1)?-2048:2048;
		ringVtx[v+1].vz = rc;
	}
}

void CreatePickupEffect( int pl, char r1, char g1, char b1, char r2, char g2, char b2 )
{
	SVECTOR pos;
	FVECTOR seUp;
	SPECFX *fx;

	SetVectorFF( &seUp, &currTile[pl]->normal );
	ScaleVector( &seUp, 200 );
	AddVectorSFS( &pos, &seUp, &frog[pl]->actor->position );
	if( (fx = CreateSpecialEffectDirect( FXTYPE_SPARKLYTRAIL, &pos, &currTile[pl]->normal, 204800, 12288, 0, 20480 )) )
	{
		SetFXColour( fx, r1, g1, b1 );
		fx->gravity = 2870;
	}
	if( (fx = CreateSpecialEffectDirect( FXTYPE_SPARKLYTRAIL, &pos, &currTile[pl]->normal, 163480, 10240, 0, 24576 )) )
	{
		SetFXColour( fx, r2, g2, b2 );
		fx->gravity = 2870;
	}
	if( (fx = CreateSpecialEffectDirect( FXTYPE_SPARKLYTRAIL, &pos, &currTile[pl]->normal, 122880, 8192, 0, 28672 )) )
	{
		SetFXColour( fx, r1, g1, b1 );
		fx->gravity = 2870;
	}
	if( (fx = CreateSpecialEffectDirect( FXTYPE_SPARKLYTRAIL, &pos, &currTile[pl]->normal, 81920, 6144, 0, 32768 )) )
	{
		SetFXColour( fx, r2, g2, b2 );
		fx->gravity = 2870;
	}
}

void CreateRingEffect( int i )
{
	FVECTOR up;
	SVECTOR pos;
	SPECFX *fx;

	SetVectorFF( &up, &currTile[i]->normal );
	ScaleVector( &up, (rsin(actFrameCount<<6)>>4)+250 );
	SetVectorSF( &pos, &up );
	AddToVectorSS( &pos, &frog[i]->actor->position );
	fx = CreateSpecialEffectDirect( FXTYPE_WAKE, &pos, &currTile[i]->normal, 123840, 1024, 0, 1024 );
	SetFXColour( fx, frogPool[player[i].character].r, frogPool[player[i].character].g, frogPool[player[i].character].b );
}



void BabyCollectEffect( ACTOR2 *baby, GAMETILE *tile, int i )
{
	SPECFX *fx;
	SVECTOR pos;
	FVECTOR up;

	SetVectorFF( &up, &tile->normal );
	ScaleVector( &up, 200 );
	AddVectorSFS( &pos, &up, &baby->actor->position );
	fx = CreateSpecialEffectDirect( FXTYPE_WAKE, &pos, &tile->normal, 65536, 4096, 82, 4096 );
	SetFXColour( fx, babyList[i].fxColour[0], babyList[i].fxColour[1], babyList[i].fxColour[2] );

	// Ring 2
	AddToVectorSF( &pos, &up );
	fx = CreateSpecialEffectDirect( FXTYPE_WAKE, &pos, &tile->normal, 32768, 2048, 82, 4915 );
	SetFXColour( fx, babyList[i].fxColour[0], babyList[i].fxColour[1], babyList[i].fxColour[2] );

	// Ring 3
	AddToVectorSF( &pos, &up );
	fx = CreateSpecialEffectDirect( FXTYPE_WAKE, &pos, &tile->normal, 16384, 1229, 82, 5734 );
	SetFXColour( fx, babyList[i].fxColour[0], babyList[i].fxColour[1], babyList[i].fxColour[2] );
	
	// Ring 4
	AddToVectorSF( &pos, &up );
	fx = CreateSpecialEffectDirect( FXTYPE_WAKE, &pos, &tile->normal, 8192, 409, 82, 6554 );
	SetFXColour( fx, babyList[i].fxColour[0], babyList[i].fxColour[1], babyList[i].fxColour[2] );

	// Ring 5
	AddToVectorSF( &pos, &up );
	fx = CreateSpecialEffectDirect( FXTYPE_WAKE, &pos, &tile->normal, 4096, 205, 82, 7373 );
	SetFXColour( fx, babyList[i].fxColour[0], babyList[i].fxColour[1], babyList[i].fxColour[2] );

	actorAnimate( baby->actor, BABY_ANIM_COLLECT, NO, NO, 180, 0 );
	actorAnimate( baby->actor, BABY_ANIM_COLLECTHOLD, YES, YES, 64, 0 );
	
	actorAnimate(frog[0]->actor,FROG_ANIM_DANCE4,NO,NO,77,0);
	actorAnimate(frog[0]->actor,FROG_ANIM_BREATHE,YES,YES,102,0);
}
