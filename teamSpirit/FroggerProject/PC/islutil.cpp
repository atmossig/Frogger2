#include "islutil.h"

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <windows.h>

#define DEBUG_FILE "C:\\frogger2.log"

//returns fixed format
unsigned long utilSqrt(unsigned long num)
{
//	return (unsigned long)(sqrt(num)*4096);
	return (unsigned long)(sqrt(num)*65536);
}

//return angle -2048 to +2048
/*int utilCalcAngle(int adj, int opp)
{
	return (int)((atan2(adj,opp) / 6.283185308) * 4096.0);
}*/


int utilPrintf(char* fmt, ...)				
{
	char buffer[1024];
	va_list args;
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);
	
	OutputDebugString(buffer);

	FILE *f = fopen(DEBUG_FILE, "a+ct");
	if (f)
	{
		fputs(buffer, f);
		fclose(f);
	}

	return 0;
}
