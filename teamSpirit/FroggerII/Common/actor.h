/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: actor.h
	Programmer	: Matthew Cloy
	Date		: 11/11/98

----------------------------------------------------------------------------------------------- */

#ifndef ACTOR_H_INCLUDED
#define ACTOR_H_INCLUDED


//----- [ ACTOR2 DRAWING FLAGS ] -----//

#define	ACTOR_DRAW_NEVER			(1 << 0)	//1
#define ACTOR_DRAW_CULLED			(1 << 1)	//2
#define ACTOR_DRAW_ALWAYS			(1 << 2)	//4
#define ACTOR_DRAW_LAST				(1 << 3)	//8


#define LOOKAT_ANYWHERE		1
#define LOOKAT_2D			2


//------------------------------------------------------------------------------------------------


typedef struct TAGACTION
{	
//	QUATERNION rot;
	
	// baby stuff
	unsigned long isSaved;
	unsigned char fxColour[4];

	// non-baby stuff
	unsigned long safe;
	unsigned long stun;

	unsigned long frogon;
	unsigned long frogunder;

	unsigned long healthPoints;
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
	float			angle;
	float			animSpeed;
	float			value1;

	float			distanceFromFrog;		// <--- will need changing !!!

	float			speed;
	float			offset;

	SPRITE			sprite;

} ACTOR2;


extern float ACTOR_DRAWDISTANCEINNER;
extern float ACTOR_DRAWDISTANCEOUTER;

extern int objectMatrix;

extern ACTOR2 *actList;
extern ACTOR2 *globalLevelActor;

extern char uniqueEnemyCount[20];
extern int uniqueActorCRC[];
extern char numUniqueActors;

ACTOR2 *CreateAndAddActor(char *name,float cx,float cy,float cz,int initFlags,float offset,int startNode);
void DrawActorList();
void FreeActorList();

void AddObjectsSpritesToSpriteList(OBJECT *obj,short flags);
void RemoveObjectSprites(OBJECT *obj,BOOL free);
void FreeObjectSprites(OBJECT *obj);

BOOL ActorsHaveCollided(ACTOR2 *act1,ACTOR2 *act2);

void MakeUniqueActor(ACTOR *actor,int type);
void RemoveUniqueActor(ACTOR *actor,int type);

void MakeUniqueVtx(OBJECT_CONTROLLER *objC);
OBJECT *MakeUniqueObject(OBJECT *object);
void RemoveUniqueObject(OBJECT *object);

void ResetUniqueActorList();
void ActorLookAt( ACTOR *act, VECTOR *at, long flags );
void CopyDrawlist(u8 *dest,u8 *source);

#endif