/*************************************************
	Name:		quat.h
	Purpose:	Unit Quaternion class
	Date:		23/10/99
	Author:		Jim Hubbard
**************************************************/
#ifndef __QUAT_H
#define __QUAT_H


#include "vertex.h"
#include "matrix.h"


#define QEPSILON	0.00001
#define QHALFPI		1.570796326794895


struct quat
{
	float x, y, z, w;
};


class Quaternion : public quat
{
public:
	Quaternion( ) { x=y=z=w=0; }
	Quaternion( float _x, float _y, float _z, float _w ) { x=_x;y=_y;z=_z;w=_w; }

	void operator = (quat &q) { x=q.x; y=q.y; z=q.z; w=q.w; }
	void operator = (vec &q) { x=q.x; y=q.y; z=q.z; w=0; }
	void Set(float _x,float _y,float _z,float _w) { x=_x; y=_y; z=_z; w=_w; }

	Quaternion operator * ( quat &q );
	Quaternion Slerp( quat &q, float t );
	Quaternion SlerpFixedSpeed( quat &q, float speed );
	Matrix ToMatrix( );
	Quaternion ToRotation( );
	Quaternion ToQuaternion( );
	Vector ToNormal( ) { return Vector(x,y,z); }
	void FromMatrix( Matrix &m );

	Vector RotateVectorByQuaternion( Vector &v );
	Vector RotateVectorByRotation( Vector &v );
};


#endif