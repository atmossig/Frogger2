#ifndef _SCRIPT_H_INCLUDED
#define _SCRIPT_H_INCLUDED

extern int InitScripting(void);

extern void LoadTestScript(const char* filename);
extern int Interpret(const UBYTE *);

extern int InitLevelScript(void *buffer);
extern int FreeLevelScript(void);

extern UBYTE* scriptBuffer;

#endif