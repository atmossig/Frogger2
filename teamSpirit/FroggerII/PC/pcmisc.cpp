/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: pcmisc.cpp
	Programmer	: Andy Eder
	Date		: 05/05/99 17:42:44

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>
#include "incs.h"
#include "gelf.h"
#include <commctrl.h>

extern "C"
{

short mouseX = 20;
short mouseY = 20;

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void InitPCSpecifics()
{
	gelfInit();
	InitCommonControls();
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void DeInitPCSpecifics()
{
	gelfShutdown();
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
short *GetGelfBmpDataAsShortPtr(char *filename)
{
	uchar palette[768];
	int pptr = -1;
	int xdim,ydim,bpp;
	short *bmp;
	
	bmp = (short *)gelfLoad_BMP(filename,NULL,(void**)&pptr,NULL,NULL,NULL,
		a565Card?GELF_IFORMAT_16BPP565:GELF_IFORMAT_16BPP555
		,GELF_IMAGEDATA_TOPDOWN);

	return bmp;
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

void FreeGelfData(void *me)
{
	gelfDefaultFree(me);
}

///////////////////////////////////////////////////////////////////////////////////////

#define MAX_TIMERS 13

long tStart[MAX_TIMERS];
long tTotal[MAX_TIMERS];
long tEnd[MAX_TIMERS];
long tS;

long tHold[MAX_TIMERS];

char *tName[MAX_TIMERS];
float timerscale = 0.4;

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

long first = 1;

void ClearTimers(void)
{
	FILE *fp;
	if (first)
	{
		fp = fopen("c:\\froghold.fhd","rb");
	}

	for (int i=0; i<MAX_TIMERS; i++)
	{
		tStart[i] = 0;
		tName[i] = 0;
		tEnd[i] = 0;
		tTotal[i] = 0;
		tS = timeGetTime();
		if (first)
		{
			if (fp)
				fread (&tHold[i],4,1,fp);
		}
	}

	if (first)
	{
		if (fp)
			fclose(fp);
		first = 0;
	}
}

void HoldTimers(void)
{
	FILE *fp = fopen("c:\\froghold.fhd","wb");
	
	for (int i=0; i<MAX_TIMERS; i++)
	{
		tHold[i] = tTotal[i];

		if (fp)
			fwrite (&tHold[i],4,1,fp);
	
	}

	if (fp)
		fclose(fp);	
	
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void StartTimer(int number,char *name)
{
	if (number<MAX_TIMERS)
	{
		tName[number] = name;
		tStart[number] = timeGetTime();
	}
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

void EndTimer(int number)
{
	if (number<MAX_TIMERS)
	{
		tEnd[number] = timeGetTime();
		tTotal[number]+=tEnd[number]-tStart[number];		
	}
}

void PrintTimers(void)
{
	RECT r;
	int i;

	r.top = 50-(200/MAX_TIMERS);
	r.bottom = 49+((MAX_TIMERS+2)*(200/MAX_TIMERS));
	r.left = 40;
	r.right = 600;
	
	DrawFlatRect(r, D3DRGBA(0,0,0,0.8) );
	HDC hdc;
	HRESULT res = IDirectDrawSurface4_GetDC(hiddenSrf, &hdc);
	if (res == DD_OK)
		{
			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, RGB(255,0,0));
			
			for (i=0; i<MAX_TIMERS; i++)
				if (tName[i])
					TextOut(hdc, 60, 48+(i*(200/MAX_TIMERS)), tName[i], strlen(tName[i]));
			IDirectDrawSurface4_ReleaseDC(hiddenSrf, hdc);
		}


	for ( i=0; i<MAX_TIMERS; i++)
	{
		r.top = 50+(i*(200/MAX_TIMERS));
		r.bottom = 49+((i+1)*(200/MAX_TIMERS));
		r.left = (50+tHold[i]/timerscale);;
		r.right = (52+tHold[i]/timerscale);
		
		DrawFlatRect(r, D3DRGBA(1,0.5,1,0.8) );

		
	}
	
	for (i=0; i<MAX_TIMERS; i++)
	{
		r.top = 50+(i*(200/MAX_TIMERS));
		r.bottom = 49+((i+1)*(200/MAX_TIMERS));

		r.left = 350;
		r.right = 352;
		DrawFlatRect(r, D3DRGBA(1,1,1,0.8) );

		r.left = 50;
		r.right = 52;
		DrawFlatRect(r, D3DRGBA(1,1,1,0.8) );

		r.right = (50+tTotal[i]/timerscale);
		
		DrawFlatRect(r, D3DRGBA(1,0,0,0.5) );
		
		r.left = (350+(tStart[i]-tS));
		r.right= r.left+(tTotal[i]);

		DrawFlatRect(r, D3DRGBA(0,1,1,0.5) );
		
		r.left = (350+(tStart[i]-tS));
		r.right= r.left+2;

		DrawFlatRect(r, D3DRGBA(0.5,1,1,0.5) );

		
		
	}


}


}
