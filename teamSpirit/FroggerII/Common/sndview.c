/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: sndview.c
	Programmer	: James Healey
	Date		: 21/04/98

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

//----------------------------------------------------------------------------------------------//
//----- SYSTEM INCLUDES ------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------//
#include <ultra64.h>

//----------------------------------------------------------------------------------------------//
//----- USER INCLUDES --------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------//

#include "incs.h"

#ifdef PC_VERSION

void RunSndView() { }

#else

char *sfxNames[] =
{
	"FX NULL",
	"FX SHIRLEYSWIM FINS",
	"FX SHIRLEY LAND",
	"FX SHIRLEY LAUNCH",
	"FX MALLET WALK_LEFT",
	"FX MALLET WALK_RIGHT",
	"FX MALLET STAMPEDE",
	"FX MALLET WAIT",
	"FX GERERALWU WALK_LEFT",
	"FX GERERALWU WALK_RIGHT",
	"FX GENERALWU DIVESNAP",
	"FX GENERALWU HITGROUND",
	"FX SELWYNJOFF ATTACK",
	"FX SELWYNJOFF LAND",
	"FX SELWYNJOFF FLAP L",
	"FX SELWYNJOFF FLAP R",
	"FX SELWYNCANCER ATTACK",
	"FX SELWYNKIRK FLY",
	"FX DENNIS WALK",
	"FX DENNIS LAUNCH",
	"FX DENNIS LAND",
	"FX CHUCK CALL",
	"FX CHUCK ATTACK",
	"FX CHUCK JUGGLE",
"FX_CHUCK_WALK",
"FX_BUGLE_CALL",
"FX_BUGLE_ATTACK",
"FX_BUGLE_DEATH",
"FX_KLOSET_CALL",
"FX_KLOSET_WALK_LEFT",
"FX_KLOSET_WALK_RIGHT",
"FX_KLOSET_ATTACK",
"FX_KLOSET_SPINFLOOR",
"FX_KLOSET_BOXINGGLOVE",
"FX_KLOSET_BOMB",
"FX_CRUMPET_ATTACK",
"FX_CRUMPET_CALL",
"FX_TRACY_DANCING",
"FX_TRACY_CALL",
"FX_TRACY_WALK_LEFT",
"FX_TRACY_WALK_RIGHT",
"FX_TRACY_LAND",
"FX_TRACY_CHARGE",
"FX_TREX_CALL",
"FX_TREX_ATTACK",
"FX_TREX_FOOTSTEP_LEFT",
"FX_TREX_FOOTSTEP_RIGHT",
"FX_FLANNEL_WINGFLAP",
"FX_FLANNEL_CALL",
"FX_THRICE_ATTACK",
"FX_THRICE_IDLE",
"FX_MIKE_WALK_LEFT",
"FX_MIKE_WALK_RIGHT",
"FX_MIKE_ATTACK",
"FX_FUMBLE_CALL",
"FX_FUMBLE_WALK",
"FX_ETHEL_CALL",
"FX_ETHEL_WIND",
"FX_ETHEL_ATTACK",
 "FX_FRANKIE_ATTACK_LOOPED",
 "FX_FRANKIE_ATTACK",
 "FX_FRANKIE_ATTACK2",
 "FX_FRANKIE_GENERALHUM",
 "FX_FRANKIE_WALK_LEFT",
 "FX_FRANKIE_WALK_RIGHT",
 "FX_FRANKIE_CALL",
 "FX_FRANKIE_DEATH",
 "FX_REGGIE_WALK",
 "FX_REGGIE_DEATH",
 "FX_SWISH_WALK_LEFT",
 "FX_SWISH_WALK_RIGHT",
 "FX_SWISH_ATTACK",
 "FX_KEG_CALL",
 "FX_KEG_WALK",
 "FX_CHESTER_ATTACK",
 "FX_CHESTER_WALK",
 "FX_CHESTER_CALL",
 "FX_SPANK_CALL_1",
 "FX_SPANK_CALL_2",
 "FX_SPANK_SWING",
 "FX_SPANK_ATTACK",
 "FX_SPANK_DROP",
 "FX_YOOFOW_FLY",
 "FX_YOOFOW_ATTACK",
 "FX_OPEC_IDLE",
 "FX_OPEC_ATTACK_LOOP",
 "FX_OPEC_ATTACK_SINGLE",
 "FX_CYMON_GENERAL",
 "FX_CYMON_WALK_LEFT",
 "FX_CYMON_WALK_RIGHT",
 "FX_SUCKER_ATTACK_LOOP",
 "FX_SUCKER_ATTACK",
 "FX_SUCKER_SPIT",
 "FX_SUCKER_CALL_1",
 "FX_SUCKER_CALL_2",
 "FX_SUCKER_CALL_3",
 "FX_SUCKER_CRYSTAL",
 "FX_ROBOT_WALK",
 "FX_ROBOT_ATTACK",
 "FX_ROBOT_GENERAL",
 "FX_BOVVA_GENERAL",
 "FX_BOVVA_ATTACK",
 "FX_CANNONBALL_WALK",
 "FX_SAMTEX_WALK",
 "FX_SAMTEX_FUSE",
 "FX_SAMTEX_EXPLODE",
 "FX_CHICKEN_CLUCKNORMAL",
 "FX_CHICKEN_CLUCKHALFMAD",
 "FX_CHICKEN_CLUCKMAD",
 "FX_CHICKEN_FART",
 "FX_CHICKEN_BELCH",
 "FX_SCUTTLE_GENERIC_1",
 "FX_SCUTTLE_GENERIC_2",
 "FX_SCUTTLE_GENERIC_3",
 "FX_SPANK_CALL_3",
 "FX_SPANK_CALL_4",
 "FX_SPANK_OOF",
 "FX_CHESTER_GROWL",
 "FX_ROBOT_GENERAL_2",
 "FX_ROBOT_GENERAL_3",
 "FX_ROBOT_GENERAL_4",
 "FX_ROBOT_GENERAL_5",
 "FX_GENERALWU_PULLUP ",
 "FX_CHUCK_JUGGLE_ENDING",
 "FX_TRACEY_JUMP",
 "FX_TREX_PANTING_LOOP",
 "FX_FUMBLE_SPOTBALL",
 "FX_FUMBLE_KEEPBALANCE",
 "FX_FUMBLE_PANTING_LOOP", 
 "FX_KLOSET_PIANOFALL",
 "FX_BALLBOUNCE_RUBBER",
 "FX_BALLBOUNCE_BOWLING",  
 "FX_BALLBOUNCE_POWER", 
 "FX_BALLBOUNCE_BEARING",   
 "FX_BALLBOUNCE_BEACH",
 "FX_BALLBOUNCE_SNOW",   
 "FX_BALLBOUNCE_CRYSTAL",
 "FX_BALLBOUNCE_CRYSTAL_SMASH",
 "FX_BALL_BURST",
 "FX_COLLECT_BONUS_1",
 "FX_COLLECT_BONUS__ECHO",
 "FX_COLLECT_BONUS_1",
 "FX_COLLECT_POWERUP_1",
 "FX_COLLECT_POWERUP_2",
 "FX_COLLECT_POWERUP_3",
 "FX_COLLECT_POWERUP_4", 
 "FX_COLLECT_POWERUP_5",
 "FX_HAND_FOOTSTEP_LEFT",
 "FX_HAND_FOOTSTEP_RIGHT",
 "FX_HAND_SKID",
 "FX_HAND_SKID_LOOP",
 "FX_HAND_JUMP_SMALL",
 "FX_HAND_JUMP_BIG",
 "FX_HAND_LAND",
 "FX_HAND_HURT_1",
 "FX_HAND_HURT_2",
 "FX_HAND_HURT_3",
 "FX_HAND_PUSH_1",
 "FX_HAND_PUSH_2",
 "FX_HAND_FALL",
 "FX_HAND_DIE_1",
 "FX_HAND_DIE_2",
 "FX_HAND_FINGERTAP_1",
 "FX_HAND_FINGERTAP_2",
 "FX_HAND_FINGERCLICK",
 "FX_HAND_FISTSLAM",
 "FX_HAND_THROW_1",
 "FX_HAND_THROW_2",
 "FX_HAND_THROW_3",
 "FX_HAND_THROW__LOOP",
 "FX_HAND_WHACKBALL",
 "FX_HAND_BOUNCETAP_1",
 "FX_HAND_BOUNCETAP_2",
 "FX_CAST_SPELL_1",
 "FX_CAST_SPELL_2",
 "FX_CAST_SPELL_3",
 "FX_POINT_DIRECTION_1",
 "FX_POINT_DIRECTION_2",
 "FX_GENERIC_BLEEP_1",
 "FX_GENERIC_BLEEP_2",
 "FX_GENERIC_BLEEP_3",
 "FX_GENERIC_BLEEP_4",
 "FX_ROLLING_SNOWBALL",
/* "FX_IMPACT_STONE_SMALL   
 "FX_IMPACT_STONE_MEDIUM  
 "FX_IMPACT_STONE_LARGE   
 "FX_IMPACT_WOOD_SMALL    
 "FX_IMPACT_WOOD_MEDIUM   
 "FX_IMPACT_WOOD_LARGE    
 "FX_IMPACT_METAL_SMALL   
 "FX_IMPACT_METAL_MEDIUM  
 "FX_IMPACT_METAL_LARGE   
 "FX_IMPACT_SAND_SMALL    
 "FX_IMPACT_SAND_MEDIUM   
 "FX_IMPACT_SAND_LARGE    
 "FX_IMPACT_SNOW_SMALL    
 "FX_IMPACT_SNOW_MEDIUM   
 "FX_IMPACT_SNOW_LARGE    
 "FX_IMPACT_MUD_SMALL     
 "FX_IMPACT_MUD_MEDIUM    
 "FX_IMPACT_MUD_LARGE     
 "FX_STONE_SCAPE_LARGE    
 "FX_STONE_SCAPE_SMALL    
 "FX_METAL_SCRAPE_LARGE   
 "FX_METAL_SCRAPE_SMALL   
 "FX_WOOD_SCRAPE_LARGE    
 "FX_WOOD_SCRAPE_SMALL    
 "FX_ICE_SCRAPE_LARGE     
 "FX_ICE_SCRAPE_SMALL     
 "FX_SWITCH_             
 "FX_SWITCH_             
 "FX_SWITCH_             
 "FX_SWITCH_             
 "FX_SWITCH_             
 "FX_SWITCH_             
 "FX_SWITCH_             
 "FX_SWITCH_             
 "FX_FROG_CALL            
 "FX_TROUSERS_FALLDOWN    
 "FX_IMPACT_WATER_SMALL   
 "FX_IMPACT_WATER_MEDIUM  
 "FX_IMPACT_WATER_LARGE   
 "FX_EERIE_WIND           
 "FX_BIRD_TWEET_         
 "FX_BIRD_TWEET_         
 "FX_BIRD_TWEET_         
 "FX_BIRD_TWEET_         
 "FX_BIRD_TWEET_         
 "FX_BIRD_TWEET_         
 "FX_WATERFALL            
 "FX_WATERFALL_STEREO     
 "FX_LAVA_               
 "FX_LAVA_               
 "FX_BAT_WINGFLAP         
 "FX_BAT_CALL             
 "FX_BAT_CALL_ECHO        
 "FX_DRIPING_WATER        
 "FX_FOUNTAIN_           
 "FX_FOUNTAIN_           
 "FX_FOUNTAIN_STEREO      
 "FX_SEA_WAVES_          
 "FX_SEA_WAVES_          
 "FX_SEA_OFF_EDGE         
 "FX_HEAVY_SWITCH         
 "FX_BADGLOVE_LAUGH_     
 "FX_RAIN                 
 "FX_RAIN_HEAVY           
 "FX_THUNDER              
 "FX_HAND_JUMP          
 "FX_HAND_JUMP          
 "FX_HAND_JUMP          
 "FX_HAND_JUMP          
 "FX_HAND_RUNJUMP       
 "FX_HAND_RUNJUMP       
 "FX_HAND_RUNJUMP       
 "FX_HAND_RUNJUMP       
 "FX_HAND_PUSH          
 "FX_HAND_PUSH          
 "FX_HAND_PUSH          
 "FX_HAND_PUSH          
 "FX_HAND_HURT          
 "FX_HAND_HURT          
 "FX_HAND_HURT          
 "FX_HAND_HURT          
 "FX_HAND_SPELL         
 "FX_HAND_SPELL         
 "FX_HAND_SPELL         
 "FX_HAND_SPELL         
 "FX_HAND_VICTORY       
 "FX_HAND_VICTORY       
 "FX_HAND_VICTORY       
 "FX_HAND_VICTORY       
 "FX_HAND_DIE           
 "FX_HAND_DIE           
 "FX_HAND_DIE           
 "FX_HAND_FALL           
 "FX_BUBBLES_            
 "FX_BUBBLES_            
 "FX_BUBBLES_            
 "FX_COLLECT_BONUS_      
 "FX_COLLECT_BONUS_      
 "FX_COLLECT_BONUS_      
 "FX_CAST_SPELL_         
 "FX_CAST_SPELL_         
 "FX_HAND_WOW            
 "FX_BIGSPELL             
 "FX_INTRO_BUBBLES        
 "FX_INTRO_THUNDER        
 "FX_INTRO_HANDOFF        
 "FX_INTRO_SPLASH         
 "FX_INTRO_HANDOUTWINDOW  
 "FX_INTRO_FLOORBREAK     
 "FX_INTRO_WIZARDFALL     
 "FX_INTRO_RUMBLE         
 "FX_INTRO_EXPLOSION      
 "FX_INTRO_WIND           
 "FX_INTRO_CHANGETOBALL   
 "FX_INTRO_BALLBOUNCE     
 "FX_INTRO_HANDHITGROUND  
 "FX_INTRO_HANDOUCH       
 "FX_INTRO_HANDGETUP      
 "FX_INTRO_HANDYEAH       
 "FX_INTRO_BADGLOVELAUGH  
 "FX_PORTCULLIS          
 "FX_PORTCULLIS          
 "FX_WATER_LAPPING_      
 "FX_WATER_LAPPING_      
 "FX_SPLASH_SMALL        
 "FX_SPLASH_SMALL        
 "FX_SPLASH_SMALL        
 "FX_SPLASH_SMALL        
 "FX_ROTORBLADE           
 "FX_SUCKER               
 "FX_ROCK_SMASH_         
 "FX_ROCK_SMASH_         
 "FX_ROCK_SMASH_         
 "FX_TELEPORT_           
 "FX_TELEPORT_           
 "FX_ENTRO_WIND           
 "FX_ENTRO_WHOOSH         
 "FX_ENTRO_STONELAND      
 "FX_ENTRO_FSTEP          
 "FX_ENTRO_BIRD          
 "FX_ENTRO_BIRD          
 "FX_ENTRO_WHAA           
 "FX_ENTRO_WHOA           
 "FX_ENTRO_BGFALL         
 "FX_ENTRO_BGLAND         
 "FX_ENTRO_JUMP           
 "FX_ENTRO_HANDATTACH     
 "FX_ENTRO_BGRUN          
 "FX_ENTRO_CASTSPELL      
 "FX_ENTRO_SPELLHIT       
 "FX_ENTRO_TRANSFORM      
 "FX_ENTRO_CHANGE         
 "FX_ENTRO_BLOW           
 "FX_ENTRO_WIZWALK        
 "FX_ENGINE_FAIL          
 "FX_ENGINE_START         
 "FX_ENGINE_RUNNING       
 "FX_ENGINE_FAIL         
 "FX_JETPACK              */
};

#define NUM_SNDVIEW_SFX		180

TEXTOVERLAY *sfxName,*musName;

unsigned long sfxNum	= 0;
unsigned long musNum	= 0;
unsigned long sfx		= 1;
unsigned long mus		= 0;
int sfxRes				= 0;

float waterFreq[2] = { 80, 41 };
float waterFactor[2] = { 0.004, 0.008 };
float waterF = 0.1;
float waterWaveHeight[2] = { 20, -10 };
VECTOR waterCentre[2] = { { 25,0,25 },{ -15,0,0 } };

float waterWaveHeightBase[2] = { 10,8 };
float waterWaveHeightAmp[2] = { 20,10 };
float waterWaveHeightFreq[2] = { 101, 102 };
float watRot[2] = { 0,0 };

int waterMode = 1;
int waterX = 11;
int waterY = 11;

float *waterVel = NULL;

float dist[2];
VECTOR tempVect;

float watX = 0,watY = -30,watZ = 50;
ACTOR2 *watActor = NULL;

/*	--------------------------------------------------------------------------------
	Function		: RunSndView
	Purpose			: runs the sound viewer
	Parameters		: 
	Returns			: void
	Info			: 
*/
void RunSndView()
{
	static TEXTOVERLAY *title,*titleShadow,*titleSfx,*titleMus;
	static u16 button,lastbutton;
	static s16 stickX,stickY;
	static SPRITEOVERLAY *curPane;
	SPRITEOVERLAY *sprPane;
	
	if(frameCount == 1)
	{
		FreeAllLists();
		
		LoadTextureBank(SYSTEM_TEX_BANK);
		LoadTextureBank(INGAMEGENERIC_TEX_BANK);
		LoadObjectBank(INGAMEGENERIC_OBJ_BANK);

		title = CreateAndAddTextOverlay(30,24,"sound player",NO,NO,255,255,255,255,smallFont,0,0,0);
		titleShadow = CreateAndAddTextOverlay(32,26,"sound player",NO,NO,0,0,0,255,smallFont,0,0,0);

		// bottom pane --------------------------------------------------------------------------
		CreateAndAddTextOverlay(75,125,"select first or last item",NO,NO,255,255,255,255,oldeFont,0,0,0);
		CreateAndAddTextOverlay(77,127,"select first or last item",NO,NO,0,0,0,255,oldeFont,0,0,0);

		CreateAndAddTextOverlay(75,140,"play selected item",NO,NO,255,255,255,255,oldeFont,0,0,0);
		CreateAndAddTextOverlay(77,142,"play selected item",NO,NO,0,0,0,255,oldeFont,0,0,0);

		CreateAndAddTextOverlay(75,155,"stop playback",NO,NO,255,255,255,255,oldeFont,0,0,0);
		CreateAndAddTextOverlay(77,157,"stop playback",NO,NO,0,0,0,255,oldeFont,0,0,0);
		// bottom pane --------------------------------------------------------------------------

		// middle pane --------------------------------------------------------------------------
		titleSfx = CreateAndAddTextOverlay(40,55,"SNDFX NAMES",NO,NO,0,255,0,255,oldeFont,0,0,0);
		sfxName = CreateAndAddTextOverlay(45,65,"",NO,NO,255,255,255,255,oldeFont,0,2,0);
		sfxName->text = sfxNames[sfxNum];

		titleMus = CreateAndAddTextOverlay(40,85,"MUSIC TRACK",NO,NO,0,255,0,95,oldeFont,0,0,0);
		musName = CreateAndAddTextOverlay(45,95,"",NO,NO,255,255,255,95,oldeFont,0,0,0);
		musName->text = gameSongs[musNum].tuneName;
		// middle pane --------------------------------------------------------------------------

		// bottom pane --------------------------------------------------------------------------
		CreateAndAddSpriteOverlay(30,120,"shldr_l.bmp",16,16,255,255,255,255,0);
		CreateAndAddSpriteOverlay(47,120,"shldr_r.bmp",16,16,255,255,255,255,0);

		CreateAndAddSpriteOverlay(34,135,"button_a.bmp",16,16,255,255,255,255,0);
		CreateAndAddSpriteOverlay(34,150,"button_b.bmp",16,16,255,255,255,255,0);
		// bottom pane --------------------------------------------------------------------------

		curPane = CreateAndAddSpriteOverlay(43,63,"tippane.bmp",200,12,255,255,255,191,0);

		// top pane
		sprPane = CreateAndAddSpriteOverlay(25,20,"tippane.bmp",270,25,255,255,255,191,0);
		// middle pane
		sprPane = CreateAndAddSpriteOverlay(25,46,"tippane.bmp",270,68,255,255,255,95,0);
		// bottom pane
		sprPane = CreateAndAddSpriteOverlay(25,115,"tippane.bmp",270,105,255,255,255,191,0);

		// add the water actor
		watActor = CreateAndAddActor("eleven.obe",watX,watY,watZ,0,0,0);
		watActor->flags = ACTOR_DRAW_ALWAYS;

		watActor->actor->qRot.x = -0.25;
		watActor->actor->qRot.y = 0;
		watActor->actor->qRot.z = 0;
		watActor->actor->qRot.w = 1;

		sfxNum	= 0;
		musNum	= 0;
		sfxRes	= 0;
	}

	// read controller
	button = controllerdata[ActiveController].button;
	stickX = controllerdata[ActiveController].stick_x;
	stickY = controllerdata[ActiveController].stick_y;

	if((button & CONT_UP) && !(lastbutton & CONT_UP))
	{
		sfx = 1;		// select sound effects
		mus = 0;

		MusHandleStop(sfxRes,0);
		MusHandleStop(audioCtrl.musicHandle[0],0);
		audioCtrl.currentTrack[0] = 0;

		curPane->xPos = 43;	curPane->yPos = 63;
	}
	if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
	{
		sfx = 0;
		mus = 1;		// select music tracks

		MusHandleStop(sfxRes,0);
		MusHandleStop(audioCtrl.musicHandle[0],0);
		audioCtrl.currentTrack[0] = 0;

		curPane->xPos = 43;	curPane->yPos = 93;
	}

	if((button & CONT_LEFT) && !(lastbutton & CONT_LEFT))
	{
		// select required sound effect
		if(sfx)
		{
			if(sfxNum > 0)
			{
				MusHandleStop(sfxRes,0);
				sfxNum--;
			}

			sfxName->text = sfxNames[sfxNum];
		}
		// select required music track
		else
		{
			if(musNum > 0)
			{
				MusHandleStop(sfxRes,0);
				audioCtrl.currentTrack[0] = 0;
				musNum--;
			}

			musName->text = gameSongs[musNum].tuneName;
		}
	}

	if((button & CONT_RIGHT) && !(lastbutton & CONT_RIGHT))
	{
		// select required sound effect
		if(sfx)
		{
			if(sfxNum < (NUM_SNDVIEW_SFX - 1))
			{
				MusHandleStop(sfxRes,0);
				sfxNum++;
			}

			sfxName->text = sfxNames[sfxNum];
		}
		// select required music track
		else
		{
			if(musNum < (NUM_TRACKS - 1))
			{
				MusHandleStop(sfxRes,0);
				audioCtrl.currentTrack[0] = 0;
				musNum++;
			}

			musName->text = gameSongs[musNum].tuneName;
		}
	}

	if((button & CONT_A) && !(lastbutton & CONT_A))
	{
		// play currently selected sample
		if(sfx)
		{
			MusHandleStop(audioCtrl.musicHandle[0],0);
			sfxRes = PlaySampleNot3D(sfxNum,255,128,128);
		}
		// play currently selected music track
		else
		{
			MusHandleStop(sfxRes,0);
			PrepareSong(musNum,0);
		}
	}

	if((button & CONT_B) && !(lastbutton & CONT_B))
	{
		// stop current sound effect
		if(sfx)
		{
			MusHandleStop(sfxRes,0);
		}
		// stop current music track
		else
		{
			MusHandleStop(audioCtrl.musicHandle[0],0);
			audioCtrl.currentTrack[0] = 0;
		}
	}

	if((button & CONT_L) && !(lastbutton & CONT_L))
	{
		// set to start of sound effect list
		if(sfx)
		{
			MusHandleStop(sfxRes,0);
			sfxNum = 0;
		}
		// set to start of music track list
		else
		{
			MusHandleStop(audioCtrl.musicHandle[0],0);
			audioCtrl.currentTrack[0] = 0;
			musNum = 0;
		}

		musName->text = gameSongs[musNum].tuneName;
		sfxName->text = sfxNames[sfxNum];
	}

	if((button & CONT_R) && !(lastbutton & CONT_R))
	{
		// set to end of sound effect list
		if(sfx)
		{
			MusHandleStop(sfxRes,0);
			sfxNum = (NUM_SNDVIEW_SFX - 1);
		}
		// set to start of music track list
		else
		{
			MusHandleStop(audioCtrl.musicHandle[0],0);
			audioCtrl.currentTrack[0] = 0;
			musNum = (NUM_TRACKS - 1);
		}

		musName->text = gameSongs[musNum].tuneName;
		sfxName->text = sfxNames[sfxNum];
	}

	if((button & CONT_G) && !(lastbutton & CONT_G))
	{
		MusHandleStop(sfxRes,0);
		MusHandleStop(audioCtrl.musicHandle[0],0);
		audioCtrl.currentTrack[0] = 0;

		if(waterVel)
			JallocFree((UBYTE **)&waterVel);

		FreeAllLists();
		developmentMode = 0;
		gameState.mode = DEVELOPMENT_MODE;
		return;
	}

	lastbutton = button;

	if(sfx)
	{
		sfxName->a	= 255;
		titleSfx->a = 255;
		musName->a	= 95;
		titleMus->a = 95;
	}
	else if(mus)
	{
		sfxName->a	= 95;
		titleSfx->a = 95;
		musName->a	= 255;
		titleMus->a = 255;
	}

	curPane->a = 255 * Fabs(sinf(frameCount/12.5));

	// update the water
	if(watActor)
	{
		// assuming non-drawlisted object
		VECTOR *wv;
		int k,j,i = watActor->actor->objectController->object->mesh->numVertices;
		int x,y;

		if(waterVel == NULL)
			waterVel = (float *)JallocAlloc(waterX*waterY*sizeof(float),YES,"wvel");

		while(i--)
		{
			wv = &watActor->actor->objectController->object->mesh->vertices[i];

			for(j=0; j<2; j++)
			{
				AddVector2D(&tempVect,wv,&waterCentre[j]);
				dist[j] = Magnitude2D(&tempVect);
			}

			wv->v[Y] =	wv->v[Y] * (1 - waterF) + 
					(	SineWave2(waterFreq[0],frameCount + dist[0] * waterFactor[0] * waterFreq[0]) * waterWaveHeight[0] + 
						SineWave2(waterFreq[1],frameCount + dist[1] * waterFactor[1] * waterFreq[1]) * waterWaveHeight[1]) * waterF;
		}

		for(j = 0;j < 2;j++)
		{
			waterWaveHeight[j] = SineWave2(waterWaveHeightFreq[j],frameCount)*waterWaveHeightAmp[j] + waterWaveHeightBase[j];
			RotateVector2D(&waterCentre[j],&waterCentre[j],watRot[j]);
		}
	}
}

#endif
