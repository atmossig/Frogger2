#ifndef _MAIN_H_INCLUDE
#define _MAIN_H_INCLUDE

#include "fixed.h"
#include "sonylibs.h"
#include "types.h"

extern GsRVIEW2	camera;
extern ULONG	frame;
extern char baseDirectory[256];

#define ASSERT(A)	if(!(A)) {utilPrintf("Debug ASSERT failed at line %d, in "__FILE__, __LINE__);CRASH;}
#define MALLOC0(S)	memoryAllocateZero(S, __FILE__, __LINE__)

void *memoryAllocateZero(unsigned long size, char *file, int line);

#endif
