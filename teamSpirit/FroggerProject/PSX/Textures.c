#define VRAM_STARTX			512
#define VRAM_PAGECOLS		8
#define VRAM_PAGEROWS		2
#define VRAM_PAGES			16
#define VRAM_PAGEW			32
#define VRAM_PAGEH			32
#define VRAM_SETX(X)		(X)
#define VRAM_SETY(Y)		((Y)*VRAM_PAGEW)
#define VRAM_SETXY(X,Y)		((X)+((Y)*VRAM_PAGEW))
#define VRAM_SETPAGE(P)		((P)<<16)
#define VRAM_GETX(HND)		((HND) & (VRAM_PAGEW-1))
#define VRAM_GETY(HND)		(((HND)/VRAM_PAGEW) & (VRAM_PAGEW-1))
#define VRAM_GETXY(HND)		((HND) & 0xffff)
#define VRAM_GETPAGE(HND)	((HND)>>16)
#define VRAM_CALCVRAMX(HND)	(512+((VRAM_GETPAGE(HND)%(VRAM_PAGECOLS))*64)+(VRAM_GETX(HND)*2))
#define VRAM_CALCVRAMY(HND)	(((VRAM_GETPAGE(HND)/(VRAM_PAGECOLS))*256)+(VRAM_GETY(HND)*8))
#define LOADPAL_PRINT_OUT_PALETTE

#include "Ultra64.h"
#include <stdio.h>

#include "isltex.h"
#include <islutil.h>
#include <islmem.h>

#include "LayOut.h"
#include "Textures.h"

#include "Main.h"
#include "memload.h"
#include "shell.h"

int numTextureBanks = 0;
int numUsedDummys		= 0;
int numUsedDummys64 = 0;


//char *palNames [ 5 ] = { "BFG01", "BFG02", "BFG03", "BFG04", "BFG05"};
char *palNames [ 5 ] = { "GREENBABY.PAL", "YELLOWBABY.PAL", "BLUEBABY.PAL", "PINKBABY.PAL", "REDBABY.PAL"};

TextureBankType *textureBanks [ MAX_TEXTURE_BANKS ] = { NULL, NULL, NULL, NULL, NULL };



TEXTUREANIMLIST textureAnimList;


TextureType *CreateSpareTextureSpace ( long dummyCrc );

void LoadTextureBank ( int textureBank )
{
	int fileLength, counter, counter1;
	char fileName[256];
	char titFileName[256];
	unsigned char *p, *textureAnims;

	TEXTUREANIM *textureAnim;

	short numAnimations = 0;

	switch ( textureBank )
	{
		case GARDEN_TEX_BANK:
				sprintf ( fileName, "TEXTURES\\GARDEN.SPT" );
				sprintf ( titFileName, "TEXTURES\\GARDEN.TIT" );
			break;

		case ANCIENT_TEX_BANK:
				sprintf ( fileName, "TEXTURES\\ANCIENTS.SPT" );
				sprintf ( titFileName, "TEXTURES\\ANCIENTS.TIT" );
			break;

		case SPACE_TEX_BANK:
				sprintf ( fileName, "TEXTURES\\SPACE.SPT" );
				sprintf ( titFileName, "TEXTURES\\SPACE.TIT" );
			break;

		case CITY_TEX_BANK:
				sprintf ( fileName, "TEXTURES\\CITY.SPT" );
				sprintf ( titFileName, "TEXTURES\\CITY.TIT" );
			break;

		case SUBTERRANEAN_TEX_BANK:
				sprintf ( fileName, "TEXTURES\\SUB.SPT" );
				sprintf ( titFileName, "TEXTURES\\SUB.TIT" );
			break;

		case LABORATORY_TEX_BANK:
				sprintf ( fileName, "TEXTURES\\LAB.SPT" );
				sprintf ( titFileName, "TEXTURES\\LAB.TIT" );
			break;

		case HALLOWEEN_TEX_BANK:
				sprintf ( fileName, "TEXTURES\\HALLOWEEN.SPT" );
				sprintf ( titFileName, "TEXTURES\\HALLOWEEN.TIT" );
			break;

		case SUPERRETRO_TEX_BANK:
				sprintf ( fileName, "TEXTURES\\SUPER.SPT" );
				sprintf ( titFileName, "TEXTURES\\SUPER.TIT" );
			break;

		case FRONTEND_TEX_BANK:
				sprintf ( fileName, "TEXTURES\\HUB.SPT" );
				sprintf ( titFileName, "TEXTURES\\HUB.TIT" );
			break;

		case INGAMEGENERIC_TEX_BANK:
				textureBanks [ numTextureBanks ] = textureLoadBank ( "TEXTURES\\NEW.SPT" );

				textureDownloadBank ( textureBanks [ numTextureBanks ]   );
				textureDestroyBank  ( textureBanks [ numTextureBanks++ ] );
				sprintf ( fileName, "TEXTURES\\GENERIC.SPT" );
//				sprintf ( fileName, "TEXTURES\\GENERIC.SPT" );
			break;

		case TITLES_TEX_BANK:
				sprintf ( fileName, "TEXTURES\\TITLES.SPT" );
			break;
	}
	// ENDSWITCH - textureBank

	utilPrintf("FileName : %s\n", fileName );

	if ( numTextureBanks < MAX_TEXTURE_BANKS )
	{
		textureBanks [ numTextureBanks ] = textureLoadBank ( fileName );

		textureDownloadBank ( textureBanks [ numTextureBanks ]   );
		textureDestroyBank  ( textureBanks [ numTextureBanks++ ] );
	}
	else
	{
		utilPrintf("Error Loading: %s, Max Texture Banks Reached", fileName );
	}
	// ENDELSEIF - 	if ( numTextureBanks < MAX_TEXTURE_BANKS )
	
	/*textureAnims = (unsigned char *)fileLoad ( titFileName, &fileLength );

	if ( !textureAnims )
		return;
	
	p = ( unsigned char* ) textureAnims;
	numAnimations = (short)*p;
	p += 2;

	for ( counter = 0; counter < (short*)p; counter++ )
	{
		p += 2;

		textureAnim = CreateTextureAnimation( ( long ) *p+2, ( short ) *p );
		p += 2;
		for ( counter1 = 0; counter1 < ( short ) *p-2; counter1 )
		{
			AddAnimFrame ( textureAnim, (long*)p, (short*)p+4, counter1 );			
			p += 6;
		}
		// ENDFOR
	}
	// ENDFOR*/
}

void LoadTextureAnimBank( int textureBank )
{
	TextureType *dummyTexture;
	int fileLength, counter, counter1, numframes, waitTime;
	long crc, destCrc, dummyCrc;
	char fileName[256], dummyString [ 256 ];
	char titFileName[256];
	unsigned long *p, *textureAnims;

	TEXTUREANIM *textureAnim;
	short numAnimations = 0;

	switch ( textureBank )
	{
		case GARDEN_TEX_BANK: sprintf ( titFileName, "TEXTURES\\GARDEN.TIT" ); break;
		case ANCIENT_TEX_BANK: sprintf ( titFileName, "TEXTURES\\ANCIENTS.TIT" ); break;
		case SPACE_TEX_BANK: sprintf ( titFileName, "TEXTURES\\SPACE.TIT" ); break;
		case SUBTERRANEAN_TEX_BANK: sprintf ( titFileName, "TEXTURES\\SUB.TIT" ); break;
		case LABORATORY_TEX_BANK: sprintf ( titFileName, "TEXTURES\\LAB.TIT" ); break;
		case HALLOWEEN_TEX_BANK: sprintf ( titFileName, "TEXTURES\\HALLOWEEN.TIT" ); break;
		case SUPERRETRO_TEX_BANK: sprintf ( titFileName, "TEXTURES\\RETRO.TIT" ); break;
		case TITLES_TEX_BANK: sprintf ( titFileName, "TEXTURES\\TITLES.TIT" ); break;
		case INGAMEGENERIC_TEX_BANK: sprintf ( titFileName, "TEXTURES\\GENERIC.TIT" ); break;
		default: return;
	}

	textureAnims = (unsigned long *)fileLoad ( titFileName, &fileLength );

	if ( !textureAnims )
		return;
	
	p = textureAnims;
	numAnimations = *p; p++;


	for ( counter = 0; counter < numAnimations; counter++ )
	{
		DR_MOVE *siMove;
		RECT moveRect;

		numframes = *p;	p++;
		crc = *p; p++;

		destCrc = crc;
		textureAnim = CreateTextureAnimation( crc, numframes );

		if ( textureAnim->animation->dest->h == 64 )
			sprintf ( dummyString, "64DUMMY%d", numUsedDummys64++ );
		else
			sprintf ( dummyString, "DUMMY%d", numUsedDummys++ );

		dummyCrc = utilStr2CRC ( dummyString );
		dummyTexture = textureFindCRCInAllBanks ( dummyCrc );

		if ( !dummyTexture )
			utilPrintf("Cound Not Find Dummy Texture....\n");
		
		moveRect.x = VRAM_CALCVRAMX(textureAnim->animation->dest->handle);
		moveRect.y = VRAM_CALCVRAMY(textureAnim->animation->dest->handle);
		moveRect.w = (textureAnim->animation->dest->w + 3) / 4;
		moveRect.h = textureAnim->animation->dest->h;

		// check for 256 colour mode
		//if(textureAnim->animation->dest->tpage & (1 << 7))
			//moveRect.w *= 2;

		// copy bit of vram
/*		BEGINPRIM ( siMove, DR_MOVE );
		SetDrawMove(siMove, &moveRect, VRAM_CALCVRAMX(dummyTexture->handle),VRAM_CALCVRAMY(dummyTexture->handle));
		ENDPRIM ( siMove, 1023, DR_MOVE );
*/
 		DrawSync(0);
		MoveImage ( &moveRect, VRAM_CALCVRAMX(dummyTexture->handle), VRAM_CALCVRAMY(dummyTexture->handle) );
 		DrawSync(0);

		for ( counter1 = 0; counter1 < numframes; counter1++ )
		{
			crc = *p;
			p++;
			waitTime = *p; p++;

			if ( crc == destCrc )
				AddAnimFrame ( textureAnim, dummyCrc, waitTime, counter1 );
			else
				AddAnimFrame ( textureAnim, crc, waitTime, counter1 );
		}
	}
}

void FreeTextureBank ( TextureBankType *textureBank )
{
	textureDestroyBank	( textureBank );
	textureUnloadBank		( textureBank );
}

void FreeAllTextureBanks ( void )
{
	short c;

	utilPrintf("Freeing All Texture Banks : TEXTURE\n");

	for ( c = 0; c < MAX_TEXTURE_BANKS; c++ )
	{
		if ( textureBanks[c] )
			FreeTextureBank ( textureBanks [c] );
		// ENDIF - if ( textureBanks[c] )
		textureBanks[c] = NULL;
	}
	// ENDIF - 	for ( c = o; c < MAX_TEXTURE_BANKS; c++ )
	numTextureBanks = 0;
	numUsedDummys		= 0;
	numUsedDummys64	= 0;

	textureDestroy();
}





static int LOADPAL_LoadPCPalette(char * const file, LOADPAL_PCPalette16Str * const pc_palette);
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
signed short LOADPAL_Load16(char * const file)
{
	signed short psx_palette[16];
	LOADPAL_PCPalette16Str pc_palette;
	int clut;

	// Load the palette into 24bit rgb components
	if (!LOADPAL_LoadPCPalette(file, &pc_palette))
		return 0;

	// Now convert the pc palette to the playstation version
	{
		int i;
		LOADPAL_24BitRGBStr *element;
		for (i=0;i<16;i++)
		{
			element = &pc_palette.component[i];

#ifdef LOADPAL_PRINT_OUT_PALETTE
			utilPrintf("pc rgb = %d, %d, %d\n", element->r, element->g, element->b);
#endif
			// Divide each element by 8 to get in the range 0-31
			// Remember the psx holds elements in the order b,g,r
			psx_palette[i] = ((element->r/8) << 0) +
							 ((element->g/8) << 5) +
							 ((element->b/8) << 10);
		}
	}

	// Finally try and load this PSX palette into VRAM
	clut = textureAddCLUT16(&psx_palette[0]);

	return clut;
}
//////////////////////////////////////////////////////////////////
//					Internal functions							//
//////////////////////////////////////////////////////////////////
static int LOADPAL_LoadPCPalette(char * const file, 
							   LOADPAL_PCPalette16Str * const pc_palette)
{
	unsigned char *palette = NULL;
	unsigned char *file_ptr;
	int file_length;
	int i;

	if (!file)
	{
		utilPrintf("ERROR: File address passed to palette load is NULL.\n");
		utilPrintf("Palette not loaded.\n");
		return 0;
	}

	palette = (unsigned char*)fileLoad(file, &file_length);

	if (!palette)
	{
		utilPrintf("ERROR: Could not load palette file %s.\n", file);
		utilPrintf("Palette not loaded.\n");
		return 0;
	}

	// Let's just check the 1st 3 letters for the right format
	if ( (palette[0] != 'J') ||
		 (palette[1] != 'A') ||
		 (palette[2] != 'S') )
	{
		utilPrintf("ERROR: Palette file is not in JASC format.\n");
		utilPrintf("Use Paint Shop Pro or Batchmaster.\n");
		utilPrintf("Palette not loaded.\n");
		return 0;
	}

	// go past the string and version number
	file_ptr = palette + 16;

	// Find out how many colours there are
	{
		unsigned char nb;			// next byte
		int nc;						// number of colours

		nb = *file_ptr;
		nc = nb - 0x30;
		file_ptr++;
		nb = *file_ptr;
		// Keep on readin in bytes until return is found
		while (nb != 0x0D)
		{
			nc *= 10;
			nc += (nb - 0x30);
			file_ptr++;
			nb = *file_ptr;
		}
		// If the number of colours is not equal to 16 throw an error
		if (nc != 16)
		{
			utilPrintf("ERROR: Palette has %d instead of 16 colours.\n");
			utilPrintf("Palette not loaded.\n");
			return 0;
		}
	}

	// increment again past the newline
	file_ptr+=2;

	// The components should be next
	for (i=0;i<16;i++)
	{
		unsigned char nb;		// next byte
		unsigned char r,g,b;

		// initialise these vars
		// get the r component (separated by a space from the green)
		nb = *file_ptr;
		r = (nb - 0x30);
		file_ptr++;
		nb = *file_ptr;
		while (nb != 0x20)
		{
			r *= 10;
			r += (nb - 0x30);
			file_ptr++;
			nb = *file_ptr;
		}
		// Go past the space
		file_ptr++;
		// get the g component (separated by a space from the green)
		nb = *file_ptr;
		g = (nb - 0x30);
		file_ptr++;
		nb = *file_ptr;
		while (nb != 0x20)
		{
			g *= 10;
			g += (nb - 0x30);
			file_ptr++;
			nb = *file_ptr;
		}
		// Go past the space
		file_ptr++;
		// get the b component (separated by a return and newline)
		nb = *file_ptr;
		b = (nb - 0x30);
		file_ptr++;
		nb = *file_ptr;
		while (nb != 0x0D)
		{
			b *= 10;
			b += (nb - 0x30);
			file_ptr++;
			nb = *file_ptr;
		}
		// Now get past the newline
		file_ptr += 2;

		// Finally store in the pc palette
		pc_palette->component[i].r = r;
		pc_palette->component[i].g = g;
		pc_palette->component[i].b = b;
	}

	FREE(palette);

	return file_length;
}
//////////////////////////////////////////////////////////////////


TEXTUREANIM *CreateTextureAnimation ( long crc, int numframes )
{
	int fileLength, counter, n;

	unsigned char *p;

	char line[255];
	char tempName[255];

	signed char type[20];

	TEXTUREANIM *textureAnim = NULL;

	textureAnim = MALLOC0 ( sizeof ( TEXTUREANIM ) );

	if ( !textureAnim )
	{
		utilPrintf ( "Could Not Malloc Texture Anim.\n" );
		return NULL;
	}

	AddTextureAnim ( textureAnim );

	textureAnim->numFrames = numframes;
	textureAnim->frame = 0;
	textureAnim->lastTime = 0;
	textureAnim->animation = MALLOC0 ( sizeof(TextureAnimType) + (sizeof(TextureType *) * textureAnim->numFrames ) );

	if ( !textureAnim->animation )
	{
		utilPrintf ( "Could Not Malloc Animation. %d\n",  textureAnim->numFrames );
		FREE ( textureAnim );
		return NULL;
	}

	textureAnim->animation->dest = textureFindCRCInAllBanks ( crc );
	textureAnim->animation->anim = (TextureType **)((unsigned char *)textureAnim->animation + sizeof(TextureAnimType));
	textureAnim->animation->waitTimes = (short *)MALLOC0( sizeof(short)*numframes );

//	for ( counter = 0; counter < textureAnim->numFrames; counter++ )
//	{
//		utilPrintf("Counter : %d\n", counter );

//		if ( counter < 9 )
//			sprintf( type, "%s%d", fileName, counter+1 );
//		else
//			sprintf( type, "%s%d", fileName, counter+1 );

//		utilPrintf("Trying To Load Texture : %s\n", type );
//		textureAnim->animation->anim [ counter ] = textureFindCRCInAllBanks ( utilStr2CRC ( type ) );

//		if ( !textureAnim->animation->anim [ counter ] )
//			utilPrintf("Could Not Find Texture : %s\n", type );
//	}

	return textureAnim;
}


void AddAnimFrame( TEXTUREANIM *anim, long crc, short waitTime, int num )
{
	anim->animation->waitTimes[num] = waitTime;
	anim->animation->anim[num] = textureFindCRCInAllBanks( crc );
	if ( !anim->animation->anim[num] )
		utilPrintf("Could Not Find Texture : %lu\n", crc );
}

void UpdateTextureAnimations( void )
{
	TEXTUREANIM *cur;

	if ( textureAnimList.numEntries == 0 )
		return;

	for ( cur = textureAnimList.head.next; cur != &textureAnimList.head; cur = cur->next )
	{
		while( cur->lastTime + cur->animation->waitTimes[cur->frame] < frame )
		{
			cur->lastTime += cur->animation->waitTimes[cur->frame];
			
			cur->frame++;
			if( cur->frame >= cur->numFrames )
				cur->frame = 0;
		}

		// JH: Copy the required texture into vram.
		CopyTexture ( cur->animation->dest, cur->animation->anim[cur->frame], 0 );
	}
}

void InitTextureAnimLinkedList ( void )
{
	textureAnimList.numEntries = 0;
	textureAnimList.head.next = textureAnimList.head.prev = &textureAnimList.head;
}

void AddTextureAnim ( TEXTUREANIM *textureAnim )
{
	if ( textureAnim->next == NULL )
	{
		textureAnimList.numEntries++;
		textureAnim->prev								= &textureAnimList.head;
		textureAnim->next								= textureAnimList.head.next;
		textureAnimList.head.next->prev	= textureAnim;
		textureAnimList.head.next				=	textureAnim;
	}
}

void FreeTextureAnimList ( void )
{
	TEXTUREANIM *cur,*next;

	if( !textureAnimList.numEntries )
		return;

	for ( cur = textureAnimList.head.next; cur != &textureAnimList.head; cur = next )
	{
		next = cur->next;
		SubTextureAnim(cur);
	}
}


void SubTextureAnim ( TEXTUREANIM *textureAnim )
{
	if(textureAnim->next == NULL)
		return;

	textureAnim->prev->next = textureAnim->next;
	textureAnim->next->prev = textureAnim->prev;
	textureAnim->next = NULL;
	textureAnimList.numEntries--;

	FREE(textureAnim);
}


TextureType *CreateSpareTextureSpace ( long dummyCrc )
{
	RECT			rect;
	DR_MOVE *siMove;
	RECT	moveRect;

	TextureType *texture, *dummy;

	dummy = ( TextureType* ) MALLOC0 ( sizeof ( TextureType ) );

	texture = textureFindCRCInAllBanks ( dummyCrc );

	if ( !texture )
	{
		utilPrintf ("Could Not Find Replacement Texture.....\n");
		return NULL;
	}
	// ENDIF - !texture

	dummy->handle = textureVRAMalloc ( texture->w, texture->h );

	if ( !dummy->handle )
	{
		utilPrintf("Could Not VRAM Alloc : Width : %d : Height : %d\n", texture->w, texture->h );
		return NULL;
	}
	// ENDIF

	rect.x = VRAM_CALCVRAMX(dummy->handle);
	rect.y = VRAM_CALCVRAMY(dummy->handle);
	rect.w = (texture->w+3)/4;
	rect.h = texture->h;

	dummy->x = VRAM_GETX ( dummy->handle ) * 8;
	dummy->y = rect.y;

	dummy->w = texture->w;
	dummy->h = texture->h;

  dummy->u0 = texture->x;
  dummy->v0 = texture->y;
  dummy->u1 = 255;
  dummy->v1 = texture->y;
  dummy->u2 = texture->x;
  dummy->v2 = 255;
  dummy->u3 = 255;
  dummy->v3 = 255;

	dummy->imageCRC = dummyCrc;

	dummy->tpage = getTPage(
		((texture->flags & NEIGHTBIT) ? 1 : 0),
		0, rect.x, rect.y);

	moveRect.x = VRAM_CALCVRAMX(dummy->handle);
	moveRect.y = VRAM_CALCVRAMY(dummy->handle);
	moveRect.w = dummy->w;
	moveRect.h = dummy->h;

	// check for 256 colour mode
	if(dummy->tpage & (1 << 7))
		moveRect.w *= 2;

	BEGINPRIM		( siMove, DR_MOVE );
	SetDrawMove	( siMove, &moveRect, VRAM_CALCVRAMX(texture->handle),VRAM_CALCVRAMY(texture->handle));
	ENDPRIM			( siMove, 1023, DR_MOVE );


	return dummy;
}




void CopyTexture ( TextureType *dest, TextureType *src, int copyPalette )
{
	DR_MOVE *siMove;
	RECT	moveRect;

	if((dest == NULL) || (src == NULL))
		return;


	moveRect.x = VRAM_CALCVRAMX(src->handle);
	moveRect.y = VRAM_CALCVRAMY(src->handle);
	moveRect.w = (dest->w + 3) / 4;
	moveRect.h = dest->h;

	// check for 256 colour mode
	if ( dest->tpage & (1 << 7) )
		moveRect.w *= 2;

//	dest->tpage = src->tpage;
	//dest->clut = src->clut;

	// copy bit of vram
	BEGINPRIM		( siMove, DR_MOVE );
	SetDrawMove ( siMove, &moveRect, VRAM_CALCVRAMX(dest->handle), VRAM_CALCVRAMY(dest->handle) );
	ENDPRIM			( siMove, 1023, DR_MOVE );

	if ( copyPalette )
	{
		moveRect.x = (src->clut & 0x3f) << 4;		// Copy up the palette
		moveRect.y = (src->clut >> 6);

		if ( dest->tpage & (1 << 7) )
			moveRect.w = 256;
		else		
			moveRect.w = 16;
		// ENDELSEIF

		moveRect.h = 1;

		BEGINPRIM		( siMove, DR_MOVE );
		SetDrawMove ( siMove, &moveRect, (dest->clut & 0x3f) << 4, (dest->clut >> 6) );
		ENDPRIM			( siMove, 1023, DR_MOVE );
	}
	// ENDIF

}
