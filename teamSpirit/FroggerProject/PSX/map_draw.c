


// comment out to link into the frogger testbed
// leave in to link into the ActionMan2 testbed
//#define USE_NEW_LIBS


#include "sonylibs.h"
#include <islutil.h>
//#include "global.h"
#include "shell.h"
#include "types.h"
//#include <islpsi.h>
#include "map_draw.h"
#include "main.h"

#include "actor2.h"

//#define WATERANIM_1 (u+((rcos(frame<<6))>>11))|((v+((rsin(frame<<6))>>11))<<8)
//#define WATERANIM_2 (u+((rsin(frame<<6))>>11))|((v+((rcos(frame<<6))>>11))<<8)

//#define WATERANIM_1 (u+((rcos(frame<<6)+4096)>>11))|((v+((rsin(frame<<6)+4096)>>11))<<8)
//#define WATERANIM_2 (u+((rsin(frame<<6)+4096)>>11))|((v+((rcos(frame<<6)+4096)>>11))<<8)

extern EXPLORE_black_CLUT;

#define WATERANIM_1  ( ( u )  ) | ( ( v + ( ( 4096 ) >> 11 ) ) << 8 )
#define WATERANIM_2  ( ( u )  ) | ( ( v + ( ( 4096 ) >> 11 ) ) << 8 )


extern int polyCount;

//#define WATERANIM_1	(u|(v<<8))
//#define WATERANIM_2	WATERANIM_1

// Scale-Depth-Down deals with shifting large depth-coordinates in a scaled-up map,
// shifting down to a smaller range of OT positions.

// Scale-Adjust allows a scaled-up map to be integrated into an otherwise NON-scaled-up
// coordinate system (probably a temporary measure), by increasing the object-camera
// distance for the duration of the map-plotting calls.

// #define MAP_SCALE_DEPTH_DOWN 3
// #define MAP_SCALE_ADJUST

// A couple of the optimisations in here are enablable by "#define". Once this is sourcesafed, it
//may be worth tidying them out to make the source neater.

// By my reckoning, doing block memory copies via 2 registers instead of via 1 gives us a 2% speed increase in the map plotter
// (But this define lets you go back to the old stuff if needs be)
#define TEMP_STORE

// Similarly, here's an optimisation that does the backface cull into a CPU register rather than a
// stacked one.
#define CLIP_REGISTER

/*

  Routines to RENDER frogger2 maps.

  Essentially, an optimised model printer that only deals with
  
	Textured Tris & Quads
	Single-Sided only.
	Non-Transparent.

*/


#define WATER_TRANS_CODE (((ULONG)2)<<24)
#define WATER_TRANS_CLUT (0x80000000)

// PSI.C variables rqd for the draw prims.
// They're defined in the latest islpsi header, so that's ok.
//long *transformedVertices;
//extern VERT *transformedNormals;
//long *transformedDepths;
displayPageType *currentDisplayPage;


// I dunno what part of the ISL lib these are in. Part of the psi lib, I think,
// but it's not in the headers yet


typedef struct{
	SHORT x,y,z,w;
}SHORTQUAT;

void ShortquaternionGetMatrix(SHORTQUAT *squat, MATRIX *dmatrix);
typedef struct
{
	short x,y;
}SHORTXY;




#ifndef ADD2POINTER
	#define ADD2POINTER(a,b) ( (void *) ( (int)(a) + (b))  )
#endif



// "addPrimLen" is faster than doing "setPolyGT3/4","addPrim",
// becuase a fair few of the numbers are already known / filled in

// In fact, IF we can guarantee that we're linking into an OT address (as we can, here),
// and NOT the tail end of another primitive, then the "length" part of the P_TAG will be zero,
// so we can just read/write the OT address as a "u_long" instead of using "getaddr/setaddr"

// The "<<24" is optimised to a constant by the compiler if "len" is constant.

// this version also assumes we've got a spare register for use as "tempvar" to get rid of a NOP

#define addPrimLen(ot,p,len,tempvar) \
{\
	tempvar = *(u_long *)(ot); \
    (*(u_long *)(ot) = (u_long)(p)); \
	tempvar |= (len<<24); \
	*(unsigned long *)(p) = tempvar; \
}


// (Previous, less-optimised, versions
/*
#define addPrimLen(ot,p,len)\
{\
	(*(unsigned long *)p = *(u_long *)(ot) | (len<<24));\
    (*(u_long *)(ot) = (u_long)p);\
}
*/
//#define addPrimLen(ot,p,len)	(*(unsigned long *)p = getaddr(ot) | (len<<24)), setaddr(ot,p)
//#define addPrimLen(ot,p,len)	{setlen(p,len); addPrim(ot,p);}




// Extra bit of direct gte-access that doesn't then store the register into a stacked variable
// (ie, this gives a nice little speed improvement)
#define gte_stotz_cpu(r)\
asm(\
  "mfc2 %0, $7;"\
  :\
  : "r" (r)\
)

#ifdef CLIP_REGISTER

// Fred, checking whether the same speed improvement works with the backface cull.
// (Answer, yes it does. Hope I've got this bit of asm right!)
#define gte_stopz_cpu(r)\
asm(\
	"mfc2 %0, $24;"\
	:\
	: "r"(r)\
)
#endif


// Extra Map Depth is now part of the mesh header. The idea is that it's set to about 10, to ensure that
// the frog appears in front of the landscape. "Underwater" objects should have their extra depth set higher,
// so they don't sort badly with the water plane itself
//#define XTRA_MAP_DEPTH 10


// The MIN map depth is basically in here because when doing the perspective transform
// close to / overlapping the camera plane, the PSX switches to an isometric transform rather than a perspective one,
// which looks horrible!
//#define MIN_MAP_DEPTH 140
//#define MIN_MAP_DEPTH 100
#define MIN_MAP_DEPTH (10)

// Must be under 1024 to work with the library's OT
// Use lower values still to define the far cut-off distance.
#define MAX_MAP_DEPTH (1000<<3)




// ===================================================
// New, mega-optimised landscape plotter
// This is the routine to draw a single mesh shape.
// Assumes that the matrix is already set up,
// and that any visibility tests have already been done


// Note that this routine has been very heavily optimised.
// If you ever want to add functionality to the map-draw (eg - double-sided polys, transparancy, subdivision)
// then many of these will actually fail & chug, due to lack of register space.
// The routine is fast because its simple.

void MapDraw_DrawFMA_Mesh2(FMA_MESH_HEADER *mesh)
{
#ifdef TEMP_STORE
// I've re-jigged the code, so "depth" is re-usable, & shares register with t1
// Also, now, clipflag shares a register with t2
	register u_long t1 asm("$16");
	register u_long t2 asm("$20");
#endif

	register PACKET * packet asm("$17");
	register char *opcd asm("$18");

	long *tfv;
	long *tfd;

// These macros are for an optimisation that requires all our vertex numbers to be multiplied by four,
// which means they're byte offsets into the tfv/tfd table rather than int-array indices
// Essentially
// tfv[n] and tfd[n], where tfv/tfd are arrays of ints, have now been replaced by these macros,
// where "n" is pre-shifted at PC map-compile time. Thus saving a shift instruction per vertex per polygon.
#define GETX(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->x )
#define GETY(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->y )
#define GETV(n)(  *(u_long *)( (int)(tfv) +(n) ) )
#define GETD(n)(  *(u_long *)( (int)(tfd) +(n) ) )


// "depth" is forced to be a CPU register so we can use the fast gte macro
// (Also note, it's shared with a temporary variable "t1")
	register long depth asm("$16");


	unsigned long *ot = currentDisplayPage->ot+mesh->extra_depth;

	int i;
#ifdef CLIP_REGISTER
	register long clipflag asm("$20");
#else
	long clipflag;
#endif

	int min_depth = MIN_MAP_DEPTH + mesh->extra_depth;
	int max_depth = MAX_MAP_DEPTH + mesh->extra_depth;




	tfv = (long*)transformedVertices;
	tfd = (long*)transformedDepths;




	if(max_depth > 1024-mesh->extra_depth)
		max_depth = 1024-mesh->extra_depth;

	transformVertexListA(mesh->verts, mesh->n_verts, tfv, tfd);

// This should really by in the (or an alternative) transformvertexlist function
// It scales the OTZ's down so that they actually fit into the size of the ordering table.
#ifdef MAP_SCALE_DEPTH_DOWN
	{
		for(i = 0; i < mesh->n_verts; i++)
			tfd[i] = tfd[i] >> MAP_SCALE_DEPTH_DOWN;
	}
#endif

	//test !!!!!
	for(i = 0; i < mesh->n_verts; i++)
//mmfrig		tfd[i] = tfd[i] >> 3;
		tfd[i] = tfd[i] >> 2;


	packet = (PACKET *)currentDisplayPage->primPtr;






// The MapDraw stores a count+list of quads and one of tris,
// rather than checking a "type" element for every polygon.
// (coz it only has those two types. No transparent. No double sided. No nothing)




#define si ((POLY_GT4*)packet)
#define op ((FMA_GT4 *)opcd)

	op = mesh->gt4s;

// Little note - Down-counters are quicker than up-counters because you can
// compare with zero rather than with a variable.
// "!=0" is also marginally faster than ">0" here.
// n_gt4s CAN be zero. Otherwise,a (do,while) loop would be ever-so-slightly faster.than the (for...) one.

	for(i = mesh->n_gt4s; i != 0; i--,op++)
	{
// Get average Z of the polygon & use as a depth pointer
		gte_ldsz4(GETD(op->vert0),GETD(op->vert1),GETD(op->vert2),GETD(op->vert3));
   	gte_avsz4();
		gte_stotz_cpu(depth);

		if(depth > min_depth && depth < max_depth)
		{
// Skip the poly if all the verts are off screen.
// A fairly hefty check, but one that gives us a 20% code-speed increase & 30% less gpu work.
//			if ( (GETX(op->vert0)+256)&(GETX(op->vert1)+256)&(GETX(op->vert2)+256)&(GETX(op->vert3)+256)&512) continue;
//			if ( (GETY(op->vert0)+128)&(GETY(op->vert1)+128)&(GETY(op->vert2)+128)&(GETY(op->vert3)+128)&256) continue;

// Here's an entertaining little optimisation for the above that does X & Y at the same time,
// masking off the low bits to avoid overflow problems, and knocks a further 10% off the code side of things
// (Looking at the source, the compiler has sufficient registers free for two of the three constants.
// Hence the speedup)
			if(
			((GETV(op->vert0) & 0xff80ff00) + 0x00800100)
			&
			((GETV(op->vert1) & 0xff80ff00) + 0x00800100)
			&
			((GETV(op->vert2) & 0xff80ff00) + 0x00800100)
			&
			((GETV(op->vert3) & 0xff80ff00) + 0x00800100)
			& 0x01000200
			)
			continue;


// Load 1st three vertices for a backface cull check
// (Yes, this check is better when placed after the offscreen one - Checked on PA)
			gte_ldsxy3(GETV(op->vert0), GETV(op->vert1), GETV(op->vert2));
			gte_nclip();
#ifdef CLIP_REGISTER
			gte_stopz_cpu(clipflag);
#else
			gte_stopz(&clipflag);
#endif

			if (clipflag<0)
			{


// Put the polygon into the OT (Note that we re-use "depth" as one of the "t" variables)
//				addPrimLen(ot+(depth), (si),12,t2);
				addPrimLen(ot+(depth), (si),12,t2);
// Copy the polygon data over into the OT
#ifdef TEMP_STORE
				t1 = *(u_long *) (&op->u0);
				t2 = *(u_long *) (&op->u1);
				*(u_long *)  (&si->u0) = t1;
				*(u_long *)  (&si->u1) = t2;
				gte_stsxy3_gt4(si);	// The first 3 x's & y's are already in the gte, so we may as well use 'em
				t1 = *(u_long *) (&op->u2);
				t2 = *(u_long *) (&op->u3);
				*(u_long *)  (&si->u2) = t1;
				*(u_long *)  (&si->u3) = t2;
				*(u_long *)  (&si->x3) = *(u_long *) (&GETV(op->vert3));
				t1 = *(u_long *) (&op->r0);
				t2 = *(u_long *) (&op->r1);
				*(u_long *)  (&si->r0) = t1;
				*(u_long *)  (&si->r1) = t2;
				t1 = *(u_long *) (&op->r2);
				t2 = *(u_long *) (&op->r3);
				*(u_long *)  (&si->r2) = t1;
				*(u_long *)  (&si->r3) = t2;
#else
				*(u_long *)  (&si->u0) = *(u_long *) (&op->u0);
				*(u_long *)  (&si->u1) = *(u_long *) (&op->u1);
				gte_stsxy3_gt4(si);
				*(u_long *)  (&si->u2) = *(u_long *) (&op->u2);
				*(u_long *)  (&si->u3) = *(u_long *) (&op->u3);
				*(u_long *)  (&si->x3) = *(u_long *) (&GETV(op->vert3));
				*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
				*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
				*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
				*(u_long *)  (&si->r3) = *(u_long *) (&op->r3);
#endif
				packet = ADD2POINTER(packet,sizeof(POLY_GT4));
				polyCount++;
			}
		}
	}

#undef si
#undef op

// That's the quads done, now let's do the triangles...

#define si ((POLY_GT3*)packet)
#define op ((FMA_GT3 *)opcd)

	op = mesh->gt3s;
	for(i = mesh->n_gt3s; i != 0; i--,op++)
	{
// Get average Z of the polygon & use as a depth pointer
		gte_ldsz3(GETD(op->vert0),GETD(op->vert1),GETD(op->vert2));
   		gte_avsz3();
		gte_stotz_cpu(depth);

		if(depth > min_depth && depth < max_depth)
		{

// Skip the poly if all the verts are off screen.
// A fairly hefty check, but one that gives us a 20% code-speed increase & 30% less gpu work.

//			if ( (GETX(op->vert0)+256)&(GETX(op->vert1)+256)&(GETX(op->vert2)+256)&512) continue;
//			if ( (GETY(op->vert0)+128)&(GETY(op->vert1)+128)&(GETY(op->vert2)+128)&256) continue;

// Optimisation - X & Y at the same time
			if(
			((GETV(op->vert0) & 0xff80ff00) + 0x00800100)
			&
			((GETV(op->vert1) & 0xff80ff00) + 0x00800100)
			&
			((GETV(op->vert2) & 0xff80ff00) + 0x00800100)
			& 0x01000200
			)
			continue;


// Load 1st three vertices for a backface cull
			gte_ldsxy3(GETV(op->vert0), GETV(op->vert1), GETV(op->vert2));
			gte_nclip();
#ifdef CLIP_REGISTER
			gte_stopz_cpu(clipflag);
#else
			gte_stopz(&clipflag);
#endif
			if(clipflag < 0)
			{
// Put the polygon into the OT (Note that we re-use "depth" as one of the "t" variables)
				addPrimLen(ot+(depth), (si),9,t2);
// Copy the polygon data over into the OT
#ifdef TEMP_STORE
				t1 = *(u_long *) (&op->u0);
				t2 = *(u_long *) (&op->u1);
				*(u_long *)  (&si->u0) = t1;
				*(u_long *)  (&si->u1) = t2;
				gte_stsxy3_gt3(si);
				t1 = *(u_long *) (&op->u2);
				t2 = *(u_long *) (&op->r0);
				*(u_long *)  (&si->u2) = t1;
				*(u_long *)  (&si->r0) = t2;
				t1 = *(u_long *) (&op->r1);
				t2 = *(u_long *) (&op->r2);
				*(u_long *)  (&si->r1) = t1;
				*(u_long *)  (&si->r2) = t2;
#else
				*(u_long *)  (&si->u0) = *(u_long *) (&op->u0);
				*(u_long *)  (&si->u1) = *(u_long *) (&op->u1);
				gte_stsxy3_gt3(si);
				*(u_long *)  (&si->u2) = *(u_long *) (&op->u2);
				*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
				*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
				*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
#endif
				packet = ADD2POINTER(packet,sizeof(POLY_GT3));
				polyCount++;
			}
		}
	}

#undef si
#undef op
#undef xy


	currentDisplayPage->primPtr = (char *)packet;

}

void SetUpWaterMesh(FMA_WORLD *pFMA)
{
	int i;
	FMA_MESH_HEADER **ppMh=(FMA_MESH_HEADER **)(((ULONG)pFMA)+sizeof(FMA_WORLD));

//	ASSERT(pFMA);

	for(i=0; i<pFMA->n_meshes; i++)
	{
		FMA_MESH_HEADER *pMh=ppMh[i];

//	int n_gt3s;
//	FMA_GT3 *gt3s;
//	int n_gt4s;
//	FMA_GT4 *gt4s;
		
		int p,v;
		int vc[4];
		int vp;

		for(v=0; v<pMh->n_verts; v++)
		{
			pMh->verts[v].pad=0;
		}

		for(p=0; p<pMh->n_gt3s; p++)
		{
			vc[0]=(pMh->gt3s[p].vert0)>>2;
			vc[1]=(pMh->gt3s[p].vert1)>>2;
			vc[2]=(pMh->gt3s[p].vert2)>>2;

			for(vp=0; vp<3; vp++)
			{
//				ASSERT(vc[vp]<pMh->n_verts);
				pMh->verts[vc[vp]].pad++;
			}
		}

		for(p=0; p<pMh->n_gt4s; p++)
		{
			vc[0]=(pMh->gt4s[p].vert0)>>2;
			vc[1]=(pMh->gt4s[p].vert1)>>2;
			vc[2]=(pMh->gt4s[p].vert2)>>2;
			vc[3]=(pMh->gt4s[p].vert3)>>2;

			for(vp=0; vp<4; vp++)
			{
//				ASSERT(vc[vp]<pMh->n_verts);
				pMh->verts[vc[vp]].pad++;
			}
		}

		for(v=0; v<pMh->n_verts; v++)
		{
//			utilPrintf("vert %d is used %d times\n", v, pMh->verts[v].pad);
		}

	}
}

/*void MapDraw_DrawFMA_Mesh2Water ( FMA_MESH_HEADER *mesh, int flags )
{
	static SVECTOR *jiggledVerts=NULL;

#ifdef TEMP_STORE
// I've re-jigged the code, so "depth" is re-usable, & shares register with t1
// Also, now, clipflag shares a register with t2
	register u_long t1 asm("$16");
	register u_long t2 asm("$20");
#endif

	register PACKET * packet asm("$17");
	register char *opcd asm("$18");

//bb
//	register long *tfv = transformedVertices;
//  register long *tfd = transformedDepths;
//	long *tfv = (long*)transformedVertices;
//	long *tfd = (long*)transformedDepths;
	long *tfv;
	long *tfd;

// These macros are for an optimisation that requires all our vertex numbers to be multiplied by four,
// which means they're byte offsets into the tfv/tfd table rather than int-array indices
// Essentially
// tfv[n] and tfd[n], where tfv/tfd are arrays of ints, have now been replaced by these macros,
// where "n" is pre-shifted at PC map-compile time. Thus saving a shift instruction per vertex per polygon.
#define GETX(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->x )
#define GETY(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->y )
#define GETV(n)(  *(u_long *)( (int)(tfv) +(n) ) )
#define GETD(n)(  *(u_long *)( (int)(tfd) +(n) ) )


// "depth" is forced to be a CPU register so we can use the fast gte macro
// (Also note, it's shared with a temporary variable "t1")
	register long depth asm("$16");




	unsigned long *ot = currentDisplayPage->ot+mesh->extra_depth;

	int i;
//#ifdef CLIP_REGISTER
//	register long clipflag asm("$20");
//#else
//	long clipflag;
//#endif

	int min_depth = MIN_MAP_DEPTH + mesh->extra_depth;
	int max_depth = MAX_MAP_DEPTH + mesh->extra_depth;

	tfv = (long*)transformedVertices;
	tfd = (long*)transformedDepths;

	if(max_depth > 1024-mesh->extra_depth)
		max_depth = 1024-mesh->extra_depth;


	if(!jiggledVerts)
		jiggledVerts = MALLOC0(sizeof(SVECTOR)*700);

	for(i=0; i<mesh->n_verts; i++)
	{
//		if(mesh->verts[i].pad>2)
		{
//			jiggledVerts[i].vx=mesh->verts[i].vx+(rsin((frame<<6)+(mesh->verts[i].vx&(mesh->verts[i].vz)))>>1);
//			jiggledVerts[i].vy=mesh->verts[i].vy+(rsin((frame<<6)+(mesh->verts[i].vx|(mesh->verts[i].vz)))>>1);
//			jiggledVerts[i].vz=mesh->verts[i].vz+(rsin((frame<<6)+(mesh->verts[i].vx^(mesh->verts[i].vz)))>>1);

			jiggledVerts[i].vx = mesh->verts[i].vx + ( rsin ( ( frame << 6 ) + ( mesh->verts[i].vx & ( mesh->verts[i].vz ) ) ) >> 7 );
			jiggledVerts[i].vy = mesh->verts[i].vy + ( rsin ( ( frame << 5 ) + ( mesh->verts[i].vx | ( mesh->verts[i].vz ) ) ) >> 8 );
			jiggledVerts[i].vz = mesh->verts[i].vz + ( rsin ( ( frame << 6 ) + ( mesh->verts[i].vx ^ ( mesh->verts[i].vz ) ) ) >> 7 );

		}
//		else
//		{
//			jiggledVerts[i].vx=mesh->verts[i].vx;
//			jiggledVerts[i].vy=mesh->verts[i].vy;
//			jiggledVerts[i].vz=mesh->verts[i].vz;
//		}
	}
	
	//transformVertexListA(mesh->verts, mesh->n_verts, tfv, tfd);
	transformVertexListA(jiggledVerts, mesh->n_verts, tfv, tfd);

// This should really by in the (or an alternative) transformvertexlist function
// It scales the OTZ's down so that they actually fit into the size of the ordering table.
#ifdef MAP_SCALE_DEPTH_DOWN
	{
		for(i = 0; i < mesh->n_verts; i++)
			tfd[i] = tfd[i] >> MAP_SCALE_DEPTH_DOWN;
	}
#endif

	//test !!!!!
	for(i = 0; i < mesh->n_verts; i++)
		tfd[i] = tfd[i] >> 2;


	packet = (PACKET *)currentDisplayPage->primPtr;






// The MapDraw stores a count+list of quads and one of tris,
// rather than checking a "type" element for every polygon.
// (coz it only has those two types. No transparent. No double sided. No nothing)




#define si ((POLY_GT4*)packet)
#define op ((FMA_GT4 *)opcd)

	op = mesh->gt4s;

// Little note - Down-counters are quicker than up-counters because you can
// compare with zero rather than with a variable.
// "!=0" is also marginally faster than ">0" here.
// n_gt4s CAN be zero. Otherwise,a (do,while) loop would be ever-so-slightly faster.than the (for...) one.

	for(i = mesh->n_gt4s; i != 0; i--,op++)
	{
// Get average Z of the polygon & use as a depth pointer
		gte_ldsz4(GETD(op->vert0),GETD(op->vert1),GETD(op->vert2),GETD(op->vert3));
   		gte_avsz4();
		gte_stotz_cpu(depth);
		if(depth > min_depth && depth < max_depth)
		{
// Skip the poly if all the verts are off screen.
// A fairly hefty check, but one that gives us a 20% code-speed increase & 30% less gpu work.
//			if ( (GETX(op->vert0)+256)&(GETX(op->vert1)+256)&(GETX(op->vert2)+256)&(GETX(op->vert3)+256)&512) continue;
//			if ( (GETY(op->vert0)+128)&(GETY(op->vert1)+128)&(GETY(op->vert2)+128)&(GETY(op->vert3)+128)&256) continue;

// Here's an entertaining little optimisation for the above that does X & Y at the same time,
// masking off the low bits to avoid overflow problems, and knocks a further 10% off the code side of things
// (Looking at the source, the compiler has sufficient registers free for two of the three constants.
// Hence the speedup)
			if(
			((GETV(op->vert0) & 0xff80ff00) + 0x00800100)
			&
			((GETV(op->vert1) & 0xff80ff00) + 0x00800100)
			&
			((GETV(op->vert2) & 0xff80ff00) + 0x00800100)
			&
			((GETV(op->vert3) & 0xff80ff00) + 0x00800100)
			& 0x01000200
			)
			continue;


// Load 1st three vertices for a backface cull check
// (Yes, this check is better when placed after the offscreen one - Checked on PA)
			gte_ldsxy3(GETV(op->vert0), GETV(op->vert1), GETV(op->vert2));

/*			gte_nclip();
#ifdef CLIP_REGISTER
			gte_stopz_cpu(clipflag);
#else
			gte_stopz(&clipflag);
#endif

			if (clipflag<0)*/
/*			{
				char u,v;

// Put the polygon into the OT (Note that we re-use "depth" as one of the "t" variables)
				addPrimLen(ot+(depth), (si),12,t2);



//				*(u_long *)  (&si->u0) = *(u_long *) (&op->u0);		// Texture coords
//				*(u_long *)  (&si->u1) = *(u_long *) (&op->u1);
					
/*				if ( flags & ACTOR_SLIDYTEX )
					si->u0 = op->u0 + ((frame/2)%32);
				else
					si->u0 = op->u0;

				si->v0 = op->v0;

				si->code = op->code;

				if ( flags & ACTOR_SLIDYTEX )
					si->u1 = op->u1 + ((frame/2)%32);
				else
					si->u1 = op->u1;
				si->v1 = op->v1;

				si->clut = op->clut;
				si->tpage = op->tpage;

				gte_stsxy3_gt4(si);

				if ( flags & ACTOR_SLIDYTEX )
					si->u2 = op->u2 + ((frame/2)%32);
				else
					si->u2 = op->u2;
				si->v2 = op->v2;

				if ( flags & ACTOR_SLIDYTEX )
					si->u3 = op->u3 + ((frame/2)%32);
				else
					si->u3 = op->u3;
				si->v3 = op->v3;

				
						
//				*(u_long *)  (&si->u2) = *(u_long *) (&op->u2);
//				*(u_long *)  (&si->u3) = *(u_long *) (&op->u3);

//				*(u_long *)  (&si->x3) = *(u_long *) (&tfv[op->v3]);
				*(u_long *)  (&si->x3) = *(u_long *) (&GETV(op->vert3));
		
				*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
				*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
				*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
				*(u_long *)  (&si->r3) = *(u_long *) (&op->r3);
		
 				// SL: modge the RGBs accordingly
/* 				si->r0 = ((globalFadeVal*(short)si->r0)/255);
 				si->g0 = ((globalFadeVal*(short)si->g0)/255);
 				si->b0 = ((globalFadeVal*(short)si->b0)/255);
 				si->r1 = ((globalFadeVal*(short)si->r1)/255);
 				si->g1 = ((globalFadeVal*(short)si->g1)/255);
 				si->b1 = ((globalFadeVal*(short)si->b1)/255);
 				si->r2 = ((globalFadeVal*(short)si->r2)/255);
 				si->g2 = ((globalFadeVal*(short)si->g2)/255);
 				si->b2 = ((globalFadeVal*(short)si->b2)/255);
 				si->r3 = ((globalFadeVal*(short)si->r3)/255);
 				si->g3 = ((globalFadeVal*(short)si->g3)/255);
 				si->b3 = ((globalFadeVal*(short)si->b3)/255);*/

 				// SL: put in the additive poly...
 	//			setPolyGT4(si);
 				// SL: store the pre-modification tpage...
// 				oldtpage = si->tpage;
 				// make it additive...
// 				si->clut = EXPLORE_black_CLUT;
 	/*			si->code |= 2<<24;
 				si->tpage |= si->tpage<<16;*/

 		/*		// SL: hokay, setup the next poly...
 				BEGINPRIM(siNext, POLY_GT4);

 				// SL: copy in all the values using long copies...
 				*(u_long *)  (&siNext->x0) = *(u_long *) (&si->x0);
 				*(u_long *)  (&siNext->x1) = *(u_long *) (&si->x1);
 				*(u_long *)  (&siNext->x2) = *(u_long *) (&si->x2);
 				*(u_long *)  (&siNext->x3) = *(u_long *) (&si->x3);
 				*(u_long *)  (&siNext->u0) = *(u_long *) (&si->u0);		// Texture coords
 				*(u_long *)  (&siNext->u1) = *(u_long *) (&si->u1);
 				*(u_long *)  (&siNext->u2) = *(u_long *) (&si->u2);
 				*(u_long *)  (&siNext->u3) = *(u_long *) (&si->u3);
 				*(u_long *)  (&siNext->r0) = *(u_long *) (&si->r0);			// SL: RGBs
 				*(u_long *)  (&siNext->r1) = *(u_long *) (&si->r1);			// SL: RGBs
 				*(u_long *)  (&siNext->r2) = *(u_long *) (&si->r2);			// SL: RGBs
 				*(u_long *)  (&siNext->r3) = *(u_long *) (&si->r3);			// SL: RGBs

 				// SL: set it as the right type
 				setPolyGT4(siNext);
 				// SL: make it subtractive
 				siNext->clut = EXPLORE_black_CLUT;
 				siNext->code |= 2;
 				siNext->tpage = oldtpage | 64;*/


/**//*				u=op->u0 + ((frame/2)%32);
				v=op->v0;

				t1 = WATERANIM_1|(op->clut<<16)|WATER_TRANS_CLUT;

				u=op->u1 + ((frame/2)%32);
				v=op->v1;

				t2 = WATERANIM_1|(op->tpage<<16);

				*(u_long *)  (&si->u0) = t1;
				*(u_long *)  (&si->u1) = t2;

				gte_stsxy3_gt4(si);	// The first 3 x's & y's are already in the gte, so we may as well use 'em

				u=op->u2 + ((frame/2)%32);
				v=op->v2;
				t1 = WATERANIM_2;

				u=op->u3 + ((frame/2)%32);
				v=op->v3;
				t2 = WATERANIM_2;

				*(u_long *)  (&si->u2) = t1;
				*(u_long *)  (&si->u3) = t2;
				*(u_long *)  (&si->x3) = *(u_long *) (&GETV(op->vert3));

				t1 = (*(u_long *) (&op->r0))|WATER_TRANS_CODE;
				t2 = *(u_long *) (&op->r1);

				*(u_long *)  (&si->r0) = t1;
				*(u_long *)  (&si->r1) = t2;

				t1 = *(u_long *) (&op->r2);
				t2 = *(u_long *) (&op->r3);

				*(u_long *)  (&si->r2) = t1;
				*(u_long *)  (&si->r3) = t2;


				packet = ADD2POINTER(packet,sizeof(POLY_GT4));
			}
		}
	}

#undef si
#undef op

// That's the quads done, now let's do the triangles...

#define si ((POLY_GT3*)packet)
#define op ((FMA_GT3 *)opcd)

	op = mesh->gt3s;
	for(i = mesh->n_gt3s; i != 0; i--,op++)
	{
// Get average Z of the polygon & use as a depth pointer
		gte_ldsz3(GETD(op->vert0),GETD(op->vert1),GETD(op->vert2));
   		gte_avsz3();
		gte_stotz_cpu(depth);

		if(depth > min_depth && depth < max_depth)
		{

// Skip the poly if all the verts are off screen.
// A fairly hefty check, but one that gives us a 20% code-speed increase & 30% less gpu work.

//			if ( (GETX(op->vert0)+256)&(GETX(op->vert1)+256)&(GETX(op->vert2)+256)&512) continue;
//			if ( (GETY(op->vert0)+128)&(GETY(op->vert1)+128)&(GETY(op->vert2)+128)&256) continue;

// Optimisation - X & Y at the same time
			if(
			((GETV(op->vert0) & 0xff80ff00) + 0x00800100)
			&
			((GETV(op->vert1) & 0xff80ff00) + 0x00800100)
			&
			((GETV(op->vert2) & 0xff80ff00) + 0x00800100)
			& 0x01000200
			)
			continue;


// Load 1st three vertices for a backface cull
			gte_ldsxy3(GETV(op->vert0), GETV(op->vert1), GETV(op->vert2));

/*			gte_nclip();
#ifdef CLIP_REGISTER
			gte_stopz_cpu(clipflag);
#else
			gte_stopz(&clipflag);
#endif
			if(clipflag < 0)*/
/*			{
				char u,v;
// Put the polygon into the OT (Note that we re-use "depth" as one of the "t" variables)
				addPrimLen(ot+(depth), (si),9,t2);
// Copy the polygon data over into the OT
//#ifdef TEMP_STORE

				u=op->u0;
				v=op->v0;

				t1 = (u+(rcos(frame)>>9))|(v+(rsin(frame)>>9))|op->clut|WATER_TRANS_CLUT;

				t2 = *(u_long *) (&op->u1);
				*(u_long *)  (&si->u0) = t1;
				*(u_long *)  (&si->u1) = t2;
				gte_stsxy3_gt3(si);
				t1 = *(u_long *) (&op->u2);
				t2 = (*(u_long *) (&op->r0))|WATER_TRANS_CODE;
				*(u_long *)  (&si->u2) = t1;
				*(u_long *)  (&si->r0) = t2;
				t1 = *(u_long *) (&op->r1);
				t2 = *(u_long *) (&op->r2);
				*(u_long *)  (&si->r1) = t1;
				*(u_long *)  (&si->r2) = t2;
/*#else
				*(u_long *)  (&si->u0) = (*(u_long *) (&op->u0))|WATER_TRANS_CLUT;
				*(u_long *)  (&si->u1) = *(u_long *) (&op->u1);
				gte_stsxy3_gt3(si);
				*(u_long *)  (&si->u2) = *(u_long *) (&op->u2);
				*(u_long *)  (&si->r0) = (*(u_long *) (&op->r0))|WATER_TRANS_CODE;
				*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
				*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
#endif*/

/*				packet = ADD2POINTER(packet,sizeof(POLY_GT3));
			}
		}
	}

#undef si
#undef op
#undef xy


	currentDisplayPage->primPtr = (char *)packet;

//	FREE(jiggledVerts);
//	jiggledVerts=NULL;
}*/



// Set up the PSX's transformation matrix for a landscape segment,
// or possibly for a NON-ROTATING platform.
void MapDraw_SetMatrix(FMA_MESH_HEADER *mesh, short posx, short posy, short posz)
{
	MATRIX tx;

	gte_SetRotMatrix(&GsWSMATRIX);
	gte_SetTransMatrix(&GsWSMATRIX);

	mesh->posx = posx;
	mesh->posy = -posy;
	mesh->posz = posz;

	// Unnecessary maths for landscape segments, where pos is always zero.
	gte_ldlvl( &mesh->posx);
	gte_rtirtr();
	gte_stlvl(&tx.t);

	gte_SetRotMatrix(&GsWSMATRIX);
	gte_SetTransMatrix(&tx);

}



#define OFFLEFT		1
#define OFFRIGHT	2
#define OFFUP		4
#define OFFDOWN		8
#define OFFFRONT	16
#define OFFBACK		32


// Run through a list of meshes, checking if they're visible, & drawing them if so.

void MapDraw_DrawFMA_World(FMA_WORLD *world)
{
#ifdef MAP_SCALE_ADJUST
	MATRIX temp;
#endif
	int i;
//	int removed_models;
	FMA_MESH_HEADER **mesh;
	mesh = ADD2POINTER(world,sizeof(FMA_WORLD));



// If we're rendering the map to a different integer scale to the objects, then we need to
// move the camera away from the increased-scale map for the duration of the map-plotter.
// Get rid of the relevant #define at the top of this file when everything's unified
#ifdef MAP_SCALE_ADJUST
	{
		temp = GsWSMATRIX;
		GsWSMATRIX.t[0] = GsWSMATRIX.t[0] << (MAP_SCALE_DEPTH_DOWN);
		GsWSMATRIX.t[1] = GsWSMATRIX.t[1] << (MAP_SCALE_DEPTH_DOWN);
		GsWSMATRIX.t[2] = GsWSMATRIX.t[2] << (MAP_SCALE_DEPTH_DOWN);
	}
#endif

//	printf("draw %d meshes\n",world->n_meshes);
//	removed_models=0;
	for(i = world->n_meshes; i != 0; i--, mesh++)
	{
// TBD - Do A clip-check on the mesh before calling
		MapDraw_SetMatrix(*mesh, 0, 0, 0);

		if(MapDraw_ClipCheck(*mesh))
		{
	//		if((*mesh)->flags & MESH_ISWATER)
	//		{
	//			MapDraw_DrawFMA_Mesh2Water(*mesh);
	//		}
	//		else
	//		{
				MapDraw_DrawFMA_Mesh2(*mesh);
	//		}

		}

		else
		{
		//	removed_models++;
		}

	}

// diagnostic message
//	if(removed_models)
//	{
//		printf("removed %d models\n",removed_models);
//	}

#ifdef MAP_SCALE_ADJUST
	{
		GsWSMATRIX = temp;

	}
#endif

}







// This is what to call to clip and draw a moving platform
/*void MapDraw_DrawFMA_Mesh ( FMA_MESH_HEADER *mesh )
{
#ifdef MAP_SCALE_ADJUST
	MATRIX temp;
#endif

// If we're rendering the map to a different integer scale to the objects, then we need to
// move the camera away from the increased-scale map for the duration of the map-plotter.
// Get rid of the relevant #define at the top of this file when everything's unified
#ifdef MAP_SCALE_ADJUST
	{
		temp = GsWSMATRIX;
		GsWSMATRIX.t[0] = GsWSMATRIX.t[0] << (MAP_SCALE_DEPTH_DOWN);
		GsWSMATRIX.t[1] = GsWSMATRIX.t[1] << (MAP_SCALE_DEPTH_DOWN);
		GsWSMATRIX.t[2] = GsWSMATRIX.t[2] << (MAP_SCALE_DEPTH_DOWN);
	}
#endif

	MapDraw_SetPlatformMatrix(mesh);

	if(MapDraw_ClipCheck(mesh))
	{
//		MapDraw_DrawFMA_Mesh2(mesh);
		MapDraw_DrawFMA_Mesh2Water ( mesh, flags );
	}

#ifdef MAP_SCALE_ADJUST
	{
		GsWSMATRIX = temp;
	}
#endif

}*/


// Check to see if a frogger map shape is on-screen
// (Assumes the projection matrix has already been set up for this shape)
int MapDraw_ClipCheck(FMA_MESH_HEADER *mesh)
{
	ULONG check,totalchecks;
	SHORTXY			*xy;
	int v;

// "9", coz the transformer only deals in multiples of three,
// (or at least, it did the last time I looked at it)
// and this array is on the stack, so overrunning would be a seriously bad idea.
	VERT pBBox[9];


// Get the 8 corners of the object's bounding box, & transform them.
	for(v=0; v<8; v++)
	{
		pBBox[v].vx=(v&1)?mesh->minx:mesh->maxx;
		pBBox[v].vy=(v&2)?mesh->miny:mesh->maxy;
		pBBox[v].vz=(v&4)?mesh->minz:mesh->maxz;
	}
	transformVertexListA(pBBox, 9, transformedVertices, transformedDepths);


// Check to see whether ALL of the vertices are on the outisde of ANY of the 6 planes of the view-frustrum
	(ULONG*)xy=transformedVertices;
	totalchecks=0xffff;
	for(v=0; v<8; v++)
	{
		check=0;
		if(xy[v].x<-256)
			check|=OFFLEFT;
		if(xy[v].x>256 )
			check|=OFFRIGHT;
		if(xy[v].y<-128)
			check|=OFFUP;
		if(xy[v].y>128 )
			check|=OFFDOWN;

#ifdef MAP_SCALE_DEPTH_DOWN
	  	if((transformedDepths[v]) <= (MIN_MAP_DEPTH<<(2+MAP_SCALE_DEPTH_DOWN)))
			check|=OFFFRONT;

		if((transformedDepths[v]) >= (MAX_MAP_DEPTH<<(2+MAP_SCALE_DEPTH_DOWN)))
			check |= OFFBACK;
#else
	  	if((transformedDepths[v]) <= (MIN_MAP_DEPTH<<2))
			check|=OFFFRONT;

		if((transformedDepths[v]) >= (MAX_MAP_DEPTH<<2))
			check |= OFFBACK;
#endif

		totalchecks&=check;
	}

	return(totalchecks==0);
}

// ===================================================
//              Platform-related code
// Note that this is ALL kept after the map code, in
// order that the map code scrunches itself into the
// instruction cache.
// ===================================================

// Set up the PSX's transformation matrix for a platform segment,
// Calculating the rot-matrix is, of course, only neccesary if the thing's rotated
// In fact, it might be nicer to cache this thing somewhere rather than recalculating it all the time
void MapDraw_SetPlatformMatrix(FMA_MESH_HEADER *mesh)
{
	MATRIX tx;
	MATRIX tx1;

// If we're using quaternions, get the matrix from (xyzw) this way
//	ShortquaternionGetMatrix((SHORTQUAT *)(&mesh->rotx), &tx1);

// If we're using xyz angles, get the matrix from (xyz) this way
	RotMatrixYXZ_gte((SVECTOR *)&mesh->rotx,&tx1);


	gte_MulMatrix0(&GsWSMATRIX,&tx1,&tx);

	gte_SetRotMatrix(&GsWSMATRIX);
	gte_SetTransMatrix(&GsWSMATRIX);

	gte_ldlvl(&mesh->posx);
	gte_rtirtr();
	gte_stlvl(&tx.t);

	gte_SetRotMatrix(&tx);
	gte_SetTransMatrix(&tx);
}





/// OLD CODE


/*	static SVECTOR *jiggledVerts=NULL;

#ifdef TEMP_STORE
// I've re-jigged the code, so "depth" is re-usable, & shares register with t1
// Also, now, clipflag shares a register with t2
	register u_long t1 asm("$16");
	register u_long t2 asm("$20");
#endif

	register PACKET * packet asm("$17");
	register char *opcd asm("$18");

//bb
//	register long *tfv = transformedVertices;
//  register long *tfd = transformedDepths;
//	long *tfv = (long*)transformedVertices;
//	long *tfd = (long*)transformedDepths;
	long *tfv;
	long *tfd;

// These macros are for an optimisation that requires all our vertex numbers to be multiplied by four,
// which means they're byte offsets into the tfv/tfd table rather than int-array indices
// Essentially
// tfv[n] and tfd[n], where tfv/tfd are arrays of ints, have now been replaced by these macros,
// where "n" is pre-shifted at PC map-compile time. Thus saving a shift instruction per vertex per polygon.
#define GETX(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->x )
#define GETY(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->y )
#define GETV(n)(  *(u_long *)( (int)(tfv) +(n) ) )
#define GETD(n)(  *(u_long *)( (int)(tfd) +(n) ) )


// "depth" is forced to be a CPU register so we can use the fast gte macro
// (Also note, it's shared with a temporary variable "t1")
	register long depth asm("$16");




	unsigned long *ot = currentDisplayPage->ot+mesh->extra_depth;

	int i;
//#ifdef CLIP_REGISTER
//	register long clipflag asm("$20");
//#else
//	long clipflag;
//#endif

	int min_depth = MIN_MAP_DEPTH + mesh->extra_depth;
	int max_depth = MAX_MAP_DEPTH + mesh->extra_depth;

	tfv = (long*)transformedVertices;
	tfd = (long*)transformedDepths;

	if(max_depth > 1024-mesh->extra_depth)
		max_depth = 1024-mesh->extra_depth;


	if(!jiggledVerts)
		jiggledVerts = MALLOC0(sizeof(SVECTOR)*700);

	for(i=0; i<mesh->n_verts; i++)
	{
		if(mesh->verts[i].pad>2)
		{
			jiggledVerts[i].vx=mesh->verts[i].vx+(rsin((frame<<6)+(mesh->verts[i].vx&(mesh->verts[i].vz)))>>6);
			jiggledVerts[i].vy=mesh->verts[i].vy+(rsin((frame<<6)+(mesh->verts[i].vx|(mesh->verts[i].vz)))>>6);
			jiggledVerts[i].vz=mesh->verts[i].vz+(rsin((frame<<6)+(mesh->verts[i].vx^(mesh->verts[i].vz)))>>6);
		}
		else
		{
			jiggledVerts[i].vx=mesh->verts[i].vx;
			jiggledVerts[i].vy=mesh->verts[i].vy;
			jiggledVerts[i].vz=mesh->verts[i].vz;
		}
	}
	
//	transformVertexListA(mesh->verts, mesh->n_verts, tfv, tfd);
	transformVertexListA(jiggledVerts, mesh->n_verts, tfv, tfd);

// This should really by in the (or an alternative) transformvertexlist function
// It scales the OTZ's down so that they actually fit into the size of the ordering table.
#ifdef MAP_SCALE_DEPTH_DOWN
	{
		for(i = 0; i < mesh->n_verts; i++)
			tfd[i] = tfd[i] >> MAP_SCALE_DEPTH_DOWN;
	}
#endif

	//test !!!!!
	for(i = 0; i < mesh->n_verts; i++)
//mmfrig		tfd[i] = tfd[i] >> 3;
		tfd[i] = tfd[i] >> 2;


	packet = (PACKET *)currentDisplayPage->primPtr;






// The MapDraw stores a count+list of quads and one of tris,
// rather than checking a "type" element for every polygon.
// (coz it only has those two types. No transparent. No double sided. No nothing)




#define si ((POLY_GT4*)packet)
#define op ((FMA_GT4 *)opcd)

	op = mesh->gt4s;

// Little note - Down-counters are quicker than up-counters because you can
// compare with zero rather than with a variable.
// "!=0" is also marginally faster than ">0" here.
// n_gt4s CAN be zero. Otherwise,a (do,while) loop would be ever-so-slightly faster.than the (for...) one.

	for(i = mesh->n_gt4s; i != 0; i--,op++)
	{
// Get average Z of the polygon & use as a depth pointer
		gte_ldsz4(GETD(op->vert0),GETD(op->vert1),GETD(op->vert2),GETD(op->vert3));
   		gte_avsz4();
		gte_stotz_cpu(depth);
		if(depth > min_depth && depth < max_depth)
		{
// Skip the poly if all the verts are off screen.
// A fairly hefty check, but one that gives us a 20% code-speed increase & 30% less gpu work.
//			if ( (GETX(op->vert0)+256)&(GETX(op->vert1)+256)&(GETX(op->vert2)+256)&(GETX(op->vert3)+256)&512) continue;
//			if ( (GETY(op->vert0)+128)&(GETY(op->vert1)+128)&(GETY(op->vert2)+128)&(GETY(op->vert3)+128)&256) continue;

// Here's an entertaining little optimisation for the above that does X & Y at the same time,
// masking off the low bits to avoid overflow problems, and knocks a further 10% off the code side of things
// (Looking at the source, the compiler has sufficient registers free for two of the three constants.
// Hence the speedup)
			if(
			((GETV(op->vert0) & 0xff80ff00) + 0x00800100)
			&
			((GETV(op->vert1) & 0xff80ff00) + 0x00800100)
			&
			((GETV(op->vert2) & 0xff80ff00) + 0x00800100)
			&
			((GETV(op->vert3) & 0xff80ff00) + 0x00800100)
			& 0x01000200
			)
			continue;


// Load 1st three vertices for a backface cull check
// (Yes, this check is better when placed after the offscreen one - Checked on PA)
			gte_ldsxy3(GETV(op->vert0), GETV(op->vert1), GETV(op->vert2));

/*			gte_nclip();
#ifdef CLIP_REGISTER
			gte_stopz_cpu(clipflag);
#else
			gte_stopz(&clipflag);
#endif

			if (clipflag<0)*/
		/*	{
				char u,v;

// Put the polygon into the OT (Note that we re-use "depth" as one of the "t" variables)
				addPrimLen(ot+(depth), (si),12,t2);
// Copy the polygon data over into the OT
//#ifdef TEMP_STORE
				u=op->u0;
				v=op->v0;
				t1 = WATERANIM_1|(op->clut<<16)|WATER_TRANS_CLUT;

				u=op->u1;
				v=op->v1;
				t2 = WATERANIM_1|(op->tpage<<16);

				*(u_long *)  (&si->u0) = t1;
				*(u_long *)  (&si->u1) = t2;
				gte_stsxy3_gt4(si);	// The first 3 x's & y's are already in the gte, so we may as well use 'em

				u=op->u2;
				v=op->v2;
				t1 = WATERANIM_2;

				u=op->u3;
				v=op->v3;
				t2 = WATERANIM_2;

//				t1 = *(u_long *) (&op->u2);
//				t2 = *(u_long *) (&op->u3);
				*(u_long *)  (&si->u2) = t1;
				*(u_long *)  (&si->u3) = t2;
				*(u_long *)  (&si->x3) = *(u_long *) (&GETV(op->vert3));
				t1 = (*(u_long *) (&op->r0))|WATER_TRANS_CODE;
				t2 = *(u_long *) (&op->r1);
				*(u_long *)  (&si->r0) = t1;
				*(u_long *)  (&si->r1) = t2;
				t1 = *(u_long *) (&op->r2);
				t2 = *(u_long *) (&op->r3);
				*(u_long *)  (&si->r2) = t1;
				*(u_long *)  (&si->r3) = t2;
/*#else
				*(u_long *)  (&si->u0) = (*(u_long *) (&op->u0))|WATER_TRANS_CLUT;
				*(u_long *)  (&si->u1) = *(u_long *) (&op->u1);
				gte_stsxy3_gt4(si);
				*(u_long *)  (&si->u2) = *(u_long *) (&op->u2);
				*(u_long *)  (&si->u3) = *(u_long *) (&op->u3);
				*(u_long *)  (&si->x3) = *(u_long *) (&GETV(op->vert3));
				*(u_long *)  (&si->r0) = (*(u_long *) (&op->r0))|WATER_TRANS_CODE;
				*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
				*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
				*(u_long *)  (&si->r3) = *(u_long *) (&op->r3);
#endif*/

	/*			packet = ADD2POINTER(packet,sizeof(POLY_GT4));
			}
		}
	}

#undef si
#undef op

// That's the quads done, now let's do the triangles...

#define si ((POLY_GT3*)packet)
#define op ((FMA_GT3 *)opcd)

	op = mesh->gt3s;
	for(i = mesh->n_gt3s; i != 0; i--,op++)
	{
// Get average Z of the polygon & use as a depth pointer
		gte_ldsz3(GETD(op->vert0),GETD(op->vert1),GETD(op->vert2));
   		gte_avsz3();
		gte_stotz_cpu(depth);

		if(depth > min_depth && depth < max_depth)
		{

// Skip the poly if all the verts are off screen.
// A fairly hefty check, but one that gives us a 20% code-speed increase & 30% less gpu work.

//			if ( (GETX(op->vert0)+256)&(GETX(op->vert1)+256)&(GETX(op->vert2)+256)&512) continue;
//			if ( (GETY(op->vert0)+128)&(GETY(op->vert1)+128)&(GETY(op->vert2)+128)&256) continue;

// Optimisation - X & Y at the same time
			if(
			((GETV(op->vert0) & 0xff80ff00) + 0x00800100)
			&
			((GETV(op->vert1) & 0xff80ff00) + 0x00800100)
			&
			((GETV(op->vert2) & 0xff80ff00) + 0x00800100)
			& 0x01000200
			)
			continue;


// Load 1st three vertices for a backface cull
			gte_ldsxy3(GETV(op->vert0), GETV(op->vert1), GETV(op->vert2));

/*			gte_nclip();
#ifdef CLIP_REGISTER
			gte_stopz_cpu(clipflag);
#else
			gte_stopz(&clipflag);
#endif
			if(clipflag < 0)*/
	/*		{
				char u,v;
// Put the polygon into the OT (Note that we re-use "depth" as one of the "t" variables)
				addPrimLen(ot+(depth), (si),9,t2);
// Copy the polygon data over into the OT
//#ifdef TEMP_STORE

				u=op->u0;
				v=op->v0;

				t1 = (u+(rcos(frame)>>9))|(v+(rsin(frame)>>9))|op->clut|WATER_TRANS_CLUT;

				t2 = *(u_long *) (&op->u1);
				*(u_long *)  (&si->u0) = t1;
				*(u_long *)  (&si->u1) = t2;
				gte_stsxy3_gt3(si);
				t1 = *(u_long *) (&op->u2);
				t2 = (*(u_long *) (&op->r0))|WATER_TRANS_CODE;
				*(u_long *)  (&si->u2) = t1;
				*(u_long *)  (&si->r0) = t2;
				t1 = *(u_long *) (&op->r1);
				t2 = *(u_long *) (&op->r2);
				*(u_long *)  (&si->r1) = t1;
				*(u_long *)  (&si->r2) = t2;
/*#else
				*(u_long *)  (&si->u0) = (*(u_long *) (&op->u0))|WATER_TRANS_CLUT;
				*(u_long *)  (&si->u1) = *(u_long *) (&op->u1);
				gte_stsxy3_gt3(si);
				*(u_long *)  (&si->u2) = *(u_long *) (&op->u2);
				*(u_long *)  (&si->r0) = (*(u_long *) (&op->r0))|WATER_TRANS_CODE;
				*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
				*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
#endif*/

	/*			packet = ADD2POINTER(packet,sizeof(POLY_GT3));
			}
		}
	}

#undef si
#undef op
#undef xy


	currentDisplayPage->primPtr = (char *)packet;
*/

void MapDraw_DrawFMA_Water ( WATER *cur )
{

	FMA_WORLD *waterObj = cur->fma_water;

	int i;

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
	}
	// ENDFOR

}