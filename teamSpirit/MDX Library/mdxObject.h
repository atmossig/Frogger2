/*

	This file is part of the M libraries,

	File		: 
	Programmer	: Matthew Cloy
	Date		: 

----------------------------------------------------------------------------------------------- */

#ifndef MDXOBJECT_H_INCLUDED
#define MDXOBJECT_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
	MDX_TEXENTRY **texture;
	short	numFrames;
	short	overrideNumFrames;
	char	filename[16];

}MDX_FRAMELIST;

typedef struct
{
	MDX_FRAMELIST	*frameList;
	unsigned char delay;
	unsigned char type;
	unsigned char startAlpha;
	unsigned char endAlpha;
	unsigned short startScale;
	unsigned short endScale;
	unsigned short spriteFlags;
	unsigned short lifespan;
	unsigned short currentFrame;
	float		counter;
	float		lifetime;

}MDX_SPRITE_ANIMATION;

typedef struct
{
	MDX_FRAMELIST	*frameList;
	unsigned char delay;
	unsigned char flags;
	unsigned char currentFrame;
	char counter;

}MDX_TEXTURE_ANIMATION;

typedef struct TAG_MDX_SPRITE
{
	struct TAG_MDX_SPRITE *next,*prev;

	MDX_TEXENTRY *texture;
	MDX_VECTOR pos;
	short scaleX;
	short scaleY;
	short flags;
	unsigned char r,g,b,a;
	unsigned char red2,green2,blue2,alpha2;
	char  offsetX;
	char  offsetY;
	MDX_SPRITE_ANIMATION anim;

	char kill;

	MDX_VECTOR sc;
	float angle;
	float angleInc;
	short arrayIndex;

}MDX_SPRITE;

typedef struct TAG_MDX_OBJECTSPRITE
{
	MDX_TEXENTRY	*textureID;
	MDX_SPRITE		*sprite;		//actual sprite in sprite list
	short		x, y, z;
	short		sx;	//xsize - 32 is normal
	short		sy;	//ysize - 32 is normal
	short		ox;	//offset from centre
	short		oy;	//offset from centre
	short		flags;

}MDX_OBJECTSPRITE;

typedef struct
{
	short			numFaces;
	short			numVertices;
	MDX_VECTOR			*vertices;
	MDX_SHORTVECTOR		*faceIndex;
	MDX_BYTEVECTOR		*notUsed;//faceNormals;
	MDX_USHORT2DVECTOR	*faceTC;
	MDX_VECTOR			*vertexNormals;
	MDX_QUATERNION		*gouraudColors;
	char		 	*faceFlags;
	MDX_TEXENTRY		**textureIDs;

}MDX_MESH;

typedef struct
{
	unsigned short *verts;
	char	numVerts;

}MDX_SKINVTX;

typedef struct
{
	float	v[3];
	unsigned char n[3];

}MDX_DUELVECTOR; 

typedef struct
{
	MDX_VECTOR	vect;
	int	time;
}MDX_VKEYFRAME;

typedef struct
{
	MDX_QUATERNION	quat;
	int	time;
}MDX_QKEYFRAME;

typedef struct TAG_MDX_OBJECT
{
	unsigned long objID;
	unsigned char name[16];

	short			padVal5;///////////////////////////
	
	unsigned char xlu;
	unsigned char	numSprites;
	MDX_OBJECTSPRITE	*sprites;

	//for non skinned objects
	MDX_MESH			*mesh;
	void			*drawList;

	unsigned long	padVal4;///////////////////////////
	
	//for skinning
	MDX_SKINVTX		*effectedVerts;
	struct TAG_MDX_ACTOR	*attachedActor;
	short		numVerts;

	MDX_VKEYFRAME	*scaleKeys;
	MDX_VKEYFRAME	*moveKeys;

	MDX_QKEYFRAME	*rotateKeys;

	short		numScaleKeys;
	short		numMoveKeys;
	short		numRotateKeys;

	MDX_VECTOR		*bBox;
	MDX_TEXENTRY	*phong;
	
	short		flags;

	short			padVal2;///////////////////////////

	struct TAG_MDX_OBJECT	*children;
	struct TAG_MDX_OBJECT	*next;

	unsigned long	padVal1;///////////////////////////
	MDX_MATRIX		objMatrix;

}MDX_OBJECT;

//----------------animation
typedef struct 
{
	short	   animStart, animEnd;
	float	   speed;
	int		   loop;
} MDX_ANIMATION;

#define MDX_ANIM_QUEUE_LENGTH 5

typedef struct
{
	short		numAnimations;
	short		currentAnimation;
	BOOL		reachedEndOfAnimation;
	BOOL		loopAnimation;
	float		animationSpeed;
	short		queueAnimation[MDX_ANIM_QUEUE_LENGTH];
	BOOL		queueLoopAnimation[MDX_ANIM_QUEUE_LENGTH];
	float		queueAnimationSpeed[MDX_ANIM_QUEUE_LENGTH];
	short 		numberQueued;
	MDX_ANIMATION	*anims;
	float		animTime;	
}MDX_ACTOR_ANIMATION;

typedef struct
{
	int		objectID;

	MDX_MATRIX	*matrix;
	short	objectSize;
	MDX_OBJECT	*object;
	float	radius;
	void	*planes;
	MDX_ACTOR_ANIMATION	*animation;
	void		*drawList;
	void		*vtx[2];
	MDX_MESH	*LOCMesh;
	short	isSkinned;
	short	numVtx;

}MDX_OBJECT_CONTROLLER;


typedef struct
{
	int		objectID;
 	MDX_OBJECT_CONTROLLER	*objCont;
}MDX_OBJECT_DESCRIPTOR;

typedef struct
{
	char	*data;
	char	*freePtr;
	int		numObjects;
	MDX_OBJECT_DESCRIPTOR	*objList;
}MDX_OBJECT_BANK;

#define OBJECT_BASE "objects\\"
#define MAX_OBJECT_BANKS 3

#define OBJECT_FLAGS_GOURAUD_SHADED			   	(1<<0)		// 1
#define OBJECT_FLAGS_XLU					   	(1<<1)		// 2
#define OBJECT_FLAGS_TRANSPARENT			   	(1<<2)		// 4
#define OBJECT_FLAGS_PRELIT					   	(1<<3)		// 8
#define OBJECT_FLAGS_MIP_MAP				   	(1<<4)		// 16
#define OBJECT_FLAGS_MODGE					   	(1<<5)		// 32
#define OBJECT_FLAGS_ADDITIVE				   	(1<<6)		// 64
#define OBJECT_FLAGS_WAVE					   	(1<<7)		// 128
#define OBJECT_FLAGS_PHONG					   	(1<<8)		// 256
#define OBJECT_FLAGS_FADEOUT				   	(1<<9)		// 512
#define OBJECT_FLAGS_FACE_COLOUR			   	(1<<10)		// 1024
#define OBJECT_FLAGS_OBJECT_COLOUR			   	(1<<11)		// 2048
#define OBJECT_FLAGS_TEXTURE_BLEND_ENV		   	(1<<12)		// 4096
#define OBJECT_FLAGS_SUBDIVIDED				   	(1<<13)		// 8192
#define OBJECT_FLAGS_GLOW					  	(1<<14)		// 16384

#define OBJECT_FLAGS_CLIPPED				  	(1<<15)		


void FindObject(MDX_OBJECT_CONTROLLER **objCPtr, int objID, char *name);
void LoadObjBank(char *bank, char *baseDir);
void TransformObject(MDX_OBJECT *obj, float time);
void SubdivideObject(MDX_OBJECT *me);
extern WORD fpuState;

#ifdef __cplusplus
}
#endif

#endif