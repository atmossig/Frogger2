/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: video.h
	Programmer	: David Swift
	Date		: 28 May 00, a bloody sunday no less

----------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <d3d.h>
#include <mdx.h>
#include <islutil.h>

#include "video.h"
#include "islpad.h"
#include "controll.h"
#include "pcaudio.h"
#include "dx_sound.h"

#include "game.h"

#ifdef USE_BINK_VIDEO
#include <bink.h>

static HBINK bink=0;
static s32 surfacetype=0;

long (*oldLoop)() = 0;

int RunVideoPlayback()
{
	if (!BinkWait(bink))
	{
		// decompress a frame
		BinkDoFrame(bink);

		DDSURFACEDESC2 ddsd;

		if (surface[PRIMARY_SRF]->Lock(NULL, &ddsd, DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR, 0) != DD_OK)
		{
			return 0;
		}

		// copy the data onto the screen
		BinkCopyToBuffer(bink, ddsd.lpSurface, ddsd.lPitch, bink->Height,0,0,surfacetype);

		surface[PRIMARY_SRF]->Unlock(0); //ddsd.lpSurface);

		if (bink->FrameNum = bink->Frames || padData.debounce[0])
		{
			// end playback mode..
			BinkClose(bink); bink=NULL;

			AppLoop = oldLoop;

		}
		else
			BinkNextFrame(bink);
	}


	return 1;
}


int StartVideoPlayback(const char* name)
{
	BinkSoundUseDirectSound(lpDS);

	bink = BinkOpen(name,0);
	if (!bink)
	{
		utilPrintf("StartVideoPlayback(): Bink failed opening '%s'\n", name);
		gameState.mode = INGAME_MODE;	// skip right out
		return 0;
	}

	surfacetype = BinkDDSurfaceType(surface[PRIMARY_SRF]);

	oldLoop = AppLoop;

	return 1;
}

/* ----------------------------------------------------------------------- */

#else // if we're compiling without video support..

int RunVideoPlayback()
{
	return 1;
}

int StartVideoPlayback(const char* name)
{
	gameState.mode = INGAME_MODE;	// skip right out
	return 0;
}

#endif // INCLUDE_VIDEO