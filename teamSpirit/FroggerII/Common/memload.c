
#include <ultra64.h>
#include "incs.h"
#include <stdio.h>
#include "memload.h"

/*	-------------------------------------------------------------------------------- */

enum MEMLOAD_EVENTS
{
	EV_DEBUG = 0,
	MEMLOAD_NUMEVENTS
};

enum MEMLOAD_TRIGGERS
{
	TR_ENEMYONTILE = 0,
	TR_FROGONTILE,
//	TR_FROGONPLATFORM,
//	TR_WITHINRADIUS,
//	TR_TIMEOUT,
	MEMLOAD_NUMTRIGGERS
};

#define MEMLOAD_ENTITY_VERSION 3
#define MEMLOAD_EVENT_VERSION 1

int MemLoadTrigger(UBYTE** p, long size);

/*	-------------------------------------------------------------------------------- */

#define MEMGETBYTE(p) (*((*p)++))

inline int MEMGETINT(uchar **p)		// get a little-endian integer
{
	unsigned int i;

	i = (unsigned int)*((*p)++);
	i += ((unsigned int)*((*p)++) << 8);
	i += ((unsigned int)*((*p)++) << 16);
	i += ((unsigned int)*((*p)++) << 24);

	return i;
}

#define MEMGETFLOAT(p) ((float)MEMGETINT(p) / (float)0x10000)

inline char *MemLoadString(uchar **p)
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
	uchar thing;
	char type[20];
	PATH *path;
	PATHNODE *node;
	VECTOR v;
	ENEMY *enemy;
	PLATFORM *platform;
	uchar *p = (uchar*)data;

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
		n = MEMGETBYTE(&p);
		memcpy(type, p, n); type[n] = 0; p+= n;
		flags = MEMGETINT(&p);
		ID = MEMGETINT(&p);
		thing = MEMGETBYTE(&p);
		
		numNodes = MEMGETINT(&p);
		startNode = MEMGETINT(&p);

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
		case 0:
			enemy = CreateAndAddEnemy(type);
			AssignPathToEnemy(enemy, flags, path, 0);
			break;

		case 1:
			platform = CreateAndAddPlatform(type);
			AssignPathToPlatform(platform, flags, path, 0);
			break;
		}
	 }

	count = MEMGETINT(&p);

	while (count--)
	{
		n = MEMGETBYTE(&p);
		v.v[X] = MEMGETFLOAT(&p);
		v.v[Y] = MEMGETFLOAT(&p);
		v.v[Z] = MEMGETFLOAT(&p);
		CreateNewGarib(v, n);
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
	UCHAR *p = (UCHAR*)data;

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
		dprintf"Adding trigger, %d bytes\n", s));
		if (!MemLoadTrigger(&p, s)) return 0;
		size -= (s + 4);
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


int MemLoadTrigger(UBYTE** p, long size)
{
	UBYTE type, psize;
	TRIGGER* trigger;
	EVENT* event;
	int events;

	void **params;

	type = MEMGETBYTE(p); dprintf"Type: %d\n", type));

	if (type > MEMLOAD_NUMTRIGGERS)
	{
		dprintf"Unrecognised trigger type %02x, skipping\n", type));
		(*p) += size; return 0;
	}

	psize = MEMGETINT(p);

	switch (type)
	{
	case TR_ENEMYONTILE:
/*		params = AllocArgs(2);
		params[0] = (void*)GetEnemyFromID(MEMGETINT(p));
		params[1] = (void*)&firstTile[MEMGETINT(p)];
		trigger = MakeTrigger(EnemyOnTile, 2, params);
*/		
		MEMGETINT(p);
		MEMGETINT(p);
		trigger = NULL;
		break;

	case TR_FROGONTILE:
		params = AllocArgs(2);
		params[0] = (void*)JallocAlloc(sizeof(int), NO, "int");	*(int*)params[0] = MEMGETINT(p);
		params[1] = (void*)GetTileFromNumber(MEMGETINT(p));
		trigger = MakeTrigger(FrogOnTile, 2, params);
		break;
	}

	if (!trigger) return 0;

	for (events = MEMGETBYTE(p); events; events--)
	{
		type = MEMGETBYTE(p);
		psize = MEMGETINT(p);

		if (type > MEMLOAD_NUMEVENTS)
		{
			dprintf"Unrecognised event type %02x, skipping\n", type));
			(*p) += psize;
			continue;
		}

		switch (type)
		{
		case EV_DEBUG:
			params = AllocArgs(1);
			params[0] = MemLoadString(p);
			event =	MakeEvent(DebugEvent, 1, params);
			break;
		}

		AttachEvent(trigger, event, TRIGGER_ALWAYS, 0);
	}

	return 1;
}