/*
	This file is part of ObjEdit2, Copyright 1997 Interactive Studios Ltd

	File        : crc32.h
	Description : CRC generation code
	Author      : R. Hackett
	Date        : 15.02.97
*/



/*	--------------------------------------------------------------------------------
	Function : InitCRCTable
	Purpose : Initialise the CRC table
	Parameters : 
	Returns : 
	Info : 
*/

void InitCRCTable();


/*	--------------------------------------------------------------------------------
	Function : UpdateCRC
	Purpose : update a CRC for specified data
	Parameters : existing CRC, data ptr, size of data in bytes
	Returns : 
	Info : 
*/

unsigned long UpdateCRC(unsigned long CRCaccum, char *ptr, int size);


