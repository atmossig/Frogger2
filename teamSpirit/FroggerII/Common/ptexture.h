/*
	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: ptexture.h
	Purpose		: Procedurally generated textures
	Programmer	: Jim Hubbard
	Date		: 2/12/99 10:30

----------------------------------------------------------------------------------------------- */
#ifndef __PTEXTURE_H
#define __PTEXTURE_H



typedef struct TAGPROCTEXTURE
{
	struct TAGPROCTEXTURE *next;

	char *buffer;
	short *palette;

	TEXTURE *tex;

	void  (*Update) ();

} PROCTEXTURE;


extern PROCTEXTURE *prcTexList;


extern void CreateAndAddProceduralTexture( TEXENTRY *tex, char *name );

extern void ProcessPTFire( PROCTEXTURE *pt );


#endif