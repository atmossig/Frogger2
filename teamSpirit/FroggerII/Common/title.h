/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: title.h
	Programmer	: Matthew Cloy
	Date		: 10/11/98

----------------------------------------------------------------------------------------------- */

#ifndef TITLE_H_INCLUDED
#define TITLE_H_INCLUDED

extern SPRITEOVERLAY *mySpr1;
extern SPRITEOVERLAY *mySpr2;
extern SPRITEOVERLAY *sprOver;


extern unsigned long levelPlayingTimer;

extern void RunTitleScreen	( void );
extern void RunLevelsDone	( void );
extern void RunTmpTitle	( void );

extern void CreateOverlays	( void );


#endif