/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: incs.h
	Programmer	: Andy Eder
	Date		: 29/04/99 09:19:40

----------------------------------------------------------------------------------------------- */


//----- [ INCLUDE FILES FOR FROGGER 2 ] -----------------------------------------------------//


#include "types.h"
#include "define.h"
#include "maths.h"
#include "types2dgfx.h"
#include "general.h"

#ifndef PC_VERSION
#include "nnsched.h"
#endif


#include "title.h"


#ifndef PC_VERSION
#include "actor.h"
#endif


#include "layoutvisual.h"

#ifndef PC_VERSION
#include "timerbar.h"
#endif


#include "levelplaying.h"

#include "jalloc.h"

#ifndef PC_VERSION
#include "sprite.h"
#endif

#include "backdrops.h"
#include "overlays.h"
#include "textoverlays.h"
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
#include "frogger.h"
#include "frogmove.h"
#include "cam.h"
#include "tongue.h"
#include "develop.h"
#include "singlemodetitle.h"
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

#include "libmus.h"
#include "libmus_data.h"
#include "audio.h"
#include "sndview.h"
#include "sfx.h"
#include "mbuffer.h"

#include "levbanks\levext.h"