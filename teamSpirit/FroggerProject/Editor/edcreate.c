/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: edcreate.c
	Programmer	: David Swift
	Date		: 07/03/00

---------------------------------------------------------------------------- */


#include <stdio.h>

#include "edittypes.h"
#include "edglobals.h"
#include "editdefs.h"
#include "edmaths.h"

#include "particle.h"
#include "sprite.h"
#include "specfx.h"
#include "collect.h"
#include "enemies.h"
#include "platform.h"
#include "cam.h"
#include "babyfrog.h"
#include "event.h"
#include "pcmisc.h"


/*	--------------------------------------------------------------------------------
	Function		: EditorCreateEntities
	Parameters		: 
	Returns			: 
*/

void EditorCreateEntities(void)
{
	EDITGROUPNODE *node, *cam;
	CREATEENTITY *create;
	PATH *path;
	PLATFORM *platform;
	ENEMY *enemy;
	EDVECTOR v;
	EDITPATHNODE *pn;
	ACTOR2 *act;
	int counts[4];
	int i;

	for (i=0; i<4; i++) counts[i] = 0;

	FreeSpecFXList( );
	InitSpecFXList( );
	ResetBabies();
	FreePlatformLinkedList();
	FreeEnemyLinkedList();
	FreeGaribList();
	InitGaribList();
	FreeTransCameraList();
	FreePathList();
	KillAllTriggers( );
	FreeSpriteList( );
	InitSpriteList( );
	InitSpriteSortArray( );
	FreeParticleList( );
	InitParticleList( );

	for (i=0; i<4; i++)
		if (currPlatform[i])
		{
			currPlatform[i]->carrying = NULL;
			currPlatform[i] = NULL;
//			BounceFrog(i, 20, 40);
		}

	for (node = createList->nodes; node; node = node->link)
	{
		create = (CREATEENTITY*)node->thing;

//		if (create->thing == CREATE_ENEMY || create->thing == CREATE_PLATFORM)
//		{
//			OBJECT_CONTROLLER *foo;

//			FindObject(&foo, UpdateCRC(create->type), create->type, YES);
//			if(!foo)
//				strcpy(create->type, "nothing.obe");
//		}

		switch (create->thing)
		{
		case CREATE_ENEMY:
			path = EditorPathMake(create->path,create->startNode);
			path->startNode = create->startNode;

			enemy = CreateAndAddEnemy(create->type,create->flags,create->ID,path, (int)(create->animSpeed*256), create->facing);
			if (!enemy) continue;

			act = enemy->nmeActor;

			act->radius = GAMEFLOAT(create->radius)*10;
			act->value1 = GAMEFLOAT(create->value1);

// todo: object flags?
//			act->actor->objectController->object->flags = create->objFlags;

			if( !(enemy->flags & ENEMY_NEW_BABYFROG) )
			{
				act->effects = create->effects;
				act->actor->size.vx = GAMEFLOAT(create->scale);
				act->actor->size.vy = GAMEFLOAT(create->scale);
				act->actor->size.vz = GAMEFLOAT(create->scale);
			}
			else
			{
				act->actor->size.vx = create->scale*BABY_SCALE;
				act->actor->size.vy = create->scale*BABY_SCALE;
				act->actor->size.vz = create->scale*BABY_SCALE;
			}

			if(gstrcmp(create->type,"nothing.obe") == 0)
				act->draw = 0;

			counts[0]++;
			break;

		case CREATE_PLACEHOLDER:
			path = EditorPathMake(create->path,create->startNode);
			path->startNode = create->startNode;

			enemy = (ENEMY*)calloc(1,sizeof(ENEMY)); //JallocAlloc(sizeof(ENEMY), YES, "place");
			enemy->path = path;
			enemy->active = 0;
			enemy->uid = create->ID;
			act = NULL;
			AddEnemy(enemy);

			counts[0]++;
			break;

		case CREATE_PLATFORM:
			path = EditorPathMake(create->path,create->startNode);
			path->startNode = create->startNode;

			platform = CreateAndAddPlatform(create->type,create->flags,create->ID,path,(int)(create->animSpeed*256),create->facing);
			if (!platform) continue;

			act = platform->pltActor;

			act->actor->size.vx = GAMEFLOAT(create->scale);
			act->actor->size.vy = GAMEFLOAT(create->scale);
			act->actor->size.vz = GAMEFLOAT(create->scale);

			act->radius = GAMEFLOAT(create->radius) * 10;
			act->value1 = GAMEFLOAT(create->value1);
			act->effects = create->effects;
			act->animSpeed = GAMEFLOAT(create->animSpeed);

// todo: object flags?
//			act->actor->objectController->object->flags = create->objFlags;

			if(gstrcmp(create->type,"nothing.obe") == 0)
				act->draw = 0;

			counts[1]++;
			break;

		case CREATE_GARIB:
			{
				SVECTOR pos;
				pn = create->path->nodes;
				SetVectorF(&v, &pn->tile->normal);
				ScaleVector(&v, pn->offset*SCALE);
				AddToVector(&v, &pn->tile->centre);
		
				SetVectorSR(&pos, &v);
				CreateNewGarib(pos, create->flags);
				counts[2]++;
			}
			break;

		case CREATE_CAMERACASE:
			for (cam = create->group->nodes; cam; cam = cam->link)
			{
				EDVECTOR v;
				SVECTOR camv;
				TRANSCAMERA *c;

				v = create->camera;
				ScaleVector(&v, 10);
				SSetVector(&camv, &v);
				c = CreateAndAddTransCamera((GAMETILE*)cam->thing, create->flags >> 16, &camv, create->flags & 0xFFFF);

				c->FOV = GAMEFLOAT(create->scale);
				c->speed = GAMEFLOAT(create->animSpeed);

				c->camLookAt.vx = GAMEFLOAT((float)((char)(create->startNode & 0xFF))/0x7F);
				c->camLookAt.vy = GAMEFLOAT((float)((char)((create->startNode>>8) & 0xFF))/0x7F);		// oof! :o)
				c->camLookAt.vz = GAMEFLOAT((float)((char)((create->startNode>>16) & 0xFF))/0x7F);
			}

			counts[3]++;
			break;
		}
	}

	sprintf(statusMessage, "Created %d enemies, %d plats, %d garibs, %d cam trans",
		counts[0], counts[1], counts[2], counts[3]);
}
