/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: backdrop.h
	Programmer	: David
	Date		: 
	Purpose		: Backdrop handling functions

----------------------------------------------------------------------------------------------- */

#ifndef BACKDROP_H_INCLUDED
#define BACKDROP_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

int InitBackdrop(const char *filename);
void FreeBackdrop(void);
void DrawBackdrop(void);

#ifdef __cplusplus
}
#endif


#endif
