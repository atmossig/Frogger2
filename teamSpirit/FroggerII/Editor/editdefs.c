/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: editdefs.c
	Programmer	: David Swift
	Date		: 08/07/99

---------------------------------------------------------------------------- */

#include <ultra64.h>
#include <stdio.h>
#include "incs.h"

#include "edglobals.h"
#include "editdefs.h"
#include "edcommand.h"

/*	------------------------------------------------------------------------
	Function		: EditorAddFlag

	Parameters		: GAMETILE*, EDITPATH*
	Returns			: EDITPATHNODE*
*/

EDITPATHNODE *EditorAddFlag(GAMETILE* tile, EDITPATH* path)
{
	EDITPATHNODE *node, *newPath;
	int i = 0;

	newPath = (EDITPATHNODE*)JallocAlloc(sizeof(EDITPATHNODE), YES, "epathnode");

	if (path->nodes)
	{
		for (node = path->nodes; node; node = node->link)
		{
			if (tile == node->tile) i++;
			if (!node->link) { node->link = newPath; break; }
		}
		newPath->speed = node->speed;
		newPath->offset = node->offset;
		newPath->offset2 = node->offset2;
	}
	else
	{
		newPath->speed = 1.0;
		newPath->offset = 0;
		newPath->offset2 = 0;
		path->nodes = newPath;
	}

	newPath->waitTime = 0;
	newPath->sample = 0;
	newPath->start = editPath ? FALSE : TRUE;
	newPath->tile = tile;
	newPath->link = NULL;
	newPath->level = i;

	return newPath;
}

/*	-------------------------------------------------------------------------
	Function		: EditorSubFlag

	Parameters		: GAMETIME*, EDITPATH*
	Returns			: 
*/

void EditorSubFlag(EDITPATHNODE *node, EDITPATH *path)
{
	EDITPATHNODE *next, *p;

	if (node == path->nodes)
	{
		path->nodes = node->link;
		JallocFree((BYTE**)&node);
	}
	else
	for (p = path->nodes; p; p = next)
	{
		next = p->link;
		if (next == node)
		{
			p->link = next->link;
			JallocFree((BYTE**)&next);
			break;
		}
	}
}


/*	-------------------------------------------------------------------------
	Function		: EditorAddFlag

	Parameters		: GAMETIME*, EDITPATH*
	Returns			:

	Follows link pointers from flags until it gets as close as possible to the target.
*/

void EditorAddFlags(GAMETILE *end, EDITPATH *path)
{
	EDITPATHNODE *node;
	GAMETILE *tile, *next;
	VECTOR v;
	int i, count;
	float dot;

	if (!path->nodes)
	{
		EditorAddFlag(end, path);
		return;
	}
	
	for (node = path->nodes; node->link; node = node->link);

	tile = node->tile; count = 50;

	while (tile != end)
	{
		next = NULL;

		SubVector(&v, &tile->centre, &end->centre);
		MakeUnit(&v);

		for (i = 0; i < 4; i++)
		{
			dot = DotProduct(&v, &tile->dirVector[i]);
			if (dot > 0.6)
			{
				next = tile->tilePtrs[i];
				break;
			}
		}
		if (!next)
			break;
		else
			EditorAddFlag(next, path);

		tile = next;

		if (!--count) return;
	}

	if (tile != end)
		EditorAddFlag(end, path);
}

/*	--------------------------------------------------------------------------------
	Function		: EditorAddCreateNode

	Parameters		: const char*, int, PATH*, CREATETYPE
	Returns			: CREATEENTITYNODE*
*/

CREATEENTITY *EditorAddCreateNode(const char *id, int flags, int ID, CREATETYPE type,
									  EDITPATH *path, int startNode)
{
	CREATEENTITY *newNode;
	EDITPATHNODE *node;
	int count;
	VECTOR iconPos;
	EDITGROUPNODE *p;

	for (count = startNode, node = path->nodes; count; count--)
	{
		if (node->link)	node = node->link;
	}

	newNode = (CREATEENTITY*)JallocAlloc(sizeof(CREATEENTITY), YES, "eentity");

	// If the paste button is pressed
	if( copyFlag && copyEntity && copyEntity->thing == type )
	{
		strncpy(newNode->type, copyEntity->type, MAXTYPELENGTH); newNode->type[MAXTYPELENGTH-1] = 0;
		newNode->flags = copyEntity->flags;
		newNode->thing = copyEntity->thing;
		newNode->scale = copyEntity->scale;
		newNode->radius = copyEntity->radius;
		newNode->animSpeed = copyEntity->animSpeed;
		newNode->value1 = copyEntity->value1;
		newNode->objFlags = copyEntity->objFlags;
		newNode->facing = copyEntity->facing;
	}
	else
	{
		strncpy(newNode->type, id, MAXTYPELENGTH); newNode->type[MAXTYPELENGTH-1] = 0;
		newNode->flags = flags;
		newNode->thing = type;
		newNode->scale = 1.0;
		newNode->radius = 50.0;
		newNode->animSpeed = 0.25;
		newNode->value1 = 0;
		newNode->objFlags = 0;
		newNode->facing = 0;
	}

	if( type == CREATE_CAMERACASE )
	{
		newNode->scale = 45.0;
		newNode->animSpeed = 1.0;
	}
	
	newNode->effects = EF_TINTRED | EF_TINTGREEN | EF_TINTBLUE | EF_MEDIUM;
	newNode->startNode = startNode;
	newNode->ID = ID;
	newNode->path = path; path->refs++;
	
	iconPos = node->tile->centre;
	iconPos.v[Z] += 10;

	for (p = createList->nodes; p; p = p->link)
		if( DistanceBetweenPointsSquared(&iconPos, &p->pos) < 20 )
			iconPos.v[Z] += 20;

	AddGroupMember(newNode, &iconPos, createList);
	
	return newNode;
}

void EditorSubCreateNode(CREATEENTITY *node)
{
	SubGroupMember(node, createList);
	FreeEditPath(node->path);
	JallocFree((BYTE**)&node);
}

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
	VECTOR v;
	EDITPATHNODE *pn;
	ACTOR2 *act;
	int counts[4];
	int i;

	for (i=0; i<4; i++) counts[i] = 0;

	FreeSpecFXList( );
	InitSpecFXList( );
	ResetBabies();
	FreePlatformList();
	FreeEnemyList();
	FreeGaribList();
	InitGaribList();
	FreeTransCameraList();
	FreePathList();
	KillAllTriggers( );
	FreeSpriteList( );
	InitSpriteList( );

	for (i=0; i<4; i++)
		if (currPlatform[i])
		{
			currPlatform[i]->carrying = NULL;
			currPlatform[i] = NULL;
			BounceFrog(i, 20, 40);
		}

	for (node = createList->nodes; node; node = node->link)
	{
		create = (CREATEENTITY*)node->thing;

		if (create->thing == CREATE_ENEMY || create->thing == CREATE_PLATFORM)
		{
			OBJECT_CONTROLLER *foo;

			FindObject(&foo, UpdateCRC(create->type), create->type, YES);
			if(!foo)
				strcpy(create->type, "nothing.obe");
		}

		switch (create->thing)
		{
		case CREATE_ENEMY:
			path = EditorPathMake(create->path);
			path->startNode = create->startNode;

			enemy = CreateAndAddEnemy(create->type,create->flags,create->ID,path, create->animSpeed, create->facing);
			act = enemy->nmeActor;
			act->actor->scale.v[X] = create->scale;
			act->actor->scale.v[Y] = create->scale;
			act->actor->scale.v[Z] = create->scale;
			act->radius = create->radius;
			act->value1 = create->value1;
			// PUT THIS IN LATER
//			act->actor->objectController->object->flags = create->objFlags;

			if( !(enemy->flags & ENEMY_NEW_BABYFROG) )
				act->effects = create->effects;

			counts[0]++;
			break;

		case CREATE_PLACEHOLDER:
			path = EditorPathMake(create->path);
			path->startNode = create->startNode;

			enemy = JallocAlloc(sizeof(ENEMY), YES, "place");
			enemy->path = path;
			enemy->active = 0;
			enemy->uid = create->ID;
			act = NULL;
			AddEnemy(enemy);

			counts[0]++;
			break;

		case CREATE_PLATFORM:
			path = EditorPathMake(create->path);
			path->startNode = create->startNode;

			platform = CreateAndAddPlatform(create->type,create->flags,create->ID,path,create->animSpeed,create->facing);

			act = platform->pltActor;
			act->actor->scale.v[X] = create->scale;
			act->actor->scale.v[Y] = create->scale;
			act->actor->scale.v[Z] = create->scale;
			act->radius = create->radius;
			act->value1 = create->value1;
			act->effects = create->effects;
			act->animSpeed = create->animSpeed;
			// PUT THIS IN LATER
//			act->actor->objectController->object->flags = create->objFlags;

			counts[1]++;
			break;

		case CREATE_GARIB:
			pn = create->path->nodes;
			SetVector(&v, &pn->tile->normal);
			ScaleVector(&v, pn->offset);
			AddToVector(&v, &pn->tile->centre);
	
			CreateNewGarib(v, create->flags);
			counts[2]++;
			break;

		case CREATE_CAMERACASE:
			for (cam = create->group->nodes; cam; cam = cam->link)
			{
				TRANSCAMERA *c = CreateAndAddTransCamera((GAMETILE*)cam->thing,
					create->flags >> 16,	// direction
					&create->camera,
					create->flags & 0xFFFF);

				c->FOV = create->scale;
				c->speed = create->animSpeed;
				c->camLookAt.v[X] = (float)((char)(create->startNode & 0xFF))/0x7F;
				c->camLookAt.v[Y] = (float)((char)((create->startNode>>8) & 0xFF))/0x7F;
				c->camLookAt.v[Z] = (float)((char)((create->startNode>>16) & 0xFF))/0x7F;
			}

			counts[3]++;
			break;
		}
	}

	sprintf(statusMessage, "Created %d enemies, %d plats, %d garibs, %d cam trans",
		counts[0], counts[1], counts[2], counts[3]);
}

/*	--------------------------------------------------------------------------------
	Function		: EditorPathMake


	Parameters		: 
	Returns			: 
*/

PATH *EditorPathMake(const EDITPATH *editPath)
{
	int count, i;
	EDITPATHNODE *path;
	PATHNODE *node;

	PATH *newPath = (PATH *)JallocAlloc(sizeof(PATH), YES, "epath");

	/* count the number of nodes first */
	for (path = editPath->nodes, count = 0; path; path = path->link, count++);

	if (!count) return NULL;

	newPath->numNodes = count;

	// allocate memory for path nodes
	newPath->nodes = (PATHNODE *)JallocAlloc(sizeof(PATHNODE) * newPath->numNodes,YES,"epathnode");

	i = 0;
	node = newPath->nodes;

	for (path = editPath->nodes; path; path = path->link, node++, i++)
	{
		node->worldTile = path->tile;
		node->speed		= path->speed;
		node->offset	= path->offset;
		node->offset2	= path->offset2;
		node->waitTime  = path->waitTime;
		node->sample	= FindSample(path->sample);

		if (path->start) newPath->startNode = i;
	}

	AddPathNodesToList(newPath->nodes);

	return newPath;
}

/*	--------------------------------------------------------------------------------
	Function		: EditorGetPath
	Parameters		: 
	Returns			: 
*/

EDITPATH *EditorGetPath(const PATH *path)
{
	int i;
	EDITPATHNODE *newPath, *prev, *start;
	EDITPATH *ep;

	prev = NULL; start = NULL;

	for (i = 0; i<path->numNodes; i++)
	{
		newPath = (EDITPATHNODE*)JallocAlloc(sizeof(EDITPATHNODE), YES, "epath");

		if (prev)
			prev->link = newPath;
		else
			start = newPath;

		newPath->tile = path->nodes[i].worldTile;
		newPath->level = 0;
		newPath->speed = path->nodes[i].speed;
		newPath->offset = path->nodes[i].offset;
		newPath->offset2 = path->nodes[i].offset2;
		newPath->start = (i == path->startNode) ? TRUE : FALSE;
		newPath->waitTime = path->nodes[i].waitTime;
		newPath->sample = (path->nodes[i].sample)?(path->nodes[i].sample->uid):0;
		newPath->link = NULL;

		prev = newPath;
	}

	ep = CreateEditPath();
	ep->nodes = start;

	return ep;
}


EDITPATH *CreateEditPath()
{
	EDITPATH *path;
	path = (EDITPATH *)calloc(1, sizeof(EDITPATH));
	path->refs = 1;
/*	
	path->next = pathList;
	if (pathList)
		pathList->prev = path;
	pathList = path;
*/
	return path;
}


void FreeEditPath(EDITPATH *editPath)
{
	EDITPATHNODE *del, *path;
	
	if (editPath)
	{
		editPath->refs--;

		if (editPath->refs < 0)
			dprintf"ERROR! FreeEditPath: refs < 0\n"));
		
		if (editPath->refs <= 0)
		{
			path = editPath->nodes;
			while (path)
			{
				del = path; path = path->link;
				JallocFree((BYTE**)&del);
			}
/*
			if (editPath->next)
				editPath->next->prev = editPath->prev;
			if (editPath->prev)
				editPath->prev->next = editPath->next;
			else
				pathList = editPath->next;
*/
			JallocFree((BYTE**)&editPath);
		}
	}
}

void FreeCreateList()
{
	while (createList->nodes) EditorSubCreateNode(createList->nodes->thing);
}

/*	------------------------------------------------------------------------------ */

EDITGROUP *MakeEditGroup(void)
{
	EDITGROUP *group = (EDITGROUP*)JallocAlloc(sizeof(EDITGROUP), YES, "egroup");
	group->refs++;
	return group;
}

void FreeEditGroup(EDITGROUP **group)
{
	EDITGROUPNODE *node, *next;

	if (!group || !(*group)) return;

	--(*group)->refs;

	if ((*group)->refs == 0)
	{
		for (node = (*group)->nodes; node; node = next)
		{
			next = node->link;
			JallocFree((BYTE**)&node);
		}

		JallocFree((BYTE**)group);
	}
	*group = NULL;
}

/*	-------------------------------------------------------------------------
	Function		: MakePathEditGroup

	Parameters		: EDITPATH*
	Returns			: EDITGROUP*

	Makes an EDITGROUP containing all the nodes from an editpath
*/
EDITGROUP *MakePathEditGroup(EDITPATH *path)
{
	EDITPATHNODE *pn;
	EDITGROUP *group;

	path->refs++;

	group = MakeEditGroup();

	for (pn = path->nodes; pn; pn = pn->link)
		AddGroupMember(pn, &pn->tile->centre, group);

	return group;
}

/*	-------------------------------------------------------------------------
	Function		: MakePathEditGroup

	Parameters		: EDITGROUP*
	Returns			: EDITPATH*

	Makes an EDITPATH from a bunch of tiles stored in an EDITGROUP
*/
EDITPATH *MakePathFromTileGroup(EDITGROUP *group)
{
	EDITGROUPNODE *node;
	EDITPATH *path;

	path = CreateEditPath();

	for (node = group->nodes; node; node = node->link)
		EditorAddFlag((GAMETILE*)node->thing, path);

	return path;
}

/*	-------------------------------------------------------------------------
	Function		: MakePathEditGroup

	Parameters		: EDITGROUP*
	Returns			: EDITPATH*

	Makes an EDITGROUP from the tiles in an EDITPATH
*/
extern EDITGROUP *MakeTileGroupFromPath(EDITPATH *path)
{
	EDITPATHNODE *node;
	EDITGROUP *group;

	group = MakeEditGroup();

	for (node = path->nodes; node; node = node->link)
		AddGroupMember((void*)node->tile, &node->tile->centre, group);

	return group;
}

void AddGroupMember(void *thing, VECTOR *pos, EDITGROUP *group)
{
	EDITGROUPNODE *s;

	s = (EDITGROUPNODE*)JallocAlloc(sizeof(EDITGROUPNODE), NO, "egrpm");
	s->thing = thing;
	s->pos = *pos;
	s->link = group->nodes;
	group->nodes = s;
}

BOOL AddUniqueGroupMember(void *thing, VECTOR *pos, EDITGROUP *group)
{
	EDITGROUPNODE *n;

	for (n = group->nodes; n; n = n->link)
		if (n->thing == thing) return FALSE;

	AddGroupMember(thing, pos, group);
	return TRUE;
}

int SubGroupMember(void *thing, EDITGROUP *group)
{
	EDITGROUPNODE *s, *prev;

	for (s = group->nodes, prev = NULL; s; prev = s, s = s->link)
	{
		if (s->thing == thing)
		{
			if (prev)
				prev->link = s->link;
			else
				group->nodes = s->link;
			JallocFree((UBYTE**)&s);
			return -1;
		}
	}
	return 0;
}

int ToggleGroupMember(void* thing, VECTOR* pos, EDITGROUP *group)
{
	EDITGROUPNODE *s, *prev;

	prev = NULL;

	for (s = group->nodes;; prev = s, s = s->link)
	{
		if (!s) {
			s = (EDITGROUPNODE*)JallocAlloc(sizeof(EDITGROUPNODE), NO, "egrpm");
			s->thing = thing;
			s->pos = *pos;
			s->link = group->nodes;
			group->nodes = s;
			return 1;
		}
		else if (s->thing == thing)
		{
			if (prev)
				prev->link = s->link;
			else
				group->nodes = s->link;
			JallocFree((UBYTE**)&s);
			return -1;
		}
	}
}

int ToggleRectSelection(int ax, int ay, int bx, int by, const EDITGROUP *list, EDITGROUP *selection)
{
	EDITGROUPNODE *node;
	VECTOR v;
	int fuzz = 0;

	// flip rect the right way round
	if (ax > bx) { int foo = ax; ax = bx; bx = foo; }
	if (ay > by) { int foo = ay; ay = by; by = foo; }

	for (node = list->nodes; node; node = node->link)
	{
		EdXfmPoint(&v, &node->pos);
		if (!v.v[Z]) continue;
		
		if (v.v[X] > ax && v.v[X] < bx && v.v[Y] > ay && v.v[Y] < by)
			fuzz += ToggleGroupMember(node->thing, &node->pos, selection);
	}

	return fuzz;
}

int ToggleRectTileSelection(int ax, int ay, int bx, int by, EDITGROUP *selection)
{
	GAMETILE *tile;
	VECTOR v;
	int fuzz = 0;

	// flip rect the right way round
	if (ax > bx) { int foo = ax; ax = bx; bx = foo; }
	if (ay > by) { int foo = ay; ay = by; by = foo; }

	for (tile = firstTile; tile; tile = tile->next)
	{
		EdXfmPoint(&v, &tile->centre);
		if (!v.v[Z]) continue;
		
		if (v.v[X] > ax && v.v[X] < bx && v.v[Y] > ay && v.v[Y] < by)
			fuzz += ToggleGroupMember(tile, &tile->centre, selection);
	}

	return fuzz;
}

int ToggleRectFlagSelection(int ax, int ay, int bx, int by, EDITGROUP *selection, EDITPATH *path)
{
	EDITPATHNODE *pnode;
	VECTOR v;
	int fuzz = 0;

	if( !selection || !path )
		return 0;

	// flip rect the right way round
	if (ax > bx) { int foo = ax; ax = bx; bx = foo; }
	if (ay > by) { int foo = ay; ay = by; by = foo; }

	for (pnode = path->nodes; pnode; pnode = pnode->link)
	{
		EdXfmPoint(&v, &pnode->tile->centre);
		if (!v.v[Z]) continue;
		
		if (v.v[X] > ax && v.v[X] < bx && v.v[Y] > ay && v.v[Y] < by)
			fuzz += ToggleGroupMember(pnode, &pnode->tile->centre, selection);
	}

	return fuzz;
}

/*	------------------------------------------------------------------------------ */


int GetSelectionTileState(EDITGROUP *group)
{
	EDITGROUPNODE *node;
	int state;

	if (!group->nodes) return -1;

	node = group->nodes;

	state = ((GAMETILE*)node->thing)->state;

	for (node = node->link; node; node = node->link)
	{
		if (((GAMETILE*)node->thing)->state != state) return -1;
	}

	return state;
}

int NextTileSelectionState(EDITGROUP *group)
{
	EDITGROUPNODE *node;
	int state;

	state = GetSelectionTileState(group);
	if (state == -1)
		state = TILESTATE_NORMAL;
	else
		if ((++state) > (NUMTILESTATES-1)) state = 0;
	
	for (node = group->nodes; node; node = node->link)
		((GAMETILE*)node->thing)->state = state;

	return state;
}


int PrevTileSelectionState(EDITGROUP *group)
{
	EDITGROUPNODE *node;
	int state;

	state = GetSelectionTileState(group);
	if (state == -1)
		state = TILESTATE_NORMAL;
	else
		if ((--state) < 0) state = NUMTILESTATES-1;
	
	for (node = group->nodes; node; node = node->link)
		((GAMETILE*)node->thing)->state = state;

	return state;
}

int CountGroupMembers(EDITGROUP *group)
{
	EDITGROUPNODE *node;
	int count;

	for (node = group->nodes, count = 0; node; node = node->link, count++);
	return count;
}

/*	--------------------------------------------------------------------------------
	Function		: FreeEnemyList
	Parameters		: 
	Returns			: 
*/

void FreeEnemyList(void)
{
	ENEMY *e;
	int i;

	for (i = enemyList.numEntries, e = enemyList.head.next; i; i--, e = e->next)
	{
		SubActor(e->nmeActor);
		e->nmeActor = NULL;
	}
	FreeEnemyLinkedList();
}

/*	--------------------------------------------------------------------------------
	Function		: FreePlatformList
	Parameters		: 
	Returns			: 
*/

void FreePlatformList(void)
{
	PLATFORM *p;
	int i;

	for (i = platformList.numEntries, p = platformList.head.next; i; i--, p = p->next)
	{
		SubActor(p->pltActor);
		p->pltActor = NULL;
	}
	FreePlatformLinkedList();
}

/*	--------------------------------------------------------------------------------
	Function		: EdXfmPoint
	Parameters		: 
	Returns			: 
*/

void EdXfmPoint (VECTOR *res, VECTOR *in)
{
	XfmPoint(res, in);
	
	if (res->v[0] < 0 || res->v[0] > SCREEN_WIDTH ||
		res->v[1] < 0 || res->v[1] > SCREEN_HEIGHT)	// clip result
		res->v[2] = 0;
}
