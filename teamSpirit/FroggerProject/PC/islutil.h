#ifndef _ISLUTIL_H_INCLUDE
#define _ISLUTIL_H_INCLUDE

int utilPrintf(char* fmt, ...);
unsigned long utilStr2CRC(char *ptr);

unsigned long utilSqrt(unsigned long num);
int utilCalcAngle(int adj, int opp);

#endif
