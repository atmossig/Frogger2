/*
	This file is part of ObjEdit2, Copyright 1997 Interactive Studios Ltd

	File        : crc32.cpp
	Description : CRC generating code
	Author      : R. Hackett
	Date        : 15.02.97
*/


#define POLYNOMIAL 0x04c11db7L

static unsigned long CRCtable[256];

/*	--------------------------------------------------------------------------------
	Function : InitCRCTable
	Purpose : Initialise the CRC table
	Parameters : 
	Returns : 
	Info : 
*/

void InitCRCTable()
{
	register int i, j;
	register unsigned long CRCaccum;

	for (i=0; i<256; i++)
		{
		CRCaccum = ((unsigned long)i<<24);
		for (j=0; j<8; j++)
			{
			if (CRCaccum & 0x80000000L)
				CRCaccum = (CRCaccum<<1)^POLYNOMIAL;
			else
				CRCaccum = (CRCaccum<<1);
			}
		CRCtable[i] = CRCaccum;
		}
}


/*	--------------------------------------------------------------------------------
	Function : UpdateCRC
	Purpose : update a CRC for specified data
	Parameters : existing CRC, data ptr, size of data in bytes
	Returns : 
	Info : 
*/

unsigned long UpdateCRC(unsigned long CRCaccum, char *ptr, int size)
{
	register int i, j;

	for (j=0; j<size; j++)
		{
		i = ((int)(CRCaccum>>24)^(*ptr++))&0xff;
		CRCaccum = (CRCaccum<<8)^CRCtable[i];
		}
	return CRCaccum;
}

