/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: banks.h
	Programmer	: David Swift
	Date		: 21/3/00

----------------------------------------------------------------------------------------------- */

#include "mdx.h"
#include "banks.h"

void LoadObjectBank(int objectBank)					
{
	char fileName[MAX_PATH] = "";

	switch ( objectBank )
	{
		case INGAMEGENERIC_OBJ_BANK:
			strcpy(fileName, "gamegeneric"); break;

// Garden

		case GARDENMASTER_OBJ_BANK:
			strcpy(fileName, "gardenmaster");	break;

		case GARDENLEV1_OBJ_BANK:
			strcpy(fileName, "garden1"); break;

		case GARDENLEV2_OBJ_BANK:
			strcpy(fileName, "garden1"); break;

// Ancient

		case ANCIENTMASTER_OBJ_BANK:
			strcpy(fileName, "ancientsmaster"); break;

		case ANCIENTLEV1_OBJ_BANK:				
			strcpy(fileName, "ancients1"); break;

		case ANCIENTLEV2_OBJ_BANK:				
			strcpy(fileName, "ancients2"); break;

		case ANCIENTLEV3_OBJ_BANK:				
			strcpy(fileName, "ancients3"); break;

		case ANCIENTBOSS_OBJ_BANK:				
			strcpy(fileName, "ancientsboss"); break;

// Space

		case SPACEMASTER_OBJ_BANK:					
			strcpy(fileName, "spacemaster"); break;

		case SPACELEV1_OBJ_BANK:				
			strcpy( fileName, "space1" ); break;

		case SPACELEV2_OBJ_BANK:
			strcpy(fileName, "space2"); break;

// City

		case CITYMASTER_OBJ_BANK:
			strcpy(fileName, "citymaster"); break;

		case CITYLEV1_OBJ_BANK:
			strcpy(fileName, "city1"); break;

		case CITYLEV2_OBJ_BANK:
			strcpy(fileName, "city2"); break;

		case CITYLEV3_OBJ_BANK:
			strcpy(fileName, "city3"); break;

// Subterranean

		case SUBTERRANEANMASTER_OBJ_BANK:
			strcpy(fileName, "submaster"); break;

		case SUBTERRANEANLEV1_OBJ_BANK:
			strcpy(fileName, "sub1"); break;

		case SUBTERRANEANLEV2_OBJ_BANK:
			strcpy(fileName, "sub2"); break;

		case SUBTERRANEANLEV3_OBJ_BANK:
			strcpy(fileName, "sub2"); break;

// Laboratory

		case LABORATORYMASTER_OBJ_BANK:
			strcpy(fileName, "labmaster"); break;

		case LABORATORYLEV1_OBJ_BANK:
			strcpy(fileName, "lab1"); break;

		case LABORATORYLEV2_OBJ_BANK:
			strcpy(fileName, "lab2"); break;

		case LABORATORYLEV3_OBJ_BANK:
			strcpy(fileName, "lab3"); break;

// Halloween

		case HALLOWEENMASTER_OBJ_BANK:
			strcpy(fileName, "halloweenmaster"); break;

		case HALLOWEENLEV1_OBJ_BANK:
			strcpy(fileName, "halloween1"); break;

		case HALLOWEENLEV2_OBJ_BANK:
			strcpy(fileName, "halloween2"); break;

		case HALLOWEENLEV3_OBJ_BANK:
			strcpy(fileName, "halloween3"); break;

// Front-end

		case FRONTEND_OBJ_BANK:
			strcpy(fileName, "title"); break;

		case FRONTEND1_OBJ_BANK:
			strcpy(fileName, "start"); break;

		case FRONTEND2_OBJ_BANK:
			strcpy(fileName, "levela"); break;

		case FRONTEND3_OBJ_BANK:
			strcpy(fileName, "levelb"); break;

		case FRONTEND4_OBJ_BANK:
			strcpy(fileName, "multisel"); break;

		case FRONTEND5_OBJ_BANK:
			strcpy(fileName, "language"); break;

// Multiplayer

		case MULTIPLAYER_OBJ_BANK:
			strcpy(fileName, "language"); break;

		default:
			utilPrintf("Didn't load object bank '%d'\n", objectBank);
			return;
	}

	//newobj(fileName);

	LoadObjBank(fileName,baseDirectory);
}

void LoadTextureBank(int textureBank)					
{
	char fileName[256];

	switch ( textureBank )
	{
		case GARDEN_TEX_BANK:
			strcpy( fileName, "Garden" );
			break;

		case ANCIENT_TEX_BANK:
			strcpy( fileName, "ancient" );
			break;

		case SPACE_TEX_BANK:				
			strcpy( fileName, "space" );
			break;

		case CITY_TEX_BANK:				
			strcpy(fileName, "city");
			break;

		case SUBTERRANEAN_TEX_BANK:
			strcpy(fileName, "subter");
			break;

		case LABORATORY_TEX_BANK:
			strcpy(fileName, "lab");
			break;

		case HALLOWEEN_TEX_BANK:
			strcpy(fileName, "Halloween");
			break;
		
		case FRONT_TEX_BANK:
			strcpy(fileName, "front");
			break;
		
		case TITLES_TEX_BANK:
		case TITLESGENERIC_TEX_BANK:
			strcpy( fileName, "titles" );
			break;

		case FRONTEND_TEX_BANK:				
			strcpy( fileName, "titles" );
			break;

		case INGAMEGENERIC_TEX_BANK:
			strcpy( fileName, "System" );
			break;

		default:
			return;
	}

	LoadTexBank(fileName,baseDirectory);	
	return;
}

void FreeTextureBank(TextureBankType *textureBank)		
{
	return;
}

void FreeAllTextureBanks(void)							
{
	return;
}
