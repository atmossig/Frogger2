#ifndef _WORLDEFF_H_INCLUDE
#define _WORLDEFF_H_INCLUDE


#define SHIFT_DEPTH ( 1 << 0 )
#define TRANSPARENT ( 1 << 1 )


void DrawWater ( FMA_MESH_HEADER *mesh, int flags );





typedef struct _SCENICOBJ
{
	struct _SCENICOBJ *next,*prev;			// ptr to next / prev platform

	int						flags;
	FMA_WORLD*		fmaObj;
	SVECTOR				position;
} SCENICOBJ;

typedef struct _SCENICOBJLIST
{
	int				numEntries;
	SCENICOBJ	head;
} SCENICOBJLIST;

extern SCENICOBJLIST scenicObjList;


void CreateAndAddScenicObject ( char *name, short posx, short posy, short posz, int newFlags );

void InitScenicObjLinkedList ( void );

void AddScenicObj ( SCENICOBJ *scenicObj );

void FreeScenicObjectList ( SCENICOBJ *scenicObj );

void SubScenicObject ( SCENICOBJ *scenicObj );

void DrawScenicObj ( FMA_MESH_HEADER *mesh, int flags );

#endif