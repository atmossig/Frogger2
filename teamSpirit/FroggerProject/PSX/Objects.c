#include <string.h>
#include <stdio.h>

#include "shell.h"
//#include "global.h"
#include "sonylibs.h"
#include <islutil.h>
//#include <islpsi.h>

#include "islpsi.h"

#include "Layout.h"
#include "bff_load.h"
#include "Objects.h"

#include "world.h"
#include "bff_load.h"

int numObjectBanks = 0;

BFF_Header *objectBanks [ MAX_OBJECT_BANKS ];

int		PSIFileListPos = 0;
char	PSIFileList [ 10 ][12];


void LoadObjectBank ( int objectBank )
{
	char BFFfileName[256];
	char PILfileName[256];

	switch ( objectBank )
	{
		case INGAMEGENERIC_OBJ_BANK:
					sprintf ( BFFfileName, "OBJECTS\\GENERIC.BFF" );					
					sprintf ( PILfileName, "OBJECTS\\GENERIC.PIL" );					
				break;

		case GARDENLEV1_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\GARDEN\\GARDEN1.BFF" );
				sprintf ( PILfileName, "OBJECTS\\GARDEN\\GARDEN1.PIL" );
			break;

		case GARDENMULTI_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\GARDEN\\GARDENMULTI.BFF" );
				sprintf ( PILfileName, "OBJECTS\\GARDEN\\GARDEN1.PIL" );
			break;

		case ANCIENTLEV1_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\ANCIENTS\\ANCIENTS1.BFF" );
				sprintf ( PILfileName, "OBJECTS\\ANCIENTS\\ANCIENTS1.PIL" );
			break;

		case ANCIENTLEV3_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\ANCIENTS\\ANCIENTS2.BFF" );
				sprintf ( PILfileName, "OBJECTS\\ANCIENTS\\ANCIENTS2.PIL" );
			break;

		case ANCIENTBOSS_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\ANCIENTS\\ANCIENTSBOSS.BFF" );
				sprintf ( PILfileName, "OBJECTS\\ANCIENTS\\ANCIENTSBOSS.PIL" );
			break;

		case ANCIENTMULTI_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\ANCIENTS\\ANCIENTSMULTI.BFF" );
				sprintf ( PILfileName, "OBJECTS\\ANCIENTS\\ANCIENTSMULTI.PIL" );
			break;

		case SPACELEV1_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\SPACE\\SPACE1.BFF" );
				sprintf ( PILfileName, "OBJECTS\\SPACE\\SPACE1.PIL" );
			break;

		case SPACELEV3_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\SPACE\\SPACE2.BFF" );
				sprintf ( PILfileName, "OBJECTS\\SPACE\\SPACE2.PIL" );
			break;

		case SPACEBOSS_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\SPACE\\SPACE3.BFF" );
				sprintf ( PILfileName, "OBJECTS\\SPACE\\SPACE3.PIL" );
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

		case CITYMULTI_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\CITY\\CITYMULTI.BFF" );
				sprintf ( PILfileName, "OBJECTS\\CITY\\CITY3.PIL" );
			break;

		case SUBTERRANEANLEV1_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\SUB\\SUB1.BFF" );
				sprintf ( PILfileName, "OBJECTS\\SUb\\SUB1.PIL" );
			break;
		
		case SUBTERRANEANLEV2_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\SUB\\SUB2.BFF" );
				sprintf ( PILfileName, "OBJECTS\\SUb\\SUB2.PIL" );
			break;

		case SUBTERRANEANBOSS_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\SUB\\SUB4.BFF" );
				sprintf ( PILfileName, "OBJECTS\\SUb\\SUB4.PIL" );
			break;

		case LABORATORYLEV1_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\LAB\\LAB1.BFF" );
				sprintf ( PILfileName, "OBJECTS\\LAB\\LAB1.PIL" );
			break;

		case LABORATORYLEV3_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\LAB\\LAB2.BFF" );
				sprintf ( PILfileName, "OBJECTS\\LAB\\LAB2.PIL" );
			break;

		case LABORATORYBOSSA_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\LAB\\LABBOSS.BFF" );
				sprintf ( PILfileName, "OBJECTS\\LAB\\LABBOSS.PIL" );
			break;

//		case LABORATORYMULTI_OBJ_BANK:
		case LABORATORYMULTI1_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\LAB\\LABMULTI.BFF" );
				sprintf ( PILfileName, "OBJECTS\\LAB\\LABMULTI.PIL" );
			break;

		case HALLOWEENLEV1_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\HALLOWEEN\\HALLOWEEN1.BFF" );
				sprintf ( PILfileName, "OBJECTS\\HALLOWEEN\\HALLOWEEN1.PIL" );
			break;

		case HALLOWEENLEV2_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\HALLOWEEN\\HALLOWEEN2.BFF" );
				sprintf ( PILfileName, "OBJECTS\\HALLOWEEN\\HALLOWEEN2.PIL" );
			break;

		case HALLOWEENLEV3_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\HALLOWEEN\\HALLOWEEN3.BFF" );
				sprintf ( PILfileName, "OBJECTS\\HALLOWEEN\\HALLOWEEN3.PIL" );
			break;

		case HALLOWEENBOSS_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\HALLOWEEN\\HALLOBOSS.BFF");
				sprintf ( PILfileName, "OBJECTS\\HALLOWEEN\\HALLOBOSS.PIL" );
			break;


		case HALLOWEENBOSSB_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\HALLOWEEN\\HALLOBOSSB.BFF" );
				sprintf ( PILfileName, "OBJECTS\\HALLOWEEN\\HALLOBOSSB.PIL" );
			break;

		case RETROLEV1_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\RETRO\\RETRO1.BFF" );
				sprintf ( PILfileName, "OBJECTS\\RETRO\\SUPER.PIL" );
			break;

		case RETROLEV2_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\RETRO\\RETRO2.BFF" );
				sprintf ( PILfileName, "OBJECTS\\RETRO\\SUPER.PIL" );
			break;

		case RETROLEV3_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\RETRO\\RETRO3.BFF" );
				sprintf ( PILfileName, "OBJECTS\\RETRO\\SUPER.PIL" );
			break;

		case RETROLEV4_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\RETRO\\RETRO4.BFF" );
				sprintf ( PILfileName, "OBJECTS\\RETRO\\SUPER.PIL" );
			break;

		case RETROLEV5_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\RETRO\\RETRO5.BFF" );
				sprintf ( PILfileName, "OBJECTS\\RETRO\\SUPER.PIL" );
			break;

		case RETROLEV6_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\RETRO\\RETRO6.BFF" );
				sprintf ( PILfileName, "OBJECTS\\RETRO\\SUPER.PIL" );
			break;

		case RETROLEV7_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\RETRO\\RETRO7.BFF" );
				sprintf ( PILfileName, "OBJECTS\\RETRO\\SUPER.PIL" );
			break;

		case RETROLEV8_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\RETRO\\RETRO8.BFF" );
				sprintf ( PILfileName, "OBJECTS\\RETRO\\SUPER.PIL" );
			break;

		case RETROLEV9_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\RETRO\\RETRO9.BFF" );
				sprintf ( PILfileName, "OBJECTS\\RETRO\\SUPER.PIL" );
			break;

		case RETROLEV10_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\RETRO\\RETRO10.BFF" );
				sprintf ( PILfileName, "OBJECTS\\RETRO\\SUPER.PIL" );
			break;


		case RETROMULTI1_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\RETRO\\RETROMULTI.BFF" );
				sprintf ( PILfileName, "OBJECTS\\RETRO\\SUPER.PIL" );
			break;

		case FRONTEND_OBJ_BANK:
				sprintf ( PILfileName, "OBJECTS\\HUB\\HUB.PIL" );
			break;

		case FRONTEND1_OBJ_BANK:
 				sprintf ( BFFfileName, "OBJECTS\\HUB\\HUB1.BFF" );
			break;

		case FRONTEND2_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\TITLE\\LEVEL.BFF" );
			break;

		case FRONTEND4_OBJ_BANK:
				sprintf ( BFFfileName, "OBJECTS\\TITLE\\MULTISEL.BFF" );
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
			int i;
			FMA_MESH_HEADER **mesh;


			utilPrintf("Loading BFF World %s \n", BFFfileName);
					//GetWorldFrom_BFF(fileName);
			objectBanks [ numObjectBanks++ ] = BFF_LoadFile ( BFFfileName );
			fma_world = (void *)BFF_FindObject(BFF_FMA_WORLD_ID,utilStr2CRC("world.psi"));	// Should only be one world in the BFF

			if ( !fma_world )
				return 0;

			mesh = ADD2POINTER(fma_world,sizeof(FMA_WORLD));
			
			for ( i = fma_world->n_meshes; i != 0; i--, mesh++ )
			{
				(*mesh)->flags |= DRAW_SEGMENT;
			}

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
	fma_world = NULL;
}


void ExtractPsiNames ( void )
{
	char *addr;
	char *PSIname=0;

	int psiM;
	int *table;//,*crcs;
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
