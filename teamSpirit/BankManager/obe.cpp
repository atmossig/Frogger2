#include <windows.h>
#include <commctrl.h>
#include <wingdi.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "types.h"
#include "bitmaps.h"
#include "files.h"
#include "ndos.h"
#include "obe.h"
#include "main.h"
#include "views.h"
#include "crc32.h"
#include "debug.h"
#include "drawlists.h"
#include "resource.h"

//#define SKINNED_FROG

char *obeData;
char *obeDataPtr;
OBECHUNK_HEADER myHeader;
int invertYpos = FALSE;
/*	--------------------------------------------------------------------------------
	Function 	: IsValudOBE
	Purpose 	: check the magic number to see if obe is valid
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
BOOL IsValidOBE()
{
	char mString[4];

	obeRead(mString, 4);
	if((mString[0] == 'O') && (mString[1] == 'B') && (mString[2] == 'E') && (mString[3] == 0))
		return TRUE;
	else
		return FALSE;

}
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void ReadOBEHeader(OBECHUNK_HEADER *header)
{
	obeRead(header, sizeof(OBECHUNK_HEADER));
}

/*	--------------------------------------------------------------------------------
	Function 	: LoadOBEChunk
	Purpose 	: loads in and deals with obe chunks
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int LoadOBEChunk(INPUT_OBJECT **object, OBECHUNK_HEADER *header)
{
	int x;

	switch(header->type)
	{
		case OBECHUNK_OBJECT:
			ReadOBEHeader(header);
			LoadOBEChunk(object, header);
			break;

		case OBECHUNK_MESH:
			LoadOBEChunkMesh(header, object);

			break;

		case OBECHUNK_ANIMSEGMENTS:
			LoadOBEChunkAnimSegments(header, currentObject);
			ReadOBEHeader(header);
			break;

		case OBECHUNK_ANIMTIME:
			LoadOBEChunkAnimTime(header, currentObject);
			ReadOBEHeader(header);
			break;

		case OBECHUNK_SKIN:
			currentObject->skinned = TRUE;
			LoadOBEChunkSkin(header, &currentObject->object);
			AddObjectFlags(currentObject->object, currentObject->object->flags);
			break;

		case OBECHUNK_OBJECTINFO:
			obeDataPtr+=header->length;
			ReadOBEHeader(header);
			dbprintf("Found chunk OBECHUNK_OBJECTINFO\n");
			break;
		case OBECHUNK_MAPPINGICONS:
			obeDataPtr+=header->length;
			ReadOBEHeader(header);
			dbprintf("Found chunk OBECHUNK_MAPPINGICO\n");
			break;
		case OBECHUNK_VIEWPORTS:
			obeDataPtr+=header->length;
			ReadOBEHeader(header);
			dbprintf("Found chunk OBECHUNK_VIEWPORT\n");
			break;
		case OBECHUNK_VERTEXFRAMES:
			obeDataPtr+=header->length;
			ReadOBEHeader(header);
			dbprintf("Found chunk OBECHUNK_VERTEXFRAME\n");
			break;
		case OBECHUNK_CONSTRUCTIONGRID:
			obeDataPtr+=header->length;
			ReadOBEHeader(header);
			dbprintf("Found chunk OBECHUNK_CONSTRUCTIONGRID\n");
			break;
		case OBECHUNK_LIGHTINGINFO:
			obeDataPtr+=header->length;
			ReadOBEHeader(header);
			dbprintf("Found chunk OBECHUNK_LIGHTINGINFO\n");
			break;
		case OBECHUNK_HISTORYENTRY:
			obeDataPtr+=header->length;
			ReadOBEHeader(header);
			dbprintf("Found chunk OBECHUNK_HISTORYENTRY\n");
			break;
		case OBECHUNK_MASTERSORTLIST:
			obeDataPtr+=header->length;
			ReadOBEHeader(header);
			dbprintf("Found chunk OBECHUNK_MASTERSORTLIST\n");
			break;
		case OBECHUNK_SKININFO:
			obeDataPtr+=header->length;
			ReadOBEHeader(header);
			dbprintf("Found chunk OBECHUNK_SKININFO\n");
			break;

	

		default:
			obeDataPtr+=header->length;
			ReadOBEHeader(header);
			break;

	}

	return header->type;

}



/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
BOOL LoadOBEObject(OBJECTINFO *obj)
{
	char buf[MAX_PATH];
	int fileSize = 0;
	FILE *obeFile;

	int result;

	currentObject = obj;


	strlwr(obj->filename);
	strlwr(obj->fullFilename);

	obeFile = fopen(obj->fullFilename, "rb");
	if(obeFile == NULL)
	{
		sprintf(buf, "Can't open OBE %s", obj->fullFilename);
		MessageBox(dlgWnd, buf, "ERROR!", MB_ICONERROR|MB_TASKMODAL);
		return 0;
	}
	dbprintf("opened OBE file %s\n", obj->fullFilename);
	fseek(obeFile, 0, SEEK_END);
	fileSize = ftell(obeFile);
	fseek(obeFile, 0, SEEK_SET);
	obeData = (char *)malloc(fileSize);
	obeDataPtr = obeData;
	fread(obeData, fileSize, 1, obeFile);
	fclose(obeFile);

	//check magic number chunk
	if(IsValidOBE() == FALSE)
	{
		sprintf(buf, "OBE file %s is not of valid format.", obj->fullFilename);
		MessageBox(dlgWnd, buf, "ERROR!", MB_ICONERROR|MB_TASKMODAL);
		return FALSE;
	}


	ReadOBEHeader(&myHeader);
	obj->object = (INPUT_OBJECT *)calloc(1, sizeof(INPUT_OBJECT));

	result = -1;
	if ( ( result = strcmp ( "frogger.obe", obj->filename ) ) == 0 )
#ifdef SKINNED_FROG
		scaleSkinnedObject = 1;
#else
		scaleSkinnedObject = 0;
#endif

	else if ( ( result = strcmp ( "hedge.obe", obj->filename ) ) == 0 )
		scaleSkinnedObject = 1;
	else if ( ( result = strcmp ( "flower.obe", obj->filename ) ) == 0 )
		scaleSkinnedObject = 1;
	else if ( ( result = strcmp ( "mole.obe", obj->filename ) ) == 0 )
		scaleSkinnedObject = 1;
	else
		scaleSkinnedObject = 0;
	// ENDELSEIF
	while(LoadOBEChunk(&obj->object, &myHeader) != OBECHUNK_END);


	free(obeData);
//	fclose(obeFile);
	return TRUE;
}






/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void LoadOBEChunkObject(OBECHUNK_HEADER	*header, OBJECTINFO *obj)
{
	dbprintf("Found chunk object\n");
	currObj = obj->object;
}
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void LoadOBEChunkObjectInfo(OBECHUNK_HEADER	*header, OBJECTINFO *obj)
{
	dbprintf("Found chunk object info\n");
	obeRead(obj->object->name, 32);

	obeDataPtr += (4+4+4+4+32+8+8+32);

	if(header->version >= 2)
		obeDataPtr += 4;
	if(header->version >= 3)
		obeDataPtr += 64;
	if(header->version >= 4)
		obeDataPtr += 1024;


}
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void LoadOBEChunkMesh(OBECHUNK_HEADER *header, INPUT_OBJECT **pObject)
{
	int numKeys, i, j, k, num, num2;
	MESH *mesh;
	INPUT_OBJECT *object;
	char buf[MAX_PATH];
	unsigned char r, g, b, a, tempCh;
	float tempFl;
	int tempInt;
	int vertsInFace;
	int chunkStart, chunkEnd;
	int meshVersion = header->version;
	int	meshLength = header->length;

	if(*pObject == NULL)
		*pObject = (INPUT_OBJECT *)calloc(1, sizeof(INPUT_OBJECT));
	object = *pObject;
	mesh = object->mesh;

	chunkStart = (int)obeDataPtr;//ftell(obeFile);

	obeRead(object->name, 32);
	ValidateFilename(object->name);
	dbprintf("Found chunk mesh %s - ", object->name);

	invertYpos = FALSE;
	LoadOBEKeyframes(object);

//mesh definition
// read in vertex data

	object->mesh = (MESH *)calloc(sizeof(MESH), 1);
	mesh = object->mesh;
	obeRead(&num, 4);
	mesh->numVertices = num;
	dbprintf("%d vertices; ", mesh->numVertices);
	mesh->vertices = (VECTOR *)calloc(sizeof(VECTOR) * mesh->numVertices, 1);
	for(i = 0; i < mesh->numVertices; i++)
	{
		//handle X
		obeRead(&num, 4);	//read num vertex positions (hopefully one!)
		for(j = 0; j < num; j++)
		{
			obeRead(&num2, 4);	//read time for vertex pos
			obeRead(&mesh->vertices[i].v[X], 4);
			if ( scaleSkinnedObject )
				mesh->vertices[i].v[X] *= scaleVal;
			// ENDIF
		}

		//handle Y
		obeRead(&num, 4);	//read num vertex positions (hopefully one!)
		for(j = 0; j < num; j++)
		{
			obeRead(&num2, 4);	//read time for vertex pos
			obeRead(&mesh->vertices[i].v[Y], 4);
			if ( scaleSkinnedObject )
				mesh->vertices[i].v[Y] *= scaleVal;
		}

		//handle Z
		obeRead(&num, 4);	//read num vertex positions (hopefully one!)
		for(j = 0; j < num; j++)
		{
			obeRead(&num2, 4);	//read time for vertex pos
			obeRead(&mesh->vertices[i].v[Z], 4);
			mesh->vertices[i].v[Z] = -mesh->vertices[i].v[Z];
			if ( scaleSkinnedObject )
				mesh->vertices[i].v[Z] *= scaleVal;
		}

		//skip past vertex state, not used
		obeDataPtr++;

	}

//read in poly data

	obeRead(&num, 4);
	mesh->numFaces = num;
	dbprintf("%d faces\n", mesh->numFaces);
	mesh->faceInfo = (FACE_INFO *)calloc(sizeof(FACE_INFO) * mesh->numFaces, 1);

	for(i = 0; i < mesh->numFaces; i++)
	{
		obeRead(mesh->faceInfo[i].textureID.name, 32);	
		mesh->faceInfo[i].textureID.crc = UpdateCRC(0, mesh->faceInfo[i].textureID.name, strlen(mesh->faceInfo[i].textureID.name));
		mesh->faceInfo[i].colour.r = *(obeDataPtr++);
		mesh->faceInfo[i].colour.g = *(obeDataPtr++);
		mesh->faceInfo[i].colour.b = *(obeDataPtr++);
		mesh->faceInfo[i].colour.a = *(obeDataPtr++);
		
		if(header->version >= 5)
		{
			tempCh = *(obeDataPtr++);	//poly translucency mode
			tempCh = *(obeDataPtr++);	// poly gouraud shading mode
		}

		obeRead(&mesh->faceInfo[i].textureID.tileX, 4);	//u-tile
		obeRead(&mesh->faceInfo[i].textureID.tileY, 4);	//u-tile

		tempCh = *(obeDataPtr++);	//poly selection

		obeRead(&mesh->faceInfo[i].flags, 2);

		obeRead(&mesh->faceInfo[i].vertsInFace, 4);	//num vertices per poly		

		if(mesh->faceInfo[i].flags & 0x0010)	//if face is a sprite
		{
			//malloc space for sprites if not there already
			if(object->sprites == NULL)//warning - assumes max 10 sprites per object
				object->sprites = (SPRITE_TYPE *)malloc(sizeof(SPRITE_TYPE) * 10);

			//read in xpos
			obeRead(&k, 4);
			for(j = 0; j < k; j++)
			{
				obeRead(&tempInt, 4);	//time val for animvect
				obeRead(&tempFl, 4);
				object->sprites[object->numSprites].x = (short)tempFl; 
			}
			//read in ypos
			obeRead(&k, 4);
			for(j = 0; j < k; j++)
			{
				obeRead(&tempInt, 4);	//time val for animvect
				obeRead(&tempFl, 4);
				object->sprites[object->numSprites].y = (short)tempFl; 
			}
			//read in zpos
			obeRead(&k, 4);
			for(j = 0; j < k; j++)
			{
				obeRead(&tempInt, 4);	//time val for animvect
				obeRead(&tempFl, 4);
				object->sprites[object->numSprites].z = (short)tempFl; 
			}
			//read in xsize
			obeRead(&k, 4);
			for(j = 0; j < k; j++)
			{
				obeRead(&tempInt, 4);	//time val for animvect
				obeRead(&tempFl, 4);
				object->sprites[object->numSprites].sx = (short)tempFl; 
			}
			//read in ysize
			obeRead(&k, 4);
			for(j = 0; j < k; j++)
			{
				obeRead(&tempInt, 4);	//time val for animvect
				obeRead(&tempFl, 4);
				object->sprites[object->numSprites].sy = (short)tempFl; 
			}
			
			object->sprites[object->numSprites].flags = SPRITE_TRANSLUCENT;
			if(*(obeDataPtr++))
				object->sprites[object->numSprites].flags |= SPRITE_FLIPX;
			if(*(obeDataPtr++))
				object->sprites[object->numSprites].flags |= SPRITE_FLIPY;


			object->sprites[object->numSprites].ox = mesh->faceInfo[i].textureID.sx / 2;
			object->sprites[object->numSprites].oy = mesh->faceInfo[i].textureID.sy / 2;
			object->sprites[object->numSprites].textureID = mesh->faceInfo[i].textureID.crc;
			object->numSprites++;
		}
		else
		{
			for(j = 0; j < mesh->faceInfo[i].vertsInFace; j++)
			{
				obeRead(&mesh->faceInfo[i].faceIndex.v[j], 2);
				tempCh = *(obeDataPtr++);	//lighting intensity
				obeRead(&tempFl, 4);
				mesh->faceInfo[i].obeTC[j].x = tempFl;
				obeRead(&tempFl, 4);
				mesh->faceInfo[i].obeTC[j].z = tempFl;
				if(header->version >= 5)
				{
					mesh->faceInfo[i].gouraudColour[j].r = *(obeDataPtr++);
					mesh->faceInfo[i].gouraudColour[j].g = *(obeDataPtr++);
					mesh->faceInfo[i].gouraudColour[j].b = *(obeDataPtr++);
				}

			}
		}

	}

	
	//mesh flags
	obeRead(&mesh->meshFlags, 2);
	object->objColour.r = *(obeDataPtr++);
	object->objColour.g = *(obeDataPtr++);
	object->objColour.b = *(obeDataPtr++);

	if(header->version >= 5)
	{
		tempCh = *(obeDataPtr++);
		if(tempCh == 1)	
			object->flags |= OBJECT_FLAGS_GOURAUD_SHADED;
		if(tempCh == 2)
			object->flags |= OBJECT_FLAGS_GOURAUD_SHADED + OBJECT_FLAGS_PRELIT;

		tempCh = *(obeDataPtr++);
		object->xluFactor = tempCh;

		tempCh = *(obeDataPtr++);//translucency mode, not used

	}

	if(header->version >= 2)
	{
		tempCh = *(obeDataPtr++); // mesh selected
	}

	//skip past sortlists
	if(header->version >= 6)
	{
		for(i = 0; i < 8; i++)
		{
			obeRead(&tempInt, 4);
			obeDataPtr += 4 * tempInt;
		}

	}


	//sort out flags 'n stuff
	RestoreObjectFlags(object);


	//read children
	ReadOBEHeader(header);
	if(header->type == OBECHUNK_MESH)
	{
		object->numChildren++;
		LoadOBEChunkMesh(header, &object->children);
	}

		
	//genesis data (need to skip this by seeking to the end of the chunk and working backwards)
	tempInt = chunkStart + meshLength;	//point to pos in file of end of chunk
	if(meshVersion >= 8)
		tempInt -= mesh->numFaces * 4;		//terrain types
	if(meshVersion >= 9)
		tempInt -= mesh->numFaces * 4;		//ndo flags
	if(meshVersion >= 10)
		tempInt -= mesh->numFaces * 36;		//overlay texture
	if(meshVersion >= 11)
		tempInt -= 32;						//phong texture

	obeDataPtr = (char *)tempInt;
//	fseek(obeFile, tempInt, SEEK_SET);	//set pos in file



	//read in extra info at end of mesh chunk
	if(meshVersion >= 8)
	{
		for(i = 0; i < mesh->numFaces; i++)
			obeRead(&mesh->faceInfo[i].terrainType, 4);
	}
	if(meshVersion >= 9)
	{
		for(i = 0; i < mesh->numFaces; i++)
			obeRead(&mesh->faceInfo[i].NDOFlags, 4);
	}
	if(meshVersion >= 10)
	{
		for(i = 0; i < mesh->numFaces; i++)
		{
			obeRead(buf, 32);
			object->mesh->faceInfo[i].duelTexID = UpdateCRC(0, buf, strlen(buf));
			if(strstr(buf, ".bmp") == NULL)
			{
				object->mesh->faceInfo[i].duelTexID = 0;				
			}
			object->mesh->faceInfo[i].vAlpha.v[0] = *(obeDataPtr++);
			object->mesh->faceInfo[i].vAlpha.v[1] = *(obeDataPtr++);
			object->mesh->faceInfo[i].vAlpha.v[2] = *(obeDataPtr++);
			object->mesh->faceInfo[i].vAlpha.v[3] = *(obeDataPtr++);

			if((userFormat == PC))
			{
				if(mesh->meshFlags & 0x40)
				{
					// get alpha vertex data
					object->mesh->faceInfo[i].gouraudColour[0].a = object->mesh->faceInfo[i].vAlpha.v[0];
					object->mesh->faceInfo[i].gouraudColour[1].a = object->mesh->faceInfo[i].vAlpha.v[1];
					object->mesh->faceInfo[i].gouraudColour[2].a = object->mesh->faceInfo[i].vAlpha.v[2];
					object->mesh->faceInfo[i].gouraudColour[3].a = object->mesh->faceInfo[i].vAlpha.v[3];
				}
				else
				{
					// set to max alpha
					object->mesh->faceInfo[i].gouraudColour[0].a = 255;
					object->mesh->faceInfo[i].gouraudColour[1].a = 255;
					object->mesh->faceInfo[i].gouraudColour[2].a = 255;
					object->mesh->faceInfo[i].gouraudColour[3].a = 255;
				}
			}
		}
	}
	else
	{
		for(i = 0; i < mesh->numFaces; i++)
		{
			if((userFormat == PC))
			{
				// set to max alpha
				object->mesh->faceInfo[i].gouraudColour[0].a = 255;
				object->mesh->faceInfo[i].gouraudColour[1].a = 255;
				object->mesh->faceInfo[i].gouraudColour[2].a = 255;
				object->mesh->faceInfo[i].gouraudColour[3].a = 255;
			}
		}
	}


	if(meshVersion >= 11)
	{
		obeRead(buf, 32);
		object->phongTexture = UpdateCRC(0, buf, strlen(buf));
		if(strstr(buf, ".bmp"))
		{
			object->flags |= OBJECT_FLAGS_PHONG;
		}
		else
			object->phongTexture = 0;
	}


	ReStructureMeshFaces(object);


	ReadOBEHeader(header);
	if(header->type == OBECHUNK_MESH)
		LoadOBEChunkMesh(header, &object->next);

	if(object->children)
	{
		sprintf(object->childName, "%s", object->children->name);
		ValidateFilename(object->childName);
	}

	if(object->next)
	{
		sprintf(object->nextName, "%s", object->next->name);
		ValidateFilename(object->nextName);
	}


}





/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void LoadOBEChunkAnimTime(OBECHUNK_HEADER *header, OBJECTINFO *obj)
{
	int tempInt;

	obj->animStart = 0;
	obeRead(&obj->animEnd, 4);
	obeRead(&obj->defaultAnimSpeed, 4);
	obeRead(&tempInt, 4);	//frame displayed

}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void LoadOBEChunkAnimSegments(OBECHUNK_HEADER *header, OBJECTINFO *obj)
{
	int i, tempInt;

	dbprintf("Found chunk animsegs\n");
	
	obeRead(&obj->numAnimRanges, 4);
	obj->ranges = (ANIMRANGE *)malloc(sizeof(ANIMRANGE) * obj->numAnimRanges);
	for(i = 0; i < obj->numAnimRanges; i++)
	{
		obeRead(&obj->ranges[i].segmentident, 32);
		obeRead(&obj->ranges[i].firstframe, 4);
		obeRead(&obj->ranges[i].lastframe, 4);
		obeRead(&obj->ranges[i].flags, 4);
		if(header->version >= 2)
		{
			obeRead(&tempInt, 4);
			if(tempInt == 0)
				tempInt = 20;//obj->defaultAnimSpeed;
			obj->ranges[i].speed = 20 / (float)tempInt;	
		}
		if(header->version >= 3)
			obeRead(&tempInt, 4);	//custom data
	}

}
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void LoadOBEChunkSkin(OBECHUNK_HEADER *header, INPUT_OBJECT **pObject)
{
	int i, j, k, l, tempInt;
	short numWeights;
	char buf[32];
	INPUT_OBJECT *object;


	if(*pObject == NULL)
		*pObject = (INPUT_OBJECT *)calloc(1, sizeof(INPUT_OBJECT));
	object = *pObject;


	dbprintf("Found chunk skin\n");


	obeRead(&object->name, 32);
	obeRead(&object->boneName, 32);
	obeRead(&object->numVertsInBone, 4);

	object->boneVerts = (VERTASSIGN *)malloc(sizeof(VERTASSIGN) * object->numVertsInBone);
	
	for(i = 0; i < object->numVertsInBone; i++)
	{
		obeRead(&object->boneVerts[i].vertIndex, 2);
		//xoffset of vertex
		obeRead(&k, 4);
		for(j = 0; j < k; j++)
		{
			obeRead(&tempInt, 4);
			obeRead(&object->boneVerts[i].offset.v[X], 4);
			if ( scaleSkinnedObject )
				object->boneVerts[i].offset.v[X] *= scaleVal;
		}
		//yoffset of vertex
		obeRead(&k, 4);
		for(j = 0; j < k; j++)
		{
			obeRead(&tempInt, 4);
			obeRead(&object->boneVerts[i].offset.v[Y], 4);
			if ( scaleSkinnedObject )
				object->boneVerts[i].offset.v[Y] *= scaleVal;
		}
		//zoffset of vertex
		obeRead(&k, 4);
		for(j = 0; j < k; j++)
		{
			obeRead(&tempInt, 4);
			obeRead(&object->boneVerts[i].offset.v[Z], 4);
			if ( scaleSkinnedObject )
				object->boneVerts[i].offset.v[Z] *= scaleVal;
		}
		obeDataPtr++;	//selection state
		//simply skip past weights for now
		obeRead(&numWeights, 2);	//number of wieghts
//		for(j = 0; j < numWeights; j++)
		{
			obeRead(&k, 4);		//animvect of weights
			for(l = 0; l < k; l++)
			{
				obeRead(&tempInt, 4);
				obeRead(&tempInt, 4);
			}
		}
		
	}


	if(object->moveKeys)
		free(object->moveKeys);
	if(object->rotateKeys)
		free(object->rotateKeys);
	if(object->scaleKeys)
		free(object->scaleKeys);

	invertYpos = TRUE;
	LoadOBEKeyframes(object);	





	
	ReadOBEHeader(header);
	if(header->type == OBECHUNK_SKIN)
	{
		object->numChildren++;
		LoadOBEChunkSkin(header, &object->children);
	}


	ReadOBEHeader(header);
	if(header->type == OBECHUNK_SKIN)
		LoadOBEChunkSkin(header, &object->next);


	if(object->children)
		sprintf(object->childName, "%s", object->children->name);

	if(object->next)
		sprintf(object->nextName, "%s", object->next->name);

	
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CrossProduct (BYTEVECTOR *res, VECTOR *vr1, VECTOR *vr2, VECTOR *vr3)
{
	VECTOR u, v, ret;


	u.v[X] = vr1->v[X] - vr2->v[X];
	u.v[Y] = vr1->v[Y] - vr2->v[Y];
	u.v[Z] = vr1->v[Z] - vr2->v[Z];

	v.v[X] = vr1->v[X] - vr3->v[X];
	v.v[Y] = vr1->v[Y] - vr3->v[Y];
	v.v[Z] = vr1->v[Z] - vr3->v[Z];

	ret.v[X] = (u.v[Y] * v.v[Z]) - (u.v[Z] * v.v[Y]);
	ret.v[Y] = (u.v[Z] * v.v[X]) - (u.v[X] * v.v[Z]); 
	ret.v[Z] = (u.v[X] * v.v[Y]) - (u.v[Y] * v.v[X]); 
		
	MakeUnit(&ret);
	ret.v[X] *= 127;
	ret.v[Y] *= 127;
	ret.v[Z] *= 127;
	res->v[X] = (signed char)ret.v[X];
	res->v[Y] = (signed char)ret.v[Y];
	res->v[Z] = (signed char)ret.v[Z];
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
float Magnitude(VECTOR *vect)
{
	return sqrt(vect->v[X]*vect->v[X] + vect->v[Y]*vect->v[Y] + vect->v[Z]*vect->v[Z]);
}

/*	--------------------------------------------------------------------------------
	Function 	: MakeUnit()
	Purpose 	: makes a vector unit magnitude
	Parameters 	: pointer to vector
	Returns 	: none
	Info 		:
*/
void MakeUnit(VECTOR *vect)
{
	float m = Magnitude(vect);

	if(m != 0)
	{
		vect->v[X] /= m;
		vect->v[Y] /= m;
		vect->v[Z] /= m;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CalculateVertexNormals(MESH *mesh)
{
	int numFaceRef = 0, ix, iy, iz, i, j;
	short faceRef[100];
	VECTOR tempVect;

	for(i = 0; i < mesh->numVertices; i++)
	{
		numFaceRef = 0;
		for(j = 0; j < mesh->numFaces; j++)
		{
			ix = mesh->faceInfo[j].faceIndex.v[X];
			iy = mesh->faceInfo[j].faceIndex.v[Y];
			iz = mesh->faceInfo[j].faceIndex.v[Z];
			if((ix == i) || (iy == i) || (iz == i))
			{
				//this vertex is referenced by a face..
				faceRef[numFaceRef] = j;
				numFaceRef++;
				if(numFaceRef > 100)
					dbprintf("ERROR: too many face references in create vertex normals!\n");
			}
		}


		tempVect.v[X] = tempVect.v[Y] = tempVect.v[Z] = 0.0;
		for(j = 0; j < numFaceRef; j++)
		{
			tempVect.v[X] += mesh->faceInfo[faceRef[j]].faceNormal.v[X];
			tempVect.v[Y] += mesh->faceInfo[faceRef[j]].faceNormal.v[Y];
			tempVect.v[Z] += mesh->faceInfo[faceRef[j]].faceNormal.v[Z];
		}

		MakeUnit(&tempVect);
		tempVect.v[X] *= 127;
		tempVect.v[Y] *= 127;
		tempVect.v[Z] *= 127;
		mesh->vertexNormals[i].v[X] = (char)tempVect.v[X];
		mesh->vertexNormals[i].v[Y] = (char)tempVect.v[Y];
		mesh->vertexNormals[i].v[Z] = (char)tempVect.v[Z];

	}

}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FillInGouraudColours(MESH *mesh)
{
	int i, j;

	for(i = 0; i < mesh->numFaces; i++)
	{
		for(j = 0; j < 3/*mesh->faceInfo[i].vertsInFace*/; j++)
		{

			mesh->vertexNormals[i * 3 + j].v[X] = mesh->faceInfo[i].gouraudColour[j].r;
			mesh->vertexNormals[i * 3 + j].v[Y] = mesh->faceInfo[i].gouraudColour[j].g;
			mesh->vertexNormals[i * 3 + j].v[Z] = mesh->faceInfo[i].gouraudColour[j].b;

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
void RestoreObjectFlags(INPUT_OBJECT *object)
{	
	char buf[MAX_PATH];
	MESH *mesh = object->mesh;
	int texPresent = FALSE;
	int i, j;
	BITMAP_TYPE *picy;
	int bmpLoaded = FALSE;
	float multi, tempFl;


/*	//object is static
	if((object->numMoveKeys == 1) && (object->children == NULL) && (object->next == NULL))
	{
		object->moveKeys[0].u.vect.v[X] = 0.0;
		object->moveKeys[0].u.vect.v[Y] = 0.0;
		object->moveKeys[0].u.vect.v[Z] = 0.0;

	}
*/

	//sort out face / vertex normals
	for(i = 0; i < 	mesh->numFaces; i++)
	{
		int ix, iy, iz;
		ix = mesh->faceInfo[i].faceIndex.v[X];
		iy = mesh->faceInfo[i].faceIndex.v[Y];
		iz = mesh->faceInfo[i].faceIndex.v[Z];
		CrossProduct(&mesh->faceInfo[i].faceNormal, &mesh->vertices[ix], &mesh->vertices[iy], &mesh->vertices[iz]);
	}



	//check to see if any textures are present on object
	for(i = 0; i < mesh->numFaces; i++)
	{
		if(mesh->faceInfo[i].textureID.crc != 0)
			texPresent = TRUE;
	}
	if(texPresent == FALSE)		//if there are no textures, object must be face coloured
	{
		object->flags |= OBJECT_FLAGS_FACECOL;
		for(i = 0; i < mesh->numFaces; i++)
		{
			mesh->faceInfo[i].textureID.crc = mesh->faceInfo[i].colour.r << 24;
			mesh->faceInfo[i].textureID.crc += mesh->faceInfo[i].colour.g << 16;
			mesh->faceInfo[i].textureID.crc += mesh->faceInfo[i].colour.b << 8;
			mesh->faceInfo[i].textureID.crc += mesh->faceInfo[i].colour.a;
		}
	}



	//set translucent------------------------------------------------
	if(object->xluFactor < 255)
		object->flags |= OBJECT_FLAGS_XLU;

	//set duel texture stuff-----------------------------------------
 	for(i = 0; i < mesh->numFaces; i++)
	{
		if(mesh->faceInfo[i].duelTexID != 0)
		{
			object->flags |= OBJECT_FLAGS_TEXTURE_BLEND;
			break;
		}
	}

	//object colour--------------------------------------------------
	if(mesh->meshFlags & 0x10)
		object->flags |= OBJECT_FLAGS_OBJECT_COLOUR;


	//magenta mask present in some bitmaps used----------------------
	if(mesh->meshFlags & 0x20)
		object->flags |= OBJECT_FLAGS_TRANSPARENT;


	//textures-------------------------------------------------------
	if((object->flags & OBJECT_FLAGS_FACECOL) == 0)
	{
		int lastTexID = 0;
		bmpLoaded = FALSE;
		for(i = 0; i < mesh->numFaces; i++)
		{

			lastTexID = mesh->faceInfo[i].textureID.crc;

			picy = NeedToLoadOBEBitmap(mesh->faceInfo[i].textureID.name);
			if(picy)
			{
				mesh->faceInfo[i].textureID.sx = picy->bmpObj->width;
				mesh->faceInfo[i].textureID.sy = picy->bmpObj->height;
				mesh->faceInfo[i].textureID.colourDepth = picy->bmpObj->bpp;

			}

			//restore tiling values
			if(mesh->faceInfo[i].textureID.tileX > 1000)
				mesh->faceInfo[i].faceFlags |= U_TILE;

			if(mesh->faceInfo[i].textureID.tileY > 1000)
				mesh->faceInfo[i].faceFlags |= V_TILE;

			//restore texture coords
			switch(mesh->faceInfo[i].textureID.sx)
			{
				case  8:	multi =  256.0;break;
				case 16:	multi =  512.0;break;
				case 32:	multi = 1024.0;break;
				case 64:	multi = 2048.0;break;
			};

			if(userFormat == PC)
			{
				multi = 1024.0F;
			}

			for(j = 0; j < mesh->faceInfo[i].vertsInFace; j++)
			{
				tempFl = mesh->faceInfo[i].obeTC[j].x * multi;
				tempFl *= mesh->faceInfo[i].textureID.tileX;
				tempFl /= 1000;
				mesh->faceInfo[i].faceTC[j].v[X] = (int)tempFl;
			
				tempFl = mesh->faceInfo[i].obeTC[j].z * multi;
				tempFl *= mesh->faceInfo[i].textureID.tileY;
				tempFl /= 1000;
				mesh->faceInfo[i].faceTC[j].v[Y] = (int)tempFl;
			}



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
void ReStructureMeshFaces(INPUT_OBJECT *object)
{
	int numTriangles = 0, i, tri = 0, ti;
	FACE_INFO *newFaceInfo;		
	MESH *mesh = object->mesh;

	//count actual number of triangles
	for(i = 0; i < mesh->numFaces; i++)
	{
		if(mesh->faceInfo[i].flags & 0x0010)
			continue;
				//dont count sprites
		numTriangles++;
		if(mesh->faceInfo[i].vertsInFace == 4)
			numTriangles++;
		if(mesh->faceInfo[i].flags & 0x0004)
		{
			if(mesh->faceInfo[i].vertsInFace == 4)
				numTriangles += 2;
			else
				numTriangles++;
		}
	}
	
	newFaceInfo = (FACE_INFO *)malloc(sizeof(FACE_INFO) * numTriangles);

	//now step through and re-construct face data
	tri = 0;
	for(i = 0; i < mesh->numFaces; i++)
	{
		if(mesh->faceInfo[i].flags & 0x0010)
			continue;	//dont count sprites
			
		newFaceInfo[tri++] = mesh->faceInfo[i];

		if(mesh->faceInfo[i].vertsInFace == 4)
		{
			newFaceInfo[tri-1].faceIndex.v[0] = mesh->faceInfo[i].faceIndex.v[0];
			newFaceInfo[tri-1].faceIndex.v[1] = mesh->faceInfo[i].faceIndex.v[1];
			newFaceInfo[tri-1].faceIndex.v[2] = mesh->faceInfo[i].faceIndex.v[2];
			newFaceInfo[tri-1].faceTC[0] = mesh->faceInfo[i].faceTC[0];
			newFaceInfo[tri-1].faceTC[1] = mesh->faceInfo[i].faceTC[1];
			newFaceInfo[tri-1].faceTC[2] = mesh->faceInfo[i].faceTC[2];
			newFaceInfo[tri-1].gouraudColour[0] = mesh->faceInfo[i].gouraudColour[0];
			newFaceInfo[tri-1].gouraudColour[1] = mesh->faceInfo[i].gouraudColour[1];
			newFaceInfo[tri-1].gouraudColour[2] = mesh->faceInfo[i].gouraudColour[2];
			newFaceInfo[tri-1].faceFlags |= QUAD;
		
			newFaceInfo[tri] = mesh->faceInfo[i];
			newFaceInfo[tri].faceIndex.v[0] = mesh->faceInfo[i].faceIndex.v[0];
			newFaceInfo[tri].faceIndex.v[1] = mesh->faceInfo[i].faceIndex.v[2];
			newFaceInfo[tri].faceIndex.v[2] = mesh->faceInfo[i].faceIndex.v[3];
			newFaceInfo[tri].faceTC[0] = mesh->faceInfo[i].faceTC[0];
			newFaceInfo[tri].faceTC[1] = mesh->faceInfo[i].faceTC[2];
			newFaceInfo[tri].faceTC[2] = mesh->faceInfo[i].faceTC[3];
			newFaceInfo[tri].gouraudColour[0] = mesh->faceInfo[i].gouraudColour[0];
			newFaceInfo[tri].gouraudColour[1] = mesh->faceInfo[i].gouraudColour[2];
			newFaceInfo[tri].gouraudColour[2] = mesh->faceInfo[i].gouraudColour[3];

			tri++;
		}
  
  		if(mesh->faceInfo[i].flags & 0x0004)
		{
			newFaceInfo[tri++] = mesh->faceInfo[i];
			if(mesh->faceInfo[i].vertsInFace == 4)
			{

				newFaceInfo[tri-1].faceIndex.v[0] = mesh->faceInfo[i].faceIndex.v[2];
				newFaceInfo[tri-1].faceIndex.v[1] = mesh->faceInfo[i].faceIndex.v[1];
				newFaceInfo[tri-1].faceIndex.v[2] = mesh->faceInfo[i].faceIndex.v[0];
				newFaceInfo[tri-1].faceTC[0] = mesh->faceInfo[i].faceTC[2];
				newFaceInfo[tri-1].faceTC[1] = mesh->faceInfo[i].faceTC[1];
				newFaceInfo[tri-1].faceTC[2] = mesh->faceInfo[i].faceTC[0];
				newFaceInfo[tri-1].gouraudColour[0] = mesh->faceInfo[i].gouraudColour[2];
				newFaceInfo[tri-1].gouraudColour[1] = mesh->faceInfo[i].gouraudColour[1];
				newFaceInfo[tri-1].gouraudColour[2] = mesh->faceInfo[i].gouraudColour[0];
				newFaceInfo[tri-1].faceFlags |= QUAD;
			
				newFaceInfo[tri] = mesh->faceInfo[i];
				newFaceInfo[tri].faceIndex.v[0] = mesh->faceInfo[i].faceIndex.v[3];
				newFaceInfo[tri].faceIndex.v[1] = mesh->faceInfo[i].faceIndex.v[2];
				newFaceInfo[tri].faceIndex.v[2] = mesh->faceInfo[i].faceIndex.v[0];
				newFaceInfo[tri].faceTC[0] = mesh->faceInfo[i].faceTC[3];
				newFaceInfo[tri].faceTC[1] = mesh->faceInfo[i].faceTC[2];
				newFaceInfo[tri].faceTC[2] = mesh->faceInfo[i].faceTC[0];
				newFaceInfo[tri].gouraudColour[0] = mesh->faceInfo[i].gouraudColour[3];
				newFaceInfo[tri].gouraudColour[1] = mesh->faceInfo[i].gouraudColour[2];
				newFaceInfo[tri].gouraudColour[2] = mesh->faceInfo[i].gouraudColour[0];

				tri++;
			}
			else
			{
				newFaceInfo[tri-1].faceIndex.v[0] = mesh->faceInfo[i].faceIndex.v[2];
				newFaceInfo[tri-1].faceIndex.v[1] = mesh->faceInfo[i].faceIndex.v[1];
				newFaceInfo[tri-1].faceIndex.v[2] = mesh->faceInfo[i].faceIndex.v[0];
				newFaceInfo[tri-1].faceTC[0] = mesh->faceInfo[i].faceTC[2];
				newFaceInfo[tri-1].faceTC[1] = mesh->faceInfo[i].faceTC[1];
				newFaceInfo[tri-1].faceTC[2] = mesh->faceInfo[i].faceTC[0];
				newFaceInfo[tri-1].gouraudColour[0] = mesh->faceInfo[i].gouraudColour[2];
				newFaceInfo[tri-1].gouraudColour[1] = mesh->faceInfo[i].gouraudColour[1];
				newFaceInfo[tri-1].gouraudColour[2] = mesh->faceInfo[i].gouraudColour[0];
			}
			



		}
		
  
  	}

	free(mesh->faceInfo);
	mesh->faceInfo = newFaceInfo;
	mesh->numFaces = numTriangles;





	if((object->flags & OBJECT_FLAGS_PRELIT) == 0)
	{
		mesh->vertexNormals = (BYTEVECTOR *)calloc(sizeof(BYTEVECTOR) * mesh->numVertices, 1);
		CalculateVertexNormals(mesh);
	}

}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void LoadOBEKeyframes(INPUT_OBJECT *object)
{
	int i;
	
	//sort out movement keys
	//xpos
	obeRead(&object->numMoveKeys, 4);
	object->moveKeys = (KEYFRAME *)calloc(sizeof(KEYFRAME) * object->numMoveKeys, 1);
	for(i = 0; i < object->numMoveKeys; i++)
	{
		obeRead(&object->moveKeys[i].time, 4);
		obeRead(&object->moveKeys[i].u.vect.v[X], 4);
		if ( scaleSkinnedObject )
			object->moveKeys[i].u.vect.v[X] *= scaleVal;
	}

	//ypos
	obeRead(&object->numMoveKeys, 4);
	for(i = 0; i < object->numMoveKeys; i++)
	{
		obeRead(&object->moveKeys[i].time, 4);
		obeRead(&object->moveKeys[i].u.vect.v[Y], 4);
		if ( scaleSkinnedObject )
			object->moveKeys[i].u.vect.v[Y] *= scaleVal;
	}

	//zpos
	obeRead(&object->numMoveKeys, 4);
	for(i = 0; i < object->numMoveKeys; i++)
	{
		obeRead(&object->moveKeys[i].time, 4);
		obeRead(&object->moveKeys[i].u.vect.v[Z], 4);
		object->moveKeys[i].u.vect.v[Z] = -object->moveKeys[i].u.vect.v[Z];
		if ( scaleSkinnedObject )
			object->moveKeys[i].u.vect.v[Z] *= scaleVal;
	}

	//sort out scale keys
	//xscale
	obeRead(&object->numScaleKeys, 4);
	object->scaleKeys = (KEYFRAME *)calloc(sizeof(KEYFRAME) * object->numScaleKeys, 1);
	for(i = 0; i < object->numScaleKeys; i++)
	{
		obeRead(&object->scaleKeys[i].time, 4);
		obeRead(&object->scaleKeys[i].u.vect.v[X], 4);
//		if ( scaleSkinnedObject )
//			object->scaleKeys[i].u.vect.v[X] *= scaleVal;
	}

	//yscale
	obeRead(&object->numScaleKeys, 4);
	for(i = 0; i < object->numScaleKeys; i++)
	{
		obeRead(&object->scaleKeys[i].time, 4);
		obeRead(&object->scaleKeys[i].u.vect.v[Y], 4);
//		if ( scaleSkinnedObject )
//			object->scaleKeys[i].u.vect.v[Y] *= scaleVal;
	}

	//zscale
	obeRead(&object->numScaleKeys, 4);				 
	for(i = 0; i < object->numScaleKeys; i++)
	{
		obeRead(&object->scaleKeys[i].time, 4);
		obeRead(&object->scaleKeys[i].u.vect.v[Z], 4);
//		if ( scaleSkinnedObject )
//			object->scaleKeys[i].u.vect.v[Z] *= scaleVal;
	}


	//sort out rotaion keys
	//xrot
	obeRead(&object->numRotateKeys, 4);
	object->rotateKeys = (KEYFRAME *)calloc(sizeof(KEYFRAME) * object->numRotateKeys, 1);
	for(i = 0; i < object->numRotateKeys; i++)
	{
		obeRead(&object->rotateKeys[i].time, 4);
		obeRead(&object->rotateKeys[i].u.quat.x, 4);
//		if ( scaleSkinnedObject )
//			object->rotateKeys[i].u.quat.x *= scaleVal;
	}

	//yrot
	obeRead(&object->numRotateKeys, 4);
	for(i = 0; i < object->numRotateKeys; i++)
	{
		obeRead(&object->rotateKeys[i].time, 4);
		obeRead(&object->rotateKeys[i].u.quat.y, 4);
//		if ( scaleSkinnedObject )
//			object->rotateKeys[i].u.quat.y *= scaleVal;
	}

	//zrot
	obeRead(&object->numRotateKeys, 4);
	for(i = 0; i < object->numRotateKeys; i++)
	{
		obeRead(&object->rotateKeys[i].time, 4);
		obeRead(&object->rotateKeys[i].u.quat.z, 4);
		object->rotateKeys[i].u.quat.z = -object->rotateKeys[i].u.quat.z;
//		if ( scaleSkinnedObject )
//			object->rotateKeys[i].u.quat.z *= scaleVal;
	}

	//wrot
	obeRead(&object->numRotateKeys, 4);
	for(i = 0; i < object->numRotateKeys; i++)
	{
		obeRead(&object->rotateKeys[i].time, 4);
		obeRead(&object->rotateKeys[i].u.quat.w, 4);
//		if ( scaleSkinnedObject )
//			object->rotateKeys[i].u.quat.w *= scaleVal;
	}


}
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FreeOBE(OBJECTINFO *obj)
{
	free(obj->object->mesh->vertices);
	free(obj->object->mesh->faceInfo);
	if(obj->object->mesh->vertexNormals)
		free(obj->object->mesh->vertexNormals);
	free(obj->object->mesh);
	free(obj->object->moveKeys);
	free(obj->object->scaleKeys);
	free(obj->object->rotateKeys);
	if(obj->object->sprites)
		free(obj->object->sprites);
	if(obj->ranges)
		free(obj->ranges);
	free(obj->object);
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
BITMAP_TYPE *NeedToLoadOBEBitmap(char *filename)
{
	int i, j;
	char buf[MAX_PATH];

	//search temp texture bank for bitmap
	for(i = 0; i < numTextureBanks; i++)
	{
		sprintf(buf, "%s\\%s", textureBanks[i].destination, filename);
		for(j = 0; j < textureBanks[MAX_TEXTURE_BANKS].numTextures; j ++)
		{
			if(strcmp(buf, textureBanks[MAX_TEXTURE_BANKS].textures[j].fullFilename) == 0)
				return &textureBanks[MAX_TEXTURE_BANKS].textures[j];
		}
	}

	j = textureBanks[MAX_TEXTURE_BANKS].numTextures;


	//not there so must load it
	for(i = 0; i < numTextureBanks; i++)
	{
		sprintf(buf, "%s\\%s", textureBanks[i].destination, filename);
		if(FileExists(buf))
		{
			//if its not there, load the bitmap and add it to the temporary bank
			strcpy(textureBanks[MAX_TEXTURE_BANKS].textures[j].fullFilename, buf);
			if(LoadPicy(&textureBanks[MAX_TEXTURE_BANKS].textures[j]))
			{
				textureBanks[MAX_TEXTURE_BANKS].numTextures++;
				return &textureBanks[MAX_TEXTURE_BANKS].textures[j];
			}
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
void obeRead(void *buf, int size)
{
	char *b = (char *)buf;
	int i;

	for(i = 0; i < size; i++)
		b[i] = obeDataPtr[i];
	
	obeDataPtr += size;
}