/*************************************************
	Name:		matrix.h
	Purpose:	4x4 matrix class
	Date:		23/10/99
	Author:		Jim Hubbard
**************************************************/
#ifndef __MATRIX_H
#define __MATRIX_H

struct vec;

class Matrix
{
public:
	float m[4][4];
	Matrix();
	Matrix operator * (Matrix &);
	void Identity();
	void Rotation(vec &);
	void Rotation(float x, float y, float z);
	void Vertices(vec &,vec &,vec &);
	Matrix invert();
};

#endif
