/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: types2dgfx.h
	Programmer	: Andy Eder
	Date		: 20/04/99 09:13:49

----------------------------------------------------------------------------------------------- */

#ifndef TYPES2DGFX_H_INCLUDED
#define TYPES2DGFX_H_INCLUDED


//----- [ DEFINES ] ----------------------------------------------------------------------------//

#define MAX_SPR_FRAMES	32


//----- [ DATA STRUCTURES ] --------------------------------------------------------------------//

typedef struct TAGBACKDROP
{
	uSprite		N64Spr;
	TEXTURE		*texture;
	int			xPos,yPos;
	short		scaleX,scaleY;
	short		flipX,flipY;
	short		draw;
	UBYTE		r,g,b,a;
} BACKDROP;


typedef struct TAGTEXTOVERLAY
{
	struct		TAGTEXTOVERLAY *next,*prev;

	char		*text;
	char		flags;
	int			ID;
	float		waveAmplitude;
	float		waveStart;
	short		xPos,yPos,zPos;
	short		xPosOld,yPosOld;
	short		centred;
	
	short		kill;
	short		life;

	UBYTE		r,g,b,a,oa;

	UBYTE		draw;
	FONT		*font;

} TEXTOVERLAY;


typedef struct TAGSPRITEOVERLAY
{
	struct		TAGSPRITEOVERLAY *next,*prev;

	short		xPos;
	short		yPos;
	short		width;
	short		height;

	short		flags;

	float		velocityX;
	float		velocityY;
	
	UBYTE		r,g,b,a;
	UBYTE		draw;

	short		numFrames;
	short		currFrame;
	float		animTime;
	float		animSpeed;
	TEXTURE		*frames[MAX_SPR_FRAMES];

} SPRITEOVERLAY;


typedef struct TAGSPRITE3D
{
	struct		TAGSPRITE3D *next;

	TEXTURE		*texture;
	float		xPos;
	float		yPos;
	short		zPos;

	float		scaleX;
	float		scaleY;

	UBYTE		r,g,b,a;
	UBYTE		draw;

	Sprite		spr;
	Bitmap		*bmp;

} SPRITE3D;



//----- [ SPECIAL FX ] --------------------------------------------------------------------------//

typedef struct TAGFX_RIPPLE
{
	struct TAGFX_RIPPLE	*next;
	struct TAGFX_RIPPLE	*prev;

	unsigned char		rippleType;

	VECTOR				origin;
	VECTOR				normal;

	Vtx					verts[4];
	unsigned char		r,g,b;
	unsigned char		alpha;
	unsigned char		alphaSpeed;

	float				radius;
	float				velocity;
	float				accel;
	
	float				yRot;
	float				yRotSpeed;
	
	float				lifetime;
	unsigned char		deadCount;

	TEXTURE				*txtr;

} FX_RIPPLE;


typedef struct TAGFX_SMOKE
{
	struct TAGFX_SMOKE	*next;
	struct TAGFX_SMOKE	*prev;

	unsigned char		smokeType;
	SPRITE				sprite;

	unsigned char		alphaSpeed;

	float				radius;
	float				velocity;
	float				accel;

	float				lifetime;
	unsigned char		deadCount;

} FX_SMOKE;


#define MAX_SWARM_ELEMENTS		4

typedef struct TAGFX_SWARM
{
	struct TAGFX_SWARM	*next;
	struct TAGFX_SWARM	*prev;

	unsigned char		swarmType;
	SPRITE				sprite[MAX_SWARM_ELEMENTS];

	VECTOR				centroid;
	
	float				xVelocity[MAX_SWARM_ELEMENTS];
	float				yVelocity[MAX_SWARM_ELEMENTS];
	float				zVelocity[MAX_SWARM_ELEMENTS];

	float				lifetime;
	unsigned char		deadCount;

} FX_SWARM;


#define MAX_EXPLODE_PARTICLES	8

typedef struct TAGFX_EXPLODEPARTICLE
{
	struct TAGFX_EXPLODEPARTICLE	*next;
	struct TAGFX_EXPLODEPARTICLE	*prev;

	unsigned char		explodeType;
	SPRITE				sprite[MAX_EXPLODE_PARTICLES];
	char				alphaDecay[MAX_EXPLODE_PARTICLES];

	char				hasHitPlane[MAX_EXPLODE_PARTICLES];
	PLANE2				reboundPlane;

	VECTOR				origin;
	VECTOR				normal;

	float				initialSpeed[MAX_EXPLODE_PARTICLES];
	VECTOR				velocity[MAX_EXPLODE_PARTICLES];

	float				lifetime;
	unsigned char		deadCount;

} FX_EXPLODEPARTICLE;



typedef struct TAGBACKPANEL
{
	VECTOR				pos;
	float				width;
	float				height;

	UBYTE				r,g,b,a;
	TEXTURE				*txtr;
	Vtx					verts[4];

} BACKPANEL;


#define MOTION_BLUR			(1<<0)	// Standard effect - sets up basic blur data
#define VERTEX_WODGE		(1<<1)  // Wibble vertices to make water / haze effect
#define TILE_SHRINK_HORZ	(1<<2)	// Start shrinking tiles to nothing horizontally
#define TILE_SHRINK_VERT	(1<<3)  // Vertically
#define SHRINK_TO_POINT		(1<<4)	// Shrink whole overlay to point
#define MEZZOTINT			(1<<5)  // Scale tiles so they don't quite join - mad window effect
#define DES_REP_MODE		(1<<6)	// Tile image bizarrely, in a Designers Republic stylee
#define BARS_HORZ			(1<<7)	// Horizontal glass bars
#define BARS_VERT			(1<<8)  // Vertical
#define RECALC_VTX			(1<<9)	// User wants vertices recalculating anyway.
#define USE_GRAB_BUFFER		(1<<10)	// Use a static buffer instead of screen. Created by Screen2Texture

#define BLUR_HEAVY			(1<<20)	// High alpha on overlay
#define BLUR_MEDIUM			(1<<21) // Medium
#define BLUR_LIGHT			(1<<22) // Low
#define OVERLAY_SOLID		(1<<23) // No translucency

#define BLUR_CENTRE			(1<<23) // Blur only when moving
#define BLUR_INWARD			(1<<24) // Constant blur towards screen centre
#define BLUR_OUTWARD		(1<<25) // Outwards

#define TINT_RED			(1<<26) // Red tint overlay
#define TINT_GREEN			(1<<27) // Green
#define TINT_BLUE			(1<<28) // Blue
#define TINT_SHADE			(1<<29) // Dark

#define NO_EFFECT			0
#define PAUSE_EXIT			1       // Shrinking screen out after pause
#define FROG_DEATH_OUT		2		// Fade to red when frog is dead
#define FROG_DEATH_IN		3		// Go back to game when frog respawns

typedef struct TAGGRABSTRUCT
{
	long xTS, maxxTS,  // size and max size of texture tiles
		yTS, maxyTS,
		xOff, yOff, zOff;  // Offset of mesh on screen
	char vR, vG, vB,       // Vertex rgb
		pR, pG, pB,        // Primitive rgb
		eR, eG, eB,        // environment rgb
		alpha;             // Translucency of mesh
	short calcVtx;         // Recalculate vertices?
	short dynVtx;          // Determines the setting of calcVtx
	short afterEffect;     // Lets effects run over into game time after pause screen, for example
	long fxTimer;		   // Random timer for stuff
	unsigned long flags;
	// For vertex wodging
	float sinAmt;          // Amount to wodge
	float sinSpeed;        // Speed of wodge
	// For tile scaling operations
	float maxScale;        // Max scale of tiles
	float speedScale;      // Speed of scaling
	float scale;           // Current scale

} GRABSTRUCT;


extern short drawScreenGrab;
extern short grabFlag;

extern GRABSTRUCT grabData;

extern void Screen2Texture();
extern void DrawScreenGrab( unsigned long flags );
extern void SetGrabData( );
extern void FreeGrabData( );

#endif
