#ifndef _MAIN_H_INCLUDE
#define _MAIN_H_INCLUDE

#include "fixed.h"
#include "sonylibs.h"
#include "types.h"

//#define MALLOC0(S)	memoryAllocateZero(S, __FILE__, __LINE__)

//GsRVIEW2	camera;
extern GsRVIEW2	camera;

extern fixed gameSpeed;
extern unsigned long actFrameCount, lastActFrameCount;
extern char UseAAMode;
extern char UseZMode;



extern ULONG	frame;

extern char baseDirectory[256];

#define ASSERT(A)	if(!(A)) {utilPrintf("Debug ASSERT failed at line %d, in "__FILE__, __LINE__);CRASH;}


#define MALLOC0(S)	memoryAllocateZero(S, __FILE__, __LINE__)
void *memoryAllocateZero(unsigned long size, char *file, int line);

/*typedef struct _displayPageType {
	DISPENV			dispenv;
	DRAWENV			drawenv;
	unsigned long	*ot;
	char			*primBuffer, *primPtr;
} displayPageType;*/

#endif
