#include "sonylibs.h"
#include "Shell.h"

#include "layout.h"
#include "main.h"
#include "lang.h"
#include "textover.h"
#include "frogger.h"
#include "loadingbar.h"

int barProgress = 0;

int barLength = 400;

LOADINGSCREEN loadingScreen;

TEXTOVERLAY *worldName;
TEXTOVERLAY *levelName;
TEXTOVERLAY *parTimeText;
TEXTOVERLAY *parNameText;
TEXTOVERLAY *coinsText;

char temp[256];
char tempCoins[128];
short loadingDisplay = 0;

void loadingInit ( int worldID, int levelID )
{
	int i, c;
	RECT rect;

	loadingDisplay = 1;

	gte_SetRotMatrix(&GsWSMATRIX);
	gte_SetTransMatrix(&GsWSMATRIX);

	if ( worldID == 8 )
		return;

	worldName = CreateAndAddTextOverlay( 2048 + 4096, 200, GAMESTRING(worldVisualData[worldID].description_str), YES, 255, font, 0 ); 
	worldName->r = 0;
	worldName->b = 0;
	worldName->xPosTo = 2048;
	worldName->speed = 4096*20;

	levelName = CreateAndAddTextOverlay( 2048 + 4096, 500, GAMESTRING(worldVisualData[worldID].levelVisualData[levelID].description_str), YES, 255, font, 0 ); 
	levelName->r = 0;
	levelName->b = 0;
	levelName->xPosTo = 2048;
	levelName->speed = 4096*20;

	sprintf(temp,GAMESTRING(STR_RECORD),worldVisualData[worldID].levelVisualData[levelID].parName,((int)worldVisualData[worldID].levelVisualData[levelID].parTime/60)%60,((int)worldVisualData[worldID].levelVisualData[levelID].parTime)%60);
	parTimeText = CreateAndAddTextOverlay ( 2048 + 4096, 1400, temp, YES, 255, font, 0 );
	parTimeText->xPosTo = 2048;
	parTimeText->speed = 4096 * 20;

	sprintf ( tempCoins, "NUM COINS : %d", worldVisualData[worldID].levelVisualData[levelID].maxCoins );
	coinsText = CreateAndAddTextOverlay ( 2048 + 4096, 1800, tempCoins, YES, 255, font, 0 );
	coinsText->xPosTo = 2048;
	coinsText->speed = 4096*20;

	/*rect.x = 1024-64;
	rect.y = 260;
	rect.w = 32;
	rect.h = 64;

	DrawSync(0);
	LoadImage (&rect, (unsigned long*)waterimage );

	rect.x = 1024-64;
	rect.y = 258;
	rect.w = 16;
	rect.h = 1;

	DrawSync(0);
	LoadImage (&rect, (unsigned long*)waterpal );

	loadingScreen.numWaterTilesX = NUM_WATER_TILESX;
	loadingScreen.numWaterTilesY = NUM_WATER_TILESY;*/

	/*for ( i = 0; i < loadingScreen.numWaterTilesX; i++ )
	{
		for ( c = 0; c < loadingScreen.numWaterTilesY; c++ )
		{
			loadingScreen.waterPolys[i+(c*1)].r0 = 0;
			loadingScreen.waterPolys[i+(c*1)].g0 = 0;
			loadingScreen.waterPolys[i+(c*1)].b0 = 0;

			loadingScreen.waterPolys[i+(c*1)].r1 = 0;
			loadingScreen.waterPolys[i+(c*1)].g1 = 0;
			loadingScreen.waterPolys[i+(c*1)].b1 = 0;

			loadingScreen.waterPolys[i+(c*1)].r2 = 0;
			loadingScreen.waterPolys[i+(c*1)].g2 = 0;
			loadingScreen.waterPolys[i+(c*1)].b2 = 0;

			loadingScreen.waterPolys[i+(c*1)].r3 = 0;
			loadingScreen.waterPolys[i+(c*1)].g3 = 0;
			loadingScreen.waterPolys[i+(c*1)].b3 = 100;

			loadingScreen.waterPolys[i+(c*1)].x0 = -256;
			loadingScreen.waterPolys[i+(c*1)].y0 = -120;

			loadingScreen.waterPolys[i+(c*1)].x1 = 256;
			loadingScreen.waterPolys[i+(c*1)].y1 = -120;

			loadingScreen.waterPolys[i+(c*1)].x2 = -256;
			loadingScreen.waterPolys[i+(c*1)].y2 = 120;

			loadingScreen.waterPolys[i+(c*1)].x3 = 256;
			loadingScreen.waterPolys[i+(c*1)].y3 = 120;

	rect.x = 1024-64;
	rect.y = 260;
	rect.w = 32;
	rect.h = 64;
			loadingScreen.waterPolys[i+(c*1)].tpage = getTPage ( 0,0, rect.x, rect.y );

	rect.x = 1024-64;
	rect.y = 258;
	rect.w = 16;
	rect.h = 1;

	loadingScreen.waterPolys[i+(c*1)].clut	= getClut(0,0);

			loadingScreen.waterPolys[i+(c*1)].code	= 0;

			loadingScreen.waterPolys[i+(c*1)].u0 = 1024-64;
			loadingScreen.waterPolys[i+(c*1)].v0 = 258;
			loadingScreen.waterPolys[i+(c*1)].u1 = 1024;
			loadingScreen.waterPolys[i+(c*1)].v1 = 258;

			loadingScreen.waterPolys[i+(c*1)].u2 = 1024-64;
			loadingScreen.waterPolys[i+(c*1)].v2 = 258+32;
			loadingScreen.waterPolys[i+(c*1)].u3 = 1024;
			loadingScreen.waterPolys[i+(c*1)].v3 = 258+32;
		}
		// ENDIF
	}
	// ENDIF*/
}

void loadingFree ( void )
{
	loadingDisplay		= 0;
	worldName->draw		= 0;
	levelName->draw		= 0;
	parTimeText->draw = 0;
	coinsText->draw		= 0;
}

void loadingWaterFrame ( void )
{
	int i, c;

	PrintTextOverlays();
		
	/*register PACKET*		packet;

	for ( i = 0; i <loadingScreen.numWaterTilesX; i++ )
	{
		for ( c = 0; c <loadingScreen.numWaterTilesY; c++ )
		{
#define si ((POLY_GT4*)packet)
	BEGINPRIM(si, POLY_GT4);

	setPolyGT4(si);

	
	si->r0 = loadingScreen.waterPolys[i+(c*1)].r0;
	si->g0 = loadingScreen.waterPolys[i+(c*1)].g0;
	si->b0 = loadingScreen.waterPolys[i+(c*1)].b0;
	si->r1 = loadingScreen.waterPolys[i+(c*1)].r1;
	si->g1 = loadingScreen.waterPolys[i+(c*1)].g1;
	si->b1 = loadingScreen.waterPolys[i+(c*1)].b1;
	si->r2 = loadingScreen.waterPolys[i+(c*1)].r2;
	si->g2 = loadingScreen.waterPolys[i+(c*1)].g2;
	si->b2 = loadingScreen.waterPolys[i+(c*1)].b2;
	si->r3 = loadingScreen.waterPolys[i+(c*1)].r3;
	si->g3 = loadingScreen.waterPolys[i+(c*1)].g3;
	si->b3 = loadingScreen.waterPolys[i+(c*1)].b3;

	si->x0 = loadingScreen.waterPolys[i+(c*1)].x0;
	si->y0 = loadingScreen.waterPolys[i+(c*1)].y0;
	si->x1 = loadingScreen.waterPolys[i+(c*1)].x1;
	si->y1 = loadingScreen.waterPolys[i+(c*1)].y1;
	si->x2 = loadingScreen.waterPolys[i+(c*1)].x2;
	si->y2 = loadingScreen.waterPolys[i+(c*1)].y2;
	si->x3 = loadingScreen.waterPolys[i+(c*1)].x3;
	si->y3 = loadingScreen.waterPolys[i+(c*1)].y3;

	si->u0 = loadingScreen.waterPolys[i+(c*1)].u0;
	si->v0 = loadingScreen.waterPolys[i+(c*1)].v0;
	si->u1 = loadingScreen.waterPolys[i+(c*1)].u1;
	si->v1 = loadingScreen.waterPolys[i+(c*1)].v1;
	si->u2 = loadingScreen.waterPolys[i+(c*1)].u2;
	si->v2 = loadingScreen.waterPolys[i+(c*1)].v2;
	si->u3 = loadingScreen.waterPolys[i+(c*1)].u3;
	si->v3 = loadingScreen.waterPolys[i+(c*1)].v3;

	si->code = GPU_COM_TG4;

	si->clut	= loadingScreen.waterPolys[i+(c*1)].clut;
	si->tpage = loadingScreen.waterPolys[i+(c*1)].tpage;


	ENDPRIM(si, 100, POLY_GT4);

#undef si
		}
		// ENDFOR
	}
	// ENDFOR*/
}

void loadingDisplayFrame ( void )
{
	if ( player[0].worldNum == 8 )
		return;

	loadingWaterFrame();
}

void StepProgressBar ( int percentAmount )
{
	// JH: Add the required amount onto the bar progress....
	barProgress += percentAmount;

	// JH: Just for safety, let's make sure we can't go above 100%....
	if ( barProgress > 100 )
		barProgress = 100;
	// ENDIF
}

void SetProgressBar ( int percentage )
{
	// JH: Equal the required amount into the bar progress....
	barProgress = percentage;

	// JH: Just for safety, let's make sure some fool hasn't enter a wrong value....
	if ( barProgress > 100 )
		barProgress = 100;

	if ( barProgress < 0 )
		barProgress = 0;
}

void DrawLoadingBar ( int execute )
{
	register PACKET*		packet;

#define si ((POLY_G4*)packet)

	if ( execute )
	{
		currentDisplayPage = (currentDisplayPage==displayPage)?(&displayPage[0]):(&displayPage[1]);
		ClearOTagR(currentDisplayPage->ot, 1024);
		currentDisplayPage->primPtr = currentDisplayPage->primBuffer;

//		DrawSync(0);
// 		VSync(2);
 		/*PutDispEnv(&currentDisplayPage->dispenv);
 		PutDrawEnv(&currentDisplayPage->drawenv);
 		DrawOTag(currentDisplayPage->ot+(1024-1));*/
	}
	// ENDIF

	BEGINPRIM(si, POLY_G4);

	setPolyG4(si);

	si->r0 = 255;
	si->g0 = 0;
	si->b0 = 0;

	si->r1 = 0;
	si->g1 = 0;
	si->b1 = 255;

	si->r2 = 255;
	si->g2 = 0;
	si->b2 = 0;

	si->r3 = 0;
	si->g3 = 0;
	si->b3 = 255;


	si->x0 = -200;
	si->y0 = 100;

	si->x2 = -200;
	si->y2 = 105;

	si->x1 = -200 + ((barLength * barProgress) / 100 );
	si->y1 = 100;

	si->x3 = -200 + ((barLength * barProgress) / 100 );
	si->y3 = 105;

	si->code = GPU_COM_G4;

	ENDPRIM(si, 1, POLY_G4);


	BEGINPRIM(si, POLY_G4);

	setPolyG4(si);

	si->r0 = 0;
	si->g0 = 0;
	si->b0 = 0;

	si->r1 = 0;
	si->g1 = 255;
	si->b1 = 0;

	si->r2 = 0;
	si->g2 = 0;
	si->b2 = 0;

	si->r3 = 0;
	si->g3 = 0;
	si->b3 = 0;


	si->x0 = -205;
	si->y0 = 97;

	si->x2 = -205;
	si->y2 = 108;

	si->x1 = -205 + barLength + 12;
	si->y1 = 97;

	si->x3 = -205 + barLength + 12;
	si->y3 = 108;

	si->code = GPU_COM_G4;

	ENDPRIM(si, 10, POLY_G4);


	if ( execute )
	{
/* 			DrawSync(0);
 			currentDisplayPage = (currentDisplayPage==displayPage)?(&displayPage[1]):(&displayPage[0]);
			ClearOTagR(currentDisplayPage->ot, 1024);
 			currentDisplayPage->primPtr = currentDisplayPage->primBuffer;
 			DrawSync(0);

 			DrawSync(0);
 			VSync(2);
 			PutDispEnv(&currentDisplayPage->dispenv);
 			PutDrawEnv(&currentDisplayPage->drawenv);
 			DrawOTag(currentDisplayPage->ot+(1024-1));

 			DrawSync(0);*/
	}
	// ENDIF
#undef si
}

