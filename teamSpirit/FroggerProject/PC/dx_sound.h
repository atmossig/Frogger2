/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: dx_sound.h
	Programmer	: James Healey
	Date		: 16/08/99

----------------------------------------------------------------------------------------------- */

#ifndef DSOUND_H_INCLUDED
#define DSOUND_H_INCLUDED



#ifdef __cplusplus
extern "C"
{
#endif

extern LPDIRECTSOUND			lpDS;
extern LPDIRECTSOUNDBUFFER		lpdsbPrimary;
extern LPDIRECTSOUND3DLISTENER	lpds3DListener;

extern int LoadWav( SAMPLE *sample );

extern int InitDirectSound ( LPGUID guid, HINSTANCE hInst,  HWND hWndMain, int prim );

extern void Update3DListener ( float sourceX, float sourceY, float sourceZ  );
extern void Get3DInterface ( LPDIRECTSOUNDBUFFER lpdsBuffer, LPDIRECTSOUND3DBUFFER lpds3DBuffer );
extern void ReleaseBuffer ( LPDIRECTSOUNDBUFFER lpdsBuffer );
extern void Set3DPosition ( LPDIRECTSOUND3DBUFFER lpds3DBuffer, float xPos, float yPos, float zPos );

#ifdef __cplusplus
}
#endif

#endif