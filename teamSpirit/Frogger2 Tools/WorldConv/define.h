/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.

	File		: define.h
	Programmer	: David Swift
	Date		: 24/1/00

----------------------------------------------------------------------------------------------- */

#ifndef DEFINE_H_INCLUDED
#define DEFINE_H_INCLUDED

typedef struct
{
	float	v[3];
} VECTOR;

#define NULL 0

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
