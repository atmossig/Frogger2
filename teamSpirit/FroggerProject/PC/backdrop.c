/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: backdrop.c
	Programmer	: David
	Date		: 16/03/00

----------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <ddraw.h>
#include <mdxddraw.h>
#include <islutil.h>
#include "backdrop.h"
#include "Main.h"

short loadProgress;

int CheckUS();

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
	strcat(path, "textures\\");
	strcat(path, filename);

	strcat(path, ".fla");	// load flatpacker compressed data

	utilPrintf("Load backdrop: %s...", path);

	mdxLoadBackdrop(path);

	utilPrintf("ok!\n");

	UpdateTiming();
	
	return 1;
}

int InitCDBackdrop(const char *filename)
{
	char path[MAX_PATH];

	strcpy(path, cdromDrive);
	strcat(path, "Gallery\\");
	strcat(path, filename);

	strcat(path, ".fla");	// load flatpacker compressed data

	utilPrintf("Load backdrop from CD: %s...", path);

	mdxLoadBackdrop(path);

	utilPrintf("ok!\n");

	UpdateTiming();
	
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


#ifdef PC_DEMO
void InitLoadingScreen( const char *filename )
{
	char path[64];

	strcpy( path, filename );

	if( CheckUS() )
		strcat( path, "US" );
	else
		strcat( path, "EU" );

	InitBackdrop( path );

	loadProgress = 0;
}
#else
void InitLoadingScreen( const char *filename )
{
	InitBackdrop( filename );

	loadProgress = 0;
}
#endif



/* -----------------------------------------------------------------------
   Function : UpdateLoadingScreen
   Purpose : update and display the loading progress
   Parameters : progess increment
   Returns : 
   Info : 
*/

void UpdateLoadingScreen(short addprog)
{
	RECT r;

	loadProgress += addprog;

	if( loadProgress > 100 )
		loadProgress = 100;

	r.top = rYRes - rYRes/4;
	r.bottom = rYRes - rYRes/4.8;
	r.left = rXRes/4;
	r.right = (((rXRes-(2*r.left))/100) * loadProgress) + r.left;

	// show fixed background screen
	DrawBackdrop();

	// *ASL* 14/06/2000
	if (rHardware)
	{
		BeginDraw();
		DrawFlatRect( r, D3DRGBA(0.9,0,0,1) );
		EndDraw();
	}
	else
	{
		// ** Unfortunately, this does not perform the alpha over the background
		// ** screen, The reason for this is that the software render buffer would
		// ** have to contain the backdrop image.

		// !! no alpha !!
		mdxDrawRectangle(r, (int)1.0f*255.0f, (int)0.2f*255.0f, (int)0.2f*255.0f);
	}

	// show our print screen
	DDrawFlip();
}


void FreeLoadingScreen( )
{
	FreeBackdrop( );
}


// Get local id from registry and use it to check if we're in the US or not
int CheckUS()
{
	HKEY lid;
	long type, size=32;
	char data[32];
	DWORD value;

	if( (RegOpenKeyEx( HKEY_CURRENT_USER, "Control Panel\\International\0", 0, KEY_READ, &lid )) != ERROR_SUCCESS )
		return TRUE;

	if( (RegQueryValueEx(lid, "Locale", NULL, &type, data, &size)) != ERROR_SUCCESS )
		return TRUE;

	value = atoi(data);

	if( value == 0x0409 || value == 0x0C0C )
		return TRUE;

	return FALSE;
}
