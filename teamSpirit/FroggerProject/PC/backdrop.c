/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: backdrop.c
	Programmer	: David
	Date		: 16/03/00

----------------------------------------------------------------------------------------------- */

#include <ddraw.h>
#include <mdxddraw.h>
#include "backdrop.h"
#include "Main.h"

/*	--------------------------------------------------------------------------------
	Function 	: InitBackdrop
	Purpose 	: Initialises a backdrop from a bitmap file
	Parameters 	: char*
	Returns 	: 
	Info 		:
*/
int InitBackdrop(const char *filename)
{
	char path[MAX_PATH];

	strcpy(path, baseDirectory);
	strcat(path, "Backdrops\\");
	strcat(path, filename);
	strcat(path, ".bmp");

	utilPrintf("Load backdrop: %s...", path);

	mdxLoadBackdrop(path);

	utilPrintf("ok!\n");
	return 1;
}

void DrawBackdrop(void)
{
	mdxDrawBackdrop();
}

void FreeBackdrop(void)
{
	mdxFreeBackdrop();
}