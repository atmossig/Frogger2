
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
void LoadCollision (int num);
void LoadScenics (int num);

void LoadMapBank(int num)
{
	LoadCollision (num);
	LoadScenics (num);
}

void FreeMapBank(void)
{
	if (mapHandle)
		FreeLibrary(mapHandle);
	if (sceHandle)
		FreeLibrary(sceHandle);
}

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: LoadCollision 

	Purpose		:
	Parameters	: (int num)
	Returns		: void 
*/

typedef long (FAR *MYFPROC)();

void LoadCollision (int num)
{
	char message[256];
	char    file[MAX_PATH]; 

	mapHandle = NULL;
    strcpy (file,baseDirectory);	
    strcat (file,COLLISION_BASE);

	switch(num)
	{
		case GARDENMASTER_COL:
			strcat(file,"gardenmaster");
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
			strcat(file,"ancientsmaster");
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
			strcat(file,"spacemaster");
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
			strcat(file,"citymaster");
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
			strcat(file,"submaster");
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

		case LABORATORYMASTER_COL:
			strcat(file,"labmaster");
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
			strcat(file,"toymaster");
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
			strcat(file,"halloweenmaster");
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

		case RETROMASTER_COL:
			strcat(file,"retromaster");
			break;

		default:
			dprintf"ERROR: no valid map bank specified....\n"));
			for(;;);
			break;
	}

	dprintf"Loading map %s\n",file));
	strcat (file,".dll");
    mapHandle = LoadLibrary(file);
	
	if (mapHandle != NULL)
	{
		FARPROC GetStartsAddress;
		FARPROC GetBabiesAddress;
		FARPROC GetSafesAddress;
		FARPROC GetPowerupsAddress;
		FARPROC GetBabiesNum;
		FARPROC GetSafesNum;
		FARPROC GetPowerupsNum;
		FARPROC GetFirst; 

		GetStartsAddress   = GetProcAddress(mapHandle, "GetStartsAddress");
		GetBabiesAddress   = GetProcAddress(mapHandle, "GetBabiesAddress");
		GetSafesAddress    = GetProcAddress(mapHandle, "GetSafesAddress");
		GetPowerupsAddress = GetProcAddress(mapHandle, "GetPowerupsAddress");
		GetBabiesNum       = GetProcAddress(mapHandle, "GetBabiesNum");
		GetSafesNum        = GetProcAddress(mapHandle, "GetSafesNum");
		GetPowerupsNum     = GetProcAddress(mapHandle, "GetPowerupsNum");
		GetFirst		   = GetProcAddress(mapHandle, "GetFirst");
		
		if (GetStartsAddress)  
			gTStart = (GAMETILE *)GetStartsAddress();
		
		if (GetBabiesAddress)   
			bTStart = (GAMETILE *)GetBabiesAddress();   
		
		if (GetSafesAddress)
			bTSafe = (GAMETILE *)GetSafesAddress();    
		
		if (GetPowerupsAddress) 
			pwrupTStart = GetPowerupsAddress(); 
		
		if (GetBabiesNum)       
			numBabies = GetBabiesNum();       
		
		if (GetSafesNum)    
			numSafe = GetSafesNum();        
		
		if (GetPowerupsNum)
			numPwrups = GetPowerupsNum();				     

		if (GetFirst)
			firstTile = (GAMETILE *)GetFirst();
		
	}
	else
	{
		dprintf"Couldn't Load Library!!!\n"));
		while(1);
	}
//firstTile = cgT;
}


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: LoadScenics 

	Purpose		:
	Parameters	: (int num)
	Returns		: void 
*/

void LoadScenics (int num)
{
	char message[256];
	char    file[MAX_PATH]; 

	sceHandle = NULL;
    strcpy (file,baseDirectory);	
    strcat (file,SCENIC_BASE);

	switch(num)
	{
case GARDENMASTER_COL:
			strcat(file,"gardenmaster");
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
			strcat(file,"ancientsmaster");
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
			strcat(file,"spacemas\ter");
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
			strcat(file,"citymaster");
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
			strcat(file,"submaster");
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

		case LABORATORYMASTER_COL:
			strcat(file,"labmaster");
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
			strcat(file,"toymaster");
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
			strcat(file,"halloweenmaster");
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

		case RETROMASTER_COL:
			strcat(file,"retromaster");
			break;
		default:
			dprintf"ERROR: no valid map bank specified....\n"));
			for(;;);
			break;
	}

	dprintf"Loading map %s\n",file));
	strcat (file,".dll");
    sceHandle = LoadLibrary(file);
	
	if (sceHandle != NULL)
	{
		FARPROC GetSc000Address;
		
		GetSc000Address   = GetProcAddress(sceHandle, "GetSC000Address");
		
		if (GetSc000Address)
			Sc_000 = GetSc000Address();
	}			
}

/*	--------------------------------------------------------------------------------
	Function		: IsATeleportTile
	Purpose			: checks if the specified tile is a teleport tile
	Parameters		: GAMETILE *
	Returns			: BOOL
	Info			: TRUE if a teleport tile, else FALSE
*/
BOOL IsATeleportTile(GAMETILE *tile)
{
	if(tile->state == TILESTATE_TELEPORTER)
		return TRUE;

	return FALSE;
}
/*	--------------------------------------------------------------------------------
	Function		: MakeTeleportTile
	Purpose			: makes the specified 'from' game tile a teleport tile to 'to' tile
	Parameters		: GAMETILE *,GAMETILE *,char
	Returns			: void
	Info			: 
*/
void MakeTeleportTile(GAMETILE *fromTile,GAMETILE *toTile,char teleportType)
{
	// determine type of teleporter
	switch(teleportType)
	{
		case TELEPORT_ONEWAY:
			// make 'from' tile a teleport tile and point it towards the 'to' tile
			fromTile->state			= TILESTATE_TELEPORTER;
			fromTile->teleportTo	= toTile;
			break;
		
		case TELEPORT_TWOWAY:
			// make 'from' tile a teleport tile and point it towards the 'to' tile
			fromTile->state			= TILESTATE_TELEPORTER;
			fromTile->teleportTo	= toTile;

			// make the 'to' tile a teleport tile and point it back towards the 'from' tile
			toTile->state			= TILESTATE_TELEPORTER;
			toTile->teleportTo		= fromTile;
			break;
	}
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
	player[pl].frogState |= FROGSTATUS_ISSTANDING;
	player[pl].frogState &= ~FROGSTATUS_ISTELEPORTING;

	// create vector with +'s...
/*
	CreateAndAddFXRipple(RIPPLE_TYPE_TELEPORT,act->actor->pos.v[X],act->actor->pos.v[Y],act->actor->pos.v[Z],&upVec,30,0,0,30);
	CreateAndAddFXRipple(RIPPLE_TYPE_TELEPORT,act->actor->pos.v[X],act->actor->pos.v[Y] + 20,act->actor->pos.v[Z],&upVec,25,0,0,25);
	CreateAndAddFXRipple(RIPPLE_TYPE_TELEPORT,act->actor->pos.v[X],act->actor->pos.v[Y] + 40,act->actor->pos.v[Z],&upVec,20,0,0,20);
	CreateAndAddFXRipple(RIPPLE_TYPE_TELEPORT,act->actor->pos.v[X],act->actor->pos.v[Y] + 60,act->actor->pos.v[Z],&upVec,15,0,0,15);
*/
}


/* --------------------------------------------------------------------------------
	Programmer	: David Swift
	Function	: LoadLevelEntities 
	Parameters	: int, int
	Returns		: void 
*/

extern void LoadLevelEntities(int worldID, int levelID)
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
	}

	JallocFree((UBYTE**)&buffer);
}

extern void LoadLevelEvents(int worldID, int levelID)
{
	char file[MAX_PATH];
	HANDLE h;
	long size, read;
	void* buffer;

	sprintf(file, "%s%sevent-%d-%d.dat", baseDirectory, ENTITY_BASE, worldID, levelID);

	h = CreateFile(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (h == INVALID_HANDLE_VALUE)
	{
		dprintf"Couldn't load event file %s\n", file)); return;
	}

	size = GetFileSize(h, NULL);
	buffer = JallocAlloc(size, NO, "entLoad");

	ReadFile(h, buffer, size, &read, NULL);
	CloseHandle(h);

	dprintf"LoadLevelEvents: Loaded %d of %d bytes\n", (int)read, (int)size));

	if (!MemLoadEvents(buffer, size))
	{
		dprintf"MemLoadEvents failed\n"));
	}

	JallocFree((UBYTE**)&buffer);
}