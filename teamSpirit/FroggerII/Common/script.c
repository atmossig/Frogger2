/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: script.c
	Programmer	: David Swift

----------------------------------------------------------------------------------------------- */


#include "ultra64.h"
#include "incs.h"
#include "codes.h"
#include "stdio.h"
#include "script.h"
#include "memload.h"

#define DEBUG_SCRIPTING

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

float volTest = 0.5;

float ChangeVolume(int a, float delta)
{
	volTest += delta;
	return volTest;
}

/* --------------------------------------------------------------------------------- */

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
			params[0] = (void*)MEMGETBYTE(p);
			params[1] = (void*)GetTileFromNumber(MEMGETINT(p));
			trigger = MakeTrigger(FrogOnTile, params);
		}
		break;

	case TR_FROGONPLATFORM:
		{
			PLATFORM *platform;

			params = AllocArgs(2);
			params[0] = (void*)frog[MEMGETBYTE(p)];
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
			params[1] = JallocAlloc(sizeof(int), NO, "int"); *(int*)params[1] = MEMGETWORD(p);
			trigger = MakeTrigger(PathAtFlag, params);
		}
		break;

	case TR_PLATATFLAG:
		{
			ENEMY *enemy;

			params = AllocArgs(2);
			if (!(enemy = GetEnemyFromUID(MEMGETINT(p)))) return 0;
			params[0] = enemy->path;
			params[1] = JallocAlloc(sizeof(int), NO, "int"); *(int*)params[1] = MEMGETWORD(p);
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

	case TR_WAIT:
		{
			float time = MEMGETFLOAT(p);

			params = AllocArgs(1);
			(int)params[0] = actFrameCount + (int)(60.0 * time);
			trigger = MakeTrigger(OnTimeout, params);
		}
		break;

	default:
#ifdef DEBUG_SCRIPTING
		dprintf"Unrecognised trigger type %02x, skipping\n", token));
#endif
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
			break;
		}
		
	case EV_SETTILE:
		{
			int tile, state;
			
			tile = MEMGETINT(p);
			state = MEMGETBYTE(p);

			firstTile[tile].state = (unsigned char)state;
			break;
		}

	case EV_SETENEMYFLAG:
		{
			ENEMY *nme;
			int flag;
			if (!(nme = GetEnemyFromUID(MEMGETINT(p)))) return 0;
			flag = MEMGETINT(p);
			nme->flags |= flag;
			break;
		}
		
	case EV_RESETENEMYFLAG:
		{
			ENEMY *nme;
			int flag;
			if (!(nme = GetEnemyFromUID(MEMGETINT(p)))) return 0;
			flag = MEMGETINT(p);
			nme->flags &= ~flag;
			break;
		}


	case EV_SETPLATFLAG:
		{
			PLATFORM *plt;
			int flag;
			if (!(plt = GetPlatformFromUID(MEMGETINT(p)))) return 0;
			flag = MEMGETINT(p);
			plt->flags |= flag;
			break;
		}

	case EV_RESETPLATFLAG:
		{
			PLATFORM *plt;
			int flag;
			if (!(plt = GetPlatformFromUID(MEMGETINT(p)))) return 0;
			flag = MEMGETINT(p);
			plt->flags &= ~flag;
			break;
		}
		
	case EV_ANIMATEACTOR:
		{
			ACTOR2 *actor;
			char anim, flags;
			float speed;

			actor = GetActorFromUID(MEMGETINT(p));
			if (!actor) return 0;

			anim = MEMGETBYTE(p);
			flags = MEMGETBYTE(p);
			speed = MEMGETFLOAT(p);

			AnimateActor(actor->actor,anim,(char)(flags & 1),(char)(flags & 2),speed,0,0);
			break;
		}

	case EV_SETENEMY:
		{
			ENEMY *nme;
			nme = GetEnemyFromUID(MEMGETINT(p));
			if (!nme) return 0;

			switch (MEMGETBYTE(p))
			{
			case FS_SET_MOVE:
				nme->isWaiting = 0;
				nme->path->startFrame = actFrameCount;
				nme->path->endFrame = nme->path->startFrame + (int)(60.0*nme->speed);
				break;
			case FS_SET_STOP:
				nme->isWaiting = -1; break;
			case FS_SET_TOGGLEMOVE:
				if (nme->isWaiting)
				{
					nme->isWaiting = 0;
					nme->path->startFrame = actFrameCount;
					nme->path->endFrame = nme->path->startFrame + (int)(60.0*nme->speed);
				}
				else
					nme->isWaiting = -1;
				break;
			}
			break;
		}

	case EV_SETPLATFORM:
		{
			PLATFORM *plt;
			plt = GetPlatformFromUID(MEMGETINT(p));
			if (!plt) return 0;

			switch (MEMGETBYTE(p))
			{
			case FS_SET_MOVE:
				plt->isWaiting = 0;
				plt->path->startFrame = actFrameCount;
				plt->path->endFrame = plt->path->startFrame + (int)(60.0*plt->currSpeed);
				break;
			case FS_SET_STOP:
				plt->isWaiting = -1; break;
			case FS_SET_TOGGLEMOVE:
				if (plt->isWaiting)
				{
					plt->isWaiting = 0;
					plt->path->startFrame = actFrameCount;
					plt->path->endFrame = plt->path->startFrame + (int)(60.0*plt->currSpeed);
				}
				else
					plt->isWaiting = -1;
				break;
			}
			break;
		}

	case EV_ON:
		{
			TRIGGER *t;
			EVENT *e;
			int flags;
			void **param = AllocArgs(1);

			if (t = LoadTrigger(p))
			{
				flags = MEMGETBYTE(p);
				*param = *p;
				e = MakeEvent( InterpretEvent, param );
				AttachEvent(t, e, (short)flags, 0);
			}
			else
				return 0;
			break;
		}

	case EV_IF:
		{
			TRIGGER *t;

			if (t = LoadTrigger(p))
			{
				if (t->func(t)) Interpret(*p);
				SubTrigger(t);
			}
			else return 0;
			break;
		}

	case EV_TELEPORT:
		{
			TRIGGER *t;
			EVENT *e;
			int fNum = MEMGETBYTE(p),
				tNum = MEMGETINT(p),
				time = (MEMGETFLOAT(p) * 60) + actFrameCount;
			VECTOR telePos;
			void **param;

			player[fNum].frogState &= ~FROGSTATUS_ISSTANDING;
			player[fNum].frogState |= FROGSTATUS_ISTELEPORTING;
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

			param = AllocArgs(2);
			param[0] = (void *)fNum;
			param[1] = (void *)tNum;
			e = MakeEvent( TeleportFrog, param );

			AttachEvent( t, e, TRIGGER_ONCE, 0 );

			break;
		}

	case EV_SPRING:
		{
			TRIGGER *t;
			EVENT *e;
			int fNum = MEMGETBYTE(p),
				tNum = MEMGETINT(p),
				ht = MEMGETINT(p),
				time = MEMGETINT(p);
			void **param;

			player[fNum].frogState &= ~FROGSTATUS_ISSTANDING;
			player[fNum].frogState |= FROGSTATUS_ISTELEPORTING;

			param = AllocArgs(2);
			param[0] = (void *)&player[fNum].frogState;
			param[1] = (void *)FROGSTATUS_ISTELEPORTING;
			t = MakeTrigger( BitCheck, param );

			param = AllocArgs(5);
			param[0] = (void *)fNum;
			param[1] = (void *)tNum;
			param[2] = (void *)time;
			param[3] = (void *)ht;
			param[4] = (void *)t;
			e = MakeEvent( SpringFrog, param );

			AttachEvent( t, e, TRIGGER_ALWAYS, 0 );

			break;
		}

	case EV_MOVEENEMY:
		{
			ENEMY *nme;
			int flag;

			nme = GetEnemyFromUID(MEMGETINT(p));
			if (!nme) return 0;

			flag = MEMGETWORD(p);
			if (flag > 0 && flag < nme->path->numNodes)
			{
				nme->path->fromNode = nme->path->toNode = flag;
			}
			break;
		}

	case EV_MOVEPLAT:
		{
			PLATFORM *plt;
			int flag;

			plt = GetPlatformFromUID(MEMGETINT(p));
			if (!plt) return 0;

			flag = MEMGETWORD(p);
			if (flag > 0 && flag < plt->path->numNodes)
			{
				plt->path->fromNode = plt->path->toNode = flag;
			}
			break;
		}

	case EV_DROPGARIB:
		{
			GARIB *garib;
			GAMETILE *tile;
			int g, t;

			g = MEMGETINT(p);
			t = MEMGETINT(p);

			if (g < 0 || g > garibCollectableList.numEntries || t < 0) return 0;

			for (garib = garibCollectableList.head.next; g; garib = garib->next, g--);
			tile = GetTileFromNumber(t);

			garib->gameTile = tile;
			garib->dropSpeed = 0.0;
			break;
		}

	case EV_CHANGEVOLUME:
		{
			float delta, amount;
			int n, platnum;
			PLATFORM *plt;
			VECTOR u, v;

			n = MEMGETBYTE(p);
			delta = MEMGETFLOAT(p);
			platnum = MEMGETINT(p);
			amount = ChangeVolume(n, delta);

			plt = GetPlatformFromUID(platnum);
			if (!plt || (plt->path->numNodes != 2)) return 0;

			GetPositionForPathNode(&u, &plt->path->nodes[1]);
			GetPositionForPathNode(&v, &plt->path->nodes[0]);

			ScaleVector(&u, amount);
			ScaleVector(&v, 1.0-amount);
			AddVector(&plt->pltActor->actor->pos, &u, &v);
			plt->isWaiting = -1;
			break;
		}

	default:
#ifdef DEBUG_SCRIPTING
		dprintf"[Interpreter] Unrecognised command\n"));
#endif
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
#ifdef PC_VERSION
void LoadTestScript(const char* filename)
{
	HANDLE h;
	long size, read;
	UBYTE* buffer;
	
	dprintf"Testing script %s\nf", filename));

	h = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (h == INVALID_HANDLE_VALUE)
	{
		sprintf(statusMessage, "Couldn't load script file %s", filename);
		dprintf "%s\n", statusMessage)); return;
	}

	size = GetFileSize(h, NULL);
	buffer = JallocAlloc(size, NO, "entLoad");

	ReadFile(h, buffer, size, &read, NULL);
	CloseHandle(h);

	if (!InitLevelScript(buffer))
	{
		sprintf(statusMessage, "Failed initialising script", filename);
		dprintf "%s\n", statusMessage)); return;
	}

	sprintf(statusMessage, "Loaded script %s", filename);
}
#endif

int InitLevelScript(void *buffer)
{
	int err = 0;

	FreeLevelScript();

	scriptBuffer = buffer;
	
	if (*scriptBuffer != 0x02)
	{
#ifdef DEBUG_SCRIPTING
		dprintf"Script failed version check\n"));
#endif
		err = 1;
	}
	else if (!Interpret(scriptBuffer + 1))
		err = 1;

	if (err)
	{
		JallocFree(&scriptBuffer); scriptBuffer = NULL;
		return 0;
	}

	return 1;
}

int FreeLevelScript(void)
{
	JallocFree(&scriptBuffer); scriptBuffer = NULL; return 1;
}


