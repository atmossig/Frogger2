
#include "particle.h"
#include "specfx.h"
#include "Eff_Draw.h"
#include "cam.h"
#include "enemies.h"
#include "platform.h"
#include "collect.h"
#include "frogmove.h"
#include "main.h"
#include "tongue.h"
#include "map_draw.h"
#include "actor2.h"
#include "fixed.h"
#include "layout.h"


#define gte_stotz_cpu(r)\
asm(\
  "mfc2 %0, $7;"\
  :\
  : "r" (r)\
)

#define addPrimLen(ot,p,len,tempvar) \
{\
	tempvar = *(u_long *)(ot); \
    (*(u_long *)(ot) = (u_long)(p)); \
	tempvar |= (len<<24); \
	*(unsigned long *)(p) = tempvar; \
}

void DrawSpecialFX ( void )
{
	int i;

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

		for( i=0; i<NUM_FROGS; i++ )
			if( tongue[i].flags & TONGUE_BEINGUSED )
				DrawTongue( i );
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

		//bbopt - use clipped flag
		if(nme->nmeActor->actor->shadow && nme->inTile && nme->nmeActor->distanceFromFrog < ACTOR_DRAWDISTANCEINNER )
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

		if(plat->pltActor->actor->shadow && plat->inTile && plat->pltActor->distanceFromFrog < ACTOR_DRAWDISTANCEINNER )
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
/*
 	unsigned long  vx, i, j, colour;
 	SVECTOR vT[4];
	FVECTOR tempV[4];
 	IQUATERNION q1, q2, q3, cross;
 	fixed tilt, t;
 
	//PUTS THE SPRITES RGB'S IN COLOUR, FIRST HALVING THEIR VALUES
	colour = ring->r>>1;
	colour += (ring->g>>1)<<8;
	colour += (ring->b>>1)<<16;

 
	//Rotate around axis
 	SetVectorFF((FVECTOR *)&q1, &ring->normal);
 	q1.w = ring->angle;
 	fixedGetQuaternionFromRotation( &q2, &q1 );
 
	//Rotate to be around normal
 	CrossProductFFF((FVECTOR *)&cross, (FVECTOR *)&q1, &upVec );
 	MakeUnit((FVECTOR *)&cross );
	t = DotProductFF( (FVECTOR *)&q1, &upVec );
	cross.w = -arccos(t);
 	fixedGetQuaternionFromRotation( &q3, &cross );

	//Combine the rotations
 	fixedQuaternionMultiply( &q1, &q2, &q3 );
 
	for( i=0,vx=0; i < NUM_RINGSEGS; i++,vx+=4 )
	{
		POLY_FT4 *ft4;
		LONG width;
		LONG height;
		LONG distancescale;
		LONG z;
		VERT tempVect;

		memcpy( tempV, &ringVtx[vx], sizeof(FVECTOR)*4 );

		//Transform to proper coords
 		for( j=0; j<4; j++ )
 		{
			//Slant the polys
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

 		}//end looping seg's 4 verts

#ifdef PC_VERSION
		Print3D3DSprite ( ring, vT, colour );
#else
		//draw a sprite at the first vertex of the 4
		//(pc uses 4 for a poly)

		//draw 
		tempVect.vx = -vT[0].vx;
		tempVect.vy = -vT[0].vy;
		tempVect.vz =  vT[0].vz;

 		ft4 = (POLY_FT4*)currentDisplayPage->primPtr;
 		setPolyFT4(ft4);
 		setRGB0(ft4, (colour&255),((colour>>8)&255),((colour>>16)&255));

		//scary scaling-and-transform-in-one-go code from the Action Man people...
//bb
//		width = (spr->texture->w);
		width = ring->tex->w;

		gte_SetLDDQB(0);		//clear offset control reg (C2_DQB)

		gte_ldv0(&tempVect);

		gte_SetLDDQA(width);	//shove sprite width into control reg (C2_DQA)
		gte_rtps();				//do the rtps
		gte_stsxy(&ft4->x0);	//get screen x and y
		gte_stszotz(&z);		//get order table z
		//end of scaling-and-transform


		//tbd - make this ditch according to on-screen SIZE
		//limit to "max poly depth", and we can ditch the "MAXDEPTH" "and" below...
		if(z < 20) return 0;


		gte_stopz(&distancescale);		// get scaled width of sprite

//		width = (distancescale * (spr->scaleX * 25 )) >> 24;
		width = (distancescale * (ring->scale.vx * 25 )) >> 24;
		width /= 2;

		//mike	if(width < 3)
		if(width < 2)
			return 0;	// better max-distance check

 		ft4->x1=ft4->x3=ft4->x0+width;
 		ft4->x0=ft4->x2=ft4->x0-width;
   		if(ft4->x1 < -256)
			return 0;
   		if(ft4->x0 > 256)
			return 0;

	//bbtest
//		height = (distancescale * (spr->scaleY * 25)) >> 24;
		height = (distancescale * (ring->scale.vy * 25)) >> 24;
		height /= 2;
		height /= 2;
	
//		if(spr->texture->w == spr->texture->h)
//			height = width;	//(distancescale * scale) >> 8;
//		else
//			height = width * spr->texture->h / spr->texture->w;
	

		ft4->y2=ft4->y3=ft4->y0+height;
		ft4->y1=ft4->y0=ft4->y0-height;
   		if (ft4->y2< -128) return 0;
   		if (ft4->y0> 128) return 0;

		*(USHORT*)&ft4->u0=			*(USHORT*)&(ring->tex)->u0;	// u0,v0, and clut info
		*(USHORT*)&ft4->u1=			*(USHORT*)&(ring->tex)->u1;	// u1,v1, and tpage info
		*(USHORT*)&ft4->u2=			*(USHORT*)&(ring->tex)->u2;	// plus pad1
		*(USHORT*)&ft4->u3=			*(USHORT*)&(ring->tex)->u3;	// plus pad2
		*(USHORT*)&ft4->clut=		*(USHORT*)&(ring->tex)->clut;	// plus pad2
		*(USHORT*)&ft4->tpage=		*(USHORT*)&(ring->tex)->tpage;	// plus pad2



//		if(spr->flags & SPRITE_TRANSLUCENT )//Only do additive if the 'sprite' fades or is translucent
		if(ring->flags & SPRITE_TRANSLUCENT )//Only do additive if the 'sprite' fades or is translucent
		{
		// SL: modge the RGBs accordingly
	  		ft4->r0 = ((ring->a*(short)ft4->r0)/255);
	  		ft4->g0 = ((ring->a*(short)ft4->g0)/255);
	  		ft4->b0 = ((ring->a*(short)ft4->b0)/255);

		// Make additive
			ft4->code |= 2;
			ft4->tpage |= 32;
		}

 		addPrim(currentDisplayPage->ot+(z>>2), ft4);
 		currentDisplayPage->primPtr += sizeof(POLY_FT4);
#endif
 	
	}//end looping segments
*/



	unsigned long vx, i, j, vxj;
	SVECTOR vT[5], vTPrev[2];
	TextureType *tEntry;
	VECTOR tempVect;
	VECTOR m;
	SVECTOR tempSvect;
	FVECTOR normal, scale, tempFVect;
	IQUATERNION q1, q2, q3, cross;
	SVECTOR fxpos, pos;
	fixed tilt, tilt2, t;
	int zeroZ = 0;
	MATRIX tempMtx;

	if( !(tEntry=ring->tex) )
		return;

	SetVectorFF(&scale, &ring->scale);
	SetVectorFF(&normal, &ring->normal);
	SetVectorSS(&pos, &ring->origin);

//	ScaleVectorFF(&scale, 410);
//	ScaleVectorSF(&pos, 410);

	//Translate to current fx pos and push
//	guTranslateF( tMtrx, pos.vx, pos.vy, pos.vz );
//	PushMatrix( tMtrx );
//bb needs to be after QuatToPSXMatrix (or we could stop it zeroing pos)
//	tempMtx.t[0] = pos.vx;
//	tempMtx.t[1] = pos.vy;
//	tempMtx.t[2] = pos.vz;

	//Rotate around axis (q2)
	SetVectorFF((FVECTOR*)&q1, &normal);
//	q1.w = fx->angle/57.6;
	q1.w = ring->angle;
//	q1.w = 0;
	fixedGetQuaternionFromRotation(&q2, &q1);

	//Rotate to be around normal (q3)
	CrossProductFFF((FVECTOR *)&cross, (FVECTOR *)&q1, &upVec);
	MakeUnit((FVECTOR *)&cross);
	t = DotProductFF((FVECTOR *)&q1, &upVec);
	cross.w = -arccos(t);
	fixedGetQuaternionFromRotation(&q3, &cross);

	//Combine the rotations and push
	fixedQuaternionMultiply(&q1, &q3, &q2);
	QuatToPSXMatrix(&q3, &tempMtx);
//	PushMatrix( rMtrx );
//	tempMtx.t[0] = -pos.vx;
//	tempMtx.t[1] = -pos.vy;
//	tempMtx.t[2] =  pos.vz;

//	if( fx->type == FXTYPE_CROAK )
//		SwapFrame(3);

//	tilt2 = (float)fx->tilt*0.000244;
	tilt2 = ring->tilt;

	for( i=0,vx=0; i < NUM_RINGSEGS; i++,vx+=2 )
	{
		// Transform to proper coords
		for( j=0,zeroZ=0; j<4; j++ )
		{
			if( i && j<2 && vTPrev[0].vz && vTPrev[1].vz )
				memcpy( vT, vTPrev, sizeof(SVECTOR)*2 );
			else
			{
				vxj = (vx+j)%(NUM_RINGSEGS<<1);

				//Slant the polys
				tilt = (!(i&1)?(j==0||j==3):(j==1||j==2)) ? 4096 : tilt2;

				//Scale and push
//				tempFVect.vx = FMul(tilt,scale.vx);
//				tempFVect.vy = FMul(tilt,scale.vy);
//				tempFVect.vz = FMul(tilt,scale.vz);
//				tempFVect.vx = scale.vx;
//				tempFVect.vy = scale.vy;
//				tempFVect.vz = scale.vz;
//				ScaleMatrix(&tempMtx, &tempFVect);

//				vT[j].vx = ringVtx[vxj].vx>>12;//;*0.000244;
//				vT[j].vy = ringVtx[vxj].vy>>12;//;*0.000244;
//				vT[j].vz = ringVtx[vxj].vz>>12;//;*0.000244;
//				vT[j].vx = ringVtx[vxj].vx>>1;
//				vT[j].vy = ringVtx[vxj].vy>>1;
//				vT[j].vz = ringVtx[vxj].vz>>1;
//				vT[j].vx = ringVtx[vxj].vx;
//				vT[j].vy = ringVtx[vxj].vy;
///				vT[j].vz = ringVtx[vxj].vz;

//				vT[j].vx += ring->origin.vx;
//				vT[j].vy += ring->origin.vy;
//				vT[j].vz += ring->origin.vz;

				//Transform point by combined matrix
//				ApplyMatrix(&tempMtx, &vT[j], &tempVect);
//				SetVectorVS(&tempVect, &vT[j]);
//				SetVectorVF(&tempVect, &vT[j]);
				tempVect.vx = 

//				tempVect.vx += ring->origin.vx;
//				tempVect.vy += ring->origin.vy;
//				tempVect.vz += ring->origin.vz;
				SetVectorSV(&tempSvect, &tempVect);

				if(j==3)
//					SetVectorSV(&fxpos, &tempVect);
					SetVectorSF(&fxpos, &tempVect);

//				XfmPoint( &m, &tempVect, NULL );
				gte_SetTransMatrix(&GsWSMATRIX);
				gte_SetRotMatrix(&GsWSMATRIX);
				gte_ldv0(&tempSvect);
				gte_rtps();
				gte_stsxy(&m.vx);
				gte_stsz(&m.vz);	//screen z/4 as otz
				

				// Assign back to vT array
				vT[j].vx = m.vx;
				vT[j].vy = m.vy;
				if(!m.vz)
					zeroZ++;
				else
//					vT[j].sz = (m.vz+DIST)*0.00025;
					vT[j].vz = m.vz;
//					vT[j].sz = m.vz+ring->zDepthOff;

//				PopMatrix( ); // Pop scale
			}
		}

		if( !zeroZ )
		{
			POLY_FT4 *ft4;
			int width;

			memcpy(vTPrev, &vT[2], sizeof(SVECTOR)*2);
			memcpy(&vT[4], &vT[0], sizeof(SVECTOR));
//			Clip3DPolygon( vT, tEntry );
//			Clip3DPolygon( &vT[2], tEntry );

			//set up a poly
			BEGINPRIM(ft4, POLY_FT4);
			setPolyFT4(ft4);
//			*(long*)(&ft4->x0) = *(long*)(&vt[0].vx);
//			*(long*)(&ft4->x1) = *(long*)(&vt[1].vx);
//			*(long*)(&ft4->x2) = *(long*)(&vt[2].vx);
//			*(long*)(&ft4->x3) = *(long*)(&vt[3].vx);
			width = (ring->tex->w*xFOV)/vT[0].vz;
			width>>=2;

			ft4->x0 = vT[0].vx - width;
			ft4->y0 = vT[0].vy - width;
			ft4->x1 = vT[0].vx + width;
			ft4->y1 = vT[0].vy - width;
			ft4->x2 = vT[0].vx - width;
			ft4->y2 = vT[0].vy + width;
			ft4->x3 = vT[0].vx + width;
			ft4->y3 = vT[0].vy + width;
			ft4->r0 = 128;
			ft4->g0 = 128;
			ft4->b0 = 128;
			ft4->u0 = tEntry->u0;
			ft4->v0 = tEntry->v0;
			ft4->u1 = tEntry->u1;
			ft4->v1 = tEntry->v1;
			ft4->u2 = tEntry->u2;
			ft4->v2 = tEntry->v2;
			ft4->u3 = tEntry->u3;
			ft4->v3 = tEntry->v3;
			ft4->tpage = tEntry->tpage;
			ft4->clut  = tEntry->clut;
			ENDPRIM(ft4, 1, POLY_FT4);
//			ENDPRIM(ft4, vT[0].vz>>2, POLY_FT4);


/*			if( (i&1) && (actFrameCount&1) )
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
*/		}
		else
		{
			vTPrev[0].vz = vTPrev[1].vz = 0;
		}
	}

//	PopMatrix( ); // Rotation
//	PopMatrix( ); // Translation

//	if( fx->type == FXTYPE_CROAK )
//		SwapFrame(0);
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
/*
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
*/

//	MDX_VECTOR tempVect, m;
	SVECTOR vT[5], vTPrev[2];
	long otz;
	TextureType *tEntry;
	PARTICLE *p;
	long i=0;

	if( !(tEntry = (fx->tex)) )
		return;

	// Additive mode
//	SwapFrame(3);

	gte_SetTransMatrix(&GsWSMATRIX);
	gte_SetRotMatrix(&GsWSMATRIX);
	p = fx->particles;
	while( i < fx->numP-1 )
	{
		SVECTOR tempSvect;

		// Copy in previous transformed vertices, if they exist
		if( i && vTPrev[0].vz && vTPrev[1].vz )
			memcpy( vT, vTPrev, sizeof(SVECTOR)*2 );
		else
		{
			// Otherwise, transform them again
//			tempVect.vx = p->poly[0].vx;//*0.1;
//			tempVect.vy = p->poly[0].vy;//*0.1;
//			tempVect.vz = p->poly[0].vz;//*0.1;
//			XfmPoint( &m, &tempVect, NULL );
//			gte_ldv0(&p->poly[0]);
			SetVectorSS(&tempSvect, &p->poly[0]);
			tempSvect.vx = -tempSvect.vx;
			tempSvect.vy = -tempSvect.vy;
			gte_ldv0(&tempSvect);
			gte_rtps();
			gte_stsxy(&vT[0].vx);
			gte_stotz(&otz);
			vT[0].vz = otz;
//			vT[0].sx = m.vx;
//			vT[0].sy = m.vy;
//			vT[0].rhw = 1;
//			vT[0].sz = (m.vz)?((m.vz+DIST)*0.00025):0;
//			vT[0].color = D3DRGBA(p->r/255.0, p->g/255.0, p->b/255.0, p->a/255.0);

//			tempVect.vx = p->poly[1].vx*0.1;
//			tempVect.vy = p->poly[1].vy*0.1;
//			tempVect.vz = p->poly[1].vz*0.1;
//			XfmPoint( &m, &tempVect, NULL );
//			gte_ldv0(&p->poly[1]);
			SetVectorSS(&tempSvect, &p->poly[1]);
			tempSvect.vx = -tempSvect.vx;
			tempSvect.vy = -tempSvect.vy;
			gte_ldv0(&tempSvect);
			gte_rtps();
			gte_stsxy(&vT[1].vx);
			gte_stotz(&otz);
			vT[1].vz = otz;
//			vT[1].sx = m.vx;
//			vT[1].sy = m.vy;
//			vT[1].rhw = 1;
//			vT[1].sz = (m.vz)?((m.vz+DIST)*0.00025):0;
//			vT[1].color = vT[0].color;
		}

		// Now the next two, to make a quad
//		tempVect.vx = p->next->poly[1].vx*0.1;
//		tempVect.vy = p->next->poly[1].vy*0.1;
//		tempVect.vz = p->next->poly[1].vz*0.1;
//		XfmPoint( &m, &tempVect, NULL );
//		vT[2].sx = m.vx;
//		vT[2].sy = m.vy;
//		vT[2].rhw = 1;
//		vT[2].sz = (m.vz)?((m.vz+DIST)*0.00025):0;
//		vT[2].color = vT[0].color;
//		gte_ldv0(&p->next->poly[1]);
		SetVectorSS(&tempSvect, &p->next->poly[1]);
		tempSvect.vx = -tempSvect.vx;
		tempSvect.vy = -tempSvect.vy;
		gte_ldv0(&tempSvect);
		gte_rtps();
		gte_stsxy(&vT[2].vx);
		gte_stotz(&otz);
		vT[2].vz = otz;


//		tempVect.vx = p->next->poly[0].vx*0.1;
//		tempVect.vy = p->next->poly[0].vy*0.1;
//		tempVect.vz = p->next->poly[0].vz*0.1;
//		XfmPoint( &m, &tempVect, NULL );
//		vT[3].sx = m.vx;
//		vT[3].sy = m.vy;
//		vT[3].rhw = 1;
//		vT[3].sz = (m.vz)?((m.vz+DIST)*0.00025):0;
//		vT[3].color = vT[0].color;
//		gte_ldv0(&p->next->poly[0]);
		SetVectorSS(&tempSvect, &p->next->poly[0]);
		tempSvect.vx = -tempSvect.vx;
		tempSvect.vy = -tempSvect.vy;
		gte_ldv0(&tempSvect);
		gte_rtps();
		gte_stsxy(&vT[3].vx);
		gte_stotz(&otz);
		vT[3].vz = otz;

		// Store first 2 vertices of the next segment
		memcpy( vTPrev, &vT[3], sizeof(SVECTOR) );
		memcpy( &vTPrev[1], &vT[2], sizeof(SVECTOR) );
		// And back to the first vertex for the second tri
		memcpy( &vT[4], &vT[0], sizeof(SVECTOR) );

		// Draw polys, if they're not clipped
		if( vT[0].vz && vT[1].vz && vT[2].vz && vT[3].vz )
		{
/*			vT[0].specular = D3DRGB(0,0,0);
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
			vT[4].specular = D3DRGB(0,0,0);
			vT[4].tu = 1;
			vT[4].tv = 1;

			Clip3DPolygon( vT, tEntry );
			Clip3DPolygon( &vT[2], tEntry );
*/
			POLY_FT4 *ft4;
//			int zAvg = (vT[0].vz + vT[1].vz + vT[2].vz + vT[3].vz)/4;

			otz = (vT[0].vz+vT[1].vz+vT[2].vz+vT[3].vz)/4;
			otz += fx->zDepthOff;


			//draw poly
			BEGINPRIM(ft4, POLY_FT4);
			setPolyFT4(ft4);
			*((long*)&ft4->x0) = *((long*)&vT[0].vx);
			*((long*)&ft4->x1) = *((long*)&vT[1].vx);
			*((long*)&ft4->x2) = *((long*)&vT[2].vx);
			*((long*)&ft4->x3) = *((long*)&vT[3].vx);
//			ft4->r0 = p->r;
//			ft4->g0 = p->g;
//			ft4->b0 = p->b;
			ft4->r0 = (p->r*p->a)>>8;
			ft4->g0 = (p->g*p->a)>>8;
			ft4->b0 = (p->b*p->a)>>8;
			ft4->u0 = tEntry->u0;
			ft4->v0 = tEntry->v0;
			ft4->u1 = tEntry->u1;
			ft4->v1 = tEntry->v1;
			ft4->u2 = tEntry->u2;
			ft4->v2 = tEntry->v2;
			ft4->u3 = tEntry->u3;
			ft4->v3 = tEntry->v3;
			ft4->tpage = tEntry->tpage;
			ft4->clut  = tEntry->clut;
	//		setSemiTrans(ft4, 1);
			//ft4->code  |= 2;//semi-trans on
 			//ft4->tpage |= 32;//add
	// 		ft4->tpage = si->tpage | 64;//sub
	//		ENDPRIM(ft4, 1, POLY_FT4);
			ENDPRIM(ft4, otz, POLY_FT4);
		}

		i++;
		p = p->next;
	} 

//	SwapFrame(0);
}
 
SVECTOR *psxCroakVtx = NULL;
void CreatePsxCroakVtx(void)
{
	fixed angStep = 4096/NUM_PSX_CROAK_VTX;
	fixed ang=0;
	int i;

	if(!psxCroakVtx)
	{
		psxCroakVtx = MALLOC0( sizeof(VECTOR) * NUM_PSX_CROAK_VTX );
		if(!psxCroakVtx)
			utilPrintf("\n\nMALLOC ERROR ! ! ! psxCroakVtx\n\n");
	}


	for(i=0; i<NUM_PSX_CROAK_VTX; i++)
	{
		//coords with radius 4096
		psxCroakVtx[i].vx = rsin(ang);
		psxCroakVtx[i].vy = 0;
		psxCroakVtx[i].vz = rcos(ang);

		//set radius to 512 (1 tile)
		psxCroakVtx[i].vx /= 8;
		psxCroakVtx[i].vz /= 8;

		ang += angStep;
	}
}

void UpdatePsxCroak(SPECFX *ring)
{
	int fo;
	fixed speed;

 	if( ring->follow )
 		SetVectorSS( &ring->origin, &ring->follow->position );

	fo = (ring->fade * gameSpeed)>>12;
	if( ring->a > fo ) ring->a -= fo;
	else ring->a = 0;

	ring->speed += FMul(ring->accn, gameSpeed);
	speed = FMul(ring->speed, gameSpeed);

	ring->scale.vx += speed;
	ring->scale.vy += speed>>2;
	ring->scale.vz += speed;

	ring->angle += (ring->spin * gameSpeed)>>12;
	
	if( actFrameCount > ring->lifetime )
		DeallocateFX(ring,1);
}

void DrawPsxCroak(SPECFX *ring)
{
	IQUATERNION q1, q2, q3, cross;
	fixed t;
	MATRIX rotMtx;
	VECTOR absPos;
	SVECTOR scPos;
	long sz;
	int otz;
	POLY_FT4 *ft4;
	int width, height;
	int i;

	for(i=0; i<NUM_PSX_CROAK_VTX; i++)
	{
		//quat to rotate current point by spinning angle
/*		SetVectorFF( (FVECTOR*)&q1, &ring->normal );
		q1.w = ring->angle;
		fixedGetQuaternionFromRotation( &q2, &q1 );

		//quat to rotate to plane (normal)
		CrossProductFFF( (FVECTOR*)&cross, (FVECTOR*)&q1, &upVec );
		MakeUnit( (FVECTOR*)&cross );
		t = DotProductFF( (FVECTOR*)&q1, &upVec );
		cross.w = -arccos(t);
		fixedGetQuaternionFromRotation(&q3, &cross);

		//combine and apply quats to current point
		fixedQuaternionMultiply(&q1, &q3, &q2);
		QuatToPSXMatrix(&q1, &rotMtx);
		ApplyMatrix(&rotMtx, &psxCroakVtx[i], &absPos);

		//center around effect's origin
		absPos.vx += ring->origin.vx;
		absPos.vy += ring->origin.vy;
		absPos.vz += ring->origin.vz;
*/

		//spinning circle
		SetVectorFF( (FVECTOR*)&q1, &ring->normal );
		q1.w = ring->angle;
		fixedGetQuaternionFromRotation( &q2, &q1 );

		QuatToPSXMatrix(&q2, &rotMtx);
		ApplyMatrix(&rotMtx, &psxCroakVtx[i], &absPos);

		//apply scale
//		absPos.vx = (absPos.vx*ring->scale.vx)>>12;
//		absPos.vy = (absPos.vy*ring->scale.vy)>>12;
//		absPos.vz = (absPos.vz*ring->scale.vz)>>12;
		absPos.vx = FMul(absPos.vx, ring->scale.vx);
		absPos.vy = FMul(absPos.vy, ring->scale.vy);
		absPos.vz = FMul(absPos.vz, ring->scale.vz);
		
		//position around centre
		absPos.vx += -ring->origin.vx;
		absPos.vy += -ring->origin.vy;
		absPos.vz += ring->origin.vz;



		//calc screen xyz
		gte_SetTransMatrix(&GsWSMATRIX);
		gte_SetRotMatrix(&GsWSMATRIX);
//		gte_ldv0(&absPos);
		gte_ldlv0(&absPos);
		gte_rtps();
		gte_stsxy(&scPos.vx);
//		gte_stsz(&scPos.vz);	//screen z/4 as otz
		gte_stsz(&sz);	//screen z/4 as otz
		gte_stotz(&otz);	//screen z/4 as otz

		scPos.vz = sz;

		//calc width
		if(scPos.vz)
		{
			width = (ring->tex->w*xFOV)/scPos.vz;
			width >>= 1;

			height = (ring->tex->w*yFOV)/scPos.vz;
			height >>= 1;
		}
		else
			width = 1;
		
		//draw poly
		BEGINPRIM(ft4, POLY_FT4);
		setPolyFT4(ft4);
		ft4->x0 = scPos.vx-width;
		ft4->y0 = scPos.vy-width;
		ft4->x1 = scPos.vx+width;
		ft4->y1 = scPos.vy-width;
		ft4->x2 = scPos.vx-width;
		ft4->y2 = scPos.vy+width;
		ft4->x3 = scPos.vx+width;
		ft4->y3 = scPos.vy+width;
//		ft4->r0 = 128;
//		ft4->g0 = 128;
//		ft4->b0 = 128;
		ft4->r0 = ring->a;
		ft4->g0 = ring->a;
		ft4->b0 = ring->a;
//		ft4->r0 = (ring->r*ring->a)>>8;
//		ft4->g0 = (ring->g*ring->a)>>8;
//		ft4->b0 = (ring->b*ring->a)>>8;
		ft4->u0 = ring->tex->u0;
		ft4->v0 = ring->tex->v0;
		ft4->u1 = ring->tex->u1;
		ft4->v1 = ring->tex->v1;
		ft4->u2 = ring->tex->u2;
		ft4->v2 = ring->tex->v2;
		ft4->u3 = ring->tex->u3;
		ft4->v3 = ring->tex->v3;
		ft4->tpage = ring->tex->tpage;
		ft4->clut  = ring->tex->clut;
//		setSemiTrans(ft4, 1);
		ft4->code  |= 2;//semi-trans on
 		ft4->tpage |= 32;//add
// 		ft4->tpage = si->tpage | 64;//sub
//		ENDPRIM(ft4, 1, POLY_FT4);
		ENDPRIM(ft4, otz, POLY_FT4);
	}
}
