#ifndef MDXMATH_H_INCLUDED
#define MDXMATH_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
	float	vx,vy,vz;
}MDX_VECTOR;

typedef struct
{
	signed char	v[4];
}MDX_BYTEVECTOR;

typedef struct
{
	short	v[3];
}MDX_SHORTVECTOR;

typedef struct
{
	unsigned short	v[2];
}MDX_USHORT2DVECTOR;

typedef struct
{
	float	x, y, z, w;
}MDX_QUATERNION;

typedef struct
{
	float	matrix[4][4];
}MDX_MATRIX;

#define MAX_STACK_SIZE	16

typedef struct
{

	float	stack[MAX_STACK_SIZE][4][4];
	short	stackPosition;
	float	result[4][4];
}MDX_MATRIXSTACK;

extern MDX_MATRIXSTACK	matrixStack;

void guTranslateF(float a[4][4], float dx, float dy, float dz);
void guMtxCatF(float b[4][4], float a[4][4], float ret[4][4]);
void guScaleF(float a[4][4], float dx, float dy, float dz);
void guLookAtF (float m[4][4],float xEye, float yEye, float zEye,float xAt, float yAt, float zAt,float xUp, float yUp, float zUp);
void guMtxXFMF(float m[4][4],float srcX,float srcY,float srcZ,float *destX,float *destY,float *destZ);

#define EPSILON		((float)0.00001)
#define HALFPI		((float)1.570796326794895)

#define DotProduct(v1,v2)	((((v1)->vx)*((v2)->vx)) + (((v1)->vy)*((v2)->vy)) + (((v1)->vz)*((v2)->vz)))
#define mdxMagnitudeSquared(vect) ((vect)->vx*(vect)->vx + (vect)->vy*(vect)->vy + (vect)->vz*(vect)->vz)
#define mdxMagnitude(vect) ((float)sqrt((vect)->vx*(vect)->vx + (vect)->vy*(vect)->vy + (vect)->vz*(vect)->vz))
#define CrossProduct(result,operand1,operand2) {(result)->vx = (operand1)->vy * (operand2)->vz - (operand1)->vz * (operand2)->vy; (result)->vy = (operand1)->vz * (operand2)->vx - (operand1)->vx * (operand2)->vz; (result)->vz = (operand1)->vx * (operand2)->vy - (operand1)->vy * (operand2)->vx;}
#define SetQuaternion(q1,q2) (*(q1)) = (*(q2))
#define ZeroQuaternion(quat) SetQuaternion(quat,&zeroQuat)
#define Bound(a,x,b) Min(b,Max(a,x))
#define Max(p1,p2)	(((p1) > (p2)) ? (p1) : (p2))
#define Min(p1,p2)	(((p1) > (p2)) ? (p2) : (p1))
#define PushMatrix(matrix)	{ guMtxCatF((matrix), matrixStack.stack[matrixStack.stackPosition], matrixStack.stack[matrixStack.stackPosition+1]); matrixStack.stackPosition++; }
#define PopMatrix() (matrixStack.stackPosition--)
#define SetVector(v1,v2)	(*(v1)) = (*(v2))
#define MatrixSet(m1,m2) (*((MDX_MATRIX *)m1)) = (*((MDX_MATRIX *)m2))
#define LinearInterp(result,from,to,interp) {(result)->vx = (from)->vx + (((to)->vx - (from)->vx) * interp);(result)->vy = (from)->vy + (((to)->vy - (from)->vy) * interp);(result)->vz = (from)->vz + (((to)->vz - (from)->vz) * interp);}
#define AddToVector(res,v1) {(res)->vx += (v1)->vx;(res)->vy += (v1)->vy;(res)->vz += (v1)->vz;}
#define SubVector(res,v1,v2) {(res)->vx = (v1)->vx - (v2)->vx;(res)->vy = (v1)->vy - (v2)->vy;(res)->vz = (v1)->vz - (v2)->vz;}
#define AddVector(res,v1,v2) {(res)->vx = (v1)->vx + (v2)->vx;(res)->vy = (v1)->vy + (v2)->vy;(res)->vz = (v1)->vz + (v2)->vz;}
#define ScaleVector(v1,scale) {(v1)->vx *= scale;(v1)->vy *= scale;(v1)->vz *= scale;}

void QuaternionToMatrix(MDX_QUATERNION *squat, MDX_MATRIX *dmatrix);
void GetQuaternionFromRotation(MDX_QUATERNION *destQ,MDX_QUATERNION *srcQ);
void QuatSlerp(MDX_QUATERNION *src1, MDX_QUATERNION *sp2, float t, MDX_QUATERNION *dquat);
void QuaternionMultiply(MDX_QUATERNION *dest,MDX_QUATERNION *src1,MDX_QUATERNION *src2);
void MatrixToQuaternion(MDX_MATRIX *smatrix, MDX_QUATERNION *dquat);

extern MDX_QUATERNION zeroQuat;

void MatrixStackInitialise();

#ifdef __cplusplus
}
#endif

#endif
