#include "world_eff.h"
#include "temp_psx.h"
#include <islpad.h>
#include <isltex.h>
#include "main.h"


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
		if ( (gstrcmp ( cur->name,"world.psi") != 0) && (gstrcmp ( cur->name,"backdrop.psi") != 0) )
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

					CreateAndAddWaterObject ( cur->name, cur->pos.vx, cur->pos.vy, cur->pos.vz, flags );

				}
				else if ( ( compare = strstr ( cur->name, "xx_" ) ) || ( compare = strstr ( cur->name, "lea_" ) ) ||
									( compare = strstr ( cur->name, "xxa_" ) ) || ( compare = strstr ( cur->name, "spl_" ) ) ||
									( compare = strstr ( cur->name, "wh" ) ) || ( compare = strstr ( cur->name, "wat_" ) )||
									( compare = strstr ( cur->name, "slu_" ) ) || ( compare = strstr ( cur->name, "spm_" ) )||
									( compare = strstr ( cur->name, "log" ) )|| ( compare = strstr ( cur->name, "cfence" ) )||
									( compare = strstr ( cur->name, "bar" ) )|| ( compare = strstr ( cur->name, "submush" ) )||
									( compare = strstr ( cur->name, "ttop" ) )|| ( compare = strstr ( cur->name, "spf_" ) )||
									( compare = strstr ( cur->name, "gunk" ) )|| ( compare = strstr ( cur->name, "mini" ) )||
									( compare = strstr ( cur->name, "plat" ) )|| ( compare = strstr ( cur->name, "poper" ) )||
									( compare = strstr ( cur->name, "tubtank" ) )|| ( compare = strstr ( cur->name, "poper" ) )||
									( compare = strstr ( cur->name,  "tcone" ))|| ( compare = strstr ( cur->name, "bin" ) )  ) 
				{
//					CreateAndAddScenicObject ( cur->name, cur->pos.vx, cur->pos.vy, cur->pos.vz, flags );
					CreateAndAddScenicObject(cur);
				}
				else
				{
					theActor = CreateAndAddActor ( cur->name, cur->pos.vx, cur->pos.vy, cur->pos.vz, INIT_ANIMATION, 0, 0 );

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
