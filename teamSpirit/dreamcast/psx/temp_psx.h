#ifndef TEMP_PSX_H
#define TEMP_PSX_H

#include "types.h"
#include <ctype.h>


#define strupr(c) _toupper(c)

#define PostQuitMessage(x)
#define RunDesignWorkViewer()
#define StartControllerView()
#define RunControllerView()
#define StopSample(x)

extern FVECTOR fmaActorScale;


void CreateLevelObjects ( unsigned long worldID,unsigned long levelID );
/*ma
void froggerShowVRAM(unsigned char palMode);
static void froggerVRAMviewNormal(DISPENV *dispenv, int *xOffs,int *yOffs, unsigned char palMode);
static void froggerVRAMviewTextures(int *currTex);
static void froggerVRAMdrawPalette(unsigned long clut, int y);
static void froggerVRAMdrawPalette256(unsigned long clut, int y);
*/
void PsxNameEntryInit(void);
void PsxNameEntryFrame(void);
void SaveGame(void);
void LoadGame(void);
//mavoid Actor2ClipCheck(ACTOR2* act);
void asciiStringToSJIS(unsigned char *string, unsigned char *dest);

short videoKeyPress();
void StartVideoPlayback(int num);

#endif
