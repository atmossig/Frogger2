#ifndef _ISLUTIL_H_INCLUDE
#define _ISLUTIL_H_INCLUDE

#ifdef __cplusplus
extern "C"
{
#endif

int utilPrintf(char* fmt, ...);
unsigned long utilStr2CRC(char *ptr);

unsigned long utilSqrt(unsigned long num);
int utilCalcAngle(int adj, int opp);

#ifdef __cplusplus
}
#endif

#endif
