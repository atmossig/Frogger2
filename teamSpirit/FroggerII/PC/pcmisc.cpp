/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: pcmisc.cpp
	Programmer	: Andy Eder
	Date		: 05/05/99 17:42:44

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>
#include "incs.h"
#include "gelf.h"

extern "C"
{

short mouseX = 20;
short mouseY = 20;


/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void InitPCSpecifics()
{
	gelfInit();
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void DeInitPCSpecifics()
{
	gelfShutdown();
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
short *GetGelfBmpDataAsShortPtr(char *filename)
{
	uchar palette[768];
	int pptr = -1;
	int xdim,ydim,bpp;
	short *bmp;
	
	bmp = (short *)gelfLoad_BMP(filename,NULL,(void**)&pptr,NULL,NULL,NULL,
		a565Card?GELF_IFORMAT_16BPP565:GELF_IFORMAT_16BPP555
		,GELF_IMAGEDATA_TOPDOWN);

	return bmp;
}


}
