//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <stdlib.h>

#pragma hdrstop

#include "Unit1.h"
#include "ObjBank.h"
#include "NewObjectBankModule.h"
#include "PropObjectBankModule.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#define MAX_FNAME_LEN 32

#define ADDATIVE        ( 1 << 0 ) //1
#define SUBTRACTIVE     ( 1 << 1 ) //2
#define HALFUV          ( 1 << 2 ) //4
#define RESET           ( 1 << 3 ) //8
#define USLIDDING       ( 1 << 4 ) //16
#define VSLIDDING       ( 1 << 5 ) //32
#define PLUSSLIDDING    ( 1 << 6 ) //64
#define MINUSSLIDDING   ( 1 << 7 ) //128
#define JIGGLE          ( 1 << 8 ) //256
#define MODGY           ( 1 << 9 ) //512
#define CENTERUV        ( 1 << 10 ) //1024
#define FLIPU           ( 1 << 11 ) //2048
#define FLIPV           ( 1 << 12 ) //4096

int mesh_version;
TForm1 *Form1;

int gamma = 256;
int shift = 0;
int halfuv = 0;
int reset = 0;
int center = 0;
int flipu = 0;
int flipv = 0;
int adda = 0;

unsigned long flags = 0;

char *obe_last_end;

void * obe_parse(char *addr);

int save_map_segment = 1;
#define FLOAT_LIMIT (0.6f)
#define FLOAT_ADD	(0.2f)

int fma_mesh_count = 0;

/*** GTE PACKET to-GPU command '<packet-name>.code' ***/
#define GPU_COM_F3    0x20
#define GPU_COM_TF3   0x24
#define GPU_COM_G3    0x30
#define GPU_COM_TG3   0x34

#define GPU_COM_F4    0x28
#define GPU_COM_TF4   0x2c
#define GPU_COM_G4    0x38
#define GPU_COM_TG4   0x3c

#define GPU_COM_NF3   0x21
#define GPU_COM_NTF3  0x25
#define GPU_COM_NG3   0x31
#define GPU_COM_NTG3  0x35

#define GPU_COM_NF4   0x29
#define GPU_COM_NTF4  0x2d
#define GPU_COM_NG4   0x39
#define GPU_COM_NTG4  0x3d

int add3 = 0;
int add4 = 0;

float additional_scale = 10.0f;
int adjust_coordinates = 1;
int flat_polygons=0;
int tri_polygons = 0;
int quad_polygons = 0;
int gouraud_polygons=0;

typedef struct
{
	float x,y,z,w;
}QuaternionType;

AnsiString configFileName;

typedef unsigned char u_char;
typedef unsigned short u_short;

typedef struct {
	short   vx, vy, vz;
	u_char  tu, tv;
}       VERT;


#define ADD2POINTER(a,b) ( (void *) ( (int)(a) + (b))  )
#define SUBPOINTERS(a,b) (((int)((int)(a) - (int)(b))))

#define COLOUR_MASK 0xffffff

typedef struct FMA_GT3
{
	u_char	r0, g0, b0, code;
	u_char	u0, v0;	u_short	clut;
	u_char	r1, g1, b1, p1;
	u_char	u1, v1;	u_short	tpage;
	u_char	r2, g2, b2, p2;
	u_char	u2, v2;	u_short	pad2;
	int vert0,vert1,vert2;
}FMA_GT3;
typedef struct FMA_GT4
{
	u_char	r0, g0, b0, code;
	u_char	u0, v0;	u_short	clut;
	u_char	r1, g1, b1, p1;
	u_char	u1, v1;	u_short	tpage;
	u_char	r2, g2, b2, p2;
	u_char	u2, v2;	u_short	pad2;
	u_char	r3, g3, b3, p3;
	u_char	u3, v3;	u_short	pad3;
	int vert0,vert1,vert2,vert3;
}FMA_GT4;

typedef struct FMA_SPR
{
	u_char	r0, g0, b0, code;
	u_char	u0, v0;	u_short	clut;
	u_char	r1, g1, b1, p1;
	u_char	u1, v1;	u_short	tpage;
	u_char	r2, g2, b2, p2;
	u_char	u2, v2;	u_short	pad2;
	u_char	r3, g3, b3, p3;
	u_char	u3, v3;	u_short	pad3;
	int vert0,vert1,vert2,vert3;
}FMA_SPR;

#define BFF_FMA_MESH_ID (('F'<<0) | ('M'<<8) | ('M'<<16) | (0<<24))
#define BFF_FMA_WORLD_ID (('F'<<0) | ('M'<<8) | ('W'<<16) | (0<<24))

typedef struct FMA_MESH_HEADER
{
	unsigned long id;	// start with a BFF header, with an eye to the future
	int length;
	unsigned long name_crc;
	unsigned long flags;
	unsigned long shift;

	int minx,miny,minz, maxx,maxy,maxz;

// For the MAP (as opposed to platforms), the position will always be zero, and the rot will be (0,0,0,4096)
	int posx,posy,posz;
	short rotx,roty,rotz,rotw;

	int extra_depth;

	int n_verts;
	VERT *verts;

	int n_gt3s;
	FMA_GT3 *gt3s;
	int n_gt4s;
	FMA_GT4 *gt4s;

//	int n_sprs;
//	FMA_SPR *sprs;

	int n_tmaps;
	unsigned long *tmap_crcs;

}FMA_MESH_HEADER;

typedef struct
{
	float matrix[3][3];
}MatrixType;

typedef struct FMA_MESH_ENTRY
{
	struct FMA_MESH_ENTRY *next;
	FMA_MESH_HEADER header;
	int scalex,scaley,scalez;	// 4096'd, but not stored in the actual mesh. That gets scaled
//	float frotx,froty,frotz,frotw;
	MatrixType rotmat;
}FMA_MESH_ENTRY;

FMA_MESH_ENTRY *fma_mesh_list = NULL;





typedef struct FMA_WORLD
{
	unsigned long id;	// start with a BFF header, with an eye to the future
	int length;
	unsigned long name_crc;

	int n_meshes;
// Followed immediately by an array of meshname CRC's to be replaced with mesh pointers
}FMA_WORLD;





//---------------------------------------------------------------------------

__fastcall TForm1::TForm1(TComponent* Owner)
  : TForm(Owner)
{
  LoadConfig();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
  Form2 = new TForm2 ( this );
  Form2->ShowModal();
  delete Form2;

  // JH: Ok now lets add the object bank name to the list.......
  ListBox1->Items->Add ( objBankList [ numObjectBanks - 1 ].bankName );

  SaveConfig();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender)
{
  Form3 = new TForm3 ( this );

  Form3->Edit1->Text = objBankList [ ListBox1->ItemIndex ].bankName;
  Form3->Edit2->Text = objBankList [ ListBox1->ItemIndex ].fileName;
  Form3->Edit3->Text = objBankList [ ListBox1->ItemIndex ].outputDir;

  Form3->ShowModal();
  delete Form3;

  SaveConfig();
  SaveBankFile ( ListBox1->ItemIndex );
  ListBox1->Clear();
  ListBox2->Clear();
  LoadConfig();

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)
{
  if ( OpenDialog2->Execute() )
  {
    int position = 128;
    Char temp[3];

    if ( OpenDialog2->Files->Count > 1 )
    {
     for ( register counter = 0; counter < OpenDialog2->Files->Count; counter++ )
      {
       OutputDebugString ( OpenDialog2->Files->Strings [ counter ].c_str() );
       ListBox2->Items->Add ( OpenDialog2->Files->Strings [ counter ] );

       sprintf ( objBankList [ ListBox1->ItemIndex ].objectList [ objBankList [ ListBox1->ItemIndex ].numObjects ].fileName, OpenDialog2->Files->Strings [ counter ].c_str()  );

       itoa ( position, temp, 10 );
       Label2->Caption = temp;

       gamma = position;

       objBankList [ ListBox1->ItemIndex ].objectList [ objBankList [ ListBox1->ItemIndex ].numObjects ].remove = 0;
       objBankList [ ListBox1->ItemIndex ].objectList [ objBankList [ ListBox1->ItemIndex ].numObjects++ ].gamma = gamma;

      }
      // ENDFOR
    }
    else
    {
      ListBox2->Items->Add ( OpenDialog2->FileName );

      sprintf ( objBankList [ ListBox1->ItemIndex ].objectList [ objBankList [ ListBox1->ItemIndex ].numObjects ].fileName, OpenDialog2->FileName.c_str()  );

      itoa ( position, temp, 10 );
      Label2->Caption = temp;

      gamma = position;

      objBankList [ ListBox1->ItemIndex ].objectList [ objBankList [ ListBox1->ItemIndex ].numObjects++ ].gamma = gamma;
    }
    // ENDELSEIF
    SaveBankFile ( ListBox1->ItemIndex );
    SaveConfig();
  }
  // ENDIF
}
//---------------------------------------------------------------------------

#define POLYNOMIAL 0x04c11db7L
unsigned long CRCtable[256];

void initCRC()
{
	register int i, j;
	register unsigned long CRCaccum;

	for (i=0; i<256; i++)
		{
		CRCaccum = ((unsigned long)i<<24);
		for (j=0; j<8; j++)
			{
			if (CRCaccum & 0x80000000L)
				CRCaccum = (CRCaccum<<1)^POLYNOMIAL;
			else
				CRCaccum = (CRCaccum<<1);
			}
		CRCtable[i] = CRCaccum;
		}
}


void *OpenMallocReadClose(char *filename)
{
	FILE *file;
	int flen;
	unsigned char *addr;

//	if(verbose)
//		printf("loading file %s\n",filename);

	file = fopen(filename,"rb");
	if(!file)
	{
		printf("File %s not found\n",filename);
		return NULL;
	}

	fseek(file,0,SEEK_END);
	flen = ftell(file);
	fseek(file,0,SEEK_SET);

//	if(verbose)
//		printf("file len = %d\n",flen);
	addr = (unsigned char*)malloc(flen);
	if(!addr)
	{
		fclose(file);
		return NULL;
	}
	fread(addr,flen,1,file);
	fclose(file);
	return addr;
}

void *zmalloc(int size)
{
	void *rval;
	rval = malloc(size);
	if(rval && size)
	{
		memset(rval,0,size);
	}
	return rval;
}

void ExtractLeafname(char *filename, char *leafname)
{
	int i;
	int len;
	int d;

	len = strlen(filename);
	for(i = len; i >=0; i--)
	{
		if(filename[i] == '\\')
		{
			i++;
			break;
		}
	}
	if(i <= 0)
		i = 0;

	for(d = 0; (filename[i] != '.') && (filename[i] != 0) && (d < MAX_FNAME_LEN-1); d++,i++)
	{
		leafname[d] = filename[i];
	}
	leafname[d] = 0;
	strupr(leafname);
}

unsigned long str2CRC(char *ptr)
{
	register int i, j;
	int size = strlen(ptr);
	unsigned long CRCaccum = 0;

	for (j=0; j<size; j++)
		{
		i = ((int)(CRCaccum>>24)^(*ptr++))&0xff;
		CRCaccum = (CRCaccum<<8)^CRCtable[i];
		}
	return CRCaccum;
}

FMA_MESH_ENTRY * fma_start_mesh(char *name)
{
	FMA_MESH_ENTRY *mesh;
	char leafname[MAX_FNAME_LEN];


	mesh = (FMA_MESH_ENTRY*)zmalloc(sizeof(FMA_MESH_ENTRY));

	mesh->header.id = BFF_FMA_MESH_ID;
	mesh->header.minx = 1000;
	mesh->header.miny = 1000;
	mesh->header.minz = 1000;
	mesh->header.maxx = -1000;
	mesh->header.maxy = -1000;
	mesh->header.maxz = -1000;

	mesh->header.extra_depth = 10;	// this needs to be higher for underwater meshes

// convert tmap name to upper case, ditch the extension, and crc the thing
	ExtractLeafname(name,&leafname[0]);
	mesh->header.name_crc = str2CRC(&leafname[0]);

	return mesh;
}

int obe_load_float_animvector(char **addr)
{
	int len;
	float val;
	len = *(int *)(*addr);
	val = *(float *)( (*addr) + 8);

#ifdef PRE_ADD_SCALE
	if(additional_scale != 1.0f)
	{
		val *= additional_scale;
	}
#endif

// Prevent duff rounding errors
	if (val>FLOAT_LIMIT) val+=FLOAT_ADD;
	else if (val<-FLOAT_LIMIT) val-=FLOAT_ADD;

	*addr += 4 + len * 8;	// time, key

	return (int)val;
}

void fma_set_mesh_pos(FMA_MESH_ENTRY *mesh, int x, int y, int z)
{
//bb
	x = -x;
	y = -y;
	z = -z;

	if(!adjust_coordinates)
	{
		if(additional_scale != 1.0f)
		{
			x = (int)((float)(x) * additional_scale);
			y = (int)((float)(y) * additional_scale);
			z = (int)((float)(z) * additional_scale);
		}
	}

// If adjust_coordinates *is* set, then the points have the pos added before scaling, so the pos shouldn't be scaled

	mesh->header.posx = x;
	mesh->header.posy = y;
	mesh->header.posz = z;

}

int obe_load_fixed_animvector(char **addr)
{
	int len;
	float val;

//	printf(" anim %d %f\n",*(int *)( (*addr) + 8),*(float *)( (*addr) + 8));

	len = *(int *)(*addr);
	val = *(float *)( (*addr) + 8);

// Note - we don't round Fixed's, coz we want a 0...1 number

	*addr += 4 + len * 8;	// time, key

	return (int)(val * 4096);
}


void fma_set_mesh_scale(FMA_MESH_ENTRY *mesh, int x, int y, int z)
{
	//bb
	mesh->scalex = -x;
	mesh->scaley = y;
	mesh->scalez = -z;
}

float obe_load_keepfloat_animvector(char **addr)
{
	int len;
	float val;
	len = *(int *)(*addr);
	val = *(float *)( (*addr) + 8);

	*addr += 4 + len * 8;	// time, key

	return val;
}




void QuaternionToMatrix(QuaternionType *squat, MatrixType *dmatrix)
{
	float	s, xs,ys,zs, wx,wy,wz, xx,xy,xz, yy,yz,zz;

	s = ((float)2.0)/(squat->x*squat->x + squat->y*squat->y + squat->z*squat->z + squat->w*squat->w);

	xs = squat->x*s;
	ys = squat->y*s;
	zs = squat->z*s;

	wx = squat->w*xs;
	wy = squat->w*ys;
	wz = squat->w*zs;

	xx = squat->x*xs;
	xy = squat->x*ys;
	xz = squat->x*zs;

	yy = squat->y*ys;
	yz = squat->y*zs;
	zz = squat->z*zs;

	dmatrix->matrix[0][0] = ((float)1.0)-(yy+zz);
	dmatrix->matrix[0][1] = xy+wz;
	dmatrix->matrix[0][2] = xz-wy;

	dmatrix->matrix[1][0] = xy-wz;
	dmatrix->matrix[1][1] = ((float)1.0)-(xx+zz);
	dmatrix->matrix[1][2] = yz+wx;

	dmatrix->matrix[2][0] = xz+wy;
	dmatrix->matrix[2][1] = yz-wx;
	dmatrix->matrix[2][2] = ((float)1.0)-(xx+yy);

// I'm working with rotation matrices only.
/*
	dmatrix->matrix[0][3] = (float)0.0;
	dmatrix->matrix[1][3] = (float)0.0;
	dmatrix->matrix[2][3] = (float)0.0;
	dmatrix->matrix[3][3] = (float)1.0;
	dmatrix->matrix[3][0] = (float)0.0;
	dmatrix->matrix[3][1] = (float)0.0;
	dmatrix->matrix[3][2] = (float)0.0;
*/
}

void fma_set_mesh_rot(FMA_MESH_ENTRY *mesh, float x, float y, float z, float w)
{
	QuaternionType q;
	q.x = x;
	q.y = y;
	q.z = z;
	q.w = w;
	QuaternionToMatrix(&q,&mesh->rotmat);

	mesh->header.rotx = (int)(x * 4096.0f);
	mesh->header.roty = (int)(y * 4096.0f);
	mesh->header.rotz = (int)(z * 4096.0f);
	mesh->header.rotw = (int)(w * 4096.0f);
}

int fma_add_vert(FMA_MESH_ENTRY *mesh, int x, int y, int z)
{
	int vn;
	VERT *v;

	y = -y;	// PC - to - PSX coordinate conversion

	x = x * mesh->scalex / 4096;
	y = y * mesh->scaley / 4096;
	z = z * mesh->scalez / 4096;

	vn = mesh->header.n_verts;
	mesh->header.n_verts++;

//	printf("vert %d\n",vn);
	mesh->header.verts = (VERT*)realloc(mesh->header.verts, (vn+1) * sizeof(VERT));
//	printf("realloc ok\n");
	v = &mesh->header.verts[vn];

	if(adjust_coordinates)
	{
		float fx,fy,fz;
		MatrixType *mat;
		float fx2,fy2,fz2;

//		printf("adj\n");
		fx = (float)x;
		fy = (float)y;
		fz = (float)z;
		mat = &mesh->rotmat;
		fx2 = fx * mat->matrix[0][0] + fy * mat->matrix[0][1] + fz * mat->matrix[0][2];
		fy2 = fx * mat->matrix[1][0] + fy * mat->matrix[1][1] + fz * mat->matrix[1][2];
		fz2 = fx * mat->matrix[2][0] + fy * mat->matrix[2][1] + fz * mat->matrix[2][2];
		x = (int) fx2;
		y = (int) fy2;
		z = (int) fz2;

//		printf("write\n");

// Rotate verts by the FP version of the rotate quaternion

// Add the mesh's origin
// TBD - deal with parents & children? Or just keep right on ignoring them
		v->vx = x + mesh->header.posx;
		v->vy = y + mesh->header.posy;
		v->vz = z + mesh->header.posz;
	}
	else
	{
		v->vx = x;
		v->vy = y;
		v->vz = z;
	}

#ifndef PRE_ADD_SCALE
	if(additional_scale != 1.0f)
	{
		v->vx = (int)((float)(v->vx) * additional_scale);
		v->vy = (int)((float)(v->vy) * additional_scale);
		v->vz = (int)((float)(v->vz) * additional_scale);
	}
#endif
//	printf("adj ok\n");

	v->tu = 0;
	v->tv = 0;

	if(v->vx < mesh->header.minx) mesh->header.minx = v->vx;
	if(v->vy < mesh->header.miny) mesh->header.miny = v->vy;
	if(v->vz < mesh->header.minz) mesh->header.minz = v->vz;

	if(v->vx > mesh->header.maxx) mesh->header.maxx = v->vx;
	if(v->vy > mesh->header.maxy) mesh->header.maxy = v->vy;
	if(v->vz > mesh->header.maxz) mesh->header.maxz = v->vz;

	return vn;
}

int fma_add_texture(FMA_MESH_ENTRY *mesh, char *tmap_name)
{
	unsigned long crc;
	char leafname[MAX_FNAME_LEN];
	int i;

// convert tmap name to upper case, ditch the extension, and crc the thing
	ExtractLeafname(tmap_name,&leafname[0]);
//	ExtractLeafnamePlusSuffix(tmap_name,&leafname[0]);

	crc = str2CRC(&leafname[0]);

	for(i = 0; i < mesh->header.n_tmaps; i++)
	{
		if(mesh->header.tmap_crcs[i] == crc)
		{
			return i;
		}
	}

//	if(verbose)
		printf("  Texture %2d, crc = &%8x, = %s\n",i,crc,leafname);

	mesh->header.n_tmaps++;

	mesh->header.tmap_crcs = (unsigned long*)realloc(mesh->header.tmap_crcs,mesh->header.n_tmaps * sizeof(unsigned long));
	mesh->header.tmap_crcs[i] = crc;
	return i;
}

char * obe_skip_animvector(char *addr)
{
    int noOfKeys;
	int len;
    
	noOfKeys = *(int *)(addr);

//	printf("   animvector len %d\n",len);
	addr += 4 + len * 8;	// time, key

	return addr;
}

float obe_load_float_directly(void *addr)
{
	float val;

	val = *(float *)(addr);

	return (float) val;
}

void fma_add_gt3(FMA_MESH_ENTRY *mesh, int tmap, int *vnos, int *tus, int *tvs, unsigned int *bgrs, int transMode )
{
	FMA_GT3 *ptr;
	mesh->header.n_gt3s++;
	mesh->header.gt3s = (FMA_GT3*)realloc(mesh->header.gt3s, mesh->header.n_gt3s * sizeof(FMA_GT3));

	if(    (bgrs[0] & COLOUR_MASK) == (bgrs[1] & COLOUR_MASK)
		&& (bgrs[0] & COLOUR_MASK) == (bgrs[2] & COLOUR_MASK)
		)
	{
		flat_polygons++;
	}
	else
	{
		gouraud_polygons++;
	}
	tri_polygons++;

	ptr = &mesh->header.gt3s[mesh->header.n_gt3s-1];

	ptr->r0 =  bgrs[0] & 0xff;
	ptr->g0 = (bgrs[0]>>8) & 0xff;
	ptr->b0 = (bgrs[0]>>16) & 0xff;

	ptr->r1 =  bgrs[1] & 0xff;
	ptr->g1 = (bgrs[1]>>8) & 0xff;
	ptr->b1 = (bgrs[1]>>16) & 0xff;

	ptr->r2 =  bgrs[2] & 0xff;
	ptr->g2 = (bgrs[2]>>8) & 0xff;
	ptr->b2 = (bgrs[2]>>16) & 0xff;

	ptr->u0 = tus[0];
	ptr->v0 = tvs[0];

	ptr->u1 = tus[1];
	ptr->v1 = tvs[1];

	ptr->u2 = tus[2];
	ptr->v2 = tvs[2];

	ptr->vert0 = vnos[0] * 4;	// optimisation for the PSX map-plotting code
	ptr->vert1 = vnos[1] * 4;
	ptr->vert2 = vnos[2] * 4;

	ptr->p1 = 0;
	ptr->p2 = 0;
   	ptr->code = GPU_COM_TG3;
  	ptr->pad2 =0;

    if ( transMode )
    {
    	ptr->pad2 |= 2;

//    	ptr->code |= 2;
    }

   	ptr->tpage = tmap;	// gets re-numbered post-load

    if ( transMode == 32 )
    {
    	ptr->pad2 |= transMode;

//        ptr->tpage |= transMode;
    }

    if ( transMode == 64 )
    {
    	ptr->pad2 |= transMode;

//        ptr->tpage |= transMode;
    }

	ptr->clut  = 0;	// gets re-numbered post-load
}

// Note that the vertex order on quads is different on the PC to the PSX.
// Jobe Quad           PSX Quad
//  0   1               0   1
//
//  3   2               2   3
// (Ie, in the quad code, vertices 2 & 3 get swapped over)

// Hurrah, let's add a polygon to our mesh!
void fma_add_gt4(FMA_MESH_ENTRY *mesh, int tmap, int *vnos, int *tus, int *tvs, unsigned int *bgrs, int transMode )
{
	FMA_GT4 *ptr;
	mesh->header.n_gt4s++;
	mesh->header.gt4s = (FMA_GT4*)realloc(mesh->header.gt4s, mesh->header.n_gt4s * sizeof(FMA_GT4));

	ptr = &mesh->header.gt4s[mesh->header.n_gt4s-1];

	if(    (bgrs[0] & COLOUR_MASK) == (bgrs[1] & COLOUR_MASK)
		&& (bgrs[0] & COLOUR_MASK) == (bgrs[2] & COLOUR_MASK)
		&& (bgrs[0] & COLOUR_MASK) == (bgrs[3] & COLOUR_MASK))
	{
		flat_polygons++;
	}
	else
	{
		gouraud_polygons++;
	}
	quad_polygons++;


	ptr->r0 =  bgrs[0] & 0xff;
	ptr->g0 = (bgrs[0]>>8) & 0xff;
	ptr->b0 = (bgrs[0]>>16) & 0xff;

	ptr->r1 =  bgrs[1] & 0xff;
	ptr->g1 = (bgrs[1]>>8) & 0xff;
	ptr->b1 = (bgrs[1]>>16) & 0xff;

	ptr->r3 =  bgrs[2] & 0xff;
	ptr->g3 = (bgrs[2]>>8) & 0xff;
	ptr->b3 = (bgrs[2]>>16) & 0xff;

	ptr->r2 =  bgrs[3] & 0xff;
	ptr->g2 = (bgrs[3]>>8) & 0xff;
	ptr->b2 = (bgrs[3]>>16) & 0xff;

	ptr->u0 = tus[0];
	ptr->v0 = tvs[0];

	ptr->u1 = tus[1];
	ptr->v1 = tvs[1];

	ptr->u3 = tus[2];
	ptr->v3 = tvs[2];

	ptr->u2 = tus[3];
	ptr->v2 = tvs[3];

	ptr->vert0 = vnos[0] * 4;	// multiply the vertex numbers by 4.
	ptr->vert1 = vnos[1] * 4;	// (Allows a handy little optimisation for the PSX end of things)
	ptr->vert3 = vnos[2] * 4;
	ptr->vert2 = vnos[3] * 4;

	ptr->p1 = 0;
	ptr->p2 = 0;
	ptr->p3 = 0;
	ptr->pad2 = 0;
	ptr->pad3 = 0;

	ptr->code = GPU_COM_TG4;
  	ptr->pad2 =0;
    if ( transMode )
	    ptr->pad2 |= 2;

	ptr->tpage = tmap;	// gets re-numbered post-load

    if ( transMode == 32 )
    {
    	ptr->pad2 |= transMode;

//        ptr->tpage |= transMode;
    }

    if ( transMode == 64 )
    {
    	ptr->pad2 |= transMode;

//        ptr->tpage |= transMode;
    }

	ptr->clut  = 0;	// gets re-numbered post-load
}

void fma_link_mesh(FMA_MESH_ENTRY *mesh)
{
	int link = TRUE;

	if(!mesh->header.n_verts)
		link = FALSE;
	if(! (mesh->header.n_gt3s +mesh->header.n_gt4s))
		link = FALSE;

	if(link)
	{
		fma_mesh_count++;
		mesh->next = fma_mesh_list;
		fma_mesh_list = mesh;
	}
	else
	{
		printf("empty mesh discarded\n");
		if(mesh->header.verts)
			free(mesh->header.verts);
		if(mesh->header.gt3s)
			free(mesh->header.gt3s);
		if(mesh->header.gt4s)
			free(mesh->header.gt4s);
		if(mesh->header.tmap_crcs)
			free(mesh->header.tmap_crcs);
		free(mesh);
	}
}

void obe_parsemesh(char *addr, int len)
{
	int n_verts;
	int n_polys;
	int i;
	char *end_addr;

	FMA_MESH_ENTRY * fma_mesh;

	end_addr = addr + len;

	fma_mesh = fma_start_mesh(addr);

//	if(verbose)
 //		printf("Mesh <%s> - len = &%8x\n",addr,len);
	addr += 32;

	{
		int x,y,z;
		x = obe_load_float_animvector(&addr);
		y = obe_load_float_animvector(&addr);
		z = obe_load_float_animvector(&addr);
//		if(verbose)
 //			printf(" pos = %d %d %d\n",x,y,z);
		fma_set_mesh_pos(fma_mesh, x,y,z);

	}

	{
		int x,y,z;
		x = obe_load_fixed_animvector(&addr);
		y = obe_load_fixed_animvector(&addr);
		z = obe_load_fixed_animvector(&addr);
//		if(verbose)
 //			printf(" scale = %d %d %d\n",x,y,z);
		fma_set_mesh_scale(fma_mesh,x,y,z);
	}

	{
		float fx,fy,fz,fw;

		fx = obe_load_keepfloat_animvector(&addr);
		fy = obe_load_keepfloat_animvector(&addr);
		fz = obe_load_keepfloat_animvector(&addr);
		fw = obe_load_keepfloat_animvector(&addr);
//		if(verbose)
 //			printf(" orientation = %2.2f %2.2f %2.2f %2.2f\n",fx,fy,fz,fw);
		fma_set_mesh_rot(fma_mesh,fx,fy,fz,fw);
// Actually, if the orientation of an lscape segment isn't (0,0,0,0) then we're knacked.
	}

	n_verts = *(int *)addr;
	addr += 4;
//	if(verbose)
 //		printf(" verts:- %d\n",n_verts);

	for(i = 0; i < n_verts; i++)
	{
		int x,y,z;
		x = obe_load_float_animvector(&addr);
		y = obe_load_float_animvector(&addr);
		z = obe_load_float_animvector(&addr);
		addr += 1;						// char of "selected/not selected

		fma_add_vert(fma_mesh, x,y,z);	// For now, I'll just add all the verts, and maintain the obe's shape allocation

	}

	n_polys = *(int *)addr;
	addr += 4;
//	if(verbose)
 //		printf(" polys:- %d\n",n_polys);






	for(i = 0; i < n_polys; i++)
	{
		int t, transMode;
		int poly_flags;
		int poly_verts;
		int tmap_no;

// Sort out as soon as we've got some polygons to work with...
		printf("poly tmap %s\n",addr);
//		{ char c = getc(stdin);}

		tmap_no = fma_add_texture(fma_mesh, addr);

		addr += 32;

		t = *(unsigned char *)addr;
		printf("  poly r = %d\n",t);
		addr += 1;
		t = *(unsigned char *)addr;
		printf("  poly g = %d\n",t);
		addr += 1;
		t = *(unsigned char *)addr;
		printf("  poly b = %d\n",t);
 	addr += 1;

		t = *(unsigned char *)addr;
		printf("  poly trans = %d\n",t);
		addr += 1;

		if(mesh_version >= 5)
		{
			transMode = *(unsigned char *)addr;	// translucency mode

//#define TRANSLUCENCYMODE_ADDITIVE		2
//#define TRANSLUCENCYMODE_SUBTRACTIVE		3

            if ( transMode == 2 )
            {
                transMode = 32;
            }
            // ENDIF
            if ( transMode == 3 )
            {
                transMode = 64;
            }
            // ENDIF

            if ( adda )
                transMode = 32;


			addr += 1;

			t = *(unsigned char *)addr;	// shading mode
			addr += 1;
		}

		t = *(int *)addr;	// u
		printf("u = %d\n",t);
		addr += 4;
		t = *(int *)addr;	// v
		printf("v = %d\n",t);
		addr += 4;

		t = *(char *)addr;	// selected
		addr += 1;

		poly_flags = *(unsigned short *)addr;	// flags
		printf("flags = &%8x\n",poly_flags);
		addr += 2;


		poly_verts = *(int *)addr;
		addr += 4;
		printf("  poly verts %d\n",poly_verts);

		if(poly_flags & 0x10)	// If it's a SPRITE
		{
			printf("  Sprite\n");
			addr = obe_skip_animvector(addr);	// x
			addr = obe_skip_animvector(addr);	// y
			addr = obe_skip_animvector(addr);	// z
			addr = obe_skip_animvector(addr);	// xsize
			addr = obe_skip_animvector(addr);	// ysize
			addr += 1;	// xflip
			addr += 1;	// yflip

		}
		else
		{
			int v;
			int vnos[4];
			int tu[4];
			int tv[4];
			unsigned int bgr[4];

			for (v = 0; v < poly_verts; v++)
			{
				if(v < 4)
					vnos[v] = *(short *)addr;	// index into vert list
				addr += 2;

				t = *(unsigned char *)addr;	// lighting intensity
				addr += 1;

				if(v < 4)
				{
					tu[v] = (int)(obe_load_float_directly(addr) * 127.0f);	// tu

          if ( center )
          {
            if ( v == 0 )
              tu[v] = 31;
            if ( v == 3 )
              tu[v] = 94;
            if ( v == 1 )
              tu[v] = 31;
            if ( v == 2 )
              tu[v] = 94;
          }
          // ENDIF

          if ( reset )
          {
            if ( v == 0 )
              tu[v] = 0;
            if ( v == 3 )
              tu[v] = 127;
            if ( v == 1 )
              tu[v] = 0;
            if ( v == 2 )
              tu[v] = 127;
          }
          // ENDIF

          if ( halfuv )
          {
            tu[v] /= 2;
            tu[v] += 1;
          }

          if ( flipu )
          {
            tu[v] += 63;
          }
          // ENDIF

					if(tu[v] < 0 || tu[v] > 127)
					{
						printf("TMAP OVERFLOW %d (from %f)\n",tu[v],*(float *)addr);
					}
					printf("tu = %f / %d\n",*(float *)addr, *(int *)addr);
				}
				addr += 4;

				if(v < 4)
				{
					tv[v] = (int)(obe_load_float_directly(addr) * 127.0f);	// tv

          if ( center )
          {
            if ( v == 0 )
              tv[v] = 31;
            if ( v == 3 )
              tv[v] = 31;
            if ( v == 1 )
              tv[v] = 94;
            if ( v == 2 )
              tv[v] = 94;
          }
          // ENDIF

          if ( reset )
          {
            if ( v == 0 )
              tv[v] = 0;
            if ( v == 3 )
              tv[v] = 0;
            if ( v == 1 )
              tv[v] = 127;
            if ( v == 2 )
              tv[v] = 127;
          }
          // ENDIF

        if ( halfuv )
        {
          tv[v] /= 2;
          tv[v] += 1;
        }

          if ( flipv )
          {
            tv[v] += 63;
          }
          // ENDIF

					printf("tv = %f / %d\n",*(float *)addr, *(int *)addr);
					if(tv[v] < 0 || tv[v] > 127)
					{
						printf("TMAP OVERFLOW %d (from %f)\n",tv[v],*(float *)addr);
					}

				}
				addr += 4;

				if(mesh_version >= 5)
				{
					if(v < 4)
					{
						unsigned int r,g,b;
						r = *(unsigned char *)(addr+0);
						g = *(unsigned char *)(addr+1);
						b = *(unsigned char *)(addr+2);

						r = (r * gamma) >>8;
						g = (g * gamma) >>8;
						b = (b * gamma) >>8;

						bgr[v] = (b<<16) | (g<<8) | (r);	// psx-way-round
					}
					addr += 3;		// gouraud rgb
				}
				else
				{
					if(v<4)
					{
						bgr[v] = 0xffffff;
					}
				}
			}

			switch(poly_verts)
			{
			case 3:
				add3++;
				printf("Adding 3 poly : %d", add3);
				fma_add_gt3(fma_mesh,tmap_no, &vnos[0],&tu[0],&tv[0],&bgr[0], transMode);
				break;
			case 4:
				add4++;
				printf("Adding 4 poly : %d", add4);
				fma_add_gt4(fma_mesh,tmap_no, &vnos[0],&tu[0],&tv[0],&bgr[0], transMode );
				break;
			}

				if(poly_flags & 4)	// double sided....
				{
					int temp;

// reverse the poly, & maintain vertex 1 as the right angle
					temp = vnos[0];
					vnos[0] = vnos[2];
					vnos[2] = temp;

					temp = tu[0];
					tu[0] = tu[2];
					tu[2] = temp;

					temp = tv[0];
					tv[0] = tv[2];
					tv[2] = temp;

					temp = bgr[0];
					bgr[0] = bgr[2];
					bgr[2] = temp;

					switch(poly_verts)
					{
					case 3:

						fma_add_gt3(fma_mesh,tmap_no, &vnos[0],&tu[0],&tv[0],&bgr[0], transMode );
						break;
					case 4:
						fma_add_gt4(fma_mesh,tmap_no, &vnos[0],&tu[0],&tv[0],&bgr[0], transMode );
						break;
					}

				}

		}
	}






	i = *(short *)(addr);		// mesh flags
	addr += 2;

	i = *(unsigned char *)(addr);		// r
//	printf(" r = %d\n",i);
	addr += 1;
	i = *(unsigned char *)(addr);		// g
//	printf(" g = %d\n",i);
	addr += 1;
	i = *(unsigned char *)(addr);		// b
//	printf(" b = %d\n",i);
	addr += 1;

	if(mesh_version >= 5)
	{
		i = *(unsigned char *)(addr);		// gouraud mode
		addr += 1;
		i = *(unsigned char *)(addr);		// transp val
		addr += 1;
		i = *(unsigned char *)(addr);		// transp mode
		addr += 1;
	}

	if(mesh_version >= 2)
	{
		i = *(unsigned char *)(addr);		// selected flag
		addr += 1;
	}

	if(mesh_version >= 6)
	{
// sortlists
		for(i = 0; i < 8; i++)
		{
			int ne;
			ne = *(int *)addr;
//			printf("sortlist len %d\n",ne);
			addr += 4 + ne * 4;
		}
	}



// If the mesh had any polygons, link it into the list to be saved
	fma_link_mesh(fma_mesh);



// Child Objects....
	{
		char *addr2 = addr;
//		if(verbose)
 //			printf("Child Object Chunks...\n");

		while(addr2)
		{
			addr2 = (char*)obe_parse(addr2);
		}

// okay. Now there's an interval between addr and the end, which is genesis data, ndo flags

		addr = obe_last_end;
	}


//	printf(" &%8x bytes undealt with\n",end_addr - addr);


	if(mesh_version >= 11)
	{
		end_addr -=32;
//		printf("phong tmap <%s>\n",end_addr);
	}
	if(mesh_version >= 10)
	{
		for(i = 0; i < n_polys; i++)
		{
			end_addr -= 36;
//			printf("overlay tmap <%s>\n",end_addr);	// note - the overlay tmap names are actually initiialised data, throughout
		}
	}
	if(mesh_version >= 9)
	{
		for(i = 0; i < n_polys; i++)
		{
			end_addr -= 4;	// NDO flags
		}
	}
	if(mesh_version >= 8)
	{
		for(i = 0; i < n_polys; i++)
		{
			end_addr -= 4;	// Terrain Type
		}
	}


//	printf(" %d bytes of Genesis data\n",end_addr - addr);

}


void * obe_parse(char *addr)
{
	long *laddr;
	long id;
	long version;
	long len;

	laddr = (long *)addr;
	id = laddr[0];
	version = laddr[1];
	len = laddr[2];

//	printf("id = %d\n",id);
	switch(id)
	{
	case OBECHUNK_OBJECT:
	{
		char *addr2 = addr + 12;
//		if(verbose)
 //			printf("Object Chunk    ver = %d, len = &%8x (Scanning Sub Objects)\n");

		while(addr2)
		{
			addr2 = (char*)obe_parse(addr2);
		}
		break;
	}
	case OBECHUNK_OBJECTINFO:
 //		if(verbose)
  //			printf("Object Info Chunk   ver = %d, len = &%8x\n",version,len);
		break;


	case OBECHUNK_MESH:
	{
		char *addr2 = addr + 12;
  //		if(verbose)
   //			printf("Mesh Chunk   ver = %d, Offset = &%8x, len = &%8x.\n",version, SUBPOINTERS(addr,base_addr),len);
		mesh_version = version;

// (Version 12 is Andy (S)'s "no-changes" revision of version 11)
		if(mesh_version > 12)
		{
			printf(" WARNING!!!! Mesh version = %d\n",mesh_version);
			printf("The Frogger map converter doesn't understand OBE mesh data above version 12.\n");
			printf("Time for someone to update the frog_map source\n");
		}

		obe_parsemesh(addr2,len);
		break;
	}



	case OBECHUNK_MAPPINGICONS:
//		if(verbose)
 //			printf("MappingIcons Chunk   ver = %d, len = &%8x\n",version,len);
		break;
	case OBECHUNK_VIEWPORTS:
  //		if(verbose)
   //			printf("Viewports Chunk   ver = %d, len = &%8x\n",version,len);
		break;
	case OBECHUNK_ANIMTIME:
 //		if(verbose)
  //			printf("Animtime Chunk   ver = %d, len = &%8x\n",version,len);
		break;
	case OBECHUNK_VERTEXFRAMES:
  //		if(verbose)
   //			printf("Vertex frames Chunk   ver = %d, len = &%8x\n",version,len);
		break;
	case OBECHUNK_ANIMSEGMENTS:
   //		if(verbose)
	//		printf("Anim Segments Chunk   ver = %d, len = &%8x\n",version,len);
		break;
	case OBECHUNK_CONSTRUCTIONGRID:
  //		if(verbose)
   //			printf("Construction Grid Chunk   ver = %d, len = &%8x\n",version,len);
		break;
	case OBECHUNK_LIGHTINGINFO:
//		if(verbose)
 //			printf("Lighting info Chunk   ver = %d, len = &%8x\n",version,len);
		break;
	case OBECHUNK_HISTORYENTRY:
 //		if(verbose)
  //			printf("History Entry Chunk   ver = %d, len = &%8x\n",version,len);
		break;
	case OBECHUNK_MASTERSORTLIST:
  //		if(verbose)
   //			printf("Master Sortlist Chunk   ver = %d, len = &%8x\n",version,len);
		break;
	case OBECHUNK_SKININFO:
   //		if(verbose)
   //			printf("Skin Info Chunk   ver = %d, len = &%8x\n",version,len);
		break;
	case OBECHUNK_SKIN:
   //		if(verbose)
   //			printf("Skin Chunk   ver = %d, len = &%8x\n",version,len);
		break;
	case OBECHUNK_END:
   //		if(verbose)
	//		printf("End Chunk   ver = %d, len = &%8x\n",version,len);
		obe_last_end = (char*)ADD2POINTER(addr,len+12);
//	printf("end\n");
		return NULL;
	default:
//		if(verbose)
 //			printf("Unknown Chunk   ver = %d, len = &%8x\n",version,len);
		break;
	}
//	printf("done\n");

	return ADD2POINTER(addr,len+12);
}

int Appendfile(void *addr, int len, char filename[256])
{
	FILE *file;

//	if(verbose)
 //		printf("appending to file %s, len %d\n",filename,len);

	file = fopen(filename,"ab");
	if(!file)
	{
//		printf("File appending problemo %s\n",filename);
		return 0;
	}

	fwrite(addr,len,1,file);
	fclose(file);
	return 1;

}

int Savefile(void *addr, int len, char filename[256])
{
	FILE *file;

//	if(verbose)
 //		printf("saving file %s, len %d\n",filename,len);

	file = fopen(filename,"wb");
	if(!file)
	{
		printf("File writing problem %s\n",filename);
		return 0;
	}

	fwrite(addr,len,1,file);
	fclose(file);
	return 1;
}

void fma_save(FMA_MESH_ENTRY *mesh, char fname[256],int append)
{
	void * temp[4];
	int vert_size;

	if(adjust_coordinates)
	{
		mesh->header.posx = 0;
		mesh->header.posy = 0;
		mesh->header.posz = 0;

		mesh->header.rotx = 0;
		mesh->header.roty = 0;
		mesh->header.rotz = 0;
		mesh->header.rotw = 4096;

	}

    mesh->header.flags = flags;
    mesh->header.shift = shift;

// Store the pointers that are part of the header structure.
// convert 'em to offsets, based on the size of the header, and the size of the things that come first.
// write the header to the output file,..
// restore the pointers so we can use them again

	vert_size = mesh->header.n_verts * sizeof(VERT);
	temp[0] = mesh->header.tmap_crcs;
	temp[1] = mesh->header.verts;
	temp[2] = mesh->header.gt3s;
	temp[3] = mesh->header.gt4s;

//	fma_fill_in_size(mesh);
	mesh->header.tmap_crcs = (unsigned long*)sizeof(FMA_MESH_HEADER);
	mesh->header.verts = (VERT*)ADD2POINTER(mesh->header.tmap_crcs, mesh->header.n_tmaps * sizeof(unsigned long));
	mesh->header.gt3s  = (FMA_GT3*)ADD2POINTER(mesh->header.verts, vert_size);
	mesh->header.gt4s  =  (FMA_GT4*)ADD2POINTER(mesh->header.gt3s, mesh->header.n_gt3s * sizeof(FMA_GT3));
	mesh->header.length = (int)ADD2POINTER(mesh->header.gt4s, mesh->header.n_gt4s * sizeof(FMA_GT4));

	if(append)
	{
		Appendfile(&mesh->header,sizeof(FMA_MESH_HEADER),fname);
	}
	else
	{
		Savefile(&mesh->header,sizeof(FMA_MESH_HEADER),fname);
	}

	mesh->header.tmap_crcs = (unsigned long*)temp[0]; 
	mesh->header.verts     = (VERT*)temp[1]; 
	mesh->header.gt3s      = (FMA_GT3*)temp[2];
	mesh->header.gt4s      = (FMA_GT4*)temp[3];


// Right. Now save out the data that the pointers/offsets point to.
	Appendfile(mesh->header.tmap_crcs,mesh->header.n_tmaps * sizeof(unsigned long),fname);
	Appendfile(mesh->header.verts,vert_size,fname);
	Appendfile(mesh->header.gt3s,mesh->header.n_gt3s * sizeof(FMA_GT3),fname);
	Appendfile(mesh->header.gt4s,mesh->header.n_gt4s * sizeof(FMA_GT4),fname);
}

void stringChange ( char *name )
{
	char *c, *d, *e;
	const char *obe = ".psi";
	const char *ndo = ".obe";

	for(c = name; *c; c++)
	{
		d = (char*)ndo, e = c;
		while(*e == *d)	// strstr!
		{
			e++, d++;
//			if (!*d) {	// end of ".obe" - found substring
				d = (char*)obe;
				while(*d) *(c++) = *(d++);	// strcpy!
				return;
	//		}
		}
	}
}

void stringChangeNew ( char *name )
{
	char *c, *d, *e;
	const char *obe = ".obe";
	const char *ndo = ".psi";

	for(c = name; *c; c++)
	{
		d = (char*)ndo, e = c;
		while(*e == *d)	// strstr!
		{
			e++, d++;
//			if (!*d) {	// end of ".obe" - found substring
				d = (char*)obe;
				while(*d) *(c++) = *(d++);	// strcpy!
				return;
	//		}
		}
	}
}

char infile[256];
char outfile[256];
void fma_savemap(char fname[256], int append, int num)
{
	FMA_MESH_ENTRY *mesh;
	FMA_WORLD *world;
	int meshcount = 0;
	int worldsize;
	unsigned long *crcs;


	for(mesh = fma_mesh_list; mesh; mesh = mesh->next)
	{

// We want platforms to be identified by OBE name, not by their internal mesh name
		if((!mesh->next) && (!save_map_segment))
		{
			char leafname[MAX_FNAME_LEN];
			ExtractLeafname(infile,&leafname[0]);
			mesh->header.name_crc = str2CRC(&leafname[0]);
		}

		fma_save(mesh, fname, append);
		append = 1;
		meshcount++;
	}

	printf("  %d FMM segments saved\n",meshcount);
	if(save_map_segment)
	{

		worldsize = sizeof(FMA_WORLD) + fma_mesh_count * sizeof(unsigned long);
		world = (FMA_WORLD*)zmalloc(worldsize);
		world->id = BFF_FMA_WORLD_ID;
		world->length = worldsize;

    int index;
    AnsiString newFile;
    char newString [ 256 ];

    newFile = objBankList [ Form1->ListBox1->ItemIndex ].objectList [ num ].fileName;
    while ( ( index = newFile.Pos ( AnsiString ( "\\" ) ) ) )
    {
      newFile.Delete ( 1, 1 );
    }
    // ENDWHILE

    sprintf ( newString, newFile.c_str());

    stringChange ( (char*)newString );

    strupr ( newString );

		world->name_crc = str2CRC( (char*)newString );
//		world->flags = flags;

    stringChangeNew ( (char*)newString );

		world->n_meshes = meshcount;

		crcs = (unsigned long*)ADD2POINTER(world, sizeof(FMA_WORLD));

		for(mesh = fma_mesh_list; mesh; mesh = mesh->next)
		{
			*crcs = mesh->header.name_crc;
			crcs++;
		}

		Appendfile(world,worldsize,fname);
		printf("  FMW segment saved\n",meshcount);
	}
}

void __fastcall TForm1::Button3Click(TObject *Sender)
{
// EXPORT FUNCTION

	char *addr;

  initCRC();
  ProgressBar1->Position = 0;

  if ( ListBox1->ItemIndex )
    SaveBankFile ( ListBox1->ItemIndex );
  SaveConfig();

  for ( register counter = 0; counter < objBankList [ ListBox1->ItemIndex ].numObjects; counter++ )
  {
    int index;

    AnsiString newInFile;
    AnsiString newOutFile;

    newInFile = objBankList [ ListBox1->ItemIndex ].objectList [ counter ].fileName;
    /*while ( ( index = newInFile.Pos ( AnsiString ( "\\" ) ) ) )
    {
      newInFile.Insert ( AnsiString ( "\\" ), index );
    }
    // ENDWHILE */

    newOutFile = objBankList [ ListBox1->ItemIndex ].outputDir;

    /*while ( ( index = newOutFile.Pos ( AnsiString ( "\\" ) ) ) )
    {
      newOutFile.Insert ( AnsiString ( "\\" ), index );
    }
    // ENDWHILE */

    sprintf ( infile, newInFile.c_str() );
    sprintf ( outfile, newOutFile.c_str() );
//    sprintf ( infile, objBankList [ ListBox1->ItemIndex ].objectList [ counter ].fileName );
//    sprintf ( outfile, objBankList [ ListBox1->ItemIndex ].outputDir );

  	gamma = objBankList [ ListBox1->ItemIndex ].objectList [ counter ].gamma;
 	  flags = objBankList [ ListBox1->ItemIndex ].objectList [ counter ].flags;

  	shift = objBankList [ ListBox1->ItemIndex ].objectList [ counter ].shift;

    addr = (char*)OpenMallocReadClose(infile);

    if(addr[0] == 'O' && addr[1] == 'B' && addr[2] == 'E' && addr[3] == 0)
    {
      addr += 4;

      if ( objBankList [ Form1->ListBox1->ItemIndex ].objectList [ counter ].flags & HALFUV )
        halfuv = 1;
      else
        halfuv = 0;
      // ENDIF

      if ( objBankList [ Form1->ListBox1->ItemIndex ].objectList [ counter ].flags & FLIPU )
        flipu = 1;
      else
        flipu = 0;
      // ENDIF
      if ( objBankList [ Form1->ListBox1->ItemIndex ].objectList [ counter ].flags & FLIPV )
        flipv = 1;
      else
        flipv = 0;
      // ENDIF

      if ( objBankList [ Form1->ListBox1->ItemIndex ].objectList [ counter ].flags & RESET )
        reset = 1;
      else
        reset = 0;
      // ENDIF

      if ( objBankList [ Form1->ListBox1->ItemIndex ].objectList [ counter ].flags & CENTERUV )
        center = 1;
      else
        center = 0;
      // ENDIF

      if ( objBankList [ Form1->ListBox1->ItemIndex ].objectList [ counter ].flags & ADDATIVE )
        adda = 1;
      else
        adda = 0;
      // ENDIF

  /*      if ( bffFiles [ Form1->ComboBox1->ItemIndex ].objects [ counter ].flags & FLIPU )
          flipU = 1;
        else
          flipU = 0;
        // ENDIF
        if ( bffFiles [ Form1->ComboBox1->ItemIndex ].objects [ counter ].flags & FLIPV )
          flipV = 1;
        else
          flipV = 0;
        // ENDIF         */

    		while ( addr )
    		{
    			addr = ( char* ) obe_parse ( addr );
    		}

        if ( counter == 0 )
    		  fma_savemap ( outfile, 0, counter );
        else
    		  fma_savemap ( outfile, 1, counter );
        // ENDELSEIF

          fma_mesh_list = NULL;
    	}
    	else
    	{
    	}
    }
    // ENDFOR


   // char temp[256];
    //FILE *fp;

  //  sprintf ( temp, "c:\\%s.bat", Form1->ComboBox1->Items->Strings [ Form1->ComboBox1->ItemIndex ].c_str() );
   // fp = fopen ( temp, "w" );


   // fclose ( fp );
//  }
  // ENDIF

}
//---------------------------------------------------------------------------

void __fastcall TForm1::ListBox1Click(TObject *Sender)
{
  static int prev = -1;

  if ( ListBox2->Items->Count && prev != -1 )
    SaveBankFile ( prev );

  SaveConfig();

  ListBox2->Items->Clear();

  LoadBankFile ( ListBox1->ItemIndex );

  Label5->Caption = objBankList [ ListBox1->ItemIndex ].numObjects;

  prev = ListBox1->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TrackBar1Change(TObject *Sender)
{
  Char temp[3];

  itoa ( TrackBar1->Position, temp, 10 );
  Label2->Caption = temp;

  gamma = TrackBar1->Position;

  objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].gamma = gamma;

  SaveBankFile ( ListBox1->ItemIndex );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::UpDown1Click(TObject *Sender, TUDBtnType Button)
{
  Edit1->Text = UpDown1->Position;

  objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].shift = atoi ( Edit1->Text.c_str() );

  SaveBankFile ( ListBox1->ItemIndex );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ListBox2Click(TObject *Sender)
{
  Char temp[3];

  if ( objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags & ADDATIVE )
    CheckBox1->State = TRUE;
  else
    CheckBox1->State = FALSE;
  // ENDIF

  if ( objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags & HALFUV )
    CheckBox3->State = TRUE;
  else
    CheckBox3->State = FALSE;
  // ENDIF

  if ( objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags & RESET )
    CheckBox4->State = TRUE;
  else
    CheckBox4->State = FALSE;
  // ENDIF

  if ( objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags & USLIDDING )
    CheckBox5->State = TRUE;
  else
    CheckBox5->State = FALSE;
  // ENDIF
  if ( objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags & VSLIDDING )
    CheckBox6->State = TRUE;
  else
    CheckBox6->State = FALSE;
  // ENDIF

  if ( objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags & PLUSSLIDDING )
    CheckBox7->State = TRUE;
  else
    CheckBox7->State = FALSE;
  // ENDIF
  if ( objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags & MINUSSLIDDING )
    CheckBox8->State = TRUE;
  else
    CheckBox8->State = FALSE;
  // ENDIF

  if ( objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags & JIGGLE )
    CheckBox9->State = TRUE;
  else
    CheckBox9->State = FALSE;
  // ENDIF
  if ( objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags & MODGY )
    CheckBox10->State = TRUE;
  else
    CheckBox10->State = FALSE;
  // ENDIF

  if ( objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags & CENTERUV )
    CheckBox11->State = TRUE;
  else
    CheckBox11->State = FALSE;
  // ENDIF

  if ( objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags & FLIPU )
    CheckBox12->State = TRUE;
  else
    CheckBox12->State = FALSE;
  // ENDIF

  if ( objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags & FLIPV )
    CheckBox13->State = TRUE;
  else
    CheckBox13->State = FALSE;
  // ENDIF

  TrackBar1->Position = objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].gamma;

  Edit1->Text = objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].shift;

  itoa ( objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].gamma, temp, 10 );

  Label2->Caption = temp;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox1Click(TObject *Sender)
{
  if ( CheckBox1->State )
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags |= ADDATIVE;
  else
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags &= ~ADDATIVE;
  // ENDIF
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox2Click(TObject *Sender)
{
  if ( CheckBox2->State )
  {
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags |= SUBTRACTIVE;
  }
  else
  {
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags &= ~SUBTRACTIVE;
  }
  // ENDIF
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
  if ( ListBox1->ItemIndex != -1 )
    SaveBankFile ( ListBox1->ItemIndex );
  SaveConfig();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox3Click(TObject *Sender)
{
  if ( CheckBox3->State )
  {
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags |= HALFUV;
  }
  else
  {
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags &= ~HALFUV;
  }
  // ENDIF
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox4Click(TObject *Sender)
{
  if ( CheckBox4->State )
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags |= RESET;
  else
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags &= ~RESET;
  // ENDIF
}
//---------------------------------------------------------------------------


void __fastcall TForm1::CheckBox5Click(TObject *Sender)
{
  if ( CheckBox5->State )
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags |= USLIDDING;
  else
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags &= ~USLIDDING;
  // ENDIF
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox7Click(TObject *Sender)
{
  if ( CheckBox7->State )
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags |= PLUSSLIDDING;
  else
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags &= ~PLUSSLIDDING;
  // ENDIF
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox8Click(TObject *Sender)
{
  if ( CheckBox8->State )
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags |= MINUSSLIDDING;
  else
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags &= ~MINUSSLIDDING;
  // ENDIF
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox6Click(TObject *Sender)
{
  if ( CheckBox6->State )
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags |= VSLIDDING;
  else
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags &= ~VSLIDDING;
  // ENDIF
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox9Click(TObject *Sender)
{
  if ( CheckBox9->State )
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags |= JIGGLE;
  else
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags &= ~JIGGLE;
  // ENDIF
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox10Click(TObject *Sender)
{
  if ( CheckBox10->State )
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags |= MODGY;
  else
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags &= ~MODGY;
  // ENDIF
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox11Click(TObject *Sender)
{
  if ( CheckBox11->State )
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags |= CENTERUV;
  else
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags &= ~CENTERUV;
  // ENDIF
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox12Click(TObject *Sender)
{
  if ( CheckBox12->State )
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags |= FLIPU;
  else
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags &= ~FLIPU;
  // ENDIF
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBox13Click(TObject *Sender)
{
  if ( CheckBox13->State )
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags |= FLIPV;
  else
    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].flags &= ~FLIPV;
  // ENDIF

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button5Click(TObject *Sender)
{
  // Jim HACK - Set all objects' remove flag to zero to make absolutely sure
  // we don't delete any we shouldn't
  for( int i=0; i<objBankList[ListBox1->ItemIndex].numObjects; i++ )
    objBankList[ListBox1->ItemIndex].objectList[i].remove = 0;

  // Then remove the one we want to
  objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].remove = 1;

  SaveBankFile ( ListBox1->ItemIndex );

  SaveConfig();

  ListBox2->Items->Clear();

  LoadBankFile ( ListBox1->ItemIndex );

  Label5->Caption = objBankList [ ListBox1->ItemIndex ].numObjects; 
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button6Click(TObject *Sender)
{
  objBankList [ ListBox1->ItemIndex ].remove = 1;

  SaveConfig();

  ListBox1->Items->Clear();
  ListBox2->Items->Clear();

  LoadConfig();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit1Change(TObject *Sender)
{
//  if ( numObjectBanks > 0 )
//    objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].shift = atoi ( Edit1->Text.c_str() );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit1KeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
//  objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].shift = atoi ( Edit1->Text.c_str() );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit1Enter(TObject *Sender)
{
  //objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].shift = atoi ( Edit1->Text.c_str() );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit1KeyPress(TObject *Sender, char &Key)
{
  objBankList [ ListBox1->ItemIndex ].objectList [ ListBox2->ItemIndex ].shift = atoi ( Edit1->Text.c_str() );

}
//---------------------------------------------------------------------------



