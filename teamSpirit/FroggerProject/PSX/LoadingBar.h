#ifndef LOADINGBAR_H_INCLUDE
#define LOADINGBAR_H_INCLUDE

#define NUM_WATER_TILESX 1
#define NUM_WATER_TILESY 1


typedef struct _LOADINGSCREEN
{
	int				numWaterTilesX;
	int				numWaterTilesY;
	POLY_GT4	waterPolys [ NUM_WATER_TILESX * NUM_WATER_TILESY ];
} LOADINGSCREEN;

extern LOADINGSCREEN loadingScreen;

extern unsigned char waterimage[];
extern unsigned short waterpal[];

extern int barProgress;

void StepProgressBar ( int percentAmount );

void SetProgressBar ( int percentage );

void DrawLoadingBar ( int execute );

void loadingInit ( int worldID, int levelID );

#endif