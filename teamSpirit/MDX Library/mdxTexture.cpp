/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: mdxReport.cpp
	Programmer	: Matthew Cloy
	Date		: 

----------------------------------------------------------------------------------------------- */


#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <math.h>
#include "mgeReport.h"
#include "mdxDDraw.h"
#include "mdxD3D.h"
#include "mdxInfo.h"
#include "mdxTiming.h"
#include "mdxCRC.h"
#include "mdxTexture.h"
#include "mdxMath.h"
#include "mdxObject.h"
#include "mdxActor.h"
#include "mdxLandscape.h"
#include "mdxRender.h"
#include "mdxPoly.h"
#include "mdxDText.h"
#include "mdxProfile.h"
#include "mdxWindows.h"
#include "gelf.h"
#include "mdxException.h"
#include <stdio.h>


#ifdef __cplusplus

extern "C"
{
#endif

MDX_TEXENTRY *cDispTexture = NULL;
MDX_TEXENTRY *texList = NULL;
MDX_TEXPAGE	*texPages = NULL;
char showMemDebug=0;
long surfacesMade = 0;

#define TEX_PAGE_SIZE 64

unsigned long maxTexturesInPage = ((TEX_PAGE_SIZE/32) * (TEX_PAGE_SIZE/32));

float textureAdjustDivider = (TEX_PAGE_SIZE/32);

unsigned long texXCoords[((TEX_PAGE_SIZE/32) * (TEX_PAGE_SIZE/32))];
unsigned long texYCoords[((TEX_PAGE_SIZE/32) * (TEX_PAGE_SIZE/32))];

MDX_TEXENTRY *GetTexEntryFromCRC (long CRC)
{
	MDX_TEXENTRY *me = texList;
	MDX_TEXENTRY *carp;

	while (me)
	{
		if (me->CRC==CRC)
			return me;
		carp = me;
		me=me->next;
	}
	return NULL;
}

MDX_TEXPAGE *GetFreeTexturePage(void)
{
	MDX_TEXPAGE *ret;
	unsigned int i,xCrd,yCrd;

	// If we have some texture pases already.
	if (texPages)
	{
		ret = texPages;
		
		// The texture page is full, start another
		if (ret->numTex >= maxTexturesInPage)
		{
			ret = (MDX_TEXPAGE *) AllocMem(sizeof(MDX_TEXPAGE));
			if ((ret->surf = D3DCreateTexSurface(TEX_PAGE_SIZE,TEX_PAGE_SIZE, 0xf81f, 0, 1)) == NULL)
			{
				dp("Could not allocate a texture page!\n");
				FreeMem (ret);
				return NULL;
			}

			ret->next = texPages;
			ret->numTex = 0;			
		}
		texPages = ret;
		return ret;
	}
	
	// We need to make our very first texture page (ahhhhhh)
	ret = (MDX_TEXPAGE *) AllocMem(sizeof(MDX_TEXPAGE));
	
	if ((ret->surf = D3DCreateTexSurface(TEX_PAGE_SIZE,TEX_PAGE_SIZE, 0xf81f, 0, 1)) == NULL)
	{
		dp("Could not allocate a texture page!\n");
		FreeMem (ret);
		return NULL;
	}
	ret->next = NULL;
	ret->numTex = 0;

	texPages = ret;

	//May as well calculate tex coords here!

	for (i=0,xCrd = 0,yCrd = 0; i<maxTexturesInPage; i++)
	{
		texXCoords[i] = xCrd;
		texYCoords[i] = yCrd;

		xCrd+=32;
		
		if (xCrd>=TEX_PAGE_SIZE)
		{
			xCrd = 0;
			yCrd+=32;
		}
	}
	return ret;				
}

long wacky = 0;


MDX_TEXENTRY *AddTextureToTexList(char *file, char *shortn, long finalTex)
{
	char mys[255],tBnk[255];
	MDX_TEXENTRY *newE;
	MDX_TEXENTRY *cEntry = texList;
	long isAnim = 0,texType = 0;
	int pptr = -1;
	int xDim,yDim,i,j,bksCount;

	strcpy (mys,shortn);
	strlwr (mys);
	
	texType = TEXTURE_NORMAL;
	if (strncmp(mys,"ai_",3)==0)
		texType = TEXTURE_AI;

	if (strncmp(mys,"ac_",3)==0)
		texType = TEXTURE_AC;

	if (strncmp(mys,"prc",3)==0)
		texType = TEXTURE_PROCEDURAL;

	newE = (MDX_TEXENTRY *) AllocMem(sizeof(MDX_TEXENTRY));
	
	newE->next = texList;
	if (texList)
		texList->prev = newE;
	newE->prev = NULL;
	texList = newE;

	strcpy (newE->name,mys);
	newE->CRC  = UpdateCRC (mys);
	newE->refCount = 0;
	newE->data = (short *)gelfLoad_BMP(file,NULL,(void**)&pptr,&xDim,&yDim,NULL,GELF_IFORMAT_16BPP555,GELF_IMAGEDATA_TOPDOWN);

	if (wacky)
	{
		for (i=0; i<xDim*yDim; i++)
		{
			long r,g,b;
			r = (newE->data[i] >> 10) & 0x1f;
			g = (newE->data[i] >> 5) & 0x1f;
			b = (newE->data[i]) & 0x1f;

	/*		if (!(r==0x1f && b==0x1f && g==0))
			{
				
				g*=0.8;
				b*=0.6;
				
				newE->data[i] = (r<<10) | (g<<5) | b;
			}*/
			//	newE->data[i] = (g<<10) | (b<<5) | r;
			//  newE->data[i] = (r<<10) | (r<<5) | r;
			newE->data[i] = newE->data[0];
		}
	}

	i = xDim;
	bksCount = 0;
	for (j=0; j<32; j++)
	{
		if (i&1)
			bksCount++;
		i>>=1;
	}

	if (bksCount>1)
	{
		texType = TEXTURE_NOTEXTURE;
		newE->type = TEXTURE_NOTEXTURE;
	}

	bksCount = 0;
	for (i=strlen(file); i>0; i--)
	{
		if (file[i] == '\\')
			bksCount++;
		if (bksCount==2)
			break;
	}

	strcpy(tBnk,&file[i+1]);
	
	for (i=0; tBnk[i]; i++)
		if (tBnk[i] == '\\')
			tBnk[i] = 0;
			
	strncpy(newE->bank,tBnk,60);

	newE->keyed = 0;
	newE->numFrames = 1;
	if (newE->data)
	{
		LPDIRECTDRAWSURFACE7 temp;

		switch (texType)
		{
			case TEXTURE_NORMAL:
			{
				newE->surf = D3DCreateTexSurface(xDim,yDim, 0xffff, 0, 1);
					
				// Create a temporary surface to hold the texture.
				if ((temp = D3DCreateTexSurface(xDim,yDim, 0xffff, 0, 1)) == NULL)
					return NULL;

				newE->keyed = DDrawCopyToSurface(temp,(unsigned short *)newE->data,0,xDim,yDim,0);
				newE->surf->BltFast(0,0,temp,NULL,DDBLTFAST_WAIT);
				newE->xPos = 0;
				newE->yPos = 0;
				
				temp->Release();
				surfacesMade--;
				//page->numTex++;

				newE->type = TEXTURE_NORMAL;
				break;
			}

			case TEXTURE_AI:
			{
				// Create a temporary surface to hold the texture.
				if ((temp = D3DCreateTexSurface(xDim,yDim, 0xf81f, 1, 1)) == NULL)
					return NULL;

				newE->keyed = DDrawCopyToSurface(temp,(unsigned short *)newE->data,1,xDim,yDim,0);			
				newE->surf = temp;
				newE->xPos = 0;
				newE->yPos = 0;
				
				newE->type = TEXTURE_AI;
				break;
			}			

			case TEXTURE_PROCEDURAL:
			{
				// Create a temporary surface to hold the texture.
				if ((temp = D3DCreateTexSurface(xDim,yDim, 0xf81f, 1, 1)) == NULL)
					return NULL;

				newE->keyed = DDrawCopyToSurface(temp,(unsigned short *)newE->data,1,xDim,yDim,0);			
				newE->surf = temp;
				newE->xPos = 0;
				newE->yPos = 0;
				
				newE->type = TEXTURE_AI;
				break;
			}								

			case TEXTURE_NOTEXTURE:
			{
				// Create a temporary surface to hold the texture.

				if ((temp = D3DCreateSurface(xDim,yDim, 0xf81f, 0)) == NULL)
					return NULL;

				newE->keyed = DDrawCopyToSurface(temp,(unsigned short *)newE->data,0,xDim,yDim,1);
				newE->surf = temp;
				newE->xPos = 0;
				newE->yPos = 0;
				
				break;
			}								
	
			case TEXTURE_AC:
			{
				// Create a temporary surface to hold the texture.
				if ((temp = D3DCreateTexSurface(xDim,yDim, 0xf81f, 1, 1)) == NULL)
					return NULL;

				newE->keyed = DDrawCopyToSurface(temp,(unsigned short *)newE->data,2,xDim,yDim,0);			
				newE->surf = temp;
				newE->xPos = 0;
				newE->yPos = 0;
				
				newE->type = TEXTURE_AI;
				break;
			}			

		}

		if (newE->surf)
			pDirect3DDevice->PreLoad(newE->surf);
			
		newE->xSize = xDim;
		newE->ySize = yDim;

			if( rHardware )
				newE->softData = NULL;
			else
			{
				newE->softData = (long *) AllocMem(sizeof(long)*xDim*yDim);

				for (int i=0; i<xDim; i++)
					for (int j=0; j<yDim; j++)
					{
						short dt;
						unsigned long d,r,g,b;
						dt = newE->data[i+j*xDim];
						r = (dt>>10) & 0x1f;
						g = (dt>>5) & 0x1f;
						b = (dt) & 0x1f;

						if ((r==0x1f) && (b==0x1f) && (g==0))
						{
							newE->keyed = 1;
							newE->softData[i+j*xDim] = 0x00ff00ff;
						}
						else
						{
							r<<=3;
							g<<=3;
							b<<=3;
							//if (r565)
							newE->softData[i+j*xDim] = (r<<16 | g<<8 | b) & 0x00ffffff;
						}
		
					}
			}

		return newE;
	}
	else
		dp("Cannot load texture %s\n",shortn);

	return NULL;
}

short tempdata[256*256];
void GrabSurfaceToTexture(long x, long y, MDX_TEXENTRY *texture, LPDIRECTDRAWSURFACE7 srf)
{
	long xS,yS,mPitch,j;
	HRESULT res;
	
	if (texture)
	{
		xS = texture->xSize;
		yS = texture->ySize;

		DDSURFACEDESC2		ddsd;
		DDINIT(ddsd);

		
		while (surface[RENDER_SRF]->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR,0)!=DD_OK);
		
		mPitch = ddsd.lPitch/2;

		for (j = 0; j<yS; j++)
			memcpy(&tempdata[j*xS],&(((short *)ddsd.lpSurface)[x+(j+y)*mPitch]),xS*2);
		
		surface[RENDER_SRF]->Unlock(NULL);
		

		DDrawCopyToSurface2(texture->surf,(unsigned short *)tempdata,xS,yS);
	}

}

unsigned long LoadTexBank(char *bank, char *baseDir)
{
	char	fName[MAX_PATH];
	char	fPath[MAX_PATH];
	char	fAnim[MAX_PATH];
	MDX_TEXENTRY *me;
	MDX_TEXENTRY *tex;
		
	WIN32_FIND_DATA fData;
	HANDLE			fHandle;

	unsigned long numTextures;

	strcpy (fPath,baseDir);
	strcat (fPath,TEXTURE_BASE);
	strcat (fPath,bank);
	strcat (fPath,"\\");

	strcpy (fName,fPath);
	strcat (fName,"*.bmp");

	fHandle = FindFirstFile (fName,&fData);


	if (fHandle != INVALID_HANDLE_VALUE)
	{
		long ret;
		char finalFile[MAX_PATH];
		char finalShort[MAX_PATH];
		FILE *animFp;

		numTextures = 0;
		do
		{
			strcpy (finalFile,fPath);
			strcat (finalFile,fData.cFileName);
			strcpy (finalShort,fData.cFileName);
			ret = FindNextFile (fHandle,&fData);					
			
			me = AddTextureToTexList (finalFile,finalShort,!ret);			

			if (me)
			{
				strcpy (fAnim,fPath);
				strcat (fAnim,finalShort);
				
				for (int i=0; i<strlen(fAnim); i++)
					if (fAnim[i]=='.')
						fAnim[i] = 0;
				
				strcat(fAnim,".txt");
				
				if (animFp = fopen(fAnim,"rt"))
				{
					char line[255];
					char tempName[255];

					fgets(line,255,animFp);
					sscanf(line,"%lu",&me->numFrames);					
					dp("texFrames: %s %lu",me->name,me->numFrames);
					
					me->frameTimes = (float *) AllocMem(sizeof(float)*me->numFrames);
					me->frames = (LPDIRECTDRAWSURFACE7 *) AllocMem(sizeof(LPDIRECTDRAWSURFACE7)*me->numFrames);
					me->lastGameFrame = me->lastFrame = 0;
					me->updated = 0;
					for (int i=0; i<me->numFrames; i++)
					{
						fgets(line,255,animFp);
						sscanf(line,"%s",tempName);
						fgets(line,255,animFp);
						if (line[0]=='"')
							me->frameTimes[i] = me->frameTimes[i-1];
						else
							sscanf(line,"%f",&(me->frameTimes[i]));										
						me->frames[i] = (LPDIRECTDRAWSURFACE7)UpdateCRC(tempName);	
					}
					fclose(animFp);
				}

			}
			numTextures++;
		}
		while (ret);
	
		FindClose (fHandle);
	}
	else
		dp("Could not load bank %s\n",fName);
	
	// Update CRC's in framelists with relevant surfaces (Will need to add the softdata, when software is implemented!)
	
	for( me = texList; me; me = me->next)
		if ((me->numFrames>1) && (!me->updated))
		{
			for (int i=0; i<me->numFrames; i++)
				if (tex = GetTexEntryFromCRC((long)me->frames[i]))
					me->frames[i] = tex->surf;
				else
					me->frames[i] = NULL;
				me->updated++;
		}
	return 1;
}

void UpdateAnimatingTextures(void)
{
	MDX_TEXENTRY *me;
	for( me = texList; me; me = me->next)
	{
		if (me->numFrames>1)
		{
			while ((me->lastGameFrame+me->frameTimes[me->lastFrame])<timeInfo.frameCount)
			{
				me->lastGameFrame+=me->frameTimes[me->lastFrame];
				
				me->lastFrame++;
				if (me->lastFrame>=me->numFrames)
					me->lastFrame = 0;
			}
			
			me->surf = me->frames[me->lastFrame];
		}
	}
	
}

void PrintTextureInfo(void)
{
	DDSCAPS2 ddCaps;
	unsigned long dwVidMemTotal, dwVidMemFree;

	ddCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE;
	pDirectDraw7->GetAvailableVidMem(&ddCaps, &dwVidMemTotal, &dwVidMemFree);					// Get the caps for the device
	//pDirectDraw->GetCaps(&ddCaps, NULL);														// Get the caps for the device (Old method...)
	dp ( "Total Mem : %lu : - Total Free : %lu :\n",dwVidMemTotal, dwVidMemFree );
}

extern MDX_TEXENTRY *haloHandle;


//_CrtMemState state1,state2,state3;

void FreeAllTextureBanks()
{
	MDX_TEXENTRY *cur, *next/*, *cur2*/;
	int numTextures;
	haloHandle = 0;
	for( cur = texList, numTextures = 0; cur; cur = next, numTextures++ )
	{
		next = cur->next;

/* Pending animated textures

		cur2 = cur->nextFrame;
		while (cur2)
		{
			cur2->surf->Release();
			cur2 = cur2->nextFrame;
		}
*/
		if( cur->data ) 
			gelfDefaultFree(cur->data);
		if( cur->softData ) 
			FreeMem (cur->softData);

		if (cur->numFrames > 1)
		{
			// Reset the frame to the origional.
			cur->surf = cur->frames[0];
			FreeMem (cur->frameTimes);
			FreeMem (cur->frames);
		}

		if( cur->surf )	
		{
			cur->surf->Release();
			surfacesMade--;
		}

		FreeMem (cur);
	}

	dp("Freed %d Textures\n",numTextures);
	dp("%lu Surfaces Made=========================================================================\n",surfacesMade);

	if( showMemDebug )
		Show_Mem();
//CrtMemCheckpoint(&state2);
//CrtMemDifference(&state3,&state2,&state1);
	
//	_CrtMemDumpStatistics(&state3);

//	_CrtMemCheckpoint(&state1);

	dp("%lu Surfaces Made=========================================================================\n",surfacesMade);

	
	texList = NULL;
	testS = NULL;
}

void WriteTexturesToFile(void)
{
	MDX_TEXENTRY *me = texList;
	char tText[255];
	FILE *fp = fopen("c:\\textures.txt","wt");
						
	while (me)
	{
		sprintf(tText,"%s			%lu references",me->name,me->refCount);
		fputs(tText,fp);
		fputs("\n",fp);

		me = me->next;
	}

	fclose (fp);
}

void ShowTextures(void)
{
	RECT r;
	MDX_TEXENTRY *me;
	HDC hdc;
	

	if (!cDispTexture)
		cDispTexture = texList;

	r.left = 0;
	r.top = 0;
	r.right = rXRes - 1;
	r.bottom = rYRes - 1;

	BeginDraw();
	DrawTexturedRect(r,D3DRGBA(0,0,0,0.8),NULL,0,0,1,1);
	EndDraw();

	// Draw the textures
	r.left = 32;
	r.top = 32;
	r.right = r.left + 64;
	r.bottom = r.top + 64;
	me = cDispTexture;
		
	while (me && (r.bottom < rYRes - 32))	
	{
		r.left = 32;
		r.right = r.left + 64;
		
		while (me && (r.right < rXRes - 32))
		{
			if (me->type != TEXTURE_NOTEXTURE)	
			{
				BeginDraw();
				DrawTexturedRect(r,D3DRGB(1,1,1),me->surf,0,0,1,1);
				EndDraw();
			}
			else
				surface[RENDER_SRF]->Blt(&r, me->surf, NULL, DDBLT_WAIT, NULL);
				//DrawTexturedRect(r,D3DRGB(0,1,0),NULL,0,0,1,1);
			r.left += 150;
			r.right += 150;
			me = me->next;
		}
		r.top += 85;
		r.bottom += 85;
	}

			
	HRESULT res = IDirectDrawSurface4_GetDC(surface[RENDER_SRF], &hdc);
	if ((res == DD_OK))
	{
			char tText[256];
			long tPC = 0,cPC;
			HFONT hfnt, hOldFont;      
			hfnt = (HFONT)GetStockObject(ANSI_VAR_FONT); 
			if (hOldFont = (HFONT)SelectObject(hdc, hfnt)) 
			{
				SetBkMode(hdc, TRANSPARENT);
		
				// Draw the info
				r.left = 32;
				r.top = 32;
				r.right = r.left + 64;
				r.bottom = r.top + 64;
				me = cDispTexture;
					
				while (me && (r.bottom < rYRes - 32))	
				{
					r.left = 32;
					r.right = r.left + 64;
					
					while (me && (r.right < rXRes - 32))
					{
						SetTextColor(hdc, RGB(255,255,255));
						
						sprintf(tText,"%s",me->name);
						TextOut(hdc, r.left+70, r.top, tText, strlen(tText));
						
						sprintf(tText,"%s",me->keyed?"Colorkey":"Opaque");
						TextOut(hdc, r.left+70, r.top+15, tText, strlen(tText));
						
						sprintf(tText,"%lux%lu",me->xSize,me->ySize);
						TextOut(hdc, r.left+70, r.top+30, tText, strlen(tText));
						
						if (me->refCount)
						{
							sprintf(tText,"%lu refernces",me->refCount);

							TextOut(hdc, r.left+70, r.top+45, tText, strlen(tText));
						}
						else
						{
							SetTextColor(hdc, RGB(255,0,0));

							sprintf(tText,"Unreferenced",me->refCount);
							TextOut(hdc, r.left+70, r.top+45, tText, strlen(tText));
						}

						SetTextColor(hdc, RGB(0,255,255));
						
						sprintf(tText,"%s",me->bank);
						TextOut(hdc, r.left+70, r.top+60, tText, strlen(tText));
						
						r.left += 150;
						r.right += 150;
						me = me->next;
					}
					r.top += 85;
					r.bottom += 85;
				}
						
				SelectObject(hdc, hOldFont); 
			}

		
		IDirectDrawSurface4_ReleaseDC(surface[RENDER_SRF], hdc);	
	}
	
}

#ifdef __cplusplus
}
#endif
