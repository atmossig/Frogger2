/*

	This file is part of the M libraries,

	File		: mliReport.h
	Programmer	: Matthew Cloy
	Date		: 

----------------------------------------------------------------------------------------------- */

#ifndef MDBREPORT_H_INCLUDED
#define MDBREPORT_H_INCLUDED

// A value, string pair for reporting purposes!
struct VALUESTRING
{
	unsigned long value;
	char *string;
};

/*	--------------------------------------------------------------------------------
	Function	: rptShowBitfields
	Purpose		: Prints out the capabilities held in a bitfield from specified VALUESTRING
	Parameters	: (unsigned long value, VALUESTRING *me)
	Returns		: number of fields matched
	Info		: 
*/

long rptShowBitfields(unsigned long value, VALUESTRING *me);

/*	--------------------------------------------------------------------------------
	Function	: rptGetString
	Purpose		: Get a string from a Value, for example and error value
	Parameters	: (unsigned long value, VALUESTRING *me)
	Returns		: string that matches
	Info		: 
*/

char *rptGetString(unsigned long value, VALUESTRING *me);

#endif