
#include "specfx.h"
#include "Eff_Draw.h"
#include "cam.h"





void DrawSpecialFX()
{
	if ( ( gameState.mode == INGAME_MODE ) || ( gameState.mode == PAUSE_MODE ) ||
		 ( gameState.mode == CAMEO_MODE ) )
	{
		ProcessShadows();
		
		if( specFXList.numEntries )
		{
			SPECFX *fx;
			for( fx = specFXList.head.next; fx != &specFXList.head; fx = fx->next )
				if( fx->Draw )
					fx->Draw( fx );
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function		: ProcessShadows
	Purpose			: processes the shadows
	Parameters		: 
	Returns			: void
	Info			: 
*/
void ProcessShadows()
{
// 	SVECTOR vec;
// 	ENEMY *nme;
// 	PLATFORM *plat;
// 	GARIB *garib;
// 	int i;
// 	long tex;
// 	float height;
// 	
// 	tex = (long)((TEXENTRY *)txtrSolidRing)->hdl;
// 
// 	for( i=0; i<NUM_FROGS; i++ )
// 		if( frog[i]->actor->shadow && frog[i]->draw )
// 		{
// 			SubVectorSSS( &vec, &frog[i]->actor->pos, &currTile[i]->centre );
// //bb
// //			height = DotProduct( &vec, &currTile[i]->normal );
// 			FVECTOR fVec;
// 			SetVectorFS(&fVec,&vec);
// //bb
// //			height = DotProduct( &fVec, &currTile[i]->normal );
// 			height = DotProduct( &fVec, &currTile[i]->normal )/4096.0f;
// 
// //bb
// //			DrawShadow( &frog[i]->actor->pos, &currTile[i]->normal, frog[i]->actor->shadow->radius, -height+1, frog[i]->actor->shadow->alpha, tex );
// 			DrawShadow( &frog[i]->actor->pos, &currTile[i]->normal, frog[i]->actor->shadow->radius/4096.0F, -height+1, frog[i]->actor->shadow->alpha, tex );
// 		}
// 
// 	//------------------------------------------------------------------------------------------------
// 
// 	// process enemy shadows
// 	for(nme = enemyList.head.next; nme != &enemyList.head; nme = nme->next)
// 	{
// 		if( !nme->active || !nme->nmeActor )
// 			continue;
// 
// //bb
// //		if(nme->nmeActor->actor->shadow && nme->inTile && nme->nmeActor->distanceFromFrog < BBACTOR_DRAWDISTANCEINNER)
// 		if(nme->nmeActor->actor->shadow && nme->inTile && (nme->nmeActor->distanceFromFrog>>12) < BBACTOR_DRAWDISTANCEINNER)
// 		{
// //bb
// //			DrawShadow( &nme->nmeActor->actor->pos, &nme->inTile->normal, nme->nmeActor->actor->shadow->radius, -nme->path->nodes[nme->path->fromNode].offset+1, nme->nmeActor->actor->shadow->alpha, tex );
// 			DrawShadow( &nme->nmeActor->actor->pos, &nme->inTile->normal, nme->nmeActor->actor->shadow->radius/4096.0F, -nme->path->nodes[nme->path->fromNode].offset>>12+1, nme->nmeActor->actor->shadow->alpha, tex );
// 		}
// 	}
// 
// 	// process platform shadows
// 	for(plat = platformList.head.next; plat != &platformList.head; plat = plat->next)
// 	{
// 		if( !plat->active || !plat->pltActor )
// 			continue;
// 
// 		if(plat->pltActor->actor->shadow && plat->inTile && plat->pltActor->distanceFromFrog < ToFixed(BBACTOR_DRAWDISTANCEINNER))
// 		{
// 			DrawShadow( &plat->pltActor->actor->pos, &plat->inTile[0]->normal, plat->pltActor->actor->shadow->radius/4096.0F, (-plat->path->nodes[plat->path->fromNode].offset+ToFixed(1))/4096.0F, plat->pltActor->actor->shadow->alpha, tex );
// 		}
// 	}
// 
// 	// process garib shadows
// /*	for(garib = garibCollectableList.head.next; garib != &garibCollectableList.head; garib = garib->next)
// 	{
// 		if(garib->distanceFromFrog < SPRITE_DRAWDISTANCE)
// 		{
// 			vec.v[X] = garib->sprite.pos.v[X];
// 			vec.v[Y] = garib->sprite.pos.v[Y] + garib->sprite.offsetY;
// 			vec.v[Z] = garib->sprite.pos.v[Z];
// 			DrawShadow( &vec, &garib->gameTile->normal, garib->shadow.radius, 0, garib->shadow.alpha, tex );
// 		}
// 	}*/
}
 
 
// /*	--------------------------------------------------------------------------------
// 	Function		: DrawTongue
// 	Purpose			: er, draw Frogger's tongue.....
// 	Parameters		:
// 	Returns			:
// 	Info			:
// */
// 
// {
// 	if(backdrop == NULL)
// 		backdrop = (BACKDROP *)JallocAlloc(sizeof(BACKDROP),YES,"backdrop");
// 	backdrop->scaleX = scalex;
// 	backdrop->scaleY = scaley;
// 	backdrop->xPos = sourceX;
// 	backdrop->yPos = sourceY;
// //	backdrop->zPos = z;
// 	backdrop->draw = 1;
// //	backdrop->flags = flags;
// 	backdrop->r = backdrop->g = backdrop->b = backdrop->a = 255;
// 	FindTexture(&backdrop->texture,texID,YES);
// /*
// 	backdrop->background.s.imageX = sourceX<<5;
// 	backdrop->background.s.imageY = sourceY<<5;
// 	backdrop->background.s.imageW = (backdrop->texture->sx+1)<<2;
// 	backdrop->background.s.imageH = backdrop->texture->sy<<2;
// 	backdrop->background.s.frameX = destX<<2;
// 	backdrop->background.s.frameY = destY<<2;
// 	backdrop->background.s.frameW = destWidth<<2;
// 	backdrop->background.s.frameH = destHeight<<2;
// 	backdrop->background.s.imagePtr = (u64*)backdrop->texture->data;
// 	backdrop->background.s.imageLoad = G_BGLT_LOADTILE;
// 	backdrop->background.s.imageFmt = backdrop->texture->format;
// 	backdrop->background.s.imageSiz = backdrop->texture->pixSize;
// 	backdrop->background.s.imagePal = 0;
// 	backdrop->background.s.imageFlip = 0;
// 	backdrop->background.s.scaleW = (1024*1024)/scalex;
// 	backdrop->background.s.scaleH = (1024*1024)/scaley;
// 	backdrop->background.s.imageYorig = 0<<5;
// 
// 	osWritebackDCache(&backdrop->background, sizeof(uObjBg));
// */
// 	AddBackdrop(backdrop);
// 
// 	return backdrop;
// }
// 
// 
// void DrawShadow( SVECTOR *pos, FVECTOR *normal, float size, float offset, short alpha, long tex )
// {
// 	SVECTOR tempVect;
// 	FLVECTOR fwd;
// 	SVECTOR m;
// 	D3DTLVERTEX vT[5];
// 	QUATERNION cross, q, up;
// 	long i, zeroZ=0;
// 	float t;
// 
// 	vT[0].sx = size;
// 	vT[0].sy = offset;
// 	vT[0].sz = size;
// 	vT[0].tu = 0;
// 	vT[0].tv = 0;
// 	vT[0].color = D3DRGBA(0,0,0,alpha/255.0);
// 	vT[0].specular = D3DRGB(0,0,0);
// 
// 	vT[1].sx = size;
// 	vT[1].sy = offset;
// 	vT[1].sz = -size;
// 	vT[1].tu = 0;
// 	vT[1].tv = 1;
// 	vT[1].color = vT[0].color;
// 	vT[1].specular = vT[0].specular;
// 
// 	vT[2].sx = -size;
// 	vT[2].sy = offset;
// 	vT[2].sz = -size;
// 	vT[2].tu = 1;
// 	vT[2].tv = 1;
// 	vT[2].color = vT[0].color;
// 	vT[2].specular = vT[0].specular;
// 	
// 	vT[3].sx = -size;
// 	vT[3].sy = offset;
// 	vT[3].sz = size;
// 	vT[3].tu = 1;
// 	vT[3].tv = 0;
// 	vT[3].color = vT[0].color;
// 	vT[3].specular = vT[0].specular;
// 
// 	// Translate to current fx pos and push
// 	guTranslateF( tMtrx, pos->v[X], pos->v[Y], pos->v[Z] );
// 	PushMatrix( tMtrx );
// 
// 	// Rotate to be around normal
// 	CrossProductFlFF((FLVECTOR *)&cross, normal, &upVec);
// 	fl_MakeUnit((FLVECTOR *)&cross );
// 	t = DotProduct( normal, &upVec )/4096.0F;
// 	cross.w = -acos(t);
// 	GetQuaternionFromRotation( &q, &cross );
// 	QuaternionToMatrix( &q,(MATRIX *)rMtrx);
// 	PushMatrix( rMtrx );
// 
// 	// Transform point by combined matrix
// 	SetMatrix( &dMtrx, &matrixStack.stack[matrixStack.stackPosition] );
// 
// 	for( i=0; i<4; i++ )
// 	{
// 		guMtxXFMF( dMtrx, vT[i].sx, vT[i].sy, vT[i].sz, &tempVect.v[X], &tempVect.v[Y], &tempVect.v[Z] );
// 		XfmPoint( &m, &tempVect );
// 
// 		// Assign back to vT array
// 		vT[i].sx = m.v[X];
// 		vT[i].sy = m.v[Y];
// 		if( !m.v[Z] ) zeroZ++;
// 		else vT[i].sz = (m.v[Z]+DIST+4)*BBZMOD;
// 	}
// 
// 	memcpy( &vT[4], &vT[0], sizeof(D3DTLVERTEX) );
// 
// 	if( tex && !zeroZ )
// 	{
// 		Clip3DPolygon( vT, tex );
// 		Clip3DPolygon( &vT[2], tex );
// 	}
// 
// 	PopMatrix( ); // Rotation
// 	PopMatrix( ); // Translation
// }







void DrawFXRipple( SPECFX *ripple )
{
	FVECTOR tempV[4];
	SVECTOR vT[4];
	IQUATERNION q1, q2, q3;
	fixed t;
	unsigned long colour;

	if(ripple->deadCount)
		return;

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
	t = DotProduct( &ripple->normal, &upVec );			  	   
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


	SetVectorFS(&tempV[0],&vT[0]);
	SetVectorFS(&tempV[1],&vT[1]);
	SetVectorFS(&tempV[2],&vT[2]);
	SetVectorFS(&tempV[3],&vT[3]);

	RotateVectorByQuaternionFF(&tempV[0],&tempV[0],&q1);
	RotateVectorByQuaternionFF(&tempV[1],&tempV[1],&q1);
	RotateVectorByQuaternionFF(&tempV[2],&tempV[2],&q1);
	RotateVectorByQuaternionFF(&tempV[3],&tempV[3],&q1);

	SetVectorSF(&vT[0],&tempV[0]);
	SetVectorSF(&vT[1],&tempV[1]);
	SetVectorSF(&vT[2],&tempV[2]);
	SetVectorSF(&vT[3],&tempV[3]);


//add world coords 
	vT[0].vx += ripple->origin.vx;
	vT[0].vy += ripple->origin.vy;
	vT[0].vz += ripple->origin.vz;

	vT[1].vx += ripple->origin.vx;
	vT[1].vy += ripple->origin.vy;
	vT[1].vz += ripple->origin.vz;

	vT[2].vx += ripple->origin.vx;
	vT[2].vy += ripple->origin.vy;
	vT[2].vz += ripple->origin.vz;

	vT[3].vx += ripple->origin.vx;
	vT[3].vy += ripple->origin.vy;
	vT[3].vz += ripple->origin.vz;

	Print3D3DSprite ( ripple->tex, vT, colour );
}

void DrawFXRing( SPECFX *ring )
{
 	unsigned long  vx, i, j, colour;
 	SVECTOR vT[4];
	FVECTOR tempV[4];
 	IQUATERNION q1, q2, q3, cross;
 	fixed tilt, t;
 
 	if( ring->deadCount )
 		return;

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
	t = DotProduct( (FVECTOR *)&q1, &upVec );
	cross.w = -arccos(t);
 	fixedGetQuaternionFromRotation( &q3, &cross );
 
// Combine the rotations
 	fixedQuaternionMultiply( &q1, &q2, &q3 );
 
	for( i=0,vx=0; i < NUM_RINGSEGS; i++,vx+=4 )
	{
		SetVectorFS(&tempV[0], &vT[0]);
		SetVectorFS(&tempV[1], &vT[1]);
		SetVectorFS(&tempV[2], &vT[2]);
		SetVectorFS(&tempV[3], &vT[3]);

		memcpy( tempV, &ringVtx[vx], sizeof(FVECTOR)*4 );

// Transform to proper coords
 		for( j=0; j<4; j++ )
 		{
// Slant the polys
 			tilt = ((j < 2) ? ring->tilt : 4096);

			FMul(tempV[j].vx,FMul(tilt,ring->scale.vx));
			FMul(tempV[j].vy,FMul(tilt,ring->scale.vy));
			FMul(tempV[j].vz,FMul(tilt,ring->scale.vz));

			RotateVectorByQuaternionFF(&tempV[j],&tempV[j],&q1);

			SetVectorSF(&vT[j],&tempV[j]);


// Scale and push
// 			guScaleF( sMtrx, tilt*ring->scale.v[X]/4096.0F, tilt*ring->scale.v[Y]/4096.0F, tilt*ring->scale.v[Z]/4096.0F );
// 			PushMatrix( sMtrx );
 
// 			// Transform point by combined matrix
// 			SetMatrix( &dMtrx, &matrixStack.stack[matrixStack.stackPosition] );
// 			guMtxXFMF( dMtrx, vT[j].sx, vT[j].sy, vT[j].sz, &tempVect.v[X], &tempVect.v[Y], &tempVect.v[Z] );
// 
// 			XfmPoint(&m, &tempVect );
// 
// 			// Assign back to vT array
// 			vT[j].sx = m.v[X];
// 			vT[j].sy = m.v[Y];
// 			if( !m.v[Z] ) zeroZ++;
// 			else vT[j].sz = (m.v[Z]+DIST)*BBZMOD;
// 

 		}
		Print3D3DSprite ( ring->tex, vT, colour );
 	}
 

}
 



void DrawFXTrail( SPECFX *trail )
{
 	unsigned long colour, i = trail->start;
 	SVECTOR vT[4], vTPrev[2];
 
 	if( trail->deadCount || (trail->start == trail->end) )
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
 
 		/*********-[ Draw the polys ]-********/
// 		tEntry = ((TEXENTRY *)trail->tex);
// 		if( tEntry && vT[0].sz && vT[1].sz && vT[2].sz && vT[3].sz )
// 		{
// 			Clip3DPolygon( vT, tEntry->hdl );
// 			Clip3DPolygon( &vT[1], tEntry->hdl );
// 		}					   
		Print3D3DSprite ( trail->tex, vT, colour );
 
 		if( ++i >= trail->numP ) i=0;
 
 	} while( i != trail->end );
}



void CalcTrailPoints( SVECTOR *vT, SPECFX *trail, int i )
{
 
 	SVECTOR pos;//, m;
	FVECTOR tempV[2];
	IQUATERNION q;
 

 	AddVectorSSS( &pos, &trail->origin, &trail->particles[i].pos );
// 	// Translate to current fx pos and push
// 	guTranslateF( tMtrx, pos.v[X], pos.v[Y], pos.v[Z] );
// 	PushMatrix( tMtrx );

	vT[0].vx = pos.vx;
	vT[0].vy = pos.vy;
	vT[0].vz = pos.vz;

	vT[1].vx = pos.vx;
	vT[1].vy = pos.vy;
	vT[1].vz = pos.vz;

 
 	if( trail->type == FXTYPE_BILLBOARDTRAIL )	// Calculate screen align rotation
 	{
 		FVECTOR normal;
 		IQUATERNION cross;
 		fixed t;
 
 		SubVectorFFS( &normal, &currCamSource[0], &pos );	  //FFS??
 		MakeUnit( &normal );
 		CrossProduct( (FVECTOR *)&cross, &normal, &upVec );
 		MakeUnit( (FVECTOR *)&cross );
 		t = DotProduct( &normal, &upVec );
 		cross.w = arccos(t);
 		fixedGetQuaternionFromRotation( &q, &cross );
// 		QuaternionToMatrix( &q, (MATRIX *)trail->particles[i].rMtrx );
 	}
 
// 	// Precalculated rotation
// //	PushMatrix( (MATRIX *)trail->particles[i].rMtrx );
// //bb??
// 	float tempFix[4][4];
// 	memcpy(tempFix, trail->particles[i].rMtrx, sizeof(float)*16);
// 	PushMatrix( tempFix );
 
// 	vT[0].sx = trail->particles[i].poly[0].v[X];
// 	vT[0].sy = trail->particles[i].poly[0].v[Y];
// 	vT[0].sz = trail->particles[i].poly[0].v[Z];

// 	vT[1].sx = trail->particles[i].poly[1].v[X];
// 	vT[1].sy = trail->particles[i].poly[1].v[Y];
// 	vT[1].sz = trail->particles[i].poly[1].v[Z];

	SetVectorFS(&tempV[0], &vT[0]);

	RotateVectorByQuaternionFF(&tempV[0],&tempV[0],&q);

	SetVectorSF(&vT[0],&tempV[0]);

	SetVectorFS(&tempV[1], &vT[1]);

	RotateVectorByQuaternionFF(&tempV[1],&tempV[1],&q);

	SetVectorSF(&vT[1],&tempV[1]);

 
// 	// Transform point by combined matrix
// 	SetMatrix( &dMtrx, &matrixStack.stack[matrixStack.stackPosition] );
 
// 	guMtxXFMF( dMtrx, vT[0].sx, vT[0].sy, vT[0].sz, &pos.v[X], &pos.v[Y], &pos.v[Z] );
// 	XfmPoint( &m, &pos );
// 	vT[0].sx = m.v[X];
// 	vT[0].sy = m.v[Y];
// 	vT[0].sz = (m.v[Z])?((m.v[Z]+DIST)*0.0005):0;
// 	guMtxXFMF( dMtrx, vT[1].sx, vT[1].sy, vT[1].sz, &pos.v[X], &pos.v[Y], &pos.v[Z] );
// 	XfmPoint( &m, &pos );
// 	vT[1].sx = m.v[X];
// 	vT[1].sy = m.v[Y];
// 	vT[1].sz = (m.v[Z])?((m.v[Z]+DIST)*0.0005):0;
 
// 	PopMatrix( ); // Rotation
// 	PopMatrix( ); // Translation

}


void DrawFXLightning( SPECFX *fx )
{

/*
	VECTOR tempVect, m;
	D3DTLVERTEX vT[5], vTPrev[2];
	TEXENTRY *tEntry;
	long i=0;

	if( fx->deadCount )
		return;

	vT[0].specular = D3DRGB(0,0,0);
	vT[0].tu = 0;
	vT[0].tv = 0;
	vT[1].specular = vT[0].specular;
	vT[1].tu = 1;
	vT[1].tv = 0;
	vT[2].specular = vT[0].specular;
	vT[2].tu = 1;
	vT[2].tv = 1;
	vT[3].specular = vT[0].specular;
	vT[3].tu = 0;
	vT[3].tv = 1;

	while( i < fx->numP-1 )
	{
		// Copy in previous transformed vertices, if they exist
		if( i != 0 && vTPrev[0].sz && vTPrev[1].sz )
			memcpy( vT, vTPrev, sizeof(D3DTLVERTEX)*2 );
		else
		{
			// Otherwise, transform them again
			tempVect.v[X] = fx->particles[i].poly[0].v[X];
			tempVect.v[Y] = fx->particles[i].poly[0].v[Y];
			tempVect.v[Z] = fx->particles[i].poly[0].v[Z];
			XfmPoint( &m, &tempVect );
			vT[0].sx = m.v[X];
			vT[0].sy = m.v[Y];
			vT[0].sz = (m.v[Z])?((m.v[Z]+DIST)*0.0005):0;
			vT[0].color = D3DRGBA(fx->particles[i].r/255.0, fx->particles[i].g/255.0, fx->particles[i].b/255.0, fx->particles[i].a/255.0);

			tempVect.v[X] = fx->particles[i].poly[1].v[X];
			tempVect.v[Y] = fx->particles[i].poly[1].v[Y];
			tempVect.v[Z] = fx->particles[i].poly[1].v[Z];
			XfmPoint( &m, &tempVect );
			vT[1].sx = m.v[X];
			vT[1].sy = m.v[Y];
			vT[1].sz = (m.v[Z])?((m.v[Z]+DIST)*0.0005):0;
			vT[1].color = vT[0].color;
		}

		// Now the next two, to make a quad
		tempVect.v[X] = fx->particles[i+1].poly[1].v[X];
		tempVect.v[Y] = fx->particles[i+1].poly[1].v[Y];
		tempVect.v[Z] = fx->particles[i+1].poly[1].v[Z];
		XfmPoint( &m, &tempVect );
		vT[2].sx = m.v[X];
		vT[2].sy = m.v[Y];
		vT[2].sz = (m.v[Z])?((m.v[Z]+DIST)*0.0005):0;
		vT[2].color = vT[0].color;

		tempVect.v[X] = fx->particles[i+1].poly[0].v[X];
		tempVect.v[Y] = fx->particles[i+1].poly[0].v[Y];
		tempVect.v[Z] = fx->particles[i+1].poly[0].v[Z];
		XfmPoint( &m, &tempVect );
		vT[3].sx = m.v[X];
		vT[3].sy = m.v[Y];
		vT[3].sz = (m.v[Z])?((m.v[Z]+DIST)*0.0005):0;
		vT[3].color = vT[0].color;

		// Store first 2 vertices of the next segment
		memcpy( vTPrev, &vT[2], sizeof(D3DTLVERTEX)*2 );

		memcpy( &vT[4], &vT[0], sizeof(D3DTLVERTEX) );
		// Draw polys, if they're not clipped
		tEntry = ((TEXENTRY *)fx->tex);
		if( tEntry && vT[0].sz && vT[1].sz && vT[2].sz && vT[3].sz )
		{
			Clip3DPolygon( vT, tEntry->hdl );
			Clip3DPolygon( &vT[2], tEntry->hdl );
		}

		i++;
	} 
*/
}
 
 
