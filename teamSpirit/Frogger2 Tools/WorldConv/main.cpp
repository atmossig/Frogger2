/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: main.cpp
	Programmer	: Matthew Cloy
	Date		: 16/11/98

	Modified 04/08/99 (David Swift)
	- Saves worlds as a contiguous array

----------------------------------------------------------------------------------------------- */


#include <windows.h>
#include "stdio.h"
#include "math.h"

char inF[255],outF[255];
char pc = 0;

enum
{
	NORMAL,
	FROG,
	BABY,
	AUTOHOP,
	SAFE,
	DEADLY,
	SINK,
	JUMP,
	
	SUPERHOP,
	JOIN
};

unsigned long mat[200];

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
	float p[3]; //Centrepoint
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

vtx vertexList [3000];
unsigned long nVtx = 0;

face faceList [3000];
unsigned long nFace = 0;

unsigned long fUsed[3000];

square squareList[5000];
unsigned long nSquare = 0;

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
	
	ax = ox[1]-ox[0];
	ay = oy[1]-oy[0];
	az = oz[1]-oz[0];
	bx = ox[2]-ox[1];
	by = oy[2]-oy[1];
	bz = oz[2]-oz[1];
	
	onx = ay*bz - az*by;
	ony = az*bx - ax*bz;
	onz = ax*by - ay*bx;
	
	len = (float)sqrt((onx*onx)+(ony*ony)+(onz*onz));
	
	if (len != ((float)0.0))
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



/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: ProcessData

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

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
	unsigned long diff = 1;
	if (fabs(x1-x2)>FACE_THRESH)
		diff = 0;
	if (fabs(y1-y2)>FACE_THRESH)
		diff = 0;
	if (fabs(z1-z2)>FACE_THRESH)
		diff = 0;
	return diff;
}

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
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
	
	for (i=0; i<nFace; i++)
	{
		if ((i % 50) == 0)
		{
			printf(".");
		}

		float p1x,p1y,p1z,p2x,p2y,p2z,e1x,e1y,e1z,e2x,e2y,e2z,e3x,e3y,e3z;

		if (!fUsed[i])
		{
			if (!faceList[i].e[AB])
			{
				p1x = vertexList[faceList[i].i[A]].x;
				p1y = vertexList[faceList[i].i[A]].y;
				p1z = vertexList[faceList[i].i[A]].z;

				p2x = vertexList[faceList[i].i[B]].x;
				p2y = vertexList[faceList[i].i[B]].y;
				p2z = vertexList[faceList[i].i[B]].z;

				e1x = p2x;
				e1y = p2y;
				e1z = p2z;
				e2x = vertexList[faceList[i].i[C]].x;
				e2y = vertexList[faceList[i].i[C]].y;
				e2z = vertexList[faceList[i].i[C]].z;
				e3x = p1x;
				e3y = p1y;
				e3z = p1z;
			}
		
			if (!faceList[i].e[BC])
			{
				p1x = vertexList[faceList[i].i[B]].x;
				p1y = vertexList[faceList[i].i[B]].y;
				p1z = vertexList[faceList[i].i[B]].z;

				p2x = vertexList[faceList[i].i[C]].x;
				p2y = vertexList[faceList[i].i[C]].y;
				p2z = vertexList[faceList[i].i[C]].z;

				e1x = p2x;
				e1y = p2y;
				e1z = p2z;
				e2x = vertexList[faceList[i].i[A]].x;
				e2y = vertexList[faceList[i].i[A]].y;
				e2z = vertexList[faceList[i].i[A]].z;
				e3x = p1x;
				e3y = p1y;
				e3z = p1z;
			}
		
			if (!faceList[i].e[CA])
			{
				p1x = vertexList[faceList[i].i[A]].x;
				p1y = vertexList[faceList[i].i[A]].y;
				p1z = vertexList[faceList[i].i[A]].z;

				p2x = vertexList[faceList[i].i[C]].x;
				p2y = vertexList[faceList[i].i[C]].y;
				p2z	= vertexList[faceList[i].i[C]].z;

				e1x = p1x;
				e1y = p1y;
				e1z = p1z;
				e2x = vertexList[faceList[i].i[B]].x;
				e2y = vertexList[faceList[i].i[B]].y;
				e2z = vertexList[faceList[i].i[B]].z;
				e3x = p2x;
				e3y = p2y;
				e3z = p2z;
			}
			
			fUsed[i]=1;

			for (j=0; j<nFace; j++)
			{
				float p3x,p3y,p3z,p4x,p4y,p4z,te1x,te1y,te1z,te2x,te2y,te2z,te3x,te3y,te3z;
				if (!fUsed[j])
				{
		
					if (!faceList[j].e[AB])
					{
						p3x = vertexList[faceList[j].i[A]].x;
						p3y = vertexList[faceList[j].i[A]].y;
						p3z = vertexList[faceList[j].i[A]].z;
						
						p4x = vertexList[faceList[j].i[B]].x;
						p4y = vertexList[faceList[j].i[B]].y;
						p4z = vertexList[faceList[j].i[B]].z;

						te1x = p4x;
						te1y = p4y;
						te1z = p4z;
						te2x = vertexList[faceList[j].i[C]].x;
						te2y = vertexList[faceList[j].i[C]].y;
						te2z = vertexList[faceList[j].i[C]].z;
						te3x = p3x;
						te3y = p3y;
						te3z = p3z;
					}
					
					if (!faceList[j].e[BC])
					{
						p3x = vertexList[faceList[j].i[B]].x;
						p3y = vertexList[faceList[j].i[B]].y;
						p3z = vertexList[faceList[j].i[B]].z;
						
						p4x = vertexList[faceList[j].i[C]].x;
						p4y = vertexList[faceList[j].i[C]].y;
						p4z = vertexList[faceList[j].i[C]].z;

						te1x = p4x;
						te1y = p4y;
						te1z = p4z;
						te2x = vertexList[faceList[j].i[A]].x;
						te2y = vertexList[faceList[j].i[A]].y;
						te2z = vertexList[faceList[j].i[A]].z;
						te3x = p3x;
						te3y = p3y;
						te3z = p3z;
					}
					
					if (!faceList[j].e[CA])
					{
						p3x = vertexList[faceList[j].i[A]].x;
						p3y = vertexList[faceList[j].i[A]].y;
						p3z = vertexList[faceList[j].i[A]].z;
						
						p4x = vertexList[faceList[j].i[C]].x;
						p4y = vertexList[faceList[j].i[C]].y;
						p4z	= vertexList[faceList[j].i[C]].z;

						te1x = p3x;
						te1y = p3y;
						te1z = p3z;
						te2x = vertexList[faceList[j].i[B]].x;
						te2y = vertexList[faceList[j].i[B]].y;
						te2z = vertexList[faceList[j].i[B]].z;
						te3x = p4x;
						te3y = p4y;
						te3z = p4z;
					}
					
					unsigned long numSame = 0;

					if (fCmp(p1x,p1y,p1z,p3x,p3y,p3z))
						numSame++;

					if (fCmp(p1x,p1y,p1z,p4x,p4y,p4z))
						numSame++;

					if (fCmp(p2x,p2y,p2z,p3x,p3y,p3z))
						numSame++;

					if (fCmp(p2x,p2y,p2z,p4x,p4y,p4z))
						numSame++;

					if (numSame == 2)
					{						
						fUsed[j]=1;

						squareList[nSquare].status = 0;

						switch (faceList[j].mat)
						{
							case NORMAL:
								break;
							case FROG:
								frogs[numFrogs++] = nSquare;
								printf("f");
								break;
							case BABY:
								babys[numBabys++] = nSquare;
								printf("b");
								break;
							case AUTOHOP:
								powers[numPowers++] = nSquare;
								printf("a");
								break;
							case SAFE:
								safes[numSafes++] = nSquare;
								printf("s");
								break;
							case DEADLY:
								printf(",");
								squareList[nSquare].status = 1;
								break;
							case SINK:
								printf("v");
								squareList[nSquare].status = 2;
								break;
							case JUMP:
								printf("^");
								squareList[nSquare].status = 3;
								break;

							case SUPERHOP:
								printf("@");
								squareList[nSquare].status = 4;
								break;
							case JOIN:
								printf("#");
								squareList[nSquare].status = 5;
								break;
						}
						
						squareList[nSquare].p[0] = (p1x+p2x)/2;
						squareList[nSquare].p[1] = (p1y+p2y)/2;
						squareList[nSquare].p[2] = (p1z+p2z)/2;

						squareList[nSquare].ed[0].x = e1x;
						squareList[nSquare].ed[0].y = e1y;
						squareList[nSquare].ed[0].z = e1z;

						squareList[nSquare].ed[1].x = e2x;
						squareList[nSquare].ed[1].y = e2y;
						squareList[nSquare].ed[1].z = e2z;
						
						squareList[nSquare].ed[2].x = e3x;
						squareList[nSquare].ed[2].y = e3y;
						squareList[nSquare].ed[2].z = e3z;
						
						squareList[nSquare].ed[3].x = te2x;
						squareList[nSquare].ed[3].y = te2y;
						squareList[nSquare].ed[3].z = te2z;
						
						squareList[nSquare].adj[0] = -1;
						squareList[nSquare].adj[1] = -1;
						squareList[nSquare].adj[2] = -1;
						squareList[nSquare].adj[3] = -1;

						CalculateNormal (&squareList[nSquare]);
						
						strcpy(squareList[nSquare].parent,faceList[j].parent);
						
						nSquare++;
					}
					if (numSame>2)
						printf ("Error - Face with more than 2 of same Vtx!\n");
				}
			}

		}
	}
	
}



/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: CalculateAdj 

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

void CheckSquare(unsigned long a, unsigned long i,float x1, float y1, float z1, float x2, float y2, float z2)
{
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
			
			if (numSame == 2)
				squareList[i].adj[a] = j;
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
		vect->x /= m;
		vect->y /= m;
		vect->z /= m;
	}
}

void CalculateAdj(void)
{
	unsigned long i;

	//Step thru squares to find adjacent ones.
	for (i=0; i<nSquare; i++)
	{
		if ((i%20) == 0)
			printf (".");

		float x1,x2,y1,y2,z1,z2;

		x1 = squareList[i].ed[0].x;
		y1 = squareList[i].ed[0].y;
		z1 = squareList[i].ed[0].z;

		x2 = squareList[i].ed[1].x;
		y2 = squareList[i].ed[1].y;
		z2 = squareList[i].ed[1].z;
		CheckSquare (0,i,x1,y1,z1,x2,y2,z2);
		
		x1 = squareList[i].ed[1].x;
		y1 = squareList[i].ed[1].y;
		z1 = squareList[i].ed[1].z;

		x2 = squareList[i].ed[2].x;
		y2 = squareList[i].ed[2].y;
		z2 = squareList[i].ed[2].z;
		CheckSquare (1,i,x1,y1,z1,x2,y2,z2);
		
		x1 = squareList[i].ed[2].x;
		y1 = squareList[i].ed[2].y;
		z1 = squareList[i].ed[2].z;

		x2 = squareList[i].ed[3].x;
		y2 = squareList[i].ed[3].y;
		z2 = squareList[i].ed[3].z;
		CheckSquare (2,i,x1,y1,z1,x2,y2,z2);

		x1 = squareList[i].ed[3].x;
		y1 = squareList[i].ed[3].y;
		z1 = squareList[i].ed[3].z;

		x2 = squareList[i].ed[0].x;
		y2 = squareList[i].ed[0].y;
		z2 = squareList[i].ed[0].z;
		CheckSquare (3,i,x1,y1,z1,x2,y2,z2);
	}
	
	// Fill out the normals...
	for (i=0; i<nSquare; i++)
	{
		vtx t[4];
		
		t[0].x = squareList[i].ed[1].x-squareList[i].ed[0].x;
		t[0].y = squareList[i].ed[1].y-squareList[i].ed[0].y;
		t[0].z = squareList[i].ed[1].z-squareList[i].ed[0].z;
		
		t[1].x = squareList[i].ed[2].x-squareList[i].ed[1].x;
		t[1].y = squareList[i].ed[2].y-squareList[i].ed[1].y;
		t[1].z = squareList[i].ed[2].z-squareList[i].ed[1].z;

		t[2].x = squareList[i].ed[3].x-squareList[i].ed[2].x;
		t[2].y = squareList[i].ed[3].y-squareList[i].ed[2].y;
		t[2].z = squareList[i].ed[3].z-squareList[i].ed[2].z;

		t[3].x = squareList[i].ed[0].x-squareList[i].ed[3].x;
		t[3].y = squareList[i].ed[0].y-squareList[i].ed[3].y;
		t[3].z = squareList[i].ed[0].z-squareList[i].ed[3].z;
		Normalise (&t[0]);
		Normalise (&t[1]);
		Normalise (&t[2]);
		Normalise (&t[3]);
		squareList[i].n[0] = t[0];
		squareList[i].n[1] = t[1];
		squareList[i].n[2] = t[2];
		squareList[i].n[3] = t[3];

	}
	
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

	if (strncmp(in,"*MATERIAL_NAME \"normal\"",23) == 0)
	{
		printf("NORM material %i found.\n",cMat);
		mat[cMat] = NORMAL;
	}

	if (strncmp(in,"*MATERIAL_NAME \"froggerstart\"",29) == 0)
	{
		printf("FROG material %i found.\n",cMat);
		mat[cMat] = FROG;
	}

	if (strncmp(in,"*MATERIAL_NAME \"babystart\"",26) == 0)
	{
		printf("BABY material %i found.\n",cMat);
		mat[cMat] = BABY;
	}

	if (strncmp(in,"*MATERIAL_NAME \"autohop\"",24) == 0)
	{
		printf("AUTOHOP material %i found.\n",cMat);
		mat[cMat] = AUTOHOP;
	}

	if (strncmp(in,"*MATERIAL_NAME \"safe\"",21) == 0)
	{
		printf("SAFE material %i found.\n",cMat);
		mat[cMat] = SAFE;
	}

	if (strncmp(in,"*MATERIAL_NAME \"deadly\"",23) == 0)
	{
		printf("DEADLY material %i found.\n",cMat);
		mat[cMat] = DEADLY;
	}

	if (strncmp(in,"*MATERIAL_NAME \"sink\"",21) == 0)
	{
		printf("SINK material %i found.\n",cMat);
		mat[cMat] = SINK;
	}

	if (strncmp(in,"*MATERIAL_NAME \"platjump\"",21) == 0)
	{
		printf("JUMP material %i found.\n",cMat);
		mat[cMat] = JUMP;
	}

	if (strncmp(in,"*MATERIAL_NAME \"superjump\"",26) == 0)
	{
		printf("SUPERJUMP material %i found.\n",cMat);
		mat[cMat] = SUPERHOP;
	}

	if (strncmp(in,"*MATERIAL_NAME \"link\"",21) == 0)
	{
		printf("JOIN material %i found.\n",cMat);
		mat[cMat] = JOIN;
	}

	//////////////////////////////////////////////////////////////////////////////

	if (!inObj)
	{
		lineNo++;
		if ((lineNo % 50) == 0)
		{
			printf(".");
		}

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
	

void ReadData(void)
{
	FILE *in;
	char inStr[255];

	printf ("Reading: %s \n",inF);
	in = fopen (inF,"rt");
	
	fgets(inStr,200,in);
	while (!feof(in))
	{	
		ProcessLine (inStr);
		fgets(inStr,200,in);
	}
		
	fclose (in);
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
			//fprintf (fp,"\nGAMETILE gT_%03i = \n",i);
			fprintf (fp,"  {\t// %u\n", i);
/*			fprintf (fp,"	{%s%03i,%s%03i,%s%03i,%s%03i},\n",
				(squareList[i].adj[0]==-1)?"":"tiles+",
				(squareList[i].adj[0]==-1)?0:squareList[i].adj[0],
				(squareList[i].adj[1]==-1)?"":"tiles+",
				(squareList[i].adj[1]==-1)?0:squareList[i].adj[1],
				(squareList[i].adj[2]==-1)?"":"tiles+",
				(squareList[i].adj[2]==-1)?0:squareList[i].adj[2],
				(squareList[i].adj[3]==-1)?"":"tiles+",
				(squareList[i].adj[3]==-1)?0:squareList[i].adj[3]);
*/			

			fprintf(fp, "	{ %s%d, %s%d, %s%d, %s%d },\n",
				(squareList[i].adj[0]==-1)?"":"tiles+",
				(squareList[i].adj[0]==-1)?0:squareList[i].adj[0],
				(squareList[i].adj[1]==-1)?"":"tiles+",
				(squareList[i].adj[1]==-1)?0:squareList[i].adj[1],
				(squareList[i].adj[2]==-1)?"":"tiles+",
				(squareList[i].adj[2]==-1)?0:squareList[i].adj[2],
				(squareList[i].adj[3]==-1)?"":"tiles+",
				(squareList[i].adj[3]==-1)?0:squareList[i].adj[3]);

//			fprintf (fp,"	%s%03i,\n",(i==(nSquare-1))?"":"tiles+",(i==(nSquare-1))?0:i+1);
			fprintf (fp,"	tiles + %i,\n",(i==(nSquare-1))?0:i+1);
			fprintf (fp,"	0,\n");
			fprintf (fp,"	%lu,\n",squareList[i].status);
			fprintf (fp,"	{%f,%f,%f},\n",squareList[i].p[0],squareList[i].p[2],squareList[i].p[1]);
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

/*	for (i=0; i<nSquare; i++)
	{
		if (squareList[i].parent[0]!=0)
		{
			fprintf (fp,"\nchar pT_name_%03i[] = %s\n",i,squareList[i].parent);
			fprintf (fp,"\nGAMETILE pT_%03i = \n",i);
			fprintf (fp,"{\n",i);
			fprintf (fp,"	{0,0,0,0},\n");
			fprintf (fp,"	%s%03i,\n",(i==(nSquare-1))?"":"&pT_",(i==(nSquare-1))?0:i+1);
			fprintf (fp,"	(unsigned long)&pT_name_%03i,\n",i);
			fprintf (fp,"	{%f,%f,%f},\n",squareList[i].p[0],squareList[i].p[2],squareList[i].p[1]);
			fprintf (fp,"	{%f,%f,%f},\n",squareList[i].vn.x,squareList[i].vn.z,squareList[i].vn.y);
			fprintf (fp,"	{{%f,%f,%f},{%f,%f,%f},{%f,%f,%f},{%f,%f,%f}},\n",
				squareList[i].n[3].x,
				squareList[i].n[3].z,
				squareList[i].n[3].y,
				
				squareList[i].n[2].x,
				squareList[i].n[2].z,
				squareList[i].n[2].y,
				
				squareList[i].n[1].x,
				squareList[i].n[1].z,
				squareList[i].n[1].y,
				
				squareList[i].n[0].x,
				squareList[i].n[0].z,
				squareList[i].n[0].y
				);
			
			fprintf (fp,"};\n",i);
		}
	}*/
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
	if (numFrogs>4)
	{
		printf("WARNING more than 4 frogs detected - Please rectify");
		exit (128);
	}
	
	unsigned long i;

	fprintf (fp,"\nGAMETILE *t_gTStart[4] = {%s%i, %s%i, %s%i, %s%i};\n",
		(frogs[0]==-1)?"":"tiles+",
		(frogs[0]==-1)?0:frogs[0],

		(frogs[1]==-1)?"":"tiles+",
		(frogs[1]==-1)?0:frogs[0],

		(frogs[2]==-1)?"":"tiles+",
		(frogs[2]==-1)?0:frogs[0],


		(frogs[3]==-1)?"":"tiles+",
		(frogs[3]==-1)?0:frogs[0]
		);

	fprintf(fp,"\nunsigned long t_numBabies = %i;\n",numBabys);

	if (pc)
	{
		if (numBabys)
		{
			fprintf(fp,"GAMETILE *t_bTStart[%i] = {",numBabys);		

			for (i=0; i<numBabys; i++)
				fprintf(fp,"tiles+%i,",babys[i]);
			fprintf(fp,"};\n");
		}
		else
			fprintf(fp,"GAMETILE *t_bTStart[1];",numBabys);		
	}
	else
	{
		fprintf(fp,"GAMETILE *t_bTStart[%i] = {",numBabys);	

		for (i=0; i<numBabys; i++)
			fprintf(fp,"tiles+%i, ",babys[i]);
		fprintf(fp,"};\n");
	}

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
	printf ("Writing: %s \n",outF);
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
	printf("World Converter - V1.01 Matthew Cloy - Interactive Studios Ltd \n\n");
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
	strcpy (inF,argv[1]);
	strcpy (outF,argv[2]);
	ReadData();
	printf("\n");
	BuildSquareList();
	printf("\n");
	CalculateAdj();
	printf("\n");
	WriteData();

	return 0;
}