/*
	(c) 2000 Interactive Studios Ltd.

	File		: DCK_System.c
	Programmer	: Andy Eder
	Created		: 19/01/00

	Updated		: 19/01/00	Andy Eder
				: < revision dates here >

----------------------------------------------------------------------------------------------- */

#include "include.h"

extern DCKVERTEX 		*globalVerts;
extern KMSTRIPCONTEXT 	StripContext;
extern int				matrixSize;
extern DCKMATRIX		matrixStack[];
extern DCKMATRIX		viewMatrix;
extern KMSTRIPHEAD 		StripHead;
extern DCKVECTOR		camPos;
extern DCKVECTOR		camTarg;

//----- [ GLOBALS ] -----------------------------------------------------------------------------

KMSTATUS 				kmStatus;			// status of KAMUI2 API
KMVERSIONINFO			kmVersionInfo;		// KAMUI2 version information
KMSYSTEMCONFIGSTRUCT	kmSystemConfig;		// KAMUI2 system configuration

KMSURFACEDESC 	primarySurfaceDesc;			// primary surface
KMSURFACEDESC 	renderSurfaceDesc;			// render surface (back buffer)
KMSURFACEDESC 	textureSurfaceDesc;			// texture surface

PKMSURFACEDESC 	pFB[2] = { NULL, NULL };	// ptrs to our framebuffers

KMVERTEXBUFFDESC vertexBufferDesc;			// global vertex buffer

PKMDWORD 		dwDataPtr;

//----- [ FUNCTION IMPLEMENTATION ] --------------------------------------------------------------

/*	--------------------------------------------------------------------------------
	Function 	: my_memcopy
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void my_memcopy(char *source, char *dest, int size)
{
	register int i;
	
	for(i=0;i<size;i++)
		*dest++ = *source++;
}

/*	--------------------------------------------------------------------------------
	Function 	: getShort
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

short getShort(char *ptr)
{
	short	testShort;

	testShort  = (*ptr++)       & 0xff;
	testShort |= (*ptr++ <<  8) & 0xff00;
		
	return testShort;
}

/*	--------------------------------------------------------------------------------
	Function 	: getInt
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int getInt(char *ptr)
{
	int	testInt;

	testInt  = ((*ptr++)       & 0xff);
	testInt |= ((*ptr++ <<  8) & 0xff00);
	testInt |= ((*ptr++ << 16) & 0xff0000);
	testInt |= ((*ptr++ << 24) & 0xff000000);
		
	return testInt;
}

/*	--------------------------------------------------------------------------------
	Function 	: getFloat
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

float getFloat(char *ptr)
{
	register int 	i;
	float			testFloat;
	char			*dest;
	
	dest = &testFloat;
	for(i=sizeof(float);i>0;i--)
		*dest++ = *ptr++;

	return testFloat;
}



