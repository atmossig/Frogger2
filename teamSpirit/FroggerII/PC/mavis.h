#ifndef MAVIS_MAKING_DINNER
#define MAVIS_MAKING_DINNER

//-------------------------------------------------------------

#define MA_MAX_VERTICES		32000	// Maximum number of VERTICES that can be drawn
#define MA_MAX_FACES		24000	// Maximum number of FACES that can be drawn

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

extern FRAME_INFO frameInfo;

// Blanks out current frame
#define BlankFrame(x) {\
frameInfo.nV = frameInfo.nF = 0;	frameInfo.cV = frameInfo.v;	frameInfo.cF = frameInfo.f;	frameInfo.cH = frameInfo.h;\
}

// Push a poly onto the buffers
#ifdef _DEBUG

void PushPolys( D3DTLVERTEX *v, int vC, short *fce, long fC, long h );

#else

#define PushPolys(v,vC,fce,fC,h) \
{	unsigned long cnt;\
	short *mfce = fce;\
	for (cnt=0;cnt<fC; cnt++)\
	{\
		*frameInfo.cF = (*mfce) + frameInfo.nV;\
		*frameInfo.cH = h;\
		frameInfo.cF++;\
		frameInfo.cH++;\
		mfce++;\
	}\
	memcpy(frameInfo.cV,v,vC*sizeof(D3DTLVERTEX)); frameInfo.cV+=vC; frameInfo.nV+=vC; frameInfo.nF+=fC;\
}

#endif

#endif
