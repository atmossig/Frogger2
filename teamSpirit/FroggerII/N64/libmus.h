
/*********************************************************

  libmus.h : Nintendo 64 Music Tools Programmers Library
  (c) Copyright 1997/1998, Software Creations (Holdings) Ltd.

  Version 2.11

  Music library header file.
  
#  Modified by Steve Okimoto
#  May 16, 1998
#  Added support for NNScheduler

**********************************************************/

#ifndef _LIBMUS_H_
#define _LIBMUS_H_

#include <ultra64.h>
#define NNSCHED
#ifndef NNSCHED
#include <sched.h>
#else
#include "nnsched.h"
#endif

#ifdef _LANGUAGE_C_PLUS_PLUS
extern "C" {
#endif

/* --- MUSIC PLAYER API TYPEDEFS --- */

/* configuration */
typedef struct 
{
  unsigned long control_flag;

  int 		channels;
  
  #ifndef NNSCHED
  OSSched	*sched;
  #else
  NNSched	*sched;
  #endif
  
  int		thread_priority;
  unsigned char	*heap;
  int		heap_length;

  unsigned char	*ptr;
  unsigned char	*wbk;

  char		**fxs;
  int		*priority;

  int		fifo_length;

  int		effect_type;

  int		syn_updates;
  int		syn_output_rate;
  int		syn_rsp_cmds;
  int 		syn_retraceCount;
  int		syn_num_dma_bufs;
  int		syn_dma_buf_size;
} musConfig;

/* --- MUSIC PLAYER API MACROS --- */

/* control flags */
#define MUSCONTROL_RAM		(1<<0)

/* channel types flags */
#define MUSFLAG_EFFECTS	1
#define MUSFLAG_SONGS	2

/* enabled/disable flag */
typedef enum
{
  MUSBOOL_OFF,
  MUSBOOL_ON
} musbool_t;

/* --- MUSIC PLAYER API FUNCTION PROTOTYPES --- */

/* initialise */
extern int MusInitialize             (musConfig *config);

/* audio configuration */
extern int MusSetFxType		     (unsigned long fxtype);
extern int MusSetSongFxChange	     (musbool_t onoff);

/* set master volume levels */
extern void MusSetMasterVolume       (unsigned long flags, int volume);

/* start songs and sound effects */
extern unsigned long MusStartSong    (void *addr);
extern unsigned long MusStartEffect  (int number);
extern unsigned long MusStartEffect2 (int number, int volume, int pan , int restartflag, int priority);

/* stop and query sound types */
extern void MusStop                  (unsigned long flags, int speed);
extern int  MusAsk                   (unsigned long flags);

/* handle based processing */
extern int MusHandleStop             (unsigned long handle, int speed);
extern int MusHandleAsk              (unsigned long handle);
extern int MusHandleSetVolume        (unsigned long handle, int volume);
extern int MusHandleSetPan           (unsigned long handle, int pan);
extern int MusHandleSetFreqOffset    (unsigned long handle, float offset);
extern int MusHandleSetTempo         (unsigned long handle, int tempo);
extern int MusHandleSetReverb	     (unsigned long handle, int reverb);

extern int MusHandlePause	     (unsigned long handle);
extern int MusHandleUnPause	     (unsigned long handle);


/* multiple sample bank support functions */
extern void MusBankInitialize	        (void *pbank, void *wbank);
extern unsigned long MusBankStartSong   (void *ipbank, void *addr);
extern unsigned long MusBankStartEffect (void *ipbank, int number);
extern unsigned long MusBankStartEffect2(void *ipbank, int number, int volume, int pan , int restartflag, int priority);
extern void *MusHandleGetPtrAddr	(unsigned long handle);



/* --- DEVELOPEMENT API FUNCTION PROTOTYPES --- */

/* wave list lookup in song header */
#ifdef SUPPORT_SONGWAVELOOKUP
extern int    MusHandleWaveCount   (int handle);
extern short *MusHandleWaveAddress (int handle);
#endif



#ifdef _LANGUAGE_C_PLUS_PLUS
}
#endif

#endif /* _LIBMUS_H_ */
