#ifndef PSXTONGUE_INCLUDED
#define PSXTONGUE_INCLUDED

#include "tongue.h"

void DrawTongueSegment ( SVECTOR *vt, TextureType *tEntry );

void CalcTongueNodes ( SVECTOR *vt, TONGUE *t, int i );

void DrawTongue ( TONGUE *t );

#endif