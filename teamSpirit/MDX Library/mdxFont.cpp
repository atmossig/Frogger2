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
#include "fxBlur.h"
#include "gelf.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define FONT_NUM32 (256/32)
float tCoords32[FONT_NUM32][FONT_NUM32][2];
char symbolChars[] = "!\"£$%^&*()-_+=[]{};'#:@~\\/,.<>?";

void InitFontSystem(void)
{
	for (int i=0; i<FONT_NUM32; i++)
		for (int j=0; j<FONT_NUM32; j++)
		{
			tCoords32[i][j][0] = ((float)i)/FONT_NUM32;
			tCoords32[i][j][1] = ((float)j)/FONT_NUM32;
		}
}

MDX_FONT *InitFont(char *bank,char *baseDir)
{
	MDX_FONT *tFont = new MDX_FONT;
	short *tData;
	int pptr = -1;

	char fPath[MAX_PATH], fPath2[MAX_PATH];
	sprintf(fPath ,"%s%s%s\\fontl.bmp",baseDir,TEXTURE_BASE,bank);
		
	tData = (short *)gelfLoad_BMP(fPath,NULL,(void**)&pptr,NULL,NULL,NULL,GELF_IFORMAT_16BPP555,GELF_IMAGEDATA_TOPDOWN);
	
	if (tData)
	{
		if ((tFont->surf[0] = D3DCreateTexSurface(256,256, 0xf81f, 0, 1)) == NULL)
		{
			dp("Couldnt create surface 0 for font %s",bank);
			delete tFont;
			return NULL;
		}

		DDrawCopyToSurface(tFont->surf[0],(unsigned short *)tData,0,256,256);

		tFont->vPtrs[0] = new D3DTLVERTEX[FONT_NUM32*FONT_NUM32];
		tFont->widths[0] = new long [FONT_NUM32*FONT_NUM32];
		for (int i=0; i<FONT_NUM32; i++)
			for (int j=0; j<FONT_NUM32; j++)
			{
				long pixelWidth,k,l,wasColored;
				// Set up basic Texture pointers!
				tFont->vPtrs[0][(i+j*FONT_NUM32)].tu = ((float)i)/FONT_NUM32;
				tFont->vPtrs[0][(i+j*FONT_NUM32)].tv = ((float)j)/FONT_NUM32;
				pixelWidth = 32;

				for (k = i*32+32; k>i*32; k--)
				{
					wasColored = 0;
					for (l = 0; l<32; l++)
						if (tData[k+((l+j*32)*256)]!=tData[0])
							wasColored = 1;
					if (wasColored)
						break;
					pixelWidth--;
				}

				tFont->widths[0][(i+j*FONT_NUM32)] = pixelWidth+1;
			}
	

		
		gelfDefaultFree(tData);
	}

	sprintf(fPath ,"%s%s%s\\fonts.bmp",baseDir,TEXTURE_BASE,bank);
		
	tData = (short *)gelfLoad_BMP(fPath,NULL,(void**)&pptr,NULL,NULL,NULL,GELF_IFORMAT_16BPP555,GELF_IMAGEDATA_TOPDOWN);
	
	if (tData)
	{
		if ((tFont->surf[1] = D3DCreateTexSurface(256,256, 0xf81f, 0, 1)) == NULL)
		{
			dp("Couldnt create surface 0 for font %s",bank);
			delete tFont;
			return NULL;
		}

		DDrawCopyToSurface(tFont->surf[1],(unsigned short *)tData,0,256,256);

		tFont->vPtrs[1] = new D3DTLVERTEX[FONT_NUM32*FONT_NUM32];
		tFont->widths[1] = new long [FONT_NUM32*FONT_NUM32];
		for (int i=0; i<FONT_NUM32; i++)
			for (int j=0; j<FONT_NUM32; j++)
			{
				long pixelWidth,k,l,wasColored;
				// Set up basic Texture pointers!
				tFont->vPtrs[1][(i+j*FONT_NUM32)].tu = ((float)i)/FONT_NUM32;
				tFont->vPtrs[1][(i+j*FONT_NUM32)].tv = ((float)j)/FONT_NUM32;
				pixelWidth = 32;

				for (k = i*32+32; k>i*32; k--)
				{
					wasColored = 0;
					for (l = 0; l<32; l++)
						if (tData[k+((l+j*32)*256)]!=tData[0])
							wasColored = 1;
					if (wasColored)
						break;
					pixelWidth--;
				}

				tFont->widths[1][(i+j*FONT_NUM32)] = pixelWidth+1;
			}
	

		
		gelfDefaultFree(tData);
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
