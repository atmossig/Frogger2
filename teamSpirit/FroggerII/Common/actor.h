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


typedef struct TAGACTION
{	
//	QUATERNION rot;
	
	//Babies Stuff
	unsigned long isSaved;

	unsigned long safe;
	unsigned long stun;

	unsigned long frogon;
	unsigned long frogunder;

	unsigned long lives;
	unsigned long dead;
	unsigned long deathBy;
	
	unsigned long isCroaking;
	unsigned long isTeleporting;
	unsigned long isOnFire;

} ACTION;


typedef struct TAGACTOR2
{
	ACTOR			*actor;
	ACTION			action;	
	
	struct TAGACTOR2	*next,*prev;

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

ACTOR2 *CreateAndAddActor(char *name,float cx,float cy,float cz,int initFlags,float offset,int startNode);
void DrawActorList();
void FreeActorList();

void AddObjectsSpritesToSpriteList(OBJECT *obj,short flags);
void RemoveObjectSprites(OBJECT *obj,BOOL free);
void FreeObjectSprites(OBJECT *obj);

BOOL ActorsHaveCollided(ACTOR2 *act1,ACTOR2 *act2);
void SetActorCollisionRadius(ACTOR2 *act,float radius);


#endif