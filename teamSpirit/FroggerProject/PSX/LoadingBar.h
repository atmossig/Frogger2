#ifndef LOADINGBAR_H_INCLUDE
#define LOADINGBAR_H_INCLUDE

#define NUM_WATER_TILESX 8
#define NUM_WATER_TILESY 8



extern int barProgress;
extern int loadFrameCount;

void StepProgressBar ( int percentAmount );

void SetProgressBar ( int percentage );

void DrawLoadingBar ( int execute );

void loadingInit ( int worldID, int levelID );

#endif