#include "sonylibs.h"
#include "Shell.h"

#include "loadingbar.h"

int barProgress = 0;

int barLength = 400;

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

	utilPrintf("Drawing Loadingbar...............................................\n");

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

