#include <isltex.h>

#include <islpad.h>
#include <islutil.h>
#include "Shell.h"
#include "overlays.h"

#include "types2d.h"
#include "psxsprite.h"



void DrawSprite ( SPRITEOVERLAY *spr )
{
	int			atbdx,atbdy;

	register PACKET*		packet;

	static TextureType		*tPtr;

#define si ((POLY_FT4*)packet)

	atbdx = (spr->xPos/8)-256;
#ifdef PALMODE==1
	atbdy = (spr->yPos/16)-120;
#else
	atbdy = (spr->yPos/17)-128;
#endif

	tPtr = spr->frames[0];

			BEGINPRIM(si, POLY_FT4);
	
			si->x0 = atbdx;
			si->y0 = atbdy;

			si->x1 = atbdx + (spr->width/8);
			si->y1 = atbdy;

			si->x2 = atbdx;
#ifdef PALMODE==1
	si->y2 = atbdy + (spr->height/16);
#else
	si->y2 = atbdy + (spr->height/17);
#endif
			//si->y2 = atbdy + (spr->height);

			si->x3 = atbdx + (spr->width/8);
#ifdef PALMODE==1
	si->y3 = atbdy + (spr->height/16);
#else
	si->y3 = atbdy + (spr->height/17);
#endif

	
			si->r0 = spr->r;
			si->g0 = spr->g;
			si->b0 = spr->b;
	
			si->u0 = tPtr->u0;
			si->v0 = tPtr->v0;
			si->u1 = tPtr->u1;
			si->v1 = tPtr->v1;
			si->u2 = tPtr->u2;
			si->v2 = tPtr->v2;
			si->u3 = tPtr->u3;
			si->v3 = tPtr->v3;
		
			si->code = GPU_COM_TF4;
			si->tpage = tPtr->tpage;
			si->clut = tPtr->clut;
		
			setPolyFT4(si);
			ENDPRIM(si, 1, POLY_FT4);


#undef si
}


void PrintSpriteOverlays ( char num )
{
	SPRITEOVERLAY *cur;

	cur = spriteOverlayList.head.next;

	while ( cur != &spriteOverlayList.head )
	{
		if ( cur->num != num )
		{
			cur = cur->next;
			continue;
		}
		// ENDIF

		if ( cur->draw )
		{
			DrawSprite ( cur );
		}
		// ENDIF

		cur = cur->next;
	}
	// ENDWHILE
}
