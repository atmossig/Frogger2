/*
	This file is part of WarGames, Copyright 1996 Interactive Studios Ltd

	File        : debug.h
	Description : Debugging/Error reporting code
	Author      : John Whigham
	Date        : 12/04/96
	Version     : 0.01
*/

//#ifndef __DEBUG_H__
//#define __DEBUG_H__




// ------------------
// Constants & Macros

#define DEBUG _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "WGS", 

#define MAXDEBUGFILES		32

// --------------------
// Structures & Classes

typedef struct tagDebugType {
	char			*dbfname[MAXDEBUGFILES];		// debug file filenames
	int				lasthandle;						// last handle opened
} DebugType;



// -------
// Globals

extern DebugType		debug;



// ----------
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : dprintf
	Purpose : print a string to the debug terminal
	Parameters : as for printf
	Returns : 
	Info : 
*/

void dbprintf(char *format, ...);


int OpenDebugFile(char *leaf);


/*	--------------------------------------------------------------------------------
	Function : dbfprintf
	Purpose : print a string to the last opened debug file
	Parameters : as for printf
	Returns : 
	Info :
*/

void dbfprintf(char *format, ...);


/*	--------------------------------------------------------------------------------
	Function : ToDebugFile
	Purpose : print a string to a given debug file
	Parameters : file handle, as for printf
	Returns : 
	Info :
*/

void ToDebugFile(int handle, char *format, ...);


/*	--------------------------------------------------------------------------------
	Function : ToLastDebugFile
	Purpose : print a string to the last opened debug file
	Parameters : as for printf
	Returns : 
	Info :
*/

void ToLastDebugFile(char *format, ...);


/*	--------------------------------------------------------------------------------
	Function : CloseDebugFile
	Purpose : close a debug file
	Parameters : file handle
	Returns : 
	Info :
*/

void CloseDebugFile(int handle);


/*	--------------------------------------------------------------------------------
	Function : CloseLastDebugFile
	Purpose : close the last debug file opened
	Parameters : 
	Returns : 
	Info :
*/

void CloseLastDebugFile();



//#endif // __DEBUG_H__
