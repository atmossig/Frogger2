#include <string.h>
#include <stdio.h>

#include "shell.h"
//#include "global.h"
#include "sonylibs.h"
#include <islutil.h>
//#include <islpsi.h>

#include <islpsi.h>

#include "Layout.h"
#include "bff_load.h"
#include "Objects.h"

#include "world.h"
#include "bff_load.h"

int numObjectBanks = 0;

BFF_Header *objectBanks [ MAX_OBJECT_BANKS ];

int		PSIFileListPos = 0;
char	PSIFileList [ 10 ][12];

void ExtractPsiNames ( void )
{
	char *addr;
	char *PSIname=0;

	int psiM;
	int *table,*crcs;
	int i;


	addr = (char*)pilLibraryList[1];// = (long*)addr;

	i = (int)*addr;
	(char*)table = addr+4;

	while ( i )
	{
		i--;
		psiM = *(table++);		psiM += (int)addr;

		PSIname = (char*)(psiM);
		utilPrintf("Extracting PSI File Name : %s\n",PSIname);
		sprintf ( PSIFileList [ PSIFileListPos++ ], "%s",  PSIname );

		psiM += 36;
	}
	// ENDWHILE
}

void LoadObjectBank ( int objectBank )
{
	char BFFfileName[256];
	char PILfileName[256];

	switch ( objectBank )
	{
		case INGAMEGENERIC_OBJ_BANK:
					sprintf ( PILfileName, "OBJECTS\\GENERIC.PIL" );					
				break;

		case GARDENMASTER_OBJ_BANK:
					sprintf ( PILfileName, "OBJECTS\\GARDEN.PIL" );					
				break;

		case GARDENLEV1_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\GARDEN\\GARDEN1.BFF" );
			break;

		case GARDENLEV2_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\GARDEN\\GARDEN2.BFF" );
			break;

// Not used becuase we are trying to save money!!!!!!
//		case ANCIENTMASTER_OBJ_BANK:
//					sprintf ( PILfileName, "OBJECTS\\ANCIENTS.PIL" );
//				break;

		case ANCIENTLEV1_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\ANCIENTS\\ANCIENTS1.BFF" );
				sprintf ( PILfileName, "OBJECTS\\ANCIENTS\\ANCIENTS1.PIL" );
			break;

// Not used in the game
//		case ANCIENTLEV2_OBJ_BANK:
//				sprintf ( BFFfileName, "OBJECTS\\ANCIENTS\\ANCIENTS2.BFF" );
//			break;

		case ANCIENTLEV3_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\ANCIENTS\\ANCIENTS3.BFF" );
				sprintf ( PILfileName, "OBJECTS\\ANCIENTS\\ANCIENTS3.PIL" );
			break;

		case ANCIENTBOSS_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\ANCIENTS\\ANCIENTSBOSS.BFF" );
				sprintf ( PILfileName, "OBJECTS\\ANCIENTS\\ANCIENTSBOSS.PIL" );
			break;

		case SPACEMASTER_OBJ_BANK:
					sprintf ( PILfileName, "OBJECTS\\SPACE.PIL" );					
				break;

		case SPACELEV1_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\SPACE\\SPACE1.BFF" );
			break;

		case SPACELEV2_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\SPACE\\SPACE2.BFF" );
			break;

		case CITYLEV1_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\CITY\\CITY1.BFF" );
				sprintf ( PILfileName, "OBJECTS\\CITY\\CITY1.PIL" );
			break;

		case CITYLEV2_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\CITY\\CITY2.BFF" );
				sprintf ( PILfileName, "OBJECTS\\CITY\\CITY2.PIL" );
			break;

		case CITYLEV3_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\CITY\\CITY3.BFF" );
				sprintf ( PILfileName, "OBJECTS\\CITY\\CITY3.PIL" );
			break;

		case SUBTERRANEANMASTER_OBJ_BANK:
					sprintf ( PILfileName, "OBJECTS\\SUB.PIL" );					
				break;

		case SUBTERRANEANLEV1_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\SUB\\SUB1.BFF" );
			break;

		case SUBTERRANEANLEV2_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\SUB\\SUB2.BFF" );
			break;


		case LABORATORYMASTER_OBJ_BANK:
					sprintf ( PILfileName, "OBJECTS\\LAB.PIL" );					
				break;

		case LABORATORYLEV1_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\LAB\\LAB1.BFF" );
			break;

		case LABORATORYLEV2_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\LAB\\LAB2.BFF" );
			break;

		case LABORATORYLEV3_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\LAB\\LAB3.BFF" );
			break;


		case HALLOWEENMASTER_OBJ_BANK:
					sprintf ( PILfileName, "OBJECTS\\HALLO.PIL" );					
				break;

		case HALLOWEENLEV1_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\HALLOWEN\\HALLO1.BFF" );
			break;

		case HALLOWEENLEV2_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\HALLOWEN\\HALLO2.BFF" );
			break;

		case HALLOWEENLEV3_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\HALLOWEN\\HALLO3.BFF" );
			break;


		case FRONTEND_OBJ_BANK:
				sprintf ( PILfileName, "OBJECTS\\HUB.PIL" );
			break;

		case FRONTEND1_OBJ_BANK:
 				sprintf ( BFFfileName, "OBJECTS\\HUB\\HUB1.BFF" );
			break;

		case FRONTEND2_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\TITLE\\LEVEL.BFF" );
			break;

		default:
				utilPrintf ( "Not a valid Object Bank type..............." );
				return;
			break;
	}
	// ENDSWITCH - textureBank

	if ( numObjectBanks < MAX_OBJECT_BANKS )
	{
		char *c = strstr ( BFFfileName, ".BFF" );

		if ( c )
		{
			utilPrintf("Loading BFF World %s \n", BFFfileName);
					//GetWorldFrom_BFF(fileName);
			objectBanks [ numObjectBanks++ ] = BFF_LoadFile ( BFFfileName );
			fma_world = (void *)BFF_FindObject(BFF_FMA_WORLD_ID,utilStr2CRC("WORLD"));	// Should only be one world in the BFF
//			ASSERT(fma_world);
//			chrisSetUpWaterMesh(fma_world);
		}
		else
		{
			utilPrintf("ERROR: Unrecognised BFF file type:  %s \n", BFFfileName);
		}
		// ENDELSEIF

		if ( ( c = strstr ( PILfileName, ".PIL" ) ) )
		{
			utilPrintf("Loading PIL Library %s \n", PILfileName);
			psiLoadPIL  ( PILfileName );
		}
		else
		{
			utilPrintf("ERROR: Unrecognised PIL file type:  %s \n", PILfileName);
		}
		// ENDELSEIF
	}
	else
	{
		utilPrintf("Error Loading: %s : %s, Max Object Banks Reached\n", PILfileName, BFFfileName );
	}
	// ENDELSEIF - 	if ( numObjectBanks < MAX_OBJECT_BANKS )

//	ExtractPsiNames();


}


void FreeObjectBank ( BFF_Header *header )
{
	BFF_ReleaseFile ( header );
}

void FreeAllObjectBanks ( void )
{
	short c;

	utilPrintf("Freeing All Object Banks : OBJECT\n");

	for ( c = 0; c < MAX_OBJECT_BANKS; c++ )
	{
		if ( objectBanks[c] )
			FreeObjectBank ( objectBanks [c] );
		// ENDIF - if ( textureBanks[c] )
	}
	// ENDIF - 	for ( c = o; c < MAX_TEXTURE_BANKS; c++ )
//MM	FREE( ( FMA_WORLD* ) fma_world );
	FREE( fma_world );
	numObjectBanks = 0;
}

