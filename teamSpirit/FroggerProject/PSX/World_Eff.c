#define DEBUG_INFO1

#define TRANS           ( 1 << 2 )
#define FLOW            ( 1 << 3 )
#define SHIFT_FORWARD   ( 1 << 4 )
#define SHIFT_BACKWARD  ( 1 << 5 )
#define MODGY           ( 1 << 6 )
#define ADDATIVE        ( 1 << 7 )
#define SUBTRACTIVE     ( 1 << 8 )
#define JIGGLE          ( 1 << 9 )
#define USLIDDING       ( 1 << 10 )
#define VSLIDDING       ( 1 << 11 )
#define PLUSSLIDDING    ( 1 << 12 )
#define MINSLIDDING     ( 1 << 13 )



#include "sonylibs.h"
#include <islutil.h>
#include <isltex.h>
#include "shell.h"
#include "types.h"
#include "main.h"
#include "map_draw.h"
#include "actor2.h"
#include "maths.h"
#include "layout.h"

#include "World_Eff.h"



#define VRAM_STARTX			512
#define VRAM_PAGECOLS		8
#define VRAM_PAGEROWS		2
#define VRAM_PAGES			16
#define VRAM_PAGEW			32
#define VRAM_PAGEH			32
#define VRAM_SETX(X)		(X)
#define VRAM_SETY(Y)		((Y)*VRAM_PAGEW)
#define VRAM_SETXY(X,Y)		((X)+((Y)*VRAM_PAGEW))
#define VRAM_SETPAGE(P)		((P)<<16)
#define VRAM_GETX(HND)		((HND) & (VRAM_PAGEW-1))
#define VRAM_GETY(HND)		(((HND)/VRAM_PAGEW) & (VRAM_PAGEW-1))
#define VRAM_GETXY(HND)		((HND) & 0xffff)
#define VRAM_GETPAGE(HND)	((HND)>>16)
#define VRAM_CALCVRAMX(HND)	(512+((VRAM_GETPAGE(HND)%(VRAM_PAGECOLS))*64)+(VRAM_GETX(HND)*2))
#define VRAM_CALCVRAMY(HND)	(((VRAM_GETPAGE(HND)/(VRAM_PAGECOLS))*256)+(VRAM_GETY(HND)*8))


SCENICOBJLIST scenicObjList;

extern int polyCount;

extern USHORT EXPLORE_black_CLUT;

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

//#define WATERANIM_1 (u+((rcos(frame<<6))>>11))|((v+((rsin(frame<<6))>>11))<<8)
//#define WATERANIM_2 (u+((rsin(frame<<6))>>11))|((v+((rcos(frame<<6))>>11))<<8)

#define WATERANIM_1A (u+((rcos(frame<<6)+4096)>>11))|((v+((rsin(frame<<6)+4096)>>11))<<8)
#define WATERANIM_2B (u+((rsin(frame<<6)+4096)>>11))|((v+((rcos(frame<<6)+4096)>>11))<<8)

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

	
	if ( mesh->flags & JIGGLE )
	{
		for ( i = 0; i < mesh->n_verts; i++ )
		{
			jiggledVerts[i].vx = mesh->verts[i].vx + ( rsin ( ( frame << 6 ) + ( mesh->verts[i].vx & ( mesh->verts[i].vz ) ) ) >> 7 );
			jiggledVerts[i].vy = mesh->verts[i].vy + ( rsin ( ( frame << 5 ) + ( mesh->verts[i].vx | ( mesh->verts[i].vz ) ) ) >> 8 );
			jiggledVerts[i].vz = mesh->verts[i].vz + ( rsin ( ( frame << 6 ) + ( mesh->verts[i].vx ^ ( mesh->verts[i].vz ) ) ) >> 7 );
		}
		// ENDFOR
		
		transformVertexListA ( jiggledVerts, mesh->n_verts, tfv, tfd );
	}
	else
	{
		transformVertexListA(mesh->verts, mesh->n_verts, tfv, tfd);
	}
	// ENDELSEIF

	for ( i = 0; i < mesh->n_verts; i++ )
	{
		tfd [ i ] = tfd [ i ] >> 2;
	}
	// ENDFOR


	packet = (PACKET *)currentDisplayPage->primPtr;

#define si ((POLY_GT4*)packet)
#define op ((FMA_GT4 *)opcd)

	polyCount += mesh->n_gt4s;

	op = mesh->gt4s;

	for ( i = mesh->n_gt4s; i != 0; i--, op++ )
	{
		char u,v;

		gte_ldsz4 ( GETD ( op->vert0 ), GETD ( op->vert1 ), GETD ( op->vert2 ), GETD ( op->vert3 ) );
   	gte_avsz4();
		gte_stotz_cpu ( depth );

//		depth = depth >> 2;

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

			if ( mesh->flags & SHIFT_FORWARD )
				addPrimLen ( ot + ( depth - mesh->shift ), si, 12, t2 )
			else if ( mesh->flags & SHIFT_BACKWARD )
				addPrimLen ( ot + ( depth + mesh->shift ), si, 12, t2 )
			else
				addPrimLen ( ot + ( depth ), si, 12, t2 );
			// ENDELSEIF

			if ( mesh->flags & FLOW )
			{
				if ( mesh->flags & USLIDDING )
				{
					if ( mesh->flags & PLUSSLIDDING )
						u = op->u0 + ( ( frame / 2 ) % 32 );

					if ( mesh->flags & MINSLIDDING )
						u = op->u0 - ( ( frame / 2 ) % 32 );
					// ENDELSEIF
					v = op->v0;

				}
				// ENDIF

				if ( mesh->flags & VSLIDDING )
				{
					if ( mesh->flags & PLUSSLIDDING )
						v = op->v0 + ( ( frame / 2 ) % 32 );

					if ( mesh->flags & MINSLIDDING )
						v = op->v0 - ( ( frame / 2 ) % 32 );

					u = op->u0;
				}
				// ENDIF

			}
			else
			{
				u = op->u0;
				v = op->v0;
			}
			// ENDELSEIF


			if ( mesh->flags & MODGY )
				t1 = WATERANIM_1A;
			else
				t1 = WATERANIM_1;


			if ( mesh->flags & TRANS )
				t1 |= ( op->clut << 16 ) | WATER_TRANS_CLUT;
			else
				t1 |= ( op->clut << 16 ) | 0;

			if ( mesh->flags & FLOW )
			{
				if ( mesh->flags & USLIDDING )
				{
					if ( mesh->flags & PLUSSLIDDING )
						u = op->u1 + ( ( frame / 2 ) % 32 );

					if ( mesh->flags & MINSLIDDING )
						u = op->u1 - ( ( frame / 2 ) % 32 );
					// ENDELSEIF
					v = op->v1;
				}
				// ENDIF

				if ( mesh->flags & VSLIDDING )
				{
					if ( mesh->flags & PLUSSLIDDING )
						v = op->v1 + ( ( frame / 2 ) % 32 );

					if ( mesh->flags & MINSLIDDING )
						v = op->v1 - ( ( frame / 2 ) % 32 );
					u = op->u1;
				}
				// ENDIF

			}
			else
			{
				u = op->u1;
				v = op->v1;
			}
			// ENDELSEIF

	//		t2 = WATERANIM_1 | ( op->tpage << 16 );


			if ( mesh->flags & MODGY )
			{
				t2 = WATERANIM_2B | ( op->tpage << 16 );
			}
			else
			{
				t2 = WATERANIM_1 | ( op->tpage << 16 );
			}
			// ENDELSEIF


			*(u_long *)  (&si->u0) = t1;
			*(u_long *)  (&si->u1) = t2;

			gte_stsxy3_gt4 ( si );	// The first 3 x's & y's are already in the gte, so we may as well use 'em

			if ( mesh->flags & FLOW )
			{
				if ( mesh->flags & USLIDDING )
				{
					if ( mesh->flags & PLUSSLIDDING )
						u = op->u2 + ( ( frame / 2 ) % 32 );

					if ( mesh->flags & MINSLIDDING )
						u = op->u2 - ( ( frame / 2 ) % 32 );
					v = op->v2;
				}
				// ENDIF

				if ( mesh->flags & VSLIDDING )
				{
					if ( mesh->flags & PLUSSLIDDING )
						v = op->v2 + ( ( frame / 2 ) % 32 );

					if ( mesh->flags & MINSLIDDING )
						v = op->v2 - ( ( frame / 2 ) % 32 );
					u = op->u2;
				}
				// ENDIF

			}
			else
			{
				u = op->u2;
				v = op->v2;
			}
			// ENDELSEIF

			//t1 = WATERANIM_2;

			if ( mesh->flags & MODGY )
				t1 = WATERANIM_1A;
			else
				t1 = WATERANIM_2;

			if ( mesh->flags & FLOW )
			{
				if ( mesh->flags & USLIDDING )
				{
					if ( mesh->flags & PLUSSLIDDING )
						u = op->u3 + ( ( frame / 2 ) % 32 );

					if ( mesh->flags & MINSLIDDING )
						u = op->u3 - ( ( frame / 2 ) % 32 );
					v = op->v3;
				}
				// ENDIF

				if ( mesh->flags & VSLIDDING )
				{
					if ( mesh->flags & PLUSSLIDDING )
						v = op->v3 + ( ( frame / 2 ) % 32 );

					if ( mesh->flags & MINSLIDDING )
						v = op->v3 - ( ( frame / 2 ) % 32 );
					u = op->u3;
				}
				// ENDIF

			}
			else
			{
				u = op->u3;
				v = op->v3;
			}
			// ENDELSEIF

			if ( mesh->flags & MODGY )
			{
				t2 = WATERANIM_2B;
			}
			else
			{
				t2 = WATERANIM_2;
			}
			// ENDELSEIF

			//t2 = WATERANIM_2;

			*(u_long *)  (&si->u2) = t1;
			*(u_long *)  (&si->u3) = t2;

			*(u_long *)  (&si->x3) = *(u_long *) ( &GETV ( op->vert3 ) );

			if ( mesh->flags & TRANS )
				t1 = ( *( u_long *) ( &op->r0 ) ) | WATER_TRANS_CODE;
			else
				t1 = ( *( u_long *) ( &op->r0 ) ) | 0;
			// ENDIF

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

//	if ( jiggledVerts )
//		FREE ( jiggledVerts );
}



/*
void CreateAndAddScenicObject ( char *name, short posx, short posy, short posz, int newFlags )
{
	SCENICOBJ *newItem;

	newItem = ( SCENICOBJ* ) MALLOC0 ( sizeof ( SCENICOBJ ) );

	AddScenicObj ( newItem );

	newItem->position.vx = posx;
	newItem->position.vy = posy;
	newItem->position.vz = posz;

	newItem->flags = newFlags;
	
	utilPrintf("Creating Scenic Object : %s\n", name);
//	utilUpperStr ( name );

	newItem->fmaObj = ( void * ) BFF_FindObject ( BFF_FMA_WORLD_ID, utilStr2CRC ( name ) );

//	utilPrintf("Creating Scenic Object : %lu\n", utilStr2CRC ( name ));

	if ( newItem->fmaObj )
	{
		SetUpWaterMesh ( newItem->fmaObj );
	}
	else
	{
		utilPrintf("Could Not Create Scenic Object.\n");
	}
	// ENDIF

}
*/
void CreateAndAddScenicObject(SCENIC *sc)
{
	SCENICOBJ *newItem;

	newItem = ( SCENICOBJ* ) MALLOC0 ( sizeof ( SCENICOBJ ) );

	AddScenicObj ( newItem );

//	newItem->position = sc.pos;
	newItem->flags = 0;
	//rot

	

	utilPrintf("SCENIC %s, ROT %d %d %d %d\n", sc->name, sc->rot.x, sc->rot.y, sc->rot.z, sc->rot.w);

	QuatToPSXMatrix(&sc->rot, &newItem->matrix);

	



	//scale
// 	newItem->matrix.m[0][0] = FMul(newItem->matrix.m[0][0], sc->scale);
// 	newItem->matrix.m[1][1] = FMul(newItem->matrix.m[1][1], sc->scale);
// 	newItem->matrix.m[2][2] = FMul(newItem->matrix.m[2][2], sc->scale);
	//pos
	newItem->matrix.t[0] =  sc->pos.vx;
	newItem->matrix.t[1] =  sc->pos.vy;
	newItem->matrix.t[2] =  sc->pos.vz;
	
	utilPrintf("Creating Scenic Object : %s\n", sc->name);
//	utilUpperStr ( name );

	newItem->fmaObj = ( void * ) BFF_FindObject ( BFF_FMA_WORLD_ID, utilStr2CRC ( sc->name ) );

//	utilPrintf("Creating Scenic Object : %lu\n", utilStr2CRC ( name ));

	if ( newItem->fmaObj )
	{
		SetUpWaterMesh ( newItem->fmaObj );
	}
	else
	{
		utilPrintf("Could Not Create Scenic Object.\n");
	}
	// ENDIF

}



void DrawScenicObjList ( void )
{
	int i;

	SCENICOBJ *cur, *next;

	FMA_MESH_HEADER **mesh;


	if ( scenicObjList.numEntries == 0 )
		return;

	for ( cur = scenicObjList.head.next; cur != &scenicObjList.head; cur = next)
	{
		next = cur->next;

		if ( cur->fmaObj )
		{
			mesh = ADD2POINTER(cur->fmaObj,sizeof(FMA_WORLD));

			for ( i = cur->fmaObj->n_meshes; i != 0; i--, mesh++ )
			{
//bb dont need this, they don't move/rotate, just animate.
//				MapDraw_SetMatrix ( *mesh, *-mesh->posx, *mesh->posy, *mesh->posz );
//				MapDraw_SetMatrix ( *mesh, -cur->matrix.t[0], cur->matrix.t[1], cur->matrix.t[2] );
				MapDraw_SetScenicMatrix(*mesh, cur);

//				MATRIX temp;
//				MulMatrix0(&GsWSMATRIX, &cur->matrix, &temp);
//				MulMatrix0(&cur->matrix, &GsWSMATRIX, &temp);
//				gte_SetRotMatrix(&temp);
//				gte_SetTransMatrix(&temp);

//				gte_SetRotMatrix(&cur->matrix);
//				gte_SetTransMatrix(&cur->matrix);



				if ( MapDraw_ClipCheck ( *mesh ) )
				{
					DrawScenicObj ( *mesh, cur->flags );
				}
				// ENDIF
			}
			// ENDFOR

		}
		// ENDIF	    
	}
	// ENDFOR






/*	FMA_WORLD *waterObj = cur->fma_water;


	FMA_MESH_HEADER **mesh;

	mesh = ADD2POINTER(waterObj,sizeof(FMA_WORLD));

	for ( i = waterObj->n_meshes; i != 0; i--, mesh++ )
	{
		MapDraw_SetMatrix ( *mesh, -cur->position.vx, cur->position.vy, cur->position.vz );

		if ( MapDraw_ClipCheck ( *mesh ) )
		{
			DrawWater ( *mesh, cur->flags );
		}
		// ENDIF
	}*/
	// ENDFOR





}


void InitScenicObjLinkedList ( void )
{
	scenicObjList.numEntries = 0;
	scenicObjList.head.next = scenicObjList.head.prev = &scenicObjList.head;
}


void AddScenicObj ( SCENICOBJ *scenicObj )
{
	if ( scenicObj->next == NULL )
	{
		scenicObjList.numEntries++;
		scenicObj->prev								= &scenicObjList.head;
		scenicObj->next								= scenicObjList.head.next;
		scenicObjList.head.next->prev	= scenicObj;
		scenicObjList.head.next				=	scenicObj;
	}
}

void FreeScenicObjectList ( void )
{
	SCENICOBJ *cur,*next;

	if( !scenicObjList.numEntries )
		return;

	for ( cur = scenicObjList.head.next; cur != &scenicObjList.head; cur = next )
	{
		next = cur->next;
		SubScenicObject(cur);
	}
//	FREE(specFXList.head);
}


void SubScenicObject ( SCENICOBJ *scenicObj )
{
	if(scenicObj->next == NULL)
		return;

	scenicObj->prev->next = scenicObj->next;
	scenicObj->next->prev = scenicObj->prev;
	scenicObj->next = NULL;
	scenicObjList.numEntries--;

	FREE(scenicObj);
}


void DrawScenicObj ( FMA_MESH_HEADER *mesh, int flags )
{
	char u,v;

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

//	int min_depth = MIN_MAP_DEPTH + mesh->extra_depth;
	int max_depth = MAX_MAP_DEPTH + mesh->extra_depth;

	tfv = (long*)transformedVertices;
	tfd = (long*)transformedDepths;

	if ( max_depth > 1024 - mesh->extra_depth )
		max_depth = 1024 - mesh->extra_depth;


	if ( !jiggledVerts )
		jiggledVerts = MALLOC0 ( sizeof ( SVECTOR ) * 700 );

	
	if ( mesh->flags & JIGGLE )
	{
		for ( i = 0; i < mesh->n_verts; i++ )
		{
			jiggledVerts[i].vx = mesh->verts[i].vx + ( rsin ( ( frame << 5 ) + ( mesh->verts[i].vx & ( mesh->verts[i].vz ) ) ) >> 7 );
			jiggledVerts[i].vy = mesh->verts[i].vy + ( rcos ( ( frame << 6 ) + ( mesh->verts[i].vx | ( mesh->verts[i].vz ) ) ) >> 5 );
			jiggledVerts[i].vz = mesh->verts[i].vz + ( rsin ( ( frame << 5 ) + ( mesh->verts[i].vx ^ ( mesh->verts[i].vz ) ) ) >> 7 );
		}
		// ENDFOR
		
		transformVertexListA ( jiggledVerts, mesh->n_verts, tfv, tfd );
	}
	else
	{
		transformVertexListA(mesh->verts, mesh->n_verts, tfv, tfd);
	}
	// ENDELSEIF

	for ( i = 0; i < mesh->n_verts; i++ )
	{
		tfd [ i ] = tfd [ i ] >> 2;
	}
	// ENDFOR



#define si ((POLY_GT4*)packet)
#define op ((FMA_GT4 *)opcd)

	op = mesh->gt4s;

	packet = (PACKET *)currentDisplayPage->primPtr;


	polyCount += mesh->n_gt4s;

	for ( i = mesh->n_gt4s; i != 0; i--, op++ )
	{


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

			if ( mesh->flags & SHIFT_FORWARD )
				addPrimLen ( ot + ( depth - mesh->shift ), si, 12, t2 )
			else if ( mesh->flags & SHIFT_BACKWARD )
				addPrimLen ( ot + ( depth + mesh->shift ), si, 12, t2 )
			else
				addPrimLen ( ot + ( depth ), si, 12, t2 );
			// ENDELSEIF

			*(u_long *)  (&si->u0) = *(u_long *) (&op->u0);		// Texture coords
			*(u_long *)  (&si->u1) = *(u_long *) (&op->u1);

			if ( mesh->flags & FLOW )
			{
#ifdef DEBUG_INFO
				utilPrintf ( "Flowing.................................\n" );
#endif
				if ( mesh->flags & USLIDDING )
				{
#ifdef DEBUG_INFO
				utilPrintf ( "U Slidding.................................\n" );
#endif
					if ( mesh->flags & PLUSSLIDDING )
					{
#ifdef DEBUG_INFO
				utilPrintf ( "+ Slidding.................................\n" );
#endif
						si->u0 += ( ( frame/2 ) % 32 );
						si->u1 += ( ( frame/2 ) % 32 );
					}
					// ENDIF

					if ( mesh->flags & MINSLIDDING )
					{
#ifdef DEBUG_INFO
				utilPrintf ( "- Slidding.................................\n" );
#endif
						si->u0 -= ( ( frame/2 ) % 32 );
						si->u1 -= ( ( frame/2 ) % 32 );
					}
					// ENDIF
				}
				// ENDIF

				if ( mesh->flags & VSLIDDING )
				{
#ifdef DEBUG_INFO
				utilPrintf ( "V Slidding.................................\n" );
#endif
					if ( mesh->flags & PLUSSLIDDING )
					{
#ifdef DEBUG_INFO
				utilPrintf ( "+ Slidding.................................\n" );
#endif
						si->v0 += ( ( frame/2 ) % 32 );
						si->v1 += ( ( frame/2 ) % 32 );
					}
					// ENDIF

					if ( mesh->flags & MINSLIDDING )
					{
#ifdef DEBUG_INFO
				utilPrintf ( "- Slidding.................................\n" );
#endif
						si->v0 -= ( ( frame/2 ) % 32 );
						si->v1 -= ( ( frame/2 ) % 32 );
					}
					// ENDIF

				}
				// ENDIF

			}
			// ENDIF
			
			if ( mesh->flags & MODGY )
			{
				u = si->u0;
				v = si->v0;
			
				si->u0 = ( u + ( ( rsin ( frame << 6 ) + 4096 ) >> 10 ) );
				si->v0 = ( ( v + ( ( rcos ( frame << 6 ) + 4096 ) >> 10 ) ) );


				u = si->u1;
				v = si->v1;

				si->u1 = (u+((rsin(frame<<6)+4096)>>11));
				si->v1 = ((v+((rcos(frame<<6)+4096)>>11)));
			}
			// ENDIF*/
			
			gte_stsxy3_gt4(si);
								
			*(u_long *)  (&si->u2) = *(u_long *) (&op->u2);
			*(u_long *)  (&si->u3) = *(u_long *) (&op->u3);

			if ( mesh->flags & FLOW )
			{
#ifdef DEBUG_INFO
				utilPrintf ( "Flowing.................................\n" );
#endif
				if ( mesh->flags & USLIDDING )
				{
#ifdef DEBUG_INFO
				utilPrintf ( "U Slidding.................................\n" );
#endif
					if ( mesh->flags & PLUSSLIDDING )
					{
#ifdef DEBUG_INFO
				utilPrintf ( "+ Slidding.................................\n" );
#endif
						si->u2 += ( ( frame/2 ) % 32 );
						si->u3 += ( ( frame/2 ) % 32 );
					}
					// ENDIF

					if ( mesh->flags & MINSLIDDING )
					{
#ifdef DEBUG_INFO
				utilPrintf ( "- Slidding.................................\n" );
#endif
						si->u2 -= ( ( frame/2 ) % 32 );
						si->u3 -= ( ( frame/2 ) % 32 );
					}
					// ENDIF
				}
				// ENDIF

				if ( mesh->flags & VSLIDDING )
				{
#ifdef DEBUG_INFO
				utilPrintf ( "V Slidding.................................\n" );
#endif
					if ( mesh->flags & PLUSSLIDDING )
					{
#ifdef DEBUG_INFO
				utilPrintf ( "+ Slidding.................................\n" );
#endif
						si->v2 += ( ( frame/2 ) % 32 );
						si->v3 += ( ( frame/2 ) % 32 );
					}
					// ENDIF

					if ( mesh->flags & MINSLIDDING )
					{
#ifdef DEBUG_INFO
				utilPrintf ( "- Slidding.................................\n" );
#endif
						si->v2 -= ( ( frame/2 ) % 32 );
						si->v3 -= ( ( frame/2 ) % 32 );
					}
					// ENDIF
				}
				// ENDIF
			}
			// ENDIF

			if ( mesh->flags & MODGY )
			{
				u = si->u2;
				v = si->v2;

				si->u2 = (u+((rcos(frame<<6)+4096)>>11));
				si->v2 = ((v+((rsin(frame<<6)+4096)>>11)));

				u = si->u3;
				v = si->v3;

				si->u3 = (u+((rcos(frame<<6)+4096)>>11));
				si->v3 = ((v+((rsin(frame<<6)+4096)>>11)));
			}
			// ENDIF

			*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
			*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
			*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
			*(u_long *)  (&si->r3) = *(u_long *) (&op->r3);

			*(u_long *)  (&si->x3) = *(u_long *) ( &GETV ( op->vert3 ) );

			if ( mesh->flags & TRANS )
				si->code  |= 2;
			// ENDIF


			if ( mesh->flags & ADDATIVE )
			{
 				si->tpage |= 32;
			}
			// ENDIF

			if ( mesh->flags & SUBTRACTIVE )
			{
 				si->tpage = si->tpage | 64;
			}
			// ENDIF

			packet = ADD2POINTER ( packet, sizeof ( POLY_GT4 ) );

	//	}
	}
#undef op
#undef si

	currentDisplayPage->primPtr = (char *)packet;

//	if ( jiggledVerts )
//		FREE ( jiggledVerts );
}

void PTTextureLoad ( void )
{
/*	CreateProceduralTexture ( "PRC_FIRE1" );		

	CreateProceduralTexture ( "PRC_WATRD1" );		

	CreateProceduralTexture ( "PRC_WATRT1" );		
//	CreateProceduralTexture ( "00WATE04" );*/

}

void CreateProceduralTexture ( char *name )
{
	unsigned long i;
//	unsigned long rVand,gVand,bVand,rVshr,gVshr,bVshr;
	unsigned short newCol,nR,nG,nB;//,nA;
	unsigned short *tempPalette;

//	unsigned char *tempBuf;

//	int counter;

	RECT texRect;


	PROCTEXTURE *pt = (PROCTEXTURE *)MALLOC0( sizeof(PROCTEXTURE) );

	// Set proc texture members
	if ( !(pt->tex = textureFindCRCInAllBanks ( utilStr2CRC ( name ) ) ) )
	{
		utilPrintf("Could Not Find Texture : %s\n", name);
		FREE ( pt );
		return;
	}
	// ENDIF

	// Add to procedural text list
	pt->next = prcTexList;
	prcTexList = pt;

	pt->buf1 = (unsigned char *)MALLOC0( 2048 );
	pt->buf2 = (unsigned char *)MALLOC0( 2048 );
	pt->palette = (unsigned short *)MALLOC0( 512 );

	tempPalette = (unsigned short *)MALLOC0( 512 );

	pt->active = 1;

	texRect.x = VRAM_CALCVRAMX(pt->tex->handle);
	texRect.y = VRAM_CALCVRAMY(pt->tex->handle);
	texRect.w = 16;
	texRect.h = 32;

	DrawSync(0);
	StoreImage ( &texRect, (unsigned long*)pt->buf1 );
	DrawSync(0);

	
	StoreImage ( &texRect, (unsigned long*)pt->buf2 );
	DrawSync(0);

	texRect.x = (pt->tex->clut & 0x3f) << 4;
	texRect.y = pt->tex->clut >> 6;
	texRect.w = 256;
	texRect.h = 1;

	DrawSync(0);
	StoreImage ( &texRect, (unsigned long*)pt->palette );
	DrawSync(0);

	for( i=0; i<256; i++ )
	{
		nB = ( pt->palette [ pt->buf1 [ i ] ] >> 10 ) & 31;
		nG = ( pt->palette [ pt->buf1 [ i ] ] >> 5  ) & 31;
		nR =   pt->palette [ pt->buf1 [ i ] ] & 31;

		newCol = ( ( ( nB ) << 10 ) | ( ( nG ) << 5 ) | ( nR ) );
		
		((unsigned short *)tempPalette)[i] = newCol;
	}
			
	LoadClut ( (unsigned long*)tempPalette, (pt->tex->clut & 0x3f) << 4, pt->tex->clut >> 6 );
	
	// Set update function and type depending on filename
	if( name[4]=='F' && name[5]=='I' && name[6]=='R' && name[7]=='E' )
		pt->Update = ProcessPTFire;
	else if( name[4]=='f' && name[5]=='f' && name[6]=='l' && name[7]=='d' )
		pt->Update = ProcessPTForcefield;
	else if( name[4]=='w' && name[5]=='a' && name[6]=='t' && name[7]=='r' )
	{
		if( name[8]=='r' )
			pt->Update = ProcessPTWaterRipples;
		else if( name[8]=='d' )
			pt->Update = ProcessPTWaterDrops;
		else if( name[8]=='b' )
			pt->Update = ProcessPTWaterBubbler;
		else if( name[8]=='t' )
			pt->Update = ProcessPTWaterTrail;
		else if( name[8]=='w' )
			pt->Update = ProcessPTWaterWaves;
	}
	else if( name[4]=='s' && name[5]=='t' && name[6]=='e' && name[7]=='a' )
	{
		pt->Update = ProcessPTSteam;
//		pt->active = 0;
//		steamTex = pt;
	}
	else if( name[4]=='c' && name[5]=='n' && name[6]=='d' && name[7]=='l' )
		pt->Update = ProcessPTCandle;

	FREE ( tempPalette );
}

void PTSurfaceBlit( TextureType *tex, unsigned long *buf, unsigned short *pal )
{
	RECT texRect;

	int			atbdx,atbdy;

	register PACKET*		packet;

	static TextureType		*tPtr;

	texRect.x = VRAM_CALCVRAMX(tex->handle);
	texRect.y = VRAM_CALCVRAMY(tex->handle);
	texRect.w = tex->w;
	texRect.h = tex->h;
	
	// JH: 
	LoadImage ( &texRect, (unsigned long*)buf );



#define si ((POLY_FT4*)packet)

	atbdx = 32;
	atbdy = 32;

	tPtr = tex;

			BEGINPRIM(si, POLY_FT4);
	
			si->x0 = atbdx;
			si->y0 = atbdy;

			si->x1 = atbdx + tex->w;
			si->y1 = atbdy;

			si->x2 = atbdx;
			si->y2 = atbdy + tex->h;
			//si->y2 = atbdy + (spr->height);

			si->x3 = atbdx + tex->w;
			si->y3 = atbdy + tex->h;

	
			si->r0 = 255;
			si->g0 = 255;
			si->b0 = 255;
	
			si->u0 = tPtr->u0;
			si->v0 = tPtr->v0;
			si->u1 = tPtr->u1;
			si->v1 = tPtr->v1;
			si->u2 = tPtr->u2;
			si->v2 = tPtr->v2;
			si->u3 = tPtr->u3;
			si->v3 = tPtr->v3;
		
			si->code = GPU_COM_TF4;
			si->tpage = tPtr->tpage;
			si->clut = tPtr->clut;
		
			setPolyFT4(si);
			ENDPRIM(si, 1, POLY_FT4);


#undef si

}


unsigned short *realWaterData;
unsigned short *waterData;
unsigned short *reflectionData;

unsigned short *waterPalette;
unsigned short *reflectionPalette;

TextureType *realWater;
TextureType *water;
TextureType *reflection;

void InitWater ( void )
{
/*	int counter;
	RECT rect;

	// JH : Get a pointer to texture that we will over right in VRAM.
	if ( !( realWater = textureFindCRCInAllBanks ( utilStr2CRC ( "00WATE04" ) ) ) )
	{
		// JH : Output debug message if not found.
		utilPrintf("Could Not Find Texture : 00WATE04\n");
		return;
	}
	// ENDIF

	// JH : atempt to find the required texture.
	if ( !( water = textureFindCRCInAllBanks ( utilStr2CRC ( "WATER" ) ) ) )
	{
		// JH : Output debug message if not found.
		utilPrintf("Could Not Find Texture : WATER\n");
		return;
	}
	// ENDIF

	// JH : atempt to find the required texture.
	if ( !( reflection = textureFindCRCInAllBanks ( utilStr2CRC ( "REFLECT" ) ) ) )
	{
		// JH : Output debug message if not found.
		utilPrintf("Could Not Find Texture : REFLECT\n");
		return;
	}
	// ENDIF

	// JH : Make sure we can get both textures before we continue. Malloc the space for each texture,
	// JH : so that we can acces the texture data;
	realWaterData				= (unsigned short *)MALLOC0( 8192 );
	waterData				= (unsigned short *)MALLOC0( 8192 );
	reflectionData	= (unsigned short *)MALLOC0( 8192 );

	waterPalette			= (unsigned char *)MALLOC0( 512 );
	reflectionPalette	= (unsigned char *)MALLOC0( 512 );

	// JH : Make the rectangle of the texture in VRAM.
	rect.x = VRAM_CALCVRAMX ( realWater->handle );
	rect.y = VRAM_CALCVRAMY ( realWater->handle );
	rect.w = 64;
	rect.h = 64;

	// JH : Copy the texture data into a buffer.
	StoreImage ( &rect, (unsigned long*)realWaterData );

	// JH : Make the rectangle of the texture in VRAM.
	rect.x = VRAM_CALCVRAMX ( water->handle );
	rect.y = VRAM_CALCVRAMY ( water->handle );
	rect.w = 64;
	rect.h = 64;

	// JH : Copy the texture data into a buffer.
	StoreImage ( &rect, (unsigned long*)waterData );

	// JH : Make the rectangle of the texture in VRAM.
	rect.x = VRAM_CALCVRAMX ( reflection->handle );
	rect.y = VRAM_CALCVRAMY ( reflection->handle );
	rect.w = 64;
	rect.h = 64;

	// JH : Copy the texture data into a buffer.
	StoreImage ( &rect, (unsigned long*)reflectionData );

	/*//*/ JH : Make the rectangle of the palette in VRAM.
	rect.x = ( water->clut & 0x3f ) << 4;
	rect.y = water->clut >> 6;
	rect.w = 256;
	rect.h = 1;

	// JH : Copy the palette data into a buffer.
	StoreImage ( &rect, (unsigned long*)waterPalette );

	LoadClut ( (unsigned long*)waterPalette, (realWater->clut & 0x3f) << 4, realWater->clut >> 6 );

*/
	// JH : Make the rectangle of the palette in VRAM.
	/*rect.x = ( water->clut & 0x3f ) << 4;
	rect.y = water->clut >> 6;
	rect.w = 256;
	rect.h = 1;

	// JH : Copy the palette data into a buffer.
	StoreImage ( &rect, (unsigned long*)waterPalette );

//	for ( counter = 0; counter < 256; counter++ )
//		reflectionPalette[counter] = ((( reflectionPalette[counter] >> 10 ) & 0x1f) * 12) / 0x1f;

	// JH : Make the rectangle of the palette in VRAM.
	rect.x = ( realWater->clut & 0x3f ) << 4;
	rect.y = realWater->clut >> 6;
	rect.w = 256;
	rect.h = 1;

	// JH : Copy the palette data into a buffer.
	LoadImage ( &rect, (unsigned long*)waterPalette );*/
}


void UpdateWater ( void )
{
	/*long wIndex1,wIndex2;
	long wOffset;

	long x,y,y1,p;

	int counterX, counterY;
	RECT rect;

//	for ( counterY = 0; counterY < 64; counterY++ )
//		for ( counterX = 0; counterX < 64; counterX++ )
//			realWaterData [ counterX + counterY * 64 ] = reflectionData [ counterX + counterY * 64 ];
		
wOffset = 0;;

	for (y=0,y1=0; y<64*64; y+= 64,y1+=64)
		for (x=0; x<64; x++)
		{
			wIndex1 = ((x + wOffset) & (64-1)) + y;
			wIndex2 = ((x + *(reflectionData+wIndex1)) & (64-1)) + y;

			*(realWaterData+x+y1) = *(waterData+wIndex2);
		}

//memcpy ( realWaterData, waterData, 4096 );

	rect.x = VRAM_CALCVRAMX ( realWater->handle );
	rect.y = VRAM_CALCVRAMY ( realWater->handle );
	rect.w = realWater->w;
	rect.h = realWater->h;
	
	LoadImage ( &rect, (unsigned long*)realWaterData );
*/
}
