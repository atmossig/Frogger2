
#ifndef MDXPOLY_H_INCLUDED
#define MDXPOLY_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

#define MAVIS_DEBUG

//-------------------------------------------------------------

#define MA_MAX_VERTICES		24000	// Maximum number of VERTICES that can be drawn in a frame
#define MA_MAX_FACES		24000	// Maximum number of FACES that can be drawn in a frame
//#define MA_MAX_FRAMES		6		// Maximum number of frames that may be active at once.

enum {MA_FRAME_NORMAL,MA_FRAME_XLU,MA_FRAME_ADDITIVE,MA_FRAME_LIGHTMAP,MA_FRAME_PHONG,MA_MAX_FRAMES};

// A type for storing all the vertices and polygons in a frame
typedef struct TAG_FRAME_INFO	
{
	// Vertex information
	unsigned long nV;						// Number of vertices
	D3DTLVERTEX *cV;						// Current vertex pointer
	D3DTLVERTEX v[MA_MAX_VERTICES];			// Actual vertex buffer
	
	// Face information
	unsigned long nF;						// Number of faces
	unsigned short *cF;						// Current face pointer
	unsigned short f[MA_MAX_FACES];			// Actual face buffer
	
	// Texture information
	LPDIRECTDRAWSURFACE7 *cT;				// Current texture;
	LPDIRECTDRAWSURFACE7 t[MA_MAX_FACES];	// Actual texture pointers
} FRAME_INFO;

//--------------------------------------------------------------

extern FRAME_INFO frameInfo[MA_MAX_FRAMES];
extern FRAME_INFO *cFInfo,*oFInfo;
// Blanks out current frame
#define BlankFrame {\
cFInfo->nV = cFInfo->nF = 0;	cFInfo->cV = cFInfo->v;	cFInfo->cF = cFInfo->f;	cFInfo->cT = cFInfo->t;\
}

// Swap to a different frameset
#define SwapFrame(to) {cFInfo = &frameInfo[to];}
#define SaveFrame {oFInfo = cFInfo;}
#define RestoreFrame {cFInfo = oFInfo;}

// Draw The Polys
void DrawBatchedPolys (void);
void InitFrames(void);


// Push a poly onto the buffers
#ifdef MAVIS_DEBUG

void PushPolys( D3DTLVERTEX *v, int vC, short *fce, long fC, LPDIRECTDRAWSURFACE7 tSrf );
void DrawFlatRect(RECT r, D3DCOLOR colour);
void DrawTexturedRect(RECT r, D3DCOLOR colour, LPDIRECTDRAWSURFACE7 tex, float u0, float v0, float u1, float v1);
void BlankAllFrames(void);
void DrawAllFrames(void);

#define FULL_TEXTURE 0,0,1,1

extern unsigned long drawLighting;
extern unsigned long drawPhong;


#else

#define PushPolys(v,vC,fce,fC,h) \
{	unsigned long cnt;\
	short *mfce = fce;\
	for (cnt=0;cnt<fC; cnt++)\
	{\
		*cFInfo->cF = (*mfce) + cFInfo->nV;\
		*cFInfo->cT = h;\
		cFInfo->cF++;\
		cFInfo->cT++;\
		mfce++;\
	}\
	memcpy(cFInfo->cV,v,vC*sizeof(D3DTLVERTEX)); cFInfo->cV+=vC; cFInfo->nV+=vC; cFInfo->nF+=fC;\
}

#endif

#ifdef __cplusplus
}
#endif

#endif
