/*

	Trigger and event codes used by Frogger2 scripting system

	NOTE: This file is SHARED in sourcesafe between the Frogger2/Common
	project	and the ETC (Compiler) project. 
*/

#ifndef _INCLUDED_CODES_H
#define _INCLUDED_CODES_H

enum COMMANDTYPES
{
	EV_DEBUG = 0,
	EV_CHANGEACTORSCALE,
	EV_TOGGLEPLATFORMMOVE,
	EV_TOGGLEENEMYMOVE,
	EV_TOGGLETILELINK,
	EV_PLAYSFX,
	EV_CHANGELEVEL,
	EV_ANIMATEACTOR,

	C_IF = 0xF0,
	C_AND,
	C_OR,
	NOSUCHCOMMAND = 0xFF
};

enum TRIGGERTYPES
{
	TR_ENEMYONTILE = 0,
	TR_FROGONTILE,
	TR_FROGONPLATFORM,
	TR_WITHINRADIUS,
};

#endif