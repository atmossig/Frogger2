/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: graphic.h
	Programmer	: Andy Eder
	Date		: 06/08/99 12:38:46

----------------------------------------------------------------------------------------------- */

#ifndef GRAPHIC_H_INCLUDED
#define GRAPHIC_H_INCLUDED


#define MAX_SLICES 10


typedef struct
{
	
	int status;
	unsigned int start[MAX_SLICES];
	unsigned int end[MAX_SLICES];
	unsigned int total[MAX_SLICES];
	unsigned int frameStart;
	char name[31];
	char slice;

} TIMECOUNT;

extern TIMECOUNT timers[10];
extern unsigned int frameStart;
extern char timerMode;

void TIMER_InitTimers();
void TIMER_StartTimer(int number,char *name);
void TIMER_EndTimer(int number);
void TIMER_PrintTimers();
void TIMER_ClearTimers();


#endif