#include "world_eff.h"
#include "temp_psx.h"
#include <islpad.h>
#include <isltex.h>
#include <islcard.h>
#include "main.h"
#include "layout.h"


void CreateLevelObjects(unsigned long worldID,unsigned long levelID)
{
	short i;
	char tmp[5];
	int flags;

	ACTOR2 *theActor;
	SCENIC *cur = firstScenic;


	char *compare;

	/*int actCount = 0;
	*/
	// go through and add items
	while (cur)
	{
		utilPrintf("%s, %d, %d, %d\n", cur->name, cur->pos.vx, cur->pos.vy, cur->pos.vz);
		if ( (gstrcmp ( cur->name,"world.psi") != 0) )
		{
			for( i=0; i<4; i++ )
				tmp[i] = cur->name[i];
			tmp[4] = '\0';

			if ( ( gstrcmp ( tmp,"cam_\0") != 0 ) )
			{
				if ( ( gstrcmp ( tmp,"wat_\0") ==0 ) )
				{
					flags = 0;

					//if ( cur->name[4] == 'f' )
						//flags = ACTOR_SLIDYTEX;
					// ENDIF

//					CreateAndAddScenicObject(cur);
					CreateAndAddWaterObject ( cur->name, cur->pos.vx, cur->pos.vy, cur->pos.vz, flags );

				}
				else if ( ( compare = strstr ( cur->name, "xx_" ) ) || ( compare = strstr ( cur->name, "lea_" ) ) ||
									( compare = strstr ( cur->name, "xxa_" ) ) || ( compare = strstr ( cur->name, "spl_" ) ) ||
									( compare = strstr ( cur->name, "wh" ) ) || ( compare = strstr ( cur->name, "wat_" ) )||
									( compare = strstr ( cur->name, "slu_" ) ) || ( compare = strstr ( cur->name, "spm_" ) )||
									( compare = strstr ( cur->name, "log" ) )|| ( compare = strstr ( cur->name, "cfence" ) )||
									( compare = strstr ( cur->name, "bar" ) )|| ( compare = strstr ( cur->name, "submush" ) )||
									( compare = strstr ( cur->name, "pylon" ) )|| ( compare = strstr ( cur->name, "spf_" ) )||
									( compare = strstr ( cur->name, "gunk" ) )|| ( compare = strstr ( cur->name, "mini" ) )||
									( compare = strstr ( cur->name, "plat" ) )|| ( compare = strstr ( cur->name, "poper" ) )||
									( compare = strstr ( cur->name, "powerl" ) )|| ( compare = strstr ( cur->name, "lamp" ) )||
									( compare = strstr ( cur->name, "tubtank" ) )|| ( compare = strstr ( cur->name, "cityrub" ) )||
									( compare = strstr ( cur->name, "candle" ) )|| ( compare = strstr ( cur->name, "cityrub" ) )||
									( compare = strstr ( cur->name,  "tcone" ))|| ( compare = strstr ( cur->name, "bin" ) )  ) 
				{
//					CreateAndAddScenicObject ( cur->name, cur->pos.vx, cur->pos.vy, cur->pos.vz, flags );
					CreateAndAddScenicObject(cur);
				}
				else
				{
					theActor = CreateAndAddActor ( cur->name, cur->pos.vx, cur->pos.vy, cur->pos.vz, INIT_ANIMATION, 0, 0 );

					if ( ( compare = strstr ( cur->name,"BACKDROP" ) ) )
					{
						theActor->flags |= ACTOR_DRAW_ALWAYS;
					}
					// ENDIF


					actorAnimate ( theActor->actor, 0, YES, NO, 150, NO );
				}
				// ENDELSEIF
			}
			// ENDIF

		}
		// ENDIF
		/*fixed tv;

		stringChange(ts->name);

		if(gstrcmp(ts->name,"backdrop.obe") == 0)
		{
			backGnd = CreateAndAddActor(ts->name, ToFixed(ts->pos.vx), ToFixed(ts->pos.vz), ToFixed(ts->pos.vy), INIT_ANIMATION, ToFixed(0), 0);
			backGnd->actor->scale.vx = ToFixed(5);
			backGnd->actor->scale.vy = ToFixed(5);
			backGnd->actor->scale.vz = ToFixed(5);
			actList = actList->next;
			if (actList)
				actList->prev = NULL;
		}
		else
		{*/



			/*if(gstrcmp(ts->name,"world.obe") == 0)
			{
				theActor->flags = ACTOR_DRAW_ALWAYS;
				globalLevelActor = theActor;
			}

			for( i=0; i<4; i++ )
				tmp[i] = ts->name[i];
			tmp[4] = '\0';
			// If a water object, draw always
			if( !gstrcmp( tmp, "wat_\0" ) )
			{
				theActor->flags = ACTOR_WATER | ACTOR_DRAW_ALWAYS;

				if (ts->name[4]=='f')
					theActor->flags |= ACTOR_SLIDYTEX;

#ifdef N64_VERSION
				AddN64WaterObjectResource(theActor->actor);
#endif
			}

			tv = ToFixed(ts->rot.y);
			ts->rot.y = ts->rot.z;
//bb
//			ts->rot.z = tv/4096;
			ts->rot.z = tv/4096.0F;

			GetQuaternionFromRotation (&theActor->actor->qRot,&ts->rot);

			actorAnimate(theActor->actor,0,YES,NO,1433/16,0);
			if(ts->name[0] == 'a')
			{
				fixed rMin,rMax,rNum;
				if(ts->name[2] == '_')
				{
					rMin = ToFixed(ts->name[1] - 30);
					if(rMin == ToFixed(0)) 
						rMin = ToFixed(10);
					rMin /= 10;
					actorAnimate(theActor->actor,0,YES,NO,rMin>>4, 0);
				}
				else if(ts->name[3] == '_')
				{
					rMin = ToFixed(ts->name[1] - 30);
					if(rMin == ToFixed(0)) 
						rMin = ToFixed(10);
					rMin /= 10;
				
					rMax = ToFixed(ts->name[1] - 30);
					if (rMax == ToFixed(0)) 
						rMax = ToFixed(10);
					rMax /= 10;

					rNum = ToFixed(Random(1000));
					rNum= rMin + FDiv( FMul(rNum,(rMax-rMin)), ToFixed(1000) );
										
					actorAnimate(theActor->actor,0,YES,NO,rNum>>4, 0);
				}
			}
			actCount++;
		}
*/
		cur = cur->next;

	}
	// ENDWHILE

//	dprintf"\n\n** ADDED %d ACTORS **\n\n",actCount));
}




extern unsigned short	currentPal16[16];
extern unsigned short	*VRAMpalBlock;
extern unsigned short	*VRAMpalCLUT;
extern unsigned short	currentPal256[256];
extern unsigned short	*VRAMpal256Block;
extern unsigned long	*VRAMpal256CLUT;
int VRAM_PALETTES = 500;
int VRAM_256PALETTES = 40;







void froggerShowVRAM(unsigned char palMode)
{
	DISPENV		dispenv;
	int			xOffs,yOffs, viewMode, currTex;

	currTex = 0;
	xOffs = 512;
	yOffs = 0;
	viewMode = 0;
	SetDefDispEnv(&dispenv, 0,0, 512,256);
	while ((padData.digital[0] & PAD_START)==0)
	{
		padHandler();
		switch(viewMode)
		{
		case 0:
			froggerVRAMviewNormal(&dispenv, &xOffs,&yOffs, palMode);
			break;
		case 1:
			froggerVRAMviewTextures(&currTex);
			break;
		}
		if (padData.debounce[0] & PAD_SELECT)
			viewMode = (viewMode+1) % 2;
	}
}


static void froggerVRAMviewNormal(DISPENV *dispenv, int *xOffs,int *yOffs, unsigned char palMode)
{
	if (padData.digital[0] & PAD_LEFT)
		*xOffs -= 6;
	if (padData.digital[0] & PAD_RIGHT)
		*xOffs += 6;
	if (*xOffs<0)
		*xOffs = 0;
	if (*xOffs>1024-512)
		*xOffs = 1024-512;
	if (padData.digital[0] & PAD_UP)
		*yOffs -= 4;
	if (padData.digital[0] & PAD_DOWN)
		*yOffs += 4;
	if (*yOffs<0)
		*yOffs = 0;
	if (*yOffs>512-((palMode)?(256):(240)))
		*yOffs = 512-((palMode)?(256):(240));
	dispenv->disp.x = *xOffs;
	dispenv->disp.y = *yOffs;
	if(palMode)
		dispenv->screen.y = 24;

	VSync(0);
	PutDispEnv(dispenv);
}

static void froggerVRAMviewTextures(int *currTex)
{
	POLY_FT4 	*ft4;
	TextureType	*tex;
	char		str[40];
	int			loop, yu = 0, yd = 0, f;

	static int	padDelay = 0;

	currentDisplayPage = (currentDisplayPage==displayPage)?(&displayPage[1]):(&displayPage[0]);
	ClearOTagR(currentDisplayPage->ot, 1024);
	currentDisplayPage->primPtr = currentDisplayPage->primBuffer;

	sprintf(str, "VRAM: TEXTURE VIEW");
	fontPrint(font, -230,-110, str, 128,128,128);

	tex = textureFindTextureN(*currTex);
	if (tex!=NULL)
	{
		BEGINPRIM(ft4, POLY_FT4);
		setPolyFT4(ft4);
		setXYWH(ft4, 150-tex->w/2,-tex->h/2, tex->w,tex->h-1);
		setRGB0(ft4, 128,128,128);
		setUVWH(ft4, tex->x,tex->y, tex->w-1, tex->h-1);
		ft4->tpage = tex->tpage;
		ft4->clut = tex->clut;
		ENDPRIM(ft4, 0, POLY_FT4);

		BEGINPRIM(ft4, POLY_FT4);
		setPolyFT4(ft4);
		setXYWH(ft4, -128-tex->w,-tex->h, tex->w*3,tex->h*2);
		setRGB0(ft4, 128,128,128);
		setUVWH(ft4, tex->x,tex->y, tex->w-1, tex->h-1);
		ft4->tpage = tex->tpage;
		ft4->clut = tex->clut;
		ENDPRIM(ft4, 1, POLY_FT4);

		if(tex->tpage & (1 << 7))
			froggerVRAMdrawPalette256(tex->clut, -90);
		else
			froggerVRAMdrawPalette(tex->clut, -90);

		sprintf(str, "Texture #%d (%dx%d) (used %dx)", *currTex, tex->w,tex->h, tex->refCount);
		fontPrint(font, -fontExtentW(font, str)/2,75, str, 128,128,128);
		yu = -tex->h/2-8;
		yd = tex->h/2+8;
	}

	for(loop=1; loop<5; loop++)
	{
		tex = textureFindTextureN(*currTex-loop);
		f = 100-loop*20;
		if (tex!=NULL)
		{
			BEGINPRIM(ft4, POLY_FT4);
			setPolyFT4(ft4);
			setXYWH(ft4, 150-tex->w/2,yu-tex->h, tex->w,tex->h-1);
			setRGB0(ft4, f,f,f);
			setUVWH(ft4, tex->x,tex->y, tex->w-1, tex->h-1);
			ft4->tpage = tex->tpage;
			ft4->clut = tex->clut;
			ENDPRIM(ft4, 2, POLY_FT4);
			yu -= tex->h+8;
		}
		tex = textureFindTextureN(*currTex+loop);
		if (tex!=NULL)
		{
			BEGINPRIM(ft4, POLY_FT4);
			setPolyFT4(ft4);
			setXYWH(ft4, 150-tex->w/2,yd, tex->w,tex->h-1);
			setRGB0(ft4, f,f,f);
			setUVWH(ft4, tex->x,tex->y, tex->w-1, tex->h-1);
			ft4->tpage = tex->tpage;
			ft4->clut = tex->clut;
			ENDPRIM(ft4, 2, POLY_FT4);
			yd += tex->h+8;
		}
	}

	if (padDelay==0)
	{
		if ((padData.digital[0] & (PAD_LEFT|PAD_UP)) && (*currTex>0))
		{
			*currTex = (*currTex)-1;
			padDelay = 3;
			if (padData.debounce[0] & (PAD_LEFT|PAD_UP))
				padDelay = 20;
		}
		if (padData.digital[0] & (PAD_RIGHT|PAD_DOWN))
		{
			if(textureFindTextureN(*currTex+1))
			{
				*currTex = (*currTex)+1;
				padDelay = 3;
				if (padData.debounce[0] & (PAD_RIGHT|PAD_DOWN))
					padDelay = 20;
			}
		}
	}
	else
	{
		if ((padData.digital[0] & (PAD_LEFT|PAD_RIGHT|PAD_UP|PAD_DOWN))==0)
			padDelay = 0;
		if (padDelay>0)
			padDelay--;
	}

	DrawSync(0);
	VSync(0);
	PutDispEnv(&currentDisplayPage->dispenv);
	PutDrawEnv(&currentDisplayPage->drawenv);
	DrawOTag(currentDisplayPage->ot+(1024-1));
}

static void froggerVRAMdrawPalette(unsigned long clut, int y)
{
	POLY_F4		*f4;
	char		str[40];
	int			pal, loop;
	RECT		rect;

	rect.x = (clut & 0x3f) << 4;
	rect.y = (clut >> 6);
	rect.w = 16;
	rect.h = 1;

	DrawSync(0);
	StoreImage(&rect, currentPal16);
   	
	BEGINPRIM(f4, POLY_F4);
	setPolyF4(f4);
	setXYWH(f4, -230-2,y-1, 16*20+3,10);
	setRGB0(f4, 128,128,128);
	ENDPRIM(f4, 1, POLY_F4);
	for(loop=0; loop<16; loop++)
	{
		BEGINPRIM(f4, POLY_F4);
		setPolyF4(f4);
		setXYWH(f4, -230+loop*20,y, 20,8);
		setRGB0(f4, 8*((currentPal16[loop]>>0) & 0x1f),
					8*((currentPal16[loop]>>5) & 0x1f),
					8*((currentPal16[loop]>>10) & 0x1f));
   		ENDPRIM(f4, 0, POLY_F4);
   	}

	for(pal=0; pal<VRAM_PALETTES; pal++)
   	{
   		if ((VRAMpalBlock[pal]) && (clut==VRAMpalCLUT[pal]))
   		{
			sprintf(str, "Palette #%d (used %dx)", pal, VRAMpalBlock[pal]);
   			fontPrint(font, -230,y+13, str, 128,128,128);
			break;
		}
	}

}

static void froggerVRAMdrawPalette256(unsigned long clut, int y)
{
	POLY_F4			*f4;
	char			str[40];
	int				pal, loop, loop2;
	RECT			rect;
	unsigned short	*palPtr;

	rect.x = (clut & 0x3f) << 4;
	rect.y = (clut >> 6);
	rect.w = 256;
	rect.h = 1;

	DrawSync(0);
	StoreImage(&rect, currentPal256);
   	
	BEGINPRIM(f4, POLY_F4);
	setPolyF4(f4);
	setXYWH(f4, -230-2,y-1, 16*20+3,10);
	setRGB0(f4, 128,128,128);
	ENDPRIM(f4, 1, POLY_F4);

	palPtr = &currentPal256[0];

	for(loop = 0; loop < 4; loop ++)
	{
		for(loop2 = 0; loop2 < 64; loop2 ++)
		{
			BEGINPRIM(f4, POLY_F4);
			setPolyF4(f4);
			setXYWH(f4, -230+loop2*5,y, 5, 2);
			setRGB0(f4, 8*(((*palPtr)>>0) & 0x1f),
						8*(((*palPtr)>>5) & 0x1f),
						8*(((*palPtr)>>10) & 0x1f));
   			ENDPRIM(f4, 0, POLY_F4);
			palPtr ++;
   		}
		y += 2;
	}

	for(pal=0; pal<VRAM_256PALETTES; pal++)
   	{
   		if ((VRAMpal256Block[pal]) && (clut==VRAMpal256CLUT[pal]))
   		{
			sprintf(str, "256Palette #%d (used %dx)", pal, VRAMpal256Block[pal]);
   			fontPrint(font, -230,y+5, str, 128,128,128);
			break;
		}
	}
}


static int cursPos = 0;

void PsxNameEntryInit(void)
{
	cursPos = 0;

	//create new character under cursor?
	if(textString[cursPos]==0)
	{
		textString[cursPos]='A';
	}
}

void PsxNameEntryFrame(void)
{
	padHandler();

	//move cursor
	if(padData.debounce[0] & PAD_LEFT)
	{
		if(cursPos>0)
		{
			cursPos--;
		}
	}

	if(padData.debounce[0] & PAD_RIGHT)
	{
		if(cursPos<8)
		{
			cursPos++;

			//create new character under cursor?
			if(textString[cursPos]==0)
			{
				textString[cursPos]='A';
			}
		}
	}


	//change char under cursor
	if(padData.debounce[0] & PAD_UP)
	{
		if(textString[cursPos]==' ')
			textString[cursPos]='A';

		else if(textString[cursPos]<'Z')
			textString[cursPos]++;
	}

	if(padData.debounce[0] & PAD_DOWN)
	{
		if(textString[cursPos]=='A')
			textString[cursPos]=' ';

		else if(textString[cursPos]>'A')
			textString[cursPos]--;
	}

	//done?
	if(padData.debounce[0]&PAD_START)
	{
		textEntry=0;
	}
}



//set to 0 on load failure
int tempUseCard = 1;

void SaveGame(void)
{
	//N.B this is the first version of this function.
	//It could do with a new format to save memory => save/load time.


	int totalLevels = 0;
	long saveSize;
	char *saveBuf = NULL;
	char *savePtr = NULL;
	int res;
	int w,l;


	if(!tempUseCard)
	{
		utilPrintf("Mem card not in use\n");
		return;
	}


	//loop through all worlds,
	//counting how many levels there are
	//(so we can work out the total save file size)
	for(w=0; w<MAX_WORLDS; w++)
	{
		totalLevels += worldVisualData[w].numLevels;
	}
	saveSize = (MAX_WORLDS*4) + (totalLevels*20);


	//make buffer
	saveBuf = MALLOC0(saveSize);
	if(!saveBuf)
	{
		utilPrintf("\n\nMALLOC ERROR DURING SAVE\n\n");
		return;
	}
	savePtr = saveBuf;


	for(w=0; w<MAX_WORLDS; w++)
	{
		*((short*)savePtr)++ = worldVisualData[w].worldOpen;
		*((short*)savePtr)++ = worldVisualData[w].numLevels;

		for(l=0; l<worldVisualData[w].numLevels; l++)
		{
			*((short*)savePtr)++ = worldVisualData[w].levelVisualData[l].levelOpen;
			*((short*)savePtr)++ = worldVisualData[w].levelVisualData[l].worldOpened;
			*((long*)savePtr)++ = worldVisualData[w].levelVisualData[l].parTime;
//			*((long*)savePtr)++ = worldVisualData[w].levelVisualData[l].parName;
			memcpy(savePtr, worldVisualData[w].levelVisualData[l].parName, 12);
			savePtr+=12;
		}
	}

	utilPrintf("Saving %d\n", saveSize);
	res = cardWrite("frogger2", saveBuf, saveSize);

	switch(res)
	{
		case CARDWRITE_OK:
		{
			utilPrintf("No errors\n");
			break;
		}
		case CARDWRITE_NOCARD:
		{
			utilPrintf("No card in slot\n");
			break;
		}
		case CARDWRITE_BADCARD:
		{
			utilPrintf("Bad card in slot\n");
			break;
		}
		case CARDWRITE_NOTFORMATTED:
		{
			utilPrintf("Card unformatted\n");
			break;
		}
		case CARDWRITE_FULL:
		{
			utilPrintf("Card is full\n");
			break;
		}
	} //end 


	FREE(saveBuf);
}


int tempNoLoad = 0;

void LoadGame(void)
{
	//N.B this is the first version of this function.
	//It could do with a new format to save memory => save/load time.


	int totalLevels = 0;
	long loadSize;
	char *loadBuf = NULL;
	char *loadPtr = NULL;
	int res;

	int w,l;


	//loop through all worlds,
	//counting how many levels there are
	//(so we can work out the total load file size)
	//N.B we will count the levels as we read them, to check for discrepency
	for(w=0; w<MAX_WORLDS; w++)
	{
		totalLevels += worldVisualData[w].numLevels;
	}
	loadSize = (MAX_WORLDS*4) + (totalLevels*20);


	
	//check for save game
	utilPrintf("Checking Save Game\n");
//	res = cardRead("frogger2", loadBuf, loadSize);
	res = cardRead("frogger2", 0, loadSize);
	switch(res)
	{
		case CARDREAD_OK:
		{
			utilPrintf("No errors\n");
			break;
		}
		case CARDREAD_NOCARD:
		{
			utilPrintf("No card in slot\n");
			tempUseCard = 0;
			break;
		}
		case CARDREAD_BADCARD:
		{
			utilPrintf("Bad card in slot\n");
			tempUseCard = 0;
			break;
		}
		case CARDREAD_NOTFORMATTED:
		{
			utilPrintf("Card unformatted\n");
			tempUseCard = 0;
			break;
		}
		case CARDREAD_NOTFOUND:
		{
			utilPrintf("No game save data found\n");
//			tempUseCard = 0;
			tempNoLoad = 1;
			break;
		}
		case CARDREAD_CORRUPT:
		{
			utilPrintf("Game save data corrupted\n");
//			tempUseCard = 0;
			tempNoLoad = 1;
			break;
		}
		case CARDREAD_NOTFOUNDANDFULL:
		{
			utilPrintf("No game save data found and card is full\n");
			tempUseCard = 0;
			break;
		}
	} //end 



	//load data?
	if(tempUseCard && !tempNoLoad)
	{
		//make buffer
		loadBuf = MALLOC0(loadSize);
		if(!loadBuf)
		{
			utilPrintf("\n\nMALLOC ERROR DURING LOAD\n\n");
			tempUseCard = 0;
			return;
		}



		utilPrintf("Loading Save Game %d\n", loadSize);
		res = cardRead("frogger2", loadBuf, loadSize);
		switch(res)
		{
			case CARDREAD_OK:
			{
				utilPrintf("No errors\n");
				break;
			}
			case CARDREAD_NOCARD:
			{
				utilPrintf("No card in slot\n");
				tempUseCard = 0;
				break;
			}
			case CARDREAD_BADCARD:
			{
				utilPrintf("Bad card in slot\n");
				tempUseCard = 0;
				break;
			}
			case CARDREAD_NOTFORMATTED:
			{
				utilPrintf("Card unformatted\n");
				tempUseCard = 0;
				break;
			}
			case CARDREAD_NOTFOUND:
			{
				utilPrintf("No game save data found\n");
//				tempUseCard = 0;
				tempNoLoad = 1;
				break;
			}
			case CARDREAD_CORRUPT:
			{
				utilPrintf("Game save data corrupted\n");
//				tempUseCard = 0;
				tempNoLoad = 1;
				break;
			}
			case CARDREAD_NOTFOUNDANDFULL:
			{
				utilPrintf("No game save data found and card is full\n");
				tempUseCard = 0;
				break;
			}
		} //end 



		//extract data
		loadPtr = loadBuf;

		for(w=0; w<MAX_WORLDS; w++)
		{
			worldVisualData[w].worldOpen = *((short*)loadPtr)++;
			worldVisualData[w].numLevels = *((short*)loadPtr)++;

			//check num levels here

			for(l=0; l<worldVisualData[w].numLevels; l++)
			{
				worldVisualData[w].levelVisualData[l].levelOpen   = *((short*)loadPtr)++;
				worldVisualData[w].levelVisualData[l].worldOpened = *((short*)loadPtr)++;
				worldVisualData[w].levelVisualData[l].parTime     = *((long*)loadPtr)++;
				memcpy(worldVisualData[w].levelVisualData[l].parName, loadPtr, 12);
				loadPtr+=12;
			}
		}
	}

	FREE(loadBuf);
}

void Actor2ClipCheck(ACTOR2* act)
{
//bbxx - PAL/NTSC specifics needed here
 	#define CLIP_TOP	-120
 	#define CLIP_RIGHT	256
 	#define CLIP_BOTT	120
 	#define CLIP_LEFT	-256

	#define CLIP_FAR	3000

//test
// 	#define CLIP_TOP	-50
// 	#define CLIP_RIGHT	100
// 	#define CLIP_BOTT	50
// 	#define CLIP_LEFT	-100

	
	long sxy,sz;
	long sx, sy;//extracted from sxy
	int distTop, distRight, distBott, distLeft;

	SVECTOR pos = act->actor->position;
	pos.vx = -pos.vx;
	pos.vy = -pos.vy;


	//calc screen coords of actor
	gte_SetTransMatrix(&GsWSMATRIX);
	gte_SetRotMatrix(&GsWSMATRIX);

	gte_ldv0(&pos);
	gte_rtps();
	gte_stsxy(&sxy);
	gte_stszotz(&sz);


	sx = (short)(sxy&0xffff);
	sy = (short)(sxy>>16);


// 	if(act->actor->psiData.flags & ACTOR_DYNAMICSORT)
// 	{
// 		char tempText[128];
// 		sprintf(tempText, "%s : %d, %d, %d", act->actor->psiData.modelName, sx, sy, sz);
// 		fontPrint(fontSmall, -100,-90, tempText, 255,255,255);
// 
// 		sprintf(tempText, "X");
// 		fontPrint(font, sx,sy, tempText, 255,255,255);
// 	}



	//calc dists from edges
	distTop		= sy - CLIP_TOP;
	distRight	= sx - CLIP_RIGHT;
	distBott	= sy - CLIP_BOTT;
	distLeft	= sx - CLIP_LEFT;


	//clip?
	if( (distTop<0) || (distRight>0) || (distBott>0) || (distLeft<0) || (sz>CLIP_FAR) )
	{
		act->clipped = 1;
	}
	else
	{
		act->clipped = 0;
	}


	//miles away?
// 	if(act->clipped)
// 	{
// 		int debugTemp = 0;
// 		int milesDist = 100;
// 
// 		if( (distTop<-milesDist) || (distRight>milesDist)
// 		 || (distBott>milesDist) || (distLeft<-milesDist) )
// 		{
// 			debugTemp++;
// 		}
// 	}

}
