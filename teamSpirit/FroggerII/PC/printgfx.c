/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: printgfx.c
	Programmer	: Andy Eder
	Date		: 19/04/99 11:12:48

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>

#include "incs.h"
#include "software.h"
#include "mavis.h"

//GRABSTRUCT grabData;

short drawScreenGrab = 0;
short grabFlag = 0;
extern float RES_DIFF;
extern float RES_DIFF2;
extern long runHardware;
long numSprites;

float tMtrx[4][4], rMtrx[4][4], sMtrx[4][4], dMtrx[4][4];

void Clip3DPolygon (D3DTLVERTEX in[3], long texture);

void CalcTrailPoints( D3DTLVERTEX *vT, SPECFX *trail, int i );
void CalcTongueNodes( D3DTLVERTEX *vT, TONGUE *t, int i );

void DrawShadow( VECTOR *pos, VECTOR *normal, float size, float offset, short alpha, long tex );
void DrawTongue( TONGUE *t );


/*	--------------------------------------------------------------------------------
	Function		: PrintTextAsOverlay
	Purpose			: prints text as an overlay
	Parameters		: none
	Returns			: void
	Info			:
*/
float wSeed = 0.0F;

void PrintTextAsOverlay(TEXTOVERLAY *tOver)
{
	int pos = 0,length;
	int x,y,width,height;
	unsigned char letter,letterCount;
	char *c;
	short u,v,letterID;
				
	x = tOver->xPos * RES_DIFF2;
	y = tOver->yPos * RES_DIFF2;
	height = tOver->font->height * RES_DIFF * tOver->scale;
	tOver->wtotal = 0;
	if(tOver->centred)
	{
		for (c = tOver->text, width = 0; *c; c++)
		{
			letterID = characterMap[*c];
			width += tOver->font->xSpacing[letterID] * RES_DIFF;
		}
		x = (SCREEN_WIDTH-width)/2;
		
		tOver->xPos = tOver->xPosTo = x/2;
		tOver->wtotal = width;

		//if((length & 1) != 0)
		//	x -= 5;
	}
	else // If told to move, goto destination
	{
		float spd = tOver->speed * gameSpeed;
		if( Fabs(tOver->xPosTo-tOver->xPos) < spd )
			tOver->xPos = tOver->xPosTo;
		else
			tOver->xPos += (tOver->xPosTo > tOver->xPos)?spd:-spd;

		if( Fabs(tOver->yPosTo-tOver->yPos) < spd )
			tOver->yPos = tOver->yPosTo;
		else
			tOver->yPos += (tOver->yPosTo > tOver->yPos)?spd:-spd;
	}

	letterCount = 0;
	for(c = tOver->text; *c; c++)
	{
		letter = tOver->text[pos];
		letterID = characterMap[letter];

		
		if(tOver->flags & TEXTOVERLAY_WAVECHARS)
		{
			float t = sinf(wSeed);
			letterCount++;
			y = tOver->yPos*RES_DIFF2 - 4 + sinf((float)wSeed + (float)tOver->waveStart + ((float)letterCount * (PI_OVER_4))) * tOver->waveAmplitude;
			height = tOver->font->height * RES_DIFF + 4 - sinf((float)wSeed + (float)tOver->waveStart + ((float)letterCount * (PI_OVER_4))) * tOver->waveAmplitude * 2;
		}

		if(letter == 32)
		{
			x += tOver->font->xSpacing[0] * tOver->scale;	
		}
		else
		{
			u = tOver->font->offset[letterID].v[X];
			v = tOver->font->offset[letterID].v[Y];
			
			if (runHardware)
			{
				DrawAlphaSprite (x, y, 0, tOver->font->height * RES_DIFF * tOver->scale, height,
				(float)u/256.0,(float)v/256.0,
				((float)u+tOver->font->width)/256.0,
				((float)v+tOver->font->height)/256.0,tOver->font->hdl,D3DRGBA(tOver->r/255.0,tOver->g/255.0,tOver->b/255.0,tOver->a/255.0));
			}
	
			x += tOver->font->xSpacing[letterID] * RES_DIFF * tOver->scale;
		}

		pos++;
	}
	if (!tOver->centred)
		tOver->wtotal = x-tOver->xPos * RES_DIFF2;
	wSeed += 0.01*gameSpeed;
}
void XfmPoint (VECTOR *vTemp2, VECTOR *in);

extern long FOV,DIST;

char IsPointVisible(VECTOR *p)
{
	VECTOR m;
	XfmPoint (&m,p);
	if (m.v[3]==0)
		return 0;
	if (m.v[0]<0) 
		return 0;
	if (m.v[0]>640) 
		return 0;
	if (m.v[1]<0) 
		return 0;
	if (m.v[1]>480)
		return 0;
	return 1;
}


/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

SPRITE *PrintSprites()
{
	int i;
	SPRITE *cur;
	
	// transform sprites to screen coords ready for sorting
	for(cur = sprList.head.next; cur != &sprList.head && numSortArraySprites < MAX_ARRAY_SPRITES; cur = cur->next)
		XfmPoint(&cur->sc,&cur->pos);

	ZSortSpriteList();

	// draw from the newly sorted static array
	
	i = numSortArraySprites;
	while(i--) if( spriteSortArray[i].draw ) PrintSprite(&spriteSortArray[i]);

	return cur;
}

/*	--------------------------------------------------------------------------------
	Function		: PrintSpriteOverlays
	Purpose			: draws overlays - icons, etc....
	Parameters		: none
	Returns			: void
	Info			:
*/
void PrintSpriteOverlays(long num)
{
	SPRITEOVERLAY *cur;
	short x,y;
	unsigned long texture;
	TEXENTRY *tEntry;


	cur = spriteOverlayList.head.next;
	if (spriteOverlayList.numEntries)
	while(cur!=&spriteOverlayList.head)
	{
		if (cur->num!=num)
		{
			cur = cur->next;
			continue;
		}

		// update the sprite animation if an animated sprite overlay
		if(cur->draw)
		{
			// Go to destination, if specified
			float spd = cur->speed * gameSpeed;
			
			if (fabs(spd)>0)
			{
				if (Fabs(cur->xPosTo-cur->xPos) > 0)
				{
					cur->speed += gameSpeed*0.2;
					cur->xPos += (cur->xPosTo > cur->xPos)?spd:-spd;
					
					if( Fabs(cur->xPosTo-cur->xPos) < Fabs(spd) )
					{
						if (fabs(cur->speed)>2)
							cur->speed = -cur->speed * 0.3;
						else
						{
							cur->speed = 0;
							cur->xPos = cur->xPosTo;
						}

						spd = cur->speed * gameSpeed;
					}
			
					
				}
				else
					if (Fabs(cur->yPosTo-cur->yPos) > 0)
					{
						cur->yPos += (cur->yPosTo > cur->yPos)?spd:-spd;
						cur->speed += gameSpeed*0.2;					

						if( Fabs(cur->yPosTo-cur->yPos) < Fabs(spd) )
						{
							if (fabs(cur->speed)>2)
							{
								cur->speed = -cur->speed * 0.3;
							}
							else
							{
								cur->speed = 0;
								cur->yPos = cur->yPosTo;
							}

							spd = cur->speed * gameSpeed;
						}
				
						
					}
			}
			
			//if( Fabs(cur->yPosTo-cur->yPos) < spd )
			//	cur->yPos = cur->yPosTo;
			//else
			//	cur->yPos += (cur->yPosTo > cur->yPos)?spd:-spd;
		


			// check animation playback type - cycle,ping-pong, etc.
			if(cur->flags & ANIMATION_CYCLE)
			{
				// sprite is cycling through frames - check if cycling forwards or backwards
				if(cur->flags & ANIMATION_FORWARDS)
				{
					// cycling forwards
					cur->currFrame = (short)cur->animTime;
					cur->animTime += cur->animSpeed * 0.4 * gameSpeed;
					if(cur->animTime > cur->numFrames)
					{
						cur->animTime = 0;
						cur->currFrame = 0;
					}
				}
				else if(cur->flags & ANIMATION_BACKWARDS)
				{
					// cycling backwards
					cur->currFrame = ROUND2SHORT(cur->animTime);
					cur->animTime -= cur->animSpeed * 0.4 * gameSpeed;
					if(cur->animTime < 0)
					{
						cur->animTime = cur->numFrames-1;
						cur->currFrame = cur->numFrames-1;
					}
				}
			}
			else if(cur->flags & ANIMATION_PINGPONG)
			{
				// sprite ping-ponging through frames - check if currently forwards or backwards
				if(cur->flags & ANIMATION_FORWARDS)
				{
					// cycling forwards
					cur->currFrame = (short)cur->animTime;
					cur->animTime += cur->animSpeed;
					if(cur->animTime > cur->numFrames)
					{
						cur->animTime = cur->numFrames-1;
						cur->currFrame = cur->numFrames-1;
						cur->flags &= ~ANIMATION_FORWARDS;
						cur->flags |= ANIMATION_BACKWARDS;
					}
				}
				else if(cur->flags & ANIMATION_BACKWARDS)
				{
					// cycling backwards
					cur->currFrame = ROUND2SHORT(cur->animTime);
					cur->animTime -= cur->animSpeed;
					if(cur->animTime < 0)
					{
						cur->animTime = 0;
						cur->currFrame = 0;
						cur->flags &= ~ANIMATION_BACKWARDS;
						cur->flags |= ANIMATION_FORWARDS;
					}
				}
			}
			else if(cur->flags & ANIMATION_RANDOMFRAMES)
			{
				// choose a random frame and display it
				cur->currFrame = rand() % cur->numFrames;
			}
			
			tEntry = ((TEXENTRY *)cur->frames[cur->currFrame]);
			
			if (tEntry)
				texture = tEntry->cFrame->hdl;
			
			//tEntry->xo = 7 * (32.0/256.0);
			//tEntry->yo = 3 * (32.0/256.0);
			numSprites++;
			if (runHardware)
			{
				if (cur->flags & XLU_ADD)
				{
					pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
					pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_ONE);
				}

				DrawAlphaSprite (cur->xPos*RES_DIFF2,cur->yPos*RES_DIFF2,0,cur->width*RES_DIFF2,cur->height*RES_DIFF2,
				0,
				0,
				1,
				1,
				texture,D3DRGBA(cur->r/255.0,cur->g/255.0,cur->b/255.0,cur->a/255.0) );

				if (cur->flags & XLU_ADD)
				{
					pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
					pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
				}

			}
		}

		cur = cur->next;
	}

}


void PrintSprite(SPRITE *sprite)
{
	VECTOR m;
	VECTOR s = {1,1,0};
	float distx,disty;
	TEXENTRY *tEntry;

	if((!sprite->texture) || (sprite->scaleX == 0) || (sprite->scaleY == 0))
		return;

	if (sprite->sc.v[Z])
	{
		tEntry = ((TEXENTRY *)sprite->texture);
		distx = disty = (FOV)/(sprite->sc.v[Z]+DIST);
		distx *= (sprite->scaleX/(64.0))*RES_DIFF;
		disty *= (sprite->scaleY/(64.0))*RES_DIFF;
		numSprites++;
		if (runHardware)
		{
			/*
			if (sprite->flags & XLU_SUBFIRST)
			{
				pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_DESTCOLOR);
				pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_DESTALPHA);

				if(sprite->flags & SPRITE_FLAGS_ROTATE)
				{
					DrawAlphaSpriteRotating(&sprite->sc.v[0],sprite->angle,sprite->sc.v[X]+sprite->offsetX*distx,sprite->sc.v[Y]+sprite->offsetY*disty,sprite->sc.v[Z]*0.00025,32*distx,32*disty,
					0,0,1,1,tEntry->cFrame->hdl,D3DRGBA(sprite->r/255.0,sprite->g/255.0,sprite->b/255.0,sprite->a/455.0) );
				}
				else
				{
					DrawAlphaSprite(sprite->sc.v[X]+sprite->offsetX*distx,sprite->sc.v[Y]+sprite->offsetY*disty,sprite->sc.v[Z]*0.00025,32*distx,32*disty,
					0,0,1,1,tEntry->cFrame->hdl,D3DRGBA(sprite->r/255.0,sprite->g/255.0,sprite->b/255.0,sprite->a/455.0) );
				}

			}
			*/
			if (sprite->flags & XLU_ADD)
			{
				pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
				pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_ONE);
			}

			if(sprite->flags & SPRITE_FLAGS_ROTATE)
			{
				DrawAlphaSpriteRotating(&sprite->sc.v[0],sprite->angle,sprite->sc.v[X]+sprite->offsetX*distx,sprite->sc.v[Y]+sprite->offsetY*disty,sprite->sc.v[Z]*0.00025,32*distx,32*disty,
				0,0,1,1,tEntry->cFrame->hdl,D3DRGBA(sprite->r/255.0,sprite->g/255.0,sprite->b/255.0,sprite->a/255.0) );
			}
			else
			{
				DrawAlphaSprite(sprite->sc.v[X]+sprite->offsetX*distx,sprite->sc.v[Y]+sprite->offsetY*disty,sprite->sc.v[Z]*0.00025,32*distx,32*disty,
				0,0,1,1,tEntry->cFrame->hdl,D3DRGBA(sprite->r/255.0,sprite->g/255.0,sprite->b/255.0,sprite->a/255.0) );
			}

			if (sprite->flags & XLU_ADD)
			{
				pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
				pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
			}

		}
	}
}


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

	if ( ( gameState.mode == INGAME_MODE ) || ( gameState.mode == PAUSE_MODE ) ||
		 ( gameState.mode == CAMEO_MODE ) )
	{
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
	VECTOR vec;
	ENEMY *nme;
	PLATFORM *plat;
	GARIB *garib;
	int i;
	long tex;
	float height;
	
	tex = (long)((TEXENTRY *)txtrSolidRing)->hdl;

	for( i=0; i<NUM_FROGS; i++ )
		if( frog[i]->actor->shadow && frog[i]->draw && frog[i]->actor->shadow->draw )
		{
			SubVector( &vec, &frog[i]->actor->pos, &currTile[i]->centre );
			height = DotProduct( &vec, &currTile[i]->normal );
			DrawShadow( &frog[i]->actor->pos, &currTile[i]->normal, frog[i]->actor->shadow->radius/max(height*0.02, 1), -height+1, frog[i]->actor->shadow->alpha/max(height*0.02, 1), tex );
		}

	//------------------------------------------------------------------------------------------------

	// process enemy shadows
	for(nme = enemyList.head.next; nme != &enemyList.head; nme = nme->next)
	{
		if( !nme->active || !nme->nmeActor )
			continue;

		if(nme->nmeActor->actor->shadow && nme->inTile && nme->nmeActor->distanceFromFrog < ACTOR_DRAWDISTANCEINNER)
		{
			SubVector( &vec, &nme->nmeActor->actor->pos, &nme->inTile->centre );
			height = DotProduct( &vec, &nme->inTile->normal );

			if (nme->path->nodes[nme->path->fromNode].worldTile==nme->inTile)
				DrawShadow( &nme->nmeActor->actor->pos, &nme->inTile->normal, nme->nmeActor->actor->shadow->radius/max(height*0.02, 1), -height+1, nme->nmeActor->actor->shadow->alpha/max(height*0.02, 1), tex );
			else
				DrawShadow( &nme->nmeActor->actor->pos, &nme->inTile->normal, nme->nmeActor->actor->shadow->radius/max(height*0.02, 1), -height+1, nme->nmeActor->actor->shadow->alpha/max(height*0.02, 1), tex );
		}
	}

	// process platform shadows
	for(plat = platformList.head.next; plat != &platformList.head; plat = plat->next)
	{
		if( !plat->active || !plat->pltActor )
			continue;

		if(plat->pltActor->actor->shadow && plat->inTile && plat->pltActor->distanceFromFrog < ACTOR_DRAWDISTANCEINNER)
		{
			SubVector( &vec, &plat->pltActor->actor->pos, &plat->inTile[0]->centre );
			height = DotProduct( &vec, &plat->inTile[0]->normal );
			DrawShadow( &plat->pltActor->actor->pos, &plat->inTile[0]->normal, plat->pltActor->actor->shadow->radius/max(height*0.02, 1), -height+1, plat->pltActor->actor->shadow->alpha/max(height*0.02, 1), tex );
		}
	}
}


void DrawShadow( VECTOR *pos, VECTOR *normal, float size, float offset, short alpha, long tex )
{
	VECTOR tempVect, m, fwd;
	D3DTLVERTEX vT[5];
	QUATERNION cross, q, up;
	long i, zeroZ=0;
	float t;

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
	guTranslateF( tMtrx, pos->v[X], pos->v[Y], pos->v[Z] );
	PushMatrix( tMtrx );

	// Rotate to be around normal
	CrossProduct( (VECTOR *)&cross, normal, &upVec );
	MakeUnit( (VECTOR *)&cross );
	t = DotProduct( normal, &upVec );
	cross.w = -acos(t);
	GetQuaternionFromRotation( &q, &cross );
	QuaternionToMatrix( &q,(MATRIX *)rMtrx);
	PushMatrix( rMtrx );

	// Transform point by combined matrix
	SetMatrix( &dMtrx, &matrixStack.stack[matrixStack.stackPosition] );

	for( i=0; i<4; i++ )
	{
		guMtxXFMF( dMtrx, vT[i].sx, vT[i].sy, vT[i].sz, &tempVect.v[X], &tempVect.v[Y], &tempVect.v[Z] );
		XfmPoint( &m, &tempVect );

		// Assign back to vT array
		vT[i].sx = m.v[X];
		vT[i].sy = m.v[Y];
		if( !m.v[Z] ) zeroZ++;
		else vT[i].sz = (m.v[Z]+DIST+4)*0.00025;
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



void DrawFXDecal( SPECFX *ripple )
{
	VECTOR tempVect, m, fwd;
	D3DTLVERTEX vT[5];
	TEXENTRY *tEntry;
	QUATERNION q1, q2, q3;
	long i, zeroZ=0;
	float t;

	if(ripple->deadCount)
		return;

	vT[0].sx = ripple->scale.v[X];
	vT[0].sy = 0;
	vT[0].sz = ripple->scale.v[Z];
	vT[0].tu = 0;
	vT[0].tv = 0;
	vT[0].color = D3DRGBA(ripple->r/255.0,ripple->g/255.0,ripple->b/255.0,ripple->a/255.0);
	vT[0].specular = D3DRGB(0,0,0);

	vT[1].sx = ripple->scale.v[X];
	vT[1].sy = 0;
	vT[1].sz = -ripple->scale.v[Z];
	vT[1].tu = 0;
	vT[1].tv = 1;
	vT[1].color = vT[0].color;
	vT[1].specular = vT[0].specular;

	vT[2].sx = -ripple->scale.v[X];
	vT[2].sy = 0;
	vT[2].sz = -ripple->scale.v[Z];
	vT[2].tu = 1;
	vT[2].tv = 1;
	vT[2].color = vT[0].color;
	vT[2].specular = vT[0].specular;
	
	vT[3].sx = -ripple->scale.v[X];
	vT[3].sy = 0;
	vT[3].sz = ripple->scale.v[Z];
	vT[3].tu = 1;
	vT[3].tv = 0;
	vT[3].color = vT[0].color;
	vT[3].specular = vT[0].specular;

	// Translate to current fx pos and push
	guTranslateF( tMtrx, ripple->origin.v[X], ripple->origin.v[Y], ripple->origin.v[Z] );
	PushMatrix( tMtrx );

	// Rotate to be around normal
	CrossProduct( (VECTOR *)&q1, &ripple->normal, &upVec );
	MakeUnit( (VECTOR *)&q1 );
	t = DotProduct( &ripple->normal, &upVec );
	q1.w = -acos(t);
	GetQuaternionFromRotation( &q2, &q1 );

	if( ripple->type == FXTYPE_GARIBCOLLECT )
	{
		// Rotate around axis
		SetVector( (VECTOR *)&q1, &ripple->normal );
		q1.w = ripple->angle;
		GetQuaternionFromRotation( &q3, &q1 );
		QuaternionMultiply( &q1, &q2, &q3 );
	}
	else SetQuaternion( &q1, &q2 );
	
	QuaternionToMatrix( &q1,(MATRIX *)rMtrx);
	PushMatrix( rMtrx );

	// Transform point by combined matrix
	SetMatrix( &dMtrx, &matrixStack.stack[matrixStack.stackPosition] );

	for( i=0; i<4; i++ )
	{
		guMtxXFMF( dMtrx, vT[i].sx, vT[i].sy, vT[i].sz, &tempVect.v[X], &tempVect.v[Y], &tempVect.v[Z] );
		XfmPoint( &m, &tempVect );

		// Assign back to vT array
		vT[i].sx = m.v[X];
		vT[i].sy = m.v[Y];
		vT[i].color = D3DRGBA(0.2,0.2,0.5,ripple->a/255.0);
		if( !m.v[Z] ) zeroZ++;
		else vT[i].sz = (m.v[Z]+DIST+4)*0.00025;
	}

	memcpy( &vT[4], &vT[0], sizeof(D3DTLVERTEX) );

	tEntry = ((TEXENTRY *)ripple->tex);
	if( tEntry && !zeroZ )
	{
		Clip3DPolygon( vT, tEntry->hdl );
		Clip3DPolygon( &vT[2], tEntry->hdl );

		if( ripple->type == FXTYPE_WAKE || ripple->type == FXTYPE_WATERRIPPLE )
		{
			for( i=0; i<4; i++ )
			{
				vT[i].sx +=3;
				vT[i].color = D3DRGBA(ripple->r/255.0,ripple->g/255.0,ripple->b/255.0,ripple->a/255.0);
			}

			SwapFrame(3);
			Clip3DPolygon( vT, tEntry->hdl );
			Clip3DPolygon( &vT[2], tEntry->hdl );
			SwapFrame(0);
		}
	}

	PopMatrix( ); // Rotation
	PopMatrix( ); // Translation
}

void DrawFXRing( SPECFX *ring )
{
	unsigned long vx, i, j;
	D3DTLVERTEX vT[4], vT2[3];
	TEXENTRY *tEntry;
	VECTOR tempVect, m;
	QUATERNION q1, q2, q3, cross;
	DWORD colour = D3DRGBA(ring->r/255.0,ring->g/255.0,ring->b/255.0,ring->a/255.0);
	float tilt, t;
	int zeroZ = 0;

	if( ring->deadCount )
		return;

	// Translate to current fx pos and push
	guTranslateF( tMtrx, ring->origin.v[X], ring->origin.v[Y], ring->origin.v[Z] );
	PushMatrix( tMtrx );

	// Rotate around axis
	SetVector( (VECTOR *)&q1, &ring->normal );
	q1.w = ring->angle;
	GetQuaternionFromRotation( &q2, &q1 );

	// Rotate to be around normal
	CrossProduct( (VECTOR *)&cross, (VECTOR *)&q1, &upVec );
	MakeUnit( (VECTOR *)&cross );
	t = DotProduct( (VECTOR *)&q1, &upVec );
	cross.w = -acos(t);
	GetQuaternionFromRotation( &q3, &cross );

	// Combine the rotations and push
	QuaternionMultiply( &q1, &q2, &q3 );
	QuaternionToMatrix( &q1,(MATRIX *)rMtrx);
	PushMatrix( rMtrx );

	for( i=0,vx=0; i < NUM_RINGSEGS; i++,vx+=4 )
	{
		memcpy( vT, &ringVtx[vx], sizeof(D3DTLVERTEX)*4 );
		// Transform to proper coords
		for( j=0; j<4; j++ )
		{
			// Slant the polys
			tilt = (j < 2) ? ring->tilt : 1;
			// Scale and push
			guScaleF( sMtrx, tilt*ring->scale.v[X], tilt*ring->scale.v[Y], tilt*ring->scale.v[Z] );
			PushMatrix( sMtrx );

			// Transform point by combined matrix
			SetMatrix( &dMtrx, &matrixStack.stack[matrixStack.stackPosition] );
			guMtxXFMF( dMtrx, vT[j].sx, vT[j].sy, vT[j].sz, &tempVect.v[X], &tempVect.v[Y], &tempVect.v[Z] );

			XfmPoint( &m, &tempVect );

			// Assign back to vT array
			vT[j].sx = m.v[X];
			vT[j].sy = m.v[Y];
			if( !m.v[Z] ) zeroZ++;
			else vT[j].sz = (m.v[Z]+DIST)*0.00025;

			vT[j].color = colour;

			PopMatrix( ); // Pop scale
		}

		tEntry = ((TEXENTRY *)ring->tex);
		if( tEntry && !zeroZ )
		{
			memcpy( &vT2[0], &vT[0], sizeof(D3DTLVERTEX) );
			memcpy( &vT2[1], &vT[2], sizeof(D3DTLVERTEX) );
			memcpy( &vT2[2], &vT[3], sizeof(D3DTLVERTEX) );
			Clip3DPolygon( vT, tEntry->hdl );
			Clip3DPolygon( vT2, tEntry->hdl );
		}
	}

	PopMatrix( ); // Rotation
	PopMatrix( ); // Translation
}


void DrawFXTrail( SPECFX *trail )
{
	unsigned long i = trail->start;
	D3DTLVERTEX vT[4], vTPrev[2];
	TEXENTRY *tEntry;

	if( trail->deadCount || (trail->start == trail->end) )
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

	do
	{
		/*********-[ First 2 points ]-********/
		if( i != trail->start && vTPrev[0].sz && vTPrev[1].sz )
			memcpy( vT, vTPrev, sizeof(D3DTLVERTEX)*2 );			// Previously transformed vertices
		else
			CalcTrailPoints( vT, trail, i );

		/*********-[ Next 2 points ]-********/
		CalcTrailPoints( &vT[2], trail, (i+1)%trail->numP );
		memcpy( vTPrev, &vT[2], sizeof(D3DTLVERTEX)*2 ); 			// Store first 2 vertices of the next segment

		/*********-[ Draw the polys ]-********/
		tEntry = ((TEXENTRY *)trail->tex);
		if( tEntry && vT[0].sz && vT[1].sz && vT[2].sz && vT[3].sz )
		{
			Clip3DPolygon( vT, tEntry->hdl );
			Clip3DPolygon( &vT[1], tEntry->hdl );
		}

		if( ++i >= trail->numP ) i=0;

	} while( i != trail->end );
}


void CalcTrailPoints( D3DTLVERTEX *vT, SPECFX *trail, int i )
{
	VECTOR pos, m;

	AddVector( &pos, &trail->origin, &trail->particles[i].pos );
	// Translate to current fx pos and push
	guTranslateF( tMtrx, pos.v[X], pos.v[Y], pos.v[Z] );
	PushMatrix( tMtrx );

	if( trail->type == FXTYPE_BILLBOARDTRAIL )	// Calculate screen align rotation
	{
		VECTOR normal;
		QUATERNION q, cross;
		float t;

		SubVector( &normal, &currCamSource, &pos );
		MakeUnit( &normal );
		CrossProduct( (VECTOR *)&cross, &normal, &upVec );
		MakeUnit( (VECTOR *)&cross );
		t = DotProduct( &normal, &upVec );
		cross.w = acos(t);
		GetQuaternionFromRotation( &q, &cross );
		QuaternionToMatrix( &q, (MATRIX *)trail->particles[i].rMtrx );
	}

	// Precalculated rotation
	PushMatrix( (MATRIX *)trail->particles[i].rMtrx );

	vT[0].sx = trail->particles[i].poly[0].v[X];
	vT[0].sy = trail->particles[i].poly[0].v[Y];
	vT[0].sz = trail->particles[i].poly[0].v[Z];
	vT[0].color = D3DRGBA(trail->particles[i].r/255.0, trail->particles[i].g/255.0, trail->particles[i].b/255.0, trail->particles[i].a/255.0);
	vT[1].sx = trail->particles[i].poly[1].v[X];
	vT[1].sy = trail->particles[i].poly[1].v[Y];
	vT[1].sz = trail->particles[i].poly[1].v[Z];
	vT[1].color = vT[0].color;

	// Transform point by combined matrix
	SetMatrix( &dMtrx, &matrixStack.stack[matrixStack.stackPosition] );

	guMtxXFMF( dMtrx, vT[0].sx, vT[0].sy, vT[0].sz, &pos.v[X], &pos.v[Y], &pos.v[Z] );
	XfmPoint( &m, &pos );
	vT[0].sx = m.v[X];
	vT[0].sy = m.v[Y];
	vT[0].sz = (m.v[Z])?((m.v[Z]+DIST)*0.00025):0;
	guMtxXFMF( dMtrx, vT[1].sx, vT[1].sy, vT[1].sz, &pos.v[X], &pos.v[Y], &pos.v[Z] );
	XfmPoint( &m, &pos );
	vT[1].sx = m.v[X];
	vT[1].sy = m.v[Y];
	vT[1].sz = (m.v[Z])?((m.v[Z]+DIST)*0.00025):0;

	PopMatrix( ); // Rotation
	PopMatrix( ); // Translation
}


void DrawFXLightning( SPECFX *fx )
{
	VECTOR tempVect, m;
	D3DTLVERTEX vT[5], vTPrev[2];
	TEXENTRY *tEntry;
	PARTICLE *p;
	long i=0;

	if( fx->deadCount )
		return;

	tEntry = ((TEXENTRY *)fx->tex);
	if( !tEntry )
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
			tempVect.v[X] = p->poly[0].v[X];
			tempVect.v[Y] = p->poly[0].v[Y];
			tempVect.v[Z] = p->poly[0].v[Z];
			XfmPoint( &m, &tempVect );
			vT[0].sx = m.v[X];
			vT[0].sy = m.v[Y];
			vT[0].tu = 0;
			vT[0].tv = 0;
			vT[0].sz = (m.v[Z])?((m.v[Z]+DIST)*0.00025):0;
			vT[0].color = D3DRGBA(p->r/255.0, p->g/255.0, p->b/255.0, p->a/255.0);

			tempVect.v[X] = p->poly[1].v[X];
			tempVect.v[Y] = p->poly[1].v[Y];
			tempVect.v[Z] = p->poly[1].v[Z];
			XfmPoint( &m, &tempVect );
			vT[1].sx = m.v[X];
			vT[1].sy = m.v[Y];
			vT[1].tu = 1;
			vT[1].tv = 0;
			vT[1].sz = (m.v[Z])?((m.v[Z]+DIST)*0.00025):0;
			vT[1].color = vT[0].color;
		}

		// Now the next two, to make a quad
		tempVect.v[X] = p->next->poly[1].v[X];
		tempVect.v[Y] = p->next->poly[1].v[Y];
		tempVect.v[Z] = p->next->poly[1].v[Z];
		XfmPoint( &m, &tempVect );
		vT[2].sx = m.v[X];
		vT[2].sy = m.v[Y];
		vT[2].tu = 1;
		vT[2].tv = 1;
		vT[2].sz = (m.v[Z])?((m.v[Z]+DIST)*0.00025):0;
		vT[2].color = vT[0].color;

		tempVect.v[X] = p->next->poly[0].v[X];
		tempVect.v[Y] = p->next->poly[0].v[Y];
		tempVect.v[Z] = p->next->poly[0].v[Z];
		XfmPoint( &m, &tempVect );
		vT[3].sx = m.v[X];
		vT[3].sy = m.v[Y];
		vT[3].tu = 0;
		vT[3].tv = 1;
		vT[3].sz = (m.v[Z])?((m.v[Z]+DIST)*0.00025):0;
		vT[3].color = vT[0].color;

		// Store first 2 vertices of the next segment
		memcpy( vTPrev, &vT[3], sizeof(D3DTLVERTEX) );
		memcpy( &vTPrev[1], &vT[2], sizeof(D3DTLVERTEX) );
		// And back to the first vertex for the second tri
		memcpy( &vT[4], &vT[0], sizeof(D3DTLVERTEX) );

		// Draw polys, if they're not clipped
		if( vT[0].sz && vT[1].sz && vT[2].sz && vT[3].sz )
		{
			Clip3DPolygon( vT, tEntry->hdl );
			Clip3DPolygon( &vT[2], tEntry->hdl );
		}

		i++;
		p = p->next;
	} 

	SwapFrame(0);
}


void DrawRandomPolyList( )
{
	POLYGON *p;

	for( p=rpList; p; p=p->next )
		TransformAndDrawPolygon( p );
}

void TransformAndDrawPolygon( POLYGON *p )
{
	VECTOR tempVect, m, fwd;
	D3DTLVERTEX vT[5];
	QUATERNION cross, q1, q2, q3, up;
	long i, zeroZ=0;
	unsigned long tex;
	float t;

	vT[0].sx = p->vT[0].v[X];
	vT[0].sy = p->vT[0].v[Y];
	vT[0].sz = p->vT[0].v[Z];
	vT[0].tu = p->u;
	vT[0].tv = p->v;
	vT[0].color = D3DRGBA(p->r/255.0,p->g/255.0,p->b/255.0,p->a/255.0);
	vT[0].specular = D3DRGB(0,0,0);

	vT[1].sx = p->vT[1].v[X];
	vT[1].sy = p->vT[1].v[Y];
	vT[1].sz = p->vT[1].v[Z];
	vT[1].tu = p->u;
	vT[1].tv = p->v1;
	vT[1].color = vT[0].color;
	vT[1].specular = vT[0].specular;

	vT[2].sx = p->vT[2].v[X];
	vT[2].sy = p->vT[2].v[Y];
	vT[2].sz = p->vT[2].v[Z];
	vT[2].tu = p->u1;
	vT[2].tv = p->v1;
	vT[2].color = vT[0].color;
	vT[2].specular = vT[0].specular;
	
	vT[3].sx = p->vT[3].v[X];
	vT[3].sy = p->vT[3].v[Y];
	vT[3].sz = p->vT[3].v[Z];
	vT[3].tu = p->u1;
	vT[3].tv = p->v;
	vT[3].color = vT[0].color;
	vT[3].specular = vT[0].specular;

	// Translate to current fx pos and push
	guTranslateF( tMtrx, p->plane.point.v[X], p->plane.point.v[Y], p->plane.point.v[Z] );
	PushMatrix( tMtrx );

	// Rotate around axis
	SetVector( (VECTOR *)&q1, &p->plane.normal );
	q1.w = p->angle;
	GetQuaternionFromRotation( &q2, &q1 );

	// Rotate to be around normal
	CrossProduct( (VECTOR *)&cross, &p->plane.normal, &upVec );
	MakeUnit( (VECTOR *)&cross );
	t = DotProduct( &p->plane.normal, &upVec );
	cross.w = -acos(t);
	GetQuaternionFromRotation( &q3, &cross );

	// Combine the rotations and push
	QuaternionMultiply( &q1, &q2, &q3 );
	QuaternionToMatrix( &q1,(MATRIX *)rMtrx);
	PushMatrix( rMtrx );

	// Transform point by combined matrix
	SetMatrix( &dMtrx, &matrixStack.stack[matrixStack.stackPosition] );

	for( i=0; i<4; i++ )
	{
		guMtxXFMF( dMtrx, vT[i].sx, vT[i].sy, vT[i].sz, &tempVect.v[X], &tempVect.v[Y], &tempVect.v[Z] );
		XfmPoint( &m, &tempVect );

		// Assign back to vT array
		vT[i].sx = m.v[X];
		vT[i].sy = m.v[Y];
		if( !m.v[Z] ) zeroZ++;
		else vT[i].sz = (m.v[Z]+DIST)*0.00025;
	}

	memcpy( &vT[4], &vT[0], sizeof(D3DTLVERTEX) );

	tex = ((TEXENTRY *)p->tex)->hdl;

	if( tex && !zeroZ )
	{
		Clip3DPolygon( vT, tex );
		Clip3DPolygon( &vT[2], tex );
	}

	PopMatrix( ); // Rotation
	PopMatrix( ); // Translation
}

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
			Clip3DPolygon( vT, tEntry->hdl );
			Clip3DPolygon( &vT[1], tEntry->hdl );
		}

		tmp = vT1;
		vT1 = vT2;
		vT2 = tmp;

		i++;
	}
}
*/
void DrawTongue( TONGUE *t )
{
	unsigned long i=0, index = (int)(t->progress*(MAX_TONGUENODES-1));
	D3DTLVERTEX vT[4], vTPrev[2];
	TEXENTRY *tEntry;

	if( index < 2 )
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
		tEntry = ((TEXENTRY *)t->tex);
		if( tEntry && vT[0].sz && vT[1].sz && vT[2].sz && vT[3].sz )
		{
			Clip3DPolygon( vT, tEntry->hdl );
			Clip3DPolygon( &vT[1], tEntry->hdl );
		}

		i++;
	}
}


void CalcTongueNodes( D3DTLVERTEX *vT, TONGUE *t, int i )
{
	QUATERNION q, cross;
	float p;
	VECTOR pos, m, normal;

	SetVector( &pos, &t->segment[i] );
	// Translate to current fx pos and push
	guTranslateF( tMtrx, pos.v[X], pos.v[Y], pos.v[Z] );
	PushMatrix( tMtrx );

	SubVector( &normal, &currCamSource, &pos );
	MakeUnit( &normal );
	CrossProduct( (VECTOR *)&cross, &normal, &upVec );
	MakeUnit( (VECTOR *)&cross );
	p = DotProduct( &normal, &upVec );
	cross.w = -acos(p);
	GetQuaternionFromRotation( &q, &cross );
	QuaternionToMatrix( &q, (MATRIX *)rMtrx );

	// Precalculated rotation
	PushMatrix( (MATRIX *)rMtrx );

	vT[0].sx = -5+(i*0.4);
	vT[0].sy = 0;
	vT[0].sz = 0;
	vT[0].color = D3DRGBA(1,0,0,1);
	vT[1].sx = 5-(i*0.4);
	vT[1].sy = 0;
	vT[1].sz = 0;
	vT[1].color = vT[0].color;

	// Transform point by combined matrix
	SetMatrix( &dMtrx, &matrixStack.stack[matrixStack.stackPosition] );

	guMtxXFMF( dMtrx, vT[0].sx, vT[0].sy, vT[0].sz, &pos.v[X], &pos.v[Y], &pos.v[Z] );
	XfmPoint( &m, &pos );
	vT[0].sx = m.v[X];
	vT[0].sy = m.v[Y];
	vT[0].sz = (m.v[Z])?((m.v[Z]+DIST)*0.00025):0;
	guMtxXFMF( dMtrx, vT[1].sx, vT[1].sy, vT[1].sz, &pos.v[X], &pos.v[Y], &pos.v[Z] );
	XfmPoint( &m, &pos );
	vT[1].sx = m.v[X];
	vT[1].sy = m.v[Y];
	vT[1].sz = (m.v[Z])?((m.v[Z]+DIST)*0.00025):0;

	PopMatrix( ); // Rotation
	PopMatrix( ); // Translation
}
