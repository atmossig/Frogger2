/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: define.h
	Programmer	: Matthew Cloy
	Date		: 10/11/98

----------------------------------------------------------------------------------------------- */

#ifndef DEFINE_H_INCLUDED
#define DEFINE_H_INCLUDED


//------------------------------------------------------------------------------------------------

enum gameModes
{
	//  Frontend / Title screen Modes
	FRONTEND_MODE,

	GAME_MODE,

	CAMEO_MODE,

	PAUSE_MODE,

	OLDEFROGGER_MODE,

	GHOST_MODE,							// Mode To Whit Your Self Against Your Self

	LEVELPLAYING_MODE,
	RECORDKEY_MODE,
  
};

struct gameStateStruct
{
	unsigned long mode;
	unsigned long oldMode;
};

//------------------------------------------------------------------------------------------------

enum
{
	TILESTATE_NORMAL,
	TILESTATE_DEADLY,
	TILESTATE_SINK,
	TILESTATE_ICE,
	TILESTATE_SUPERHOP,
	TILESTATE_JOIN,
	TILESTATE_SPRING,
	TILESTATE_TELEPORTER,
};

enum
{
	TELEPORT_ONEWAY,
	TELEPORT_TWOWAY,
};

// For accessing the direction based on the current camera. (Clockwise)

typedef struct _GAMETILE
{
	struct _GAMETILE	*tilePtrs[4];		// north, south, east and west
	struct _GAMETILE	*next;				// next tile in linked list
	struct _GAMETILE	*teleportTo;		// tile to teleport to if this tile is a teleporter

	unsigned char		state;				// state of tile
	
	VECTOR		centre;						// centre position
	VECTOR		normal;						// face normal

	VECTOR		dirVector[4];			// the 4 possible camera vectors for this tile

} GAMETILE;

// Scenic objects //

#define	TYPE_NORMAL    (0)
#define	TYPE_HASPATH   (1<<0)
#define	TYPE_HASSPIN   (1<<1)
#define	TYPE_MAINWORLD (1<<2)

typedef struct _TYPEDATA
{
	unsigned long noPts;	
	VECTOR *movementPath;
	VECTOR *allocedVect;
	unsigned long animSpeed;
	unsigned long ofsPts;
	unsigned long dirn;
} TYPEDATA;

typedef struct _SCENIC
{
	struct _SCENIC *next;
	char name[32];
	VECTOR pos;
	QUATERNION rot;
	unsigned long type;
	TYPEDATA *typedata;
	GAMETILE *tileptrs;
} SCENIC;

////////////////////

extern char statusMessage[255];
//------------------------------------------------------------------------------------------------

#endif
