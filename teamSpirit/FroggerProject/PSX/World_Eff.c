
#include "sonylibs.h"
#include <islutil.h>
#include "shell.h"
#include "types.h"
#include "main.h"
#include "map_draw.h"
#include "actor2.h"
#include "maths.h"

#include "World_Eff.h"


SCENICOBJLIST scenicObjList;

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

//#define WATERANIM_1 (u+((rcos(frame<<6)+4096)>>11))|((v+((rsin(frame<<6)+4096)>>11))<<8)
//#define WATERANIM_2 (u+((rsin(frame<<6)+4096)>>11))|((v+((rcos(frame<<6)+4096)>>11))<<8)

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

			if ( mesh->flags & ( 1 << 4 ) )
				addPrimLen ( ot + ( depth - mesh->shift ), si, 12, t2 )
			else if ( mesh->flags & ( 1 << 5 ) )
				addPrimLen ( ot + ( depth + mesh->shift ), si, 12, t2 )
			else
				addPrimLen ( ot + ( depth ), si, 12, t2 );
			// ENDELSEIF

			if ( mesh->flags & ( 1 << 3 ) )
				u = op->u0 + ( ( frame / 2 ) % 32 );
			else
				u = op->u0;
			// ENDELSEIF

			v = op->v0;

			if ( mesh->flags & ( 1 << 2 ) )
				t1 = WATERANIM_1 | ( op->clut << 16 ) | WATER_TRANS_CLUT;
			else
				t1 = WATERANIM_1 | ( op->clut << 16 ) | 0;

			if ( mesh->flags & ( 1 << 3 ) )
				u = op->u1 + ( ( frame / 2 ) % 32 );
			else
				u = op->u1;
			// ENDELSEIF
			v = op->v1;

			t2 = WATERANIM_1 | ( op->tpage << 16 );

			*(u_long *)  (&si->u0) = t1;
			*(u_long *)  (&si->u1) = t2;

			gte_stsxy3_gt4 ( si );	// The first 3 x's & y's are already in the gte, so we may as well use 'em

			if ( mesh->flags & ( 1 << 3 ) )
				u = op->u2 + ( ( frame / 2 ) % 32 );
			else
				u = op->u2;
			// ENDELSEIF
			v = op->v2;

			t1 = WATERANIM_2;

			if ( mesh->flags & ( 1 << 3 ) )
				u = op->u3 + ( ( frame / 2 ) % 32 );
			else
				u = op->u3;
			// ENDELSEIF
			v = op->v3;

			t2 = WATERANIM_2;

			*(u_long *)  (&si->u2) = t1;
			*(u_long *)  (&si->u3) = t2;
			*(u_long *)  (&si->x3) = *(u_long *) ( &GETV ( op->vert3 ) );

			if ( mesh->flags & ( 1 << 2 ) )
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
//				MapDraw_SetMatrix ( *mesh, -cur->position.vx, cur->position.vy, cur->position.vz );
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

void FreeScenicObjectList ( SCENICOBJ *scenicObj )
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
	
	transformVertexListA(mesh->verts, mesh->n_verts, tfv, tfd);
	//transformVertexListA ( jiggledVerts, mesh->n_verts, tfv, tfd );

	for ( i = 0; i < mesh->n_verts; i++ )
	{
		tfd [ i ] = tfd [ i ] >> 2;
	}
	// ENDFOR



#define si ((POLY_GT4*)packet)
#define op ((FMA_GT4 *)opcd)

	op = mesh->gt4s;

	packet = (PACKET *)currentDisplayPage->primPtr;


	for ( i = mesh->n_gt4s; i != 0; i--, op++ )
	{
		char u,v;


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

			if ( mesh->flags & ( 1 << 4 ) )
				addPrimLen ( ot + ( depth - mesh->shift ), si, 12, t2 )
			else if ( mesh->flags & ( 1 << 5 ) )
				addPrimLen ( ot + ( depth + mesh->shift ), si, 12, t2 )
			else
				addPrimLen ( ot + ( depth ), si, 12, t2 );
			// ENDELSEIF

			*(u_long *)  (&si->u0) = *(u_long *) (&op->u0);		// Texture coords
			*(u_long *)  (&si->u1) = *(u_long *) (&op->u1);

			if ( mesh->flags & ( 1 << 3 ) )
			{
				si->u0 += ( ( frame / 2 ) % 32 );
				si->u1 += ( ( frame / 2 ) % 32 );
			}
			// ENDIF
			
			gte_stsxy3_gt4(si);
								
			*(u_long *)  (&si->u2) = *(u_long *) (&op->u2);
			*(u_long *)  (&si->u3) = *(u_long *) (&op->u3);

			if ( mesh->flags & ( 1 << 3 ) )
			{
				si->u2 += ( ( frame / 2 ) % 32 );
				si->u3 += ( ( frame / 2 ) % 32 );
			}
			// ENDIF

			*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
			*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
			*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
			*(u_long *)  (&si->r3) = *(u_long *) (&op->r3);

			*(u_long *)  (&si->x3) = *(u_long *) ( &GETV ( op->vert3 ) );

			if ( mesh->flags & ( 1 << 2 ) )
				si->code  |= 2;
			// ENDIF


			if ( mesh->flags & ( 1 << 7 ) )
			{
 				si->tpage |= 32;
			}
			// ENDIF

			if ( mesh->flags & ( 1 << 8 ) )
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

}

