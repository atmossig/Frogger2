/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: edcamera.c
	Programmer	: David Swift
	Date		: 05/08/99

---------------------------------------------------------------------------- */

#include "ultra64.h"
#include "incs.h"
#include "editdefs.h"
#include "edglobals.h"
#include "edcamera.h"

#define FLOAT2INT(x) ((int)(0x10000 * (x)))

CREATEENTITY *EditorAddCameraCase(EDITGROUP *selection, unsigned long flags, VECTOR *v)
{
	CREATEENTITY *entity;
	EDITGROUPNODE *node;
	VECTOR avg;
	int count;
	
	entity = (CREATEENTITY*)JallocAlloc(sizeof(CREATEENTITY), YES, "e_ent_cam");

	entity->thing = CREATE_CAMERACASE;
	entity->flags = flags;

	//entity->path = MakePathFromTileGroup(selection);
	entity->group = selection; selection->refs++;
	entity->path = NULL;
	
	entity->scale = 45.0;	// FOV
	entity->animSpeed = 1.0; // Camera speed

	SetVector(&entity->camera, v);

	// stick icon at the "middle" (mean average) position
	ZeroVector(&avg);
	for (node = selection->nodes, count = 0; node; node = node->link, count++)
	{
		AddToVector(&avg, &((GAMETILE*)node->thing)->centre);
	}

	if( count )
	{
		avg.v[X] /= (float)count;
		avg.v[Y] /= (float)count;
		avg.v[Z] /= (float)count;
	}
	else 
	{
		SetVector( &avg, &frog[0]->actor->pos );
		dprintf"MAPPING ERROR - Zero tile camera case!\n"));
	}

	AddGroupMember(entity, &avg, createList);

	// Old selectionList (selection) is now part of the camera case.
	// Make a new selectionList and add the camera case to it.
	selectionList = MakeEditGroup( );
	selection->refs--;
	AddGroupMember( entity, &avg, selectionList );

	return entity;
}
