/*

	This file is part of the M libraries,

	File		: mliReport.h
	Programmer	: Matthew Cloy
	Date		: 

----------------------------------------------------------------------------------------------- */

#ifndef MDXFONT_H_INCLUDED
#define MDXFONT_H_INCLUDED

#ifdef __cplusplus

extern "C"
{
#endif

typedef struct TAG_FONT
{
	LPDIRECTDRAWSURFACE7 surf[2];
	
	D3DTLVERTEX *vPtrs[2];
	long *widths[2];
	
} MDX_FONT;

void InitFontSystem(void);
MDX_FONT *InitFont(char *bank,char *baseDir);
long DrawFontCharAtLoc(long x,long y,char c,unsigned long color, MDX_FONT *font,float scale);
long DrawFontStringAtLoc(long x,long y,char *c,unsigned long color, MDX_FONT *font, float scale);
void UpdateFontHilite(void);
extern float charHilite;
#define HiliteUp(x) {charHilite = x;}

#ifdef __cplusplus
}
#endif

#endif