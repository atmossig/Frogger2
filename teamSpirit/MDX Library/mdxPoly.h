
#ifndef MDXPOLY_H_INCLUDED
#define MDXPOLY_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

#define MAVIS_DEBUG

//-------------------------------------------------------------

#define MA_MAX_VERTICES		124000	// Maximum number of VERTICES that can be drawn in a frame
#define MA_MAX_FACES		124000	// Maximum number of FACES that can be drawn in a frame
#define MA_MAX_HALOS		50		// Maximum number of halo points that can be drawn in a frame
#define MA_SOFTWARE_DEPTH	3000	// Maximum depth of a software polygon.
//#define MA_MAX_FRAMES		6		// Maximum number of frames that may be active at once.

#define MA_SORTNONE			0
#define MA_SORTFRONTBACK	1
#define MA_SORTBACKFRONT	2

enum {MA_FRAME_NORMAL,MA_FRAME_GLOW,MA_FRAME_XLU,MA_FRAME_ADDITIVE,MA_FRAME_LIGHTMAP,MA_FRAME_PHONG,MA_FRAME_OVERLAY,MA_FRAME_COLORKEY,MA_FRAME_WRAP,MA_MAX_FRAMES};

typedef struct TAG_SOFTPOLY
{
	struct TAG_SOFTPOLY *next; 
	unsigned short f[3];
	long flags;
	LPDIRECTDRAWSURFACE7 t;
	MDX_TEXENTRY *tEntry;
} SOFTPOLY;

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
	char *cK;								// Current Key
	char key[MA_MAX_FACES];					// Colorkey Flags
} FRAME_INFO;

//--------------------------------------------------------------

extern FRAME_INFO frameInfo[MA_MAX_FRAMES];
extern FRAME_INFO *cFInfo,*oFInfo;

// Blanks out current frame
#define BlankFrame {\
cFInfo->nV = cFInfo->nF = 0;	cFInfo->cV = cFInfo->v;	cFInfo->cF = cFInfo->f;	cFInfo->cT = cFInfo->t; cFInfo->cK = cFInfo->key;\
}

// Swap to a different frameset
#define SwapFrame(to) {cFInfo = &frameInfo[to];}
#define SaveFrame {oFInfo = cFInfo;}
#define RestoreFrame {cFInfo = oFInfo;}

// Draw The Polys
void DrawBatchedPolys (void);
void InitFrames(void);
void SetTexture(MDX_TEXENTRY *me);
HRESULT DrawPoly(D3DPRIMITIVETYPE d3dptPrimitiveType,DWORD  dwVertexTypeDesc, LPVOID lpvVertices, DWORD  dwVertexCount, LPWORD lpwIndices, DWORD  dwIndexCount, DWORD  dwFlags);
//void PushPolys( D3DTLVERTEX *v, int vC, short *fce, long fC, MDX_TEXENTRY *tEntry );
// Add a halo point to be tested at draw time/
void AddHalo(MDX_VECTOR *point, float flareScaleA,float flareScaleB, unsigned long color, unsigned long size);

void PushPolys_Software( D3DTLVERTEX *v, int vC, short *fce, long fC, MDX_TEXENTRY *tEntry);

#define PushPolys(v,vC,fce,fC,tEntry)\
{\
	long cnt;\
	short *mfce = fce;\
	LPDIRECTDRAWSURFACE7 tH = 0;\
	char key;\
	if (!rHardware)\
	{\
		if (tEntry)\
		{\
			PushPolys_Software(v,vC,fce,fC,tEntry);\
			memcpy(&softV[numSoftVertex],v,vC*sizeof(D3DTLVERTEX));\
			numSoftVertex+=vC;\
		}\
	}\
	else\
	{\
	if  (tEntry)\
	{\
		tH = tEntry->surf;\
		key = tEntry->keyed;\
	}\
	if ((cFInfo->nV + vC) < MA_MAX_VERTICES && (cFInfo->nF + fC) < MA_MAX_FACES)\
	{\
		\
			for (cnt=fC;cnt; cnt--)\
			{\
				*cFInfo->cF = (unsigned short)((*mfce) + cFInfo->nV);\
				*cFInfo->cT = tH;\
				*cFInfo->cK = key;\
				cFInfo->cF++;\
				cFInfo->cT++;\
				cFInfo->cK++;\
				mfce++;\
			}\
		\
			memcpy(cFInfo->cV,v,vC*sizeof(D3DTLVERTEX));\
			\
			cFInfo->cV+=vC;\
			cFInfo->nV+=vC;\
			cFInfo->nF+=fC;\
		}\
	}\
}


// Push a poly onto the buffers

void CopySoftScreenToSurface(LPDIRECTDRAWSURFACE7 srf);

void DrawFlatRect(RECT r, D3DCOLOR colour);
void DrawTexturedRect(RECT r, D3DCOLOR colour, LPDIRECTDRAWSURFACE7 tex, float u0, float v0, float u1, float v1);
void DrawTexturedRect2(RECT r, D3DCOLOR colour, float u0, float v0, float u1, float v1);
void BlankAllFrames(void);
void DrawAllFrames(void);
void SetSoftwareState(unsigned long *me);
void DrawAlphaSprite (float x, float y, float z, float xs, float ys, float u1, float v1, float u2, float v2, MDX_TEXENTRY *tex, DWORD colour );
void DrawAlphaSpriteRotating(MDX_VECTOR *pos,float angle,float x, float y, float z, float xs, float ys, float u1, float v1, float u2, float v2, MDX_TEXENTRY *tex, DWORD colour );

#define FULL_TEXTURE 0,0,1,1

extern MDX_TEXENTRY *cTexture;
extern unsigned long totalFacesDrawn;
extern unsigned long drawLighting;
extern unsigned long drawPhong;
extern LPDIRECTDRAWSURFACE7 haloS;
extern LPDIRECTDRAWSURFACE7 flareS;
extern LPDIRECTDRAWSURFACE7 flareS2;
extern unsigned long numSoftVertex;
extern D3DTLVERTEX softV[MA_MAX_VERTICES];

extern unsigned long lightingMapRS[];
extern unsigned long normalAlphaCmpRS[];
extern unsigned long tightAlphaCmpRS[];
extern unsigned long normalZRS[];
extern unsigned long xluZRS[];
extern unsigned long xluSubRS[];
extern unsigned long xluAddRS[];
extern unsigned long xluSemiRS[];
extern unsigned long noZRS[];
extern unsigned long cullNoneRS[];
extern unsigned long cullCWRS[];
extern unsigned long cullCCWRS[];
extern unsigned long sortMode;
extern unsigned long numSeperates;
extern unsigned long fogging;

extern short softScreen[640*480];

#ifdef __cplusplus
}
#endif

#endif
