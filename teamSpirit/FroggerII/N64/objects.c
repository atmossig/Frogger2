/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: objects.c
	Programmer	: 
	Date		: 29/04/99 11:14:23

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

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
								FindTexture(&tempTex, address,YES,"");
								if(tempTex)
									*addrP = (u32)tempTex->data;
								temp += 56;
							}
							else if(*(temp + 8) == 0xe8)
							{
								addrP = (u32 *)(temp + 4);
								address = *addrP;
								FindTexture(&tempTex, address,YES,"");
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
	int x;
	char mess[16];

////////////initialise mesh
	tempInt = (u32)obj->mesh;
	if(tempInt)
	{
		obj->mesh = (MESH *)Rom2Ram((u32)obj->mesh, memoryOffset);

		obj->mesh->vertices = (VECTOR *)Rom2Ram((u32)obj->mesh->vertices, memoryOffset);
		
		if(obj->mesh->numFaces)
		{

			obj->mesh->faceIndex = (SHORTVECTOR *)Rom2Ram((u32)obj->mesh->faceIndex, memoryOffset);

			if(obj->mesh->faceNormals)
				obj->mesh->faceNormals = (BYTEVECTOR *)Rom2Ram((u32)obj->mesh->faceNormals, memoryOffset);
			if(obj->mesh->faceTC)
			{
				obj->mesh->faceTC = (SHORT2DVECTOR *)Rom2Ram((u32)obj->mesh->faceTC, memoryOffset);
				sprintf(mess,"OFTC%d",numoftc++);
				obj->mesh->originalFaceTC = (SHORT2DVECTOR *)JallocAlloc(sizeof(SHORT2DVECTOR) * 3 * obj->mesh->numFaces, YES,mess);
				memcpy(obj->mesh->originalFaceTC, obj->mesh->faceTC, sizeof(SHORT2DVECTOR) * 3 * obj->mesh->numFaces);
			}

			if(obj->mesh->faceFlags)
				obj->mesh->faceFlags = (char *)Rom2Ram((u32)obj->mesh->faceFlags, memoryOffset);
			if(obj->mesh->textureIDs)
			{
				obj->mesh->textureIDs = (TEXTURE **)Rom2Ram((u32)obj->mesh->textureIDs, memoryOffset);

				for(x = 0; x < obj->mesh->numFaces; x++)
					FindTexture(&(obj->mesh->textureIDs[x]), (u32)obj->mesh->textureIDs[x],YES,"");
			}

		}
		if(obj->mesh->vertexNormals)
			obj->mesh->vertexNormals = (BYTEVECTOR *)Rom2Ram((u32)obj->mesh->vertexNormals, memoryOffset);
	}

//////////initialise all sprites within object
	tempInt = (u32)obj->sprites;
	if(tempInt)
	{
		obj->sprites = (OBJECTSPRITE *)Rom2Ram((u32)obj->sprites, memoryOffset);
		for(x = 0; x < obj->numSprites; x++)
		{
			FindTexture(&(obj->sprites[x].textureID), (u32)obj->sprites[x].textureID,YES,"");
		}
	}

	tempInt = (u32)obj->drawList;
	if(tempInt)
		obj->drawList = (Gfx *)Rom2Ram((u32)obj->drawList, memoryOffset);


	tempInt = (u32)obj->scaleKeys;
	if(tempInt)
		obj->scaleKeys = (KEYFRAME *)Rom2Ram((u32)obj->scaleKeys, memoryOffset);

	tempInt = (u32)obj->moveKeys;
	if(tempInt)
		obj->moveKeys = (KEYFRAME *)Rom2Ram((u32)obj->moveKeys, memoryOffset);

	tempInt = (u32)obj->rotateKeys;
	if(tempInt)
		obj->rotateKeys = (KEYFRAME *)Rom2Ram((u32)obj->rotateKeys, memoryOffset);

	tempInt = (u32)obj->children;
	if(tempInt)
		obj->children = (OBJECT *)Rom2Ram((u32)obj->children, memoryOffset);

	tempInt = (u32)obj->next;
	if(tempInt)
		obj->next = (OBJECT *)Rom2Ram((u32)obj->next, memoryOffset);


	if(obj->drawList)
		RestoreDrawList(obj->drawList, memoryOffset,obj);


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
void LoadObjectBank(int num)
{
	char				*objectBank;
	u32					bankRomStart, bankRomEnd, bankSize;
	short				x = -1, y = 0;
	OBJECT				*obj;
	OBJECT_CONTROLLER	*objCont;

	switch(num)
	{

		case GARDENMASTER_OBJ_BANK:
			bankRomStart	= (u32)&_objBank1SegmentRomStart;
			bankRomEnd		= (u32)&_objBank1SegmentRomEnd;
			sprintf(message, "GAR_MAS");
			break;

		case GARDENLEV1_OBJ_BANK:
			bankRomStart	= (u32)&_objBank2SegmentRomStart;
			bankRomEnd		= (u32)&_objBank2SegmentRomEnd;
			sprintf(message, "GAR_LEV1");
			break;

		case GARDENLEV2_OBJ_BANK:
			bankRomStart	= (u32)&_objBank3SegmentRomStart;
			bankRomEnd		= (u32)&_objBank3SegmentRomEnd;
			sprintf(message, "GAR_LEV2");
			break;

		case GARDENLEV3_OBJ_BANK:
			bankRomStart	= (u32)&_objBank4SegmentRomStart;
			bankRomEnd		= (u32)&_objBank4SegmentRomEnd;
			sprintf(message, "GAR_LEV3");
			break;

		case GARDENLEV4_OBJ_BANK:
			bankRomStart	= (u32)&_objBank5SegmentRomStart;
			bankRomEnd		= (u32)&_objBank5SegmentRomEnd;
			sprintf(message, "GAR_LEV4");
			break;

		case GARDENBONUS_OBJ_BANK:
			bankRomStart	= (u32)&_objBank6SegmentRomStart;
			bankRomEnd		= (u32)&_objBank6SegmentRomEnd;
			sprintf(message, "GAR_BON");
			break;

		case GARDENMULTI_OBJ_BANK:
			bankRomStart	= (u32)&_objBank7SegmentRomStart;
			bankRomEnd		= (u32)&_objBank7SegmentRomEnd;
			sprintf(message, "GAR_MUL");
			break;

		case SPACEMASTER_OBJ_BANK:
			bankRomStart	= (u32)&_objBank8SegmentRomStart;
			bankRomEnd		= (u32)&_objBank8SegmentRomEnd;
			sprintf(message, "SPA_MAS");
			break;

		case SPACELEV1_OBJ_BANK:
			bankRomStart	= (u32)&_objBank9SegmentRomStart;
			bankRomEnd		= (u32)&_objBank9SegmentRomEnd;
			sprintf(message, "SPA_LEV1");
			break;

		case SPACELEV2_OBJ_BANK:
			bankRomStart	= (u32)&_objBank10SegmentRomStart;
			bankRomEnd		= (u32)&_objBank10SegmentRomEnd;
			sprintf(message, "SPA_LEV2");
			break;

		case SPACELEV3_OBJ_BANK:
			bankRomStart	= (u32)&_objBank11SegmentRomStart;
			bankRomEnd		= (u32)&_objBank11SegmentRomEnd;
			sprintf(message, "SPA_LEV3");
			break;

		case SPACELEV4_OBJ_BANK:
			bankRomStart	= (u32)&_objBank12SegmentRomStart;
			bankRomEnd		= (u32)&_objBank12SegmentRomEnd;
			sprintf(message, "SPA_LEV4");
			break;

		case SPACEBONUS_OBJ_BANK:
			bankRomStart	= (u32)&_objBank13SegmentRomStart;
			bankRomEnd		= (u32)&_objBank13SegmentRomEnd;
			sprintf(message, "SPA_BON");
			break;

		case SPACEMULTI_OBJ_BANK:
			bankRomStart	= (u32)&_objBank14SegmentRomStart;
			bankRomEnd		= (u32)&_objBank14SegmentRomEnd;
			sprintf(message, "SPA_MUL");
			break;
				  
		case CITYMASTER_OBJ_BANK:
			bankRomStart	= (u32)&_objBank15SegmentRomStart;
			bankRomEnd		= (u32)&_objBank15SegmentRomEnd;
			sprintf(message, "CIT_MAS");
			break;

		case CITYLEV1_OBJ_BANK:
			bankRomStart	= (u32)&_objBank16SegmentRomStart;
			bankRomEnd		= (u32)&_objBank16SegmentRomEnd;
			sprintf(message, "CIT_LEV1");
			break;

		case CITYLEV2_OBJ_BANK:
			bankRomStart	= (u32)&_objBank17SegmentRomStart;
			bankRomEnd		= (u32)&_objBank17SegmentRomEnd;
			sprintf(message, "CIT_LEV2");
			break;

		case CITYLEV3_OBJ_BANK:
			bankRomStart	= (u32)&_objBank18SegmentRomStart;
			bankRomEnd		= (u32)&_objBank18SegmentRomEnd;
			sprintf(message, "CIT_LEV3");
			break;

		case CITYLEV4_OBJ_BANK:
			bankRomStart	= (u32)&_objBank19SegmentRomStart;
			bankRomEnd		= (u32)&_objBank19SegmentRomEnd;
			sprintf(message, "CIT_LEV4");
			break;

		case CITYBONUS_OBJ_BANK:
			bankRomStart	= (u32)&_objBank20SegmentRomStart;
			bankRomEnd		= (u32)&_objBank20SegmentRomEnd;
			sprintf(message, "CIT_BON");
			break;

		case CITYMULTI_OBJ_BANK:
			bankRomStart	= (u32)&_objBank21SegmentRomStart;
			bankRomEnd		= (u32)&_objBank21SegmentRomEnd;
			sprintf(message, "CIT_MUL");
			break;

		case HALLOWEENMASTER_OBJ_BANK:
			bankRomStart	= (u32)&_objBank22SegmentRomStart;
			bankRomEnd		= (u32)&_objBank22SegmentRomEnd;
			sprintf(message, "HAL_MAS");
			break;

		case HALLOWEENLEV1_OBJ_BANK:
			bankRomStart	= (u32)&_objBank23SegmentRomStart;
			bankRomEnd		= (u32)&_objBank23SegmentRomEnd;
			sprintf(message, "HAL_LEV1");
			break;

		case HALLOWEENLEV2_OBJ_BANK:
			bankRomStart	= (u32)&_objBank24SegmentRomStart;
			bankRomEnd		= (u32)&_objBank24SegmentRomEnd;
			sprintf(message, "HAL_LEV2");
			break;

		case HALLOWEENLEV3_OBJ_BANK:
			bankRomStart	= (u32)&_objBank25SegmentRomStart;
			bankRomEnd		= (u32)&_objBank25SegmentRomEnd;
			sprintf(message, "HAL_LEV3");
			break;

		case HALLOWEENLEV4_OBJ_BANK:
			bankRomStart	= (u32)&_objBank26SegmentRomStart;
			bankRomEnd		= (u32)&_objBank26SegmentRomEnd;
			sprintf(message, "HAL_LEV4");
			break;

		case HALLOWEENBONUS_OBJ_BANK:
			bankRomStart	= (u32)&_objBank27SegmentRomStart;
			bankRomEnd		= (u32)&_objBank27SegmentRomEnd;
			sprintf(message, "HAL_BON");
			break;

		case HALLOWEENMULTI_OBJ_BANK:
			bankRomStart	= (u32)&_objBank28SegmentRomStart;
			bankRomEnd		= (u32)&_objBank28SegmentRomEnd;
			sprintf(message, "HAL_MUL");
			break;

		case ANCIENTMASTER_OBJ_BANK:
			bankRomStart	= (u32)&_objBank29SegmentRomStart;
			bankRomEnd		= (u32)&_objBank29SegmentRomEnd;
			sprintf(message, "ANC_MAS");
			break;

		case ANCIENTLEV1_OBJ_BANK:
			bankRomStart	= (u32)&_objBank30SegmentRomStart;
			bankRomEnd		= (u32)&_objBank30SegmentRomEnd;
			sprintf(message, "ANC_LEV1");
			break;

		case ANCIENTLEV2_OBJ_BANK:
			bankRomStart	= (u32)&_objBank31SegmentRomStart;
			bankRomEnd		= (u32)&_objBank31SegmentRomEnd;
			sprintf(message, "ANC_LEV2");
			break;

		case ANCIENTLEV3_OBJ_BANK:
			bankRomStart	= (u32)&_objBank32SegmentRomStart;
			bankRomEnd		= (u32)&_objBank32SegmentRomEnd;
			sprintf(message, "ANC_LEV3");
			break;

		case ANCIENTLEV4_OBJ_BANK:
			bankRomStart	= (u32)&_objBank33SegmentRomStart;
			bankRomEnd		= (u32)&_objBank33SegmentRomEnd;
			sprintf(message, "ANC_LEV4");
			break;

		case ANCIENTBONUS_OBJ_BANK:
			bankRomStart	= (u32)&_objBank34SegmentRomStart;
			bankRomEnd		= (u32)&_objBank34SegmentRomEnd;
			sprintf(message, "ANC_BON");
			break;

		case ANCIENTMULTI_OBJ_BANK:
			bankRomStart	= (u32)&_objBank35SegmentRomStart;
			bankRomEnd		= (u32)&_objBank35SegmentRomEnd;
			sprintf(message, "ANC_MUL");
			break;

		case RETROMASTER_OBJ_BANK:
			bankRomStart	= (u32)&_objBank36SegmentRomStart;
			bankRomEnd		= (u32)&_objBank36SegmentRomEnd;
			sprintf(message, "RET_MAS");
			break;

		case RETROLEV1_OBJ_BANK:
			bankRomStart	= (u32)&_objBank37SegmentRomStart;
			bankRomEnd		= (u32)&_objBank37SegmentRomEnd;
			sprintf(message, "RET_LEV1");
			break;

		case RETROLEV2_OBJ_BANK:
			bankRomStart	= (u32)&_objBank38SegmentRomStart;
			bankRomEnd		= (u32)&_objBank38SegmentRomEnd;
			sprintf(message, "RET_LEV2");
			break;

		case RETROLEV3_OBJ_BANK:
			bankRomStart	= (u32)&_objBank39SegmentRomStart;
			bankRomEnd		= (u32)&_objBank39SegmentRomEnd;
			sprintf(message, "RET_LEV3");
			break;

		case RETROLEV4_OBJ_BANK:
			bankRomStart	= (u32)&_objBank40SegmentRomStart;
			bankRomEnd		= (u32)&_objBank40SegmentRomEnd;
			sprintf(message, "RET_LEV4");
			break;

		case RETROBONUS_OBJ_BANK:
			bankRomStart	= (u32)&_objBank41SegmentRomStart;
			bankRomEnd		= (u32)&_objBank41SegmentRomEnd;
			sprintf(message, "RET_BON");
			break;

		case RETROMULTI_OBJ_BANK:
			bankRomStart	= (u32)&_objBank42SegmentRomStart;
			bankRomEnd		= (u32)&_objBank42SegmentRomEnd;
			sprintf(message, "RET_MUL");
			break;


		/*

		case SYSTEM_OBJ_BANK:
			bankRomStart	= (u32)&_objBank1SegmentRomStart;
			bankRomEnd		= (u32)&_objBank1SegmentRomEnd;
			sprintf(message, "SYS_OB");
			break;

		case GARDEN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank2SegmentRomStart;
			bankRomEnd		= (u32)&_objBank2SegmentRomEnd;
			sprintf(message, "GARD_OB");
			break;

		case SUPERRETRO_OBJ_BANK:
			bankRomStart	= (u32)&_objBank3SegmentRomStart;
			bankRomEnd		= (u32)&_objBank3SegmentRomEnd;
			sprintf(message, "SUPER_OB");
			break;
		case SUPERRETRO_OBJ_BANK5:
			bankRomStart	= (u32)&_objBank9SegmentRomStart;
			bankRomEnd		= (u32)&_objBank9SegmentRomEnd;
			sprintf(message, "SUPER_OB");
			break;

		case HALLOWEEN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank4SegmentRomStart;
			bankRomEnd		= (u32)&_objBank4SegmentRomEnd;
			sprintf(message, "HALLW_OB");
			break;

		case SPACE_OBJ_BANK:
			bankRomStart	= (u32)&_objBank5SegmentRomStart;
			bankRomEnd		= (u32)&_objBank5SegmentRomEnd;
			sprintf(message, "SPACE_OB");
			break;

		case CITY_OBJ_BANK:
			bankRomStart	= (u32)&_objBank6SegmentRomStart;
			bankRomEnd		= (u32)&_objBank6SegmentRomEnd;
			sprintf(message, "CITY_OB");
			break;

		case ANCIENT_OBJ_BANK:
			bankRomStart	= (u32)&_objBank7SegmentRomStart;
			bankRomEnd		= (u32)&_objBank7SegmentRomEnd;
			sprintf(message, "ANCIENT_OB");
			break;

		case LANGUAGE_OBJ_BANK:
			bankRomStart	= (u32)&_objBank8SegmentRomStart;
			bankRomEnd		= (u32)&_objBank8SegmentRomEnd;
			sprintf(message, "LANG_OB");
			break;
		  */
		default:
			dprintf"ERROR: no object bank specified....\n"));
			for(;;);
			break;
	}

	bankSize = bankRomEnd - bankRomStart;

	objectBank = (char *)JallocAlloc(DMAGetSize(bankRomStart,bankRomEnd),YES,message);

//start download from rom

	DMAMemory(objectBank, bankRomStart, bankRomEnd);
	if(objectBank)
		dprintf"Loaded object bank %s (size %d)\n",message,(int)bankSize));
	else
	{
		dprintf"Unable to load object bank %s\n",message));
		return;
	}

//store a pointer to the object bank so it may be free'd at a later date	
	while(objectBanks[++x].freePtr != 0);
	if(x >= MAX_OBJECT_BANKS)
	{
		dprintf"ERROR:too many object banks\n"));
		return;
	}

	objectBanks[x].freePtr = objectBank;
	objectBanks[x].objList = (OBJECT_DESCRIPTOR *)objectBank;
	y = 0;
	while(objectBanks[x].objList[y].objCont)
	{
		obj = (OBJECT *)objectBanks[x].objList[y].objCont;
		objectBanks[x].objList[y].objCont = (OBJECT_CONTROLLER *)Rom2Ram((u32)obj, (u32)objectBank);
		objCont = objectBanks[x].objList[y].objCont;
				
		objCont->object = (OBJECT *)Rom2Ram((u32)objCont->object, (u32)objectBank);
		
		//restore animation pointers
		if(objCont->animation)
		{
			objCont->animation = (ACTOR_ANIMATION *)Rom2Ram((u32)objCont->animation, (u32)objectBank);
			if(objCont->animation->anims)
				objCont->animation->anims = (animation *)Rom2Ram((u32)objCont->animation->anims, (u32)objectBank);
		}
		RestoreObjectPointers(objCont->object, (u32)objectBank);
//		if((objCont->object->numScaleKeys == 1) && (objCont->object->numMoveKeys == 1) && (objCont->object->numRotateKeys == 1))
//			objCont->radius = CalcRadius(objCont->object->mesh);

		y++;
	}
	objectBanks[x].numObjects = y;	

//	if(num == SYSTEM_OBJ_BANK)
//		InitFadeOverlay();
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
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteObjectDisplayListGouraud(OBJECT *obj)
{
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
				(int)vertex->v[1],
				(int)vertex->v[2],0,
				tcp->v[0], (tcp++)->v[1],
				vNormalPtr->v[X], vNormalPtr->v[Y], vNormalPtr->v[Z], xluFact);

			vertex = baseVertices + faceIndex->v[Y];
			vNormalPtr = baseVNormalPtr + faceIndex->v[Y];
			V((vtxPtr+1), 
				(int)vertex->v[0],
				(int)vertex->v[1],
				(int)vertex->v[2],0,
				tcp->v[0], (tcp++)->v[1],
				vNormalPtr->v[X], vNormalPtr->v[Y], vNormalPtr->v[Z], xluFact);

			vertex = baseVertices + faceIndex->v[Z];
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
				gDPLoadTextureBlock(glistp++, texture->data , texture->format, G_IM_SIZ_16b, texture->sx, texture->sy, 0,G_TX_WRAP /*| G_TX_MIRROR*/, G_TX_WRAP /*| G_TX_MIRROR*/,texture->TCScaleX, texture->TCScaleY, G_TX_NOLOD, G_TX_NOLOD);
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
				gDPLoadTextureBlock(glistp++, texture->data , texture->format, G_IM_SIZ_8b, texture->sx, texture->sy, 0,G_TX_WRAP/* | G_TX_MIRROR*/, G_TX_WRAP/* | G_TX_MIRROR*/,texture->TCScaleX, texture->TCScaleY, G_TX_NOLOD, G_TX_NOLOD);
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
				gDPLoadTextureBlock_4b(glistp++, texture->data , texture->format, texture->sx, texture->sy, 0,G_TX_WRAP/* | G_TX_MIRROR*/, G_TX_WRAP/* | G_TX_MIRROR*/,texture->TCScaleX, texture->TCScaleY, G_TX_NOLOD, G_TX_NOLOD);
			}
			break;
	}
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
	if(codeDrawingRequest == TRUE)
		dprintf"Stop Drawing Requested from '%s'\n", whereami));
	codeDrawingRequest = FALSE;
	while(gfxIsDrawing == TRUE);
	disableGraphics = TRUE;
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
	if(codeDrawingRequest == FALSE)
		dprintf"Start Drawing Requested from '%s'\n", whereami));
	codeDrawingRequest = TRUE;
	while(gfxIsDrawing == FALSE);
	disableGraphics = FALSE;
	onlyDrawBackdrops = FALSE;
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
