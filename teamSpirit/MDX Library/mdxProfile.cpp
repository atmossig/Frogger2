/*

	This file is part of the M libraries,

	File		: 
	Programmer	: Matthew Cloy
	Date		: 

----------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <crtdbg.h>
#include <stdio.h>

#include "mdxDDraw.h"
#include "mdxD3D.h"
#include "mdxMath.h"
#include "mdxTexture.h"
#include "mdxObject.h"
#include "mdxActor.h"
#include "mdxCRC.h"
#include "mgeReport.h"
#include "mdxPoly.h"
#include "gelf.h"

#ifdef __cplusplus
extern "C"
{
#endif

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
		fp = fopen("c:\\hold.hif","rb");
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
	FILE *fp = fopen("c:\\hold.hif","wb");
	
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

unsigned long timeY = 200;

void PrintTimers(void)
{
	RECT r;
	int i;
	HDC hdc;
	
	pDirect3DDevice->BeginScene();
	
	r.top = timeY+50-(200/MAX_TIMERS);
	r.bottom = timeY+49+((MAX_TIMERS+2)*(200/MAX_TIMERS));
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
	
	r.bottom = timeY+49+((MAX_TIMERS+2)*(200/MAX_TIMERS));
	r.top = r.bottom - 5;
	DrawFlatRect(r, D3DRGBA(0,0,1,0.8));
	
	for ( i=0; i<MAX_TIMERS; i++)
	{
		r.top = timeY+50+(i*(200/MAX_TIMERS));
		r.bottom = timeY+49+((i+1)*(200/MAX_TIMERS));
		r.left = (50+tHold[i]/timerscale);;
		r.right = (52+tHold[i]/timerscale);
		
		DrawFlatRect(r, D3DRGBA(1,0.5,1,0.8) );
	}
	
	for (i=0; i<MAX_TIMERS; i++)
	{
		r.top = timeY+50+(i*(200/MAX_TIMERS));
		r.bottom = timeY+49+((i+1)*(200/MAX_TIMERS));
		r.left = 50;
		r.right = 52;
		DrawFlatRect(r, D3DRGBA(1,1,1,0.8) );
		r.right = (50+tTotal[i]/timerscale);
		DrawFlatRect(r, D3DRGBA(1,0,0,0.5) );
	}

	r.top = timeY+50+(0*(200/MAX_TIMERS));
	r.bottom = timeY+49+((MAX_TIMERS)*(200/MAX_TIMERS));
	
	r.left = (50+(1000/60)/timerscale);;
	r.right = (52+(1000/60)/timerscale);
	DrawFlatRect(r, D3DRGBA(1,1,1,0.5) );
	
	r.left = (50+(1000/30)/timerscale);;
	r.right = (52+(1000/30)/timerscale);
	DrawFlatRect(r, D3DRGBA(1,1,1,0.5) );
	
	r.left = (50+(1000/15)/timerscale);;
	r.right = (52+(1000/15)/timerscale);
	DrawFlatRect(r, D3DRGBA(1,1,1,0.5) );

	r.left = (50+(1000/7)/timerscale);;
	r.right = (52+(1000/7)/timerscale);
	DrawFlatRect(r, D3DRGBA(1,1,1,0.5) );

	pDirect3DDevice->EndScene();

	HRESULT res = IDirectDrawSurface4_GetDC(surface[RENDER_SRF], &hdc);

	if (res == DD_OK)
	{
		HFONT hfnt, hOldFont;      
		hfnt = (HFONT)GetStockObject(ANSI_VAR_FONT); 
		if (hOldFont = (HFONT)SelectObject(hdc, hfnt)) 
		{
			SetBkMode(hdc, TRANSPARENT);
				
			for (i=0; i<MAX_TIMERS; i++)
				if (tName[i])
				{			
					SetTextColor(hdc, RGB(55,0,0));
					TextOut(hdc, 61, timeY+49+(i*(200/MAX_TIMERS)), tName[i], strlen(tName[i]));
					SetTextColor(hdc, RGB(255,255,255));
					TextOut(hdc, 60, timeY+48+(i*(200/MAX_TIMERS)), tName[i], strlen(tName[i]));
				}

			SelectObject(hdc, hOldFont); 
		}

		IDirectDrawSurface4_ReleaseDC(surface[RENDER_SRF], hdc);	
	}
	
}

#ifdef __cplusplus
}
#endif
