#define F3DEX_GBI

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
void CreateAndAdd3DText( char *str, unsigned long w, char r, char g, char b, char a, short type, unsigned long motion, VECTOR *spd, float rSpd, long xO, long yO, long zO )
{
	TEXT3D *t;
	TEXT3D *t3d = (TEXT3D *)JallocAlloc(sizeof(TEXT3D),YES,"Text3D");
	unsigned long len = strlen(str);
	float tmp = PI2; // Don't ask why I have to do this, it's due to N64 remedial maths

	t3d->width = w;
	// Scale factor - desired width over normal width of texture (32*numChars)
	t3d->scale = (float)w/((float)len*32);
	t3d->angle = 0;

	t3d->vel.v[0] = spd->v[0];
	t3d->vel.v[1] = spd->v[1];
	t3d->vel.v[2] = spd->v[2];
	t3d->rSpeed = rSpd;
	
	t3d->radius = (float)w / tmp;
	t3d->prev = t3d->next = NULL;
	t3d->string = str;
	t3d->vR = r;
	t3d->vG = g;
	t3d->vB = b;
	t3d->tileSize = t3d->scale*32;
	t3d->vA = a;

	t3d->xOffs = t3d->oldXOffs = xO;
	t3d->yOffs = t3d->oldYOffs = yO;
	t3d->zOffs = t3d->oldZOffs = zO;

	t3d->motion = motion;
	t3d->type = type;

	t3d->vT = (Vtx *)JallocAlloc(sizeof(Vtx)*len*4, NO, "Vtx");

	// Finally, add new text3D to global list
	t = text3DList.head.next;
	t3d->prev = t;
	t3d->next = t->next;
	t->next->prev = t3d;
	t->next = t3d;
	text3DList.numEntries++;
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
	unsigned long v, c, len;
	char texName[13];
	TEXTURE *texture;

	QUATERNION q;
	float transMtx[4][4],rotMtx[4][4],tempMtx[4][4];
	float newRotMtx[4][4];

	gDPPipeSync(glistp++);

	gSPDisplayList(glistp++,polyNoZ_dl);

	for( t3d=text3DList.head.next; t3d!=&text3DList.head; t3d=t3d->next )
	{
		if( !(t3d->motion & T3D_CALCULATED) )
			continue;
		
		guTranslateF(transMtx,-0.5,-0.5,10);

		NormalToQuaternion(&q,&inVec);
		QuaternionToMatrix(&q,(MATRIX *)rotMtx);
		guMtxCatF(rotMtx,transMtx,tempMtx);

		guMtxF2L(tempMtx,&dynamicp->modeling4[objectMatrix]);
		gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->modeling4[objectMatrix++])),
											G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);

		gDPSetPrimColor(glistp++,0,0,255,255,255,255);
		gDPSetEnvColor(glistp++,128,128,128,255);

		len = strlen(t3d->string);
		for( c=0,v=0; c < len; c++,v+=4 )
		{
			gSPVertex(glistp++,&t3d->vT[v],4,0);

			switch( t3d->string[c] )
			{
			case '0': cmemcpy( texName, "zero_32.bmp\0", 12 ); break;
			case '1': cmemcpy( texName, "one_32.bmp\0", 11 ); break;
			case '2': cmemcpy( texName, "two_32.bmp\0", 11 ); break;
			case '3': cmemcpy( texName, "three_32.bmp\0", 13 ); break;
			case '4': cmemcpy( texName, "four_32.bmp\0", 12 ); break;
			case '5': cmemcpy( texName, "five_32.bmp\0", 12 ); break;
			case '6': cmemcpy( texName, "six_32.bmp\0", 11 ); break;
			case '7': cmemcpy( texName, "seven_32.bmp\0", 13 ); break;
			case '8': cmemcpy( texName, "eight_32.bmp\0", 13 ); break;
			case '9': cmemcpy( texName, "nine_32.bmp\0", 12 ); break;
			case ' ': cmemcpy( texName, "_\0", 2 ); break;
			default:
				cmemcpy( texName, "__32.bmp\0", 9 );
				texName[0] = t3d->string[c];
				break; // Character is a letter
			}

			if( texName[0] != '_' )
			{
				FindTexture( &texture, UpdateCRC(texName), YES, texName );

				gDPLoadTextureBlock_4b(glistp++,texture->data,G_IM_FMT_RGBA,texture->sx,texture->sy,
										0,G_TX_CLAMP|G_TX_NOMIRROR,G_TX_CLAMP|G_TX_NOMIRROR,
										0,0,G_TX_NOLOD,G_TX_NOLOD);

				gSP2Triangles(glistp++,0,1,2,0,2,3,0,0);
			}
		}

		gDPPipeSync(glistp++);
	}

	gSPPopMatrix(glistp++,G_MTX_MODELVIEW);
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
	Vtx *vPtr = t3d->vT;
	float tesa, tesb, teca,tecb;
	float arcStep = PI2 / strlen(t3d->string);
	int v;
	float pB, radians = t3d->angle/57.6;
	unsigned long len = strlen(t3d->string), i;
	unsigned int tS = t3d->tileSize;

	for( i=0; i<len; i++ )
	{
		pB = (i*arcStep)+radians;
		v = i*4;

		tesa = t3d->xOffs+t3d->radius*(sinf(pB));
		tesb = t3d->xOffs+t3d->radius*(sinf(pB+arcStep));
		teca = t3d->zOffs+t3d->radius*(cosf(pB));
		tecb = t3d->zOffs+t3d->radius*(cosf(pB+arcStep));

		V((&vPtr[v+0]),tesa,	teca,	t3d->yOffs,		0,	0,		0,		t3d->vR,t3d->vG,t3d->vB,t3d->vA);
		V((&vPtr[v+1]),tesb,	tecb,	t3d->yOffs,		0,	1024,	0,		t3d->vR,t3d->vG,t3d->vB,t3d->vA);
		V((&vPtr[v+2]),tesb,	tecb,	t3d->yOffs+tS,	0,	1024,	1024,	t3d->vR,t3d->vG,t3d->vB,t3d->vA);
		V((&vPtr[v+3]),tesa,	teca,	t3d->yOffs+tS,	0,	0,		1024,	t3d->vR,t3d->vG,t3d->vB,t3d->vA);
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
	Vtx *vPtr = t3d->vT;
	float pB;
	int v;
	unsigned long len = strlen(t3d->string), i;
	unsigned int tS = t3d->tileSize;

	if( t3d->type == T3D_HORIZONTAL )
	{
		for( i=0; i<len; i++ )
		{
			pB = i*tS;
			v = i*4;
			V((&vPtr[v+0]),-pB+t3d->xOffs,		t3d->zOffs,	t3d->yOffs,		0,		0,		0,		t3d->vR,t3d->vG,t3d->vB,t3d->vA);
			V((&vPtr[v+1]),-pB-tS+t3d->xOffs,	t3d->zOffs,	t3d->yOffs,		0,		1024,	0,		t3d->vR,t3d->vG,t3d->vB,t3d->vA);
			V((&vPtr[v+2]),-pB-tS+t3d->xOffs,	t3d->zOffs,	tS+t3d->yOffs,	0,		1024,	1024,	t3d->vR,t3d->vG,t3d->vB,t3d->vA);
			V((&vPtr[v+3]),-pB+t3d->xOffs,		t3d->zOffs,	tS+t3d->yOffs,	0,		0,		1024,	t3d->vR,t3d->vG,t3d->vB,t3d->vA);
		}
	}
	else
	{
		for( i=0; i<len; i++ )
		{
			pB = i*tS;
			v = i*4;
			V((&vPtr[v+0]),t3d->xOffs,		t3d->zOffs,	pB+t3d->yOffs,			0,		0,		0,		t3d->vR,t3d->vG,t3d->vB,t3d->vA);
			V((&vPtr[v+1]),-tS+t3d->xOffs,	t3d->zOffs,	pB+t3d->yOffs,			0,		1024,	0,		t3d->vR,t3d->vG,t3d->vB,t3d->vA);
			V((&vPtr[v+2]),-tS+t3d->xOffs,	t3d->zOffs,	pB+tS+t3d->yOffs,		0,		1024,	1024,	t3d->vR,t3d->vG,t3d->vB,t3d->vA);
			V((&vPtr[v+3]),t3d->xOffs,		t3d->zOffs,	pB+tS+t3d->yOffs,		0,		0,		1024,	t3d->vR,t3d->vG,t3d->vB,t3d->vA);
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

	if( t3d->motion & T3D_MOVE_SPIN )
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

		// z check
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

		// z check
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

		// z check
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

			t = t3d->next;
			JallocFree( (UBYTE **)&t3d );
			t3d = t;
		}
	}
}
