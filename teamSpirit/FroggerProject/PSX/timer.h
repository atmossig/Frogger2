/************************************************************************************
	ACTION MAN PS	(c) 1998-9 ISL

	timer.h:		Debug timing functions

************************************************************************************/

#ifndef _TIMER_H_
#define _TIMER_H_

enum {
	TIMER_TOTAL,
	TIMER_GAMELOOP,
	TIMER_UPDATE_WATER,
	TIMER_DRAW_WORLD,
	TIMER_DRAW_SPECFX,
	TIMER_PRINT_SPRITES,
	TIMER_DRAW_SCENICS,
	TIMER_DRAW_WATER,
	TIMER_ACTOR_DRAW,
	TIMER_PRINT_OVERS,
	TIMER_PROCTEX,
	TIMER_DRAWSYNC,
	TIMER_TIMERS,
	TIMER_TRANSFORM,
	TIMER_GTFOR,
	TIMER_GTTHREE,

	TIMER_NUMTIMERS
};


#define TIMER_NAMES	   	"TOTAL", \
						"GAMELOOP", \
						"UPDATE_WATER", \
						"DRAW_WORLD", \
						"DRAW_SPECFX", \
						"PRINT_SPRITES", \
						"DRAW_SCENICS", \
						"DRAW_WATER", \
						"ACTOR_DRAW", \
						"PRINT_OVERS", \
						"PROCTEX", \
						"DRAWSYNC", \
						"TIMERS", \
						"TRANSFORM", \
						"GT4", \
						"GT3"








extern char	timerActive;
extern char	*timerName[TIMER_NUMTIMERS];
extern int	globalTimer[TIMER_NUMTIMERS];
extern int	startTimer[TIMER_NUMTIMERS];
extern int	prevTimer[TIMER_NUMTIMERS];


/**************************************************************************
	FUNCTION:	TIMER_START()
	PURPOSE:	Start timing
	PARAMETERS:	Timer number
	RETURNS:	
**************************************************************************/

//#define TIMER_START(n)		globalTimer[n] = VSync(1)
#define TIMER_START(n)		startTimer[n] = VSync(1)


/**************************************************************************
	FUNCTION:	TIMER_STOP()
	PURPOSE:	Stop timing
	PARAMETERS:	Timer number
	RETURNS:	
**************************************************************************/

//#define TIMER_STOP(n)		globalTimer[n] = VSync(1)-globalTimer[n]
#define TIMER_STOP(n)		globalTimer[n] = VSync(1)-startTimer[n]

#define TIMER_STOP_ADD(n)	globalTimer[n] += VSync(1)-startTimer[n]


/**************************************************************************
	FUNCTION:	TIMER_ENDFRAME()
	PURPOSE:	Start timing
	PARAMETERS:	Timer number
	RETURNS:	
**************************************************************************/

#define TIMER_ENDFRAME		memcpy(prevTimer, globalTimer, sizeof(globalTimer))

//need to zero, cos globalTimer may not be reset if only using TIMER_STOP_ADD
#define TIMER_ZERO			memset(globalTimer, 0, sizeof(globalTimer))


/**************************************************************************
	FUNCTION:	timerDisplay()
	PURPOSE:	Display timer readings if enabled
	PARAMETERS:	
	RETURNS:	
**************************************************************************/

void timerDisplay();


#endif
