#include "sonylibs.h"
#include "Types.h"
#include "Main.h"
#include "Map_Draw.h"
#include "Water.h"

WATERLIST waterList;

void CreateAndAddWaterObject ( char *name, short posx, short posy, short posz, int newFlags )
{
	WATER *newItem;

	newItem = ( WATER* ) MALLOC0 ( sizeof ( WATER ) );

	AddWater ( newItem );

	newItem->position.vx = posx;
	newItem->position.vy = posy;
	newItem->position.vz = posz;

	newItem->flags = newFlags;
	
	utilPrintf("Creating water object : %s\n", name);
	utilUpperStr ( name );

	newItem->fma_water = ( void * ) BFF_FindObject ( BFF_FMA_WORLD_ID, utilStr2CRC ( name ) );

	if ( newItem->fma_water )
	{
		SetUpWaterMesh ( newItem->fma_water );
	}
	else
	{
		utilPrintf("Could Not Create Water.\n");
	}
	// ENDIF

}


void DrawWaterList ( void )
{
	WATER *cur, *next;

	if ( waterList.numEntries == 0 )
		return;

	for ( cur = waterList.head.next; cur != &waterList.head; cur = next)
	{
		next = cur->next;

		if ( cur->fma_water )
		{
			MapDraw_DrawFMA_Water ( cur );
		}
		// ENDIF	    
	}
	// ENDFOR

}


void InitWaterLinkedList ( void )
{
	waterList.numEntries = 0;
	waterList.head.next = waterList.head.prev = &waterList.head;
}


void AddWater ( WATER *waterObj )
{
	if ( waterObj->next == NULL )
	{
		waterList.numEntries++;
		waterObj->prev						= &waterList.head;
		waterObj->next						= waterList.head.next;
		waterList.head.next->prev = waterObj;
		waterList.head.next				=	waterObj;
	}
}

void FreeWaterObjectList ( void )
{
	WATER *cur,*next;

	if( !waterList.numEntries )
		return;

	for(cur = waterList.head.next; cur != &waterList.head; cur = next)
	{
		next = cur->next;
		SubWaterObject(cur);
	}
//	FREE(specFXList.head);
}


void SubWaterObject ( WATER *waterObj )
{
	if(waterObj->next == NULL)
		return;

	waterObj->prev->next = waterObj->next;
	waterObj->next->prev = waterObj->prev;
	waterObj->next = NULL;
	waterList.numEntries--;

	FREE(waterObj);
}
