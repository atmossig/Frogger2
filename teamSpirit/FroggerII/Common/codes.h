/*
	Trigger and event codes used by Frogger2 scripting system

	Programmer: David Swift
*/

#ifndef _INCLUDED_CODES_H
#define _INCLUDED_CODES_H

// Commands ("Events")

#define EV_DEBUG			0
#define EV_ANIMATEACTOR		6
#define EV_SETENEMY			7
#define EV_SETPLATFORM		8
#define EV_TELEPORT			9

#define EV_ON 64

// Triggers

#define TR_ENEMYONTILE		0
#define TR_FROGONTILE		1
#define TR_FROGONPLATFORM	2
#define TR_PLATNEARPOINT	3
#define TR_ENEMYNEARPOINT	4
#define TR_FROGNEARPOINT	5
#define TR_FROGNEARPLATFORM	6
#define TR_FROGNEARENEMY	7
#define TR_ENEMYATFLAG		8
#define TR_PLATATFLAG		9

#define TR_AND	64
#define TR_OR	65

#endif