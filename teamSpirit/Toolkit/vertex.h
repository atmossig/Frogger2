/*************************************************
	Name:		vertex.h
	Purpose:	Basic point; Mathematical vector; Drawing vertex
	Date:		23/10/99
	Author:		Jim Hubbard
**************************************************/
#ifndef __VERTEX_H
#define __VERTEX_H

#include <math.h>
#include "matrix.h"

struct vec
{
	float x,y,z;
};


class Vector : public vec
{
public:
	Vector() { x=y=z=0; }
	Vector(float _x,float _y, float _z) { x=_x;y=_y;z=_z; }
	Vector(vec &v) { x=v.x; y=v.y; z=v.z; }

	Vector& operator = (vec &v) { x=v.x; y=v.y; z=v.z; return *this; }
	Vector operator + (vec &v) const { return Vector( x+v.x, y+v.y, z+v.z ); }
	Vector& operator += (vec &v) { x+=v.x; y+=v.y; z+=v.z; return *this; }
	Vector operator - (vec &v) const { return Vector( x-v.x, y-v.y, z-v.z ); }
	Vector& operator -= (vec &v) { x-=v.x; y-=v.y; z-=v.z; return *this; }
	Vector operator * (float s) const { return Vector( x*s, y*s, z*s ); }
	Vector& operator *= (float s) { x*=s; y*=s; z*=s; return *this; }
	Vector operator / (float s) const { return Vector( x/s, y/s, z/s ); }
	Vector& operator /= (float s) { x/=s; y/=s; z/=s; return *this; }
	Vector operator * (Matrix &m) { return Vector( x*m.m[0][0] + y*m.m[0][1] + z*m.m[0][2], 
													x*m.m[1][0] + y*m.m[1][1] + z*m.m[1][2], 
													x*m.m[2][0] + y*m.m[2][1] + z*m.m[2][2] ); }


	float Size() const { return( sqrtf(x*x + y*y + z*z) ); }
	float SizeSq() const { return( x*x + y*y + z*z ); }
	float Size2D() const { return( sqrtf(x*x + z*z) ); }
	float SizeSq2D() const { return( x*x + z*z ); }

	float Dist( Vector &v ) const { Vector tmp=*this-v; return tmp.Size(); }
	float DistSq( Vector &v ) const { Vector tmp=*this-v; return tmp.SizeSq(); }
	float Dist2D( Vector &v ) const { Vector tmp=*this-v; return tmp.Size2D(); }
	float DistSq2D( Vector &v ) const { Vector tmp=*this-v; return tmp.SizeSq2D(); }

	Vector &Set(float _x,float _y,float _z) { x=_x; y=_y; z=_z; return *this; }
	Vector &MakeUnit() { *this /= this->Size(); return *this; }
	float InBox( vec &v1, vec &v2 ) const { return( (x>=v1.x && x<=v2.x) && (y>=v1.y && y<=v2.y) && (z>=v1.z && z<=v2.z) ); }

	friend Vector CrossProduct( vec &a, vec &b ) { return Vector( a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x ); }
	friend float DotProduct( vec &a, vec &b ) { return (a.x * b.x) + (a.y * b.y) + (a.z * b.z); }
};

class Vertex : public Vector
{
public:
	Vector normal;
	float r, g, b, a, u, v;

	Vertex() { normal.Set(0,0,0); x=y=z=u=v=g=0; r=b=a=1; }
	Vertex(float _x,float _y, float _z) { normal.Set(0,0,0); x=_x;y=_y;z=_z; r=b=a=1; u=v=g=0; } // Default to magenta - it's easy to spot :)

	void Colour( float _r, float _g, float _b, float _a ) { r=_r;g=_g;b=_b;a=_a; }
	void UV( float _u, float _v ) { u=_u; v=_v; }
};

#endif
