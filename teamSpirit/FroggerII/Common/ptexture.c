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

/*	--------------------------------------------------------------------------------
	Function		: ProcessPTFire
	Purpose			: Procedural fire
	Parameters		: 
	Returns			: 
	Info			: 
*/
void ProcessPTFire( PROCTEXTURE *pt )
{
	long i = 1024,j,k;
	char *tmp;
#ifdef PC_VERSION
	TEXENTRY *tx = (TEXENTRY *)pt->tex;
#else
	TEXTURE *tx = pt->tex;
#endif

	// Copy resultant buffer into texture
	while( i-- ) tx->data[i] = pt->palette[pt->buf1[i]];

	// Set the bottom line to be white-ish
	for( i=992; i<1024; i++ )
		pt->buf1[i] = Random(31)+224;

	// Make a few blocks of nine white things
	j = Random(3);
	for( i=0; i<j; i++ )
	{
		k=Random(32)+224;
		pt->buf1[k-32] = k;
		pt->buf1[k-31] = k;
		pt->buf1[k-33] = k;
//		pt->buf1[k-64] = k;
//		pt->buf1[k-63] = k;
//		pt->buf1[k-65] = k;
	}

	// Smooth, move up and fade
	for( i=990; i>1; i-- )
		pt->buf2[i] = ((pt->buf1[i+33] + pt->buf1[i+31] + pt->buf1[i-33] + pt->buf1[i-31])>>2)-1;

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
