/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: main.cpp
	Programmer	: Matthew Cloy (with modifications by David Swift)
	Date		: 16/11/98

----------------------------------------------------------------------------------------------- */


#include <windows.h>
#include "stdio.h"
#include "math.h"

#include "lookup.h"

char inF[255],outF[255];
char pc = 0;

enum
{
	TILESTATE_NORMAL,
	TILESTATE_DEADLY,
	TILESTATE_SINK,
	TILESTATE_ICE,
	TILESTATE_SUPERHOP,
	TILESTATE_JOIN,
	TILESTATE_SAFE,
	TILESTATE_RESERVED,
	TILESTATE_GRAPPLE,
	TILESTATE_SMASH,
	TILESTATE_BARRED,
	TILESTATE_LEVELCHANGE,

	TILESTATE_FROGGER1AREA,
	TILESTATE_FROGGER2AREA,
	TILESTATE_FROGGER3AREA,
	TILESTATE_FROGGER4AREA,

	TILESTATE_CONVEYOR0SLOW = 0x20,
	TILESTATE_CONVEYOR1SLOW,
	TILESTATE_CONVEYOR2SLOW,
	TILESTATE_CONVEYOR3SLOW,

	TILESTATE_CONVEYOR0MED,
	TILESTATE_CONVEYOR1MED,
	TILESTATE_CONVEYOR2MED,
	TILESTATE_CONVEYOR3MED,

	TILESTATE_CONVEYOR0FAST,
	TILESTATE_CONVEYOR1FAST,
	TILESTATE_CONVEYOR2FAST,
	TILESTATE_CONVEYOR3FAST,
};

#define TILESTATE_CONVEYOR 0x20
#define TILESTATE_CONVEYOR_SLOW TILESTATE_CONVEYOR0SLOW
#define TILESTATE_CONVEYOR_MED	TILESTATE_CONVEYOR0MED
#define TILESTATE_CONVEYOR_FAST TILESTATE_CONVEYOR0FAST

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
	FROGGER1AREA,
	FROGGER2AREA,
	FROGGER3AREA,
	FROGGER4AREA,

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
	"frogger1area",
	"frogger2area",
	"frogger3area",
	"frogger4area"
};

unsigned long numFrogs = 0;
unsigned long numBabys = 0;
unsigned long numSafes = 0;
unsigned long numPowers = 0;

unsigned long frogs[100] = {-1,-1,-1,-1,-1};
unsigned long babys[100];
unsigned long safes[100];
unsigned long powers[100];

#define CFILE_HEADER "/*\n\n    This file was generated automatically by the World Converter.\n\n---------------------------------------------------------------------*/\n\n"
#define CFILEPC_INCLUDES "#include <ultra64.h>\n#include \"incs.h\"\n"
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
	char parent[100];
};

struct square
{
	vtx	centre; //Centrepoint
	vtx   n[4]; //normals to squares
	vtx   ed[4]; //Edges
	vtx	  vn;
	unsigned long adj[4];
	char parent[100];
	char status;
};

#define A 0
#define B 1
#define C 2

#define AB 0
#define BC 1
#define CA 2

unsigned long mat[10000];

vtx vertexList [20000];
unsigned long nVtx = 0;

face faceList [20000];
unsigned long nFace = 0;

unsigned long fUsed[10000];

square squareList[10000];
unsigned long nSquare = 0;

Lookup materialLookup;

char wiz[5] = "|/-\\";
int spinner = 0;

void PrintSpinner(void)
{
	putchar(wiz[spinner]);
	putchar('\b');
	
	if (++spinner > 3) spinner = 0;
}

void PutTileChar(char c)
{
	putchar(c);
	putchar('\b');
}

/* -------------------------------------------------------------------------------- */

void InitTables(void)
{
	for (int i=1; i<NUM_MATERIALS; i++)
		materialLookup.AddEntry(materialnames[i], i);
}

/* -------------------------------------------------------------------------------- */

void CalculateNormal (square *me)
{
	float ox[3],oy[3],oz[3];	
	float ax,ay,az,bx,by,bz;
	float onx,ony,onz,len;
	
	for(unsigned long l = 0; l < 3; l++)
	{
		ox[l] = me->ed[l].x;
		oy[l] = me->ed[l].y;
		oz[l] = me->ed[l].z;
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
	
	me->vn.x=-onx;
	me->vn.y=-ony;
	me->vn.z=-onz;
}


#define FACE_THRESH 0.1


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: fCmp

	Purpose		:
	Parameters	: (float x1, float y1, float z1, float x2, float y2, float z2)
	Returns		: unsigned long 
*/

unsigned long fCmp(float x1, float y1, float z1, float x2, float y2, float z2)
{
	if (fabs(x1-x2)>FACE_THRESH) return 0;
	if (fabs(y1-y2)>FACE_THRESH) return 0;
	if (fabs(z1-z2)>FACE_THRESH) return 0;

	return 1;
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
	unsigned long i=0,j=0;
	for (i=0; i<nFace; i++)
		fUsed[i]=0;

	printf("Building tile list.. ");
	
	for (i=0; i<nFace; i++)
	{
		if ((i % 50) == 0)
			PrintSpinner();

		long p1, p2;
		vtx e1, e2, e3;

		if (fUsed[i]) continue;

		/*	let AB = invisible edge of this triangle
			
			p1 and p2 are the indices of A and B
			
			e1 = co-ords of B
			e2 = co-ords of A	} I'd prefer to use indices for these
			e3 = co-ords of C
		*/

		if (!faceList[i].e[AB])
		{
			p1 = faceList[i].i[A];
			p2 = faceList[i].i[B];

			e1 = vertexList[p2];
			e2 = vertexList[faceList[i].i[C]];
			e3 = vertexList[p1];
		}
		else if (!faceList[i].e[BC])
		{
			p1 = faceList[i].i[B];
			p2 = faceList[i].i[C];

			e1 = vertexList[p2];
			e2 = vertexList[faceList[i].i[A]];
			e3 = vertexList[p1];
		}
		else if (!faceList[i].e[CA])
		{
			p1 = faceList[i].i[A];
			p2 = faceList[i].i[C];

			e1 = vertexList[p1];
			e2 = vertexList[faceList[i].i[B]];
			e3 = vertexList[p2];
		}
		else // It's just a triangle!
		{
			e1 = vertexList[faceList[i].i[A]];
			e2 = vertexList[faceList[i].i[B]];
			e3 = vertexList[faceList[i].i[C]];

			fUsed[i] = 1;
			TileMaterial(faceList[i].mat, nSquare);

			squareList[nSquare].centre.x = (e1.x + e2.x + e3.x) * 1/3.0f;
			squareList[nSquare].centre.y = (e1.y + e2.y + e3.y) * 1/3.0f;
			squareList[nSquare].centre.z = (e1.z + e2.z + e3.z) * 1/3.0f;

			squareList[nSquare].ed[0] = e1;
			squareList[nSquare].ed[1] = e2;
			squareList[nSquare].ed[2] = squareList[nSquare].ed[3] = e3;
			
			//squareList[nSquare].ed[3].x = squareList[nSquare].ed[3].y = squareList[nSquare].ed[3].z = 100000.0; // nice out of range value

			for (int a=0; a<4; a++)
				squareList[nSquare].adj[a] = -1;

			CalculateNormal(&squareList[nSquare]);	// safe since it only uses AB and AC (CD is zero)

			strcpy(squareList[nSquare].parent,faceList[i].parent);

			nSquare++;
			continue;
		}
					
		fUsed[i]=1;

		for (j=(i+1); j<nFace; j++)
		{
			long p3, p4;
			vtx te1, te2, te3;

			if (fUsed[j]) continue;

			if (!faceList[j].e[AB])
			{
				p3 = faceList[j].i[A];
				p4 = faceList[j].i[B];

				te1 = vertexList[p4];
				te2 = vertexList[faceList[j].i[C]];
				te3 = vertexList[p3];
			}
			else if (!faceList[j].e[BC])
			{
				p3 = faceList[j].i[B];
				p4 = faceList[j].i[C];

				te1 = vertexList[p4];
				te2 = vertexList[faceList[j].i[A]];
				te3 = vertexList[p3];
			}
			else if (!faceList[j].e[CA])
			{
				p3 = faceList[j].i[A];
				p4 = faceList[j].i[C];

				te1 = vertexList[p3];
				te2 = vertexList[faceList[j].i[B]];
				te3 = vertexList[p4];
			}
			else
				continue;
			
			unsigned long numSame = (p1 == p3) + (p1 == p4) + (p2 == p3) + (p2 == p4);

			if (numSame == 2)
			{						
				fUsed[j]=1;

				TileMaterial(faceList[j].mat, nSquare);

				squareList[nSquare].centre.x = (e1.x+e2.x+e3.x+te2.x)/4;
				squareList[nSquare].centre.y = (e1.y+e2.y+e3.y+te2.y)/4;
				squareList[nSquare].centre.z = (e1.z+e2.z+e3.z+te2.z)/4;

				squareList[nSquare].ed[0] = e1;
				squareList[nSquare].ed[1] = e2;
				squareList[nSquare].ed[2] = e3;
				squareList[nSquare].ed[3] = te2;
				
				for (int a=0; a<4; a++)
					squareList[nSquare].adj[a] = -1;

				CalculateNormal (&squareList[nSquare]);
				
				strcpy(squareList[nSquare].parent,faceList[j].parent);
				
				nSquare++;

				break;
			}
			if (numSame>2)
				printf ("Error - Face with more than 2 of same Vtx!\n");
		}
	}

	printf("done\n");
}



/* --------------------------------------------------------------------------------
	Function	: CalculateAdj 
	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

void CheckSquare(unsigned long a, unsigned long i,float x1, float y1, float z1, float x2, float y2, float z2)
{

/*
	This test is insanely, ludicrously inefficient and it's no wonder it takes so long
*/
	
	unsigned long j;
	for (j=0; j<nSquare; j++)
	{
		if (j!=i)
		{
			unsigned long numSame = 0;
			
			if (fCmp(x1,y1,z1,squareList[j].ed[0].x,squareList[j].ed[0].y,squareList[j].ed[0].z))
				numSame++;

			if (fCmp(x1,y1,z1,squareList[j].ed[1].x,squareList[j].ed[1].y,squareList[j].ed[1].z))
				numSame++;

			if (fCmp(x1,y1,z1,squareList[j].ed[2].x,squareList[j].ed[2].y,squareList[j].ed[2].z))
				numSame++;

			if (fCmp(x1,y1,z1,squareList[j].ed[3].x,squareList[j].ed[3].y,squareList[j].ed[3].z))
				numSame++;
			
			
			if (fCmp(x2,y2,z2,squareList[j].ed[0].x,squareList[j].ed[0].y,squareList[j].ed[0].z))
				numSame++;
			if (fCmp(x2,y2,z2,squareList[j].ed[1].x,squareList[j].ed[1].y,squareList[j].ed[1].z))
				numSame++;
			if (fCmp(x2,y2,z2,squareList[j].ed[2].x,squareList[j].ed[2].y,squareList[j].ed[2].z))
				numSame++;
			if (fCmp(x2,y2,z2,squareList[j].ed[3].x,squareList[j].ed[3].y,squareList[j].ed[3].z))
				numSame++;
			
			if (numSame >= 2)	// I'm not sure this is 100% safe...
			{
				squareList[i].adj[a] = j;
				return;
			}
		}
	}
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

void CalculateAdj(void)
{
	unsigned long i;

	printf("Calculating adjacent squares.. ");

	/*	This is now the worst part of the program, using a brute force search,
		floating point comparisons on points and hundreds of duplicate checks
		
		- For every quad:
		- For all four edges:
		- Compare with all four edges of every other quad
		- TWICE to cater for edges in opposite directions...

		This is a good example of how NOT to code this test
	*/

	//Step thru squares to find adjacent ones.
	for (i=0; i<nSquare; i++)
	{
		if ((i%20) == 0) PrintSpinner();

		vtx a, b;

		a = squareList[i].ed[0];
		b = squareList[i].ed[1];
		CheckSquare (0,i,a.x,a.y,a.z,b.x,b.y,b.z);
		
		a = squareList[i].ed[1];
		b = squareList[i].ed[2];
		CheckSquare (1,i,a.x,a.y,a.z,b.x,b.y,b.z);
		
		a = squareList[i].ed[2];
		b = squareList[i].ed[3];
		CheckSquare (2,i,a.x,a.y,a.z,b.x,b.y,b.z);

		a = squareList[i].ed[3];
		b = squareList[i].ed[0];
		CheckSquare (3,i,a.x,a.y,a.z,b.x,b.y,b.z);
	}
	
	// Fill out the "normals", aka direction vectors
	for (i=0; i<nSquare; i++)
	{
		vtx t[4];
		
		// Average east & west EDGE vectors to find north DIRECTION vector
		t[0].x = (squareList[i].ed[1].x-squareList[i].ed[0].x) + (squareList[i].ed[2].x-squareList[i].ed[3].x);
		t[0].y = (squareList[i].ed[1].y-squareList[i].ed[0].y) + (squareList[i].ed[2].y-squareList[i].ed[3].y);
		t[0].z = (squareList[i].ed[1].z-squareList[i].ed[0].z) + (squareList[i].ed[2].z-squareList[i].ed[3].z);
		Normalise (&t[0]);
		
		// Average north & south edge vectors to find east direction vector
		t[1].x = (squareList[i].ed[2].x-squareList[i].ed[1].x) + (squareList[i].ed[3].x-squareList[i].ed[0].x);
		t[1].y = (squareList[i].ed[2].y-squareList[i].ed[1].y) + (squareList[i].ed[3].y-squareList[i].ed[0].y);
		t[1].z = (squareList[i].ed[2].z-squareList[i].ed[1].z) + (squareList[i].ed[3].z-squareList[i].ed[0].z);
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

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: ReadData

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

unsigned long lineNo = 0;

void ProcessLine(char *in)
{
	char *cPos = in;
	static char par[100];
	static unsigned char inObj = 0;
	static unsigned long cMat = 0;
	
	while ((in[0]!='*') && (in[0]!=0))in++;	
	
	if (inObj == 1)
		if (strncmp(in,"*NODE_NAME \"collision\"",22) == 0)
			inObj = 0;

	if (strncmp(in,"*GEOMOBJECT {",13) == 0)
	{
		inObj = 1;
		par[0] = 0;
	}

	if (strncmp(in,"*NODE_PARENT",12) == 0)
	{
		strcpy (par,&in[13]);
	}
	
	if (strncmp(in,"*SUBMATERIAL",12) == 0)
	{
		unsigned long k=0;
		char *in2;
		while (((in[0]<'0') || (in[0]>'9')) && (in[0]!=0)) in++;

		in2 = in;
		
		while (((in[0]>='0') && (in[0]<='9')) && (in[0]!=0)) in++;
		
		in[0] = 0;

		sscanf (in2,"%i",&cMat);		
	}
	
	/////////////////////////////////////////////////////////////////////////////
	//17+name

	if (strncmp(in,"*MATERIAL_NAME \"",16) == 0)
	{
		char material[20];
		char *i = in + 16, *j = material;
		int m;

		while(*i && (*i != '\"'))
			*(j++) = *(i++);

		*j = 0;

		m = materialLookup.GetEntry(material);

		if (m) mat[cMat] = m;
	}

	//////////////////////////////////////////////////////////////////////////////

	if (!inObj)
	{
		lineNo++;
		if ((lineNo % 50) == 0) PrintSpinner();

		if (strncmp(in,"*MESH_VERTEX ",13) == 0)
		{
			float x,y,z;
			in+=18;
			sscanf (in,"%f	%f	%f",&x,&y,&z);
			vertexList[nVtx].x = x;
			vertexList[nVtx].y = -y;
			vertexList[nVtx].z = z;
			nVtx++;
		}
		
		//A:  430 B:  431 C:  433 AB:    1 BC:    0 CA:    1	 *MESH_SMOOTHING 2 	*MESH_MTLID 1
		if (strncmp(in,"*MESH_FACE ",11) == 0)
		{
			long a,b,c,ab,bc,ca;			
			while ((in[0]!=':') && (in[0]!=0))in++;
			while ((in[0]!=':') && (in[0]!=0))in++;
			while (((in[0]<'0') || (in[0]>'9')) && (in[0]!=0))in++;
			sscanf (in,"%i",&a);
			
			while ((in[0]!=':') && (in[0]!=0))in++;
			while (((in[0]<'0') || (in[0]>'9')) && (in[0]!=0))in++;
			sscanf (in,"%i",&b);
			
			while ((in[0]!=':') && (in[0]!=0))in++;
			while (((in[0]<'0') || (in[0]>'9')) && (in[0]!=0))in++;
			sscanf (in,"%i",&c);
			
			while ((in[0]!=':') && (in[0]!=0))in++;
			while (((in[0]<'0') || (in[0]>'9')) && (in[0]!=0))in++;
			sscanf (in,"%i",&ab);
			
			while ((in[0]!=':') && (in[0]!=0))in++;
			while (((in[0]<'0') || (in[0]>'9')) && (in[0]!=0))in++;
			sscanf (in,"%i",&bc);
			
			while ((in[0]!=':') && (in[0]!=0))in++;
			while (((in[0]<'0') || (in[0]>'9')) && (in[0]!=0))in++;
			sscanf (in,"%i",&ca);
			
			faceList[nFace].i[A]=a;
			faceList[nFace].i[B]=b;
			faceList[nFace].i[C]=c;
			
			faceList[nFace].e[AB]=ab;
			faceList[nFace].e[BC]=bc;
			faceList[nFace].e[CA]=ca;

			while ((in[0]!=0) && (strncmp(in,"*MESH_MTLID",11)!=0))in++;

			if (strncmp(in,"*MESH_MTLID",11)==0)
			{
				unsigned long m;
				while (((in[0]<'0') || (in[0]>'9')) && (in[0]!=0))in++;
				sscanf (in,"%i",&m);
				faceList[nFace].mat=mat[m];
			}

			strcpy(faceList[nFace].parent,par);		
			printf(par);
			nFace++;
		}
	}
}
	

bool ReadData(void)
{
	FILE *in;
	char inStr[255];

	printf ("Reading %s.. ",inF);
	in = fopen (inF,"rt");
	
	if (!in)
	{
		fprintf(stderr, "Couldn't open file\n");
		return false;
	}

	fgets(inStr,200,in);
	while (!feof(in))
	{	
		ProcessLine (inStr);
		fgets(inStr,200,in);
	}
		
	fclose (in);
	printf("done\n");
	return true;
}

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: WriteData

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

void WriteTiles (FILE *fp)
{
	unsigned int j = 0;

	fprintf (fp, "\n\nGAMETILE tiles[%u] = {\n", nSquare);

	for (unsigned int i=0; i<nSquare; i++)
	{
		if (squareList[i].parent[0]==0)
		{
			fprintf (fp,"  {\t// %u\n", i);

			fprintf(fp, "	{ %s%d, %s%d, %s%d, %s%d },\n",
				(squareList[i].adj[0]==-1)?"":"tiles+",
				(squareList[i].adj[0]==-1)?0:squareList[i].adj[0],
				(squareList[i].adj[1]==-1)?"":"tiles+",
				(squareList[i].adj[1]==-1)?0:squareList[i].adj[1],
				(squareList[i].adj[2]==-1)?"":"tiles+",
				(squareList[i].adj[2]==-1)?0:squareList[i].adj[2],
				(squareList[i].adj[3]==-1)?"":"tiles+",
				(squareList[i].adj[3]==-1)?0:squareList[i].adj[3]);

			fprintf (fp,"	%s%3i,\n",(i==(nSquare-1))?"":"tiles+",(i==(nSquare-1))?0:i+1);
			fprintf (fp,"	0,\n");
			fprintf (fp,"	%lu,\n",squareList[i].status);
			
			fprintf (fp,"	{%f,%f,%f},\n",
				squareList[i].centre.x,
				squareList[i].centre.z,
				squareList[i].centre.y);
			
			fprintf (fp,"	{%f,%f,%f},\n",squareList[i].vn.x,squareList[i].vn.z,squareList[i].vn.y);
			
			fprintf (fp,"	{{%f,%f,%f},{%f,%f,%f},{%f,%f,%f},{%f,%f,%f}},\n",
				squareList[i].n[1].x,
				squareList[i].n[1].z,
				squareList[i].n[1].y,
				
				squareList[i].n[2].x,
				squareList[i].n[2].z,
				squareList[i].n[2].y,
				
				squareList[i].n[3].x,
				squareList[i].n[3].z,
				squareList[i].n[3].y,
				
				squareList[i].n[0].x,
				squareList[i].n[0].z,
				squareList[i].n[0].y
				);
			
			fprintf (fp,"  },\n",i);
		}
	}

	fprintf(fp, "} // END OF ARRAY\n;");
}

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
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
	if (numFrogs == 0)
	{
		puts("Error: No start tiles");
		exit(1);
	}

	if (numFrogs>4)
	{
		puts("Error: More than 4 start tiles");
		exit (1);
	}
	
	unsigned long i;

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

	fprintf(fp,"\nunsigned long t_numBabies = %i;\n",numBabys);

	fprintf(fp,"GAMETILE *t_bTStart[%i] = {",numBabys);		

	for (i=0; i<numBabys; i++)
		fprintf(fp,"tiles+%i,",babys[i]);
	fprintf(fp,"};\n");

	fprintf(fp,"\nunsigned long t_numSafe = %i;\n",numSafes);
	if (pc)
	{
		if (numSafes)
		{
			fprintf(fp,"GAMETILE *t_bTSafe[%i] = {",numSafes);
			for (i=0; i<numSafes; i++)
				fprintf(fp,"tiles+%i, ",safes[i]);
			fprintf(fp,"};\n");
		}
		else
			fprintf(fp,"GAMETILE *t_bTSafe[1];",numSafes);
	}
	else
	{
		fprintf(fp,"GAMETILE *t_bTSafe[%i] = {",numSafes);
		for (i=0; i<numSafes; i++)
			fprintf(fp,"tiles+%i, ",safes[i]);
		fprintf(fp,"};\n");
	}

	fprintf(fp,"\nunsigned long t_numPow = %i;\n",numPowers);
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
}

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: WriteData

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

void WriteData(void)
{
	FILE *out;
	printf ("Writing %s..",outF);
	out = fopen (outF,"wt");

	fprintf(out,"/* Squares: %04lu  Faces:%4lu */\n\n",nSquare,nFace);

	fputs (CFILE_HEADER,out);
	if (pc)
		fputs (CFILEPC_INCLUDES,out);
	else
		fputs (CFILE_INCLUDES,out);

	fputs (CFILE_BREAK,out);

	WriteExterns (out);

	WriteHeaders (out);
	
	WriteTiles (out);
	
	if (pc)
	fprintf(out,"\n\
		#define DllExport __declspec( dllexport )\n\
		\n\
		extern \"C\" DllExport long GetStartsAddress()\n\
		{\n\
			return (long)&(t_gTStart[0]);\n\
		}\n\
		\n\
		extern \"C\" DllExport long GetBabiesAddress()\n\
		{\n\
			return (long)&(t_bTStart[0]);\n\
		}\n\
		\n\
		extern \"C\" DllExport long GetSafesAddress()\n\
		{\n\
			return (long)&(t_bTSafe[0]);\n\
		}\n\
		\n\
		extern \"C\" DllExport long GetPowerupsAddress()\n\
		{\n\
			return (long)&(t_gTpow[0]);\n\
		}\n\
		\n\
		\n\
		extern \"C\" DllExport long GetBabiesNum()\n\
		{\n\
			return t_numBabies;\n\
		}\n\
		\n\
		extern \"C\" DllExport long GetSafesNum()\n\
		{\n\
			return t_numSafe;\n\
		}\n\
		\n\
		extern \"C\" DllExport long GetPowerupsNum()\n\
		{\n\
			return t_numPow;\n\
		}\n\
		extern \"C\" DllExport long GetFirst()\n\
		{\n\
			return (long)tiles;\n\
		}\n\n");
		

	
	fclose (out);
	printf("done\n");
}

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: main 

	Purpose		:
	Parameters	: (int argc, char *argv[])
	Returns		: int 
*/

int main (int argc, char *argv[])
{
	printf("Frogger 2 World Converter (built "__DATE__") - Interactive Studios Ltd \n\n");
	if (argc < 3)
	{		
		printf("Parameters: [Infile] [OutFile]\n");
		exit (1);
	}
	else if (argc > 3)
	{
		if (!_strnicmp("/PC", argv[3], 3))
			pc = 1;
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
	WriteData();

	return 0;
}