/****************************************************************

	ISL DREAMCAST LIBRARY (c) 2000 Blitz Games Ltd.		
	islvideo.c	Video player
														tgaulton
*****************************************************************/

#ifndef __ISLVIDEO_H__
#define __ISLVIDEO_H__

/* ---------------------------------------------------------------------------
 * - CONSTANTS
 * ---------------------------------------------------------------------------
 */
 
// Streaming modes.
#define STR_MODE24				1
#define STR_MODE16				0


// Border modes.
#define STR_BORDERS_OFF			0
#define STR_BORDERS_ON			1


// PlayStream return values.
#define	PLAYSTR_END				1
#define	PLAYSTR_USER			2
#define	PLAYSTR_ERROR			3

/* ---------------------------------------------------------------------------
 * - DATA TYPE AND STRUCTURE DECLARATIONS
 * ---------------------------------------------------------------------------
 */

typedef struct _StrDataType
{
		char		*strName;					// Stream file name. 
		short		mode;						// 24-Bit or 16-Bit streaming.
		short		drawBorders;				// != 0 if borders on.
		short		scrWidth;					// Screen res width.
		short		x;							// X,Y position. 
		short		y;
		short		width;						// Stream width and height.
		short		height;
		long		endFrame;					// Last frame No.
		long		vlcBufSize;					// Size of each VLC buffer (including header).
		short		volume;						// Left and Right ADPCM volume.
} StrDataType;

extern int quitAllVideo;

/* ---------------------------------------------------------------------------
 * - PUBLIC FUNCTION PROTOTYPES
 * ---------------------------------------------------------------------------
 */
 
short videoPlayStream(StrDataType *str, int palMode, short (*keyHandler)(void));

void videoSetAudioChannel(int channel);

#endif // __ISLVIDEO_H__
