#ifndef _PSXSPRITE_H_INCLUDE
#define _PSXSPRITE_H_INCLUDE

void PrintSpriteOverlays ( char num );
void InitTiledBackdrop(char *filename);
void DrawTiledBackdrop();
void FreeTiledBackdrop();

extern  TextureType *tileTexture;

#endif