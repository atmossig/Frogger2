/*

	This file is part of the M libraries,

	File		: 
	Programmer	: Matthew Cloy
	Date		: 

----------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <math.h>
#include "mgeReport.h"
#include "mdxDDraw.h"
#include "mdxD3D.h"
#include "mdxInfo.h"
#include "mdxTiming.h"
#include "mdxCRC.h"
#include "mdxTexture.h"
#include "mdxMath.h"
#include "mdxObject.h"
#include "mdxActor.h"
#include "mdxLandscape.h"
#include "mdxRender.h"
#include "mdxPoly.h"
#include "mdxDText.h"
#include "mdxProfile.h"
#include "mdxWindows.h"
#include "mdxFont.h"
#include "commctrl.h"
#include "gelf.h"
#include "stdio.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define FONT_NUM32 (256/32)
float tCoords32[FONT_NUM32][FONT_NUM32][2];
char symbolChars[] = "!\"£$%^&*()-_+=[]{};'#:@~\\/,.<>?";

float charHilite = 0;
float hiliteSpeed = 0.015f;
char toHilite,toHilite2,toHilite3;
float hR = 0.5, hG = 1, hB = 0;
float cR = 1, cG = 1, cB = 1;


void UpdateFontHilite(void)
{
	if (charHilite>1)
	{
		toHilite = 26+(char)((float)rand()/RAND_MAX)*26;
		toHilite2 = 26+(char)((float)rand()/RAND_MAX)*26;
		toHilite3 = 26+(char)((float)rand()/RAND_MAX)*26;
		charHilite = 1;
	}
	else
	{
		charHilite -= hiliteSpeed*timeInfo.speed;			

		if (charHilite<0)
			charHilite = 0;

		cR = 1-(charHilite*hR);
		cG = 1-(charHilite*hG);
		cB = 1-(charHilite*hB);
	}
}

void InitFontSystem(void)
{
	for (int i=0; i<FONT_NUM32; i++)
		for (int j=0; j<FONT_NUM32; j++)
		{
			tCoords32[i][j][0] = ((float)i)/FONT_NUM32;
			tCoords32[i][j][1] = ((float)j)/FONT_NUM32;
		}
}

bool AddCharsToFont(MDX_FONT *tFont, const char *bank, const char* fPath, int surf)
{
	int pptr = -1;
	short *tData = (short *)gelfLoad_BMP((char*)fPath,NULL,(void**)&pptr,NULL,NULL,NULL,GELF_IFORMAT_16BPP555,GELF_IMAGEDATA_TOPDOWN);
	
	if (tData)
	{
		if ((tFont->surf[surf] = D3DCreateTexSurface(256,256, 0xf81f, 0, 1)) == NULL)
			return false;

		DDrawCopyToSurface(tFont->surf[surf],(unsigned short *)tData,0,256,256);

		tFont->vPtrs[surf] = new D3DTLVERTEX[FONT_NUM32*FONT_NUM32];
		tFont->widths[surf] = new long [FONT_NUM32*FONT_NUM32];
		
		for (int i=0; i<FONT_NUM32; i++)
			for (int j=0; j<FONT_NUM32; j++)
			{
				long pixelWidth,k,l,wasColored;
				// Set up basic Texture pointers!
				tFont->vPtrs[surf][(i+j*FONT_NUM32)].tu = ((float)i)/FONT_NUM32;
				tFont->vPtrs[surf][(i+j*FONT_NUM32)].tv = ((float)j)/FONT_NUM32;
				pixelWidth = 32;

				for (k = i*32+31; k>i*32; k--)
				{
					wasColored = 0;
					for (l = 0; l<32; l++)
						if (tData[k+((l+j*32)*256)]!=tData[0])
							wasColored = 1;
					if (wasColored)
						break;
					pixelWidth--;
				}

				tFont->widths[surf][(i+j*FONT_NUM32)] = pixelWidth+1;
			}
			
		gelfDefaultFree(tData);
	}

	return true;
}

MDX_FONT *InitFont(char *bank,char *baseDir)
{
	MDX_FONT *tFont = new MDX_FONT;

	char fPath[MAX_PATH];

	sprintf(fPath ,"%s%s%s\\fontl.bmp",baseDir,TEXTURE_BASE,bank);
	if (!AddCharsToFont(tFont, bank, fPath, 0))
	{
		dp("Couldnt create surface 0 for font %s",bank);
		delete tFont;
		return false;
	}

	sprintf(fPath ,"%s%s%s\\fonts.bmp",baseDir,TEXTURE_BASE,bank);
	if (!AddCharsToFont(tFont, bank, fPath, 1))
	{
		dp("Couldnt create surface 1 for font %s",bank);
		delete tFont;
		return false;
	}

	return tFont;
}

long DrawFontCharAtLoc(long x,long y,char c,unsigned long color, MDX_FONT *font, float scale)
{
	RECT m;
	long fNum;
	char *s = symbolChars,oc;
	fNum = 0;
	while (*s)
	{
		if (*s==c)
		{
			c = s-symbolChars;
			fNum=1;
		}
		s++;
	}
	
	if (!fNum)
	{
		oc = c;
		if (c==' ')
			return font->widths[fNum][0]*scale;
		
		// Convert c to a 0 indexed letter (if a letter)
		if (c>='A' && c<='Z')
			c -= 'A';
		else
		if (c>='a' && c<='z')
			c -= 'a'-26;
		else	
		if (c>='1' && c<='9')
			c -= '1'-26-26;
		else
		if (c=='0')
			c = 26+26+9;
			
	}

	if (charHilite)
	{
		if ((c==toHilite) || (c==toHilite2)|| (c==toHilite3))
		{
			float r = RGB_GETRED(color)		* (1.0f/255.0f);
			float g = RGB_GETGREEN(color)	* (1.0f/255.0f);
			float b = RGB_GETBLUE(color)	* (1.0f/255.0f);
			color = D3DRGB(r*cR,g*cG,b*cB) | (color & 0xff000000);
		}
	}

	m.left = x;
	m.top = y;
	m.bottom = y+(32*scale);
	m.right = x+(font->widths[fNum][c])*scale;

	DrawTexturedRect(m,color,font->surf[fNum],font->vPtrs[fNum][c].tu,font->vPtrs[fNum][c].tv,font->vPtrs[fNum][c].tu+((font->widths[fNum][c])/256.0),font->vPtrs[fNum][c].tv+(32.0/256.0));
	
	return font->widths[fNum][c]*scale;
}

long DrawFontStringAtLoc(long x,long y,char *c,unsigned long color, MDX_FONT *font, float scale)
{
	unsigned long cx = x;
	while (*c)
	{
		cx+=DrawFontCharAtLoc(cx,y,*c,color,font,scale);
		c++;
	}
	return x;
}

#ifdef __cplusplus
}
#endif
