/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: editfile.c
	Programmer	: David Swift
	Date		: 02/07/99

----------------------------------------------------------------------------------------------- */

#include <stdio.h>
#include "incs.h"

/*	--------------------------------------------------------------------------------
	Function		: SaveGaribList
	Purpose			: 
	Parameters		: const char*
	Returns			: 
	Info			: Saves a snapshot of the current garib list, in plain text for now
*/

void SaveGaribList(const char *filename)
{
	FILE *f;
	GARIB *garib;

	f = fopen(filename, "wt"); // text file

	fprintf(f,"GARIB LIST\n-----------\n\nEntries: %i\n\n",garibCollectableList.numEntries);

	garib = garibCollectableList.
}