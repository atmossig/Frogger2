/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: types.h
	Programmer	: Andy Eder
	Date		: 15/04/99 14:17:22

----------------------------------------------------------------------------------------------- */

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED


#define	SCREEN_HT	240

#define X 0
#define Y 1
#define Z 2

#define YES 1
#define NO  0
#define ON  1
#define OFF 0

#define INACTIVE	0
#define ACTIVE		1
#define DEAD		2

#define BYTE	signed char
#define UBYTE	unsigned char
#define DWORD	int
#define USHORT	unsigned short
#define SHORT	short

#define MOD		%

#define	INIT_ANIMATION	(1 << 0)
#define	INIT_SHADOW		(1 << 1)

//object flags
#define OBJECT_FLAGS_GOURAUD_SHADED	(1 << 0)
#define OBJECT_FLAGS_XLU			(1 << 1)
#define OBJECT_FLAGS_TRANSPARENT	(1 << 2)
#define OBJECT_FLAGS_PRELIT			(1 << 3)
#define OBJECT_FLAGS_XFORMMESH		(1 << 4)
#define OBJECT_FLAGS_MODGE			(1 << 5)
#define OBJECT_FLAGS_CONSTANIM		(1 << 6)
#define OBJECT_FLAGS_IA				(1 << 7)


//--------------
#define Max(p1,p2)	(((p1) > (p2)) ? (p1) : (p2))
#define Min(p1,p2)	(((p1) > (p2)) ? (p2) : (p1))

#define min Min
#define max Max

#define BOOL	int



typedef struct
{
	float	x, y, z, w;
}QUATERNION;

typedef struct
{
	float	matrix[4][4];
}MATRIX;

typedef struct
{
	float	v[3];
}VECTOR;

typedef struct
{
	float	x,z;
}POINT2D;

typedef struct
{
	VECTOR	pos;
	VECTOR	size;
}BOX;

typedef struct
{
	signed char	v[4];
}BYTEVECTOR;

typedef struct
{
	UBYTE	v[4];
}UBYTEVECTOR;


typedef struct
{
	unsigned short	v[2];
}SHORT2DVECTOR;

typedef struct
{
	float	v[2];
}VECTOR2D;

typedef struct
{
	short	v[3];
}SHORTVECTOR;

typedef struct
{
	unsigned short v[2];
}USHORT2DVECTOR;

typedef struct
{
	char r,g,b,a;
}COLOUR;


typedef struct
{
	int cx,cy;
}SIZE;

typedef struct
{
	VECTOR	normal;
	float	J;
	VECTOR	vertex[3];
	VECTOR	edge[3];
	float	edgeLen[3];
#ifdef STORE_EDGE_J
	float	edgeJ[3];
#endif
	float	u1,u2;
	float	v1,v2;
	UBYTE	terrain;
	UBYTE	i[2];
	UBYTE	space;
}PLANE;

typedef struct
{
	int	handle, volume, actualVolume,origVolume;
	short pitch,sampleNum;
	VECTOR *pos;
	float radius;
}SFX;

typedef struct
{
	int volume;
	short pitch,sampleNum;
}SPOT_SFX;

//======================texture type
typedef struct
{
	int		texID;
	UBYTE	cycleStart;
	UBYTE	cycleEnd;
	short	flags;
	short	cycleSpeed;
	short	cycleCount;
	unsigned short	sx;
	unsigned short	sy;
	unsigned short	TCScaleX;
	unsigned short	TCScaleY;
	int		size;
	short	format;
	short	pixSize;
	char	*data;
	short	*palette;
}TEXTURE;


typedef struct
{
	VECTOR	offset;
	VECTOR	oldOffset;
	float	radius;
	float	radiusAim;
	UBYTE	posTag;
	UBYTE	quatTag;
	short	flags;
}COLLSPHERE;


typedef struct
{
	short			numFaces;
	short			numVertices;
	VECTOR			*vertices;
	SHORTVECTOR		*faceIndex;
	BYTEVECTOR		*faceNormals;
	SHORT2DVECTOR	*faceTC;
	SHORT2DVECTOR	*originalFaceTC;
	BYTEVECTOR		*vertexNormals;
	char		 	*faceFlags;
	TEXTURE			**textureIDs;
}MESH;


typedef struct
{
	union
	{
		QUATERNION	quat;
		VECTOR	vect;
	}u;
	int	time;
}KEYFRAME;


typedef struct
{
	float			altitude;
	PLANE			*altPlane;
	float			radius;
	UBYTE			alpha;
	UBYTE			alphaAim;
	UBYTE			alphaAimSpeed;
	char			collSphere;
	Vtx				vert[4];
} ACTOR_SHADOW;


//----------------animation
typedef struct 
{
	short	   animStart, animEnd;
	float	   speed;
	int		   loop;
} animation;

#define ANIM_QUEUE_LENGTH 5

typedef struct
{
	short		numAnimations;
	short		currentAnimation;
	BOOL		reachedEndOfAnimation;
	BOOL		loopAnimation;
	float		animationSpeed;
	short		queueAnimation[ANIM_QUEUE_LENGTH];
	BOOL		queueLoopAnimation[ANIM_QUEUE_LENGTH];
	float		queueAnimationSpeed[ANIM_QUEUE_LENGTH];
	short 		numberQueued;
	animation	*anims;
	float		animTime;//, animTimeDelta;
}ACTOR_ANIMATION;

typedef struct
{
	TEXTURE **texture;
	short	numFrames;
	short	overrideNumFrames;
	char	filename[8];
}FRAMELIST;

typedef struct
{
	FRAMELIST	*frameList;
	UBYTE		delay;
	UBYTE		type;
	UBYTE		startAlpha;
	UBYTE		endAlpha;
	USHORT		startScale;
	USHORT		endScale;
	USHORT		spriteFlags;
	short		pad;
}SPRITE_ANIMATION_TEMPLATE;

typedef struct
{
	FRAMELIST	*frameList;
	UBYTE		delay;
	UBYTE		type;
	UBYTE		startAlpha;
	UBYTE		endAlpha;
	USHORT		startScale;
	USHORT		endScale;
	USHORT		spriteFlags;
	USHORT		lifespan;
	USHORT		lifetime;
	UBYTE		currentFrame;
	BYTE		counter;
}SPRITE_ANIMATION;

typedef struct TAGSPRITE
{
	struct TAGSPRITE *next,*prev;

	TEXTURE *texture;
	VECTOR pos;
	short scaleX;
	short scaleY;
	UBYTE sameAsLast;
	UBYTE r,g,b,a;

	char kill;

	UBYTE flags;
	BYTE  offsetX;
	BYTE  offsetY;
	SPRITE_ANIMATION anim;
}SPRITE;

typedef struct OBJECTSPRITE
{
	TEXTURE		*textureID;
	SPRITE		*sprite;		//actual sprite in sprite list
	short		x, y, z;
	short		sx;	//xsize - 32 is normal
	short		sy;	//ysize - 32 is normal
	short		ox;	//offset from centre
	short		oy;	//offset from centre
	short		flags;
}OBJECTSPRITE;


typedef struct OBJECT
{
	int			objID;
	char		name[8];
	u8			active;
	u8			xlu;
	short		numScaleKeys;
	short		numMoveKeys;
	short		numRotateKeys;

	MESH		*mesh;
	Gfx			*drawList;
//vitals

//animation
	KEYFRAME	*scaleKeys;
	KEYFRAME	*moveKeys;
	KEYFRAME	*rotateKeys;

	int			numSprites;
	OBJECTSPRITE	*sprites;

//family
	short		numChildren;
	short		flags;
	struct		OBJECT	*children;
	struct		OBJECT	*next;
	COLLSPHERE	*collSphere;
	Mtx			myMatrix;
}OBJECT;

typedef struct
{
	int		objectID;

	Mtx		*matrix;
	int		objectSize;
	OBJECT	*object;
	float	radius;
	PLANE	*planes;
	ACTOR_ANIMATION	*animation;
}OBJECT_CONTROLLER;


typedef struct
{
	int		objectID;
 	OBJECT_CONTROLLER	*objCont;
}OBJECT_DESCRIPTOR;


typedef struct
{
	char	*data;
	char	*freePtr;
	int		numObjects;
	OBJECT_DESCRIPTOR	*objList;
}OBJECT_BANK;

typedef struct TAGACTOR
{
	struct TAGACTOR				*next;

	Mtx							*matrix;
	SHORT						type;
	unsigned long				status;
	short						lifetime;
	short						xluOverride;
	int							flags;

	VECTOR						pos;
	VECTOR						oldpos;
	VECTOR						vel;
	VECTOR						oldVel;
	float						velInertia;
	float						speed;
	VECTOR						rot;
	VECTOR						rotaim;
								
	QUATERNION					qRot;
	QUATERNION					qRotVel;
												
	VECTOR	  					scale;

	OBJECT_CONTROLLER			*objectController;
	ACTOR_ANIMATION				*animation;
	ACTOR_SHADOW				*shadow;
	PLANE  						*planes;
	unsigned long	loopCount;
	int rotateAboutY;
}ACTOR;

typedef struct TAGTEXT
{
	struct TAGTEXT *next,*prev;
	char message[64];
	int x,y;
	float scaleX,scaleY;
	UBYTE r,g,b,a;
	int byebye;
}TEXT;

typedef struct
{
	TEXT head;
	int numEntries;
}TEXTLIST;


typedef struct TAGPLANE2
{
	float	distance;
	float	J;
	VECTOR	point;
	VECTOR	normal;

} PLANE2;



#define dprintf debugPrintf(sprintf(outputMessageBuffer, 

extern char outputMessageBuffer[];

extern Gfx *glistp;


#endif