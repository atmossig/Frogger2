/*	A demo of i-MAGE (Written for E3'99)
	Written by Matthew Cloy (c)1999 Interactive Studios Ltd.
	
	directx.cpp		: All directx related gumph.

	Created			: Sat 01 May 1999 - Matthew Cloy - Chesham

********************************************************************************************/

#ifndef DIRECTX_H_INCLUDED_198725368872
#define DIRECTX_H_INCLUDED_198725368872

#define RELEASE(x) if (x) {x->Release(); x=NULL;}
#define DDINIT(x) {memset(&x,0,sizeof(x)); x.dwSize = sizeof(x);}

#define SCREEN_WIDTH	640		// 320
#define SCREEN_HEIGHT	480		// 240

#ifdef __cplusplus
extern "C"
{
#endif

extern LPDIRECTDRAW				pDirectDraw;
extern LPDIRECTDRAWSURFACE		primarySrf;
extern LPDIRECTDRAWSURFACE		hiddenSrf;
extern LPDIRECT3D2				pDirect3D;
extern LPDIRECT3DDEVICE2		pDirect3DDevice;
extern LPDIRECT3DVIEWPORT2		pDirect3DViewport;
extern long a565Card;

extern int dumpScreen;
extern int prim;

long DirectXInit(HWND window, long hardware );
void SetupRenderstates(void);
void DirectXFlip(void);

extern  LPDIRECTDRAWSURFACE CreateTextureSurface(long xs,long ys, short *data, BOOL hardware, long cKey, long aiSurf);
extern D3DTEXTUREHANDLE ConvertSurfaceToTexture(LPDIRECTDRAWSURFACE srf);
void DrawAHardwarePoly (D3DTLVERTEX *v,long vC, short *fce, long fC, D3DTEXTUREHANDLE h);
void DrawALine (float x1, float y1, float x2, float y2, D3DCOLOR color);
void DrawASprite (float x, float y, float xs, float ys, float u1, float v1, float u2, float v2, D3DTEXTUREHANDLE h);
void DrawAlphaSprite (float x, float y, float z, float xs, float ys, float u1, float v1, float u2, float v2, D3DTEXTUREHANDLE h, float alpha);
void DrawFlatRect(RECT r, D3DCOLOR colour);
void BeginDrawHardware (void);
void EndDrawHardware (void);
extern void ReleaseSurface(LPDIRECTDRAWSURFACE surf);
BOOL CALLBACK DLGKeyMapDialogue(HWND hDlg,UINT msg,WPARAM wParam,LPARAM lParam);


extern void dp(char *format, ...);


#ifdef __cplusplus
}
#endif



#endif