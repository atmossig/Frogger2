/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: music.c
	Programmer	: 
	Date		: 29/04/99 11:13:14

----------------------------------------------------------------------------------------------- */


#include <ultra64.h>

#include "incs.h"

musConfig musicPlayer;


/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  [GLOBAL FUNCTION]
  InitMusicDriver()

  [Explantion]
  Download ROM files and initialise the music player.

  [Return value]
  NONE
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

int musicMemoryUsed = 0;
void InitMusicDriver(void)
{
	audioCtrl.musicPtr = audio_memory + AUDIO_HEAP_SIZE;
	DMAMemory(audioCtrl.musicPtr, (u32)PBANK_START, (u32)PBANK_END);
	
	audioCtrl.sfxPtr = audioCtrl.musicPtr + (u32)PBANK_END - (u32)PBANK_START;
	DMAMemory(audioCtrl.sfxPtr, (u32)PBANK2_START, (u32)PBANK2_END);


	musicPlayer.control_flag = 0;//MUSCONTROL_RAM;
	musicPlayer.channels		= 24;
	musicPlayer.sched		= &sched;
 	musicPlayer.thread_priority	= 140;//116;//119;
	
	musicPlayer.heap		= audio_memory;
	musicPlayer.heap_length	= AUDIO_HEAP_SIZE;
	
	musicPlayer.ptr		= audioCtrl.sfxPtr;//_memory + AUDIO_HEAP_SIZE;//pointer_buf;
	musicPlayer.wbk		= WBANK2_START;

	musicPlayer.fxs		= libmus_sfx_list;
	musicPlayer.priority		= libmus_priority_list;

	//musicPlayer.syn_output_rate	= 44100;
	musicPlayer.syn_output_rate	= 22050;
	musicPlayer.syn_updates	= 512 * 2;
	musicPlayer.syn_rsp_cmds	= 4096 * 2;// * 2;

	musicPlayer.syn_retraceCount = 3;
	musicPlayer.syn_num_dma_bufs = 40;//40 * 2;//36;
	musicPlayer.syn_dma_buf_size = 0x600;//800;

	musicMemoryUsed = MusInitialize(&musicPlayer);




	MusBankInitialize(audioCtrl.musicPtr, WBANK_START);
//	MusBankInitialize(audioCtrl.sfxPtr, WBANK2_START);
}


/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  [INTERNAL FUNCTION]
  DmaRomToRam(src, dest, len)

  [Explantion]
  Download an area of ROM to RAM. Note this function limits the size of any DMA
  generated to 16k so as to cause clashes with audio DMAs.

  [Parameters]
  src		address of ROM source
  dest		address of RAM destination 
  len		number of bytes to transfer

  [Return value]
  NONE
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

// avoid large DMAs these can cause problems with the music DMAs...
#define DMA_LEN 16384

void DmaRomToRam(char *src, char *dest, int len)
{
    OSIoMesg dmaIoMesgBuf;
    OSMesg dummyMesg;
    int length;
    
    while (len)
    {
      if (len>DMA_LEN)
        length = DMA_LEN;
      else
        length = len;

      osInvalDCache(dest, length);
      osPiStartDma(&dmaIoMesgBuf, OS_MESG_PRI_NORMAL, OS_READ,
                   (u32)src, dest, length, &dmaMessageQ);
      (void) osRecvMesg(&dmaMessageQ, &dummyMesg, OS_MESG_BLOCK);
      src+=length;
      dest+=length;
      len-=length;
    }
}





/* end of file */
