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
#define TEXTURE_NOTEXTURE 3
typedef struct TAG_MDX_TEXENTRY
{
	long CRC;
	char name[32];
	char bank[64];
	short *data;
	long *softData;
	char keyed;
	long type;
	LPDIRECTDRAWSURFACE7 surf;
	float xPos,yPos;
	long xSize,ySize;
	long refCount;

	struct TAG_MDX_TEXENTRY *next,*prev;	
} MDX_TEXENTRY;

typedef struct TAG_MDX_TEXPAGE
{
	LPDIRECTDRAWSURFACE7 surf;
	unsigned long numTex;

	struct TAG_MDX_TEXPAGE *next;	
} MDX_TEXPAGE;

#define TEXTURE_BASE "textures\\"

extern MDX_TEXENTRY *texList;

extern float textureAdjustDivider;
extern LPDIRECTDRAWSURFACE7 testS;

MDX_TEXENTRY *GetTexEntryFromCRC (long CRC);
unsigned long LoadTexBank(char *bank, char *baseDir);
void FreeAllTextureBanks( );
void InitWater(char *file);
void UpdateWater(void);
void ShowTextures(void);
void mdxSetBackdropToTex(MDX_TEXENTRY *t);

#ifdef __cplusplus
}
#endif

#endif