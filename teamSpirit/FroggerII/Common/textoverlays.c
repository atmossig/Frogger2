/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: textoverlays.c
	Programmer	: Andy Eder
	Date		: 16/04/99 09:32:23

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


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
TEXTOVERLAY *CreateAndAddTextOverlay(short x,short y,char *text,char centred,char del,char r,char g,char b,char a,FONT *fontPtr,char flags,float waveAmplitude,float waveStart)
{
	TEXTOVERLAY *newItem;

	newItem = (TEXTOVERLAY *)JallocAlloc(sizeof(TEXTOVERLAY),YES,"TOLAY");
	AddTextOverlay(newItem);

	newItem->text		= text;
	newItem->flags		= flags;
	newItem->draw		= 1;
	newItem->font		= fontPtr;
		
	newItem->xPos		= x;
	newItem->yPos		= y;
	newItem->xPosOld	= x;
	newItem->yPosOld	= y;
	newItem->centred	= centred;
	newItem->r			= r;
	newItem->g			= g;
	newItem->b			= b;
	newItem->a			= a;

	newItem->kill		= del;
	newItem->life		= del;

	newItem->waveAmplitude	= waveAmplitude;
	newItem->waveStart		= waveStart;

	return newItem;
}


/*	--------------------------------------------------------------------------------
	Function		: PrintTextOverlayList
	Purpose			: prints the text overlay list
	Parameters		: none
	Returns			: void
	Info			:
*/
void PrintTextOverlays()
{
	TEXTOVERLAY *cur,*next;

	if(textOverlayList.numEntries == 0)
		return;

	for(cur = textOverlayList.head.next; cur != &textOverlayList.head; cur = next)
	{
		next = cur->next;

		if(cur->kill)
		{
//			SubTextOverlay(cur);
			continue;
		}

		if(cur->draw)
			PrintTextAsOverlay(cur);
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
	tover->draw = 1;
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
	tover->draw = 0;
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

	if(textOverlayList.numEntries == 0)
		return;

	dprintf"Freeing linked list : TEXTOVERLAY : (%d elements)\n",textOverlayList.numEntries));
	for(cur = textOverlayList.head.next; cur != &textOverlayList.head; cur = next)
	{
		next = cur->next;

		SubTextOverlay(cur);
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

	JallocFree((UBYTE **)&tOver);
}
