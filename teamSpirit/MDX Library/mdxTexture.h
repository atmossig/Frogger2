/*

	This file is part of the M libraries,

	File		: 
	Programmer	: Matthew Cloy
	Date		: 

----------------------------------------------------------------------------------------------- */

#ifndef MDXTEXTURE_H_INCLUDED
#define MDXTEXTURE_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

#define TEXTURE_NORMAL 0
#define TEXTURE_AI	1
#define TEXTURE_PROCEDURAL 2

typedef struct TAG_MDX_TEXENTRY
{
	long CRC;
	char name[32];
	short *data;
	long *softData;
	long type;
	LPDIRECTDRAWSURFACE7 surf;
	float xPos,yPos;
	long xSize,ySize;

	struct TAG_MDX_TEXENTRY *next;	
} MDX_TEXENTRY;

typedef struct TAG_MDX_TEXPAGE
{
	LPDIRECTDRAWSURFACE7 surf;
	unsigned long numTex;

	struct TAG_MDX_TEXPAGE *next;	
} MDX_TEXPAGE;

#define TEXTURE_BASE "textures\\"

extern MDX_TEXENTRY *texList;

MDX_TEXENTRY *GetTexEntryFromCRC (long CRC);
unsigned long LoadTexBank(char *bank, char *baseDir);
extern float textureAdjustDivider;
void InitWater(char *file);
void UpdateWater(void);
extern LPDIRECTDRAWSURFACE7 testS;

#ifdef __cplusplus
}
#endif

#endif