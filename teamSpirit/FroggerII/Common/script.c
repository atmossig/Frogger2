

#include "ultra64.h"
#include "incs.h"
#include "codes.h"
#include "stdio.h"
#include "script.h"

/* --------------------------------------------------------------------------------- */

#define FS_SET_MOVE			1
#define FS_SET_STOP			2
#define FS_SET_TOGGLEMOVE	3
#define FS_SET_INVIS		4
#define FS_SET_VIS			5
#define FS_SET_TOGGLEVIS	6

/* --------------------------------------------------------------------------------- */

//int Interpret(const UBYTE *buffer);
TRIGGER *LoadTrigger(UBYTE **p);
BOOL ExecuteCommand(UBYTE *buffer);

UBYTE* scriptBuffer;

/* --------------------------------------------------------------------------------- */

#define MEMGETBYTE(p) (*((*p)++))
/*
inline int MEMGETINT(UBYTE **p)		// get a little-endian integer
{
	unsigned int i;

	i = (unsigned int)*((*p)++);
	i += ((unsigned int)*((*p)++) << 8);
	i += ((unsigned int)*((*p)++) << 16);
	i += ((unsigned int)*((*p)++) << 24);

	return i;
}
*/
#define MEMGETFLOAT(p) ((float)MEMGETINT(p) / (float)0x10000)
/*
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
*/
ACTOR2 *GetActorFromUID(int UID)
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


GAMETILE *GetTileFromNumber(int number)
{
	GAMETILE *tile;
	for (tile = firstTile; number; number--, tile = tile->next);
	return tile;
}


/*	--------------------------------------------------------------------------------
    Function		: InterpretEvent
	Parameters		: EVENT*
	Returns			: 

	Passed to the Event/Trigger code as a function pointer in an EVENT. Parameter 0
	contains a pointer to a block of code which is sent to interpret()
*/

void InterpretEvent( EVENT *e )
{
	UBYTE *buffer = (UBYTE *)e->data[0];
	Interpret(buffer);
}

int ANDtrigger(TRIGGER *t)
{
	TRIGGER *a = (TRIGGER*)t->data[0], *b = (TRIGGER*)t->data[1];
	return a->func(a) && b->func(b);
}

int ORtrigger(TRIGGER *t)
{
	TRIGGER *a = (TRIGGER*)t->data[0], *b = (TRIGGER*)t->data[1];
	return a->func(a) || b->func(b);
}

/*	--------------------------------------------------------------------------------
    Function		: LoadTrigger
	Parameters		: UBYTE*
	Returns			: TRIGGER*
*/
TRIGGER *LoadTrigger(UBYTE **p)
{
	TRIGGER *trigger = NULL;
	void **params;
	UBYTE token = MEMGETBYTE(p);

	switch (token)
	{
	case TR_ENEMYONTILE:
		{
			ENEMY *enemy;

			params = AllocArgs(2);
			if (!(enemy = GetEnemyFromUID(MEMGETINT(p)))) return 0;
			params[0] = (void*)enemy;
			params[1] = (void*)GetTileFromNumber(MEMGETINT(p));
			trigger = MakeTrigger(EnemyOnTile, params);
		}
		break;

	case TR_FROGONTILE:
		{
			params = AllocArgs(2);
			params[0] = (void*)MEMGETINT(p);
			params[1] = (void*)GetTileFromNumber(MEMGETINT(p));
			trigger = MakeTrigger(FrogOnTile, params);
		}
		break;

	case TR_FROGONPLATFORM:
		{
			PLATFORM *platform;

			params = AllocArgs(2);
			params[0] = (void*)frog[MEMGETINT(p)];
			if (!(platform = GetPlatformFromUID(MEMGETINT(p)))) return 0;
			params[1] = (void*)platform;
			trigger = MakeTrigger(FrogOnPlatform, params);
		}
		break;

	case TR_PLATNEARPOINT:
		{
			PLATFORM *platform; VECTOR *v;

			params = AllocArgs(3);
			if (!(platform = GetPlatformFromUID(MEMGETINT(p)))) return 0;
			params[0] = platform->pltActor->actor;
			v = (VECTOR*)JallocAlloc(sizeof(VECTOR), NO, "vect");
			v->v[X] = MEMGETFLOAT(p);
			v->v[Y] = MEMGETFLOAT(p);
			v->v[Z] = MEMGETFLOAT(p);
			params[1] = (void*)v;
			params[2] = JallocAlloc(sizeof(float), NO, "float"); *(float*)params[1] = MEMGETFLOAT(p);
			trigger = MakeTrigger(ActorWithinRadius, params);
		}
		break;

	case TR_ENEMYNEARPOINT:
		{
			ENEMY *enemy; VECTOR *v;

			params = AllocArgs(3);
			if (!(enemy = GetEnemyFromUID(MEMGETINT(p)))) return 0;
			params[0] = enemy->nmeActor->actor;
			v = (VECTOR*)JallocAlloc(sizeof(VECTOR), NO, "vect");
			v->v[X] = MEMGETFLOAT(p);
			v->v[Y] = MEMGETFLOAT(p);
			v->v[Z] = MEMGETFLOAT(p);
			params[1] = (void*)v;
			params[2] = JallocAlloc(sizeof(float), NO, "float"); *(float*)params[1] = MEMGETFLOAT(p);
			trigger = MakeTrigger(ActorWithinRadius, params);
		}
		break;

	case TR_ENEMYATFLAG:
		{
			ENEMY *enemy;

			params = AllocArgs(2);
			if (!(enemy = GetEnemyFromUID(MEMGETINT(p)))) return 0;
			params[0] = enemy->path;
			params[1] = JallocAlloc(sizeof(int), NO, "int"); *(int*)params[1] = MEMGETINT(p);
			trigger = MakeTrigger(PathAtFlag, params);
		}
		break;

	case TR_PLATATFLAG:
		{
			ENEMY *enemy;

			params = AllocArgs(2);
			if (!(enemy = GetEnemyFromUID(MEMGETINT(p)))) return 0;
			params[0] = enemy->path;
			params[1] = JallocAlloc(sizeof(int), NO, "int"); *(int*)params[1] = MEMGETINT(p);
			trigger = MakeTrigger(PathAtFlag, params);
		}
		break;

	case TR_OR:
		{
			TRIGGER *a, *b;

			a = LoadTrigger(p);
			if (!a) return 0;

			b = LoadTrigger(p);
			if (!b) return 0;

			params = AllocArgs(2);
			params[0] = (void*)a; params[1] = (void*)b;
			trigger = MakeTrigger(ORtrigger, params);
		}
		break;

	case TR_AND:
		{
			TRIGGER *a, *b;

			a = LoadTrigger(p);
			if (!a) return 0;

			b = LoadTrigger(p);
			if (!b) return 0;

			params = AllocArgs(2);
			params[0] = (void*)a; params[1] = (void*)b;
			trigger = MakeTrigger(ANDtrigger, params);
		}
		break;

	default:
		dprintf"Unrecognised trigger type %02x, skipping\n", token));
		return NULL;
	}

	return trigger;
}

/*	--------------------------------------------------------------------------------
    Function		: ExecuteCommand
	Parameters		: UBYTE*
	Returns			: TRUE if successful, FALSE otherwise
*/
BOOL ExecuteCommand(UBYTE *buffer)
{
	UBYTE **p;
	UBYTE command;
	
	p = &buffer;
	
	command = MEMGETBYTE(p);

	switch (command)
	{
	case EV_DEBUG:
		{
			char string[80];
			int len;

			len = MEMGETBYTE(p);
			memcpy(string, buffer, len);
			string[len] = 0;

			dprintf"[Interpreter Debug] %s\n", string));
		}
		break;

	case EV_ANIMATEACTOR:
		{
			ACTOR2 *actor;
			int anim, flags;
			float speed;

			actor = GetActorFromUID(MEMGETINT(p));
			if (!actor) return 0;

			anim = MEMGETINT(p);
			flags = MEMGETINT(p);
			speed = MEMGETFLOAT(p);

			AnimateActor(actor->actor,anim,(char)(flags & 1),(char)(flags & 2),speed,0,0);
		}
		break;

	case EV_SETENEMY:
		{
			ENEMY *nme;
			nme = GetEnemyFromUID(MEMGETINT(p));
			if (!nme) return 0;

			switch (MEMGETINT(p))
			{
			case FS_SET_MOVE:
				nme->isWaiting = 0;
				nme->path->startFrame = actFrameCount;
				nme->path->endFrame = nme->path->startFrame + (60*nme->speed);
				break;
			case FS_SET_STOP:
				nme->isWaiting = -1; break;
			case FS_SET_TOGGLEMOVE:
				if (nme->isWaiting)
				{
					nme->isWaiting = 0;
					nme->path->startFrame = nme->path->endFrame;
					nme->path->endFrame = nme->path->startFrame + (60*nme->speed);
				}
				else
					nme->isWaiting = -1;
				break;
			}
		}
		break;

	case EV_ON:
		{
			TRIGGER *t;
			EVENT *e;
			int flags;
			void **param = AllocArgs(1);

			if (t = LoadTrigger(p))
			{
				flags = MEMGETINT(p);
				*param = *p;
				e = MakeEvent( InterpretEvent, param );
				AttachEvent(t, e, (short)flags, 0);
			}
			else
				return 0;
		}
		break;

	case EV_TELEPORT:
		{
			TRIGGER *t;
			EVENT *e;
			int fNum = MEMGETINT(p),
				tNum = MEMGETINT(p),
				ht = MEMGETINT(p),
				type = MEMGETINT(p),
				time = actFrameCount+25;

			VECTOR telePos;
			void **param;

			switch( type )
			{
			case TELEPORT_TELEPORT:
				player[fNum].frogState &= ~FROGSTATUS_ISSTANDING;
				player[fNum].frogState |= FROGSTATUS_ISTELEPORTING;
				//frog[fNum]->action.isTeleporting = 25;
				//frog[fNum]->action.safe = 65;

				fadeDir		= FADE_OUT;
				fadeOut		= 1;
				fadeStep	= 8;
				doScreenFade = 63;

				SetVector(&telePos,&frog[fNum]->actor->pos);
				CreateAndAddFXRipple(RIPPLE_TYPE_TELEPORT,&telePos,&upVec,30,0,0,15);
				telePos.v[Y] += 20;
				CreateAndAddFXRipple(RIPPLE_TYPE_TELEPORT,&telePos,&upVec,25,0,0,20);
				telePos.v[Y] += 40;
				CreateAndAddFXRipple(RIPPLE_TYPE_TELEPORT,&telePos,&upVec,20,0,0,25);
				telePos.v[Y] += 60;
				CreateAndAddFXRipple(RIPPLE_TYPE_TELEPORT,&telePos,&upVec,15,0,0,30);
				PlaySample(88,NULL,255,128);

				param = AllocArgs(1);
				param[0] = (void *)time;
				t = MakeTrigger( OnTimeout, param );

				param = AllocArgs(3);
				param[0] = (void *)fNum;
				param[1] = (void *)tNum;
				param[2] = (void *)type;
				e = MakeEvent( TeleportFrog, param );

				AttachEvent( t, e, TRIGGER_ONCE, 0 );

				break;

			case TELEPORT_SPRING:

				break;
			}
			
		}
		break;

	default:
		dprintf"[Interpreter] Unrecognised command\n"));
		return 0;
	}

	return 1;
}			

/*	--------------------------------------------------------------------------------
    Function		: Interpret
	Parameters		: UBYTE*
	Returns			: TRUE if success, FALSE otherwise
*/
int Interpret(const UBYTE *buffer)
{
	UBYTE *p, *q;
	int events, size;
	UBYTE command;

	p = (UBYTE*)buffer;

	events = MEMGETINT(&p);

	while (events--)
	{
		size = MEMGETINT(&p);
		q = p;
		if (!ExecuteCommand(q)) return 0;
		p += size;
	}

	return 1;
}


/*	--------------------------------------------------------------------------------
    Function		: Interpret
	Parameters		: UBYTE*
	Returns			: TRUE if success, FALSE otherwise
*/
void LoadTestScript(const char* filename)
{
#ifdef PC_VERSION
	HANDLE h;
	long size, read;
	UBYTE* buffer;
	UBYTE version;
	
	dprintf"Testing script %s\n", filename));

	h = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (h == INVALID_HANDLE_VALUE)
	{
		dprintf"Couldn't load script file %s\n", filename)); return;
	}

	size = GetFileSize(h, NULL);
	buffer = JallocAlloc(size, NO, "entLoad");

	ReadFile(h, buffer, size, &read, NULL);
	CloseHandle(h);

	if (!InitLevelScript(buffer))
	{
		dprintf"Failed initialising script\n", filename)); return;
	}
#endif
}

int InitLevelScript(void *buffer)
{
	FreeLevelScript();

	scriptBuffer = buffer;
	
	if (!(*scriptBuffer == 0x02 && Interpret(scriptBuffer + 1)))
	{
		JallocFree(&scriptBuffer); scriptBuffer = NULL;
		return 0;
	}

	return 1;
}

int FreeLevelScript(void)
{
	JallocFree(&scriptBuffer); scriptBuffer = NULL;
}


