/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: tongue.h
	Programmer	: Andrew Eder
	Date		: 12/15/98

----------------------------------------------------------------------------------------------- */

#ifndef TONGUE_H_INCLUDED
#define TONGUE_H_INCLUDED


#define TONGUE_RADIUSNORMAL			125.0
#define TONGUE_RADIUSLONG			225.0

//extern float TONGUE_RADIUSNORMAL;
//extern float TONGUE_RADIUSLONG;

#define TONGUE_STICKYRADIUS			10.0
#define MAX_TONGUENODES				8

#define TONGUE_OFFSET_UP			-15.0
#define TONGUE_OFFSET_FORWARD		5.0

#define TONGUE_WRAPAROUNDTHRESHOLD	-0.2

enum
{
	TONGUE_GET_BABY,
	TONGUE_GET_GARIB,
	TONGUE_GET_GRAPPLE,
};


//----- [ FROG TONGUE FLAGS ] ----------------------------------------------

#define TONGUE_NONE				0
#define TONGUE_IDLE				(1 << 0)
#define TONGUE_BEINGUSED		(1 << 1)
#define TONGUE_OUTGOING			(1 << 2)
#define TONGUE_INCOMING			(1 << 3)
#define TONGUE_HASITEMONIT		(1 << 4)
#define TONGUE_HASITEMINMOUTH	(1 << 5)
#define TONGUE_THROWING			(1 << 6)
#define TONGUE_SEARCHING		(1 << 7)
#define TONGUE_GRAPPLE          (1 << 8)

//----- [ GLOBALS ] --------------------------------------------------------


extern float			tongueRadius;

extern unsigned long	tongueState;

extern VECTOR			tonguePos;
extern Vtx				*tongueVtx;
extern Vtx				*tongueVtx2;
extern char				tongueToCollect;

extern int				tongueCoordIndex;

extern VECTOR			fu;
extern VECTOR			fr;

extern int				tongueSegment;
extern VECTOR			tongueCoords[MAX_TONGUENODES];

extern SPRITE			tongueSprite[MAX_TONGUENODES];


//----- [ FUNCTION PROTOTYPES ] --------------------------------------------

extern void InitTongue(char toGet);
extern void UpdateFrogTongue();
extern void CreateTongueSegment(char idx);
extern void RemoveTongueSegment(char idx);
extern void RemoveFrogTongue();


extern void AddTongueSprite(short index,float x,float y,float z);

#endif
