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

extern void Convert3CharTo2Char ( char twoChar[2], char threeChar[3] );
extern void Convert2CharTo3Char ( char threeChar[3], char twoChar[2] );

//***********************************
// Global Externs

#endif