#include <windows.h>
#include <commctrl.h>
#include <wingdi.h>
#include <string.h>
#include <stdio.h>

#include "types.h"
#include "main.h"
#include "bitmaps.h"
#include "views.h"
#include "files.h"
#include "debug.h"
#include "ndos.h"
#include "resource.h"
#include "commctrl.h"

int objectTexturesOffset = 0;
int textureBankOffset = 0;


INPUT_OBJECT *currentSubObject = NULL;
int currentpicyNum=0;
int currentobjectNum=0;

MOUSE mouse; 

ObjectExplorerType objectexplorer;
	  
int textR = 255, textG = 255, textB = 255;


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void TextColour(int r, int g, int b)
{
	textR = r;
	textG = g;
	textB = b;
}
void PrintText(char * text, int x, int y, HDC dc)
{

	SetTextColor(dc, RGB(0,0,0));
	TextOut(dc, x,y, text, strlen(text));
	SetTextColor(dc, RGB(textR,textG,textB));
	TextOut(dc, x-1,y-1, text, strlen(text));

}

/*
void PrintObjectStats(PAINTSTRUCT *ps)
{
	int line = 0;
	int space = 14;
	char	buf[256];
//	PAINTSTRUCT ps;
	OBJECTINFO	*obj;
//	RECT	rc;
	char	*fname;
	char	*viewName;

//	BeginPaint(hObjWnd,&ps);
//	GetWindowRect(hObjWnd, &rc);
//	FillRect(ps.hdc,&ps.rcPaint,(HBRUSH)(COLOR_3DSHADOW+1));


	SelectObject(ps->hdc, infoFont);
	SetBkMode(ps->hdc, TRANSPARENT);
	SetBkColor(ps->hdc, RGB(0,0,0));
	SetTextColor(ps->hdc, RGB(0,0,0));

	if((PicyList.numEntries == 0) && (ObjectList.numEntries == 0))
	{
		TextColour(255, 0, 0);
		PrintText("No data loaded", 5, line++ * space, ps->hdc);
	}
	else
	{
		TextColour(255, 255, 255);

		if(mode == TEXTURES)
		{
			if(currentpicy == NULL)
				return;

			sprintf(buf, "(%d / %d)", currentpicyNum+1, PicyList.numEntries);
			PrintText(buf, 5, line++ * space, ps->hdc);

			sprintf(buf, currentpicy->filename);
			strlwr(buf);

			viewName = strstr(buf, ".");	//cut off the extension.
			if(viewName)
				*viewName = 0;

			if(currentpicy->flags & GIF_FORMAT)
				sprintf(buf, "%s [IA]", buf);
			else
				sprintf(buf, buf);

			sprintf(buf, "%s", buf);
			PrintText(buf, 5, line++ * space, ps->hdc);
			
//			sprintf(buf, "%X", UpdateCRC(0, currentpicy->filename, strlen(currentpicy->filename)));
			fname = strlwr(currentpicy->filename);
			sprintf(buf, "%X", UpdateCRC(0, fname, strlen(fname)));

			PrintText(buf, 5, line++ * space, ps->hdc);

			sprintf(buf, "%d x %d", currentpicy->bmpObj->width, currentpicy->bmpObj->height);
			PrintText(buf, 5, line++ * space, ps->hdc);

			sprintf(buf, "%d Bpp", currentpicy->bmpObj->bitInf.bi.biBitCount);
			PrintText(buf, 5, line++ * space, ps->hdc);

	 
	 
	 	}
		if(mode == OBJECTS)
		{
			
			sprintf(buf, "(%d / %d)", currentobjectNum+1, ObjectList.numEntries);
			PrintText(buf, 5, line++ * space, ps->hdc);

			sprintf(buf, "%s (%X)", currentobject->filename);
			PrintText(buf, 5,(line++) * space, ps->hdc);

			if(currentSubObject)
			{
				TextColour(155, 155, 155);

				line++;
				PrintText("______________", 5,(line++) * space, ps->hdc);
		
				sprintf(buf, "<%s>", currentSubObject->name);
				PrintText(buf, 5,(line++) * space, ps->hdc);

				

				sprintf(buf, "%d faces", currentSubObject->mesh->numFaces);
				PrintText(buf, 5,(line++) * space, ps->hdc);
				sprintf(buf, "%d vertices", currentSubObject->mesh->numVertices);
				PrintText(buf, 5,(line++) * space, ps->hdc);
			}
		}
	}
//	EndPaint(hObjWnd,&ps);

	
}


*/






/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
/*
BOOL FAR PASCAL RenameObject(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam)
{
	char	buf[256];
	int x;

	switch(msg)
	{
		case	WM_COMMAND:
			if(LOWORD(wParam) == IDOK)
			{
				GetDlgItemText(hwnd, IDC_EDIT1, buf, 16);
				if(strlen(buf) > 0)
				{
					sprintf(currentobject->object->name, buf);	
				 	EndDialog(hwnd, TRUE);
				}
			}
			if(LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hwnd, TRUE);
			}

			break;
		case	WM_INITDIALOG:

			return TRUE;
				
	}
	return FALSE;





}


*/





/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
HTREEITEM InsertObject(INPUT_OBJECT *obj, HTREEITEM parent)
{
	HTREEITEM resitem;

	resitem = (HTREEITEM)InsertTreeItem(parent, obj->name, obj->children ? 1 : 0, 0, 0, (long)obj);

	if(obj->children)
		InsertObject(obj->children, resitem);

	if(obj->next)
		InsertObject(obj->next, parent);

	return resitem;
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FillExplorerTree(INPUT_OBJECT *obj)
{
	HTREEITEM fileItem = InsertObject(obj, NULL);
	SelectFirstObjectInTree();
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CreateTreeWindow()
{

//	SendDlgItemMessage(dlgWnd,IDC_TREE1, TVM_EXPAND,TVE_COLLAPSE | TVE_COLLAPSERESET, (LPARAM) (HTREEITEM)TVI_ROOT);
	SendDlgItemMessage(dlgWnd,IDC_TREE1, TVM_DELETEITEM, 0, (LPARAM) (HTREEITEM)TVI_ROOT);


	return;


}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitTreeWindow()
{
	int i;
	INPUT_OBJECT *tempObj;
	OBJECTINFO *ptr;

	CreateTreeWindow();
	if(selectedObject == -1)
		return;

	ptr = objectBanks[selectedObjectBank].objectList.head.next;
	for(i = 0; i < selectedObject; i++)
	{
		ptr = ptr->next;
	}
	UpdateObjectBitmapWindow(ptr->object);


	tempObj = &ptr->fileObject;
	sprintf(tempObj->name, "%s", ptr->filename);
	tempObj->numChildren = 1;
	tempObj->children = ptr->object;
	tempObj->flags = tempObj->children->flags;

	FillExplorerTree(tempObj);

}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FillObjList()
{
	OBJECTINFO *ptr;
	int x = 0, i;
	char buf[32];
	
	x = SendDlgItemMessage(dlgWnd,IDC_OBJLIST, LB_GETCOUNT, 0, 0);
	while(x)
		SendDlgItemMessage(dlgWnd,IDC_OBJLIST, LB_DELETESTRING, --x, 0);

	for(ptr = objectBanks[selectedObjectBank].objectList.head.next;ptr != &objectBanks[selectedObjectBank].objectList.head;ptr = ptr->next)
	{
		SendDlgItemMessage(dlgWnd,IDC_OBJLIST, LB_INSERTSTRING, x++, (long)ptr->filename);
	}

	sprintf(buf, "NOB:%d", objectBanks[selectedObjectBank].objectList.numEntries);
	SetDlgItemText(dlgWnd, IDC_NUMOBJ, buf);
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void DestroyTreeWindow()
{
	DestroyWindow(hTreeWnd);
}
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
LONG InsertTreeItem(HTREEITEM parent, char *text, int haschildren, int isexpanded, int imageid, LPARAM lp)
{
	TV_INSERTSTRUCT	tis;

	tis.hParent = parent;
	tis.hInsertAfter = TVI_SORT;
	tis.item.mask = TVIF_STATE | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE |
						TVIF_PARAM | ((haschildren) ? TVIF_CHILDREN : 0);
	tis.item.state = (isexpanded) ? TVIS_EXPANDED : 0;
	tis.item.stateMask = TVIS_EXPANDED;
	tis.item.pszText = text;
	tis.item.cChildren = haschildren;
	tis.item.iImage = imageid;
	tis.item.iSelectedImage = imageid;
	tis.item.lParam = lp;

	return SendDlgItemMessage(dlgWnd,IDC_TREE1, TVM_INSERTITEM, 0, (long)&tis);
//	return TreeView_InsertItem(objectexplorer.treehandle, &tis);
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FindObjExpItemInfo(HTREEITEM item, INPUT_OBJECT **obj)
{
	HTREEITEM	topitem;
	TV_ITEM		itemdat, topitemdat;

	topitem = item;

	SendDlgItemMessage(dlgWnd,IDC_TREE1, TVM_GETNEXTITEM, (WPARAM) (UINT) TVGN_ROOT, (long)&topitem);

	itemdat.mask = TVIF_PARAM;
	itemdat.hItem = item;
	SendDlgItemMessage(dlgWnd,IDC_TREE1, TVM_GETITEM, 0, (long)&itemdat);
//	TreeView_GetItem(objectexplorer.treehandle, &itemdat);
/*	if (topitem == item)
		{
		*obj = NULL;
		}
	else
*/		{
		topitemdat.mask = TVIF_PARAM;
		topitemdat.hItem = topitem;
		SendDlgItemMessage(dlgWnd,IDC_TREE1, TVM_GETNEXTITEM, 0, (long)&topitemdat);
//		TreeView_GetItem(objectexplorer.treehandle, &topitemdat);
		*obj = (INPUT_OBJECT*)itemdat.lParam;
		}
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void SelectFirstObjectInTree()
{
	currentSubObject = NULL;
}

/*	--------------------------------------------------------------------------------
	Function : HandleTreeNotification
	Purpose : handle notification messages from the explorer tree control
	Parameters : notification header
	Returns : handled?
	Info : 
*/

int HandleTreeNotification(NMHDR *nmh)
{
	NM_TREEVIEW		*tvhdr;
//	ObjectInfoType	*objinfo;
	INPUT_OBJECT	*obj = NULL;

	tvhdr = (NM_TREEVIEW*)nmh;

	if (tvhdr->hdr.code == TVN_BEGINLABELEDIT)
		{
/*		editwh = TreeView_GetEditControl(objectexplorer.treehandle);
		SendMessage(editwh, EM_SETLIMITTEXT, 31, 0);
*/		return TRUE;
		}
/*
	if (tvhdr->hdr.code == TVN_ENDLABELEDIT)
		{
		dispinfo = (TV_DISPINFO*)nmh;
		if (dispinfo->item.pszText == NULL)
			return TRUE;
		FindObjExpItemInfo(dispinfo->item.hItem, &objectexplorer.selectobjinfo, &objectexplorer.selectobject);
		if (objectexplorer.selectobject == NULL)
			strcpy(objectexplorer.selectobjinfo->ident, dispinfo->item.pszText);
		else
			strcpy(objectexplorer.selectobject->ident, dispinfo->item.pszText);
		UpdateTreeItem(dispinfo->item.hItem);
		return TRUE;
		}
*/
	if (tvhdr->hdr.code == TVN_SELCHANGED)
		{

			

			FindObjExpItemInfo(tvhdr->itemNew.hItem, &obj);

			if (obj == NULL)
			{
				return 0;
			}
			else
				currentSubObject = obj;


			UpdateObjectBitmapWindow(currentSubObject);
			UpdateObjectStatusBar();

			return 0;


		}

/*	if (tvhdr->hdr.code == NM_RCLICK)
		return ObjExpRClickTree();
*/
	return FALSE;
}

int CycleColour(int palEntry, int palOffset, int cycleStart, int cycleEnd)
{
	if((palEntry >= cycleStart) && (palEntry <= cycleEnd))
	{
		palEntry += palOffset;
		if(palEntry > cycleEnd)
			palEntry -= (cycleEnd - cycleStart);
	}
	return palEntry;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
unsigned char tempBitmapData[640 * 640 * 3];
void CreateImage(BITMAPOBJ *ptr, int paletteOffset, int cycleStart, int cycleEnd)
{
	unsigned char *sPtr, *dPtr;
	int x, y, col;
	unsigned char r, g, b;
	int xs = ptr->width <= 64 ? ptr->width : 64;
	int ys = ptr->width <= 64 ? ptr->width : 64;

	xs = ptr->width;
	ys = ptr->height;

	if(ptr == NULL)
		return;

	sPtr = ptr->data;
	dPtr = tempBitmapData;


	switch(ptr->bpp)
	{
		case 24:
			for(y = 0; y < ys; y++)
			{
				for(x = 0; x < xs; x++)
				{
					*(dPtr++) = *(sPtr++);
					*(dPtr++) = *(sPtr++);
					*(dPtr++) = *(sPtr++);
//					sPtr+=3;
				}
				sPtr += (ptr->width - xs);// * 3;
			}
			
			break;
		case 8:
			for(y = 0; y < ys; y++)
			{
				for(x = 0; x < xs; x++)
				{
					col = *(sPtr++);
					*(dPtr++) = ptr->palette[(col * 3) + 2];
					*(dPtr++) = ptr->palette[(col * 3) + 1];
					*(dPtr++) = ptr->palette[(col * 3) + 0];

				}
				sPtr += (ptr->width - xs);
			}
			break;
		case 4:
			for(y = 0; y < ys; y++)
			{
				for(x = 0; x < xs / 2; x++)
				{
					col = *(sPtr);
					col = (col >> 4) & 0x0f;

					col = CycleColour(col, paletteOffset, cycleStart, cycleEnd);


					r = ptr->palette[(col * 3) + 2];
					g = ptr->palette[(col * 3) + 1];
					b = ptr->palette[(col * 3) + 0];
					*(dPtr++) = r;
					*(dPtr++) = g;
					*(dPtr++) = b;

					col = *(sPtr);
					col = col & 0x0f;
  
					col = CycleColour(col, paletteOffset, cycleStart, cycleEnd);
  //					col = (col + paletteOffset) % 16;
					r = ptr->palette[(col * 3) + 2];
					g = ptr->palette[(col * 3) + 1];
					b = ptr->palette[(col * 3) + 0];
					*(dPtr++) = r;
					*(dPtr++) = g;
					*(dPtr++) = b;



					sPtr++;
				}
			}
			break;

	}



}
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void PrintBitmap(PAINTSTRUCT *ps, BITMAPOBJ *ptr, int xp, int yp, int xsize, int ysize, int offset, int cycleStart, int cycleEnd)
{
	unsigned char *imagePtr;
	HPEN pen;
	BITMAP_TYPE *bmp;

	CreateImage(ptr, offset, cycleStart, cycleEnd);
	
	imagePtr = tempBitmapData;
	StretchDIBits(ps->hdc, xp, yp, xsize, ysize, 0,0, ptr->width, ptr->height, imagePtr, ptr->bmi, DIB_RGB_COLORS, SRCCOPY);

	for(int x = 0; x < mouse.numSelectedTiles; x++)
	{
		bmp = (BITMAP_TYPE *)mouse.selectedTile[x].bitmap;
		if(bmp == NULL)
			return;
		if(bmp->bmpObj == ptr)
		{
			if(bmp->cycleSpeed)
				SelectObject(ps->hdc,GetStockObject(BLACK_PEN));
			else
				SelectObject(ps->hdc,GetStockObject(WHITE_PEN));

			MoveToEx(ps->hdc, xp - 2, yp - 2, NULL);
			LineTo(ps->hdc, xp + xsize + 1, yp - 2);
			LineTo(ps->hdc, xp + xsize + 1, yp + ysize + 1);
			LineTo(ps->hdc, xp - 2, yp + ysize + 1);
			LineTo(ps->hdc, xp - 2, yp - 2);

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
void DrawObjectTextures(HWND hwnd, PAINTSTRUCT *ps)
{
	BITMAP_TYPE *Picy=&myBmp;
	BITMAPOBJ	*ptr = Picy->bmpObj;
	int xp = objectTextureRect.left + 3, yp = objectTextureRect.top + 2;
	int count = objectTexturesOffset;

	if(numObjectBitmaps < 1)
		return;

	for(int j = 0; j < OBJECT_TEXTURES_Y; j++)
	{
		for(int i = 0; i < OBJECT_TEXTURES_X; i++)
		{
			if(count >= numObjectBitmaps)
				return;

			Picy = objectBitmaps[count];
			ptr = Picy->bmpObj;
			count++;
			
			PrintBitmap(ps, ptr, xp, yp, 32, 32, 0, 0, 0); 


			xp += 38;
		}
		yp += 38;
		xp -= 38 * i;
	}

}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void DrawTextureBankTextures(HWND hwnd, PAINTSTRUCT *ps)
{
	BITMAP_TYPE *Picy=&myBmp;
	BITMAPOBJ	*ptr = Picy->bmpObj;
	int xp = 240, yp = 203;
	int count = textureBankOffset;
	HPALETTE syspal;
	unsigned char *imagePtr;


	if(selectedTextureBank == -1)
		return;

	if(textureBanks[selectedTextureBank].numTextures == 0)
		return;

	for(int j = 0; j < TEXTURE_BANK_Y; j++)
	{
		for(int i = 0; i < TEXTURE_BANK_X; i++)
		{
			if(count == textureBanks[selectedTextureBank].numTextures)
				return;

			Picy = &textureBanks[selectedTextureBank].textures[count];
			ptr = Picy->bmpObj;
			count++;

			PrintBitmap(ps, ptr, xp, yp, 32, 32, 0, 0, 0); 

			xp += 38;
		}
		yp += 38;
		xp -= 38 * i;
	}

}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int CountObjectTextureSize()
{
	int size = 0, i, bpp;
	float temp;

	for(i = 0; i < numObjectBitmaps; i++)
	{
		bpp = objectBitmaps[i]->bmpObj->bpp;
		if(bpp == 24)
			bpp = 16;

		if(objectBitmaps[i]->flags & GIF_FORMAT)
			bpp = 16;

		if(objectBitmaps[i]->flags & GIF_FORMATPH)
			bpp = 16;


		temp = (objectBitmaps[i]->bmpObj->width * objectBitmaps[i]->bmpObj->height * bpp) / 8;
		size += temp;
		size += sizeof(TEXTURE);	//add size of texture struct

		if(bpp == 4)
			size += 16 * 2;			//also add palette size
		if(bpp == 8)
			size += 256 * 2;

	}

	return size;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int CountTextureBankSize(short num)
{
	int size = 0, i, bpp;
	float temp;

	for(i = 0; i < textureBanks[num].numTextures; i++)
	{
		bpp = textureBanks[num].textures[i].bmpObj->bpp;
		if(bpp == 24)
			bpp = 16;

		if(textureBanks[num].textures[i].flags & GIF_FORMAT)
			bpp = 16;
		if(textureBanks[num].textures[i].flags & GIF_FORMATPH)
			bpp = 16;


		temp = (textureBanks[num].textures[i].bmpObj->width * textureBanks[num].textures[i].bmpObj->height * bpp) / 8;
		size += temp;
		size += sizeof(TEXTURE);	//add size of texture struct

		if(bpp == 4)
			size += 16 * 2;			//also add palette size
		if(bpp == 8)
			size += 256 * 2;
		
	}

	return size;

}
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void UpdateObjectStatusBar()
{
	char buf[256];
	int textureMem;

	textureMem = CountObjectTextureSize();

	sprintf(buf, "%d textures using %d bytes (%d K)               ", numObjectBitmaps, textureMem, textureMem / 1024);
	SendDlgItemMessage(dlgWnd,IDC_OBJSTATUS, WM_SETTEXT, 0, (long)buf);

/*
	if(currentobject->drawList)
		SendDlgItemMessage(dlgWnd,IDC_DRAWLIST, BM_SETCHECK, BST_CHECKED, 0);
	else
		SendDlgItemMessage(dlgWnd,IDC_DRAWLIST, BM_SETCHECK, BST_UNCHECKED, 0);
*/		

}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void UpdateTextureStatusBar()
{
	char buf[256];
	int textureMem;

	textureMem = CountTextureBankSize(selectedTextureBank);

	sprintf(buf, "%d textures using %d bytes (%d K)               ", textureBanks[selectedTextureBank].numTextures, textureMem, textureMem / 1024);
	SendDlgItemMessage(dlgWnd,IDC_TEXSTATUS, WM_SETTEXT, 0, (long)buf);


}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void UpdateMouse()
{
	int xPos = 0, yPos = 0;
	int xTile = 0, yTile = 0;
	char buf[256];


	mouse.currentTile.window = 0;

	if((mouse.xPos > objectTextureRect.left) && (mouse.yPos > objectTextureRect.top) &&
	   (mouse.xPos < objectTextureRect.right) && (mouse.yPos < objectTextureRect.bottom))
	{
		mouse.currentTile.tileX = (mouse.xPos - objectTextureRect.left) / 38;
		mouse.currentTile.tileY = (mouse.yPos - objectTextureRect.top) / 38;

		mouse.currentTile.window = OBJECT_WINDOW;
		mouse.currentTile.tileNum = (mouse.currentTile.tileX + OBJECT_TEXTURES_X * mouse.currentTile.tileY) + objectTexturesOffset;

		if(mouse.currentTile.tileNum < numObjectBitmaps)// - objectTexturesOffset)
			mouse.currentTile.tileValid = TRUE;
		else
			mouse.currentTile.tileValid = FALSE;

		mouse.currentTile.bitmap = (void *)objectBitmaps[mouse.currentTile.tileNum];


	}

	if((mouse.xPos > textureRect.left) && (mouse.yPos > textureRect.top) &&
	   (mouse.xPos < textureRect.right) && (mouse.yPos < textureRect.bottom))
	{
		mouse.currentTile.tileX = (mouse.xPos - textureRect.left) / 38;
		mouse.currentTile.tileY = (mouse.yPos - textureRect.top) / 38;

		mouse.currentTile.window = TEXTURE_WINDOW;
		mouse.currentTile.tileNum = mouse.currentTile.tileX + TEXTURE_BANK_X * mouse.currentTile.tileY + textureBankOffset;

		if(mouse.currentTile.tileNum < textureBanks[selectedTextureBank].numTextures)// - textureBankOffset - 1)
			mouse.currentTile.tileValid = TRUE;
		else
			mouse.currentTile.tileValid = FALSE;

		mouse.currentTile.bitmap = (void *)&textureBanks[selectedTextureBank].textures[mouse.currentTile.tileNum];

	}




	if((mouse.flags & MK_LBUTTON) && (mouse.currentTile.window))
	{

		if(mouse.flags & MK_SHIFT)
		{
			if((mouse.numSelectedTiles) && (mouse.currentTile.tileValid) && (mouse.currentTile.window == mouse.selectedTile[0].window))
			{
				dbprintf("%d %d\n", mouse.selectedTile[0].tileNum, mouse.currentTile.tileNum);
				int from, to;

				if(mouse.selectedTile[0].tileNum > mouse.currentTile.tileNum)
				{
					from = mouse.currentTile.tileNum;
					to = mouse.selectedTile[0].tileNum;
				}
				else
				{
					to = mouse.currentTile.tileNum;
					from = mouse.selectedTile[0].tileNum;
				}

				int i = 0;
				for(int x = from; x <= to; x++)
				{
					if(mouse.currentTile.window == OBJECT_WINDOW)
						mouse.selectedTile[i].bitmap = (void *)objectBitmaps[x];
					else
						mouse.selectedTile[i].bitmap = (void *)&textureBanks[selectedTextureBank].textures[x];
					i++;
				}
				mouse.numSelectedTiles = i;
			}
		}
		else if(mouse.flags & MK_CONTROL)
		{
			if(mouse.currentTile.tileValid)
			{
				BITMAP_TYPE *bmp, *bmp2;
				
				for(int i = 0; i < mouse.numSelectedTiles; i++)
				{
					bmp = (BITMAP_TYPE *)mouse.selectedTile[i].bitmap;
					bmp2 = (BITMAP_TYPE *)mouse.currentTile.bitmap;
					if(strcmp(bmp->name, bmp2->name) == 0)
						return;
				}
				memcpy(&mouse.selectedTile[mouse.numSelectedTiles], &mouse.currentTile, sizeof(TILE));
				if(mouse.currentTile.window == OBJECT_WINDOW)
					mouse.selectedTile[mouse.numSelectedTiles].bitmap = (void *)objectBitmaps[mouse.currentTile.tileNum];
				else
					mouse.selectedTile[mouse.numSelectedTiles].bitmap = (void *)&textureBanks[selectedTextureBank].textures[mouse.currentTile.tileNum];

				mouse.numSelectedTiles++;
			}

		}
		else
		{
			if(mouse.currentTile.tileValid)
			{
				memcpy(&mouse.selectedTile[0], &mouse.currentTile, sizeof(TILE));
				if(mouse.currentTile.window == OBJECT_WINDOW)
				{
					mouse.selectedTile[0].bitmap = (void *)objectBitmaps[mouse.currentTile.tileNum];
				}
				else
					mouse.selectedTile[0].bitmap = (void *)&textureBanks[selectedTextureBank].textures[mouse.currentTile.tileNum];

				mouse.numSelectedTiles = 1;
			}
		}

		InvalidateRect(dlgWnd, &objectTextureRect, FALSE);
		InvalidateRect(dlgWnd, &textureRect, FALSE);
		UpdateReferenceList();
	}

}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void ClearMouseSelection()
{
	mouse.selectedTile[0].tileValid = 0;
	mouse.numSelectedTiles = 0;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void UpdateReferenceList()
{
	int numEntries;
	BITMAP_TYPE *bmp;
	char buf[MAX_PATH];

	numEntries = 1;
	while(numEntries > 0)
		numEntries = SendDlgItemMessage(dlgWnd,IDC_REFLIST, LB_DELETESTRING, 0, 0);

	numEntries = 1;
	while(numEntries > 0)
		numEntries = SendDlgItemMessage(dlgWnd,IDC_BMPLIST, LB_DELETESTRING, 0, 0);


	for(numEntries = 0; numEntries < mouse.numSelectedTiles; numEntries++)
	{
		bmp = (BITMAP_TYPE *)mouse.selectedTile[numEntries].bitmap;
		sprintf(buf, "%s:%dx%d,%d", bmp->name, bmp->bmpObj->width, bmp->bmpObj->height, bmp->bmpObj->bpp);
		SendDlgItemMessage(dlgWnd,IDC_BMPLIST, LB_INSERTSTRING, 0, (long)buf);
		ListTextureReference(bmp);
	}

}