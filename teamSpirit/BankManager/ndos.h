#ifndef __NDOS_H
#define __NDOS_H








int LoadObject(OBJECTINFO *obj);
void ValidateFilename(char *name);
int InitObject(INPUT_OBJECT **objectPtr, short flags);
void AddObject(OBJECTINFO *obj);
void LoadNDOSFromBank(int bankNum);
BOOL FAR PASCAL ObjBankProperties(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam);
BOOL FAR PASCAL NewObjBankProperties(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam);
void AddObjectFlags(INPUT_OBJECT *obj, int flags);
void StripObjectFlags(INPUT_OBJECT *obj, int flags);
void FreeNDOSFromBank(int bankNum);
void LoadAllNDOS();

void AddObject(OBJECTLIST *objList, OBJECTINFO *obj);
void AddNDOToBank();
void AddOBEToBank();
void SubObject(OBJECTLIST *objList, OBJECTINFO *ptr);



extern OBJECTINFO	*currentObject;
extern OBJECTINFO	currentNDO;

#endif