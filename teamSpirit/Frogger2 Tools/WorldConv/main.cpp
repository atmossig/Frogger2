/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.

	File		: main.cpp
	Date		: 16/11/98

----------------------------------------------------------------------------------------------- */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "define.h"
#include "lookup.h"


char inF[255],outF[255];

enum { pc_source, psx_data } outputVersion;

enum
{
	INVALID = 0,
	NORMAL,
	FROG,
	FROG0,
	FROG1,
	FROG2,
	FROG3,
	BABY,
	AUTOHOP,
	SAFE,
	DEADLY,
	FALL,
	FALLDEATH,
	ICE,
	SINK,
	JUMP,
	BARRED,
	SUPERHOP,
	JOIN,
	CONV0SLOW,
	CONV1SLOW,
	CONV2SLOW,
	CONV3SLOW,
	CONV0,
	CONV1,
	CONV2,
	CONV3,
	CONV0FAST,
	CONV1FAST,
	CONV2FAST,
	CONV3FAST,
	CONV0ONEWAY,
	CONV1ONEWAY,
	CONV2ONEWAY,
	CONV3ONEWAY,
	FROGGER1AREA,
	FROGGER2AREA,
	FROGGER3AREA,
	FROGGER4AREA,
	HOT,

	NUM_MATERIALS
};

const char* materialnames[NUM_MATERIALS] =
{
	"",
	"normal",
	"froggerstart",
	"frogger1start",
	"frogger2start",
	"frogger3start",
	"frogger4start",
	"babystart",
	"autohop",
	"safe",
	"deadly",
	"fall",
	"deadlyfall",
	"ice",
	"sink",
	"platjump",
	"barred",
	"superjump",
	"link",
	"conveyor0slow",
	"conveyor1slow",
	"conveyor2slow",
	"conveyor3slow",
	"conveyor0",
	"conveyor1",
	"conveyor2",
	"conveyor3",
	"conveyor0fast",
	"conveyor1fast",
	"conveyor2fast",
	"conveyor3fast",
	"slide0",
	"slide1",	// short names for slidey conveyors
	"slide2",
	"slide3",
	"frogger1area",
	"frogger2area",
	"frogger3area",
	"frogger4area",
	"hot"
};

int numFrogs = 0;
int numBabys = 0;
int numSafes = 0;
int numPowers = 0;

int frogs[100] = {-1,-1,-1,-1,-1};
int babys[100];
int safes[100];
int powers[100];

#define CFILE_HEADER "/*\n\n    This file was generated automatically by the World Converter.\n\n---------------------------------------------------------------------*/\n\n"
//#define CFILEPC_INCLUDES "#include <ultra64.h>\n#include \"incs.h\"\n"
#define CFILEPC_INCLUDES "#include \"define.h\"\n"
#define CFILE_INCLUDES "#include <ultra64.h>\n#include \"incs.h\"\n#include \"define.h\"\n"
#define CFILE_BREAK "\n// ------------------------------------------------------------------\n\n"
#define CFILE_EXTERNDEF "extern GAMETILE "


struct vtx
{
	float x,y,z;
};

struct face
{
	long i[3];
	long e[3];
	long mat;
	bool used;
};

struct square
{
	vtx	centre;		// Centrepoint
	vtx   n[4];		// camera vectors to adjacent squares
	//vtx   ed[4];	// Edges
	long ed[4];
	vtx	  vn;		// Normal (up vector)
	int adj[4];
	char status;
};

// --------------------- Camera ------------------

struct cam_plane
{
	vtx normal;
	float k;
	unsigned long status;
};

#define MAX_CAM_VERTS	20
#define MAX_CAM_TRIS	30
#define MAX_CAM_PLANES	15

struct build_camera_box
{
	vtx vertices[MAX_CAM_VERTS];
	face triangles[MAX_CAM_TRIS];
	//cam_plane planes[MAX_CAM_PLANES];

	int num_vertices, num_triangles;
};

struct camera_box
{
	int firstplane;
	int num_planes;
};

// ---------------------------------

cam_plane camera_planes[500];	// lots
int num_cam_planes = 0;			// none

camera_box camera_boxes[100];	// some
int num_cam_boxes = 0;			// none


build_camera_box testcambox;


#define A 0
#define B 1
#define C 2

#define AB 0
#define BC 1
#define CA 2

int mat[50];

vtx vertexList [20000];
int nVtx = 0;

face faceList [20000];
int nFace = 0;

square squareList[10000];
long nSquare = 0;

Lookup materialLookup;

char wiz[5] = "|/-\\";
int spinner = 0;
int tile_x = 64;

void PrintSpinner(void)
{
	putchar(wiz[spinner]);
	putchar('\b');
	
	if (++spinner > 3) spinner = 0;
}

void PutTileChar(char c)
{
	if (tile_x == 64)
	{
		tile_x = 0;
		printf("\n\t");
	}
	putchar(c);
	tile_x++;
	//putchar('\b');
}

float Magnitude(vtx *vect)
{
	return sqrtf(vect->x*vect->x + vect->y*vect->y + vect->z*vect->z);
}

void Normalise(vtx *vect)
{
	float m = Magnitude(vect);

	if(m != 0)
	{
		m = 1.0f/m;
		vect->x *= m;
		vect->y *= m;
		vect->z *= m;
	}
}

/* --------------------------------------------------------------------------------
	Function	: getToken
	Purpose		: if the given char** points to a substring, move it ahead and return 1
	Parameters	: char**, const char*
	Returns		: 1 if starts with substring, 0 otherwise
*/
int getToken(char **ptr, const char* substr)
{
	char *p = *ptr, *q = (char*)substr;

	while (*q)
		if (*(p++) != *(q++)) return 0;

	*ptr = p;
	return 1;
}


char *get_delim_str(char *dest, const char *str, const char d)
{
	char *a = (char*)str, *p = dest;

	while (*a && *a != d) a++;

	if (!*a) return 0;
	
	a++;

	while (*a && *a != d) *(p++) = *(a++);

	*p = 0;

	return dest;
}

/* -------------------------------------------------------------------------------- */

void InitTables(void)
{
	int i;

	for (i=1; i<NUM_MATERIALS; i++)
		materialLookup.AddEntry(materialnames[i], i);

	for (i=0; i<NUM_MATERIALS; i++)
		mat[i] = 0;

//	testcambox.num_planes = 0;
	testcambox.num_triangles = 0;
	testcambox.num_vertices = 0;
}

/* -------------------------------------------------------------------------------- */

void CalcNormal2(vtx *result, vtx *tris)
{
	float ox[3],oy[3],oz[3];	
	float ax,ay,az,bx,by,bz;
	float onx,ony,onz,len;
	
	for(int l = 0; l < 3; l++)
	{
		ox[l] = tris[l].x;
		oy[l] = tris[l].y;
		oz[l] = tris[l].z;
	}

	/* Perform cross-product */
	
	ax = ox[1]-ox[0];
	ay = oy[1]-oy[0];
	az = oz[1]-oz[0];
	bx = ox[2]-ox[1];
	by = oy[2]-oy[1];
	bz = oz[2]-oz[1];
	
	onx = ay*bz - az*by;
	ony = az*bx - ax*bz;
	onz = ax*by - ay*bx;
	
	/* Normalise */
	
	len = (float)sqrt((onx*onx)+(ony*ony)+(onz*onz));
	
	if (len)
	{
		len = 1.0f/len;
		onx *= len;
		ony *= len;
		onz *= len;
	}
	else
	{
		onx = (float)0.0;
		ony = (float)1.0;
		onz = (float)0.0;
	}
	
	result->x=onx;
	result->y=ony;
	result->z=onz;
}

void CalculateNormal (square *me)
{
	vtx v[3];

	for (int i=0; i<3; i++)
		v[i] = vertexList[me->ed[i]];

	CalcNormal2(&me->vn, v);
}

/*	----
	Mark faces.. maybe it'll work, maybe it won't. Who knows?
	Ooh neato, it does work.
*/

void markFace(face *faces, long index, long num);

void markFaceEdge(face *faces, int i1, int i2, long num)
{
	face *f = faces;
	long i;

	for (i = 0; i < num; i++, f++)
	{
		if (f->used) continue;
		
		if ((!f->e[0] && (f->i[1] == i1 && f->i[0] == i2)) ||
			(!f->e[1] && (f->i[2] == i1 && f->i[1] == i2)) ||	// good heavens
			(!f->e[2] && (f->i[0] == i1 && f->i[2] == i2)))
		{
			markFace(faces, i, num);
		}
	}
}

/*	------
	Mark face (index) of the list as used, and any adjacent...
*/
void markFace(face *faces, long index, long num)
{
	face *f = faces + index;
	
	f->used = true;

	if (!f->e[0])
		markFaceEdge(faces, faces->i[0], faces->i[1], num);	// AB

	if (!f->e[1])
		markFaceEdge(faces, faces->i[1], faces->i[2], num);	// BC

	if (!f->e[2])
		markFaceEdge(faces, faces->i[2], faces->i[0], num);	// CA
}


/*	------------------------------------------------
	Function:	CalcCameraBox
	Purpose:	Calculate the planes of a test camera box

*/

void CalcCameraBox(void)
{
	vtx normal, v[3];
	float k;
	int i, num_bounding = 0;

	build_camera_box *box = &testcambox;

	cam_plane *p = &camera_planes[num_cam_planes];
	camera_box *b = &camera_boxes[num_cam_boxes];

	b->firstplane = num_cam_planes;
	b->num_planes = 0;

	for (i = 0; i < box->num_triangles; i++)
	{
		if (box->triangles[i].used) continue;

		markFace((box->triangles+i), 0, (box->num_triangles-i));

		for (int j=0; j<3; j++)
			v[j] = box->vertices[box->triangles[i].i[j]];

		CalcNormal2(&normal, v);

		// Calculate 'k' with a dot product
		k = (normal.x * v[0].x) + (normal.y * v[0].y) + (normal.z * v[0].z);

		p->normal = normal;	p->k = k;
		
		if (box->triangles[i].mat == JOIN)
			p->status = 0;
		else
		{
			p->status = 1;
			num_bounding++;
		}
		
		num_cam_planes++, p++;
		b->num_planes++;
	}

	num_cam_boxes++;

	printf("(%d planes, %d bounding)", b->num_planes, num_bounding);
}



/* --------------------------------------------------------------------------------
	Function	: TileMaterial
	Purpose		: Assigns materials to gametiles
	Parameters	: material, square#
	Returns		:
*/
void TileMaterial(int mat, int nSquare)
{
	int tileType = TILESTATE_NORMAL;

	switch (mat)
	{
		case FROG:
			frogs[numFrogs++] = nSquare;
			PutTileChar('f');
			break;
		case FROG0:
			frogs[0] = nSquare;
			PutTileChar('1');
			break;
		case FROG1:
			frogs[1] = nSquare;
			if (numFrogs<2) numFrogs = 2;
			PutTileChar('2');
			break;
		case FROG2:
			frogs[2] = nSquare;
			if (numFrogs<3) numFrogs = 3;
			PutTileChar('3');
			break;
		case FROG3:
			frogs[3] = nSquare;
			if (numFrogs<4) numFrogs = 4;
			PutTileChar('4');
			break;
		case BABY:
			babys[numBabys++] = nSquare;
			PutTileChar('b');
			break;
		case AUTOHOP:
			powers[numPowers++] = nSquare;
			PutTileChar('a');
			break;
		case DEADLY:
			PutTileChar(',');
			tileType = TILESTATE_DEADLY;
			break;
		case FALLDEATH:
			PutTileChar('\'');
			tileType = TILESTATE_DEADLYFALL;
			break;
		case FALL:
			PutTileChar('F');
			tileType = TILESTATE_FALL;
			break;
		case SINK:
			PutTileChar('v');
			tileType = TILESTATE_SINK;
			break;
		case SAFE:
			PutTileChar('s');
			tileType = TILESTATE_SAFE;
			break;
		case ICE:
			PutTileChar('i');
			tileType = TILESTATE_ICE;
			break;
		case SUPERHOP:
			PutTileChar('@');
			tileType = TILESTATE_SUPERHOP;
			break;
		case JOIN:
			PutTileChar('#');
			tileType = TILESTATE_JOIN;
			break;
		case BARRED:
			PutTileChar('£');
			tileType = TILESTATE_BARRED;
			break;
		case HOT:
			PutTileChar('*');
			tileType = TILESTATE_HOT;
			break;

		case FROGGER1AREA:
		case FROGGER2AREA:
		case FROGGER3AREA:
		case FROGGER4AREA:
			PutTileChar('1'+(mat-FROGGER1AREA));
			tileType = TILESTATE_FROGGER1AREA+(mat-FROGGER1AREA);
			break;

		case CONV0:
		case CONV1:
		case CONV2:
		case CONV3:
			PutTileChar('<');
			tileType = mat - CONV0 + TILESTATE_CONVEYOR_MED;
			break;

		case CONV0SLOW:
		case CONV1SLOW:
		case CONV2SLOW:
		case CONV3SLOW:
			PutTileChar('>');
			tileType = mat - CONV0SLOW + TILESTATE_CONVEYOR_SLOW;
			break;

		case CONV0FAST:
		case CONV1FAST:
		case CONV2FAST:
		case CONV3FAST:
			PutTileChar('^');
			tileType = mat - CONV0FAST + TILESTATE_CONVEYOR_FAST;
			break;

		case CONV0ONEWAY:
		case CONV1ONEWAY:
		case CONV2ONEWAY:
		case CONV3ONEWAY:
			PutTileChar('/');
			tileType = mat - CONV0FAST + TILESTATE_CONVEYOR_ONEWAY;
			break;
	}

	squareList[nSquare].status = tileType;
}


/* --------------------------------------------------------------------------------
	Function	: BuildSquareList
	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

void BuildSquareList(void)
{
	int i=0,j=0;
	for (i=0; i<nFace; i++)
		faceList[i].used = false;

	printf("Building tile list.. ");
	
	for (i=0; i<nFace; i++)
	{
		long tri1[3], tri2[3];

		//long p1, p2;
		vtx e1, e2, e3;

		if (faceList[i].used) continue;

		/*	let AB = invisible edge of this triangle
			
			tri1[0] and tri1[1] are the indices of A and B
		*/

		if (!faceList[i].e[AB])
		{
			tri1[0] = faceList[i].i[A];
			tri1[1] = faceList[i].i[B];
			tri1[2] = faceList[i].i[C];
		}
		else if (!faceList[i].e[BC])
		{
			tri1[0] = faceList[i].i[B];
			tri1[1] = faceList[i].i[C];
			tri1[2] = faceList[i].i[A];
		}
		else if (!faceList[i].e[CA])
		{
			tri1[0] = faceList[i].i[C];
			tri1[1] = faceList[i].i[A];
			tri1[2] = faceList[i].i[B];
		}
		else // It's just a triangle!
		{
			e1 = vertexList[faceList[i].i[A]];
			e2 = vertexList[faceList[i].i[B]];
			e3 = vertexList[faceList[i].i[C]];

			faceList[i].used = true;
			TileMaterial(faceList[i].mat, nSquare);

			squareList[nSquare].centre.x = (e1.x + e2.x + e3.x) * 1/3.0f;
			squareList[nSquare].centre.y = (e1.y + e2.y + e3.y) * 1/3.0f;
			squareList[nSquare].centre.z = (e1.z + e2.z + e3.z) * 1/3.0f;

			squareList[nSquare].ed[0] = faceList[i].i[A];
			squareList[nSquare].ed[1] = faceList[i].i[B];
			squareList[nSquare].ed[2] = squareList[nSquare].ed[3] = faceList[i].i[C];
			
			for (int a=0; a<4; a++)
				squareList[nSquare].adj[a] = -1;

			CalculateNormal(&squareList[nSquare]);	// safe since it only uses AB and AC (CD is zero)

			nSquare++;
			continue;
		}
					
		faceList[i].used = true;

		e1 = vertexList[tri1[1]];
		e2 = vertexList[tri1[2]];
		e3 = vertexList[tri1[0]];

		for (j=(i+1); j<nFace; j++)
		{
			//long tri2[3], tri2[3];
			vtx te1, te2, te3;

			if (faceList[j].used || faceList[j].mat != faceList[i].mat) continue;

			if (!faceList[j].e[AB])
			{
				tri2[0] = faceList[j].i[A];
				tri2[1] = faceList[j].i[B];
				tri2[2] = faceList[j].i[C];
			}
			else if (!faceList[j].e[BC])
			{
				tri2[0] = faceList[j].i[B];
				tri2[1] = faceList[j].i[C];
				tri2[2] = faceList[j].i[A];
			}
			else if (!faceList[j].e[CA])
			{
				tri2[0] = faceList[j].i[C];
				tri2[1] = faceList[j].i[A];
				tri2[2] = faceList[j].i[B];
			}
			else
				continue;

			te1 = vertexList[tri2[0]];
			te2 = vertexList[tri2[2]];
			te3 = vertexList[tri2[1]];
			
			//int numSame = (tri1[0] == tri2[0]) +  + (tri1[1] == tri2[1]);

			// verts are always numbered in the same direction for triangles facing the same way
			if ((tri1[0] == tri2[1]) && (tri1[1] == tri2[0]))	
			{						
				faceList[j].used = true;

				TileMaterial(faceList[j].mat, nSquare);

				squareList[nSquare].centre.x = (e1.x+e2.x+e3.x+te2.x)/4;
				squareList[nSquare].centre.y = (e1.y+e2.y+e3.y+te2.y)/4;
				squareList[nSquare].centre.z = (e1.z+e2.z+e3.z+te2.z)/4;

				squareList[nSquare].ed[0] = tri1[1];
				squareList[nSquare].ed[1] = tri1[2];
				squareList[nSquare].ed[2] = tri2[1];
				squareList[nSquare].ed[3] = tri2[2];

				for (int a=0; a<4; a++)
					squareList[nSquare].adj[a] = -1;

				CalculateNormal (&squareList[nSquare]);
				
				nSquare++;

				break;
			}
			//if (numSame>2)
			//	printf ("Error - Face with more than 2 of same Vtx!\n");
		}
	}

	printf(" done\n");
}


/* --------------------------------------------------------------------------------
	Function	: FindAdjacentEdge
	Purpose		: brute-force searches the square list for a shared edge
	Parameters	: edge to check, index of square, index of vert 1, index of vert 2
	Returns		: void
*/

struct adj_match
{
	long tile;
	int edge;
};

void FindAdjacentEdge(long edge, long tile)
{
	long j, k;
	long v1, v2;
	adj_match join, closest;

	join.tile = -1;
	closest.tile = -1;

	if (squareList[tile].adj[edge] != -1) return;

	v1 = squareList[tile].ed[edge], v2 = squareList[tile].ed[(edge+1) % 4];

	// Go through and check with every square

	for (j=0; j<nSquare; j++)
	{
		if (j == tile) continue;

		for (k = 0; k<4; k++)
		{
			//if (squareList[j].adj[k] != -1) continue;

			if (squareList[j].ed[k] == v2 && squareList[j].ed[(k+1) % 4] == v1)
			{
				if (squareList[j].status == TILESTATE_JOIN)
				{
					join.tile = j;
					join.edge = k;
				}
				else
				{
					closest.tile = j;
					closest.edge = k;
				}

/*				squareList[tile].adj[edge] = j;
				
				if (squareList[j].adj[k] != -1)
					squareList[j].adj[k] = tile;
				return;
*/
			}
		}
	}

	// We want to always accept normal tiles above join tiles

	if (closest.tile != -1)
	{
		squareList[tile].adj[edge] = closest.tile;
	}
	else if (join.tile != -1)
	{
		squareList[tile].adj[edge] = join.tile;
	}
}

/* --------------------------------------------------------------------------------
	Function	: CalculateAdj
	Purpose		: Finds the adjacent edges in the square list
	Parameters	: 
	Returns		: void
*/

void CalculateAdj(void)
{
	long i, edge;

	printf("Calculating adjacent squares.. ");

	//Step thru squares to find adjacent ones.
	for (i=0; i<nSquare; i++)
	{
		if (i%20==0) PrintSpinner();

		for (edge=0; edge<4; edge++)
			FindAdjacentEdge(edge, i);
	}
	
	// Fill out the "normals", aka direction vectors
	for (i=0; i<nSquare; i++)
	{
		vtx t[4];
		square *s = &squareList[i];
		
		// Average east & west EDGE vectors to find north DIRECTION vector
		t[0].x = (vertexList[s->ed[1]].x-vertexList[s->ed[0]].x) + (vertexList[s->ed[2]].x-vertexList[s->ed[3]].x);
		t[0].y = (vertexList[s->ed[1]].y-vertexList[s->ed[0]].y) + (vertexList[s->ed[2]].y-vertexList[s->ed[3]].y);
		t[0].z = (vertexList[s->ed[1]].z-vertexList[s->ed[0]].z) + (vertexList[s->ed[2]].z-vertexList[s->ed[3]].z);
		Normalise (&t[0]);
		
		// Average north & south edge vectors to find east direction vector
		t[1].x = (vertexList[s->ed[2]].x-vertexList[s->ed[1]].x) + (vertexList[s->ed[3]].x-vertexList[s->ed[0]].x);
		t[1].y = (vertexList[s->ed[2]].y-vertexList[s->ed[1]].y) + (vertexList[s->ed[3]].y-vertexList[s->ed[0]].y);
		t[1].z = (vertexList[s->ed[2]].z-vertexList[s->ed[1]].z) + (vertexList[s->ed[3]].z-vertexList[s->ed[0]].z);
		Normalise (&t[1]);

		// south is opposite of north
		t[2].x = -t[0].x;
		t[2].y = -t[0].y;
		t[2].z = -t[0].z;

		// west is opposite of east
		t[3].x = -t[1].x;
		t[3].y = -t[1].y;
		t[3].z = -t[1].z;

		squareList[i].n[0] = t[0];
		squareList[i].n[1] = t[1];
		squareList[i].n[2] = t[2];
		squareList[i].n[3] = t[3];

	}

	printf("done\n");
}

/* ------------------------------------------------ */

int GetParamInt(char **p)
{
	char *in = *p;
	int a;

	while (*(++in) != ':');
	while ((*in < '0') || (*in > '9')) in++;
	a = atoi(in);
	
	*p = in;
	return a;
}

int ProcessFaceInfo(face *f, char *in)
{
	int a;
	while (*(++in) != ':');
	in++;

	// Get vertex numbers	
	for (a=0; a<3; a++)
		f->i[a] = GetParamInt(&in);

	// Get edge info
	for (a=0; a<3; a++)
		f->e[a] = GetParamInt(&in);

	//while ((in[0]!=0) && (strncmp(in,"*MESH_MTLID",11)!=0))in++;

	f->used = false;

	while (1)
	{
		while (*in && (*in != '*')) in++;

		if (!*in) break;

		if (strncmp(in,"*MESH_MTLID",11)==0)
		{
			int m;

			while (*in && (*in < '0' || *in > '9')) in++;
			m = atoi(in);

			f->mat = mat[m];
		}

		in++;
	}

	return 1;
}

int ProcessMeshInfo(char *in, vtx* verts, int *nv, face* faces, int *nf)
{
	if (getToken(&in,"*MESH_VERTEX "))
	{
		float x,y,z;
	
		in += 5;	// skip vertex num.. boh

		sscanf (in,"%f%f%f",&x,&y,&z);

		verts[*nv].x = x;
		verts[*nv].y = z;		// convert to our coord space
		verts[*nv].z = -y;
		(*nv)++;

		return 1;
	}
	else if (getToken(&in,"*MESH_FACE "))
	{
		ProcessFaceInfo(&faces[*nf], in);
		(*nf)++;

		return 1;
	}
	else
		return 0;
}


char objectName[50] = "";
enum { collision, none, cam_box } inObj = none;	// what "mode" are we in?

void FinishObject()
{
	switch (inObj)
	{
	case cam_box:
		CalcCameraBox();
		break;
	}
	
	inObj = none;
}

/* --------------------------------------------------------------------------------
	Function	: ReadData
	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

void ProcessLine(char *in)
{
	static int cMat = 0;

	// Munch whitespace
	while (*in && (*in != '*')) in++;
	if (!*in) return;
	
	if (inObj == none)
		if (getToken(&in,"*NODE_NAME "))
		{
			char name[100];
			get_delim_str(name, in, '\"');

			if (strcmp(objectName, name) != 0)
			{
				strcpy(objectName, name);
			}
				
			if (strncmp(name, "cam_", 4) == 0)
			{
				inObj = cam_box;
				testcambox.num_triangles = 0;
				testcambox.num_vertices = 0;
				
				printf("\n\t+ %s ", name);
			}
			else if (strcmp(name, "collision") == 0)
			{
				inObj = collision;
				printf("\n\t+ %s ", name);
			}
		}

	if (getToken(&in,"*GEOMOBJECT {"))
	{
		FinishObject();
		return;
	}
	
	if (inObj == none)
	{
		if (getToken(&in,"*SUBMATERIAL"))
		{
			int k=0;
			char *num;
			
			while (*in && (*in<'0' || *in>'9')) in++;
			if (!*in) return;

			num = in;
			
			while (*in>='0' && *in<='9') in++;
			*in = 0;

			cMat = atoi(num);
			return;
		}
		else if (getToken(&in,"*MATERIAL_NAME \""))
		{
			/* Match defined material names with the material numbers in the file */
			
			char material[80];
			char *i = in, *j = material;
			int m;

			// Find end quote

			while(*i && (*i != '\"'))
				*(j++) = *(i++);

			*j = 0;

			m = materialLookup.GetEntry(material);

			if (m && cMat) mat[cMat] = m;

			return;
		}
	}
	else
	{
		switch (inObj)
		{
		case collision:
			ProcessMeshInfo(in, vertexList, &nVtx, faceList, &nFace);
			break;

		case cam_box:
			build_camera_box *box = &testcambox;
			ProcessMeshInfo(in, box->vertices, &box->num_vertices, box->triangles, &box->num_triangles);
			break;
		}
	}
}
	

bool ReadData(void)
{
	FILE *in;
	char inStr[255];
	int line = 1;

	printf ("Reading %s.. ",inF);
	in = fopen (inF,"rt");
	
	if (!in)
	{
		fprintf(stderr, "Couldn't open file\n");
		return false;
	}

	/* Process file one line at a time */

	while (fgets(inStr,254,in))
	{	
		ProcessLine (inStr);
		line++;
	}
		
	fclose (in);
	FinishObject();

	putchar('\n');

	return true;
}


/* --------------------------------------------------------------------------------
	Function	: WriteData2
	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

int WriteInt(FILE *f, int i)
{
	int rit = fwrite(&i, 4, 1, f);
	if (rit != 1)
		printf("Bugger!\n");
	return rit;
}

int WriteShort(FILE *f, short s)
{
	int rit = fwrite(&s, 2, 1, f);
	if (rit != 1)
		printf("Bugger!\n");
	return rit;
}

#define RECORDSIZE		92

#define WRITEINT(v)		WriteInt(f, (int)(v))
#define WRITEINDEX(v)	WriteInt(f, ((v)==-1) ? -1 : (v)*RECORDSIZE)
#define WRITESHORT(v)	WriteShort(f, (short)(v))
#define WRITEFIXED(v)	WriteInt(f, (int)((v)*4096));

void WritePSXTileData(FILE *f)
{
	int i, tile;

	WRITEINT(nSquare);

	for (i=0; i<4; i++)
		WRITEINT(frogs[i]);

	WRITEINT(numBabys);

	for (i=0; i<numBabys; i++)
		WRITEINT(babys[i]);

	for (tile = 0; tile<nSquare; tile++)
	{
		for (i=0; i<4; i++)
			WRITEINT(squareList[tile].adj[i]);

		WRITEINT(tile+1);	// next

		WRITEINT(squareList[tile].status);
		
		WRITESHORT(squareList[tile].centre.x);
		WRITESHORT(squareList[tile].centre.y);
		WRITESHORT(squareList[tile].centre.z);
		WRITESHORT(0);

		WRITEINT(squareList[tile].vn.x * 4096);
		WRITEINT(squareList[tile].vn.y * 4096);
		WRITEINT(squareList[tile].vn.z * 4096);
		
		for (i=0; i<4; i++)
		{
			WRITEINT(squareList[tile].n[(i+1)%4].x * 4096);
			WRITEINT(squareList[tile].n[(i+1)%4].y * 4096);
			WRITEINT(squareList[tile].n[(i+1)%4].z * 4096);
		}
	}
}

void WritePSXCameraBoxes(FILE *f)
{
	int i;
	camera_box *box;
	cam_plane *p;

	WRITEINT(num_cam_planes);

	for (i = 0, p = camera_planes; i < num_cam_planes; i++, p++)
	{
		WRITEFIXED(p->normal.x);
		WRITEFIXED(p->normal.y);
		WRITEFIXED(p->normal.z);
		WRITEFIXED(p->k);
		WRITEINT(p->status);
	}

	WRITEINT(num_cam_boxes);
	
	for (i = 0, box = camera_boxes; i < num_cam_boxes; i++, box++)
	{
		WRITEINT(box->num_planes);
		WRITEINT(box->firstplane);
	}
}

/* --------------------------------------------------------------------------------
	Function	: WriteData
	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

void WriteTiles (FILE *fp)
{
	int j = 0;

	fprintf (fp, "\n\nGAMETILE tiles[%u] = {\n", nSquare);

	for (int i=0; i<nSquare; i++)
	{
		fprintf (fp,"  {\t// %u\n", i);

		// Yikes! This exciting mess prints "tiles+xxx" or just '0'
		// for our adjacent tiles.

		fprintf(fp, "	{ %s%d, %s%d, %s%d, %s%d },\n",
			(squareList[i].adj[0]==-1)?"":"tiles+",
			(squareList[i].adj[0]==-1)?0:squareList[i].adj[0],
			(squareList[i].adj[1]==-1)?"":"tiles+",
			(squareList[i].adj[1]==-1)?0:squareList[i].adj[1],
			(squareList[i].adj[2]==-1)?"":"tiles+",
			(squareList[i].adj[2]==-1)?0:squareList[i].adj[2],
			(squareList[i].adj[3]==-1)?"":"tiles+",
			(squareList[i].adj[3]==-1)?0:squareList[i].adj[3]);

		// 'next' pointer
		
		if (i < nSquare-1)
			fprintf(fp, "\ttiles+%d,\n", i+1);
		else
			fprintf(fp, "\tNULL,\n");
		
		// I don't know what this value is, or why it exists. Heigh ho.

		fprintf (fp,"	NULL,\n");

		// Tile status

		fprintf (fp,"	%lu,\n",squareList[i].status);
		
		// centre

		fprintf (fp,"	{%f,%f,%f},\n",
			squareList[i].centre.x,
			squareList[i].centre.y,
			squareList[i].centre.z);
		
		// normal

		fprintf (fp,"	{%f,%f,%f},\n",squareList[i].vn.x,squareList[i].vn.y,squareList[i].vn.z);
		
		// direction vectors

		fprintf (fp,"	{{%f,%f,%f},{%f,%f,%f},{%f,%f,%f},{%f,%f,%f}},\n",
			squareList[i].n[1].x,
			squareList[i].n[1].y,
			squareList[i].n[1].z,
			
			squareList[i].n[2].x,
			squareList[i].n[2].y,
			squareList[i].n[2].z,
			
			squareList[i].n[3].x,
			squareList[i].n[3].y,
			squareList[i].n[3].z,
			
			squareList[i].n[0].x,
			squareList[i].n[0].y,
			squareList[i].n[0].z
			);
		
		fprintf (fp,"  },\n",i);
	}

	fprintf(fp, "}; // END OF ARRAY\n\n");
}

/* --------------------------------------------------------------------------------
	Function	: WriteExterns
	Purpose		:
	Parameters	: (FILE *fp)
	Returns		: void 
*/

void WriteExterns(FILE *fp)
{
/*	for (unsigned int i=0; i<nSquare; i++)
	{
		if (squareList[i].parent[0]==0)
			fprintf(fp,"%sgT_%03i;\n",CFILE_EXTERNDEF,i);
		//else
		//	fprintf(fp,"%spT_%03i;\n",CFILE_EXTERNDEF,i);	
	}
*/

	fprintf(fp, CFILE_EXTERNDEF " tiles[%u];\n", nSquare);
}

void WriteHeaders (FILE *fp)
{
	int i;

	fprintf (fp,"\nGAMETILE *t_gTStart[4] = {%s%i, %s%i, %s%i, %s%i};\n",
		(frogs[0]==-1)?"":"tiles+",
		(frogs[0]==-1)?0:frogs[0],

		(frogs[1]==-1)?"":"tiles+",
		(frogs[1]==-1)?0:frogs[1],

		(frogs[2]==-1)?"":"tiles+",
		(frogs[2]==-1)?0:frogs[2],

		(frogs[3]==-1)?"":"tiles+",
		(frogs[3]==-1)?0:frogs[3]
		);

	fprintf(fp,"\nint t_numBabies = %i;\n",numBabys);

	if (numBabys)
	{
		fprintf(fp,"GAMETILE *t_bTStart[%i] = {",numBabys);		

		for (i=0; i<numBabys; i++)
			fprintf(fp,"tiles+%i,",babys[i]);
		fprintf(fp,"};\n");
	}
	else
		fprintf(fp, "GAMETILE **t_bTStart = NULL;\n");

	fprintf(fp,"\nint t_numSafe = %i;\n",0);
	fprintf(fp,"GAMETILE **t_bTSafe = NULL;");
	fprintf(fp,"\nint t_numPow = %i;\n",numPowers);
	fprintf(fp,"GAMETILE **t_gTpow = NULL;");

/*	if (pc)
	{
		if (numSafes)
		{
			fprintf(fp,"GAMETILE *t_bTSafe[%i] = {",numSafes);
			for (i=0; i<numSafes; i++)
				fprintf(fp,"tiles+%i, ",safes[i]);
			fprintf(fp,"};\n");
		}
		else
	}
	else
	{
		fprintf(fp,"GAMETILE *t_bTSafe[%i] = {",numSafes);
		for (i=0; i<numSafes; i++)
			fprintf(fp,"tiles+%i, ",safes[i]);
		fprintf(fp,"};\n");
	}

	if (pc)
	{
		if (numPowers)
		{
			fprintf(fp,"GAMETILE *t_gTpow[%i] = {",numPowers);
			for (i=0; i<numPowers; i++)
				fprintf(fp,"tiles+%i, ",powers[i]);
			fprintf(fp,"};\n");		
		}
		else
			fprintf(fp,"GAMETILE *t_gTpow[1];",numPowers);
	}
	else
	{
		fprintf(fp,"GAMETILE *t_gTpow[%i] = {",numPowers);
		for (i=0; i<numPowers; i++)
			fprintf(fp,"tiles+%i, ",powers[i]);
		fprintf(fp,"};\n");		
	}
*/
}

void WriteCameras(FILE *f)
{
	int i;
	camera_box *box;
	cam_plane *p;

	if (num_cam_boxes == 0)
	{
		fprintf(f, "CAM_BOX_LIST camera_list = { 0, NULL };\n\n");
		return;
	}

	fprintf(f, "/* Cameras */\n\n");
	fprintf(f, "CAM_PLANE camera_planes[%d] = {\n", num_cam_planes);

	for (i = 0, p = camera_planes; i < num_cam_planes; i++, p++)
	{
		fprintf(f, "\t{ { %f, %f, %f }, %0.2f, %d }",
			p->normal.x, p->normal.y, p->normal.z,
			p->k, p->status);
		
		if (i<(num_cam_planes-1)) putc(',', f);
		putc('\n', f);
	}

	fprintf(f, "};\n\n");

	fprintf(f, "CAM_BOX camera_boxes[%d] = {\n", num_cam_boxes);

	for (i = 0, box = camera_boxes; i < num_cam_boxes; i++, box++)
	{
		fprintf(f, "\t{ %d, camera_planes + %d }", box->num_planes, box->firstplane);
		
		if (i<(num_cam_planes-1)) putc(',', f);
		putc('\n', f);
	}

	fprintf(f, "};\n\n");

	fprintf(f, "CAM_BOX_LIST camera_list = { %d, camera_boxes };\n\n", num_cam_boxes);
}

int WritePCSource(const char* outF)
{
	FILE *out;

	printf ("Writing C source file %s..",outF);
	out = fopen (outF,"wt");

	fprintf(out,"/* Squares: %04lu  Faces:%4lu */\n\n",nSquare,nFace);

	fputs (CFILE_HEADER,out);
	fputs (CFILEPC_INCLUDES,out);
	fputs (CFILE_BREAK,out);

	WriteExterns (out);

	WriteHeaders (out);
	
	WriteTiles (out);

	WriteCameras (out);
	
	fprintf(out,"\n"
		"#define EXPORT extern \"C\" __declspec( dllexport )\n"
		"\n"
		"EXPORT long GetStartsAddress()\n"
		"{\n"
		"	return (long)&(t_gTStart[0]);\n"
		"}\n\n"
		"EXPORT long GetBabiesAddress()\n"
		"{\n"
		"	return (long)&(t_bTStart[0]);\n"
		"}\n\n"
/*		"EXPORT long GetSafesAddress()\n"
		"{\n"
		"	return (long)&(t_bTSafe[0]);\n"
		"}\n"
		"\n"
		"EXPORT long GetPowerupsAddress()\n"
		"{\n"
		"	return (long)&(t_gTpow[0]);\n"
		"}\n"
		"\n"
		"\n"*/
		"EXPORT long GetBabiesNum()\n"
		"{\n"
		"	return t_numBabies;\n"
		"}\n\n"
/*		"EXPORT long GetSafesNum()\n"
		"{\n"
		"	return t_numSafe;\n"
		"}\n"
		"\n"
		"EXPORT long GetPowerupsNum()\n"
		"{\n"
		"	return t_numPow;\n"
		"}\n"*/
		"EXPORT long GetFirst()\n"
		"{\n"
		"	return (long)tiles;\n"
		"}\n\n"
		"EXPORT long GetCameras()\n"
		"{\n"
		"	return (long)&camera_list;\n"
		"}\n\n");
		
	fclose (out);
	printf("done\n");

	return 1;
}

int WritePSXData(const char* outF)
{
	FILE *f;

	printf ("Writing PSX data file %s..",outF);
	f = fopen(outF, "wb");
	if (!f)
	{
		fprintf(stderr, "Couldn't open file!\n");
		return 0;
	}
	WritePSXTileData(f);
	WritePSXCameraBoxes(f);
	fclose(f);
	printf ("done\n");

	return 1;
}

/* --------------------------------------------------------------------------------
	Function	: WriteData
	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

int WriteData(void)
{
	if (numFrogs == 0)
	{
		puts("Error: No start tiles");
		return 0;
	}

	if (numFrogs>4)
	{
		puts("Error: More than 4 start tiles");
		return 0;
	}
	
	if (outputVersion == psx_data)
		return WritePSXData(outF);
	else
		return WritePCSource(outF);
}

/* --------------------------------------------------------------------------------
	Function	: main 
	Purpose		:
	Parameters	: (int argc, char *argv[])
	Returns		: int 
*/

int main (int argc, char *argv[])
{
	printf(
		"Frogger 2 World Converter (built "__DATE__") - Interactive Studios Ltd \n"
		"IN DEVELOPMENT! Please report any problems to dswift@intstudios.co.uk\n\n"
	);
	
	if (argc < 3)
	{		
		printf(
			"Parameters: [Infile] [OutFile]\n"
			"/pc\tOutput C source\n"
			"/psx\tOutput PSX-compatible raw data\n"
		);
		exit (1);
	}
	else if (argc > 3)
	{
		if (strcmp("/pc", argv[3]) == 0)
			outputVersion = pc_source;
		else if (strcmp("/psx", argv[3]) == 0)
			outputVersion = psx_data;
		else
		{
			printf("Unrecognised parameter\n");
			exit(1);
		}
	}

	InitTables();

	strcpy (inF,argv[1]);
	strcpy (outF,argv[2]);
	if (!ReadData()) return 1;
	BuildSquareList();
	CalculateAdj();

	return WriteData() ? 0 : 128;
}