#ifndef _SCRIPT_H_INCLUDED
#define _SCRIPT_H_INCLUDED

#include "types.h"
#include "define.h"

extern int InitScripting(void);

extern void LoadTestScript(const char* filename);
extern int Interpret(const UBYTE *);

extern int LoadLevelScript(int world, int level);
extern int FreeLevelScript(void);

extern UBYTE* scriptBuffer;

extern int lineNumber;

#endif

