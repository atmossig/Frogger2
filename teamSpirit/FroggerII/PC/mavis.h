#ifndef MAVIS_MAKING_DINNER
#define MAVIS_MAKING_DINNER

//-------------------------------------------------------------

#define MA_MAX_VERTICES		16000	// Maximum number of VERTICES that can be drawn
#define MA_MAX_FACES		64000	// Maximum number of FACES that can be drawn

// A type for storing all the vertices and polygons in a frame
struct FRAME_INFO	
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

};
extern FRAME_INFO frameInfo;

//--------------------------------------------------------------

// Blanks out current frame
#define BlankFrame(x) {frameInfo.nV = frameInfo.nF = 0;	frameInfo.cV = frameInfo.v;	frameInfo.cF = frameInfo.f;	frameInfo.cH = frameInfo.h;}

// Push a poly onto the buffers
#define PushPolys(v,vC,fce,fC,h) \
{	unsigned long i = 0;\
	for (;i<fC; i++,curF++,curH++,fce++)\
		{*curF = (*fce) + nVerts; *curH = h;}\
	memcpy(curV,v,vC*sizeof(D3DTLVERTEX));\
	curV+=vC; nVerts+=vC; nFaces+=fC;\
}

#endif