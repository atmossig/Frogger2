/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: incs.h
	Programmer	: Andy Eder
	Date		: 29/04/99 09:19:40

----------------------------------------------------------------------------------------------- */


//----- [ INCLUDE FILES FOR FROGGER 2 ] -----------------------------------------------------//

#ifdef PC_VERSION
#include <ddraw.h>
#include <d3d.h>
#include <dinput.h>

#ifndef __cplusplus

#include <math.h>
#endif

#include "pcmisc.h"
#include "directx.h"

#endif

#ifndef PC_VERSION
#define F3DEX_GBI_2
#define NNSCHED
#include <ultra64.h>

#include <PR/ramrom.h>	// needed for argument passing into the app
#include <assert.h>
#include <pr/os.h>
#include <pr/gu.h>
#include <pr/gs2dex.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdarg.h>
#endif

#include "types.h"
#include "define.h"
#include "maths.h"
#include "font.h"
#include "types2dgfx.h"
#include "general.h"

#ifndef PC_VERSION
#include "nnsched.h"
#endif

#include "title.h"
#include "actor.h"
#include "layoutvisual.h"

#ifndef PC_VERSION
#include "timerbar.h"
#endif

#include "levelplaying.h"
#include "jalloc.h"
#include "sprite.h"
#include "backdrops.h"
#include "overlays.h"
#include "textoverlays.h"
#include "frogger.h"
#include "hud.h"
#include "babyfrogs.h"
#include "printgfx.h"
#include "specfx.h"
#include "anim.h"
#include "path.h"
#include "enemies.h"
#include "platform.h"
#include "cam.h"
#include "game.h"
#include "objects.h"
#include "texture.h"
#include "collect.h"
#include "path.h"
#include "platform.h"
#include "frogmove.h"
#include "cam.h"
#include "tongue.h"
//#include "event.h"
//#include "eventfuncs.h"

#ifndef PC_VERSION
#include "develop.h"
#endif

#include "frontend.h"
#include "sndview.h"
#include "map.h"
#include "controll.h"

#include "subcube.h"
#include "block.h"
#include "cameos.h"
#include "savegame.h"
#include "menus.h"
#include "defaultdata.h"
#include "levelprogession.h"

#ifndef PC_VERSION
#include "libmus.h"
#include "libmus_data.h"
#include "audio.h"
#endif

#include "sndview.h"

#ifndef PC_VERSION
#include "sfx.h"
#include "mbuffer.h"
#endif

#ifndef PC_VERSION
#include "levbanks\levext.h"
#else
#include "\work\froggerII\pc\levbanks\levext.h"
#endif
