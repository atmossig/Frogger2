#include "temp_psx.h"

void QuatToPSXMatrix(IQUATERNION* q, MATRIX* M)
{
    fixed Nq = FMul(q->x,q->x) + FMul(q->y,q->y)
    		 + FMul(q->z,q->z) + FMul(q->w,q->w);
    fixed s = (Nq > 0) ? FDiv(8192,Nq) : 0;
    fixed xs = FMul(q->x,s),	ys = FMul(q->y,s),	zs = FMul(q->z,s);
    fixed wx = FMul(q->w,xs),	wy = FMul(q->w,ys),	wz = FMul(q->w,zs);
    fixed xx = FMul(q->x,xs),	xy = FMul(q->x,ys),	xz = FMul(q->x,zs);
    fixed yy = FMul(q->y,ys),	yz = FMul(q->y,zs),	zz = FMul(q->z,zs);
    
//     M->m[0][0] = 4096 - (yy + zz);
//     M->m[0][1] = xy - wz;
//     M->m[0][2] = xz + wy;
//     
//     M->m[1][0] = xy + wz;
//     M->m[1][1] = 4096 - (xx + zz);
//     M->m[1][2] = yz - wx;
//     
//     M->m[2][0] = xz - wy;
//     M->m[2][1] = yz + wx;
//     M->m[2][2] = 4096 - (xx + yy);
    
    M->m[0][0] = 4096 - (yy + zz);
    M->m[1][0] = xy - wz;
    M->m[2][0] = xz + wy;
    
    M->m[0][1] = xy + wz;
    M->m[1][1] = 4096 - (xx + zz);
    M->m[2][1] = yz - wx;
    
    M->m[0][2] = xz - wy;
    M->m[1][2] = yz + wx;
    M->m[2][2] = 4096 - (xx + yy);


//    M.m[3][0]=
//    M.m[3][1]=
//    M.m[3][2]=
//    M.m[0][3]=
//    M.m[1][3]=
//    M.m[2][3]=0;
    M->t[0]=
    M->t[1]=
    M->t[2]=0;

	
	//bbtest

	//invert x axis
 	M->m[0][0] = -M->m[0][0];
 	M->m[1][0] = -M->m[1][0];
 	M->m[2][0] = -M->m[2][0];

	//invert y axis
// 	M->m[0][1] = -M->m[0][1];
// 	M->m[1][1] = -M->m[1][1];
// 	M->m[2][1] = -M->m[2][1];

	//invert z axis
 	M->m[0][2] = -M->m[0][2];
 	M->m[1][2] = -M->m[1][2];
 	M->m[2][2] = -M->m[2][2];

//    M.m[3][3]=4096;
}
