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


#endif
