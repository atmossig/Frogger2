
#include <ultra64.h>
#include "incs.h"
#include <stdio.h>
#include "memload.h"
#include "eventfuncs.h"
#include "script.h"

/*	-------------------------------------------------------------------------------- */

#define MEMLOAD_ENTITY_VERSION 11
#define MEMLOAD_SCRIPT_VERSION 2

typedef enum { CREATE_ENEMY, CREATE_PLATFORM, CREATE_GARIB, CREATE_CAMERACASE } CREATETYPE;

/*	-------------------------------------------------------------------------------- */

#define MEMGETBYTE(p) (*((*p)++))

int MemLoadInt(UBYTE **p)		// get a little-endian integer
{
	unsigned int i;

	i = (unsigned int)*((*p)++);
	i += ((unsigned int)*((*p)++) << 8);
	i += ((unsigned int)*((*p)++) << 16);
	i += ((unsigned int)*((*p)++) << 24);

	return i;
}

short MemLoadWord(UBYTE **p)		// get a little-endian word
{
	unsigned short i;

	i = (unsigned int)*((*p)++);
	i += ((unsigned int)*((*p)++) << 8);

	return (short)i;
}

char *MemLoadString(UBYTE **p)
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

#ifndef PC_VERSION
			stringChange(type);
#endif

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
				if( flags & ENEMY_NEW_SWARM )
					enemy = CreateAndAddEnemy(type,0/*INIT_SWARM*/);
				else
					enemy = CreateAndAddEnemy(type,0);
				enemy->uid = ID;
				AssignPathToEnemy(enemy, flags, path, 0);
				act = enemy->nmeActor;
//				if( enemy->flags & ENEMY_NEW_SWARM )
//					CreateAndAddFXSwarm( SWARM_TYPE_CROWS, &act->actor->pos, 32, 65535, 0 );

				break;

			case CREATE_PLATFORM:
				platform = CreateAndAddPlatform(type);
				platform->uid = ID;
				AssignPathToPlatform(platform, flags, path, 0);
				act = platform->pltActor;

#ifdef PLATFORM_NEW_SHAKABLESCENIC
				// check for platforms that are 'shakable' scenics - TESTING - ANDYE
				if(	gstrcmp(type,"appltree.obe") == 0 || gstrcmp(type,"appltree.ndo") == 0 ||
					gstrcmp(type,"barrel.obe") == 0 || gstrcmp(type,"barrel.ndo") == 0 ||
					gstrcmp(type,"pltbarel.obe") == 0 || gstrcmp(type,"pltbarel.ndo") == 0 ||
					gstrcmp(type,"bucket.obe") == 0 || gstrcmp(type,"bucket.ndo") == 0)
				{
					// make this 'platform' shakable
					SetVector(&platform->pltActor->actor->oldpos,&platform->pltActor->actor->pos);
					platform->flags |= PLATFORM_NEW_SHAKABLESCENIC;
				}
#endif
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
//			CreateNewGarib(v, n, &firstTile[1], 0.0f);
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
				CreateAndAddTransCamera(FindNearestTile(w), flags >> 16, &w, flags & 0xFFFF);
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
	UBYTE *p = (UBYTE*)data;

	if (*p != MEMLOAD_SCRIPT_VERSION) {
		dprintf"ERROR: Attempting to load incorrect version (%03d) of script (should be %03d)\n",
			ver, MEMLOAD_SCRIPT_VERSION));
		return 0;
	}

	return (Interpret(p + 1));
}
