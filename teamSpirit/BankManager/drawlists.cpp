#include <windows.h>
#include <commctrl.h>
#include <wingdi.h>
#include <string.h>
#include <stdio.h>

#include "types.h"
#include "main.h"
#include "drawlists.h"
#include "bitmaps.h"
#include "files.h"
#include "views.h"
#include "crc32.h"
#include "ndos.h"
#include "resource.h"
#include "commctrl.h"
#include "debug.h"
#include <gelf.h>

#include "c:\n64\include\pr\gbi.h"

#define CACHE_SIZE 32





OBJECTINFO *CollisionMeshPresent(OBJECTINFO *obj);

int	numTotalObjects = 0;
char	outputPath[256];
char	outputFileName[256];

INPUT_OBJECT	*allObjects[500];
OBJECTINFO		*allObjectsInfo[500];
OBJECTINFO		*currObjInfo;
INPUT_OBJECT	*currObj;
int				lastUTile = 0;
int				lastVTile = 0;
int				lastTextureID = 0;

TEXTUREDETAIL	currTexDetails;

TextureGroup textureFaces[80];
int numberTextures;

N64vertex vertexCache[CACHE_SIZE];
int cacheUsed=0;
int vertexPos=0;
char objectType;
unsigned char myAlpha;
unsigned char myColouredGouraud;
float	brightnessValue = 1.0;
INPUT_OBJECT	*myObject;

N64vertex bBox[8];

BOOL	outputDrawList = TRUE;
BOOL	collisionMesh = FALSE;

DS_OBJECT_INFO	objectInfo[500];
int				dsCurrObject=0;
int				optimiseKeyframes = TRUE;

short scaleVal = 250;
short scaleSkinnedObject = 0;

void CountChildren(INPUT_OBJECT *obj)
{
	allObjects[numTotalObjects] = obj;
	allObjectsInfo[numTotalObjects] = currObjInfo;

	numTotalObjects++;
	if(obj->children)	
	{
		CountChildren(obj->children);
	}
	if(obj->next)	
	{
		CountChildren(obj->next);
	}
}

int CompareWithCache(FaceInfo *face,int faceVNumber,int cacheVNumber)
{
	if ((short)face->verts[faceVNumber].v[X] != vertexCache[cacheVNumber].x) return 0;
	if ((short)face->verts[faceVNumber].v[Y] != vertexCache[cacheVNumber].y) return 0;
	if ((short)face->verts[faceVNumber].v[Z] != vertexCache[cacheVNumber].z) return 0;
	if (face->vertexNormal[faceVNumber].v[X] != (char)vertexCache[cacheVNumber].nx) return 0;
	if (face->vertexNormal[faceVNumber].v[Y] != (char)vertexCache[cacheVNumber].ny) return 0;
	if (face->vertexNormal[faceVNumber].v[Z] != (char)vertexCache[cacheVNumber].nz) return 0;
	return 1;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CalculateNumTotalObjects()
{
	int x;
	OBJECTINFO	*ptr;

	numTotalObjects = 0;
	
	for(ptr = objectBanks[selectedObjectBank].objectList.head.next;ptr != &objectBanks[selectedObjectBank].objectList.head;ptr = ptr->next)
	{
  		currObjInfo = ptr;
		CountChildren(ptr->object);
		
		if((ptr->object->children) && (ptr->object->children->flags & OBJECT_FLAGS_IA))
			ptr->object->flags |= OBJECT_FLAGS_IA;

	}

	

/*	for(x = 0; x < NumObjects; x++)
	{
		CountChildren(ObjectList[x].object);
	}
*/
}




FILE *fp;
FILE *display,*verts;

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void ExportDrawList(int bankNum)
{
	char buf[256], buf2[256];
	int i, crc, x;//, size;
	char *fname;
	int tempSize;
	char *tempData;

	if(strlen(objectBanks[bankNum].objectOutput) == 0)
		return;

	CalculateNumTotalObjects();


	sprintf(buf, objectBanks[bankNum].filename);
	buf[strlen(buf) - 4] = 0;

	sprintf(buf2, "Export object bank '%s'", buf);
	if(MessageBox(NULL,buf2,"Are you sure?",MB_ICONQUESTION|MB_TASKMODAL|MB_OKCANCEL) == IDCANCEL)
		return;

	if(userFormat != PC)
		sprintf(buf2,"%s\\%s.c2", objectBanks[bankNum].objectOutput, buf);
	else
		sprintf(buf2,"%s\\%s.cpp", objectBanks[bankNum].objectOutput, buf);

	fp=fopen(buf2,"wb");
	if (!fp)
	{
		sprintf(buf,"Unable to create\n%s",buf2);
		MessageBox(NULL,buf,"Error",MB_ICONWARNING|MB_TASKMODAL);
		return;
	}


/*
	fp=fopen(buf,"wt");
	if (!fp)
	{
		sprintf(buf,"Unable to create %s",buf);
		MessageBox(NULL,buf,"Error",MB_ICONWARNING|MB_TASKMODAL);
		return;
	}
*/

	sprintf(buf,"%s%s.dis", outputPath, outputFileName);
	display=fopen("c:\\display","wt+");
	if(display == NULL)
	{
		printf("File error\n");
		exit(1);	
	}
	sprintf(buf,"%s%s.ver", outputPath, outputFileName);
	verts=fopen("c:\\verts","wt+");
	if(verts == NULL)
	{
		printf("File error\n");
		exit(1);	
	}
	dsCurrObject=0;
	

	SetupProgressBar(0, 11, "Exporting object bank");

	WriteHeader(fp);
	StepProgressBar();

	WriteDrawlists(fp);
	StepProgressBar();

	if(userFormat != PC)
	{
		WriteObjectMeshes(fp);
		StepProgressBar();
	}
	
	WriteObjectKeyframes(fp);
	StepProgressBar();

	WriteObjectSpriteDefs(fp);
	StepProgressBar();

	WriteObjectsSkinInfo();
	StepProgressBar();

	if(userFormat == PC)
	{
		WriteObjectMeshes(fp);
		StepProgressBar();
	}

	dsCurrObject=0;
	WriteObjectDefinitions(fp);
	StepProgressBar();

	WriteAnimations(fp);
	StepProgressBar();

	WriteAnimationDef(fp);
	StepProgressBar();

	WriteObjectControllers(fp);
	StepProgressBar();

	PrintTextureList(fp);
	StepProgressBar();

	
	fclose(fp);
	fclose(display);
	fclose(verts);
	
	KillProgressBar();
//open .h file for compiled drawlist output
/*
	sprintf(buf,"%s%s.h", outputPath, outputFileName);
	fp=fopen(buf,"wt");
	if (!fp)
	{
		sprintf(buf,"Unable to create\n%s\\output\\%s",PicyDir,saveFileName);
		MessageBox(NULL,buf,"Error",MB_ICONWARNING|MB_TASKMODAL);
		return;
	}

	WriteHeaderFile(fp);		
	fclose(fp);
*/
/*
	if(deleteSourceFiles == TRUE)
	{
		sprintf(buf, "del %s.c\n", outputFileName);
		system(buf);

		sprintf(buf, "del %s.h\n", outputFileName);
		system(buf);
	}
*/
/*	
	sprintf(buf, "del display\n");
	system(buf);
	sprintf(buf, "del verts\n");
	system(buf);
*/



}

void CreateTextureLists(TextureGroup *texList,INPUT_OBJECT *obj)
{
	int i,j;
	char faceFlag;

	vertexPos=0;
	numberTextures = 0;

	for (i=0;i<obj->mesh->numFaces;i++)
	{
		FindPlace(i,&obj->mesh->faceInfo[i].textureID,obj);
	}
}

void FindPlace(int face,TEXTURE_ID *texID,INPUT_OBJECT *obj)
{
	int i,j;
	TextureGroup *list;

	list = textureFaces;

	if (numberTextures)
	{
		for (j=0;j<numberTextures;j++)
		{
			if (list[j].texture==texID->crc)
			{
//				if(obj->duelTexID)
				{
					if(obj->mesh->faceInfo[face].duelTexID == 0)
					{
						FillFace(&list[j].faceList[list[j].numFaces],face,obj);
						list[j].numFaces++;
						return;
					}
					else if(obj->mesh->faceInfo[face].duelTexID == list[j].texture2)
					{
						FillFace(&list[j].faceList[list[j].numFaces],face,obj);
						list[j].numFaces++;
						return;
					}
				}
/*				else
				{
					FillFace(&list[j].faceList[list[j].numFaces],face,obj);
					list[j].numFaces++;
					return;
				}
*/			}
		}
	}

	list[numberTextures].texture = texID->crc;
//	if(obj->duelTexID)
		list[numberTextures].texture2 = obj->mesh->faceInfo[face].duelTexID;
//	else
//		list[numberTextures].texture2 = 0;
	list[numberTextures].texID = texID;
	FillFace(&list[numberTextures].faceList[0],face,obj);
	list[numberTextures].numFaces = 1;
	numberTextures++;
}

void FillFace(FaceInfo *face,int faceNo,INPUT_OBJECT *obj)
{
	BYTEVECTOR *vect;
	int vert0,vert1,vert2,i;

	face->faceNo = faceNo;

	vert0 = obj->mesh->faceInfo[faceNo].faceIndex.v[X];
	vert1 = obj->mesh->faceInfo[faceNo].faceIndex.v[Y];
	vert2 = obj->mesh->faceInfo[faceNo].faceIndex.v[Z];

	face->verts[0].v[X] = obj->mesh->vertices[vert0].v[X];
	face->verts[0].v[Y] = obj->mesh->vertices[vert0].v[Y];
	face->verts[0].v[Z] = obj->mesh->vertices[vert0].v[Z];

	face->verts[1].v[X] = obj->mesh->vertices[vert1].v[X];
	face->verts[1].v[Y] = obj->mesh->vertices[vert1].v[Y];
	face->verts[1].v[Z] = obj->mesh->vertices[vert1].v[Z];

	face->verts[2].v[X] = obj->mesh->vertices[vert2].v[X];
	face->verts[2].v[Y] = obj->mesh->vertices[vert2].v[Y];
	face->verts[2].v[Z] = obj->mesh->vertices[vert2].v[Z];

	for (i=0;i<3;i++)
	{
		if (face->verts[i].v[X] < bBox[0].x) bBox[0].x = (short)face->verts[i].v[X];
		if (face->verts[i].v[X] < bBox[1].x) bBox[1].x = (short)face->verts[i].v[X];
		if (face->verts[i].v[X] > bBox[2].x) bBox[2].x = (short)face->verts[i].v[X];
		if (face->verts[i].v[X] > bBox[3].x) bBox[3].x = (short)face->verts[i].v[X];

		if (face->verts[i].v[Y] < bBox[0].y) bBox[0].y = (short)face->verts[i].v[Y];
		if (face->verts[i].v[Y] < bBox[1].y) bBox[1].y = (short)face->verts[i].v[Y];
		if (face->verts[i].v[Y] < bBox[2].y) bBox[2].y = (short)face->verts[i].v[Y];
		if (face->verts[i].v[Y] < bBox[3].y) bBox[3].y = (short)face->verts[i].v[Y];

		if (face->verts[i].v[Z] < bBox[0].z) bBox[0].z = (short)face->verts[i].v[Z];
		if (face->verts[i].v[Z] > bBox[1].z) bBox[1].z = (short)face->verts[i].v[Z];
		if (face->verts[i].v[Z] > bBox[2].z) bBox[2].z = (short)face->verts[i].v[Z];
		if (face->verts[i].v[Z] < bBox[3].z) bBox[3].z = (short)face->verts[i].v[Z];

		if (face->verts[i].v[X] < bBox[4].x) bBox[4].x = (short)face->verts[i].v[X];
		if (face->verts[i].v[X] < bBox[5].x) bBox[5].x = (short)face->verts[i].v[X];
		if (face->verts[i].v[X] > bBox[6].x) bBox[6].x = (short)face->verts[i].v[X];
		if (face->verts[i].v[X] > bBox[7].x) bBox[7].x = (short)face->verts[i].v[X];

		if (face->verts[i].v[Y] > bBox[4].y) bBox[4].y = (short)face->verts[i].v[Y];
		if (face->verts[i].v[Y] > bBox[5].y) bBox[5].y = (short)face->verts[i].v[Y];
		if (face->verts[i].v[Y] > bBox[6].y) bBox[6].y = (short)face->verts[i].v[Y];
		if (face->verts[i].v[Y] > bBox[7].y) bBox[7].y = (short)face->verts[i].v[Y];

		if (face->verts[i].v[Z] < bBox[4].z) bBox[4].z = (short)face->verts[i].v[Z];
		if (face->verts[i].v[Z] > bBox[5].z) bBox[5].z = (short)face->verts[i].v[Z];
		if (face->verts[i].v[Z] > bBox[6].z) bBox[6].z = (short)face->verts[i].v[Z];
		if (face->verts[i].v[Z] < bBox[7].z) bBox[7].z = (short)face->verts[i].v[Z];
	}

	if (objectType==1)
	{
		if(obj->flags & OBJECT_FLAGS_PRELIT)
		{
			face->vertexNormal[0].v[X] = obj->mesh->faceInfo[faceNo].gouraudColour[0].r;
			face->vertexNormal[0].v[Y] = obj->mesh->faceInfo[faceNo].gouraudColour[0].g;
			face->vertexNormal[0].v[Z] = obj->mesh->faceInfo[faceNo].gouraudColour[0].b;

			face->vertexNormal[1].v[X] = obj->mesh->faceInfo[faceNo].gouraudColour[1].r;
			face->vertexNormal[1].v[Y] = obj->mesh->faceInfo[faceNo].gouraudColour[1].g;
			face->vertexNormal[1].v[Z] = obj->mesh->faceInfo[faceNo].gouraudColour[1].b;

			face->vertexNormal[2].v[X] = obj->mesh->faceInfo[faceNo].gouraudColour[2].r;
			face->vertexNormal[2].v[Y] = obj->mesh->faceInfo[faceNo].gouraudColour[2].g;
			face->vertexNormal[2].v[Z] = obj->mesh->faceInfo[faceNo].gouraudColour[2].b;

		}
		else
		{
			face->vertexNormal[0].v[X] = obj->mesh->vertexNormals[vert0].v[X];
			face->vertexNormal[0].v[Y] = obj->mesh->vertexNormals[vert0].v[Y];
			face->vertexNormal[0].v[Z] = obj->mesh->vertexNormals[vert0].v[Z];

			face->vertexNormal[1].v[X] = obj->mesh->vertexNormals[vert1].v[X];
			face->vertexNormal[1].v[Y] = obj->mesh->vertexNormals[vert1].v[Y];
			face->vertexNormal[1].v[Z] = obj->mesh->vertexNormals[vert1].v[Z];

			face->vertexNormal[2].v[X] = obj->mesh->vertexNormals[vert2].v[X];
			face->vertexNormal[2].v[Y] = obj->mesh->vertexNormals[vert2].v[Y];
			face->vertexNormal[2].v[Z] = obj->mesh->vertexNormals[vert2].v[Z];

		}
	}
	else
	{
		face->vertexNormal[0].v[X] = obj->mesh->faceInfo[faceNo].faceNormal.v[X];
		face->vertexNormal[0].v[Y] = obj->mesh->faceInfo[faceNo].faceNormal.v[Y];
		face->vertexNormal[0].v[Z] = obj->mesh->faceInfo[faceNo].faceNormal.v[Z];

		face->vertexNormal[1].v[X] = obj->mesh->faceInfo[faceNo].faceNormal.v[X];
		face->vertexNormal[1].v[Y] = obj->mesh->faceInfo[faceNo].faceNormal.v[Y];
		face->vertexNormal[1].v[Z] = obj->mesh->faceInfo[faceNo].faceNormal.v[Z];

		face->vertexNormal[2].v[X] = obj->mesh->faceInfo[faceNo].faceNormal.v[X];
		face->vertexNormal[2].v[Y] = obj->mesh->faceInfo[faceNo].faceNormal.v[Y];
		face->vertexNormal[2].v[Z] = obj->mesh->faceInfo[faceNo].faceNormal.v[Z];
	}
/*
	if (myColouredGouraud)
	{
		vect = obj->mesh->vertexNormals;
		vect += face->faceNo * 3;
		face->vertexNormal[0].v[X] = vect->v[X];
		face->vertexNormal[0].v[Y] = vect->v[Y];
		face->vertexNormal[0].v[Z] = vect->v[Z];
		vect ++;
		face->vertexNormal[1].v[X] = vect->v[X];
		face->vertexNormal[1].v[Y] = vect->v[Y];
		face->vertexNormal[1].v[Z] = vect->v[Z];
		vect ++;
		face->vertexNormal[2].v[X] = vect->v[X];
		face->vertexNormal[2].v[Y] = vect->v[Y];
		face->vertexNormal[2].v[Z] = vect->v[Z];
	}
*/
	face->textureCoords[0].v[X] = obj->mesh->faceInfo[faceNo].faceTC[0].v[X];
	face->textureCoords[0].v[Y] = obj->mesh->faceInfo[faceNo].faceTC[0].v[Y];
	face->textureCoords[1].v[X] = obj->mesh->faceInfo[faceNo].faceTC[1].v[X];
	face->textureCoords[1].v[Y] = obj->mesh->faceInfo[faceNo].faceTC[1].v[Y];
	face->textureCoords[2].v[X] = obj->mesh->faceInfo[faceNo].faceTC[2].v[X];
	face->textureCoords[2].v[Y] = obj->mesh->faceInfo[faceNo].faceTC[2].v[Y];

	face->faceNormal = obj->mesh->faceInfo[faceNo].faceNormal;
	face->flags = obj->mesh->faceInfo[faceNo].faceFlags;
}

void PrintTextureList(FILE *fp)
{
	int i,j,k;
	return;

	fprintf(fp,"----------------------------New List------------------------------\n");

	for (i=0;i<numberTextures;i++)
	{
		fprintf(fp,"----------------Texture %x-----------------\n\n",textureFaces[i].texture);
		for (j=0;j<textureFaces[i].numFaces;j++)
		{
			fprintf(fp,"Face %d\n",textureFaces[i].faceList[j].faceNo);

			fprintf(fp,"Vertices\n");
			for (k=0;k<3;k++)
			{
				fprintf(fp,"X %d Y %d Z %d\n",(short)textureFaces[i].faceList[j].verts[k].v[X],
				(short)textureFaces[i].faceList[j].verts[k].v[Y],
				(short)textureFaces[i].faceList[j].verts[k].v[Z]);
			}
			fprintf(fp,"Vertex Normals\n");
			for (k=0;k<3;k++)
			{
				fprintf(fp,"X %d Y %d Z %d\n",textureFaces[i].faceList[j].vertexNormal[k].v[X],
				textureFaces[i].faceList[j].vertexNormal[k].v[Y],
				textureFaces[i].faceList[j].vertexNormal[k].v[Z]);
			}
			fprintf(fp,"Texture Coords\n");
			for (k=0;k<3;k++)
			{
				fprintf(fp,"X %d Y %d\n",textureFaces[i].faceList[j].textureCoords[k].v[X],
				textureFaces[i].faceList[j].textureCoords[k].v[Y]);
			}
			fprintf(fp,"Face Normal\n");
			fprintf(fp,"X %d Y %d Z %d\n",textureFaces[i].faceList[j].faceNormal.v[X],
				textureFaces[i].faceList[j].faceNormal.v[Y],
				textureFaces[i].faceList[j].faceNormal.v[Z]);
			fprintf(fp,"Flag-%d\n\n",textureFaces[i].faceList[j].flags);
		}
	}
}

char used[MAXPOLYSPERTEXTURE];
int order[MAXPOLYSPERTEXTURE];

void AddCache(FaceInfo *face,int vert)
{
	BYTEVECTOR	*vect;


	if (cacheUsed>=CACHE_SIZE)
	{
		fprintf(fp,"Cache Overflow!\n");
		return;
	}
	vertexCache[cacheUsed].x = (short)face->verts[vert].v[X];
	vertexCache[cacheUsed].y = (short)face->verts[vert].v[Y];
	vertexCache[cacheUsed].z = (short)face->verts[vert].v[Z];
	vertexCache[cacheUsed].tx = (short)face->textureCoords[vert].v[X];
	vertexCache[cacheUsed].ty = (short)face->textureCoords[vert].v[Y];

	//must output gouraud colours if the object says so	
/*	if(myColouredGouraud)
	{
		vect = myObject->mesh->vertexNormals;
		vect += face->faceNo * 3;
		vect += vert;


		vertexCache[cacheUsed].nx = vect->v[X];
		vertexCache[cacheUsed].ny = vect->v[Y];
		vertexCache[cacheUsed].nz = vect->v[Z];
	}
	else
*/	{
		vertexCache[cacheUsed].nx = (short)face->vertexNormal[vert].v[X];
		vertexCache[cacheUsed].ny = (short)face->vertexNormal[vert].v[Y];
		vertexCache[cacheUsed].nz = (short)face->vertexNormal[vert].v[Z];
	}

	
	if(myObject->flags & OBJECT_FLAGS_TEXTURE_BLEND)
	{
		if(myObject->mesh->faceInfo[face->faceNo].duelTexID == 0)
			vertexCache[cacheUsed].alpha = 0;//myObject->vAlphas[face->faceNo].v[vert];
		else		
			vertexCache[cacheUsed].alpha = myObject->mesh->faceInfo[face->faceNo].vAlpha.v[vert];
	}
	else
		vertexCache[cacheUsed].alpha = myAlpha;
	
	cacheUsed++;
}

void CalcBestOrder(TextureGroup *polys,INPUT_OBJECT *obj)
{
	int i,j,k;
	int orderPos=0;
	int cur,search;
	char match=3;
	char similar;
	int polysDone;
	int noMatches;
	int curVert;
	BYTEVECTOR cnormal;
	char textured;

	for (i=0;i<MAXPOLYSPERTEXTURE;i++) used[i] = 0;
	cacheUsed=0;


	PrintTextureInfo(display, polys);//->texID);
	
	if (polys->texID->crc)
		textured=1;
	else
		textured=0;


	cur = 0;
	polysDone=0;
	order[orderPos++] = 0;
	if ((polys->faceList[0].flags&QUAD)==0)
	{
		used[polys->faceList[0].faceNo] = 1;
		AddPolyToCache(&polys->faceList[0]);
		polysDone++;
	}
	else
	{
		used[polys->faceList[0].faceNo] = 1;
		used[polys->faceList[1].faceNo] = 1;
		AddQuadToCache(&polys->faceList[0],&polys->faceList[1]);
		polysDone+=2;
	}

	for (;;)
	{
		noMatches=0;
		for (search=0;search<polys->numFaces;search++)
		{
			if (used[polys->faceList[search].faceNo]==0)
			{
				// Compare poly verts
				similar = ComparePolyWithCache(&polys->faceList[search]);
				if (similar>=match)
				{
					if ((polys->faceList[search].flags&QUAD)==0)
					{
						if (AddPolyToCache(&polys->faceList[search]) == 1)
						{
							order[orderPos++] = search;
							used[polys->faceList[search].faceNo] = 1;
							polysDone++;
							noMatches=1;
							if (polysDone==polys->numFaces) break;
						}
						else
						{
							for (i=0;i<cacheUsed;i++)
							{
								if(obj->flags & OBJECT_FLAGS_PRELIT)
								{
									if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
									{
										fprintf(verts, "  {  %d, %d, %d, 0, %d, %d, %d, %d, %d, 0x%X },\n",
											vertexCache[i].x,vertexCache[i].y,vertexCache[i].z,
											vertexCache[i].tx,vertexCache[i].ty,(uchar)vertexCache[i].nx,(uchar)vertexCache[i].ny,
											(uchar)vertexCache[i].nz,vertexCache[i].alpha);
									}
									// ENDIF

									if(obj->drawListVtxList)
										obj->drawListVtxList[obj->numVtx++] = vertexCache[i];

								}
								else
								{
									if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
									{
										fprintf(verts, "  {  %d, %d, %d, 0, %d, %d, %d, %d, %d, 0x%X },\n",
											vertexCache[i].x,vertexCache[i].y,vertexCache[i].z,
											vertexCache[i].tx,vertexCache[i].ty,vertexCache[i].nx,vertexCache[i].ny,
											vertexCache[i].nz,vertexCache[i].alpha);
									}
									// ENDIf
									if(obj->drawListVtxList)
										obj->drawListVtxList[obj->numVtx++] = vertexCache[i];
								}
							}
							if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
							{
								if(currObjInfo->skinned)
									fprintf(display, "  gsSPVertex(SKINNED_SEG + (16 * %d), %d, 0),\n", vertexPos,cacheUsed);	
								else
									fprintf(display, "  gsSPVertex(&(%s_vtxa[%d]), %d, 0),\n", obj->name, vertexPos,cacheUsed);
							}
							// ENDIF

//							fprintf(display, "  gsSPVertex(&(%s_vtxa[%d]), %d, 0),\n", obj->name, vertexPos,cacheUsed);	
							objectInfo[dsCurrObject].numDrawList++;
							curVert=0;
							for (i=0;i<orderPos;i++)
							{
								if ((polys->faceList[order[i]].flags&QUAD)==0)
								{
									WritePolyInstruction(&polys->faceList[order[i]],curVert);
									curVert+=3;
								}
								else
								{
									WritePolyInstructionQuad(&polys->faceList[order[i]],&polys->faceList[order[i]+1],curVert,curVert+1);
									curVert+=4;
								}
							}
							orderPos=0;
							vertexPos += cacheUsed;
							cacheUsed = 0;
							AddPolyToCache(&polys->faceList[search]);
							order[orderPos++] = search;
							used[polys->faceList[search].faceNo] = 1;
							polysDone++;
							noMatches=1;
							if (polysDone>=polys->numFaces) break;
						}
					}
					else
					{
						if (AddQuadToCache(&polys->faceList[search],&polys->faceList[search+1]) == 1)
						{
							order[orderPos++] = search;
							used[polys->faceList[search].faceNo] = 1;
							used[polys->faceList[search+1].faceNo] = 1;
							polysDone+=2;
							noMatches=1;
							if (polysDone>=polys->numFaces) break;
						}
						else
						{
							for (i=0;i<cacheUsed;i++)
							{
								if(obj->flags & OBJECT_FLAGS_PRELIT)
								{
									if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
									{
										fprintf(verts, "  {  %d, %d, %d, 0, %d, %d, %d, %d, %d, 0x%X },\n",
											vertexCache[i].x,vertexCache[i].y,vertexCache[i].z,
											vertexCache[i].tx,vertexCache[i].ty,(uchar)vertexCache[i].nx,(uchar)vertexCache[i].ny,
											(uchar)vertexCache[i].nz,vertexCache[i].alpha);
									}
									// ENDIF

									if(obj->drawListVtxList)
										obj->drawListVtxList[obj->numVtx++] = vertexCache[i];
								}
								else
								{
									if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
									{
										fprintf(verts, "  {  %d, %d, %d, 0, %d, %d, %d, %d, %d, 0x%X },\n",
											vertexCache[i].x,vertexCache[i].y,vertexCache[i].z,
											vertexCache[i].tx,vertexCache[i].ty,vertexCache[i].nx,vertexCache[i].ny,
											vertexCache[i].nz,vertexCache[i].alpha);
									}
									// ENDIf

									if(obj->drawListVtxList)
										obj->drawListVtxList[obj->numVtx++] = vertexCache[i];
								}
							}
							if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
							{
								if(currObjInfo->skinned)
									fprintf(display, "  gsSPVertex(SKINNED_SEG + (16 * %d), %d, 0),\n", vertexPos,cacheUsed);	
								else
									fprintf(display, "  gsSPVertex(&(%s_vtxa[%d]), %d, 0),\n", obj->name, vertexPos,cacheUsed);	
							}
							// ENDIF
//							fprintf(display, "  gsSPVertex(&(%s_vtxa[%d]), %d, 0),\n", obj->name, vertexPos,cacheUsed);	
							objectInfo[dsCurrObject].numDrawList++;

							curVert=0;
							for (i=0;i<orderPos;i++)
							{
								if ((polys->faceList[order[i]].flags&QUAD)==0)
								{
									WritePolyInstruction(&polys->faceList[order[i]],curVert);
									curVert+=3;
								}
								else
								{
									WritePolyInstructionQuad(&polys->faceList[order[i]],&polys->faceList[order[i]+1],curVert,curVert+1);
									curVert+=4;
								}
							}
							orderPos=0;
							vertexPos += cacheUsed;
							cacheUsed = 0;
							AddQuadToCache(&polys->faceList[search],&polys->faceList[search+1]);
							order[orderPos++] = search;
							used[polys->faceList[search].faceNo] = 1;
							used[polys->faceList[search+1].faceNo] = 1;
							polysDone+=2;
							noMatches=1;
							if (polysDone>=polys->numFaces) break;
						}

					}
				}
				else
				{
					if ((polys->faceList[search].flags&QUAD)!=0) search++;
				}
			}
		}
		if (polysDone>=polys->numFaces) break;
		if (noMatches==0) 
		{
			if (match>0) match--;
		}
	}
	if (cacheUsed!=0)
	{
		for (i=0;i<cacheUsed;i++)
		{
			if(obj->flags & OBJECT_FLAGS_PRELIT)
			{
				if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
				{
					fprintf(verts, "  {  %d, %d, %d, 0, %d, %d, %d, %d, %d, 0x%X },\n",
						vertexCache[i].x,vertexCache[i].y,vertexCache[i].z,
						vertexCache[i].tx,vertexCache[i].ty,(uchar)vertexCache[i].nx,(uchar)vertexCache[i].ny,
						(uchar)vertexCache[i].nz,vertexCache[i].alpha);
				}
				// ENDIF

				if(obj->drawListVtxList)
					obj->drawListVtxList[obj->numVtx++] = vertexCache[i];
			}
			else
			{
				if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
				{
					fprintf(verts, "  {  %d, %d, %d, 0, %d, %d, %d, %d, %d, 0x%X },\n",
						vertexCache[i].x,vertexCache[i].y,vertexCache[i].z,
						vertexCache[i].tx,vertexCache[i].ty,vertexCache[i].nx,vertexCache[i].ny,
						vertexCache[i].nz,vertexCache[i].alpha);
				}
				// ENDIF
				if(obj->drawListVtxList)
					obj->drawListVtxList[obj->numVtx++] = vertexCache[i];
			}
		}
		if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
		{
			if(currObjInfo->skinned)
				fprintf(display, "  gsSPVertex(SKINNED_SEG + (16 * %d), %d, 0),\n", vertexPos,cacheUsed);	
			else
				fprintf(display, "  gsSPVertex(&(%s_vtxa[%d]), %d, 0),\n", obj->name, vertexPos,cacheUsed);	
		}
		// ENDIF
//		fprintf(display, "  gsSPVertex(&(%s_vtxa[%d]), %d, 0),\n", obj->name, vertexPos,cacheUsed);	
		objectInfo[dsCurrObject].numDrawList++;

		vertexPos += cacheUsed;
	}

	curVert=0;
	if (orderPos)
	{
		for (i=0;i<orderPos;i++)
		{
			if ((polys->faceList[order[i]].flags&QUAD)==0)
			{
				WritePolyInstruction(&polys->faceList[order[i]],curVert);
				curVert+=3;
			}
			else
			{
				WritePolyInstructionQuad(&polys->faceList[order[i]],&polys->faceList[order[i]+1],curVert,curVert+1);
				curVert+=4;
			}
		}
	}
}



int CheckForMatchingVerts(FaceInfo *poly)
{
	int i,j;

	for (i=0;i<3;i++)
	{
		for (j=0;j<3;j++)
		{
			if (j==i) continue;
			if (((short)poly->verts[i].v[X]==(short)poly->verts[j].v[X])
				&&((short)poly->verts[i].v[Y]==(short)poly->verts[j].v[Y])
				&&((short)poly->verts[i].v[Z]==(short)poly->verts[j].v[Z]))
			{
//				fprintf(fp,"Matching Verts %f %f %f \n", poly->verts[i].v[X], poly->verts[i].v[Y], poly->verts[i].v[Z]);
//				fprintf(fp,"Matching Verts %f %f %f \n", poly->verts[j].v[X], poly->verts[j].v[Y], poly->verts[j].v[Z]);
				return 1;
			}
		}
	}
	return 0;
}

int ComparePolyWithCache(FaceInfo *poly1)
{
	int i,j;
	int matches=0;
	int dest[3];
	int used=0;

	for (i=0;i<3;i++) dest[i]=0;

	for (i=0;i<cacheUsed;i++)
	{
		if (used==3) break;
		for (j=0;j<3;j++)
		{
			if (used>0)
			{
				if (dest[0]==i) continue;
				if (used>1)
				{
					if (dest[1]==i) continue;
					if (used>2)
					{
						if (dest[2]==i) continue;
					}
				}
			}

			if (objectType==1)
			{
				if (CompareWithCache(poly1,j,i))
/*				if (((short)poly1->verts[j].v[X]==vertexCache[i].x)
					&&((short)poly1->verts[j].v[Y]==vertexCache[i].y)
					&&((short)poly1->verts[j].v[Z]==vertexCache[i].z))
*/				{
					matches++;
					dest[used++] = i;
					break;
				}
			}
		}
	}
	return matches;
}

char AddPolyToCache(FaceInfo *poly1)
{
	int i,j;
	int dest[3];
	int src[3];
	int used=0;
	int newVerts=0;
	int verts[3];

	for (i=0;i<3;i++) dest[i]=0;
	for (i=0;i<3;i++) src[i]=0;

	for (i=0;i<cacheUsed;i++)
	{
		if (used==3) break;
		for (j=0;j<3;j++)
		{
			if (used>0)
			{
				if (dest[0]==i) continue;
				if (used>1)
				{
					if (dest[1]==i) continue;
					if (used>2)
					{
						if (dest[2]==i) continue;
					}
				}
			}
			if (objectType==1)
			{
				if (CompareWithCache(poly1,j,i))
/*				if (((short)poly1->verts[j].v[X]==vertexCache[i].x)
					&&((short)poly1->verts[j].v[Y]==vertexCache[i].y)
					&&((short)poly1->verts[j].v[Z]==vertexCache[i].z))
*/				{
					dest[used++] = i;
					src[j] = 1;
					verts[j] = i;
					break;
				}
			}
		}
	}
	
	for (j=0;j<3;j++)
	{
		if (src[j]==0) newVerts++;
	}
	if ((cacheUsed+newVerts) >= (CACHE_SIZE))
	{
		return 0;
	}

	for (i=0;i<3;i++)
	{
		if (src[i]==0)
		{
			verts[i] = cacheUsed;
			AddCache(poly1,i);
		}
	}
	//fprintf(display, "  gsSP1Triangle(%d, %d, %d, 0),\n", verts[0],verts[1],verts[2]);
	return 1;														   
}

char AddQuadToCache(FaceInfo *poly1,FaceInfo *poly2)
{
	int i,j;
	int dest[4];
	int src[4];
	int used=0;
	int newVerts=0;
	int verts[4];

	for (i=0;i<4;i++) dest[i]=0;
	for (i=0;i<4;i++) src[i]=0;

/*	for (i=0;i<3;i++) fprintf(fp,"poly1 vert %d - %d,%d,%d\n",i,(short)poly1->verts[i].v[X],
		(short)poly1->verts[i].v[Y],(short)poly1->verts[i].v[Z]);
	for (i=0;i<3;i++) fprintf(fp,"poly2 vert %d - %d,%d,%d\n",i,(short)poly2->verts[i].v[X],
		(short)poly2->verts[i].v[Y],(short)poly2->verts[i].v[Z]);
*/
	for (i=0;i<cacheUsed;i++)
	{
		if (used==4) break;
		for (j=0;j<4;j++)
		{
			if (used>0)
			{
				if (dest[0]==i) continue;
				if (used>1)
				{
					if (dest[1]==i) continue;
					if (used>2)
					{
						if (dest[2]==i) continue;
						if (used>3)
						{
							if (dest[3]==i) continue;
						}
					}
				}
			}
			if (objectType==1)
			{
				if (j==3)
				{
					if (CompareWithCache(poly2,2,i))
/*					if (((short)poly2->verts[2].v[X]==vertexCache[i].x)
						&&((short)poly2->verts[2].v[Y]==vertexCache[i].y)
						&&((short)poly2->verts[2].v[Z]==vertexCache[i].z))
*/					{
						dest[used++] = i;
						src[j] = 1;
						verts[j] = i;
						break;
					}
				}
				else
				{
					if (CompareWithCache(poly1,j,i))
/*					if (((short)poly1->verts[j].v[X]==vertexCache[i].x)
						&&((short)poly1->verts[j].v[Y]==vertexCache[i].y)
						&&((short)poly1->verts[j].v[Z]==vertexCache[i].z))
*/					{
						dest[used++] = i;
						src[j] = 1;
						verts[j] = i;
						break;
					}
				}
			}
		}
	}
	
	for (j=0;j<4;j++)
	{
		if (src[j]==0) newVerts++;
	}

	if ((cacheUsed+newVerts) >= (CACHE_SIZE))
	{
		return 0;
	}

	for (i=0;i<4;i++)
	{
		if (src[i]==0)
		{
			verts[i] = cacheUsed;
			if (i==3) AddCache(poly2,2);
			else AddCache(poly1,i);
		}
	}
	//fprintf(display, "  gsSP1Triangle(%d, %d, %d, 0),\n", verts[0],verts[1],verts[2]);
	return 1;														   
}

char WritePolyInstructionQuad(FaceInfo *poly1,FaceInfo *poly2,int vertex,int vertex2)
{
	int i,j;
	int dest[4];
	int src[4];
	int used=0;
	int newVerts=0;
	int verts[4];
	int newTex[4];
	int changeUTile = 0, changeVTile = 0;
	int cmt, cms;
	unsigned int texVal[4];

	for (i=0;i<4;i++) dest[i]=0;
	for (i=0;i<4;i++) src[i]=0;
	for (i=0;i<4;i++) newTex[i]=0;

	for (i=0;i<cacheUsed;i++)
	{
		if (used==4) break;
		for (j=0;j<4;j++)
		{
			if (used>0)
			{
				if (dest[0]==i) continue;
				if (used>1)
				{
					if (dest[1]==i) continue;
					if (used>2)
					{
						if (dest[2]==i) continue;
						if (used>3)
						{
							if (dest[3]==i) continue;
						}
					}
				}
			}
			if (objectType==1)
			{
				if (j==3)
				{
					if (CompareWithCache(poly2,2,i))
					{
						dest[used++] = i;
						src[j] = 1;
						verts[j] = i;
						if (((short)poly2->textureCoords[2].v[X]!=vertexCache[i].tx)
							||((short)poly2->textureCoords[2].v[Y]!=vertexCache[i].ty))
						{
							newTex[j] = 1;
							texVal[j] = ((((unsigned int)poly2->textureCoords[2].v[X])/texscalex) << 16)|
								(((unsigned int)poly2->textureCoords[2].v[Y])/texscaley);
							vertexCache[i].tx = poly2->textureCoords[2].v[X];
							vertexCache[i].ty = poly2->textureCoords[2].v[Y];
						}

						break;
					}
				}
				else
				{
					if (CompareWithCache(poly1,j,i))
					{
						dest[used++] = i;
						src[j] = 1;
						verts[j] = i;
						if (((short)poly1->textureCoords[j].v[X]!=vertexCache[i].tx)
							||((short)poly1->textureCoords[j].v[Y]!=vertexCache[i].ty))
						{
							newTex[j] = 1;
							texVal[j] = ((((unsigned int)poly1->textureCoords[j].v[X])/texscalex) << 16)|
								(((unsigned int)poly1->textureCoords[j].v[Y])/texscaley);
							vertexCache[i].tx = poly1->textureCoords[j].v[X];
							vertexCache[i].ty = poly1->textureCoords[j].v[Y];
						}

						break;
					}
				}
			}
		}
	}

	if((currObj->flags & OBJECT_FLAGS_FACECOL) == 0)
	{
		for (i=0;i<4;i++)
		{
			if (newTex[i]==1)
			{
				if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
				{
					if ( !( currObj->flags & OBJECT_FLAGS_CHANGEDRAWLIST ) )
					{
						fprintf(display, "  gsSPModifyVertex(%d,G_MWO_POINT_ST,0x%X),\n",verts[i],texVal[i]);
						objectInfo[dsCurrObject].numDrawList++;
					}
					// ENDIF
				}
				// ENDIF
			}
		}

		//determine whether the tiling flags set in the current texture need to be changed
		if (lastUTile==0)
		{
			if (currObj->mesh->faceInfo[poly1->faceNo].faceFlags & U_TILE)
			{
				lastUTile = 1;
				changeUTile = 1;
			}
		}
		else
		{
			if (!(currObj->mesh->faceInfo[poly1->faceNo].faceFlags & U_TILE))
			{
				lastUTile = 0;
				changeUTile = 1;
			}
		}
		if (lastVTile==0)
		{
			if (currObj->mesh->faceInfo[poly1->faceNo].faceFlags & V_TILE)
			{
				lastVTile = 1;
				changeVTile = 1;
			}
		}
		else
		{
			if (!(currObj->mesh->faceInfo[poly1->faceNo].faceFlags & V_TILE))
			{
				lastVTile = 0;
				changeVTile = 1;
			}
		}
		
		if((changeUTile) || (changeVTile))
		{
			char format[16];
			char pixSize[16];
			int line;

			switch(currTexDetails.format)
			{
				case G_IM_FMT_RGBA:	sprintf(format, "G_IM_FMT_RGBA");
									break;
				case G_IM_FMT_IA:	sprintf(format, "G_IM_FMT_IA");
									break;
				default:			sprintf(format, "ARSE");
									break;

			}

			if(currObj->flags & OBJECT_FLAGS_IA)
				sprintf(format, "G_IM_FMT_IA");

			switch(currTexDetails.colDepth)
			{
				case G_IM_SIZ_4b:	sprintf(pixSize, "G_IM_SIZ_4b");
									line = 4;
									break;
				case G_IM_SIZ_8b:	sprintf(pixSize, "G_IM_SIZ_8b");
									line = 8;
									break;
				case G_IM_SIZ_16b:	sprintf(pixSize, "G_IM_SIZ_16b");
									line = 16;
									break;
				default:			sprintf(pixSize, "ARSE");
									break;
			}

			if(currObj->flags & OBJECT_FLAGS_IA)
			{
				sprintf(pixSize, "G_IM_SIZ_16b");
				line = 16;
			}


			if (currTexDetails.texID)
			{
				line *= currTexDetails.texID->sx;
				line /= 64;
			}
			else
			{
				line *= 16;
				line /= 64;
			}

			if (!line) line=1;

			if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
			{
				fprintf(display, "  gsDPTileSync(),\n");
				objectInfo[dsCurrObject].numDrawList++;
			}
			// ENDIF

			if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
			{
				if ( !( currObj->flags & OBJECT_FLAGS_CHANGEDRAWLIST ) )
				{
					fprintf(display, "  gsDPSetTile(%s, %s, %d, 0, 0, 0, %s, %d, G_TX_NOLOD, %s, %d, G_TX_NOLOD),\n",
								format, pixSize, (int)line,
								(lastVTile ==1) ? "G_TX_WRAP" : "G_TX_CLAMP",
								currTexDetails.scx, 
								(lastUTile ==1) ? "G_TX_WRAP" : "G_TX_CLAMP",
								currTexDetails.scy);
					objectInfo[dsCurrObject].numDrawList++;
				}
				// ENDIF
			}
			// ENDIF
		}
	}

	if (objectType==1)
	{
		if(userFormat != PC)
			fprintf(display, "  gsSP2Triangles(%d, %d, %d, 0, %d, %d ,%d, 0),\n", verts[0],verts[1],verts[2],
									verts[0],verts[2],verts[3]);
		objectInfo[dsCurrObject].numDrawList++;
	}
	else
	{
		if(userFormat != PC)
			fprintf(display, "  gsSP2Triangles(%d, %d, %d, 0, %d, %d, %d, 0),\n", vertex,vertex+1,vertex+2,
							vertex,vertex+2,vertex+3);
		objectInfo[dsCurrObject].numDrawList++;
	}

	return 1;														   
}

char WritePolyInstruction(FaceInfo *poly1,int vertex)
{
	int i,j;
	int dest[3];
	int src[3];
	int used=0;
	int newVerts=0;
	int verts[3];
	int newTex[3];
	int changeUTile = 0, changeVTile = 0;
	unsigned int texVal[3];

	if (CheckForMatchingVerts(poly1)==1)
		fprintf(fp,"//Warning matching verts\n");

	for (i=0;i<3;i++) dest[i]=0;
	for (i=0;i<3;i++) src[i]=0;
	for (i=0;i<3;i++) newTex[i]=0;

	for (i=0;i<cacheUsed;i++)
	{
		if (used==3) break;
		for (j=0;j<3;j++)
		{
			if (used>0)
			{
				if (dest[0]==i) continue;
				if (used>1)
				{
					if (dest[1]==i) continue;
					if (used>2)
					{
						if (dest[2]==i) continue;
					}
				}
			}
			if (objectType==1)
			{
				if (CompareWithCache(poly1,j,i))
/*				if (((short)poly1->verts[j].v[X]==vertexCache[i].x)
					&&((short)poly1->verts[j].v[Y]==vertexCache[i].y)
					&&((short)poly1->verts[j].v[Z]==vertexCache[i].z))
*/				{
					dest[used++] = i;
					src[j] = 1;
					verts[j] = i;
					if (((short)poly1->textureCoords[j].v[X]!=vertexCache[i].tx)
						||((short)poly1->textureCoords[j].v[Y]!=vertexCache[i].ty))
					{
						newTex[j] = 1;
						texVal[j] = ((((unsigned int)poly1->textureCoords[j].v[X])/texscalex) << 16)|
							(((unsigned int)poly1->textureCoords[j].v[Y])/texscaley);
						vertexCache[i].tx = poly1->textureCoords[j].v[X];
						vertexCache[i].ty = poly1->textureCoords[j].v[Y];
					}

					break;
				}
			}
		}
	}


	if((currObj->flags & OBJECT_FLAGS_FACECOL) == 0)
	{
		for (i=0;i<3;i++)
		{
			if (newTex[i]==1)
			{
				if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
				{
					if ( !( currObj->flags & OBJECT_FLAGS_CHANGEDRAWLIST ) )
					{
						fprintf(display, "  gsSPModifyVertex(%d,G_MWO_POINT_ST,0x%X),\n",verts[i],texVal[i]);
						objectInfo[dsCurrObject].numDrawList++;
					}
					// ENDIF
				}
				// ENDIF
			}
		}

		//determine whether the tiling flags set in the current texture need to be changed
		if (lastUTile==0)
		{
			if (currObj->mesh->faceInfo[poly1->faceNo].faceFlags & U_TILE)
			{
				lastUTile = 1;
				changeUTile = 1;
			}
		}
		else
		{
			if (!(currObj->mesh->faceInfo[poly1->faceNo].faceFlags & U_TILE))
			{
				lastUTile = 0;
				changeUTile = 1;
			}
		}
		if (lastVTile==0)
		{
			if (currObj->mesh->faceInfo[poly1->faceNo].faceFlags & V_TILE)
			{
				lastVTile = 1;
				changeVTile = 1;
			}
		}
		else
		{
			if (!(currObj->mesh->faceInfo[poly1->faceNo].faceFlags & V_TILE))
			{
				lastVTile = 0;
				changeVTile = 1;
			}
		}

		if((changeUTile) || (changeVTile))
		{
			char format[16];
			char pixSize[16];
			int line;

			switch(currTexDetails.format)
			{
				case G_IM_FMT_RGBA:	sprintf(format, "G_IM_FMT_RGBA");
									break;
				case G_IM_FMT_IA:	sprintf(format, "G_IM_FMT_IA");
									break;
				default:			sprintf(format, "ARSE");
									break;

			}

			if(currObj->flags & OBJECT_FLAGS_IA)
				sprintf(format, "G_IM_FMT_IA");

			switch(currTexDetails.colDepth)
			{
				case G_IM_SIZ_4b:	sprintf(pixSize, "G_IM_SIZ_4b");
									line = 4;
									break;
				case G_IM_SIZ_8b:	sprintf(pixSize, "G_IM_SIZ_8b");
									line = 8;
									break;
				case G_IM_SIZ_16b:	sprintf(pixSize, "G_IM_SIZ_16b");
									line = 16;
									break;
				default:			sprintf(pixSize, "ARSE");
									break;
			}

			if(currObj->flags & OBJECT_FLAGS_IA)
			{
				sprintf(pixSize, "G_IM_SIZ_16b");
				line = 16;
			}


			if (currTexDetails.texID)
			{
				line *= currTexDetails.texID->sx;
				line /= 64;
			}
			else
			{
				line *= 16;
				line /= 64;
			}

			if (!line) line=1;

			if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
			{
				fprintf(display, "  gsDPTileSync(),\n");
				objectInfo[dsCurrObject].numDrawList++;
			}
			// ENDIF

			if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
			{

				if ( !( currObj->flags & OBJECT_FLAGS_CHANGEDRAWLIST ) )
				{
					fprintf(display, "  gsDPSetTile(%s, %s, %d, 0, 0, 0, %s, %d, G_TX_NOLOD, %s, %d, G_TX_NOLOD),\n",
								format, pixSize, (int)line,
								(lastVTile ==1) ? "G_TX_WRAP" : "G_TX_CLAMP",
								currTexDetails.scx, 
								(lastUTile ==1) ? "G_TX_WRAP" : "G_TX_CLAMP",
								currTexDetails.scy);
					objectInfo[dsCurrObject].numDrawList++;
				}
				// ENDIF
			}
			// ENDIf
		}
	}

	if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
	{
		if (objectType==1)
		{
			fprintf(display, "  gsSP1Triangle(%d, %d, %d, 0),\n", verts[0],verts[1],verts[2]);
			objectInfo[dsCurrObject].numDrawList++;
		}
		else
		{
			fprintf(display, "  gsSP1Triangle(%d, %d, %d, 0),\n", vertex,vertex+1,vertex+2);
			objectInfo[dsCurrObject].numDrawList++;
		}
	}
	// ENDIF
	return 1;														   
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteHeader(FILE *fp)
{
	int x, y, crc;
	char buf[256];
	char	*fname;
	OBJECTINFO	*ptr;

	char extMsg[16];

	if(userFormat == PC)
		strcpy(extMsg,"extern ");
	else
		extMsg[0] = 0;

	fprintf(fp, "//File generated by BankMaster(tm) - trial drawlist exporting.... arggh!\n\n");

	for(x = 0; x < numIncludeFiles; x++)
		fprintf(fp, "%s\n", includeFiles[x]);


	for(x = 0; x < numTotalObjects; x++)
	{
		fprintf(fp, "\n%sOBJECT %s_obj;\n", extMsg, allObjects[x]->name);
	
		if((user == SPIRIT) && (userFormat == N64))
		{
			if(allObjectsInfo[x]->drawList)
			{
				fprintf(fp, "Gfx %s_dl[];\n", allObjects[x]->name);
				fprintf(fp, "static Vtx %s_vtxa[];\n", allObjects[x]->name);
			}
			else
			{
				// added by Sharky

				// this is a non-drawlisted object
				fprintf(fp,"// %s is a non-drawlisted object\n",allObjects[x]->name);
			}

		}
		// ENDIF

//		fprintf(fp, "%sVKEYFRAME %s_scaleKey[];\n", extMsg, allObjects[x]->name);
//		fprintf(fp, "%sVKEYFRAME %s_moveKey[];\n", extMsg, allObjects[x]->name);
//		fprintf(fp, "%sQKEYFRAME %s_rotateKey[];\n", extMsg, allObjects[x]->name);

		if ((allObjects[x]->mesh) || (userFormat != PC))
			fprintf(fp, "%sMESH %s_mesh;\n", extMsg, allObjects[x]->name);
	}

	for(ptr = objectBanks[selectedObjectBank].objectList.head.next;ptr != &objectBanks[selectedObjectBank].objectList.head;ptr = ptr->next)
	{
		fprintf(fp, "%sOBJECT_CONTROLLER %s_controller;\n", extMsg, ptr->object->name);
	}

//	fprintf(fp, "\n\nint numDrawLists = %d;\n", numTotalObjects);
	fprintf(fp, "\nOBJECT_DESCRIPTOR\tobjectsPresent%X%s[] = {\n",UpdateCRC(0,outputPath,strlen(outputPath)),outputFileName);
	for(ptr = objectBanks[selectedObjectBank].objectList.head.next;ptr != &objectBanks[selectedObjectBank].objectList.head;ptr = ptr->next)
	{
  		crc = UpdateCRC(0, ptr->filename, strlen(ptr->filename));
		fprintf(fp, "\t0x%X, &%s_controller, \n", crc, ptr->object->name);
	}
	fprintf(fp, "\t0x00000000, NULL, \n");
	fprintf(fp, "\t};\n\n\n");

}



/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void WriteObjectVertices(FILE *fp, INPUT_OBJECT *obj, int flags)
{
	int x, y, i, fX, fY, fZ;
	int face = 0;
	int	mod;
	unsigned char alpha;
	BYTEVECTOR	*normals;
	BYTEVECTOR	cnormals;


	if(flags & OBJECT_FLAGS_XLU)
		alpha = 0x8f;
	else
		alpha = 0xff;



	face = 0;
	if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
	{
		fprintf(fp, "static Vtx %s_vtx[] = \n{\n", obj->name);
		for(y = 0; y < obj->mesh->numFaces; y++)
		{

			for(i = 0; i < 3; i++)
			{
				fX = obj->mesh->faceInfo[face].faceIndex.v[i];
													  
				if(flags & OBJECT_FLAGS_GOURAUD_SHADED)
					normals = &obj->mesh->vertexNormals[fX];
				else
					normals = &obj->mesh->faceInfo[face].faceNormal;

				fprintf(fp, "  {  %d, %d, %d, 0, %d, %d, %d, %d, %d, 0x%X },\n",
								(short)obj->mesh->vertices[fX].v[X],
								(short)obj->mesh->vertices[fX].v[Y],
								(short)obj->mesh->vertices[fX].v[Z],
								obj->mesh->faceInfo[face].faceTC[i].v[X],
								obj->mesh->faceInfo[face].faceTC[i].v[Y],
								normals->v[X],
								normals->v[Y],
								normals->v[Z], alpha
								);
			}
			face++;
		}


		fprintf(fp, "};\n");	
	}
	// ENDIF
	
//recurse my life away.....
	if(obj->children)
		WriteObjectVertices(fp, obj->children, flags);
	if(obj->next)
		WriteObjectVertices(fp, obj->next, flags);

		
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteVertices(FILE *fp)
{
	int x, alpha;
	OBJECTINFO	*ptr;

	for(ptr = objectBanks[selectedObjectBank].objectList.head.next;ptr != &objectBanks[selectedObjectBank].objectList.head;ptr = ptr->next)
	{
		WriteObjectVertices(fp, ptr->object, ptr->object->flags);
	}

}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int texscalex=1;
int texscaley=1;
int	lastTexID = 0;
void PrintTextureInfo(FILE *fp, TextureGroup *polys)
//void PrintTextureInfo(FILE *fp, TEXTURE_ID *texID)
{
	int scx, scy;
	int scalex,scaley;
	BITMAP_TYPE *bitmap;
	int mipMap = FALSE;
	TEXTURE_ID *texID = polys->texID;
	
	if((currObj->flags & OBJECT_FLAGS_FACECOL) == 0)
	{
		//if object is textured

		bitmap = FindTexture(texID->crc);
		if(bitmap)
		{
			if(strstr(bitmap->name,"mm_") == bitmap->name)
			{
				//texture is mip-mapped
				mipMap = TRUE;
				currObj->flags |= OBJECT_FLAGS_MIP_MAP;
			}
		}

		if(polys->texture2)
		{
			//this is a duel texture group
		}



		lastTextureID = texID->crc;

		if (texID->crc!=0)
		{

			switch(texID->sx)
			{
				case 8:
					scx = 3;
					scalex = 0x2000;
					texscalex = 1;//1;
					break;
				case 16:
					scx = 4;
					scalex = 0x4000;
					texscalex = 1;//1;
					break;
				case 32:
					scx = 5;
					scalex = 0x8000;
					texscalex = 1;//1;
					break;
				case 64:
					scx = 6;
					scalex = 0x10000;
					texscalex = 1;//1;
					break;
				default:
					scx = 5;
					scalex = 0x8000;
					texscalex = 1;//1;
					break;
			};
			switch(texID->sy)
			{
				case 8:
					scy = 3;
					scaley = 0x2000;
					texscaley = 1;//1;
					break;
				case 16:
					scy = 4;
					scaley = 0x4000;
					texscaley = 1;//1;
					break;
				case 32:
					scy = 5;
					scaley = 0x8000;
					texscaley = 1;//1;
					break;
				case 64:
					scy = 6;
					scaley = 0x10000;
					texscaley = 1;//1;
					break;
				default:
					scy = 5;
					scaley = 0x8000;
					texscaley = 1;//1;
					break;
			};


			currTexDetails.scx = scx;
			currTexDetails.scy = scy;
			currTexDetails.scalex = scalex;
			currTexDetails.scaley = scaley;
			
			currTexDetails.format = G_IM_FMT_RGBA;
			currTexDetails.texID = texID;			

			lastUTile = 1;
			lastVTile = 1;
			
			if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
			{
				fprintf(display, "  gsDPLoadSync(),\n");
				objectInfo[dsCurrObject].numDrawList++;
			}
			// ENDIF

			if(mipMap == FALSE)
			{
				switch(texID->colourDepth)
				{
					case 4:
							if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
							{
								fprintf(fp, "  gsDPSetTextureLUT(G_TT_RGBA16),\n");
								fprintf(fp, "  gsDPLoadTLUT_pal16(0, 0x%X),\n", texID->crc);
								fprintf(fp, "  gsDPLoadTextureBlock_4b(0x%X , G_IM_FMT_RGBA, %d, %d, 0,G_TX_WRAP, G_TX_WRAP, %d, %d, G_TX_NOLOD, G_TX_NOLOD),\n",
											texID->crc, texID->sx, texID->sy, scx, scy);
								currTexDetails.colDepth = G_IM_SIZ_4b;
								objectInfo[dsCurrObject].numDrawList+=14;	// gsDPLoadTLUT_pal16 = 6, gsDPLoadTextureBlock_4b = 7
							}
							// ENDIF
							break;
					case 8:
							if(currObj->flags & OBJECT_FLAGS_IA)
							{
								if ( !( currObj->flags & OBJECT_FLAGS_CHANGEDRAWLIST ) )
								{
									if(userFormat != PC)
									{
										fprintf(fp, "  gsDPSetTextureLUT(G_TT_NONE),\n");
										fprintf(fp, "  gsDPLoadTextureBlock(0x%X , G_IM_FMT_IA, G_IM_SIZ_16b, %d, %d, 0,G_TX_WRAP, G_TX_WRAP, %d, %d, G_TX_NOLOD, G_TX_NOLOD),\n",
											texID->crc, texID->sx, texID->sy, scx, scy);
									}
								}
								// ENDIF
							}
							else
							{
								if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
								{
									fprintf(fp, "  gsDPSetTextureLUT(G_TT_RGBA16),\n");
									fprintf(fp, "  gsDPLoadTLUT_pal256(0x%X),\n", texID->crc);
									fprintf(fp, "  gsDPLoadTextureBlock(0x%X , G_IM_FMT_RGBA, G_IM_SIZ_8b, %d, %d, 0,G_TX_WRAP, G_TX_WRAP, %d, %d, G_TX_NOLOD, G_TX_NOLOD),\n",
											texID->crc, texID->sx, texID->sy, scx, scy);
								}
								// ENDIF
							}
							// ENDELSEIF
							currTexDetails.colDepth = G_IM_SIZ_8b;
							objectInfo[dsCurrObject].numDrawList+=14;	// gsDPLoadTLUT_pal256 = 6, gsDPLoadTextureBlock = 7
							break;
					case 16:
					case 24:
							if ( !( currObj->flags & OBJECT_FLAGS_CHANGEDRAWLIST ) )
							{
								if(userFormat != PC)
									fprintf(fp, "  gsDPSetTextureLUT(G_TT_NONE),\n");
								if(polys->texture2)
								{
									if(userFormat != PC)
										fprintf(fp, "  _gsDPLoadTextureBlockTile(0x%X, 0, 0,G_IM_FMT_RGBA, G_IM_SIZ_16b, %d, %d, 0,G_TX_WRAP, G_TX_WRAP, %d, %d, 0, 0),\n", 
											texID->crc, texID->sx, texID->sy, scx, scy);
									if(userFormat != PC)
										fprintf(fp, "  _gsDPLoadTextureBlockTile(0x%X,256,1,G_IM_FMT_RGBA, G_IM_SIZ_16b, %d, %d, 0,G_TX_WRAP, G_TX_WRAP, %d, %d, 0, 0),\n",
											polys->texture2, texID->sx, texID->sy, scx, scy);
								}
								else
								{
									if(userFormat != PC)
										fprintf(fp, "  gsDPLoadTextureBlock(0x%X , G_IM_FMT_RGBA, G_IM_SIZ_16b, %d, %d, 0,G_TX_WRAP, G_TX_WRAP, %d, %d, G_TX_NOLOD, G_TX_NOLOD),\n",
												texID->crc, texID->sx, texID->sy, scx, scy);
								}
								currTexDetails.colDepth = G_IM_SIZ_16b;
								objectInfo[dsCurrObject].numDrawList+=8;	// gsDPLoadTextureBlock = 7
							}
							// ENDIF
							break;
					default:
							dbprintf("Colour depth not supported\n");
							currTexDetails.colDepth = G_IM_SIZ_4b;
							break;

				}
			}
			else
			{
				dbprintf("Found mip-mapped texture.. %s\n", bitmap->name);
				//set up mipmap texture load
				if(userFormat == N64)
				{
					fprintf(fp, "  gsDPSetTextureLUT(G_TT_NONE),\n");
					fprintf(fp, "  gsDPSetTextureImage( 0, 2, 1, 0x%X),\n", texID->crc);
					fprintf(fp, "  gsDPSetTile( 0, 2, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP, 5, 0, G_TX_WRAP, 5, 0),\n");
					fprintf(fp, "  gsDPLoadSync(),\n");
					fprintf(fp, "  gsDPLoadBlock( G_TX_LOADTILE, 0, 0, 1372, 0),\n");
					fprintf(fp, "  gsDPPipeSync(),\n");

					fprintf(fp, "  gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, 0, 0, G_TX_WRAP, 5, 0, G_TX_WRAP, 5, 0),\n");
					fprintf(fp, "  gsDPSetTileSize( 0,  0, 0, 31 << G_TEXTURE_IMAGE_FRAC, 31 << G_TEXTURE_IMAGE_FRAC),\n");

					fprintf(fp, "  gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 256, 1, 0, G_TX_WRAP, 4, 1, G_TX_WRAP, 4, 1),\n");
					fprintf(fp, "  gsDPSetTileSize( 1,  1, 1, 15 << G_TEXTURE_IMAGE_FRAC, 15 << G_TEXTURE_IMAGE_FRAC),\n");

					fprintf(fp, "  gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 2, 320, 2, 0, G_TX_WRAP, 3, 2, G_TX_WRAP, 3, 2),\n");
					fprintf(fp, "  gsDPSetTileSize( 2,  1, 1, 7 << G_TEXTURE_IMAGE_FRAC, 7 << G_TEXTURE_IMAGE_FRAC),\n");
  

					fprintf(fp, "  gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, 336, 3, 0, G_TX_WRAP, 2, 3, G_TX_WRAP, 2, 3),\n");
					fprintf(fp, "  gsDPSetTileSize( 3,  1, 1, 3 << G_TEXTURE_IMAGE_FRAC, 3 << G_TEXTURE_IMAGE_FRAC),\n");

					fprintf(fp, "  gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, 340, 4, 0, G_TX_WRAP, 1, 4, G_TX_WRAP, 1, 4),\n");
					fprintf(fp, "  gsDPSetTileSize( 4,  1, 1, 1 << G_TEXTURE_IMAGE_FRAC, 1 << G_TEXTURE_IMAGE_FRAC),\n");

					fprintf(fp, "  gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, 342, 5, 0, G_TX_WRAP, 0, 5, G_TX_WRAP, 0, 5),\n");
					fprintf(fp, "  gsDPSetTileSize( 5,  1, 1, 0 << G_TEXTURE_IMAGE_FRAC, 0 << G_TEXTURE_IMAGE_FRAC),\n");
				}

			}

		}
		else
		{
			currTexDetails.scx = 4;
			currTexDetails.scy = 4;
			currTexDetails.scalex = 0x4000;
			currTexDetails.scaley = 0x4000;
			
			currTexDetails.format = G_IM_FMT_RGBA;
			currTexDetails.texID = NULL;

			if(userFormat == N64)
			{
			fprintf(display, "  gsDPLoadSync(),\n");
			objectInfo[dsCurrObject].numDrawList++;

			fprintf(fp, "  gsDPSetTextureLUT(G_TT_NONE),\n");
			objectInfo[dsCurrObject].numDrawList++;

			fprintf(fp, "  gsDPLoadTextureBlock(0x%X , G_IM_FMT_RGBA, G_IM_SIZ_16b, %d, %d, 0,G_TX_CLAMP, G_TX_CLAMP, %d, %d, G_TX_NOLOD, G_TX_NOLOD),\n",
			UpdateCRC(0, "white.bmp", 9), 16, 16, 4, 4);
			objectInfo[dsCurrObject].numDrawList+=7;	// gsDPLoadTextureBlock = 7

			fprintf(fp, "  gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 0, 0, 0, G_TX_CLAMP, 4, G_TX_NOLOD, G_TX_CLAMP, 4, G_TX_NOLOD),\n");
			objectInfo[dsCurrObject].numDrawList++;
			}

			lastUTile = 0;
			lastVTile = 0;
		}
	}
	else
	{
		//object is face coloured
		unsigned char r, g, b;
		int temp;

		temp = texID->crc;
		r = (temp >> 24) & 0xff;
		g = (temp >> 16) & 0xff;
		b = (temp >>  8) & 0xff;
		fprintf(fp, "  gsDPSetPrimColor(0, 0, %d, %d, %d, 255),\n", r, g, b);

	}
}



/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteDrawlists(FILE *fp)
{
	int x;
	int i;
	OBJECTINFO	*ptr;



	for(ptr = objectBanks[selectedObjectBank].objectList.head.next;ptr != &objectBanks[selectedObjectBank].objectList.head;ptr = ptr->next)
	{
		//skip if no drawlist is required
		if(ptr->drawList == FALSE)
			continue;


		//skip if object is collision only		
		if(strstr(ptr->filename, "c_") == ptr->filename)
			continue;			

		currObjInfo = ptr;
		lastTextureID = 0;
		WriteObject(ptr->object);

	}
}

void InitBBox(void)
{
	bBox[0].x = 30000;
	bBox[1].x = 30000;
	bBox[2].x = -30000;
	bBox[3].x = -30000;
	bBox[4].x = 30000;
	bBox[5].x = 30000;
	bBox[6].x = -30000;
	bBox[7].x = -30000;

	bBox[0].z = 30000;
	bBox[1].z = -30000;
	bBox[2].z = -30000;
	bBox[3].z = 30000;
	bBox[4].z = 30000;
	bBox[5].z = -30000;
	bBox[6].z = -30000;
	bBox[7].z = 30000;

	bBox[0].y = 30000;
	bBox[1].y = 30000;
	bBox[2].y = 30000;
	bBox[3].y = 30000;
	bBox[4].y = -30000;
	bBox[5].y = -30000;
	bBox[6].y = -30000;
	bBox[7].y = -30000;
}

void PrintBBox(void)
{
	int i;

	for (i=0;i<8;i++)
	{
		fprintf(verts, "  {  %d, %d, %d, 0, 0, 0, 0, 0, 0, 0},\n",
				bBox[i].x,bBox[i].y,bBox[i].z);
	}
}

void WriteObject(INPUT_OBJECT *object)
{
	int x;
	int i;
	int bound = FALSE;

	currObj = object;

	dbprintf("Writing object %s\n", object->name);
	
	if ((object->flags&OBJECT_FLAGS_GOURAUD_SHADED)||(object->flags&OBJECT_FLAGS_GOURAUD_SHADED))
		objectType=1;
	else
		objectType=0;

	if(object->flags & OBJECT_FLAGS_XLU)
		myAlpha = object->xluFactor;
	else
		myAlpha = 0xff;

//	if (user==DSTORM) object->flags |= OBJECT_FLAGS_PRELIT;
	
	if(object->flags & OBJECT_FLAGS_PRELIT)
		myColouredGouraud = 1;
	else
		myColouredGouraud = 0;

	myObject = object;
	objectInfo[dsCurrObject].numDrawList = 0;

	if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
	{
		fprintf(display, "Gfx %s_dl[] = \n", object->name);
		fprintf(display, "{\n");
	}
	if(object->mesh->numFaces)
	{
		if(object->mesh->numFaces > 8)
			bound = TRUE;
	
		if(bound)
			InitBBox();

		object->bound = bound;

		CreateTextureLists(textureFaces,object);

		if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
		{
			fprintf(verts, "static Vtx %s_vtxa[] = \n{\n", object->name);
		}
		// ENDIF

		if(bound)
		{
			if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
			{
				PrintBBox();
			}
			// ENDIF

			if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
			{
				fprintf(display, "  gsSPClearGeometryMode(G_LIGHTING),\n");
				if(currObjInfo->skinned)
					fprintf(display, "  gsSPVertex(SKINNED_SEG + (16 * %d), %d, 0),\n", vertexPos,8);
				else
					fprintf(display, "  gsSPVertex(&(%s_vtxa[%d]), %d, 0),\n", object->name, vertexPos,8);
				objectInfo[dsCurrObject].numDrawList+=2;
			}
			// ENDIF
		}
		else
			if(userFormat != PC)
				fprintf(display, "  gsSPClearGeometryMode(G_LIGHTING),\n");


		if(bound)
		{
			vertexPos+=8;
			if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
			{
				fprintf(display,"  gsSPCullDisplayList(0,7),\n");
				objectInfo[dsCurrObject].numDrawList++;
			}
			// ENDIF
		}


		if(!(object->flags & OBJECT_FLAGS_PRELIT))
		{
			if(userFormat != PC)
				fprintf(display, "  gsSPSetGeometryMode(G_LIGHTING),\n");
			objectInfo[dsCurrObject].numDrawList++;
		}

		if ( ( currObj->flags & OBJECT_FLAGS_CHANGEDRAWLIST ) )
		{
			if(userFormat != PC)
				fprintf(display, "  gsDPSetCombineMode(G_CC_DECALRGB,G_CC_DECALRGB),\n");
  			objectInfo[dsCurrObject].numDrawList++;
		}
		// ENDIF



		if(currObjInfo->skinned)
		{
			object->numVtx = 0;		//stores how many verts are in the Vtx list, used for skinning
			object->drawListVtxList = (N64vertex *)malloc(sizeof(N64vertex) * 1500);	//warning, assumes 500 vertices only
		}
		for (i=0;i<numberTextures;i++)
		{
			CalcBestOrder(&textureFaces[i],object);
		}

		objectInfo[dsCurrObject].numVertices = vertexPos;
		dbprintf("vertices: #%d => '%s' => %d\n",dsCurrObject,object->name,objectInfo[dsCurrObject].numVertices);

		if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
		{
			fprintf(verts,"};\n");
		}
		// ENDIF

		if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
		{
			fprintf(display, "  gsSPEndDisplayList(),\n");
			objectInfo[dsCurrObject].numDrawList++;
		}
		// ENDIF
	}
	else
	{
		if(userFormat != PC)
			fprintf(verts, "static Vtx %s_vtx[] = \n{\n};\n", object->name);
	}
	if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
	{
		fprintf(display, "};\n\n");
	}
	// ENDIF
	CopyFileIntoStream(fp,display);
	fclose(display);
	display=fopen("c:\\display","wt+");
	if(display == NULL)
	{
		printf("File error\n");
		exit(1);	
	}
	fprintf(fp,"\n"); 
	CopyFileIntoStream(fp,verts);
	fclose(verts);


	if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
	{
		if(currObjInfo->skinned)	//massive bodge to double buffer the Vtx's
		{
			fprintf(fp, "static Vtx %s_vtxb[] = \n{\n", object->name);		
			if(object->bound)
			{
				for (i=0;i<8;i++)
				{
					fprintf(fp, "  {  %d, %d, %d, 0, 0, 0, 0, 0, 0, 0},\n",
							bBox[i].x,bBox[i].y,bBox[i].z);
				}
			}			
			for(i = 0; i < object->numVtx; i++)
			{
			
				fprintf(fp, "  {  %d, %d, %d, 0, %d, %d, %d, %d, %d, 0x%X },\n",
					object->drawListVtxList[i].x,object->drawListVtxList[i].y,object->drawListVtxList[i].z,
					object->drawListVtxList[i].tx,object->drawListVtxList[i].ty,
					(uchar)object->drawListVtxList[i].nx,(uchar)object->drawListVtxList[i].ny,(uchar)object->drawListVtxList[i].nz,
					object->drawListVtxList[i].alpha);


			}
		
			fprintf(fp, "};\t//double buffered vtx\n");
		}
		// ENDIF
	}
	// ENDIF









	verts=fopen("c:\\verts","wt+");
	if(verts == NULL)
	{
		printf("File error\n");
		exit(1);	
	}
	fprintf(fp,"\n");

	dbprintf("drawlist: #%d => '%s' => %d\n",dsCurrObject,object->name,objectInfo[dsCurrObject].numDrawList);
	dsCurrObject++;

	if(currObjInfo->skinned)
		return;

	if((object->children))// && (object->children->mesh->numFaces))
		WriteObject(object->children);

	if((object->next))// && (object->next->mesh->numFaces))
		WriteObject(object->next);
}

void CopyFileIntoStream(FILE *dest,FILE *src)
{
	int filesize,i;
	int temp;
	fseek(src,0L,SEEK_END);
	filesize = ftell(src);
	fseek(src,0L,SEEK_SET);
	//for (i=0;i<filesize+1;i++)
	for (;;)
	{
		//fread(&temp,1,1,src);
		//fwrite(&temp,1,1,dest);
		temp = fgetc(src);
		if (feof(src)!=0) return;
		fputc(temp,dest);
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int SortAnimRanges(const void *e1, const void *e2)
{
	ANIMRANGE *r1, *r2;

	r1 = *((ANIMRANGE**)e1);
	r2 = *((ANIMRANGE**)e2);

	return(strcmp(r1->segmentident, r2->segmentident));
}
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteObjectAnimations(FILE *fp, OBJECTINFO *obj)
{
	char buf[256];
	int x, numRanges = obj->numAnimRanges;
	ANIMRANGE	*range;
	FILE	*animNames;
	ANIMRANGE	**tempRanges;

	if(numRanges == 0)
		numRanges = 1;

	if(obj->numAnimRanges)
	{
		fprintf(fp, "animation %s_animationRange1[%d] = \n{\n", obj->object->name, numRanges);

		
		tempRanges = (ANIMRANGE **)malloc(sizeof(ANIMRANGE *) * obj->numAnimRanges);
		for(x = 0; x < obj->numAnimRanges; x++)
			tempRanges[x] = &obj->ranges[x];

		qsort(tempRanges, obj->numAnimRanges, sizeof(ANIMRANGE *), SortAnimRanges);

		for(x = 0; x < obj->numAnimRanges; x++)
		{
			range = tempRanges[x];
//			range = obj->ranges + x;

			fprintf(fp, "\t//%s\n", range->segmentident);
			fprintf(fp, "\t{\n");
			fprintf(fp, "\t\t%d,\n", range->firstframe);		//animstart
			fprintf(fp, "\t\t%d,\n", range->lastframe);		//animend
			fprintf(fp, "\t\t%f,\n", range->speed);	//animspeed
			
			if(range->flags & 1)//animloop
				fprintf(fp, "\t\t1,\n");		//animLoop
			else
				fprintf(fp, "\t\t0,\n");		//animLoop

			fprintf(fp, "\t},\n");

		}
		fprintf(fp, "};\n");

		free(tempRanges);
	}

}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteAnimations(FILE *fp)
{
	OBJECTINFO	*ptr;
	int x;

	for(ptr = objectBanks[selectedObjectBank].objectList.head.next;ptr != &objectBanks[selectedObjectBank].objectList.head;ptr = ptr->next)
	{
		if ((ptr->object->numScaleKeys >= 1)||(ptr->object->numRotateKeys >= 1)||
			(ptr->object->numMoveKeys >= 1))
		{
			WriteObjectAnimations(fp, ptr);
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteAnimationDef(FILE *fp)
{
	OBJECTINFO	*ptr;
	INPUT_OBJECT	*obj;
	int x;

	for(ptr = objectBanks[selectedObjectBank].objectList.head.next;ptr != &objectBanks[selectedObjectBank].objectList.head;ptr = ptr->next)
	{
		obj = ptr->object;
//		if(strcmp(obj->name, "G_Heart2"))
//			continue;

		fprintf(fp, "ACTOR_ANIMATION %s_animation = {\n", obj->name);

		
		fprintf(fp, "\t%d, \n", ptr->numAnimRanges);	//numanimations


		fprintf(fp, "\t0, \n");	//currentanimation
		fprintf(fp, "\t0, \n");	//reached end
		fprintf(fp, "\t0, \n");	//loopanimation
		fprintf(fp, "\t0, \n");	//speedanim
		fprintf(fp, "\t0, 0, 0, 0, 0, \n");//queueAnimation
		fprintf(fp, "\t0, 0, 0, 0, 0, \n");//queueloopanim
		fprintf(fp, "\t0, 0, 0, 0, 0, \n");//queuespeedanim
		fprintf(fp, "\t0,\n");				//num queued


		if(ptr->numAnimRanges)
			fprintf(fp, "\t&%s_animationRange1[0],\n", obj->name);			//anims
		else
			fprintf(fp, "\tNULL,\n", obj->name);			//anims

		fprintf(fp, "\t0,\n");				//animtime
		fprintf(fp, "};\n");
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteObjectDef(FILE *fp, INPUT_OBJECT *obj)
{
	char	temp[256];
	int tFlags;
	
	strcpy(temp, obj->name);
	if(strlen(temp) > 15)
		temp[15] = 0;

	fprintf(fp, "\nOBJECT %s_obj = {\n", obj->name);

	//objid
	fprintf(fp, "\t0x%X,\n", UpdateCRC(0, obj->name, strlen(obj->name)));	//objID

	//name
	fprintf(fp, "\t\"%s\",\n", temp);						//name

	//pixel out alpha
	fprintf(fp, "\t0,\n");					//pixel-out-alpha

	//xlufact
	fprintf(fp, "\t%d,\n", (unsigned char)obj->xluFactor);					//xlu


	//num sprites
	fprintf(fp, "\t%d,\t//number of sprites\n", obj->numSprites);//numSprites

	//sprites
	if(obj->numSprites > 0)
		fprintf(fp, "\t&%s_sprite1,\n", obj->name);
	else
		fprintf(fp, "\tNULL,\n", obj->name);

	//mesh
	if((obj->mesh) || (userFormat != PC))
		fprintf(fp, "\t&%s_mesh,\n", obj->name);				//mesh
	else
		fprintf(fp, "\tNULL,\n");				//mesh

	//drawlist
	if((outputDrawList == TRUE) && (obj->mesh) && (obj->mesh->numFaces) && (userFormat != PC))
		fprintf(fp, "\t%s_dl,\n", obj->name);
	else
		fprintf(fp, "\tNULL,\n");

	//animating texture
	fprintf(fp, "\tNULL,\t//animating texture\n");			//animating texture


	if(currObjInfo->skinned)
	{
		//effected verts (skinning)
		if(obj->numVertsInBone)
			fprintf(fp, "\t(SKINVTX *)%s_effectedVerts,\t//effected verts\n", obj->name);
		else
			fprintf(fp, "\tNULL,\t//effected verts\n");

		//original verts (skinning)
		if(obj->numVertsInBone)
			fprintf(fp, "\t%s_origVerts,\t//original verts\n", obj->name);
		else
			fprintf(fp, "\tNULL,\t//original verts\n");
	}
	else
	{
		fprintf(fp, "\tNULL,\n");
		fprintf(fp, "\tNULL,\n");
	}
	//numverts (skinning)
	fprintf(fp, "\t%d,\t//numVertsInBone\n", obj->numVertsInBone);

	//keyframes
	fprintf(fp, "\t%s_scaleKeys,\n", obj->name);
	fprintf(fp, "\t%s_moveKeys,\n", obj->name);
	fprintf(fp, "\t%s_rotateKeys,\n", obj->name);
	fprintf(fp, "\t%d,\n", obj->numScaleKeys);				
	fprintf(fp, "\t%d,\n", obj->numMoveKeys);
	fprintf(fp, "\t%d,\n", obj->numRotateKeys);

	//object colour
//	if(obj->objColour)
		fprintf(fp, "\t%d,%d,%d,255,\t//object colour\n", obj->objColour.r, obj->objColour.g, obj->objColour.b);
//	else
//		fprintf(fp, "\t255,255,255,255,\t//object colour\n");
		
	//phong texture
	if(obj->flags & OBJECT_FLAGS_PHONG)
		fprintf(fp, "\t(TEXTURE*)0x%X,\t//phong texture\n", obj->phongTexture);
	else
		fprintf(fp, "\tNULL,\t//phone texture\n");
		

	//flags
	fprintf(fp, "\t%d,//flags\n", (unsigned short)obj->flags);
	
	//numChildren
	fprintf(fp, "\t%d,//numchildren\n", obj->numChildren);

	if(obj->children)
		fprintf(fp, "\t&%s_obj,\n", obj->childName);
	else
		fprintf(fp, "\tNULL,\n");
	if(obj->next)
		fprintf(fp, "\t&%s_obj,\n", obj->nextName);
	else
		fprintf(fp, "\tNULL,\n");

	//collsphere
	fprintf(fp, "\tNULL,//collSphere\n");

	//matrix
//	fprintf(fp, "\tNULL,//matrix\n");
	fprintf(fp, "\t1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1, //matrix\n");
	

	fprintf(fp, "};\n\n");

	dsCurrObject++;


	if(obj->children)
		WriteObjectDef(fp, obj->children);
	if(obj->next)
		WriteObjectDef(fp, obj->next);


	
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteObjectDefinitions(FILE *fp)
{
	int x;
	OBJECTINFO	*ptr;
		
	for(ptr = objectBanks[selectedObjectBank].objectList.head.next;ptr != &objectBanks[selectedObjectBank].objectList.head;ptr = ptr->next)
	{
		currObjInfo = ptr;

		if ( ( ptr->drawList == TRUE ) && ( ptr->skinned == FALSE ) )
			outputDrawList = TRUE;
		else
			outputDrawList = FALSE;
		// ENDELSEIF - ( ( ptr->drawList == TRUE ) && ( ptr->skinned == FALSE ) )

		WriteObjectDef(fp, ptr->object);
	}	
}

BOOL IsKeyOnSegmentBoundary(short num, short time)
{
	INPUT_OBJECT	*obj = allObjects[num];
	OBJECTINFO		*objInfo = allObjectsInfo[num];
	int				x;

	for(x = 0; x < objInfo->numAnimRanges; x++)
	{
		if((time == objInfo->ranges[x].firstframe) || (time == objInfo->ranges[x].lastframe))
			return TRUE;
	}

	return FALSE;

}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteObjectKeyframe(FILE *fp, short num)//INPUT_OBJECT *obj)
{
	int			x;
	KEYFRAME	*key, *key2;
	float		xval = -1.0, yval = -1.0, zval = -1.0, wval = -1.0;
	int		numKeys;
	INPUT_OBJECT	*obj = allObjects[num];
	OBJECTINFO		*objInfo = allObjectsInfo[num];


	numKeys = obj->numScaleKeys;
	fprintf(fp, "\nVKEYFRAME %s_scaleKeys[] = \n{\n", obj->name);
	for(x = 0; x < obj->numScaleKeys; x++)
	{
		key = obj->scaleKeys + x;
		key2 = obj->scaleKeys + x + 1;

		if((optimiseKeyframes == FALSE) ||
		   (x == obj->numScaleKeys -1) ||
		   (x == 0) ||
		   (key->u.vect.v[X] != xval) ||
		   (key->u.vect.v[Y] != yval) ||
		   (key->u.vect.v[Z] != zval) ||
		   (key2->u.vect.v[X] != xval) ||
		   (key2->u.vect.v[Y] != yval) ||
		   (key2->u.vect.v[Z] != zval) || 
		   (IsKeyOnSegmentBoundary(num, key->time))
		   ) 

			fprintf(fp, "\t%f, %f, %f, %d,\n", key->u.vect.v[X], key->u.vect.v[Y], key->u.vect.v[Z], key->time);
		else
		{
			fprintf(fp, "//Saved Keyframe!\n");
  //			dbprintf("Saved Keyframe\n");
			numKeys--;
		}

//		fprintf(fp, "\t%f, %f, %f, %d,\n", key->u.vect.v[X], key->u.vect.v[Y], key->u.vect.v[Z], key->time);

		xval = key->u.vect.v[X];
		yval = key->u.vect.v[Y];
		zval = key->u.vect.v[Z];

	}
	fprintf(fp, "};\n");
	xval = yval = zval = wval = -1.0;
	obj->numScaleKeys = numKeys;

	numKeys = obj->numMoveKeys;
	fprintf(fp, "\nVKEYFRAME %s_moveKeys[] = \n{\n", obj->name);
	for(x = 0; x < obj->numMoveKeys; x++)
	{
		key = obj->moveKeys + x;
		key2 = obj->moveKeys + x + 1;

		if((optimiseKeyframes == FALSE) ||
		  (x == obj->numMoveKeys - 1) ||
		  (x == 0) ||
		  (key->u.vect.v[X] != xval) ||
		  (key->u.vect.v[Y] != yval) ||
		  (key->u.vect.v[Z] != zval) ||
		  (key2->u.vect.v[X] != xval) ||
		  (key2->u.vect.v[Y] != yval) ||
		  (key2->u.vect.v[Z] != zval) || 
		  (IsKeyOnSegmentBoundary(num, key->time))
		  )
			fprintf(fp, "\t%f, %f, %f, %d,\n", key->u.vect.v[X], key->u.vect.v[Y], key->u.vect.v[Z], key->time);
		else
		{
			fprintf(fp, "//Saved Keyframe!\n");
//			dbprintf("Saved Keyframe\n");
			numKeys--;
		}

		xval = key->u.vect.v[X];
		yval = key->u.vect.v[Y];
		zval = key->u.vect.v[Z];

	}
	fprintf(fp, "};\n");
	xval = yval = zval = wval = -1.0;
	obj->numMoveKeys = numKeys;

	numKeys = obj->numRotateKeys;

	if(userFormat == PC)
		fprintf(fp, "\nQKEYFRAME %s_rotateKeys[] = \n{\n", obj->name);
	else
		fprintf(fp, "\nKEYFRAMESHORT %s_rotateKeys[] = \n{\n", obj->name);

	for(x = 0; x < obj->numRotateKeys; x++)
	{
		key = obj->rotateKeys + x;
		key2 = obj->rotateKeys + x + 1;

		if((optimiseKeyframes == FALSE) ||
		  (x == obj->numRotateKeys - 1) ||
		  (x == 0) ||
		  (key->u.quat.x != xval) || 
		  (key->u.quat.y != yval) || 
		  (key->u.quat.z != zval) || 
		  (key->u.quat.w != wval) ||
		  (key2->u.quat.x != xval) || 
		  (key2->u.quat.y != yval) || 
		  (key2->u.quat.z != zval) || 
		  (key2->u.quat.w != wval) ||
		  (IsKeyOnSegmentBoundary(num, key->time))
		  )
		{
			if(userFormat == PC)
			{
				fprintf(fp, "\t%f, %f, %f, %f, %d,\n", key->u.quat.x, key->u.quat.y, key->u.quat.z, key->u.quat.w, key->time);
			}
			else
			{
				short qx,qy,qz,qw;

				// convert floating-point to fixed-point shorts
				qx = (short)(key->u.quat.x * 32767);
				qy = (short)(key->u.quat.y * 32767);
				qz = (short)(key->u.quat.z * 32767);
				qw = (short)(key->u.quat.w * 32767);

				fprintf(fp, "\t%d, %d, %d, %d, %d,\n", qx, qy, qz, qw, key->time);
			}
		}
		else
		{
			fprintf(fp, "//Saved Keyframe!\n");
//			dbprintf("Saved Keyframe\n");
			numKeys--;
		}
		 
		xval = key->u.quat.x;
		yval = key->u.quat.y;
		zval = key->u.quat.z;
		wval = key->u.quat.w;

	}
	fprintf(fp, "};\n");
	xval = yval = zval = wval = -1.0;
	obj->numRotateKeys = numKeys;



}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteObjectKeyframes(FILE *fp)
{
	int x;

	for(x = 0; x < numTotalObjects; x++)
	{
		WriteObjectKeyframe(fp, x);//allObjects[x]);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteObjectControllers(FILE *fp)
{
	int x, crc;
	INPUT_OBJECT *obj;
	OBJECTINFO	*ptr, *cPtr;

	for(ptr = objectBanks[selectedObjectBank].objectList.head.next;ptr != &objectBanks[selectedObjectBank].objectList.head;ptr = ptr->next)
	{
		obj = ptr->object;
		fprintf(fp, "OBJECT_CONTROLLER %s_controller = \n{\n", obj->name);
		crc = UpdateCRC(0, ptr->filename, strlen(ptr->filename));
		fprintf(fp, "\t0x%X,\n", crc);
		fprintf(fp, "\tNULL,\n");
		fprintf(fp, "\t0,\n");
		fprintf(fp, "\t&%s_obj,\n", obj->name);
		fprintf(fp, "\t0,\n");
		fprintf(fp, "\tNULL,\n");

		fprintf(fp, "\t&%s_animation,\n", obj->name);
		if((ptr->skinned) && (userFormat != PC))
		{
			fprintf(fp, "\t%s_dl, \n", obj->name);
			fprintf(fp, "\t%s_vtxa, \n", obj->name);
			fprintf(fp, "\t%s_vtxb, \n", obj->name);
		}
		else
		{
			fprintf(fp, "\tNULL,\n");				//Vtx a - used for skinning
			fprintf(fp, "\tNULL,\n");				//Vtx a - used for skinning
			fprintf(fp, "\tNULL,\n");				//Vtx b - used for skinning
		}
		
		cPtr = CollisionMeshPresent(ptr);
		if(cPtr)
			fprintf(fp, "\t&%s_mesh,\n", cPtr->object->name);
		else
			fprintf(fp, "\tNULL,\n");

		if ((userFormat == PC) && (ptr->skinned))
			fprintf(fp, "\t1,\t//skinned [PC]\n");
		else
			fprintf(fp, "\t0,\t//vertex buffer (not skinned [PC])\n");

		if(ptr->object->bound)
			fprintf(fp, "\t%d,\t//num vtx's\n", ptr->object->numVtx + 8);
		else
			fprintf(fp, "\t%d,\t//num vtx's\n", ptr->object->numVtx);

			
		fprintf(fp, "};\n");
	}

	if((user == SPIRIT) && (userFormat == PC))
	{
		fprintf(fp, "\n\n#define DllExport __declspec( dllexport )\n");
		fprintf(fp, "extern \"C\" DllExport OBJECT_DESCRIPTOR *GetbankAddress()\n{\n");
		fprintf(fp, "\treturn &objectsPresent%X%s[0];\n}\n",UpdateCRC(0,outputPath,strlen(outputPath)),outputFileName);
	}
/*
	if((user == WNR) && (userFormat == PC))
	{
		fprintf(fp, "\n\n#define DllExport __declspec( dllexport )\n");
		fprintf(fp, "extern \"C\" DllExport OBJECT_DESCRIPTOR *GetbankAddress()\n{\n");
		fprintf(fp, "\treturn &objectsPresent%X%s[0];\n}\n",UpdateCRC(0,outputPath,strlen(outputPath)),outputFileName);
	}
*/
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteObjectSpriteDefs(FILE *fp)
{
	INPUT_OBJECT	*obj;
	int		x, y;

	for(x = 0; x < numTotalObjects; x++)
	{
		obj = allObjects[x];
		if(obj->numSprites > 0)
		{
			for(y = 0; y < obj->numSprites; y++)
			{
				fprintf(fp, "OBJECTSPRITE %s_sprite%d = {\n", obj->name, y+1);
				fprintf(fp, "\t(TEXTURE *)0x%X, \n", obj->sprites[y].textureID);
				fprintf(fp, "\tNULL, \n");
				fprintf(fp, "\t%d, \n", obj->sprites[y].x);
				fprintf(fp, "\t%d, \n", obj->sprites[y].y);
				fprintf(fp, "\t%d, \n", obj->sprites[y].z);
				fprintf(fp, "\t%d, \n", obj->sprites[y].sx);
				fprintf(fp, "\t%d, \n", obj->sprites[y].sy);
				fprintf(fp, "\t%d, \n", obj->sprites[y].ox);
				fprintf(fp, "\t%d, \n", obj->sprites[y].oy);
				fprintf(fp, "\t%d, \n", obj->sprites[y].flags);
				fprintf(fp, "};\n\n");
			}
						


		}



	}

}


//****************************************************************************
//****************************************************************************
//****************************************************************************
//	All the mesh stuff - this will change when steve decides what data he needs
//	for collision detection

BOOL IsObjectAnimating(INPUT_OBJECT *obj)
{
	if((obj->numScaleKeys > 1) || (obj->numMoveKeys > 1) || (obj->numRotateKeys > 1))
		return TRUE;

	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteMeshVertices(FILE *fp, INPUT_OBJECT *obj)
{
	int i;
	int x,y,z;
	float fx,fy,fz;

	if(obj->mesh->numVertices)
	{
		fprintf(fp, "VECTOR %s_vertices[] = {\n", obj->name);
		if((IsObjectAnimating(obj) == FALSE)  || (userFormat == PC))
//		if((IsObjectAnimating(obj) == FALSE) || ((user == WNR) && (userFormat == PC)))
		{
			for(i = 0; i < obj->mesh->numVertices; i++)
			{
				x = (int)obj->mesh->vertices[i].v[X];
				y = (int)obj->mesh->vertices[i].v[Y];
				z = (int)obj->mesh->vertices[i].v[Z];
				fx = obj->mesh->vertices[i].v[X];
				fy = obj->mesh->vertices[i].v[Y];
				fz = obj->mesh->vertices[i].v[Z];
				if ((fx-x)>0.5) fx = (float)(x+1);
				else fx = (float)x;
				if ((fy-y)>0.5) fy = (float)(y+1);
				else fy = (float)y;
				if ((fz-z)>0.5) fz = (float)(z+1);
				else fz = (float)z;
				fprintf(fp, "\t%f, %f, %f,\n", fx,fy,fz);
		/*		fprintf(fp, "\t%f, %f, %f,\n", obj->mesh->vertices[i].v[X],
												obj->mesh->vertices[i].v[Y],
												obj->mesh->vertices[i].v[Z]);
		*/	}
		}
		fprintf(fp, "};\n");
	}
}	
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int WriteMeshFaceIndex(FILE *fp, INPUT_OBJECT *obj)
{					 
	int x;
	int numFaces = obj->mesh->numFaces;

	if(obj->mesh->numFaces)
	{
		fprintf(fp, "SHORTVECTOR %s_faceIndex[] = {\n", obj->name);

		if ((IsObjectAnimating(obj) == FALSE) || (userFormat == PC))
//		if((IsObjectAnimating(obj) == FALSE) || ((user == WNR) && (userFormat == PC)))
		{
			for(x = 0; x < obj->mesh->numFaces; x++)
			{
				if (obj->mesh->faceInfo[x].faceFlags & NO_COLL) 
				{
					numFaces --;
					continue;
				}
				if ((obj->mesh->faceInfo[x].faceIndex.v[X]==obj->mesh->faceInfo[x].faceIndex.v[Y])
					||(obj->mesh->faceInfo[x].faceIndex.v[X]==obj->mesh->faceInfo[x].faceIndex.v[Z])
					||(obj->mesh->faceInfo[x].faceIndex.v[Y]==obj->mesh->faceInfo[x].faceIndex.v[Z]))
					fprintf(fp,"//Warning matching index\n");
				
				fprintf(fp, "\t%d, %d, %d,\n", obj->mesh->faceInfo[x].faceIndex.v[X],
												obj->mesh->faceInfo[x].faceIndex.v[Y],
												obj->mesh->faceInfo[x].faceIndex.v[Z]);
			}
		}
		fprintf(fp, "};\n");
	}
	return numFaces;
}	
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteMeshFaceNormals(FILE *fp, INPUT_OBJECT *obj)
{					 
	int x;

	if(obj->mesh->numFaces)
	{
		fprintf(fp, "BYTEVECTOR %s_faceNormals[] = {\n", obj->name);

		for(x = 0; x < obj->mesh->numFaces; x++)
		{
			fprintf(fp, "\t%d, %d, %d, 0,\n", obj->mesh->faceInfo[x].faceNormal.v[X],
											obj->mesh->faceInfo[x].faceNormal.v[Y],
											obj->mesh->faceInfo[x].faceNormal.v[Z]);
		}
		fprintf(fp, "};\n");
	}
}	
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteMeshFaceTC(FILE *fp, INPUT_OBJECT *obj)
{					 
	int x, j;
	float	s, t;
//	float ns, nt;

	if(obj->mesh->numFaces)
	{
		fprintf(fp, "USHORT2DVECTOR %s_faceTC[] = {\n", obj->name);

		for(x = 0; x < obj->mesh->numFaces; x++)
//		for(x = 0; x < obj->mesh->numFaces * 4; x++)
		{

			for(j = 0; j < 3; j++)
			{
				s = obj->mesh->faceInfo[x].faceTC[j].v[X];
				t = obj->mesh->faceInfo[x].faceTC[j].v[Y];

				fprintf(fp, "\t%d, %d,\n", (short)s, (short)t);
			}

		}
		fprintf(fp, "};\n");
	}
}	
void WriteMeshVertexNormals(FILE *fp, INPUT_OBJECT *obj)
{
	int x, y;
	int a, b, c;
	float vx, vy, vz;
	unsigned char tx, ty, tz;

	if(obj->mesh->numVertices && obj->mesh->numFaces)
	{
//		fprintf(fp, "BYTEVECTOR %s_vertexNormals[] = {\n", obj->name);

		if(userFormat == PC)
		{
			fprintf(fp, "QUATERNION %s_vertexNormals[] = {\n", obj->name);
				
			for(x = 0; x < obj->mesh->numFaces; x++)
			{
				for(y = 0; y < 3; y++)
				{
					fprintf(fp, "\t%f, %f, %f, %f,\n",	obj->mesh->faceInfo[x].gouraudColour[y].r/255.0F,
														obj->mesh->faceInfo[x].gouraudColour[y].g/255.0F,
														obj->mesh->faceInfo[x].gouraudColour[y].b/255.0F,
														obj->mesh->faceInfo[x].gouraudColour[y].a/255.0F);
				}
			}
/*
			if((obj->flags & OBJECT_FLAGS_PRELIT))
			{
				fprintf(fp, "QUATERNION %s_vertexNormals[] = {\n", obj->name);
				
				for(x = 0; x < obj->mesh->numFaces; x++)
				{
					for(y = 0; y < 3; y++)
					{
						fprintf(fp, "\t%f, %f, %f, %f,\n",	obj->mesh->faceInfo[x].gouraudColour[y].r/256.0F,
															obj->mesh->faceInfo[x].gouraudColour[y].g/256.0F,
															obj->mesh->faceInfo[x].gouraudColour[y].b/256.0F,
															obj->mesh->faceInfo[x].gouraudColour[y].a/256.0F);
					}
				}
			}
			else
			{
				fprintf(fp, "VECTOR %s_vertexNormals[] = {\n", obj->name);

				for(x = 0; x < obj->mesh->numVertices; x++)
				{
					fprintf(fp, "\t%d, %d, %d, \n", obj->mesh->vertexNormals[x].v[X],
													obj->mesh->vertexNormals[x].v[Y],
													obj->mesh->vertexNormals[x].v[Z]);
				}
			}
*/
		}
		else
		{
			fprintf(fp, "BYTEVECTOR %s_vertexNormals[] = {\n", obj->name);
	
	
			if((obj->flags & OBJECT_FLAGS_PRELIT))
			{
				for(x = 0; x < obj->mesh->numFaces; x++)
				{
					for(y = 0; y < 3; y++)
					{
						fprintf(fp, "\t%d, %d, %d, %d,\n", obj->mesh->faceInfo[x].gouraudColour[y].r,
													obj->mesh->faceInfo[x].gouraudColour[y].g,
													obj->mesh->faceInfo[x].gouraudColour[y].b,
													obj->mesh->faceInfo[x].gouraudColour[y].a);
					}
				}
			}
			else
			{
				for(x = 0; x < obj->mesh->numVertices; x++)
				{
					fprintf(fp, "\t%d, %d, %d, %d,\n", obj->mesh->vertexNormals[x].v[X],
													obj->mesh->vertexNormals[x].v[Y],
													obj->mesh->vertexNormals[x].v[Z],
													obj->mesh->vertexNormals[x].v[3]);
				}
			}
		}

		fprintf(fp, "};\n");
	}

}	
void WriteMeshFaceFlags(FILE *fp, INPUT_OBJECT *obj)
{					 
	int x;

	if(obj->mesh->numFaces)
	{
		fprintf(fp, "char %s_faceFlags[] = {\n", obj->name);

		for(x = 0; x < obj->mesh->numFaces; x++)
		{
			fprintf(fp, "\t%d,\n", obj->mesh->faceInfo[x].faceFlags);
		}
		fprintf(fp, "};\n");
	}
}	

void WriteMeshTextureID(FILE *fp, INPUT_OBJECT *obj)
{					 
	int x;

	if(obj->mesh->numFaces)
	{
		fprintf(fp, "TEXTURE *%s_textureIDs[] = {\n", obj->name);

		for(x = 0; x < obj->mesh->numFaces; x++)
		{
			fprintf(fp, "\t(TEXTURE *)0x%X,\n", obj->mesh->faceInfo[x].textureID);
		}
		fprintf(fp, "};\n");
	}
}	


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
OBJECTINFO *CollisionMeshPresent(OBJECTINFO *obj)
{
	OBJECTINFO *ptr;
	char buf[MAX_PATH];

	for(ptr = objectBanks[selectedObjectBank].objectList.head.next;ptr != &objectBanks[selectedObjectBank].objectList.head;ptr = ptr->next)
	{
		sprintf(buf, "c_%s", obj->filename);
		if(strcmp(buf, ptr->filename) == 0)
		{
			dbprintf("Found collision mesh for %s\n", ptr->filename);
			return ptr;
		}		
	}
	return NULL;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteObjectMesh(INPUT_OBJECT *obj)
{
	int numFaces;
	int collisionMesh = FALSE;

	//only write out mesh definitions if there is no seperate collision mesh
	collisionMesh = (int)CollisionMeshPresent(currObjInfo);
	if(collisionMesh == FALSE)
	{
		WriteMeshVertices(fp, obj);
		numFaces = WriteMeshFaceIndex(fp, obj);

//		WriteMeshVertices(fp, obj);
//		numFaces = WriteMeshFaceIndex(fp, obj);
	}

	//only output this data if the model is to be drawn from mesh definitions
	if(outputDrawList == FALSE)
	{	
		WriteMeshVertexNormals(fp, obj);
		WriteMeshFaceNormals(fp, obj);
		WriteMeshFaceTC(fp, obj);
		WriteMeshFaceFlags(fp, obj);
		WriteMeshTextureID(fp, obj);
	}

	fprintf(fp, "\nMESH %s_mesh = \n{\n", obj->name);	

//	if((IsObjectAnimating(obj) == FALSE) || ((user == WNR) && (userFormat == PC)))
	if ((IsObjectAnimating(obj) == FALSE) || (userFormat == PC))
	{
//		fprintf(fp, "\t%d,\n", obj->mesh->numFaces);
		fprintf(fp, "\t%d,\n", numFaces);
		fprintf(fp, "\t%d,\n", obj->mesh->numVertices);
	}
	else
	{
		fprintf(fp, "\t0,\n");
		fprintf(fp, "\t0,\n");
	}

	if((obj->mesh->numVertices) && (collisionMesh == FALSE))
		fprintf(fp, "\t%s_vertices,\n", obj->name);
	else
		fprintf(fp, "\tNULL,\n", obj->name);

	if((obj->mesh->numFaces) && (collisionMesh == FALSE))
		fprintf(fp, "\t%s_faceIndex,\n", obj->name);
	else
		fprintf(fp, "\tNULL,\n", obj->name);



	if(outputDrawList == FALSE)
	{
		if(obj->mesh->numFaces)
			fprintf(fp, "\t%s_faceNormals,\n", obj->name);
		else
			fprintf(fp, "\tNULL,\n", obj->name);

		if(obj->mesh->numFaces)
			fprintf(fp, "\t%s_faceTC,\n", obj->name);
		else
			fprintf(fp, "\tNULL,\n", obj->name);

		fprintf(fp, "\tNULL,\n");

		if(obj->mesh->numVertices && obj->mesh->numFaces)
			fprintf(fp, "\t(BYTEVECTOR *)%s_vertexNormals,\n", obj->name);
		else
			fprintf(fp, "\tNULL,\n", obj->name);

		if(obj->mesh->numFaces)
			fprintf(fp, "\t%s_faceFlags,\n", obj->name);
		else
			fprintf(fp, "\tNULL,\n", obj->name);


		if(obj->mesh->numFaces)
			fprintf(fp, "\t%s_textureIDs,\n", obj->name);
		else
			fprintf(fp, "\tNULL,\n", obj->name);

	}
	else
	{
		fprintf(fp, "\tNULL,\n");
		fprintf(fp, "\tNULL,\n");
		fprintf(fp, "\tNULL,\n");
		fprintf(fp, "\tNULL,\n");
		fprintf(fp, "\tNULL,\n");
		fprintf(fp, "\tNULL,\n");

	}

	fprintf(fp, "};\n");


	if(currObjInfo->skinned == TRUE)
		return;

	if(obj->children)
		WriteObjectMesh(obj->children);

	if(obj->next)
		WriteObjectMesh(obj->next);
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteObjectMeshes(FILE *fp)
{
	int x;
	OBJECTINFO	*ptr;

	fprintf(fp, "//--------------------Mesh definitions follow------------\n");

	for(ptr = objectBanks[selectedObjectBank].objectList.head.next;ptr != &objectBanks[selectedObjectBank].objectList.head;ptr = ptr->next)
	{
		currObjInfo = ptr;

		if ( ( user == SPIRIT ) && ( userFormat == N64 ) )
		{
			if ( ( ptr->drawList == TRUE ) || ( strstr ( ptr->filename, "c_" ) == ptr->filename ) )
				outputDrawList = TRUE;
			else
				outputDrawList = FALSE;
			// ENDELSEIF - ( ( ptr->drawList == TRUE ) || ( strstr ( ptr->filename, "c_" ) == ptr->filename ) )
		}
		else if ( ( user == SPIRIT ) && ( userFormat == PC ) )
		{
			outputDrawList = FALSE;
		}
		// ENDELSEIF - ( ( user == SPIRIT ) && ( userFormat == N64 ) )

		WriteObjectMesh(ptr->object);
	}


}



////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////skininfo//////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteObjectSkinInfo(INPUT_OBJECT *object, INPUT_OBJECT *hostObject)
{
//	int vertRefs[100][100];
//	int numVertRefs[100];
	int i, j, vertNum;

	int **vertRefs;
	int *numVertRefs;

	vertRefs = (int **)calloc(sizeof(int) * 1500,1);
	for(i=0; i<1500; i++)
		vertRefs[i] = (int *)calloc(sizeof(int) * 1500,1);

	numVertRefs = (int *)calloc(sizeof(int) * 1500,1);
	
	//first need to count references to each vertex in the Vtx list

	if(userFormat != PC)
	{
		for(i = 0; i < object->numVertsInBone; i++)
		{
			numVertRefs[i] = 0;
			vertNum = object->boneVerts[i].vertIndex;
   			for(j = 0; j < hostObject->numVtx; j++)
			{

				if(
				   ((int)hostObject->mesh->vertices[vertNum].v[X] == hostObject->drawListVtxList[j].x) &&
				   ((int)hostObject->mesh->vertices[vertNum].v[Y] == hostObject->drawListVtxList[j].y) &&
				   ((int)hostObject->mesh->vertices[vertNum].v[Z] == hostObject->drawListVtxList[j].z))
				{
					vertRefs[i][numVertRefs[i]] = j;
					if(hostObject->bound)
						vertRefs[i][numVertRefs[i]] += 8;
					numVertRefs[i]++;
				}
			}


		}
	}
	else
	{
		for(i = 0; i < object->numVertsInBone; i++)
		{
			vertNum = object->boneVerts[i].vertIndex;

			hostObject->mesh->vertices[vertNum].v[X] = object->boneVerts[i].offset.v[X];
			hostObject->mesh->vertices[vertNum].v[Y] = object->boneVerts[i].offset.v[Y];
			hostObject->mesh->vertices[vertNum].v[Z] = -object->boneVerts[i].offset.v[Z];

			
		/*	for(j = 0; j < hostObject->numVtx; j++)
			{

				if(
				   ((int)hostObject->mesh->vertices[vertNum].v[X] == hostObject->drawListVtxList[j].x) &&
				   ((int)hostObject->mesh->vertices[vertNum].v[Y] == hostObject->drawListVtxList[j].y) &&
				   ((int)hostObject->mesh->vertices[vertNum].v[Z] == hostObject->drawListVtxList[j].z))
				{
					vertRefs[i][numVertRefs[i]] = j;
					if(hostObject->bound)
						vertRefs[i][numVertRefs[i]] += 8;
					numVertRefs[i]++;
				}
			}*/


		}
	}

	//lay down the references to each bone vertex in the vtx list
/*	for(i = 0; i < object->numVertsInBone; i++)
	{
		fprintf(fp, "\nVtx *%sRvtx%d[] =\n{\n", object->name, i);
		for(j = 0; j < numVertRefs[i]; j++)
		{
			fprintf(fp, "\t&%s_vtxa[%d],\n", hostObject->name, vertRefs[i][j]);
		}
		fprintf(fp, "};\n");
	}
*/
	if(userFormat != PC)
	{

		for(i = 0; i < object->numVertsInBone; i++)
		{
			fprintf(fp, "\ns16 %sRvtx%d[] =\n{\n", object->name, i);
			for(j = 0; j < numVertRefs[i]; j++)
			{
				fprintf(fp, "\t%d,\n", vertRefs[i][j]);
			}
			fprintf(fp, "};\n");
		}


		//define list of references for each bone vert

		// MODIFY - ANDYE
		if(object->numVertsInBone)
		{
			fprintf(fp, "\nSKINVTX %s_effectedVerts[%d] =\n{\n", object->name, object->numVertsInBone);
			for(i = 0; i < object->numVertsInBone; i++)
			{
		/*		fprintf(fp, "\t{");
				for(j = 0; j < numVertRefs[i]; j++)
					fprintf(fp, "%d,", vertRefs[i][j]);

				fprintf(fp, "-1},\n");
		*/
				fprintf(fp, "\t{%sRvtx%d, %d},\n", object->name, i, numVertRefs[i]);
		//		fprintf(fp, "\t{%sRvtx%d, %d},\n", object->name, i, numVertRefs[i]);
			}
			fprintf(fp, "};\n");
		}
	}
	else
	{
		if(object->numVertsInBone)
		{
			fprintf(fp, "\nunsigned long %s_effectedVerts[%d] =\n{\n", object->name, object->numVertsInBone);
			for(i = 0; i < object->numVertsInBone; i++)
				fprintf(fp, "\t%lu,\n", object->boneVerts[i].vertIndex);
			fprintf(fp, "};\n");
		}
	}

	//vertex positions relative to the bone
	if ( (( user == SPIRIT ) && ( userFormat == N64 || userFormat == PC)) && (object->numVertsInBone) )
	{
		fprintf(fp, "\nDUELVECTOR %s_origVerts[] =\n{\n", object->name);
		for(i = 0; i < object->numVertsInBone; i++)
		{
			vertNum = object->boneVerts[i].vertIndex;		
			if(hostObject->mesh->vertexNormals)
			{
				fprintf(fp, "\t%f, %f, %f, %d, %d, %d,\n", object->boneVerts[i].offset.v[X],
											object->boneVerts[i].offset.v[Y], 
											-object->boneVerts[i].offset.v[Z],
											hostObject->mesh->vertexNormals[object->boneVerts[i].vertIndex].v[X],
											hostObject->mesh->vertexNormals[object->boneVerts[i].vertIndex].v[Y],
											hostObject->mesh->vertexNormals[object->boneVerts[i].vertIndex].v[Z]
											);		
			}
			else
			{
				fprintf(fp, "\t%f, %f, %f, 0, 0, 0,\n", object->boneVerts[i].offset.v[X],
											object->boneVerts[i].offset.v[Y], 
											-object->boneVerts[i].offset.v[Z]
											);		

			}
		}
		fprintf(fp, "};\n");
	}
	// ENDIF


	free(numVertRefs);
	for(i=0; i<512; i++)
		free(vertRefs[i]);
	free(vertRefs);

	

	if(object->children)
		WriteObjectSkinInfo(object->children, hostObject);
	if(object->next)
		WriteObjectSkinInfo(object->next, hostObject);

}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteObjectsSkinInfo()
{
	int i = 0;
	OBJECTINFO *ptr;


	for(ptr = objectBanks[selectedObjectBank].objectList.head.next;ptr != &objectBanks[selectedObjectBank].objectList.head;ptr = ptr->next)
	{
		currObjInfo = ptr;
		if(ptr->skinned)
			WriteObjectSkinInfo(ptr->object, ptr->object);

		free(ptr->object->drawListVtxList);
 	}

}