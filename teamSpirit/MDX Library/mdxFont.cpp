/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: mdxFont.cpp
	Programmer	: David Swift (reluctantly)
	Date		: 18 May 00

----------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <gelf.h>

#include "mdxException.h"	// for memory allocation, OBVIOUSLY...
#include "mgeReport.h"		// for 'reporting' stuff?

#include "mdxD3D.h"
#include "mdxMath.h"
#include "mdxFont.h"
#include "mdxTexture.h"
#include "mdxPoly.h"

struct MDX_FONTCHAR
{
	LPDIRECTDRAWSURFACE7 surf;
	float coords[4];
	short width;
};

typedef struct _MDX_FONT
{
	LPDIRECTDRAWSURFACE7 *surf;
	long numSurfs;

	short *data;
	long *softData[2];

	MDX_FONTCHAR characters[256];
	
	long height;

} MDX_FONT;

#define FONT_TEXTURE_SIZE	256
#define FONT_TEMP_SURFACES	4

const char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()-=+[]{}:;\"'|\\,<.>/?"
	"\xe0\xe8\xec\xf2\xf9\xc0\xc8\xcc\xd2\xd9\xe1\xe9\xed\xf3\xfa\xfd\xc1\xc9\xcd\xd3\xda\xdd\xe2\xea\xee\xf4\xfb\xc2\xca\xce\xd4\xdb\xe3\xf1\xf5\xc3\xd1\xd5\xe4\xeb\xef\xf6\xfc\xff\xc4\xcb\xcf\xd6\xdc\xe5\xc5\xe6\xc6\xe7\xc7\xf0\xd0\xf8\xd8\xbf\xa1\xdf";

//		DDrawCopyToSurface(tFont->surf[surf],(unsigned short *)tData,0,dim,dim,0);

long CalcStringWidth(const char *string,MDX_FONT *font, float scale);
long GetCharWidth(char c, MDX_FONT *font, float scale);

/*	--------------------------------------------------------------------------------
	Function		: InitFont
	Purpose			: initialises a font from a bitmap file
	Parameters		: 
	Returns			: 
	Info			: 
*/
MDX_FONT *InitFont(const char *filename)
{
	short *tData, *scratch; short transparent;
	int i, pptr = -1, bmpWidth, bmpHeight, charSize;
	int currSurf = 0;
	int txpos = 0;
	POINT charUV = { 0, 0 };

	LPDIRECTDRAWSURFACE7 surfaces[FONT_TEMP_SURFACES];

	tData = (short *)gelfLoad_BMP((char*)filename,NULL,(void**)&pptr,&bmpWidth,&bmpHeight,NULL,GELF_IFORMAT_16BPP555,GELF_IMAGEDATA_TOPDOWN);
	
	if (!tData)
		return NULL;

	transparent = tData[0];	// take transparent colour as the top-left pixel colour

	MDX_FONT *font = (MDX_FONT*)AllocMem(sizeof(MDX_FONT));

	charSize = bmpHeight;
	//for (charSize=1;charSize<bmpHeight;charSize<<=1);	

	font->height = charSize;

	scratch = (short*)AllocMem(2*FONT_TEXTURE_SIZE*FONT_TEXTURE_SIZE);

	//surfaces[0] = D3DCreateTexSurface(FONT_TEXTURE_SIZE, FONT_TEXTURE_SIZE, 0xf81f, 0, 1);
	surfaces[currSurf] = D3DCreateTexSurface(FONT_TEXTURE_SIZE, FONT_TEXTURE_SIZE, 0xf81f, 0, 1);

	for (i=0; i<strlen(alphabet) && (currSurf == 0); i++)
	{
		int yscan, found = 0;

		// find first scan containing a coloured pixel
		while (txpos<bmpWidth)
		{
			for (yscan=0;yscan<bmpHeight;yscan++)
				if (tData[yscan*bmpWidth + txpos] != transparent) { found=1; break; }
			
			if (found)
				break;
			else
				txpos++;
		}

		int left = txpos, width = 1;
		txpos++;

		// find first scan NOT containing a coloured pixel, incrementing width
		while (txpos<bmpWidth)
		{
			found = 0;
			for (yscan=0;yscan<bmpHeight;yscan++)
				if (tData[yscan*bmpWidth + txpos] != transparent) { found=1; break; }
			
			if (!found) break;
				
			width++, txpos++;

		}

		if (charUV.x+width >= FONT_TEXTURE_SIZE)
		{
			charUV.x = 0;
			charUV.y += charSize;

			if (charUV.y >= (FONT_TEXTURE_SIZE-charSize))
			{
				DDrawCopyToSurface(surfaces[currSurf], (unsigned short *)scratch,0,FONT_TEXTURE_SIZE,FONT_TEXTURE_SIZE,0);
				memset(scratch, 0, 2*FONT_TEXTURE_SIZE*FONT_TEXTURE_SIZE);
				charUV.y = 0;

				currSurf++;
				surfaces[currSurf] = D3DCreateTexSurface(FONT_TEXTURE_SIZE, FONT_TEXTURE_SIZE, 0xf81f, 0, 1);
			}
		}

		// copy stuff into texture scratch buffer

		for (int y=0; y<bmpHeight; y++)
			for (int x=0; x<width; x++)
			{
				short dt;
				//unsigned long d,r,g,b;
				dt = tData[(x+left)+(y*bmpWidth)];

				if (dt==transparent) dt = (0x1f<<11)+(0x1f);
			
				scratch[(y+charUV.y)*FONT_TEXTURE_SIZE+(x+charUV.x)] = dt;
			}

		MDX_FONTCHAR *c = &font->characters[alphabet[i]];

		c->surf = surfaces[currSurf];
		c->coords[0] = charUV.x/(float)FONT_TEXTURE_SIZE;
		c->coords[1] = charUV.y/(float)FONT_TEXTURE_SIZE;
		c->coords[2] = (charUV.x + width)/(float)FONT_TEXTURE_SIZE;
		c->coords[3] = (charUV.y + charSize)/(float)FONT_TEXTURE_SIZE;
		c->width = width;

		charUV.x += width+1;

		if (txpos == bmpWidth) break;
		//dp("char '%c' @ %d,0, %d x %d\n", alphabet[i], left, width, bmpHeight);
	}

	DDrawCopyToSurface(surfaces[currSurf], (unsigned short *)scratch,0,FONT_TEXTURE_SIZE,FONT_TEXTURE_SIZE,0);

	currSurf++;

	// copy surfaces into the right place
	font->surf = (LPDIRECTDRAWSURFACE7*)AllocMem(sizeof(LPDIRECTDRAWSURFACE7)*currSurf);
	memcpy(font->surf, surfaces, sizeof(LPDIRECTDRAWSURFACE7)*currSurf);

	font->numSurfs = currSurf;
	font->characters[' '].width = font->characters['!'].width;

	FreeMem(scratch);
	//free(tData);

	dp("Font '%s' loaded, %d textures used\n", filename, currSurf);

	return font;
}


/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void InitFontSystem(void)
{
	
}


long DrawFontCharAtLoc(long x,long y,char ch,unsigned long color, MDX_FONT *font,float scale)
{
	RECT r;

	if (!font)
		return 0;

	MDX_FONTCHAR *c = &font->characters[ch];

	if (!c->surf) return c->width;
	
	r.left = x;
	r.right = x+(c->width*scale);
	r.top = y;
	r.bottom = y+(font->height*scale);

	DrawTexturedRect(r, color, c->surf, c->coords[0], c->coords[1], c->coords[2], c->coords[3]);

	return c->width*scale;
}

long DrawFontStringAtLoc(long x,long y,char *c,unsigned long color, MDX_FONT *font, float scale,long centredX,long centredY)
{
	if (!font)
		return 0;

	long cx = x;

	if (centredX)
		cx = centredX-CalcStringWidth(c,font,scale)/2;

	while (*c)
	{
		cx += DrawFontCharAtLoc(cx,y,*c,color,font,scale);
		c++;
	}

	return cx;
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
long GetCharWidth(char c, MDX_FONT *font, float scale)
{
	if (!font)
		return 0;

	return font->characters[c].width;
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
long CalcStringWidth(const char *string,MDX_FONT *font, float scale)
{
	if (!font)
		return 0;

	long width = 0;
	char *c = (char*)string;
	
	while (*c)
	{
		width += font->characters[*c].width;
		c++;
	}

	return width;
}


/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
long WrapStringToArray(const char* str, long maxWidth, char* buffer, long bufferSize, char** array, long arraySize, MDX_FONT *font)
{
	char *wordStart, *lineStart, *p;
	char **line = array;
	long lines = 0, lineWidth = 0, lineChars = 0, wordChars = 0;

	memset(array, 0, sizeof(char*)*arraySize);

	lineStart = wordStart = p = (char*)str;
	*line = buffer;

	while (1)
	{
		char c = *p;
		long charWidth = GetCharWidth(c, font, 1.0f);
		
		if (lineWidth+charWidth >= maxWidth || c == 0)
		{
			if (c) // && wordChars < lineChars/2)
				lineChars -= wordChars;		

			if (lines == arraySize-1)
			{
				strcpy(*line, lineStart);
				lines++;
				break;
			}
			else
			{			
				memcpy(*line, lineStart, lineChars);
				*((*line)+lineChars) = 0;

				lines++;

				if (c)
				{
					*(line+1) = *line + lineChars + 1;
					line++;
					lineStart = p = wordStart;
					lineWidth = lineChars = 0;
					continue;
				}
				else
				{
					while (lines < arraySize)
						(array[lines++]) = *line + lineChars;
					break;	// we're done!
				}
			}
		}

		lineChars++;

		if (c == ' ')
		{
			wordChars = 0; wordStart = p+1;
		}			
		else
		{
			wordChars++;
		}
		
		lineWidth += charWidth;
		p++;
	}

	return lines;
}
