#include "map_draw.h"
#include "world_eff.h"
#include <islpad.h>
#include <isltex.h>
#include <islcard.h>
#include "main.h"
#include "layout.h"
#include "frogger.h"
#include "savegame.h"
#include "water.h"
#include "actor2.h"
#include "temp_psx.h"
#include "cam.h"
#include "textures.h"
#include "map_draw.h"
#include "timer.h"
#include "memcard.h"
#include "lang.h"
#include "audio.h"
#include <Libmcrd.h>

#define MIN min
#define MAX max

extern TextureType	*buttonSprites[6];

int framePause = 0;
static int	optChosen, optFrame, optSaveAlready, optSaveSlot;
static char	optStr[256];

#ifdef FINAL_MASTER
int useMemCard = 1;
#else
int useMemCard = 1;
#endif

char memmessage[256];
char slotNumStr[4];

SAVE_INFO saveInfo;

int delayTimer;
int cardChanged = NO;
#define DELAY_TIME 200

int gameSaveGetCardStatus()
{
	long	cmds, result;

	switch(MemCardSync(1, &cmds, &result))
	{
	case -1:	// Start off status request
		MemCardExist(0);
		break;
	case 1:		// Return status result
		if (cmds==McFuncExist)
			return result;
		break;
	}
	return -1;
}

#define SAVE_FILENAME "BESLES-00000FROGGER2"

int gameSaveHandleSave()
{
	void *info;
	int res;
	unsigned long size;

	MakeSaveGameBlock(&info, &size);
	utilPrintf("Saving %d\n", size);
	res = cardWrite(SAVE_FILENAME, info, size);

	switch(res)
	{
		case CARDWRITE_OK:
		{
			utilPrintf("No errors\n");
			break;
		}
		case CARDWRITE_NOCARD:
		{
			utilPrintf("No card in slot\n");
			break;
		}
		case CARDWRITE_BADCARD:
		{
			utilPrintf("Bad card in slot\n");
			break;
		}
		case CARDWRITE_NOTFORMATTED:
		{
			utilPrintf("Card unformatted\n");
			break;
		}
		case CARDWRITE_FULL:
		{
			utilPrintf("Card is full\n");
			break;
		}
	} //end 


	FREE(info);

	return res;
}

int gameSaveHandleLoad(int justChecking)
{
	void *info;
	int size;
	int res;

	if(justChecking)
	{
		utilPrintf("Checking Save Game\n");
		res = cardRead(SAVE_FILENAME, 0, SAVEGAME_SIZE+PSXCARDHEADER_SIZE); //checking, not loading
		switch(res)
		{
			case CARDREAD_OK:
			{
				utilPrintf("No errors\n");
				break;
			}
			case CARDREAD_NOCARD:
			{
				utilPrintf("No card in slot\n");
				break;
			}
			case CARDREAD_BADCARD:
			{
				utilPrintf("Bad card in slot\n");
				break;
			}
			case CARDREAD_NOTFORMATTED:
			{
				utilPrintf("Card unformatted\n");
				break;
			}
			case CARDREAD_NOTFOUND:
			{
				utilPrintf("No game save data found\n");
				break;
			}
			case CARDREAD_CORRUPT:
			{
				utilPrintf("Game save data corrupted\n");
				break;
			}
			case CARDREAD_NOTFOUNDANDFULL:
			{
				utilPrintf("No game save data found and card is full\n");
				break;
			}
		} //end 


		return res;
	}

	//make buffer
	info = MALLOC0(SAVEGAME_SIZE+PSXCARDHEADER_SIZE);
	if(!info)
	{
		utilPrintf("\n\nMALLOC ERROR DURING LOAD\n\n");
		return;
	}

	utilPrintf("Loading Save Game %d\n", SAVEGAME_SIZE+PSXCARDHEADER_SIZE);
	res = cardRead(SAVE_FILENAME, info, SAVEGAME_SIZE+PSXCARDHEADER_SIZE);

	//bb - skip over PsxCardHeader
	//NO - responsibility of updated LoadSaveGameBlock (like MakeSaveGameBlock).

	switch(res)
	{
		case CARDREAD_OK:
		{
			utilPrintf("No errors\n");
			LoadSaveGameBlock(info, size);
			break;
		}
		case CARDREAD_NOCARD:
		{
			utilPrintf("No card in slot\n");
			break;
		}
		case CARDREAD_BADCARD:
		{
			utilPrintf("Bad card in slot\n");
			break;
		}
		case CARDREAD_NOTFORMATTED:
		{
			utilPrintf("Card unformatted\n");
			break;
		}
		case CARDREAD_NOTFOUND:
		{
			utilPrintf("No game save data found\n");
			break;
		}
		case CARDREAD_CORRUPT:
		{
			utilPrintf("Game save data corrupted\n");
			break;
		}
		case CARDREAD_NOTFOUNDANDFULL:
		{
			utilPrintf("No game save data found and card is full\n");
			break;
		}

	} //end 

	//free buffer
	FREE(info);

	return res;
}

void StartChooseOption()
{
	optFrame = 1;
	optChosen = 0;
}


int FontInSpace(int x, int y, char *str, int w, int l, uchar r, uchar g, uchar b)
{
	char	buf[200], *bufPtr;
	int		loop, numLines;


	sprintf(memmessage,str,slotNumStr);

	numLines = fontFitToWidth(fontSmall, w, memmessage, buf);
	if (numLines<l)
		y += (l-numLines)*6;
	bufPtr = buf;
	for(loop=0; loop<numLines; loop++)
	{
		fontPrint(fontSmall, x,y, bufPtr, r,g,b);
		bufPtr += strlen(bufPtr)+1;
		y += 12;
	}
	return numLines;
}

void SimpleMessage(char *msg, uchar rgb)
{
	char	*bufPtr;
	int		y, loop, numLines;
	POLY_F4	*f4;
	DR_MODE	*dm;


	sprintf(memmessage,msg,slotNumStr);

	numLines = fontFitToWidth(fontSmall, 420, memmessage, optStr);
	bufPtr = optStr;
	y = -47-numLines*6;

	BEGINPRIM(f4, POLY_F4);
	setXYWH(f4, -256,y-7, 512,numLines*12+15);
	setRGB0(f4, rgb/2,rgb/2,rgb/2);
	setPolyF4(f4);
	setSemiTrans(f4, 1);
	ENDPRIM(f4, 5, POLY_F4);
	BEGINPRIM(dm, DR_MODE);
	SetDrawMode(dm, 0,1, (SEMITRANS_SUB-1)<<5,0);
	ENDPRIM(dm, 5, DR_MODE);

	for(loop=0; loop<numLines; loop++)
	{
		fontPrint(fontSmall, -fontExtentW(fontSmall, bufPtr)/2,y, bufPtr, rgb,rgb,rgb);
		fontSmall->alpha = 3;
		fontPrint(fontSmall, -fontExtentW(fontSmall, bufPtr)/2,y+2, bufPtr, rgb,rgb,rgb);
		fontSmall->alpha = 2;
		bufPtr += strlen(bufPtr)+1;
		y += 12;
	}
}


int ChooseOption(char *msg, char *msg1, char *msg2)
{
	int		x,y, f[2], f2, loop, p,numLines = 0;
	uchar	r[4],g[4],b[4];
	POLY_F4	*f4;
	DR_MODE	*dm;


	framePause++;
	f2 = optFrame<<5;
	for(loop=0; loop<4; loop++)
	{
		r[loop] = MIN(f2,100); g[loop] = MIN(f2,25); b[loop] = 0;
	}
	switch(optChosen)
	{
	case 0:		// Fade up
		f[0] = (optFrame<<5)-0;
		f[1] = MAX((optFrame<<4)-50,0);
		optFrame++;
		break;
	case 2:		// Fade down except 2/circle
		f2 = MAX(optFrame-10,0);
		r[1] = 100; g[1] = 25; b[1] = 0;
		f[0] = (f2<<4)-0;
		f[1] = MAX((f2<<4)-50,0);
		optFrame--;
		break;
	case 3:		// Fade down except 3/cross
		f2 = MAX(optFrame-10,0);
		r[2] = 100; g[2] = 25; b[2] = 0;
		f[0] = (f2<<4)-0;
		f[1] = MAX((f2<<4)-50,0);
		optFrame--;
		break;
	}

	f2 = MIN(f[1],128);
	SimpleMessage(msg, f2);

	x = -160;
	y = -77+38*2;

	if ((p = ((optFrame<<2) & 63))>31)
		p = 31-(p-31);
	if (msg2!=NULL)
	{
		f2 = MIN(f[1],127);
		FontInSpace(x,y, msg2, 380,2, f2,f2,f2);		// 2/triangle
		fontSmall->alpha = 3;
		FontInSpace(x,y+2, msg2, 380,2, f2,f2,f2);		// 2/triangle
		fontSmall->alpha = 2;
		fontDispSprite(buttonSprites[0], x-50,y+1, f2*2,f2*2,f2*2,2);
		if(f2 > 120)
			fontDispSprite(buttonSprites[0], x-50,y+3, 0,0,0,1);
	}
	y += 48;

	f2 = MIN(f[1],127);
	numLines = FontInSpace(x,y, msg1, 380,2, f2,f2,f2);		// 3/Cross
	fontSmall->alpha = 3;
	numLines = FontInSpace(x,y+2, msg1, 380,2, f2,f2,f2);		// 3/Cross
	fontSmall->alpha = 2;
	fontDispSprite(buttonSprites[2], x-50,y+1, f2*2,f2*2,f2*2, 2);
	if(f2 > 120)
		fontDispSprite(buttonSprites[2], x-50,y+3, 0,0,0, 1);
	y += 40;
												// Help buttons
	BEGINPRIM(f4, POLY_F4);
	if(msg2)
	{
		setXYWH(f4, -256,-77+38*2-7, 512,numLines*12 + 48 + 20);
	}
	else
	{
		setXYWH(f4, -256,-77+38*2-7+48+5, 512,numLines*12 + 15);
	}
	setRGB0(f4, f2/2,f2/2,f2/2);
	setPolyF4(f4);
	setSemiTrans(f4, 1);
	ENDPRIM(f4, 5, POLY_F4);
	BEGINPRIM(dm, DR_MODE);
	SetDrawMode(dm, 0,1, (SEMITRANS_SUB-1)<<5,0);
	ENDPRIM(dm, 5, DR_MODE);

	if (!optChosen)
	{
		x = -14*(1+(msg2!=NULL))-fontExtentW(fontSmall, GAMESTRING(STR_SELECTOPTION))/2-5;
		if (msg2!=NULL)
		{
			fontDispSprite(buttonSprites[0], x,y-3, 255,255,255, 2);
			fontDispSprite(buttonSprites[0], x,y-1, 0,0,0, 1);
			x += 28;
		}
		fontDispSprite(buttonSprites[2], x,y-3, 255,255,255, 2);
		fontDispSprite(buttonSprites[2], x,y-1, 0,0,0, 1);
		x += 28;
		x += 10;
		fontPrint(fontSmall, x,y, GAMESTRING(STR_SELECTOPTION), 128,128,128);
		fontSmall->alpha = 3;
		fontPrint(fontSmall, x,y+2, GAMESTRING(STR_SELECTOPTION), 128,128,128);
		fontSmall->alpha = 2;
	}

	if (!optChosen)								// Make selection
	{
		if (optFrame>20)
		{
			if ((msg2!=NULL) && (padData.debounce[0] & PAD_TRIANGLE))
			{
				optChosen = 2;
				optFrame = 25;
				PlaySample(genSfx[GEN_SUPER_HOP],NULL,0,SAMPLE_VOLUME,-1);
			}
			if (padData.debounce[0] & PAD_CROSS)
			{
				optChosen = 3;
				optFrame = 25;
				PlaySample(genSfx[GEN_SUPER_HOP],NULL,0,SAMPLE_VOLUME,-1);
			}
		}
	}
	else
	{
		if (optFrame==1)						// Fade complete, save and/or continue
		{
			return optChosen;
			optFrame = 0;
		}
	}

	return 0;
}

enum {
	SAVEMENU_CHECK,
	SAVEMENU_FULL,
	SAVEMENU_NOCARD,
	SAVEMENU_SAVE,
	SAVEMENU_COMPLETE,
	SAVEMENU_FAIL,
	SAVEMENU_FORMATYN,
	SAVEMENU_FORMAT,
	SAVEMENU_FORMATFAIL,
	SAVEMENU_NEEDFORMAT,
	SAVEMENU_LOAD,
	SAVEMENU_LOADERROR,
	SAVEMENU_LOADCOMPLETE,
	SAVEMENU_SAVEYN,
	SAVEMENU_CHANGED,
	SAVEMENU_OVERWRITE,
	SAVEMENU_FORMATCOMPLETE,
};


void StartChooseLoadSave(int load)
{
	saveInfo.saveFrame = 1;
	saveInfo.saveChosen = 0;
	saveInfo.saveStage = SAVEMENU_CHECK;
	saveInfo.load = load;
	framePause++;
	StartChooseOption();
	if(padData.numPads[0] == 4)
		sprintf(slotNumStr,"1-A");
	else
		sprintf(slotNumStr,"1");
}


static void saveChooseSlot()
{
/*
	int		x,y, f[4], f2, loop, p;
	uchar	r[4],g[4],b[4];
	char	str[50];

	framePause++;
	x = -160;
	y = -57;
	if (!optChosen)					// Fade in/out lozenges
		f2 = optFrame<<5;
	else
		f2 = MAX((optFrame-40)<<4,0);
	for(loop=0; loop<4; loop++)
	{
		r[loop] = MIN(f2,100); g[loop] = MIN(f2,25); b[loop] = 0;
	}
	switch(optChosen)
	{
	case 0:		// Fade up
		f[0] = (optFrame<<4)-0;
		f[1] = MAX((optFrame<<4)-50,0);
		f[2] = MAX((optFrame<<4)-100,0);
		f[3] = MAX((optFrame<<4)-150,0);
		optFrame++;
		break;
	case 1:		// Fade down except 1/triangle
		f2 = MAX(optFrame-40,0);
		r[0] = 100; g[0] = 25; b[0] = 0;
		f[0] = 255;
		f[1] = MAX((f2<<4)-50,0);
		f[2] = MAX((f2<<4)-100,0);
		f[3] = MAX((f2<<4)-150,0);
		optFrame--;
		break;
	case 2:		// Fade down except 2/circle
		f2 = MAX(optFrame-40,0);
		r[1] = 100; g[1] = 25; b[1] = 0;
		f[0] = (f2<<4)-0;
		f[1] = 255;
		f[2] = MAX((f2<<4)-100,0);
		f[3] = MAX((f2<<4)-150,0);
		optFrame--;
		break;
	case 3:		// Fade down except 3/cross
		f2 = MAX(optFrame-40,0);
		r[2] = 100; g[2] = 25; b[2] = 0;
		f[0] = (f2<<4)-0;
		f[1] = MAX((f2<<4)-50,0);
		f[2] = 255;
		f[3] = MAX((f2<<4)-150,0);
		optFrame--;
		break;
	case 4:		// Fade down except 4/square
		f2 = MAX(optFrame-40,0);
		r[3] = 100; g[3] = 25; b[3] = 0;
		f[0] = (f2<<4)-0;
		f[1] = MAX((f2<<4)-50,0);
		f[2] = MAX((f2<<4)-100,0);
		f[3] = 255;
		optFrame--;
		break;
	}

	if ((p = ((optFrame<<2) & 63))>31)
		p = 31-(p-31);
	f2 = MIN(f[0],100);
	streetFontInSpace(x,y, GAMESTRING(STR_MCARD_CANCELSAVE), 375,2, f2,f2,f2);// 1/Triangle
	menuSprite(street.HUDpieces[HUD_BIGTRIANGLE], x-50,y+1, 0, 0,MIN(f[0],200-63+p*2),0, SEMITRANS_ADD);
	streetLozenge(x-49,y-5, 425, r[0],g[0],b[0]);
	y += 38;

	f2 = MIN(f[1],100);
	if (gameSaveData.slot[0].gamemode!=-1)					// 2/Circle
		sprintf(str, GAMESTRING(STR_MENU_SLOT), 'A', gameSaveData.slot[0].name);
	else
		sprintf(str, GAMESTRING(STR_MENU_SLOTEMPTY), 'A');
	streetFontInSpace(x,y, str, 375,2, f2,f2,f2);
	menuSprite(street.HUDpieces[HUD_BIGCIRCLE], x-50,y+1, 0, MIN(f[1],200-31+p),MIN(f[1],40-31+p),MIN(f[1],40-31+p), SEMITRANS_ADD);
	streetLozenge(x-49,y-5, 425, r[1],g[1],b[1]);
	y += 38;

	f2 = MIN(f[2],100);
	if (gameSaveData.slot[1].gamemode!=-1)					// 3/Cross
		sprintf(str, GAMESTRING(STR_MENU_SLOT), 'B', gameSaveData.slot[1].name);
	else
		sprintf(str, GAMESTRING(STR_MENU_SLOTEMPTY), 'B');
	streetFontInSpace(x,y, str, 375,2, f2,f2,f2);
	menuSprite(street.HUDpieces[HUD_BIGCROSS], x-50,y+1, 0, MIN(f[2],80-31+p),MIN(f[2],80-31+p),MIN(f[2],200-31+p), SEMITRANS_ADD);
	streetLozenge(x-49,y-5, 425, r[2],g[2],b[2]);
	y += 38;

	f2 = MIN(f[3],100);
	if (gameSaveData.slot[2].gamemode!=-1)					// 4/Square
		sprintf(str, GAMESTRING(STR_MENU_SLOT), 'C', gameSaveData.slot[2].name);
	else
		sprintf(str, GAMESTRING(STR_MENU_SLOTEMPTY), 'C');
	streetFontInSpace(x,y, str, 375,2, f2,f2,f2);
	menuSprite(street.HUDpieces[HUD_BIGSQUARE], x-50,y+2, 0, MIN(f[3],200-31+p),MIN(f[3],80-31+p),MIN(f[3],200-31+p), SEMITRANS_ADD);
	streetLozenge(x-49,y-5, 425, r[3],g[3],b[3]);
	y += 35;
												// Help buttons
	if (!optChosen)
	{
		x = -14*4-fontExtentW(font, GAMESTRING(STR_MCARD_SELECTSAVE))/2-5;
		menuSprite(street.HUDpieces[HUD_TRIANGLE], x,y-2, 0, 128,128,128, 0);
		x += 28;
		menuSprite(street.HUDpieces[HUD_CIRCLE], x,y-2, 0, 128,128,128, 0);
		x += 28;
		menuSprite(street.HUDpieces[HUD_CROSS], x,y-2, 0, 128,128,128, 0);
		x += 28;
		menuSprite(street.HUDpieces[HUD_SQUARE], x,y-2, 0, 128,128,128, 0);
		x += 28;
		x += 10;
		fontPrint(font, x,y, GAMESTRING(STR_MCARD_SELECTSAVE), 80,80,80);
	}

	if (!optChosen)					// Make selection
	{
		if (optFrame>20)
		{
			if (padData.debounce[0] & PAD_TRIANGLE)
				optChosen = 1;
			if (padData.debounce[0] & PAD_CIRCLE)
				optChosen = 2;
			if (padData.debounce[0] & PAD_CROSS)
				optChosen = 3;
			if (padData.debounce[0] & PAD_SQUARE)
				optChosen = 4;
			if (padData.debounce[0] & (PAD_TRIANGLE|PAD_CIRCLE|PAD_CROSS|PAD_SQUARE))
				optFrame = 60;
		}
	}
	else
	{											// Fade complete, trigger mission
		if (optFrame==1)
		{
			optFrame = 0;
			framePause = 0;
		}
	}
*/
}


/* FULL CARD PAGE ******************************************************************************************************/

static void saveMenuFull()
{
	switch(ChooseOption(GAMESTRING(STR_MCARD_FULL), GAMESTRING(STR_MCARD_RECHECK), GAMESTRING(STR_MCARD_PROCEED_WITHOUT_SAVE)))
	{
	case 3:
		saveInfo.saveStage = SAVEMENU_CHECK;
		StartChooseOption();
		break;
	case 2:
		saveInfo.saveFrame = 0;
		useMemCard = 0;
		break;
	}
	switch(gameSaveGetCardStatus())
	{
   	case McErrCardNotExist:
   	case McErrCardInvalid:
		saveInfo.saveStage = SAVEMENU_NOCARD;
		StartChooseOption();
		break;
	}
}

/* NO CARD PAGE ********************************************************************************************************/

static void saveMenuNoCard()
{
	int choice;

	choice = ChooseOption(GAMESTRING(STR_MCARD_NOCARD), GAMESTRING(STR_MCARD_RECHECK), GAMESTRING(STR_MCARD_PROCEED_WITHOUT_SAVE));
//		ChooseOption(GAMESTRING(STR_MCARD_NOCARD), GAMESTRING(STR_MCARD_SAVEGAME), GAMESTRING(STR_MCARD_CANCELSAVE));
	switch(choice)
	{
	case 3:
		saveInfo.saveStage = SAVEMENU_CHECK;
		StartChooseOption();
		break;
	case 2:
		saveInfo.saveFrame = 0;
		if(saveInfo.load)
			useMemCard = 0;
		break;
	}
/*
	switch(gameSaveGetCardStatus())
	{
   	case McErrNone:
   	case McErrNewCard:
		saveInfo.saveStage = SAVEMENU_CHECK;
		StartChooseOption();
		break;
	}
*/
}

static void saveMenuLoad()
{
	SimpleMessage(GAMESTRING(STR_MCARD_LOADING), MIN(255,optFrame*60));
	if (optFrame++>4)
	{
		if (gameSaveHandleLoad(NO) != CARDREAD_OK)
			saveInfo.saveStage = SAVEMENU_LOADERROR;
		else
		{
			saveInfo.saveStage = SAVEMENU_LOADCOMPLETE;
			delayTimer = 0;
		}
		StartChooseOption();
	}
}

/* CHECK CARD PAGE *****************************************************************************************************/

static void saveMenuCheck()
{
	int res;

	SimpleMessage(GAMESTRING(STR_MCARD_CHECK), MIN(255,optFrame*60));
	if (optFrame++>4)
	{
		switch(gameSaveHandleLoad(YES))
		{
		case CARDREAD_OK:						// Loaded fine
			optSaveAlready = 1;
			if(saveInfo.load)
			{
				saveInfo.saveStage = SAVEMENU_LOAD;
				StartChooseOption();
			}
			else if(cardChanged)
			{
				saveInfo.saveStage = SAVEMENU_OVERWRITE;
				StartChooseOption();
			}
			else
			{
				saveInfo.saveStage = SAVEMENU_SAVE;
				StartChooseOption();
			}
			break;
		case CARDREAD_NOTFOUND:		// Data not found, space on card
//		case -2:					// Corrupt data
			optSaveAlready = 0;
			saveInfo.saveStage = SAVEMENU_SAVEYN;
			StartChooseOption();
			break;
   		case CARDREAD_NOCARD:		// No card
   		case CARDREAD_BADCARD:		// Bad card
			saveInfo.saveStage = SAVEMENU_NOCARD;
			StartChooseOption();
			break;
		case CARDREAD_NOTFOUNDANDFULL:					// Data not found, full card
			saveInfo.saveStage = SAVEMENU_FULL;
			StartChooseOption();
			break;
		case CARDREAD_NOTFORMATTED: 		// Card not formatted
			saveInfo.saveStage = SAVEMENU_FORMATYN;
			StartChooseOption();
			break;
		}
	}
}

/* SAVE PAGE ***********************************************************************************************************/


static void saveMenuSave()
{
	SimpleMessage(GAMESTRING(STR_MCARD_SAVING), MIN(255,optFrame*60));
	if (optFrame++>4)
	{
		if (gameSaveHandleSave() != CARDWRITE_OK)
			saveInfo.saveStage = SAVEMENU_FAIL;
		else
		{
			saveInfo.saveStage = SAVEMENU_COMPLETE;
			delayTimer = 0;
		}
		StartChooseOption();
	}
}

/* SAVE COMPLETE PAGE **************************************************************************************************/

static void saveMenuComplete()
{
	if ((delayTimer++ > DELAY_TIME) || (ChooseOption(GAMESTRING(STR_MCARD_COMPLETE), GAMESTRING(STR_MCARD_CONTINUE), NULL)))
	{
		saveInfo.saveFrame = 0;
		cardChanged = NO;
	}
}

static void saveMenuLoadComplete()
{
	if ((delayTimer++ > DELAY_TIME) || (ChooseOption(GAMESTRING(STR_MCARD_LOADCOMPLETE), GAMESTRING(STR_MCARD_CONTINUE), NULL)))
		saveInfo.saveFrame = 0;
}

static void saveMenuFormatComplete()
{
	if ((delayTimer++ > DELAY_TIME) || (ChooseOption(GAMESTRING(STR_MCARD_FORMATCOMPLETE), GAMESTRING(STR_MCARD_CONTINUE), NULL)))
	{
		saveInfo.saveStage = SAVEMENU_SAVEYN;
		StartChooseOption();
	}
}

static void saveMenuLoadError()
{
	switch(ChooseOption(GAMESTRING(STR_MCARD_LOADERROR), GAMESTRING(STR_MCARD_RECHECK), GAMESTRING(STR_MCARD_PROCEED_WITHOUT_SAVE)))
	{
	case 3:
		saveInfo.saveStage = SAVEMENU_CHECK;
		StartChooseOption();
		break;
	case 2:
		saveInfo.saveFrame = 0;
		useMemCard = 0;
		break;
	}
}

/* SAVE FAIL PAGE ******************************************************************************************************/

static void saveMenuFail()
{
	if (ChooseOption(GAMESTRING(STR_MCARD_SAVEERROR), GAMESTRING(STR_MCARD_RETRYSAVE), NULL))
	{
		saveInfo.saveStage = SAVEMENU_CHECK;
		StartChooseOption();
	}
	switch(gameSaveGetCardStatus())
	{
   	case McErrCardNotExist:
   	case McErrCardInvalid:
		saveInfo.saveStage = SAVEMENU_NOCARD;
		StartChooseOption();
		break;
	}
}

static void saveMenuOverwrite()
{
	switch(ChooseOption(GAMESTRING(STR_MCARD_OVERWRITE), GAMESTRING(STR_YES), GAMESTRING(STR_NO)))
	{
	case 3:
		saveInfo.saveStage = SAVEMENU_SAVE;
		StartChooseOption();
		break;
	case 2:
		saveInfo.saveStage = SAVEMENU_CHANGED;
		StartChooseOption();
		break;
	}
	switch(gameSaveGetCardStatus())
	{
   	case McErrCardNotExist:
   	case McErrCardInvalid:
		saveInfo.saveStage = SAVEMENU_NOCARD;
		StartChooseOption();
		break;
	}
}

/* FORMAT CONFIRMATION PAGE ********************************************************************************************/

static void saveMenuFormatYN()
{
	switch(ChooseOption(GAMESTRING(STR_MCARD_UNFORMAT), GAMESTRING(STR_MCARD_FORMATCARD), GAMESTRING(STR_MCARD_DONTFORMAT)))
	{
	case 3:
		saveInfo.saveStage = SAVEMENU_FORMAT;
		StartChooseOption();
		break;
	case 2:
		saveInfo.saveStage = SAVEMENU_NEEDFORMAT;
		StartChooseOption();
		break;
	}
	switch(gameSaveGetCardStatus())
	{
   	case McErrCardNotExist:
   	case McErrCardInvalid:
		saveInfo.saveStage = SAVEMENU_NOCARD;
		StartChooseOption();
		break;
	}
}


static void saveMenuChanged()
{
	switch(ChooseOption(GAMESTRING(STR_MCARD_CHANGED), GAMESTRING(STR_MCARD_CHECK_THIS_CARD), GAMESTRING(STR_MCARD_PROCEED_WITHOUT_SAVE)))
	{
	case 3:
		saveInfo.saveStage = SAVEMENU_CHECK;
		StartChooseOption();
		break;
	case 2:
		saveInfo.saveFrame = 0;
		useMemCard = 0;
		break;
	}
}


static void saveMenuSaveYN()
{
	switch(ChooseOption(GAMESTRING(STR_MCARD_NEEDS_ONE_BLOCK), GAMESTRING(STR_MCARD_CREATE_SAVE), GAMESTRING(STR_MCARD_PROCEED_WITHOUT_SAVE)))
	{
	case 3:
		saveInfo.saveStage = SAVEMENU_SAVE;
		StartChooseOption();
		break;
	case 2:
		saveInfo.saveFrame = 0;
		useMemCard = 0;
		break;
	}
	switch(gameSaveGetCardStatus())
	{
   	case McErrCardNotExist:
   	case McErrCardInvalid:
		saveInfo.saveStage = SAVEMENU_NOCARD;
		StartChooseOption();
		break;
	}
}

/* FORMAT PAGE *********************************************************************************************************/

static void saveMenuFormat()
{
	SimpleMessage(GAMESTRING(STR_MCARD_FORMATTING), MIN(255,optFrame*60));
	if (optFrame++>4)
	{
		if (cardFormat())
		{
			saveInfo.saveStage = SAVEMENU_FORMATFAIL;
			StartChooseOption();
		}
		else
		{
			saveInfo.saveStage = SAVEMENU_FORMATCOMPLETE;
			delayTimer = 0;
			StartChooseOption();
		}
	}
}

/* FORMAT FAILURE PAGE *************************************************************************************************/

static void saveMenuFormatFail()
{
	if (ChooseOption(GAMESTRING(STR_MCARD_FORMATERROR), GAMESTRING(STR_MCARD_RECHECK), NULL))
	{
		saveInfo.saveStage = SAVEMENU_CHECK;
		StartChooseOption();
	}
/*
	switch(gameSaveGetCardStatus())
	{
   	case McErrCardNotExist:
   	case McErrCardInvalid:
		saveInfo.saveStage = SAVEMENU_NOCARD;
		StartChooseOption();
		break;
	}
*/
}


void saveMenuNeedFormat()
{
	switch(ChooseOption(GAMESTRING(STR_MCARD_NEEDFORMAT), GAMESTRING(STR_MCARD_RECHECK), GAMESTRING(STR_MCARD_PROCEED_WITHOUT_SAVE)))
	{
	case 3:
		saveInfo.saveStage = SAVEMENU_CHECK;
		StartChooseOption();
		break;
	case 2:
		saveInfo.saveFrame = 0;
		useMemCard = 0;
		break;
	}
	switch(gameSaveGetCardStatus())
	{
   	case McErrCardNotExist:
   	case McErrCardInvalid:
		saveInfo.saveStage = SAVEMENU_NOCARD;
		StartChooseOption();
		break;
	}
}

void ChooseLoadSave()
{
	int alpha = fontSmall->alpha;


	if(saveInfo.load == 0)
	{
		switch(gameSaveGetCardStatus())
		{
   		case McErrNewCard:
			if(cardChanged == NO)
			{
				saveInfo.saveStage = SAVEMENU_CHANGED;
				StartChooseOption();
				cardChanged = YES;
			}
			break;
		}
		VSync(0);
	}

	fontSmall->alpha = 2;
	switch(saveInfo.saveStage)
	{
	case SAVEMENU_CHECK:
		saveMenuCheck();
		break;
	case SAVEMENU_FULL:
		saveMenuFull();
		break;
	case SAVEMENU_NOCARD:
		saveMenuNoCard();
		break;
	case SAVEMENU_SAVE:
		saveMenuSave();
		break;
	case SAVEMENU_COMPLETE:
		saveMenuComplete();
		break;
	case SAVEMENU_FAIL:
		saveMenuFail();
		break;
	case SAVEMENU_FORMATYN:
		saveMenuFormatYN();
		break;
	case SAVEMENU_FORMAT:
		saveMenuFormat();
		break;
	case SAVEMENU_FORMATFAIL:
		saveMenuFormatFail();
		break;
	case SAVEMENU_NEEDFORMAT:
		saveMenuNeedFormat();
		break;
	case SAVEMENU_LOAD:
		saveMenuLoad();
		break;
	case SAVEMENU_LOADERROR:
		saveMenuLoadError();
		break;
	case SAVEMENU_LOADCOMPLETE:
		saveMenuLoadComplete();
		break;
	case SAVEMENU_SAVEYN:
		saveMenuSaveYN();
		break;
	case SAVEMENU_CHANGED:
		saveMenuChanged();
		break;
	case SAVEMENU_OVERWRITE:
		saveMenuOverwrite();
		break;
	case SAVEMENU_FORMATCOMPLETE:
		saveMenuFormatComplete();
		break;	
	}
	fontSmall->alpha = alpha;
}

void SaveGame(void)
{
	if(useMemCard)
		StartChooseLoadSave(NO);
}

void LoadGame(void)
{
	if(useMemCard)
		StartChooseLoadSave(YES);
}


