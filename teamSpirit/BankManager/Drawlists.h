#define MAXPOLYSPERTEXTURE 2000

#define uchar unsigned char

typedef struct{

	int faceNo;
	VECTOR verts[3];
	BYTEVECTOR faceNormal;
	BYTEVECTOR vertexNormal[3];
	SHORT2DVECTOR textureCoords[3];
	char flags;

} FaceInfo;

typedef struct{

	TEXTURE_ID *texID;
	int texture;
	int texture2;
	int numFaces;
	FaceInfo faceList[MAXPOLYSPERTEXTURE];

} TextureGroup;


typedef struct
{
	TEXTURE_ID	*texID;
	int			scx, scy;
	int			scalex, scaley;
	int			format;
	int			colDepth;
}TEXTUREDETAIL;

#define QUAD	(1<<0)
#define U_TILE	(1<<1)
#define V_TILE	(1<<2)
#define DRAW	(1<<3)
#define FILTER	(1<<4)
#define NO_COLL (1<<5)
//#define MIP_MAP (1<<6)
#define FACE_COL (1<<6)

extern int	numTotalObjects;
extern int texscalex,texscaley;
extern float	brightnessValue;

extern short scaleVal;
extern short scaleSkinnedObject;


void WriteHeaderFile(FILE *fp);		
void WriteHeader(FILE *fp);
void WriteVertices(FILE *fp);
void WriteDrawlists(FILE *fp);

void WriteObjectDefinitions(FILE *fp);
void WriteObjectKeyframes(FILE *fp);
void WriteObjectControllers(FILE *fp);
void WriteObjectMeshes(FILE *fp);
void PrintTextureInfo(FILE *fp, TextureGroup *polys);
//void PrintTextureInfo(FILE *fp, TEXTURE_ID *texID);
void WriteAnimationDef(FILE *fp);
void WriteAnimations(FILE *fp);
void WriteObjectSpriteDefs(FILE *fp);
void CreateTextureLists(TextureGroup *texList,INPUT_OBJECT *obj);
void FindPlace(int face,TEXTURE_ID *texID,INPUT_OBJECT *obj);
void PrintTextureList(FILE *fp);
void FillFace(FaceInfo *face,int faceNo,INPUT_OBJECT *obj);
int ComparePolys(FaceInfo *poly1,FaceInfo *poly2);
int ComparePolyWithCache(FaceInfo *poly1);
void AddCache(FaceInfo *face,int vert);
char AddPolyToCache(FaceInfo *poly1);
void CalcBestOrder(TextureGroup *polys,INPUT_OBJECT *obj);
void CopyFileIntoStream(FILE *dest,FILE *src);
void WriteObject(INPUT_OBJECT *obj);
char WritePolyInstruction(FaceInfo *poly1,int vertex);
char WritePolyInstructionQuad(FaceInfo *poly1,FaceInfo *poly2,int vertex,int vertex2);
char AddQuadToCache(FaceInfo *poly1,FaceInfo *poly2);
int CheckForMatchingVerts(FaceInfo *poly);
void InitBBox(void);
void PrintBBox(void);
void WriteObjectsSkinInfo();


int	SaveObjectBank();
void ExportDrawList(int bankNum);

void CalculateNumTotalObjects();
