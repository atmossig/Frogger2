/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: actor.h
	Programmer	: Matthew Cloy
	Date		: 11/11/98

----------------------------------------------------------------------------------------------- */

#ifndef ACTOR_H_INCLUDED
#define ACTOR_H_INCLUDED


//----- [ ACTOR2 DRAWING FLAGS ] -----//

#define	ACTOR_DRAW_NEVER			(1 << 0)
#define ACTOR_DRAW_CULLED			(1 << 1)
#define ACTOR_DRAW_ALWAYS			(1 << 2)


//------------------------------------------------------------------------------------------------

typedef struct _ACTION
{	
//	QUATERNION rot;
	
	//Babies Stuff
	unsigned long isSaved;

	unsigned long safe;
	unsigned long lives;
	unsigned long dead;
	unsigned long deathBy;
	
	unsigned long isCroaking;
	unsigned long isTeleporting;
	unsigned long isOnFire;

} ACTION;


typedef struct _ACTOR2
{
	ACTOR			*actor;
	ACTION			action;	
	
	struct _ACTOR2	*next,*prev;

	char			draw;
	int				flags;
	float			radius;

	float			distanceFromFrog;		// <--- will need changing !!!

	float			speed;
	float			offset;

	SPRITE			sprite;

} ACTOR2;


extern float ACTOR_DRAWDISTANCEINNER;
extern float ACTOR_DRAWDISTANCEOUTER;

extern int objectMatrix;

extern ACTOR2 *actList;

extern ACTOR2 *CreateAndAddActor(char *name,float cx,float cy,float cz,int initFlags,float offset,int startNode);
extern void DrawActorList();
extern void FreeActorList();

extern void AddObjectsSpritesToSpriteList(OBJECT *obj,short flags);
extern void RemoveObjectSprites(OBJECT *obj,BOOL free);
extern void FreeObjectSprites(OBJECT *obj);


#endif