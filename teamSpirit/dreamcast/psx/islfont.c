/******************************************************************************************
	AM2 PS   (c) 1999-2001 ISL

	psfont.c:			Font support
******************************************************************************************/

#include "include.h"
#include "main.h"

void fontDispSprite(TextureType *tex, short x,short y, uchar alpha, float size);

KMSTRIPCONTEXT	fontStripContext;
KMSTRIPHEAD		fontStripHead;
KMVERTEX_03		fontVertices[] =
{
{ KM_VERTEXPARAM_NORMAL,      0,  32, 1.0f, 0.0f,   0.125f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,      0,   0, 1.0f, 0.0f,   0.0f,   RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     32,  32, 1.0f, 0.125f, 0.125f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, 32,   0, 1.0f, 0.125f, 0.0f,   RGBA(255,255,255,255), 0 }
};

KMSTRIPCONTEXT	buttonStripContext;
KMSTRIPHEAD		buttonStripHead;
KMVERTEX_03		buttonVertices[] =
{
{ KM_VERTEXPARAM_NORMAL,      0,  32, 1.0f, 0.0f,   0.125f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,      0,   0, 1.0f, 0.0f,   0.0f,   RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     32,  32, 1.0f, 0.125f, 0.125f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, 32,   0, 1.0f, 0.125f, 0.0f,   RGBA(255,255,255,255), 0 }
};

#define ANG2FIX(a)		(((a)*4096)/360)

#define BITMAP_XD		256.0
#define BITMAP_YD		512.0
#define U_OFFSET		32.0/BITMAP_XD
#define V_OFFSET		32.0/BITMAP_YD


static TextureType	*buttonSprites[4];

unsigned char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()-=+[]{}:;\"'|\\,<.>/?"
					"\xe0\xe8\xec\xf2\xf9\xc0\xc8\xcc\xd2\xd9\xe1\xe9\xed\xf3\xfa\xfd\xc1\xc9\xcd\xd3\xda\xdd\xe2\xea\xee\xf4\xfb\xc2\xca\xce\xd4\xdb\xe3\xf1\xf5\xc3\xd1\xd5\xe4\xeb\xef\xf6\xfc\xff\xc4\xcb\xcf\xd6\xdc\xe5\xc5\xe6\xc6\xe7\xc7\xf0\xd0\xf8\xd8\xbf\xa1\xdf";

char	fontwidths[255] = {27,21,19,21,17,17,20,22,
						   10,14,23,17,24,20,21,20,
						   22,22,19,18,20,22,32,25,
						   23,21,						   
						         27,21,19,21,17,17,
						   20,22,10,14,23,17,24,20,
						   21,20,22,22,19,18,20,22,
						   32,25,23,21,
						               21,10,17,16,
						   20,19,21,18,20,21,
						                     10,22,
						   17,20,27,15,23,18,13,13,
						   10,16,16,13,13,11,11,10,
						   10,13,10,14, 9,10,16,10,
						   16, 9,14};

#define OVERLAY_X 640.0/4096.0
#define OVERLAY_Y 480.0/4096.0

static void fontDownload(psFont *font, char *fontdata, int character)
{
/*	int			    w,h, handle;
	RECT			rect;
	unsigned char	*offset;
	TextureType		*txPtr = &font->txPtr[character];
	
	w = (font->width[character]+7)/8;
	h = (font->height+7)/8;
	handle = textureVRAMalloc(w,h);
	rect.x = VRAM_CALCVRAMX(handle);
	rect.y = VRAM_CALCVRAMY(handle);
	rect.w = (font->width[character]+3)/4;
	rect.h = font->height;

	offset = fontdata + font->dataoffset[character];
	DrawSync(0);
	LoadImage(&rect,(unsigned long *)offset);
	DrawSync(0);

	txPtr->tpage = getTPage(0,0,rect.x,rect.y);
	txPtr->x = VRAM_GETX(handle)*8;
	txPtr->y = rect.y;
	txPtr->w = font->width[character];
	txPtr->h = font->height;
   	txPtr->u0 = txPtr->x;
   	txPtr->v0 = txPtr->y;
   	txPtr->u1 = txPtr->x+font->width[character]-1;
   	txPtr->v1 = txPtr->y;
   	txPtr->u2 = txPtr->x;
   	txPtr->v2 = txPtr->y+font->height-1;
   	txPtr->u3 = txPtr->x+font->width[character]-1;
   	txPtr->v3 = txPtr->y+font->height-1;
	txPtr->clut = font->clut;
	txPtr->handle = handle;
*/	
}

/**************************************************************************
	FUNCTION:	fontLoad()
	PURPOSE:	Load font into VRAM ready for use
	PARAMETERS:	Filename
	RETURNS:	Ptr to font info
**************************************************************************/

psFont *fontLoad(char *fontname)
{
	int				loop;
	unsigned char	*fontdata;
	int				*fontptr;
	psFont			*font;
	char			*c,name[64];

	font = (psFont*)MALLOC0(sizeof(psFont));	

	if(c = strstr(fontname,"small"))
	{
		font->scale = 0.75;
		font->size = 24;
		sprintf(name,"%s",(c+5));
	}
	else
	{
		font->scale = 1.0;
		font->size = 32;
		sprintf(name,"%s",fontname);
	}

	// load font and setup font texture
	if(loadPVRFileIntoSurface("font.pvr", "fonts", &font->surface, TRUE) == 0)
		return NULL;
		
	font->numchars = 62+31;
	font->height = 32;

		
	memset(font->charlookup, -1, 256);
	for(loop = 0; loop < font->numchars; loop++)
	{
		font->dataoffset[loop] = 0;
		font->width[loop] = (float)fontwidths[loop] * font->scale;
		font->pixelwidth[loop] = (float)fontwidths[loop] * font->scale;
//		font->width[loop] = fontwidths[loop];
//		font->pixelwidth[loop] = fontwidths[loop];
		font->charlookup[alphabet[loop]] = loop;
	
		font->uvOffsets[loop].v[0] = (float)((float)(loop % 8) * 32.0) / BITMAP_XD;
		font->uvOffsets[loop].v[1] = (float)((float)(loop/8) * 32.0) / BITMAP_YD;
	}
	font->alpha = 255;

	// initialise font strip context and head
	memset(&fontStripContext,0,sizeof(fontStripContext));
	memset(&fontStripHead,0,sizeof(fontStripHead));
	fontStripContext.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &fontStripContext);
//	fontStripContext.StripControl.nListType		 						= KM_TRANS_POLYGON;
	fontStripContext.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	fontStripContext.StripControl.nShadowMode			 				= KM_NORMAL_POLYGON;
	fontStripContext.StripControl.bGouraud		 						= KM_TRUE;
	fontStripContext.ObjectControl.nDepthCompare			 			= KM_ALWAYS;
	fontStripContext.ObjectControl.nCullingMode			 				= KM_NOCULLING;
	fontStripContext.ObjectControl.bZWriteDisable						= KM_FALSE;
	fontStripContext.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_SRCALPHA;
	fontStripContext.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_INVSRCALPHA;
	fontStripContext.ImageControl[KM_IMAGE_PARAM1].bSRCSelect			= KM_FALSE;
	fontStripContext.ImageControl[KM_IMAGE_PARAM1].bDSTSelect			= KM_FALSE;
	fontStripContext.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;
	fontStripContext.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;	
	fontStripContext.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_TRUE;
	fontStripContext.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	fontStripContext.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_POINT_SAMPLE;
    fontStripContext.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc = &font->surface;
	kmGenerateStripHead03(&fontStripHead,&fontStripContext);

	// initialise button strip context and head
	memset(&buttonStripContext,0,sizeof(buttonStripContext));
	memset(&buttonStripHead,0,sizeof(buttonStripHead));
	buttonStripContext.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &buttonStripContext);
	buttonStripContext.StripControl.nListType		 						= KM_TRANS_POLYGON;
	buttonStripContext.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	buttonStripContext.StripControl.nShadowMode			 				= KM_NORMAL_POLYGON;
	buttonStripContext.StripControl.bGouraud		 						= KM_TRUE;
	buttonStripContext.ObjectControl.nDepthCompare			 			= KM_ALWAYS;
	buttonStripContext.ObjectControl.nCullingMode			 				= KM_NOCULLING;
	buttonStripContext.ObjectControl.bZWriteDisable						= KM_FALSE;
	buttonStripContext.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_SRCALPHA;
	buttonStripContext.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_INVSRCALPHA;
	buttonStripContext.ImageControl[KM_IMAGE_PARAM1].bSRCSelect			= KM_FALSE;
	buttonStripContext.ImageControl[KM_IMAGE_PARAM1].bDSTSelect			= KM_FALSE;
	buttonStripContext.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;
	buttonStripContext.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;	
	buttonStripContext.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_TRUE;
	buttonStripContext.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	buttonStripContext.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_POINT_SAMPLE;
	kmGenerateStripHead03(&buttonStripHead,&buttonStripContext);

/*	fontptr = (int *)fontdata;
	font->numchars = *fontptr++;
	font->height = *fontptr++;
	font->clut = textureAddCLUT16((unsigned short *)fontptr);

	fontptr = (int *)fontdata + 10;

	font->txPtr = (TextureType *)MALLOC(sizeof(TextureType)*font->numchars);
	memset(font->charlookup, -1, 256);
	for(loop = 0; loop < font->numchars; loop++)
	{
		font->dataoffset[loop] = *fontptr++;
		font->width[loop] = *fontptr++;
		font->pixelwidth[loop] = *fontptr++;
		fontDownload(font, fontdata, loop);
		font->charlookup[alphabet[loop]] = loop;
//		debugPrintf("%c ", alphabet[loop]);
	}
//	debugPrintf("\n");
	FREE(fontdata);

	font->alpha = 0;
*/
	return font;
}


/**************************************************************************
	FUNCTION:	fontUnload()
	PURPOSE:	Unload font from VRAM and free info
	PARAMETERS:	Font ptr
	RETURNS:	
**************************************************************************/

void fontUnload(psFont *font)
{
	kmFreeTexture(&font->surface);
	FREE(font);
}


void fontDispChar(DCKFLOAT2DVECTOR offset, short x,short y, unsigned char r, unsigned char g, unsigned char b, unsigned char alpha, int size)
{
	USHORT		w,h;
	
	fontVertices[0].fX = x;
	fontVertices[0].fY = y + size;
	fontVertices[0].u.fZ = 1.0;
	fontVertices[0].fU = offset.v[0];
	fontVertices[0].fV = offset.v[1] + V_OFFSET;
	fontVertices[0].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

	fontVertices[1].fX = x;
	fontVertices[1].fY = y;
	fontVertices[1].u.fZ = 1.0;
	fontVertices[1].fU = offset.v[0];
	fontVertices[1].fV = offset.v[1];
	fontVertices[1].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

	fontVertices[2].fX = x + size;
	fontVertices[2].fY = y + size;
	fontVertices[2].u.fZ = 1.0;
	fontVertices[2].fU = offset.v[0] + U_OFFSET;
	fontVertices[2].fV = offset.v[1] + V_OFFSET;
	fontVertices[2].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);
	
	fontVertices[3].fX = x + size;
	fontVertices[3].fY = y;
	fontVertices[3].u.fZ = 1.0;
	fontVertices[3].fU = offset.v[0] + U_OFFSET;
	fontVertices[3].fV = offset.v[1];
	fontVertices[3].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);
	
	kmStartStrip(&vertexBufferDesc, &fontStripHead);	
	kmSetVertex(&vertexBufferDesc, &fontVertices[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &fontVertices[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &fontVertices[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmSetVertex(&vertexBufferDesc, &fontVertices[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmEndStrip(&vertexBufferDesc);
}


/**************************************************************************
	FUNCTION:	fontPrint()
	PURPOSE:	Display string in font at pos in colour
	PARAMETERS:	font ptr, x,y, string, r,g,b
	RETURNS:	
**************************************************************************/

void fontPrint(psFont *font, short x,short y, char *text, unsigned char r, unsigned char g, unsigned char b)
{
	unsigned char	*strPtr, c;
	int				cx,cy, loop;
	TextureType		*letter;
	
	x += 256;
	y += 128;
	x = ((float)x * (640.0/512.0));
	y = ((float)y * (480.0/256.0));

	strPtr = text;
	cx = x;
	cy = y;
	while(*strPtr)
	{
		c = *strPtr;
		switch(c)
		{
			case '\n':
				y += font->height;
				cx = x;
				cy = y;
				break;
				
			case ' ':
				x += font->height/2;
				break;
				
			case '@':
				switch(*(strPtr+1))
				{
					case 'X':
					    fontDispSprite(buttonSprites[2], x+3,y-3, font->alpha, font->size);
						strPtr++;
						x += buttonSprites[2]->w+6;
						break;
					case 'C':
					   	fontDispSprite(buttonSprites[1], x+3,y-3, font->alpha, font->size);
						strPtr++;
						x += buttonSprites[1]->w+6;
						break;
					case 'S':
					   	fontDispSprite(buttonSprites[3], x+3,y-3, font->alpha, font->size);
						strPtr++;
						x += buttonSprites[3]->w+6;
						break;
					case 'T':
					   	fontDispSprite(buttonSprites[0], x+3,y-3, font->alpha, font->size);
						strPtr++;
						x += buttonSprites[0]->w+6;
						break;
				}
				break;

			default:
				loop = font->charlookup[c];
				if(loop<font->numchars)
				{
					letter = &font->txPtr[loop];
//					if((x>-350)&&(x<640))
						fontDispChar(font->uvOffsets[loop], x,y-((c>127)?(1):(0)), r,g,b, font->alpha, font->size);
					x += font->pixelwidth[loop]+2;
				}
		}
		strPtr++;
	}
}


/**************************************************************************
	FUNCTION:	fontExtentW()
	PURPOSE:	Get width of string using font
	PARAMETERS:	font ptr, string
	RETURNS:	Width in pixels
**************************************************************************/

int fontExtentW(psFont *font, char *text)
{
	unsigned char	*strPtr, c;
	int		loop, x, maxW = 0;

	strPtr = text;
	x = 0;
	while(*strPtr)
	{
		c = *strPtr;
		switch(c)
		{
			case '\n':
				if (x>maxW)
					maxW = x;
				x = 0;
				break;
				
			case ' ':
				x += font->height/2;
				break;
				
			case '@':
				switch(*(strPtr+1))
				{
					case 'X':
						strPtr++;
						x += buttonSprites[2]->w+6;
						break;
					case 'C':
						strPtr++;
						x += buttonSprites[1]->w+6;
						break;
					case 'S':
						strPtr++;
						x += buttonSprites[3]->w+6;
						break;
					case 'T':
						strPtr++;
						x += buttonSprites[0]->w+6;
						break;
				}
				break;
				
			default:
				loop = font->charlookup[c];
				if(loop<font->numchars)
					x += font->pixelwidth[loop]+2;
		}
		strPtr++;
	}
	if(x>maxW)
		maxW = x;

	if(maxW > 0)
		maxW = ((float)maxW * (512.0/640.0));
	
	return maxW;
}


/**************************************************************************
	FUNCTION:	fontFitToWidth()
	PURPOSE:	Wrap string to width into buffer
	PARAMETERS:	font ptr, width, string, buffer
	RETURNS:	Number of lines
**************************************************************************/

int fontFitToWidth(psFont *font, int width, char *text, char *buffer)
{
	char	buf[256], *bufPtr, *outPtr = buffer;
	int		pixLen, lines = 0;

	memset(buf, 0, sizeof(buf));
	bufPtr = buf;
	while(1)
	{
		if(*text>=' ')
			*bufPtr++ = *text;
		pixLen = fontExtentW(font, buf);
		if((pixLen>width)||(*text=='\0'))
		{
			if(pixLen>width)
			{
				while(*bufPtr!=' ')
				{
					bufPtr--;
					text--;
				}
				text++;
			}
			*bufPtr = '\0';
			strcpy(outPtr, buf);
			outPtr += strlen(buf)+1;
			lines++;
			bufPtr = buf;
			memset(buf, 0, sizeof(buf));
			if(*text=='\0')
				break;
		}
		text++;
	}
	
	outPtr = buffer;
	for(pixLen=0; pixLen<lines; pixLen++)
	{
		outPtr += strlen(outPtr)+1;
	}
	
	return lines;
}



/**************************************************************************
	FUNCTION:	fontPrintN()
	PURPOSE:	Display (partial) string in font at pos in colour
	PARAMETERS:	font ptr, x,y, string, r,g,b, max chars
	RETURNS:	
**************************************************************************/

void fontPrintN(psFont *font, short x,short y, char *text, unsigned char r, unsigned char g, unsigned char b, int n)
{
	unsigned char	*strPtr, c;
	int				cx,cy, loop;
	TextureType		*letter;

	x += 256;
	y += 128;
	x = ((float)x * (640.0/512.0));
	y = ((float)y * (480.0/256.0));

	strPtr = text;
	cx = x;
	cy = y;
	while ((*strPtr) && (n>0))
	{
		c = *strPtr;
		switch(c)
		{
			case '\n':
				y += font->height;
				cx = x;
				cy = y;
				break;
				
			case ' ':
				x += font->height/2;
				break;
				
			case '@':
				switch(*(strPtr+1))
				{
					case 'X':
					    fontDispSprite(buttonSprites[2], x+3,y-3, font->alpha, font->size);
						strPtr++;
						x += buttonSprites[2]->w+6;
						break;
					case 'C':
					   	fontDispSprite(buttonSprites[1], x+3,y-3, font->alpha, font->size);
						strPtr++;
						x += buttonSprites[1]->w+6;
						break;
					case 'S':
					   	fontDispSprite(buttonSprites[3], x+3,y-3, font->alpha, font->size);
						strPtr++;
						x += buttonSprites[3]->w+6;
						break;
					case 'T':
					   	fontDispSprite(buttonSprites[0], x+3,y-3, font->alpha, font->size);
						strPtr++;
						x += buttonSprites[0]->w+6;
						break;
				}
				break;
				
			default:
				loop = font->charlookup[c];
				if(loop<font->numchars)
				{
//					letter = &font->txPtr[loop];
					if((x>-350)&&(x<640))
						fontDispChar(font->uvOffsets[loop], x,y-((c>127)?(1):(0)), r,g,b,font->alpha, font->size);
					x += font->pixelwidth[loop]+2;
				}
		}
		strPtr++;
		n--;
	}	
}

void fontDispSprite(TextureType *tex, short x,short y, uchar alpha, float size)
{
	USHORT		w,h;
//	float		size = 32;

	buttonVertices[0].fX = x;
	buttonVertices[0].fY = y + (size*1.5);
	buttonVertices[0].u.fZ = 1.0;
	buttonVertices[0].fU = 0;
	buttonVertices[0].fV = 1;
	buttonVertices[0].uBaseRGB.dwPacked = RGBA(255,255,255,alpha);

	buttonVertices[1].fX = x;
	buttonVertices[1].fY = y;
	buttonVertices[1].u.fZ = 1.0;
	buttonVertices[1].fU = 0;
	buttonVertices[1].fV = 0;
	buttonVertices[1].uBaseRGB.dwPacked = RGBA(255,255,255,alpha);

	buttonVertices[2].fX = x + (size*1.5);
	buttonVertices[2].fY = y + (size*1.5);
	buttonVertices[2].u.fZ = 1.0;
	buttonVertices[2].fU = 1;
	buttonVertices[2].fV = 1;
	buttonVertices[2].uBaseRGB.dwPacked = RGBA(255,255,255,alpha);
	
	buttonVertices[3].fX = x + (size*1.5);
	buttonVertices[3].fY = y;
	buttonVertices[3].u.fZ = 1.0;
	buttonVertices[3].fU = 1;
	buttonVertices[3].fV = 0;
	buttonVertices[3].uBaseRGB.dwPacked = RGBA(255,255,255,alpha);

	kmChangeStripTextureSurface(&buttonStripHead,KM_IMAGE_PARAM1,&tex->surface);
	
	kmStartStrip(&vertexBufferDesc, &buttonStripHead);	
	kmSetVertex(&vertexBufferDesc, &buttonVertices[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &buttonVertices[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &buttonVertices[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &buttonVertices[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmEndStrip(&vertexBufferDesc);
}

void fontInitButtonSprites()
{
	buttonSprites[0] = FindTexture("BUT_TRIA");
	buttonSprites[1] = FindTexture("BUT_CIRC");
	buttonSprites[2] = FindTexture("BUT_CROS");
	buttonSprites[3] = FindTexture("BUT_SQUA");

/*
	buttonSprites[0] = triangle;
	buttonSprites[1] = circle;
	buttonSprites[2] = cross;
	buttonSprites[3] = square;
*/
}

void fontRegisterButtonSprites(TextureType *triangle, TextureType *circle, TextureType *cross, TextureType *square)
{
	buttonSprites[0] = triangle;
	buttonSprites[1] = circle;
	buttonSprites[2] = cross;
	buttonSprites[3] = square;
}