
// PC-ONLY FILE

/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: map.c
	Programmer	: Matthew Cloy
	Date		: 04/12/98

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

#include <ultra64.h>

#include "incs.h"

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
		case DOCKSA_COLL_BANK:
			sprintf(message, "DOCKSA");
			strcat(file,"docksa");
			break;
		case LAWN_COLL_BANK:
			sprintf(message, "LAWN");
			strcat(file,"lawn");
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
		case DOCKSA_COLL_BANK:
			sprintf(message, "DOCKSA");
			strcat(file,"docksa");
			break;
		case LAWN_COLL_BANK:
			sprintf(message, "LAWN");
			strcat(file,"lawn");
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
void TeleportActorToTile(ACTOR2 *act,GAMETILE *tile)
{
	// make the teleport 'to' tile the current tile
	currTile = tile;
	SetVector(&act->actor->pos,&tile->centre);
	frogState |= FROGSTATUS_ISSTANDING;
	frogState &= ~FROGSTATUS_ISTELEPORTING;

	// create vector with +'s...
/*
	CreateAndAddFXRipple(RIPPLE_TYPE_TELEPORT,act->actor->pos.v[X],act->actor->pos.v[Y],act->actor->pos.v[Z],&upVec,30,0,0,30);
	CreateAndAddFXRipple(RIPPLE_TYPE_TELEPORT,act->actor->pos.v[X],act->actor->pos.v[Y] + 20,act->actor->pos.v[Z],&upVec,25,0,0,25);
	CreateAndAddFXRipple(RIPPLE_TYPE_TELEPORT,act->actor->pos.v[X],act->actor->pos.v[Y] + 40,act->actor->pos.v[Z],&upVec,20,0,0,20);
	CreateAndAddFXRipple(RIPPLE_TYPE_TELEPORT,act->actor->pos.v[X],act->actor->pos.v[Y] + 60,act->actor->pos.v[Z],&upVec,15,0,0,15);
*/
}

