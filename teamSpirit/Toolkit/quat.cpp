#include <math.h>

#include "vertex.h"
#include "matrix.h"
#include "quat.h"


/*************************************************
	Name:		Quaternion::Muliply
	Purpose:	Create a sum of both quaternions
	Parameters:	
	Info:		
**************************************************/
Quaternion Quaternion::operator * ( quat &q )
{
	float dp;
	Quaternion dest;

	dp = x*q.x + y*q.y + z*q.z;

	dest.w = w*q.w - dp;
	dest.x = w*q.x + q.w*x + y*q.z - z*q.y;
	dest.y = w*q.y + q.w*y + z*q.x - x*q.z;
	dest.z = w*q.z + q.w*z + x*q.y - y*q.x;

	return dest;
}


/*************************************************
	Name:		Quaternion::Slerp
	Purpose:	Slerp (interpolate) between this and another quat
	Parameters:	
	Info:		
**************************************************/
Quaternion Quaternion::Slerp( quat &q, float t )
{
	float omega, cosom, sinom, sclp, sclq, pdist,ndist;
	float dx,dy,dz,dw;
	quat negs, *src;
	Quaternion dest;

	dx = x - q.x;
	dy = y - q.y;
	dz = z - q.z;
	dw = w - q.w;
	pdist = dx*dx + dy*dy + dz*dz + dw*dw;

	dx = x + q.x;
	dy = y + q.y;
	dz = z + q.z;
	dw = w + q.w;
	ndist = dx*dx + dy*dy + dz*dz + dw*dw;

	if( pdist < ndist ) // +ve arc is shorter, so interpolate along that
		src = &q;
	else // -ve arc is shorter, so interpolate along that
	{
		negs.x = -q.x;
		negs.y = -q.y;
		negs.z = -q.z;
		negs.w = -q.w;
		src = &negs;
	}

	cosom = x*src->x + y*src->y + z*src->z + w*src->w;

	if( 1.0+cosom > QEPSILON )
	{
		if( 1.0-cosom > QEPSILON )
		{
			omega = (float)acos(cosom);
			sinom = 1.0f / (float)sinf(omega);
			sclp = (float)sinf((1.0f-t)*omega) * sinom;
			sclq = (float)sinf(t*omega) * sinom;
		}
		else
		{
			sclp = 1.0f-t;
			sclq = t;
		}

		dest.x = sclp*x + sclq*src->x;
		dest.y = sclp*y + sclq*src->y;
		dest.z = sclp*z + sclq*src->z;
		dest.w = sclp*w + sclq*src->w;
	}
	else
	{
		dest.x = -y;
		dest.y = x;
		dest.z = -w;
		dest.w = z;
		sclp = (float)sinf((1.0f-t)*(float)QHALFPI);
		sclq = (float)sinf(t*(float)QHALFPI);
		dest.x = sclp*x + sclq*dest.x;
		dest.y = sclp*y + sclq*dest.y;
		dest.z = sclp*z + sclq*dest.z;
	}

	return dest;
}


/*************************************************
	Name:		Quaternion::SlerpFixedSpeed
	Purpose:	As above, but fixed speed
	Parameters:	
	Info:		Can be more efficient
**************************************************/
Quaternion Quaternion::SlerpFixedSpeed( quat &q, float speed )
{
	Quaternion d;
	float t,t2;

	t = (x-q.x)*(x-q.x) + (y-q.y)*(y-q.y) + (z-q.z)*(z-q.z) + (w-q.w)*(w-q.w);
	t2 = (x+q.x)*(x+q.x) + (y+q.y)*(y+q.y) + (z+q.z)*(z+q.z) + (w+q.w)*(w+q.w);
	
	if( t2 > t )
		t = x*q.x + y*q.y + z*q.z + w*q.w;
	else
		t = -(x*q.x + y*q.y + z*q.z + w*q.w);

	t = (float)fabs(acos(t));

	if( t > speed )
		d = Slerp( q, speed/t );
	else
		d = q;

	return d;
}


/*************************************************
	Name:		Quaternion::ToMatrix
	Purpose:	Create a matrix to emulate this quat
	Parameters:	
	Info:		
**************************************************/
Matrix Quaternion::ToMatrix( )
{
	Matrix mat;
	float s, xs,ys,zs, wx,wy,wz, xx,xy,xz, yy,yz,zz;

	s = 2.0f/(x*x + y*y + z*z + w*w);

	xs = x*s;
	ys = y*s;
	zs = z*s;

	wx = w*xs;
	wy = w*ys;
	wz = w*zs;

	xx = x*xs;
	xy = x*ys;
	xz = x*zs;

	yy = y*ys;
	yz = y*zs;
	zz = z*zs;

	mat.m[0][0] = 1.0f-(yy+zz);
	mat.m[0][1] = xy+wz;
	mat.m[0][2] = xz-wy;

	mat.m[1][0] = xy-wz;
	mat.m[1][1] = 1.0f-(xx+zz);
	mat.m[1][2] = yz+wx;

	mat.m[2][0] = xz+wy;
	mat.m[2][1] = yz-wx;
	mat.m[2][2] = 1.0f-(xx+yy);

	mat.m[0][3] = 0;
	mat.m[1][3] = 0;
	mat.m[2][3] = 0;
	mat.m[3][3] = 1;
	mat.m[3][0] = 0;
	mat.m[3][1] = 0;
	mat.m[3][2] = 0;

	return mat;
}


/*************************************************
	Name:		Quaternion::ToRotation
	Purpose:	Generate a roation (normal+angle) from unit quat
	Parameters:	
	Info:		
**************************************************/
Quaternion Quaternion::ToRotation( )
{
	Quaternion d;
	float theta,sinThetaOver2,m;

	theta = 2 * (float)acos(w);
	sinThetaOver2 = (float)sinf(theta/2);
	
	d.w = theta;
	if( sinThetaOver2 )
	{
		d.x = x/sinThetaOver2;
		d.y = y/sinThetaOver2;
		d.z = z/sinThetaOver2;

		m = sqrtf(d.x*d.x + d.y*d.y + d.z*d.z);
		if(m)
		{
			d.x /= m;
			d.y /= m;
			d.z /= m;
		}
	}
	else
	{
		d.x = 0;
		d.y = 1;
		d.z = 0;
	}

	return d;
}


/*************************************************
	Name:		Quaternion::ToQuaternion
	Purpose:	Get unit quat from rotation
	Parameters:	
	Info:		
**************************************************/
Quaternion Quaternion::ToQuaternion( )
{
	Quaternion d;
	float thetaOver2;
	float sinThetaOver2;

	thetaOver2 = w*0.5f;
	sinThetaOver2 = sinf(thetaOver2);

	d.w = cosf(thetaOver2);
	d.x = sinThetaOver2 * x;
	d.y = sinThetaOver2 * y;
	d.z = sinThetaOver2 * z;

	return d;
}


/*************************************************
	Name:		Quaternion::FromMatrix
	Purpose:	Generate a quaternion from a rotation matrix
	Parameters:	
	Info:		
**************************************************/
void Quaternion::FromMatrix( Matrix &mat )
{
	float tr, s, *fi,*fj,*fk;
	int	i,j,k;

	tr = mat.m[0][0] + mat.m[1][1] + mat.m[2][2];
	if( tr > 0 )
	{
		s = sqrtf( tr+1 );
		w = s*0.5f;
		s = 0.5f/s;

		x = (mat.m[1][2] - mat.m[2][1]) * s;
		y = (mat.m[2][0] - mat.m[0][2]) * s;
		z = (mat.m[0][1] - mat.m[1][0]) * s;
	}
	else
	{
		if( mat.m[1][1] > mat.m[0][0] )
		{
			if( mat.m[2][2] > mat.m[1][1] )
			{
				fi = &z; i = 2;
				fj = &x; j = 0;
				fk = &y; k = 1;
			}
			else
			{
				fi = &y; i = 1;
				fj = &z; j = 2;
				fk = &x; k = 0;
			}
		}
		else
		{
			if( mat.m[2][2] > mat.m[0][0] )
			{
				fi = &z; i = 2;
				fj = &x; j = 0;
				fk = &y; k = 1;
			}
			else
			{
				fi = &x; i = 0;
				fj = &y; j = 1;
				fk = &z; k = 2;
			}
		}

		s = sqrtf((mat.m[i][i]-(mat.m[j][j]+mat.m[k][k]))) + 1;

		*fi = s*0.5f;
		s = 0.5f/s;
		w = (mat.m[j][k]-mat.m[k][j]) * s;
		*fj = (mat.m[i][j]+mat.m[j][i]) * s;
		*fk = (mat.m[i][k]+mat.m[k][i]) * s;
	}
}


/*************************************************
	Name:		Quaternion::RotateVectorByQuaternion
	Purpose:	Rotate around this quat
	Parameters:	
	Info:		Actually vector math, it makes a rotation first.
**************************************************/
Vector Quaternion::RotateVectorByQuaternion( Vector &v )
{
	Quaternion q = ToRotation( );
	return q.RotateVectorByRotation( v );
}


/*************************************************
	Name:		Quaternion::RotateVectorByRotation
	Purpose:	Rotate around normal by w
	Parameters:	
	Info:		
**************************************************/
Vector Quaternion::RotateVectorByRotation( Vector &v )
{
	float m,n,sinTheta,cosTheta;
	Vector mVec,pVec,vVec,res;

	m = v.x*x;
	m += v.y*y;
	m += v.z*z;

	mVec.x = m*x;
	mVec.y = m*y;
	mVec.z = m*z;

	pVec = v - mVec;

	vVec = CrossProduct( ToNormal( ), pVec );

	m = pVec.Size();

	if( !m )
		res = v;
	else
	{
		n = vVec.Size( );
		if( n )
		{
			m /= n;
			vVec *= m;
		}

		cosTheta = cosf(w);
		sinTheta = sinf(w);

		res.x = mVec.x + cosTheta * pVec.x + sinTheta * vVec.x;
		res.y = mVec.y + cosTheta * pVec.y + sinTheta * vVec.y;
		res.z = mVec.z + cosTheta * pVec.z + sinTheta * vVec.z;
	}

	return res;
}

