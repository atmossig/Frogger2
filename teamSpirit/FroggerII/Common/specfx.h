/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: specfx.h
	Programmer	: Andy Eder
	Date		: 14/05/99 09:32:16

----------------------------------------------------------------------------------------------- */

#ifndef SPECFX_H_INCLUDED
#define SPECFX_H_INCLUDED


enum
{
	RIPPLE_TYPE_CROAK,
	RIPPLE_TYPE_WATER,
	RIPPLE_TYPE_DUST,
	RIPPLE_TYPE_PICKUP,
	RIPPLE_TYPE_RING,
	RIPPLE_TYPE_TELEPORT,
	RIPPLE_TYPE_NUMTYPES,
};

enum
{
	SMOKE_TYPE_NORMAL,
	SMOKE_TYPE_FLAMING,
	SMOKE_TYPE_POWERUP,
};

enum
{
	SWARM_TYPE_STARSTUN,
	SWARM_TYPE_FLIES,
};

enum
{
	EXPLODEPARTICLE_TYPE_NORMAL,
	EXPLODEPARTICLE_TYPE_TRIGGERRIPPLE,
	EXPLODEPARTICLE_TYPE_COLOURBURST,
};



//----- [ RIPPLE SPECIAL FX ] -----//

typedef struct TAGFX_RIPPLELIST
{
	int					numEntries;
	FX_RIPPLE			head;

} FX_RIPPLELIST;

extern FX_RIPPLELIST rippleFXList;


//----- [ SMOKE SPECIAL FX ] -----//

typedef struct TAGFX_SMOKELIST
{
	int					numEntries;
	FX_SMOKE			head;

} FX_SMOKELIST;

extern FX_SMOKELIST smokeFXList;


//----- [ SWARM SPECIAL FX ] -----//

typedef struct TAGFX_SWARMLIST
{
	int					numEntries;
	FX_SWARM			head;

} FX_SWARMLIST;

extern FX_SWARMLIST swarmFXList;


//----- [ PAPTICLE EXPLODE SPECIAL FX ] -----//

typedef struct TAGFX_EXPLODEPARTICLELIST
{
	int					numEntries;
	FX_EXPLODEPARTICLE	head;

} FX_EXPLODEPARTICLELIST;

extern FX_EXPLODEPARTICLELIST explodeParticleFXList;


extern VECTOR debug_globalVectorPoint;
extern VECTOR debug_globalVectorNormal;


//----- [ FUNCTION PROTOTYPES ] -----------------------------------------------------------------//

extern FX_RIPPLE *CreateAndAddFXRipple(char rippleType,VECTOR *origin,VECTOR *normal,float size,float velocity,float acceleration,float lifetime);
extern void FreeFXRippleLinkedList();
extern void AddFXRipple(FX_RIPPLE *ripple);
extern void SubFXRipple(FX_RIPPLE *ripple);
extern void UpdateFXRipples();

extern FX_SMOKE *CreateAndAddFXSmoke(char smokeType,VECTOR *origin,short size,float velocity,float acceleration,float lifetime);
extern void FreeFXSmokeLinkedList();
extern void AddFXSmoke(FX_SMOKE *smoke);
extern void SubFXSmoke(FX_SMOKE *smoke);
extern void UpdateFXSmoke();

extern FX_SWARM *CreateAndAddFXSwarm(char swarmType,VECTOR *centroid,short size,float lifetime);
extern void FreeFXSwarmLinkedList();
extern void AddFXSwarm(FX_SWARM *swarm);
extern void SubFXSwarm(FX_SWARM *swarm);
extern void UpdateFXSwarm();

extern FX_EXPLODEPARTICLE *CreateAndAddFXExplodeParticle(char explodeType,VECTOR *origin,VECTOR *normal,float maxSpeed,short size,PLANE2 *reboundPlane,float lifetime);
extern void FreeFXExplodeParticleLinkedList();
extern void AddFXExplodeParticle(FX_EXPLODEPARTICLE *explode);
extern void SubFXExplodeParticle(FX_EXPLODEPARTICLE *explode);
extern void UpdateFXExplodeParticle();


extern void InitFXLinkedLists();
extern void UpdateSpecialFX();

extern void CalcBounce(VECTOR *vel,VECTOR *normal);



#endif
