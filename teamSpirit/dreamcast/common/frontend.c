/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: frontend.c
	Programmer	: Jim & Dave
	Date		: 30/03/99

----------------------------------------------------------------------------------------------- */

//#include "directx.h"

//#include "ultra64.h"
//#include <stdio.h>
#include "sonylibs.h"
#include "shell.h"
#include "islpsi.h"

#include "frontend.h"
#include "game.h"
#include "define.h"
#include "layout.h"
#include <islfont.h>
#include <islpad.h>
#include <islmem.h>
#include <islutil.h>
#include "textover.h"
#include "backdrop.h"
//#include "objects.h"
#include "menus.h"
#include "frogger.h"
#include "frogmove.h"
#include "overlays.h"
#include "main.h"
#include "newpsx.h"
#include "cam.h"
#include "collect.h"
#include "hud.h"
#include "multi.h"
#include "story.h"
#include "cam.h"
#include "levplay.h"
#include "e3_demo.h"
#include "babyfrog.h"
#include "types.h"
#include "lang.h"
#include "training.h"
#include "fadeout.h"
#include "options.h"

#ifdef PC_VERSION
#include <controll.h>
#include <stdio.h>
#include <banks.h>
#include <pcaudio.h>
#include <pcmisc.h>
#include <pcsprite.h>
#else PSX_VERSION
#include "audio.h"
#include "temp_psx.h"
#include "psxsprite.h"
//ma#include "memcard.h"
//ma#include <libspu.h>
#endif


//------ [ GLOBALS ] ---------------------------------------------------------------------------//

extern SPRITEOVERLAY *babyFlash;
int frameCount			= 0;

//int	frameCount2			= 0;
//int dispFrameCount		= 0;

char ActiveController	= 0;
unsigned long levelTime;

long backCharsX[4] = {64,3520,3520,64};
long backCharsY[4] = {420+64,420+64,3360-128-64,3360-128-64};
long backTextX[4] = {64+256+128,3640,3640,64+256+128};
long backTextY[4] = {160+64,160+64,3640,3640};
long backWinsTextX[4] = {64+512,3520-200,3520-200,64+512};
long backWinsTextY[4] = {420+128,420+128,3360-64,3360-64};


/*	--------------------------------------------------------------------------------
	Function		: GameLoop
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

extern SOUNDLIST soundList;					// Actual Sound Samples List
void GameLoop(void)
{
	int i;


#ifndef FINAL_MASTER
	if(((padData.debounce[0] & PAD_TRIANGLE) && (padData.digital[0] & PAD_SELECT)) || ((padData.debounce[0] & PAD_SELECT) && (padData.digital[0] & PAD_TRIANGLE)))
		camControlMode = 1 - camControlMode;

	if(camControlMode)
		ControlCamera();
#endif

	UpdateCheatText();


	switch (gameState.mode)
	{
	case INGAME_MODE:
		RunGameLoop();
		break;

	case DEMO_MODE:
		RunDemoMode();
		break;

	case FRONTEND_MODE:
		RunFrontendGameLoop();
		break;

	case TRAINING_MODE:
		RunTrainingMode();
		break;

	case LEVELCOMPLETE_MODE:
		RunLevelComplete( );
		break;

	case WORLDCOMPLETE_MODE:
		RunWorldComplete( );
		break;

	case GAMECOMPLETE_MODE:
		RunGameComplete( );
		break;

	case GAMEOVER_MODE:
		RunGameOver( );
		break;
		
	case INTRO_MODE:
		RunGameIntro( );
		break;
	
	case CAMEO_MODE:
	case PAUSE_MODE:
		RunPauseMenu();
		break;

	case MULTI_WINBATTLE_MODE:
		RunMultiWinCollect( );
		break;

	case MULTI_WINCOLLECT_MODE:
		RunMultiWinCollect( );
		break;
	case MULTI_WINRACE_MODE:
		RunMultiWinRace( );
		break;

	case MULTI_WINMATCH_MODE:
		RunMultiWinMatch( );
		break;

	case ARTVIEWER_MODE:
		RunArtViewer();
		break;
/*
	case CONTROLLERVIEW_MODE:
		RunControllerView();
		break;

#ifdef E3_DEMO
	case E3_LEVELSELECT_MODE:
		RunE3LevelSelect();
		break;
#endif
*/

	}

	frameCount++;

	i = NUM_FROGS;
	while(i--)
		if(player[i].inputPause > 0)
			player[i].inputPause = 0;
}



/*	--------------------------------------------------------------------------------
	Function		: CreateOverlaysFromLogo
	Purpose			: Creates a series of sprite overlays from a LOGO, at screen resolution
	Parameters		: LOGO*, left, top
	Returns			: 
	Info			: 
*/

void CreateOverlaysFromLogo(const LOGO* logo, int x, int y)
{
	int i, j;
	char texture[32];
	unsigned short *tex, t;

	tex = (unsigned short*)logo->tiles;

	for (i = 0; i < logo->height; i++)
		for (j = 0; j < logo->width; j++)
		{
			if ( (t=*(tex++)) )
			{
				sprintf(texture, "%s%02d", logo->texname, t);
				CreateAndAddSpriteOverlay( x + (j*32), y + (i*32), texture, 32, 32, 255, 0);
			}
		}
}

/*	--------------------------------------------------------------------------------
	Function		: RunLevelComplete
	Purpose			: What happens at the end of a level
	Parameters		: 
	Returns			: 
	Info			: 
*/

//extern long drawLandscape;
unsigned long createTime;
unsigned long createFrames = 30;

char currentName[64] = "";
char initialName[64] = "";
long waitFrame = 0;
long grade,moreCoins,levelOpened;
long cOption = 0;
long goFrontend;
TEXTOVERLAY *tText,*coinText,*nameText,*newBestText,*extraText;
TEXTOVERLAY *nText,*oText[2];
SPRITEOVERLAY *extraIcon,*coinIcon,*bIcon;
char coinStr[64];

void LevelCompleteProcessController(long pl)
{
	unsigned long changedLevel = 0;
	u16 button[4];

	button[pl] = padData.debounce[pl];
	
	if(button[pl] & PAD_UP)
	{
		if (cOption>0)
			cOption--;
	}	    
	else if(button[pl] & PAD_DOWN)
	{
		if ((cOption<1) && (oText[1]))
			cOption++;
	}

	if(button[pl] & (PAD_CROSS|PAD_START))
    {
		showEndLevelScreen = -1;

		if (cOption == 1)
			goFrontend = 1;
	}
}

fixed eolcamspeed = 4096*30;

int coinsMissed;
int coinCounter = 0;
void RunLevelComplete( )
{	
	long i;
	FVECTOR v1,v2,seUp;
	SPECFX *fx;
	IQUATERNION q;
	fixed slerp = 250;
	int exitAllowed = YES;
	SPRITEOVERLAY *coinOver;
	SPRITE *cur,*next;

	drawLandscape = 0;

#ifdef PSX_VERSION
/*ma	if(saveInfo.saveFrame)
	{
		frog[0]->draw = 0;
		skipTextOverlaysSpecFX = YES;
		if(!fadingOut)
			ChooseLoadSave();
		return;
	}
	skipTextOverlaysSpecFX = NO;
*/	
#endif

	frog[0]->draw = 1;
	if ((nText == NULL) && (keepFade == 0) && (showEndLevelScreen == -1))
	{
		ScreenFade(255,0,30);
		keepFade = YES;
//		GTInit(&modeTimer, 10);
	}

//	GTUpdate( &modeTimer, -1 );		
	babyFlash->draw = 0;

	if(!showEndLevelScreen || ((keepFade) && (fadingOut == 0)))
	{
		short wld = player[0].worldNum, lvl = player[0].levelNum;

		if ((levelTime/60) < worldVisualData[wld].levelVisualData[lvl].parTime && !dkPressed)
			worldVisualData[wld].levelVisualData[lvl].parTime = (levelTime/60);

		FreeTiledBackdrop();
//bb - don't want to save after retry/quit.
//moved further down, to save before retry/quit
//		SaveGame();

#ifdef E3_DEMO

		if (goFrontend)
		{
			StartE3LevelSelect();
			return;
		}
		else
			gameState.mode = INGAME_MODE;

#else
		if (goFrontend)
		{
			gameState.mode = FRONTEND_MODE;
			player[0].worldNum = WORLDID_FRONTEND;
			player[0].levelNum = LEVELID_FRONTEND1;
			player[0].character = FROG_FROGGER;
		}
		else if (gameState.single == STORY_MODE)
		{
			// otherwise we don't do something special at the end of this level...
			gameState.mode = INGAME_MODE;

			if( gameState.storySequenceLevel > 16 )
			{
				gameState.storySequenceLevel = 0;
				player[0].worldNum = WORLDID_FRONTEND;
				player[0].levelNum = LEVELID_FRONTEND1;
				gameState.mode = FRONTEND_MODE;
			}
			else
			{
				player[0].worldNum = storySequence[gameState.storySequenceLevel].worldNum;
				player[0].levelNum = storySequence[gameState.storySequenceLevel].levelNum;
			}
		}
		else
		{
			// todo: place Frogger 
			gameState.mode = INGAME_MODE;
		}
#endif // E3_DEMO

		FreeAllLists();
		frameCount = 0;

//		spawnCounter = 0;

		worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].levelOpen |= LEVEL_OPEN;
		InitLevel(player[0].worldNum,player[0].levelNum);

		showEndLevelScreen = 1; // Normal level progression is default
		frameCount = 0;		
	}
	else
	{
		Orientate( &q, &currTile[0]->dirVector[frogFacing[0]], &currTile[0]->normal );
		IQuatSlerp(&frog[0]->actor->qRot,&q,slerp,&frog[0]->actor->qRot);

		SetVectorFS(&v1, &frog[0]->actor->position);
		SetVectorFF(&v2, &currTile[0]->normal);
		ScaleVectorFF(&v2, ToFixed(300));
		AddToVectorFF(&v1,&v2);

		SlideVectorToVectorFF(&camTarget,&v1,eolcamspeed);
		SetVectorFF(&currCamTarget,&camTarget);

		SetVectorFS(&v1, &frog[0]->actor->position);
		SetVectorFF(&v2, &currTile[0]->dirVector[frogFacing[0]]);
		ScaleVectorFF(&v2, ToFixed(1000));
		AddToVectorFF(&v1,&v2);
		SetVectorFF(&v2, &currTile[0]->normal);
		ScaleVectorFF(&v2, ToFixed(300));
		AddToVectorFF(&v1,&v2);

		SlideVectorToVectorFF(&camSource,&v1,eolcamspeed);
		SetVectorFF(&currCamSource,&camSource);
		
		SetCamFF(currCamSource,currCamTarget);

		if(player[0].numSpawn != garibList.maxCoins)
		{
			if(nText)
				nText->draw = 0;
			exitAllowed = NO;
			if(coinText->xPos == coinText->xPosTo)
			{
				if(coinsMissed < garibList.maxCoins - player[0].numSpawn)
				{
					if(padData.digital[0] & PAD_CROSS)
						coinCounter += 10;
					else
						coinCounter += actFrameCount-lastActFrameCount;
			
					if(coinCounter > 10)
					{
						coinsMissed++;
						sprintf(coinStr,GAMESTRING(STR_MISSED_COINS),coinsMissed);//,garibList.maxCoins - player[0].numSpawn);
						coinCounter = 0;
						PlaySample(genSfx[GEN_COLLECT_COIN],NULL,0,SAMPLE_VOLUME,-1);
						coinOver = CreateAndAddSpriteOverlay(arcadeHud.coinsOver->xPos,arcadeHud.coinsOver->yPos,"SCOIN0001",205,273,0xff,0);
						coinOver->xPosTo += 10000;
						coinOver->speed = 4096*50;
					}
				}
				else
				{
					arcadeHud.coinsOver->draw = 0;
					if((grade == 0) && (nText) && (textEntry == -1) && (!(padData.digital[0] & PAD_CROSS)))
					{
						textEntry = NAME_LENGTH;
					}
					if(nText)
						nText->draw = 1;
					exitAllowed = YES;
				}
			}
		}
		else if((grade == 0) && (nText) && (textEntry == -1) && (!(padData.digital[0] & PAD_CROSS)))
			textEntry = NAME_LENGTH;

		if(grade == 0)
		{
			tText->r = 127+((rsin(actFrameCount*4200)+4096)*64)/4096;
			tText->g = 127+((rcos(actFrameCount*4300)+4096)*64)/4096;
			tText->b = 127+((rcos(actFrameCount*4400)+4096)*64)/4096;
			tText->a = 128+32+((rcos(actFrameCount*4000)+4096)*31)/4096;

			if (createTime<actFrameCount)
			{
				SetVectorFF(&seUp, &currTile[0]->normal);	
			
				if( (fx = CreateSpecialEffect( FXTYPE_SPARKLYTRAIL, &frog[0]->actor->position, &seUp, 90960, Random(8192)+8192, 0, 16192 )) )
				{
					SetFXColour(fx,255,Random(255),Random(255));
					SetVectorSS(&fx->rebound->point, &frog[0]->actor->position);
					SetVectorFF(&fx->rebound->normal, &seUp);
					fx->gravity = 8190;
				}

				createTime = actFrameCount + createFrames;
			}	

			tText->r;
			tText->g;
			tText->b;
			tText->a;
		}

		
		fixedPos = 1;
		fixedDir = 1;

#ifndef E3_DEMO
		if(nText)
		{
			#ifdef PSX_VERSION
				PsxNameEntryFrame();	//sets texEntry to 0 when done
			#endif

			if (textEntry==0)
			{
				nText->draw = 0;
				nText = NULL;
				cOption = 0;
				oText[0]->draw = 1;
				if(oText[1])
					oText[1]->draw = 1;
				if (grade==0)
				{
					strcpy(worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].parName,textString);
					SaveGame();
#ifdef PSX_VERSION
/*ma					if(saveInfo.saveFrame)
					{
						for ( cur = sprList.head.next; ( cur != &sprList.head ); cur = next)
						{
							next = cur->next;
							cur->draw = 0;
						}
					}
*/					
#endif
				}
			}

			sprintf(currentName,"%s %s",GAMESTRING(STR_ENTER_NAME),textString);
		}
		else
#endif
		{
			waitFrame++;
			if((waitFrame>10) && (exitAllowed))
				LevelCompleteProcessController(0);
		
			for (i=0; i<2; i++)
			{
				if(oText[i])
				{
					oText[i]->r = 128;
					oText[i]->g = 128;
					oText[i]->b = 128;
				}
			}

			oText[cOption]->r = 127+((rsin(actFrameCount*4200)+4096)*64)/4096;
			oText[cOption]->g = 127+((rcos(actFrameCount*4300)+4096)*64)/4096;
			oText[cOption]->b = 127+((rcos(actFrameCount*4400)+4096)*64)/4096;
		}


//		SlurpCamPosition();
		UpdateSpecialEffects();	

	}
}

//char *levText = "Level complete";
//char *levText2[4] = {"You beat the time!","Fantastic!","Good","Average"};
//char levTimeTemp[64] = "You took %imin %isec";
//char levTimeTemp2[64] = "You took %isec";
char levTimeText[64];

char *trophyName[3] = 
{
	"TRPHYGOLD",
	"TRPHYSILV",
	"TRPHYBRONZ",
};


void StartLevelComplete()
{
	ACTOR2 *c;
	SPRITE *cur,*next;
	int loop,i,num;
	FVECTOR tempVect;
	fixed dp,mindp;
	unsigned long time;
	

#ifdef PSX_VERSION
//ma	SpuSetKey(SPU_OFF,0xffffff);
#endif
	if(train)
	{
		if(train->bg)
		{
			train->bg->draw = 0;
			train->bg = 0;
		}
		if(train->txtover[0])
		{
			train->txtover[0]->draw = 0;
			train->txtover[0] = 0;
		}
		if(train->txtover[1])
		{
			train->txtover[1]->draw = 0;
			train->txtover[1] = 0;
		}
	}

	coinsMissed = coinCounter = 0;
	arcadeHud.collectText->draw = 0;

	FreeAmbientSoundList();
	ScreenFade(0,255,20);
	keepFade = NO;
	flashScreen = YES;

	levelOpened = 0;
	arcadeHud.collectText->draw = 0;

	if(worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].levelCompleted == 0)
		levelOpened = 1;
	worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].levelCompleted = 1;

	if((gameState.single == STORY_MODE) && (gameState.storySequenceLevel < NUM_STORY_LEVELS))
	{
		gameState.storySequenceLevel++;
		if(worldVisualData[storySequence[gameState.storySequenceLevel].worldNum].worldOpen == WORLD_CLOSED)
			levelOpened = 1;
		worldVisualData[storySequence[gameState.storySequenceLevel].worldNum].worldOpen = WORLD_OPEN;
		if(worldVisualData[storySequence[gameState.storySequenceLevel].worldNum].levelVisualData[storySequence[gameState.storySequenceLevel].levelNum].levelOpen == LEVEL_CLOSED)
			levelOpened = 1;
		worldVisualData[storySequence[gameState.storySequenceLevel].worldNum].levelVisualData[storySequence[gameState.storySequenceLevel].levelNum].levelOpen = LEVEL_OPEN;
	}

	cOption = 0;
	DisableHUD();


//#ifdef PSX_VERSION
	//FreeTextureAnimList();
	//FreeWaterObjectList();
	//FreeScenicObjectList();
//#endif


	InitTiledBackdrop ("LOGO");

	
	for(loop=0; loop<numBabies; loop++)
		babyIcons[loop]->draw = 0;
	babyFlash->draw = 0;


	if(!train)
	{
#ifdef PSX_VERSION
		PsxNameEntryInit();
#else
		PcNameEntryInit();
#endif
	}

	for (c = actList; c; c = c->next)
		c->draw = 0;
	
	
	for ( cur = sprList.head.next; ( cur != &sprList.head ); cur = next)
	{
		next = cur->next;
		cur->draw = 0;
	}

	createTime = actFrameCount + createFrames;


	time = actFrameCount/60;


	moreCoins = 0;

	if(garibList.maxCoins)
	{
		if(player[0].numSpawn == garibList.maxCoins)
		{
			coinText = CreateAndAddTextOverlay(2048+4096,980+300,GAMESTRING(STR_GOT_ALL_COINS),YES,255,font,TEXTOVERLAY_SHADOW);
			coinText->xPosTo = 2048;
			coinText->speed = 4096*75;
			coinIcon = CreateAndAddSpriteOverlay(2048 - 256 - 4096,980+600,"COINMEDAL",512,512,255,0);
			coinIcon->xPosTo = 2048-256;
			coinIcon->speed = 4096*75;
			if(player[0].numSpawn > worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].maxCoins)
			{
				worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].maxCoins = player[0].numSpawn;
				moreCoins = 1;
				for(i = 1,num = 0;i < NUM_STORY_LEVELS;i++)
				{
					if(worldVisualData[storySequence[i].worldNum].levelVisualData[storySequence[i].levelNum].maxCoins == 25)
						num++;
				}

		 		extraText = CreateAndAddTextOverlay(2048+4096+4096,980+1050,GAMESTRING(STR_OPENED_EXTRA_1 + num - 1),YES,255,font,TEXTOVERLAY_SHADOW);
				extraText->xPosTo = 2048;
				extraText->speed = 4096*75;
				extraIcon = CreateAndAddSpriteOverlay(2048-256-4096-4096,980+1350,storySequence[num].iconName,512,512,255,0);
				extraIcon->xPosTo = 2048-256;
				extraIcon->speed = 4096*75;
			}
		}
		else
		{
			sprintf(coinStr,GAMESTRING(STR_MISSED_COINS),0);//,garibList.maxCoins - player[0].numSpawn);
			coinText = CreateAndAddTextOverlay(2048+4096,980+300,coinStr,YES,255,font,TEXTOVERLAY_SHADOW);
			coinText->xPosTo = 2048;
			coinText->speed = 4096*75;
	 		extraText = CreateAndAddTextOverlay(2048+4096+4096,980+700,GAMESTRING(STR_NO_BONUS),YES,255,font,TEXTOVERLAY_SHADOW);
			extraText->xPosTo = 2048;
			extraText->speed = 4096*75;
			arcadeHud.coinsOver->draw = 1;
			arcadeHud.coinsOver->xPos = 3200 + 4096;
			arcadeHud.coinsOver->xPosTo = 3200;
			arcadeHud.coinsOver->speed = 4096*75;
			arcadeHud.coinsOver->yPosTo = arcadeHud.coinsOver->yPos = 980+350;
		}
	}

	if(player[0].numSpawn > worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].maxCoins)
	{
		worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].maxCoins = player[0].numSpawn;
		moreCoins = 1;
	}

	if(train)
	{
		grade = 3;
		coinText = CreateAndAddTextOverlay(2048+4096,980+300,GAMESTRING(STR_TRAINING_COMPLETE),YES,255,font,TEXTOVERLAY_SHADOW);
		coinText->xPosTo = 2048;
		coinText->speed = 4096*75;
	}
	else
	{
		if(time < worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].parTime)
			grade = 0;
		else if(time < (worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].parTime * 3)/2)
			grade = 1;
		else if(time < worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].parTime * 2)
			grade = 2;
	}


	waitFrame = 0;
	goFrontend = 0;

	if (grade==0)
	{
		worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].parTime = time;
		newBestText = CreateAndAddTextOverlay(2048,2900,GAMESTRING(STR_NEW_BEST_TIME),YES,255,font,TEXTOVERLAY_SHADOW);
	}

	if(train)
		sprintf(levTimeText,"");
	else
		sprintf(levTimeText,GAMESTRING(STR_YOUTOOKTIMEMIN),((int)time/60)%60,((int)time)%60);

	
	CreateAndAddTextOverlay(2048, 200+210,
		GAMESTRING(worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].description_str),
		YES, (char)0xFF, font, TEXTOVERLAY_SHADOW);
	
	if(grade < 3)
	{
		tText = CreateAndAddTextOverlay(2048-512, 850, levTimeText, YES, (char)0xFF, font, TEXTOVERLAY_SHADOW);
		bIcon = CreateAndAddSpriteOverlay(1024*3-512+4096,770,trophyName[grade],512,512,255,0);
		bIcon->speed = 4096*75;
		bIcon->xPosTo = 1024*3-512;
	}

	if(gameState.single == STORY_MODE)
	{
		oText[0] = CreateAndAddTextOverlay(2048, 3220, GAMESTRING(STR_PRESS_X_TO_CONTINUE), YES, (char)0xFF, font, TEXTOVERLAY_SHADOW);
		oText[1] = NULL;
	}
	else
	{
		oText[0] = CreateAndAddTextOverlay(2048, 3220, GAMESTRING(STR_RESTARTLEVEL), YES, (char)0xFF, font, TEXTOVERLAY_SHADOW);
		oText[1] = CreateAndAddTextOverlay(2048, 3610, GAMESTRING(STR_QUIT), YES, (char)0xFF, font, TEXTOVERLAY_SHADOW);
	}
	nText = NULL;
	
	if (grade==0)
	{
		nText = CreateAndAddTextOverlay(200, 3610, currentName, NO, (char)0xFF, font, TEXTOVERLAY_SHADOW);
		textEntry = -1;

		oText[0]->draw = 0;
		if(oText[1])
			oText[1]->draw = 0;
	}
	else
		nText = 0;

	actorAnimate(frog[0]->actor,FROG_ANIM_DANCE1,YES,NO,80,0);

	SetVectorFF(&camTarget,&currCamTarget);
	SetVectorFF(&camSource,&currCamSource);

	SubVectorFFF(&tempVect,&camTarget,&camSource);

	for(i = 0;i < 4;i++)
	{
		dp = DotProductFF(&tempVect,&currTile[0]->dirVector[i]);
		if((i == 0) || (dp < mindp))
		{
			frogFacing[0] = i;
			mindp = dp;
		}
	}
	if((grade != 0) && ((moreCoins) || (levelOpened)))
		SaveGame();
	if(train)
	{
		FREE(train);
		train = 0;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: RunWorldComplete
	Purpose			: What happens at the end of a world
	Parameters		: 
	Returns			: 
	Info			: 
*/
void RunWorldComplete( )
{

}


/*	--------------------------------------------------------------------------------
	Function		: RunGameComplete
	Purpose			: The final complete screen
	Parameters		: 
	Returns			: 
	Info			: 
*/
void RunGameComplete( )
{

}


/*	--------------------------------------------------------------------------------
	Function		: RunGameOver
	Purpose			: Runs the game-over screen
	Parameters		: 
	Returns			: 
*/
void RunGameOver( )
{
	GTUpdate( &modeTimer, -1 );
	if((keepFade == 0) && (!modeTimer.time || padData.debounce[0]))
	{
		ScreenFade(255,0,30);
		keepFade = 1;
	}
	if((keepFade) && (fadingOut == 0))
	{
		FreeAllLists();
#ifdef E3_DEMO
		StartE3LevelSelect();
#else
		gameState.mode = FRONTEND_MODE;

		player[0].lives = 10;
		player[0].levelNum = LEVELID_FRONTEND1;
		player[0].worldNum = WORLDID_FRONTEND;
		player[0].frogState &= ~FROGSTATUS_ISDEAD;
		InitLevel(player[0].worldNum,player[0].levelNum);

		frameCount = 0;
#endif
	}
}



void RunGameIntro( )
{
	// todo?
}

/*	--------------------------------------------------------------------------------
	Function		: StartGameOver
	Purpose			: Die, froggy! Wuaahahahahahahaha
	Parameters		: 
	Returns			: 
*/
void StartGameOver()
{
	gameState.mode = GAMEOVER_MODE;
	GTInit( &modeTimer, 3 );

//	FreeAllGameLists();

	CreateAndAddTextOverlay(2048, 1980, GAMESTRING(STR_GAMEOVER), YES, (char)255, font, TEXTOVERLAY_SHADOW);
	frog[0]->draw = 0;
}

/*	--------------------------------------------------------------------------------
	Function		: StartGameIntro
	Purpose			: Runs the intro sequence, ISL logos and so on
	Parameters		: 
	Returns			: 

	NOTE! This is a very crude placeholder intro sequence. Replace it!
*/
void StartGameIntro()
{
	// todo? pending animation playback, I suppose.

	player[0].worldNum = WORLDID_FRONTEND;
	player[0].levelNum = LEVELID_FRONTEND1;
	
	gameState.mode = LEVELCOMPLETE_MODE;
	gameState.multi = SINGLEPLAYER;
	GTInit( &modeTimer, 1 );
	showEndLevelScreen = 0;
}


/*	--------------------------------------------------------------------------------
	Function		: Multiplayer win screens
	Purpose			: countdown scores and play animations and stuff
	Parameters		: 
	Returns			: 
*/
typedef struct 
{
	TEXTOVERLAY		*countTextOver[MAX_FROGS];
	char countText[MAX_FROGS][32], current, mode;
	unsigned long countdown;
} MULTIGAMEWIN;

enum
{
	MGW_MOVING,
	MGW_COUNTING,
	MGW_FLASHING,
	MGW_FINISHED
};

MULTIGAMEWIN *mgWin;


void StartMultiWinGame( )
{
	int i;
	SPRITE *cur,*next;

	ScreenFade(255,128,30);
	keepFade = 1;
	fadeText = NO;

	mgWin = (MULTIGAMEWIN *)MALLOC0(sizeof(MULTIGAMEWIN));

	switch( multiplayerMode )
	{
	case MULTIMODE_RACE:
		gameState.mode = MULTI_WINRACE_MODE;


		for( i=0; i<NUM_FROGS; i++ )
		{
			sprintf( mgWin->countText[i], "%d:%02d:%02d", (mpl[i].timer/3600), ((mpl[i].timer%3600)/60), (mpl[i].timer%60) );
			mgWin->countTextOver[i] = CreateAndAddTextOverlay(5000, 1024+(512*i), mgWin->countText[i], NO, 255, font, TEXTOVERLAY_SHADOW);
			mgWin->countTextOver[i]->speed = 50<<12;
		}

		mgWin->mode = MGW_MOVING;
		break;

	case MULTIMODE_COLLECT:
	case MULTIMODE_BATTLE:
		mgWin->mode = MGW_MOVING;
		GTInit( &modeTimer, 3 );
		if(matchWinner != -1)
		{
			sprintf(countdownString,GAMESTRING(STR_MULTI_WINMATCH),frogPool[player[gameWinner].character].name);
		}
		else if(gameWinner == MULTI_ROUND_DRAW)
		{
			sprintf(countdownString,GAMESTRING(STR_MULTI_DRAW),frogPool[player[gameWinner].character].name);
		}
		else
		{
			sprintf(countdownString,GAMESTRING(STR_MULTI_PLAYERWINS),frogPool[player[gameWinner].character].name);
		}
		multiHud.centreText->a = 0;
		multiHud.centreText->scale = 4096;
		gameState.mode = MULTI_WINCOLLECT_MODE;
		break;
	}


	if (matchWinner != -1)
	{
		drawGame = 0;

		InitTiledBackdrop("LOGO");
		for ( cur = sprList.head.next; ( cur != &sprList.head ); cur = next)
		{
			next = cur->next;
			cur->draw = 0;
		}

	}
}

void SlideSpriteOverlayToPos(SPRITEOVERLAY *s,long x,long y,long fromX,long fromY,long numFrames)
{
	long speed;
	FVECTOR vect;

	vect.vx = x - fromX;
	vect.vy = y - fromY;
	vect.vz = 0;

	speed = MagnitudeF(&vect);
	speed = FMul(speed,gameSpeed);

	vect.vx = x - s->xPos;
	vect.vy = y - s->yPos;

	MakeUnit(&vect);

	if(fabs(s->xPos - x) <= speed/numFrames)
		s->xPos = x;
	else
		s->xPos += FMul(vect.vx,speed/numFrames);
	if(fabs(s->yPos - y) <= speed/numFrames)
		s->yPos = y;
	else
		s->yPos += FMul(vect.vy,speed/numFrames);

	s->xPosTo = s->xPos;
	s->yPosTo = s->yPos;
}

void SlideTextOverlayToPos(TEXTOVERLAY *s,long x,long y,long fromX,long fromY,long numFrames)
{
	long speed;
	FVECTOR vect;

	vect.vx = x - fromX;
	vect.vy = y - fromY;
	vect.vz = 0;

	speed = MagnitudeF(&vect);
	speed = FMul(speed,gameSpeed);

	vect.vx = x - s->xPos;
	vect.vy = y - s->yPos;

	MakeUnit(&vect);

	if(fabs(s->xPos - x) <= speed/numFrames)
		s->xPos = x;
	else
		s->xPos += FMul(vect.vx,speed/numFrames);
	if(fabs(s->yPos - y) <= speed/numFrames)
		s->yPos = y;
	else
		s->yPos += FMul(vect.vy,speed/numFrames);

	s->xPosTo = s->xPos;
	s->yPosTo = s->yPos;
}


void RunMultiWinRace( )
{
	SPRITEOVERLAY *s;
	TEXTOVERLAY *t;
	int n = mgWin->current, i,numArrived;
	int j;

	if( mgWin->mode == MGW_MOVING )
	{
		DEC_ALPHA(multiHud.timeTextHSec);
		DEC_ALPHA(multiHud.timeTextSec);
		DEC_ALPHA(multiHud.timeTextMin);
		numArrived = 0;
		for (n=0; n<NUM_FROGS; n++)
		{
			s = multiHud.backChars[n];
			t = mgWin->countTextOver[n];

			SlideSpriteOverlayToPos(s,700,1024 + 512*n-128,backCharsX[n],backCharsY[n],50);
			SlideTextOverlayToPos(multiHud.penaliseText[n],1300,1024 + 512*n,backTextX[n],backTextY[n],50);

			t->xPosTo = 2000;
			if((t->xPos == t->xPosTo))
				numArrived++;
			
		}

		// If overlays have got to proper positions, start counting
		if(numArrived == NUM_FROGS)
		{
			mgWin->mode = MGW_COUNTING;
			mgWin->countdown = 0;
			mgWin->current = 0;
		}
	}
	else if( mgWin->mode == MGW_COUNTING )
	{
		if(padData.digital[0] & PAD_CROSS)
			mgWin->countdown += 5;
		else
			mgWin->countdown += actFrameCount-lastActFrameCount;
		t = mgWin->countTextOver[mgWin->current];
		
		// Every half a second, countdown a penalty point and make a noise
		if( mgWin->countdown > 5 )
		{
			mgWin->countdown = 0;
			if( mpl[n].penalty ) // Countdown another penalty if we can
			{
				mpl[n].penalty -= 60;
				mpl[n].timer += 60;
				sprintf( t->text, "%d:%02d:%02d", (mpl[n].timer/3600), ((mpl[n].timer%3600)/60), (mpl[n].timer%60) );

   				PlaySample( genSfx[GEN_CLOCKTICK], NULL, 0, SAMPLE_VOLUME, -1 );
			}
			else if( n < NUM_FROGS-1 ) // Else if more frogs to go then bring on the next set of overlays
			{
				mgWin->current++;
			}
			else // Else we've finished, so prepare to replay.
			{
				for(i = 0;i < NUM_FROGS;i++)
				{
					for(j = 0;j < mpl[i].wins;j++)
						multiHud.trophies[i][j]->draw = 1;
				}
				mgWin->mode = MGW_FLASHING;
				multiHud.centreText->a = 0;
				GTInit( &modeTimer, 3 );
			}
		} 
	}
	else if( mgWin->mode == MGW_FLASHING )
	{
		GTUpdate( &modeTimer, -1 );
		if((gameWinner == MULTI_ROUND_DRAW) || (!modeTimer.time))
		{
			mgWin->mode = MGW_FINISHED;
			GTInit( &modeTimer, 4 );
		}
		else
		{
			multiHud.backChars[gameWinner]->r = multiHud.penaliseText[gameWinner]->r = mgWin->countTextOver[gameWinner]->r = 127+((rsin(actFrameCount*4000)+4096)*64)/4096;
			multiHud.backChars[gameWinner]->g = multiHud.penaliseText[gameWinner]->g = mgWin->countTextOver[gameWinner]->g = 127+((rcos(actFrameCount*4000)+4096)*64)/4096;
			multiHud.backChars[gameWinner]->b = multiHud.penaliseText[gameWinner]->b = mgWin->countTextOver[gameWinner]->b = 10;
		}
	}
	else if( mgWin->mode == MGW_FINISHED )
	{
		if(modeTimer.time)
		{
			multiHud.backChars[gameWinner]->r = multiHud.penaliseText[gameWinner]->r = mgWin->countTextOver[gameWinner]->r = 255;
			multiHud.backChars[gameWinner]->g = multiHud.penaliseText[gameWinner]->g = mgWin->countTextOver[gameWinner]->g = 255;
			multiHud.backChars[gameWinner]->b = multiHud.penaliseText[gameWinner]->b = mgWin->countTextOver[gameWinner]->b = 255;
			if(matchWinner != -1)
			{
				for( i=0; i<NUM_FROGS; i++ )
				{
					SlideSpriteOverlayToPos(multiHud.backChars[i],backCharsX[i],backCharsY[i],700,1024 + 512*n-128,50);
					DEC_ALPHA(multiHud.penaliseText[i]);
					mgWin->countTextOver[i]->xPosTo = 5200;
					DEC_ALPHA( mgWin->countTextOver[i] );
				}
			}
			else
			{
				INC_ALPHA(multiHud.timeTextHSec,255);
				INC_ALPHA(multiHud.timeTextSec,255);
				INC_ALPHA(multiHud.timeTextMin,255);
				for( i=0; i<NUM_FROGS; i++ )
				{
					SlideSpriteOverlayToPos(multiHud.backChars[i],backCharsX[i],backCharsY[i],700,1024 + 512*n-128,50);
					SlideTextOverlayToPos(multiHud.penaliseText[i],backTextX[i],backTextY[i],1300,1024 + 512*n,50);
					mgWin->countTextOver[i]->xPosTo = 5200;
					DEC_ALPHA( mgWin->countTextOver[i] );
				}
			}
			INC_ALPHA(multiHud.centreText,255);
		}

#ifdef E3_DEMO
		if (gameWinner == MULTI_ROUND_DRAW)
		{
			strcpy(countdownString, "DRAW");
			multiHud.centreText->draw = 1;
		}
#else
		if(matchWinner != -1)
			sprintf(countdownString,GAMESTRING(STR_MULTI_WINMATCH),frogPool[player[gameWinner].character].name);
		else if( gameWinner == MULTI_ROUND_DRAW )
			sprintf(countdownString,GAMESTRING(STR_MULTI_DRAW));
		else
			sprintf(countdownString,GAMESTRING(STR_MULTI_PLAYERWINS),frogPool[player[gameWinner].character].name);
		multiHud.centreText->scale = 4096;
#endif

		if(modeTimer.time)
			GTUpdate( &modeTimer, -1 );
		if(!modeTimer.time)
		{
			if(matchWinner != -1)
			{
				DoEndMulti();
				return;
			}


			ScreenFade(128,255,30);
			keepFade = 0;
			fadeText = NO;

			for( i=0; i<NUM_FROGS; i++ )
			{
				SubTextOverlay( mgWin->countTextOver[i] );
			}

//			FreeBackdrop();
//			drawGame = 1;

			FREE( mgWin );
			mgWin = NULL;
			frameCount = 2; // Not 0, because 0 and 1 are used to trigger initialisation stuff elsewhere
			ResetMultiplayer( );
//			EnableMultiHUD();

			gameState.mode = INGAME_MODE;
		}
	}

	UpDateMultiplayerInfo( );
}


void RunMultiWinCollect( )
{
	int i,j,maxScore = 0,numDraw = 0,num = 0;
	switch(mgWin->mode)
	{
		case MGW_FLASHING:
			if(modeTimer.time)
			{
				GTUpdate( &modeTimer, -1 );
				INC_ALPHA(multiHud.centreText,255);
			}
			if( !modeTimer.time )
			{
		//		FreeBackdrop();
		//		drawGame = 1;

				if(matchWinner != -1)
				{
					DoEndMulti();
					return;
				}

				multiHud.centreText->draw = 0;
				ScreenFade(128,255,30);
				keepFade = 0;
				fadeText = NO;

				FREE( mgWin );
				mgWin = NULL;
				frameCount = 2; // Not 0, because 0 and 1 are used to trigger initialisation stuff elsewhere
				ResetMultiplayer( );
		//		EnableMultiHUD();

				gameState.mode = INGAME_MODE;
			}
			break;

		case MGW_MOVING:
		case MGW_COUNTING:
			for(j = 0;j < NUM_FROGS;j++)
			{
				if(mpl[j].score > maxScore)
					maxScore = mpl[j].score;
			}

			for(j = 0;j < NUM_FROGS;j++)
			{
				if(mpl[j].score == maxScore)
					numDraw++;
			}

			if(mgWin->mode == MGW_MOVING)
			{
				for(j = 0;j < NUM_FROGS;j++)
				{
					if(mpl[j].score == maxScore)
					{
						SlideSpriteOverlayToPos(multiHud.backChars[j],2048 - 256 - (numDraw - 1)*256 + num*512,2100,backCharsX[j],backCharsY[j],30);
						num++;
					}
				}
				GTUpdate( &modeTimer, -1 );
				INC_ALPHA(multiHud.centreText,255);
				if(!modeTimer.time)
				{
					for(i = 0;i < NUM_FROGS;i++)
					{
						for(j = 0;j < mpl[i].wins;j++)
							multiHud.trophies[i][j]->draw = 1;
					}
					GTInit(&modeTimer,2);
					mgWin->mode = MGW_COUNTING;
				}
			}
			else
			{
				for(j = 0;j < NUM_FROGS;j++)
				{
					if(mpl[j].score == maxScore)
					{
						SlideSpriteOverlayToPos(multiHud.backChars[j],backCharsX[j],backCharsY[j],2048 - 256 - (numDraw-1)*256 + num*512,2100,30);
						num++;
					}
				}
				GTUpdate( &modeTimer, -1 );
				INC_ALPHA(multiHud.centreText,255);
				if(!modeTimer.time)
				{
					mgWin->mode = MGW_FLASHING;
				}
			}
			break;
	}
}


void RunMultiWinBattle( )
{
	int j;

	switch(mgWin->mode)
	{
		case MGW_FLASHING:
			if(modeTimer.time)
			{
				GTUpdate( &modeTimer, -1 );
				INC_ALPHA(multiHud.centreText,255);
			}
			break;

		case MGW_MOVING:
			for(j = 0;j < NUM_FROGS;j++)
			{
				
			}
			break;
	}
	if( !modeTimer.time )
	{
//		FreeBackdrop();
//		drawGame = 1;


		if(matchWinner != -1)
		{
			DoEndMulti();
			return;
		}


		multiHud.centreText->draw = 0;
		ScreenFade(128,255,30);
		keepFade = 0;
		fadeText = NO;

		FREE( mgWin );
		mgWin = NULL;

		frameCount = 2; // Not 0, because 0 and 1 are used to trigger initialisation stuff elsewhere
		ResetMultiplayer( );
//		EnableMultiHUD();

		gameState.mode = INGAME_MODE;
	}
}

void StartMultiWinMatch( )
{
	DisableHUD();

	gameState.mode = MULTI_WINMATCH_MODE;
}


void RunMultiWinMatch( )
{

}

TEXTOVERLAY *menuText[2] = {NULL,NULL};
int menuOption = -1;
int optionSelected = 0;
void DoEndMulti()
{
	int i;

#ifdef E3_DEMO
	StartE3LevelSelect();
#else


	if(menuText[0] == NULL)
	{
		menuText[0] = CreateAndAddTextOverlay(2048,2400,GAMESTRING(STR_RESTARTLEVEL),YES,0,font,TEXTOVERLAY_SHADOW);
		menuText[1] = CreateAndAddTextOverlay(2048,2700,GAMESTRING(STR_QUIT),YES,0,font,TEXTOVERLAY_SHADOW);
	}

	if(menuOption == -1)
	{
		menuOption = 0;
		optionSelected = 0;
	}

	if(optionSelected == 0)
	{
		INC_ALPHA(menuText[0],255);
		INC_ALPHA(menuText[1],255);
		if((menuOption == 1) && (padData.digital[0] & PAD_UP))
			menuOption = 0;
		else if((menuOption == 0) && (padData.digital[0] & PAD_DOWN))
			menuOption = 1;
		else if(padData.digital[0] & PAD_CROSS)
		{
			optionSelected = 1;
		}

		menuText[menuOption]->r = 127+((rsin(actFrameCount*4000)+4096)*64)/4096;
		menuText[menuOption]->g = 127+((rcos(actFrameCount*4000)+4096)*64)/4096;
		menuText[menuOption]->b = 10;

		menuText[1 - menuOption]->r = menuText[1 - menuOption]->g = menuText[1 - menuOption]->b = 255;

		return;
	}


	if(fadingOut == 0)
	{
		if(keepFade)
		{
			if(menuOption == 0)
			{
				ScreenFade(128,255,30);
				fadeText = NO;
				keepFade = 0;
			}
			else
			{
				ScreenFade(128,0,30);
				keepFade = 0;
			}
		}
		else
		{
			FreeTiledBackdrop();
			drawGame = 1;

			if(menuOption == 0)
			{
				RestartMulti();
				gameState.mode = INGAME_MODE;
				menuText[0]->draw = menuText[1]->draw = 0;
			}
			else
			{
				player[0].character = FROG_FROGGER;
				NUM_FROGS=1;

				if( gameState.multi != MULTIREMOTE )
					gameState.multi = SINGLEPLAYER;

				gameState.mode = FRONTEND_MODE;
				FreeMultiplayer( );
				InitLevel(WORLDID_FRONTEND,LEVELID_FRONTEND1);

				frameCount = 0;
				menuText[0] = NULL;
			}
			menuOption = -1;
		}
	}
	else
	{
		if(menuOption == 1)
		{
			if(multiHud.centreText)
				DEC_ALPHA(multiHud.centreText);
			if(multiHud.timeTextHSec)
				DEC_ALPHA(multiHud.timeTextHSec);
			if(multiHud.timeTextSec)
				DEC_ALPHA(multiHud.timeTextSec);
			if(multiHud.timeTextMin)
				DEC_ALPHA(multiHud.timeTextMin);
			for(i = 0;i < NUM_FROGS;i++)
			{
				DEC_ALPHA(multiHud.backChars[i]);
				DEC_ALPHA(multiHud.penaliseText[i]);
			}
		}
		DEC_ALPHA(menuText[0]);
		DEC_ALPHA(menuText[1]);
	}
#endif
}
