/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: general.c
	Programmer	: Matthew Cloy
	Date		: 12/01/99

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>

#include "incs.h"

char statusMessage[255];

unsigned long gstrlen (const char *a)
{
	char *b = (char*)a;
	unsigned long i;
	for (i=0; b[0]==0; b++,i++);
	return i;
}

unsigned long gstrcmp(char *a,char *b)
{
	char *x = a,*y = b;

	while ((x[0] && y[0]) && (x[0]==y[0]))
	{
		x++;
		y++;
	}
	
	if (x[0]!=0)
		return 1;

	if (y[0]!=0)
		return 1;
	
	return 0;
}

void stringChange ( char *name )
{
	char *tmpName;

	while ( *name != '.' )
	{
		dprintf"%c", *name ));
		name++;
	}

	name[1] = 'o';
	name[2] = 'b';
	name[3] = 'e';
	name[4] = '\0';
	dprintf"%c\n", name ));

}

inline void cmemcpy( char *a, char *b, unsigned long size )
{
	while( size-- )
		a[size] = b[size];
}

inline void smemcpy( unsigned short *a, unsigned short *b, unsigned long size )
{
	while( size-- )
		a[size] = b[size];
}

inline void lmemcpy( unsigned long *a, unsigned long *b, unsigned long size )
{
	while( size-- )
		a[size] = b[size];
}


/*	--------------------------------------------------------------------------------
    Function		: FindNearestTile
	Parameters		: VECTOR
	Returns			: GAMETILE*
*/


GAMETILE* FindNearestTile(VECTOR v)
{
	GAMETILE *t, *closest;
	int dist, closestDist;

	closest = NULL; closestDist = 0x10000000;

	for (t = firstTile; t != NULL; t = t->next)
	{
		dist = DistanceBetweenPointsSquared(&v, &t->centre);

		if (dist < closestDist)
		{
			closest = t;
			closestDist = dist;
		}
	}

	return closest;
}


/*	--------------------------------------------------------------------------------
    Function		: MemLoadEntities
	Parameters		: FILE*
	Returns			: PATH*

	Load enemies and plats from a data file in memory
*/

#define MEMGETBYTE(p) (*((*p)++))

inline int MEMGETINT(uchar **p)		// get a little-endian integer
{
	unsigned int i;

	i = (unsigned int)*((*p)++);
	i += ((unsigned int)*((*p)++) << 8);
	i += ((unsigned int)*((*p)++) << 16);
	i += ((unsigned int)*((*p)++) << 24);

	return i;
}

#define MEMGETFLOAT(p) ((float)MEMGETINT(p) / (float)0x10000)

int MemLoadEntities(const void* data, long size)
{
	int count, flags, numNodes, startNode, n, ID;
	uchar thing;
	char type[20];
	PATH *path;
	PATHNODE *node;
	VECTOR v;
	ENEMY *enemy;
	PLATFORM *platform;
	uchar *p = (uchar*)data;

	// Version check - only load files with the current version
	n = MEMGETBYTE(&p);
	
	if (n != 3) {
		dprintf"ERROR: Attempting to load incorrect version (%03d) of level file (should be 003)\n", n));
		return 0;
	}

	count = MEMGETINT(&p);
	dprintf"MemLoadEntities: %d items\n", count));

	while (count--)
	{
		n = MEMGETBYTE(&p);
		memcpy(type, p, n); type[n] = 0; p+= n;
		flags = MEMGETINT(&p);
		ID = MEMGETINT(&p);
		thing = MEMGETBYTE(&p);
		
		numNodes = MEMGETINT(&p);
		startNode = MEMGETINT(&p);

		dprintf"'%s' %08x with %d path nodes\n", type, flags, numNodes));

		path = (PATH *)JallocAlloc(sizeof(PATH), YES, "epath");
		node = (PATHNODE *)JallocAlloc(sizeof(PATHNODE) * numNodes,YES,"enode");
		path->nodes = node;
		path->startNode = startNode;
		path->numNodes = numNodes;

		while (numNodes--)
		{
			v.v[X] = MEMGETFLOAT(&p);
			v.v[Y] = MEMGETFLOAT(&p);
			v.v[Z] = MEMGETFLOAT(&p);
			node->worldTile = FindNearestTile(v);
			node->offset = MEMGETFLOAT(&p);
			node->offset2 = MEMGETFLOAT(&p);
			node->speed = MEMGETFLOAT(&p);
			node->waitTime = MEMGETINT(&p);
			
			node++;
		}

		switch (thing)
		{
		case 0:
			enemy = CreateAndAddEnemy(type);
			AssignPathToEnemy(enemy, flags, path, 0);
			break;

		case 1:
			platform = CreateAndAddPlatform(type);
			AssignPathToPlatform(platform, flags, path, 0);
			break;
		}
	 }

	count = MEMGETINT(&p);

	while (count--)
	{
		n = MEMGETBYTE(&p);
		v.v[X] = MEMGETFLOAT(&p);
		v.v[Y] = MEMGETFLOAT(&p);
		v.v[Z] = MEMGETFLOAT(&p);
		CreateNewGarib(v, n);
	}

	return 1;
}