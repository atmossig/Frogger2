#include <windows.h>
#include <commctrl.h>
#include <wingdi.h>
#include <string.h>
#include <stdio.h>
#include <shlobj.h>
#include <direct.h>

#include "types.h"
#include "main.h"
#include "bitmaps.h"
#include "files.h"
#include "debug.h"
#include "ndos.h"
#include "views.h"
#include "gelf.h"
#include "resource.h"

char filePath[MAX_PATH];
char commandPath[MAX_PATH];
char recentAddPath[MAX_PATH];

OBJECT_BANK_TYPE	objectBanks[MAX_OBJECT_BANKS];
int					numObjectBanks = 0;
int					selectedObjectBank = -1;
int					selectedObject = -1;



TEXTURE_BANK_TYPE	textureBanks[MAX_TEXTURE_BANKS+1];
int					numTextureBanks = 0;
int					selectedTextureBank = -1;
int					selectedTexture = -1;


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void ReadString(char *buf, FILE *in)
{
	fgets(buf, MAX_PATH, in);	//get header
	buf[strlen(buf) - 1] = 0;	
}



/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void LoadTextureBank(int num, char *name)
{
	char buf[256];
	FILE *in;
	int i;

	KillTextureBank(num, 1);


	if(name)
	{
		dbprintf("FileFound: %s\n", name);
		sprintf(textureBanks[num].filename, name);
	}

	//load in bank file
	sprintf(buf, "%s\\%s", filePath, textureBanks[num].filename);
	in = fopen(buf, "rt");
	if(in == NULL)
	{
		dbprintf("Error:could not open file %s\n", buf);
		return;
	}

	fgets(buf, MAX_PATH, in);
	fgets(buf, MAX_PATH, in);
	ReadString(textureBanks[num].destination, in);
	ReadString(textureBanks[num].exportPath, in);
	fgets(buf, MAX_PATH, in);
	fgets(buf, MAX_PATH, in);

	textureBanks[num].numTextures = atoi(buf);

	SetupProgressBar(0, textureBanks[num].numTextures, textureBanks[num].filename);

	for(i = 0; i < textureBanks[num].numTextures; i++)
	{
		char *tBuf;

		//read in bitmap name
		fgets(buf, MAX_PATH, in);
		strlwr(buf);
		buf[strlen(buf)-1] = 0;
		sprintf(textureBanks[num].textures[i].fullFilename, "%s", buf);
		tBuf = buf + strlen(buf);
		while(*(tBuf--) != '\\');
		tBuf += 2;
		
		sprintf(textureBanks[num].textures[i].name, "%s", tBuf);


		//read in texture flags
		fgets(buf, MAX_PATH, in);
		textureBanks[num].textures[i].flags = atoi(buf);


		fgets(buf, MAX_PATH, in);
		textureBanks[num].textures[i].cycleStart = atoi(buf);
		fgets(buf, MAX_PATH, in);
		textureBanks[num].textures[i].cycleEnd = atoi(buf);
		fgets(buf, MAX_PATH, in);
		textureBanks[num].textures[i].cycleSpeed = atoi(buf);



		//load in bitmap
		LoadPicy(&textureBanks[num].textures[i]);
		
		StepProgressBar();
	
	}

	textureBanks[num].loaded = TRUE;
	fclose(in);
	KillProgressBar();
}
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void KillTextureBank(int num, int removeOffset)
{
	int i;

	for(i = 0; i < textureBanks[num].numTextures; i++)
	{
//		free(textureBanks[num].textures[i].bmpObj->logpal);
		free(textureBanks[num].textures[i].bmpObj->palette);
		free(textureBanks[num].textures[i].bmpObj->data);
		textureBanks[num].textures[i].bmpObj->data = NULL;
		free(textureBanks[num].textures[i].bmpObj);
		textureBanks[num].textures[i].bmpObj = NULL;
	}
	textureBanks[num].numTextures = 0;
	if(removeOffset)
		textureBankOffset = 0;
}
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void LoadDefaults()
{
	FILE *in;
	FILE *in2;
	HANDLE fileSearch;
	WIN32_FIND_DATA fData;
	char searchName[] = "*.bmf";
	char buf[256];
	int i;
	int numBmos = 0, numBmts = 0;



	for(i = 0; i < MAX_TEXTURE_BANKS+1; i++)
	{
		textureBanks[i].textures = (BITMAP_TYPE *)calloc(MAX_TEXTURES, sizeof(BITMAP_TYPE));
	}


	//load in object banks-------------------------------------
	//search directory for all files matching 
	sprintf(buf, "%s\\*.bmo", filePath, searchName);

	fileSearch = FindFirstFile(buf, &fData);  
	if(fileSearch == INVALID_HANDLE_VALUE)
	{
		dbprintf("Warning: no Bankmanager files present\n");
		goto done;
	}


	//retrieve rest of matching files
	do
	{
		dbprintf("FileFound: %s\n", fData.cFileName);
		sprintf(objectBanks[numObjectBanks].filename, fData.cFileName);

		//load in bank file
		sprintf(buf, "%s\\%s", filePath, fData.cFileName);
		in = fopen(buf, "rt");
		if(in == NULL)
		{
			dbprintf("Error:could not open file %s\n", buf);
			continue;
//			return;
		}
	
		for(i = 0; i < 1; i++)
		{
			fgets(buf, MAX_PATH, in);	//get header
		}

		ReadString(objectBanks[numObjectBanks].bankFile, in);
		ReadString(objectBanks[numObjectBanks].objectOutput, in);
		ReadString(objectBanks[numObjectBanks].textureOutput, in);
		ReadString(objectBanks[numObjectBanks].lndSource, in);
		ReadString(objectBanks[numObjectBanks].lndDest, in);


		fclose(in);

		objectBanks[numObjectBanks].objectList.numEntries = 0;
		objectBanks[numObjectBanks].objectList.head.next = &(objectBanks[numObjectBanks].objectList.head);
		objectBanks[numObjectBanks].objectList.head.prev = &(objectBanks[numObjectBanks].objectList.head);

		LoadObjectBank(numObjectBanks);

		numObjectBanks++;

	}while(FindNextFile(fileSearch, &fData));



	//load in texture banks-------------------------------------
	//search directory for all files matching 
	sprintf(buf, "%s\\*.bmt", filePath);

	fileSearch = FindFirstFile(buf, &fData);  
	if(fileSearch == INVALID_HANDLE_VALUE)
	{
		dbprintf("Warning: no Bankmanager files present\n");
		goto done;
	}


	//retrieve rest of matching files
	do
	{
		sprintf(textureBanks[numTextureBanks].filename, fData.cFileName);

		sprintf(buf, "%s\\%s", filePath, textureBanks[numTextureBanks].filename);
		in2 = fopen(buf, "rt");
		if(in2 != NULL)
		{
		 	fgets(buf, MAX_PATH, in2);
			fgets(buf, MAX_PATH, in2);
			ReadString(textureBanks[numTextureBanks].destination, in2);
			ReadString(textureBanks[numTextureBanks].exportPath, in2);
		}

		numTextureBanks++;


	}while(FindNextFile(fileSearch, &fData));


done:
	sprintf(missingBmp.fullFilename, "\\\\jamesh\\d\\work\\tools\\bankmanager\\bitmaps\\missing.bmp");
	LoadPicy(&missingBmp);

}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int LoadObjectBank(int num)
{
	FILE *in;
	char buf[MAX_PATH];
	int i;
	OBJECTINFO *ptr;
	int numObjects = 0;

	objectBanks[num].objectList.numEntries = 0;
	objectBanks[num].objectList.head.next = &(objectBanks[num].objectList.head);
	objectBanks[num].objectList.head.prev = &(objectBanks[num].objectList.head);



	in = fopen(objectBanks[num].bankFile, "rt");
	if(in == NULL)
	{
		dbprintf("Error:could not open file %s\n", objectBanks[numObjectBanks].bankFile);
  		return 0;
	}

	for(i = 0; i < 5; i++)
	{
		fgets(buf, MAX_PATH, in);	//skip header
	}


	numObjects = atoi(buf);

	objectBanks[num].objectList.numEntries = 0;
	objectBanks[num].objectList.head.next = &(objectBanks[num].objectList.head);
	objectBanks[num].objectList.head.prev = &(objectBanks[num].objectList.head);

	


	for(i = 0; i < numObjects; i++)
	{
		char *tBuf;

		//read in object name
		fgets(buf, MAX_PATH, in);
		buf[strlen(buf)-1] = 0;

		ptr = (OBJECTINFO *)calloc(sizeof(OBJECTINFO), 1);
		AddObject(&objectBanks[num].objectList, ptr);
		sprintf(ptr->fullFilename, "%s", buf);
		tBuf = buf + strlen(buf);
		while(*(tBuf--) != '\\');
		tBuf += 2;
		sprintf(ptr->filename, "%s", tBuf);


		//read in object flags
		fgets(buf, MAX_PATH, in);
		ptr->drawList = atoi(buf);
	}
	

	return 1;
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteObjectBank(int x)
{
	FILE *fp;
	int i = 0;
	OBJECTINFO *ptr;

//	return;

	if(objectBanks[x].objectList.numEntries == 0)
		return;
	fp = fopen(objectBanks[x].bankFile, "wt");
	if(fp == NULL)
	{
		MessageBox(NULL,objectBanks[x].bankFile,"Error opening file:",MB_ICONWARNING|MB_TASKMODAL);
		return;
	}

	fprintf(fp, "BankMaster_bank_file\n");
	fprintf(fp, ".\n");
	fprintf(fp, ".\n");
	fprintf(fp, "OBJECTS\n");
	fprintf(fp, "%d\n", objectBanks[x].objectList.numEntries);

	for(ptr = objectBanks[x].objectList.head.next;ptr != &objectBanks[x].objectList.head;ptr = ptr->next)
	{
		fprintf(fp, "%s\n", ptr->fullFilename);
		if(ptr->drawList)
			fprintf(fp, "1\n");
		else
			fprintf(fp, "0\n");
	}

	fclose(fp);

}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void SaveObjectBanks(int writeBanks)
{
	int x;
	FILE *fp;
	char buf[256];

	//save object banks
	for(x = 0; x < numObjectBanks; x++)
	{
		if(objectBanks[x].saveMe)
		{
			sprintf(buf, "Saving bank %s\n", objectBanks[x].filename);
			dbprintf(buf);
		
			sprintf(buf, "%s\\%s", filePath, objectBanks[x].filename);
			if(strstr(buf, ".bmo") == NULL)
				strcat(buf, ".bmo");

			fp = fopen(buf,"wt");

			if(fp == NULL)
			{
				MessageBox(NULL,buf,"Error opening file:",MB_ICONWARNING|MB_TASKMODAL);
				return;
			}

			fprintf(fp, "BankMasterObject\n");
			fprintf(fp, "%s\n", objectBanks[x].bankFile);
			fprintf(fp, "%s\n", objectBanks[x].objectOutput);
			fprintf(fp, "%s\n", objectBanks[x].textureOutput);
			fprintf(fp, "%s\n", objectBanks[x].lndSource);
			fprintf(fp, "%s\n", objectBanks[x].lndDest);
			fclose(fp);

			if(writeBanks)
				WriteObjectBank(x);
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
int	GetSaveName(char *saveFileName, char *title, char *filter)
{
	OPENFILENAME	ofn;
	int				i;

	ZeroMemory(&ofn,sizeof(ofn));
	saveFileName[0]=0;
	ofn.lStructSize=sizeof(ofn);
	ofn.hwndOwner=dlgWnd;
	ofn.hInstance=hAppInst;
	ofn.lpstrFilter=filter;//"Nintendo Texture Bank Files (*.ntb)\0*.ntb\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile=saveFileName;    
	ofn.lpstrInitialDir=".";//exportPath;	
	ofn.nMaxFile=4095;
	ofn.lpstrTitle=title; 
	ofn.Flags=OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_EXPLORER|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT; 

	i=GetSaveFileName(&ofn);
	if(i == 0)
		return i;


	if(!(strstr(saveFileName, ".")))
		strcat(saveFileName, ".ntb");

	return i;
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteInt(FILE *fp, int outInt)
{

	fputc((outInt >> 24) & 0xff, fp);
	fputc((outInt >> 16) & 0xff, fp);
	fputc((outInt >> 8) & 0xff, fp);
	fputc(outInt & 0xff, fp);

}
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteShort(FILE *fp, unsigned short outShort)
{
	fputc((outShort >> 8) & 0xff, fp);
	fputc(outShort & 0xff, fp);
}
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteChar(FILE *fp, char outShort)
{
	fputc(outShort, fp);
}



/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
char commandFileName[MAX_PATH];

FILE *GenerateUniqueCommandFile()
{
	char buf[256];
	FILE *fp;
	int i = 0;
	int fileok = 0;

	do
	{
		sprintf(commandFileName, "%s\\command%02d.cmd", commandPath, i);
		fp = fopen(commandFileName, "rt");
		if(fp)
		{
			fileok = 1;
			fclose(fp);
		}
		else
			fileok = 0;
		i++;

	}while(fileok);

	fp = fopen(commandFileName, "wt");
	return fp;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WaitForCommand()
{
	FILE *fp;
	int res;

	do
	{
		Sleep(100);
		res = GetFileAttributes(commandFileName);

	}while(res != -1);
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
BOOL FAR PASCAL WaitForFlynn(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam)
{
	int timerID;

	switch(msg)
	{
		case	WM_COMMAND:
			break;
		case	WM_INITDIALOG:
				timerID = SetTimer(hwnd, NULL, 50,NULL);
			return TRUE;

		case WM_TIMER:
			if(GetFileAttributes(commandFileName) == -1)
			 	EndDialog(hwnd, TRUE);
			break;

				
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
BOOL CheckForValidTextureFormat(char *name)
{
	int sx, sy, bpp, limit;

	if(gelfInfo_BMP(name, &sx, &sy, &bpp))
	{
		if(bpp == 24)
			limit = 4096;
		else
			limit = 2048;

		if((sx == 320) && (sy == 240))
			return TRUE;
		else if((sx == 160) && (sy == 120))
			return TRUE;
		else if(((sx * sy * bpp) / 8) > limit)
			return FALSE;

	}
	else
	{
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
int AddTextures()
{
	FILE *fp;
	char buf[256];
	OPENFILENAME ofn;
	HWND hDlg;
	static char buffer[4096];
	char addPath[256];
	char tempPath[256];
	char * cptr;
	int i, NumFiles;
	OBJECTINFO	*objectPtr;
	
	ZeroMemory(&ofn,sizeof(ofn));
	buffer[0]=0;
    ofn.lStructSize=sizeof(ofn);
    ofn.hwndOwner=hMainWnd;
    ofn.hInstance=hAppInst;


    ofn.lpstrFilter="Bitmap Files (*.bmp)\0*.bmp\0IA Image Files (*.gif)\0*.gif\0"; 
	
	ofn.lpstrFile=buffer;    
	ofn.lpstrInitialDir=".";//objectAddPath;	
	ofn.nMaxFile=4095;
    ofn.lpstrTitle="Add files"; 
    ofn.Flags=OFN_ALLOWMULTISELECT|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_EXPLORER|OFN_HIDEREADONLY; 
    
	i=GetOpenFileName(&ofn);
	if (!i) return 0;

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


//replace c: with name of computer...
	strcpy(tempPath, addPath);
	strlwr(tempPath);
	if(strstr(tempPath, "c:\\") != NULL)
	{
		char *tPtr;
		//artist has added texture from local drive...
		strcpy(tempPath, addPath);
		tPtr = tempPath + 3;
		sprintf(addPath, "\\\\%s\\c\\%s", userName, tPtr);
	}
	if(strstr(tempPath, "d:\\") != NULL)
	{
		char *tPtr;
		//artist has added texture from local drive...
		strcpy(tempPath, addPath);
		tPtr = tempPath + 3;
		sprintf(addPath, "\\\\%s\\d\\%s", userName, tPtr);
	}


//generate new command - must be unique filename

	fp = GenerateUniqueCommandFile();
	if(fp == NULL)
	{
		MessageBox(NULL,buf,"Error opening file",MB_ICONWARNING|MB_TASKMODAL);
		return 0;
	}
	fprintf(fp, "add_textures\n");
	fprintf(fp, "%s\n", textureBanks[selectedTextureBank].filename);
	fprintf(fp, "%d\n", NumFiles);
	for (i=0;*cptr;i++)
	{
		char texName[MAX_PATH];

		sprintf(texName, "%s\\%s", addPath, cptr);
		if(CheckForValidTextureFormat(texName) == FALSE)
		{
			MessageBox(NULL,"Bitmap is of illegal format, buttmunch!","Warning....",MB_ICONWARNING|MB_TASKMODAL);
		}

		dbprintf("%s\\%s\n", addPath, cptr);
		fprintf(fp, "%s\\%s\n", addPath, cptr);

		cptr+=strlen(cptr)+1;
	}
	fclose(fp);

	return 0;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int DeleteTextures()
{
	FILE *fp;
	int res, x;
	char buf[256];
	BITMAP_TYPE *bmp;

	if(mouse.numSelectedTiles == 0)
		return 0;



	for(x = 0; x < mouse.numSelectedTiles; x++)
	{
		bmp = (BITMAP_TYPE *)mouse.selectedTile[x].bitmap;
		res = CountTextureReference(bmp);
//		res = 0;
		if(res == 0)
		{
			fp = GenerateUniqueCommandFile();
			if(fp == NULL)
			{
				MessageBox(NULL,NULL,"Error opening file",MB_ICONWARNING|MB_TASKMODAL);
				return 0;
			}

			fprintf(fp, "delete_textures\n");
			fprintf(fp, "%s\n", textureBanks[selectedTextureBank].filename);
			fprintf(fp, "%d\n", 1);
			fprintf(fp, "%s\n", bmp->name);
			
			fclose(fp);

			DialogBox(hAppInst, "FLYNN", dlgWnd, (DLGPROC)WaitForFlynn);

		}
		else
		{
			sprintf(buf, "The bitmap <%s> is referenced by %d objects!", bmp->name, res);
			if ( MessageBox(dlgWnd, buf ,"No you don't naughty artist!",MB_ICONQUESTION|MB_TASKMODAL|MB_OKCANCEL) == IDOK )
			{
				fp = GenerateUniqueCommandFile();
				if(fp == NULL)
				{
					MessageBox(NULL,NULL,"Error opening file",MB_ICONWARNING|MB_TASKMODAL);
					return 0;
				}

				fprintf(fp, "delete_textures\n");
				fprintf(fp, "%s\n", textureBanks[selectedTextureBank].filename);
				fprintf(fp, "%d\n", 1);
				fprintf(fp, "%s\n", bmp->name);
				
				fclose(fp);

				DialogBox(hAppInst, "FLYNN", dlgWnd, (DLGPROC)WaitForFlynn);

			}
			// ENDIF
		}

	}
	RefreshTextureBank(FALSE);
	numObjectBitmaps = 0;

	ClearMouseSelection();
	return 1;

}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void ChangeCyclingSpeed(BITMAP_TYPE *bmp)
{
	FILE *fp;


	fp = GenerateUniqueCommandFile();
	if(fp == NULL)
	{
		MessageBox(NULL,NULL,"Error opening file",MB_ICONWARNING|MB_TASKMODAL);
		return;
	}

	fprintf(fp, "adjust_texture\n");
	fprintf(fp, "%s\n", textureBanks[selectedTextureBank].filename);
	fprintf(fp, "%d\n", 1);
	fprintf(fp, "%s\n", bmp->name);
	fprintf(fp, "%d\n", bmp->cycleSpeed);
	fprintf(fp, "%d\n", bmp->cycleStart);
	fprintf(fp, "%d\n", bmp->cycleEnd);
	
	fclose(fp);


}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void ChangeTextureBankInfo(char *origName, char *newName)
{
	FILE *fp;


	fp = GenerateUniqueCommandFile();
	if(fp == NULL)
	{
		MessageBox(NULL,NULL,"Error opening file",MB_ICONWARNING|MB_TASKMODAL);
		return;
	}

	fprintf(fp, "modify_texturebank\n");
	fprintf(fp, "%s\n", origName);
	fprintf(fp, "%d\n", 3);
	fprintf(fp, "%s\n", textureBanks[selectedTextureBank].destination);
	fprintf(fp, "%s\n", textureBanks[selectedTextureBank].exportPath);
	fprintf(fp, "%s\n", newName);
	
	fclose(fp);


}



/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void RefreshTextureBank(int removeOffset)
{
	KillTextureBank(selectedTextureBank, removeOffset);
	LoadTextureBank(selectedTextureBank, NULL);
	InvalidateRect(dlgWnd, NULL, FALSE);

	UpdateTextureStatusBar();
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int BrowseForFile(char *title, char *filter, char *name)
{
	char buf[256];
	OPENFILENAME ofn;
	static char buffer[4096];
	char addPath[256];
	char * cptr;
	int i, NumFiles;
	
	ZeroMemory(&ofn,sizeof(ofn));
	buffer[0]=0;
    ofn.lStructSize=sizeof(ofn);
    ofn.hwndOwner=dlgWnd;
    ofn.hInstance=hAppInst;

    ofn.lpstrFilter=filter;//"Bank filess (*.bnk)\0*.bnk\0"; 
	
	ofn.lpstrFile=buffer;    
	ofn.lpstrInitialDir=".";//objectAddPath;	
	ofn.nMaxFile=4095;
    ofn.lpstrTitle="Select bank file"; 
    ofn.Flags=OFN_CREATEPROMPT |/*OFN_FILEMUSTEXIST|*/OFN_PATHMUSTEXIST|OFN_EXPLORER|OFN_HIDEREADONLY; 
    
	i=GetOpenFileName(&ofn);
	if (!i)
	{
		return 0;
	}

	strcpy(name, buffer);
	return 1;	
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int BrowseForDirectory(HWND hwnd, char *title, char *name)
{
	BROWSEINFO	bInfo;

	ITEMIDLIST	*idlist;
	ITEMIDLIST  *pidl;

	char *folder = ".";

	idlist = (ITEMIDLIST*)malloc(sizeof(ITEMIDLIST)+strlen(folder));

	idlist->mkid.cb = strlen(folder)+3;
	strcpy((char *)(idlist->mkid.abID), folder);

									 
	memset(&bInfo, 0, sizeof(BROWSEINFO));
	bInfo.hwndOwner = hwnd;
	bInfo.pidlRoot = NULL;//idlist;//NULL;
	bInfo.pszDisplayName = name;
	bInfo.lpszTitle = title;
    bInfo.ulFlags = BIF_RETURNONLYFSDIRS;

    pidl = SHBrowseForFolder(&bInfo);

    if (pidl)
    {
        SHGetPathFromIDList(pidl, name);
		return 1;
    }
	else
		return 0;


}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
char *textureRoot = "\\\\dwood\\c\\temp\\*.";
BOOL FAR PASCAL MyBrowseForDirectory(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam)
{
	char buf[MAX_PATH];
	char buf2[MAX_PATH];
	static int depth = 0;
	switch(msg)
	{
		case	WM_COMMAND:
			if(LOWORD(wParam) == IDOK)
			{
			 	EndDialog(hwnd, TRUE);
			}
			if(LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hwnd, TRUE);
			}
			if(LOWORD(wParam) == IDC_MAKEDIR)
			{
				EndDialog(hwnd, TRUE);
			}

			if(HIWORD(wParam) == LBN_SELCHANGE)
			{
				switch(LOWORD(wParam))
				{
					case IDC_DIRLIST:
						DlgDirSelectEx(hwnd, buf, MAX_PATH, IDC_DIRLIST);
						if(strcmp(buf, "..\\") == 0)
						{
							if(depth > 0)
							{
								if(DlgDirList(hwnd, buf, IDC_DIRLIST, 0, DDL_DIRECTORY))
									depth--;
							}
						}
						else
						{
							if(DlgDirList(hwnd, buf, IDC_DIRLIST, 0, DDL_DIRECTORY))
								depth++;
						}
						_getcwd(buf2, MAX_PATH);
//						sprintf(buf2, "%d:%s", depth, buf);
						SetDlgItemText(hwnd, IDC_DIRTEXT, buf2);

						break;
				}
			}


			break;
		case	WM_INITDIALOG:
				strcpy(buf, textureRoot);
				DlgDirList(hwnd, textureRoot, IDC_DIRLIST, 0, DDL_DIRECTORY);
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
int CountFilesInDirectory(char *buf)
{
	HANDLE fileSearch;
	WIN32_FIND_DATA fData;
	int numFiles = 0;

	fileSearch = FindFirstFile(buf, &fData);  
	if(fileSearch == INVALID_HANDLE_VALUE)
	{
		dbprintf("Warning: no Bankmanager files present\n");
		return 0;
	}


	//retrieve rest of matching files
	do
	{
		dbprintf("FileFound: %s\n", fData.cFileName);
		numFiles++;
	}while(FindNextFile(fileSearch, &fData));

	return numFiles;

}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void ReadConfigFile()
{
	FILE *fp;
	char buf[256];
	int i;

	fp = fopen("c:\\windows\\bankmanager.cfg", "rt");
	if ( fp == NULL )
		fp = fopen("d:\\windows\\bankmanager.cfg", "rt");
	// ENDIF
	if(fp == NULL)
	{
		//no config file
		sprintf(includeFiles[0], "..\\types.h");
		sprintf(commandPath, "\\\\jamesh\\d\\work\\tools\\commands");
		sprintf(filePath, "X:\\TeamSpirit\\GameCode\\Graphics\\BankmanagerStuff");
		sprintf(recentAddPath, ".");
		numIncludeFiles = 1;
	}
	else
	{
		ReadString(buf, fp);
		numIncludeFiles = atoi(buf);
		for(i = 0; i < numIncludeFiles; i++)
		{
			ReadString(includeFiles[i], fp);
		}
		ReadString(commandPath, fp);
		ReadString(filePath, fp);
		ReadString(recentAddPath, fp);
		fclose(fp);
 	}

}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteConfigFile()
{
	FILE *fp;
	int i;

	fp = fopen("c:\\windows\\bankmanager.cfg", "wt");
	if(fp == NULL)
		return;

	fprintf(fp, "%d\n", numIncludeFiles);
	for(i = 0; i < numIncludeFiles; i++)
	{
		fprintf(fp, "%s\n", includeFiles[i]);
	}
	fprintf(fp, "%s\n", commandPath);
	fprintf(fp, "%s\n", filePath);
	fprintf(fp, "%s\n", recentAddPath);
	fclose(fp);
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CopyLNDFile(int bankNum)
{
	char dest[MAX_PATH];
	char *sPtr;

	if((strlen(objectBanks[bankNum].lndSource)) && (strlen(objectBanks[bankNum].lndDest)))
	{
		sPtr = objectBanks[bankNum].lndSource;
		sPtr += strlen(sPtr);
		while(*sPtr != '\\')
			sPtr--;

		sPtr++;
		
		sprintf(dest, "%s\\%s", objectBanks[bankNum].lndDest, sPtr);

		
		if(CopyFile(objectBanks[bankNum].lndSource, dest, FALSE) == FALSE)
			MessageBox(dlgWnd,"There was an error copying LND file... :(","Error",MB_ICONERROR|MB_TASKMODAL);
	}

}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
BOOL FileExists(char *name)
{
	FILE *fp;

	fp = fopen(name, "rb");
	if(fp)
	{
		fclose(fp);
		return TRUE;
	}

	return FALSE;
}