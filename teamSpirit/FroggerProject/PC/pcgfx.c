/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: pcgfx.c
	Programmer	: Jim
	Date		: 16/03/00

----------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <mdx.h>

#include "game.h"
#include "cam.h"
#include "frogger.h"
#include "frogmove.h"
#include "tongue.h"
#include "enemies.h"
#include "platform.h"
#include "particle.h"
#include "specfx.h"
#include "pcmisc.h"
#include "pcgfx.h"
#include "layout.h"


float tMtrx[4][4], rMtrx[4][4], sMtrx[4][4], dMtrx[4][4];

void CalcTrailPoints( D3DTLVERTEX *vT, SPECFX *fx, int i );
void CalcTongueNodes( D3DTLVERTEX *vT, TONGUE *t, int i );

void DrawShadow( MDX_VECTOR *pos, MDX_VECTOR *normal, float size, float offset, short alpha, MDX_TEXENTRY *tex );
void DrawTongue( TONGUE *t );


/*	--------------------------------------------------------------------------------
	Function		: DrawSpecialFX
	Purpose			: updates and draws the various special FX
	Parameters		: 
	Returns			: void
	Info			: 
*/
void DrawSpecialFX()
{
	int i;

	SwapFrame(0);
	ProcessShadows();
	
	if( sfxList.count )
	{
		SPECFX *fx;
		for( fx = sfxList.head.next; fx != &sfxList.head; fx = fx->next )
			if( fx->Draw )
				fx->Draw( fx );
	}

	for( i=0; i<NUM_FROGS; i++ )
		if( tongue[i].flags & TONGUE_BEINGUSED )
			DrawTongue( &tongue[i] );
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
	MDX_VECTOR vec, pos, up, tilePos;
	ENEMY *nme;
	PLATFORM *plat;
	GARIB *garib;
	int i;
	MDX_TEXENTRY *tex;
	short alpha;
	float height, size;
	
	tex = (MDX_TEXENTRY *)txtrSolidRing;

	for( i=0; i<NUM_FROGS; i++ )
		if( frog[i]->actor->shadow && frog[i]->draw && frog[i]->actor->shadow->draw )
		{
			SetVectorRS( &tilePos, &currTile[i]->centre );
			SetVectorRS( &pos, &frog[i]->actor->position );
			SetVectorRF( &up, &currTile[i]->normal );

			SubVector( &vec, &pos, &tilePos );
			height = DotProduct( &vec, &up )/SCALE;
			size = (float)(frog[i]->actor->shadow->radius>>12)/max(height*0.025, 1);
			alpha = frog[i]->actor->shadow->alpha/max(height*0.025, 1);

			DrawShadow( &pos, &up, size, -height+1, alpha, tex );
		}

	//------------------------------------------------------------------------------------------------

	// process enemy shadows
	for(nme = enemyList.head.next; nme != &enemyList.head; nme = nme->next)
	{
		if( !nme->active || !nme->nmeActor )
			continue;

		if( nme->nmeActor->actor->shadow && nme->inTile && nme->nmeActor->distanceFromFrog < BBACTOR_DRAWDISTANCEINNER<<12 )
		{
			SetVectorRS( &tilePos, &nme->inTile->centre );
			SetVectorRS( &pos, &nme->nmeActor->actor->position );
			SetVectorRF( &up, &nme->inTile->normal );

			SubVector( &vec, &pos, &tilePos );
			height = DotProduct( &vec, &up )/SCALE;
			size = (float)(nme->nmeActor->actor->shadow->radius>>12)/max(height*0.02, 1);
			alpha = nme->nmeActor->actor->shadow->alpha/max(height*0.02, 1);

			if (nme->path->nodes[nme->path->fromNode].worldTile==nme->inTile)
				DrawShadow( &pos, &up, size, -height+1, alpha, tex );
			else
				DrawShadow( &pos, &up, size, -height+1, alpha, tex );
		}
	}

	// process platform shadows
	for(plat = platformList.head.next; plat != &platformList.head; plat = plat->next)
	{
		if( !plat->active || !plat->pltActor )
			continue;

		if( plat->pltActor->actor->shadow && plat->inTile && plat->pltActor->distanceFromFrog < BBACTOR_DRAWDISTANCEINNER<<12 )
		{
			SetVectorRS( &tilePos, &plat->inTile[0]->centre );
			SetVectorRS( &pos, &plat->pltActor->actor->position );
			SetVectorRF( &up, &plat->inTile[0]->normal );

			SubVector( &vec, &pos, &tilePos );
			height = DotProduct( &vec, &up )/10;
			size = (float)(plat->pltActor->actor->shadow->radius>>12)/max(height*0.02, 1);
			alpha = plat->pltActor->actor->shadow->alpha/max(height*0.02, 1);

			DrawShadow( &pos, &up, size, -height+1, alpha, tex );
		}
	}
}


void DrawShadow( MDX_VECTOR *pos, MDX_VECTOR *normal, float size, float offset, short alpha, MDX_TEXENTRY *tex )
{
	MDX_VECTOR tempVect, m, fwd;
	D3DTLVERTEX vT[5];
	MDX_QUATERNION cross, q, up;
	long i, zeroZ=0;
	float t;

	ScaleVector( pos, 0.1 );
	size *= 0.1;

	vT[0].sx = size;
	vT[0].sy = offset;
	vT[0].sz = size;
	vT[0].tu = 0;
	vT[0].tv = 0;
	vT[0].color = D3DRGBA(0,0,0,alpha/255.0);
	vT[0].specular = D3DRGB(0,0,0);

	vT[1].sx = size;
	vT[1].sy = offset;
	vT[1].sz = -size;
	vT[1].tu = 0;
	vT[1].tv = 1;
	vT[1].color = vT[0].color;
	vT[1].specular = vT[0].specular;

	vT[2].sx = -size;
	vT[2].sy = offset;
	vT[2].sz = -size;
	vT[2].tu = 1;
	vT[2].tv = 1;
	vT[2].color = vT[0].color;
	vT[2].specular = vT[0].specular;
	
	vT[3].sx = -size;
	vT[3].sy = offset;
	vT[3].sz = size;
	vT[3].tu = 1;
	vT[3].tv = 0;
	vT[3].color = vT[0].color;
	vT[3].specular = vT[0].specular;

	// Translate to current fx pos and push
	guTranslateF( tMtrx, pos->vx, pos->vy, pos->vz );
	PushMatrix( tMtrx );

	// Rotate to be around normal
	CrossProduct( (MDX_VECTOR *)&cross, normal, &upV );
	Normalise( (MDX_VECTOR *)&cross );
	t = DotProduct( normal, &upV );
	cross.w = -acos(t);
	GetQuaternionFromRotation( &q, &cross );
	QuaternionToMatrix( &q,(MDX_MATRIX *)rMtrx);
	PushMatrix( rMtrx );

	// Transform point by combined matrix
	MatrixSet( &dMtrx, &matrixStack.stack[matrixStack.stackPosition] );

	for( i=0; i<4; i++ )
	{
		guMtxXFMF( dMtrx, vT[i].sx, vT[i].sy, vT[i].sz, &tempVect.vx, &tempVect.vy, &tempVect.vz );
		XfmPoint( &m, &tempVect, NULL );

		// Assign back to vT array
		vT[i].sx = m.vx;
		vT[i].sy = m.vy;
		if( !m.vz ) zeroZ++;
		else vT[i].sz = (m.vz+DIST-4)*0.00025;
	}

	memcpy( &vT[4], &vT[0], sizeof(D3DTLVERTEX) );

	if( tex && !zeroZ )
	{
		Clip3DPolygon( vT, tex );
		Clip3DPolygon( &vT[2], tex );
	}

	PopMatrix( ); // Rotation
	PopMatrix( ); // Translation
}


void DrawFXDecal( SPECFX *fx )
{
	MDX_VECTOR tempVect, m, fwd, scale, normal, pos;
	D3DTLVERTEX vT[5];
	MDX_TEXENTRY *tEntry;
	MDX_QUATERNION q1, q2, q3;
	long i, zeroZ=0;
	float t;

	if( !(tEntry = ((MDX_TEXENTRY *)fx->tex)) )
		return;

	SetVectorRF( &scale, &fx->scale );
	SetVectorRF( &normal, &fx->normal );
	SetVectorRS( &pos, &fx->origin );

	ScaleVector( &pos, 0.1 );
	ScaleVector( &scale, 0.1 );

	vT[0].sx = scale.vx;
	vT[0].sy = 0;
	vT[0].sz = scale.vz;
	vT[0].tu = 0;
	vT[0].tv = 0;
	vT[0].specular = D3DRGB(0,0,0);

	vT[1].sx = scale.vx;
	vT[1].sy = 0;
	vT[1].sz = -scale.vz;
	vT[1].tu = 0;
	vT[1].tv = 1;
	vT[1].specular = vT[0].specular;

	vT[2].sx = -scale.vx;
	vT[2].sy = 0;
	vT[2].sz = -scale.vz;
	vT[2].tu = 1;
	vT[2].tv = 1;
	vT[2].specular = vT[0].specular;
	
	vT[3].sx = -scale.vx;
	vT[3].sy = 0;
	vT[3].sz = scale.vz;
	vT[3].tu = 1;
	vT[3].tv = 0;
	vT[3].specular = vT[0].specular;

	// Translate to current fx pos and push
	guTranslateF( tMtrx, pos.vx, pos.vy, pos.vz );
	PushMatrix( tMtrx );

	// Rotate to be around normal
	CrossProduct( (MDX_VECTOR *)&q1, &normal, &upV );
	Normalise( (MDX_VECTOR *)&q1 );
	t = DotProduct( &normal, &upV );
	q1.w = -acos(t);
	GetQuaternionFromRotation( &q2, &q1 );

	if( fx->type == FXTYPE_GARIBCOLLECT )
	{
		// Rotate around axis
		SetVector( (MDX_VECTOR *)&q1, &normal );
		q1.w = fx->angle;
		GetQuaternionFromRotation( &q3, &q1 );
		QuaternionMultiply( &q1, &q2, &q3 );
	}
	else SetQuaternion( &q1, &q2 );
	
	QuaternionToMatrix( &q1,(MDX_MATRIX *)rMtrx);
	PushMatrix( rMtrx );

	// Transform point by combined matrix
	MatrixSet( &dMtrx, &matrixStack.stack[matrixStack.stackPosition] );

	// Dull "shadow" for ripple types, otherwise normal colour
	if( fx->type == FXTYPE_WAKE || fx->type == FXTYPE_WATERRIPPLE )
		vT[0].color = D3DRGBA(0.2,0.2,0.5,fx->a/255.0);
	else
		vT[0].color = D3DRGBA((float)fx->r/255.0,(float)fx->g/255.0,(float)fx->b/255.0,(float)fx->a/255.0);

	for( i=0; i<4; i++ )
	{
		guMtxXFMF( dMtrx, vT[i].sx, vT[i].sy, vT[i].sz, &tempVect.vx, &tempVect.vy, &tempVect.vz );
		XfmPoint( &m, &tempVect, NULL );

		// Assign back to vT array
		vT[i].sx = m.vx;
		vT[i].sy = m.vy;
		vT[i].color = vT[0].color;

		if( !m.vz ) zeroZ++;
		else vT[i].sz = (m.vz+DIST-4)*0.00025;
	}

	memcpy( &vT[4], &vT[0], sizeof(D3DTLVERTEX) );

	if( !zeroZ )
	{
		SwapFrame(0);
		Clip3DPolygon( vT, tEntry );
		Clip3DPolygon( &vT[2], tEntry );

		// If we want a pseudo-cheaty-bumpmap effect, shift vertices slightly and draw another, additive, copy.
		if( fx->type == FXTYPE_WAKE || fx->type == FXTYPE_WATERRIPPLE )
		{
			vT[0].color = D3DRGBA((float)fx->r/255.0,(float)fx->g/255.0,(float)fx->b/255.0,(float)fx->a/255.0);
			for( i=0; i<4; i++ )
			{
				vT[i].sx += 3;
				vT[i].color = vT[0].color;
			}

			SwapFrame(3);
			Clip3DPolygon( vT, tEntry );
			Clip3DPolygon( &vT[2], tEntry );
			SwapFrame(0);
		}
	}

	PopMatrix( ); // Rotation
	PopMatrix( ); // Translation
}


void DrawFXRing( SPECFX *fx )
{
	unsigned long vx, i, j, vxj;
	D3DTLVERTEX vT[5];
	MDX_TEXENTRY *tEntry;
	MDX_VECTOR tempVect, m, scale, normal, pos;
	MDX_QUATERNION q1, q2, q3, cross;
	SVECTOR fxpos;
	float tilt, t;
	int zeroZ = 0;

	if( !(tEntry = ((MDX_TEXENTRY *)fx->tex)) )
		return;

	SetVectorRF( &scale, &fx->scale );
	SetVectorRF( &normal, &fx->normal );
	SetVectorRS( &pos, &fx->origin );

	ScaleVector( &scale, 0.1 );
	ScaleVector( &pos, 0.1 );

	vT[0].tu = 0;
	vT[0].tv = 1;
	vT[0].color = D3DRGBA((float)fx->r/255.0,(float)fx->g/255.0,(float)fx->b/255.0,(float)fx->a/255.0);
	vT[0].specular = D3DRGB(0,0,0);

	vT[1].tu = 0;
	vT[1].tv = 0;
	vT[1].color = vT[0].color;
	vT[1].specular = vT[0].specular;

	vT[2].tu = 1;
	vT[2].tv = 0;
	vT[2].color = vT[0].color;
	vT[2].specular = vT[0].specular;
	
	vT[3].tu = 1;
	vT[3].tv = 1;
	vT[3].color = vT[0].color;
	vT[3].specular = vT[0].specular;

	// Translate to current fx pos and push
	guTranslateF( tMtrx, pos.vx, pos.vy, pos.vz );
	PushMatrix( tMtrx );

	// Rotate around axis
	SetVector( (MDX_VECTOR *)&q1, &normal );
	q1.w = fx->angle/57.6;
	GetQuaternionFromRotation( &q2, &q1 );

	// Rotate to be around normal
	CrossProduct( (MDX_VECTOR *)&cross, (MDX_VECTOR *)&q1, &upV );
	Normalise( (MDX_VECTOR *)&cross );
	t = DotProduct( (MDX_VECTOR *)&q1, &upV );
	cross.w = -acos(t);
	GetQuaternionFromRotation( &q3, &cross );

	// Combine the rotations and push
	QuaternionMultiply( &q1, &q2, &q3 );
	QuaternionToMatrix( &q1,(MDX_MATRIX *)rMtrx);
	PushMatrix( rMtrx );

	if( fx->type == FXTYPE_CROAK )
		SwapFrame(3);

	for( i=0,vx=0; i < NUM_RINGSEGS; i++,vx+=2 )
	{
		// Transform to proper coords
		for( j=0; j<4; j++ )
		{
			vxj = (vx+j)%(NUM_RINGSEGS<<1);
			vT[j].sx = ringVtx[vxj].vx*0.000244;
			vT[j].sy = ringVtx[vxj].vy*0.000244;
			vT[j].sz = ringVtx[vxj].vz*0.000244;
			// Slant the polys
			tilt = (float)(((i&1)?(j==0||j==3):(j==1||j==2)) ? fx->tilt : 4096)/4096;
			vT[j].tv = 1-vT[j].tv;//(i&1) ? 1-vT[j].tv : vT[j].tv;
			// Scale and push
			guScaleF( sMtrx, tilt*scale.vx, tilt*scale.vy, tilt*scale.vz );
			PushMatrix( sMtrx );

			// Transform point by combined matrix
			MatrixSet( &dMtrx, &matrixStack.stack[matrixStack.stackPosition] );
			guMtxXFMF( dMtrx, vT[j].sx, vT[j].sy, vT[j].sz, &tempVect.vx, &tempVect.vy, &tempVect.vz );

			if( !j ) SetVectorSR( &fxpos, &tempVect );

			XfmPoint( &m, &tempVect, NULL );

			// Assign back to vT array
			vT[j].sx = m.vx;
			vT[j].sy = m.vy;
			if( !m.vz ) zeroZ++;
			else vT[j].sz = (m.vz+DIST)*0.00025;

			PopMatrix( ); // Pop scale
		}

		if( !zeroZ )
		{
			memcpy( &vT[4], &vT[0], sizeof(D3DTLVERTEX) );
			Clip3DPolygon( vT, tEntry );
			Clip3DPolygon( &vT[2], tEntry );
		}

		if( i&1 && (actFrameCount&1) )
		{
			SPECFX *trail;

			ScaleVector( &fxpos, 10 );

			if( (trail = CreateSpecialEffect( FXTYPE_TWINKLE, &fxpos, &fx->normal, 81920, 0, 0, 4096 )) )
			{
				trail->tilt = 8192;
				if( i&2 ) SetFXColour( trail, 180, 220, 180 );
				else SetFXColour( trail, fx->r, fx->g, fx->b );
			}
		}
	}

	PopMatrix( ); // Rotation
	PopMatrix( ); // Translation

	if( fx->type == FXTYPE_CROAK )
		SwapFrame(0);
}


void DrawFXTrail( SPECFX *fx )
{
	unsigned long i = fx->start;
	D3DTLVERTEX vT[4], vTPrev[2];
	MDX_TEXENTRY *tEntry;

	tEntry = ((MDX_TEXENTRY *)fx->tex);
	if( !tEntry || (fx->start == fx->end) )
		return;

	vT[0].specular = D3DRGB(0,0,0);
	vT[0].tu = 1;
	vT[0].tv = 1;
	vT[1].specular = vT[0].specular;
	vT[1].tu = 0;
	vT[1].tv = 1;
	vT[2].specular = vT[0].specular;
	vT[2].tu = 0;
	vT[2].tv = 0;
	vT[3].specular = vT[0].specular;
	vT[3].tu = 1;
	vT[3].tv = 0;

	SwapFrame(3);

	do
	{
		/*********-[ First 2 points ]-********/
		if( i != fx->start && vTPrev[0].sz && vTPrev[1].sz )
			memcpy( vT, vTPrev, sizeof(D3DTLVERTEX)*2 );			// Previously transformed vertices
		else
			CalcTrailPoints( vT, fx, i );

		/*********-[ Next 2 points ]-********/
		CalcTrailPoints( &vT[2], fx, (i+1)%fx->numP );
		memcpy( vTPrev, &vT[2], sizeof(D3DTLVERTEX)*2 ); 			// Store first 2 vertices of the next segment

		/*********-[ Draw the polys ]-********/
		if( vT[0].sz && vT[1].sz && vT[2].sz && vT[3].sz )
		{
			Clip3DPolygon( vT, tEntry );
			Clip3DPolygon( &vT[1], tEntry );
		}

		if( ++i >= fx->numP ) i=0;

	} while( i != fx->end );

	SwapFrame(0);
}


void CalcTrailPoints( D3DTLVERTEX *vT, SPECFX *fx, int i )
{
	MDX_VECTOR pos, m;

	AddVector( &pos, &fx->origin, &fx->particles[i].pos );
	ScaleVector( &pos, 0.1 );

	// Translate to current fx pos and push
	guTranslateF( tMtrx, pos.vx, pos.vy, pos.vz );
	PushMatrix( tMtrx );

	if( fx->type == FXTYPE_BILLBOARDTRAIL )	// Calculate screen align rotation
	{
		MDX_VECTOR normal, cam;
		MDX_QUATERNION q, cross;
		float t;

		SetVectorRF( &cam, &currCamSource );
		SubVector( &normal, &cam, &pos );
		Normalise( &normal );
		CrossProduct( (MDX_VECTOR *)&cross, &normal, &upV );
		Normalise( (MDX_VECTOR *)&cross );
		t = DotProduct( &normal, &upV );
		cross.w = acos(t);
		GetQuaternionFromRotation( &q, &cross );
		QuaternionToMatrix( &q, (MDX_MATRIX *)fx->particles[i].rMtrx );
	}

	// Precalculated rotation
	PushMatrix( (MDX_MATRIX *)fx->particles[i].rMtrx );

	vT[0].sx = fx->particles[i].poly[0].vx*0.1;
	vT[0].sy = fx->particles[i].poly[0].vy*0.1;
	vT[0].sz = fx->particles[i].poly[0].vz*0.1;
	vT[0].color = D3DRGBA(fx->particles[i].r/255.0, fx->particles[i].g/255.0, fx->particles[i].b/255.0, fx->particles[i].a/255.0);
	vT[1].sx = fx->particles[i].poly[1].vx*0.1;
	vT[1].sy = fx->particles[i].poly[1].vy*0.1;
	vT[1].sz = fx->particles[i].poly[1].vz*0.1;
	vT[1].color = vT[0].color;

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


void DrawFXLightning( SPECFX *fx )
{
	MDX_VECTOR tempVect, m;
	D3DTLVERTEX vT[5], vTPrev[2];
	MDX_TEXENTRY *tEntry;
	PARTICLE *p;
	long i=0;

	if( !(tEntry = ((MDX_TEXENTRY *)fx->tex)) )
		return;

	vT[0].specular = D3DRGB(0,0,0);
	vT[1].specular = vT[0].specular;
	vT[2].specular = vT[0].specular;
	vT[3].specular = vT[0].specular;

	// Additive mode
	SwapFrame(3);

	p = fx->particles;
	while( i < fx->numP-1 )
	{
		// Copy in previous transformed vertices, if they exist
		if( i && vTPrev[0].sz && vTPrev[1].sz )
			memcpy( vT, vTPrev, sizeof(D3DTLVERTEX)*2 );
		else
		{
			// Otherwise, transform them again
			tempVect.vx = p->poly[0].vx*0.1;
			tempVect.vy = p->poly[0].vy*0.1;
			tempVect.vz = p->poly[0].vz*0.1;
			XfmPoint( &m, &tempVect, NULL );
			vT[0].sx = m.vx;
			vT[0].sy = m.vy;
			vT[0].tu = 0;
			vT[0].tv = 0;
			vT[0].sz = (m.vz)?((m.vz+DIST)*0.00025):0;
			vT[0].color = D3DRGBA(p->r/255.0, p->g/255.0, p->b/255.0, p->a/255.0);

			tempVect.vx = p->poly[1].vx*0.1;
			tempVect.vy = p->poly[1].vy*0.1;
			tempVect.vz = p->poly[1].vz*0.1;
			XfmPoint( &m, &tempVect, NULL );
			vT[1].sx = m.vx;
			vT[1].sy = m.vy;
			vT[1].tu = 1;
			vT[1].tv = 0;
			vT[1].sz = (m.vz)?((m.vz+DIST)*0.00025):0;
			vT[1].color = vT[0].color;
		}

		// Now the next two, to make a quad
		tempVect.vx = p->next->poly[1].vx*0.1;
		tempVect.vy = p->next->poly[1].vy*0.1;
		tempVect.vz = p->next->poly[1].vz*0.1;
		XfmPoint( &m, &tempVect, NULL );
		vT[2].sx = m.vx;
		vT[2].sy = m.vy;
		vT[2].tu = 1;
		vT[2].tv = 1;
		vT[2].sz = (m.vz)?((m.vz+DIST)*0.00025):0;
		vT[2].color = vT[0].color;

		tempVect.vx = p->next->poly[0].vx*0.1;
		tempVect.vy = p->next->poly[0].vy*0.1;
		tempVect.vz = p->next->poly[0].vz*0.1;
		XfmPoint( &m, &tempVect, NULL );
		vT[3].sx = m.vx;
		vT[3].sy = m.vy;
		vT[3].tu = 0;
		vT[3].tv = 1;
		vT[3].sz = (m.vz)?((m.vz+DIST)*0.00025):0;
		vT[3].color = vT[0].color;

		// Store first 2 vertices of the next segment
		memcpy( vTPrev, &vT[3], sizeof(D3DTLVERTEX) );
		memcpy( &vTPrev[1], &vT[2], sizeof(D3DTLVERTEX) );
		// And back to the first vertex for the second tri
		memcpy( &vT[4], &vT[0], sizeof(D3DTLVERTEX) );

		// Draw polys, if they're not clipped
		if( vT[0].sz && vT[1].sz && vT[2].sz && vT[3].sz )
		{
			Clip3DPolygon( vT, tEntry );
			Clip3DPolygon( &vT[2], tEntry );
		}

		i++;
		p = p->next;
	} 

	SwapFrame(0);
}

void DrawTongue( TONGUE *t )
{
	unsigned long i=0, index = (t->progress*(MAX_TONGUENODES-1)>>12);
	D3DTLVERTEX vT[4], vTPrev[2];
	MDX_TEXENTRY *tEntry;

	tEntry = ((MDX_TEXENTRY *)t->tex);

	if( !tEntry || index < 2 )
		return;

	vT[0].specular = D3DRGB(0,0,0);
	vT[0].tu = 1;
	vT[0].tv = 1;
	vT[1].specular = vT[0].specular;
	vT[1].tu = 0;
	vT[1].tv = 1;
	vT[2].specular = vT[0].specular;
	vT[2].tu = 0;
	vT[2].tv = 0;
	vT[3].specular = vT[0].specular;
	vT[3].tu = 1;
	vT[3].tv = 0;

	while( i < index )
	{
		//********-[ First 2 points ]-*******
		if( i && vTPrev[0].sz && vTPrev[1].sz )
			memcpy( vT, vTPrev, sizeof(D3DTLVERTEX)*2 );			// Previously transformed vertices
		else
			CalcTongueNodes( vT, t, i );

		//********-[ Next 2 points ]-********
		CalcTongueNodes( &vT[2], t, i+1 );
		memcpy( vTPrev, &vT[2], sizeof(D3DTLVERTEX)*2 ); 			// Store first 2 vertices of the next segment

		//********-[ Draw the polys ]-********
		if( vT[0].sz && vT[1].sz && vT[2].sz && vT[3].sz )
		{
			Clip3DPolygon( vT, tEntry );
			Clip3DPolygon( &vT[1], tEntry );
		}

		i++;
	}
}


void CalcTongueNodes( D3DTLVERTEX *vT, TONGUE *t, int i )
{
	MDX_QUATERNION q, cross;
	float p;
	MDX_VECTOR pos, m, normal, cam;

	SetVectorRF( &pos, &t->segment[i] );
	ScaleVector( &pos, 0.1 );
	// Translate to current fx pos and push
	guTranslateF( tMtrx, pos.vx, pos.vy, pos.vz );
	PushMatrix( tMtrx );

	SetVectorRF( &cam, &currCamSource );
	SubVector( &normal, &cam, &pos );
	Normalise( &normal );
	CrossProduct( (MDX_VECTOR *)&cross, &normal, &upV );
	Normalise( (MDX_VECTOR *)&cross );
	p = DotProduct( &normal, &upV );
	cross.w = -acos(p);
	GetQuaternionFromRotation( &q, &cross );
	QuaternionToMatrix( &q, (MDX_MATRIX *)rMtrx );

	// Precalculated rotation
	PushMatrix( (MDX_MATRIX *)rMtrx );

	vT[0].sx = -5+(i*0.4);
	vT[0].sy = 0;
	vT[0].sz = 0;
	vT[0].color = D3DRGBA(1,0,0,1);
	vT[1].sx = 5-(i*0.4);
	vT[1].sy = 0;
	vT[1].sz = 0;
	vT[1].color = vT[0].color;

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

/*
void DrawRandomPolyList( )
{
	POLYGON *p;

	for( p=rpList; p; p=p->next )
		TransformAndDrawPolygon( p );
}

void TransformAndDrawPolygon( POLYGON *p )
{
	MDX_VECTOR tempVect, m, fwd;
	D3DTLVERTEX vT[5];
	MDX_QUATERNION cross, q1, q2, q3, up;
	long i, zeroZ=0;
	MDX_TEXENTRY *tex;
	float t;

	vT[0].sx = p->vT[0].vx;
	vT[0].sy = p->vT[0].vy;
	vT[0].sz = p->vT[0].vz;
	vT[0].tu = p->u;
	vT[0].tv = p->v;
	vT[0].color = D3DRGBA(p->r/255.0,p->g/255.0,p->b/255.0,p->a/255.0);
	vT[0].specular = D3DRGB(0,0,0);

	vT[1].sx = p->vT[1].vx;
	vT[1].sy = p->vT[1].vy;
	vT[1].sz = p->vT[1].vz;
	vT[1].tu = p->u;
	vT[1].tv = p->v1;
	vT[1].color = vT[0].color;
	vT[1].specular = vT[0].specular;

	vT[2].sx = p->vT[2].vx;
	vT[2].sy = p->vT[2].vy;
	vT[2].sz = p->vT[2].vz;
	vT[2].tu = p->u1;
	vT[2].tv = p->v1;
	vT[2].color = vT[0].color;
	vT[2].specular = vT[0].specular;
	
	vT[3].sx = p->vT[3].vx;
	vT[3].sy = p->vT[3].vy;
	vT[3].sz = p->vT[3].vz;
	vT[3].tu = p->u1;
	vT[3].tv = p->v;
	vT[3].color = vT[0].color;
	vT[3].specular = vT[0].specular;

	// Translate to current fx pos and push
	guTranslateF( tMtrx, p->plane.point.vx, p->plane.point.vy, p->plane.point.vz );
	PushMatrix( tMtrx );

	// Rotate around axis
	SetVector( (MDX_VECTOR *)&q1, &p->plane.normal );
	q1.w = p->angle;
	GetQuaternionFromRotation( &q2, &q1 );

	// Rotate to be around normal
	CrossProduct( (MDX_VECTOR *)&cross, &p->plane.normal, &upV );
	Normalise( (MDX_VECTOR *)&cross );
	t = DotProduct( &p->plane.normal, &upV );
	cross.w = -acos(t);
	GetQuaternionFromRotation( &q3, &cross );

	// Combine the rotations and push
	QuaternionMultiply( &q1, &q2, &q3 );
	QuaternionToMatrix( &q1,(MDX_MATRIX *)rMtrx);
	PushMatrix( rMtrx );

	// Transform point by combined matrix
	MatrixSet( &dMtrx, &matrixStack.stack[matrixStack.stackPosition] );

	for( i=0; i<4; i++ )
	{
		guMtxXFMF( dMtrx, vT[i].sx, vT[i].sy, vT[i].sz, &tempVect.vx, &tempVect.vy, &tempVect.vz );
		XfmPoint( &m, &tempVect, NULL );

		// Assign back to vT array
		vT[i].sx = m.vx;
		vT[i].sy = m.vy;
		if( !m.vz ) zeroZ++;
		else vT[i].sz = (m.vz+DIST)*0.00025;
	}

	memcpy( &vT[4], &vT[0], sizeof(D3DTLVERTEX) );

	tex = (MDX_TEXENTRY *)p->tex;

	if( tex && !zeroZ )
	{
		Clip3DPolygon( vT, tex );
		Clip3DPolygon( &vT[2], tex );
	}

	PopMatrix( ); // Rotation
	PopMatrix( ); // Translation
}
*/

/*	THIS IS A MORE OPTIMAL VERSION OF DRAWTONGUE THAT RELIES ON QUAD DRAWING. SO IT WONT WORK YET :)
void DrawTongue( TONGUE *t )
{
	unsigned long i=0, index = (int)(t->progress*(MAX_TONGUENODES-1));
	D3DTLVERTEX vT1[2], vT2[2], *tmp;
	TEXENTRY *tEntry;

	if( index < 2 )
		return;

	vT1[0].specular = D3DRGB(0,0,0);
	vT1[0].tu = 1;
	vT1[0].tv = 1;
	vT1[1].specular = vT1[0].specular;
	vT1[1].tu = 0;
	vT1[1].tv = 1;
	vT2[0].specular = vT1[0].specular;
	vT2[0].tu = 0;
	vT2[0].tv = 0;
	vT2[1].specular = vT1[0].specular;
	vT2[1].tu = 1;
	vT2[1].tv = 0;


	while( i < index )
	{
		//********-[ First 2 points ]-*******
		if( !(i && vT1[0].sz && vT1[1].sz) )
			CalcTongueNodes( vT1, t, i );

		//********-[ Next 2 points ]-********
		CalcTongueNodes( vT2, t, i+1 );

		//********-[ Draw the polys ]-********
		tEntry = ((TEXENTRY *)t->tex);
		if( tEntry && vT1[0].sz && vT1[1].sz && vT2[0].sz && vT2[1].sz )
		{
			Clip3DPolygon( vT, tEntry );
			Clip3DPolygon( &vT[1], tEntry );
		}

		tmp = vT1;
		vT1 = vT2;
		vT2 = tmp;

		i++;
	}
}
*/
