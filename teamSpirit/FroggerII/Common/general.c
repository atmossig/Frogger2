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
	char *b=a;
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
