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

short loadProgress;

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
	backdrop = NULL;
}


void InitLoadingScreen( const char *filename )
{
	InitBackdrop( filename );

	loadProgress = 0;
}

void UpdateLoadingScreen( short addprog )
{
	RECT r;

	loadProgress += addprog;

	if( loadProgress > 100 )
		loadProgress = 100;

	BeginDraw( );
	DrawBackdrop( );
	EndDraw( );

	r.top = rYRes - rYRes/4;
	r.bottom = rYRes - rYRes/4.8;
	r.left = rXRes/4;
	r.right = (((rXRes-(2*r.left))/100) * loadProgress) + r.left;

	BeginDraw( );
	DrawFlatRect( r, D3DRGBA(1,0.5,0.5,0.8) );
	EndDraw( );

	DDrawFlip();
}

void FreeLoadingScreen( )
{
	FreeBackdrop( );
}
