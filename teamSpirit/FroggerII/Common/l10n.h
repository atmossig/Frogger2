/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: l10n.h
	Programmer	: David Swift (from $/Tools/PSXLibraries/ISLLocal)
	Date		: 23/03/99

----------------------------------------------------------------------------------------------- */

#ifndef _L10N_H_INCLUDED
#define _L10N_H_INCLUDED

// use this macro to access the string you want
#define TEXTSTR(n) (gameTextStr[n])

extern void	**gameTextStr;

/**************************************************************************
	FUNCTION:	gameTextInitFromFile
	PURPOSE:	Read in the text from a gametext file
	PARAMETERS:	Filename, num strings, number of languages, current language
	RETURNS:	
**************************************************************************/

void gameTextInit(char *fName, int numStrings, int numLang, int currLang);


/**************************************************************************
	FUNCTION:	gameTextDestroy
	PURPOSE:	Free up text
	PARAMETERS:	
	RETURNS:	
**************************************************************************/

void gameTextDestroy();

#endif