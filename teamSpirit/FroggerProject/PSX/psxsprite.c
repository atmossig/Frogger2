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
	int			atbdx,atbdy, w,h;
	POLY_F4		*f4;
	POLY_FT4	*ft4;
	TextureType	*tPtr;

	atbdx = (spr->xPos/8)-256;
	atbdy = (spr->yPos/(17-PALMODE))-120-PALMODE*8;
	tPtr = spr->frames[0];

	if(spr->frames[0] == 0)
	{
		BEGINPRIM(f4,POLY_F4);
		setPolyF4(f4);

		f4->x0 = atbdx;
		f4->y0 = atbdy;
		f4->x1 = atbdx + (spr->width/8);
		f4->y1 = atbdy;
		f4->x2 = atbdx;
		f4->y2 = atbdy + (spr->height/(17-PALMODE));
		f4->x3 = atbdx + (spr->width/8);
		f4->y3 = atbdy + (spr->height/(17-PALMODE));
		f4->r0 = spr->r;
		f4->g0 = spr->g;
		f4->b0 = spr->b;
		if ( spr->flags & SPRITE_ADDITIVE )
		{
			f4->code  |= 2;
		}
		else if ( spr->flags & SPRITE_SUBTRACTIVE )
		{
			f4->code  |= 2;
		}
		ENDPRIM(f4, 1, POLY_F4);
	}
	else
	{
		w = tPtr->w-1;
		h = tPtr->h-1;

		if (spr->width==4096)
		{	// NEW! 1:1 texels to pixels
			BEGINPRIM(ft4, POLY_FT4);
			setPolyFT4(ft4);
			ft4->x0 = atbdx;
			ft4->y0 = atbdy;
			ft4->x1 = atbdx+w;
			ft4->y1 = atbdy;
			ft4->x2 = atbdx;
			ft4->y2 = atbdy+h;
			ft4->x3 = atbdx+w;
			ft4->y3 = atbdy+h;
			ft4->r0 = ( spr->r * 128 ) >> 8;
			ft4->g0 = ( spr->g * 128 ) >> 8;
			ft4->b0 = ( spr->b * 128 ) >> 8;
			ft4->u0 = tPtr->u0;
			ft4->v0 = tPtr->v0;
			ft4->u1 = tPtr->u1;
			ft4->v1 = tPtr->v1;
			ft4->u2 = tPtr->u2;
			ft4->v2 = tPtr->v2;
			ft4->u3 = tPtr->u3;
			ft4->v3 = tPtr->v3;
			ft4->tpage = tPtr->tpage;
			if ( spr->flags & SPRITE_ADDITIVE )
			{
				ft4->code  |= 2;
 				ft4->tpage |= 32;
			}
			else if ( spr->flags & SPRITE_SUBTRACTIVE )
			{
				ft4->code  |= 2;
				ft4->tpage |= 64;
			}
			ft4->clut = tPtr->clut;
			ENDPRIM(ft4, 1, POLY_FT4);
		}
		else
		{	// Original random scaling method (slightly tidier)
			BEGINPRIM(ft4, POLY_FT4);
			setPolyFT4(ft4);
			ft4->x0 = atbdx;
			ft4->y0 = atbdy;
			ft4->x1 = atbdx + (spr->width/8);
			ft4->y1 = atbdy;
			ft4->x2 = atbdx;
			ft4->y2 = atbdy + (spr->height/(17-PALMODE));
			ft4->x3 = atbdx + (spr->width/8);
			ft4->y3 = atbdy + (spr->height/(17-PALMODE));
			ft4->r0 = ( spr->r * 128 ) >> 8;
			ft4->g0 = ( spr->g * 128 ) >> 8;
			ft4->b0 = ( spr->b * 128 ) >> 8;
			ft4->u0 = tPtr->u0;
			ft4->v0 = tPtr->v0;
			ft4->u1 = tPtr->u1;
			ft4->v1 = tPtr->v1;
			ft4->u2 = tPtr->u2;
			ft4->v2 = tPtr->v2;
			ft4->u3 = tPtr->u3;
			ft4->v3 = tPtr->v3;
			ft4->tpage = tPtr->tpage;
			if ( spr->flags & SPRITE_ADDITIVE )
			{
				ft4->code  |= 2;
				ft4->tpage |= 32;
			}
			else if ( spr->flags & SPRITE_SUBTRACTIVE )
			{
				ft4->code  |= 2;
				ft4->tpage = tPtr->tpage | 64;
			}
			ft4->clut = tPtr->clut;
			ENDPRIM(ft4, 1, POLY_FT4);
		}
	}
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
