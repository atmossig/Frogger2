/*

	This file is part of the M libraries,

	File		: 
	Programmer	: Matthew Cloy
	Date		: 

----------------------------------------------------------------------------------------------- */

#ifndef MDXRENDER_H_INCLUDED
#define MDXRENDER_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif
extern MDX_TEXENTRY *phong;
extern MDX_TEXENTRY *lightMap;
extern float globalXLU;
extern float globalXLU2;
extern MDX_MATRIX vMatrix;
extern unsigned long drawingSwampy;
extern unsigned long numObjDrawn;
extern float clx0, cly0, clx1, cly1;

extern float farClip, nearClip;

void InitOneOverTable(void);
void DrawObject(MDX_OBJECT *obj, int skinned, MDX_MESH *masterMesh);
void DrawLandscape(MDX_LANDSCAPE *me);

void PCRenderObject (MDX_OBJECT *obj);
void PCRenderModgyObject (MDX_OBJECT *obj);
void PCRenderObjectPhong (MDX_OBJECT *obj);

void PCPrepareModgyObject (MDX_OBJECT *obj, MDX_MESH *me, float m[4][4]);
void PCPrepareObject (MDX_OBJECT *obj, MDX_MESH *me, float m[4][4]);
void PCCalcModgeValues(MDX_OBJECT *obj);
void XfmPoint(MDX_VECTOR *vTemp2,MDX_VECTOR *in,MDX_MATRIX *d);
	
#ifdef __cplusplus
}
#endif

#endif