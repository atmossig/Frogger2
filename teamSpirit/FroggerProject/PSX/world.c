#include <string.h>
#include <islutil.h>
//#include "global.h"
#include "map_draw.h"
#include "world.h"

BFF_Header* level_bff = NULL;
FMA_WORLD* fma_world = NULL;


void DrawWorld()
{
	if ( fma_world )	
		MapDraw_DrawFMA_World ( fma_world );
	// ENDIF

}

