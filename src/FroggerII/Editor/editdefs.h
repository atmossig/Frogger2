
#ifndef _EDITDEFS_INCLUDED_H
#define _EDITDEFS_INCLUDED_H

#include "edittypes.h"

/*	------------------------------------------------------------------------------------------- */

#define EDITORTEXTURES 9
#define EDITORICONSIZE 0.25f

typedef struct TAGEDITICONS
{
	D3DTEXTUREHANDLE handle;
	LPDIRECTDRAWSURFACE surface;
} EDITICONS;

EDITICONS editicons[EDITORTEXTURES];


/*	------------------------------------------------------------------------------------------- */

void EdXfmPoint (VECTOR *res, VECTOR *in);

void FreeEnemyList(void);
void FreePlatformList(void);

extern EDITPATHNODE *EditorAddFlag(GAMETILE* tile, EDITPATH* path);
extern void EditorSubFlag(EDITPATHNODE *node, EDITPATH *path);
extern void EditorAddFlags(GAMETILE *end, EDITPATH *path);
extern EDITPATH* CreateEditPath(void);
extern CREATEENTITY *EditorAddCreateNode(const char *id, int flags, int ID, CREATETYPE type, EDITPATH *path, int startNode);
extern void EditorSubCreateNode(CREATEENTITY *node);
extern void FreeEditPath(EDITPATH *path);
extern void FreeCreateList(void);
extern PATH *EditorPathMake(const EDITPATH *path);
extern EDITPATH *EditorGetPath(const PATH *path);
//EDITPATH *DuplicateEditPath(EDITPATH *path);
extern void EditorCreateEntities(void);

extern int GetTileSelectionState(EDITGROUP *group);
extern int NextTileSelectionState(EDITGROUP *group);
extern int PrevTileSelectionState(EDITGROUP *group);

extern EDITGROUP *MakeEditGroup(void);
extern void FreeEditGroup(EDITGROUP **group);
extern EDITGROUP *MakePathEditGroup(EDITPATH *path);
extern EDITPATH *MakePathFromTileGroup(EDITGROUP *group);
extern EDITGROUP *MakeTileGroupFromPath(EDITPATH *path);
extern void AddGroupMember(void *thing, VECTOR *pos, EDITGROUP *group);
extern int SubGroupMember(void *thing, EDITGROUP *group);
extern BOOL AddUniqueGroupMember(void *thing, VECTOR *pos, EDITGROUP *group);
extern int ToggleGroupMember(void* thing, VECTOR* pos, EDITGROUP *group);
extern int ToggleRectSelection(int ax, int ay, int bx, int by, const EDITGROUP *list, EDITGROUP *selection);
extern int ToggleRectTileSelection(int ax, int ay, int bx, int by, EDITGROUP *selection);
extern int ToggleRectFlagSelection(int ax, int ay, int bx, int by, EDITGROUP *selection, EDITPATH *path);
extern int CountGroupMembers(EDITGROUP *group);

#endif