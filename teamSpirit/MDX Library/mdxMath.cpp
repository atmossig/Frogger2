/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: mdxReport.cpp
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
#include "mgeReport.h"
#include "gelf.h"
#include "math.h"

#ifdef __cplusplus
extern "C"
{
#endif

MDX_VECTOR	upV = {0,1,0},
			rightV = {1,0,0},
			inV = {0,0,1};

MDX_QUATERNION zeroQuat = {0,0,0,1};
MDX_MATRIXSTACK	matrixStack;
MDX_VECTOR curAt;
MDX_VECTOR curEye;

void Normalise(MDX_VECTOR *vect)
{
	float m = mdxMagnitude(vect);
	

	if(m != 0)
	{
		vect->vx /= m;
		vect->vy /= m;
		vect->vz /= m;
	}
}

void guMtxIdent (float a[4][4])
{
	int i,j;
	for (i=0; i<4; i++)
		for (j=0; j<4; j++)
			a[i][j] = (i==j) ? 1.0f : 0.0f;
}

void guScaleF(float a[4][4], float dx, float dy, float dz)
{
	guMtxIdent (a);
	
	a[0][0] = dx;
	a[1][1] = dy;
	a[2][2] = dz;
}

void guTranslateF(float a[4][4], float dx, float dy, float dz)
{
	guMtxIdent (a);
	
	a[3][0] = dx;
	a[3][1] = dy;
	a[3][2] = dz;		
}


void guMtxCatF(float *b, float *a, float *ret)
{
	float adj[4][4]={0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
	float *p,*o,*n,*m = (float *)adj;
	long i,j,k,l;
	
	i=4;

 	while(i--)
	{		
		for (j=0; j<4; j++) 
		{
			n = a+j;
			o = m+j;
			p = b;
			k=4;
			while(k--) *o += *n * *p++, n+=4;
			
		}
		m+=4;
		b+=4;		
	}

	memcpy (ret,adj,sizeof(float)*16);
}

void guMtxXFMF(float m[4][4],float srcX,float srcY,float srcZ,float *destX,float *destY,float *destZ)
{
	*destX = (m[0][0]*srcX)+(m[1][0]*srcY)+(m[2][0]*srcZ)+(m[3][0]);
	*destY = (m[0][1]*srcX)+(m[1][1]*srcY)+(m[2][1]*srcZ)+(m[3][1]);
	*destZ = (m[0][2]*srcX)+(m[1][2]*srcY)+(m[2][2]*srcZ)+(m[3][2]);
}


void guLookAtF (float m[4][4],float xEye, float yEye, float zEye,float xAt, float yAt, float zAt,float xUp, float yUp, float zUp)
{
    MDX_VECTOR  up, right, view_dir,world_up,from;

	guMtxIdent (m);
	
	curAt.vx = xAt;
	curAt.vy = yAt;
	curAt.vz = zAt;

	curEye.vx = xEye;
	curEye.vy = yEye;
	curEye.vz = zEye;

	view_dir.vx = xEye - xAt;
	view_dir.vy = yEye - yAt;
	view_dir.vz = zEye - zAt;
	
	world_up.vx = xUp;
	world_up.vy = yUp;
	world_up.vz = zUp;
	
	from.vx = xAt;
	from.vy = yAt;
	from.vz = zAt;
	
	Normalise(&view_dir);
    
	CrossProduct(&right, &world_up, &view_dir);
    CrossProduct(&up, &view_dir, &right);
    
    Normalise(&right);
    Normalise(&up);
    
    m[0][0] = right.vx;
    m[1][0] = right.vy;
    m[2][0] = right.vz;
    m[0][1] = up.vx;
    m[1][1] = up.vy;
    m[2][1] = up.vz;
    m[0][2] = view_dir.vx;
    m[1][2] = view_dir.vy;
    m[2][2] = view_dir.vz;
    m[3][0] = -DotProduct(&right, &from);
    m[3][1] = -DotProduct(&up, &from);
    m[3][2] = -DotProduct(&view_dir, &from);
    
}

void MatrixStackInitialise()
{
	int	x, y, i;
	
	matrixStack.stackPosition = 0;
	for(i = 0; i < MAX_STACK_SIZE; i++)
	{
		for(x = 0; x < 4; x++)
		{
			for(y = 0; y < 4; y++)
			{
				matrixStack.stack[i][x][y] = 0;
			}
		}
	}
	
	//set first matrix in stack to be a unit matrix
	matrixStack.stack[0][0][0] = 1.0;
	matrixStack.stack[0][1][1] = 1.0;
	matrixStack.stack[0][2][2] = 1.0;
	matrixStack.stack[0][3][3] = 1.0;
}

void GetQuaternionFromRotation(MDX_QUATERNION *destQ,MDX_QUATERNION *srcQ)
{
	float thetaOver2;
	float sinThetaOver2;

	thetaOver2 = srcQ->w/2;
	sinThetaOver2 = sinf(thetaOver2);

	destQ->w = cosf(thetaOver2);
	destQ->x = sinThetaOver2 * srcQ->x;
	destQ->y = sinThetaOver2 * srcQ->y;
	destQ->z = sinThetaOver2 * srcQ->z;
}

void QuaternionToMatrix(MDX_QUATERNION *squat, MDX_MATRIX *dmatrix)
{
	float	s, xs,ys,zs, wx,wy,wz, xx,xy,xz, yy,yz,zz;
	float	*m = (float *)dmatrix->matrix;

	s = ((float)2.0)/(squat->x*squat->x + squat->y*squat->y + squat->z*squat->z + squat->w*squat->w);

	xs = squat->x*s;
	ys = squat->y*s;
	zs = squat->z*s;

	wx = squat->w*xs;
	wy = squat->w*ys;
	wz = squat->w*zs;

	xx = squat->x*xs;
	xy = squat->x*ys;
	xz = squat->x*zs;

	yy = squat->y*ys;
	yz = squat->y*zs;
	zz = squat->z*zs;

	*m++ = ((float)1.0)-(yy+zz);
	*m++ = xy+wz;
	*m++ = xz-wy;
	*m++ = (float)0.0;
	
	*m++ = xy-wz;
	*m++ = ((float)1.0)-(xx+zz);
	*m++ = yz+wx;
	*m++ = (float)0.0;
	
	*m++ = xz+wy;
	*m++ = yz-wx;
	*m++ = ((float)1.0)-(xx+yy);
	*m++ = (float)0.0;
	
	*m++ = (float)0.0;
	*m++ = (float)0.0;
	*m++ = (float)0.0;
	*m++ = (float)1.0;
	
	/*
	dmatrix->matrix[0][0] = ((float)1.0)-(yy+zz);
	dmatrix->matrix[0][1] = xy+wz;
	dmatrix->matrix[0][2] = xz-wy;
	dmatrix->matrix[0][3] = (float)0.0;
	
	dmatrix->matrix[1][0] = xy-wz;
	dmatrix->matrix[1][1] = ((float)1.0)-(xx+zz);
	dmatrix->matrix[1][2] = yz+wx;
	dmatrix->matrix[1][3] = (float)0.0;
	
	dmatrix->matrix[2][0] = xz+wy;
	dmatrix->matrix[2][1] = yz-wx;
	dmatrix->matrix[2][2] = ((float)1.0)-(xx+yy);
	dmatrix->matrix[2][3] = (float)0.0;
	
	dmatrix->matrix[3][0] = (float)0.0;
	dmatrix->matrix[3][1] = (float)0.0;
	dmatrix->matrix[3][2] = (float)0.0;
	dmatrix->matrix[3][3] = (float)1.0;
	*/
}

void QuatSlerp(MDX_QUATERNION *src1, MDX_QUATERNION *sp2, float t, MDX_QUATERNION *dquat)
{
	float			omega, cosom, sinom, sclp, sclq, pdist,ndist;
	float			x,y,z,w;
	MDX_QUATERNION	negs2, *src2;

	x = src1->x - sp2->x;
	y = src1->y - sp2->y;
	z = src1->z - sp2->z;
	w = src1->w - sp2->w;
	pdist = x*x + y*y + z*z + w*w;

	x = src1->x + sp2->x;
	y = src1->y + sp2->y;
	z = src1->z + sp2->z;
	w = src1->w + sp2->w;
	ndist = x*x + y*y + z*z + w*w;

	if (pdist < ndist)
	{
		// +ve arc is shorter, so interpolate along that
		src2 = sp2;
	}
	else
	{
		// -ve arc is shorter, so interpolate along that
		negs2.x = -sp2->x;
		negs2.y = -sp2->y;
		negs2.z = -sp2->z;
		negs2.w = -sp2->w;
		src2 = &negs2;
	}

	cosom = src1->x*src2->x + src1->y*src2->y + src1->z*src2->z + src1->w*src2->w;

	if ((((float)1.0)+cosom) > EPSILON)
	{
		if ((((float)1.0)-cosom) > EPSILON)
		{
			omega = (float)acos(cosom);
			sinom = (float)1.0/sinf(omega);
			sclp = (float)sinf(((float)1.0-t)*omega)*sinom;
			sclq = (float)sinf(t*omega)*sinom;
		}
		else
		{
			sclp = (float)1.0-t;
			sclq = t;
		}
		dquat->x = sclp*src1->x + sclq*src2->x;
		dquat->y = sclp*src1->y + sclq*src2->y;
		dquat->z = sclp*src1->z + sclq*src2->z;
		dquat->w = sclp*src1->w + sclq*src2->w;
	}
	else
	{
		dquat->x = -src1->y;
		dquat->y = src1->x;
		dquat->z = -src1->w;
		dquat->w = src1->z;
		sclp = (float)sinf(((float)1.0-t)*HALFPI);
		sclq = (float)sinf(t*HALFPI);
		dquat->x = sclp*src1->x + sclq*dquat->x;
		dquat->y = sclp*src1->y + sclq*dquat->y;
		dquat->z = sclp*src1->z + sclq*dquat->z;
	}
}

void GetRotationFromQuaternion(MDX_QUATERNION *destQ,MDX_QUATERNION *srcQ)
{
	if (srcQ->w)
	{
        destQ->w = 2.0*acos(srcQ->w);
		SetVector((MDX_VECTOR *)&destQ,(MDX_VECTOR *)&srcQ);
		Normalise((MDX_VECTOR *)&destQ);
	}
	else
	{
        destQ->w = 0;
        destQ->x = 0.0;
        destQ->y = 1.0;
        destQ->z = 0.0;		
	}
}

void QuaternionMultiply(MDX_QUATERNION *dest,MDX_QUATERNION *src1,MDX_QUATERNION *src2)
{
	float dp;
	MDX_QUATERNION temp;

	dp = src1->x*src2->x + src1->y*src2->y + src1->z*src2->z;

	temp.w = src1->w*src2->w - dp;
	temp.x = src1->w*src2->x + src2->w*src1->x + src1->y*src2->z - src1->z*src2->y;
	temp.y = src1->w*src2->y + src2->w*src1->y + src1->z*src2->x - src1->x*src2->z;
	temp.z = src1->w*src2->z + src2->w*src1->z + src1->x*src2->y - src1->y*src2->x;

	memcpy(dest,&temp,sizeof(MDX_QUATERNION));
}

/*	--------------------------------------------------------------------------------
	Function 	: RotateVectorByQuaternion
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: destination CAN be same as source
*/
void RotateVectorByRotation(MDX_VECTOR *result,MDX_VECTOR *vect,MDX_QUATERNION *rot)
{
	float m,n,sinTheta,cosTheta;
	MDX_VECTOR mVec,pVec,vVec;

	m = vect->vx*rot->x;
	m += vect->vy*rot->y;
	m += vect->vz*rot->z;

	mVec.vx = m*rot->x;				
	mVec.vy = m*rot->y;
	mVec.vz = m*rot->z;				

	SubVector(&pVec,vect,&mVec);

	CrossProduct(&vVec,(MDX_VECTOR *)rot,&pVec);

	m = mdxMagnitude(&pVec);

	if(m == 0)
		SetVector(result,vect);
	else
	{
		n = mdxMagnitude(&vVec);
		if(n)
		{
			m /= n;
			ScaleVector(&vVec,m);
		}

		cosTheta = cosf(rot->w);
		sinTheta = sinf(rot->w);

		result->vx = mVec.vx + cosTheta * pVec.vx + sinTheta * vVec.vx;
		result->vy = mVec.vy + cosTheta * pVec.vy + sinTheta * vVec.vy;
		result->vz = mVec.vz + cosTheta * pVec.vz + sinTheta * vVec.vz;
	}
}

void MatrixToQuaternion(MDX_MATRIX *smatrix, MDX_QUATERNION *dquat)
{
	float	tr, s, *fi,*fj,*fk;
	int		i,j,k;

	tr = smatrix->matrix[0][0]+smatrix->matrix[1][1]+smatrix->matrix[2][2];
	if (tr > (float)0.0)
	{
		s = sqrtf(tr+(float)1.0);
		dquat->w = s*(float)0.5;
		s = ((float)0.5)/s;

		dquat->x = (smatrix->matrix[1][2]-smatrix->matrix[2][1])*s;
		dquat->y = (smatrix->matrix[2][0]-smatrix->matrix[0][2])*s;
		dquat->z = (smatrix->matrix[0][1]-smatrix->matrix[1][0])*s;
	}
	else
	{
		if (smatrix->matrix[1][1] > smatrix->matrix[0][0])
		{
			if (smatrix->matrix[2][2] > smatrix->matrix[1][1])
			{
				fi = &(dquat->z); i = 2;
				fj = &(dquat->x); j = 0;
				fk = &(dquat->y); k = 1;
			}
			else
			{
				fi = &(dquat->y); i = 1;
				fj = &(dquat->z); j = 2;
				fk = &(dquat->x); k = 0;
			}
		}
		else
		{
			if (smatrix->matrix[2][2] > smatrix->matrix[0][0])
			{
				fi = &(dquat->z); i = 2;
				fj = &(dquat->x); j = 0;
				fk = &(dquat->y); k = 1;
			}
			else
			{
				fi = &(dquat->x); i = 0;
				fj = &(dquat->y); j = 1;
				fk = &(dquat->z); k = 2;
			}
		}

		s = sqrtf((smatrix->matrix[i][i]-(smatrix->matrix[j][j]+smatrix->matrix[k][k]))+(float)1.0);

		*fi = s*(float)0.5;
		s = ((float)0.5)/s;
		dquat->w = (smatrix->matrix[j][k]-smatrix->matrix[k][j])*s;
		*fj = (smatrix->matrix[i][j]+smatrix->matrix[j][i])*s;
		*fk = (smatrix->matrix[i][k]+smatrix->matrix[k][i])*s;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: FindShortestAngle()
	Purpose 	: returns the smallest angle between 2 directions (rads)
	Parameters 	: angle1, angle2
	Returns 	: angle
	Info 		:
*/
float mdxFindShortestAngle(float val1, float val2)
{
	float temp;

	if(abs(val1-val2) > 0.5)
	{
		if(val1 > val2)
			temp = val2 - val1;
		else
			temp = val1 - val2;

		temp ++;
		return temp;
	}
	else
	{
	   return abs(val1 - val2);
	}
}


#ifdef __cplusplus
}
#endif
