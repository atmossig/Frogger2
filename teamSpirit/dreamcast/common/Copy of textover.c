/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: textoverlays.c
	Programmer	: Andy Eder
	Date		: 16/04/99 09:32:23

----------------------------------------------------------------------------------------------- */

//#define F3DEX_GBI_2

#ifndef PSX_VERSION
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#endif

#include "ultra64.h"

#include "textover.h"
#include "maths.h"
#include "types2d.h"
#include "main.h"
#include <islmem.h>
#include "layout.h"

#ifdef PC_VERSION
#include <mdx.h>
#include <pcmisc.h>
#endif

long grabToTexture = 0;

//----- [ GLOBALS ] ----------------------------------------------------------------------------//

TEXTOVERLAYLIST	textOverlayList;			// the text overlay linked list


//----- [ TEXTOVERLAY HANDLING ] ---------------------------------------------------------------//

/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddTextOverlay
	Purpose			: creates and adds a text overlay string to the list
	Parameters		:
	Returns			: void
	Info			:
*/
TEXTOVERLAY *CreateAndAddTextOverlay(short x, short y, char *text, char centred, char alpha,
									 psFont *fontPtr, char flags, fixed waveAmplitude)
{
	TEXTOVERLAY *newItem;

	if( !fontPtr ) fontPtr = font;

	newItem = (TEXTOVERLAY *)MALLOC0(sizeof(TEXTOVERLAY));
	AddTextOverlay(newItem);

	newItem->text		= text;
	newItem->flags		= flags;
	newItem->draw		= 1;
	newItem->font		= fontPtr;
	newItem->scale		= 4096;
		
	newItem->xPos		= x;
	newItem->yPos		= y;
	newItem->xPosTo		= x;
	newItem->yPosTo		= y;
	newItem->centred	= centred;
	newItem->r			= 255;
	newItem->g			= 255;	// these were 128... why?
	newItem->b			= 255;
	newItem->a			= alpha;

	newItem->kill		= NO;
	newItem->life		= NO;

	newItem->waveAmplitude	= waveAmplitude;
	newItem->waveStart		= 0;

	return newItem;
}


/*	--------------------------------------------------------------------------------
	Function		: PrintTextOverlayList
	Purpose			: prints the text overlay list
	Parameters		: none
	Returns			: void
	Info			:
*/

#ifndef PSX_VERSION
extern MDX_FONT *pcFont;
extern MDX_FONT *pcFont2;
#endif

void PrintTextOverlays()
{
	TEXTOVERLAY *cur,*next;

	if(textOverlayList.numEntries == 0)
		return;

	for(cur = textOverlayList.head.next; cur != &textOverlayList.head; cur = next)
	{
		next = cur->next;

		if(cur->kill)
			continue;

		cur->font->alpha = cur->a;
		
		if(cur->draw)
		{
			if (cur->speed && !cur->centred)
			{
				fixed spd = FMul(gameSpeed, cur->speed)>>12;

				if( Fabs(cur->xPosTo-cur->xPos) )
				{
					cur->xPos += (cur->xPosTo > cur->xPos)?spd:-spd;
					
					if( Fabs(cur->xPosTo-cur->xPos) < Fabs(spd) )
					{
						cur->speed = 0;
						cur->xPos = cur->xPosTo;
					}
				}
				else if( Fabs(cur->yPosTo-cur->yPos) )
				{
					cur->yPos += (cur->yPosTo > cur->yPos)?spd:-spd;

					if( Fabs(cur->yPosTo-cur->yPos) < Fabs(spd) )
					{
						cur->speed = 0;
						cur->yPos = cur->yPosTo;
					}
				}
			}

#ifdef PSX_VERSION
			if ( cur->centred )
			{
				int length = fontExtentW(cur->font, cur->text)/2;
				cur->tWidth = length;
#if PALMODE==1
				fontPrint(cur->font, -length,-128+(cur->yPos>>4), cur->text, cur->r,cur->g,cur->b);
#else
				fontPrint(cur->font, -length,-120+((cur->yPos*15)>>8), cur->text, cur->r,cur->g,cur->b);
#endif
			}
			else
			{
				int length;

				length = fontExtentW(cur->font, cur->text);
				cur->tWidth = (length*640)/512;
#if PALMODE==1
				fontPrint(cur->font, -244+(cur->xPos>>3),-128+(cur->yPos>>4), cur->text, cur->r,cur->g,cur->b);
#else
				fontPrint(cur->font, -244+(cur->xPos>>3),-120+((cur->yPos*15)>>8), cur->text, cur->r,cur->g,cur->b);
#endif
			}

#else
			if (cur->centred)
				cur->tWidth = DrawFontStringAtLoc(cur->xPos*OVERLAY_X,cur->yPos*OVERLAY_Y,cur->text,
				RGBA_MAKE(cur->r,cur->g,cur->b,cur->a), (MDX_FONT *)cur->font,((float)cur->scale/4096) * ((float)rXRes/640.0),rXRes/2,0) - cur->xPos*OVERLAY_X;
			else
				cur->tWidth = DrawFontStringAtLoc(cur->xPos*OVERLAY_X,cur->yPos*OVERLAY_Y,cur->text,
				RGBA_MAKE(cur->r,cur->g,cur->b,cur->a), (MDX_FONT *)cur->font,((float)cur->scale/4096) * ((float)rXRes/640.0),0,0) - cur->xPos*OVERLAY_X;

			cur->tWidth *= (640.0/(float)rXRes);
#endif
			//utilPrintf("cur->xPos : %d, cur->yPos : %d, cur->xPosTo : %d, cur->yPosTo : %d\n", cur->xPos, cur->yPos, cur->xPosTo, cur->yPosTo);
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: EnableTextOverlay
	Purpose			: enables a text overlay for display
	Parameters		: TEXTOVERLAY *
	Returns			: none
	Info			:
*/
void EnableTextOverlay(TEXTOVERLAY *tover)
{
	if( tover ) tover->draw = 1;
}

/*	--------------------------------------------------------------------------------
	Function		: DisableTextOverlay
	Purpose			: disables a text overlay from displaying
	Parameters		: TEXTOVERLAY *
	Returns			: none
	Info			:
*/
void DisableTextOverlay(TEXTOVERLAY *tover)
{
	if (tover) tover->draw = 0;
}


/*	--------------------------------------------------------------------------------
	Function		: InitTextOverlayLinkedList
	Purpose			: initialises the text overlay linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void InitTextOverlayLinkedList()
{
	textOverlayList.numEntries = 0;
	textOverlayList.head.next = textOverlayList.head.prev = &textOverlayList.head;
}

/*	--------------------------------------------------------------------------------
	Function		: FreeTextOverlayLinkedList
	Purpose			: frees the text overlay linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeTextOverlayLinkedList()
{
	TEXTOVERLAY *cur,*next;

	if(textOverlayList.numEntries != 0)
	{
		for(cur = textOverlayList.head.next; cur != &textOverlayList.head; cur = next)
		{
			next = cur->next;

			SubTextOverlay(cur);
		}
	}

	InitTextOverlayLinkedList();
}

/*	--------------------------------------------------------------------------------
	Function		: AddTextOverlay
	Purpose			: adds a text overlay to the list
	Parameters		: TEXTOVERLAY *
	Returns			: void
	Info			: 
*/
void AddTextOverlay(TEXTOVERLAY *tOver)
{
	if(tOver->next == NULL)
	{
		textOverlayList.numEntries++;
		tOver->prev = &textOverlayList.head;
		tOver->next = textOverlayList.head.next;
		textOverlayList.head.next->prev = tOver;
		textOverlayList.head.next = tOver;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: SubTextOverlay
	Purpose			: removes a text overlay from the list
	Parameters		: TEXTOVERLAY *
	Returns			: void
	Info			: 
*/
void SubTextOverlay(TEXTOVERLAY *tOver)
{
	if(tOver->next == NULL)
		return;

	tOver->prev->next = tOver->next;
	tOver->next->prev = tOver->prev;
	tOver->next = NULL;
	textOverlayList.numEntries--;

	FREE(tOver);
}



/*	--------------------------------------------------------------------------------
	Function		: StringWrap
	Purpose			: wraps a string into a buffer and saves a pointer to the start of each line in an array
	Parameters		: string, max width of string (overlay units), work buffer, size of buffer, array, length of array, font
	Returns			: void
	Info			: 
*/
long StringWrap(const char* str, long maxWidth, char* buffer, long bufferSize, char** array, long arraySize, psFont *font)
{
#ifdef PC_VERSION
	WrapStringToArray(str, (maxWidth*rXRes)>>12, buffer, bufferSize, array, arraySize, (MDX_FONT*)font);
#else
//bb - do this in a bit
//#error "Insert string wrapping function here! (See mdxFont.cpp for PC version)"
#endif
}
