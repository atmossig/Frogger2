/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: flatpack.c
	Programmer	: David Swift (from code by Richard Hackett)
	Date		: 25 May 00

----------------------------------------------------------------------------------------------- */

#include <islutil.h>
#include <islmem.h>

/*	--------------------------------------------------------------------------------
	Function		: getFileFromStack()
	Purpose			: Get ptr to (sub)file from inside flatstack file
	Parameters		: Ptr to flatstack file, name of (sub)file, ptr to length int
	Returns			: Ptr to base of required file
*/

char *getFileFromStack(char *stackFile, char *name, int *length)
{
	unsigned long 	crc;
	int				loop, numFiles, *iPtr;

	crc = utilStr2CRC(name);
	iPtr = (int *)stackFile;
	numFiles = iPtr[1];
	for(loop=0; loop<numFiles; loop++)
	{
		if (iPtr[2+loop*3]==crc)
		{
			utilPrintf("STACK FILE: %s\n", name);
			if (length!=NULL)
				*length = iPtr[2+loop*3+2];
			return stackFile+iPtr[2+loop*3+1];
		}
	}
	utilPrintf("Couldn't find '%s' in stack file\n", name);
	return NULL;
}
