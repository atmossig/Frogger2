#include "sonylibs.h"
#include <islutil.h>
#include "shell.h"
#include "types.h"
#include "main.h"
#include "map_draw.h"
#include "actor2.h"

#include "World_Eff.h"




#define gte_stotz_cpu(r)\
asm(\
  "mfc2 %0, $7;"\
  :\
  : "r" (r)\
)

#define addPrimLen(ot,p,len,tempvar) \
{\
	tempvar = *(u_long *)(ot); \
    (*(u_long *)(ot) = (u_long)(p)); \
	tempvar |= (len<<24); \
	*(unsigned long *)(p) = tempvar; \
}

#define WATERANIM_1  ( ( u )  ) | ( ( v + ( ( 4096 ) >> 11 ) ) << 8 )
#define WATERANIM_2  ( ( u )  ) | ( ( v + ( ( 4096 ) >> 11 ) ) << 8 )

#define WATER_TRANS_CODE (((ULONG)2)<<24)
#define WATER_TRANS_CLUT (0x80000000)

#define MIN_MAP_DEPTH (10)

#define MAX_MAP_DEPTH (1000<<3)




void DrawWater ( FMA_MESH_HEADER *mesh, int flags )
{
	static SVECTOR *jiggledVerts = NULL;

	register u_long t1 asm("$16");
	register u_long t2 asm("$20");

	register PACKET * packet asm("$17");
	register char *opcd asm("$18");

	long *tfv;
	long *tfd;

	#define GETX(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->x )
	#define GETY(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->y )
	#define GETV(n)(  *(u_long *)( (int)(tfv) +(n) ) )
	#define GETD(n)(  *(u_long *)( (int)(tfd) +(n) ) )

	register long depth asm("$16");

	unsigned long *ot = currentDisplayPage->ot+mesh->extra_depth;

	int i;

	int min_depth = MIN_MAP_DEPTH + mesh->extra_depth;
	int max_depth = MAX_MAP_DEPTH + mesh->extra_depth;

	tfv = (long*)transformedVertices;
	tfd = (long*)transformedDepths;

	if ( max_depth > 1024 - mesh->extra_depth )
		max_depth = 1024 - mesh->extra_depth;


	if ( !jiggledVerts )
		jiggledVerts = MALLOC0 ( sizeof ( SVECTOR ) * 700 );

	for ( i = 0; i < mesh->n_verts; i++ )
	{
		jiggledVerts[i].vx = mesh->verts[i].vx + ( rsin ( ( frame << 6 ) + ( mesh->verts[i].vx & ( mesh->verts[i].vz ) ) ) >> 7 );
		jiggledVerts[i].vy = mesh->verts[i].vy + ( rsin ( ( frame << 5 ) + ( mesh->verts[i].vx | ( mesh->verts[i].vz ) ) ) >> 8 );
		jiggledVerts[i].vz = mesh->verts[i].vz + ( rsin ( ( frame << 6 ) + ( mesh->verts[i].vx ^ ( mesh->verts[i].vz ) ) ) >> 7 );
	}
	
	//transformVertexListA(mesh->verts, mesh->n_verts, tfv, tfd);
	transformVertexListA ( jiggledVerts, mesh->n_verts, tfv, tfd );

	for ( i = 0; i < mesh->n_verts; i++ )
	{
		tfd [ i ] = tfd [ i ] >> 2;
	}
	// ENDFOR


	packet = (PACKET *)currentDisplayPage->primPtr;

#define si ((POLY_GT4*)packet)
#define op ((FMA_GT4 *)opcd)

	op = mesh->gt4s;

	for ( i = mesh->n_gt4s; i != 0; i--, op++ )
	{
		char u,v;

		gte_ldsz4 ( GETD ( op->vert0 ), GETD ( op->vert1 ), GETD ( op->vert2 ), GETD ( op->vert3 ) );
   	gte_avsz4();
		gte_stotz_cpu ( depth );

		if ( depth > min_depth && depth < max_depth )
		{
			if( ( ( GETV ( op->vert0 ) & 0xff80ff00 ) + 0x00800100 ) &
					( ( GETV ( op->vert1 ) & 0xff80ff00 ) + 0x00800100 ) &
					( ( GETV ( op->vert2 ) & 0xff80ff00 ) + 0x00800100 ) &
					( ( GETV ( op->vert3 ) & 0xff80ff00 ) + 0x00800100 ) & 0x01000200 )
			{
				continue;
			}
			// ENDIF

			gte_ldsxy3 ( GETV ( op->vert0 ), GETV ( op->vert1 ), GETV ( op->vert2 ) );

			addPrimLen ( ot + ( depth ), si, 12, t2 );

			if ( flags & ACTOR_SLIDYTEX )
				u = op->u0 + ( ( frame / 2 ) % 32 );
			else
				u = op->u0;
			// ENDELSEIF

			v = op->v0;

			t1 = WATERANIM_1 | ( op->clut << 16 ) | WATER_TRANS_CLUT;

			if ( flags & ACTOR_SLIDYTEX )
				u = op->u1 + ( ( frame / 2 ) % 32 );
			else
				u = op->u1;
			// ENDELSEIF
			v = op->v1;

			t2 = WATERANIM_1 | ( op->tpage << 16 );

			*(u_long *)  (&si->u0) = t1;
			*(u_long *)  (&si->u1) = t2;

			gte_stsxy3_gt4 ( si );	// The first 3 x's & y's are already in the gte, so we may as well use 'em

			if ( flags & ACTOR_SLIDYTEX )
				u = op->u2 + ( ( frame / 2 ) % 32 );
			else
				u = op->u2;
			// ENDELSEIF
			v = op->v2;

			t1 = WATERANIM_2;

			if ( flags & ACTOR_SLIDYTEX )
				u = op->u3 + ( ( frame / 2 ) % 32 );
			else
				u = op->u3;
			// ENDELSEIF
			v = op->v3;

			t2 = WATERANIM_2;

			*(u_long *)  (&si->u2) = t1;
			*(u_long *)  (&si->u3) = t2;
			*(u_long *)  (&si->x3) = *(u_long *) ( &GETV ( op->vert3 ) );

			t1 = ( *( u_long *) ( &op->r0 ) ) | WATER_TRANS_CODE;
			t2 = *(u_long *) (&op->r1);

			*(u_long *)  (&si->r0) = t1;
			*(u_long *)  (&si->r1) = t2;

			t1 = *(u_long *) (&op->r2);
			t2 = *(u_long *) (&op->r3);

			*(u_long *)  (&si->r2) = t1;
			*(u_long *)  (&si->r3) = t2;


			packet = ADD2POINTER ( packet, sizeof ( POLY_GT4 ) );
		}
	}

#undef si
#undef op

	currentDisplayPage->primPtr = (char *)packet;

}
