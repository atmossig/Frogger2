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
#include "frontend.h"

#include "World_Eff.h"

// JH: Let's define a name so that we can use sprite's in the scenic drawing routines.
#define NEW_SPRITE_MAPS

#define ADDITIVE		(1<<0)
#define SUBTRACTIVE		(1<<1)
#define USLIDING		(1<<4)
#define VSLIDING		(1<<5)
#define PLUSSLIDING		(1<<6)
#define MINUSSLIDING	(1<<7)
#define JIGGLE			(1<<8)
#define MODGY			(1<<9)
#define SLIDESPEED		(1<<10)
#define SLIDESPEED2		(1<<11)
#define SLOWSLIDE		(1<<12)

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


long slideSpeeds[] = {0, 1, 2, 4};

SVECTOR jiggledVerts[250];


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

//#define WATERANIM_1A (u+((rcos(frame<<10)+4096)>>5))|((v+((rsin(frame<<10)+4096)>>5))<<4)
//#define WATERANIM_2B (u+((rsin(frame<<10)+4096)>>5))|((v+((rcos(frame<<10)+4096)>>5))<<8)

//#define WATERANIM_1  ( ( u )  ) | ( ( v + ( ( 4096 ) >> 11 ) ) << 8 )
//#define WATERANIM_2  ( ( u )  ) | ( ( v + ( ( 4096 ) >> 11 ) ) << 8 )


//#define WATER_TRANS_CODE (((ULONG)2)<<24)
//#define WATER_TRANS_CLUT (0x80000000)

#define MIN_MAP_DEPTH (10)
	
#define MAX_MAP_DEPTH (1000)




void CreateAndAddScenicObject(SCENIC *sc)
{
	SCENICOBJ *newItem;

	newItem = ( SCENICOBJ* ) MALLOC0 ( sizeof ( SCENICOBJ ) );

	AddScenicObj ( newItem );

	newItem->flags = 0;

	utilPrintf("SCENIC %s, ROT %d %d %d %d\n", sc->name, sc->rot.x, sc->rot.y, sc->rot.z, sc->rot.w);

	QuatToPSXMatrix(&sc->rot, &newItem->matrix);

	newItem->matrix.t[0] =  sc->pos.vx;
	newItem->matrix.t[1] =  sc->pos.vy;
	newItem->matrix.t[2] =  sc->pos.vz;
	
	utilPrintf("Creating Scenic Object : %s\n", sc->name);

	newItem->fmaObj = ( void * ) BFF_FindObject ( BFF_FMA_WORLD_ID, utilStr2CRC ( sc->name ) );

	if ( newItem->fmaObj )
		SetUpWaterMesh ( newItem->fmaObj );
	else
		utilPrintf("Could Not Create Scenic Object.\n");

	if( sc->name[0] == 'S' && sc->name[1] == 'P')
	{
		switch( sc->name[2] )
		{
			case 'L': newItem->flags |= SLIDESPEED; break;
			case 'M': newItem->flags |= SLIDESPEED2; break;
			case 'F': newItem->flags |= (SLIDESPEED | SLIDESPEED2); break;
		}
	}
}



void DrawScenicObjList ( void )
{
	int i;
	SCENICOBJ *cur;
	FMA_MESH_HEADER **mesh;

	if ( scenicObjList.numEntries == 0 )
		return;

	for ( cur = scenicObjList.head.next; cur != &scenicObjList.head; cur = cur->next)
	{
		if ( cur->fmaObj )
		{
			mesh = ADD2POINTER(cur->fmaObj,sizeof(FMA_WORLD));

			for ( i = cur->fmaObj->n_meshes; i != 0; i--, mesh++ )
			{
				MapDraw_SetScenicMatrix(*mesh, cur);

				if ( MapDraw_ClipCheck ( *mesh ) )
					DrawScenicObj ( *mesh, cur->flags );
			}
		}
	}
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

	int i, j;

	int min_depth = MIN_MAP_DEPTH;// + mesh->extra_depth;
	int max_depth = MAX_MAP_DEPTH;// + mesh->extra_depth;

	// Store UV modge vals for a poly
	short mVs[8];

	// Is this an sp(m/f/l) type slidy thing? If so, what speed?
	unsigned short pcStyleSlide = ((flags>>10)&3);
	int sldSpd;//, rc, rs;
	unsigned long period1 = frameCount<<5, period2 = frameCount<<7;

	// Multiply speed if we have a multiple
	if( pcStyleSlide )
		sldSpd = ((frame>>1)*slideSpeeds[pcStyleSlide])%32;
	else
		sldSpd = (frame>>1)%32;

	// Reverse if told to
	if( mesh->flags & MINUSSLIDING )
		sldSpd = -sldSpd;

	if(mesh->n_verts <= 126)	// Not 128. TransformVerts rounds up to nearest 3, remember
	{
		tfv = (void *)0x1f800000;
		tfd = (void *)(0x1f800000 + 512);
	}
	else if(mesh->n_verts <= 255)	// Not 256. TransformVerts rounds up to nearest 3, remember
	{
		tfv = (void *)0x1f800000;
		tfd = transformedDepths;
	}
	else
	{
		tfv = transformedVertices;
		tfd = transformedDepths;
	}

	if( max_depth > 1024 - mesh->extra_depth )
		max_depth = 1024 - mesh->extra_depth;
	
	if( mesh->flags & JIGGLE )
	{
		// Interesting. The framecount shift controls the periiod of oscillation,
		// and the downshift at the end decreases the amplitude from huge.
		for ( i = 0; i < mesh->n_verts; i++ )
		{
			jiggledVerts[i].vx = mesh->verts[i].vx + (rcos(period1 + (mesh->verts[i].vx & mesh->verts[i].vz))>>8);
			jiggledVerts[i].vy = mesh->verts[i].vy + (rsin(period2 + (mesh->verts[i].vx ^ mesh->verts[i].vz))>>7);
			jiggledVerts[i].vz = mesh->verts[i].vz + (rcos(period1 + (mesh->verts[i].vx | mesh->verts[i].vz))>>8);
		}

		transformVertexListA ( jiggledVerts, mesh->n_verts, tfv, tfd );
	}
	else
	{
		transformVertexListA(mesh->verts, mesh->n_verts, tfv, tfd);
	}

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

		if ( depth > min_depth && depth < max_depth )
		{
			if( ( ( GETV ( op->vert0 ) & 0xff80ff00 ) + 0x00800100 ) &
					( ( GETV ( op->vert1 ) & 0xff80ff00 ) + 0x00800100 ) &
					( ( GETV ( op->vert2 ) & 0xff80ff00 ) + 0x00800100 ) &
					( ( GETV ( op->vert3 ) & 0xff80ff00 ) + 0x00800100 ) & 0x01000200 )
			{
				continue;
			}

			gte_ldsxy3 ( GETV ( op->vert0 ), GETV ( op->vert1 ), GETV ( op->vert2 ) );

			addPrimLen ( ot + ( depth + mesh->shift), si, 12, t2 );

			// Precalc modge values for this quad
			if ( mesh->flags & MODGY )
			{
				int fr=(frame<<5), vv;
				for( j=3; j>=0; j-- )
				{
					vv = fr + (mesh->verts[*(&op->vert0+j)].vx * mesh->verts[*(&op->vert0+j)].vz);
					mVs[j<<1]		= rsin(vv)>>10;
					mVs[(j<<1)+1]	= rcos(vv)>>10;
				}
			}

			*(u_long *)  (&si->u0) = *(u_long *) (&op->u0);		// Texture coords
			*(u_long *)  (&si->u1) = *(u_long *) (&op->u1);

			if( mesh->flags & USLIDING )
			{
				si->u0 += sldSpd;
				si->u1 += sldSpd;
			}
			if( pcStyleSlide || (mesh->flags & VSLIDING) )
			{
				si->v0 += sldSpd;
				si->v1 += sldSpd;
			}

			if ( mesh->flags & MODGY )
			{
				si->u0 += mVs[0];
				si->v0 += mVs[1];
				si->u1 += mVs[2];
				si->v1 += mVs[3];
			}
			
			gte_stsxy3_gt4(si);
								
			*(u_long *)  (&si->u2) = *(u_long *) (&op->u2);
			*(u_long *)  (&si->u3) = *(u_long *) (&op->u3);

			if ( mesh->flags & USLIDING )
			{
				si->u2 += sldSpd;
				si->u3 += sldSpd;
			}
			if( pcStyleSlide || (mesh->flags & VSLIDING) )
			{
				si->v2 += sldSpd;
				si->v3 += sldSpd;
			}

			if ( mesh->flags & MODGY )
			{
				si->u2 += mVs[4];
				si->v2 += mVs[5];
				si->u3 += mVs[6];
				si->v3 += mVs[7];
			}

			*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
			*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
			*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
			*(u_long *)  (&si->r3) = *(u_long *) (&op->r3);

			*(u_long *)  (&si->x3) = *(u_long *) ( &GETV ( op->vert3 ) );

			si->code	= op->code | ( op->pad2 & 2 );
			si->tpage = op->tpage | ( op->pad2 & 96 );

			packet = ADD2POINTER ( packet, sizeof ( POLY_GT4 ) );
		}
	}
#undef op
#undef si

#define si ((POLY_GT3*)packet)
#define op ((FMA_GT3 *)opcd)

	op = mesh->gt3s;

	polyCount += mesh->n_gt3s;

	for ( i = mesh->n_gt3s; i != 0; i--, op++ )
	{
		gte_ldsz3 ( GETD ( op->vert0 ), GETD ( op->vert1 ), GETD ( op->vert2 ) );
	   	gte_avsz3();
		gte_stotz_cpu ( depth );

		if ( depth > min_depth && depth < max_depth )
		{
			if(
			((GETV(op->vert0) & 0xff80ff00) + 0x00800100)
			&
			((GETV(op->vert1) & 0xff80ff00) + 0x00800100)
			&
			((GETV(op->vert2) & 0xff80ff00) + 0x00800100)
			& 0x01000200
			)
			continue;

			gte_ldsxy3 ( GETV ( op->vert0 ), GETV ( op->vert1 ), GETV ( op->vert2 ) );
			addPrimLen ( ot + ( depth + mesh->shift), si, 9, t2 );

			// Precalc modge values for this vertex
			if ( mesh->flags & MODGY )
			{
				int fr=(frame<<5), vv;
				for( j=2; j>=0; j-- )
				{
					vv = fr+(mesh->verts[*(&op->vert0+j)].vx * mesh->verts[*(&op->vert0+j)].vz);
					mVs[j<<1]		= rsin(vv)>>10;
					mVs[(j<<1)+1]	= rcos(vv)>>10;
				}
			}

			*(u_long *)  (&si->u0) = *(u_long *) (&op->u0);
			*(u_long *)  (&si->u1) = *(u_long *) (&op->u1);

			if ( mesh->flags & USLIDING )
			{
				si->u0 += sldSpd;
				si->u1 += sldSpd;
			}
			if( pcStyleSlide || (mesh->flags & VSLIDING) )
			{
				si->v0 += sldSpd;
				si->v1 += sldSpd;
			}

			if ( mesh->flags & MODGY )
			{
				si->u0 += mVs[0];
				si->v0 += mVs[1];
				si->u1 += mVs[2];
				si->v1 += mVs[3];
			}
			
			gte_stsxy3_gt3(si);
			*(u_long *)  (&si->u2) = *(u_long *) (&op->u2);

			if ( mesh->flags & USLIDING )
				si->u2 += sldSpd;
			if( pcStyleSlide || (mesh->flags & VSLIDING) )
				si->v2 += sldSpd;

			if ( mesh->flags & MODGY )
			{
				si->u2 += mVs[4];
				si->v2 += mVs[5];
			}

			*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
			*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
			*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);

			si->code	= op->code | ( op->pad2 & 2 );
			si->tpage = op->tpage | ( op->pad2 & 96 );

			packet = ADD2POINTER ( packet, sizeof ( POLY_GT3 ) );
		}
	}
#undef op
#undef si

	currentDisplayPage->primPtr = (char *)packet;
}




















void PTTextureLoad ( void )
{
	//CreateProceduralTexture ( "PRC_FIRE1" );		

//	CreateProceduralTexture ( "PRC_WATRD1" );		

//	CreateProceduralTexture ( "PRC_WATRT1" );		
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

