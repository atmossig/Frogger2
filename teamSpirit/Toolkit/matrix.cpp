#include <math.h>

#include "vertex.h"
#include "matrix.h"


Matrix::Matrix()
{
	Identity();
}


void Matrix::Identity()
{
	m[0][0] = 1;
	m[0][1] = 0;
	m[0][2] = 0;
	m[0][3] = 0;
	
	m[1][0] = 0;
	m[1][1] = 1;
	m[1][2] = 0;
	m[1][3] = 0;

	m[2][0] = 0;
	m[2][1] = 0;
	m[2][2] = 1;
	m[2][3] = 0;

	m[3][0] = 0;
	m[3][1] = 0;
	m[3][2] = 0;
	m[3][3] = 1;
}


Matrix Matrix::operator *(Matrix &o)
{
	Matrix t;

	t.m[0][0] = m[0][0]*o.m[0][0] + m[1][0]*o.m[0][1] + m[2][0]*o.m[0][2];
	t.m[1][0] = m[0][0]*o.m[1][0] + m[1][0]*o.m[1][1] + m[2][0]*o.m[1][2];
	t.m[2][0] = m[0][0]*o.m[2][0] + m[1][0]*o.m[2][1] + m[2][0]*o.m[2][2];
	t.m[3][0] = m[0][0]*o.m[3][0] + m[1][0]*o.m[3][1] + m[2][0]*o.m[3][2];

	t.m[0][1] = m[0][1]*o.m[0][0] + m[1][1]*o.m[0][1] + m[2][1]*o.m[0][2];
	t.m[1][1] = m[0][1]*o.m[1][0] + m[1][1]*o.m[1][1] + m[2][1]*o.m[1][2];
	t.m[2][1] = m[0][1]*o.m[2][0] + m[1][1]*o.m[2][1] + m[2][1]*o.m[2][2];
	t.m[3][1] = m[0][1]*o.m[3][0] + m[1][1]*o.m[3][1] + m[2][1]*o.m[3][2];

	t.m[0][2] = m[0][2]*o.m[0][0] + m[1][2]*o.m[0][1] + m[2][2]*o.m[0][2];
	t.m[1][2] = m[0][2]*o.m[1][0] + m[1][2]*o.m[1][1] + m[2][2]*o.m[1][2];
	t.m[2][2] = m[0][2]*o.m[2][0] + m[1][2]*o.m[2][1] + m[2][2]*o.m[2][2];
	t.m[3][2] = m[0][2]*o.m[3][0] + m[1][2]*o.m[3][1] + m[2][2]*o.m[3][2];

	t.m[0][3] = m[0][3]*o.m[0][0] + m[1][3]*o.m[0][1] + m[2][3]*o.m[0][2];
	t.m[1][3] = m[0][3]*o.m[1][0] + m[1][3]*o.m[1][1] + m[2][3]*o.m[1][2];
	t.m[2][3] = m[0][3]*o.m[2][0] + m[1][3]*o.m[2][1] + m[2][3]*o.m[2][2];
	t.m[3][3] = m[0][3]*o.m[3][0] + m[1][3]*o.m[3][1] + m[2][3]*o.m[3][2];

	return t;
}


void Matrix::Rotation(vec &v)
{
	Rotation(v.x, v.y, v.z);
}


void Matrix::Rotation(float x, float y, float z)
{
	static float sinX,cosX;
	static float sinY,cosY;
	static float sinZ,cosZ;

	sinX = (float)sin(x);	cosX = (float)cos(x);
	sinY = (float)sin(y);	cosY = (float)cos(y);
	sinZ = (float)sin(z);	cosZ = (float)cos(z);

	m[0][0]= sinY*sinX*sinZ + cosY*cosZ;
	m[0][1]= cosX*sinZ;
	m[0][2]= sinY*cosZ - cosY*sinX*sinZ;
	m[0][3]= 0;

	m[1][0]= sinY*sinX*cosZ - cosY*sinZ;
	m[1][1]= cosX*cosZ;
	m[1][2]= -cosY*sinX*cosZ - sinY*sinZ;
	m[1][3]= 0;

	m[2][0]= -sinY*cosX;
	m[2][1]= sinX;
	m[2][2]= cosY*cosX;
	m[2][3]= 0;

	m[3][0]= 0;
	m[3][1]= 0;
	m[3][2]= 0;
	m[3][3]= 1;
}


Matrix Matrix::invert()
{
	Matrix t;

	t.m[0][0] = m[0][0];
	t.m[0][1] = m[1][0];
	t.m[0][2] = m[2][0];
	t.m[0][3] = m[3][0];

	t.m[1][0] = m[0][1];
	t.m[1][1] = m[1][1];
	t.m[1][2] = m[2][1];
	t.m[1][3] = m[3][1];

	t.m[2][0] = m[0][2];
	t.m[2][1] = m[1][2];
	t.m[2][2] = m[2][2];
	t.m[2][3] = m[3][2];
	
	t.m[3][0] = m[0][3];
	t.m[3][1] = m[1][3];
	t.m[3][2] = m[2][3];
	t.m[3][3] = m[3][3];

	return t;
}


void Matrix::Vertices(vec &a,vec &b,vec &c)
{
	m[0][0]= a.x;
	m[0][1]= b.x;
	m[0][2]= c.x;
	m[0][3]= 0;

	m[1][0]= a.y;
	m[1][1]= b.y;
	m[1][2]= c.y;
	m[1][3]= 0;

	m[2][0]= a.z;
	m[2][1]= b.z;
	m[2][2]= c.z;
	m[2][3]= 0;

	m[3][0]= 0;
	m[3][1]= 0;
	m[3][2]= 0;
	m[3][3]= 1;
}
