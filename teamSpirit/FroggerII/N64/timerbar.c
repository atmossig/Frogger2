/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: timerbar.c
	Programmer	: Andrew Eder
	Date		: 11/23/98

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>

#include "incs.h"


char message[20];

TIMECOUNT		timers[10];
TIMECOUNT		printTimer[10];
unsigned int	frameStart;
unsigned int	printFrameStart[10];
char			timerMode = TIMERMODE_OFF;//TEXT; // TIMERMODE_BARS / TIMERMODE_OFF
unsigned int	renderTime,renderTimeMax;
unsigned int	totalTimeUsed;
char			showTime = 150;

/*	--------------------------------------------------------------------------------
	Function 	: ToggleTimerMode
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ToggleTimerMode(void)
{
	if(timerMode < TIMERMODE_INFO)
		timerMode++;
	else
		timerMode = TIMERMODE_OFF;
}

/*	--------------------------------------------------------------------------------
	Function 	: InitTimers
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void InitTimers(void)
{
	int i,j;

	for (i=0;i<10;i++)
	{
		timers[i].status = 0;

		for (j=0;j<MAX_SLICES;j++)
		{
			timers[i].start[j] = 0;
			timers[i].end[j] = 0;
			timers[i].total[j] = 0;
		}

		timers[i].name[0] = NULL;
		timers[i].slice = 0;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: ClearTimers
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ClearTimers(void)
{
	int		i,j;
	char	overrun=0;
	frameStart = osGetCount();

	for (i=0;i<10;i++)
	{
		if (timers[i].status==0) printTimer[i] = timers[i];
		else overrun=1;
	}
	if(overrun==1)
		return;

	for (i=0;i<10;i++)
	{
		// printTimer[i] = timers[i];
		timers[i].slice=0;
		timers[i].name[0] = NULL;
		timers[i].frameStart = frameStart;

		for (j=0;j<MAX_SLICES;j++)
		{
			timers[i].start[j] = 0;
			timers[i].end[j] = 0;
			timers[i].total[j] = 0;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: StartTimer
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void StartTimer(int number,char *name)
{
	char slice;

	slice = timers[number].slice;

	if (slice>(MAX_SLICES-1)) return;

	timers[number].start[slice] = osGetCount();
	timers[number].status = 1;
//	timers[number].frameStart = frameStart;
	sprintf(timers[number].name,name);
}

/*	--------------------------------------------------------------------------------
	Function 	: EndTimer
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void EndTimer(int number)
{
	unsigned int	maxCount = -1;
	char			slice;

	slice = timers[number].slice;

	if(slice>(MAX_SLICES-1))
		return;

	timers[number].end[slice] = osGetCount();
	if(timers[number].end[slice] > timers[number].start[slice])
	{
		timers[number].total[slice] += timers[number].end[slice] - timers[number].start[slice];
	}
	else
	{
		timers[number].total[slice] += maxCount-timers[number].start[slice]+timers[number].end[slice];
	}
	timers[number].status = 0;

	if(slice<MAX_SLICES)
		timers[number].slice++;
}

/*	--------------------------------------------------------------------------------
	Function 	: PrintTimers
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PrintTimers(void)
{
/*	int i,j;
	unsigned int ulx,uly,lrx,lry,size,start;
	unsigned int maxCount = -1;

	if(!timerMode)
		return;

	gDPPipeSync(glistp++);
	gDPSetRenderMode(glistp++,G_RM_OPA_SURF , G_RM_OPA_SURF2);      
	gDPPipeSync(glistp++);
	gDPSetCycleType(glistp++, G_CYC_FILL);

	gDPSetFillColor(glistp++, GPACK_RGBA5551(255,255,255,1) << 16 | 
		GPACK_RGBA5551(255,255,255,1));

		
	gDPFillRectangle(glistp++,100,30,101,195);
	gDPFillRectangle(glistp++,100+((TicksPerFrame)/10000),30,101+((TicksPerFrame)/10000),195);
	gDPFillRectangle(glistp++,100+((TicksPerFrame)/20000),30,101+((TicksPerFrame)/20000),195);
	gDPFillRectangle(glistp++,100+((TicksPerFrame)/1),30,101+((TicksPerFrame)/1),195);

	ulx = 100;
	uly = 30;
	lry = 35;
	for (i=0;i<10;i++)
	{
		if (timerMode==2)
		{
//			sprintf(message,"  %d",printTimer[i].total[0]);
//			sprintf(message,"%s %d",printTimer[i].name, printTimer[i].total[0]);
//			NEW_TEXT(20,30+(i*16),255,255,255,255,1,1,1,message, NO, SMALL_FONT, 0);
		}
		for (j=0;j<printTimer[i].slice;j++)
		{
			if (printTimer[i].start[j]!=0)
			{
				gDPPipeSync(glistp++);
				switch(j&3)
				{
				case 0:
					gDPSetFillColor(glistp++, GPACK_RGBA5551(240,0,0,1) << 16 | 
						GPACK_RGBA5551(240,0,0,1));
					break;
				case 1:
					gDPSetFillColor(glistp++, GPACK_RGBA5551(0,240,0,1) << 16 | 
						GPACK_RGBA5551(0,240,0,1));
					break;
				case 2:
					gDPSetFillColor(glistp++, GPACK_RGBA5551(0,0,240,1) << 16 | 
						GPACK_RGBA5551(0,0,240,1));
					break;
				case 3:
					gDPSetFillColor(glistp++, GPACK_RGBA5551(240,240,10,1) << 16 | 
						GPACK_RGBA5551(240,240,10,1));
					break;
				}
				

				size = printTimer[i].total[j]/20000;
				if (printTimer[i].start[j] > printTimer[i].frameStart)
				{
					start = (printTimer[i].start[j]-printTimer[i].frameStart)/20000;
				}
				else
				{
					start = (maxCount-printTimer[i].frameStart+printTimer[i].start[j])/20000;
				}
				lrx = 100+size;
				if(timerMode)
					gDPFillRectangle(glistp++,ulx+start,uly,lrx+start,lry);
			}
		}
		uly += 16;
		lry += 16;
	}


	j = 0;

//	sprintf(message,"  %d : %f",(int)framesPerSec,GAME_SPEED);
//	NEW_TEXT(160,200,255,255,255,255,1,1,1,message, NO, SMALL_FONT, 0);

	gDPPipeSync(glistp++);
	gDPSetCycleType(glistp++, G_CYC_1CYCLE);      
	gDPPipeSync(glistp++);
	gDPSetRenderMode(glistp++,G_RM_AA_ZB_OPA_SURF , G_RM_AA_ZB_OPA_SURF2);
*/

	int				i,j;
	unsigned int	ulx,uly,lrx,lry,size,start,temp;
	TEXT			*text;
	unsigned int	maxCount = -1;
	
	renderTime = 0;	

	// Update render time information for editor
	for(j=0;j<printTimer[9].slice;j++)
	{
		temp = (printTimer[9].total[j] + printTimer[9].start[j] - printTimer[9].frameStart)/20000;
		if (temp > renderTime) renderTime = temp;
	}
	renderTimeMax = TicksPerFrame/20000;

	if((timerMode==TIMERMODE_OFF)||(timerMode==TIMERMODE_INFO))
		return;
	
	gDPPipeSync(glistp++);
	gDPSetRenderMode(glistp++,G_RM_OPA_SURF , G_RM_OPA_SURF2);      
	gDPPipeSync(glistp++);
	gDPSetCycleType(glistp++, G_CYC_FILL);

	gDPSetFillColor(glistp++, GPACK_RGBA5551(255,255,255,1) << 16 | 
		GPACK_RGBA5551(255,255,255,1));
	
	gDPFillRectangle(glistp++,STARTX-150,STARTY,STARTX-150+((TicksPerFrame*3)/20000),STARTY+1);
	gDPFillRectangle(glistp++,STARTX-150,STARTY+80,STARTX-150+((TicksPerFrame*3)/20000),STARTY+80+1);
	gDPFillRectangle(glistp++,STARTX-150,STARTY,STARTX-150+1,STARTY+80);
	gDPFillRectangle(glistp++,STARTX-150+((TicksPerFrame)/20000),STARTY,STARTX-150+1+((TicksPerFrame)/20000),STARTY+80);
	gDPFillRectangle(glistp++,STARTX-150+((TicksPerFrame*2)/20000),STARTY,STARTX-150+1+((TicksPerFrame*2)/20000),STARTY+80);
	gDPFillRectangle(glistp++,STARTX-150+((TicksPerFrame*3)/20000),STARTY,STARTX-150+1+((TicksPerFrame*3)/20000),STARTY+80);

	gDPFillRectangle(glistp++,STARTX,STARTY,STARTX+((TicksPerFrame*3)/40000)+1,STARTY+1);
	gDPFillRectangle(glistp++,STARTX,STARTY+80,STARTX+((TicksPerFrame*3)/40000)+1,STARTY+80+1);
	gDPFillRectangle(glistp++,STARTX,STARTY,STARTX+1,STARTY+80);
	gDPFillRectangle(glistp++,STARTX+((TicksPerFrame)/40000),STARTY,STARTX+1+((TicksPerFrame)/40000),STARTY+80);
	gDPFillRectangle(glistp++,STARTX+((TicksPerFrame*2)/40000),STARTY,STARTX+1+((TicksPerFrame*2)/40000),STARTY+80);
	gDPFillRectangle(glistp++,STARTX+((TicksPerFrame*3)/40000),STARTY,STARTX+1+((TicksPerFrame*3)/40000),STARTY+80);

	ulx = STARTX;
	uly = STARTY+2;
	lry = STARTY+5+2;
	for (i=0;i<10;i++)
	{
		if (timerMode==TIMERMODE_BARSTEXT)
		{
			if (printTimer[i].name!=NULL)
			{
				sprintf(message,"%s",printTimer[i].name);
//				NEW_TEXT(STARTX-200,STARTY+(i<<3),255,255,255,255,0.6,0.6,1,message,NO);
				//AddText(-20,-50,message,0);
			}
		}
		totalTimeUsed = 0;
		for(j=0;j<printTimer[i].slice;j++)
		{
			if (printTimer[i].start[j]!=0)
			{
				gDPPipeSync(glistp++);
				switch(j&3)
				{
				case 0:
					gDPSetFillColor(glistp++, GPACK_RGBA5551(240,0,0,1) << 16 | 
						GPACK_RGBA5551(240,0,0,1));
					break;
				case 1:
					gDPSetFillColor(glistp++, GPACK_RGBA5551(0,240,0,1) << 16 | 
						GPACK_RGBA5551(0,240,0,1));
					break;
				case 2:
					gDPSetFillColor(glistp++, GPACK_RGBA5551(0,0,240,1) << 16 | 
						GPACK_RGBA5551(0,0,240,1));
					break;
				case 3:
					gDPSetFillColor(glistp++, GPACK_RGBA5551(240,240,10,1) << 16 | 
						GPACK_RGBA5551(10,10,10,1));
					break;
				}
				
//				gDPSetFillColor(glistp++, GPACK_RGBA5551(255-(j<<5),255-(j<<5),255-(j<<5),0) << 16 | 
//					GPACK_RGBA5551(255-(j<<5),255-(j<<5),255-(j<<5),0));

//				gDPSetFillColor(glistp++, GPACK_RGBA5551(0,0,0,1) << 16 | 
//					GPACK_RGBA5551(0,0,0,1));

				size = printTimer[i].total[j]/40000;
				if (printTimer[i].start[j] > printTimer[i].frameStart)
				{
					start = (printTimer[i].start[j]-printTimer[i].frameStart)/40000;
				}
				else
				{
					start = (maxCount-printTimer[i].frameStart+printTimer[i].start[j])/40000;
				}
				lrx = STARTX+size;
				totalTimeUsed += printTimer[i].total[j];
				if (start<320)
					gDPFillRectangle(glistp++,ulx+start,uly,lrx+start,lry);
//				gDPFillRectangle(glistp++,lrx+start,uly-5,lrx+start,lry+5);
			}
			gDPPipeSync(glistp++);
			gDPSetFillColor(glistp++, GPACK_RGBA5551(240,0,0,1) << 16 | 
				GPACK_RGBA5551(240,0,0,1));
			
			gDPFillRectangle(glistp++,STARTX-150,uly,STARTX-150+(totalTimeUsed/20000),lry);
			if ((showTime==i)||(showTime==100))
			{
				sprintf(message, "%d",totalTimeUsed);
//				NEW_TEXT(30,26+(i*16),255,255,255,255,1,1,1,message, NO);
			}
		}
		uly += 8;
		lry += 8;
	}

	gDPPipeSync(glistp++);
	gDPSetCycleType(glistp++, G_CYC_1CYCLE);      
	gDPPipeSync(glistp++);
}
