/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: general.h
	Programmer	: Matthew Cloy
	Date		: 12/01/99

----------------------------------------------------------------------------------------------- */


#ifndef GENERAL_H_INCLUDED
#define GENERAL_H_INCLUDED

#define inline __inline

extern unsigned long gstrcmp(char *a, char *b);
extern unsigned long gstrlen(const char *a);
extern void stringChange ( char *name );
inline extern void smemcpy( unsigned short *a, unsigned short *b, unsigned long size );
inline extern void lmemcpy( unsigned long *a, unsigned long *b, unsigned long size );

extern GAMETILE* FindNearestTile(VECTOR v);
extern int MemLoadEntities(const void* data, long size);

#endif