/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: savegame.h
	Programmer	: James Healey
	Date		: 08/06/99

----------------------------------------------------------------------------------------------- */

#ifndef SAVEGAME_H_INCLUDED
#define SAVEGAME_H_INCLUDED


//***********************************
// Defines

//***********************************
// Type Defines


//***********************************
// Function Prototypes

extern void InitSaveData			( void );
extern void SetUpHiScoreData		( void );
extern void SaveHiScores			( void );

extern void SetUpSavedGames			( void );
extern void SaveGameData			( void );

extern void CalculateHiScoreList	( void );

//***********************************
// Global Externs

#endif