/************************************************************************************
	ISL PSX LIBRARY	(c) 1999 Interactive Studios Ltd.

	islxa.h:		XA audio streaming functions

************************************************************************************/

#include "include.h"
#include "islxa.h"
#include "main.h"
//#include "sound.h"

//#define XAROOT	"\\"

// Moved to "sound.h"



XADataType	XAData;

// are we enabled or not
static int	XAenable;

#define ADX_WORKSIZE (ADXT_CALC_WORK((1), (ADXT_PLY_STM), (3), (44100)))
static char xaWorkArea[ADX_WORKSIZE];

XAFileType *curXA=NULL;


/**************************************************************************
	FUNCTION:	XAsetStatus()
	PURPOSE:	Enable/disable xa playing
	PARAMETERS:	0 - disable / 1 - enable
	RETURNS:	
**************************************************************************/

void XAsetStatus(int enable)
{
	if(enable)
		XAenable = 1;
	else
		XAenable = 0;
}


/**************************************************************************
	FUNCTION:	XAgetStatus()
	PURPOSE:	Get current status of XA playing
	PARAMETERS:	
	RETURNS:	0 - disabled / 1 - enabled
**************************************************************************/

/*int XAgetStatus()
{
	return XAenable;
}*/


/**************************************************************************
	FUNCTION:	XAcallback()
	PURPOSE:	Callback for XA audio stream
	PARAMETERS:	
	RETURNS:	
**************************************************************************/

static void XAcallback(int intr, u_char *result)
{
	// JIM : Ummm........not sure if we can have callbacks with the middleware stuff, 
	// but we'll find out in a little while
}


/**************************************************************************
	FUNCTION:	XAgetFileInfo()

	PURPOSE:	For the PSX, this get's position information for XA file on
				the CD, but we just need it's filename and an ADX file handle 
				thingy.

	PARAMETERS:	Filename

	RETURNS:	
**************************************************************************/

XAFileType *XAgetFileInfo(char *fileName)
{
	XAFileType	*xaf;
	char		fName[40];
	
	xaf = (XAFileType *)MALLOC0(sizeof(XAFileType));				//
	memset(xaf, 0, sizeof(*xaf));								//

	sprintf(xaf->fileInfo,fileName);
	xaf->startPos		= 0;									//
	xaf->endPos			= 0;									//
	xaf->status			= 0;									//

	xaf->adxt			= ADXT_Create(2, xaWorkArea, ADX_WORKSIZE);	//

	return xaf;
}


/**************************************************************************
	FUNCTION:	XAstart()
	PURPOSE:	Get CD ready to play XA audio stream
	PARAMETERS:	CD speed 0=single 1=double
	RETURNS:	
**************************************************************************/

void XAstart(int speed)
{
	// JIM - TODO : Dummy function !! This is all handled by the middleware stuff
}


/**************************************************************************
	FUNCTION:	XAstop()
	PURPOSE:	CD back to data - finished playing XA audio streams
	PARAMETERS:	
	RETURNS:	
**************************************************************************/

void XAstop()
{
	if (curXA)						// Is there an adx currently playing ?
	{								// yup, so........
		if(curXA->adxt)
		{
			ADXT_Destroy(curXA->adxt);		// Tell middleware to quit playing it......
			curXA = NULL;
		}
	} // end-if
}


/**************************************************************************
	FUNCTION:	XApause()
	PURPOSE:	CD back to data - pause playing XA audio streams
	PARAMETERS:	
	RETURNS:	
**************************************************************************/

void XApause()
{
	// JIM - TODO : Pause playing the current ADX stuff......


}


/**************************************************************************
	FUNCTION:	XAplayChannelOffset()
	PURPOSE:	Start playing channel from given XA audio stream
	PARAMETERS:	XA file, offset (sectors), channel number, loop
	RETURNS:	
**************************************************************************/

void XAplayChannelOffset(XAFileType *xaF, int offset, int channel, int loop, int vol)
{
	gdFsChangeDir("\\");						// Change to the audio directory on the GD
	gdFsChangeDir("MUSIC");						// 

	curXA		= xaF;							// Umm...for convenience let's have a global 
												// var letting us know the current ADX that's
												// playing.

	xaF->loop	= loop;							// loop flag
	xaF->vol	= vol;							// volume to play the ADX at 

	ADXT_StartFname(xaF->adxt, xaF->fileInfo);	// Start playing the XDA using the middlware
}


/**************************************************************************
	FUNCTION:	XAcheckPlay()
	PURPOSE:	Test if XA has begun playback
	PARAMETERS:	
	RETURNS:	0 = Not playing yet, 1 = Is playing now
**************************************************************************/

int XAcheckPlay()
{
unsigned char result[8];

	if (!XAenable)
		return 1;
	return 0;
}


/**************************************************************************
	FUNCTION:	XAstorePrevious()
	PURPOSE:	Store current XA playback track/channel
	PARAMETERS:	
	RETURNS:	
**************************************************************************/

void XAstorePrevious()
{
	XAData.prevXA = XAData.currXA;
	XAData.prevChannel = XAData.activeChannel;
	XAData.prevSector = XAData.currSector;
}


/**************************************************************************
	FUNCTION:	XArestartPrevious()
	PURPOSE:	Restart previously stored XA playback track/channel
	PARAMETERS:	
	RETURNS:	
**************************************************************************/

void XArestartPrevious()
{

	if (!XAenable)
	{
		printf("XArestartPrevious: disabled\n");
		XAData.prevXA->status = 0;
		return;
	}
	if (XAData.prevXA==NULL)
	{
		return;
	}

	
	XAData.activeChannel = XAData.prevChannel;
		
	XAData.currXA = XAData.prevXA;
	XAData.currSector = XAData.prevSector;
	XAData.prevXA->status = 1;

	printf("XArestartPrevious: %d [%d->%d]\n", XAData.prevSector,XAData.prevXA->startPos,XAData.prevXA->endPos);	
}

