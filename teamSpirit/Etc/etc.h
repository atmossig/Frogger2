#ifndef _ETC_H_INCLUDED
#define _ETC_H_INCLUDED

#ifndef BYTE
typedef unsigned char BYTE;
#endif

typedef enum { PARAM_NONE = 0, PARAM_INT, PARAM_FLOAT, PARAM_STRING } PARAMTYPES;

typedef BYTE PARAMTYPE;

typedef struct TAGCOMMANDTABLE
{
	const char *str;
	int token;
} COMMAND_TABLE;


char WHITESPACE[] = " \t\n\r";
char NUMERIC[] = "0123456789+-.";

typedef struct TAGPARAM
{
	PARAMTYPE type;
	union
	{
		float vFloat;
		int vInt;
		char *vString;
	};
} PARAM;

enum COMMANDTYPES
{
	C_DEBUG = 0,
	C_CHANGEACTORSCALE,
	C_TOGGLEPLATFORMMOVE,
	C_TOGGLEENEMYMOVE,
	C_TOGGLETILELINK,
	C_PLAYSFX,
	C_CHANGELEVEL,
	C_ANIMATEACTOR,

	C_IF = 0xF0,
	C_AND,
	C_OR,
	NOSUCHCOMMAND = 0xFF
};

enum TRIGGERTYPES
{
	C_ENEMYONTILE = 0,
	C_FROGONTILE,
	C_FROGONPLATFORM,
	C_WITHINRADIUS,
	C_TIMEOUT
};

#define NUMTRIGGERS 5

COMMAND_TABLE triggerLookup[] =
{
	{	"ENEMYONTILE",	0 },
	{	"FROGONTILE",	1 },
	{	"FROGONPLATFORM", 2 },
	{	"WITHINRADIUS", 3 },
	{	"TIMEOUT", 4 }
};

PARAMTYPE triggerList[][8] =
{ 
	{ PARAM_INT, PARAM_INT, 0 },	// EnemyOnTile enemy, tile
	{ PARAM_INT, PARAM_INT, 0},		// FrogOnTile frog, tile
	{ PARAM_INT, PARAM_INT, 0 },	// FrogOnPlatform frog, platform
	{ PARAM_INT, PARAM_FLOAT, PARAM_FLOAT, PARAM_FLOAT, PARAM_FLOAT, 0 }, // actorwithinradius actor, x, y, z, radius
	{ PARAM_INT, 0 },	// timer ID
};

#define NUMCOMMANDS 11

COMMAND_TABLE commandLookup[NUMCOMMANDS] =
{
	{	"DEBUG",				C_DEBUG },
	{	"SCALEACTOR",			C_CHANGEACTORSCALE },
	{	"TOGGLEPLATFORMMOVE",	C_TOGGLEPLATFORMMOVE },
	{	"TOGGLEENEMYMOVE",		C_TOGGLEENEMYMOVE },
	{	"TOGGLETILELINK",		C_TOGGLETILELINK },
	{	"PLAYSFX",				C_PLAYSFX },
	{	"CHANGELEVEL",			C_CHANGELEVEL },
	{	"ANIMATE",				C_ANIMATEACTOR },
	{	"IF",	C_IF },
	{	"AND",	C_AND },
	{	"OR",	C_OR }
};

PARAMTYPE commandList[][8] =
{
	{ PARAM_STRING, 0 },	// debug
	{ PARAM_INT, PARAM_FLOAT, 0 }, // ScaleActor (uniform)
	{ PARAM_INT, 0 }, // TogglePlatformMove #
	{ PARAM_INT, 0 }, // ToggleEnemyMove #
	{ PARAM_INT, 0 }, // ToggleTileLink #
	{ PARAM_INT, PARAM_INT, PARAM_INT, PARAM_FLOAT,
		PARAM_FLOAT, PARAM_FLOAT, PARAM_FLOAT, 0 }, // PlaySfx # vol pitch x y z radius
	{ PARAM_INT, PARAM_INT, 0 }, // changelevel world, level
	{ PARAM_INT, PARAM_INT, PARAM_INT, PARAM_FLOAT, 0 } // animate # anim flags speed
};

const char* PARAMNAMESTRING[] = { "end of line", "integer", "float", "string" };


/* ------------------------- Interpreter stuff ----------------------------- */



/* --------------------------- Compiler stuff ----------------------------- */



#endif