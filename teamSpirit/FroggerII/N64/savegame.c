/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: savegame.c
	Programmer	: James Healey
	Date		: 07/06/99

----------------------------------------------------------------------------------------------- */

//***********************************
// Defines
#define F3DEX_GBI

#define EEPROM_DELAY 30000

//***********************************
// System Includes
#include <ultra64.h>

//***********************************
// User Includes
#include "incs.h"

//***********************************
// Globals

SAVE_SLOT saveSlot[NUM_SAVE_SLOTS] = 
{
	{"jim", 50000, 0, 0,},
	{"and", 40000, 0, 0,},
	{"alx", 30000, 0, 0,},
	{"mat", 20000, 0, 0,},
	{"jok", 10000, 0, 0,},
	{"sim", 5000, 0, 0,},
};


LEVEL_HISCORE levelTable[MAX_WORLDS*3] = {
	{ "AAA", 5000 },
	{ "BBB", 4000 },
	{ "CCC", 3000 },
	{ "AAA", 5000 },
	{ "BBB", 4000 },
	{ "CCC", 3000 },
	{ "AAA", 5000 },
	{ "BBB", 4000 },
	{ "CCC", 3000 },
	{ "AAA", 5000 },
	{ "BBB", 4000 },
	{ "CCC", 3000 },
	{ "AAA", 5000 },
	{ "BBB", 4000 },
	{ "CCC", 3000 },
	{ "AAA", 5000 },
	{ "BBB", 4000 },
	{ "CCC", 3000 },
	{ "AAA", 5000 },
	{ "BBB", 4000 },
	{ "CCC", 3000 },
	{ "AAA", 5000 },
	{ "BBB", 4000 },
	{ "CCC", 3000 },
	{ "AAA", 5000 },
	{ "BBB", 4000 },
	{ "CCC", 3000 },
};

char	validEeprom = FALSE;

char	eepromMessageQueue [ MAX_EEPROM_MESSAGES ];
char	eepromMessageNum;

short	eepromPresent = FALSE;

//LEVEL_HISCORE	levelTable [ MAX_WORLDS ] [ 4 ];
SAVE_SLOT		saveSlot [ NUM_SAVE_SLOTS ];
GAME_PROGRESS	gameProgress;

//***********************************
// Function Definitions


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void Wait ( short time )
{
	u64	curTime, stTime;

	stTime = curTime = OS_CYCLES_TO_USEC ( osGetTime ( ) );

	while ( ( curTime - stTime ) < time )
		curTime = OS_CYCLES_TO_USEC ( osGetTime ( ) );
	// ENDWHILE
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitEepromMessageQueue ( void )
{
	char x;

	//initialise eeprom message queue
	eepromMessageNum = 0;
	for(x = 0; x < MAX_EEPROM_MESSAGES; x++)
		eepromMessageQueue[x] = EEPROM_IDLE;
	// ENDFOR
}



/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitEeprom ( void )
{
	eepromPresent = osEepromProbe(&controllerMsgQ);
	PostEepromMessage(EEPROM_VALID);
	while(eepromMessageQueue[0] != EEPROM_IDLE);	//wait for eeprom to finish
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void GetEepromMessage ( void )
{
	short x;

	for(x = 0; x < MAX_EEPROM_MESSAGES - 1; x++)
	{
		eepromMessageQueue[x] = eepromMessageQueue[x + 1];
	}
	eepromMessageQueue[x] = EEPROM_IDLE;
	eepromMessageNum--;
}



/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void PostEepromMessage ( short message ) 
{
	//add new message to the queue
	eepromMessageQueue[eepromMessageNum++] = message;
}



char *cartID = {"GVR8"};
		
void SaveID(void)
{
	PostEepromMessage(EEPROM_SAVEID);
	while(eepromMessageQueue[0] != EEPROM_IDLE);	//wait for eeprom to finish
}	

void EepromSaveID(void )
{
	short	res;
	char	buffer[8];

	sprintf(buffer, cartID);

	if(eepromPresent)
	{
		res = osEepromLongWrite(&controllerMsgQ, 0, (u8 *)buffer, 8);
		Wait(EEPROM_DELAY);
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void EepromValid ( void )
{
	char	buffer[8];
	short	res;

	if(eepromPresent)
	{																					
		res = osEepromLongRead(&controllerMsgQ, 0, (u8 *)buffer, 8);
		Wait(EEPROM_DELAY);
		if((buffer[0] == cartID[0]) && (buffer[1] == cartID[1]) && (buffer[2] == cartID[2]) && (buffer[3] == cartID[3]))
		{
			//cart is empty - must fill in the data
			validEeprom = TRUE;
		}
		else
			validEeprom = FALSE;

	}

}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void SaveLevelScores ( void )
{
	PostEepromMessage ( EEPROM_SAVELEVELSCORES );
	while ( eepromMessageQueue[0] != EEPROM_IDLE );	//wait for eeprom to finish
}



/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void EepromSaveLevelScores ( void )
{
	short	res = 1;

	if ( eepromPresent )
	{
		do
		{
			res = osEepromLongWrite(&controllerMsgQ, 1, (u8 *)levelTable, sizeof ( LEVEL_HISCORE ) * (MAX_WORLDS*3));
			Wait(EEPROM_DELAY);
		}while(res != 0);
	}
	// ENDIF
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void LoadLevelScores ( void )
{
	PostEepromMessage ( EEPROM_LOADLEVELSCORES );
	while ( eepromMessageQueue[0] != EEPROM_IDLE );	//wait for eeprom to finish
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void EepromLoadLevelScores ( void )
{
	short	res = 1;

	if ( eepromPresent )
	{
		do
		{
			res = osEepromLongRead(&controllerMsgQ, 1, (u8 *)levelTable, sizeof ( LEVEL_HISCORE ) * (MAX_WORLDS*3) );
			Wait(EEPROM_DELAY);
		}while(res != 0);
	}
	// ENDIF
}


/*	--------------------------------------------------------------------------------
	Function 	: SaveGameProgress
	Purpose 	: Tell the controller thread to store how far the frog has got to, 
					and which keys he has collected
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void SaveGameProgress( void )
{
	PostEepromMessage ( EEPROM_SAVEGAMEPROGRESS );
	while ( eepromMessageQueue[0] != EEPROM_IDLE );	//wait for eeprom to finish
}


/*	--------------------------------------------------------------------------------
	Function 	: EepromSaveGameProgress
	Purpose 	: Store how far the frog has got to, and which keys he has collected
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void EepromSaveGameProgress( void )
{
	short	res = 1;

	if ( eepromPresent )
	{
		do
		{
			res = osEepromLongWrite(&controllerMsgQ, /*sizeof(LEVEL_HISCORE)+*/1, (u8 *)&gameProgress, sizeof(GAME_PROGRESS));
			Wait(EEPROM_DELAY);
		}while(res != 0);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: LoadGameProgress
	Purpose 	: Tell the controller thread to reload how far the frog has got to, 
					and which keys he has collected
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void LoadGameProgress( void )
{
	PostEepromMessage ( EEPROM_LOADGAMEPROGRESS );
	while ( eepromMessageQueue[0] != EEPROM_IDLE );	//wait for eeprom to finish
}


/*	--------------------------------------------------------------------------------
	Function 	: EepromLoadGameProgress
	Purpose 	: Reload how far the frog has got to, and which keys he has collected
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void EepromLoadGameProgress( void )
{
	short	res = 1;

	if ( eepromPresent )
	{
		do
		{
			res = osEepromLongRead(&controllerMsgQ, /*sizeof(LEVEL_HISCORE)+*/1, (u8 *)&gameProgress, sizeof(GAME_PROGRESS));
			Wait(EEPROM_DELAY);
		}while(res != 0);
	}
}

//OSMesgQueue	eepromMsgQ;
//OSMesg		eepromMsgBuf;
//EEPROM_CRCS eepromCRCS;




 // TEST STUFF

/*HISCORE_ENTRY	hiscoreTable[NUM_HISCORE_ENTRIES] = {
	{500000, "STE"},
	{400000, "WUK"},
	{300000, "JIM"},
	{200000, "KEV"},
	{100000, "MAT"},
	{50000, "DIB"},
};
	*/
/*HISCORE_ENTRY	hiscoreTableCopy[NUM_HISCORE_ENTRIES] = {
	{500000, "STE"},
	{400000, "WUK"},
	{300000, "JIM"},
	{200000, "KEV"},
	{100000, "MAT"},
	{50000, "DIB"},
};
	  */
/*SAVE_SLOT		saveSlot[NUM_SAVE_SLOTS] = {
	{"JIM", 50000, 3, 2},
	{"MAT", 40000, 2, 1},
	{"AND", 30000, 2, 0}, 
	{"JOF", 20000, 0, 2},
	{"SIM", 10000, 0, 2}, 
	{"ALX", 5000,  0, 0},
};

SAVE_SLOT blankSlot = {"\r",0/*, 0, 0, 0, 0, 0/*DIFFICULTY_MEDIUM, 0, 0, 0, 0, 0*//*};*/

/*short		currentSaveSlot = 0;
char		currentName[4];
short	eepromRequest = EEPROM_IDLE;

char	eepromMessageNum = 0;
	 */



/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
/*void LoadHiscoreTable()
{
	PostEepromMessage(EEPROM_LOADHISCORES);
	while(eepromMessageQueue[0] != EEPROM_IDLE);	//wait for eeprom to finish
}
void SaveHiscoreTable()
{
	PostEepromMessage(EEPROM_SAVEHISCORES);
	while(eepromMessageQueue[0] != EEPROM_IDLE);	//wait for eeprom to finish
}
void LoadBestTimes()
{
	PostEepromMessage(EEPROM_LOADTIMES);
	while(eepromMessageQueue[0] != EEPROM_IDLE);	//wait for eeprom to finish
}
void SaveBestTimes()
{
	PostEepromMessage(EEPROM_SAVETIMES);
	while(eepromMessageQueue[0] != EEPROM_IDLE);	//wait for eeprom to finish
}
void LoadSlots()
{
	PostEepromMessage(EEPROM_LOADSLOT);
	while(eepromMessageQueue[0] != EEPROM_IDLE);	//wait for eeprom to finish
}

void SaveSlots()
{
	PostEepromMessage(EEPROM_SAVESLOT);
	while(eepromMessageQueue[0] != EEPROM_IDLE);	//wait for eeprom to finish
}
void LoadCRC()
{
	PostEepromMessage(EEPROM_LOADCRC);
	while(eepromMessageQueue[0] != EEPROM_IDLE);	//wait for eeprom to finish
}
void SaveCRC()
{
	PostEepromMessage(EEPROM_SAVECRC);
	while(eepromMessageQueue[0] != EEPROM_IDLE);	//wait for eeprom to finish
}

/*void EepromSaveHiscoreTable()
{
	short	res = 1;

	if(eepromPresent)
	{
		do
		{
			res = osEepromLongWrite(&controllerMsgQ, 1, (u8 *)hiscoreTable, sizeof(HISCORE_ENTRY) * NUM_HISCORE_ENTRIES);
			Wait(EEPROM_DELAY);
		}while(res != 0);
	}
}
void EepromLoadHiscoreTable()
{
	short	res = 1;

	if(eepromPresent)
	{
		do
		{
			res = osEepromLongRead(&controllerMsgQ, 1, (u8 *)hiscoreTable, sizeof(HISCORE_ENTRY) * NUM_HISCORE_ENTRIES);
			Wait(EEPROM_DELAY);
		}while(res != 0);
	}

} */
/*void EepromSaveBestTimes()
{
	short	res = 1;

	if(eepromPresent)
	{
		do
		{
//			res = osEepromLongWrite(&controllerMsgQ, 7, (u8 *)bestTimes, sizeof(BEST_TIME)*(RL_NUM_LEVELS+2));
			Wait(EEPROM_DELAY);
		}while(res != 0);
	}
}
void EepromLoadBestTimes()
{
	short	res = 1;

	if(eepromPresent)
	{
		do
		{
//			res = osEepromLongRead(&controllerMsgQ, 7, (u8 *)bestTimes, sizeof(BEST_TIME)*(RL_NUM_LEVELS+2));
			Wait(EEPROM_DELAY);
		}while(res != 0);

	}
} 
void EepromSaveSlots()
{
	short	res = 1;

	if(eepromPresent)
	{
		do
		{
			res = osEepromLongWrite(&controllerMsgQ, 7, (u8 *)saveSlot, sizeof(SAVE_SLOT) * NUM_SAVE_SLOTS);
			Wait(EEPROM_DELAY);
		}while(res != 0);
	}
}
void EepromLoadSlots()
{
	short	res = 1;

	if(eepromPresent)
	{
		do
		{
			res = osEepromLongRead(&controllerMsgQ, 7, (u8 *)saveSlot, sizeof(SAVE_SLOT) * NUM_SAVE_SLOTS);
			Wait(EEPROM_DELAY);
		}while(res != 0);
	}
}


/*void EepromSaveCRC()
{
	short	res;
	int j;

	eepromCRCS.hiscoreCRC = UpdateCRCData((char *)hiscoreTable,sizeof(HISCORE_ENTRY)*NUM_HISCORE_ENTRIES);
	//eepromCRCS.bestTimesCRC = UpdateCRCData((char *)bestTimes,sizeof(BEST_TIME)*(RL_NUM_LEVELS+2));
	for(j = 0;j < NUM_SAVE_SLOTS;j++)
		eepromCRCS.slotCRC[j] = UpdateCRCData((char *)&saveSlot[j],sizeof(SAVE_SLOT));

	if(eepromPresent)
	{
		res = osEepromLongWrite(&controllerMsgQ, 52, (u8 *)&eepromCRCS, sizeof(EEPROM_CRCS));
		Wait(EEPROM_DELAY);
	}
}

/*void EepromLoadCRC()
{
	short	res;

	if(eepromPresent)
	{
		res = osEepromLongRead(&controllerMsgQ, 52, (u8 *)&eepromCRCS, sizeof(EEPROM_CRCS));
		Wait(EEPROM_DELAY);
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
/*void LoadGame(short num)
{

}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
/*void SaveGame(short num)
{

}



/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
/*void UpdateSaveSlot(char conquered)
{
	saveSlot[currentSaveSlot].name[0] = currentName[0];
	saveSlot[currentSaveSlot].name[1] = currentName[1];
	saveSlot[currentSaveSlot].name[2] = currentName[2];

	//saveSlot[currentSaveSlot].score = player.score + player.garibs*10;
	//saveSlot[currentSaveSlot].lives = player.numLives;
//	if(gameInfo.flags & LEVEL_COMPLETED)
//		saveSlot[currentSaveSlot].levelCompleted |= (1 << levelInfo[gameInfo.lastLevel].realLevel);
//	if(conquered)
//		saveSlot[currentSaveSlot].levelConquered |= (1 << levelInfo[gameInfo.lastLevel].realLevel);

//	saveSlot[currentSaveSlot].numWorldsCompleted = gameInfo.numWorldsCompleted;
//	saveSlot[currentSaveSlot].numBallsInPlace = gameInfo.numBallsInPlace;

	SaveSlots();
	SaveCRC();

}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
/*void CheckCRC()
{
	int j;
	unsigned int crc;

	for(j = 0;j < NUM_SAVE_SLOTS;j++)
	{
		crc = UpdateCRCData((char *)&saveSlot[j],sizeof(SAVE_SLOT));
		if(crc != eepromCRCS.slotCRC[j])
		{
			memcpy((char *)&saveSlot[j],(char *)&blankSlot,sizeof(SAVE_SLOT));
		}
	}
	crc = UpdateCRCData((char *)hiscoreTable,sizeof(HISCORE_ENTRY)*NUM_HISCORE_ENTRIES);
	if(crc != eepromCRCS.hiscoreCRC)
	{
		memcpy (hiscoreTable,hiscoreTableCopy,sizeof (HISCORE_ENTRY) * NUM_HISCORE_ENTRIES);			
	}
	//crc = UpdateCRCData((char *)bestTimes,sizeof(BEST_TIME)*(RL_NUM_LEVELS+2));
	/*if(crc != eepromCRCS.bestTimesCRC)
	{
		memcpy (&bestTimes[RL_ATLANTIS_LEVEL1],&bestTimesCopy[RL_ATLANTIS_LEVEL1],sizeof(BEST_TIME)*5);
		memcpy (&bestTimes[RL_CARNIVAL_LEVEL1],&bestTimesCopy[RL_CARNIVAL_LEVEL1],sizeof(BEST_TIME)*5);
		memcpy (&bestTimes[RL_PIRATES_LEVEL1],&bestTimesCopy[RL_PIRATES_LEVEL1],sizeof(BEST_TIME)*5);
		memcpy (&bestTimes[RL_PREHIST_LEVEL1],&bestTimesCopy[RL_PREHIST_LEVEL1],sizeof(BEST_TIME)*5);
		memcpy (&bestTimes[RL_FORTRESS_LEVEL1],&bestTimesCopy[RL_FORTRESS_LEVEL1],sizeof(BEST_TIME)*5);
		memcpy (&bestTimes[RL_OOTW_LEVEL1],&bestTimesCopy[RL_OOTW_LEVEL1],sizeof(BEST_TIME)*5);
	} */
/*}
*/