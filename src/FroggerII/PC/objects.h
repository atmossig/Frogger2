#ifndef __OBJECTS_H
#define __OBJECTS_H

#define MAX_OBJECT_BANKS 5


float CalcRadius(MESH *mesh);
BOOL LoadObjectBank(int num);
void InitObjectBanks();
void FindObject(OBJECT_CONTROLLER **, int, char *,BOOL vital);
void FindSubObject(OBJECT **obj, int objID);
OBJECT *FindSubObjectFromObject(OBJECT *obj, int objID);
void LoadTexture(TEXTURE *texture);

void SetObjectXlu(OBJECT *object,BOOL xlu);


extern OBJECT_BANK	objectBanks[];
extern int numoftc;


#endif