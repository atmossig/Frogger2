/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: texture.c
	Programmer	: Andy Eder
	Date		: 19/04/99 12:39:48

	Define TEXTURE_DEBUG to print lots of debug stuff!

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>
#include "gelf.h"
#include "incs.h"

#ifdef TEXTURE_DEBUG
unsigned char *hdlCheck = NULL;
#endif

char message[32];

TEXENTRY *texList = NULL;

/*	--------------------------------------------------------------------------------
	Function		: FreeAllTextureBanks
	Purpose			: frees up ALL texture banks
	Parameters		: 
	Returns			: void
	Info			: NOTE: leaves system texture bank alone !
*/

void FreeAllTextureBanks()
{
	TEXENTRY *cur, *next, *cur2;
	int numTextures;

	for (cur = texList, numTextures = 0; cur; cur = next, numTextures++)
	{
		next = cur->next;
		
		if (cur->surf)
		{
			IDirectDraw_Release(cur->surf);
			cur->surf = NULL;
		}

		cur2 = cur->nextFrame;
		while (cur2)
		{
			IDirectDraw_Release(cur2->surf);
			cur2->surf = NULL;
			cur2 = cur2->nextFrame;
		}

		JallocFree((BYTE**)&cur->data);
		JallocFree((BYTE**)&cur);
	}

	dprintf"Freed %d Textures\n",numTextures ));

#ifdef TEXTURE_DEBUG
	if( hdlCheck ) JallocFree( (UBYTE **)&hdlCheck );

	hdlCheck = (unsigned char *)JallocAlloc( MAX_HDLCHECKS, YES, "hdlcheck" );
#endif

	texList = NULL;
}


/*	--------------------------------------------------------------------------------
	Function 	: LoadTexureBank
	Purpose 	: dma's a texture bank from rom into ram.
	Parameters 	: char *texturebank
	Returns 	: none
	Info 		:
*/


short *GetTexDataFromCRC (long CRC)
{
	TEXENTRY *me = texList;
	
	while (me)
	{
		if (me->CRC==CRC)
			return me->data;
		
		me=me->next;
	}

#ifdef TEXTURE_DEBUG
	dprintf"TEXTURE NOT FOUND %xl\n",CRC));
#endif
	return NULL;
}

D3DTEXTUREHANDLE GetTexHandleFromCRC (long CRC)
{
	TEXENTRY *me = texList;
	
	while (me)
	{
		if (me->CRC==CRC)
			return me->hdl;
		
		me=me->next;
	}

#ifdef TEXTURE_DEBUG
	dprintf"TEXTURE NOT FOUND %xl\n",CRC));
#endif
	return NULL;
}

TEXENTRY *GetTexEntryFromCRC (long CRC)
{
	TEXENTRY *me = texList;
	
	while (me)
	{
		if (me->CRC==CRC)
			return me;

		me=me->next;
	}

#ifdef TEXTURE_DEBUG
	dprintf"TEXTURE NOT FOUND %xl\n",CRC));
#endif
	return NULL;
}

void AnimateTexturePointers(void)
{
	TEXENTRY *cEntry = texList,*tE;
	while (cEntry)
	{
		if (cEntry->nextFrame)
		{
			if (cEntry->nextFrameAt<=0)
			{
				tE = cEntry->cFrame->nextFrame; 
				if (tE)
					cEntry->cFrame = tE;
				else
					cEntry->cFrame = cEntry;
				cEntry->nextFrameAt = 2;//cEntry->cFrame->frameTime; 
			}
			else
				cEntry->nextFrameAt-=gameSpeed;

		}

		cEntry = cEntry->next;
	}
}

void AddTextureToTexList(char *file, char *shortn, long finalTex)
{
	char mys[255];
	short *dat;
	TEXENTRY *newE;
	long isAnim = 0, i;
	strcpy (mys,shortn);
	strlwr (mys);

#ifdef TEXTURE_DEBUG
	dprintf"%s\n",mys));
#endif

	// Animated or procedural texture
	if( ((mys[0]>='0') && (mys[0]<='9')) && ((mys[1]>='0') && (mys[1]<='9')) ) isAnim = 1;
	else if( mys[0]=='p' && mys[1]=='r' && mys[2]=='c' )
		isAnim = 2;
		
	
	if( (isAnim == 1) && (!((shortn[0]=='0') && (shortn[1]=='0'))))
	{
		TEXENTRY *cEntry = texList;
		
		newE = (TEXENTRY*)JallocAlloc (sizeof(TEXENTRY),NO,"txtur");
		newE->next = NULL;
		newE->nextFrame = NULL;

		while (cEntry)
		{
			if (strcmp(&mys[3],&cEntry->name[3])==0)
			{
				if (cEntry->nextFrame)
				{
					TEXENTRY *tEn = cEntry->nextFrame;
					while (tEn->nextFrame) tEn = tEn->nextFrame;
					tEn->nextFrame = newE;
				}
				else
					cEntry->nextFrame = newE;

				cEntry = 0;
			}
			else
				cEntry = cEntry->next;
		}
	}
	else
	{
		newE = (TEXENTRY*)JallocAlloc (sizeof(TEXENTRY),NO,"txtur");
	
		newE->next = texList;
		newE->nextFrame = NULL;

		texList = newE;
	}

	if( isAnim == 1 )
	{
		newE->frameTime = (((shortn[7]-'0')+1) * ((shortn[7]-'0')+1));
	}
	newE->nextFrameAt=0;

	strcpy (newE->name,mys);
	newE->CRC  = UpdateCRC (mys);

	if (isAnim != 2)
		newE->data = GetGelfBmpDataAsShortPtr(file,GELF_IFORMAT_16BPP555);
	else
		newE->data = GetGelfBmpDataAsShortPtr(file,0);

	newE->cFrame = newE;

	if (newE->data)
	{
		if( isAnim == 2 )
		{
			CreateAndAddProceduralTexture( (TEXTURE *)newE, mys );
			newE->surf = CreateTextureSurface(32,32, newE->data, 1,0xf81f,1);
			newE->type = TEXTURE_AI;
		}
		else if(((shortn[0]=='a') & (shortn[1]=='i')) & (shortn[2]=='_'))
		{
			newE->surf = CreateTextureSurface(32,32, newE->data, 1,0xf81f,1);
			newE->type = TEXTURE_AI;
		}
		else
		{
			newE->surf = CreateTextureSurface(32,32, newE->data, 1,0xf81f,0);
			newE->type = TEXTURE_NORMAL;
		}

		if ( newE->surf )
		{
			newE->hdl = ConvertSurfaceToTexture(newE->surf);

			// If texture debugging then flag if a surface has been successfully created
#ifdef TEXTURE_DEBUG
			if( newE->hdl && newE->hdl < MAX_HDLCHECKS )
				hdlCheck[newE->hdl] = 1;
			else
				dprintf"Invalid texture handle %d\n", newE->hdl));
#endif
		}
		else
		{
			newE->hdl = 0;
			dprintf"Error creating texture surface\n"));
		}
	}
	else
		dprintf"Cannot find texture %s\n",shortn));
}


void PrintTextureInfo(void);

void LoadTextureBank(int num)
{
	char			message[255];
	char			filename[MAX_PATH];
	char			Sfilename[MAX_PATH];
	WIN32_FIND_DATA fData;
	HANDLE			fHandle;
	int numTextures;

    strcpy (filename,baseDirectory);	
	strcat (filename,TEXTURE_BASE);
	
	PrintTextureInfo();
	
	switch(num)
	{
		case SYSTEM_TEX_BANK:
			sprintf(message,"SYS_TB");
			strcat (filename,"system\\");
			break;

		case INGAMEGENERIC_TEX_BANK:
			break;

		case GARDEN_TEX_BANK:
			sprintf(message,"GARD_TB");
			strcat (filename,"garden\\");
			break;

		case SUPERRETRO_TEX_BANK:
			sprintf(message,"SUPER_TB");
			strcat (filename,"super\\");
			break;

		case HALLOWEEN_TEX_BANK:
			sprintf(message,"HOLLW_TB");
			strcat (filename,"halloween\\");
			break;

		case SPACE_TEX_BANK:
			sprintf(message,"SPACE_TB");
			strcat (filename,"space\\");
			break;

		case CITY_TEX_BANK:
			sprintf(message,"CITY_TB");
			strcat (filename,"city\\");
			break;

		case ANCIENT_TEX_BANK:
			sprintf(message,"ANCIENT_TB");
			strcat (filename,"ancient\\");
			break;

		case SUBTERRANEAN_TEX_BANK:
			sprintf(message,"ANCIENT_TB");
			strcat (filename,"subter\\");
			break;

		case LABORATORY_TEX_BANK:
			sprintf(message,"ANCIENT_TB");
			strcat (filename,"lab\\");
			break;

		case TOYSHOP_TEX_BANK:
			sprintf(message,"ANCIENT_TB");
			strcat (filename,"toy\\");
			break;

		case TITLES_TEX_BANK:
		case TITLESGENERIC_TEX_BANK:
			sprintf(message,"TITLE_TB");
			strcat (filename,"titles\\");
			break;

		case OLDEFROG_TEX_BANK:
			sprintf(message,"OLDE_TB");
			strcat (filename,"olde\\");
			break;

		case FRONT_TEX_BANK:
			sprintf(message,"FRONT_TB");
			strcat (filename,"front\\");
			break;
		
		case SNDVIEW_TEX_BANK:
			sprintf(message,"SNDVIEW_TB");
			strcat (filename,"sndview\\");
			break;
		
		case FRONTEND_TEX_BANK:
			sprintf(message,"TITLES_TB");
			strcat (filename,"titles\\");
			break;
		
		default:
			dprintf"Invalid texture bank!\n"));
			for(;;);
			break;
	}


	strcpy (Sfilename,filename);
	strcat (Sfilename,"*.bmp");
#ifdef TEXTURE_DEBUG
	dprintf"Loading %s from %s\n",message,filename));
#endif
	fHandle = FindFirstFile (Sfilename,&fData);

	if (fHandle != INVALID_HANDLE_VALUE)
	{
		long ret;
		char finalFile[MAX_PATH];
		char finalShort[MAX_PATH];

		numTextures = 0;
		do
		{
			strcpy (finalFile,filename);
			strcat (finalFile,fData.cFileName);
			strcpy (finalShort,fData.cFileName);
#ifdef TEXTURE_DEBUG
			dprintf"Loading %s %s\n",finalFile,fData.cFileName));
#endif
			ret = FindNextFile (fHandle,&fData);

			AddTextureToTexList (finalFile,finalShort,!ret);			
			numTextures++;
		}
		while (ret);
	
		dprintf"Loaded %d Textures\n",numTextures ));

		FindClose (fHandle);
	}

	PrintTextureInfo();

}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FindTexture(TEXTURE **texPtr, int texID, BOOL report)
{
	int x, y;
	TEXTURE *tex;
	char *temp;
			
	*texPtr = (TEXTURE *)GetTexEntryFromCRC(texID);

	return;
}
