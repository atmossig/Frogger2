#ifndef __OBE_H
#define __OBE_H



#define	OBECHUNK_OBJECT				1
#define	OBECHUNK_OBJECTINFO			2
#define	OBECHUNK_MESH				3
#define	OBECHUNK_MAPPINGICONS		4
#define	OBECHUNK_VIEWPORTS			5
#define	OBECHUNK_ANIMTIME			6
#define	OBECHUNK_VERTEXFRAMES		7
#define	OBECHUNK_ANIMSEGMENTS		8
#define	OBECHUNK_CONSTRUCTIONGRID	9
#define	OBECHUNK_LIGHTINGINFO		10
#define	OBECHUNK_HISTORYENTRY		11
#define	OBECHUNK_MASTERSORTLIST		12
#define	OBECHUNK_SKININFO			13
#define	OBECHUNK_SKIN				14
#define	OBECHUNK_END				255



typedef struct
{
	int		type;
	int		version;
	int		length;
//	char	*data;
}OBECHUNK_HEADER;






extern OBJECTINFO		*currObjInfo;
extern INPUT_OBJECT		*currObj;




BOOL LoadOBEObject(OBJECTINFO *obj);
void LoadOBEChunkObject(OBECHUNK_HEADER	*header, INPUT_OBJECT **object);
void LoadOBEChunkObjectInfo(OBECHUNK_HEADER	*header, OBJECTINFO *obj);
void LoadOBEChunkMesh(OBECHUNK_HEADER	*header, INPUT_OBJECT **object);
void LoadOBEChunkAnimSegments(OBECHUNK_HEADER	*header, OBJECTINFO *obj);
void LoadOBEChunkSkin(OBECHUNK_HEADER	*header, INPUT_OBJECT **object);

void CrossProduct (BYTEVECTOR *res, VECTOR *vr1, VECTOR *vr2, VECTOR *vr3);
void MakeUnit(VECTOR *vect);
void CalculateVertexNormals(MESH *mesh);
void RestoreObjectFlags(INPUT_OBJECT *object);

void FillInGouraudColours(MESH *mesh);
void ReStructureMeshFaces(INPUT_OBJECT *object);
void LoadOBEChunkAnimTime(OBECHUNK_HEADER *header, OBJECTINFO *obj);
void FreeOBE(OBJECTINFO *obj);
void LoadOBEKeyframes(INPUT_OBJECT *object);
BITMAP_TYPE *NeedToLoadOBEBitmap(char *filename);
void obeRead(void *buffer, int size);




#endif