/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.

	File		: define.h
	Programmer	: David Swift
	Date		: 24/1/00

----------------------------------------------------------------------------------------------- */

#ifndef DEFINE_H_INCLUDED
#define DEFINE_H_INCLUDED

#define NULL 0

enum
{
	TILESTATE_NORMAL,
	TILESTATE_DEADLY,
	TILESTATE_SINK,
	TILESTATE_ICE,
	TILESTATE_SUPERHOP,
	TILESTATE_JOIN,
	TILESTATE_SAFE,
	TILESTATE_RESERVED,
	TILESTATE_GRAPPLE,
	TILESTATE_SMASH,
	TILESTATE_BARRED,
	TILESTATE_LEVELCHANGE,
	TILESTATE_FALL,

	TILESTATE_CONVEYOR0SLOW = 0x20,
	TILESTATE_CONVEYOR1SLOW,
	TILESTATE_CONVEYOR2SLOW,
	TILESTATE_CONVEYOR3SLOW,

	TILESTATE_CONVEYOR0MED,
	TILESTATE_CONVEYOR1MED,
	TILESTATE_CONVEYOR2MED,
	TILESTATE_CONVEYOR3MED,

	TILESTATE_CONVEYOR0FAST,
	TILESTATE_CONVEYOR1FAST,
	TILESTATE_CONVEYOR2FAST,
	TILESTATE_CONVEYOR3FAST,
};

#define TILESTATE_CONVEYOR 0x20
#define TILESTATE_CONVEYOR_SLOW TILESTATE_CONVEYOR0SLOW
#define TILESTATE_CONVEYOR_MED	TILESTATE_CONVEYOR0MED
#define TILESTATE_CONVEYOR_FAST TILESTATE_CONVEYOR0FAST

typedef struct
{
	float	v[3];
} VECTOR;

typedef struct
{
	VECTOR	normal;
	float	k;
	unsigned long status;
} CAM_PLANE;

typedef struct
{
	int numPlanes;
	CAM_PLANE *planes;
} CAM_BOX;

typedef struct
{
	int numBoxes;
	CAM_BOX *boxes;
} CAM_BOX_LIST;

typedef struct _GAMETILE
{
	struct _GAMETILE	*tilePtrs[4];	// north, south, east and west
	struct _GAMETILE	*next;			// next tile in linked list
	
	void *blank;	// dunno what this does

	unsigned char	state;		// state of tile
	
	VECTOR		centre;			// centre position
	VECTOR		normal;			// face normal
	VECTOR		dirVector[4];	// the 4 possible direction (camera) vectors for this tile

} GAMETILE;

#endif
