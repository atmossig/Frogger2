
#ifndef _EDITDEFS_INCLUDED_H
#define _EDITDEFS_INCLUDED_H


const char* TILESTATESTRINGS[] = {
	"Normal",	//	TILESTATE_NORMAL,
	"Deadly",	//	TILESTATE_DEADLY,
	"Sink",		//	TILESTATE_SINK,
	"Ice",		//	TILESTATE_ICE,
	"Superhop",	//	TILESTATE_SUPERHOP,
	"Join",		//	TILESTATE_JOIN,
	"Spring",	//	TILESTATE_SPRING,
	"Teleport"	//	TILESTATE_TELEPORTER,
};

const char* GARIBTYPESTRINGS[] = {
	"SPAWN_GARIB",			//0
	"EXTRAHEALTH_GARIB",	//1
	"EXTRALIFE_GARIB",		//2
	"AUTOHOP_GARIB",		//3
	"LONGHOP_GARIB",		//4
	"LONGTONGUE_GARIB",		//5
	"WHOLEKEY_GARIB",		//6
	"HALFLKEY_GARIB",		//7
	"HALFRKEY_GARIB",		//8
};

static int platformFlagIcons[] = {
	PLATFORM_NEW_FORWARDS,
	PLATFORM_NEW_BACKWARDS, 
	PLATFORM_NEW_CYCLE,
	PLATFORM_NEW_PINGPONG,
	PLATFORM_NEW_MOVEUP,
	PLATFORM_NEW_MOVEDOWN,
	PLATFORM_NEW_STEPONACTIVATED
};

const char *EDITVARSTRINGS[] = { "char*", "int","float" };

/*	------------------------------------------------------------------------------------------- */

typedef enum {
	EDIT_PLACE_GARIB,
	EDIT_PLACE_ENEMY,
	EDIT_PLACE_PLATFORM,
	EDIT_MAKE_PATH,
	EDIT_MOVECAMERA,
	EDIT_FIND_THING,
	EDIT_ROTATE_CAMERA,
	EDIT_SET_SPEED,
	EDIT_SET_START,
	EDIT_SET_OFFSET,
	EDIT_SET_OFFSET2,
	EDIT_NONE
} EDITMODE;

typedef enum { EDITVAR_STRING, EDITVAR_INT, EDITVAR_FLOAT } EDITVAR;
typedef enum { CREATE_ENEMY, CREATE_PLATFORM } CREATETYPE;

typedef struct TAGEDITICONS
{
	D3DTEXTUREHANDLE handle;
	LPDIRECTDRAWSURFACE surface;
} EDITICONS;

EDITICONS editicons;

typedef struct TAGEDITPATHNODE
{
	struct TAGEDITPATHNODE *next;
	float speed, offset, offset2;

	BOOL start;
	GAMETILE *tile;
} EDITPATHNODE;

typedef struct TAGCREATEENTITYNODE
{
	struct TAGCREATEENTITYNODE *prev, *next;

	char	type[12]; // 8+3+1
	int		flags;
	EDITPATHNODE* path;
	UBYTE	thing;
	VECTOR	iconPos;

} CREATEENTITYNODE;

/*	-------------------------------------------------------------------------------------------
	Globals
*/

extern EDITPATHNODE *editPath;

/*	------------------------------------------------------------------------------------------- */

CREATEENTITYNODE *EditorAddCreateNode(const char *id, int flags, PATH *path, CREATETYPE type);
void EditorSubCreateNode(CREATEENTITYNODE *node);
void FreeEditPath(EDITPATHNODE *path);
PATH *EditorPathMake();
void EditorGetPath(PATH *path);


#endif