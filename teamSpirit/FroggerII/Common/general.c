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

#ifdef PC_VERSION

	char *c = strstr(name, ".ndo");
	if (c)
	{
		dprintf "Converted %s to ", name));
		strcpy(c, ".obe");
		dprintf "%s\n", name));
	}

#else
	
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

#endif
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


