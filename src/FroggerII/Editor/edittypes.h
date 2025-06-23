#ifndef _EDITTYPES_H_INCLUDED
#define _EDITTYPES_H_INCLUDED

/* ------------------------------------------------------------------------- */

#define MAXTYPELENGTH 20
#define CE_NUMVALUES 8

typedef struct TAGEDITPATHNODE
{
	struct TAGEDITPATHNODE *link;
	float speed, offset, offset2;
	int start, waitTime, level;
	unsigned long sample;

	GAMETILE *tile;

} EDITPATHNODE;

typedef struct TAGEDITPATH
{
	//struct TAGEDITPATH *prev, *next;
	int refs;				// number of references - dealloc when 0
	EDITPATHNODE *nodes;
} EDITPATH;

typedef struct TAGEDITGROUPNODE
{
	struct TAGEDITGROUPNODE* link;
	//int icon;
	VECTOR pos;
	void* thing;
} EDITGROUPNODE;

typedef struct TAGEDITGROUP
{
	EDITGROUPNODE* nodes;
	int refs;
} EDITGROUP;

typedef struct TAGCREATEENTITY
{
	char	type[MAXTYPELENGTH];
	UBYTE	thing;

	EDITGROUP* group;
	EDITPATH* path;

	unsigned char facing;
	int		flags, ID, startNode, effects, objFlags;
	float	scale, radius, animSpeed, value1; // Value1 is quite random
	VECTOR	camera;

} CREATEENTITY;

typedef struct _KMAP
{
	char key;
	int command;
} KMAP;

typedef struct tagEDLOADSTATE
{
	HANDLE f;
	int ver;
	EDITPATH **pathList;
	int numPaths;
} EDLOADSTATE;

typedef struct { int icon, command; } TOOLBAR_ENTRY;
typedef TOOLBAR_ENTRY *TOOLBAR_SUBMENU;
typedef enum { EDITVAR_STRING, EDITVAR_INT, EDITVAR_FLOAT, EDITVAR_PASSTOFUNCTION } EDITVAR;
typedef enum { CREATE_ENEMY, CREATE_PLATFORM, CREATE_GARIB, CREATE_CAMERACASE, CREATE_PLACEHOLDER } CREATETYPE;

/* ------------------------------------------------------------------------- */

#endif
