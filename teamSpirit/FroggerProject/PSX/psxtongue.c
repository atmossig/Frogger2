#include "tongue.h"
#include "map_draw.h"
#include "psxtongue.h"

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

void DrawTongueSegment ( POLY_G4 *vt, TextureType *tEntry )
{
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

	unsigned long *ot = currentDisplayPage->ot;

	int i;

	tfv = ( long* ) transformedVertices;
	tfd = ( long* ) transformedDepths;

	transformVertexListA ( vt, 1, tfv, tfd );

	for ( i = 0; i < 1; i++ )
	{
//		tfd [ i ] = tfd [ i ] >> 2;
	}
	// ENDFOR


#define si ((POLY_G4*)packet)
#define op ((POLY_G4 *)opcd)

	op = vt;

	packet = (PACKET *)currentDisplayPage->primPtr;

	depth = 1;
//	for ( i = 1; i != 0; i--, op++ )
//	{
//gte_ldsz4 ( GETD ( op->vert0 ), GETD ( op->vert1 ), GETD ( op->vert2 ), GETD ( op->vert3 ) );
// 	gte_avsz4();
//gte_stotz_cpu ( depth );

	/*	if ( depth > min_depth && depth < max_depth )
		{
			if( ( ( GETV ( op->vert0 ) & 0xff80ff00 ) + 0x00800100 ) &
					( ( GETV ( op->vert1 ) & 0xff80ff00 ) + 0x00800100 ) &
					( ( GETV ( op->vert2 ) & 0xff80ff00 ) + 0x00800100 ) &
					( ( GETV ( op->vert3 ) & 0xff80ff00 ) + 0x00800100 ) & 0x01000200 )
			{
				continue;
			}*/
			// ENDIF


			//gte_ldsxy3 ( GETV ( op->vert0 ), GETV ( op->vert1 ), GETV ( op->vert2 ) );

			addPrimLen ( ot + ( depth ), si, 12, t2 );

//			*(u_long *)  (&si->u0) = *(u_long *) (&op->u0);		// Texture coords
//			*(u_long *)  (&si->u1) = *(u_long *) (&op->u1);

			
			gte_stsxy3_gt4(si);
								
//			*(u_long *)  (&si->u2) = *(u_long *) (&op->u2);
//			*(u_long *)  (&si->u3) = *(u_long *) (&op->u3);


			*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
			*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
			*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
			*(u_long *)  (&si->r3) = *(u_long *) (&op->r3);

			si->x0 = op->x0;
			si->y0 = op->y0;

			si->x1 = op->x1;
			si->y1 = op->y1;

			si->x2 = op->x2;
			si->y2 = op->y2;

			si->x3 = op->x3;
			si->y3 = op->y3;

			setPolyG4(si);

			packet = ADD2POINTER ( packet, sizeof ( POLY_G4 ) );

	//	}
//	}
#undef op
#undef si

	currentDisplayPage->primPtr = (char *)packet;
}

void CalcTongueNodes ( POLY_G4 *vt, TONGUE *t, int i )
{
}

void DrawTongue ( TONGUE *t )
{
	unsigned long i=0;
	unsigned long index = ( t->progress * ( MAX_TONGUENODES - 1 ) >> 12 );
	TextureType *tEntry;

	POLY_G4 vT, vTPrev[2];

	tEntry = ( ( TextureType *) t->tex );

	if( !tEntry || index < 2 )
		return;

/*	vT.u0 = 1;
	vT.v0 = 1;
	vT.u1 = 0;
	vT.v1 = 1;
	vT.u2 = 0;
	vT.v2 = 0;
	vT.u3 = 1;
	vT.v3 = 0;*/

	while( i < index )
	{
		//********-[ First 2 points ]-*******
//		if( i && vTPrev.z && vTPrev[1].sz )
//			memcpy( vT, vTPrev, sizeof(VERTC)*2 );			// Previously transformed vertices
//		else
//			CalcTongueNodes( vT, t, i );
		// ENDELSEIF

		//********-[ Next 2 points ]-********
//		CalcTongueNodes( &vT[2], t, i+1 );
//		memcpy( vTPrev, &vT[2], sizeof(VERTC)*2 ); 			// Store first 2 vertices of the next segment

		//********-[ Draw the polys ]-********
//		if( vT[0].sz && vT[1].sz && vT[2].sz && vT[3].sz )
		{

			vT.x0 = t->segment[i].vx-5;
			vT.y0 = t->segment[i].vy-5;

			vT.x1 = t->segment[i].vx+5;
			vT.y1 = t->segment[i].vy-5;

			vT.x2 = t->segment[i].vx-5;
			vT.y2 = t->segment[i].vy+5;

			vT.x3 = t->segment[i].vx+5;
			vT.y3 = t->segment[i].vy+5;

			utilPrintf("Segment [ %d ] : %d : %d\n", i, t->segment[i].vx, t->segment[i].vy);

			vT.r0 = 128;
			vT.g0 = 128;
			vT.b0 = 128;

			vT.r1 = 128;
			vT.g1 = 128;
			vT.b1 = 128;

			vT.r2 = 128;
			vT.g2 = 128;
			vT.b2 = 128;

			vT.r3 = 128;
			vT.g3 = 128;
			vT.b3 = 128;

			DrawTongueSegment ( &vT, tEntry );


//		Clip3DPolygon( vT, tEntry );
//		Clip3DPolygon( &vT[1], tEntry );
		}
		// ENDIF

		i++;
	}
	// ENDWHILE
}



