
#ifndef __JALLOC_H__
#define __JALLOC_H__

#define UBYTE unsigned char
#define ULONG unsigned long
#define LONG  long

#define JALLOC_NAME_LEN 11
//#define CHECKJALLOC

typedef unsigned char 	uchar;
//typedef unsigned short 	ushort;
typedef unsigned long	ulong;

#ifndef PC_VERSION
#define MAXJALLOCS	1600	//1280
#else
#define MAXJALLOCS	4096
#endif

typedef struct 
{
	UBYTE			*address;				      		// where in memory the block is
    long			size;				      			// size of this block
	char			name[JALLOC_NAME_LEN];
	char			inuse;				  	   			// is this block in use?
} jallocentType;

typedef struct 
{
	jallocentType	blocks[MAXJALLOCS];	  	   			// array of control structs
	ulong		base;					  	   			// base address for global block
	long		size;					  	   			// size of area
	int			lastUsedStatic;			  	   			// number of last used block
	int			lastUsedDynamic;		  	   			// number of last used block
	int			staticUsed;
	int			dynamicUsed;
} JALLOCTYPE;


void CheckJalloc();
void JallocInit(ULONG base,ULONG size);
UBYTE *JallocAlloc(LONG size, int clear,char *name);
UBYTE *JallocAllocDynamic(LONG size, int clear,char *name);
UBYTE *JallocAllocStatic(LONG size, int clear,char *name);
void JallocFree(UBYTE **blk);
BOOL JallocFreeDynamic(UBYTE **blk);
BOOL JallocFreeStatic(UBYTE **blk);
void JallocReset();
void ShowJalloc();
void memclear(char *data, int size);
void DMAMemory(char *, u32 ROMStart, u32 ROMEnd);
u32 DMAGetSize(u32 ROMStart, u32 ROMEnd);

extern JALLOCTYPE	jallocControl;

#endif
