#ifndef _ISLUTIL_H_INCLUDE
#define _ISLUTIL_H_INCLUDE

#include "crc32.h"

#ifdef __cplusplus
extern "C"
{
#endif

int utilPrintf(char* fmt, ...);

unsigned long utilSqrt(unsigned long num);
int utilCalcAngle(int adj, int opp);

// jhubbard
#define utilStr2CRC(x) UpdateCRC(x)

#ifdef __cplusplus
}
#endif

#endif
