#ifndef TEMP_PSX_H
#define TEMP_PSX_H

#include "types.h"


#define PostQuitMessage(x)
#define RunDesignWorkViewer()




void CreateLevelObjects ( unsigned long worldID,unsigned long levelID );
void froggerShowVRAM(unsigned char palMode);
static void froggerVRAMviewNormal(DISPENV *dispenv, int *xOffs,int *yOffs, unsigned char palMode);
static void froggerVRAMviewTextures(int *currTex);
static void froggerVRAMdrawPalette(unsigned long clut, int y);
static void froggerVRAMdrawPalette256(unsigned long clut, int y);
void PsxNameEntryInit(void);
void PsxNameEntryFrame(void);
void SaveGame(void);
void LoadGame(void);
void Actor2ClipCheck(ACTOR2* act);

#endif
