#ifndef SOFTWARE_H_INCLUDED
#define SOFTWARE_H_INCLUDED

long SoftwareInit(long screenX, long screenY, long IS555);
void SoftwareSetup(void);
void SoftwareBeginFrame(void);
void DrawSoftwarePolygon(float x1, float y1, float u1, float v1,
						 float x2, float y2, float u2, float v2,
						 float x3, float y3, float u3, float v3,
						 short *tex);

extern short screen[640*480];

#endif