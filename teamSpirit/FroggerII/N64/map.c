/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: map.c
	Programmer	: Matthew Cloy
	Date		: 04/12/98

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


char *mapBank;
char *levBank;

// global ptrs used to free allocated resources
char *aMapBank				= NULL;
char *aLevBank				= NULL;
char *globalPtrEntityBank	= NULL;

unsigned long numSafe = 1;
unsigned long numPwrups = 3;

GAMETILE **bTSafe;
GAMETILE **pwrupTStart;

SCENIC *Sc_000;

char message[256];

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: LoadMapBank

	Purpose		:
	Parameters	: (int num)
	Returns		: void 
*/
void LoadMapBank(short worldID,short levelID)
{
	LoadCollision(worldID,levelID);
	LoadScenics(worldID,levelID);
}

void FreeMapBank()
{
	if(aMapBank)
		JallocFree((UBYTE**)&aMapBank);
	if(aLevBank)
		JallocFree((UBYTE**)&aLevBank);
}

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: LoadCollision 
	Purpose		:
	Parameters	: short,short
	Returns		: void 
*/

void LoadCollision(short worldID,short levelID)
{
	int					tnum = -1;
	char				*objectBank;
	u32					bankRomStart, bankRomEnd, bankSize;
	short				y, x;
	GAMETILE			*cgT;

	switch(worldID)
	{
		case WORLDID_GARDEN:
			switch(levelID)
			{
				case LEVELID_GARDENLEV1:
					tnum = GARDEN1_COLL_BANK;
					break;
				case LEVELID_GARDENLEV2:
					tnum = GARDEN2_COLL_BANK;
					break;
				case LEVELID_GARDENLEV3:
					tnum = GARDEN3_COLL_BANK;
					break;
				case LEVELID_GARDENBOSSA:
					tnum = GARDENBOSSA_COLL_BANK;
					break;
				case LEVELID_GARDENBOSSB:
					tnum = GARDENBOSSB_COLL_BANK;
					break;
				case LEVELID_GARDENBONUS:
					tnum = GARDENBONUS_COLL_BANK;
					break;
				case LEVELID_GARDENMULTI:
					tnum = GARDENMULTI_COLL_BANK;
					break;
			}
			break;

		case WORLDID_ANCIENT:
			switch(levelID)
			{
				case LEVELID_ANCIENTLEV1:
					tnum = ANCIENTS1_COLL_BANK;
					break;
				case LEVELID_ANCIENTLEV2:
					tnum = ANCIENTS2_COLL_BANK;
					break;
				case LEVELID_ANCIENTLEV3:
					tnum = ANCIENTS3_COLL_BANK;
					break;
			}
			break;

		case WORLDID_SPACE:
			switch(levelID)
			{
				case LEVELID_SPACELEV1:
					tnum = SPACE1_COLL_BANK;
					break;
				case LEVELID_SPACELEV2:
					tnum = SPACE2_COLL_BANK;
					break;
				case LEVELID_SPACELEV3:
					tnum = SPACE3_COLL_BANK;
					break;
				case LEVELID_SPACEBOSSA:
					tnum = SPACEBOSS_COLL_BANK;
					break;
			}
			break;
	
		case WORLDID_CITY:
			break;

		case WORLDID_SUBTERRANEAN:
			break;

		case WORLDID_LABORATORY:
			switch(levelID)
			{
				case LEVELID_LABORATORYLEV1:
					tnum = LAB1_COLL_BANK;
					break;
				case LEVELID_LABORATORYLEV2:
					tnum = LAB2_COLL_BANK;
					break;
				case LEVELID_LABORATORYLEV3:
					tnum = LAB3_COLL_BANK;
					break;
			}
			break;
		
		case WORLDID_TOYSHOP:
			break;
		
		case WORLDID_HALLOWEEN:
			break;
		
		case WORLDID_SUPERRETRO:
			break;
	}
	
	bankRomStart	= (u32)collBanksStart[tnum];
	bankRomEnd		= (u32)collBanksEnd[tnum];			
	bankSize = bankRomEnd - bankRomStart;

	objectBank = (char *)JallocAlloc(DMAGetSize(bankRomStart, bankRomEnd), FALSE,"COLLBANK");
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

void LoadScenics(short worldID,short levelID)
{
	int					tnum = -1;
	char				*objectBank;
	u32					bankRomStart, bankRomEnd, bankSize;
	short				y, x;
	SCENIC				*cSc;
	char				message[64];

	switch(worldID)
	{
		case WORLDID_GARDEN:
			switch(levelID)
			{
				case LEVELID_GARDENLEV1:
					tnum = GARDEN1_COLL_BANK;
					break;
				case LEVELID_GARDENLEV2:
					tnum = GARDEN2_COLL_BANK;
					break;
				case LEVELID_GARDENLEV3:
					tnum = GARDEN3_COLL_BANK;
					break;
				case LEVELID_GARDENBOSSA:
					tnum = GARDENBOSSA_COLL_BANK;
					break;
				case LEVELID_GARDENBOSSB:
					tnum = GARDENBOSSB_COLL_BANK;
					break;
				case LEVELID_GARDENBONUS:
					tnum = GARDENBONUS_COLL_BANK;
					break;
				case LEVELID_GARDENMULTI:
					tnum = GARDENMULTI_COLL_BANK;
					break;
			}
			break;
			
		case WORLDID_ANCIENT:
			switch(levelID)
			{
				case LEVELID_ANCIENTLEV1:
					tnum = ANCIENTS1_COLL_BANK;
					break;
				case LEVELID_ANCIENTLEV2:
					tnum = ANCIENTS2_COLL_BANK;
					break;
				case LEVELID_ANCIENTLEV3:
					tnum = ANCIENTS3_COLL_BANK;
					break;
			}
			break;

		case WORLDID_SPACE:
			switch(levelID)
			{
				case LEVELID_SPACELEV1:
					tnum = SPACE1_COLL_BANK;
					break;
				case LEVELID_SPACELEV2:
					tnum = SPACE2_COLL_BANK;
					break;
				case LEVELID_SPACELEV3:
					tnum = SPACE3_COLL_BANK;
					break;
				case LEVELID_SPACEBOSSA:
					tnum = SPACEBOSS_COLL_BANK;
					break;
			}
			break;
	
		case WORLDID_CITY:
			break;

		case WORLDID_SUBTERRANEAN:
			break;

		case WORLDID_LABORATORY:
			switch(levelID)
			{
				case LEVELID_LABORATORYLEV1:
					tnum = LAB1_COLL_BANK;
					break;
				case LEVELID_LABORATORYLEV2:
					tnum = LAB2_COLL_BANK;
					break;
				case LEVELID_LABORATORYLEV3:
					tnum = LAB3_COLL_BANK;
					break;
			}
			break;
		
		case WORLDID_TOYSHOP:
			break;
		
		case WORLDID_HALLOWEEN:
			break;
		
		case WORLDID_SUPERRETRO:
			break;
	}

	bankRomStart	= (u32)scenBanksStart[tnum];
	bankRomEnd		= (u32)scenBanksEnd[tnum];			
	bankSize = bankRomEnd - bankRomStart;

	objectBank = (char *)JallocAlloc(DMAGetSize(bankRomStart, bankRomEnd), FALSE,"SCENBANK");
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
	Function		: LoadLevelEntities
	Purpose			: Loads in the data for level, inc enemies, platforms etc
	Parameters		: short worldID - world number
				 	  short levelID - level number
	Returns			: void
	Info			: 
*/
void LoadLevelEntities(short worldID,short levelID)
{
	char	*entityDat;
	u32		bankRomStart,bankRomEnd,bankSize;

	if(worldID == WORLDID_GARDEN)
	{
		switch(levelID)
		{
			case GARDENLEV1_ENT:
					bankRomStart	= (u32)&_levData_1_1_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_1_1_SegmentRomEnd;
					sprintf(message, "GAR_ENT1");				
				break;

			case GARDENLEV2_ENT:
					bankRomStart	= (u32)&_levData_1_2_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_1_2_SegmentRomEnd;
					sprintf(message, "GAR_ENT2");				
				break;

			case GARDENLEV3_ENT:
					bankRomStart	= (u32)&_levData_1_3_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_1_3_SegmentRomEnd;
					sprintf(message, "GAR_ENT3");				
				break;

			case GARDENBOSSA_ENT:
					bankRomStart	= (u32)&_levData_1_4_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_1_4_SegmentRomEnd;
					sprintf(message, "GAR_BOSA");
				break;

			case GARDENBOSSB_ENT:
					bankRomStart	= (u32)&_levData_1_5_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_1_5_SegmentRomEnd;
					sprintf(message, "GAR_BOSB");
				break;

			case GARDENBONUS_ENT:
					bankRomStart	= (u32)&_levData_1_6_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_1_6_SegmentRomEnd;
					sprintf(message, "GAR_BONS");
				break;

			case GARDENMULTI_ENT:
					bankRomStart	= (u32)&_levData_1_7_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_1_7_SegmentRomEnd;
					sprintf(message, "GAR_MULT");
				break;

			default:
				return;
		}
	}
	else if(worldID == WORLDID_ANCIENT)
	{
		switch(levelID)
		{
			case ANCIENTLEV1_ENT:
					bankRomStart	= (u32)&_levData_2_1_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_2_1_SegmentRomEnd;
					sprintf(message, "ANC_ENT1");				
				break;

			case ANCIENTLEV2_ENT:
					bankRomStart	= (u32)&_levData_2_2_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_2_2_SegmentRomEnd;
					sprintf(message, "ANC_ENT2");				
				break;

			case ANCIENTLEV3_ENT:
					bankRomStart	= (u32)&_levData_2_3_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_2_3_SegmentRomEnd;
					sprintf(message, "ANC_ENT3");				
				break;

			case ANCIENTBOSSA_ENT:
					bankRomStart	= (u32)&_levData_2_4_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_2_4_SegmentRomEnd;
					sprintf(message, "ANC_BOSA");
				break;

			case ANCIENTBOSSB_ENT:
					bankRomStart	= (u32)&_levData_2_5_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_2_5_SegmentRomEnd;
					sprintf(message, "ANC_BOSB");
				break;

			case ANCIENTBOSSC_ENT:
					bankRomStart	= (u32)&_levData_2_6_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_2_6_SegmentRomEnd;
					sprintf(message, "ANC_BOSC");
				break;

			case ANCIENTBOSSD_ENT:
					bankRomStart	= (u32)&_levData_2_7_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_2_7_SegmentRomEnd;
					sprintf(message, "ANC_BOSD");
				break;

			case ANCIENTBONUS_ENT:
					bankRomStart	= (u32)&_levData_2_8_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_2_8_SegmentRomEnd;
					sprintf(message, "ANC_BONS");
				break;

			case ANCIENTMULTI_ENT:
					bankRomStart	= (u32)&_levData_2_9_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_2_9_SegmentRomEnd;
					sprintf(message, "ANC_MULT");
				break;

			default:
				return;
		}
	}
	else if(worldID == WORLDID_SPACE)
	{
		switch(levelID)
		{
			case SPACELEV1_ENT:
					bankRomStart	= (u32)&_levData_3_1_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_3_1_SegmentRomEnd;
					sprintf(message, "SPC_ENT1");				
				break;

			case SPACELEV2_ENT:
					bankRomStart	= (u32)&_levData_3_2_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_3_2_SegmentRomEnd;
					sprintf(message, "SPC_ENT2");				
				break;

			case SPACELEV3_ENT:
					bankRomStart	= (u32)&_levData_3_3_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_3_3_SegmentRomEnd;
					sprintf(message, "SPC_ENT3");				
				break;
		}
	}
	else if(worldID == WORLDID_CITY)
	{
		switch(levelID)
		{
		}
	}
	else if(worldID == WORLDID_SUBTERRANEAN)
	{
		switch(levelID)
		{
		}
	}
	else if(worldID == WORLDID_LABORATORY)
	{
		switch(levelID)
		{
		}
	}
	else if(worldID == WORLDID_TOYSHOP)
	{
		switch(levelID)
		{
		}
	}
	else if(worldID == WORLDID_HALLOWEEN)
	{
		switch(levelID)
		{
		}
	}
	else if(worldID == WORLDID_SUPERRETRO)
	{
		switch(levelID)
		{
		}
	}

	bankSize = bankRomEnd - bankRomStart;

	entityDat = (char *)JallocAlloc(DMAGetSize(bankRomStart,bankRomEnd),YES,"ENTBANK");
	globalPtrEntityBank = entityDat;
	
//start download from rom

	DMAMemory ( entityDat, bankRomStart, bankRomEnd );
	if ( entityDat )
	{
		dprintf"Loaded entity data %s (size %d)\n",message,(int)bankSize));
	}
	else
	{
		dprintf"Unable to load entity data %s\n",message));
		return;
	}
	// ENDIF - entityDat

	MemLoadEntities(entityDat,bankSize);
}

/*	--------------------------------------------------------------------------------
	Function		: FreeLevelEntitys
	Purpose			: frees the entity bank associated with the level
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeLevelEntitys()
{
	if(globalPtrEntityBank)
		JallocFree((UBYTE**)&globalPtrEntityBank);
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
	int i = 8;

	// make the teleport 'to' tile the current tile
	currTile[pl] = tile;
	SetVector(&act->actor->pos,&tile->centre);
	player[pl].frogState |= FROGSTATUS_ISSTANDING;
	player[pl].frogState &= ~FROGSTATUS_ISTELEPORTING;

	frog[pl]->actor->scale.v[X] = globalFrogScale;
	frog[pl]->actor->scale.v[Y] = globalFrogScale;
	frog[pl]->actor->scale.v[Z] = globalFrogScale;

	while(i--)
		CreateAndAddFXSmoke(&tile->centre,64,30);

	drawScreenGrab = 0;
}
