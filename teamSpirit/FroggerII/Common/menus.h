/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: menus.h
	Programmer	: James Healey
	Date		: 27/05/99

----------------------------------------------------------------------------------------------- */

#ifndef MENUS_H_INCLUDED
#define MENUS_H_INCLUDED


typedef struct LEVELNAMESTRUCT
{
	char name[64];
} LNAMESTRUCT;

extern SPRITE *sp;

extern void RunTitleScreen ( void );
extern void RunLevelSelect ( void );
extern void RunPauseMenu ( void );
extern void RunHiscoreScreen( );

#endif

/*
enum
{
	OPTION_MODE,
	SLOT_MODE,
	YESNO_MODE,
	NAME_MODE,
};

struct slotSelectStateStruct
{
	unsigned long mode;
};

extern struct slotSelectStateStruct slotSelectState;

extern void RunDemoOption	( void );

extern void RunSaveLoadSelect ( void );

extern void RunSelectAllLevels( void );

extern void RunGameMode ( void );

extern void RunOptionsMode ( void );
extern void RunHiScoresMode ( void );

*/