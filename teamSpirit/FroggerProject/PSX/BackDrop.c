#include <islmem.h>
#include <islfile.h>

#include "Shell.h"
#include "SonyLibs.h"

#include "Frontend.h"

#include "BackDrop.h"

BACKDROP backDrop;

extern USHORT EXPLORE_black_CLUT;

void InitBackdrop ( char * filename )
{
	if (backDrop.init)
		return;

	backDrop.init = 1;
	backDrop.rect.x = 0;			// leave the y till the update
	backDrop.rect.y = 0;
	backDrop.rect.w = 512; 
	backDrop.rect.h = 256;

	DrawSync(0);
	ClearImage2(&backDrop.rect, 0,0,0);
	DrawSync(0);
	backDrop.rect.y = 256;
	DrawSync(0);
	ClearImage2(&backDrop.rect, 0,0,0);
	DrawSync(0);

	backDrop.rect.y = 0;

	strcat ( filename, ".RAW" );
	backDrop.data = fileLoad(filename, NULL);


	// set the drawing clscols to NOT clearscreen
//	INTERUPT_ScreenClear(INTERUPT_CLEAR_OFF);
}

#define ScreenGetBuffer()			( (currentDisplayPage==displayPage)?0:1 )

void DrawBackDrop ( void )
{
//	register PACKET			*packet;//, *packetNext;

	int fogFade;

	fogFade = 100;

	if ( ( backDrop.init ) && ( backDrop.data ) )
	{

//	rect.x = 0;
//	rect.y = 0;//(TITAN_SCREEN_H-OVERLAY_BACKDROP_H)/2;		// Center on screen
//	rect.w = 512; 
//	rect.h = 256;

	if( !ScreenGetBuffer() )
	{
		backDrop.rect.y = 0;
		DrawSync(0);
		LoadImage ( &backDrop.rect, (ULONG *) backDrop.data );
	}
	else
	{
		backDrop.rect.y+=256;
		DrawSync(0);
		LoadImage ( &backDrop.rect, (ULONG *) backDrop.data );
	}

	DrawSync(0);

	}

/*


				backDrop.rect.y = 256;
				DrawSync(0);
				LoadImage(&backDrop.rect, (ULONG *)backDrop.data);
				DrawSync(0);
				backDrop.rect.y = 0;
				DrawSync(0);
				LoadImage(&backDrop.rect, (ULONG *)backDrop.data);
				DrawSync(0);*/
			//VSync(2);

/*				if ( frameCount > 50 )
				{
				}*/
				// ENDIF


/*#define si ((POLY_G4*)packet)
#define siNext ((POLY_G4*)packetNext)

//				BEGINPRIM(si, POLY_G4);

/*				gte_ldsxy3(tfv[op->v0], tfv[op->v1], tfv[op->v2]);		// Load 1st three vertices
				
				*(u_long *)  (&si->u0) = *(u_long *) (&op->tu0);		// Texture coords
				*(u_long *)  (&si->u1) = *(u_long *) (&op->tu1);
					
				gte_stsxy3_gt4(si);
				
						
				*(u_long *)  (&si->u2) = *(u_long *) (&op->tu2);
				*(u_long *)  (&si->u3) = *(u_long *) (&op->tu3);

				*(u_long *)  (&si->x3) = *(u_long *) (&tfv[op->v3]);
		
				switch (lightmode)
				{
					case DIRECTIONAL:
						gte_ldrgb3(&op->r0, &op->r1, &op->r2);
						gte_ldv3(&tfn[op->v0], &tfn[op->v1], &tfn[op->v2]);
						gte_ncct();
						gte_strgb3(&si->r0, &si->r1, &si->r2);

						gte_ldrgb(&op->r3);
						gte_ldv0(&tfn[op->v3]);
						gte_nccs();			// NormalColorCol
						gte_strgb(&si->r3);
						break;
					case DIRECTIONONLY:
						gte_ldv3(&tfn[op->v0], &tfn[op->v1], &tfn[op->v2]);
						gte_nct();			
						gte_strgb3(&si->r0, &si->r1, &si->r2);

						gte_ldv0(&tfn[op->v3]);
						gte_ncs();			// NormalColorCol
						gte_strgb(&si->r3);
						break;
					case COLOURIZE:
						*(u_long *)  (&si->r0) = *(u_long *) (&modctrl->col);
						*(u_long *)  (&si->r1) = *(u_long *) (&modctrl->col);
						*(u_long *)  (&si->r2) = *(u_long *) (&modctrl->col);
						*(u_long *)  (&si->r3) = *(u_long *) (&modctrl->col);
						break;
					default:
						*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
						*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
						*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
						*(u_long *)  (&si->r3) = *(u_long *) (&op->r3);
				}*/
		
// FOG CODE: --------------------------------------------------------
				
				// SL: if it's near enough to not need fogging, or it's already additive or subtractive...
//				if ( (fogFade >= 255))
//				{
					// SL: put in the additive poly...
//					setPolyGT4(si);
//					si->code = op->cd | modctrl->semitrans;
//					ENDPRIM(si, depth, POLY_GT4);
//				}
//				else
					// SL: modge the RGBs accordingly
			/*		si->r0 = ((fogFade*(short)si->r0)/255);
					si->g0 = ((fogFade*(short)si->g0)/255);
					si->b0 = ((fogFade*(short)si->b0)/255);
					si->r1 = ((fogFade*(short)si->r1)/255);
					si->g1 = ((fogFade*(short)si->g1)/255);
					si->b1 = ((fogFade*(short)si->b1)/255);
					si->r2 = ((fogFade*(short)si->r2)/255);
					si->g2 = ((fogFade*(short)si->g2)/255);
					si->b2 = ((fogFade*(short)si->b2)/255);
					si->r3 = ((fogFade*(short)si->r3)/255);
					si->g3 = ((fogFade*(short)si->g3)/255);
					si->b3 = ((fogFade*(short)si->b3)/255);

					// SL: put in the additive poly...
					setPolyGT4(si);
					// SL: store the pre-modification tpage...
					oldtpage = si->tpage;
					// make it additive...
					si->code |= 2;
					si->tpage |= 32;
					// SL: put it in t' table
					ENDPRIM(si, depth, POLY_GT4);
*/
					// SL: hokay, setup the next poly...
	/*				BEGINPRIM(siNext, POLY_G4);

					// SL: copy in all the values using long copies...
					siNext->x0 = 0;
					siNext->y0 = 0;

					siNext->x1 = 100;
					siNext->y1 = 0;

					siNext->x2 = 0;
					siNext->y2 = 100;

					siNext->x3 = 100;
					siNext->y3 = 100;

					siNext->r0 = 100;
					siNext->g0 = 100;
					siNext->b0 = 100;

					siNext->r1 = 100;
					siNext->g1 = 100;
					siNext->b1 = 100;

					siNext->r2 = 100;
					siNext->g2 = 100;
					siNext->b2 = 100;

					siNext->r3 = 100;
					siNext->g3 = 100;
					siNext->b3 = 100;

					// SL: set it as the right type
					setPolyG4(siNext);
					// SL: make it subtractive
//					siNext->clut = EXPLORE_black_CLUT;
					//siNext->code |= 2;
//					siNext->tpage = si->tpage | 64;

					// SL: put it in t' table
					ENDPRIM(siNext, 1, POLY_G4);
// END FOG CODE: --------------------------------------------------------

#undef si
#undef siNext*/


//	}
	// ENDIF
}

void FreeBackdrop ( void )
{
	backDrop.init = 0;

	backDrop.rect.y = 0;
	DrawSync(0);
	ClearImage2(&backDrop.rect, 0,0,0);
	DrawSync(0);
	backDrop.rect.y = 256;
	DrawSync(0);
	ClearImage2(&backDrop.rect, 0,0,0);
	DrawSync(0);

	backDrop.rect.y = 0;

	FREE(backDrop.data);

}