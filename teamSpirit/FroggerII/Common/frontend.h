/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: frontend.h
	Programmer	: James Healey
	Date		: 30/03/99

----------------------------------------------------------------------------------------------- */

#ifndef FRONTEND_H_INCLUDED
#define FRONTEND_H_INCLUDED

enum frontEndModes
{
	//  Frontend / Title screen Modes
	TITLE_MODE,
	LEVELSELECT_MODE,
	OBJVIEW_MODE,
	DEVELOPMENTMENU_MODE
};

struct frontEndStateStruct
{
	unsigned long mode;
};


//----- [ GLOBALS ] ----------------------------------------------------------------------------//

extern struct frontEndStateStruct frontEndState;

extern int frameCount;
extern int frameCount2;
extern int dispFrameCount;

extern char	ActiveController;


//----- [ FUNCTION PROTOTYPES ] ----------------------------------------------------------------//

void RunFrontEnd();
void GameLoop();

void RunLevelSelect();
void RunTitleScreen();

#endif