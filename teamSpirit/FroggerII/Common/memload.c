
#include <ultra64.h>
#include "incs.h"
#include <stdio.h>
#include "memload.h"
#include "eventfuncs.h"
#include "codes.h"

/*	-------------------------------------------------------------------------------- */

#define MEMLOAD_ENTITY_VERSION 11
#define MEMLOAD_EVENT_VERSION 1

int MemLoadTrigger(UBYTE** p, long size);

typedef enum { CREATE_ENEMY, CREATE_PLATFORM, CREATE_GARIB, CREATE_CAMERACASE } CREATETYPE;

/*	-------------------------------------------------------------------------------- */

#define MEMGETBYTE(p) (*((*p)++))

inline int MEMGETINT(UBYTE **p)		// get a little-endian integer
{
	unsigned int i;

	i = (unsigned int)*((*p)++);
	i += ((unsigned int)*((*p)++) << 8);
	i += ((unsigned int)*((*p)++) << 16);
	i += ((unsigned int)*((*p)++) << 24);

	return i;
}

#define MEMGETFLOAT(p) ((float)MEMGETINT(p) / (float)0x10000)

inline char *MemLoadString(UBYTE **p)
{
	char *ptr;
	int size;
	
	size = MEMGETBYTE(p);
	ptr = JallocAlloc(size + 1, NO, "str");
	memcpy(ptr, *p, size);
	ptr[size] = 0;

	(*p) += size;

	return ptr;
}

/*	--------------------------------------------------------------------------------
    Function		: MemLoadEntities
	Parameters		: void*, long
	Returns			: int

	Load enemies and plats from a data file in memory
*/

int MemLoadEntities(const void* data, long size)
{
	int count, flags, numNodes, startNode, n, ID;
	float scale, radius, animSpeed, value1;
	UBYTE thing;
	char type[20];
	PATH *path;
	PATHNODE *node;
	VECTOR v,w;
	ENEMY *enemy;
	PLATFORM *platform;
	UBYTE *p = (UBYTE*)data;
	ACTOR2 *act;

	// Version check - only load files with the current version
	n = MEMGETBYTE(&p);
	
	if (n != MEMLOAD_ENTITY_VERSION ) {
		dprintf"ERROR: Attempting to load incorrect version (%03d) of level file (should be %03d)\n", n, MEMLOAD_ENTITY_VERSION));
		return 0;
	}

	count = MEMGETINT(&p);
	dprintf"MemLoadEntities: %d items\n", count));

	while (count--)
	{
		thing = MEMGETBYTE(&p);

		switch (thing)
		{
		case CREATE_ENEMY:
		case CREATE_PLATFORM:
			n = MEMGETBYTE(&p);
			memcpy(type, p, n); type[n] = 0; p+= n;
			flags	= MEMGETINT(&p);
			ID		= MEMGETINT(&p);
			scale	= MEMGETFLOAT(&p);
			radius	= MEMGETFLOAT(&p);
			animSpeed = MEMGETFLOAT(&p);
			value1 = MEMGETFLOAT(&p);
			startNode = MEMGETINT(&p);
		
			numNodes = MEMGETINT(&p);

			dprintf"'%s' %08x with %d path nodes\n", type, flags, numNodes));

			path = (PATH *)JallocAlloc(sizeof(PATH), YES, "epath");
			node = (PATHNODE *)JallocAlloc(sizeof(PATHNODE) * numNodes,YES,"enode");
			path->nodes = node;
			path->startNode = startNode;
			path->numNodes = numNodes;

			while (numNodes--)
			{
				v.v[X] = MEMGETFLOAT(&p);
				v.v[Y] = MEMGETFLOAT(&p);
				v.v[Z] = MEMGETFLOAT(&p);
				node->worldTile = FindNearestTile(v);
				node->offset = MEMGETFLOAT(&p);
				node->offset2 = MEMGETFLOAT(&p);
				node->speed = MEMGETFLOAT(&p);
				node->waitTime = MEMGETINT(&p);
				
				node++;
			}

			switch (thing)
			{
			case CREATE_ENEMY:
				enemy = CreateAndAddEnemy(type);
				enemy->uid = ID;
				AssignPathToEnemy(enemy, flags, path, 0);
				act = enemy->nmeActor;
				if( enemy->flags & ENEMY_NEW_SWARM )
					CreateAndAddFXSwarm( SWARM_TYPE_CROWS, &act->actor->pos, 32, 65535, 0 );

				break;

			case CREATE_PLATFORM:
				platform = CreateAndAddPlatform(type);
				platform->uid = ID;
				AssignPathToPlatform(platform, flags, path, 0);
				act = platform->pltActor;
				break;
			}

			act->radius = radius;
			act->actor->scale.v[X] = scale;
			act->actor->scale.v[Y] = scale;
			act->actor->scale.v[Z] = scale;
			act->animSpeed = animSpeed;
			act->value1 = value1;

			break;

		case CREATE_GARIB:
			n = MEMGETBYTE(&p);
			v.v[X] = MEMGETFLOAT(&p);
			v.v[Y] = MEMGETFLOAT(&p);
			v.v[Z] = MEMGETFLOAT(&p);
			CreateNewGarib(v, n);
			break;

		case CREATE_CAMERACASE:
			flags = MEMGETINT(&p);
			v.v[X] = MEMGETFLOAT(&p);
			v.v[Y] = MEMGETFLOAT(&p);
			v.v[Z] = MEMGETFLOAT(&p);
			
			// A vector that isn't used yet
			MEMGETFLOAT(&p);
			MEMGETFLOAT(&p);
			MEMGETFLOAT(&p);

			numNodes = MEMGETINT(&p);
			while (numNodes--)
			{
				w.v[X] = MEMGETFLOAT(&p);
				w.v[Y] = MEMGETFLOAT(&p);
				w.v[Z] = MEMGETFLOAT(&p);
				CreateAndAddTransCamera(FindNearestTile(w), 0,
					w.v[X], w.v[Y], w.v[Z], flags);
			}
			break;
		}
	 }

	return 1;
}


/*	--------------------------------------------------------------------------------
    Function		: MemLoadEvents
	Parameters		: void*, long
	Returns			: int

	Load events and triggers from an ETC output file
*/

int MemLoadEvents(const void* data, long size)
{
	int ver, s;
	UBYTE *pp, *p = (UBYTE*)data;

	ver = MEMGETBYTE(&p);

	if (ver != MEMLOAD_EVENT_VERSION) {
		dprintf"ERROR: Attempting to load incorrect version (%03d) of event file (should be %03d)\n",
			ver, MEMLOAD_EVENT_VERSION));
		return 0;
	}

	size -= 1;

	while (size)
	{
		s = MEMGETINT(&p);
		pp = p;
		dprintf"Adding trigger, %d bytes (%08x)\n", s, pp));
		if (!MemLoadTrigger(&pp, s))
		{
			dprintf"Failed to create trigger\n"));
		} // return 0;
		p += s; size -= (s + 4);
	}

	return 1;
}

int DebugTrigger(TRIGGER *trigger)
{
	return 1;
}

void DebugEvent(EVENT *event)
{
	char *str = (char*)event->data[0];
	sprintf(statusMessage, "Debug event: %s", str);
}

GAMETILE *GetTileFromNumber(int number)
{
	GAMETILE *tile;
	for (tile = firstTile; number; number--, tile = tile->next);
	return tile;
}

ACTOR2 *GetUniqueActor2(int UID)
{
	ENEMY *e;
	PLATFORM *p;

	e = GetEnemyFromUID(UID);
	if (e)
		return e->nmeActor;
	
	p = GetPlatformFromUID(UID);
	if (p)
		return p->pltActor;

	return NULL;
}

EVENT* MemLoadEvent(UBYTE** p, int type)
{
	EVENT* event = NULL;
	ACTOR2 *actor;
	VECTOR *v;
	float f;
	void **params;

	switch (type)
	{
	case EV_DEBUG: // string
		params = AllocArgs(1);
		params[0] = MemLoadString(p);
		event =	MakeEvent(DebugEvent, 1, params);
		break;

	case EV_CHANGEACTORSCALE: // ID, float
		params = AllocArgs(2);
		if (!(actor = GetUniqueActor2(MEMGETINT(p)))) return 0;
		params[0] = (void*)actor->actor;
		v = (VECTOR*)JallocAlloc(sizeof(VECTOR), NO, "vect");
		v->v[X] = v->v[Y] = v->v[Z] = MEMGETFLOAT(p);
		params[1] = (void*)v;
		event =	MakeEvent(ChangeActorScale, 2, params);
		break;

	case EV_TOGGLEPLATFORMMOVE:
		params = AllocArgs(1);
		params[0] = (void*)GetPlatformFromUID(MEMGETINT(p));
		event = MakeEvent(TogglePlatformMove, 1, params);
		break;

	case EV_TOGGLEENEMYMOVE:
		params = AllocArgs(1);
		params[0] = (void*)GetEnemyFromUID(MEMGETINT(p));
		event = MakeEvent(ToggleEnemyMove, 1, params);
		break;

	case EV_TOGGLETILELINK:
		params = AllocArgs(1);
		params[0] = (void*)GetTileFromNumber(MEMGETINT(p));
		event = MakeEvent(ToggleTileLink, 1, params);
		break;

	case EV_ANIMATEACTOR:
		params = AllocArgs(4);
		if (!(actor = GetUniqueActor2(MEMGETINT(p)))) return 0;
		params[0] = (void*)actor->actor;
		
		params[1] = JallocAlloc(sizeof(int), NO, "");
		params[2] = JallocAlloc(sizeof(int), NO, "");
		params[3] = JallocAlloc(sizeof(float), NO, "");

		*(int*)params[1] = MEMGETINT(p);
		*(int*)params[2] = MEMGETINT(p);
		*(float*)params[3] = MEMGETFLOAT(p);

		event = MakeEvent(EvAnimateActor, 4, params);
		break;
	}

	return event;
}

int MemLoadTrigger(UBYTE** p, long size)
{
	UBYTE type, psize;
	UBYTE* buf;
	TRIGGER* trigger;
	EVENT* event;
	int events;
	VECTOR *v;

	ENEMY *enemy;
	PLATFORM *platform;
	ACTOR2 *actor;

	void **params;

	type = MEMGETBYTE(p);
	psize = MEMGETINT(p);

	switch (type)
	{
	case TR_ENEMYONTILE:
		params = AllocArgs(2);
		if (!(enemy = GetEnemyFromUID(MEMGETINT(p)))) return 0;

		params[0] = (void*)enemy;
		params[1] = (void*)GetTileFromNumber(MEMGETINT(p));
		trigger = MakeTrigger(EnemyOnTile, 2, params);
		break;

	case TR_FROGONTILE:
		params = AllocArgs(2);
		params[0] = (void*)JallocAlloc(sizeof(int), NO, "int");	*(int*)params[0] = MEMGETINT(p);
		params[1] = (void*)GetTileFromNumber(MEMGETINT(p));
		trigger = MakeTrigger(FrogOnTile, 2, params);
		break;

	case TR_FROGONPLATFORM:
		params = AllocArgs(2);
		params[0] = (void*)frog[MEMGETINT(p)];
		if (!(platform = GetPlatformFromUID(MEMGETINT(p)))) return 0;
		params[1] = (void*)platform;
		trigger = MakeTrigger(FrogOnPlatform, 2, params);
		break;

	case TR_PLATNEARPOINT:
		params = AllocArgs(3);
		if (!(platform = GetPlatformFromUID(MEMGETINT(p)))) return 0;
		params[0] = platform->pltActor->actor;
		v = (VECTOR*)JallocAlloc(sizeof(VECTOR), NO, "vect");
		v->v[X] = MEMGETFLOAT(p);
		v->v[Y] = MEMGETFLOAT(p);
		v->v[Z] = MEMGETFLOAT(p);
		params[1] = (void*)v;
		params[2] = JallocAlloc(sizeof(float), NO, "float"); *(float*)params[1] = MEMGETFLOAT(p);
		trigger = MakeTrigger(ActorWithinRadius, 3, params);
		break;

	case TR_ENEMYNEARPOINT:
		params = AllocArgs(3);
		if (!(enemy = GetEnemyFromUID(MEMGETINT(p)))) return 0;
		params[0] = enemy->nmeActor->actor;
		v = (VECTOR*)JallocAlloc(sizeof(VECTOR), NO, "vect");
		v->v[X] = MEMGETFLOAT(p);
		v->v[Y] = MEMGETFLOAT(p);
		v->v[Z] = MEMGETFLOAT(p);
		params[1] = (void*)v;
		params[2] = JallocAlloc(sizeof(float), NO, "float"); *(float*)params[1] = MEMGETFLOAT(p);
		trigger = MakeTrigger(ActorWithinRadius, 3, params);
		break;

	case TR_ENEMYATFLAG:
		params = AllocArgs(2);
		if (!(enemy = GetEnemyFromUID(MEMGETINT(p)))) return 0;
		params[0] = enemy->path;
		params[1] = JallocAlloc(sizeof(int), NO, "int"); *(int*)params[1] = MEMGETINT(p);
		trigger = MakeTrigger(PathAtFlag, 2, params);
		break;

	case TR_PLATATFLAG:
		params = AllocArgs(2);
		if (!(enemy = GetEnemyFromUID(MEMGETINT(p)))) return 0;
		params[0] = enemy->path;
		params[1] = JallocAlloc(sizeof(int), NO, "int"); *(int*)params[1] = MEMGETINT(p);
		trigger = MakeTrigger(PathAtFlag, 2, params);
		break;

	default:
		dprintf"Unrecognised trigger type %02x, skipping\n", type));
		return 0;
	}

	if (!trigger) return 0;

	events = MEMGETBYTE(p);
	dprintf"Loading %d events\n", events));

	while (events--)
	{
		type = MEMGETBYTE(p);
		psize = MEMGETINT(p);
		buf = *p;

		dprintf"Adding event, %d bytes, type %d (%08x)\n", psize, type, buf));

		event = MemLoadEvent(&buf, type);

		if (!event)
		{
			dprintf"Failed loading event\n"));
		}

		(*p) += psize;
		AttachEvent(trigger, event, TRIGGER_RISING, 0);
	}

	return 1;
}