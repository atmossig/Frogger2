/************************************************************************************
	ACTION MAN PS	(c) 1998-9 ISL

	timer.h:		Debug timing functions

************************************************************************************/

#ifndef _TIMER_H_
#define _TIMER_H_

enum {
	TIMER_TOTAL,
	TIMER_TIMERS,
	TIMER_LAND_UPDATE,
	TIMER_LAND_DRAW,
	TIMER_OBJECT_UPDATE,
	TIMER_OBJECT_DRAW,
	TIMER_MAN_UPDATE,
	TIMER_MAN_DRAW,
	TIMER_CAMERA,
	TIMER_DRAWSYNC,
	TIMER_COLLISION,
	TIMER_PARTICLES,
	TIMER_PROJECTILES,
	TIMER_OVERLAYS,
	TIMER_CUSTOM1,
	TIMER_CUSTOM2,

	TIMER_NUMTIMERS
};


#define TIMER_NAMES	   	"TOTAL", \
					   	"TIMERS", \
					   	"LAND UPDATE", \
						"LAND DRAW", \
					   	"OBJ UPDATE", \
					   	"OBJ DRAW", \
					   	"MAN UPDATE", \
					   	"MAN DRAW", \
					   	"CAMERA", \
					   	"DRAWSYNC", \
					   	"COLLISION", \
					   	"PARTICLES", \
					   	"PROJECTILES", \
					   	"OVERLAYS", \
						"CUSTOM1", \
						"CUSTOM2"


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
