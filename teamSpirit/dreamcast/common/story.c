/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: sprite.c
	Programmer	: Jim Hubbard
	Date		: 25/02/00

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include "layout.h"
#include "Main.h"
#include "frogger.h"
#include "define.h"
#include "game.h"
#include "frontend.h"
#include "story.h"

#include <stdio.h>

#ifdef PC_VERSION
#include <gelf.h>
#include <mdxddraw.h>
#endif

#define IMAGETIME_SECONDS 10

FMV_DATA fmv[NUM_FMV_SEQUENCES] = 
{
	"",0,
	"TREX",2143,
	"INTRO",1640,
	"TREX",2143,
	"TREX",2143,
	"TREX",2143,
	"TREX",2143,
	"TREX",2143,
	"TREX",2143,
	"TREX",2143,
	"TREX",2143,
};

GAME_LEVEL storySequence[] = 
{
	{WORLDID_FRONTEND,3,1,FROG_FROGGER,0,""},
	{WORLDID_GARDEN,0,0,FROG_FROGGER,FMV_FROGGER_LILLIE_BABIES,"ICONRETROP"},

	{WORLDID_ANCIENT,0,1,FROG_LILLIE,FMV_SWAMPY_PLAN,"ICONRETROP"},
	{WORLDID_SUBTERRANEAN,0,0,FROG_FROGGER,0,"ICONRETROP"},

	{WORLDID_ANCIENT,2,1,FROG_LILLIE,FMV_SWAMPY_ESCAPES,"ICONRETROP"},
	{WORLDID_ANCIENT,1,0,FROG_LILLIE,0,"ICONRETROP"},

	{WORLDID_SUBTERRANEAN,1,1,FROG_FROGGER,FMV_LILLIE_TO_GATE,"ICONRETROP"},
	{WORLDID_SUBTERRANEAN,2,0,FROG_FROGGER,0,"ICONRETROP"},

	{WORLDID_LABORATORY,0,1,FROG_LILLIE,FMV_BOTH_AT_GATE,"ICONRETROP"},
	{WORLDID_LABORATORY,1,0,FROG_LILLIE,0,"ICONRETROP"},

	{WORLDID_SPACE,1,1,FROG_FROGGER,FMV_LILLIE_COMPUTER,"ICONMULTOP"},
	{WORLDID_SPACE,0,0,FROG_FROGGER,0,"ICONMULTOP"},

	{WORLDID_LABORATORY,2,1,FROG_LILLIE,FMV_SWAMPY_COMMERCIAL,"ICONMULTOP"},
	{WORLDID_SPACE,2,0,FROG_FROGGER,0,"ICONMULTOP"},

	{WORLDID_HALLOWEEN,0,0,FROG_FROGGER,FMV_TELEPORT_TO_EARTH,"ICONCHEATOP"},
	
	{WORLDID_HALLOWEEN,1,0,FROG_FROGGER,FMV_ENTER_HAUNTED_HOUSE,"ICONCHEATOP"},
	
	{WORLDID_HALLOWEEN,2,0,FROG_BABYFROG,FMV_LILLIE_IN_CAGE,"ICONCHEATOP"},
};

unsigned long currentChapter = 1;
unsigned long currentSubChapter = 1;

unsigned long finishTime = 0;

short *screenInfo;

void LoadCurrentFMVImage(void)
{
	char filename[256];
	//if (screenInfo)
	//	gelfDefaultFree(screenInfo);

#ifdef PC_VERSION
	sprintf(filename,"%stextures\\story\\SBscene%lu.%lu.bmp",baseDirectory,currentChapter,currentSubChapter);
#endif
	
	//todo: screen... stuff
	//screenInfo = GetGelfBmpDataAsShortPtr(filename,NULL,NULL);
	finishTime = actFrameCount+60*IMAGETIME_SECONDS;	
}


long InitChapterFMV(void)
{
	screenInfo = NULL;
	currentSubChapter = 1;
	
	LoadCurrentFMVImage();
	return 1;
}



long ShowChapterFMV(void)
{
	if (actFrameCount>finishTime)
	{
		return 1;
	}

	if (screenInfo)
	{
		//todo: Screen.. stuff
		//CopyShortDataToScreen(screenInfo);
	}

	return 0;
}

long DoneChapterFMV(void)
{
	screenInfo = 0;
	finishTime = 0;

	gameState.single = STORY_MODE;
					
	lastActFrameCount = actFrameCount;
	gameState.mode = LEVELCOMPLETE_MODE;
	GTInit( &modeTimer, 1 );
	showEndLevelScreen = 0;		

	return 1;
}

	
/*	--------------------------------------------------------------------------------
    Function		: StoryStartLevel
	Parameters		: void
	Returns			: 1 if we went into a new mode, 0 if we want to do a default 'next level' thing
*/
void StoryStartLevel(void)
{
	player[0].character = storySequence[gameState.storySequenceLevel].character;
}

