#ifndef TEMP_PSX_H
#define TEMP_PSX_H

#include "types.h"

void QuatToPSXMatrix(IQUATERNION* q, MATRIX* m);

void CreateLevelObjects ( unsigned long worldID,unsigned long levelID );

#endif
