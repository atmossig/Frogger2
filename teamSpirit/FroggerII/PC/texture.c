/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: texture.c
	Programmer	: Andy Eder
	Date		: 19/04/99 12:39:48

----------------------------------------------------------------------------------------------- */

#include <ultra64.h>

#include "incs.h"


//#define PRINT_TEXTURE_DEBUG		// uncomment to print tons of debug messages


TEXTURE_BANK	textureBanks[MAX_TEXTURE_BANKS];		//final texture bank is for font
TEXTURE *tempTexture;

unsigned long numTextureBanks = 0;

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
	TEXENTRY *cur, *next;

	cur		= texList;

	numTextures = 0;
	while ( cur )
	{
		next = cur->next;

		ReleaseSurface(cur->surf); cur->surf = NULL; // aaaghhh
		JallocFree((BYTE**)&cur->data);
//		JallocFree((BYTE**)&tme->hdl);
		JallocFree((BYTE**)&cur);

		cur = next;


		numTextures++;
	}
	// ENDWHILE

/*	TEXENTRY *me = texList;
	TEXENTRY *tme = texList;
	int numTextures = 0;

	while (me)
	{
		tme=me;
		me=me->next;

		if (tme->surf)
			ReleaseSurface(tme->surf);
		JallocFree((BYTE**)&tme->data);
//		JallocFree((BYTE**)&tme->hdl);
		JallocFree((BYTE**)&tme);
		numTextures++;
	}
*/
	dprintf"Freed %d Textures\n",numTextures ));


	texList = NULL;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitTextureBanks()
{
	int x;

	for(x = 0; x < MAX_TEXTURE_BANKS; x++)
	{
		textureBanks[x].freePtr = NULL;
		textureBanks[x].data = NULL;
		textureBanks[x].numTextures = 0;
	}
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

#ifdef PRINT_TEXTURE_DEBUG
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

#ifdef PRINT_TEXTURE_DEBUG
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

#ifdef PRINT_TEXTURE_DEBUG
	dprintf"TEXTURE NOT FOUND %xl\n",CRC));
#endif
	return NULL;
}

void AddTextureToTexList(char *file, char *shortn, long finalTex)
{
	char mys[255];
	short *dat;

	TEXENTRY *newE = (TEXENTRY*)JallocAlloc (sizeof(TEXENTRY),NO,"txtur");

	newE->next = texList;
	
	texList = newE;
	
	strcpy (mys,shortn);
	strlwr (mys);
	
	newE->CRC  = UpdateCRC (mys);
	
	newE->data = GetGelfBmpDataAsShortPtr(file);
	
	if (newE->data)
	{
		if (((shortn[0]=='a') & (shortn[1]=='i')) & (shortn[2]=='_'))
		{
			newE->surf = CreateTextureSurface(32,32, newE->data, 1,0xf81f,1);
			if ( newE->surf )
				newE->hdl = ConvertSurfaceToTexture(newE->surf);
			
		}
		else
		{
			newE->surf = CreateTextureSurface(32,32, newE->data, 1,0xf81f,0);
			if ( newE->surf )
				newE->hdl = ConvertSurfaceToTexture(newE->surf);
		}
	}
	else
		dprintf"Cannot find texture %s\n",shortn));
}

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

	switch(num)
	{
		case SYSTEM_TEX_BANK:
			sprintf(message,"SYS_TB");
			strcat (filename,"system\\");
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
#ifdef PRINT_TEXTURE_DEBUG
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
#ifdef PRINT_TEXTURE_DEBUG
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
			
	*texPtr = GetTexEntryFromCRC(texID);

	return;
}
