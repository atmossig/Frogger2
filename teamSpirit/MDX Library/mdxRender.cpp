/*

	This file is part of the M libraries,

	File		: 
	Programmer	: Matthew Cloy
	Date		: 

----------------------------------------------------------------------------------------------- */
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <crtdbg.h>
#include <stdio.h>

#include "mdxDDraw.h"
#include "mdxD3D.h"
#include "mdxMath.h"
#include "mdxTexture.h"
#include "mdxObject.h"
#include "mdxActor.h"
#include "mdxCRC.h"
#include "mdxTiming.h"
#include "mgeReport.h"
#include "gelf.h"
#include "mdxLandscape.h"
#include "mdxRender.h"
#include "mdxPoly.h"
#include "mdxProfile.h"
#include "math.h"

#ifdef __cplusplus
extern "C"
{
#endif

unsigned long drawingSwampy;

#define MAX_OBJECT_VERTICES 8000
#define SETALPHA(rgba, x) ((((long)(x)) << 24) | ((rgba & 0x00ffffff)))
#define MODALPHA(rgba, x) ((((long) ((x*(rgba>>24))>>8)) << 24) | ((rgba & 0x00ffffff)))
#define BETWEEN(x,a,b) ((x>a) && (x<b))

float globalXLU;
float globalXLU2;
MDX_MATRIX vMatrix;
MDX_VECTOR tV[MAX_OBJECT_VERTICES];
float tMx[MAX_OBJECT_VERTICES];
float tMy[MAX_OBJECT_VERTICES];
float tMz[MAX_OBJECT_VERTICES];
MDX_VECTOR tN[MAX_OBJECT_VERTICES];

short facesON[3] = {0,1,2};

float nearClip = 1;
float farClip = 2500;

float horizClip = 3000;
float vertClip = 2500;

long DIST=-10;
long FOV=450;
float oneOver[65535];
float halfWidth;
float halfHeight;


// Outcode enum
enum {OUTCODE_LEFT, OUTCODE_RIGHT, OUTCODE_TOP, OUTCODE_BOTTOM};

// Outcode macros for clipping.
#define OUTCODEF_LEFT	(1<<OUTCODE_LEFT)
#define OUTCODEF_RIGHT	(1<<OUTCODE_RIGHT)
#define OUTCODEF_TOP	(1<<OUTCODE_TOP)
#define OUTCODEF_BOTTOM	(1<<OUTCODE_BOTTOM)

// Calculate the actual outcode
#define CALC_OUTCODE(x,y,x0,y0,x1,y1) \
						((((y)>=(y1))<<OUTCODE_BOTTOM)| \
						 (((y)<=(y0))<<OUTCODE_TOP)| \
						 (((x)>=(x1))<<OUTCODE_RIGHT)| \
						 (((x)<=(x0))<<OUTCODE_LEFT))

// The 2D clip Volume.
float clx0 = 1,
      cly0 = 1,
	  clx1 = 639,
	  cly1 = 479;

// Subtracted Square!
//#define sbsqr(x,y) ((x-y)*(x-y))

float xl = 1;


/*---------------------------------------------------------------------------------------------
	Function	: calcIntVertex
	Parameters	: (D3DTLVERTEX *vOut, int outcode, D3DTLVERTEX *v0,D3DTLVERTEX *v1, float cx0, float cy0, float cx1, float cy1)
	Returns		: inline int 
*/

float   rCol,gCol,bCol;

int calcIntVertex(D3DTLVERTEX *vOut, int outcode, D3DTLVERTEX *v0,D3DTLVERTEX *v1, float cx0, float cy0, float cx1, float cy1)
{
	float segLen, totalLen, vt;
	long r1,g1,b1,a1;
	long r2,g2,b2,a2;

	switch(outcode)
	{
		case OUTCODE_LEFT:
			segLen = cx0-v0->sx;
			totalLen = v1->sx-v0->sx;
			vt = segLen/totalLen;
			vOut->sx = cx0;
			vOut->sy = v0->sy+((v1->sy-v0->sy)*vt);
			break;
		case OUTCODE_RIGHT:
			segLen = cx1-v0->sx;
			totalLen = v1->sx-v0->sx;
			vt = segLen/totalLen;
			vOut->sx = cx1;
			vOut->sy = v0->sy+((v1->sy-v0->sy)*vt);
			break;
		case OUTCODE_TOP:
			segLen = cy0-v0->sy;
			totalLen = v1->sy-v0->sy;
			vt = segLen/totalLen;
			vOut->sx = v0->sx+((v1->sx-v0->sx)*vt);
			vOut->sy = cy0;
			break;
		case OUTCODE_BOTTOM:
			segLen = cy1-v0->sy;
			totalLen = v1->sy-v0->sy;
			vt = segLen/totalLen;
			vOut->sx = v0->sx+((v1->sx-v0->sx)*vt);
			vOut->sy = cy1;
			break;		
	}
	if (vOut->sx > 6400000)
		vOut->sx += 0;
	if (vOut->sy > 4800000)
		vOut->sy += 0;

	vOut->tu = (v0->tu+((v1->tu-v0->tu)*vt));
    vOut->tv = (v0->tv+((v1->tv-v0->tv)*vt));
	vOut->sz = (v0->sz+((v1->sz-v0->sz)*vt));
	vOut->rhw = 1/vOut->sz;
	
	vOut->specular = D3DRGBA(0,0,0,0);
	
	a1 = RGBA_GETALPHA(v0->color);
	r1 = RGBA_GETRED(v0->color);
	g1 = RGBA_GETGREEN(v0->color);
	b1 = RGBA_GETBLUE(v0->color);

	a2 = RGBA_GETALPHA(v1->color);
	r2 = RGBA_GETRED(v1->color);
	g2 = RGBA_GETGREEN(v1->color);
	b2 = RGBA_GETBLUE(v1->color);

	vOut->color = RGBA_MAKE ((long)(r1+(r2-r1)*vt),(long)(g1+(g2-g1)*vt),(long)(b1+(b2-b1)*vt),(long)(a1+(a2-a1)*vt));

	return !((vOut->sx==v0->sx)&&(vOut->sy==v0->sy));
}

void Clip3DPolygon (D3DTLVERTEX in[3], LPDIRECTDRAWSURFACE7 texture,MDX_TEXENTRY *tEntry)
{
	D3DTLVERTEX		vList[10];
	int				out0, out1;
	D3DTLVERTEX 	*v0ptr, *v1ptr, vBuf1[8], vBuf2[8], *vIn, *vOut, *vTmp;
	int				vInCount, vOutCount;
	int				sideLp, vertLp, sideMask;
	short			faceList[8*3];
	long			numFaces,i,j;
	// Set up in & out buffers											

	vIn = vBuf1;
	
	memcpy (vIn,in,sizeof(D3DTLVERTEX)*3);
	
	vInCount = 3;
	vOut = vBuf2;
	
	vOutCount = 0;

	for(sideLp=0; sideLp<4; sideLp++)
	{
		sideMask = 1<<sideLp;
		for(vertLp=0; vertLp<vInCount; vertLp++)
		{
			v0ptr = &vIn[vertLp];
			if ((vertLp+1)<vInCount)
				v1ptr = &vIn[(vertLp+1)];
			else
				v1ptr = vIn;

			out0 = CALC_OUTCODE(v0ptr->sx,v0ptr->sy, clx0,cly0,clx1,cly1);
			out1 = CALC_OUTCODE(v1ptr->sx,v1ptr->sy, clx0,cly0,clx1,cly1);
			if ((out0 & sideMask)==0)		// v0 on
			{
				// add v0 to output
				memcpy(&vOut[vOutCount++], v0ptr, sizeof(D3DTLVERTEX));
				if (out1 & sideMask)		// v0 on, v1 off
				{
					// add intersection to output
					if (calcIntVertex(&vOut[vOutCount], sideLp, v0ptr, v1ptr, clx0,cly0,clx1,cly1))
						vOutCount++;
				}
			}
			else
			{
				if ((out1 & sideMask)==0)	// v0 off, v1 on
				{
					// add intersection to output
					if (calcIntVertex(&vOut[vOutCount], sideLp, v1ptr, v0ptr, clx0,cly0,clx1,cly1))
						vOutCount++;
				}
			}
		}
		vTmp = vIn;					// Swap in & out bufs
		vIn = vOut;
		vOut = vTmp;
		vInCount = vOutCount;
		vOutCount = 0;
	}

	numFaces = 0;
	j=0;
	for (i=1; i<(vInCount-1); i++, j+=3)
	{
		faceList[j] = 0;
		faceList[j+1] = i;
		faceList[j+2] = i+1;
		numFaces++;
	}
	
	if (numFaces)
	{
		PushPolys(vIn,vInCount,faceList,j,texture,tEntry);
	}
}


void XfmPoint(MDX_VECTOR *vTemp2,MDX_VECTOR *in,MDX_MATRIX *d)
{
	float c[4][4];

	if (d)
	{
		guMtxCatF(d->matrix,vMatrix.matrix,c);
		guMtxXFMF(c,in->vx,in->vy,in->vz,&(vTemp2->vx),&(vTemp2->vy),&(vTemp2->vz));
	}
	else
		guMtxXFMF(vMatrix.matrix,in->vx,in->vy,in->vz,&(vTemp2->vx),&(vTemp2->vy),&(vTemp2->vz));
	
	if  (((vTemp2->vz+DIST)>nearClip) &&
		((vTemp2->vz+DIST)<farClip) &&
		((vTemp2->vx)>-horizClip) &&
		((vTemp2->vx)<horizClip) &&
		((vTemp2->vy)>-vertClip) &&
		((vTemp2->vy)<vertClip))
	{
		long x = (long)vTemp2->vz+DIST;
		float oozd = -FOV * oneOver[x];
			
		vTemp2->vx = halfWidth+(vTemp2->vx * oozd);
		vTemp2->vy = halfHeight+(vTemp2->vy * oozd);
	}
	else
		vTemp2->vz = 0;
}

void InitOneOverTable(void)
{
	int i;
	float rDiff = rXRes/640.0F;
	for(i=1; i<65535; i++)
		oneOver[i] = (rDiff)/(float)i;
	halfWidth = rXRes/2.0F;
	halfHeight = rYRes/2.0F;
}




void PCPrepareObject (MDX_OBJECT *obj, MDX_MESH *me, float m[4][4])
{
	float f[4][4];
	MDX_VECTOR *in;
	MDX_VECTOR *vTemp2;
	long i;

	in = obj->mesh->vertices;

	guMtxCatF(m,vMatrix.matrix,f);
	
	vTemp2 = tV;
	
	for (i=0; i<obj->mesh->numVertices; i++)
	{
		vTemp2->vx = (f[0][0]*in->vx)+(f[1][0]*in->vy)+(f[2][0]*in->vz)+(f[3][0]);
		vTemp2->vy = (f[0][1]*in->vx)+(f[1][1]*in->vy)+(f[2][1]*in->vz)+(f[3][1]);
		vTemp2->vz = (f[0][2]*in->vx)+(f[1][2]*in->vy)+(f[2][2]*in->vz)+(f[3][2]);

		if (((vTemp2->vz+DIST)>nearClip) &&
			(((vTemp2->vz+DIST)<farClip) &&
			((vTemp2->vx)>-horizClip) &&
			((vTemp2->vx)<horizClip) &&
			((vTemp2->vy)>-vertClip) &&
			((vTemp2->vy)<vertClip)))
		{
			long x = (long)vTemp2->vz+DIST;
			float oozd = -FOV * oneOver[x];
			
			vTemp2->vx = halfWidth+(vTemp2->vx * oozd);
			vTemp2->vy = halfHeight+(vTemp2->vy * oozd);
		}
		else
			vTemp2->vz = 0;

		vTemp2++;
		in++;
	}
}

void PCCalcModgeValues(MDX_OBJECT *obj)
{
	float *mTemp,*mTemp2,*mTemp3;
	MDX_VECTOR *in;
	long i;

	in = obj->mesh->vertices;
	mTemp = tMx;
	mTemp2 = tMz;
	mTemp3 = tMy;

	for (i=0; i<obj->mesh->numVertices; i++)
	{
		*mTemp  = (sinf((in->vz*(1.5/35)+sinf((in->vx*(1.3/49))+timeInfo.frameCount*(2.0/56.0)))+timeInfo.frameCount*(1.5/70.0))+1)*0.1;
		*mTemp2 = (cosf((in->vx*(1.5/49)+cosf((in->vz*(1.3/29))+timeInfo.frameCount*(2.0/66.0)))+timeInfo.frameCount*(1.5/66.0))+1)*0.1;
		*mTemp3 = (sinf(	(in->vz*(1.2/26))+cosf((in->vx*(1.3/49))+timeInfo.frameCount*(1.0/26.0))+timeInfo.frameCount*(1.0/26.0))+1)*5;
		mTemp++;
		mTemp2++;
		mTemp3++;
		in++;
	}
}

void PCPrepareModgyObject (MDX_OBJECT *obj, MDX_MESH *me, float m[4][4])
{
	float f[4][4],*mTemp,*mTemp2,ty;
	MDX_VECTOR *in;
	MDX_VECTOR *vTemp2;
	long i;

	in = obj->mesh->vertices;

	guMtxCatF(m,vMatrix.matrix,f);
	
	vTemp2 = tV;
	mTemp = tMy;
	
	for (i=0; i<obj->mesh->numVertices; i++)
	{
		ty = in->vy + *mTemp;

		vTemp2->vx = (f[0][0]*in->vx)+(f[1][0]*ty)+(f[2][0]*in->vz)+(f[3][0]);
		vTemp2->vy = (f[0][1]*in->vx)+(f[1][1]*ty)+(f[2][1]*in->vz)+(f[3][1]);
		vTemp2->vz = (f[0][2]*in->vx)+(f[1][2]*ty)+(f[2][2]*in->vz)+(f[3][2]);

		if (((vTemp2->vz+DIST)>nearClip) &&
			(((vTemp2->vz+DIST)<farClip) &&
			((vTemp2->vx)>-horizClip) &&
			((vTemp2->vx)<horizClip) &&
			((vTemp2->vy)>-vertClip) &&
			((vTemp2->vy)<vertClip)))
		{
			long x = (long)vTemp2->vz+DIST;
			float oozd = -FOV * oneOver[x];
			
			vTemp2->vx = halfWidth+(vTemp2->vx * oozd);
			vTemp2->vy = halfHeight+(vTemp2->vy * oozd);
		}
		else
			vTemp2->vz = 0;

		vTemp2++;
		mTemp++;
		in++;
	}
}

void PCPrepareObjectNormals(MDX_OBJECT *obj, MDX_MESH *mesh, float m[4][4])
{
	float f[4][4];
	MDX_VECTOR *in;
	MDX_VECTOR *vTemp2;
	long i,j;
	
	guMtxCatF(m,vMatrix.matrix,f);

	vTemp2 = tN;
	in = obj->mesh->vertexNormals;
	
			
	if (obj->mesh->vertexNormals)
	{
		// Go thru the affected vertices, and xfm the corresponding vertices
		for (i=0; i<obj->mesh->numVertices; i++)
		{
			// Transform by our matrix
			vTemp2->vx = (f[0][0]*in->vx)+(f[1][0]*in->vy)+(f[2][0]*in->vz);
			vTemp2->vy = (f[0][1]*in->vx)+(f[1][1]*in->vy)+(f[2][1]*in->vz);
			vTemp2->vz = (f[0][2]*in->vx)+(f[1][2]*in->vy)+(f[2][2]*in->vz);
			vTemp2++;
			in++;
		}
	}
}


void PCPrepareSkinnedObject(MDX_OBJECT *obj, MDX_MESH *mesh, float m[4][4])
{
	float f[4][4];
	MDX_VECTOR *in;
	MDX_VECTOR *vTemp2;
	long i,j;
	
	guMtxCatF(m,vMatrix.matrix,f);

	// Go thru the affected vertices, and xfm the corresponding vertices
	for (i=0; i<obj->numVerts; i++)
	{
		j = ((long *)(obj->effectedVerts))[i];
		if (j==0)
			j=0;
		// Setup Our pointers
		vTemp2 = &tV[j];
		in = &mesh->vertices[j];

		// Transform by our matrix
		vTemp2->vx = (f[0][0]*in->vx)+(f[1][0]*in->vy)+(f[2][0]*in->vz)+(f[3][0]);
		vTemp2->vy = (f[0][1]*in->vx)+(f[1][1]*in->vy)+(f[2][1]*in->vz)+(f[3][1]);
		vTemp2->vz = (f[0][2]*in->vx)+(f[1][2]*in->vy)+(f[2][2]*in->vz)+(f[3][2]);

		// Transform to screen space
		if (((vTemp2->vz+DIST)>nearClip) &&
			(((vTemp2->vz+DIST)<farClip) &&
			((vTemp2->vx)>-horizClip) &&
			((vTemp2->vx)<horizClip) &&
			((vTemp2->vy)>-vertClip) &&
			((vTemp2->vy)<vertClip)))
		{
			long x = (long)vTemp2->vz+DIST;
			float oozd = -FOV * oneOver[x];
			
			vTemp2->vx = halfWidth+(vTemp2->vx * oozd);
			vTemp2->vy = halfHeight+(vTemp2->vy * oozd);
		}
		else
			vTemp2->vz = 0;
	}
}

void PCPrepareSkinnedObjectNormals(MDX_OBJECT *obj, MDX_MESH *mesh, float m[4][4])
{
	float f[4][4];
	MDX_VECTOR *in;
	MDX_VECTOR *vTemp2;
	long i,j;
	
	guMtxCatF(m,vMatrix.matrix,f);

	if (mesh->vertexNormals)
	{
		// Go thru the affected vertices, and xfm the corresponding vertices
		for (i=0; i<obj->numVerts; i++)
		{
			j = ((long *)(obj->effectedVerts))[i];
			if (j==0)
				j=0;
			// Setup Our pointers
			vTemp2 = &tN[j];
			in = &mesh->vertexNormals[j];
	
			// Transform by our matrix
			vTemp2->vx = (f[0][0]*in->vx)+(f[1][0]*in->vy)+(f[2][0]*in->vz);
			vTemp2->vy = (f[0][1]*in->vx)+(f[1][1]*in->vy)+(f[2][1]*in->vz);
			vTemp2->vz = (f[0][2]*in->vx)+(f[1][2]*in->vy)+(f[2][2]*in->vz);
		}
	}
}

void __fastcall PCPrepareLandscape (MDX_LANDSCAPE *me)
{
	float f[4][4];
	MDX_VECTOR *in;
	D3DTLVERTEX *vTemp2;
	short *tFace;
	long i,x,j;
	float oozd;
	float a0,b0,c0,d0;
	float a1,b1,c1,d1;
	float a2,b2,c2,d2;

	a0 = vMatrix.matrix[0][0];
	a1 = vMatrix.matrix[0][1];
	a2 = vMatrix.matrix[0][2];

	b0 = vMatrix.matrix[1][0];
	b1 = vMatrix.matrix[1][1];
	b2 = vMatrix.matrix[1][2];

	c0 = vMatrix.matrix[2][0];
	c1 = vMatrix.matrix[2][1];
	c2 = vMatrix.matrix[2][2];

	d0 = vMatrix.matrix[3][0];
	d1 = vMatrix.matrix[3][1];
	d2 = vMatrix.matrix[3][2];

	in = me->vertices;
	vTemp2 = me->xfmVert;
	tFace = me->faceIndex;
	
	for (i=0,j=me->numFaces*3; j>0; i++,j--)
	{
		in = &me->vertices[*tFace];
		
		vTemp2->sx = (a0*in->vx)+(b0*in->vy)+(c0*in->vz)+d0;
		vTemp2->sy = (a1*in->vx)+(b1*in->vy)+(c1*in->vz)+d1;
		vTemp2->sz = (a2*in->vx)+(b2*in->vy)+(c2*in->vz)+d2+DIST;

		if (vTemp2->sz>nearClip)
		{
			x = (long)vTemp2->sz;
			oozd = -FOV * *(oneOver+x);
			
			vTemp2->sx = halfWidth+(vTemp2->sx * oozd);
			vTemp2->sy = halfHeight+(vTemp2->sy * oozd);
		
		}
		else
			vTemp2->sz = 0;

		vTemp2++;
		tFace++;
		in++;
	}
}

void PCRenderLandscape(MDX_LANDSCAPE *me)
{
	unsigned long x1on,x2on,x3on,y1on,y2on,y3on;
	short facesON[3] = {0,1,2};
	D3DTLVERTEX *v = me->xfmVert;
	LPDIRECTDRAWSURFACE7 *tex = me->textures;
	MDX_TEXENTRY **tEnt = me->tEntrys;

	for (int i=0; i<me->numFaces; i++)
	{
		if (v[0].sz && v[1].sz && v[2].sz)
		{
			x1on = BETWEEN(v[0].sx,clx0,clx1);
			x2on = BETWEEN(v[1].sx,clx0,clx1);
			x3on = BETWEEN(v[2].sx,clx0,clx1);
			y1on = BETWEEN(v[0].sy,cly0,cly1);
			y2on = BETWEEN(v[1].sy,cly0,cly1);
			y3on = BETWEEN(v[2].sy,cly0,cly1);
			v[0].sz *= 0.00025F;
			v[1].sz *= 0.00025F;
			v[2].sz *= 0.00025F;
			v[0].rhw = 1/v[0].sz;
			v[1].rhw = 1/v[1].sz;
			v[2].rhw = 1/v[2].sz;
			if ((x1on || x2on || x3on) && (y1on || y2on || y3on))
			{
				if ((x1on && x2on && x3on) && (y1on && y2on && y3on))
				{
					SampleGraph(i>>1);		
					PushPolys(v,3,facesON,3,*tex,*tEnt);
				}
				else
				{
					SampleGraph(i<<1);		
					Clip3DPolygon(v,*tex,*tEnt);
				}
			}	
		}
			tEnt++;
			tex++;
			v+=3;		
	}
}

unsigned long numObjDrawn;
void DrawLandscape(MDX_LANDSCAPE *me)
{
	if (CheckBoundingBox(me->bBox,&me->objMatrix)==0)
	{
		StartTimer(8,"Prepare landscape");	
		PCPrepareLandscape(me);
		EndTimer(8);

		StartTimer(9,"Render landscape");	
		PCRenderLandscape(me);
		EndTimer(9);
		numObjDrawn++;
	}

	if (me->next)
		DrawLandscape(me->next);

	if (me->children)
		DrawLandscape(me->children);


}

void DrawObject(MDX_OBJECT *obj, int skinned, MDX_MESH *masterMesh)
{
	SaveFrame;

	if (obj->flags & OBJECT_FLAGS_GLOW)
	{
		MDX_VECTOR v;
		
		v.vx = obj->objMatrix.matrix[3][0];
		v.vy = obj->objMatrix.matrix[3][1];
		v.vz = obj->objMatrix.matrix[3][2];
		
		AddHalo(&v,vMatrix.matrix[2][0],vMatrix.matrix[2][2]);
		SwapFrame(MA_FRAME_GLOW);		
	}
	
	if (obj->flags & OBJECT_FLAGS_XLU)
		SwapFrame(MA_FRAME_XLU);

	if (obj->flags & OBJECT_FLAGS_ADDITIVE)
		SwapFrame(MA_FRAME_ADDITIVE);		
	
	// If we are a skinned object then we just need to prepare all the skinned vertices, so do that for this sub-object.
	if (skinned)
	{
		PCPrepareSkinnedObject(obj, masterMesh,  obj->objMatrix.matrix);
		if (skinned == 2)
			PCPrepareSkinnedObjectNormals(obj, masterMesh, obj->objMatrix.matrix);
	}
	else
	{ 
		if (!(obj->flags & OBJECT_FLAGS_CLIPPED))
		{
			if (obj->mesh)
			{
				globalXLU2 = (((float)obj->xlu) / ((float)0xff)) * globalXLU;
				
				if ((obj->flags & OBJECT_FLAGS_MODGE) || (obj->flags & OBJECT_FLAGS_WAVE))
					PCCalcModgeValues(obj);					
					
				if (obj->flags & OBJECT_FLAGS_WAVE)
					PCPrepareModgyObject(obj, obj->mesh,  obj->objMatrix.matrix);
				else
					PCPrepareObject(obj, obj->mesh,  obj->objMatrix.matrix);

				if (obj->phong)
				{
					//phong = obj->phong;
					PCPrepareObjectNormals(obj, obj->mesh,  obj->objMatrix.matrix);
					PCRenderObjectPhong(obj);
				}
				else
					if (obj->flags & OBJECT_FLAGS_MODGE)
						PCRenderModgyObject(obj);
					else
						PCRenderObject(obj);

			}
		}
	}

	RestoreFrame;

	// Recurse.
	if(obj->children)
		DrawObject(obj->children, skinned, masterMesh);

	if(obj->next)
		DrawObject(obj->next, skinned, masterMesh);
}

MDX_TEXENTRY *phong;
MDX_TEXENTRY *lightMap;

void PCRenderObject (MDX_OBJECT *obj)
{
	long i,j;
	unsigned short fce[3] = {0,1,2};		
	MDX_QUATERNION *c1,*c2,*c3;
	D3DTLVERTEX v[3],*vTemp;
	MDX_SHORTVECTOR *facesIdx;
	unsigned long x1on,x2on,x3on,y1on,y2on,y3on;
	unsigned long v0,v1,v2;
	unsigned long v0a,v1a,v2a;
	long alphaVal;
	MDX_TEXENTRY *tex;
	MDX_TEXENTRY **tex2;
	MDX_VECTOR *tV0,*tV1,*tV2, *tN0,*tN1,*tN2;
	MDX_QUATERNION *cols;
	
	facesIdx = obj->mesh->faceIndex;
	tex2 = obj->mesh->textureIDs;
	cols = obj->mesh->gouraudColors;
	alphaVal = (long)(globalXLU2*255.0);

	for (j=0, i=0; i<obj->mesh->numFaces; i++, j+=3)
	{
		// Get information from the mesh!
		v0 = facesIdx->v[0];
		v1 = facesIdx->v[1];
		v2 = facesIdx->v[2];
		
		
		tV0 = &tV[v0];
		tV1 = &tV[v1];
		tV2 = &tV[v2];

		tN0 = &tN[v0];
		tN1 = &tN[v1];
		tN2 = &tN[v2];
		
		tex = (MDX_TEXENTRY *)(*tex2);
		
		// If we are to be drawn.
		if (((tV0->vz) && (tV1->vz) && (tV2->vz)) && (tex))
		{
			// Get rest of info from mesh
			v0a = j;
			v1a = j+1;
			v2a = j+2;

			c1 = &(cols[v0a]);
			c2 = &(cols[v1a]);
			c3 = &(cols[v2a]);
			
			// Fill out D3DVertices...
			vTemp = v;
				
			vTemp->sx = tV0->vx;
			vTemp->sy = tV0->vy;
			vTemp->sz = (tV0->vz) * 0.00025F;
			vTemp->rhw = 1/vTemp->sz;
			
			vTemp->color = MODALPHA(*((unsigned long *)(&(c1->x))),alphaVal);
			vTemp->specular = D3DRGBA(0,0,0,0);
			vTemp->tu = (obj->mesh->faceTC[v0a].v[0]*0.000975F);
			vTemp->tv = (obj->mesh->faceTC[v0a].v[1]*0.000975F);
			
			//vTemp->tv = (tN0->vy+0.5);
			//vTemp->tu = (tN0->vx+0.5);
			vTemp++;

			vTemp->sx = tV1->vx;
			vTemp->sy = tV1->vy;
			vTemp->sz = (tV1->vz) * 0.00025F;
			vTemp->rhw = 1/vTemp->sz;
			vTemp->color = MODALPHA(*((unsigned long *)(&(c2->x))),alphaVal);
			vTemp->specular = D3DRGBA(0,0,0,0);
			vTemp->tu = (obj->mesh->faceTC[v1a].v[0]*0.000975F);
			vTemp->tv = (obj->mesh->faceTC[v1a].v[1]*0.000975F);
			//vTemp->tv = (tN1->vy+0.5);
			//vTemp->tu = (tN1->vx+0.5);
			vTemp++;
			
			vTemp->sx = tV2->vx;
			vTemp->sy = tV2->vy;
			vTemp->sz = (tV2->vz) * 0.00025F;
			vTemp->rhw = 1/vTemp->sz;
			
			vTemp->color = MODALPHA(*((unsigned long *)(&(c3->x))),alphaVal);
			vTemp->specular = D3DRGBA(0,0,0,0);
			vTemp->tu = (obj->mesh->faceTC[v2a].v[0]*0.000975F);
			vTemp->tv = (obj->mesh->faceTC[v2a].v[1]*0.000975F);
			//vTemp->tv = (tN2->vy+0.5);
			//vTemp->tu = (tN2->vx+0.5);
			
			x1on = BETWEEN(v[0].sx,clx0,clx1);
			x2on = BETWEEN(v[1].sx,clx0,clx1);
			x3on = BETWEEN(v[2].sx,clx0,clx1);
			y1on = BETWEEN(v[0].sy,cly0,cly1);
			y2on = BETWEEN(v[1].sy,cly0,cly1);
			y3on = BETWEEN(v[2].sy,cly0,cly1);

			if ((x1on || x2on || x3on) && (y1on || y2on || y3on))
			{
				if ((x1on && x2on && x3on) && (y1on && y2on && y3on))
				{
					PushPolys(v,3,facesON,3,tex->surf,tex);
				}
				else
				{
					Clip3DPolygon(v,tex->surf,tex);
				}
			}
		}
		
		// Update our pointers
		facesIdx++;
		tex2++;
	}
}

void PCRenderObjectPhong (MDX_OBJECT *obj)
{
	long i,j;
	unsigned short fce[3] = {0,1,2};		
	MDX_QUATERNION *c1,*c2,*c3;
	D3DTLVERTEX v[3],*vTemp;
	MDX_SHORTVECTOR *facesIdx;
	unsigned long x1on,x2on,x3on,y1on,y2on,y3on;
	unsigned long v0,v1,v2;
	unsigned long v0a,v1a,v2a;
	long alphaVal;
	MDX_TEXENTRY *tex;
	MDX_TEXENTRY **tex2;
	MDX_VECTOR *tV0,*tV1,*tV2, *tN0,*tN1,*tN2;
	MDX_QUATERNION *cols;
	
	facesIdx = obj->mesh->faceIndex;
	tex2 = obj->mesh->textureIDs;
	cols = obj->mesh->gouraudColors;
	alphaVal = (long)(globalXLU2*255.0);

	for (j=0, i=0; i<obj->mesh->numFaces; i++, j+=3)
	{
		// Get information from the mesh!
		v0 = facesIdx->v[0];
		v1 = facesIdx->v[1];
		v2 = facesIdx->v[2];
		
		
		tV0 = &tV[v0];
		tV1 = &tV[v1];
		tV2 = &tV[v2];

		tN0 = &tN[v0];
		tN1 = &tN[v1];
		tN2 = &tN[v2];

		
		tex = (MDX_TEXENTRY *)(*tex2);
		
		// If we are to be drawn.
		if (((tV0->vz) && (tV1->vz) && (tV2->vz)) && (tex))
		{
			// Get rest of info from mesh
			v0a = j;
			v1a = j+1;
			v2a = j+2;

			c1 = &(cols[v0a]);
			c2 = &(cols[v1a]);
			c3 = &(cols[v2a]);
			
			// Fill out D3DVertices...
			vTemp = v;
				
			vTemp->sx = tV0->vx;
			vTemp->sy = tV0->vy;
			vTemp->sz = (tV0->vz) * 0.00025F;
			vTemp->rhw = 1/vTemp->sz;
			
			vTemp->color = MODALPHA(*((unsigned long *)(&(c1->x))),alphaVal);
			vTemp->specular = D3DRGBA(0,0,0,0);
			vTemp->tu = (obj->mesh->faceTC[v0a].v[0]*0.000975F);
			vTemp->tv = (obj->mesh->faceTC[v0a].v[1]*0.000975F);
			
			//vTemp->tv = (tN0->vy+0.5);
			//vTemp->tu = (tN0->vx+0.5);
			vTemp++;

			vTemp->sx = tV1->vx;
			vTemp->sy = tV1->vy;
			vTemp->sz = (tV1->vz) * 0.00025F;
			vTemp->rhw = 1/vTemp->sz;
			vTemp->color = MODALPHA(*((unsigned long *)(&(c2->x))),alphaVal);
			vTemp->specular = D3DRGBA(0,0,0,0);
			vTemp->tu = (obj->mesh->faceTC[v1a].v[0]*0.000975F);
			vTemp->tv = (obj->mesh->faceTC[v1a].v[1]*0.000975F);
			//vTemp->tv = (tN1->vy+0.5);
			//vTemp->tu = (tN1->vx+0.5);
			vTemp++;
			
			vTemp->sx = tV2->vx;
			vTemp->sy = tV2->vy;
			vTemp->sz = (tV2->vz) * 0.00025F;
			vTemp->rhw = 1/vTemp->sz;
			
			vTemp->color = MODALPHA(*((unsigned long *)(&(c3->x))),alphaVal);
			vTemp->specular = D3DRGBA(0,0,0,0);
			vTemp->tu = (obj->mesh->faceTC[v2a].v[0]*0.000975F);
			vTemp->tv = (obj->mesh->faceTC[v2a].v[1]*0.000975F);
			//vTemp->tv = (tN2->vy+0.5);
			//vTemp->tu = (tN2->vx+0.5);
			
				x1on = BETWEEN(v[0].sx,clx0,clx1);
			x2on = BETWEEN(v[1].sx,clx0,clx1);
			x3on = BETWEEN(v[2].sx,clx0,clx1);
			y1on = BETWEEN(v[0].sy,cly0,cly1);
			y2on = BETWEEN(v[1].sy,cly0,cly1);
			y3on = BETWEEN(v[2].sy,cly0,cly1);

			if ((x1on || x2on || x3on) && (y1on || y2on || y3on))
			{
				if ((x1on && x2on && x3on) && (y1on && y2on && y3on))
				{
					PushPolys(v,3,facesON,3,tex->surf,tex);

					// Push Lightmap polys - only for single pass cards.
					vTemp = v;

					vTemp->tv = (tN0->vy+0.5);
					vTemp->tu = (tN0->vx+0.5);
					vTemp->color = 0xffffffff;
					vTemp++;
					vTemp->tv = (tN1->vy+0.5);
					vTemp->tu = (tN1->vx+0.5);
					vTemp->color = 0xffffffff;
					vTemp++;
					vTemp->tv = (tN2->vy+0.5);
					vTemp->tu = (tN2->vx+0.5);
					vTemp->color = 0xffffffff;
					
					SaveFrame;
					SwapFrame(MA_FRAME_PHONG);
					PushPolys(v,3,facesON,3,phong->surf,phong);					
					SwapFrame(MA_FRAME_LIGHTMAP);
					PushPolys(v,3,facesON,3,lightMap->surf,phong);					
					RestoreFrame;
				}
				else
				{
					Clip3DPolygon(v,tex->surf,tex);
				}
			}
		}
		
		// Update our pointers
		facesIdx++;
		tex2++;
	}
}


void PCRenderModgyWaterObject (MDX_OBJECT *obj)
{
	long i,j;
	unsigned short fce[3] = {0,1,2};		
	MDX_QUATERNION *c1,*c2,*c3;
	D3DTLVERTEX v[3],*vTemp;
	MDX_SHORTVECTOR *facesIdx;
	unsigned long x1on,x2on,x3on,y1on,y2on,y3on;
	unsigned long v0,v1,v2;
	unsigned long v0a,v1a,v2a;
	float m1x,m2x,m3x;
	float m1z,m2z,m3z;
	long alphaVal;
	MDX_TEXENTRY *tex;
	MDX_TEXENTRY **tex2;
	MDX_VECTOR *tV0,*tV1,*tV2, *tN0,*tN1,*tN2;
	MDX_QUATERNION *cols;
	
	facesIdx = obj->mesh->faceIndex;
	tex2 = obj->mesh->textureIDs;
	cols = obj->mesh->gouraudColors;
	alphaVal = (long)(globalXLU2*255.0);

	for (j=0, i=0; i<obj->mesh->numFaces; i++, j+=3)
	{
		// Get information from the mesh!
		v0 = facesIdx->v[0];
		v1 = facesIdx->v[1];
		v2 = facesIdx->v[2];
		
		
		tV0 = &tV[v0];
		tV1 = &tV[v1];
		tV2 = &tV[v2];

		tN0 = &tN[v0];
		tN1 = &tN[v1];
		tN2 = &tN[v2];
		
		m1x = tMx[v0];
		m2x = tMx[v1];
		m3x = tMx[v2];
		m1z = tMz[v0];
		m2z = tMz[v1];
		m3z = tMz[v2];
		
		tex = (MDX_TEXENTRY *)(*tex2);
		
		// If we are to be drawn.
		if (((tV0->vz) && (tV1->vz) && (tV2->vz)) && (tex))
		{
			// Get rest of info from mesh
			v0a = j;
			v1a = j+1;
			v2a = j+2;

			c1 = &(cols[v0a]);
			c2 = &(cols[v1a]);
			c3 = &(cols[v2a]);
			
			// Fill out D3DVertices...
			vTemp = v;
				
			vTemp->sx = tV0->vx;
			vTemp->sy = tV0->vy;
			vTemp->sz = (tV0->vz) * 0.00025F;
			vTemp->rhw = 1/vTemp->sz;
			
			vTemp->color = MODALPHA(*((unsigned long *)(&(c1->x))),alphaVal);
			vTemp->specular = D3DRGBA(0,0,0,0);
			vTemp->tu = (obj->mesh->faceTC[v0a].v[0]*0.000975F) + m1x;
			vTemp->tv = (obj->mesh->faceTC[v0a].v[1]*0.000975F) + m1z;
			
			//vTemp->tv = (tN0->vy+0.5);
			//vTemp->tu = (tN0->vx+0.5);
			vTemp++;

			vTemp->sx = tV1->vx;
			vTemp->sy = tV1->vy;
			vTemp->sz = (tV1->vz) * 0.00025F;
			vTemp->rhw = 1/vTemp->sz;
			vTemp->color = MODALPHA(*((unsigned long *)(&(c2->x))),alphaVal);
			vTemp->specular = D3DRGBA(0,0,0,0);
			vTemp->tu = (obj->mesh->faceTC[v1a].v[0]*0.000975F) + m2x;
			vTemp->tv = (obj->mesh->faceTC[v1a].v[1]*0.000975F) + m2z;
			//vTemp->tv = (tN1->vy+0.5);
			//vTemp->tu = (tN1->vx+0.5);
			vTemp++;
			
			vTemp->sx = tV2->vx;
			vTemp->sy = tV2->vy;
			vTemp->sz = (tV2->vz) * 0.00025F;
			vTemp->rhw = 1/vTemp->sz;
			
			vTemp->color = MODALPHA(*((unsigned long *)(&(c3->x))),alphaVal);
			vTemp->specular = D3DRGBA(0,0,0,0);
			vTemp->tu = (obj->mesh->faceTC[v2a].v[0]*0.000975F) + m3x;
			vTemp->tv = (obj->mesh->faceTC[v2a].v[1]*0.000975F) + m3z;
			//vTemp->tv = (tN2->vy+0.5);
			//vTemp->tu = (tN2->vx+0.5);
			
			x1on = BETWEEN(v[0].sx,clx0,clx1);
			x2on = BETWEEN(v[1].sx,clx0,clx1);
			x3on = BETWEEN(v[2].sx,clx0,clx1);
			y1on = BETWEEN(v[0].sy,cly0,cly1);
			y2on = BETWEEN(v[1].sy,cly0,cly1);
			y3on = BETWEEN(v[2].sy,cly0,cly1);

			if ((x1on || x2on || x3on) && (y1on || y2on || y3on))
			{
				if ((x1on && x2on && x3on) && (y1on && y2on && y3on))
				{
					PushPolys(v,3,facesON,3,tex->surf,tex);
				}
				else
				{
					Clip3DPolygon(v,tex->surf,tex);
				}
			}
		}
		
		// Update our pointers
		facesIdx++;
		tex2++;
	}
}

void PCRenderModgyObject (MDX_OBJECT *obj)
{
	long i,j;
	unsigned short fce[3] = {0,1,2};		
	MDX_QUATERNION *c1,*c2,*c3;
	D3DTLVERTEX v[3],*vTemp;
	MDX_SHORTVECTOR *facesIdx;
	unsigned long x1on,x2on,x3on,y1on,y2on,y3on;
	unsigned long v0,v1,v2;
	unsigned long v0a,v1a,v2a;
	float m1x,m2x,m3x;
	float m1z,m2z,m3z,cVal;
	long alphaVal;
	MDX_TEXENTRY *tex;
	MDX_TEXENTRY **tex2;
	MDX_VECTOR *tV0,*tV1,*tV2, *tN0,*tN1,*tN2;
	MDX_QUATERNION *cols;
	
	facesIdx = obj->mesh->faceIndex;
	tex2 = obj->mesh->textureIDs;
	cols = obj->mesh->gouraudColors;
	alphaVal = (long)(globalXLU2*255.0);

	for (j=0, i=0; i<obj->mesh->numFaces; i++, j+=3)
	{
		// Get information from the mesh!
		v0 = facesIdx->v[0];
		v1 = facesIdx->v[1];
		v2 = facesIdx->v[2];
		
		
		tV0 = &tV[v0];
		tV1 = &tV[v1];
		tV2 = &tV[v2];

		tN0 = &tN[v0];
		tN1 = &tN[v1];
		tN2 = &tN[v2];
		
		m1x = tMx[v0];
		m2x = tMx[v1];
		m3x = tMx[v2];
		m1z = tMz[v0];
		m2z = tMz[v1];
		m3z = tMz[v2];
		
		tex = (MDX_TEXENTRY *)(*tex2);
		
		// If we are to be drawn.
		if (((tV0->vz) && (tV1->vz) && (tV2->vz)) && (tex))
		{
			// Get rest of info from mesh
			v0a = j;
			v1a = j+1;
			v2a = j+2;

			c1 = &(cols[v0a]);
			c2 = &(cols[v1a]);
			c3 = &(cols[v2a]);
			
			// Fill out D3DVertices...
			vTemp = v;
				
			vTemp->sx = tV0->vx;
			vTemp->sy = tV0->vy;
			vTemp->sz = (tV0->vz) * 0.00025F;
			vTemp->rhw = 1/vTemp->sz;

			cVal = 1;//cVal = fabs((m1x+m1z)*2.5);		
			vTemp->color = D3DRGBA(cVal,cVal,1,0.2+(m1x+m1z)*2);
			vTemp->specular = D3DRGBA(0,0,0,0);
			vTemp->tu = (obj->mesh->faceTC[v0a].v[0]*0.000975F) + m1x*0.4;
			vTemp->tv = (obj->mesh->faceTC[v0a].v[1]*0.000975F) + m1z*0.4;
			
			//vTemp->tv = (tN0->vy+0.5);
			//vTemp->tu = (tN0->vx+0.5);
			vTemp++;

			vTemp->sx = tV1->vx;
			vTemp->sy = tV1->vy;
			vTemp->sz = (tV1->vz) * 0.00025F;
			vTemp->rhw = 1/vTemp->sz;
			cVal = 1;//fabs((m2x+m2z)*2.5);		
			vTemp->color = D3DRGBA(cVal,cVal,1,0.2+(m2x+m2z)*2);
			vTemp->specular = D3DRGBA(0,0,0,0);
			vTemp->tu = (obj->mesh->faceTC[v1a].v[0]*0.000975F) + m2x*0.4;
			vTemp->tv = (obj->mesh->faceTC[v1a].v[1]*0.000975F) + m2z*0.4;
			//vTemp->tv = (tN1->vy+0.5);
			//vTemp->tu = (tN1->vx+0.5);
			vTemp++;
			
			vTemp->sx = tV2->vx;
			vTemp->sy = tV2->vy;
			vTemp->sz = (tV2->vz) * 0.00025F;
			vTemp->rhw = 1/vTemp->sz;
			cVal = 1;//cVal = fabs((m3x+m3z)*2.5);		
			vTemp->color = D3DRGBA(cVal,cVal,1,0.2+(m3x+m3z)*2);
			vTemp->specular = D3DRGBA(0,0,0,0);
			vTemp->tu = (obj->mesh->faceTC[v2a].v[0]*0.000975F) + m3x *0.4;
			vTemp->tv = (obj->mesh->faceTC[v2a].v[1]*0.000975F) + m3z *0.4;
			//vTemp->tv = (tN2->vy+0.5);
			//vTemp->tu = (tN2->vx+0.5);
			
			x1on = BETWEEN(v[0].sx,clx0,clx1);
			x2on = BETWEEN(v[1].sx,clx0,clx1);
			x3on = BETWEEN(v[2].sx,clx0,clx1);
			y1on = BETWEEN(v[0].sy,cly0,cly1);
			y2on = BETWEEN(v[1].sy,cly0,cly1);
			y3on = BETWEEN(v[2].sy,cly0,cly1);

			if ((x1on || x2on || x3on) && (y1on || y2on || y3on))
			{
				if ((x1on && x2on && x3on) && (y1on && y2on && y3on))
					PushPolys(v,3,facesON,3,tex->surf,tex);
				else
					Clip3DPolygon(v,tex->surf,tex);
			}
		}
		
		// Update our pointers
		facesIdx++;
		tex2++;
	}
}

#ifdef __cplusplus
}
#endif
