#include <am.h>
#include "include.h"
#include "am_audio.h"
#include <shinobi.h>
#include <sg_chain.h>
#include <sg_syg2.h>
#include "frogger.h"

/*
AM_ERROR_PTR		gMyAmError;
AC_ERROR_PTR		gMyAcError;
GDFS				fileHandleArray[MY_FILE_MAX_HANDLES];
AM_SOUND 			gMySfxInstances[MAX_SFX_INSTANCES];

KTU32 				gTransferBufferSize			=	0;
KTU32 				gPlayBufferSize				=	0;
volatile KTU32 		*gPlayBuffer				=	KTNULL;
volatile KTU32 		*gTransferBuffer			=	KTNULL;
AM_STREAM_PTR		gStream						=	KTNULL;
KTU32				gStreamVolume				=	127;

KTU32				gMyPlayBufferSize							=	PLAY_BUFFER_SIZE;
volatile KTBOOL		gStreamCallbackFlag							=	KTFALSE;
volatile KTU32 *	gMyTransferBufferBaseAddress				=	KTNULL;
AM_STREAM			gMyStreamInstances[STREAM_MAX_INSTANCES];
AM_STREAM_PTR		gMyActiveStreams[STREAM_MAX_INSTANCES]	=	{KTNULL,KTNULL};
volatile KTBOOL		gMyStreamFlags[STREAM_MAX_INSTANCES]		=	{KTFALSE,KTFALSE};
AM_STREAM_ISR		gMyStreamIsrs[STREAM_MAX_INSTANCES]		=	{_amStreamIsr0,_amStreamIsr1};

KTBOOL 	_myStreamInstanceToIndex	(AM_STREAM_PTR theStream,volatile KTU32 *index);
KTBOOL 	_myStreamAddInstance		(AM_STREAM_PTR theStream);	// add an instance to the server queue
KTBOOL 	_myStreamRemoveInstance		(AM_STREAM_PTR theStream);	// remove an instance from the server queue
KTBOOL 	_myStreamFreeInstance		(AM_STREAM_PTR theStream);	// free a instance memory block
KTBOOL 	_myStreamAllocateInstance	(AM_STREAM_PTR *theStream); // allocate an instance from the pool

char music_names[][32] =
{
	"garden.str",
	"ancients1.str",
	"space.str",
	"city.str",
	"subterranean.str",
	"laboratory.str",
	"halloween.str",
	"swampyworld.str",
	"superretro.str",
	"garden.str",
	"levelselect.str",
};

// Alloc Routines

KTBOOL MySh4Alloc(volatile KTU32 ** base,volatile KTU32 ** aligned,KTU32 size,KTU32 alignment)
{
	volatile KTU32 *	baseAddress		=	KTNULL;
	volatile KTU32 *	alignedAddress	=	KTNULL;

	if((baseAddress=(volatile KTU32 *)syMalloc(size + (alignment-1)))==0x00)
		return KTFALSE;

	alignedAddress = (volatile KTU32 *)(((KTU32)baseAddress) + (alignment - ((KTU32)baseAddress) % alignment));

	*base		=	baseAddress;
	*aligned	=	alignedAddress;

	return KTTRUE;
}

void MySh4Free(volatile KTU32 * block)
{
	syFree((void *)block);
}

// DMA routines

KTBOOL MyDmaTransferProc(volatile KTU32 *target,volatile KTU32 *source,KTU32 size,AC_DMA_TRANSFER_OPERATION operation,AC_DMA_CALLBACK callback)
{
	SYE_G2_ERR 	error		=	SYE_G2_ERR_NOTHING;
	Bool		busyFlag	=	false;

	switch(operation)
	{
		case AC_DMA_NOP:
			return KTFALSE;
	
		case AC_DMA_SUSPEND_ALL:
			error		=	syG2DmaSuspendAll();	
			break;
				
		case AC_DMA_RESUME_ALL:
			error		=	syG2DmaResumeAll();	
			break;

		default:
			break;
	}

	if(error != SYE_G2_ERR_NOTHING)
		return KTFALSE;
	else	
		return KTTRUE;
}

// Interrupt Routines

KTU32 MyAddChain(KTS16 chainTo,AC_ARM_INTERRUPT_HANDLER theInterruptHandler,KTU32 priority,void *theInterruptId)
{
	SYCHAIN  theChainId		=	0;

	theChainId	=	syChainAddHandler(chainTo,theInterruptHandler,priority,theInterruptId);

	return theChainId;
}

void MyRemoveChain(KTU32 theChainId)
{
	syChainDeleteHandler(theChainId);
}

void MyCallbackHandler(volatile KTU32 messageNumber)
{
	volatile KTU32			theOwner	=	0;
	volatile AM_STREAM_PTR	theStream	=	KTNULL;
	volatile AM_SOUND_PTR	theSound	=	KTNULL;
	AM_USER_CALLBACK		userCallback	=	KTNULL;

	if(messageNumber > 0xff)
	{
		amErrorSet(AM_OUT_OF_RANGE_PARAMETER,"_amVoiceIssueCallback - messageNumber is out of range");
		return;
	}

	// this fixes problems with the user int message numbers
	if(messageNumber >= gAcSystem.maxVoices && messageNumber <= 0xff)
	{
		// this range of message numbers is for client usage, if you want to have the callback
		// do special things here is where you will catch the message...
		return;
	}

	// ----- end parameter testing -----

	if(gAmVoicePool.channelType[messageNumber]==AM_ONESHOT_VOICE)
	{
		if(gAmVoicePool.channels[messageNumber]==KTFALSE)
			return;

		// get the owner
		theOwner	=	(KTU32)gAmVoicePool.channelOwner[messageNumber];

		theSound	=	(AM_SOUND_PTR)theOwner;

		// 7/22ish Cary@luxoflux.com reported this bug, user callback 
		// was called prior to the voice being free, I changed the order 
		// here to fix that.

		// pop off the system callback
		if(gAmVoicePool.systemCallback != KTNULL)
			gAmVoicePool.systemCallback(messageNumber);

		// now the channel is closed but the voice pool logic is still
		// open, preserve the userCallback and clear the voice pool
		// structure

		if(gAmVoicePool.userCallback[messageNumber] != KTNULL)
			userCallback	=	gAmVoicePool.userCallback[messageNumber];
	
		// now clear the channel
		_amVoiceClearChannel(messageNumber);

		// if a user callback was installed pop it off too
		// at this point the voice channels (physical and logical)
		// are both free and clear and we are on the way out the door.
		if(userCallback != KTNULL)
			userCallback(messageNumber);

		return;
	}
	else if(gAmVoicePool.channelType[messageNumber]==AM_MIDI_VOICE)
	{
		// 7/22ish Cary@luxoflux.com reported this bug, user callabck 
		// was called prior to the voice being free, I changed the order 
		// here to fix that.

		// pop off the system callback
		if(gAmVoicePool.systemCallback != KTNULL)
			gAmVoicePool.systemCallback(messageNumber);

		// now the channel is closed but the voice pool logic is still
		// open, preserve the userCallback and clear the voice pool
		// structure

		if(gAmVoicePool.userCallback[messageNumber] != KTNULL)
			userCallback	=	gAmVoicePool.userCallback[messageNumber];
	
		// now clear the channel
		_amVoiceClearChannel(messageNumber);

		// if a user callback was installed pop it off too
		// at this point the voice channels (physical and logical)	
		// are both free and clear and we are on the way out the door.
		if(userCallback != KTNULL)
			userCallback(messageNumber);

		return; 
	}
	else
	{
		if(gAmVoicePool.streamIsr[messageNumber] != KTNULL)
		{		
			// get the owner
			theOwner	=	(KTU32)gAmVoicePool.channelOwner[messageNumber];

			theStream	=	(AM_STREAM_PTR)theOwner;

			// invoke the stream data server ISR
			gAmVoicePool.streamIsr[messageNumber](theOwner);
		
			if(theStream->isrCounter == theStream->interruptsTillEnd)
			{
				if(gAmVoicePool.userCallback[messageNumber] != KTNULL)
					gAmVoicePool.userCallback[messageNumber](messageNumber);
			
				theStream->serviceFlag		=	KTFALSE;
			}
		}

		return;
	}

	return; // should never exit from here...
}


void MyArmInterruptHandler(void *theInterruptId)
{
	volatile KTU32 *		intWriteCursor		=	KTNULL;
	volatile KTU32			currentOffset		=	0;
	volatile KTU32			Arm7WriteIndex		=	0;
	volatile KTU32			startOfArray		=	0;
	volatile KTU32			nbrOfVoices			=	0;
	volatile KTU32			oldImask			=	0;
	volatile KTU32 *		target				=	(volatile KTU32 *)&gAcSystem.intArray[0];
	volatile KTU32 *		source				=	gAcSystem.intArrayStartPtr;
	volatile KTU32			i					=	0;
	volatile KTU32			fifoCount			=	0;

	if(((KTU32)theInterruptId) != AC_ARM_INTERRUPT_HANDLER_ID && ((KTU32)theInterruptId) != AC_ARM_POLLING_HANDLER_ID)
		return;

	// adjustable polling rate
	if(gAcMessageMode==AC_POLL_MESSAGES)
	{
		if(gAcPollingRateCounter > gAcPollingRate)
		{
			gAcPollingRateCounter = 0;
		}
		else
		{
			++gAcPollingRateCounter;
			return;
		}
	}

	// ----- Start G2 Countermeasures -----

	oldImask	=	get_imask();			// get the old imask value

	set_imask(15);							// disable interrupts

	acDmaSuspendAll();					// suspend all DMA

	acG2FifoCheck();	// wait for the AICA/HOLLY FIFO's to empty

	// ------------------------------------

	// reset the ARM interrupt status if it is not polling
	if(((KTU32)theInterruptId) != AC_ARM_POLLING_HANDLER_ID)
	{
		*gAcSystem.intResetFlag	=	0x20;
	//	acSystemResetArmInterrupt(); does a G2WriteLong()
	}

	startOfArray = (KTU32)gAcSystem.intArrayStartPtr;

	//nbrOfVoices		=	(KTU32)gAcSystem.maxVoices - 1;
	nbrOfVoices		=	63;

	// get the int array write cursor register
	intWriteCursor	=	(KTU32 *)(gAcSystem.soundMemoryBaseAddress + AC_INT_ARRAY_OFFSET);

	// get the current write cursor offset
	currentOffset	=	(KTU32)(*intWriteCursor);
	//acG2ReadLong(intWriteCursor,&currentOffset);

	// moved this to above the int msg array read
	Arm7WriteIndex	=	currentOffset - gAcSystem.intArrayStartOffset; // byte array in terms of ptrs

	// if polling, return if no messages posted
	if(((KTU32)theInterruptId) == AC_ARM_POLLING_HANDLER_ID)
	{
		if(gLastSH4ReadIndex==Arm7WriteIndex)
		{
			// no messages pending

			// ----- End G2 Countermeasures -----

			acG2FifoCheck();	// wait for the AICA/HOLLY FIFO's to empty

			acDmaResumeAll();					// resume all DMA

			set_imask(oldImask);				// enable interrupts

			// ------------------------------------

			return;
		}
	}

	
	// get the interrupt array
	//acG2Read((volatile KTU32 *)&gAcSystem.intArray[0],(volatile KTU32 *)gAcSystem.intArrayStartPtr,(KTU32)64);
	while(i < 16) 
	{
		*(target+i)=*(source+i);												// write a long
	
		++i;																	// inc dword counter

		++fifoCount;															// inc FIFO counter
	
		if(fifoCount > 7)														// is FIFO full ?
		{										
			fifoCount	=	0;													// clear FIFO counter

			acG2FifoCheck();	// wait for the AICA/HOLLY FIFO's to empty
		}										
	}

	// ----- End G2 Countermeasures -----

	acG2FifoCheck();	// wait for the AICA/HOLLY FIFO's to empty

	acDmaResumeAll();					// resume all DMA

	set_imask(oldImask);				// enable interrupts

	// ------------------------------------

	// dispatch the messages
	while(gLastSH4ReadIndex != Arm7WriteIndex) 
	{
		gAcCallbackHandler(gAcSystem.intArray[gLastSH4ReadIndex]);
	
		gLastSH4ReadIndex++;
	
		gLastSH4ReadIndex &= nbrOfVoices;
	}
	
	return;
}

// File Loading Routines

KTBOOL	_MyFileAllocateHandle(ACFILE *fd)
{
	KTU32	i	=	0;

	for(i=0;i<MY_FILE_MAX_HANDLES;i++)
	{
		if(fileHandleArray[i]==KTNULL)
		{
			*fd	=	i;
			return KTTRUE;
		}
	}
	
	return KTFALSE;
}

void _MyFileFreeHandle(ACFILE fd)
{
	fileHandleArray[fd]	=	KTNULL;
}

void MyFileInit(void)
{
	memset(&fileHandleArray,0x00,sizeof(fileHandleArray));
}

KTBOOL MyFileIoProc(KTSTRING fileName,ACFILE * fd,KTU8 * buffer,KTU32 * size,AM_FILE_OPERATION_MODE mode)
{
	ACFILE		fileDescriptor	=	0;
	KTU32		fileSize		=	0;
	KTU8 *	fileBuffer		=	KTNULL;

	if(mode==AM_OPEN)
	{
		// open the file
		if(MyFileOpen(fileName,fd)==KTFALSE)
			return KTFALSE;
	}
	else if(mode==AM_CLOSE)
	{
		// close the file
		if(MyFileClose(*fd)==KTFALSE)
			return KTFALSE;
	}
	else if(mode==AM_READ)
	{
		// read from the file	
		if(MyFileRead(*fd,buffer,*size)==KTFALSE)
			return KTFALSE;
	}
	else if(mode==AM_LOAD)
	{
		ACFILE gd;

		if(MyFileGetSize(fileName,&fileSize)==KTFALSE)
			return KTFALSE;

		if(MyFileOpen(fileName,&gd)==KTFALSE)
			return KTFALSE;

		// read from the file
		if(MyFileRead(gd,buffer,fileSize)==KTFALSE)
			return KTFALSE;
		
		if(MyFileClose(gd)==KTFALSE)	
			return KTFALSE;
	}
	else if(mode==AM_GET_SIZE)
	{
		// get the files size
		if(MyFileGetSize(fileName,size)==KTFALSE)
			return KTFALSE;
	}
	else if(mode==AM_REWIND)
	{
		// get the files size
		if(MyFileRewind(*fd)==KTFALSE)
			return KTFALSE;
	}
	else
		return KTFALSE;		// bad arg for mode, it should never come to this.

	return KTTRUE;
}


KTBOOL MyStreamIoProc(KTSTRING fileName,ACFILE * sd,KTU8 * buffer,KTU32 * size,AM_FILE_OPERATION_MODE mode)
{
	KTU32		fileDescriptor	=	0;
	KTU32		fileSize		=	0;
	KTU8 *	fileBuffer		=	KTNULL;

	if(mode==AM_OPEN)
	{
		// open the file
		if(MyFileOpen(fileName,(ACFILE *)sd)==KTFALSE)
			return KTFALSE;
	}
	else if(mode==AM_CLOSE)
	{
		// close the file
		if(MyFileClose((ACFILE)*sd)==KTFALSE)
			return KTFALSE;
	}
	else if(mode==AM_READ)
	{
		// read from the file
		if(MyFileRead((ACFILE)*sd,buffer,*size)==KTFALSE)
			return KTFALSE;
	}
	else if(mode==AM_GET_SIZE)
	{
		// get the files size
		if(MyFileGetSize(fileName,size)==KTFALSE)
			return KTFALSE;
	}
	else if(mode==AM_REWIND)
	{
		// get the files size
		if(MyFileRewind(*sd)==KTFALSE)
			return KTFALSE;
	}
	else
		return KTFALSE;		// bad arg for mode, it should never come to this.

	return KTTRUE;
}

KTBOOL MyFileLoad(KTSTRING fileName,volatile KTU32 *buffer)
{
	KTU32 		fileSize	=	0;
	ACFILE		gd			=	KTNULL;

	if(fileName==KTNULL || buffer==KTNULL)
	{
		acErrorSet(AC_NULL_PARAMETER,"MyFileLoad - fileName of buffer is NULL");
		return KTFALSE;
	}

	if(MyFileGetSize(fileName,&fileSize)==KTFALSE)
		return KTFALSE;

	if(MyFileOpen(fileName,&gd)==KTFALSE)
		return KTFALSE;

	// read from the file
	if(MyFileRead(gd,(KTU8 *)buffer,fileSize)==KTFALSE)
		return KTFALSE;

	if(MyFileClose(gd)==KTFALSE)
		return KTFALSE;

	return KTTRUE;
}
 
KTBOOL MyFileRewind(ACFILE fd)
{
	if(fileHandleArray[fd]==NULL)
	{
		acErrorSet(AC_NULL_PARAMETER,"MyFileRewind - fd is NULL");
		return KTFALSE;
	}

	// ----- end parameter testing -----

	if(gdFsSeek(fileHandleArray[fd],0,GDD_SEEK_SET) != GDD_ERR_OK)
	{
		acErrorSet(AC_SEEK_FAIL,"MyFileRewind - gdFsSeek fail");
		return KTFALSE;
	}

	return KTTRUE;
}

KTBOOL MyFileRead(ACFILE fd,KTU8 * buffer,KTU32 size)
{
	KTU32	numberOfSectors	=	0;
	KTU32	sectorSize		=	2048;
	KTU32	moduloSize		=	0;
	KTU32	remainder		=	0;
	KTU8	tempBuffer[2048+32];
	KTU8 *	tempPtr			=	&tempBuffer[0];
	KTU32	tempOff			=	0;
	KTU8 *	writeCursor		=	buffer;
	Sint32	error			=	0;

	if(fileHandleArray[fd]==NULL)
	{
		acErrorSet(AC_NULL_PARAMETER,"MyFileRead - fd is NULL");
		return KTFALSE;
	}

	if(buffer==KTNULL)
	{
		acErrorSet(AC_NULL_PARAMETER,"MyFileRead - buffer is NULL");
		return KTFALSE;
	}

	if(((KTU32)buffer) % AC_FILE_BOUNDRY_ALIGNMENT)
	{
		acErrorSet(AC_NON_32_BYTE_ALIGNED_ADDRESS,"MyFileRead - buffer is not 32 byte aligned");
		return KTFALSE;
	}

	if(size==0)
	{
		acErrorSet(AC_0_SIZE_PARAMETER,"MyFileRead - size is 0");
		return KTFALSE;
	}

	// ----- end parameter testing -----

	remainder	=	(size % sectorSize);

	if(remainder) // works
	{
		moduloSize		=	(size-remainder) + sectorSize;
		numberOfSectors	=	moduloSize/sectorSize;
	}
	else
		numberOfSectors	=	size/sectorSize;


	if(remainder==0)	// it is an even number of sectors read once
	{
		if((error=gdFsRead(fileHandleArray[fd],numberOfSectors,buffer)) != GDD_ERR_OK)
		{
			acErrorSet(AC_READ_FAIL,"MyFileRead - gdFsRead failed");
			return KTFALSE;
		}
	
	}
	else
	{
		memset(&tempBuffer[0],0x00,remainder); // dont set the whole buffer if not necessary

		if(gdFsRead(fileHandleArray[fd],numberOfSectors-1,buffer) != GDD_ERR_OK)
		{
			acErrorSet(AC_READ_FAIL,"MyFileRead - gdFsRead failed");
			return KTFALSE;
		}

		// fix up the tempPtr

		if(((unsigned long)tempPtr) % 32) // works
		{
			tempOff	= 32 - 	(((unsigned long)tempPtr) % 32);
			tempPtr += tempOff;
		}

		//acPrintf("Read PARTIAL sector fd=%p,numberOfSectors=%u,buffer=%p\n",fd,1,buffer);

		if(gdFsRead(fileHandleArray[fd],1,tempPtr) != GDD_ERR_OK)
		{
			acErrorSet(AC_READ_FAIL,"MyFileRead - gdFsRead failed");
			return KTFALSE;
		}

		writeCursor	=	buffer + ((numberOfSectors-1) * 2048);

		memcpy(writeCursor,tempPtr,remainder); // paste it at the end of the last read
	}

	return KTTRUE;
}

KTBOOL MyFileOpen(KTSTRING fileName,ACFILE *fd)
{
	GDFS	gd;
	ACFILE	descriptor;
	
	if(fileName==KTNULL)
	{
		acErrorSet(AC_NULL_PARAMETER,"MyFileOpen - fileName is NULL");
		return KTFALSE;
	}

	if(fd==KTNULL)
	{
		acErrorSet(AC_NULL_PARAMETER,"MyFileOpen - fd is NULL");
		return KTFALSE;
	}

	// ----- end parameter testing -----

	if(_MyFileAllocateHandle(&descriptor)==KTFALSE)
	{
		return KTFALSE;
	}

	fileHandleArray[descriptor]=gdFsOpen(fileName,NULL);

	if(fileHandleArray[descriptor]==NULL)
	{
		_MyFileFreeHandle(descriptor);
		acErrorSet(AC_FILE_NOT_FOUND,"MyFileOpen - file not found");
		return KTFALSE;
	}
	else
		*fd	=	descriptor;
			
return KTTRUE;
}

KTBOOL MyFileClose(ACFILE fd)
{
	if(fileHandleArray[fd]==NULL)
	{	
		acErrorSet(AC_NULL_PARAMETER,"MyFileClose - fd is 0");
		return KTFALSE;
	}

	gdFsClose(fileHandleArray[fd]);

	_MyFileFreeHandle(fd);

	return KTTRUE;
}

KTBOOL MyFileGetSize(KTSTRING fileName, KTU32 * size)
{
	GDFS	fp			=	NULL;
	Sint32	fileSize	=	0;

	if(size==NULL)
	{
		acErrorSet(AC_NULL_PARAMETER,"MyFileGetSize - size is NULL");
		return KTFALSE;
	}

	if(fileName==KTNULL)
	{
		acErrorSet(AC_NULL_PARAMETER,"MyFileGetSize - fileName is NULL");
		return KTFALSE;
	}

	// ----- end parameter testing -----

	if((fp=gdFsOpen(fileName,NULL))==NULL)
	{
		acErrorSet(AC_FILE_NOT_FOUND,"MyFileGetSize - gdFsOpen fail (file not found)");
		return KTFALSE;
	}

	if(gdFsGetFileSize(fp,&fileSize) != TRUE)
	{
		acErrorSet(AC_GET_SIZE_FAIL,"MyFileGetSize - gdFsGetFileSize fail");
		gdFsClose(fp);
		return KTFALSE;
	}
	
	gdFsClose(fp);

	*size	=	fileSize;

	return KTTRUE;   
}

KTBOOL MyAmSetup(KTBOOL usePolling)
{
	volatile KTU32* 	base							=	KTNULL;
	volatile KTU32* 	aligned							=	KTNULL;
	KTU32 				driverSize						=	0;
	char*				driverFileName					=	KTNULL;
	KTU32				totalMem;
	
	// ----------------------------------------------------------------------------------------------
	// Get error message pointers
	// ----------------------------------------------------------------------------------------------

	gMyAmError	=	amErrorGetLast();
	gMyAcError	=	acErrorGetLast();

	// ----------------------------------------------------------------------------------------------
	// Init my file system for the example
	// ----------------------------------------------------------------------------------------------

	MyFileInit();

	// ----------------------------------------------------------------------------------------------
	// Set the messaging mode, NEW!!!! R8.2
	//
	// If you don't call this function the message mode will be interrupts.
	// ----------------------------------------------------------------------------------------------

	if(usePolling==KTTRUE)
		acIntSetMessagingMode(AC_POLL_MESSAGES);
	else
		acIntSetMessagingMode(AC_INTERRUPT_MESSAGES);
	
	// ----------------------------------------------------------------------------------------------
	// Setup to init the am library by installing the needed procs, see MyFile.c and MyInt.c 
	// ----------------------------------------------------------------------------------------------
	
	// install SH4 memory allocation routines (note the audio64 lib does not allocate SH4 memory)
	amMemInstallAlternateMemoryManager(MySh4Alloc,MySh4Free);

	// install a custom file IO proc, see MyFile.c
	amFileInstallAlternateIoManager(MyFileIoProc);

	// Install a custom stream IO proc,, see MyFile.c
	amStreamIoInstallAlternateIoManager(MyStreamIoProc);

	// ----------------------------------------------------------------------------------------------
	// set the AICA interrupt chain ID
	// ----------------------------------------------------------------------------------------------

	acIntSetAicaChainId(AC_INT_AICA_CHAIN_ID);

	// ----------------------------------------------------------------------------------------------
	// install the int chain managers, see MyInt.c
	// ----------------------------------------------------------------------------------------------

	acIntInstallOsChainAddManager(MyAddChain);
	acIntInstallOsChainDeleteManager(MyRemoveChain);

	// ----------------------------------------------------------------------------------------------
	// install the DMA shell handler
	//
	// This is REQUIRED for the G2 compliance functions DmaSuspendAll and ResumeAll(), see MyDma.c
	// ----------------------------------------------------------------------------------------------
 
	if(acDmaInstallHandler(MyDmaTransferProc)==KTFALSE)
	{
		return KTFALSE;
	}
		
	// ----------------------------------------------------------------------------------------------
	// select the DA driver
	// ----------------------------------------------------------------------------------------------

	if(amInitSelectDriver(AM_DA_DRIVER)==KTFALSE)
	{
		return KTFALSE;
	}

	// ----------------------------------------------------------------------------------------------
	// get the driver file name
	// ----------------------------------------------------------------------------------------------

	driverFileName	=	amInitGetDriverFileName();

	// ----------------------------------------------------------------------------------------------
	// Get the size of the selected driver
	// ----------------------------------------------------------------------------------------------

	if(amFileGetSize(driverFileName,&driverSize)==KTFALSE)
	{
		return;
	}
		
	// ----------------------------------------------------------------------------------------------
	// Allocate the temporary driver buffer
	// ----------------------------------------------------------------------------------------------

	if(amMemSh4Alloc(&base,&aligned,driverSize,32)==KTFALSE)
	{
		return KTFALSE;
	}
	
	// ----------------------------------------------------------------------------------------------
	// Initialize the am library
	// ----------------------------------------------------------------------------------------------

	if(amInit( (KTU32 *)aligned, driverSize)==KTFALSE)
	{
		return KTFALSE;
	}
	
	// ----------------------------------------------------------------------------------------------
	// Free the temporary driver buffer
	// ----------------------------------------------------------------------------------------------
	
	amMemSh4Free(base);

	amHeapGetFree(&totalMem);		
	
	return KTTRUE;
}


void MyAmShutdown(void)
{
	amShutdown();
}

KTBOOL MyAllocateSfxInstance(AM_SOUND_PTR *theSound)
{
	KTU32 maxDaVoices	=	AC_TOTAL_VOICES;
	KTU32 i				=	0;

	*theSound	=	KTNULL;	// no undefined result...

	for(i=0;i<maxDaVoices;i++)
	{
		if(gMySfxInstances[i].isPlaying==KTFALSE)	
		{
			memset(&gMySfxInstances[i],0x00,sizeof(AM_SOUND));
			*theSound	=	&gMySfxInstances[i];
			return KTTRUE;
		}
	}

	return KTFALSE;
}

KTBOOL MySoundEffectIsPlaying(AM_SOUND_PTR theSound)
{
	return amSoundIsPlaying(theSound);
}
 
AM_BANK_PTR MyLoadBank(char *bankFileName)
{
	int				i;
	KTU32 			bankSize		=	0;
	volatile KTU32 *aicaBuffer		=	KTNULL;
	AM_BANK_PTR		tempSh4Buffer	=	KTNULL;
    char			buffer[256],*fptr;

	gdFsChangeDir("\\");

	// change to the appropriate directory
	i = 0;
	fptr = bankFileName;
	while(*fptr != 0)
	{
		switch(*fptr)
		{
			case '\\':
				buffer[i] = 0;
				gdFsChangeDir(buffer);
				fptr++;	
				i=0;			
				break;
				
			default:
				buffer[i++] = *fptr++;				
				break;		
		}
	}
	buffer[i] = 0;
	
	if(amBankGetSize(buffer,&bankSize)==KTFALSE)
		return KTNULL;

	if((tempSh4Buffer=(AM_BANK_PTR)syMalloc(bankSize))==KTNULL)
		return KTNULL;

	if(amBankLoad(buffer,tempSh4Buffer)==KTFALSE)
		return KTNULL;

	if(amHeapAlloc(&aicaBuffer,bankSize,32,AM_PURGABLE_MEMORY,KTNULL)==KTFALSE)
	{
		syFree(tempSh4Buffer);
		return KTNULL;
	}

	if(amBankTransfer(aicaBuffer,tempSh4Buffer,bankSize)==KTFALSE)
	{
		syFree(tempSh4Buffer);
		return KTNULL;
	}

	syFree(tempSh4Buffer);
	
	return aicaBuffer;
}

KTBOOL MyInstallEffectsPatch(AM_BANK_PTR theBank, KTU32 effectsProgramBankIndex,KTU32 effectsOutputBankIndex)
{
	if(amDspFetchProgramBank(theBank,effectsProgramBankIndex)==KTFALSE)
	{
		return KTFALSE;
	}

	if(amDspFetchOutputBank(theBank,effectsOutputBankIndex)==KTFALSE)
		return KTFALSE;

	return KTTRUE;
}

KTBOOL MyStopSoundEffect(AM_SOUND_PTR theSound)
{
	if(amSoundStop(theSound)==KTTRUE)
		return KTTRUE;
	else
		return KTFALSE;
}

AM_SOUND_PTR MyPlaySoundEffect(AM_BANK_PTR theBank,KTU32 soundNumber,KTU32 volume,KTU32 pan)
{
	AM_SOUND_PTR theSound	=	KTNULL;

	if(MyAllocateSfxInstance(&theSound)==KTFALSE)
		return KTNULL;

	if(amSoundFetchSample((KTU32 *)theBank,soundNumber,theSound)==KTFALSE)
		return KTNULL;

	if(amSoundAllocateVoiceChannel(theSound)==KTFALSE)
		return KTNULL;

	if(amSoundSetVolume(theSound,volume)==KTFALSE)
	{
		amSoundReleaseVoiceChannel(theSound);
		return KTFALSE;
	}

	if(amSoundSetPan(theSound,pan)==KTFALSE)
	{
		amSoundReleaseVoiceChannel(theSound);
		return KTFALSE;
	}

	if(amSoundPlay(theSound)==KTFALSE)
		return KTNULL;

	return theSound;
}

AM_SOUND_PTR MyPlaySoundEffectFx(AM_BANK_PTR theBank,KTU32 soundNumber,KTU32 volume,KTU32 pan,KTU32 effectSourceMix,KTBOOL qSoundOnOrOff)
{
	KTU32 	dspMixerChannel	=	0;
	AM_SOUND_PTR theSound	=	KTNULL;

	if(MyAllocateSfxInstance(&theSound)==KTFALSE)
		return KTNULL;

	if(qSoundOnOrOff==KTTRUE)
	{
		dspMixerChannel	=	MY_SFX_QSOUND_CHANNEL;
	}
	else
		dspMixerChannel	=	MY_SFX_REVERB_CHANNEL;
	
	if(amSoundFetchSample((KTU32 *)theBank,soundNumber,theSound)==KTFALSE)
		return KTNULL;

	if(amSoundAllocateVoiceChannel(theSound)==KTFALSE)
		return KTNULL;

	if(amSoundSetVolume(theSound,volume)==KTFALSE)
	{
		amSoundReleaseVoiceChannel(theSound);
		return KTFALSE;
	}

	if(amSoundSetPan(theSound,pan)==KTFALSE)
	{
		amSoundReleaseVoiceChannel(theSound);
		return KTFALSE;
	}

	if(amSoundSetEffectsBus(theSound,dspMixerChannel,effectSourceMix)==KTFALSE)
	{	
		amSoundReleaseVoiceChannel(theSound);
		return KTFALSE;
	}

	if(amSoundPlay(theSound)==KTFALSE)
		return KTNULL;
	
	return theSound;
}

// Streaming Rountines

AM_STREAM_PTR MyStreamPrepareFile(char *streamFileName,KTU32 volume,KTU32 pan,volatile KTU32 *transferBuffer,volatile KTU32 *playBuffer,AM_USER_CALLBACK userCallback,AM_STREAM_TRANSFER_METHOD transferMethod)
{
	AM_STREAM_PTR 	theStream	=	KTNULL;
	volatile KTU32	index		=	0;
	AM_STREAM_ISR	isr			=	KTNULL;

	if(_myStreamAllocateInstance(&theStream)==KTFALSE)
		return KTNULL;

	if(_myStreamInstanceToIndex(theStream,&index)==KTFALSE)
	{
		_myStreamFreeInstance(theStream);
		return KTNULL;
	}

	isr	=	gMyStreamIsrs[index];

	if(amStreamInitFile(theStream,streamFileName)==KTFALSE)
	{
		_myStreamFreeInstance(theStream);
	
		return KTNULL;
	}

	if(amStreamSetBufferSizes(theStream,gMyPlayBufferSize * 2,gMyPlayBufferSize)==KTFALSE)
	{
		_myStreamFreeInstance(theStream);

		return KTNULL;
	}

	if(amStreamOpen(theStream)==KTFALSE)
	{
		_myStreamFreeInstance(theStream);

		return KTNULL;
	}

	if(amStreamSetBuffers(theStream,transferBuffer,playBuffer)==KTFALSE)
	{	
		amStreamClose(theStream);

		_myStreamFreeInstance(theStream);
	
		return KTNULL;
	}

	if(amStreamAllocateVoiceChannels(theStream)==KTFALSE)
	{
		amStreamClose(theStream);

		_myStreamFreeInstance(theStream);

		return KTNULL;
	}

	if(userCallback != KTNULL)
	{
		if(amStreamInstallUserCallback(theStream,userCallback)==KTFALSE)
		{
			amStreamClose(theStream);
		
			amStreamReleaseVoiceChannels(theStream);

			_myStreamFreeInstance(theStream);

			return KTNULL;
		}
	}

	if(amStreamPrimeBuffers(theStream)==KTFALSE)
	{
		amStreamClose(theStream);

		amStreamReleaseVoiceChannels(theStream);

		_myStreamFreeInstance(theStream);

		return KTNULL;
	}

	if(amStreamSetIsr(theStream,isr)==KTFALSE)
	{
		amStreamClose(theStream);

		amStreamReleaseVoiceChannels(theStream);

		_myStreamFreeInstance(theStream);

		return KTNULL;
	}

	amStreamSetVolume(theStream,volume,0);

	amStreamSetPan(theStream,pan,0);

	if(_myStreamAddInstance(theStream)==KTFALSE)
	{
		amStreamClose(theStream);

		amStreamReleaseVoiceChannels(theStream);

		_myStreamFreeInstance(theStream);

		return KTNULL;
	}
	
	return theStream;
}

KTBOOL _myStreamInstanceToIndex(AM_STREAM_PTR theStream,volatile KTU32 *index)
{
	KTU32 i	=	0;

	for(i=0;i<STREAM_MAX_INSTANCES;i++)
	{
		if(theStream==&gMyStreamInstances[i])
		{
			*index	=	i;
			return KTTRUE;
		}
	}
	
	return KTFALSE;
}

KTBOOL _myStreamAllocateInstance(AM_STREAM_PTR *theStream)
{
	KTU32 i	=	0;

	for(i=0;i<STREAM_MAX_INSTANCES;i++)
	{
		if(gMyStreamFlags[i]==KTFALSE)
		{
			*theStream			=	&gMyStreamInstances[i];
			gMyStreamFlags[i]	=	KTTRUE;
			return KTTRUE;
		}
	}

	return KTFALSE;
}

KTBOOL _myStreamFreeInstance(AM_STREAM_PTR theStream)
{
	volatile KTU32	index	=	0;

	if(_myStreamInstanceToIndex(theStream,&index)==KTFALSE)
		return KTFALSE;

	gMyStreamFlags[index]	=	KTFALSE;	

	return KTTRUE;
}

KTBOOL _myStreamAddInstance(AM_STREAM_PTR theStream)
{
	KTU32 	i	=	0;

	for(i=0;i<STREAM_MAX_INSTANCES;i++)
	{
		if(gMyActiveStreams[i]==KTNULL)
		{
			gMyActiveStreams[i]	=	theStream;
			return KTTRUE;
		}
	}
	
	return KTFALSE;
}

KTBOOL _myStreamRemoveInstance(AM_STREAM_PTR theStream)
{
	KTU32 	i	=	0;

	for(i=0;i<STREAM_MAX_INSTANCES;i++)
	{
		if(gMyActiveStreams[i]==theStream)
		{
			gMyActiveStreams[i]	=	KTNULL;
			return KTTRUE;
		}
	}
	return KTFALSE;
}

void MyStreamServer(void)
{
	KTU32 	i	=	0;

//	for(i=0;i<STREAM_MAX_INSTANCES;i++)
	{
		if(gMyActiveStreams[i] != KTNULL)
		{
			amStreamServer(gMyActiveStreams[i]);
		}
	}

//	if(amStreamPlaying(gStream)==KTFALSE)
//	{
//		MyStreamDestroy(gStream);
//
//		if(StreamSetup(music_names[player[0].worldNum])==KTFALSE)
//			return;
//
//		MyStreamStart(gStream);
//	}	
	
}

KTBOOL MyStreamDestroy(AM_STREAM_PTR theStream)
{
	if(MyStreamIsPlaying(theStream)==KTTRUE)
		amStreamStop(theStream);

	amStreamClose(theStream);

	// remove the instance from the server queue
	if(_myStreamRemoveInstance(theStream)==KTFALSE)
		return KTFALSE;

	// free the instance
	if(_myStreamFreeInstance(theStream)==KTFALSE)
		return KTFALSE;
	
	return KTTRUE;
}

KTBOOL MyStreamDestroyBuffer(AM_STREAM_PTR theStream)
{
	if(MyStreamIsPlaying(theStream)==KTTRUE)
		amStreamStop(theStream);

	// remove the instance from the server queue
	if(_myStreamRemoveInstance(theStream)==KTFALSE)
		return KTFALSE;
	
	// free the instance
	if(_myStreamFreeInstance(theStream)==KTFALSE)
		return KTFALSE;
	
	return KTTRUE;
}

void MyStreamSetPlayBufferSize(KTU32 playBufferSize)
{
	gMyPlayBufferSize		=	playBufferSize;
}

KTBOOL MyStreamGetBufferSpecs(char *streamFileName,KTU32 *transferBufferSize,KTU32 *playBufferSize,AM_STREAM_TRANSFER_METHOD transferMethod)
{
	AM_STREAM theStream;

	if(amStreamInitFile(&theStream,streamFileName)==KTFALSE)
	{
		return KTFALSE;
	}

	if(amStreamSetBufferSizes(&theStream,gMyPlayBufferSize * 2,gMyPlayBufferSize)==KTFALSE)
	{
		return KTFALSE;
	}

	if(amStreamOpen(&theStream)==KTFALSE)
	{
		return KTFALSE;
	}

	if(amStreamGetMemoryRequirement(&theStream,transferBufferSize,playBufferSize)==KTFALSE)
	{
		return KTFALSE;
	}

	if(amStreamClose(&theStream)==KTFALSE)
	{
		return KTFALSE;
	}

	return KTTRUE;
}

KTBOOL StreamSetup(char *filename)
{
	gdFsChangeDir("\\");

	MyStreamSetPlayBufferSize(PLAY_BUFFER_SIZE);

	if(MyStreamGetBufferSpecs(filename,&gTransferBufferSize,&gPlayBufferSize,AM_STREAM_NON_DMA)==KTFALSE)
		return KTFALSE;

	if(amHeapAlloc(&gPlayBuffer,gPlayBufferSize,32,AM_PURGABLE_MEMORY,KTNULL)==KTFALSE)
		return KTFALSE;

	if((gTransferBuffer=(volatile KTU32 *)syMalloc(gTransferBufferSize))==KTNULL)
		return KTFALSE;

	if((gStream = MyStreamPrepareFile(filename,gStreamVolume,64,gTransferBuffer,gPlayBuffer,KTNULL,AM_STREAM_NON_DMA))==KTNULL)
		return KTFALSE;

	return KTTRUE;
}

KTBOOL MyStreamStop(AM_STREAM_PTR theStream)
{
	return amStreamStop(theStream);
}

KTBOOL MyStreamIsPlaying(AM_STREAM_PTR theStream)
{
	return amStreamPlaying(theStream);
}

KTBOOL MyStreamStart(AM_STREAM_PTR theStream)
{
	if(amStreamStart(theStream)==KTFALSE)
	{
		return KTFALSE;
	}

	return KTTRUE;
}

KTBOOL MyStreamSetVolume(AM_STREAM_PTR theStream,KTU32 volume)
{
	return amStreamSetVolume(theStream,volume,0);
}

KTBOOL MyStreamRestart(AM_STREAM_PTR theStream,AM_USER_CALLBACK userCallback)
{	
	AM_STREAM_ISR	isr	=	KTNULL;
	volatile KTU32 index	=	0;

	if(_myStreamInstanceToIndex(theStream,&index)==KTFALSE)
		return KTFALSE;

	isr	=	gMyStreamIsrs[index];

	if(amStreamRewind(theStream)==KTFALSE)
	{
		return;
	}

	if(amStreamAllocateVoiceChannels(theStream)==KTFALSE)
	{
		return;
	}

	if(amStreamPrimeBuffers(theStream)==KTFALSE)
	{
		return;
	}

	if(userCallback != KTNULL)
	{
		if(amStreamInstallUserCallback(theStream,userCallback)==KTFALSE)
		{
			return KTNULL;
		}
	}

	if(amStreamSetIsr(theStream,isr)==KTFALSE)
	{
		return;
	}

	if(amStreamStart(theStream)==KTFALSE)
	{
		return;
	}
}

KTBOOL MyStreamSetMix(AM_STREAM_PTR theStream,AM_STREAM_MIX_PTR theMix)
{
	return(amStreamSetMix(theStream,theMix));
}

AM_STREAM_PTR MyStreamPrepareBuffer(	volatile KTU32 *buffer,	
										KTU32 size,
										volatile KTU32 *playBuffer,
										KTU32 volume,
										KTU32 pan,
										KTU32 sampleRate,
										KTU32 bitDepth,
										AM_TRACK_TYPE monoOrStereo,
										AM_USER_CALLBACK userCallback)
{
	AM_STREAM_PTR 	theStream	=	KTNULL;
	volatile KTU32	index		=	0;	
	AM_STREAM_ISR	isr			=	KTNULL;

	if(_myStreamAllocateInstance(&theStream)==KTFALSE)	
		return KTNULL;

	if(_myStreamInstanceToIndex(theStream,&index)==KTFALSE)
	{	
		_myStreamFreeInstance(theStream);
		return KTNULL;
	}
	
	isr	=	gMyStreamIsrs[index];

	if(amStreamInitBuffer(theStream,buffer,size,sampleRate,bitDepth,monoOrStereo)==KTFALSE)
	{
		_myStreamFreeInstance(theStream);
	
		return KTNULL;
	}
	
	if(amStreamSetBufferSizes(theStream,size,gMyPlayBufferSize)==KTFALSE)
	{
		_myStreamFreeInstance(theStream);
		
		return KTNULL;
	}

	if(amStreamOpen(theStream)==KTFALSE)
	{
		_myStreamFreeInstance(theStream);
	
		return KTNULL;
	}

	if(amStreamSetBuffers(theStream,buffer,playBuffer)==KTFALSE)
	{
		_myStreamFreeInstance(theStream);

		return KTNULL;
	}

	if(amStreamAllocateVoiceChannels(theStream)==KTFALSE)
	{
		_myStreamFreeInstance(theStream);

		return KTNULL;
	}

	if(userCallback != KTNULL)
	{
		if(amStreamInstallUserCallback(theStream,userCallback)==KTFALSE)
		{
			amStreamReleaseVoiceChannels(theStream);

			_myStreamFreeInstance(theStream);

			return KTNULL;
		}
	}

	if(amStreamPrimeBuffers(theStream)==KTFALSE)
	{
		amStreamReleaseVoiceChannels(theStream);

		_myStreamFreeInstance(theStream);

		return KTNULL;
	}

	if(amStreamSetIsr(theStream,isr)==KTFALSE)
	{
		amStreamReleaseVoiceChannels(theStream);

		_myStreamFreeInstance(theStream);

		return KTNULL;
	}

	amStreamSetVolume	(theStream,volume,0);
	amStreamSetPan		(theStream,pan,0);

	if(_myStreamAddInstance(theStream)==KTFALSE)
	{
		amStreamReleaseVoiceChannels(theStream);

		_myStreamFreeInstance(theStream);

		return KTNULL;
	}
	
	return theStream;
}
*/