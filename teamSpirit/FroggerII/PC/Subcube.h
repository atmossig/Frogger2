/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: Subcube.h
	Programmer	: Andy Eder
	Date		: 15/04/99 10:40:33

----------------------------------------------------------------------------------------------- */

#ifndef SUBCUBE_H_INCLUDED
#define SUBCUBE_H_INCLUDED


extern void InitActor(ACTOR *tempActor, char *name, float x, float y, float z, int initFlags);
extern void InitActorStructures(ACTOR *tempActor, int initFlags);
extern void InitActorList();
extern void LoadTexture(TEXTURE *texture);

extern void WriteObjectDisplayListGouraud(OBJECT *obj);
extern void WriteObjectDisplayListFlat(OBJECT *obj);
extern void TransformAndDrawObject(OBJECT *obj, float time, short animStart, short animEnd);
extern void DrawActor(ACTOR *ptr);

extern TEXTURE	*tempTexture;
extern char		*tempObjectPtr, *tempObjectPtr2;
extern VECTOR	*actorScale, parentScale;
extern Vtx		*vtxPtr;
extern short	xluFact;
extern char		changeRenderMode;
extern long		DIST;

extern float nearClip;
extern float farClip;

extern float horizClip;
extern float vertClip;

#endif