#include <ultra64.h>
#include "incs.h"

TEXT3DLIST text3DList;


/*	--------------------------------------------------------------------------------
	Function		: CreateAndAdd3DText
	Purpose			: Make a series of polys such that character textures can be 
						mapped onto the polys
	Parameters		: Far too many
	Returns			: 
	Info			: Uses TEXT3D structure
*/
void CreateAndAdd3DText( char *str, unsigned long w, char r, char g, char b, char a, short type, unsigned long motion, VECTOR *spd, float rSpd, float initAngle, long xO, long yO, long zO, float sinA, float sinS, float twA )
{
	TEXT3D *t;
	TEXT3D *t3d = (TEXT3D *)JallocAlloc(sizeof(TEXT3D),YES,"Text3D");
	unsigned long len = strlen(str);
	float tmp = PI2; // Don't ask why I have to do this, it's due to N64 remedial maths

	t3d->width = w;
	// Scale factor - desired width over normal width of texture (32*numChars)
	t3d->scale = (float)w/((float)len*32);

	t3d->angle = initAngle;
	
	t3d->sinA = sinA;
	t3d->sinS = sinS;
	t3d->twistA = twA;

	t3d->vel.v[0] = spd->v[0];
	t3d->vel.v[1] = spd->v[1];
	t3d->vel.v[2] = spd->v[2];
	t3d->rSpeed = rSpd;
	
	t3d->radius = (float)w / tmp;
	t3d->prev = t3d->next = NULL;
	t3d->string = (char *)JallocAlloc(len+1,YES,"String");
	sprintf( t3d->string, "%s\0", str );
	t3d->vR = r;
	t3d->vG = g;
	t3d->vB = b;
	t3d->tileSize = t3d->scale*32;
	t3d->vA = a;

	t3d->xOffs = xO;
	t3d->yOffs = yO;
	t3d->zOffs = zO;

	t3d->motion = motion;
	t3d->type = type;

	//t3d->timer = T360_TIMER; // Default value

	t3d->vT = (D3DTLVERTEX *)JallocAlloc(sizeof(D3DTLVERTEX)*len*4, NO, "D3DTLVERTEX");

	// Finally, add new text3D to global list
	t = text3DList.head.next;
	t3d->prev = t;
	t3d->next = t->next;
	t->next->prev = t3d;
	t->next = t3d;
	text3DList.numEntries++;

	t3d->motion |= T3D_CREATED;
}


/*	--------------------------------------------------------------------------------
	Function		: Print3DText
	Purpose			: Draw all TEXT3D structs in list. Must load in the appropriate
						texture for each character of the string and scale it to the poly.
	Parameters		: 
	Returns			: 
	Info			: 
*/
void Print3DText( )
{
	TEXT3D *t3d;
	unsigned long vx, c, len, i;

	VECTOR m, tmp;
	QUATERNION q;
	float transMtx[4][4],rotMtx[4][4],tempMtx[4][4];
	float newRotMtx[4][4];
	short u, v, letterID;
	static short f[6] = {0,1,2,0,2,3};

	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
	pDirect3DDevice->lpVtbl->SetRenderState(pDirect3DDevice,D3DRENDERSTATE_ZWRITEENABLE,FALSE);

	guTranslateF(transMtx,-0.5,-0.5,10);

	NormalToQuaternion(&q,&inVec);
	QuaternionToMatrix(&q,(MATRIX *)rotMtx);
	guMtxCatF(rotMtx,transMtx,tempMtx);

	PushMatrix(&tempMtx[0][0]);

	for( t3d=text3DList.head.next; t3d!=&text3DList.head; t3d=t3d->next )
	{
		if( !((t3d->motion & T3D_CALCULATED) && (t3d->motion & T3D_CREATED)) )
			continue;
		
		len = strlen(t3d->string);
		for( c=0,vx=0; c < len; c++,vx+=4 )
		{
			letterID = characterMap[t3d->string[c]];
			u = bigFont->offset[letterID].v[X];
			v = bigFont->offset[letterID].v[Y];
			t3d->vT[vx+0].tu = u;
			t3d->vT[vx+0].tv = v;
			t3d->vT[vx+1].tu = u+32;
			t3d->vT[vx+1].tv = v;
			t3d->vT[vx+2].tu = u+32;
			t3d->vT[vx+2].tv = v+32;
			t3d->vT[vx+3].tu = u;
			t3d->vT[vx+3].tv = v+32;

			// Transform to screen coords (I HOPE)
			for( i=0; i<4; i++ )
			{
				tmp.v[0] = t3d->vT[vx+i].sx;
				tmp.v[1] = t3d->vT[vx+i].sy;
				tmp.v[2] = t3d->vT[vx+i].sz;
				XfmPoint( &m, &tmp );
				t3d->vT[vx+i].sx = m.v[0];
				t3d->vT[vx+i].sy = m.v[1];
				t3d->vT[vx+i].sz = m.v[2];
			}

			DrawAHardwarePoly(t3d->vT,4,f,6,bigFont->hdl);
		}
	}

	PopMatrix( );
}

/*	--------------------------------------------------------------------------------
	Function		: Calculate3DText
	Purpose			: Do vertex recalculation here so gfx thread is happy
	Parameters		: 
	Returns			: 
	Info			: 
*/
void Calculate3DText( )
{
	TEXT3D *t3d;
	
	for( t3d=text3DList.head.next; t3d!=&text3DList.head; t3d=t3d->next )
	{
		if( !(t3d->motion & T3D_CREATED) )
			continue;

		if( t3d->type == T3D_CIRCLE )
			MakeTextCircle( t3d );
		else
			MakeTextLine( t3d );

		UpdateT3DMotion( t3d );

		t3d->motion |= T3D_CALCULATED;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: MakeTextCircle
	Purpose			: Calculate vertices of text so they lie in a circle
	Parameters		: 
	Returns			: 
	Info			: 
*/
void MakeTextCircle( TEXT3D *t3d )
{
	float tesa, tesb, teca,tecb;
	float arcStep = PI2 / strlen(t3d->string);
	int v;
	float pB, radians = t3d->angle/57.6;
	unsigned long len = strlen(t3d->string), i;
	unsigned int tS = t3d->tileSize;
	float yPa, yPb, yPc, yPd;

	for( i=0; i<len; i++ )
	{
		pB = (i*arcStep)+radians;
		v = i*4;

		tesa = t3d->xOffs+t3d->radius*(sinf(pB));
		tesb = t3d->xOffs+t3d->radius*(sinf(pB+arcStep));

		teca = t3d->zOffs+t3d->radius*(cosf(pB));
		tecb = t3d->zOffs+t3d->radius*(cosf(pB+arcStep));

		yPa = t3d->yOffs;
		yPb = yPa;
		yPc = yPb+tS;
		yPd = yPc;

		if( t3d->motion & T3D_MOVE_SQUISH )
		{
			float sf1 = sinf(pB+frameCount*t3d->sinS)*t3d->sinA,
				sf2 = sinf(pB+tS+frameCount*t3d->sinS)*t3d->sinA;
			yPa += sf1;
			yPb = yPa;
			yPc += sf2;
			yPd = yPc;
		}
		if( t3d->motion & T3D_MOVE_SINE )
		{
			float sf1 = sinf(i+frameCount*t3d->sinS)*t3d->sinA,
				sf2 = sinf(i+1+frameCount*t3d->sinS)*t3d->sinA;
			yPa += sf1;
			yPb += sf2;
			yPc += sf2;
			yPd += sf1;
		}

		t3d->vT[v+0].sx = tesa;
		t3d->vT[v+0].sy = teca;
		t3d->vT[v+0].sz = yPa;//+DIST)/2000;
		t3d->vT[v+0].color = D3DRGBA(t3d->vR,t3d->vG,t3d->vB,t3d->vA);
		t3d->vT[v+0].specular = D3DRGB(0,0,0);
		t3d->vT[v+1].sx = tesb;
		t3d->vT[v+1].sy = tecb;
		t3d->vT[v+1].sz = yPb;//+DIST)/2000;
		t3d->vT[v+1].color = D3DRGBA(t3d->vR,t3d->vG,t3d->vB,t3d->vA);
		t3d->vT[v+1].specular = D3DRGB(0,0,0);
		t3d->vT[v+2].sx = tesb;
		t3d->vT[v+2].sy = tecb;
		t3d->vT[v+2].sz = yPc;//+DIST)/2000;
		t3d->vT[v+2].color = D3DRGBA(t3d->vR,t3d->vG,t3d->vB,t3d->vA);
		t3d->vT[v+2].specular = D3DRGB(0,0,0);
		t3d->vT[v+3].sx = tesa;
		t3d->vT[v+3].sy = teca;
		t3d->vT[v+3].sz = yPd;//+DIST)/2000;
		t3d->vT[v+3].color = D3DRGBA(t3d->vR,t3d->vG,t3d->vB,t3d->vA);
		t3d->vT[v+3].specular = D3DRGB(0,0,0);
	}
}


/*	--------------------------------------------------------------------------------
	Function		: MakeTextLine
	Purpose			: Calculate vertices of text so they form a line
	Parameters		: 
	Returns			: 
	Info			: 
*/
void MakeTextLine( TEXT3D *t3d )
{
	float pB;
	int v;
	unsigned long len = strlen(t3d->string), i;
	unsigned int tS = t3d->tileSize;

	if( t3d->type == T3D_HORIZONTAL )
	{
		float yPa, yPb, yPc, yPd,
			zPa, zPb, zPc, zPd;

		for( i=0; i<len; i++ )
		{
			pB = i*tS;
			v = i*4;

			yPa = t3d->yOffs;
			yPb = yPa;
			yPc = yPb+tS;
			yPd = yPc;
			zPa = zPb = zPc = zPd = t3d->zOffs;

			if( t3d->motion & T3D_MOVE_SQUISH )
			{
				float sf1 = sinf(pB+frameCount*t3d->sinS)*t3d->sinA,
					sf2 = sinf(pB+tS+frameCount*t3d->sinS)*t3d->sinA;
				yPa += sf1;
				yPb = yPa;
				yPc += sf2;
				yPd = yPc;
			}
			if( t3d->motion & T3D_MOVE_SINE )
			{
				float sf1 = sinf(i+frameCount*t3d->sinS)*t3d->sinA,
					sf2 = sinf(i+1+frameCount*t3d->sinS)*t3d->sinA;
				yPa += sf1;
				yPb += sf2;
				yPc += sf2;
				yPd += sf1;
			}
			if( t3d->motion & T3D_MOVE_SPIN )
			{
				float radians = t3d->angle / 57.6,
					twa = t3d->twistA+radians,
					sfa = sinf(twa)*tS,
					sfb = cosf(twa)*tS;
					
				yPa = t3d->yOffs + sfa;
				yPb = t3d->yOffs + sfa;
				yPc = t3d->yOffs - sfa;
				yPd = t3d->yOffs - sfa;
				zPa = t3d->zOffs + sfb;
				zPb = t3d->zOffs + sfb;
				zPc = t3d->zOffs - sfb;
				zPd = t3d->zOffs - sfb;
			}
			if( t3d->motion & T3D_MOVE_TWIST )
			{
				float radians = t3d->angle / 57.6,
					twa = (i*t3d->twistA)+radians,
					twb = ((i+1)*t3d->twistA)+radians,
					sfy1 = sinf(twa)*tS,
					sfy2 = sinf(twb)*tS,
					sfz1 = cosf(twa)*tS,
					sfz2 = cosf(twb)*tS;
					
				yPa = t3d->yOffs + sfy1;
				yPb = t3d->yOffs + sfy2;
				yPc = t3d->yOffs - sfy2;
				yPd = t3d->yOffs - sfy1;
				zPa = t3d->zOffs + sfz1;
				zPb = t3d->zOffs + sfz2;
				zPc = t3d->zOffs - sfz2;
				zPd = t3d->zOffs - sfz1;
			}

			t3d->vT[v+0].sx = -pB+t3d->xOffs;
			t3d->vT[v+0].sy = zPa;
			t3d->vT[v+0].sz = yPa;//+DIST)/2000;
			t3d->vT[v+0].color = D3DRGBA(t3d->vR,t3d->vG,t3d->vB,t3d->vA);
			t3d->vT[v+0].specular = D3DRGB(0,0,0);
			t3d->vT[v+1].sx = -pB-tS+t3d->xOffs;
			t3d->vT[v+1].sy = zPb;
			t3d->vT[v+1].sz = yPb;//+DIST)/2000;
			t3d->vT[v+1].color = D3DRGBA(t3d->vR,t3d->vG,t3d->vB,t3d->vA);
			t3d->vT[v+1].specular = D3DRGB(0,0,0);
			t3d->vT[v+2].sx = -pB-tS+t3d->xOffs;
			t3d->vT[v+2].sy = zPc;
			t3d->vT[v+2].sz = yPc;//+DIST)/2000;
			t3d->vT[v+2].color = D3DRGBA(t3d->vR,t3d->vG,t3d->vB,t3d->vA);
			t3d->vT[v+2].specular = D3DRGB(0,0,0);
			t3d->vT[v+3].sx = -pB+t3d->xOffs;
			t3d->vT[v+3].sy = zPd;
			t3d->vT[v+3].sz = yPd;//+DIST)/2000;
			t3d->vT[v+3].color = D3DRGBA(t3d->vR,t3d->vG,t3d->vB,t3d->vA);
			t3d->vT[v+3].specular = D3DRGB(0,0,0);
		}
	}
	else
	{
		float xPa, xPb, xPc, xPd,
			zPa, zPb, zPc, zPd,
			yPa, yPb, yPc, yPd;

		for( i=0; i<len; i++ )
		{
			pB = i*tS;
			v = i*4;

			xPa = t3d->xOffs;
			xPb = xPa-tS;
			xPc = xPb;
			xPd = xPa;
			yPa = pB+t3d->yOffs;
			yPb = yPa;
			yPc = yPb+tS;
			yPd = yPc;
			zPa = zPb = zPc = zPd = t3d->zOffs;

			if( t3d->motion & T3D_MOVE_SQUISH )
			{
				float sf1 = sinf(pB+frameCount*t3d->sinS)*t3d->sinA,
					sf2 = sinf(pB+tS+frameCount*t3d->sinS)*t3d->sinA;
				xPa += sf1;
				xPb += sf2;
				xPc = xPb;
				xPd = xPa;
			}
			if( t3d->motion & T3D_MOVE_SINE )
			{
				float sf1 = sinf(i+frameCount*t3d->sinS)*t3d->sinA,
					sf2 = sinf(i+1+frameCount*t3d->sinS)*t3d->sinA;
				xPa += sf1;
				xPb += sf1;
				xPc += sf2;
				xPd += sf2;
			}
			if( t3d->motion & T3D_MOVE_SPIN )
			{
				float radians = t3d->angle / 57.6,
					twa = t3d->twistA+radians,
					sfa = sinf(twa)*tS,
					sfb = cosf(twa)*tS;
					
				xPa = t3d->xOffs + sfa;
				xPb = t3d->xOffs - sfa;
				xPc = t3d->xOffs - sfa;
				xPd = t3d->xOffs + sfa;
				zPa = t3d->zOffs + sfb;
				zPb = t3d->zOffs - sfb;
				zPc = t3d->zOffs - sfb;
				zPd = t3d->zOffs + sfb;
			}
			if( t3d->motion & T3D_MOVE_TWIST )
			{
				float radians = t3d->angle / 57.6,
					twa = (i*t3d->twistA)+radians,
					twb = ((i+1)*t3d->twistA)+radians,
					sfx1 = sinf(twa)*tS,
					sfx2 = sinf(twb)*tS,
					sfz1 = cosf(twa)*tS,
					sfz2 = cosf(twb)*tS;
					
				xPa = t3d->xOffs + sfx1;
				xPb = t3d->xOffs - sfx1;
				xPc = t3d->xOffs - sfx2;
				xPd = t3d->xOffs + sfx2;
				zPa = t3d->zOffs + sfz1;
				zPb = t3d->zOffs - sfz1;
				zPc = t3d->zOffs - sfz2;
				zPd = t3d->zOffs + sfz2;
			}
			/*
			if( t3d->motion & T3D_MOVE_360 && !t3d->timer )
			{
				float radians = t3d->angle / 57.6,
					twa = (i*t3d->twistA)+radians,
					twb = ((i+1)*t3d->twistA)+radians,
					sfx1 = sinf(twa)*tS,
					sfx2 = sinf(twb)*tS,
					sfz1 = cosf(twa)*tS,
					sfz2 = cosf(twb)*tS;

				xPa = t3d->xOffs + sfx1;
				xPb = t3d->xOffs - sfx1;
				xPc = t3d->xOffs - sfx2;
				xPd = t3d->xOffs + sfx2;
				zPa = t3d->zOffs + sfz1;
				zPb = t3d->zOffs - sfz1;
				zPc = t3d->zOffs - sfz2;
				zPd = t3d->zOffs + sfz2;
			}
			*/
			t3d->vT[v+0].sx = xPa;
			t3d->vT[v+0].sy = zPa;
			t3d->vT[v+0].sz = yPa;//+DIST)/2000;
			t3d->vT[v+0].color = D3DRGBA(t3d->vR,t3d->vG,t3d->vB,t3d->vA);
			t3d->vT[v+0].specular = D3DRGB(0,0,0);
			t3d->vT[v+1].sx = xPb;
			t3d->vT[v+1].sy = zPb;
			t3d->vT[v+1].sz = yPb;//+DIST)/2000;
			t3d->vT[v+1].color = D3DRGBA(t3d->vR,t3d->vG,t3d->vB,t3d->vA);
			t3d->vT[v+1].specular = D3DRGB(0,0,0);
			t3d->vT[v+2].sx = xPc;
			t3d->vT[v+2].sy = zPc;
			t3d->vT[v+2].sz = yPc;//+DIST)/2000;
			t3d->vT[v+2].color = D3DRGBA(t3d->vR,t3d->vG,t3d->vB,t3d->vA);
			t3d->vT[v+2].specular = D3DRGB(0,0,0);
			t3d->vT[v+3].sx = xPd;
			t3d->vT[v+3].sy = zPd;
			t3d->vT[v+3].sz = yPd;//+DIST)/2000;
			t3d->vT[v+3].color = D3DRGBA(t3d->vR,t3d->vG,t3d->vB,t3d->vA);
			t3d->vT[v+3].specular = D3DRGB(0,0,0);
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateT3DMotion
	Purpose			: Move text, check bounds and reset flags.
	Parameters		: Text3d object
	Returns			: 
	Info			: 
*/
void UpdateT3DMotion( TEXT3D *t3d )
{
	int temp;

	if( (t3d->motion & T3D_MOVE_SPIN) || (t3d->motion & T3D_MOVE_TWIST) )
	{
		t3d->angle += t3d->rSpeed;
		if( t3d->angle >= 360 )
			t3d->angle = 0;
		else if( t3d->angle <= 0 )
			t3d->angle = 360;
	}

	if( (t3d->motion & T3D_MOVE_LEFT) || (t3d->motion & T3D_MOVE_RIGHT) )
		t3d->xOffs += t3d->vel.v[0];

	if( (t3d->motion & T3D_MOVE_UP) || (t3d->motion & T3D_MOVE_DOWN) )
		t3d->yOffs += t3d->vel.v[1];

	if( (t3d->motion & T3D_MOVE_IN) || (t3d->motion & T3D_MOVE_OUT) )
		t3d->zOffs += t3d->vel.v[2];
/*
	if( t3d->motion & T3D_MOVE_360 )
	{
		if( t3d->timer )
			t3d->timer--;
		else
		{
			t3d->angle += t3d->rSpeed;

			if( t3d->angle >= 360 )
				t3d->angle = 0;
			else if( t3d->angle <= 0 )
				t3d->angle = 360;
		}
	}
*/
	/*
	*	The rest of the function is checking whether the text is offscreen, and
	*	resetting parameters if it is.
	*/

	if( t3d->type == T3D_CIRCLE ) // Circular text formation
	{
		if( t3d->xOffs >= 110+t3d->radius ) // Off left
		{
			if( t3d->motion & T3D_MOVE_LEFT )
			{
				if( t3d->motion & T3D_PATH_BOUNCE )
				{
					t3d->motion &= ~T3D_MOVE_LEFT;
					t3d->motion |= T3D_MOVE_RIGHT;
					t3d->vel.v[0] *= -1;
				}
				else if( t3d->motion & T3D_PATH_LOOP )
				{
					t3d->xOffs = -110-t3d->radius;
				}
			}
		}
		else if( t3d->xOffs <= -110-t3d->radius ) // Off right
		{
			if( t3d->motion & T3D_MOVE_RIGHT )
			{
				if( t3d->motion & T3D_PATH_BOUNCE )
				{
					t3d->motion &= ~T3D_MOVE_RIGHT;
					t3d->motion |= T3D_MOVE_LEFT;
					t3d->vel.v[0] *= -1;
				}
				else if( t3d->motion & T3D_PATH_LOOP )
				{
					t3d->xOffs = 110+t3d->radius;
				}
			}
		}
	}
	else if( t3d->type == T3D_HORIZONTAL )
	{
		if( t3d->xOffs >= 110+t3d->width ) // Off left
		{
			if( t3d->motion & T3D_MOVE_LEFT )
			{
				if( t3d->motion & T3D_PATH_BOUNCE )
				{
					t3d->motion &= ~T3D_MOVE_LEFT;
					t3d->motion |= T3D_MOVE_RIGHT;
					t3d->vel.v[0] *= -1;
				}
				else if( t3d->motion & T3D_PATH_LOOP )
				{
					t3d->xOffs = -110;
				}
			}
		}
		else if( t3d->xOffs <= -110 ) // Off right
		{
			if( t3d->motion & T3D_MOVE_RIGHT )
			{
				if( t3d->motion & T3D_PATH_BOUNCE )
				{
					t3d->motion &= ~T3D_MOVE_RIGHT;
					t3d->motion |= T3D_MOVE_LEFT;
					t3d->vel.v[0] *= -1;
				}
				else if( t3d->motion & T3D_PATH_LOOP )
				{
					t3d->xOffs = 110+t3d->width;
				}
			}
		}
	}
	else if( t3d->type == T3D_VERTICAL )
	{
		temp = 110+t3d->tileSize;
		if( t3d->xOffs >= temp ) // Off left
		{
			if( t3d->motion & T3D_MOVE_LEFT )
			{
				if( t3d->motion & T3D_PATH_BOUNCE )
				{
					t3d->motion &= ~T3D_MOVE_LEFT;
					t3d->motion |= T3D_MOVE_RIGHT;
					t3d->vel.v[0] *= -1;
				}
				else if( t3d->motion & T3D_PATH_LOOP )
				{
					t3d->xOffs = -110;
				}
			}
		}
		else if( t3d->xOffs <= -110 ) // Off right
		{
			if( t3d->motion & T3D_MOVE_RIGHT )
			{
				if( t3d->motion & T3D_PATH_BOUNCE )
				{
					t3d->motion &= ~T3D_MOVE_RIGHT;
					t3d->motion |= T3D_MOVE_LEFT;
					t3d->vel.v[0] *= -1;
				}
				else if( t3d->motion & T3D_PATH_LOOP )
				{
					t3d->xOffs = 110+t3d->tileSize;
				}
			}
		}
	}

	if( t3d->type == T3D_VERTICAL )
	{
		if( (t3d->yOffs+t3d->width <= -100) && (t3d->motion & T3D_MOVE_UP) ) // Off the top
		{
			if( t3d->motion & T3D_PATH_BOUNCE )
			{
				t3d->motion &= ~T3D_MOVE_UP;
				t3d->motion |= T3D_MOVE_DOWN;
				t3d->vel.v[1] *= -1;
			}
			else if( t3d->motion & T3D_PATH_LOOP )
			{
				t3d->yOffs = 100;
			}
		}
		else if( t3d->yOffs >= 100 && (t3d->motion & T3D_MOVE_DOWN) )
		{
			if( t3d->motion & T3D_PATH_BOUNCE )
			{
				t3d->motion &= ~T3D_MOVE_DOWN;
				t3d->motion |= T3D_MOVE_UP;
				t3d->vel.v[1] *= -1;
			}
			else if( t3d->motion & T3D_PATH_LOOP )
			{
				t3d->yOffs = -100-t3d->width;
			}
		}
	}
	else
	{
		temp = 100+t3d->tileSize;

		if( (t3d->yOffs <= -temp) && (t3d->motion & T3D_MOVE_UP) ) // Off the top
		{
			if( t3d->motion & T3D_PATH_BOUNCE )
			{
				t3d->motion &= ~T3D_MOVE_UP;
				t3d->motion |= T3D_MOVE_DOWN;
				t3d->vel.v[1] *= -1;
			}
			else if( t3d->motion & T3D_PATH_LOOP )
			{
				t3d->yOffs = 100;
			}
		}
		else if( t3d->yOffs >= 100 && (t3d->motion & T3D_MOVE_DOWN) )
		{
			if( t3d->motion & T3D_PATH_BOUNCE )
			{
				t3d->motion &= ~T3D_MOVE_DOWN;
				t3d->motion |= T3D_MOVE_UP;
				t3d->vel.v[1] *= -1;
			}
			else if( t3d->motion & T3D_PATH_LOOP )
			{
				t3d->yOffs = -100-t3d->tileSize;
			}
		}
	}
	/* CAN'T DO Z BOUNCING BECAUSE OF MATTS WEIRD 3D-NESS
	if( t3d->type == T3D_CIRCLE )
	{
		if( t3d->zOffs+t3d->radius >= farPlaneDist && (t3d->motion & T3D_MOVE_OUT) )
		{
			if( t3d->motion & T3D_PATH_BOUNCE )
			{
				t3d->motion &= ~T3D_MOVE_OUT;
				t3d->motion |= T3D_MOVE_IN;
				t3d->vel.v[2] *= -1;
			}
			else if( t3d->motion & T3D_PATH_LOOP )
			{
				t3d->zOffs = nearPlaneDist+t3d->radius;
			}
		}
		else if( t3d->zOffs-t3d->radius <= nearPlaneDist && (t3d->motion & T3D_MOVE_IN) )
		{
			if( t3d->motion & T3D_PATH_BOUNCE )
			{
				t3d->motion &= ~T3D_MOVE_IN;
				t3d->motion |= T3D_MOVE_OUT;
				t3d->vel.v[2] *= -1;
			}
			else if( t3d->motion & T3D_PATH_LOOP )
			{
				t3d->yOffs = farPlaneDist-t3d->radius;
			}
		}
	}
	else
	{
		if( t3d->zOffs >= farPlaneDist && (t3d->motion & T3D_MOVE_OUT) )
		{
			if( t3d->motion & T3D_PATH_BOUNCE )
			{
				t3d->motion &= ~T3D_MOVE_OUT;
				t3d->motion |= T3D_MOVE_IN;
				t3d->vel.v[2] *= -1;
			}
			else if( t3d->motion & T3D_PATH_LOOP )
			{
				t3d->zOffs = nearPlaneDist;
			}
		}
		else if( t3d->zOffs <= nearPlaneDist && (t3d->motion & T3D_MOVE_IN) )
		{
			if( t3d->motion & T3D_PATH_BOUNCE )
			{
				t3d->motion &= ~T3D_MOVE_IN;
				t3d->motion |= T3D_MOVE_OUT;
				t3d->vel.v[2] *= -1;
			}
			else if( t3d->motion & T3D_PATH_LOOP )
			{
				t3d->yOffs = farPlaneDist;
			}
		}
	}
	*/
}

/*	--------------------------------------------------------------------------------
	Function		: Init3DTextList
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void Init3DTextList( )
{
	text3DList.head.next = text3DList.head.prev = &text3DList.head;
	text3DList.numEntries = 0;
}


/*	--------------------------------------------------------------------------------
	Function		: Sub3DText
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void Sub3DText( TEXT3D *t )
{
	t->prev->next = t->next;
	t->next->prev = t->prev;
	text3DList.numEntries--;
}


/*	--------------------------------------------------------------------------------
	Function		: Free3DTextList
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void Free3DTextList( )
{
	TEXT3D *t3d, *t;

	for( t3d = text3DList.head.next; t3d != &text3DList.head; )
	{
		if( t3d != NULL )
		{
			Sub3DText( t3d );

			JallocFree( (UBYTE **)&t3d->string );
			JallocFree( (UBYTE **)&t3d->vT );

			t = t3d->next;
			JallocFree( (UBYTE **)&t3d );
			t3d = t;
		}
	}
}
