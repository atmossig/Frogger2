/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: frontend.h
	Programmer	: Andy Eder (originally Random Healey, but now rewritten)
	Date		: 14/10/99 11:58:26

----------------------------------------------------------------------------------------------- */

#ifndef FRONTEND_H_INCLUDED
#define FRONTEND_H_INCLUDED


//----- [ GLOBALS ] ----------------------------------------------------------------------------//

extern int frameCount;
extern int frameCount2;
extern int dispFrameCount;

extern char	ActiveController;


//----- [ FUNCTION PROTOTYPES ] ----------------------------------------------------------------//

extern void GameLoop();

extern void RunLevelSelect();
extern void RunTitleScreen();

#endif