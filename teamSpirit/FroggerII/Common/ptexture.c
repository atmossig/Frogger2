/*
	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: ptexture.c
	Purpose		: Procedurally generated textures
	Programmer	: Jim Hubbard
	Date		: 2/12/99 10:30

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>
#include "incs.h"


PROCTEXTURE *prcTexList = NULL;
POLYGON *rpList = NULL;

/*	--------------------------------------------------------------------------------
	Function		: ProcessPTFire
	Purpose			: Procedural fire
	Parameters		: 
	Returns			: 
	Info			: 
*/
void ProcessPTFire( PROCTEXTURE *pt )
{
	long i = 1024,j;
	unsigned char *tmp;
	short res, c, p;
	static short pp[8] = {961, 965, 969, 973, 977, 981, 985, 989 };

	// Copy resultant buffer into texture
#ifdef PC_VERSION
	PTSurfaceBlit( ((TEXENTRY *)pt->tex)->surf, pt->buf1, pt->palette );
#else
	TEXTURE *tx = pt->tex;

	// N64 surface blit
#endif

	for( i=992; i<1024; i++ )
		pt->buf1[i] = 50;

	// Make a few blocks of nine white things
	pp[Random(8)] = (short)(960+Random(32));

	for( i=0; i<8; i++ )
	{
		c = (unsigned char)Random(31)+224;
		pt->buf1[pp[i]] = c;
		pt->buf1[pp[i]-1] = c;
		pt->buf1[pp[i]+1] = c;
		pt->buf1[pp[i]-32] = c;
		pt->buf1[pp[i]-33] = c;
		pt->buf1[pp[i]-31] = c;
		pt->buf1[pp[i]+32] = c;
		pt->buf1[pp[i]+33] = c;
		pt->buf1[pp[i]+31] = c;
	}

	// Smooth, move up and fade
	for( i=30; i; i-- )
		for( j=30; j; j-- )
		{
			p = (i<<5)+j;
			res = ( pt->buf1[p+33] + pt->buf1[p+31] + pt->buf1[p-32] + pt->buf1[p+64] )>>2;

			if( res ) res--;
			pt->buf2[p] = res;
		}

	// Swap buffers
	tmp = pt->buf1;
	pt->buf1 = pt->buf2;
	pt->buf2 = tmp;
}


/*	--------------------------------------------------------------------------------
	Function		: ProcessProcTextures
	Purpose			: Call update function pointers for all procedural textures
	Parameters		: 
	Returns			: 
	Info			: 
*/
void ProcessProcTextures( )
{
	PROCTEXTURE *pt;

	for( pt=prcTexList; pt; pt=pt->next )
		if( pt->Update ) pt->Update( pt );
}


/*	--------------------------------------------------------------------------------
	Function		: FreeProcTextures
	Purpose			: Free procedural texture memory
	Parameters		: 
	Returns			: 
	Info			: 
*/
void FreeProcTextures( )
{
	PROCTEXTURE *pt;

	if( !prcTexList ) return;

	for( pt=prcTexList; pt; pt=prcTexList )
	{
		// Remove from list
		prcTexList = pt->next;

		// Free buffers
		if( pt->palette ) JallocFree( (UBYTE **)&pt->palette );
		if( pt->buf1 ) JallocFree( (UBYTE **)&pt->buf1 );
		if( pt->buf2 ) JallocFree( (UBYTE **)&pt->buf2 );

		// And free the object
		JallocFree( (UBYTE **)&pt );
	}
}


/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddProceduralTexture
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void CreateAndAddProceduralTexture( TEXTURE *tex, char *name )
{
#ifdef PC_VERSION
	TEXENTRY *tx = (TEXENTRY *)tex;
#else
	TEXTURE *tx = tex;
#endif

	PROCTEXTURE *pt = (PROCTEXTURE *)JallocAlloc( sizeof(PROCTEXTURE), YES, "prcTex" );

	// Add to procedural text list
	pt->next = prcTexList;
	prcTexList = pt;

	// Set proc texture members
	pt->tex = tex;
	pt->buf1 = (unsigned char *)JallocAlloc( 1024, YES, "ptdata" );  // sizeof(char)*32*32
	pt->buf2 = (unsigned char *)JallocAlloc( 1024, YES, "ptdata" );  // sizeof(char)*32*32
	pt->palette = (short *)JallocAlloc( 512, NO, "ptpal" );  // sizeof(short)*256
	memcpy( pt->palette, tx->data, 512 );	// Texture contains palette - real image is dynamically generated

	// Set update function and type depending on filename
	if( name[4]=='f' && name[5]=='i' && name[6]=='r' && name[7]=='e' )
		pt->Update = ProcessPTFire;
}


/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddRandomPoly
	Purpose			: Draw a quad with a procedural texture on it
	Parameters		: 
	Returns			: 
	Info			: 
*/
void CreateAndAddRandomPoly( TEXTURE *tex, VECTOR *pos, VECTOR *normal, float w, float h )
{
	POLYGON *p = (POLYGON *)JallocAlloc( sizeof(POLYGON), YES, "Poly" );
	
	SetVector( &p->plane.point, pos );
	SetVector( &p->plane.normal, normal );

	p->vT = (VECTOR *)JallocAlloc( sizeof(VECTOR)*4, NO, "V" );
	p->vT[0].v[X] = w;
	p->vT[0].v[Y] = 0;
	p->vT[0].v[Z] = -h;
	p->vT[1].v[X] = w;
	p->vT[1].v[Y] = 0;
	p->vT[1].v[Z] = h;
	p->vT[2].v[X] = -w;
	p->vT[2].v[Y] = 0;
	p->vT[2].v[Z] = h;
	p->vT[3].v[X] = -w;
	p->vT[3].v[Y] = 0;
	p->vT[3].v[Z] = -h;

	p->tex = tex;

	p->r = 255;
	p->g = 255;
	p->b = 255;
	p->a = 255;

	p->next = rpList;
	rpList = p;
}


/*	--------------------------------------------------------------------------------
	Function		: FreeRandomPolyList
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void FreeRandomPolyList( )
{
	POLYGON *p;

	if( !rpList ) return;

	for( p=rpList; p; p=rpList )
	{
		// Remove from list
		rpList = p->next;

		if( p->vT ) JallocFree( (UBYTE **)&p->vT );

		// And free the object
		JallocFree( (UBYTE **)&p );
	}
}