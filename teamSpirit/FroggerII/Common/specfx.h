/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: specfx.h
	Programmer	: Andy Eder
	Date		: 14/05/99 09:32:16

----------------------------------------------------------------------------------------------- */

#ifndef SPECFX_H_INCLUDED
#define SPECFX_H_INCLUDED


//----- [ DEFINES & ENUMS ] --------------------------------------------------------------------//

#define FADE_IN				0
#define FADE_OUT			1

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
	EXPLODEPARTICLE_TYPE_NORMAL,
	EXPLODEPARTICLE_TYPE_TRIGGERRIPPLE,
	EXPLODEPARTICLE_TYPE_SMOKEBURST,
};



//----- [ RIPPLE SPECIAL FX ] -----//

typedef struct TAGFX_RIPPLELIST
{
	int					numEntries;
	FX_RIPPLE			head;

} FX_RIPPLELIST;


//----- [ SMOKE SPECIAL FX ] -----//

typedef struct TAGFX_SMOKELIST
{
	int					numEntries;
	FX_SMOKE			head;

} FX_SMOKELIST;


//----- [ PAPTICLE EXPLODE SPECIAL FX ] -----//

typedef struct TAGFX_EXPLODEPARTICLELIST
{
	int					numEntries;
	FX_EXPLODEPARTICLE	head;

} FX_EXPLODEPARTICLELIST;


//----- [ GLOBALS ] -----------------------------------------------------------------------------//

extern FX_RIPPLELIST rippleFXList;
extern FX_SMOKELIST smokeFXList;
extern FX_EXPLODEPARTICLELIST explodeParticleFXList;

//----- [ TEXTURES USED FOR SPECIAL FX ] -----//

extern TEXTURE *txtrRipple;
extern TEXTURE *txtrWake;
extern TEXTURE *txtrStar;
extern TEXTURE *txtrRing;
extern TEXTURE *txtrSmoke;

extern char doScreenFade;
extern char	fadeDir;
extern short fadeOut;
extern short fadeStep;

extern char pauseMode;
extern UBYTE testR,testG,testB,testA;


//----- [ FUNCTION PROTOTYPES ] -----------------------------------------------------------------//

extern FX_RIPPLE *CreateAndAddFXRipple(char rippleType,VECTOR *origin,VECTOR *normal,float size,float velocity,float acceleration,float lifetime);
extern void FreeFXRippleLinkedList();
extern void AddFXRipple(FX_RIPPLE *ripple);
extern void SubFXRipple(FX_RIPPLE *ripple);
extern void UpdateFXRipples();

extern FX_SMOKE *CreateAndAddFXSmoke(VECTOR *origin,short size,float lifetime);
extern void FreeFXSmokeLinkedList();
extern void AddFXSmoke(FX_SMOKE *smoke);
extern void SubFXSmoke(FX_SMOKE *smoke);
extern void UpdateFXSmoke();

extern FX_EXPLODEPARTICLE *CreateAndAddFXExplodeParticle(char explodeType,VECTOR *origin,VECTOR *normal,float maxSpeed,short size,PLANE2 *reboundPlane,float lifetime);
extern void FreeFXExplodeParticleLinkedList();
extern void AddFXExplodeParticle(FX_EXPLODEPARTICLE *explode);
extern void SubFXExplodeParticle(FX_EXPLODEPARTICLE *explode);
extern void UpdateFXExplodeParticle();

extern void InitFXLinkedLists();
extern void UpdateSpecialFX();

extern void CalcBounce(VECTOR *vel,VECTOR *normal);



#endif
