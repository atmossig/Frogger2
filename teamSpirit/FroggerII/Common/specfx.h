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

extern char doScreenFade;
extern char	fadeDir;
extern short fadeOut;
extern short fadeStep;

extern char pauseMode;


// These are used in the editor and translated into effect parameters later
#define EF_RIPPLE_RINGS					(1 << 0)	// Lillypad thing
#define EF_SMOKE_STATIC					(1 << 1)	// Smoke that doesn't grow
#define EF_SPARK_BURSTS					(1 << 2)	// 
#define EF_FLAMEBURST					(1 << 3)	// Vent up
#define EF_FLYSWARM						(1 << 4)
#define EF_BUBBLES						(1 << 5)
#define EF_SMOKE_GROWS					(1 << 6)	// Grows as it fades

#define EF_FAST							(1 << 26)
#define EF_MEDIUM						(1 << 27)
#define EF_SLOW							(1 << 28)
#define EF_TINTRED						(1 << 29)
#define EF_TINTGREEN					(1 << 30)
#define EF_TINTBLUE						(1 << 31)


// Preset types. Any other can be made by selecting custom and specifying all the stucture members by hand
enum
{
	FXTYPE_CUSTOM,
	FXTYPE_WATERRIPPLE,
	FXTYPE_GARIBCOLLECT,
	FXTYPE_JUMPBLUR,
	FXTYPE_FROGSTUN,
	FXTYPE_SMOKE_STATIC,
	FXTYPE_SMOKE_GROWS,
	FXTYPE_BASICRING,
	FXTYPE_SPLASH,
	FXTYPE_SMOKEBURST,
	FXTYPE_FIREVENT,
	FXTYPE_FLYSWARM,
	FXTYPE_BUBBLES,

	FXTYPE_NUMTYPES
};

typedef struct
{
	VECTOR pos, vel;
	unsigned char r, g, b, a, bounce;

} PARTICLE;


typedef struct TAGSPECFX
{
	struct TAGSPECFX *next, *prev;

	VECTOR normal, origin, vel;
	PLANE2 *rebound;
	PARTICLE *particles;						// For swarm, explosions etc.

	Vtx *verts;									// Persistent vertices on N64

	short type, fade;
	float speed, accn, angle, size, spin;
	long lifetime, deadCount, numP;				// numP is number of particles

	unsigned char r, g, b, a;

	SPRITE *sprites;
	TEXTURE *tex;
	ACTOR *follow;								// Go where it goes

	int (*Update) ();							// Just like C++
	void (*Draw) ();							// Update and draw functions, specified for different types.

} SPECFX;


typedef struct
{
	SPECFX head;
	int numEntries;

} SPECFXLIST;


extern SPECFXLIST specFXList;


extern SPECFX *CreateAndAddSpecialEffect( short type, VECTOR *origin, VECTOR *normal, int size, float speed, float accn, float lifetime );

extern void UpdateSpecialEffects( );

extern void AddSpecFX( SPECFX *fx );
extern void SubSpecFX( SPECFX *fx );
extern void InitSpecFXList( );
extern void FreeSpecFXList( );

extern void CreateTeleportEffect( VECTOR *pos, VECTOR *normal );

#endif

