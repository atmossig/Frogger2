#include "islutil.h"
#include "temp_pc.h"

#include <math.h>

//returns fixed format
unsigned long utilSqrt(unsigned long num)
{
	return sqrt(num)*4096;
}

//return angle -2048 to +2048
int utilCalcAngle(int adj, int opp)
{
	return (atan2(adj,opp) /PI2) * 4096.0;
}


int utilPrintf(char* fmt, ...)				
{
	return 0;
}

unsigned long utilStr2CRC(char *ptr) 		
{
	return 0;
}
