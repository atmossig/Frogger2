#ifdef __cplusplus
extern "C"
{
#endif

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <crtdbg.h>
#include <stdio.h>

#include "mdxDDraw.h"
#include "fxBlur.h"

/*	--------------------------------------------------------------------------------
	Function	: D3DSetupRenderstates
	Purpose		: Setup Renderstates from a list
	Parameters	: a pointer to a pair of unsigned long values
	Returns		:
	Info		: 
*/

void fxBlurSurfaces(LPDIRECTDRAWSURFACE7 scr, LPDIRECTDRAWSURFACE7 bak, unsigned long is565)
{
	DDSURFACEDESC2 a,b;
	unsigned short maskVal;

	if (is565)
		maskVal = ((0x0f) | (0x1f<<5) | (0x0f<<11));
	else
		maskVal = ((0x0f) | (0x0f<<5) | (0x0f<<10));

	DDINIT(a);
	DDINIT(b);

	while (bak->Lock(NULL,&a,DDLOCK_SURFACEMEMORYPTR,NULL)!=DD_OK)
	{
		// IDLETIME
	}

	while (scr->Lock(NULL,&b,DDLOCK_SURFACEMEMORYPTR,NULL)!=DD_OK)
	{
		// IDLETIME
	}

	fxBlur((unsigned short *)b.lpSurface, (unsigned short *)a.lpSurface, b.lPitch, a.lPitch, maskVal);
	
	scr->Unlock(NULL);
	bak->Unlock(NULL);

	while (bak->BltFast(0,0,scr,NULL,NULL)!=DD_OK)
	{
		// IDLETIME
	}
}

/*	--------------------------------------------------------------------------------
	Function	: D3DSetupRenderstates
	Purpose		: Setup Renderstates from a list
	Parameters	: a pointer to a pair of unsigned long values
	Returns		:
	Info		: 
*/

void fxBlur(unsigned short *screen, unsigned short *oldscreen, unsigned long pitch1, unsigned long pitch2, unsigned short maskVal)
{
	unsigned long scrPtr = (unsigned long)screen;
	unsigned long oldPtr = (unsigned long)oldscreen;
	unsigned long add1 = pitch1/2-640;
	unsigned long add2 = pitch2/2-640;
	unsigned long i,j;
	unsigned short tVal;
	
	for (j=240; j; j--)
	{
		for (i=640; i; i--)
		{
			tVal = ((*screen)>>1) & maskVal;
			tVal += ((*oldscreen)>>1) & maskVal;
			
			*screen = tVal;

			screen++;
			oldscreen++;
		}
		screen+=add1;
		oldscreen+=add2;		
	}
/*
	__asm
	{
		push ebp

		mov bp,[maskVal]
		mov esi,[oldPtr]
		mov edi,[scrPtr]
		
		mov ebx,480
		loopY:
		mov ecx,640
		loopX:

		mov ax,[esi+ecx*2]
		mov bx,[edi+ecx*2]

		sub ecx,1
		jnz loopX
		
		//add esi,[add2]
		//add edi,[add1]

		sub ebx,1
		jnz loopY

		pop ebp
	}*/
}

#ifdef __cplusplus
}
#endif
