/************************************************************************************
	ACTION MAN PS	(c) 1998-9 ISL

	timer.h:		Debug timing functions

************************************************************************************/

#ifndef _TIMER_H_
#define _TIMER_H_

enum {
	TIMER_TOTAL,
	TIMER_TIMERS,
	TIMER_DRAWSYNC,
	TIMER_LAND_DRAW,
	TIMER_ACTOR_DRAW,
	TIMER_UPDATE,
	TIMER_OVERLAYS,

	TIMER_NUMTIMERS
};


#define TIMER_NAMES	   	"TOTAL", \
					   	"TIMERS", \
					   	"DRAW SYNC", \
						"LAND DRAW", \
					   	"ACTOR UPDATE", \
					   	"UPDATE", \
					   	"OVERLAYS"


extern char	timerActive;
extern char	*timerName[TIMER_NUMTIMERS];
extern int	globalTimer[TIMER_NUMTIMERS];
extern int	prevTimer[TIMER_NUMTIMERS];


/**************************************************************************
	FUNCTION:	TIMER_START()
	PURPOSE:	Start timing
	PARAMETERS:	Timer number
	RETURNS:	
**************************************************************************/

#define TIMER_START(n)		globalTimer[n] = VSync(1)


/**************************************************************************
	FUNCTION:	TIMER_STOP()
	PURPOSE:	Stop timing
	PARAMETERS:	Timer number
	RETURNS:	
**************************************************************************/

#define TIMER_STOP(n)		globalTimer[n] = VSync(1)-globalTimer[n]


/**************************************************************************
	FUNCTION:	TIMER_ENDFRAME()
	PURPOSE:	Start timing
	PARAMETERS:	Timer number
	RETURNS:	
**************************************************************************/

#define TIMER_ENDFRAME		memcpy(prevTimer, globalTimer, sizeof(globalTimer))


/**************************************************************************
	FUNCTION:	timerDisplay()
	PURPOSE:	Display timer readings if enabled
	PARAMETERS:	
	RETURNS:	
**************************************************************************/

void timerDisplay();


#endif
