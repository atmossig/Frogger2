/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: pcmisc.h
	Programmer	: Andy Eder
	Date		: 05/05/99 17:43:09

----------------------------------------------------------------------------------------------- */

#ifndef PCMISC_H_INCLUDED
#define PCMISC_H_INCLUDED

#pragma warning( disable: 4761 )	// disable spurious type conversion warnings!

#ifdef __cplusplus
extern "C"
{
#endif

extern short mouseX;
extern short mouseY;

//----- [ FUNCTION PROTOTYPES ] ----------------------------------------------------------------//

void InitPCSpecifics();
void DeInitPCSpecifics();
short *GetGelfBmpDataAsShortPtr(char *filename, unsigned long formats,int *dim);
void FreeGelfData(void *me);
void ClearTimers(void);
void StartTimer(int number,char *name);
void EndTimer(int number);
void PrintTimers(void);
void HoldTimers(void);

void *fileLoad(const char* filename, int *bytesRead);

#ifdef __cplusplus
}
#endif


#endif
