
#include "particle.h"
#include "specfx.h"
#include "Eff_Draw.h"
#include "cam.h"
#include "enemies.h"
#include "platform.h"
#include "collect.h"
#include "frogmove.h"
#include "main.h"





void DrawSpecialFX()
{
	if ( ( gameState.mode == INGAME_MODE ) || ( gameState.mode == PAUSE_MODE ) ||
		 ( gameState.mode == CAMEO_MODE ) )
	{
		ProcessShadows();
		
		if( sfxList.count )
		{
			SPECFX *fx;
			for( fx = sfxList.head.next; fx != &sfxList.head; fx = fx->next )
				if( fx->Draw )
					fx->Draw( fx );
		}

// 		for( i=0; i<NUM_FROGS; i++ )
// 			if( tongue[i].flags & TONGUE_BEINGUSED )
// 				DrawTongue( &tongue[i] );
	}
}






void ProcessShadows()
{
	FVECTOR vec;
	ENEMY *nme;
	PLATFORM *plat;
//	GARIB *garib;
	int i;
	long tex;
	fixed height;
	
//	tex = (long)((TextureType *)txtrSolidRing)->handle;
	tex = (long*)txtrSolidRing;

	for( i=0; i<NUM_FROGS; i++ )
		if( frog[i]->actor->shadow && frog[i]->draw && frog[i]->actor->shadow->draw )
		{
			SubVectorFSS( &vec, &frog[i]->actor->position, &currTile[i]->centre );
			height = DotProductFF( &vec, &currTile[i]->normal );
//			DrawShadow( &frog[i]->actor->position, &currTile[i]->normal, FDiv(frog[i]->actor->shadow->radius,max(FMul(height,82),4096 )), -height+4096, FDiv(ToFixed(frog[i]->actor->shadow->alpha),max(FMul(height,82), 4096))>>12, tex );
//bb		DrawShadow( &frog[i]->actor->position, &currTile[i]->normal, FDiv(frog[i]->actor->shadow->radius,max(FMul(height,8),4096 )), -height+4096, FDiv(ToFixed(frog[i]->actor->shadow->alpha),max(FMul(height,8), 4096))>>12, tex );
			DrawShadow( &frog[i]->actor->position, &currTile[i]->normal, FDiv(frog[i]->actor->shadow->radius,max(FMul(height,8),4096 )), -height+4096, FDiv(ToFixed(frog[i]->actor->shadow->alpha),max(FMul(height,8), 4096))>>12, tex );
		}

	//------------------------------------------------------------------------------------------------

	// process enemy shadows
	for(nme = enemyList.head.next; nme != &enemyList.head; nme = nme->next)
	{
		if( !nme->active || !nme->nmeActor )
			continue;

		if(nme->nmeActor->actor->shadow && nme->inTile && nme->nmeActor->distanceFromFrog < ToFixed(BBACTOR_DRAWDISTANCEINNER))
		{
			SubVectorFSS( &vec, &nme->nmeActor->actor->position, &nme->inTile->centre );
			height = DotProductFF( &vec, &nme->inTile->normal );

			if (nme->path->nodes[nme->path->fromNode].worldTile==nme->inTile)
//				DrawShadow( &nme->nmeActor->actor->position, &nme->inTile->normal, FDiv(nme->nmeActor->actor->shadow->radius,max(FMul(height,82*SCALE), 4096)), -height+4096, FDiv(ToFixed(nme->nmeActor->actor->shadow->alpha),max(FMul(height,82), 4096))>>12, tex );
				DrawShadow( &nme->nmeActor->actor->position, &nme->inTile->normal, FDiv(nme->nmeActor->actor->shadow->radius,max(FMul(height,8), 4096)), -height+4096, FDiv(ToFixed(nme->nmeActor->actor->shadow->alpha),max(FMul(height,8), 4096))>>12, tex );
			else
//				DrawShadow( &nme->nmeActor->actor->position, &nme->inTile->normal, FDiv(nme->nmeActor->actor->shadow->radius,max(FMul(height,82*SCALE), 4096)), -height+4096, FDiv(ToFixed(nme->nmeActor->actor->shadow->alpha),max(FMul(height,82), 4096))>>12, tex );
				DrawShadow( &nme->nmeActor->actor->position, &nme->inTile->normal, FDiv(nme->nmeActor->actor->shadow->radius,max(FMul(height,8), 4096)), -height+4096, FDiv(ToFixed(nme->nmeActor->actor->shadow->alpha),max(FMul(height,8), 4096))>>12, tex );
		}
	}

	// process platform shadows
	for(plat = platformList.head.next; plat != &platformList.head; plat = plat->next)
	{
		if( !plat->active || !plat->pltActor )
			continue;

		if(plat->pltActor->actor->shadow && plat->inTile && plat->pltActor->distanceFromFrog < ToFixed(BBACTOR_DRAWDISTANCEINNER))
		{
			SubVectorFSS( &vec, &plat->pltActor->actor->position, &plat->inTile[0]->centre );
			height = DotProductFF( &vec, &plat->inTile[0]->normal );
//			DrawShadow( &plat->pltActor->actor->position, &plat->inTile[0]->normal, FDiv(plat->pltActor->actor->shadow->radius*SCALE,max(FMul(height,82), 4096)), -height+4096, FDiv(ToFixed(plat->pltActor->actor->shadow->alpha),max(FMul(height,82), 4096))>>12, tex );
			DrawShadow( &plat->pltActor->actor->position, &plat->inTile[0]->normal, FDiv(plat->pltActor->actor->shadow->radius*SCALE,max(FMul(height,8), 4096)), -height+4096, FDiv(ToFixed(plat->pltActor->actor->shadow->alpha),max(FMul(height,8), 4096))>>12, tex );
		}
	}

	// process garib shadows
/*	for(garib = garibCollectableList.head.next; garib != &garibCollectableList.head; garib = garib->next)
	{
		if(garib->distanceFromFrog < SPRITE_DRAWDISTANCE)
		{
			vec.v[X] = garib->sprite.pos.v[X];
			vec.v[Y] = garib->sprite.pos.v[Y] + garib->sprite.offsetY;
			vec.v[Z] = garib->sprite.pos.v[Z];
			DrawShadow( &vec, &garib->gameTile->normal, garib->shadow.radius, 0, garib->shadow.alpha, tex );
		}
	}*/
}







void DrawShadow( SVECTOR *pos, FVECTOR *normal, fixed size, fixed offset, short alpha, long tex )
{
	FVECTOR tempV[4];//, m;//, fwd;
	SVECTOR vT[4];
	IQUATERNION cross, q;//, up;
	long i;//, zeroZ=0;
	fixed t;
	static SPECFX fx;
	unsigned long colour;

//PUTS THE SPRITES RGB'S IN COLOUR, FIRST HALVING THEIR VALUES
	colour = 255>>1;// 191;;
	colour += 255>>1 <<8;//255<<8;
	colour += 255>>1 <<16;//0<<16;


// 	if(!fx)
// 		fx = (SPECFX*)MALLOC0(sizeof(SPECFX));

	vT[0].vx = size>>12;
	vT[0].vy = offset>>12;
	vT[0].vz = size>>12;
	

	vT[1].vx = size>>12;
	vT[1].vy = offset>>12;
	vT[1].vz = -size>>12;

	vT[2].vx = -size>>12;
	vT[2].vy = offset>>12;
	vT[2].vz = -size>>12;
	
	vT[3].vx = -size>>12;
	vT[3].vy = offset>>12;
	vT[3].vz = size>>12;

// 	// Translate to current fx pos and push
// 	guTranslateF( tMtrx, pos->v[X], pos->v[Y], pos->v[Z] );
// 	PushMatrix( tMtrx );

	// Rotate to be around normal
	CrossProductFFF( (FVECTOR *)&cross, normal, &upVec );
	MakeUnit( (FVECTOR *)&cross );
	t = DotProductFF( normal, &upVec );
	cross.w = -arccos(t);
	fixedGetQuaternionFromRotation( &q, &cross );
// 	QuaternionToMatrix( &q,(MATRIX *)rMtrx);
// 	PushMatrix( rMtrx );

// 	// Transform point by combined matrix
// 	SetMatrix( &dMtrx, &matrixStack.stack[matrixStack.stackPosition] );

	for( i=0; i<4; i++ )
	{
// 		guMtxXFMF( dMtrx, vT[i].sx, vT[i].sy, vT[i].sz, &tempVect.v[X], &tempVect.v[Y], &tempVect.v[Z] );
// 		XfmPoint( &m, &tempVect );
// 
// 		// Assign back to vT array
// 		vT[i].sx = m.v[X];
// 		vT[i].sy = m.v[Y];
// 		if( !m.v[Z] ) zeroZ++;
// 		else vT[i].sz = (m.v[Z]+DIST+4)*0.00025;


		SetVectorFS(&tempV[i],&vT[i]);
	   	RotateVectorByQuaternionFF(&tempV[i],&tempV[i],&q);
	  	SetVectorSF(&vT[i],&tempV[i]);								

// 	//add world coords 
// 		vT[i].vx += ripple->origin.vx;
// 		vT[i].vy += ripple->origin.vy;
// 		vT[i].vz += ripple->origin.vz;
	//add world coords 
 		vT[i].vx += pos->vx;
 		vT[i].vy += pos->vy;//mmfrig  debug to ensure the shadow appears above landscape CHANGE!!!!
 		vT[i].vz += pos->vz;


	}

//	memcpy( &vT[4], &vT[0], sizeof(SVECTOR ));

	fx.tex = tex;
	fx.flags = SPRITE_SUBTRACTIVE;
//	fx.a = alpha;
	fx.a = 60;
	fx.zDepthOff = -18;	//bring the shadow closer to the camera


	Print3D3DSprite ( &fx, vT, colour );


}


		   
void DrawFXDecal( SPECFX *ripple )
{
	FVECTOR tempV[4];
	SVECTOR vT[4];
	IQUATERNION q1, q2, q3;
	fixed t;
	unsigned long colour;
	long i;

//PUTS THE SPRITES RGB'S IN COLOUR, FIRST HALVING THEIR VALUES
	colour = ripple->r>>1;
	colour += (ripple->g>>1)<<8;
	colour += (ripple->b>>1)<<16;

//set the size of the effect
	vT[0].vx = (ripple->scale.vx>>12);
	vT[0].vy = 0;
	vT[0].vz = (ripple->scale.vz>>12);

	vT[1].vx = (ripple->scale.vx>>12);
	vT[1].vy = 0;
	vT[1].vz = (-ripple->scale.vz>>12);

	vT[2].vx = (-ripple->scale.vx>>12) ;
	vT[2].vy = 0;
	vT[2].vz = (-ripple->scale.vz>>12) ;
	
	vT[3].vx = (-ripple->scale.vx>>12);
	vT[3].vy = 0;
	vT[3].vz = (ripple->scale.vz>>12) ;


// Rotate to be around normal
	CrossProductFFF((FVECTOR *)&q1, &ripple->normal, &upVec ); 
	MakeUnit((FVECTOR *)&q1 );								   
	t = DotProductFF( &ripple->normal, &upVec );			  	   
	q1.w = -arccos(t);								   
	fixedGetQuaternionFromRotation( &q2, &q1 );		   

	if( ripple->type == FXTYPE_GARIBCOLLECT )		   
	{												   
		// Rotate around axis						   
		SetVectorFF((FVECTOR *)&q1, &ripple->normal ); 
		q1.w = ripple->angle;						   
		fixedGetQuaternionFromRotation( &q3, &q1 );	   
		fixedQuaternionMultiply( &q1, &q2, &q3 );	   
	}												   
	else SetQuaternion( &q1, &q2 );					   


	for( i=3; i>=0; i-- )
	{

		SetVectorFS(&tempV[i],&vT[i]);
	   	RotateVectorByQuaternionFF(&tempV[i],&tempV[i],&q1);
	  	SetVectorSF(&vT[i],&tempV[i]);								

	//add world coords 
		vT[i].vx += ripple->origin.vx;
		vT[i].vy += ripple->origin.vy;
		vT[i].vz += ripple->origin.vz;

	}

//	#ifdef PSX_VERSION
	Print3D3DSprite ( ripple, vT, colour );
//	#else
//	Print3D3DSprite ( ripple->tex, vT, colour,ripple->a );
//	#endif
}




void DrawFXRing( SPECFX *ring )
{
 	unsigned long  vx, i, j, colour;
 	SVECTOR vT[4];
	FVECTOR tempV[4];
 	IQUATERNION q1, q2, q3, cross;
 	fixed tilt, t;
 
//PUTS THE SPRITES RGB'S IN COLOUR, FIRST HALVING THEIR VALUES
	colour = ring->r>>1;
	colour += (ring->g>>1)<<8;
	colour += (ring->b>>1)<<16;

 
// Rotate around axis
 	SetVectorFF((FVECTOR *)&q1, &ring->normal);
 	q1.w = ring->angle;
 	fixedGetQuaternionFromRotation( &q2, &q1 );
 
// Rotate to be around normal
 	CrossProductFFF((FVECTOR *)&cross, (FVECTOR *)&q1, &upVec );
 	MakeUnit((FVECTOR *)&cross );
	t = DotProductFF( (FVECTOR *)&q1, &upVec );
	cross.w = -arccos(t);
 	fixedGetQuaternionFromRotation( &q3, &cross );

// Combine the rotations
 	fixedQuaternionMultiply( &q1, &q2, &q3 );
 
	for( i=0,vx=0; i < NUM_RINGSEGS; i++,vx+=4 )
	{
		memcpy( tempV, &ringVtx[vx], sizeof(FVECTOR)*4 );

// Transform to proper coords
 		for( j=0; j<4; j++ )
 		{
// Slant the polys
 			tilt = ((j < 2) ? ring->tilt : 4096);

			tempV[j].vx	= FMul(tempV[j].vx,FMul(tilt,ring->scale.vx));
			tempV[j].vy	= FMul(tempV[j].vy,FMul(tilt,ring->scale.vy));
			tempV[j].vz = FMul(tempV[j].vz,FMul(tilt,ring->scale.vz));

			RotateVectorByQuaternionFF(&tempV[j],&tempV[j],&q1);

			SetVectorSF(&vT[j],&tempV[j]);
//add world coords 
			vT[j].vx += ring->origin.vx;
			vT[j].vy += ring->origin.vy;
			vT[j].vz += ring->origin.vz;
 		}
//		#ifdef PSX_VERSION
		Print3D3DSprite ( ring, vT, colour );
//		#else
//		Print3D3DSprite ( ring->tex, vT, colour,ring->a );
//		#endif

 	}
}
 




void DrawFXTrail( SPECFX *trail )
{
 	unsigned long colour, i = trail->start;
 	SVECTOR vT[4], vTPrev[2];
 
 	if( trail->start == trail->end )
 		return;


//PUTS THE SPRITES RGB'S IN COLOUR, FIRST HALVING THEIR VALUES
	colour = trail->r>>1;
	colour += (trail->g>>1)<<8;
	colour += (trail->b>>1)<<16; 
 
 	do
 	{
 		/*********-[ First 2 points ]-********/
 		if( i != trail->start && vTPrev[0].vz && vTPrev[1].vz )
 			memcpy( vT, vTPrev, sizeof(SVECTOR)*2 );			// Previously transformed vertices
 		else
 			CalcTrailPoints( vT, trail, i );
 
 		/*********-[ Next 2 points ]-********/
 		CalcTrailPoints( &vT[2], trail, (i+1)%trail->numP );
 		memcpy( vTPrev, &vT[2], sizeof(SVECTOR)*2 ); 			// Store first 2 vertices of the next segment
 
//add world coords 
		vT[0].vx += trail->origin.vx;
		vT[0].vy += trail->origin.vy;
		vT[0].vz += trail->origin.vz;

		vT[1].vx += trail->origin.vx;
		vT[1].vy += trail->origin.vy;
		vT[1].vz += trail->origin.vz;

		vT[2].vx += trail->origin.vx;
		vT[2].vy += trail->origin.vy;
		vT[2].vz += trail->origin.vz;

		vT[3].vx += trail->origin.vx;
		vT[3].vy += trail->origin.vy;
		vT[3].vz += trail->origin.vz;

//		#ifdef PSX_VERSION
		Print3D3DSprite ( trail, vT, colour );
//		#else
//		Print3D3DSprite ( trail->tex, vT, colour,ripple->a );
//		#endif

 
 		if( ++i >= trail->numP ) i=0;
 
 	} while( i != trail->end );
}




void CalcTrailPoints( SVECTOR *vT, SPECFX *trail, int i )
{
	FVECTOR tempV[2];
	IQUATERNION q = {0,0,0,4096};
	SVECTOR pos;

	AddVectorSSS( &pos, &trail->origin, &trail->particles[i].pos );

	if( trail->type == FXTYPE_BILLBOARDTRAIL )	// Calculate screen align rotation
	{
		FVECTOR normal;
		IQUATERNION cross;
		fixed t;

		SubVectorFFS( &normal, &currCamSource, &pos );
		MakeUnit( &normal );
		CrossProductFFF( (FVECTOR *)&cross, &normal, &upVec );
		MakeUnit( (FVECTOR *)&cross );
		t = DotProductFF( &normal, &upVec );
		cross.w = arccos(t);
		fixedGetQuaternionFromRotation( &q, &cross );
	}

	vT[0].vx = trail->particles[i].poly[0].vx;
	vT[0].vy = trail->particles[i].poly[0].vy;
	vT[0].vz = trail->particles[i].poly[0].vz;
	vT[1].vx = trail->particles[i].poly[1].vx;
	vT[1].vy = trail->particles[i].poly[1].vy;
	vT[1].vz = trail->particles[i].poly[1].vz;


	SetVectorFS(&tempV[0], &vT[0]);

	RotateVectorByQuaternionFF(&tempV[0],&tempV[0],&q);

	SetVectorSF(&vT[0],&tempV[0]);

	SetVectorFS(&tempV[1], &vT[1]);

	RotateVectorByQuaternionFF(&tempV[1],&tempV[1],&q);

	SetVectorSF(&vT[1],&tempV[1]);

 	ScaleVector(&vT[0],SCALE);
 	ScaleVector(&vT[1],SCALE);

}



							   

void DrawFXLightning( SPECFX *fx )
{


// 	VECTOR tempVect, m;
 	SVECTOR	 vT[5], vTPrev[2];
// 	TEXENTRY *tEntry;
 	long i=0;
 	unsigned long colour;
 
//PUTS THE SPRITES RGB'S IN COLOUR, FIRST HALVING THEIR VALUES
	colour = fx->r>>1;
	colour += (fx->g>>1)<<8;
	colour += (fx->b>>1)<<16; 

 
 	while( i < fx->numP-1 )
 	{
// Copy in previous transformed vertices, if they exist
 		if( i != 0 && vTPrev[0].vz && vTPrev[1].vz )
 			memcpy( vT, vTPrev, sizeof(SVECTOR)*2 );
 		else
 		{
// Otherwise, transform them again 			
	   	vT[0].vx = fx->particles[i].poly[0].vx;
	   	vT[0].vy = fx->particles[i].poly[0].vy;
	   	vT[0].vz = fx->particles[i].poly[0].vz;

		vT[1].vx = fx->particles[i].poly[1].vx;
		vT[1].vy = fx->particles[i].poly[1].vy;
		vT[1].vz = fx->particles[i].poly[1].vz;
 		}
 
 		// Now the next two, to make a quad
    
		vT[2].vx = fx->particles[i+1].poly[1].vx;
		vT[2].vy = fx->particles[i+1].poly[1].vy;
		vT[2].vz = fx->particles[i+1].poly[1].vz;
 
		vT[3].vx = fx->particles[i+1].poly[0].vx;
		vT[3].vy = fx->particles[i+1].poly[0].vy;
		vT[3].vz = fx->particles[i+1].poly[0].vz;
 
 		// Store first 2 vertices of the next segment
 		memcpy( vTPrev, &vT[2], sizeof(SVECTOR)*2 );
 
// 		memcpy( &vT[4], &vT[0], sizeof(D3DTLVERTEX) );
// Draw polys, if they're not clipped

//		#ifdef PSX_VERSION
		Print3D3DSprite ( fx, vT, colour );
//		#else
//		Print3D3DSprite ( fx->tex, vT, colour,ripple->a );
//		#endif
 
 		i++;
 	} 
 
}
 
/* 
void DrawTongue( TONGUE *t )
{
	unsigned long i=0, index = (t->progress*(MAX_TONGUENODES-1)>>12);
	VERTC vT[4], vTPrev[2];
	TextureType *tEntry;

	tEntry = ((MDX_TEXENTRY *)t->tex);

	if( !tEntry || index < 2 )
		return;

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

		//********-[ Next 2 points ]-********
		CalcTongueNodes( &vT[2], t, i+1 );
		memcpy( vTPrev, &vT[2], sizeof(VERTC)*2 ); 			// Store first 2 vertices of the next segment

		//********-[ Draw the polys ]-********
		if( vT[0].sz && vT[1].sz && vT[2].sz && vT[3].sz )
		{
			Clip3DPolygon( vT, tEntry );
			Clip3DPolygon( &vT[1], tEntry );
		}

		i++;
	}
}

void CalcTongueNodes( VERTC *vT, TONGUE *t, int i )
{
	IQUATERNION q, cross;
	fixed p;
	FVECTOR pos, m, normal, cam;
	MATRIX rMtrx. tMtrx = GsIDMATRIX;
	FVECTOR upV = {0,4096,0};

	SetVectorFF(&pos, &t->segment[i]);
//	ScaleVectorFF(&pos, 410);
	// Translate to current fx pos and push
// 	guTranslateF( tMtrx, pos.vx, pos.vy, pos.vz );
// 	PushMatrix( tMtrx );
	tMtrx.t[0] = pos.vx;
	tMtrx.t[1] = pos.vy;
	tMtrx.t[2] = pos.vz;
	gte_SetTransMatrix(&tMtrx);

	SetVectorFF(&cam, &currCamSource);
	SubVectorFFF(&normal, &cam, &pos);
//	Normalise( &normal );
	MakeUnit(&normal);
	CrossProductFFF((FVECTOR*)&cross, &normal, &upV);
	Normalise((FVECTOR*)&cross);
	p = DotProductFF(&normal, &upV);
	cross.w = -arccos(p);
	fixedGetQuaternionFromRotation(&q, &cross);
//	QuaternionToMatrix( &q, (MDX_MATRIX *)rMtrx );
	QuatToPSXMatrix(&q, &rMtrx);

	// Precalculated rotation
//	PushMatrix( (MDX_MATRIX *)rMtrx );
	gte_SetRotMatrix(&rMtrx);

//	vT[0].vx = -5+(i*0.4);
	vT[0].vx = -5+((i*2)/5);
	vT[0].vy = 0;
	vT[0].vz = 0;
	vT[0].col.r = 255;
	vT[0].col.g = 0;
	vT[0].col.b = 0;
	vT[0].col.cd= 255;
//	vT[1].vx = 5-(i*0.4);
	vT[1].vx = 5-((i*2)/5);
	vT[1].vy = 0;
	vT[1].vz = 0;
	vT[1].col = vT[0].color;

	// Transform point by combined matrix
	MatrixSet( &dMtrx, &matrixStack.stack[matrixStack.stackPosition] );

	guMtxXFMF( dMtrx, vT[0].sx, vT[0].sy, vT[0].sz, &pos.vx, &pos.vy, &pos.vz );
	XfmPoint( &m, &pos, NULL );
	vT[0].sx = m.vx;
	vT[0].sy = m.vy;
	vT[0].sz = (m.vz)?((m.vz+DIST)*0.00025):0;
	guMtxXFMF( dMtrx, vT[1].sx, vT[1].sy, vT[1].sz, &pos.vx, &pos.vy, &pos.vz );
	XfmPoint( &m, &pos, NULL );
	vT[1].sx = m.vx;
	vT[1].sy = m.vy;
	vT[1].sz = (m.vz)?((m.vz+DIST)*0.00025):0;

	PopMatrix( ); // Rotation
	PopMatrix( ); // Translation
}
*/