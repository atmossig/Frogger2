#ifndef _MAP_H_INCLUDE
#define _MAP_H_INCLUDE

#include "define.h"

extern SCENIC		*firstScenic;

extern GAMETILE *firstTile;
extern GAMETILE *gTStart[4];

void LoadLevelEntities ( int worldID, int levelID );

void LoadMapBank			( int collBank );
void LoadCollision			( int collBank );
void LoadScenics			( int collBank );
void FreeMap				( void );
void FreeEntityBank			( void );
void FreeScenic				( void );

#endif