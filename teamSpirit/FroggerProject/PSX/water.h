#ifndef WATER_H_INCLUDED
#define WATER_H_INCLUDED

typedef struct _WATER
{
	struct _WATER *next,*prev;			// ptr to next / prev platform

	FMA_WORLD*		fma_water;
	SVECTOR				position;
} WATER;

typedef struct _WATERLIST
{
	int			numEntries;
	WATER		head;
} WATERLIST;

extern WATERLIST waterList;


void CreateAndAddWaterObject ( char *name, short posx, short posy, short posz );

void InitWaterLinkedList ( void );

void AddWater ( WATER *waterObj );

void SubWaterObject ( WATER *waterObj );

void FreeWaterObjectList ( void );

#endif
