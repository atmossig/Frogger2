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
	unsigned long i,j,t;
	unsigned char *tmp;
	short p;

	// Copy resultant buffer into texture
#ifdef PC_VERSION
	PTSurfaceBlit( ((TEXENTRY *)pt->tex)->surf, pt->buf1, pt->palette );
#else
	TEXTURE *tx = pt->tex;

	// N64 surface blit
	memcpy(pt->tex->data,pt->buf1,928);
#endif

	for( i=992; i<1024; i++ )
		pt->buf1[i] = 0;

	for( i=0; i<9; i++ )
	{
		p = Random(30)-15;
		pt->buf1[1008+p] = 0xff;
		pt->buf1[1008+p+1] = 0xff;
		pt->buf1[1008+p-1] = 0xff;
		pt->buf1[1008+p-32] = 0xff;
		pt->buf1[1008+p-31] = 0xff;
		pt->buf1[1008+p-33] = 0xff;
	}
	
	// Smooth, move up and fade
	for( i=30; i; i-- )
		for( j=30; j; j-- )
		{
			p = (i<<5)+j;
			t = ( pt->buf1[p+31]+ pt->buf1[p+33] + pt->buf1[p+64] );
			pt->buf2[p]=((t+pt->buf1[p-64])+(t+pt->buf1[p-32]))>>3;
		}

	// Swap buffers
	tmp = pt->buf1;
	pt->buf1 = pt->buf2;
	pt->buf2 = tmp;
}


/*	--------------------------------------------------------------------------------
	Function		: ProcessPTForcefield
	Purpose			: Procedural forcefield effect
	Parameters		: 
	Returns			: 
	Info			: 
*/
void ProcessPTForcefield( PROCTEXTURE *pt )
{
	unsigned long i,j;
	unsigned char *tmp;
	unsigned long t;
	unsigned short res;
	short p;

	// Copy resultant buffer into texture
#ifdef PC_VERSION
	PTSurfaceBlit( ((TEXENTRY *)pt->tex)->surf, pt->buf1, pt->palette );
#else
	TEXTURE *tx = pt->tex;

	// N64 surface blit
	memcpy(pt->tex->data,pt->buf1,928);
#endif

	p = ((Random(30)+1)*32) + Random(30)+1;
	pt->buf1[p]-=30;
	pt->buf1[p+1]-=15;
	pt->buf1[p-1]-=15;
	pt->buf1[p-32]-=15;
	pt->buf1[p+32]-=15;

	for (i=0; i<1024; i++)
	{
		res = (((unsigned long)((pt->buf1[(i)&1023] + pt->buf1[(i-1)&1023] + pt->buf1[(i-31)&1023] + pt->buf1[(i+32)&1023]-10)))) >>2;
		pt->buf2[i] = (unsigned char)res;
	}

	// Swap buffers
	tmp = pt->buf1;
	pt->buf1 = pt->buf2;
	pt->buf2 = tmp;
}


/*	--------------------------------------------------------------------------------
	Function		: ProcessPTWater
	Purpose			: Procedural water drops effect
	Parameters		: 
	Returns			: 
	Info			: 
*/
void ProcessPTWaterDrops( PROCTEXTURE *pt )
{
	unsigned long i,j;
	unsigned char *tmp;
	short p, res;

	// Copy resultant buffer into texture
#ifdef PC_VERSION
	PTSurfaceBlit( ((TEXENTRY *)pt->tex)->surf, pt->buf1, pt->palette );
#else
	TEXTURE *tx = pt->tex;

	// N64 surface blit
	memcpy(pt->tex->data,pt->buf1,928);
#endif

	if( Random(2) )
		pt->buf1[((Random(30)+1)*32) + Random(30)+1] += 200;

	for( i=30; i; i-- )
		for( j=30; j; j-- )
		{
			p = (i<<5)+j;
			res = (pt->buf1[p+32] + pt->buf1[p-32] + pt->buf1[p+1] + pt->buf1[p-1] + pt->buf1[p+33] + pt->buf1[p+31] + pt->buf1[p-33] + pt->buf1[p-31] )>>2;
			res -= pt->buf2[p] + (pt->buf2[p]>>2);
			pt->buf2[p] = max(res,0);
		}

	// Swap buffers
	tmp = pt->buf1;
	pt->buf1 = pt->buf2;
	pt->buf2 = tmp;
}


/*	--------------------------------------------------------------------------------
	Function		: ProcessPTWater
	Purpose			: Procedural water random rings effect
	Parameters		: 
	Returns			: 
	Info			: 
*/
void ProcessPTWaterBubbler( PROCTEXTURE *pt )
{
	unsigned long i,j;
	unsigned char *tmp;
	short p, res;
	static unsigned long x=16, y=16;

	// Copy resultant buffer into texture
#ifdef PC_VERSION
	PTSurfaceBlit( ((TEXENTRY *)pt->tex)->surf, pt->buf1, pt->palette );
#else
	TEXTURE *tx = pt->tex;

	// N64 surface blit
	memcpy(pt->tex->data,pt->buf1,928);
#endif

	x += Random(3)-1;
	y += Random(3)-1;

	if( x < 10 ) x = 10;
	else if( x > 20 ) x = 20;
	if( y < 10 ) y = 10;
	else if( y > 20 ) y = 20;

	pt->buf1[(y*32) + x] += 200;

	for( i=30; i; i-- )
		for( j=30; j; j-- )
		{
			p = (i<<5)+j;
			res = (pt->buf1[p+32] + pt->buf1[p-32] + pt->buf1[p+1] + pt->buf1[p-1] + pt->buf1[p+33] + pt->buf1[p+31] + pt->buf1[p-33] + pt->buf1[p-31] )>>2;
			res -= pt->buf2[p] + (pt->buf2[p]>>2);
			pt->buf2[p] = max(res,0);
		}

	// Swap buffers
	tmp = pt->buf1;
	pt->buf1 = pt->buf2;
	pt->buf2 = tmp;
}


/*	--------------------------------------------------------------------------------
	Function		: ProcessPTWater
	Purpose			: Procedural water random rings effect
	Parameters		: 
	Returns			: 
	Info			: 
*/
void ProcessPTWaterTrail( PROCTEXTURE *pt )
{
	unsigned long i,j;
	unsigned char *tmp;
	short p, res;
	static short x=16, y=16, xv=2, yv=2;

	// Copy resultant buffer into texture
#ifdef PC_VERSION
	PTSurfaceBlit( ((TEXENTRY *)pt->tex)->surf, pt->buf1, pt->palette );
#else
	TEXTURE *tx = pt->tex;

	// N64 surface blit
	memcpy(pt->tex->data,pt->buf1,928);
#endif

	if( Random(10) > 8 )
	{
		xv += Random(3)-1;
		yv += Random(3)-1;

		if( xv > 2 ) xv = 2;
		else if( xv < -2 ) xv = -2;
		if( yv > 2 ) yv = 2;
		else if( yv < -2 ) yv = -2;
	}

	x += xv;
	y += yv;

	if( x < 2 || x > 30 )
	{
		xv *= -1;
		x += xv;
	}
	if( y < 2 || y > 30 )
	{
		yv *= -1;
		y += yv;
	}

	pt->buf1[(y*32) + x] += 200;

	for( i=30; i; i-- )
		for( j=30; j; j-- )
		{
			p = (i<<5)+j;
			res = (pt->buf1[p+32] + pt->buf1[p-32] + pt->buf1[p+1] + pt->buf1[p-1] + pt->buf1[p+33] + pt->buf1[p+31] + pt->buf1[p-33] + pt->buf1[p-31] )>>2;
			res -= pt->buf2[p] + (pt->buf2[p]>>2);
			pt->buf2[p] = max(res,0);
		}

	// Swap buffers
	tmp = pt->buf1;
	pt->buf1 = pt->buf2;
	pt->buf2 = tmp;
}


/*	--------------------------------------------------------------------------------
	Function		: ProcessPTWaterRipples
	Purpose			: Procedural water ripple effect
	Parameters		: 
	Returns			: 
	Info			: 
*/
void ProcessPTWaterRipples( PROCTEXTURE *pt )
{
	unsigned long i,j;
	unsigned char *tmp;
	short p, res;

	// Copy resultant buffer into texture
#ifdef PC_VERSION
	PTSurfaceBlit( ((TEXENTRY *)pt->tex)->surf, pt->buf1, pt->palette );
#else
	TEXTURE *tx = pt->tex;

	// N64 surface blit
	memcpy(pt->tex->data,pt->buf1,928);
#endif

	pt->buf1[(Random(30)+1)+960] = 255;

	for( i=30; i; i-- )
		for( j=30; j; j-- )
		{
			p = (i<<5)+j;
			res = (pt->buf1[p+32] + pt->buf1[p-32] + pt->buf1[p+1] + pt->buf1[p-1] + pt->buf1[p+33] + pt->buf1[p+31] + pt->buf1[p-33] + pt->buf1[p-31] )>>2;
			res -= pt->buf2[p] + (pt->buf2[p]>>3);
			pt->buf2[p] = max(res,0);
		}

	// Swap buffers
	tmp = pt->buf1;
	pt->buf1 = pt->buf2;
	pt->buf2 = tmp;
}


/*	--------------------------------------------------------------------------------
	Function		: ProcessPTWaterRipplesBM
	Purpose			: Apply fake lighting to ripples and modify a base texture
	Parameters		: 
	Returns			: 
	Info			: 
*/
void ProcessPTWaterRipplesBM( PROCTEXTURE *pt )
{
	unsigned long i,j;
	unsigned char *tmp;
	unsigned short ox, oy, ix, iy;
	short p, res;

#ifdef PC_VERSION
	PTSurfaceBlit( ((TEXENTRY *)pt->tex)->surf, pt->bump, pt->palette );
#else
	TEXTURE *tx = pt->tex;

	// N64 surface blit
	memcpy(pt->tex->data,pt->buf1,928);
#endif

	pt->buf1[(Random(30)+1)+960] = 255;

	for( i=30; i; i-- )
		for( j=30; j; j-- )
		{
			// Calculate ripples as usual
			p = (i<<5)+j;
			res = (pt->buf1[p+32] + pt->buf1[p-32] + pt->buf1[p+1] + pt->buf1[p-1] + pt->buf1[p+33] + pt->buf1[p+31] + pt->buf1[p-33] + pt->buf1[p-31] )>>2;
			res -= pt->buf2[p] + (pt->buf2[p]>>3);
			pt->buf2[p] = max(res,0);

			// Pseudo normal of pixel
			ox = pt->buf2[p] - pt->buf2[p+32];
			oy = pt->buf2[p] - pt->buf2[p+1];

			// Calculate colour by indexing into base texture and modifying by bump amount
			res = (128-ox)&0xff;
			ox = ((ox<<3)+(ox<<1)+ox)>>5; // divide by three
			oy >>= 3;
			ix = ox+j;
			iy = oy+i;
			pt->bump[p] = ((pt->buf1[(iy<<5)+ix]<<8)+res)>>8;
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
		if( pt->Update && (actFrameCount > pt->timer) )
		{
			pt->timer = actFrameCount+1; // Max 60fps - slow machines will just have to slow down
			pt->Update( pt );
		}
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
	unsigned long i;
	unsigned long rVand,gVand,bVand,rVshr,gVshr,bVshr;
	unsigned short newCol,nR,nG,nB,nA;
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
	pt->palette = (unsigned short *)JallocAlloc( 512, NO, "ptpal" );  // sizeof(short)*256

	// Convert palette to 4444 format
	if (
#ifdef PC_VERSION
		a565Card
#else
		FALSE		// Eat my hack brother trucker!
#endif
		)
	{
		rVand = 0x1f;
		gVand = 0x3f;
		bVand = 0x1f;
		rVshr = 11;
		gVshr = 5;
		bVshr = 0;
	}
	else
	{
		rVand = 0x1f;
		gVand = 0x1f;
		bVand = 0x1f;
		rVshr = 10;
		gVshr = 5;
		bVshr = 0;
	}

	for( i=0; i<256; i++ )
	{
		nR = ((unsigned short *)tx->data)[i] >> rVshr;
		nG = ((unsigned short *)tx->data)[i] >> gVshr;
		nB = ((unsigned short *)tx->data)[i] >> bVshr;
		nA = ((unsigned short *)tx->data)[i+256] >> bVshr;
		
		nR &= rVand;
		nG &= gVand;
		nB &= bVand;
		nA &= bVand;
		
		nR = (nR * 0x0f )/rVand;
		nG = (nG * 0x0f )/gVand;
		nB = (nB * 0x0f )/bVand;
		nA = (nA * 0x0f )/bVand;

		newCol = ((nA << 12) | (nR<<8) | (nG<<4) | (nB));
		
		((unsigned short *)pt->palette)[i] = newCol;
	}

	// Set update function and type depending on filename
	if( name[4]=='f' && name[5]=='i' && name[6]=='r' && name[7]=='e' )
		pt->Update = ProcessPTFire;
	else if( name[4]=='f' && name[5]=='f' && name[6]=='l' && name[7]=='d' )
		pt->Update = ProcessPTForcefield;
	else if( name[4]=='w' && name[5]=='a' && name[6]=='t' && name[7]=='r' )
	{
		// Allocate bump map buffer
		pt->bump = (unsigned char *)JallocAlloc( 1024, YES, "ptdata" );  // sizeof(char)*32*32

		if( name[8]=='1' )
			pt->Update = ProcessPTWaterRipplesBM;
		else if( name[8]=='2' )
			pt->Update = ProcessPTWaterDrops;
		else if( name[8]=='3' )
			pt->Update = ProcessPTWaterBubbler;
		else if( name[8]=='4' )
			pt->Update = ProcessPTWaterTrail;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddRandomPoly
	Purpose			: Draw a quad with a procedural texture on it
	Parameters		: 
	Returns			: 
	Info			: 
*/
POLYGON *CreateAndAddRandomPoly( TEXTURE *tex, VECTOR *pos, VECTOR *normal, float w, float h )
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

	p->u = 0;
	p->v = 0;
	p->u1 = 1;
	p->v1 = 1;

	p->angle = 0;

	p->next = rpList;
	rpList = p;

	return p;
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