#include "tongue.h"

#include "psxtongue.h"


void DrawTongue ( TONGUE *t )
{
	unsigned long i=0;
	unsigned long index = ( t->progress * ( MAX_TONGUENODES - 1 ) >> 12 );
	TextureType *tEntry;

	FMA_GT4 vT[4], vTPrev[2];

	tEntry = ( ( TextureType *) t->tex );

	if( !tEntry || index < 2 )
		return;

		//********-[ First 2 points ]-*******
//		if( i && vTPrev[0].vz && vTPrev[1].vz )
//			memcpy( vT, vTPrev, sizeof(VERT)*2 );			// Previously transformed vertices
//		else
//			CalcTongueNodes( vT, t, i );

		//********-[ Next 2 points ]-********
//		CalcTongueNodes( &vT[2], t, i+1 );
//		memcpy( vTPrev, &vT[2], sizeof(VERT)*2 ); 			// Store first 2 vertices of the next segment

		vT[0].x0 = t->pos.vx; 		
		vT[0].y0 = t->pos.vy; 		

		//********-[ Draw the polys ]-********
		if( vT[0].vz && vT[1].vz && vT[2].vz && vT[3].vz )
		{
			DrawTongueSegment ( vt, tEntry );
//			DrawTongueSegment();
//			Clip3DPolygon( vT, tEntry );
//			Clip3DPolygon( &vT[1], tEntry );
		}
		// ENDIF

		i++;


	vT[0].tu = 1;
	vT[0].tv = 1;
	vT[1].tu = 0;
	vT[1].tv = 1;
	vT[2].tu = 0;
	vT[2].tv = 0;
	vT[3].tu = 1;
	vT[3].tv = 0;

	while( i < index )
	{
		//********-[ First 2 points ]-*******
		if( i && vTPrev[0].sz && vTPrev[1].sz )
			memcpy( vT, vTPrev, sizeof(VERTC)*2 );			// Previously transformed vertices
		else
			CalcTongueNodes( vT, t, i );
		// ENDELSEIF

		//********-[ Next 2 points ]-********
		CalcTongueNodes( &vT[2], t, i+1 );
		memcpy( vTPrev, &vT[2], sizeof(VERTC)*2 ); 			// Store first 2 vertices of the next segment

		//********-[ Draw the polys ]-********
		if( vT[0].sz && vT[1].sz && vT[2].sz && vT[3].sz )
		{
			Clip3DPolygon( vT, tEntry );
			Clip3DPolygon( &vT[1], tEntry );
		}
		// ENDIF

		i++;
	}
	// ENDWHILE
}



