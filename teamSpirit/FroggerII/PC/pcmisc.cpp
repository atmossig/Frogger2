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
#include <ddraw.h>

short mouseX = 20;
short mouseY = 20;

#define _EXC_MASK    \
    _EM_UNDERFLOW  + \
    _EM_OVERFLOW   + \
    _EM_ZERODIVIDE + \
    _EM_INEXACT

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

#ifdef _DEBUG
	// throw floating point exceptions!
    _controlfp(_EXC_MASK, _MCW_EM); 
#endif
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

short *GetGelfBmpDataAsShortPtr(char *filename, unsigned long format)
{
	uchar palette[768];
	int pptr = -1;
	int xdim,ydim,bpp;
	short *bmp;
	
	if (!format)
		format = a565Card?GELF_IFORMAT_16BPP565:GELF_IFORMAT_16BPP555;

	bmp = (short *)gelfLoad_BMP(filename,NULL,(void**)&pptr,NULL,NULL,NULL,format,GELF_IMAGEDATA_TOPDOWN);

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

#define MAX_TIMERS 20

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

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
extern long drawTimers;

void PrintTimers(void)
{
	RECT r;
	int i;
	HDC hdc;

	
	r.top = 50-(200/MAX_TIMERS);
	r.bottom = 49+((MAX_TIMERS+2)*(200/MAX_TIMERS));
	r.left = 40;
	r.right = 600;
	DrawFlatRect(r, D3DRGBA(0,0.1,0,0.8));

	r.right = 45;
	DrawFlatRect(r, D3DRGBA(0,0,1,0.8));

	r.left = 595;
	r.right = 600;
	DrawFlatRect(r, D3DRGBA(0,0,1,0.8));
	
	r.bottom = r.top+9;
	r.bottom -= 17;
	r.left = 40;
	r.right = 600;
	DrawFlatRect(r, D3DRGBA(0,0,1,0.8));
	
	r.bottom = 49+((MAX_TIMERS+2)*(200/MAX_TIMERS));
	r.top = r.bottom - 5;
	DrawFlatRect(r, D3DRGBA(0,0,1,0.8));
	
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
		r.left = 50;
		r.right = 52;
		DrawFlatRect(r, D3DRGBA(1,1,1,0.8) );
		r.right = (50+tTotal[i]/timerscale);
		DrawFlatRect(r, D3DRGBA(1,0,0,0.5) );
	}

	r.top = 50+(0*(200/MAX_TIMERS));
	r.bottom = 49+((MAX_TIMERS)*(200/MAX_TIMERS));
	
	r.left = (50+(1000/60)/timerscale);;
	r.right = (52+(1000/60)/timerscale);
	DrawFlatRect(r, D3DRGBA(1,1,1,0.8) );
	
	r.left = (50+(1000/30)/timerscale);;
	r.right = (52+(1000/30)/timerscale);
	DrawFlatRect(r, D3DRGBA(1,1,1,0.8) );
	
	r.left = (50+(1000/15)/timerscale);;
	r.right = (52+(1000/15)/timerscale);
	DrawFlatRect(r, D3DRGBA(1,1,1,0.8) );

	r.left = (50+(1000/7)/timerscale);;
	r.right = (52+(1000/7)/timerscale);
	DrawFlatRect(r, D3DRGBA(1,1,1,0.8) );

	
//	if (drawTimers==2)
	{
		HRESULT res = IDirectDrawSurface4_GetDC(surface[RENDER_SRF], &hdc);

		if (res == DD_OK)
		{
			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, RGB(255,0,0));
			
			for (i=0; i<MAX_TIMERS; i++)
				if (tName[i])
					TextOut(hdc, 60, 49+(i*(200/MAX_TIMERS)), tName[i], strlen(tName[i]));

			IDirectDrawSurface4_ReleaseDC(surface[RENDER_SRF], hdc);	
		}
	}

	
}

void LoadTextureForTrophy( TEXTURE *tex )
{

}



/*	--------------------------------------------------------------------------------
	Function		: fileLoad
	Purpose			: loads a file into a memory buffer, allocating memory
	Parameters		: char *filename(in), int *bytesRead(out)
	Returns			: void*
*/
void *fileLoad(const char* filename, int *bytesRead)
{
	void *buffer;
	DWORD size, read;
	HANDLE h;

	h = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h == INVALID_HANDLE_VALUE) return NULL;
	size = GetFileSize(h, NULL);
	buffer = JallocAlloc(size, 0, "entLoad");
	ReadFile(h, buffer, size, &read, NULL);
	CloseHandle(h);
	
	if (bytesRead) *bytesRead = read;
	return buffer;
}

