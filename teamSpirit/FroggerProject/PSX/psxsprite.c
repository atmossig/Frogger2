#include <isltex.h>

#include <islpad.h>
#include <islutil.h>
#include "Shell.h"
#include "overlays.h"

#include "types2d.h"
#include "psxsprite.h"
#include "sprite.h"
#include "layout.h"
#include "maths.h"



void DrawSprite ( SPRITEOVERLAY *spr )
{
	int			atbdx,atbdy;

	register PACKET*		packet;

	static TextureType		*tPtr;

#define si ((POLY_GT4*)packet)

	atbdx = (spr->xPos/8)-256;
#if PALMODE==1
	atbdy = (spr->yPos/16)-128;
#else
	atbdy = (spr->yPos/17)-120;
#endif

	tPtr = spr->frames[0];

			BEGINPRIM(si, POLY_GT4);
	
			setPolyGT4(si);

			si->x0 = atbdx;
			si->y0 = atbdy;

			si->x1 = atbdx + (spr->width/8);
			si->y1 = atbdy;

			si->x2 = atbdx;
#if PALMODE==1
	si->y2 = atbdy + (spr->height/16);
#else
	si->y2 = atbdy + (spr->height/17);
#endif
			//si->y2 = atbdy + (spr->height);

			si->x3 = atbdx + (spr->width/8);
#if PALMODE==1
	si->y3 = atbdy + (spr->height/16);
#else
	si->y3 = atbdy + (spr->height/17);
#endif

	
			si->r0 = ( spr->r * 128 ) >> 8;
			si->g0 = ( spr->g * 128 ) >> 8;
			si->b0 = ( spr->b * 128 ) >> 8;
	
			si->r1 = ( spr->r * 128 ) >> 8;
			si->g1 = ( spr->g * 128 ) >> 8;
			si->b1 = ( spr->b * 128 ) >> 8;

			si->r2 = ( spr->r * 128 ) >> 8;
			si->g2 = ( spr->g * 128 ) >> 8;
			si->b2 = ( spr->b * 128 ) >> 8;

			si->r3 = ( spr->r * 128 ) >> 8;
			si->g3 = ( spr->g * 128 ) >> 8;
			si->b3 = ( spr->b * 128 ) >> 8;

			si->u0 = tPtr->u0;
			si->v0 = tPtr->v0;
			si->u1 = tPtr->u1;
			si->v1 = tPtr->v1;
			si->u2 = tPtr->u2;
			si->v2 = tPtr->v2;
			si->u3 = tPtr->u3;
			si->v3 = tPtr->v3;
		
			si->code = GPU_COM_TG4;
			si->tpage = tPtr->tpage;

			if ( spr->flags & SPRITE_ADDITIVE )
			{
				si->code  |= 2;
 				si->tpage |= 32;
			}
			else if ( spr->flags & SPRITE_SUBTRACTIVE )
			{
				si->code  |= 2;
 				si->tpage = tPtr->tpage | 64;
			}
			// ENDELSEIF

			si->clut = tPtr->clut;
		
			ENDPRIM(si, 1, POLY_GT4);


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


		// update the sprite animation if an animated sprite overlay
		if(cur->draw)
		{
			// Go to destination, if specified
			fixed spd = FMul(gameSpeed, cur->speed)>>12;

			if( Fabs(cur->xPosTo-cur->xPos) )
			{
				cur->xPos += (cur->xPosTo > cur->xPos)?spd:-spd;
				
				if( Fabs(cur->xPosTo-cur->xPos) < Fabs(spd) )
				{
					cur->speed = 0;
					cur->xPos = cur->xPosTo;
				}
			}
			else if( Fabs(cur->yPosTo-cur->yPos) )
			{
				cur->yPos += (cur->yPosTo > cur->yPos)?spd:-spd;

				if( Fabs(cur->yPosTo-cur->yPos) < Fabs(spd) )
				{
					cur->speed = 0;
					cur->yPos = cur->yPosTo;
				}
			}
		
			DrawSprite ( cur );
		}
		// ENDIF

		cur = cur->next;
	}
	// ENDWHILE
}
