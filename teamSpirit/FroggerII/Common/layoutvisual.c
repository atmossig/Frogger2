/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: layoutvisual.c
	Programmer	: Andrew Eder
	Date		: 01/03/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

//#define N64_PLAY_SONG

#include <ultra64.h>

#include "incs.h"


unsigned long worldNum;
unsigned long levelNum;


// world visual data - for texture and object banks
WORLD_VISUAL_DATA worldVisualData[MAX_WORLDS];

UBYTE darkenedLevel		= 0;
short lightIntensity	= 255;


// OVERALL WORLD / LEVEL VISUAL DATA BLUEPRINT

WORLD_VISUAL_DATA worldVisualData[MAX_WORLDS] = 
{
//------------------------------------------------ GARDEN
	{
	"garden",					// description
	WORLDID_GARDEN,				// world ID
	GENERIC_GARDEN_OBJ_BANK,		// master object bank
	GENERIC_GARDEN_TEX_BANK,	// master texture bank
	WORLD_OPEN,
	6,							// number of levels
		{ 
		"the spawn lawn",		LEVELID_GARDENLAWN,LEVEL1_GARDEN_OBJ_BANK,LEVEL1_GARDEN_TEX_BANK,GARDENLEV1_COL,GARDENLEV1_ENT,LEVEL_OPEN,WORLDID_ANCIENT,0,
		"a-maze-ing garden",	LEVELID_GARDENMAZE,LEVEL2_GARDEN_OBJ_BANK,LEVEL2_GARDEN_TEX_BANK,GARDENLEV2_COL,GARDENLEV2_ENT,LEVEL_OPEN,WORLDID_SPACE,0,
		"vegging out",			LEVELID_GARDENVEGPATCH,LEVEL3_GARDEN_OBJ_BANK,LEVEL3_GARDEN_TEX_BANK,GARDENLEV3_COL,GARDENLEV3_ENT,LEVEL_OPEN,WORLDID_CITY,0,
		"garden boss",			LEVELID_GARDENTREETOPSA,BOSSA_GARDEN_OBJ_BANK,BOSSA_GARDEN_TEX_BANK,GARDENMASTERA_COL,GARDENLEV4_ENT,LEVEL_OPEN,WORLDID_SUBTERRANEAN,0,
		"tree tops b", 			LEVELID_GARDENTREETOPSB,BOSSB_GARDEN_OBJ_BANK,BOSSB_GARDEN_TEX_BANK,GARDENMASTERB_COL,GARDENLEV5_ENT,LEVEL_OPEN,0,0,
		"garden bonus",			LEVELID_GARDENLOGROLL,BONUS_GARDEN_OBJ_BANK,BONUS_GARDEN_TEX_BANK,GARDENBONUS_COL,GARDENLEV1_ENT,LEVEL_OPEN,-1,0,
		"garden multi",			LEVELID_GARDEN6,MULTI_GARDEN_OBJ_BANK,MULTI_GARDEN_TEX_BANK,GARDENMULTI_COL,GARDENLEV1_ENT,LEVEL_OPEN,-1,0,
		},
	},

//------------------------------------------------ ANCIENT

	{
	"ancient",			// description
	WORLDID_ANCIENT,		// world ID
	GENERIC_ANCIENT_OBJ_BANK,	// master object bank
	GENERIC_ANCIENT_TEX_BANK,	// master texture bank
	WORLD_CLOSED,
	5,						// number of levels
		{ 
		"ancient 1",	LEVELID_ANCIENTRUINEDCITY,LEVEL1_ANCIENT_OBJ_BANK,LEVEL1_ANCIENT_TEX_BANK,ANCIENTLEV1_COL,GARDENLEV1_ENT,LEVEL_OPEN,-1,0,
		"ancient 2",	LEVELID_ANCIENTVOTG,LEVEL2_ANCIENT_OBJ_BANK,LEVEL2_ANCIENT_TEX_BANK,ANCIENTLEV2_COL,GARDENLEV1_ENT,LEVEL_OPEN,-1,0,
		"ancient 3",	LEVELID_ANCIENTMOVEIT,LEVEL3_ANCIENT_OBJ_BANK,LEVEL3_ANCIENT_TEX_BANK,ANCIENTLEV3_COL,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"ancient boss",	LEVELID_ANCIENTTOWEROTRAPS,BOSSA_ANCIENT_OBJ_BANK,BOSSA_ANCIENT_TEX_BANK,ANCIENTMASTERA_COL,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"ancient bonus",LEVELID_ANCIENT5,BONUS_ANCIENT_OBJ_BANK,BONUS_ANCIENT_TEX_BANK,ANCIENTBONUS_COL,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"ancient multi",LEVELID_ANCIENT6,MULTI_ANCIENT_OBJ_BANK,MULTI_ANCIENT_TEX_BANK,ANCIENTMULTI_COL,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		},
	},


//------------------------------------------------ SPACE

	{
	"space",			// description
	WORLDID_SPACE,		// world ID
	GENERIC_SPACE_OBJ_BANK,		// master object bank
	GENERIC_SPACE_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	5,						// number of levels
		{ 
		"space 1",		LEVELID_SPACE1,LEVEL1_SPACE_OBJ_BANK,LEVEL1_SPACE_TEX_BANK,SPACELEV1_COL,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"space 2",		LEVELID_SPACE2,LEVEL2_SPACE_OBJ_BANK,LEVEL2_SPACE_TEX_BANK,SPACELEV2_COL,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"space 3",		LEVELID_SPACE3,LEVEL3_SPACE_OBJ_BANK,LEVEL3_SPACE_TEX_BANK,SPACELEV3_COL,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"space boss",	LEVELID_SPACE4,BOSSA_SPACE_OBJ_BANK,BOSSA_SPACE_TEX_BANK,SPACEMASTER_COL,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"space bonus",	LEVELID_SPACE5,BONUS_SPACE_OBJ_BANK,BONUS_SPACE_TEX_BANK,SPACEBONUS_COL,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"space multi",	LEVELID_SPACE6,MULTI_SPACE_OBJ_BANK,MULTI_SPACE_TEX_BANK,SPACEMULTI_COL,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		},
	},

//------------------------------------------------ CITY

	{
	"city",				// description
	WORLDID_CITY,		// world ID
	GENERIC_CITY_OBJ_BANK,		// master object bank
	GENERIC_CITY_TEX_BANK,		// master texture bank
	WORLD_OPEN,
	5,						// number of levels
		{ 
		"down the docks",		LEVELID_CITYDOCKS,LEVEL1_CITY_OBJ_BANK,LEVEL1_CITY_TEX_BANK,CITYLEV1_COL,GARDENLEV1_ENT,LEVEL_OPEN,WORLDID_TOYSHOP,0,
		"warehouse wandering",	LEVELID_CITYWAREHOUSE,LEVEL2_CITY_OBJ_BANK,LEVEL2_CITY_TEX_BANK,CITYLEV2_COL,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"on the streets",		LEVELID_CITYSTREETS,LEVEL3_CITY_OBJ_BANK,LEVEL3_CITY_TEX_BANK,CITYLEV3_COL,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"city tower",			LEVELID_CITYTOWER,BOSSA_CITY_OBJ_BANK,BOSSA_CITY_TEX_BANK,CITYMASTER_COL,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"city bonus",			LEVELID_CITYBONUS,BONUS_CITY_OBJ_BANK,BONUS_CITY_TEX_BANK,CITYBONUS_COL,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"city multi",			LEVELID_CITYMULTI,MULTI_CITY_OBJ_BANK,MULTI_CITY_TEX_BANK,CITYMULTI_COL,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		},
	},

//------------------------------------------------ SUBTERRANEAN

	{
	"subterranean",				// description
	WORLDID_SUBTERRANEAN,		// world ID
	GENERIC_SUBTERRANEAN_OBJ_BANK,		// master object bank
	GENERIC_SUBTERRANEAN_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	5,						// number of levels
		{ 
		"sub 1",		LEVELID_SUBTERRANEAN1,LEVEL1_SUBTERRANEAN_OBJ_BANK,LEVEL1_SUBTERRANEAN_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"sub 2",		LEVELID_SUBTERRANEAN2,LEVEL2_SUBTERRANEAN_OBJ_BANK,LEVEL2_SUBTERRANEAN_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"sub 3",		LEVELID_SUBTERRANEAN3,LEVEL3_SUBTERRANEAN_OBJ_BANK,LEVEL3_SUBTERRANEAN_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"sub boss",		LEVELID_SUBTERRANEAN4,BOSSA_SUBTERRANEAN_OBJ_BANK,BOSSA_SUBTERRANEAN_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"sub bonus",	LEVELID_SUBTERRANEAN5,BONUS_SUBTERRANEAN_OBJ_BANK,BONUS_SUBTERRANEAN_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"sub multi",	LEVELID_SUBTERRANEAN6,MULTI_SUBTERRANEAN_OBJ_BANK,MULTI_SUBTERRANEAN_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		},
	},

//------------------------------------------------ LABORATORY

	{
	"laboratory",				// description
	WORLDID_LABORATORY,		// world ID
	GENERIC_LABORATORY_OBJ_BANK,		// master object bank
	GENERIC_LABORATORY_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	5,						// number of levels
		{ 
		"lab 1",		LEVELID_LABORATORY1,LEVEL1_LABORATORY_OBJ_BANK,LEVEL1_LABORATORY_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"lab 2",		LEVELID_LABORATORY2,LEVEL2_LABORATORY_OBJ_BANK,LEVEL2_LABORATORY_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"lab 3",		LEVELID_LABORATORY3,LEVEL3_LABORATORY_OBJ_BANK,LEVEL3_LABORATORY_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"lab boss",		LEVELID_LABORATORY4,BOSSA_LABORATORY_OBJ_BANK,BOSSA_LABORATORY_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"lab bonus",	LEVELID_LABORATORY5,BONUS_LABORATORY_OBJ_BANK,BONUS_LABORATORY_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"lab multi",	LEVELID_LABORATORY6,MULTI_LABORATORY_OBJ_BANK,MULTI_LABORATORY_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		},
	},

//------------------------------------------------ TOYSHOP

	{
	"toy shop",				// description
	WORLDID_TOYSHOP,		// world ID
	GENERIC_TOYSHOP_OBJ_BANK,		// master object bank
	GENERIC_TOYSHOP_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	5,						// number of levels
		{ 
		"toy shop 1",		LEVELID_TOYSHOP1,LEVEL1_TOYSHOP_OBJ_BANK,LEVEL1_TOYSHOP_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"toy shop 2",		LEVELID_TOYSHOP2,LEVEL2_TOYSHOP_OBJ_BANK,LEVEL2_TOYSHOP_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"toy shop 3",		LEVELID_TOYSHOP3,LEVEL3_TOYSHOP_OBJ_BANK,LEVEL3_TOYSHOP_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"toy shop boss",	LEVELID_TOYSHOP4,BOSSA_TOYSHOP_OBJ_BANK,BOSSA_TOYSHOP_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"toy shop bonus",	LEVELID_TOYSHOP5,BONUS_TOYSHOP_OBJ_BANK,BONUS_TOYSHOP_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"toy shop multi",	LEVELID_TOYSHOP6,MULTI_TOYSHOP_OBJ_BANK,MULTI_TOYSHOP_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		},
	},


//------------------------------------------------ HALLOWEEN

	{
	"halloween",					// description
	WORLDID_HALLOWEEN,			// world ID
	GENERIC_HALLOWEEN_OBJ_BANK,			// master object bank
	GENERIC_HALLOWEEN_TEX_BANK,			// master texture bank
	WORLD_CLOSED,
	5,						// number of levels 
		{ 
		"halloween 1",		LEVELID_HALLOWEEN1,	LEVEL1_HALLOWEEN_OBJ_BANK,LEVEL1_HALLOWEEN_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"halloween 2",		LEVELID_HALLOWEEN2,	LEVEL2_HALLOWEEN_OBJ_BANK,LEVEL2_HALLOWEEN_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"halloween 3",		LEVELID_HALLOWEEN3,	LEVEL3_HALLOWEEN_OBJ_BANK,LEVEL3_HALLOWEEN_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"halloween boss",	LEVELID_HALLOWEEN4,	BOSSA_HALLOWEEN_OBJ_BANK,BOSSA_HALLOWEEN_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"halloween bonus",	LEVELID_HALLOWEEN5,	BONUS_HALLOWEEN_OBJ_BANK,BONUS_HALLOWEEN_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"halloween multi",	LEVELID_HALLOWEEN6,	MULTI_HALLOWEEN_OBJ_BANK,MULTI_HALLOWEEN_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		},
	},

	

//------------------------------------------------ SUPERRETRO

	{
	"super retro",			// description
	WORLDID_SUPERRETRO,		// world ID
	GENERIC_RETRO_OBJ_BANK,	// master object bank
	GENERIC_RETRO_TEX_BANK,	// master texture bank
	WORLD_CLOSED,
	5,						// number of levels
		{ 
		"super 1",		LEVELID_SUPERRETRO1,LEVEL1_RETRO_OBJ_BANK,LEVEL1_RETRO_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"super 2",		LEVELID_SUPERRETRO2,LEVEL2_RETRO_OBJ_BANK,LEVEL2_RETRO_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"super 3",		LEVELID_SUPERRETRO3,LEVEL3_RETRO_OBJ_BANK,LEVEL3_RETRO_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"super boss",	LEVELID_SUPERRETRO4,BOSSA_RETRO_OBJ_BANK,BOSSA_RETRO_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"super bonus",	LEVELID_SUPERRETRO5,BONUS_RETRO_OBJ_BANK,BONUS_RETRO_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		"super multi",	LEVELID_SUPERRETRO6,MULTI_RETRO_OBJ_BANK,MULTI_RETRO_TEX_BANK,-1,GARDENLEV1_ENT,LEVEL_CLOSED,-1,0,
		},
	}

};


/*	--------------------------------------------------------------------------------
	Function		: LoadVisualBanksForWorld
	Purpose			: loads the relevant object and texture banks for a world
	Parameters		: unsigned long,unsigned long
	Returns			: void
	Info			: 
*/
void LoadVisualBanksForWorld(unsigned long worldID,unsigned long levelID)
{
	unsigned long i;
	long objBankToUse;
	long texBankToUse;

	objBankToUse = worldVisualData[worldID].masterObjectBank;
	texBankToUse = worldVisualData[worldID].masterTextureBank;

	// load the relevant master object / texture banks
	dprintf"Loading...\n"));
	LoadTextureBank(SYSTEM_TEX_BANK);
	LoadTextureBank ( GENERIC_TEX_BANK );
	LoadTextureBank(texBankToUse);
	LoadObjectBank(objBankToUse);

	// load the relevant child object bank(s) if required
	if(levelID == LOAD_ALL_LEVELBANKS)
	{
		for(i=0; i< MAX_LEVELS; i++)
		{
			if(worldVisualData[worldID].levelVisualData[i].collBank > -1)
			{
				if(worldVisualData[worldID].levelVisualData[i].textureBankToUse > 0)
					LoadTextureBank(worldVisualData[worldID].levelVisualData[i].textureBankToUse);
				if(worldVisualData[worldID].levelVisualData[i].objectBankToUse > 0)
					LoadObjectBank(worldVisualData[worldID].levelVisualData[i].objectBankToUse);
			}
		}
	}
	else
	{
		if(worldVisualData[worldID].levelVisualData[levelID].collBank > -1)
		{
			if(worldVisualData[worldID].levelVisualData[levelID].textureBankToUse > 0)
				LoadTextureBank(worldVisualData[worldID].levelVisualData[levelID].textureBankToUse);
			if(worldVisualData[worldID].levelVisualData[levelID].objectBankToUse > 0)
				LoadObjectBank(worldVisualData[worldID].levelVisualData[levelID].objectBankToUse);

		}
	}

}


/*	--------------------------------------------------------------------------------
	Function		: FreeAllObjectBanks
	Purpose			: frees up ALL object banks
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeAllObjectBanks()
{
	unsigned long i;

	for(i=0; i<MAX_OBJECT_BANKS; i++)
	{
		if(textureBanks[i].freePtr)
			JallocFree((UBYTE**)&objectBanks[i].freePtr);
	}
}


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: InitLevel
	Purpose		:
	Parameters	: unsigned long , unsigned long
	Returns		: void 
*/
void InitLevel ( unsigned long worldID, unsigned long levelID )
{
	StopDrawing("initlev");

	// prepare screen for fade in
	fadeOut		= 255;
	fadeStep	= 8;
	fadeDir		= FADE_IN;
	doScreenFade = 63;
	
	LoadMapBank ( worldVisualData[worldID].levelVisualData[levelID].collBank );

	// load relevant texture and object banks
	LoadVisualBanksForWorld(worldID,levelID);

	InitSpriteFrameLists();
	InitFXLinkedLists();

	player[0].worldNum = worldID;
	player[0].levelNum = levelID;

	InitCamera( worldID, levelID );

	if ( demoTug )
	{
		JallocFree ( (UBYTE**)&demoTug );
		demoTug = NULL;
	}
	// ENDIF

	CreateLevelObjects(worldID,levelID);

	InitEnemyLinkedList();
	InitPlatformLinkedList();
	InitGaribLinkedList();
	InitSpriteOverlayLinkedList();
	InitTextOverlayLinkedList();
	
	// Load level entitys
	//LoadLevelEntitys ( worldVisualData[worldID].levelVisualData[levelID].entityDat );

	//InitPlatformsForLevel(worldID,levelID);
	//InitEnemiesForLevel(worldID,levelID);
	//InitCollectablesForLevel(worldID,levelID);
	InitCameraForLevel(worldID,levelID);
	//InitCameosForLevel ( worldID, levelID );
		
	CreateFrogger(1,1,1,1); // This also creates the babies

	//InitEventsForLevel(worldID, levelID);

	//LoadTextureBank ( SYSTEM_TEX_BANK );
	switch ( worldID )
	{
		case WORLDID_GARDEN:
			switch ( levelID )
			{
				case LEVELID_GARDENLAWN:
					camFacing = ((camFacing+2)%4);
				break;
			};
		break;
		case WORLDID_CITY:
			switch ( levelID )
			{
				case LEVELID_CITYDOCKS:
					camFacing = ((camFacing+2)%4);
				break;
				case LEVELID_CITYWAREHOUSE:
					//camFacing = ( ( camFacing
				break;
			};
		break;
	};

	// prepare the text overlays for the current level
	InitInGameTextOverlays(worldID,levelID);

	StartDrawing("initlev");

#ifndef PC_VERSION
	MusSetMasterVolume(MUSFLAG_SONGS,16500);
#ifdef N64_PLAY_SONG
	PrepareSong(1);
#endif
#endif
}


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: FreeLevel
	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/
void FreeLevel(void)
{
	FreeActorList();

	FreeFXSwarmLinkedList();
	FreeFXSmokeLinkedList();
	FreeFXRippleLinkedList();
	FreeFXExplodeParticleLinkedList();
	FreeGaribLinkedList();
	FreeTextOverlayLinkedList();
	FreeSpriteOverlayLinkedList();
	FreeSpriteLinkedList();
	FreeEnemyLinkedList();
	FreePlatformLinkedList();

	FreeAnimationList();
	FreeTextureList();
	FreeBackdrop(myBackdrop);
	FreeAllObjectBanks();
	FreeAllTextureBanks();
	FreeCameoList();
	FreeTransCameraList();
	FreeMapBank();

	InitTextOverlayLinkedList();
	InitSpriteOverlayLinkedList();
	InitSpriteLinkedList();
	
	myBackdrop	= NULL;

	pauseMode		= 0;
	darkenedLevel	= 0;
}



/*	--------------------------------------------------------------------------------
	Function		: FreeAllLists
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void FreeAllLists ( void )
{
	/*<JIM>*/
	if( triggerList.numEntries )
		KillAllTriggers( );

	//	if( ambientSoundList.numEntries )
	//		KillAmbientSfx( );
	/*</JIM>*/

	FreeActorList();
	FreeAnimationList();
	FreeTextureList();
	FreeBackdrop(myBackdrop);

	FreeLevel();
	
	FreeAllObjectBanks();
	FreeAllTextureBanks();
	FreeCameoList();

	myBackdrop	= NULL;

	pauseMode		= 0;
	darkenedLevel	= 0;
}
