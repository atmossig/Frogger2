#ifndef MAVIS_MAKING_DINNER
#define MAVIS_MAKING_DINNER

//-------------------------------------------------------------

#ifdef RELEASE_BUILD
#error
#endif
#define MA_MAX_VERTICES		32000	// Maximum number of VERTICES that can be drawn in a frame
#define MA_MAX_FACES		24000	// Maximum number of FACES that can be drawn in a frame
#define MA_MAX_FRAMES		4		// Maximum number of frames that may be active at once.

#define MAX_FRAMES	17

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
	unsigned long *cH;						// Current handle pointer (handles match to faces in number)
	unsigned long h[MA_MAX_FACES];			// Actual Handle buffer
} FRAME_INFO;

//--------------------------------------------------------------

extern FRAME_INFO frameInfo[MAX_FRAMES];
extern FRAME_INFO *cFInfo;

// Blanks out current frame
#define BlankFrame(x) {\
cFInfo->nV = cFInfo->nF = 0;	cFInfo->cV = cFInfo->v;	cFInfo->cF = cFInfo->f;	cFInfo->cH = cFInfo->h;\
}

// Swap to a different frameset
#define SwapFrame(to) {cFInfo = &frameInfo[to];}

// Push a poly onto the buffers
#ifdef _DEBUG

void PushPolys( D3DTLVERTEX *v, int vC, short *fce, long fC, long h );

#else

#define PushPolys(v,vC,fce,fC,h) \
{	unsigned long cnt;\
	short *mfce = fce;\
	for (cnt=0;cnt<fC; cnt++)\
	{\
		*cFInfo->cF = (*mfce) + cFInfo->nV;\
		*cFInfo->cH = h;\
		cFInfo->cF++;\
		cFInfo->cH++;\
		mfce++;\
	}\
	memcpy(cFInfo->cV,v,vC*sizeof(D3DTLVERTEX)); cFInfo->cV+=vC; cFInfo->nV+=vC; cFInfo->nF+=fC;\
}

#endif

#endif
