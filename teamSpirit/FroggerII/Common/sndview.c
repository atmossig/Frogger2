/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: sndview.c
	Programmer	: James Healey
	Date		: 21/04/98

----------------------------------------------------------------------------------------------- */

//----------------------------------------------------------------------------------------------//
//----- SYSTEM INCLUDES ------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------//
#include <ultra64.h>

//----------------------------------------------------------------------------------------------//
//----- USER INCLUDES --------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------//

#include "incs.h"


unsigned long sfxNum = 0;
unsigned long musNum = 0;

TEXTOVERLAY *title[11];
unsigned long dir[11];

unsigned long sfx = 1;
unsigned long mus = 0;

char *sfxNames[] =
{	"FX NULL",
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

TEXTOVERLAY *sfxName;

TEXTOVERLAY *musName;

char *musNames[] = 
{	"NO TRACK",
/*	"ATLANTIS TRACK",
	"ATLANTIS 2 TRACK",
	
	"ATLANTIS 3 TRACK",
	"ATLANTIS BOSS TRACK",*/
	"CARNIVAL TRACK",
/*	"CARNIVAL 2 TRACK",
	"CARNIVAL 3 TRACK",
	"CARNIVAL BOSS TRACK",
	"PIRATES TRACK",
	"PIRATES 2 TRACK",
	"PIRATES 3 TRACK",	
	"PIRATESBOSS_TRACK",
	"PREHISTORIC_TRACK",
	"PREHISTORIC2_TRACK",
	"PREHISTORIC3_TRACK",
	"PREHISTORICBOSS_TRACK",
	"FORTRESS_TRACK",
	"FORTRESS2_TRACK",
	"FORTRESS3_TRACK",
	"FORTRESSBOSS_TRACK",
	"OOTW_TRACK",
	"OOTW2_TRACK",
	"OOTW3_TRACK",
	"OOTWBOSS_TRACK",
	"TITLE_TRACK",
	"INTRO_TRACK",
	"OUTRO_TRACK",
	"HUB1_TRACK",
	"HUB2_TRACK",
	"HUB3_TRACK",
	"HUB4_TRACK",
	"HUB5_TRACK",
	"HUB6_TRACK",
	"ATLANTIS_KILL1_TRACK",
	"ATLANTIS_KILL2_TRACK",
	"ATLANTIS_WIN1_TRACK",
	"ATLANTIS_WIN2_TRACK",
	"CARNIVAL_KILL1_TRACK",
	"CARNIVAL_KILL2_TRACK",
	"CARNIVAL_WIN1_TRACK",
	"CARNIVAL_WIN2_TRACK",
	"PIRATES_KILL1_TRACK",
	"PIRATES_KILL2_TRACK",
	"PIRATES_WIN1_TRACK",
	"PIRATES_WIN2_TRACK",
	"PREHISTORIC_KILL1_TRACK",
	"PREHISTORIC_KILL2_TRACK",
	"PREHISTORIC_WIN1_TRACK",
	"PREHISTORIC_WIN2_TRACK",
	"FORTRESS_KILL1_TRACK",
	"FORTRESS_KILL2_TRACK",
	"FORTRESS_WIN1_TRACK",
	"FORTRESS_WIN2_TRACK",
	"OOTW_KILL1_TRACK",
	"OOTW_KILL2_TRACK",
	"OOTW_WIN1_TRACK",
	"OOTW_WIN2_TRACK",
	"BONUS_TRACK",
	"HASBRO_TRACK",
	//TITLE2_TRACK,	*/
};



void RunSndView ( void )
{
	unsigned long counter;

	static unsigned long turn;

	static u16 button,lastbutton;
	static s16 stickX,stickY;

	
	if ( frameCount == 1 )
	{
		FreeAllLists();
		
		LoadTextureBank(SYSTEM_TEX_BANK);
//		LoadTextureBank(TITLES_TEX_BANK);
//		LoadTextureBank ( SNDVIEW_TEX_BANK );

		currFont = smallFont;

		title[0]  = CreateAndAddTextOverlay ( 30, 15, "S", NO,NO, 255, 255, 255, 250, currFont,0,2,0);
		title[1]  = CreateAndAddTextOverlay ( 30, 50, "O", NO,NO, 255, 255, 255, 200, currFont,0,2,0);
		title[2]  = CreateAndAddTextOverlay ( 30, 85, "U", NO,NO, 255, 255, 255, 150, currFont,0,2,0);
		title[3]  = CreateAndAddTextOverlay ( 30, 120, "N", NO,NO, 255, 255, 255, 100,  currFont,0,2,0);
		title[4]  = CreateAndAddTextOverlay ( 30, 155, "D", NO,NO, 255, 255, 255, 50,  currFont,0,2,0);

		title[5]  = CreateAndAddTextOverlay ( 30, 190, "P", NO,NO, 255, 255, 255, 0,  currFont,0,2,0);
		title[6]  = CreateAndAddTextOverlay ( 50, 190, "L", NO,NO, 255, 255, 255, 0,  currFont,0,2,0);
		title[7]  = CreateAndAddTextOverlay ( 85, 190, "A", NO,NO, 255, 255, 255, 0,  currFont,0,2,0);
		title[8]  = CreateAndAddTextOverlay ( 120, 190, "Y", NO,NO, 255, 255, 255, 0,  currFont,0,2,0);
		title[9]  = CreateAndAddTextOverlay ( 155, 190, "E", NO,NO, 255, 255, 255, 0,  currFont,0,2,0);
		title[10] = CreateAndAddTextOverlay ( 190, 190, "R", NO,NO, 255, 255, 255, 0,  currFont,0,2,0);

		dir[0]	= 1; dir[1]	 = 1;
		dir[2]	= 1; dir[3]	 = 1;
		dir[4]	= 1; dir[5]	 = 0;
		dir[6]	= 0; dir[7]	 = 0;
		dir[8]	= 0; dir[9]	 = 0;
		dir[10] = 0;

		currFont = oldeFont;

		CreateAndAddTextOverlay ( 30, 90, "SFX NAMES", YES,NO, 255, 0, 0, 250, currFont,0,2,0);

		sfxName = CreateAndAddTextOverlay ( 30, 105, "", YES,NO, 255, 255, 255, 250, currFont,0,2,0);
		sfxName->text = sfxNames [ sfxNum ];

		CreateAndAddTextOverlay ( 30, 130, "MUSIC NAMES", YES,NO, 255, 0, 0, 250, currFont,0,2,0);

		musName = CreateAndAddTextOverlay ( 30, 145, "", YES,NO, 255, 255, 255, 250, currFont,0,2,0);
		musName->text = musNames [ musNum ];

//		CreateAndAddTextOverlay ( 250, 20, "CONTROLS", NO,NO, 255, 255, 255, 255,  currFont, 0, 2, 0 );

//		CreateAndAddSpriteOverlay ( 250, 40,"n64g.bmp",32,32,255,255,255,255,0);
//		CreateAndAddTextOverlay   ( 280, 40, "EXIT", NO,NO, 255, 255, 255, 255,  currFont, 0, 2, 0 );

//		CreateAndAddSpriteOverlay ( 250, 60,"n64a.bmp",32,32,255,255,255,255,0);
//		CreateAndAddTextOverlay   ( 270, 60, "MUSIC", NO,NO, 255, 255, 255, 255,  currFont, 0, 2, 0 );

		sfxNum	= 0;
		musNum	= 0;

		turn	= 10;
	}
	// ENDIF - frameCount == 1

	button = controllerdata[ActiveController].button;
	stickX = controllerdata[ActiveController].stick_x;
	stickY = controllerdata[ActiveController].stick_y;

	if((button & CONT_D) && !(lastbutton & CONT_D))
	{
		sfx = 0;
		mus = 1;
	}
	if((button & CONT_E) && !(lastbutton & CONT_E))
	{
		sfx = 1;
		mus = 0;
	}

	if((button & CONT_L) && !(lastbutton & CONT_L))
	{
		if ( sfx )
		{
			if ( sfxNum != 0 )
			{
				sfxNum--;
			}
			// endif
			sfxName->text = sfxNames [ sfxNum ];
		}
		else
		{
			if ( musNum != 0 )
			{
				musNum--;
			}
			// endif
			musName->text = musNames [ musNum ];
		}
		// ENDIF
	}

	if((button & CONT_R) && !(lastbutton & CONT_R))
	{
		if ( sfx )
		{
			sfxNum++;
			sfxName->text = sfxNames [ sfxNum ];
		}
		else
		{
			musNum++;
			musName->text = musNames [ musNum ];
		}
		// ENDIF
	}

	if((button & CONT_A) && !(lastbutton & CONT_A))
	{
		if ( sfx )
		{
			PlaySample ( sfxNum, NULL, 255, 128 );
		}
		else
		{
#ifdef PC_VERSION
			PrepareSong ( musNum );
#else
			PrepareSong( musNum, 0 );
#endif
		}
		// ENDIF
	}

	if((button & CONT_B) && !(lastbutton & CONT_B))
	{
		if ( sfx )
		{
//			MusHandleStop ( musresult, 0 );
		}
		else
		{
//			MusHandleStop ( audioCtrl.musicHandle, 0 );
//			audioCtrl.currentTrack = 0;
		}
		// ENDIF
	}

	if((button & CONT_START) && !(lastbutton & CONT_START))
	{
		FreeAllLists();
		frontEndState.mode = TITLE_MODE;
	}

	if ( !turn )
	{
		for ( counter = 0; counter < 11; counter ++ )
		{

			if ( dir [ counter ] )
			{
				title [ counter ]->a -= 10;
				if ( title [ counter ]->a <= 0 )
				{
					dir [ counter ] = 0;
				}
				// ENDIF
			}
			// ENDIF
			if ( !dir [ counter ] )
			{
				title [ counter ]->a += 10;
				if ( title [ counter ]->a >= 250 )
				{
					dir [ counter ] = 1;
				}
				// ENDIF
			}
			// ENDIF

		}
		// ENDFOR
		turn = 3;
	}
	else
	{
		turn--;
	}
	// ENDIF

	lastbutton = button;
}
