/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: layoutvisual.c
	Programmer	: Andrew Eder
	Date		: 01/03/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#define N64_PLAY_SONG

#include <ultra64.h>

#include "incs.h"


unsigned long worldNum;
unsigned long levelNum;

// world visual data - for texture and object banks
WORLD_VISUAL_DATA worldVisualData[MAX_WORLDS];

UBYTE darkenedLevel		= 0;
short lightIntensity	= 255;

//////////////////////////////////////////////////////////////////////////////////////////////////
//************************************************************************************************
// PC VERSION ONLY - PC VERSION ONLY - PC VERSION ONLY - PC VERSION ONLY - PC VERSION ONLY
//************************************************************************************************
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef PC_VERSION

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
		"the spawn lawn",		LEVELID_GARDENLAWN,GARDENLEV1_OBJ_BANK,0,GARDENLEV1_COL,LEVEL_OPEN,WORLDID_ANCIENT,0,
		"a-maze-ing garden",	LEVELID_GARDENMAZE,GARDENLEV2_OBJ_BANK,0,GARDENLEV2_COL,LEVEL_OPEN,WORLDID_SPACE,0,
		"vegging out",			LEVELID_GARDENVEGPATCH,GARDENLEV3_OBJ_BANK,0,GARDENLEV3_COL,LEVEL_OPEN,WORLDID_CITY,0,
		"garden boss",			LEVELID_GARDEN4,GARDENBOSSA_OBJ_BANK,0,GARDENMASTERA_COL,LEVEL_OPEN,WORLDID_SUBTERRANEAN,0,
		"garden bonus",			LEVELID_GARDEN5,GARDENBONUS_OBJ_BANK,0,GARDENBONUS_COL,LEVEL_CLOSED,-1,0,
		"garden multi",			LEVELID_GARDEN6,GARDENMULTI_OBJ_BANK,0,GARDENMULTI_COL,LEVEL_CLOSED,-1,0,
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
		"ancient 1",	LEVELID_ANCIENT1,ANCIENTLEV1_OBJ_BANK,0,ANCIENTLEV1_COL,LEVEL_OPEN,-1,0,
		"ancient 2",	LEVELID_ANCIENT2,ANCIENTLEV2_OBJ_BANK,0,ANCIENTLEV2_COL,LEVEL_OPEN,-1,0,
		"ancient 3",	LEVELID_ANCIENT3,ANCIENTLEV3_OBJ_BANK,0,ANCIENTLEV3_COL,LEVEL_OPEN,-1,0,
		"ancient boss",	LEVELID_ANCIENT4,ANCIENTBOSS_OBJ_BANK,0,ANCIENTMASTER_COL,LEVEL_CLOSED,-1,0,
		"ancient bonus",LEVELID_ANCIENT5,ANCIENTBONUS_OBJ_BANK,0,ANCIENTBONUS_COL,LEVEL_CLOSED,-1,0,
		"ancient multi",LEVELID_ANCIENT6,ANCIENTMULTI_OBJ_BANK,0,ANCIENTMULTI_COL,LEVEL_CLOSED,-1,0,
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
		"space 1",		LEVELID_SPACE1,SPACELEV1_OBJ_BANK,0,SPACELEV1_COL,LEVEL_CLOSED,-1,0,
		"space 2",		LEVELID_SPACE2,SPACELEV2_OBJ_BANK,0,SPACELEV2_COL,LEVEL_CLOSED,-1,0,
		"space 3",		LEVELID_SPACE3,SPACELEV3_OBJ_BANK,0,SPACELEV3_COL,LEVEL_CLOSED,-1,0,
		"space boss",	LEVELID_SPACE4,SPACEBOSS_OBJ_BANK,0,SPACEMASTER_COL,LEVEL_CLOSED,-1,0,
		"space bonus",	LEVELID_SPACE5,SPACEBONUS_OBJ_BANK,0,SPACEBONUS_COL,LEVEL_CLOSED,-1,0,
		"space multi",	LEVELID_SPACE6,SPACEMULTI_OBJ_BANK,0,SPACEMULTI_COL,LEVEL_CLOSED,-1,0,
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
		"down the docks",		LEVELID_CITYDOCKS,CITYLEV1_OBJ_BANK,0,CITYLEV1_COL,LEVEL_OPEN,WORLDID_TOYSHOP,0,
		"warehouse wandering",	LEVELID_CITYWAREHOUSE,CITYLEV2_OBJ_BANK,0,CITYLEV2_COL,LEVEL_CLOSED,-1,0,
		"on the streets",		LEVELID_CITYSTREETS,CITYLEV3_OBJ_BANK,0,CITYLEV3_COL,LEVEL_CLOSED,-1,0,
		"city tower",			LEVELID_CITYTOWER,CITYBOSS_OBJ_BANK,0,CITYMASTER_COL,LEVEL_CLOSED,-1,0,
		"city bonus",			LEVELID_CITYBONUS,CITYBONUS_OBJ_BANK,0,CITYBONUS_COL,LEVEL_CLOSED,-1,0,
		"city multi",			LEVELID_CITYMULTI,CITYMULTI_OBJ_BANK,0,CITYMULTI_COL,LEVEL_CLOSED,-1,0,
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
		"sub boss",		LEVELID_SUBTERRANEAN4,SUBTERRANEANBOSS_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
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
		"lab 1",		LEVELID_LABORATORY1,LABORATORYLEV1_OBJ_BANK,0,LABORATORYLEV1_COL,LEVEL_OPEN,-1,0,
		"lab 2",		LEVELID_LABORATORY2,LABORATORYLEV2_OBJ_BANK,0,LABORATORYLEV2_COL,LEVEL_CLOSED,-1,0,
		"lab 3",		LEVELID_LABORATORY3,LABORATORYLEV3_OBJ_BANK,0,LABORATORYLEV3_COL,LEVEL_CLOSED,-1,0,
		"lab boss a",	LEVELID_LABORATORY4,LABORATORYBOSSA_OBJ_BANK,0,LABORATORYMASTERA_COL,LEVEL_CLOSED,-1,0,
		"lab boss b",	LEVELID_LABORATORY5,LABORATORYBOSSB_OBJ_BANK,0,LABORATORYMASTERB_COL,LEVEL_CLOSED,-1,0,
		"lab boss c",	LEVELID_LABORATORY6,LABORATORYBOSSC_OBJ_BANK,0,LABORATORYMASTERC_COL,LEVEL_CLOSED,-1,0,
		"lab bonus",	LEVELID_LABORATORY7,LABORATORYBONUS_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"lab multi",	LEVELID_LABORATORY8,LABORATORYMULTI_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
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
		"toy shop 1",		LEVELID_TOYSHOP1,TOYSHOPLEV1_OBJ_BANK,0,TOYSHOPLEV1_COL,LEVEL_OPEN,WORLDID_TOYSHOP,0,
		"toy shop 2",		LEVELID_TOYSHOP2,TOYSHOPLEV2_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"toy shop 3",		LEVELID_TOYSHOP3,TOYSHOPLEV3_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"toy shop boss",	LEVELID_TOYSHOP4,TOYSHOPBOSS_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
		"toy shop bonus",	LEVELID_TOYSHOP5,TOYSHOPBONUS_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
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
		"halloween 1",		LEVELID_HALLOWEEN1,	HALLOWEENLEV1_OBJ_BANK,0,HALLOWEENLEV1_COL,LEVEL_OPEN,-1,0,
		"halloween 2",		LEVELID_HALLOWEEN2,	HALLOWEENLEV2_OBJ_BANK,0,HALLOWEENLEV2_COL,LEVEL_OPEN,-1,0,
		"halloween 3",		LEVELID_HALLOWEEN3,	HALLOWEENLEV3_OBJ_BANK,0,HALLOWEENLEV3_COL,LEVEL_OPEN,-1,0,
		"halloween boss",	LEVELID_HALLOWEEN4,	HALLOWEENBOSS_OBJ_BANK,0,HALLOWEENMASTER_COL,LEVEL_OPEN,-1,0,
		"halloween bonus",	LEVELID_HALLOWEEN5,	HALLOWEENBONUS_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0,
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
		"super 1",		LEVELID_SUPERRETRO1,RETROLEV1_OBJ_BANK,0,RETROLEV1_COL,LEVEL_OPEN,-1,0,
		"super 2",		LEVELID_SUPERRETRO2,RETROLEV2_OBJ_BANK,0,RETROLEV2_COL,LEVEL_OPEN,-1,0,
		"super 3",		LEVELID_SUPERRETRO3,RETROLEV3_OBJ_BANK,0,RETROLEV3_COL,LEVEL_OPEN,-1,0,
		"super boss",	LEVELID_SUPERRETRO4,RETROBOSS_OBJ_BANK,0,RETROMASTER_COL,LEVEL_CLOSED,-1,0,
		"super bonus",	LEVELID_SUPERRETRO5,RETROBONUS_OBJ_BANK,0,RETROBONUS_COL,LEVEL_CLOSED,-1,0,
		"super multi",	LEVELID_SUPERRETRO6,RETROMULTI_OBJ_BANK,0,RETROMULTI_COL,LEVEL_CLOSED,-1,0,
		},
	},

//------------------------------------------------

	{
	"frontend",			// description
	WORLDID_FRONTEND,		// world ID
	FRONTEND_OBJ_BANK,	// master object bank
	FRONTEND_TEX_BANK,	// master texture bank
	WORLD_OPEN,
	4,						// number of levels
		{ 
		"start",		LEVELID_FRONTEND1,FRONTEND1_OBJ_BANK,0,FRONTEND1_COL,LEVEL_OPEN,-1,0,
		"levela",		LEVELID_FRONTEND2,FRONTEND2_OBJ_BANK,0,FRONTEND2_COL,LEVEL_OPEN,-1,0,
		"levelb",		LEVELID_FRONTEND3,FRONTEND3_OBJ_BANK,0,FRONTEND3_COL,LEVEL_OPEN,-1,0,
		"multisel",		LEVELID_FRONTEND4,FRONTEND4_OBJ_BANK,0,FRONTEND4_COL,LEVEL_OPEN,-1,0,
		"language",		LEVELID_FRONTEND5,FRONTEND5_OBJ_BANK,0,FRONTEND5_COL,LEVEL_OPEN,-1,0,
		},
	}
};





//////////////////////////////////////////////////////////////////////////////////////////////////
//************************************************************************************************
// N64 VERSION ONLY - N64 VERSION ONLY - N64 VERSION ONLY - N64 VERSION ONLY - N64 VERSION ONLY
//************************************************************************************************
//////////////////////////////////////////////////////////////////////////////////////////////////
#else

WORLD_VISUAL_DATA worldVisualData[MAX_WORLDS] = 
{
//------------------------------------------------ GARDEN
	{
	"garden",					// description
	WORLDID_GARDEN,				// world ID
	GENERIC_GARDEN_OBJ_BANK,		// master object bank
	GENERIC_GARDEN_TEX_BANK,	// master texture bank
	WORLD_OPEN,
	7,							// number of levels
		{ 
		"the spawn lawn",		LEVELID_GARDENLEV1,LEVEL1_GARDEN_OBJ_BANK,LEVEL1_GARDEN_TEX_BANK,GARDENLEV1_COL,LEVEL_OPEN,WORLDID_ANCIENT,0,
		"a-maze-ing garden",	LEVELID_GARDENLEV2,LEVEL2_GARDEN_OBJ_BANK,LEVEL2_GARDEN_TEX_BANK,GARDENLEV2_COL,LEVEL_OPEN,WORLDID_SPACE,0,
		"vegging out",			LEVELID_GARDENLEV3,LEVEL3_GARDEN_OBJ_BANK,LEVEL3_GARDEN_TEX_BANK,GARDENLEV3_COL,LEVEL_OPEN,WORLDID_CITY,0,
		"garden boss",			LEVELID_GARDENBOSSA,BOSSA_GARDEN_OBJ_BANK,BOSSA_GARDEN_TEX_BANK,GARDENMASTERA_COL,LEVEL_OPEN,WORLDID_SUBTERRANEAN,0,
		"tree tops b", 			LEVELID_GARDENBOSSB,BOSSB_GARDEN_OBJ_BANK,BOSSB_GARDEN_TEX_BANK,GARDENMASTERB_COL,LEVEL_OPEN,0,0,
		"garden bonus",			LEVELID_GARDENBONUS,BONUS_GARDEN_OBJ_BANK,BONUS_GARDEN_TEX_BANK,GARDENBONUS_COL,LEVEL_OPEN,-1,0,
		"garden multi",			LEVELID_GARDENMULTI,MULTI_GARDEN_OBJ_BANK,MULTI_GARDEN_TEX_BANK,GARDENMULTI_COL,LEVEL_OPEN,-1,0,
		},
	},

//------------------------------------------------ ANCIENT

	{
	"ancient",			// description
	WORLDID_ANCIENT,		// world ID
	GENERIC_ANCIENT_OBJ_BANK,	// master object bank
	GENERIC_ANCIENT_TEX_BANK,	// master texture bank
	WORLD_CLOSED,
	9,						// number of levels
		{ 
		"ancient 1",		LEVELID_ANCIENTLEV1,LEVEL1_ANCIENT_OBJ_BANK,LEVEL1_ANCIENT_TEX_BANK,ANCIENTLEV1_COL,LEVEL_OPEN,-1,0,
		"ancient 2",		LEVELID_ANCIENTLEV2,LEVEL2_ANCIENT_OBJ_BANK,LEVEL2_ANCIENT_TEX_BANK,ANCIENTLEV2_COL,LEVEL_OPEN,-1,0,
		"ancient 3",		LEVELID_ANCIENTLEV3,LEVEL3_ANCIENT_OBJ_BANK,LEVEL3_ANCIENT_TEX_BANK,ANCIENTLEV3_COL,LEVEL_OPEN,-1,0,
		"ancient boss",		LEVELID_ANCIENTBOSSA,BOSSA_ANCIENT_OBJ_BANK,BOSSA_ANCIENT_TEX_BANK,ANCIENTMASTERA_COL,LEVEL_OPEN,-1,0,
		"ancient boss a",	LEVELID_ANCIENTBOSSB,BOSSA_ANCIENT_OBJ_BANK,BOSSA_ANCIENT_TEX_BANK,ANCIENTMASTERB_COL,LEVEL_OPEN,-1,0,
		"ancient boss b",	LEVELID_ANCIENTBOSSC,BOSSA_ANCIENT_OBJ_BANK,BOSSA_ANCIENT_TEX_BANK,ANCIENTMASTERC_COL,LEVEL_OPEN,-1,0,
		"ancient boss c",	LEVELID_ANCIENTBOSSD,BOSSA_ANCIENT_OBJ_BANK,BOSSA_ANCIENT_TEX_BANK,ANCIENTMASTERD_COL,LEVEL_OPEN,-1,0,
		"ancient bonus",	LEVELID_ANCIENTBONUS,BONUS_ANCIENT_OBJ_BANK,BONUS_ANCIENT_TEX_BANK,ANCIENTBONUS_COL,LEVEL_OPEN,-1,0,
		"ancient multi",	LEVELID_ANCIENTMULTI,MULTI_ANCIENT_OBJ_BANK,MULTI_ANCIENT_TEX_BANK,ANCIENTMULTI_COL,LEVEL_OPEN,-1,0,
		},
	},


//------------------------------------------------ SPACE

	{
	"space",			// description
	WORLDID_SPACE,		// world ID
	GENERIC_SPACE_OBJ_BANK,		// master object bank
	GENERIC_SPACE_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	6,						// number of levels
		{ 
		"space 1",		LEVELID_SPACELEV1,LEVEL1_SPACE_OBJ_BANK,LEVEL1_SPACE_TEX_BANK,SPACELEV1_COL,LEVEL_OPEN,-1,0,
		"space 2",		LEVELID_SPACELEV2,LEVEL2_SPACE_OBJ_BANK,LEVEL2_SPACE_TEX_BANK,SPACELEV2_COL,LEVEL_OPEN,-1,0,
		"space 3",		LEVELID_SPACELEV3,LEVEL3_SPACE_OBJ_BANK,LEVEL3_SPACE_TEX_BANK,SPACELEV3_COL,LEVEL_OPEN,-1,0,
		"space boss",	LEVELID_SPACEBOSSA,BOSSA_SPACE_OBJ_BANK,BOSSA_SPACE_TEX_BANK,SPACEMASTERA_COL,LEVEL_OPEN,-1,0,
		"space bonus",	LEVELID_SPACEBONUS,BONUS_SPACE_OBJ_BANK,BONUS_SPACE_TEX_BANK,SPACEBONUS_COL,LEVEL_OPEN,-1,0,
		"space multi",	LEVELID_SPACEMULTI,MULTI_SPACE_OBJ_BANK,MULTI_SPACE_TEX_BANK,SPACEMULTI_COL,LEVEL_OPEN,-1,0,
		},
	},

//------------------------------------------------ CITY

	{
	"city",				// description
	WORLDID_CITY,		// world ID
	GENERIC_CITY_OBJ_BANK,		// master object bank
	GENERIC_CITY_TEX_BANK,		// master texture bank
	WORLD_OPEN,
	6,						// number of levels
		{ 
		"city 1",		LEVELID_CITYLEV1,LEVEL1_CITY_OBJ_BANK,LEVEL1_CITY_TEX_BANK,CITYLEV1_COL,LEVEL_OPEN,WORLDID_TOYSHOP,0,
		"city 2",		LEVELID_CITYLEV2,LEVEL2_CITY_OBJ_BANK,LEVEL2_CITY_TEX_BANK,CITYLEV2_COL,LEVEL_OPEN,-1,0,
		"city 3",		LEVELID_CITYLEV3,LEVEL3_CITY_OBJ_BANK,LEVEL3_CITY_TEX_BANK,CITYLEV3_COL,LEVEL_OPEN,-1,0,
		"city boss",	LEVELID_CITYBOSSA,BOSSA_CITY_OBJ_BANK,BOSSA_CITY_TEX_BANK,CITYMASTERA_COL,LEVEL_OPEN,-1,0,
		"city bonus",	LEVELID_CITYBONUS,BONUS_CITY_OBJ_BANK,BONUS_CITY_TEX_BANK,CITYBONUS_COL,LEVEL_OPEN,-1,0,
		"city multi",	LEVELID_CITYMULTI,MULTI_CITY_OBJ_BANK,MULTI_CITY_TEX_BANK,CITYMULTI_COL,LEVEL_OPEN,-1,0,
		},
	},

//------------------------------------------------ SUBTERRANEAN

	{
	"subterranean",				// description
	WORLDID_SUBTERRANEAN,		// world ID
	GENERIC_SUBTERRANEAN_OBJ_BANK,		// master object bank
	GENERIC_SUBTERRANEAN_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	6,						// number of levels
		{ 
		"sub 1",		LEVELID_SUBTERRANEANLEV1,LEVEL1_SUBTERRANEAN_OBJ_BANK,LEVEL1_SUBTERRANEAN_TEX_BANK,SUBTERRANEANLEV1_COL,LEVEL_OPEN,-1,0,
		"sub 2",		LEVELID_SUBTERRANEANLEV2,LEVEL2_SUBTERRANEAN_OBJ_BANK,LEVEL2_SUBTERRANEAN_TEX_BANK,SUBTERRANEANLEV2_COL,LEVEL_OPEN,-1,0,
		"sub 3",		LEVELID_SUBTERRANEANLEV3,LEVEL3_SUBTERRANEAN_OBJ_BANK,LEVEL3_SUBTERRANEAN_TEX_BANK,SUBTERRANEANLEV3_COL,LEVEL_OPEN,-1,0,
		"sub boss",		LEVELID_SUBTERRANEANBOSSA,BOSSA_SUBTERRANEAN_OBJ_BANK,BOSSA_SUBTERRANEAN_TEX_BANK,SUBTERRANEANMASTERA_COL,LEVEL_OPEN,-1,0,
		"sub bonus",	LEVELID_SUBTERRANEANBONUS,BONUS_SUBTERRANEAN_OBJ_BANK,BONUS_SUBTERRANEAN_TEX_BANK,SUBTERRANEANBONUS_COL,LEVEL_OPEN,-1,0,
		"sub multi",	LEVELID_SUBTERRANEANMULTI,MULTI_SUBTERRANEAN_OBJ_BANK,MULTI_SUBTERRANEAN_TEX_BANK,SUBTERRANEANMULTI_COL,LEVEL_OPEN,-1,0,
		},
	},

//------------------------------------------------ LABORATORY

	{
	"laboratory",				// description
	WORLDID_LABORATORY,		// world ID
	GENERIC_LABORATORY_OBJ_BANK,		// master object bank
	GENERIC_LABORATORY_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	6,						// number of levels
		{ 
		"lab 1",		LEVELID_LABORATORYLEV1,LEVEL1_LABORATORY_OBJ_BANK,LEVEL1_LABORATORY_TEX_BANK,LABORATORYLEV1_COL,LEVEL_OPEN,-1,0,
		"lab 2",		LEVELID_LABORATORYLEV2,LEVEL2_LABORATORY_OBJ_BANK,LEVEL2_LABORATORY_TEX_BANK,LABORATORYLEV2_COL,LEVEL_OPEN,-1,0,
		"lab 3",		LEVELID_LABORATORYLEV3,LEVEL3_LABORATORY_OBJ_BANK,LEVEL3_LABORATORY_TEX_BANK,LABORATORYLEV3_COL,LEVEL_OPEN,-1,0,
		"lab boss",		LEVELID_LABORATORYBOSSA,BOSSA_LABORATORY_OBJ_BANK,BOSSA_LABORATORY_TEX_BANK,LABORATORYMASTERA_COL,LEVEL_OPEN,-1,0,
		"lab bonus",	LEVELID_LABORATORYBONUS,BONUS_LABORATORY_OBJ_BANK,BONUS_LABORATORY_TEX_BANK,LABORATORYBONUS_COL,LEVEL_OPEN,-1,0,
		"lab multi",	LEVELID_LABORATORYMULTI,MULTI_LABORATORY_OBJ_BANK,MULTI_LABORATORY_TEX_BANK,LABORATORYMULTI_COL,LEVEL_OPEN,-1,0,
		},
	},

//------------------------------------------------ TOYSHOP

	{
	"toy shop",				// description
	WORLDID_TOYSHOP,		// world ID
	GENERIC_TOYSHOP_OBJ_BANK,		// master object bank
	GENERIC_TOYSHOP_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	6,						// number of levels
		{ 
		"toy shop 1",		LEVELID_TOYSHOPLEV1,LEVEL1_TOYSHOP_OBJ_BANK,LEVEL1_TOYSHOP_TEX_BANK,TOYSHOPLEV1_COL,LEVEL_OPEN,-1,0,
		"toy shop 2",		LEVELID_TOYSHOPLEV2,LEVEL2_TOYSHOP_OBJ_BANK,LEVEL2_TOYSHOP_TEX_BANK,TOYSHOPLEV2_COL,LEVEL_OPEN,-1,0,
		"toy shop 3",		LEVELID_TOYSHOPLEV3,LEVEL3_TOYSHOP_OBJ_BANK,LEVEL3_TOYSHOP_TEX_BANK,TOYSHOPLEV3_COL,LEVEL_OPEN,-1,0,
		"toy shop boss",	LEVELID_TOYSHOPBOSSA,BOSSA_TOYSHOP_OBJ_BANK,BOSSA_TOYSHOP_TEX_BANK,TOYSHOPMASTERA_COL,LEVEL_OPEN,-1,0,
		"toy shop bonus",	LEVELID_TOYSHOPBONUS,BONUS_TOYSHOP_OBJ_BANK,BONUS_TOYSHOP_TEX_BANK,TOYSHOPBONUS_COL,LEVEL_OPEN,-1,0,
		"toy shop multi",	LEVELID_TOYSHOPMULTI,MULTI_TOYSHOP_OBJ_BANK,MULTI_TOYSHOP_TEX_BANK,TOYSHOPMULTI_COL,LEVEL_OPEN,-1,0,
		},
	},


//------------------------------------------------ HALLOWEEN

	{
	"halloween",					// description
	WORLDID_HALLOWEEN,			// world ID
	GENERIC_HALLOWEEN_OBJ_BANK,			// master object bank
	GENERIC_HALLOWEEN_TEX_BANK,			// master texture bank
	WORLD_CLOSED,
	6,						// number of levels 
		{ 
		"halloween 1",		LEVELID_HALLOWEENLEV1,	LEVEL1_HALLOWEEN_OBJ_BANK,LEVEL1_HALLOWEEN_TEX_BANK,HALLOWEENLEV1_COL,LEVEL_OPEN,-1,0,
		"halloween 2",		LEVELID_HALLOWEENLEV2,	LEVEL2_HALLOWEEN_OBJ_BANK,LEVEL2_HALLOWEEN_TEX_BANK,HALLOWEENLEV2_COL,LEVEL_OPEN,-1,0,
		"halloween 3",		LEVELID_HALLOWEENLEV3,	LEVEL3_HALLOWEEN_OBJ_BANK,LEVEL3_HALLOWEEN_TEX_BANK,HALLOWEENLEV3_COL,LEVEL_OPEN,-1,0,
		"halloween boss",	LEVELID_HALLOWEENBOSSA,	BOSSA_HALLOWEEN_OBJ_BANK,BOSSA_HALLOWEEN_TEX_BANK,HALLOWEENMASTERA_COL,LEVEL_OPEN,-1,0,
		"halloween bonus",	LEVELID_HALLOWEENBONUS,	BONUS_HALLOWEEN_OBJ_BANK,BONUS_HALLOWEEN_TEX_BANK,HALLOWEENBONUS_COL,LEVEL_OPEN,-1,0,
		"halloween multi",	LEVELID_HALLOWEENMULTI,	MULTI_HALLOWEEN_OBJ_BANK,MULTI_HALLOWEEN_TEX_BANK,HALLOWEENMULTI_COL,LEVEL_OPEN,-1,0,
		},
	},

	

//------------------------------------------------ SUPERRETRO

	{
	"super retro",			// description
	WORLDID_SUPERRETRO,		// world ID
	GENERIC_RETRO_OBJ_BANK,	// master object bank
	GENERIC_RETRO_TEX_BANK,	// master texture bank
	WORLD_CLOSED,
	6,						// number of levels
		{ 
		"super 1",		LEVELID_SUPERRETROLEV1,LEVEL1_RETRO_OBJ_BANK,LEVEL1_RETRO_TEX_BANK,RETROLEV1_COL,LEVEL_OPEN,-1,0,
		"super 2",		LEVELID_SUPERRETROLEV2,LEVEL2_RETRO_OBJ_BANK,LEVEL2_RETRO_TEX_BANK,RETROLEV2_COL,LEVEL_OPEN,-1,0,
		"super 3",		LEVELID_SUPERRETROLEV3,LEVEL3_RETRO_OBJ_BANK,LEVEL3_RETRO_TEX_BANK,RETROLEV3_COL,LEVEL_OPEN,-1,0,
		"super boss",	LEVELID_SUPERRETROBOSSA,BOSSA_RETRO_OBJ_BANK,BOSSA_RETRO_TEX_BANK,RETROMASTERA_COL,LEVEL_OPEN,-1,0,
		"super bonus",	LEVELID_SUPERRETROBONUS,BONUS_RETRO_OBJ_BANK,BONUS_RETRO_TEX_BANK,RETROBONUS_COL,LEVEL_OPEN,-1,0,
		"super multi",	LEVELID_SUPERRETROMULTI,MULTI_RETRO_OBJ_BANK,MULTI_RETRO_TEX_BANK,RETROMULTI_COL,LEVEL_OPEN,-1,0,
		},
	},

//------------------------------------------------

	{
	"frontend",				// description
	WORLDID_FRONTEND,		// world ID
	FRONTEND_OBJ_BANK,		// master object bank
	FRONTEND_TEX_BANK,		// master texture bank
	WORLD_OPEN,
	5,						// number of levels
		{ 
		"start",		LEVELID_FRONTEND1,FRONTEND1_OBJ_BANK,FRONTEND1_TEX_BANK,FRONTEND1_COL,LEVEL_OPEN,-1,0,
		"levela",		LEVELID_FRONTEND2,FRONTEND2_OBJ_BANK,FRONTEND2_TEX_BANK,FRONTEND2_COL,LEVEL_OPEN,-1,0,
		"levelb",		LEVELID_FRONTEND3,FRONTEND3_OBJ_BANK,FRONTEND3_TEX_BANK,FRONTEND3_COL,LEVEL_OPEN,-1,0,
		"multisel",		LEVELID_FRONTEND4,FRONTEND4_OBJ_BANK,FRONTEND4_TEX_BANK,FRONTEND4_COL,LEVEL_OPEN,-1,0,
		"language",		LEVELID_FRONTEND5,FRONTEND5_OBJ_BANK,FRONTEND5_TEX_BANK,FRONTEND5_COL,LEVEL_OPEN,-1,0,
		},
	}
};

#endif


//////////////////////////////////////////////////////////////////////////////////////////////////
//************************************************************************************************
// BOTH VERSIONS (PC AND N64)
//************************************************************************************************
//////////////////////////////////////////////////////////////////////////////////////////////////


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
	LoadTextureBank(INGAMEGENERIC_TEX_BANK);
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

#ifdef PC_VERSION
	void FreeObjectBank(long i);
#endif

void FreeAllObjectBanks()
{
	unsigned long i;

#ifdef PC_VERSION
	
	for(i=0; i<MAX_OBJECT_BANKS; i++)
		FreeObjectBank(i);
#else

	for(i=0; i<MAX_OBJECT_BANKS; i++)
	{
		if(objectBanks[i].freePtr)
			JallocFree((UBYTE**)&objectBanks[i].freePtr);
	}

#endif
}


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: InitLevel
	Purpose		:
	Parameters	: unsigned long , unsigned long
	Returns		: void 
*/
long initialCamera = 0;
void InitLevel(unsigned long worldID,unsigned long levelID)
{
	int i;
	StopDrawing("initlev");

	// prepare screen for fade in
	fadeOut		= 255;
	fadeStep	= 8;
	fadeDir		= FADE_IN;
	doScreenFade = 63;

	// load relevant collison, texture and object banks
	LoadMapBank(worldVisualData[worldID].levelVisualData[levelID].collBank);
	LoadVisualBanksForWorld(worldID,levelID);

	InitSpriteFrameLists();
	InitFXLinkedLists();

	player[0].worldNum = worldID;
	player[0].levelNum = levelID;

	CreateLevelObjects(worldID,levelID);

	InitEnemyLinkedList();
	InitPlatformLinkedList();
	InitGaribLinkedList();
	InitSpriteOverlayLinkedList();
	InitTextOverlayLinkedList();
	InitTriggerList();

	LoadLevelEntities(worldID,levelID);

#ifdef PC_VERSION
	/* Whoever it is, stop fucking well deleting this line */
	LoadLevelScript(worldID, levelID);
#endif

	i=0;
	initialCamera = 1;
	SetVector(&(currCamSource[i]),&outVec);
	SetVector(&(currCamTarget[i]),&inVec);
	//InitCameosForLevel ( worldID, levelID );
		
	CreateFrogger(1,1,1,1); // This also creates the babies

	//InitEventsForLevel(worldID,levelID);

	// prepare the text overlays for the current level
	InitInGameTextOverlays(worldID,levelID);

	StartDrawing("initlev");

#ifndef PC_VERSION
	MusSetMasterVolume(MUSFLAG_SONGS,32000);
#ifdef N64_PLAY_SONG
	PrepareSong(TEST1_TRACK,0);
#endif

#endif
}


/*	--------------------------------------------------------------------------------
	Function		: FreeAllLists
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void FreeAllLists()
{
	dprintf"----- FREEING ALL LISTS -----\n"));

	KillAllTriggers();

	FreeFXSmokeLinkedList();
	FreeFXRippleLinkedList();
	FreeFXExplodeParticleLinkedList();
	FreeFXObjectBlurLinkedList();
	FreeSpriteFrameLists();
	FreeGaribLinkedList();
	FreeTextOverlayLinkedList();
	FreeSpriteOverlayLinkedList();
	FreeSpriteLinkedList();
	FreeEnemyLinkedList();
	FreePlatformLinkedList();

	FreeAnimationList();
	FreeTextureList();
	FreeAllObjectBanks();
	FreeAllTextureBanks();
	FreeCameoList();
	FreeTransCameraList();
#ifndef PC_VERSION
	FreeLevelEntitys();
	FreeGrabData();
#endif
	FreeMapBank();

	FreeActorList();

#ifndef PC_VERSION
	// global ptrs
	aMapBank = NULL;
	globalPtrEntityBank = NULL;
#endif

	dprintf"-----------------------------\n"));

	InitTextOverlayLinkedList();
	InitSpriteOverlayLinkedList();
	InitSpriteLinkedList();

	fog.r = fog.g = fog.b = 0;
	pauseMode		= 0;
	darkenedLevel	= 0;
}
