#ifndef _MAIN_H_INCLUDE
#define _MAIN_H_INCLUDE

#include <isltex.h>
#include <islfont.h>
#include "fixed.h"
#include "sonylibs.h"
#include "types.h"

extern GsRVIEW2	camera;
extern ULONG	frame;
extern char baseDirectory[256];
extern psFont *font;
extern psFont *fontSmall;

extern long textEntry;	
extern char textString[255];

extern int drawLandscape;


#define ASSERT(A)	if(!(A)) {utilPrintf("Debug ASSERT failed at line %d, in "__FILE__, __LINE__);CRASH;}
#define MALLOC0(S)	memoryAllocateZero(S, __FILE__, __LINE__)

void *memoryAllocateZero(unsigned long size, char *file, int line);

#endif
