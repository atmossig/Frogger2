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
	"garden",				// description
	WORLDID_GARDEN,			// world ID
	GARDENMASTER_OBJ_BANK,		// master object bank
	GARDEN_TEX_BANK,		// master texture bank
	WORLD_OPEN,
	5,						// number of levels
		{ 
		"the spawn lawn",		LEVELID_GARDENLAWN,GARDENLEV1_OBJ_BANK,0,LAWN_COLL_BANK,LEVEL_OPEN,WORLDID_ANCIENT,0,
		"a-maze-ing garden",	LEVELID_GARDENMAZE,GARDENLEV2_OBJ_BANK,0,MAZE_COLL_BANK,LEVEL_OPEN,WORLDID_SPACE,0,
		"vegging out",			LEVELID_GARDENVEGPATCH,GARDENLEV3_OBJ_BANK,0,VEGPATCH_COLL_BANK,LEVEL_OPEN,WORLDID_CITY,0,
		"garden boss",			LEVELID_GARDEN4,GARDENLEV4_OBJ_BANK,0,-1,LEVEL_OPEN,WORLDID_SUBTERRANEAN,0,
		"garden bonus",			LEVELID_GARDEN5,GARDENBONUS_OBJ_BANK,0,-1,LEVEL_OPEN,-1,0,
		"garden multi",			LEVELID_GARDEN6,GARDENMULTI_OBJ_BANK,0,GARDENMULTI_COLL_BANK,LEVEL_OPEN,-1,0,
		},
	},

//------------------------------------------------ ANCIENT

	{
	"ancient",			// description
	WORLDID_ANCIENT,		// world ID
	ANCIENTMASTER_OBJ_BANK,	// master object bank
	ANCIENT_TEX_BANK,	// master texture bank
	WORLD_CLOSED,
	5,						// number of levels
		{ 
		"ancient 1",	LEVELID_ANCIENT1,ANCIENTLEV1_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"ancient 2",	LEVELID_ANCIENT2,ANCIENTLEV2_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"ancient 3",	LEVELID_ANCIENT3,ANCIENTLEV3_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"ancient boss",	LEVELID_ANCIENT4,ANCIENTLEV4_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"ancient bonus",LEVELID_ANCIENT5,ANCIENTBONUS_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"ancient multi",LEVELID_ANCIENT6,ANCIENTMULTI_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		},
	},


//------------------------------------------------ SPACE

	{
	"space",			// description
	WORLDID_SPACE,		// world ID
	SPACEMASTER_OBJ_BANK,		// master object bank
	SPACE_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	5,						// number of levels
		{ 
		"space 1",		LEVELID_SPACE1,SPACELEV1_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"space 2",		LEVELID_SPACE2,SPACELEV2_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"space 3",		LEVELID_SPACE3,SPACELEV3_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"space boss",	LEVELID_SPACE4,SPACELEV4_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"space bonus",	LEVELID_SPACE5,SPACEBONUS_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"space multi",	LEVELID_SPACE6,SPACEMULTI_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		},
	},

//------------------------------------------------ CITY

	{
	"city",				// description
	WORLDID_CITY,		// world ID
	CITYMASTER_OBJ_BANK,		// master object bank
	CITY_TEX_BANK,		// master texture bank
	WORLD_OPEN,
	5,						// number of levels
		{ 
		"down the docks",		LEVELID_CITYDOCKS,CITYLEV1_OBJ_BANK,0,DOCKSA_COLL_BANK,LEVEL_OPEN,WORLDID_TOYSHOP,0,
		"warehouse wandering",	LEVELID_CITYWAREHOUSE,CITYLEV2_OBJ_BANK,0,WAREHOUSE_COLL_BANK,LEVEL_CLOSED,-1,0,
		"on the streets",		LEVELID_CITYSTREETS,CITYLEV3_OBJ_BANK,0,CSTREETS_COLL_BANK,LEVEL_CLOSED,-1,0,
		"city tower",			LEVELID_CITYTOWER,CITYLEV4_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"city bonus",			LEVELID_CITYBONUS,CITYBONUS_OBJ_BANK,0,CITYBON_COLL_BANK,LEVEL_CLOSED,-1,0,
		"city multi",			LEVELID_CITYMULTI,CITYMULTI_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		},
	},

//------------------------------------------------ SUBTERRANEAN

	{
	"subterranean",				// description
	WORLDID_SUBTERRANEAN,		// world ID
	SUBTERRANEANMASTER_OBJ_BANK,		// master object bank
	SUBTERRANEAN_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	5,						// number of levels
		{ 
		"sub 1",		LEVELID_SUBTERRANEAN1,SUBTERRANEANLEV1_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"sub 2",		LEVELID_SUBTERRANEAN2,SUBTERRANEANLEV2_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"sub 3",		LEVELID_SUBTERRANEAN3,SUBTERRANEANLEV3_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"sub boss",		LEVELID_SUBTERRANEAN4,SUBTERRANEANLEV4_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"sub bonus",	LEVELID_SUBTERRANEAN5,SUBTERRANEANBONUS_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"sub multi",	LEVELID_SUBTERRANEAN6,SUBTERRANEANMULTI_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		},
	},

//------------------------------------------------ LABORATORY

	{
	"laboratory",				// description
	WORLDID_LABORATORY,		// world ID
	LABORATORYMASTER_OBJ_BANK,		// master object bank
	LABORATORY_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	5,						// number of levels
		{ 
		"lab 1",		LEVELID_LABORATORY1,LABORATORYLEV1_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"lab 2",		LEVELID_LABORATORY2,LABORATORYLEV2_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"lab 3",		LEVELID_LABORATORY3,LABORATORYLEV3_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"lab boss",		LEVELID_LABORATORY4,LABORATORYLEV4_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"lab bonus",	LEVELID_LABORATORY5,LABORATORYBONUS_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"lab multi",	LEVELID_LABORATORY6,LABORATORYMULTI_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		},
	},

//------------------------------------------------ TOYSHOP

	{
	"toy shop",				// description
	WORLDID_TOYSHOP,		// world ID
	TOYSHOPMASTER_OBJ_BANK,		// master object bank
	TOYSHOP_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	5,						// number of levels
		{ 
		"toy shop 1",		LEVELID_TOYSHOP1,TOYSHOPLEV1_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"toy shop 2",		LEVELID_TOYSHOP2,TOYSHOPLEV2_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"toy shop 3",		LEVELID_TOYSHOP3,TOYSHOPLEV3_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"toy shop boss",			LEVELID_TOYSHOP4,TOYSHOPLEV4_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"toy shop bonus",		LEVELID_TOYSHOP5,TOYSHOPBONUS_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"toy shop multi",	LEVELID_TOYSHOP6,TOYSHOPMULTI_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		},
	},


//------------------------------------------------ HALLOWEEN

	{
	"halloween",					// description
	WORLDID_HALLOWEEN,			// world ID
	HALLOWEENMASTER_OBJ_BANK,			// master object bank
	HALLOWEEN_TEX_BANK,			// master texture bank
	WORLD_CLOSED,
	5,						// number of levels 
		{ 
		"halloween 1",		LEVELID_HALLOWEEN1,	HALLOWEENLEV1_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"halloween 2",		LEVELID_HALLOWEEN2,	HALLOWEENLEV1_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"halloween 3",		LEVELID_HALLOWEEN3,	HALLOWEENLEV1_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"halloween boss",			LEVELID_HALLOWEEN4,	HALLOWEENLEV1_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"halloween bonus",		LEVELID_HALLOWEEN5,	HALLOWEENBONUS_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"halloween multi",	LEVELID_HALLOWEEN6,	HALLOWEENMULTI_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		},
	},

	

//------------------------------------------------ SUPERRETRO

	{
	"super retro",			// description
	WORLDID_SPACE,		// world ID
	RETROMASTER_OBJ_BANK,	// master object bank
	SUPERRETRO_TEX_BANK,	// master texture bank
	WORLD_CLOSED,
	5,						// number of levels
		{ 
		"super 1",	LEVELID_SUPERRETRO1,RETROLEV1_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"super 2",		LEVELID_SUPERRETRO2,RETROLEV2_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"super 3",		LEVELID_SUPERRETRO3,RETROLEV3_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"super boss",			LEVELID_SUPERRETRO4,RETROLEV4_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"super bonus",	LEVELID_SUPERRETRO5,RETROBONUS_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"super multi",	LEVELID_SUPERRETRO6,RETROMULTI_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
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
	
	InitPlatformsForLevel(worldID,levelID);
	InitEnemiesForLevel(worldID,levelID);
	InitCollectablesForLevel(worldID,levelID);
	InitCameraForLevel(worldID,levelID);
	//InitCameosForLevel ( worldID, levelID );
		
	CreateFrogger(1,1,1,1); // This also creates the babies

	InitEventsForLevel(worldID, levelID);

	LoadTextureBank ( SYSTEM_TEX_BANK );
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

	testPause		= 0;
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
	FreeActorList();
	FreeAnimationList();
	FreeTextureList();
	FreeBackdrop(myBackdrop);

	FreeLevel();
	
	FreeAllObjectBanks();
	FreeAllTextureBanks();
	FreeCameoList();

	myBackdrop	= NULL;

	testPause		= 0;
	darkenedLevel	= 0;
}
