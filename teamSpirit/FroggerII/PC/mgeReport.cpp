/*

	This file is part of the M libraries,

	File		: mliReport.c
	Programmer	: Matthew Cloy
	Date		: 

----------------------------------------------------------------------------------------------- */

//TODO: rename from mli to mge (Library -> Generic)

//TODO: move following out
// mge - Generic
// mdb - Debugging
// mdx - DirectX

/*	--------------------------------------------------------------------------------
	Function	: rptShowBitfields
	Purpose		: Prints out the capabilities held in a bitfield from specified VALUESTRING
	Parameters	: (unsigned long value, VALUESTRING *me)
	Returns		: number of fields matched
	Info		: 
*/

long rptShowBitfields(unsigned long value, VALUESTRING *me)
{
	unsigned long matched = 0;
	
	while (me->string)
	{
		if (me->value & value)
		{
			dp(me->string);
			matched++;
		}
		me++;
	}

	return matching;
}

/*	--------------------------------------------------------------------------------
	Function	: rptGetString
	Purpose		: Get a string from a Value, for example and error value
	Parameters	: (unsigned long value, VALUESTRING *me)
	Returns		: string that matches
	Info		: 
*/

char *rptGetString(unsigned long value, VALUESTRING *me)
{
	while (me->string)
	{
		if (me->value & value)
			return me->string;
		me++;
	}
	return NULL;
}
