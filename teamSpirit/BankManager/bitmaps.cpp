#include <windows.h>
#include <commctrl.h>
#include <wingdi.h>
#include <string.h>
#include <stdio.h>

#include "types.h"
#include <gelf.h>
#include "bitmaps.h"
#include "main.h"
#include "files.h"
#include "ndos.h"
#include "crc32.h"
#include "resource.h"
#include "debug.h"
#include "views.h"
#include "gif.h"

#include "c:\n64\include\pr\gbi.h"

LPBITMAPINFO		bmi;
HPALETTE			syspal;
LOGPALETTE			*logpal;

BITMAP_TYPE			myBmp;

BITMAP_TYPE	*objectBitmaps[MAX_OBJECT_BITMAPS];

int					numObjectBitmaps = 0;
int					currentObjectBitmap = -1;

int					highColTextures = 0;
int					adjustColours = 1;


BITMAP_TYPE missingBmp;




/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
BOOL FAR PASCAL TexBankProperties(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam)
{
	char buf[MAX_PATH];
	static char origName[MAX_PATH];
	int saveObject = FALSE;
	static int	newBank = FALSE;

	switch(msg)
	{
		case	WM_COMMAND:
			if(LOWORD(wParam) == IDOK)
			{
				GetDlgItemText(hwnd, IDC_TEXBANKPATH, buf, MAX_PATH);
				if(strlen(buf) == 0)
					break;
				else
					strcpy(textureBanks[selectedTextureBank].destination, buf);
	
				GetDlgItemText(hwnd, IDC_EXPBANKPATH, textureBanks[selectedTextureBank].exportPath, MAX_PATH);

				GetDlgItemText(hwnd, IDC_TEXBANKNAME, buf, MAX_PATH);
				if(strlen(buf) == 0)
					break;

				if(strstr(buf, ".bmt") == NULL)
					strcat(buf, ".bmt");
	
				if(newBank)
				{
					ChangeTextureBankInfo(buf, buf);
					SendDlgItemMessage(dlgWnd,IDC_LIST2, LB_INSERTSTRING, selectedTextureBank, (long)buf);
					numTextureBanks++;
				}
				else
				{
					ChangeTextureBankInfo(origName, buf);
					SendDlgItemMessage(dlgWnd,IDC_LIST2, LB_DELETESTRING, selectedTextureBank, 0);
					SendDlgItemMessage(dlgWnd,IDC_LIST2, LB_INSERTSTRING, selectedTextureBank, (long)buf);

				}

				DialogBox(hAppInst, "FLYNN", hwnd, (DLGPROC)WaitForFlynn);
//				WaitForCommand();
				strcpy(textureBanks[selectedTextureBank].filename, buf);
				
				RefreshTextureBank(FALSE);
				EndDialog(hwnd, FALSE);
			}
			if(LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hwnd, FALSE);
			}
			if(LOWORD(wParam) == IDC_TEXPATHBROWSE)
			{
				if(BrowseForDirectory(hwnd, "Select target texture directory", buf))
				{
					SendDlgItemMessage(hwnd,IDC_TEXBANKPATH, WM_SETTEXT, 0, (long)buf);
				}
			}
			if(LOWORD(wParam) == IDC_EXPPATHBROWSE)
			{
				if(BrowseForFile("Select texture bank to export to", "NTB files (*.ntb)\0*.ntb\0", buf))
				{
					SendDlgItemMessage(hwnd,IDC_EXPBANKPATH, WM_SETTEXT, 0, (long)buf);
				}
			}



			break;
		case	WM_INITDIALOG:
					SetDlgItemText(hwnd,IDC_TEXBANKPATH, textureBanks[selectedTextureBank].destination);
					SetDlgItemText(hwnd,IDC_EXPBANKPATH, textureBanks[selectedTextureBank].exportPath);
					SetDlgItemText(hwnd, IDC_TEXBANKNAME, textureBanks[selectedTextureBank].filename);
					if(selectedTextureBank == numTextureBanks)
					{
						//must be a new bank, so open up name field
						newBank = TRUE;
						HWND tmp = GetDlgItem(hwnd, IDC_TEXBANKNAME);
						EnableWindow(tmp, 1);
						origName[0] = 0;
					}					
					else
					{
						strcpy(origName, textureBanks[selectedTextureBank].filename);
						newBank = FALSE;
					}

			return TRUE;
				
	}
	return FALSE;

}








/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int LoadPicy(BITMAP_TYPE * picy)
{
	unsigned char	*pp;
	int res, l;

	picy->bmpObj = (BITMAPOBJ *)malloc(sizeof(BITMAPOBJ));
	wsprintf(picy->bmpObj->filename,"%s",picy->fullFilename);

	if(strstr(picy->name, "ai_"))
		picy->flags |= GIF_FORMAT;

	if(strstr(picy->name, "ph_"))
		picy->flags |= GIF_FORMATPH;


	return setupPic(picy->bmpObj);
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
// Create a bitmap in mem and load BMP

int setupPic(BITMAPOBJ * ptr)
{
	short	l;
	unsigned char	*pp;
	int		rBpp;
	int		bitmapBpp, storeFormat;
	unsigned char *tPtr = NULL;
	


//	dbprintf("Loading bitmap %s\n", ptr->filename);

/*
		if (!load_bmp(ptr->filename, &(ptr->data), ptr->palette, &ptr->width, &ptr->height))
		{
			dbprintf("Cant load bitmap!\n");
			return 0;
		}
*/

		bitmapBpp = GetBMPInfo(ptr->filename);
		if(bitmapBpp == 0)
			return 0;

		ptr->bpp = bitmapBpp;

		ptr->data = NULL;
		switch(bitmapBpp)
		{
			case 4:	storeFormat = GELF_IFORMAT_4BPP;
					break;
			case 8:	storeFormat = GELF_IFORMAT_8BPP;
					break;
			case 24:storeFormat = GELF_IFORMAT_24BPPBGR;
					break;
		}


		ptr->palette = NULL;
		ptr->data = (unsigned char *)gelfLoad_BMP(ptr->filename, NULL, (void **)&ptr->palette, &ptr->width, &ptr->height, &rBpp, storeFormat, 1);

		rBpp = 24;

		ptr->bitInf.bi.biSize = sizeof(BITMAPINFOHEADER);
		ptr->bitInf.bi.biWidth = ptr->width;
		ptr->bitInf.bi.biHeight = ptr->height;
		ptr->bitInf.bi.biPlanes = 1;
		ptr->bitInf.bi.biBitCount = rBpp;
		ptr->bitInf.bi.biCompression = 0;
		ptr->bitInf.bi.biSizeImage = (ptr->width * ptr->height * rBpp) /8;
		ptr->bitInf.bi.biXPelsPerMeter = 0;
		ptr->bitInf.bi.biYPelsPerMeter = 0;
		ptr->bitInf.bi.biClrUsed = (rBpp == 8) ? 256 : 16;
		ptr->bitInf.bi.biClrImportant = 0;
		ptr->bmi = (LPBITMAPINFO)&ptr->bitInf.bi;

/*
		if(bitmapBpp == 8)
		{

			ptr->logpal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) + 256 * sizeof(PALETTEENTRY));

			ptr->logpal->palVersion = 0x300;
			ptr->logpal->palNumEntries = 256;

			pp = ptr->palette;
			for(l = 0; l < 256; l++)
			{
				ptr->bitInf.palette[l].rgbRed = *pp;
				ptr->bitInf.palette[l].rgbGreen = *(pp+1);
				ptr->bitInf.palette[l].rgbBlue = *(pp+2);

				ptr->logpal->palPalEntry[l].peRed = *pp;
				ptr->logpal->palPalEntry[l].peGreen = *(pp+1);
				ptr->logpal->palPalEntry[l].peBlue = *(pp+2);

				pp += 3;
			}
			ptr->syspal = CreatePalette(ptr->logpal);
			free(ptr->logpal);

		}
		if(bitmapBpp == 4)
		{

			ptr->logpal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) + 16 * sizeof(PALETTEENTRY));

			ptr->logpal->palVersion = 0x300;
			ptr->logpal->palNumEntries = 16;

			pp = ptr->palette;
			for(l = 0; l < 16; l++)
				{
				ptr->bitInf.palette[l].rgbRed = *pp;
				ptr->bitInf.palette[l].rgbGreen = *(pp+1);
				ptr->bitInf.palette[l].rgbBlue = *(pp+2);

				ptr->logpal->palPalEntry[l].peRed = *pp;
				ptr->logpal->palPalEntry[l].peGreen = *(pp+1);
				ptr->logpal->palPalEntry[l].peBlue = *(pp+2);

				pp += 3;
				}
			ptr->syspal = CreatePalette(ptr->logpal);
			free(ptr->logpal);
		}

*/
		return 1;
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int GetBMPInfo(char *filename)
{
	int sx, sy, bpp;	
	unsigned char *tempData = NULL;
	unsigned char *tempPalette = NULL;


	if(gelfInfo_BMP(filename, &sx, &sy, &bpp))
		return bpp;
	else
		return 0;


/*
	tempData = (unsigned char *)gelfLoad_BMP(filename, NULL, (void **)&tempPalette, &sx, &sy, &bpp, GELF_IFORMAT_24BPPRGB, 1);
	if(tempData)
	{
		if(tempData)
			gelfDefaultFree(tempData);
		if(tempPalette)
			gelfDefaultFree(tempPalette);
		return bpp;
	}
	else
	{
		return 0;
	}
*/}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
BITMAP_TYPE *FindTexture(unsigned long crc)
{
	int i, j, len;

	for(i = 0; i < numTextureBanks; i++)
	{
		for(j = 0; j < textureBanks[i].numTextures; j++)
		{
			len = strlen(textureBanks[i].textures[j].name);			
			if(UpdateCRC(0, textureBanks[i].textures[j].name, len) == crc)
				return &textureBanks[i].textures[j];
		}
	}
	return NULL;
}
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
BOOL IsObjectTextureUnique(BITMAP_TYPE *bmp)
{
	int i = 0;

	for(i = 0; i < numObjectBitmaps; i++)
	{
		if(bmp == objectBitmaps[i])
			return FALSE;
	}
	return TRUE;
}
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void AddObjectTextures(INPUT_OBJECT *obj, BOOL doChildren)
{
	int x;
	BITMAP_TYPE *bmp;

	if(obj->mesh == NULL)
		return;
	for(x = 0; x < obj->mesh->numFaces; x++)
	{
		bmp = FindTexture(obj->mesh->faceInfo[x].textureID.crc);
		if(bmp)
		{
			if(IsObjectTextureUnique(bmp) == TRUE)
			{
				objectBitmaps[numObjectBitmaps++] = bmp;
			}
		}
		else
		{
			if(IsObjectTextureUnique(&missingBmp) == TRUE)
				objectBitmaps[numObjectBitmaps++] = &missingBmp;
		}
	}

	for(x = 0; x < obj->numSprites; x++)
	{
		bmp = FindTexture(obj->sprites[x].textureID);
		if(bmp)
		{
			if(IsObjectTextureUnique(bmp) == TRUE)
			{
				objectBitmaps[numObjectBitmaps++] = bmp;
			}
		}
		else
		{
			objectBitmaps[numObjectBitmaps++] = &missingBmp;
		}

	}


//	if(obj->duelTexID)
	{
		for(x = 0; x < obj->mesh->numFaces; x++)
		{
			bmp = FindTexture(obj->mesh->faceInfo[x].duelTexID);
			if(bmp)
			{
				if(IsObjectTextureUnique(bmp) == TRUE)
				{
					objectBitmaps[numObjectBitmaps++] = bmp;
				}
			}
		}
	}

	if(obj->phongTexture)
	{
		bmp = FindTexture(obj->phongTexture);
		if((bmp) && (IsObjectTextureUnique(bmp) == TRUE))
			objectBitmaps[numObjectBitmaps++] = bmp;
	}

	if(doChildren)
	{
		if(obj->children)
			AddObjectTextures(obj->children, doChildren);

		if(obj->next)
			AddObjectTextures(obj->next, doChildren);
	}

}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void UpdateObjectBitmapWindow(INPUT_OBJECT *obj)
{
	RECT iRect;
	char buf[256];

	numObjectBitmaps = 0;
	currentObjectBitmap = -1;
	objectTexturesOffset = 0;

	GetObjectBitmapsFromNDO(obj);

	InvalidateRect(dlgWnd,&objectTextureRect,FALSE);

}
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void GetObjectBitmapsFromNDO(INPUT_OBJECT *obj)
{

//		AddObjectTextures(obj, 1);
//		return;
	//object is a sub-object
	if(obj->mesh)
	{
		AddObjectTextures(obj, 1);
	}
	else //this must be a parent, so look at all of its children
	{
		if(obj->children)
			AddObjectTextures(obj->children, 1);
	}

}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int bitmapInObject = FALSE;

BOOL IsBitmapInObject(BITMAP_TYPE *tex, INPUT_OBJECT *obj)
{
	int x;
	int len = strlen(tex->name);
	int texCRC = UpdateCRC(0, tex->name, len);

	if(obj->mesh == NULL)
		return FALSE;

	for(x = 0; x < obj->mesh->numFaces; x++)
	{
		if(obj->mesh->faceInfo[x].textureID.crc == texCRC)
		{
			bitmapInObject = TRUE;
			goto out;
		}
	}

	for(x = 0; x < obj->numSprites; x++)
	{
		if(obj->sprites[x].textureID == texCRC)
		{
			bitmapInObject = TRUE;
			goto out;
		}
	}

//	if(obj->duelTexID)
	{
		for(x = 0; x < obj->mesh->numFaces; x++)
		{
			if(obj->mesh->faceInfo[x].duelTexID == texCRC)
			{
				bitmapInObject = TRUE;
				goto out;
			}
		}
	}


	if(obj->children)
		if(IsBitmapInObject(tex, obj->children))
			bitmapInObject = TRUE;

	if(obj->next)
		if(IsBitmapInObject(tex, obj->next))
			bitmapInObject = TRUE;

out:
	return bitmapInObject;

}
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void ShowUnusedTextures(int bankNum)
{
	BITMAP_TYPE *tempBitmaps;
	RECT iRect;
	int i, j, k, ref;
	OBJECTINFO *ptr;

	numObjectBitmaps = 0;
	currentObjectBitmap = -1;
	objectTexturesOffset = 0;

	for(i = 0; i < textureBanks[bankNum].numTextures; i++)
	{
		ref = 0;
		for(j = 0; j < numObjectBanks; j++)
		{
			for(ptr = objectBanks[j].objectList.head.next;ptr != &objectBanks[j].objectList.head;ptr = ptr->next)
			{
				bitmapInObject = FALSE;
				if(IsBitmapInObject(&textureBanks[bankNum].textures[i], ptr->object) == TRUE)
				{
					ref++;
				}
			}
		}
		if(ref == 0)
			objectBitmaps[numObjectBitmaps++] = &textureBanks[bankNum].textures[i];
	}

	UpdateObjectStatusBar();
	InvalidateRect(dlgWnd,&objectTextureRect,FALSE);

}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int CountTextureReference(BITMAP_TYPE *bitmap)
{
	int ref = 0, j, k;
	OBJECTINFO *ptr;
	
	for(j = 0; j < numObjectBanks; j++)
	{
		if(objectBanks[j].loaded)
		{
			for(ptr = objectBanks[j].objectList.head.next;ptr != &objectBanks[j].objectList.head;ptr = ptr->next)
			{
				bitmapInObject = FALSE;
				if(ptr->object)
				{
					if(IsBitmapInObject(bitmap, ptr->object) == TRUE)
					{
						ref++;
					}
				}
			}
		}
	}

	return ref;
}
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void ListTextureReference(BITMAP_TYPE *bitmap)
{
	int ref = 0, j, k;
	char buf[256];
	OBJECTINFO *ptr;
		
	for(j = 0; j < numObjectBanks; j++)
	{
		if(objectBanks[j].loaded)
		{
			for(ptr = objectBanks[j].objectList.head.next;ptr != &objectBanks[j].objectList.head;ptr = ptr->next)
			{
				
				if(ptr->object)
				{
					bitmapInObject = FALSE;
					if(IsBitmapInObject(bitmap, ptr->object) == TRUE)
					{
						ref++;
						sprintf(buf, "%s    <%s>", ptr->object->name, objectBanks[j].filename);
						SendDlgItemMessage(dlgWnd,IDC_REFLIST, LB_INSERTSTRING, 0, (long)buf);
					}
				}
			}
		}
	}
/*
	if(ref == 0)
	{
		sprintf(buf, "<none>");
		SendDlgItemMessage(dlgWnd,IDC_REFLIST, LB_INSERTSTRING, 0, (long)buf);
	}
*/
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void GetObjectBitmapsFromBank(int bankNum)
{
	char buf[256];
	OBJECTINFO *ptr;

	numObjectBitmaps = 0;
	if(objectBanks[bankNum].loaded == 0)
		LoadNDOSFromBank(bankNum);

	for(ptr = objectBanks[bankNum].objectList.head.next;ptr != &objectBanks[bankNum].objectList.head;ptr = ptr->next)
	{
		if(ptr->object)
			GetObjectBitmapsFromNDO(ptr->object);
	}

	UpdateObjectStatusBar();

}
/*	--------------------------------------------------------------------------------
	Function 	: EdgeCorrect
	Purpose 	: checks the surrounding pixels and changes the colour to match the average
				  this reduces the purple outline effect.
	Parameters 	: x, y position of pixel, r, g, b values to return
	Returns 	: 
	Info 		:
*/
void EdgeCorrect(int x, int y, unsigned char *rv, unsigned char *gv, unsigned char *bv, BITMAPOBJ *bmp)
{
	int	cx, cy;
	int count = 0;
	int srval, sgval, sbval;
	float tr = 0.0, tg = 0.0, tb = 0.0;
	int tx, ty;

	//check surrounding eight pixels

	for(cy = y - 1; cy <= y + 1; cy++)
	{
		for(cx = x - 1; cx <= x+1; cx++)
		{
			//if pixel is not source pixel
			if((cx == x) && (cy == y));
			else
			{

				//if pixel is on bitmap
				tx = cx;
				ty = cy;


				if(tx < 0)
					tx = bmp->width -1;

				if(tx >= bmp->width)
					tx = 0;

				if(ty < 0)
					ty = bmp->height -1;

				if(ty >= bmp->height)
					ty = 0;


//				if((cx >= 0) && (cy >= 0) && (cx < bmp->width) && (cy < bmp->height))
				{
					//if colour is not transparent
					sbval = bmp->data[(ty * bmp->width + tx) * 3 + 0] >> 3;
					sgval = bmp->data[(ty * bmp->width + tx) * 3 + 1] >> 3;
					srval = bmp->data[(ty * bmp->width + tx) * 3 + 2] >> 3;
					if((srval == 31) && (sbval == 31) && (sgval == 0));
					else
					{
						count++;
						tr += srval;
						tg += sgval;
						tb += sbval;
					}					

				}
			}
		}
	}

	if(count)
	{
		tr /= count;
		tg /= count;
		tb /= count;

		(*rv) = (char)tr;
		(*gv) = (char)tg;
		(*bv) = (char)tb;

	}
}



/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void AdjustColours(unsigned char *r, unsigned char *g, unsigned char *b)
{
	unsigned char rv, gv, bv;
	float	tempFloat;
	float	saturation = 2.0;	

	rv = *r;
	gv = *g;
	bv = *b;

 	if(adjustColours==1)
	{
 		saturation = 1.0;
		saturation /= 93;
		saturation *= (rv + gv + bv);	
		saturation  = 1 - saturation;

//red		
		saturation = 1.0;
		saturation /= 31;
		saturation *= rv;	
		saturation  = 1 - saturation;

		tempFloat = rv;
		tempFloat /= 1 + (0.086 * saturation);
		if(tempFloat > 31)tempFloat = (float)31;
		rv = (unsigned char)tempFloat;								

//green
		saturation = 1.0;
		saturation /= 31;
		saturation *= gv;	
		saturation  = 1 - saturation;

		tempFloat = gv;
		tempFloat *= 1 + (0.04 * saturation);
		if(tempFloat > 31)tempFloat = (float)31;
		gv = (unsigned char)tempFloat;								

//blue
		saturation = 1.0;
		saturation /= 31;
		saturation *= bv;	
		saturation  = 1 - saturation;

		tempFloat = bv;
		tempFloat /= 1 + (0.06 * saturation);
		if(tempFloat > 31)tempFloat = (float)31;
		bv = (unsigned char)tempFloat;								
	}
 	if(adjustColours==2)
	{
 		saturation = 1.0;
		saturation /= 93;
		saturation *= (rv + gv + bv);	
		saturation  = 1 - saturation;

//red		
		saturation = 1.0;
		saturation /= 31;
		saturation *= rv;	
		saturation  = 1 - saturation;

		tempFloat = rv;
		tempFloat /= 1 + (0.086 * saturation);
		if(tempFloat > 31)tempFloat = (float)31;
		rv = (unsigned char)tempFloat;								

//green
		saturation = 1.0;
		saturation /= 31;
		saturation *= gv;	
		saturation  = 1 - saturation;

		tempFloat = gv;
		tempFloat *= 1 + (0.04 * saturation);
		if(tempFloat > 31)tempFloat = (float)31;
		gv = (unsigned char)tempFloat;								

//blue
		saturation = 1.0;
		saturation /= 31;
		saturation *= bv;	
		saturation  = 1 - saturation;

		tempFloat = bv;
		tempFloat /= 1 + (0.06 * saturation);
		if(tempFloat > 31)tempFloat = (float)31;
		bv = (unsigned char)tempFloat;								
	}

	*r = rv;
	*g = gv;
	*b = bv;

}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteBitmap(BITMAP_TYPE *ptr, FILE *fp)
{
	int x,y,k, crc,r,g,b,a;
	unsigned char rv, gv, bv;
	int bpp;
	char *fname;
	BITMAPOBJ	*bmp;
	float	textureSize;
	int		paletteSize, pad = 0;
	int		temp;
	BOOL	alpha;
	int		index = 0;



	bmp = ptr->bmpObj;

	bpp = bmp->bpp;
//	bpp = bmp->bitInf.bi.biBitCount;
	if(bpp == 24)
		bpp = 16;

	if(ptr->cycleSpeed != 0)
		ptr->flags |= COLOUR_CYCLING;

	fname = strlwr(ptr->name);
	crc = UpdateCRC(0, fname, strlen(fname));
	WriteInt(fp, crc);														//id
	dbprintf("%s : 0x%X\n", fname, crc);

	WriteChar(fp, ptr->cycleStart);
	WriteChar(fp, ptr->cycleEnd);
	WriteShort(fp, ptr->flags);
	WriteShort(fp, ptr->cycleSpeed);
	WriteShort(fp, 0);	//cycle count

	WriteShort(fp, (unsigned short)bmp->width);												//sx
	WriteShort(fp, (unsigned short)bmp->height);												//sy

	switch(bmp->width)
	{
		case 8:
			WriteShort(fp, 3);
			break;
		case 16:
			WriteShort(fp, 4);
			break;
		case 32:
			WriteShort(fp, 5);
			break;
		case 64:
			WriteShort(fp, 6);
			break;
		default:
			WriteShort(fp, 5);
			break;
	};
	switch(bmp->height)
	{
		case 8:
			WriteShort(fp, 3);
			break;
		case 16:
			WriteShort(fp, 4);
			break;
		case 32:
			WriteShort(fp, 5);
			break;
		case 64:
			WriteShort(fp, 6);
			break;
		default:
			WriteShort(fp, 5);
			break;
	};

//G_IM_FMT_RGBA, G_IM_SIZ_16b
	
	textureSize = (float)(bmp->width * bmp->height);
	textureSize *= (float)bpp / 8;
	textureSize += sizeof(TEXTURE);

	pad = 0;
	while(((int)(textureSize + pad) % 8))
		pad++;

	if(highColTextures == FALSE)
	{
		switch(bpp)
		{
			case 4:
//						dbprintf("outputting 4 bit image textureSize:%d\n", (int)textureSize);
				paletteSize = 16 * 2;

				WriteInt(fp, (int)textureSize + paletteSize + pad);	//size
				WriteShort(fp, G_IM_FMT_RGBA);		//type
				WriteShort(fp, G_IM_SIZ_4b);		//colsize
				WriteInt(fp, (int)sizeof(TEXTURE));	//data
				WriteInt(fp, (int)textureSize + pad);	//palette


				for (y = bmp->height - 1; y >= 0; y--)
				{
					for (x = 0; x < bmp->width / 2; x++)
					{
						bv = bmp->data[(y * (bmp->width/2) + x)];
						WriteChar(fp, bv);
					}
				}

				while(pad--)
					WriteChar(fp, 0);

				//palette
				y = 0;
				for(x = 0; x < 16; x++)
				{
					rv = bmp->palette[y++] >> 3;
					gv = bmp->palette[y++] >> 3;
					bv = bmp->palette[y++] >> 3;

					alpha = FALSE;
					if((bv == 31) && (rv == 31) && (gv == 0))		
						alpha = TRUE;

					adjustColours = 1;
					AdjustColours(&rv, &gv, &bv);

					k = rv << 10;
					k += gv << 5;
					k += bv;

					k <<= 1;

					if(!alpha)
						k++;
/*				
					if(k != 63550)
						k+=1;
*/
					WriteShort(fp, (unsigned short)k);
				}
				break;

			case 8:
				if(ptr->flags & GIF_FORMAT)		//check to see if it is IA texture
				{
					dbprintf("\t%s is GIF\n", fname);

					textureSize = (float)(bmp->width * bmp->height);
					textureSize *= (float)16 / 8;
					textureSize += sizeof(TEXTURE);

					paletteSize = 0;

					WriteInt(fp, (int)textureSize + paletteSize + pad);	//size
					WriteShort(fp, G_IM_FMT_IA);		//type
					WriteShort(fp, G_IM_SIZ_16b);		//colsize
					WriteInt(fp, (int)sizeof(TEXTURE));	 //data
					WriteInt(fp, 0);//palette
//							WriteInt(fp, (int)textureSize + pad);//palette

					a = 0;						
					for (y = bmp->height - 1; y >= 0; y--)
					{
						for (x = 0; x < bmp->width; x++)
						{
							bv = bmp->data[(y * (bmp->width) + x)];
	
							WriteChar(fp, (char)(255));
							WriteChar(fp, (char)(bv));
						}
					}

					while(pad--)
						WriteChar(fp, 0);
				}
				else if(ptr->flags & GIF_FORMATPH)		//check to see if it is IA texture
				{
					dbprintf("\t%s is phongGIF\n", fname);

					textureSize = (float)(bmp->width * bmp->height);
					textureSize *= (float)16 / 8;
					textureSize += sizeof(TEXTURE);

					paletteSize = 0;

					WriteInt(fp, (int)textureSize + paletteSize + pad);	//size
					WriteShort(fp, G_IM_FMT_IA);		//type
					WriteShort(fp, G_IM_SIZ_16b);		//colsize
					WriteInt(fp, (int)sizeof(TEXTURE));	 //data
					WriteInt(fp, 0);//palette
//							WriteInt(fp, (int)textureSize + pad);//palette

					a = 0;						
					for (y = bmp->height - 1; y >= 0; y--)
					{
						for (x = 0; x < bmp->width; x++)
						{
							bv = bmp->data[(y * (bmp->width) + x)];
	
							WriteChar(fp, (char)(bv));
							WriteChar(fp, (char)(255));
						}
					}

					while(pad--)
						WriteChar(fp, 0);
				}
				else
				{
					paletteSize = 256 * 2;

					WriteInt(fp, (int)textureSize + paletteSize + pad);	//size
					WriteShort(fp, G_IM_FMT_RGBA);		//type
					WriteShort(fp, G_IM_SIZ_8b);		//colsize
					WriteInt(fp, (int)sizeof(TEXTURE));														//data
					WriteInt(fp, (int)textureSize + pad);														//palette

					a = 0;						
					for (y = bmp->height - 1; y >= 0; y--)
					{
						for (x = 0; x < bmp->width; x++)
						{
							bv = bmp->data[(y * (bmp->width) + x)];
	
							WriteChar(fp, (char)(bv));
						}
					}

					while(pad--)
						WriteChar(fp, 0);

					y = 0;
					for(x = 0; x < 256; x++)
					{
						rv = bmp->palette[y++] >> 3;
						gv = bmp->palette[y++] >> 3;
						bv = bmp->palette[y++] >> 3;

						alpha = FALSE;
						if((bv == 31) && (rv == 31) && (gv == 0))		
							alpha = TRUE;

						adjustColours = 1;
						AdjustColours(&rv, &gv, &bv);

						k = rv << 10;
						k += gv << 5;
						k += bv;

						k <<= 1;		
						if(!alpha)
						{
							k+=1;
						}
						WriteShort(fp, (unsigned short)k);
					}
				}					
				break;
			case 16:
//						dbprintf("outputting 24 bit image textureSize:%d\n", (int)textureSize);

				if(strstr(ptr->name,"mm_") == ptr->name)
				{
					CalculateAndOutputMipMap(bmp,fp);
				}
				else
				{
					if(textureSize + pad == 0)
						y = 10;
					WriteInt(fp, (int)textureSize + pad);	//size
					WriteShort(fp, G_IM_FMT_RGBA);			//type
					WriteShort(fp, G_IM_SIZ_16b);			//colsize
					WriteInt(fp, (int)sizeof(TEXTURE));		//data
					WriteInt(fp, 0);						//palette

					temp = (bmp->width * bmp->height * 3) -1;
					for (y = bmp->height - 1; y >= 0; y--)
					{
						for (x = 0; x < bmp->width; x++)
						{
							index = (y * bmp->width + x) * 3;
							bv = bmp->data[index + 0] >> 3;
							gv = bmp->data[index + 1] >> 3;
							rv = bmp->data[index + 2] >> 3;
	
							alpha = FALSE;
							
							if((bv == 31) && (rv == 31) && (gv == 0))		
							{
								alpha = TRUE;

								//this pixel is transparent - must check surrounding pixels
								//this changes colour of pixel to match neighbours
								//note it is STILL transparent....
								EdgeCorrect(x, y, &rv, &gv, &bv, bmp);
							}							
															
							adjustColours = 1;
							AdjustColours(&rv, &gv, &bv);

							k = rv << 10;
							k += gv << 5;
							k += bv;

//								k=*(unsigned short*)(PicyList[i].bmpObj->data + 3 * x + y * bmp->width);
							k <<= 1;
				
							if(!alpha)
								k+=1;

							WriteShort(fp, (unsigned short)k);
						}					
					}
					while(pad--)
						WriteChar(fp, 0);
				}
				break;
			default:
				dbprintf("Whoops, what colour is that?\n");
				break;
		}
	}			
	else if(highColTextures == TRUE)
	{
		a = 255;
		for (y=0; y < bmp->height;y++)
		{
			for (x=0;x<bmp->width;x++)
			{
				k=*(unsigned short*)(bmp->data+2*x+y);
//						k=*(unsigned short*)(bmp->data+2*x+y*bmp->pitch);

				r = k >> 10;
				k = k & 0x3ff;
				g = k >> 5;
				k = k & 0x1f;
				b = k;

				r = (r * 255)/31;
				g = (g * 255)/31;
				b = (b * 255)/31;
	
				k = r<<24;
				k += g<<16;
				k += b<<8;
				k += a;

				WriteInt(fp, (unsigned int)k);
			}
		}
	}
	
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void ExportTextureBank(int bankNum)
{
	char buf[300],buf2[300];
	int i;
	FILE* fp;
	char *tptr;
	int		pad = 0;
	BITMAP_TYPE *ptr;

/*
	if(!GetSaveName(buf2, "Save texture bank", "Nintendo Texture Bank Files (*.ntb)\0*.ntb\0All Files (*.*)\0*.*\0"))
		return;			
*/

	if(strlen(textureBanks[bankNum].exportPath) == 0)
		return;	

	sprintf(buf, "Writing texture bank %s", textureBanks[bankNum].filename);
	SetupProgressBar(0, textureBanks[bankNum].numTextures, buf);

	fp=fopen(textureBanks[bankNum].exportPath,"wb");
	if (!fp)
	{
		sprintf(buf,"Unable to create\n%s",buf2);
		MessageBox(NULL,buf,"Error",MB_ICONWARNING|MB_TASKMODAL);
		return;
	}

	InitCRCTable();	//init crc table used for storing texture names as numbers

	WriteInt(fp, textureBanks[bankNum].numTextures);


	for(i = 0; i < textureBanks[bankNum].numTextures; i++)
	{
		ptr = &textureBanks[bankNum].textures[i];
		WriteBitmap(ptr, fp);
		StepProgressBar();
	}

	KillProgressBar();
	fclose(fp);

}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void ExportObjectTextureBank(int bankNum)
{

	char buf[300],buf2[300];
	int i,x,y,k, crc,r,g,b,a;
	unsigned char rv, gv, bv;
	int tempInt;
	FILE* fp;
	char *fname,*tptr;
	int bpp;
	BITMAPOBJ	*bmp;
	float	textureSize;
	int		paletteSize, pad = 0;
	int		temp;
	float	tempFloat;
	BOOL	alpha;
	BITMAP_TYPE *ptr;
	OBJECTINFO *oPtr;


	if(strlen(objectBanks[bankNum].textureOutput) == 0)
		return;

	sprintf(buf, objectBanks[bankNum].filename);
	buf[strlen(buf) - 4] = 0;

	sprintf(buf2, "Export texture bank '%s'", buf);
	if(MessageBox(NULL,buf2,"Are you sure?",MB_ICONQUESTION|MB_TASKMODAL|MB_OKCANCEL) == IDCANCEL)
		return;


	sprintf(buf2,"%s\\%s.ntb", objectBanks[bankNum].textureOutput, buf);
	fp=fopen(buf2,"wb");
	if (!fp)
	{
		sprintf(buf,"Unable to create\n%s",buf2);
		MessageBox(NULL,buf,"Error",MB_ICONWARNING|MB_TASKMODAL);
		return;
	}

	InitCRCTable();	//init crc table used for storing texture names as numbers


	//make sure all textures are loaded
	for(x = 0; x < numTextureBanks; x++)
		if(textureBanks[x].numTextures == 0)
			LoadTextureBank(x, textureBanks[x].filename);


	numObjectBitmaps = 0;
	for(oPtr = objectBanks[bankNum].objectList.head.next;oPtr != &objectBanks[bankNum].objectList.head;oPtr = oPtr->next)
	{
		if(oPtr->object)
			GetObjectBitmapsFromNDO(oPtr->object);
	}

	SetupProgressBar(0, numObjectBitmaps, "Exporting texture bank");

	WriteInt(fp, numObjectBitmaps);
	for(i = 0; i < numObjectBitmaps; i++)
	{
		ptr = objectBitmaps[i];

		WriteBitmap(ptr, fp);
		StepProgressBar();
	}

	fclose(fp);
	KillProgressBar();
}



/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CalculateAndOutputMipMap(BITMAPOBJ *bmp,FILE *fp)
{
	unsigned short *data,*newData;
	int width,height,size;

	data = (unsigned short *)malloc(bmp->width*bmp->height*2);

	width = bmp->width;
	height = bmp->height;
	size = width*height;

	ConvertBitmapDataToShorts(data,bmp);
	OutputTextureHeader(fp,(1372*2) + 36);
	OutputData(fp,data,size,32,0);

	do
	{
		width /= 2;
		height /= 2;
		size = width*height;
		
		newData = (unsigned short *)malloc(size*2);
		FilterData(newData,data,width,height);
		free(data);
		if(size == 4)
		{
			OutputData(fp,newData,2,2,0);
			OutputData(fp,newData,4,4,0);
			OutputData(fp,newData+2,2,2,0);
		}
		else if(size == 1)
		{
			OutputData(fp,newData,size,1,0);
			OutputData(fp,newData,size,1,0);
			OutputData(fp,newData,size,1,0);
			OutputData(fp,newData,size,1,0);
		}
		else
			OutputData(fp,newData,size,width,0);
		data = newData;
	}while((height > 1) && (width > 1));
	free(newData);
}


void ConvertBitmapDataToShorts(unsigned short *destData,BITMAPOBJ *bmp)
{
	unsigned char bv,gv,rv;
	int x,y,alpha,k,yVal = 0;

	for (y = bmp->height - 1; y >= 0; y--)
	{
		for (x = 0; x < bmp->width; x++)
		{
			bv = bmp->data[(y * bmp->width + x) * 3 + 0] >> 3;
			gv = bmp->data[(y * bmp->width + x) * 3 + 1] >> 3;
			rv = bmp->data[(y * bmp->width + x) * 3 + 2] >> 3;

			alpha = FALSE;
			
			if((bv == 31) && (rv == 31) && (gv == 0))		
			{
				alpha = TRUE;

				//this pixel is transparent - must check surrounding pixels
				//this changes colour of pixel to match neighbours
				//note it is STILL transparent....
				EdgeCorrect(x, y, &rv, &gv, &bv, bmp);
			}							
											
			adjustColours = 1;
			AdjustColours(&rv, &gv, &bv);

			k = rv << 10;
			k += gv << 5;
			k += bv;

	//								k=*(unsigned short*)(PicyList[i].bmpObj->data + 3 * x + y * bmp->width);
			k <<= 1;

			if(!alpha)
				k+=1;

			destData[yVal*bmp->width+x] = k;
		}
		yVal++;
	}
}

void OutputTextureHeader(FILE *fp,int size)
{
	WriteInt(fp, size);
	WriteShort(fp, G_IM_FMT_RGBA);			
	WriteShort(fp, G_IM_SIZ_16b);			
	WriteInt(fp, (int)sizeof(TEXTURE));		
	WriteInt(fp, 0);						
}

void OutputData(FILE *fp,unsigned short *data,int numShorts,int width,int pitch)
{
	int j = 0,offset = 0,num = 0;


	for(num = 0;num < numShorts/width;num++)
	{
		if(num & 1)
		{
			for(j = 0;j < width/4;j++)
			{
				WriteShort(fp,data[num*width+j*4+2]);
				WriteShort(fp,data[num*width+j*4+3]);
				WriteShort(fp,data[num*width+j*4]);
				WriteShort(fp,data[num*width+j*4+1]);
			}
		}
		else
			for(j = 0;j < width;j++)
				WriteShort(fp,data[num*width+j]);
	}
}

void FilterData(unsigned short *destData,unsigned short *data,int width,int height)
{
	int x,y,j,count;
	int rv[4],gv[4],bv[4];
	int R,G,B,k;

	for(x = 0;x < width;x++)
	{
		for(y = 0;y < height;y++)
		{
			count = 0;

			rv[0] = (data[(y*2 * width*2) + x*2] & 0xF800)>>11;
			gv[0] = (data[(y*2 * width*2) + x*2] & 0x7C0)>>6;
			bv[0] = (data[(y*2 * width*2) + x*2] & 0x3E)>>1;

			rv[1] = (data[(y*2 * width*2) + x*2+1] & 0xF800)>>11;
			gv[1] = (data[(y*2 * width*2) + x*2+1] & 0x7C0)>>6;
			bv[1] = (data[(y*2 * width*2) + x*2+1] & 0x3E)>>1;

			rv[2] = (data[((y*2+1) * width*2) + x*2+1] & 0xF800)>>11;
			gv[2] = (data[((y*2+1) * width*2) + x*2+1] & 0x7C0)>>6;
			bv[2] = (data[((y*2+1) * width*2) + x*2+1] & 0x3E)>>1;

			rv[3] = (data[((y*2+1) * width*2) + x*2] & 0xF800)>>11;
			gv[3] = (data[((y*2+1) * width*2) + x*2] & 0x7C0)>>6;
			bv[3] = (data[((y*2+1) * width*2) + x*2] & 0x3E)>>1;

			if(!(data[(y*2 * width*2) + x*2] & 1))
			{
				rv[0] = bv[0] = 0;
				count++;
			}

			if(!(data[(y*2 * width*2) + x*2+1] & 1))
			{
				rv[1] = bv[1] = 0;
				count++;
			}

			if(!(data[((y*2+1) * width*2) + x*2+1] & 1))
			{
				rv[2] = bv[2] = 0;
				count++;
			}

			if(!(data[((y*2+1) * width*2) + x*2] & 1))
			{
				rv[3] = bv[3] = 0;
				count++;
			}

			if(count < 4)
			{
				R = (rv[0] + rv[1] + rv[2] + rv[3])/(4-count);
				G = (gv[0] + gv[1] + gv[2] + gv[3])/(4-count);
				B = (bv[0] + bv[1] + bv[2] + bv[3])/(4-count);
			}
			else
			{
				R = 31;
				G = 0;
				B = 31;
			}

			k = R << 11;
			k += G << 6;
			k += B << 1;
			if(count < 2)
				k += 1;

			destData[y*width + x] = k;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
BOOL FAR PASCAL BitmapProperties(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam)
{
	char buf[256];
	PAINTSTRUCT ps;
	BITMAP_TYPE *bmp = (BITMAP_TYPE *)mouse.selectedTile[0].bitmap;
	static int cycleOffset = 0;
	static int cycleStart = 0;
	static int cycleEnd = 0;
	static int cycleStep = 0;
	static int cycleInc = 0;
	static int timerSpeed;
	static int timerID;
	short xs, ys;

	switch(msg)
	{
		case	WM_COMMAND:
			if(LOWORD(wParam) == IDOK)
			{
				if(bmp->bmpObj->bpp == 4)
				{
					bmp->cycleSpeed = GetDlgItemInt(hwnd, IDC_CYCLESPEED, NULL, TRUE);
					bmp->cycleStart = GetDlgItemInt(hwnd, IDC_CYCLESTART, NULL, FALSE);
					bmp->cycleEnd = GetDlgItemInt(hwnd, IDC_CYCLEEND, NULL, FALSE);

					ChangeCyclingSpeed(bmp);
					DialogBox(hAppInst, "FLYNN", hwnd, (DLGPROC)WaitForFlynn);
					RefreshTextureBank(FALSE);
				}
 				KillTimer(hwnd, timerID);
			 	EndDialog(hwnd, TRUE);
			}
			if(LOWORD(wParam) == IDCANCEL)
			{
 				KillTimer(hwnd, timerID);
				EndDialog(hwnd, FALSE);
			}

			if(LOWORD(wParam) == IDPREVIEW)
			{
				cycleInc = GetDlgItemInt(hwnd, IDC_CYCLESPEED, NULL, TRUE);
				cycleStart = GetDlgItemInt(hwnd, IDC_CYCLESTART, NULL, FALSE);
				cycleEnd = GetDlgItemInt(hwnd, IDC_CYCLEEND, NULL, FALSE);
 
 				KillTimer(hwnd, timerID);
				timerID = SetTimer(hwnd, NULL, 50,NULL);

				if(cycleInc > 0)
					cycleOffset = 0;
				else
					cycleOffset = cycleEnd - cycleStart;
			}

			if(LOWORD(wParam) == IDCLEAR)
			{
				SetDlgItemInt(hwnd, IDC_CYCLESPEED, 0, FALSE);
				SetDlgItemInt(hwnd, IDC_CYCLESTART, 0, FALSE);
				SetDlgItemInt(hwnd, IDC_CYCLEEND, 0, FALSE);
			}	
			break;
		case	WM_INITDIALOG:
			cycleOffset = 0;
			
			SendDlgItemMessage(hwnd,IDC_BITMAPNAME, WM_SETTEXT, 0, (long)bmp->fullFilename);
			sprintf(buf, "%d x %d :%d bpp", bmp->bmpObj->width, bmp->bmpObj->height, bmp->bmpObj->bpp);
			SendDlgItemMessage(hwnd,IDC_BMPINFO, WM_SETTEXT, 0, (long)buf);

			if(bmp->bmpObj->bpp == 4)
			{
				SetDlgItemInt(hwnd, IDC_CYCLESPEED, bmp->cycleSpeed, TRUE);
				SetDlgItemInt(hwnd, IDC_CYCLESTART, bmp->cycleStart, FALSE);
				SetDlgItemInt(hwnd, IDC_CYCLEEND, bmp->cycleEnd, FALSE);
			}
			else
			{
				SendDlgItemMessage(hwnd,IDC_CYCLESPEED, WM_ENABLE, 0, 0);
				SendDlgItemMessage(hwnd,IDC_CYCLESTART, WM_ENABLE, 0, 0);
				SendDlgItemMessage(hwnd,IDC_CYCLEEND, WM_ENABLE, 0, 0);
			}
 
			return TRUE;
			break;				

		case WM_TIMER:
			RECT rect;
			xs = bmp->bmpObj->width < 160 ? bmp->bmpObj->width : 160;
			ys = bmp->bmpObj->height < 120 ? bmp->bmpObj->height : 120;

			if(cycleInc > 0)
			{
				cycleStep += cycleInc;
				while(cycleStep > 64)
				{
					cycleStep -= 64;
					cycleOffset = (cycleOffset + 1) % (cycleEnd - cycleStart);
				}
			}
			else
			{
				cycleStep += -cycleInc;
				while(cycleStep > 64)
				{
					cycleStep -= 64;
					cycleOffset--;
					if(cycleOffset < 0)
						cycleOffset = cycleEnd - cycleStart;
				}

			}
			
			rect.left = 45 + ((160 - xs) / 2); rect.top = 50 + ((120 - ys) / 2); rect.right = rect.left + xs; rect.bottom = rect.top + ys;
			InvalidateRect(hwnd, &rect, FALSE);
			break;



		case WM_PAINT:
			xs = bmp->bmpObj->width < 160 ? bmp->bmpObj->width : 160;
			ys = bmp->bmpObj->height < 120 ? bmp->bmpObj->height : 120;
			BeginPaint(hwnd,&ps);
			PrintBitmap(&ps, bmp->bmpObj, 45 + ((160 - xs) / 2), 50 + ((120 - ys) / 2), xs, ys, cycleOffset, cycleStart, cycleEnd);
			EndPaint(hwnd,&ps);


			break;

	}
	return FALSE;

}


