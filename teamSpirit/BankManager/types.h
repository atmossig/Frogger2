#ifndef __TYPES_H__
#define __TYPES_H__

#define X 0
#define Y 1
#define Z 2

#define BYTE	signed char
#define UBYTE	unsigned char
#define DWORD	int
#define USHORT	unsigned short
#define SHORT	short

#define MOD		%

//object flags

//object flags


#define OBJECT_FLAGS_GOURAUD_SHADED	( 1 << 0 )
#define OBJECT_FLAGS_XLU			( 1 << 1 )
#define OBJECT_FLAGS_TRANSPARENT	( 1 << 2 )
#define OBJECT_FLAGS_PRELIT			( 1 << 3 )
#define OBJECT_FLAGS_MIP_MAP		( 1 << 4 )
#define OBJECT_FLAGS_TEXTURE_BLEND	( 1 << 6 )
#define OBJECT_FLAGS_IA				( 1 << 7 )
#define OBJECT_FLAGS_PHONG			( 1 << 8 )
#define OBJECT_FLAGS_FACECOL		( 1 << 10 )
#define OBJECT_FLAGS_OBJECT_COLOUR	( 1 << 11 )
#define	OBJECT_FLAGS_CHANGEDRAWLIST ( 1 << 12 )


//--------------
#define Max(p1,p2)	(((p1) > (p2)) ? (p1) : (p2))
#define Min(p1,p2)	(((p1) > (p2)) ? (p2) : (p1))


#define SHOWFONT(glp,str,x,y)   {                  \
                font_set_color( 255,255,255,200);  \
                font_set_pos( x, y );              \
                font_show_string( glp, str );}


#define BOOL	int

typedef struct
{
	float	x, y, z, w;
}QUATERNION;

typedef struct
{
	unsigned char a, b, g, r;
}COLOUR;

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
	char	v[4];
}BYTEVECTOR;

typedef struct
{
	signed char	v[4];
}SBYTEVECTOR;


typedef struct
{
	short	v[2];
}SHORT2DVECTOR;


typedef struct
{
	short	v[4];
}SHORTVECTOR;

typedef struct
{
	VECTOR pos;
	VECTOR *target;
}CAMERA;


typedef struct
{
	VECTOR	normal;
	float	J;
	UBYTE	i[2];
	DWORD	numVertices;
	VECTOR	vertex[4];
	float	u1,u2;
	float	v1,v2;
	int		index;
	UBYTE	terrain;
}PLANE;


//======================texture type
typedef struct
{
	int		texID;
	int		flags;
	int		bastard;
	unsigned short	sx;
	unsigned short	sy;
	unsigned short	TCScaleX;
	unsigned short	TCScaleY;
	int		size;
	short	format;
	short	pixSize;
	char	*data;
	char	*palette;
}TEXTURE;

typedef struct
{
	int		crc;
	short	sx, sy;
	int	tileX, tileY;
	short	colourDepth;
	short	flags;
	char	name[32];
//	char	r, g, b, a;
}TEXTURE_ID;

typedef struct
{
	char			faceFlags;
	int				vertsInFace;
	int				NDOFlags;
	int				terrainType;
	COLOUR			colour;
	COLOUR			gouraudColour[4];
	SHORT2DVECTOR	faceTC[4];
	POINT2D			obeTC[4];
	BYTEVECTOR		faceNormal;
	TEXTURE_ID		textureID;
	SHORTVECTOR		faceIndex;
	int				duelTexID;
	BYTEVECTOR		vAlpha;
	short			flags;
}FACE_INFO;

typedef struct
{
	short			numFaces;
	short			numVertices;
	VECTOR			*vertices;
	BYTEVECTOR		*vertexNormals;
//	char			*faceFlags;
	short			meshFlags;
//	int				*NDOFlags;
//	int				*terrainTypes;
//	COLOUR			*faceColours;
	FACE_INFO		*faceInfo;
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

#define SPRITE_TRANSLUCENT	(1<<0)
#define SPRITE_FLIPX		(1<<1)
#define SPRITE_FLIPY		(1<<2)


typedef struct SPRITE_TYPE
{
	int			textureID;
	short			x, y, z;
	short			sx;	//xsize - 32 is normal
	short			sy;	//ysize - 32 is normal
	char			ox;	//offset from centre
	char			oy;	//offset from centre
	short			flags;
}SPRITE_TYPE;



typedef struct 
{
	unsigned short	vertIndex;
	VECTOR			offset;
}VERTASSIGN;

typedef struct{

	short x,y,z;
	short tx,ty;
	short nx,ny,nz;
	unsigned char alpha;

} N64vertex;


typedef struct INPUT_OBJECT
{
	char		name[32];
	char		boneName[32];
	short		active;
	int			numScaleKeys;
	int			numMoveKeys;
	int			numRotateKeys;
	int			numVertsInBone;
	VERTASSIGN	*boneVerts;
	
	MESH		*mesh;

	// animation
	KEYFRAME	*scaleKeys;
	KEYFRAME	*moveKeys;
	KEYFRAME	*rotateKeys;

	int			numSprites;
	SPRITE_TYPE *sprites;

	// family
	short		numChildren;
	short		flags;
	int			xluFactor;
	struct		INPUT_OBJECT	*children;
	char		childName[16];
	struct		INPUT_OBJECT	*next;
	char		nextName[16];
	COLOUR		objColour;
	int			phongTexture;

	//skinning
	VECTOR		*effectedVerts;
	VECTOR		*originalVerts;
	short		numVerts;

	N64vertex	*drawListVtxList;
	int			numVtx;
	int			bound;

}INPUT_OBJECT;


typedef struct {
	short						numberOfFrames;
	short						animStart, animEnd;
	float						speed;
} animation;

typedef struct
{
	short						currentAnimation;
	BOOL						reachedEndOfAnimation;
	BOOL						loopAnimation;
	float						speedAnimation;
	short						queueAnimation[5];
	BOOL						queueLoopAnimation[5];
	float						queueSpeedAnimation[5];
	UBYTE 						numberQueued;
	animation					*anims;
	float						animTime;//, animTimeDelta;
} ACTOR_ANIMATION;

typedef struct
{
	short						numDrawList;
	short						numVertices;
} DS_OBJECT_INFO;


#define dprintf debugPrintf(sprintf(outputMessageBuffer, 
extern char outputMessageBuffer[];




#endif