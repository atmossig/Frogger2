/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: savegame.c
	Programmer	: James Healey
	Date		: 07/06/99

----------------------------------------------------------------------------------------------- */

//***********************************
// Defines

//***********************************
// System Includes
#include <stdio.h>
#include <ultra64.h>

//***********************************
// User Includes

#include "incs.h"

//***********************************
// Globals

//***********************************
// Function Definitions


/*	--------------------------------------------------------------------------------
	Function 	: InitSaveData
	Purpose 	: calls all of the functions required to setup save data
	Parameters 	: void
	Returns 	: void
	Info 		: void
*/
void InitSaveData ( void )
{
	SetUpHiScoreData();
	SetUpSavedGames();
}


/*	--------------------------------------------------------------------------------
	Function 	: SetUpHiScoreData
	Purpose 	: set's up the data file for the hiscore table.
	Parameters 	: void
	Returns 	: void
	Info 		: void
*/
void SetUpHiScoreData ( void )
{
	FILE *fp;

	fp = fopen ( "x:\\teamspirit\\pcversion\\savedata\\hiscore.dat", "r" );

	if ( fp == NULL )
	{
		// Let's write out the data to a new file, first time playing or some idiot has deleted the file......
		fp = fopen ( "x:\\teamspirit\\pcversion\\savedata\\hiscore.dat", "w" );
		
	/*	for ( i = 0; i < MAX_HISCORE_SLOTS; i++ )
		{
			Convert3CharTo2Char( &hiScoreData[i].name, scoreName[i] );
		}
		// ENDFOR*/

		fwrite ( hiScoreData, sizeof ( HISCORE ), MAX_HISCORE_SLOTS, fp );
	}
	else
	{
		// This loads in the data from the existing file
		fread ( hiScoreData, sizeof ( HISCORE ), MAX_HISCORE_SLOTS, fp );
	}
	// ENDELSIF
	close  ( fp );
}

/*	--------------------------------------------------------------------------------
	Function 	: SaveHiScores
	Purpose 	: saves out the data file containing the hiscores.
	Parameters 	: void
	Returns 	: void
	Info 		: void
*/
void SaveHiScores ( void )
{
	FILE *fp;

	// Save out the new hiscore data, some one has a new hiscore, no cannot be!!!!!!!!
	fp = fopen ( "x:\\teamspirit\\pcversion\\savedata\\hiscore.dat", "w" );

	fwrite ( hiScoreData, sizeof ( HISCORE ), MAX_HISCORE_SLOTS, fp );

	close ( fp );

}


/*	--------------------------------------------------------------------------------
	Function 	: SetUpSavedGames
	Purpose 	: creates an existing data file or loads one in.
	Parameters 	: void
	Returns 	: void
	Info 		: void
*/
void SetUpSavedGames ( void )
{
	FILE *fp;

	fp = fopen ( "x:\\teamspirit\\pcversion\\savedata\\gamedata.dat", "r" );

	if ( fp == NULL )
	{
		// Let's write out the data to a new file, first time playing or some idiot has deleted the file......
		fp = fopen ( "x:\\teamspirit\\pcversion\\savedata\\gamedata.dat", "w" );
		fwrite ( saveSlots, sizeof ( SAVE_SLOT ), MAX_SAVE_SLOTS, fp );
	}
	else
	{
		// This loads in the data from the existing file
		fread ( saveSlots, sizeof ( SAVE_SLOT ), MAX_SAVE_SLOTS, fp );
	}
	// ENDELSEIF

	close ( fp );
}

/*	--------------------------------------------------------------------------------
	Function 	: SaveGameData
	Purpose 	: saves out the data file containing the save games.
	Parameters 	: void
	Returns 	: void
	Info 		: void
*/
void SaveGameData ( void )
{
	FILE *fp;

	fp = fopen ( "x:\\teamspirit\\pcversion\\savedata\\gamedata.dat", "w" );

	fwrite ( saveSlots, sizeof ( SAVE_SLOT ), MAX_SAVE_SLOTS, fp );

	close ( fp );
}

void Convert3CharTo2Char ( char twoChar[2], char threeChar[3] )
{
	char ta, tb, tc;
	char a, b, c;

	ta = ( threeChar[0] - 'a' ) & 31;
	tb = ( threeChar[1] - 'a' ) & 31;
	tc = ( threeChar[2] - 'a' ) & 31;

	twoChar[0] = ( a << 3 ) | ( ( b >> 2 ) & 7 );
	twoChar[1] = ( b << 6 ) | ( c << 1 );


/*	char a,b,c;
	char a1,b2,c3;
	char c1,c2;

	char t;

	char test[2];
	char test2[3];

	sprintf ( player[0].name, "jam" );

	a = (player[0].name[0] - 'a') & 31;
	b = (player[0].name[1] - 'a') & 31;
	c = (player[0].name[2] - 'a') & 31;

	// aaaa abbb = c1
	c1 = (a << 3) | ((b >> 2) & 7);
	// bbcc cccn = c2
	c2 = (b << 6) | (c << 1);


	/// 

	// 000a aaaa
	a1 = (c1 >> 3) + 'a';

	// 000b bbbb
	b2 = (((c1 & 7) << 2) | (c2>>6)) + 'a';

	// 0bbc cccc
	c3 = ((c2 >> 1) & 31) + 'a';*/

}

void Convert2CharTo2Char ( char threeChar[3], char twoChar[2] )
{
	threeChar[0] = (twoChar[0] >> 3) + 'a';
	threeChar[1] = (((twoChar[0] & 7) << 2) | (twoChar[1]>>6)) + 'a';
	threeChar[2] = ((twoChar[1] >> 1) & 31) + 'a';
}

