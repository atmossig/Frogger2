#include <windows.h>
#include <commctrl.h>
#include <wingdi.h>
#include <string.h>
#include <stdio.h>

#include "types.h"
#include "bitmaps.h"
#include "files.h"
#include "ndos.h"
#include "obe.h"
#include "main.h"
#include "views.h"

#include "debug.h"
#include "resource.h"

OBJECTINFO	currentNDO;
OBJECTINFO	*currentObject;

char	*tempObjectPtr, *tempObjectPtr2;


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void AddObject(OBJECTLIST *objList, OBJECTINFO *obj)
{
	OBJECTINFO *ptr = objList->head.next;

	objList->numEntries++;
	obj->next = ptr;
	obj->prev = ptr->prev;
	ptr->prev->next = obj;
	ptr->prev = obj;		

}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void SubObject(OBJECTLIST *objList, OBJECTINFO *ptr)
{

	if(ptr->next == &objList->head)
	{
		currentObject = ptr->next->next;
		currentobjectNum = 0;
	}
	else
	{
		currentObject = ptr->next;
	}
	ptr->prev->next = ptr->next;
	ptr->next->prev = ptr->prev;
	objList->numEntries--;
	ptr->next = NULL;
	free(ptr);

}






/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void ValidateFilename(char *name)
{
	unsigned int i;

	for(i = 0; i < strlen(name); i++)
	{
		if(name[i] == '(')
			name[i] = '_';

		if(name[i] == ')')
			name[i] = '_';

		if(name[i] == '-')
			name[i] = '_';

		if(name[i] == '.')
			name[i] = '_';

		if(name[i] == ',')
			name[i] = '_';

		if(name[i] == ':')
			name[i] = '_';

		if(name[i] == ';')
			name[i] = '_';

	}

	name[15] = 0;

}



/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void AddNDOToBank()
{
	OBJECTINFO *ptr;
	FILE *fp;
	char buf[256];
	OPENFILENAME ofn;
	HWND hDlg;
	static char buffer[4096];
	char addPath[256];
	char tempPath[256];
	char *cptr, *tBuf;
	int i, NumFiles;
	OBJECTINFO	*objectPtr;
	
	ZeroMemory(&ofn,sizeof(ofn));
	buffer[0]=0;
    ofn.lStructSize=sizeof(ofn);
    ofn.hwndOwner=hMainWnd;
    ofn.hInstance=hAppInst;


    ofn.lpstrFilter="OBE Files (*.obe)\0*.obe\0"; 
	
	ofn.lpstrFile=buffer;    
	ofn.lpstrInitialDir=recentAddPath;//".";//objectAddPath;	
	ofn.nMaxFile=4095;
    ofn.lpstrTitle="Add object files"; 
    ofn.Flags=OFN_ALLOWMULTISELECT|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_EXPLORER|OFN_HIDEREADONLY; 
    
	i=GetOpenFileName(&ofn);
	if (!i) return;

	if (buffer[strlen(buffer)+1]==0)	//Double Terminated ie one name
	{
		if (strrchr(buffer,'\\'))
			*strrchr(buffer,'\\')=0;
	}
	strcpy(addPath,buffer);

	cptr=buffer+ofn.nFileOffset;		

	//Get number of files (i)
	for (i=0;*cptr;i++)
		cptr+=strlen(cptr)+1;


	NumFiles = i;
	cptr=buffer+ofn.nFileOffset;		

	strcpy(recentAddPath, ofn.lpstrFile);
	for (i=0;*cptr;i++)
	{
		char texName[MAX_PATH];


		ptr = (OBJECTINFO *)calloc(sizeof(OBJECTINFO), 1);
		AddObject(&objectBanks[selectedObjectBank].objectList, ptr);
		ptr->drawList = TRUE;
		sprintf(ptr->fullFilename, "%s\\%s", addPath, cptr);
		tBuf = ptr->fullFilename + strlen(ptr->fullFilename);
		while(*(tBuf--) != '\\');
		tBuf += 2;
		sprintf(ptr->filename, "%s", tBuf);

		LoadOBEObject(ptr);
		cptr+=strlen(cptr)+1;
	}


	return;


	
}











/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void LoadNDOSFromBank(int bankNum)
{
	int i = 0;
	OBJECTINFO	*ptr;
	char buf[256];
//	FreeNDOSFromBank(bankNum);

	sprintf(buf, "object bank %s", objectBanks[bankNum].filename);
	SetupProgressBar(0, objectBanks[bankNum].objectList.numEntries, buf);

	//the last texture banks is used as a temporary bank to store bitmaps associated with the obe
	KillTextureBank(MAX_TEXTURE_BANKS, 1);

	
	for(ptr = objectBanks[bankNum].objectList.head.next;ptr != &objectBanks[bankNum].objectList.head;ptr = ptr->next)
	{
		if(strstr(ptr->filename, ".obe"))
		{
			if (!LoadOBEObject(ptr))
				continue;
		}
		else
		{
//			if (!LoadObject(ptr))
//				continue;
		}
		currentobjectNum = 0;

		StepProgressBar();

	}
	KillProgressBar();
	objectBanks[bankNum].loaded = TRUE;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void LoadOBESFromBank(int bankNum)
{
	int i = 0;
	OBJECTINFO	*ptr;
	char buf[256];
//	FreeNDOSFromBank(bankNum);

	sprintf(buf, "object bank %s", objectBanks[bankNum].filename);
	SetupProgressBar(0, objectBanks[bankNum].objectList.numEntries, buf);
	
	for(ptr = objectBanks[bankNum].objectList.head.next;ptr != &objectBanks[bankNum].objectList.head;ptr = ptr->next)
	{
		if (!LoadOBEObject(ptr))
		{
//			dbprintf("Error:an error has occured\n");
//			objectBanks[bankNum].numObjects = 0;
			continue;
//				return;
		}
//		AddObject(&objectBanks[bankNum].objectList, ptr);
//		objectPtr->drawList = TRUE;

		currentobjectNum = 0;

		StepProgressBar();

	}
	KillProgressBar();
	objectBanks[bankNum].loaded = TRUE;
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FreeNDOSFromBank(int bankNum)
{
	int i = 0;
	OBJECTINFO	*ptr, *ptr2;

	if(objectBanks[bankNum].loaded == 0)
		return;

	for(ptr = objectBanks[bankNum].objectList.head.next;ptr != &objectBanks[bankNum].objectList.head;ptr = ptr2)
	{
		FreeOBE(ptr);
/*		if(ptr->pData)
		{
			free(ptr->pData);
			ptr->pData = NULL;
		}
*/		ptr2 = ptr->next;
				
		SubObject(&objectBanks[bankNum].objectList, ptr);

	}
	objectBanks[bankNum].loaded = 0;
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void LoadAllNDOS()
{
	int i;

	for(i = 0; i < numObjectBanks; i++)
	{
		if(objectBanks[i].loaded == 0)
			LoadNDOSFromBank(i);
	}


}
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
BOOL FAR PASCAL ObjBankProperties(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam)
{
	char buf[MAX_PATH];
	int saveObject = FALSE;

	switch(msg)
	{
		case	WM_COMMAND:
			if(LOWORD(wParam) == IDOK)
			{
				SendDlgItemMessage(hwnd,IDC_OBJBANKNAME, WM_GETTEXT, MAX_PATH, (long)buf);
				if(strcmp(buf, objectBanks[selectedObjectBank].filename))
				{
					char buf2[256];
					sprintf(buf2, "%s\\%s", filePath, objectBanks[selectedObjectBank].filename);
					DeleteFile(buf2);
					strcpy(objectBanks[selectedObjectBank].filename, buf);
					SendDlgItemMessage(dlgWnd,IDC_LIST1, LB_DELETESTRING, selectedObjectBank, 0);
					SendDlgItemMessage(dlgWnd,IDC_LIST1, LB_INSERTSTRING, -1, (long)objectBanks[selectedObjectBank].filename);
					objectBanks[selectedObjectBank].saveMe = TRUE;
				}
				SendDlgItemMessage(hwnd,IDC_OBJBANKPATH, WM_GETTEXT, MAX_PATH, (long)buf);
				if(strcmp(buf, objectBanks[selectedObjectBank].bankFile))
				{
					strcpy(objectBanks[selectedObjectBank].bankFile, buf);
					FreeNDOSFromBank(selectedObjectBank);
					LoadObjectBank(selectedObjectBank);
					LoadNDOSFromBank(selectedObjectBank);
					objectBanks[selectedObjectBank].saveMe = TRUE;
					FillObjList();
					InitTreeWindow();
				}
				SendDlgItemMessage(hwnd,IDC_OBJOUTPATH, WM_GETTEXT, MAX_PATH, (long)buf);
				if(strcmp(buf, objectBanks[selectedObjectBank].objectOutput))
				{
					strcpy(objectBanks[selectedObjectBank].objectOutput, buf);
					objectBanks[selectedObjectBank].saveMe = TRUE;
				}
				SendDlgItemMessage(hwnd,IDC_TEXOUTPATH, WM_GETTEXT, MAX_PATH, (long)buf);
				if(strcmp(buf, objectBanks[selectedObjectBank].textureOutput))
				{
					strcpy(objectBanks[selectedObjectBank].textureOutput, buf);
					objectBanks[selectedObjectBank].saveMe = TRUE;
				}

				GetDlgItemText(hwnd,IDC_LNDSOURCE, buf, MAX_PATH);
				if(strcmp(buf, objectBanks[selectedObjectBank].lndSource))
				{
					strcpy(objectBanks[selectedObjectBank].lndSource, buf);
					objectBanks[selectedObjectBank].saveMe = TRUE;
				}
				GetDlgItemText(hwnd,IDC_LNDDEST, buf, MAX_PATH);
				if(strcmp(buf, objectBanks[selectedObjectBank].lndDest))
				{
					strcpy(objectBanks[selectedObjectBank].lndDest, buf);
					objectBanks[selectedObjectBank].saveMe = TRUE;
				}





				
//				if(saveObject)
//					SaveObjectBanks(FALSE);


			 	EndDialog(hwnd, TRUE);
			}
			if(LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hwnd, FALSE);
			}
			if(LOWORD(wParam) == IDC_BANKFILEBROWSE)
			{
				if(BrowseForFile("Select bank file", "Bank filess (*.bnk)\0*.bnk\0", buf))
				{
					SendDlgItemMessage(hwnd,IDC_OBJBANKPATH, WM_SETTEXT, 0, (long)buf);
				}
			}
			if(LOWORD(wParam) == IDC_OBJOUTBROWSE)
			{
				if(BrowseForDirectory(hwnd, "Selectdirectory to export textures to", buf))
				{
					SendDlgItemMessage(hwnd,IDC_OBJOUTPATH, WM_SETTEXT, 0, (long)buf);
				}
			}
			if(LOWORD(wParam) == IDC_TEXOUTBROWSE)
			{
				if(BrowseForDirectory(hwnd, "Select directory to export textures to", buf))
				{
					SendDlgItemMessage(hwnd,IDC_TEXOUTPATH, WM_SETTEXT, 0, (long)buf);
				}
			}

			if(LOWORD(wParam) == IDC_LNDSRCBROWSE)
			{
				if(BrowseForFile("Select LND file", "LND filess (*.lnd)\0*.lnd\0", buf))
				{
					SendDlgItemMessage(hwnd,IDC_LNDSOURCE, WM_SETTEXT, 0, (long)buf);
				}
			}
			if(LOWORD(wParam) == IDC_LNDDESTBROWSE)
			{
				if(BrowseForDirectory(hwnd, "Select directory to save LND to", buf))
				{
					SendDlgItemMessage(hwnd,IDC_LNDDEST, WM_SETTEXT, 0, (long)buf);
				}
			}




			break;
		case	WM_INITDIALOG:
					SetDlgItemText(hwnd,IDC_OBJBANKNAME, objectBanks[selectedObjectBank].filename);
					SetDlgItemText(hwnd,IDC_OBJBANKPATH, objectBanks[selectedObjectBank].bankFile);
					SetDlgItemText(hwnd,IDC_OBJOUTPATH, objectBanks[selectedObjectBank].objectOutput);
					SetDlgItemText(hwnd,IDC_TEXOUTPATH, objectBanks[selectedObjectBank].textureOutput);
					SetDlgItemText(hwnd,IDC_LNDSOURCE, objectBanks[selectedObjectBank].lndSource);
					SetDlgItemText(hwnd,IDC_LNDDEST, objectBanks[selectedObjectBank].lndDest);
					
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
BOOL FAR PASCAL NewObjBankProperties(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam)
{
	char buf[MAX_PATH];

	switch(msg)
	{
		case	WM_COMMAND:
			if(LOWORD(wParam) == IDOK)
			{
				char tbuf1[256];
				char tbuf2[256];
				char tbuf3[256];
				char tbuf4[256];
				GetDlgItemText(hwnd, IDC_OBJBANKNAME, tbuf1, 256);
				GetDlgItemText(hwnd, IDC_OBJBANKPATH, tbuf2, 256);
				GetDlgItemText(hwnd, IDC_OBJOUTPATH, tbuf3, 256);
				GetDlgItemText(hwnd, IDC_TEXOUTPATH, tbuf4, 256);
				if((strlen(tbuf1)) && (strlen(tbuf2)))// && (strlen(tbuf3)) && (strlen(tbuf4)))
				{
				   	GetDlgItemText(hwnd,IDC_OBJBANKNAME, objectBanks[selectedObjectBank].filename, MAX_PATH);
					GetDlgItemText(hwnd,IDC_OBJBANKPATH, objectBanks[selectedObjectBank].bankFile, MAX_PATH);
					GetDlgItemText(hwnd,IDC_OBJOUTPATH, objectBanks[selectedObjectBank].objectOutput, MAX_PATH);
					GetDlgItemText(hwnd,IDC_TEXOUTPATH, objectBanks[selectedObjectBank].textureOutput, MAX_PATH);

					GetDlgItemText(hwnd,IDC_LNDSOURCE, objectBanks[selectedObjectBank].lndSource, MAX_PATH);
					GetDlgItemText(hwnd,IDC_LNDDEST, objectBanks[selectedObjectBank].lndDest, MAX_PATH);

					if(strstr(objectBanks[selectedObjectBank].filename, ".bmo") == NULL)
						strcat(objectBanks[selectedObjectBank].filename, ".bmo");


					FreeNDOSFromBank(selectedObjectBank);
					if(LoadObjectBank(selectedObjectBank))
					{
						SendDlgItemMessage(dlgWnd,IDC_LIST1, LB_INSERTSTRING, selectedObjectBank, (long)objectBanks[selectedObjectBank].filename);
						LoadNDOSFromBank(selectedObjectBank);
						numObjectBanks++;
//						SaveObjectBank(FALSE);
						objectBanks[selectedObjectBank].saveMe = TRUE;
					

				 		EndDialog(hwnd, TRUE);
					}
					else
					{
						MessageBox(hwnd,"Object bank is empty","F.Y.I",MB_ICONWARNING|MB_TASKMODAL);
						SendDlgItemMessage(dlgWnd,IDC_LIST1, LB_INSERTSTRING, selectedObjectBank, (long)objectBanks[selectedObjectBank].filename);
						numObjectBanks++;
//						SaveObjectBanks(FALSE);
						objectBanks[selectedObjectBank].saveMe = TRUE;

				 		EndDialog(hwnd, TRUE);
					}
				}
			}
			if(LOWORD(wParam) == IDCANCEL)
			{
//				numObjectBanks--;
				selectedObjectBank = 0;
				EndDialog(hwnd, FALSE);
			}
			if(LOWORD(wParam) == IDC_BANKFILEBROWSE)
			{
				if(BrowseForFile("Select bank file", "Bank filess (*.bnk)\0*.bnk\0", buf))
				{
					SendDlgItemMessage(hwnd,IDC_OBJBANKPATH, WM_SETTEXT, 0, (long)buf);
				}
			}
			if(LOWORD(wParam) == IDC_OBJOUTBROWSE)
			{
				if(BrowseForDirectory(hwnd, "Selectdirectory to export textures to", buf))
				{
					SendDlgItemMessage(hwnd,IDC_OBJOUTPATH, WM_SETTEXT, 0, (long)buf);
				}
			}
			if(LOWORD(wParam) == IDC_TEXOUTBROWSE)
			{
				if(BrowseForDirectory(hwnd, "Select directory to export textures to", buf))
				{
					SendDlgItemMessage(hwnd,IDC_TEXOUTPATH, WM_SETTEXT, 0, (long)buf);
				}
			}

			if(LOWORD(wParam) == IDC_LNDSRCBROWSE)
			{
				if(BrowseForFile("Select LND file", "LND filess (*.lnd)\0*.lnd\0", buf))
				{
					SendDlgItemMessage(hwnd,IDC_LNDSOURCE, WM_SETTEXT, 0, (long)buf);
				}
			}
			if(LOWORD(wParam) == IDC_LNDDESTBROWSE)
			{
				if(BrowseForDirectory(hwnd, "Select directory to save LND to", buf))
				{
					SendDlgItemMessage(hwnd,IDC_LNDDEST, WM_SETTEXT, 0, (long)buf);
				}
			}




			break;
		case	WM_INITDIALOG:
					SetDlgItemText(hwnd,IDC_OBJBANKNAME, objectBanks[selectedObjectBank].filename);
					SetDlgItemText(hwnd,IDC_OBJBANKPATH, objectBanks[selectedObjectBank].bankFile);
					SetDlgItemText(hwnd,IDC_OBJOUTPATH, objectBanks[selectedObjectBank].objectOutput);
					SetDlgItemText(hwnd,IDC_TEXOUTPATH, objectBanks[selectedObjectBank].textureOutput);
					SetDlgItemText(hwnd,IDC_LNDSOURCE, objectBanks[selectedObjectBank].lndSource);
					SetDlgItemText(hwnd,IDC_LNDDEST, objectBanks[selectedObjectBank].lndDest);
					
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
void AddObjectFlags(INPUT_OBJECT *obj, int flags)
{
	obj->flags |= flags;
	if(obj->children)
		AddObjectFlags(obj->children, flags);
	if(obj->next)
		AddObjectFlags(obj->next, flags);
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void StripObjectFlags(INPUT_OBJECT *obj, int flags)
{
	obj->flags &= -1 -flags;
	if(obj->children)
		StripObjectFlags(obj->children, flags);
	if(obj->next)
		StripObjectFlags(obj->next, flags);
}
