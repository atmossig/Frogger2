/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: map.c
	Programmer	: Matthew Cloy
	Date		: 04/12/98

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

#include <ultra64.h>

#include "incs.h"


char *mapBank;
char *levBank;
char *aMapBank = NULL;
char *aLevBank = NULL;

unsigned long numSafe = 1;
unsigned long numPwrups = 3;

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
	if (aMapBank)
		JallocFree ((UBYTE**)&aMapBank);
	if (aLevBank)
		JallocFree ((UBYTE**)&aLevBank);
}

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: LoadCollision 

	Purpose		:
	Parameters	: (int num)
	Returns		: void 
*/

void LoadCollision (int num)
{
	int					tnum = -1;
	char				*objectBank;
	u32					bankRomStart, bankRomEnd, bankSize;
	short				y, x;
	GAMETILE			*cgT;
	char				message[64];

	switch(num)
	{
		// GARDEN LEVELS
		case GARDENMASTER_COL:
			break;
		case GARDENMULTI_COL:
			tnum = GARDENMULTI_COLL_BANK;
			break;
		case GARDENLEV1_COL:
			tnum = LAWN_COLL_BANK;
			break;
		case GARDENLEV2_COL:
			tnum = MAZE_COLL_BANK;
			break;
		case GARDENLEV3_COL:
			tnum = VEGPATCH_COLL_BANK;
			break;

		// ANCIENTS LEVELS
		case ANCIENTMASTER_COL:
			break;
		case ANCIENTMULTI_COL:
			break;
		case ANCIENTLEV1_COL:
			break;
		case ANCIENTLEV2_COL:
			tnum = RUINEDCITY_COLL_BANK;
			break;
		case ANCIENTLEV3_COL:
			break;
	}
	
	bankRomStart	= (u32)collBanksStart[tnum];
	bankRomEnd		= (u32)collBanksEnd[tnum];			
	bankSize = bankRomEnd - bankRomStart;

	objectBank = (char *)JallocAlloc(DMAGetSize(bankRomStart, bankRomEnd), FALSE,"mBank");
	aMapBank = objectBank;
	//start download from rom

	DMAMemory(objectBank, bankRomStart, bankRomEnd);

	if(objectBank)
		dprintf"Loaded map bank %i (size %d)\n",tnum,(int)bankSize));
	else
	{
		dprintf"Unable to load map bank %s\n",message));
		return;
	}

	//store a pointer to the object bank so it may be free'd at a later date	
	
	//mapBank = &objectBank[36];
	mapBank = &objectBank[0];

	x = 0;

	// relocate GAMETILE *gTStart[4] = {&gT_141,&gT_015,0,0};

	gTStart = &mapBank[0];
	
	#define LONGVAL(x) (((unsigned long *)mapBank)[x])

	for (y=0; y<4; y++,x++)
		if (LONGVAL(x))
			LONGVAL(x) = Rom2Ram((u32)LONGVAL(x), (u32)objectBank);

	numBabies = LONGVAL(x++);
	
	// relocate GAMETILE *bTStart[4] = {&gT_026,&gT_134,&gT_005};

	bTStart = &LONGVAL(x);
	for (y=0; y<numBabies; y++,x++)
		if (LONGVAL(x))
			LONGVAL(x) = Rom2Ram((u32)LONGVAL(x), (u32)objectBank);

	numSafe = LONGVAL(x++);
	
	// relocate GAMETILE *bTSafe[4] =  {&gT_141,NULL,NULL,NULL};
	bTSafe = &LONGVAL(x);
	for (y=0; y<numSafe; y++,x++)
		if (LONGVAL(x))
			LONGVAL(x) = Rom2Ram((u32)LONGVAL(x), (u32)objectBank);

	numPwrups = LONGVAL(x++);

    // relocate GAMETILE *pwrupTStart[4] = { &gT_163,&gT_039,&gT_037,&gT_047 };
	pwrupTStart = &LONGVAL(x);
	for (y=0; y<numPwrups; y++,x++)
		if (LONGVAL(x))
			LONGVAL(x) = Rom2Ram((u32)LONGVAL(x), (u32)objectBank);

	cgT = &LONGVAL(x);

	firstTile = cgT;

	while (cgT)
	{
		for (y=0; y<4; y++)
			if (cgT->tilePtrs[y])
				cgT->tilePtrs[y] = Rom2Ram((u32)cgT->tilePtrs[y], (u32)objectBank);

		if (cgT->next)
			cgT->next = Rom2Ram((u32)cgT->next, (u32)objectBank);
		cgT = cgT->next;
	}

	
	//Rom2Ram((u32)objCont->object, (u32)objectBank);	
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
	int					tnum = -1;
	char				*objectBank;
	u32					bankRomStart, bankRomEnd, bankSize;
	short				y, x;
	SCENIC				*cSc;
	char				message[64];
	
/*

	SYSTEM_COL,

	SPACEMASTER_COL,
	SPACELEV1_COL,
	SPACELEV2_COL,
	SPACELEV3_COL,
	SPACEBONUS_COL,
	SPACEMULTI_COL,

	CITYMASTER_COL,
	CITYLEV1_COL,
	CITYLEV2_COL,
	CITYLEV3_COL,
	CITYBONUS_COL,
	CITYMULTI_COL,

	SUBTERRANEANMASTER_COL,
	SUBTERRANEANLEV1_COL,
	SUBTERRANEANLEV2_COL,
	SUBTERRANEANLEV3_COL,
	SUBTERRANEANBONUS_COL,
	SUBTERRANEANMULTI_COL,

	LABORATORYMASTER_COL,
	LABORATORYLEV1_COL,
	LABORATORYLEV2_COL,
	LABORATORYLEV3_COL,
	LABORATORYBONUS_COL,
	LABORATORYMULTI_COL,

	TOYSHOPMASTER_COL,
	TOYSHOPLEV1_COL,
	TOYSHOPLEV2_COL,
	TOYSHOPLEV3_COL,
	TOYSHOPBONUS_COL,
	TOYSHOPMULTI_COL,

	HALLOWEENMASTER_COL,
	HALLOWEENLEV1_COL,
	HALLOWEENLEV2_COL,
	HALLOWEENLEV3_COL,
	HALLOWEENBONUS_COL,
	HALLOWEENMULTI_COL,

	RETROMASTER_COL,
	RETROLEV1_COL,
	RETROLEV2_COL,
	RETROLEV3_COL,
	RETROBONUS_COL,
	RETROMULTI_COL,
*/

	switch(num)
	{
		// GARDEN LEVELS
		case GARDENMASTER_COL:
			break;
		case GARDENMULTI_COL:
			tnum = GARDENMULTI_COLL_BANK;
			break;
		case GARDENLEV1_COL:
			tnum = LAWN_COLL_BANK;
			break;
		case GARDENLEV2_COL:
			tnum = MAZE_COLL_BANK;
			break;
		case GARDENLEV3_COL:
			tnum = VEGPATCH_COLL_BANK;
			break;

		// ANCIENTS LEVELS
		case ANCIENTMASTER_COL:
			break;
		case ANCIENTMULTI_COL:
			break;
		case ANCIENTLEV1_COL:
			break;
		case ANCIENTLEV2_COL:
			tnum = RUINEDCITY_COLL_BANK;
			break;
		case ANCIENTLEV3_COL:
			break;
	}

	bankRomStart	= (u32)scenBanksStart[tnum];
	bankRomEnd		= (u32)scenBanksEnd[tnum];			
	bankSize = bankRomEnd - bankRomStart;

	objectBank = (char *)JallocAlloc(DMAGetSize(bankRomStart, bankRomEnd), FALSE,"sBank");
	aLevBank = objectBank;
	//start download from rom

	DMAMemory(objectBank, bankRomStart, bankRomEnd);

	if(objectBank)
		dprintf"Loaded scee bank %i (size %d)\n",tnum,(int)bankSize));
	else
	{
		dprintf"Unable to load map bank %s\n",message));
		return;
	}

	//store a pointer to the object bank so it may be free'd at a later date	
	
	//levBank = &objectBank[36];
	levBank = &objectBank[0];
	
	x = 0;

	Sc_000 = &levBank[0];
	
	#define LONGVAL(x) (((unsigned long *)levBank)[x])
	
	cSc = Sc_000;

	while (cSc)
	{		
		if (cSc->next)
			cSc->next = Rom2Ram((u32)cSc->next, (u32)objectBank);
		if (cSc->typedata)
		{
			TYPEDATA *m;
			cSc->typedata = Rom2Ram((u32)cSc->typedata, (u32)objectBank);
			m = cSc->typedata;
			if (m->movementPath)
				m->movementPath = Rom2Ram((u32)m->movementPath, (u32)objectBank);		
		}
		cSc = cSc->next;
	}		
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
	Function		: TeleportActorToTile
	Purpose			: teleports the actor to the specified tile
	Parameters		: ACTOR2 *,GAMETILE *
	Returns			: void
	Info			: 
*/
void TeleportActorToTile(ACTOR2 *act,GAMETILE *tile,long pl)
{
	VECTOR telePos;

	// make the teleport 'to' tile the current tile
	currTile[pl] = tile;
	SetVector(&act->actor->pos,&tile->centre);
	player[pl].frogState |= FROGSTATUS_ISSTANDING;
	player[pl].frogState &= ~FROGSTATUS_ISTELEPORTING;

	SetVector(&telePos,&act->actor->pos);

	CreateAndAddFXRipple(RIPPLE_TYPE_TELEPORT,&telePos,&upVec,30,0,0,30);
	telePos.v[Y] += 20;
	CreateAndAddFXRipple(RIPPLE_TYPE_TELEPORT,&telePos,&upVec,25,0,0,25);
	telePos.v[Y] += 40;
	CreateAndAddFXRipple(RIPPLE_TYPE_TELEPORT,&telePos,&upVec,20,0,0,20);
	telePos.v[Y] += 60;
	CreateAndAddFXRipple(RIPPLE_TYPE_TELEPORT,&telePos,&upVec,15,0,0,15);
}





// THIS IS A FUDGE - MOVE TO PRINTGFX.C (N64) EVENTUALLY *****************************************
char IsPointVisible(VECTOR *p)
{
	return 1;
}
