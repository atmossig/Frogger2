#include "tongue.h"
#include "map_draw.h"
#include "psxtongue.h"
#include "cam.h"

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


void DrawTongueSegment(SVECTOR *vt, TextureType *tEntry)
{
	long *tfv;
	long *tfd;
	int i;
	POLY_FT4 *ft4;
	POLY_F4  *f4;

	//transform the 3d coords into screen xyz
//	tfv = (long*)transformedVertices;
//	tfd = (long*)transformedDepths;
//	transformVertexListA(vt, 4, tfv, tfd);
//	for(i=0; i<4; i++)
//		tfd[i] = tfd[i] >> 2;

	//set up a poly
/*	BEGINPRIM(ft4, POLY_FT4);
	setPolyFT4(ft4);
	*(long*)(&ft4->x0) = *(long*)(&vt[0].vx);
	*(long*)(&ft4->x1) = *(long*)(&vt[1].vx);
	*(long*)(&ft4->x2) = *(long*)(&vt[2].vx);
	*(long*)(&ft4->x3) = *(long*)(&vt[3].vx);
	ft4->r0 = 128;
	ft4->g0 = 128;
	ft4->b0 = 128;
	ft4->u0 = tEntry->u0;
	ft4->v0 = tEntry->v0;
	ft4->u1 = tEntry->u1;
	ft4->v1 = tEntry->v1;
	ft4->u2 = tEntry->u2;
	ft4->v2 = tEntry->v2;
	ft4->u3 = tEntry->u3;
	ft4->v3 = tEntry->v3;
	ft4->tpage = tEntry->tpage;
	ft4->clut  = tEntry->clut;
	ENDPRIM(ft4, 1, POLY_FT4);
*/
	BEGINPRIM(f4, POLY_F4);
	setPolyF4(f4);
	f4->x0 = vt[0].vx;
	f4->y0 = vt[0].vy;
	f4->x1 = vt[1].vx;
	f4->y1 = vt[1].vy;
	f4->x2 = vt[2].vx;
	f4->y2 = vt[2].vy;
	f4->x3 = vt[3].vx;
	f4->y3 = vt[3].vy;
	f4->r0 = 200;
	f4->g0 = 48;
	f4->b0 = 48;
//	ENDPRIM(f4, 1, POLY_F4);
	ENDPRIM(f4, (vt[0].vz+vt[1].vz+vt[2].vz+vt[3].vz)>>4, POLY_F4);
}





void CalcTongueNodes(SVECTOR *vT, int pl, int i)
{
	TONGUE *t = &tongue[pl];
	FVECTOR p1, p2;
	IQUATERNION q, cross;
	fixed p;
	FVECTOR pos, m, normal, cam;
	VECTOR tempVect;
	SVECTOR tempSvect;
	long sxy, sz;

	MATRIX tempMtx;

	SetVectorFF(&pos, &t->segment[i]);
//	ScaleVectorFF(&pos, 410);


	SetVectorFF(&cam, &currCamSource);
	SubVectorFFF(&normal, &cam, &pos);
	MakeUnit(&normal);
	CrossProductFFF((FVECTOR*)&cross, &normal, &upVec);
	MakeUnit((FVECTOR*)&cross);
	p = DotProductFF(&normal, &upVec);
	cross.w = -arccos(p);
	fixedGetQuaternionFromRotation(&q, &cross);
	QuatToPSXMatrix(&q, &tempMtx);

	tempMtx.t[0] = -pos.vx>>12;
	tempMtx.t[1] = -pos.vy>>12;
	tempMtx.t[2] = pos.vz>>12;



//	p1.vx = 20480-(i*1638);
	p1.vx = -12000+(i*1024);
	p1.vx *= SCALE;
	p1.vy = 0;
	p1.vz = 0;
	RotateVectorByQuaternionFF( &p2, &p1, &frog[pl]->actor->qRot );
	vT[0].vx = p2.vx>>12;
	vT[0].vy = p2.vy>>12;
	vT[0].vz = p2.vz>>12;

//	p1.vx = 20480-(i*1638);
	p1.vx = 12000+(i*1024);
	p1.vx *= SCALE;
	p1.vy = 0;
	p1.vz = 0;
	RotateVectorByQuaternionFF( &p2, &p1, &frog[pl]->actor->qRot );
	vT[1].vx = p2.vx>>12;
	vT[1].vy = p2.vy>>12;
	vT[1].vz = p2.vz>>12;




	ApplyMatrix(&tempMtx, &vT[0], &tempVect);
	SetVectorSV(&tempSvect, &tempVect);
	//bbdebug
	tempSvect.vx += tempMtx.t[0]; tempSvect.vy += tempMtx.t[1]; tempSvect.vz += tempMtx.t[2];
	gte_SetTransMatrix(&GsWSMATRIX);
	gte_SetRotMatrix(&GsWSMATRIX);
	gte_ldv0(&tempSvect);
	gte_rtps();
	gte_stsxy(&sxy);
//	gte_stsz(&sz);	//screen z/4 as otz
	gte_stszotz(&sz);	//screen z/4 as otz
	vT[0].vx = (short)(sxy&0xffff);
	vT[0].vy = (short)(sxy>>16);
	vT[0].vz = sz;



	ApplyMatrix(&tempMtx, &vT[1], &tempVect);
	SetVectorSV(&tempSvect, &tempVect);
	//bbdebug
	tempSvect.vx += tempMtx.t[0]; tempSvect.vy += tempMtx.t[1]; tempSvect.vz += tempMtx.t[2];
	gte_SetTransMatrix(&GsWSMATRIX);
	gte_SetRotMatrix(&GsWSMATRIX);
	gte_ldv0(&tempSvect);
	gte_rtps();
	gte_stsxy(&sxy);
//	gte_stsz(&sz);	//screen z/4 as otz
	gte_stszotz(&sz);	//screen z/4 as otz
	vT[1].vx = (short)(sxy&0xffff);
	vT[1].vy = (short)(sxy>>16);
	vT[1].vz = sz;
}


void DrawTongue( int pl )
{
	unsigned long i=0, index = (tongue[pl].progress*(MAX_TONGUENODES-1)>>12);
	SVECTOR vT[4], vTPrev[2];
	TextureType *tEntry;

	tEntry = tongue[pl].tex;

	if(/*!tEntry ||*/ index<2)
		return;

	while(i < index)
	{
		//********-[ First 2 points ]-*******
		if(i && vTPrev[0].vz && vTPrev[1].vz)
			memcpy( vT, vTPrev, sizeof(SVECTOR)*2 );			// Previously transformed vertices
		else
			CalcTongueNodes( vT, pl, i );

		//********-[ Next 2 points ]-********
		CalcTongueNodes( &vT[2], pl, i+1 );
		memcpy(vTPrev, &vT[2], sizeof(SVECTOR)*2); 			// Store first 2 vertices of the next segment

		//********-[ Draw the polys ]-********
		if(vT[0].vz && vT[1].vz && vT[2].vz && vT[3].vz)
		{
			DrawTongueSegment(vT, tEntry);
		}

		i++;
	}

	//need to draw some sort of end 'blob' on the tongue?
}

