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
	DEMO_MODE,

	SLOT_SELECT_MODE,	
		
	TITLE_MODE,	

	GAMETYPE_MODE,
	LEVELSELECT_MODE,

	OPTIONS_MODE,
	SOUNDADJUST_MODE,
	CONTROLCONFIG_MODE,
	HIGHSCORE_MODE,

	DEVELOPMENTMENU_MODE,
	OBJVIEW_MODE,
	SNDVIEW_MODE,
	DEVELOPMENT_MODE,
	MULTIPLAYER_MODE,

LANGUAGE_SELECT_MODE,
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

extern void RunFrontEnd			( void );



#endif