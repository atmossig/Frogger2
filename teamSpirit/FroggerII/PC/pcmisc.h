/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: pcmisc.h
	Programmer	: Andy Eder
	Date		: 05/05/99 17:43:09

----------------------------------------------------------------------------------------------- */

#ifndef PCMISC_H_INCLUDED
#define PCMISC_H_INCLUDED


#ifdef __cplusplus
extern "C"
{
#endif

extern short mouseX;
extern short mouseY;

//----- [ FUNCTION PROTOTYPES ] ----------------------------------------------------------------//

void InitPCSpecifics();
void DeInitPCSpecifics();
short *GetGelfBmpDataAsShortPtr(char *filename);
void FreeGelfData(void *me);

#ifdef __cplusplus
}
#endif


#endif
