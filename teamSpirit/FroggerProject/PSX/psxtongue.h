#ifndef PSXTONGUE_INCLUDED
#define PSXTONGUE_INCLUDED

#include "tongue.h"

void DrawTongueSegment ( POLY_G4 *vt, TextureType *tEntry );

void CalcTongueNodes ( POLY_G4 *vt, TONGUE *t, int i );

void DrawTongue ( TONGUE *t );

#endif