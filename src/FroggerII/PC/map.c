
// PC-ONLY FILE

/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: map.c
	Programmer	: Matthew Cloy
	Date		: 04/12/98

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>
#include "incs.h"
#include "memload.h"

HINSTANCE mapHandle = NULL;
HINSTANCE sceHandle = NULL;
char *aLevBank = NULL;

unsigned long numSafe = 1;
unsigned long numPwrups = 3;
unsigned long numTiles;

GAMETILE *firstTile;

GAMETILE **bTSafe;
GAMETILE **pwrupTStart;

SCENIC *Sc_000;

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: LoadMapBank

	Purpose		:
	Parameters	: (int num)
	Returns		: void 
*/
BOOL LoadCollision (int num);
BOOL LoadScenics (int num);

BOOL LoadMapBank(int num)
{
	return (LoadCollision(num) && LoadScenics(num));
}

void FreeMapBank(void)
{
	if (mapHandle)
		FreeLibrary(mapHandle);
	if (sceHandle)
		FreeLibrary(sceHandle);

	cameraBoxes.numBoxes = 0;
}

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: LoadCollision 

	Purpose		:
	Parameters	: (int num)
	Returns		: void 
*/

typedef long (FAR *MYFPROC)();

BOOL LoadCollision(int num)
{
	char message[256];
	char    file[MAX_PATH]; 
	GAMETILE *tile;

	mapHandle = NULL;
    strcpy (file,baseDirectory);	
    strcat (file,COLLISION_BASE);

	switch(num)
	{
		case GARDENMASTERA_COL:
			strcat(file,"gardenbossa");
			break;
		case GARDENMASTERB_COL:
			strcat(file,"gardenbossb");
			break;
		case GARDENLEV1_COL:
			strcat(file,"garden1");
			break;
		case GARDENLEV2_COL:
			strcat(file,"garden2");
			break;
		case GARDENLEV3_COL:
			strcat(file,"garden3");
			break;
		case GARDENBONUS_COL:
			strcat(file,"gardenbonus");
			break;
		case GARDENMULTI_COL:
			strcat(file,"gardenmulti");
			break;

		case ANCIENTMASTER_COL:
			strcat(file,"ancientsboss");
			break;
		case ANCIENTLEV1_COL:
			strcat(file,"ancients1");
			break;
		case ANCIENTLEV2_COL:
			strcat(file,"ancients2");
			break;
		case ANCIENTLEV3_COL:
			strcat(file,"ancients3");
			break;
		case ANCIENTBONUS_COL:
			strcat(file,"ancientsbonus");
			break;
		case ANCIENTMULTI_COL:	
			strcat(file,"ancientsmulti");
			break;

		case SPACEMASTER_COL:
			strcat(file,"spaceboss");
			break;
		case SPACELEV1_COL:
			strcat(file,"space1");
			break;
		case SPACELEV2_COL:
			strcat(file,"space2");
			break;
		case SPACELEV3_COL:
			strcat(file,"space3");
			break;
		case SPACEBONUS_COL:
			strcat(file,"spacebonus");
			break;
		case SPACEMULTI_COL:
			strcat(file,"spacemulti");
			break;

		case CITYMASTER_COL:
			strcat(file,"cityboss");
			break;
		case CITYLEV1_COL:
			strcat(file,"city1");
			break;
		case CITYLEV2_COL:
			strcat(file,"city2");
			break;
		case CITYLEV3_COL:
			strcat(file,"city3");
			break;
		case CITYBONUS_COL:
			strcat(file,"citybonus");
			break;
		case CITYMULTI_COL:
			strcat(file,"citymulti");
			break;

		case SUBTERRANEANMASTER_COL:
			strcat(file,"subboss");
			break;
		case SUBTERRANEANMASTERB_COL:
			strcat(file,"subtest");		// <--- interesting, new, fragrant
			break;
		case SUBTERRANEANLEV1_COL:
			strcat(file,"sub1");
			break;
		case SUBTERRANEANLEV2_COL:
			strcat(file,"sub2");
			break;
		case SUBTERRANEANLEV3_COL:
			strcat(file,"sub3");
			break;
		case SUBTERRANEANBONUS_COL:
			strcat(file,"subbonus");
			break;
		case SUBTERRANEANMULTI_COL:
			strcat(file,"submulti");
			break;

		case LABORATORYMASTERA_COL:
			strcat(file,"labbossa");
			break;
		case LABORATORYMASTERB_COL:
			strcat(file,"labbossb");
			break;
		case LABORATORYMASTERC_COL:
			strcat(file,"labbossc");
			break;
		case LABORATORYLEV1_COL:
			strcat(file,"lab1");
			break;
		case LABORATORYLEV2_COL:
			strcat(file,"lab2");
			break;
		case LABORATORYLEV3_COL:
			strcat(file,"lab3");
			break;
		case LABORATORYBONUS_COL:
			strcat(file,"labbonus");
			break;
		case LABORATORYMULTI_COL:
			strcat(file,"labmulti");
			break;

		case TOYSHOPMASTER_COL:
			strcat(file,"toyboss");
			break;
		case TOYSHOPLEV1_COL:
			strcat(file,"toy1");
			break;
		case TOYSHOPLEV2_COL:
			strcat(file,"toy2");
			break;
		case TOYSHOPLEV3_COL:
			strcat(file,"toy3");
			break;
		case TOYSHOPBONUS_COL:
			strcat(file,"toybonus");
			break;
		case TOYSHOPMULTI_COL:
			strcat(file,"toymulti");
			break;

		case HALLOWEENMASTER_COL:
			strcat(file,"halloweenboss");
			break;
		case HALLOWEENMASTERB_COL:
			strcat(file,"halloweenbossb");
			break;
		case HALLOWEENLEV1_COL:
			strcat(file,"halloween1");
			break;
		case HALLOWEENLEV2_COL:
			strcat(file,"halloween2");
			break;
		case HALLOWEENLEV3_COL:
			strcat(file,"halloween3");
			break;
		case HALLOWEENBONUS_COL:
			strcat(file,"halloweenbonus");
			break;
		case HALLOWEENMULTI_COL:
			strcat(file,"halloweenmulti");
			break;

		case RETROLEV1_COL:
			strcat(file,"sretro1");
			break;
		case RETROLEV2_COL:
			strcat(file,"sretro2");
			break;
		case RETROLEV3_COL:
			strcat(file,"sretro3");
			break;
		case RETROLEV4_COL:
			strcat(file,"sretro4");
			break;
		case RETROLEV5_COL:
			strcat(file,"sretro5");
			break;
		case RETROLEV6_COL:
			strcat(file,"sretro6");
			break;
		case RETROLEV7_COL:
			strcat(file,"sretro7");
			break;
		case RETROLEV8_COL:
			strcat(file,"sretro8");
			break;
		case RETROLEV9_COL:
			strcat(file,"sretro9");
			break;
		case RETROLEV10_COL:
			strcat(file,"sretro10");
			break;
		case RETROMASTER_COL:
			strcat(file,"sretroboss");
			break;
		
		case FRONTEND1_COL:
			strcat(file,"start");
			break;
		case FRONTEND2_COL:
			strcat(file,"levela");
			break;
		case FRONTEND3_COL:
			strcat(file,"levelb");
			break;
		case FRONTEND4_COL:
			strcat(file,"multisel");
			break;
		case FRONTEND5_COL:
			strcat(file,"language");
			break;

		default:
			dprintf"ERROR: %d is not a valid map ID\n", num));
			return FALSE;
	}

	dprintf"Loading map %s\n",file));
	strcat (file,".dll");
    mapHandle = LoadLibrary(file);
	
	if (mapHandle != NULL)
	{
		FARPROC GetStartsAddress;
		FARPROC GetBabiesAddress;
		//FARPROC GetSafesAddress;
		//FARPROC GetPowerupsAddress;
		FARPROC GetBabiesNum;
		//FARPROC GetSafesNum;
		//FARPROC GetPowerupsNum;
		FARPROC GetFirst; 
		FARPROC GetCameras;

		GetStartsAddress	= GetProcAddress(mapHandle, "GetStartsAddress");
		GetBabiesAddress	= GetProcAddress(mapHandle, "GetBabiesAddress");
		//GetSafesAddress    = GetProcAddress(mapHandle, "GetSafesAddress");
		//GetPowerupsAddress = GetProcAddress(mapHandle, "GetPowerupsAddress");
		GetBabiesNum		= GetProcAddress(mapHandle, "GetBabiesNum");
		//GetSafesNum        = GetProcAddress(mapHandle, "GetSafesNum");
		//GetPowerupsNum     = GetProcAddress(mapHandle, "GetPowerupsNum");
		GetFirst			= GetProcAddress(mapHandle, "GetFirst");
		GetCameras			= GetProcAddress(mapHandle, "GetCameras");

		if (GetStartsAddress)  
			gTStart = (GAMETILE**)GetStartsAddress();
		
	
		if (GetBabiesAddress)   
			bTStart = (GAMETILE**)GetBabiesAddress();   
/*		
		if (GetSafesAddress)
			bTSafe = (GAMETILE**)GetSafesAddress();

		if (GetPowerupsAddress) 
			pwrupTStart = (GAMETILE**)GetPowerupsAddress(); 
*/		
		
		if (GetBabiesNum)       
			numBabies = GetBabiesNum();       
/*		
		if (GetSafesNum)    
			numSafe = GetSafesNum();        
		
		if (GetPowerupsNum)
			numPwrups = GetPowerupsNum();				     
*/
		if (GetFirst)
			firstTile = (GAMETILE *)GetFirst();

		for( tile = firstTile, numTiles=0; tile; tile=tile->next )
			numTiles++;

		if (GetCameras)
			cameraBoxes = *(CAM_BOX_LIST*)GetCameras();
		else
			cameraBoxes.numBoxes = 0;
	}
	else
	{
		dprintf"Couldn't Load Library: %s\n", file));
		return FALSE;
	}
	return TRUE;
}


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: LoadScenics 

	Purpose		:
	Parameters	: (int num)
	Returns		: void 
*/

BOOL LoadScenics (int num)
{
	char message[256];
	char    file[MAX_PATH]; 

	sceHandle = NULL;
    strcpy (file,baseDirectory);	
    strcat (file,SCENIC_BASE);

	switch(num)
	{
		case GARDENMASTERA_COL:
			strcat(file,"gardenbossa");
			break;
		case GARDENMASTERB_COL:
			strcat(file,"gardenbossb");
			break;
		case GARDENLEV1_COL:
			strcat(file,"garden1");
			break;
		case GARDENLEV2_COL:
			strcat(file,"garden2");
			break;
		case GARDENLEV3_COL:
			strcat(file,"garden3");
			break;
		case GARDENBONUS_COL:
			strcat(file,"gardenbonus");
			break;
		case GARDENMULTI_COL:
			strcat(file,"gardenmulti");
			break;

		case ANCIENTMASTER_COL:
			strcat(file,"ancientsboss");
			break;
		case ANCIENTLEV1_COL:
			strcat(file,"ancients1");
			break;
		case ANCIENTLEV2_COL:
			strcat(file,"ancients2");
			break;
		case ANCIENTLEV3_COL:
			strcat(file,"ancients3");
			break;
		case ANCIENTBONUS_COL:
			strcat(file,"ancientsbonus");
			break;
		case ANCIENTMULTI_COL:	
			strcat(file,"ancientsmulti");
			break;

		case SPACEMASTER_COL:
			strcat(file,"spaceboss");
			break;
		case SPACELEV1_COL:
			strcat(file,"space1");
			break;
		case SPACELEV2_COL:
			strcat(file,"space2");
			break;
		case SPACELEV3_COL:
			strcat(file,"space3");
			break;
		case SPACEBONUS_COL:
			strcat(file,"spacebonus");
			break;
		case SPACEMULTI_COL:
			strcat(file,"spacemulti");
			break;

		case CITYMASTER_COL:
			strcat(file,"cityboss");
			break;
		case CITYLEV1_COL:
			strcat(file,"city1");
			break;
		case CITYLEV2_COL:
			strcat(file,"city2");
			break;
		case CITYLEV3_COL:
			strcat(file,"city3");
			break;
		case CITYBONUS_COL:
			strcat(file,"citybonus");
			break;
		case CITYMULTI_COL:
			strcat(file,"citymulti");
			break;

		case SUBTERRANEANMASTER_COL:
			strcat(file,"subboss");
			break;
		case SUBTERRANEANMASTERB_COL:
			strcat(file,"subtest");	//   <-- NOTE! Different, interesting.
			break;
		case SUBTERRANEANLEV1_COL:
			strcat(file,"sub1");
			break;
		case SUBTERRANEANLEV2_COL:
			strcat(file,"sub2");
			break;
		case SUBTERRANEANLEV3_COL:
			strcat(file,"sub3");
			break;
		case SUBTERRANEANBONUS_COL:
			strcat(file,"subbonus");
			break;
		case SUBTERRANEANMULTI_COL:
			strcat(file,"submulti");
			break;

		case LABORATORYMASTERA_COL:
			strcat(file,"labbossa");
			break;
		case LABORATORYMASTERB_COL:
			strcat(file,"labbossb");
			break;
		case LABORATORYMASTERC_COL:
			strcat(file,"labbossc");
			break;
		case LABORATORYLEV1_COL:
			strcat(file,"lab1");
			break;
		case LABORATORYLEV2_COL:
			strcat(file,"lab2");
			break;
		case LABORATORYLEV3_COL:
			strcat(file,"lab3");
			break;
		case LABORATORYBONUS_COL:
			strcat(file,"labbonus");
			break;
		case LABORATORYMULTI_COL:
			strcat(file,"labmulti");
			break;

		case TOYSHOPMASTER_COL:
			strcat(file,"toyboss");
			break;
		case TOYSHOPLEV1_COL:
			strcat(file,"toy1");
			break;
		case TOYSHOPLEV2_COL:
			strcat(file,"toy2");
			break;
		case TOYSHOPLEV3_COL:
			strcat(file,"toy3");
			break;
		case TOYSHOPBONUS_COL:
			strcat(file,"toybonus");
			break;
		case TOYSHOPMULTI_COL:
			strcat(file,"toymulti");
			break;

		case HALLOWEENMASTER_COL:
			strcat(file,"halloweenboss");
			break;
		case HALLOWEENMASTERB_COL:
			strcat(file,"halloweenbossb");
			break;
		case HALLOWEENLEV1_COL:
			strcat(file,"halloween1");
			break;
		case HALLOWEENLEV2_COL:
			strcat(file,"halloween2");
			break;
		case HALLOWEENLEV3_COL:
			strcat(file,"halloween3");
			break;
		case HALLOWEENBONUS_COL:
			strcat(file,"halloweenbonus");
			break;
		case HALLOWEENMULTI_COL:
			strcat(file,"halloweenmulti");
			break;

		case RETROLEV1_COL:
			strcat(file,"sretro1");
			break;
		case RETROLEV2_COL:
			strcat(file,"sretro2");
			break;
		case RETROLEV3_COL:
			strcat(file,"sretro3");
			break;
		case RETROLEV4_COL:
			strcat(file,"sretro4");
			break;
		case RETROLEV5_COL:
			strcat(file,"sretro5");
			break;
		case RETROLEV6_COL:
			strcat(file,"sretro6");
			break;
		case RETROLEV7_COL:
			strcat(file,"sretro7");
			break;
		case RETROLEV8_COL:
			strcat(file,"sretro8");
			break;
		case RETROLEV9_COL:
			strcat(file,"sretro9");
			break;
		case RETROLEV10_COL:
			strcat(file,"sretro10");
			break;
		case RETROMASTER_COL:
			strcat(file,"sretroboss");
			break;
		
		case FRONTEND1_COL:
			strcat(file,"start");
			break;
		case FRONTEND2_COL:
			strcat(file,"levela");
			break;
		case FRONTEND3_COL:
			strcat(file,"levelb");
 			break;
		case FRONTEND4_COL:
			strcat(file,"multisel");
			break;
		case FRONTEND5_COL:
			strcat(file,"language");
			break;

		default:
			dprintf"ERROR: %d is not a valid map bank\n", num));
			return FALSE;
	}

	dprintf"Loading map %s\n",file));
	strcat (file,".dll");
    sceHandle = LoadLibrary(file);
	
	if (sceHandle != NULL)
	{
		FARPROC GetSc000Address;
		
		GetSc000Address   = GetProcAddress(sceHandle, "GetSC000Address");
		
		if (GetSc000Address)
			Sc_000 = (SCENIC*)GetSc000Address();
		else
			Sc_000 = NULL;
	}
	else
		return FALSE;

	return TRUE;
}


/*	--------------------------------------------------------------------------------
	Function		: TeleportActorToTile
	Purpose			: teleports the actor to the specified tile
	Parameters		: ACTOR2 *,GAMETILE *
	Returns			: void
	Info			: 
*/

void TeleportActorToTile(ACTOR2 *act,GAMETILE *tile,long pl)
{
	// make the teleport 'to' tile the current tile
	currTile[pl] = tile;
	SetVector(&act->actor->pos,&tile->centre);
//	player[pl].frogState = 0;
	player[pl].frogState &= ~FROGSTATUS_ISTELEPORTING;
	player[pl].canJump = 1;

	Orientate( &act->actor->qRot, &tile->dirVector[frogFacing[pl]], &tile->normal );
}


/* --------------------------------------------------------------------------------
	Programmer	: David Swift
	Function	: LoadLevelEntities 
	Parameters	: int, int
	Returns		: void 
*/

void LoadLevelEntities(int worldID, int levelID)
{
	char file[MAX_PATH];
	HANDLE h;
	long size, read;
	void* buffer;

	sprintf(file, "%s%sentity-%d-%d.dat", baseDirectory, ENTITY_BASE, worldID, levelID);

	h = CreateFile(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (h == INVALID_HANDLE_VALUE)
	{
		dprintf"Couldn't load entity file %s\n", file)); return;
	}

	size = GetFileSize(h, NULL);
	buffer = JallocAlloc(size, NO, "entLoad");

	ReadFile(h, buffer, size, &read, NULL);
	CloseHandle(h);

	dprintf"LoadLevelEntities: Loaded %d of %d bytes\n", (int)read, (int)size));

	if (!MemLoadEntities(buffer, size))
	{
		dprintf"MemLoadEntities failed\n"));

	#ifdef LOADWITHEDITOR
		dprintf"Using Editor loader instead...\n", file));
		
		FreeCreateList();

		if (LoadCreateList(file))
			EditorCreateEntities();

		FreeCreateList();
	#endif
	}
	
	JallocFree((UBYTE**)&buffer);
}

void LoadLevelScript(int worldID, int levelID)
{
	char file[MAX_PATH];
	HANDLE h;
	long size, read;
	void* buffer;

	sprintf(file, "%s%sscript-%d-%d.fev", baseDirectory, ENTITY_BASE, worldID, levelID);

	h = CreateFile(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (h == INVALID_HANDLE_VALUE)
	{
		dprintf"Couldn't load script %s\n", file)); return;
	}

	size = GetFileSize(h, NULL);
	buffer = JallocAlloc(size, NO, "entLoad");

	ReadFile(h, buffer, size, &read, NULL);
	CloseHandle(h);

	dprintf"LoadLevelScript: Loaded %d of %d bytes\n", (int)read, (int)size));

	InitLevelScript(buffer);
}