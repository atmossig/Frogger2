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

void DrawTongueSegment ( SVECTOR *vt, TextureType *tEntry )
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

	transformVertexListA ( vt, 4, tfv, tfd );

	for ( i = 0; i < 4; i++ )
	{
		tfd [ i ] = tfd [ i ] >> 2;
	}
	// ENDFOR


#define si ((POLY_G4*)packet)
#define op ((FMA_GT4 *)opcd)

	op = tfv;

	packet = (PACKET *)currentDisplayPage->primPtr;

	for ( i = 4; i != 0; i--, op++ )
	{
		op->vert0 = 0;
		op->vert1 = 1;
		op->vert2 = 2;
		op->vert3 = 3;

		gte_ldsz4 ( GETD ( op->vert0 ), GETD ( op->vert1 ), GETD ( op->vert2 ), GETD ( op->vert3 ) );
 		gte_avsz4();
		gte_stotz_cpu ( depth );

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


			gte_ldsxy3 ( GETV ( op->vert0 ), GETV ( op->vert1 ), GETV ( op->vert2 ) );

			addPrimLen ( ot + ( depth ), si, 12, t2 );

//			*(u_long *)  (&si->u0) = *(u_long *) (&op->u0);		// Texture coords
//			*(u_long *)  (&si->u1) = *(u_long *) (&op->u1);

			
			gte_stsxy3_g4(si);
								
//			*(u_long *)  (&si->u2) = *(u_long *) (&op->u2);
//			*(u_long *)  (&si->u3) = *(u_long *) (&op->u3);


			*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
			*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
			*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
			*(u_long *)  (&si->r3) = *(u_long *) (&op->r3);

/*			si->x0 = op->x0;
			si->y0 = op->y0;

			si->x1 = op->x1;
			si->y1 = op->y1;

			si->x2 = op->x2;
			si->y2 = op->y2;

			si->x3 = op->x3;
			si->y3 = op->y3;*/

			setPolyG4(si);

			packet = ADD2POINTER ( packet, sizeof ( POLY_G4 ) );

		}
//	}
#undef op
#undef si

	currentDisplayPage->primPtr = (char *)packet;
}

void CalcTongueNodes ( SVECTOR *vt, TONGUE *t, int i )
{
}

void DrawTongue ( TONGUE *t )
{
	unsigned long i=0;
	unsigned long index = ( t->progress * ( MAX_TONGUENODES - 1 ) >> 12 );
	TextureType *tEntry;

	SVECTOR vT[4], vTPrev[2];

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

	gte_SetRotMatrix(&GsWSMATRIX);
	gte_SetTransMatrix(&GsWSMATRIX);

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

/*			vT.vert0 = ( t->segment[i].vx >> 12 ) - 5;
			vT.vert0 += ( (t->segment[i].vy<<4) - (5<<16) ) &0xffff0000;

			vT.vert1 = (t->segment[i].vx>>12) + 5;
			vT.vert1 += ( (t->segment[i].vy<<4) - (5<<16) ) &0xffff0000;

			vT.vert2 = ( t->segment[i].vx >> 12 ) - 5;
			vT.vert2 += ( (t->segment[i].vy<<4) - (5<<16) ) &0xffff0000;

			vT.vert3 = ( t->segment[i].vx >> 12 ) + 5;
			vT.vert3 += ( (t->segment[i].vy<<4) - (5<<16) ) &0xffff0000;*/

			vT[0].vx = t->segment[i].vx-5;
			vT[0].vy = t->segment[i].vy-5;
			vT[0].vz = t->segment[i].vz;

			vT[1].vx = t->segment[i].vx+5;
			vT[1].vy = t->segment[i].vy-5;
			vT[1].vz = t->segment[i].vz;

			vT[2].vx = t->segment[i].vx-5;
			vT[2].vy = t->segment[i].vy+5;
			vT[2].vz = t->segment[i].vz;

			vT[3].vx = t->segment[i].vx+5;
			vT[3].vy = t->segment[i].vy+5;
			vT[3].vz = t->segment[i].vz;

//			utilPrintf("Segment [ %d ] : %d : %d\n", i, t->segment[i].vx, t->segment[i].vy);

/*			vT.r0 = 255;
			vT.g0 = 0;
			vT.b0 = 0;

			vT.r1 = 255;
			vT.g1 = 0;
			vT.b1 = 0;

			vT.r2 = 255;
			vT.g2 = 0;
			vT.b2 = 0;

			vT.r3 = 255;
			vT.g3 = 0;
			vT.b3 = 0;*/

			DrawTongueSegment ( (SVECTOR*)&vT, tEntry );


//		Clip3DPolygon( vT, tEntry );
//		Clip3DPolygon( &vT[1], tEntry );
		}
		// ENDIF

		i++;
	}
	// ENDWHILE
}



