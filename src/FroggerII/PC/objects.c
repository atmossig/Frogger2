/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: objects.c
	Programmer	: 
	Date		: 29/04/99 11:14:23

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>

#include "incs.h"

OBJECT_BANK	objectBanks[MAX_OBJECT_BANKS];
OBJECT *tempObject;

char message[256];

extern OBJECT sec02;

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

u32 Rom2Ram(u32 x, u32 addressOffset)
{
	u32	temp;
	temp = (u32)x & 0x00ffffff;
	temp += addressOffset;
	return temp;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void RestoreDrawList(Gfx *dl, u32 offset,OBJECT *obj)
{
	u8	*temp;
	u32	address;
	u32 *addrP;
	TEXTURE	*tempTex;

	temp = (u8 *)dl; 

	while(*temp != 0xB8)
	{
		switch(*temp)
		{
			//loadvertices
			case 0x04:		addrP = (u32 *)(temp + 4);
							address = *addrP;
							address = Rom2Ram(address, offset);
							*addrP = address;
							temp += 8;
							break;

			//load texture block
			case 0xFD:		
							//load texture and load texture palette share the same first byte

							//this appears to be the code for a texture load
							if(*(temp + 8) == 0xf5)
							{
								addrP = (u32 *)(temp + 4);
								address = *addrP;
								FindTexture(&tempTex, address,YES);
								if(tempTex)
									*addrP = (u32)tempTex->data;
								temp += 56;
							}
							else if(*(temp + 8) == 0xe8)
							{
								addrP = (u32 *)(temp + 4);
								address = *addrP;
								FindTexture(&tempTex, address,YES);
								if(tempTex)
									*addrP = (u32)tempTex->palette;
								temp += 48;
							}
							else
								temp += 16;
								
							break;


			default:		temp += 8;
							break;

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

int numoftc = 0;
void RestoreObjectPointers(OBJECT *obj, u32 memoryOffset)
{
	u32	tempInt;
	int x,y;
	char mess[16];

	tempInt = (u32)obj->sprites;
	if(tempInt)
	{
		for(x = 0; x < obj->numSprites; x++)
		{
			FindTexture(&(obj->sprites[x].textureID), (u32)obj->sprites[x].textureID,YES);
		}
	}

	if (obj->mesh)
	{
		float r,g,b,a;
		unsigned long dupCount = 0;
if (obj->mesh->numFaces)
		{
			unsigned long cTex = (unsigned long)obj->mesh->textureIDs[0];

			for (x=1; x<obj->mesh->numFaces; x++)
			{
				if ((unsigned long)obj->mesh->textureIDs[x] != cTex)
				{
					for (y=x; y<obj->mesh->numFaces; y++)
						if ((unsigned long)obj->mesh->textureIDs[y] == cTex)
							break;
					
					if (y!=obj->mesh->numFaces)
					{
						QUATERNION t;
						SHORTVECTOR t1;
						USHORT2DVECTOR t2;

						obj->mesh->textureIDs[y] = obj->mesh->textureIDs[x];
						obj->mesh->textureIDs[x] = (TEXTURE *)cTex;
						
						t = ((QUATERNION *)obj->mesh->vertexNormals)[x*3];
						((QUATERNION *)obj->mesh->vertexNormals)[x*3] = ((QUATERNION *)obj->mesh->vertexNormals)[y*3];
						((QUATERNION *)obj->mesh->vertexNormals)[y*3] = t;

						t = ((QUATERNION *)obj->mesh->vertexNormals)[x*3+1];
						((QUATERNION *)obj->mesh->vertexNormals)[x*3+1] = ((QUATERNION *)obj->mesh->vertexNormals)[y*3+1];
						((QUATERNION *)obj->mesh->vertexNormals)[y*3+1] = t;

						t = ((QUATERNION *)obj->mesh->vertexNormals)[x*3+2];
						((QUATERNION *)obj->mesh->vertexNormals)[x*3+2] = ((QUATERNION *)obj->mesh->vertexNormals)[y*3+2];
						((QUATERNION *)obj->mesh->vertexNormals)[y*3+2] = t;
						
						t1 = obj->mesh->faceIndex[x];
						obj->mesh->faceIndex[x] = obj->mesh->faceIndex[y];
						obj->mesh->faceIndex[y] = t1;
						
						t2 = obj->mesh->faceTC[x*3];
						obj->mesh->faceTC[x*3] = obj->mesh->faceTC[y*3];
						obj->mesh->faceTC[y*3] = t2;
						
						t2 = obj->mesh->faceTC[x*3+1];
						obj->mesh->faceTC[x*3+1] = obj->mesh->faceTC[y*3+1];
						obj->mesh->faceTC[y*3+1] = t2;
						
						t2 = obj->mesh->faceTC[x*3+2];
						obj->mesh->faceTC[x*3+2] = obj->mesh->faceTC[y*3+2];
						obj->mesh->faceTC[y*3+2] = t2;
							
					}
				}

				cTex = (unsigned long)obj->mesh->textureIDs[x];
			}
		}
		obj->renderData = JallocAlloc (sizeof(D3DTLVERTEX)*obj->mesh->numFaces*3,0,"vtxPC");

		for (x=0; x<obj->mesh->numVertices; x++)
		{
			for (y=0; y<obj->mesh->numVertices; y++)
			{
				if (x!=y)
					if (fabs(obj->mesh->vertices[x].v[0] - obj->mesh->vertices[y].v[0])<0.009)
						if (fabs(obj->mesh->vertices[x].v[1] - obj->mesh->vertices[y].v[1])<0.009)
							if (fabs(obj->mesh->vertices[x].v[2] - obj->mesh->vertices[y].v[2])<0.009)
							{
								dupCount++; //dprintf"Duplicate Vertex in %s\n",obj->name));
							}
			}			
		}

		if (dupCount)
			dprintf"%lu duplicate vertices in %s\n",dupCount, obj->name));

		for (x=0; x<obj->mesh->numFaces; x++)
		{
			TEXENTRY *me;
			me = obj->mesh->textureIDs[x] = (TEXTURE *)GetTexEntryFromCRC( (long)obj->mesh->textureIDs[x]);
			
			if (me)
				if (me->type == TEXTURE_AI)
					obj->flags |= OBJECT_FLAGS_XLU;
		}
		
		for (x=0; x<obj->mesh->numFaces*3; x++)
		{
			r = ((QUATERNION *)obj->mesh->vertexNormals)[x].x;
			g = ((QUATERNION *)obj->mesh->vertexNormals)[x].y;
			b = ((QUATERNION *)obj->mesh->vertexNormals)[x].z;
			a = ((QUATERNION *)obj->mesh->vertexNormals)[x].w;
			((long *)(& ((QUATERNION *)obj->mesh->vertexNormals)[x].x))[0] = D3DRGBA(r,g,b,a);

			((D3DTLVERTEX *)obj->renderData)[x].tu = (obj->mesh->faceTC[x].v[0]*0.000975F);
			((D3DTLVERTEX *)obj->renderData)[x].tv = (obj->mesh->faceTC[x].v[1]*0.000975F);
			((D3DTLVERTEX *)obj->renderData)[x].color = D3DRGBA(1,1,1,1);
			((D3DTLVERTEX *)obj->renderData)[x].specular = D3DRGBA(0,0,0,0);
		}

	}

	if(obj->children)
		RestoreObjectPointers(obj->children, memoryOffset);
	if(obj->next)
		RestoreObjectPointers(obj->next, memoryOffset);
}

/*	--------------------------------------------------------------------------------
	Function 	: LoadObjectBank
	Purpose 	: dma's an object bank from rom into ram.
	Parameters 	: char *objectbank
	Returns 	: none
	Info 		:
*/

BOOL LoadObjectBank(int num)
{
	char				*objectBank = NULL;
	u32					bankRomStart, bankRomEnd;
	short				x = -1, y = 0;
	OBJECT				*obj;
	OBJECT_CONTROLLER	*objCont;
	char    file[MAX_PATH]; 
    HINSTANCE handle = NULL;
	FARPROC func;

    strcpy (file,baseDirectory);	
    strcat (file,OBJECT_BASE);

	switch(num)
	{
		case INGAMEGENERIC_OBJ_BANK:
			strcat (file,"GameGeneric");
			break;
		case GARDENMASTER_OBJ_BANK:
			strcat (file,"gardenmaster");
			break;
		case GARDENBOSSA_OBJ_BANK:
			strcat (file,"gardenbossa");
			break;
		case GARDENBOSSB_OBJ_BANK:
			strcat (file,"gardenbossb");
			break;
		case GARDENLEV1_OBJ_BANK:
			strcat (file,"garden1");
			break;
		case GARDENLEV2_OBJ_BANK:
			strcat (file,"garden2");
			break;
		case GARDENLEV3_OBJ_BANK:
			strcat (file,"garden3");
			break;
		case GARDENBONUS_OBJ_BANK:
			strcat (file,"gardenbonus");
			break;
		case GARDENMULTI_OBJ_BANK:
			strcat (file,"gardenmulti");
			break;

		case SPACEMASTER_OBJ_BANK:
			strcat (file,"spacemaster");
			break;
		case SPACEBOSS_OBJ_BANK:
			strcat (file,"spaceboss");
			break;
		
		case SPACELEV1_OBJ_BANK:
			strcat (file,"space1");
			break;

		case SPACELEV2_OBJ_BANK:
			strcat (file,"space2");
			break;

		case SPACELEV3_OBJ_BANK:
			strcat (file,"space3");
			break;

		case SPACEBONUS_OBJ_BANK:
			strcat (file,"spacebonus");
			break;

		case SPACEMULTI_OBJ_BANK:
			strcat (file,"spacemulti");
			break;

		case SUBTERRANEANMASTER_OBJ_BANK:
			strcat (file,"submaster");
			break;

		case SUBTERRANEANMULTI_OBJ_BANK:
			strcat( file, "submulti" );
			break;

		case SUBTERRANEANBOSS_OBJ_BANK:
			strcat (file, "subboss");
			break;

		case SUBTERRANEANBOSSB_OBJ_BANK:
			strcat (file, "subtest");		// <----- NOTE! Interesting, different, new.
			break;

		case SUBTERRANEANLEV1_OBJ_BANK:
			strcat (file, "sub1");
			break;
		
		case SUBTERRANEANLEV2_OBJ_BANK:
			strcat (file, "sub2");
			break;
		
		case SUBTERRANEANLEV3_OBJ_BANK:
			strcat (file, "sub3");
			break;
		
		case CITYMASTER_OBJ_BANK:
			strcat (file,"citymaster");
			break;

		case CITYBOSS_OBJ_BANK:
			strcat (file,"cityboss");
			break;
		
		case CITYLEV1_OBJ_BANK:
			strcat (file,"city1");
			break;

		case CITYLEV2_OBJ_BANK:
			strcat (file,"city2");
			break;

		case CITYLEV3_OBJ_BANK:
			strcat (file,"city3");
			break;

		case CITYBONUS_OBJ_BANK:
			strcat (file,"citybonus");
			break;

		case CITYMULTI_OBJ_BANK:
			strcat (file,"citymulti");
			break;

		case HALLOWEENMASTER_OBJ_BANK:
			strcat (file,"halloweenmaster");
			break;

		case HALLOWEENBOSS_OBJ_BANK:
			strcat (file,"halloweenboss");
			break;

		case HALLOWEENBOSSB_OBJ_BANK:
			strcat (file,"halloweenbossb");
			break;

		case HALLOWEENLEV1_OBJ_BANK:
			strcat (file,"halloween1");
			break;

		case HALLOWEENLEV2_OBJ_BANK:
			strcat (file,"halloween2");
			break;

		case HALLOWEENLEV3_OBJ_BANK:
			strcat (file,"halloween3");
			break;

		case HALLOWEENBONUS_OBJ_BANK:
			strcat (file,"halloweenbonus");
			break;

		case HALLOWEENMULTI_OBJ_BANK:
			strcat (file,"halloweenmulti");
			break;

		case ANCIENTMASTER_OBJ_BANK:
			strcat (file,"ancientsmaster");
			break;
		
		case ANCIENTBOSS_OBJ_BANK:
			strcat (file,"ancientsboss");
			break;

		case ANCIENTLEV1_OBJ_BANK:
			strcat (file,"ancients1");
			break;

		case ANCIENTLEV2_OBJ_BANK:
			strcat (file,"ancients2");
			break;

		case ANCIENTLEV3_OBJ_BANK:
			strcat (file,"ancients3");
			break;

		case ANCIENTBONUS_OBJ_BANK:
			strcat (file,"ancientsbonus");
			break;

		case ANCIENTMULTI_OBJ_BANK:
			strcat (file,"ancientsmulti");
			break;

		
		case TOYSHOPMASTER_OBJ_BANK:
			strcat (file,"toymaster");
			break;
		
		case TOYSHOPLEV1_OBJ_BANK:
			strcat (file,"toy1");
			break;

		case TOYSHOPLEV2_OBJ_BANK:
			strcat (file,"toy2");
			break;

		case TOYSHOPLEV3_OBJ_BANK:
			strcat (file,"toy3");
			break;

		case TOYSHOPBONUS_OBJ_BANK:
			strcat (file,"toybonus");
			break;

		case TOYSHOPMULTI_OBJ_BANK:
			strcat (file,"toymulti");
			break;
	

		case RETROMASTER_OBJ_BANK:
			strcat (file,"sretromaster");
			break;
		
		case RETROLEV1_OBJ_BANK:
			strcat (file,"sretro1");
			break;

		case RETROLEV2_OBJ_BANK:
			strcat (file,"sretro2");
			break;

		case RETROLEV3_OBJ_BANK:
			strcat (file,"sretro3");
			break;

		case RETROLEV4_OBJ_BANK:
			strcat (file,"sretro4");
			break;

		case RETROLEV5_OBJ_BANK:
			strcat (file,"sretro5");
			break;

		case RETROLEV6_OBJ_BANK:
			strcat (file,"sretro6");
			break;

		case RETROLEV7_OBJ_BANK:
			strcat (file,"sretro7");
			break;

		case RETROLEV8_OBJ_BANK:
			strcat (file,"sretro8");
			break;

		case RETROLEV9_OBJ_BANK:
			strcat (file,"sretro9");
			break;

		case RETROLEV10_OBJ_BANK:
			strcat (file,"sretro10");
			break;

		case RETROBONUS_OBJ_BANK:
			strcat (file,"sretrobonus");
			break;

		case RETROMULTI_OBJ_BANK:
			strcat (file,"sretromulti");
			break;
	
		case LABORATORYMASTER_OBJ_BANK:
			strcat (file,"labmaster");
			break;

		case LABORATORYBOSSA_OBJ_BANK:
			strcat (file, "labbossa");
			break;
	
		case LABORATORYBOSSB_OBJ_BANK:
			strcat (file, "labbossb");
			break;

		case LABORATORYBOSSC_OBJ_BANK:
			strcat (file, "labbossc");
			break;

		case LABORATORYLEV1_OBJ_BANK:
			strcat (file,"lab1");
			break;
			
		case LABORATORYLEV2_OBJ_BANK:
			strcat (file,"lab2");
			break;

		case LABORATORYLEV3_OBJ_BANK:
			strcat (file,"lab3");
			break;

		case LABORATORYMULTI_OBJ_BANK:
			strcat (file,"labmulti");
			break;

		case FRONTEND_OBJ_BANK:
			strcat(file,"title");
			break;

		case FRONTEND1_OBJ_BANK:
			strcat(file,"start");
			break;

		case FRONTEND2_OBJ_BANK:
			strcat(file,"levela");
			break;

		case FRONTEND3_OBJ_BANK:
			strcat(file,"levelb");
			break;

		case FRONTEND4_OBJ_BANK:
			strcat(file,"multisel");
			break;

		case FRONTEND5_OBJ_BANK:
			strcat(file,"language");
			break;

		case MULTIPLAYER_OBJ_BANK:
			strcat(file,"multiplayer");
			break;

		default:
			dprintf"ERROR: no object bank specified....\n"));
			return 0;
			break;
	}

	strcat (file,".dll");

    handle = LoadLibrary(file);
	
	if (handle != NULL)
	{
		func = GetProcAddress(handle, "GetbankAddress");
		if (func != NULL)
		objectBank=(char *)func();
//		FreeLibrary(handle);
	}
	
	if( !objectBank )
	{
		dprintf"Unable to load object bank %s\n", message));
		return 0;
	}

	while(objectBanks[++x].freePtr != 0);
	if(x >= MAX_OBJECT_BANKS)
	{
		dprintf"ERROR:too many object banks\n"));
		return 0;
	}

//PC-CHANGE
	objectBanks[x].freePtr = (char *)handle;
	objectBanks[x].objList = (OBJECT_DESCRIPTOR *)objectBank;

	y = 0;
	while(objectBanks[x].objList[y].objCont)
	{							   
//		calculateTexPtrs(objectBanks[x].objList[y].objCont->object);


//PC-CHANGE
		RestoreObjectPointers(objectBanks[x].objList[y].objCont->object,(u32)objectBank);
		y++;
	}
	
	objectBanks[x].numObjects = y;	
	return 1;
}

void FreeObjectBank(long i)
{
	FreeLibrary(objectBanks[i].freePtr);
	objectBanks[i].freePtr = NULL;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
float CalcRadius(MESH *mesh)
{
	int i;
	float radius = 0,temp;
	VECTOR *vertex;

	for(i = 0;i < mesh->numVertices;i++)
	{
		vertex = mesh->vertices+i;
		if((temp = Magnitude(vertex)) > radius)
			radius = temp;
	}
	return radius;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FindObject(OBJECT_CONTROLLER **objCPtr, int objID, char *name,BOOL vital)
{
	int x, y;
	char *temp;
			
	for(x = 0; x < MAX_OBJECT_BANKS; x++)
	{
		if(objectBanks[x].freePtr)
		{
			temp = objectBanks[x].data;
			for(y = 0; y < objectBanks[x].numObjects; y++)
			{
				if(objectBanks[x].objList[y].objectID == objID)
				{
					(*objCPtr) = objectBanks[x].objList[y].objCont;
//					if(name)
//						dprintf"Found object %s\n", name));
					return;
				}

			}
		}
	}
	*objCPtr = NULL;
	if(name)
	{
		if(vital)
		{
			dprintf"Object '%s' not found\n", name));
//			sprintf(gameInfo.error,"LEVEL UNPLAYABLE - %s MISSING",name);
		}
//		Crash();
	}
	else
	{
		dprintf"Whoops, object '%X' not found!\n", objID));
//		if(vital)
//			sprintf(gameInfo.error,"LEVEL UNPLAYABLE - OBJECT MISSING");
 //		Crash();
	}
	if(vital)
	{
//		StopDrawing("fondobj");
//help		disableGraphics = TRUE;
//		gameInfo.state = ERROR_SCREEN_MODE;
	}

	return;
}

/*	--------------------------------------------------------------------------------
	Function 	: FindSubObjectFromObject
	Purpose 	: called from FindSubObject - recursive search of an object
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
OBJECT *FindSubObjectFromObject(OBJECT *obj, int objID)
{
	OBJECT	*tempObj = NULL;

	tempObj = obj;

	if(tempObj->objID == objID)
		return tempObj;	

	if(obj->children)
	{
		tempObj = FindSubObjectFromObject(obj->children, objID);
		if(tempObj)
		{
			if(tempObj->objID == objID)
				return tempObj;	
		}
	}

	if(obj->next)
	{
		tempObj = FindSubObjectFromObject(obj->next, objID);
		if(tempObj)
		{
			if(tempObj->objID == objID)
				return tempObj;	
		}
	}

	return NULL;
}

/*	--------------------------------------------------------------------------------
	Function 	: FindSubObject
	Purpose 	: searches an object and all its children for a specified object
	Parameters 	: OBJECT **, required id
	Returns 	: none
	Info 		:
*/
void FindSubObject(OBJECT **objPtr, int objID)
{
	int x, y;
	OBJECT	*tempObj, *foundObj;

	for(x = 0; x < MAX_OBJECT_BANKS; x++)
	{
		if(objectBanks[x].freePtr)
		{
			for(y = 0; y < objectBanks[x].numObjects; y++)
			{
				tempObj = objectBanks[x].objList[y].objCont->object;
				foundObj = FindSubObjectFromObject(tempObj, objID);				   		
				if(foundObj)
				{
					(*objPtr) = foundObj;
					return;
				}
					
			}
		}
	}

	*objPtr = NULL;
	dprintf"Whoops, object '%X' not found!\n", objID));
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteObjectDisplayListFlat(OBJECT *obj)
{
/*
	int x, y, face = 0,j;
	MESH	*mesh = obj->mesh;
	SHORT2DVECTOR	*tcp = mesh->faceTC;
	VECTOR		*baseVertices = mesh->vertices;
	VECTOR		*vertex;
	SHORTVECTOR	*baseFaceIndex = mesh->faceIndex;
	SHORTVECTOR *faceIndex;
	Vtx			*batchPtr;
	int			fC = mesh->numFaces / 10;
	int			mod = mesh->numFaces % 10, temp;
	TEXTURE		*texture = NULL;
	BYTEVECTOR	*normalPtr;
	BYTEVECTOR	*baseNormalPtr = mesh->faceNormals;

    gDPPipeSync(glistp++);

	for(x = 0; x < fC; x++)
	{
		batchPtr = vtxPtr;

		for(y = 0; y < 10; y++)
		{		
			faceIndex = baseFaceIndex + face;
			normalPtr = baseNormalPtr + face;

			vertex = baseVertices + faceIndex->v[X];
  			V((vtxPtr), 
				(int)vertex->v[0],
				(int)vertex->v[1],
				(int)vertex->v[2],0,
				tcp->v[0], (tcp++)->v[1],
				normalPtr->v[X], normalPtr->v[Y], normalPtr->v[Z], xluFact);

			vertex = baseVertices + faceIndex->v[Y];
			V((vtxPtr+1), 
				(int)vertex->v[0],
				(int)vertex->v[1],
				(int)vertex->v[2],0,
				tcp->v[0], (tcp++)->v[1],
				normalPtr->v[X], normalPtr->v[Y], normalPtr->v[Z], xluFact);

			vertex = baseVertices + faceIndex->v[Z];
			V((vtxPtr+2), 
				(int)vertex->v[0],
				(int)vertex->v[1],
				(int)vertex->v[2],0,
				tcp->v[0], (tcp++)->v[1],
				normalPtr->v[X], normalPtr->v[Y], normalPtr->v[Z], xluFact);

			face++;			
			vtxPtr+=3;
//			if(face == mesh->numFaces)
//				break;

		}

		gSPVertex(glistp++, batchPtr , 30, 0);

		for(j = 0;j < 10;j++)
		{
			if(texture != obj->mesh->textureIDs[face-10+j]	)
			{
				texture = obj->mesh->textureIDs[face-10+j];
			    gDPPipeSync(glistp++);
				LoadTexture(texture);
			}
			gSP1Triangle(glistp++, j*3, j*3+1, j*3+2, 2);
		}
	}

//handle remaining faces
	batchPtr = vtxPtr;

	for(y = 0; y < mod; y++)
	{		
		faceIndex = baseFaceIndex + face;
		normalPtr = baseNormalPtr + face;

		vertex = baseVertices + faceIndex->v[0];
		V((vtxPtr), 
			(int)vertex->v[0],
			(int)vertex->v[1],
			(int)vertex->v[2],0,
			tcp->v[0], (tcp++)->v[1],
			normalPtr->v[X], normalPtr->v[Y], normalPtr->v[Z], xluFact);

		vertex = baseVertices + faceIndex->v[1];
		V((vtxPtr+1), 
			(int)vertex->v[0],
			(int)vertex->v[1],
			(int)vertex->v[2],0,
			tcp->v[0], (tcp++)->v[1],
  			normalPtr->v[X], normalPtr->v[Y], normalPtr->v[Z], xluFact);

		vertex = baseVertices + faceIndex->v[2];
  		V((vtxPtr+2), 
			(int)vertex->v[0],
			(int)vertex->v[1],
			(int)vertex->v[2],0,
			tcp->v[0], (tcp++)->v[1],
			normalPtr->v[X], normalPtr->v[Y], normalPtr->v[Z], xluFact);


		face++;			
		vtxPtr+=3;

	}

	if(mod)
	{
		face -= mod;
		gSPVertex(glistp++, batchPtr , mod * 3, 0);
		temp = 0;
		for(x = 0; x < mod; x++)
		{
			if(texture != obj->mesh->textureIDs[face])
			{
				texture = obj->mesh->textureIDs[face];		
				gDPPipeSync(glistp++);
				LoadTexture(texture);
			}
			gSP1Triangle(glistp++, temp++, temp++, temp++, 2);
			face++;
		}
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

float vtxFact = 9;
float fFact = 0.3;

float vtxFact2 = 10;
float fFact2 = 0.4;
extern float meMod;

float mulP = 11;
float mulP2 = 100;

void WriteObjectDisplayListGouraud(OBJECT *obj)
{
/*
	int x, y, face = 0,j;
	MESH	*mesh = obj->mesh;
	SHORT2DVECTOR	*tcp = mesh->faceTC;
	VECTOR		*baseVertices = mesh->vertices;
	VECTOR		*vertex;
	SHORTVECTOR	*baseFaceIndex = mesh->faceIndex;
	SHORTVECTOR *faceIndex;
	Vtx			*batchPtr;
	int			fC = mesh->numFaces / 10;
	int			mod = mesh->numFaces % 10, temp;
	TEXTURE		*texture = NULL;
	BYTEVECTOR	*vNormalPtr;
	BYTEVECTOR	*baseVNormalPtr = mesh->vertexNormals;


    gDPPipeSync(glistp++);

	for(x = 0; x < fC; x++)
	{
		batchPtr = vtxPtr;

		for(y = 0; y < 10; y++)
		{		
			faceIndex = baseFaceIndex + face;
//			tcp = mesh->faceTC + face;

			vertex = baseVertices + faceIndex->v[X];
			vNormalPtr = baseVNormalPtr + faceIndex->v[X];
  			V((vtxPtr), 
				(int)vertex->v[0],
				(int)vertex->v[1]+vtxFact+sinf(faceIndex->v[X]*mulP + frameCount*fFact)*vtxFact,
				(int)vertex->v[2],0,
				tcp->v[0]+vtxFact2+sinf(frameCount*fFact2)*vtxFact2*meMod, 
				(tcp++)->v[1]+vtxFact2+cosf(frameCount*fFact2)*vtxFact2*meMod,
				(sinf(faceIndex->v[X]*mulP2 + frameCount*fFact)*15)+240,
				(sinf(faceIndex->v[X]*mulP2 + frameCount*fFact)*15)+240,
				(sinf(faceIndex->v[X]*mulP2 + frameCount*fFact)*15)+240,
				xluFact);

			vertex = baseVertices + faceIndex->v[Y];
			vNormalPtr = baseVNormalPtr + faceIndex->v[Y];
			V((vtxPtr+1), 
				(int)vertex->v[0],
				(int)vertex->v[1]+vtxFact+sinf(faceIndex->v[Y]*mulP + frameCount*fFact)*vtxFact,
				(int)vertex->v[2],0,
				tcp->v[0]      +vtxFact2+sinf(frameCount*fFact2)*vtxFact2*meMod,
				(tcp++)->v[1]  +vtxFact2+cosf(frameCount*fFact2)*vtxFact2*meMod,
				(sinf(faceIndex->v[Y]*mulP2 + frameCount*fFact)*15)+240,
				(sinf(faceIndex->v[Y]*mulP2 + frameCount*fFact)*15)+240,
				(sinf(faceIndex->v[Y]*mulP2 + frameCount*fFact)*15)+240,
				xluFact);

			vertex = baseVertices + faceIndex->v[Z];
			vNormalPtr = baseVNormalPtr + faceIndex->v[Z];
			V((vtxPtr+2), 
				(int)vertex->v[0],
				(int)vertex->v[1]+vtxFact+sinf(faceIndex->v[Z]*mulP + frameCount*fFact)*vtxFact,
				(int)vertex->v[2],0,
				tcp->v[0]+vtxFact2+sinf(frameCount*fFact2)*vtxFact2*meMod, 
				(tcp++)->v[1]+vtxFact2+cosf(frameCount*fFact2)*vtxFact2*meMod,

				(sinf(faceIndex->v[Z]*mulP2 + frameCount*fFact)*15)+240,
				(sinf(faceIndex->v[Z]*mulP2 + frameCount*fFact)*15)+240,
				(sinf(faceIndex->v[Z]*mulP2 + frameCount*fFact)*15)+240,
				xluFact);

  
			face++;			
			vtxPtr+=3;

		}

		gSPVertex(glistp++, batchPtr , 30, 0);
	    gDPPipeSync(glistp++);


		for(j = 0;j < 10;j++)
		{
			if(texture != obj->mesh->textureIDs[face-10+j]	)
			{
				texture = obj->mesh->textureIDs[face-10+j];
			    gDPPipeSync(glistp++);
				LoadTexture(texture);
			}
			gSP1Triangle(glistp++, j*3, j*3+1, j*3+2, 2);
		}
	}

//handle remaining faces
	batchPtr = vtxPtr;

	for(y = 0; y < mod; y++)
	{		
		faceIndex = baseFaceIndex + face;

		vertex = baseVertices + faceIndex->v[0];
		vNormalPtr = baseVNormalPtr + faceIndex->v[X];
		V((vtxPtr), 
			(int)vertex->v[0],
			(int)vertex->v[1],
			(int)vertex->v[2],0,
			tcp->v[0], (tcp++)->v[1],
			vNormalPtr->v[X], vNormalPtr->v[Y], vNormalPtr->v[Z], xluFact);

		vertex = baseVertices + faceIndex->v[1];
		vNormalPtr = baseVNormalPtr + faceIndex->v[Y];
		V((vtxPtr+1), 
			(int)vertex->v[0],
			(int)vertex->v[1],
			(int)vertex->v[2],0,
			tcp->v[0], (tcp++)->v[1],
			vNormalPtr->v[X], vNormalPtr->v[Y], vNormalPtr->v[Z], xluFact);

		vertex = baseVertices + faceIndex->v[2];
		vNormalPtr = baseVNormalPtr + faceIndex->v[Z];
  		V((vtxPtr+2), 
			(int)vertex->v[0],
			(int)vertex->v[1],
			(int)vertex->v[2],0,
			tcp->v[0], (tcp++)->v[1],
			vNormalPtr->v[X], vNormalPtr->v[Y], vNormalPtr->v[Z], xluFact);


		face++;			
		vtxPtr+=3;

	}

	if(mod)
	{
		face -= mod;
		gSPVertex(glistp++, batchPtr , mod * 3, 0);
		temp = 0;
		for(x = 0; x < mod; x++)
		{
			if(texture != obj->mesh->textureIDs[face])
			{
				texture = obj->mesh->textureIDs[face];
			    gDPPipeSync(glistp++);
				LoadTexture(texture);
			}
			gSP1Triangle(glistp++, temp++, temp++, temp++, 2);
			face++;
		}
	}

    gDPPipeSync(glistp++);
*/
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void LoadTexture(TEXTURE *texture)
{
	if(texture == 0)
		return;
/*
	switch(texture->pixSize)
	{
		case G_IM_SIZ_16b:
			gDPSetTextureLUT(glistp++,	G_TT_NONE);
			if(mirrorTextures)
			{
				gDPLoadTextureBlock(glistp++, texture->data , texture->format, G_IM_SIZ_16b, texture->sx, texture->sy, 0,G_TX_WRAP | G_TX_MIRROR, G_TX_WRAP | G_TX_MIRROR,texture->TCScaleX, texture->TCScaleY, G_TX_NOLOD, G_TX_NOLOD);
			}
			else
			{
				gDPLoadTextureBlock(glistp++, texture->data , texture->format, G_IM_SIZ_16b, texture->sx, texture->sy, 0,G_TX_WRAP , G_TX_WRAP,texture->TCScaleX, texture->TCScaleY, G_TX_NOLOD, G_TX_NOLOD);
			}
			break;

		case G_IM_SIZ_8b:

			gDPSetTextureLUT(glistp++,	G_TT_RGBA16);
			gDPLoadTLUT_pal256(glistp++, texture->palette);
			if(mirrorTextures)
			{
				gDPLoadTextureBlock(glistp++, texture->data , texture->format, G_IM_SIZ_8b, texture->sx, texture->sy, 0,G_TX_WRAP | G_TX_MIRROR, G_TX_WRAP | G_TX_MIRROR,texture->TCScaleX, texture->TCScaleY, G_TX_NOLOD, G_TX_NOLOD);
			}
			else
			{
				gDPLoadTextureBlock(glistp++, texture->data , texture->format, G_IM_SIZ_8b, texture->sx, texture->sy, 0,G_TX_WRAP, G_TX_WRAP,texture->TCScaleX, texture->TCScaleY, G_TX_NOLOD, G_TX_NOLOD);
			}
			break;

		case G_IM_SIZ_4b:

			gDPSetTextureLUT(glistp++,	G_TT_RGBA16);
			gDPLoadTLUT_pal16(glistp++,0,texture->palette);
			if(mirrorTextures)
			{
				gDPLoadTextureBlock_4b(glistp++, texture->data , texture->format, texture->sx, texture->sy, 0,G_TX_WRAP | G_TX_MIRROR, G_TX_WRAP | G_TX_MIRROR,texture->TCScaleX, texture->TCScaleY, G_TX_NOLOD, G_TX_NOLOD);
			}
			else
			{
				gDPLoadTextureBlock_4b(glistp++, texture->data , texture->format, texture->sx, texture->sy, 0,G_TX_WRAP, G_TX_WRAP,texture->TCScaleX, texture->TCScaleY, G_TX_NOLOD, G_TX_NOLOD);
			}
			break;
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
void StopDrawing(char *whereami)
{
/*
	if(codeDrawingRequest == TRUE)
		dprintf"Stop Drawing Requested from '%s'\n", whereami));
	codeDrawingRequest = FALSE;
	while(gfxIsDrawing == TRUE);
	disableGraphics = TRUE;
*/
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void StartDrawing(char *whereami)
{

//	if(codeDrawingRequest == FALSE)
	dprintf"Start Drawing Requested from '%s'\n", whereami));
/*	codeDrawingRequest = TRUE;
	while(gfxIsDrawing == FALSE);
	disableGraphics = FALSE;
	onlyDrawBackdrops = FALSE;
*/
}


/*	--------------------------------------------------------------------------------
	Function		: SetObjectXlu
	Purpose			: sets an object to be translucent
	Parameters		: OBJECT *,BOOL
	Returns			: void
	Info			: 
*/
void SetObjectXlu(OBJECT *object,BOOL xlu)
{
	if(xlu)
		object->flags |= OBJECT_FLAGS_XLU;
	else
		object->flags &= -1 - OBJECT_FLAGS_XLU;
	if(object->children)
		SetObjectXlu(object->children,xlu);
	if(object->next)
		SetObjectXlu(object->next,xlu);
}
