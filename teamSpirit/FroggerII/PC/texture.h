/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: texture.h
	Programmer	: Andy Eder
	Date		: 19/04/99 12:22:29

----------------------------------------------------------------------------------------------- */

#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED


enum
{
	TEXTURE_NORMAL,
	TEXTURE_AI
};

typedef struct tTEXENTRY
{
	long CRC;
	char name[32];
	short *data;
	long type;
	struct tTEXENTRY *next;
	LPDIRECTDRAWSURFACE surf;
	D3DTEXTUREHANDLE hdl;
	
	struct tTEXENTRY *nextFrame;
	float nextFrameAt;
	float frameTime;

	struct tTEXENTRY *cFrame;
	
} TEXENTRY;

// If texture debugging, flag when a texture handle has been successfully created
#ifdef TEXTURE_DEBUG
#define MAX_HDLCHECKS 64000
extern unsigned char *hdlCheck;
#endif

extern void LoadTextureBank(int num);
extern void InitTextureBanks();
extern void FindTexture(TEXTURE **texPtr, int texID, BOOL report);
extern short *GetTexDataFromCRC (long CRC);
D3DTEXTUREHANDLE GetTexHandleFromCRC (long CRC);
TEXENTRY *GetTexEntryFromCRC (long CRC);


#endif