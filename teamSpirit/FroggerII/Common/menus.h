/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: menus.h
	Programmer	: James Healey
	Date		: 27/05/99

----------------------------------------------------------------------------------------------- */

#ifndef MENUS_H_INCLUDED
#define MENUS_H_INCLUDED




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

typedef struct LEVELNAMESTRUCT
{
	char name[64];
} LNAMESTRUCT;

extern struct slotSelectStateStruct slotSelectState;

extern SPRITE *sp;

extern void RunDemoOption	( void );

extern void RunSaveLoadSelect ( void );
extern void RunTitleScreen ( void );
extern void RunLevelSelect ( void );
extern void RunSelectAllLevels( void );

extern void RunGameMode ( void );

extern void RunOptionsMode ( void );
extern void RunHiScoresMode ( void );

extern void RunPauseMenu ( void );

#endif