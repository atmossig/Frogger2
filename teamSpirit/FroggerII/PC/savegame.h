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

#define NUM_SAVE_SLOTS		6

typedef struct 
{
	char name[3];
	int score;
} LEVEL_HISCORE;


typedef struct
{
	// Player
	char	name[3];
	long	score;
//	char	livesnhealth; // 6-2 l-h 
		
	// State
//	char	worldState[9]; // 2-2-2-2 l-l-l-B   0 = Closed; 1 = gold; 2= silv 3  = bronz... The first "closed" world is in fact open but uncompleted
//	short 	bonuses; // Each bit is whether bonus is open


	short	currentWorld;
	short	currentLevel;

/*	int		levelCompleted;
	int		levelConquered;
	int		score;
	char	lives;
	char	difficulty;
	char	numWorldsCompleted;
	char	numBallsInPlace;
	int		levelVisited;
	short	sfxVol;
	short	musVol;*/
}SAVE_SLOT;



extern LEVEL_HISCORE	levelTable [ MAX_WORLDS*3 ];
extern SAVE_SLOT		saveSlot [ NUM_SAVE_SLOTS ];

//***********************************
// Function Prototypes

extern void Wait ( short time );

extern void InitEepromMessageQueue	( void );
extern void InitEeprom				( void );
extern void GetEepromMessage		( void );
extern void PostEepromMessage		( short message );

extern void SaveID					( void );
extern void EepromSaveID			( void );
extern void EepromValid				( void );

extern void SaveLevelScores			( void );
extern void EepromSaveLevelScores	( void );

extern void LoadLevelScores			( void );
extern void EepromLoadLevelScores	( void );


#endif