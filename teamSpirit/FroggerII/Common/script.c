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

#ifdef DEBUG_SCRIPTING
int lineNumber = 0;

#endif

/* --------------------------------------------------------------------------------- */

//int Interpret(const UBYTE *buffer);
TRIGGER *LoadTrigger(UBYTE **p);
BOOL ExecuteCommand(UBYTE *buffer);

UBYTE* scriptBuffer;


/* --------------------------------------------------------------------------------- */

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

	dprintf"Finding actor %d: ", UID));
	e = GetEnemyFromUID(UID);
	if (e)
		return e->nmeActor;
	
	dprintf"Searching plats: "));
	p = GetPlatformFromUID(UID);
	if (p)
	{
		dprintf"Found\n"));
		return p->pltActor;
	}

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
	if (!Interpret(buffer))
		dprintf "Error running script within trigger!\n"));
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

int AlwaysTrigger(TRIGGER *t) { return TRUE; }

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
			PLATFORM *plt;

			params = AllocArgs(2);
			if (!(plt = GetPlatformFromUID(MEMGETINT(p)))) return 0;
			params[0] = plt->path;
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

			params = AllocArgs(2);
			(int)params[0] = actFrameCount + (int)(60.0 * time);
			(int)params[1] = (int)(60.0 * time);
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
#ifdef DEBUG_SCRIPTING
	case EV_DEBUG:
		{
			char string[80];
			int len;

			len = MEMGETBYTE(p);
			memcpy(string, buffer, len);
			string[len] = 0;

			dprintf"[Interpreter Debug] %s", string));

			if(lineNumber) dprintf" (line %d)", lineNumber));

			dprintf"\n"));
			break;
		}
#endif
		
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
			RecalculatePlatform(plt);
			break;
		}

	case EV_RESETPLATFLAG:
		{
			PLATFORM *plt;
			int flag;
			if (!(plt = GetPlatformFromUID(MEMGETINT(p)))) return 0;
			flag = MEMGETINT(p);
			plt->flags &= ~flag;
			RecalculatePlatform(plt);
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
Move:
			case FS_SET_MOVE:
				if (nme->isWaiting)
				{
					nme->isWaiting = 0;
					nme->path->startFrame = actFrameCount;
					nme->path->endFrame = nme->path->startFrame + (int)(60.0*nme->speed);
				}
				break;
Stop:			
			case FS_SET_STOP:
				nme->isWaiting = -1; break;
			
			case FS_SET_TOGGLEMOVE:
				if (nme->isWaiting) goto Move; else goto Stop;	// gotos! I have no shame - Dave
Invis:
			case FS_SET_INVIS:
				nme->active = 0;
				nme->nmeActor->draw = 0;
				break;
Vis:
			case FS_SET_VIS:
				nme->active = 1;
				nme->nmeActor->draw = 1;
				break;

			case FS_SET_TOGGLEVIS:
				if (nme->active) goto Invis; else goto Vis;
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
				if (plt->isWaiting)
				{
					plt->isWaiting = 0;
					plt->path->startFrame = actFrameCount;
					plt->path->endFrame = plt->path->startFrame + (int)(60.0*plt->currSpeed);
				}
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
				param[0] = *p;
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

			player[fNum].frogState = FROGSTATUS_ISTELEPORTING;	// clear ALL other flags
			FrogLeavePlatform(fNum);	// bah

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
			void **param;
			SPRINGINFO *info;
			GAMETILE *tile;
			int frogNum, tileNum;
			float height, time;

			frogNum = MEMGETBYTE(p);
			tileNum = MEMGETINT(p);
			height = MEMGETFLOAT(p);
			time = MEMGETFLOAT(p);

			player[frogNum].frogState = FROGSTATUS_ISTELEPORTING;	// clear ALL flags
			FrogLeavePlatform(frogNum);	// bah

			info = (SPRINGINFO*)JallocAlloc(sizeof(SPRINGINFO), NO, "spring");

			info->frog = frogNum;
			tile = GetTileFromNumber(tileNum);
			info->dest = tile;
			SetVector(&info->S, &frog[frogNum]->actor->pos);
			SubVector(&info->V, &tile->centre, &info->S);
			SetVector(&info->H, &tile->normal);
			ScaleVector(&info->H, height);

			info->start = actFrameCount;
			info->end = actFrameCount + (time * 60.0);

			t = MakeTrigger( AlwaysTrigger, NULL );

			param = AllocArgs(2);
			param[0] = (void*)info;
			param[1] = (void*)t;

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
				plt->path->fromNode = flag;
				RecalculatePlatform(plt);
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

	case EV_BABYONPLATFORM:
		{
			long pid = MEMGETINT(p);
			int baby = MEMGETBYTE(p);
			PLATFORM *plat = GetPlatformFromUID( pid );

			babyFollow[baby] = plat->pltActor->actor;
			break;
		}

	case EV_CHANGELEVEL:
		{
			int world = MEMGETBYTE(p);
			int level = MEMGETBYTE(p);

			player[0].worldNum = world;
			player[0].levelNum = level;

			levelIsOver = 10;
			showEndLevelScreen = 0;
			break;
		}

	case EV_SETSTARTTILE:	gTStart[0] = GetTileFromNumber(MEMGETINT(p)); break;

	case EV_FOG:
		{
			fog.r = MEMGETBYTE(p);
			fog.g = MEMGETBYTE(p);
			fog.b = MEMGETBYTE(p);
			fog.min = MEMGETWORD(p);
			fog.max = MEMGETWORD(p);
			fog.mode = 1;
			break;
		}

	case EV_NO_FOG:	fog.mode = 0; break;

	case EV_CAMEOMODE_ON:
		gameState.oldMode = gameState.mode;
		gameState.mode = CAMEO_MODE;
		break;

	case EV_CAMEOMODE_OFF:
		gameState.mode = gameState.oldMode;
		break;

#ifdef DEBUG_SCRIPTING
	case C_DEBUG:
		MEMGETWORD(p);	// blank
		lineNumber = MEMGETWORD(p);
		break;
#endif

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
		if (!ExecuteCommand(q))
		{
#ifdef DEBUG_SCRIPTING
			if (lineNumber)
				dprintf"Possibly an error on line %d?\n", lineNumber));
#endif
			return 0;
		}
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
	
	dprintf"Testing script %s\n", filename));

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
		sprintf(statusMessage, "Failed initialising script %s", filename);
		dprintf "%s\n", statusMessage)); return;
	}

	sprintf(statusMessage, "Loaded script %s", filename);
}
#endif

int InitLevelScript(void *buffer)
{
	int err = 0;

	KillAllTriggers();
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


