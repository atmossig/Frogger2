#include <windows.h>
#include <commctrl.h>
#include <wingdi.h>
#include <string.h>
#include <stdio.h>

#include "types.h"
#include "main.h"
#include "drawlists.h"
#include "bitmaps.h"
#include "files.h"
#include "views.h"
#include "obe.h"
#include "crc32.h"
#include "ndos.h"
#include "resource.h"
#include "commctrl.h"
#include "debug.h"
#include <gelf.h>

HINSTANCE hAppInst;
HACCEL	winAccel;
HWND hMainWnd;
HWND hSBarWnd;
HWND hTBarWnd;
HWND hViewWnd; 
HWND hObjWnd;
HWND hTreeWnd;

HWND hWorkingWnd;

HFONT	infoFont;
HBITMAP backpic;

HWND dlgWnd;
char userName[MAX_PATH];

RECT objectTextureRect = {485, 11, 713, 162};
RECT textureRect = {236, 200, 540, 352};

char uniqueUserFilename[] = "hermit.crab";
char includeFiles[10][MAX_PATH];
char numIncludeFiles;

int user = SPIRIT;
int userFormat = N64;

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hNull,LPSTR CmdLine,int sw)
{
	int i;

	hAppInst = hInstance;

	if(strlen(CmdLine))
	{
		userFormat = PC;
		MessageBox(NULL,"Configured for PC output","BankManager v2.0",MB_OK);
	}
	else
	{
		userFormat = N64;
		MessageBox(NULL,"Configured for N64 output","BankManager v2.0",MB_OK);
	}

	//can only allow one user at a time, cos I'm not prepared to lose my sanity over this
	//program
	ReadConfigFile();

	if(AmITheOnlyUser() == FALSE)
		return FALSE;



	InitCRCTable();
	gelfInit();
	LoadDefaults();

	InitCommonControls();
	hMainWnd = CreateDialog(hAppInst, "MAINDLG", NULL, (DLGPROC)mainDlgProc);

	winAccel = LoadAccelerators(hAppInst, "ACCEL");

//	RegisterWindowClasses();
//	i=InitGlobals();	

//	InitCRCTable();	//init crc table used for storing texture names as numbers

//	ReadConfigFile();
//	InitLists();

/*	if(strlen(CmdLine))
	{
		sprintf(autoLoadName, "%s", CmdLine);
		autoLoad = TRUE;
	}
*/		
		
			
//	if (!i) return -1;
/*
	hMainWnd=CreateWindow("MAINWNDCLASS", "BankManager - take a seat...",
		WS_OVERLAPPEDWINDOW,10,10,480,300,
		NULL, NULL, hAppInst, NULL);
*/

	ShowWindow(hMainWnd, SW_SHOWDEFAULT);
	UpdateWindow(hMainWnd);

	i=MessageLoop();	

	FreeGlobals();
	
	WriteConfigFile();
	gelfShutdown();	
	return i;
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int MessageLoop()
{
	MSG msg;

	for(;;)
	{
		if (PeekMessage(&msg, NULL, 0, 0,PM_REMOVE))
		{

			if (!TranslateAccelerator(hMainWnd, winAccel, &msg))
			{
				if (msg.message == WM_QUIT)
					break;
				TranslateMessage(&msg); 
				DispatchMessage(&msg);
			}


		}
		else
		{
			WaitMessage();
		}
	}

	return 0;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
LRESULT CALLBACK MainWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
	int y,u;

	switch (message)
	{
		case WM_CREATE:



//			hSBarWnd = CreateStatusWindow(WS_CHILD|WS_VISIBLE,"Ready",hWnd,1);

/*			hTBarWnd = CreateMyToolbar(hWnd);
			SendMessage(hTBarWnd,WM_SIZE,0,0);
			SendMessage(hSBarWnd,WM_SIZE,0,0);
			hViewWnd = CreateWindowEx(WS_EX_CLIENTEDGE,"VIEWWNDCLASS",NULL,
				WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN,0,0,10,10, hWnd, NULL, hAppInst, NULL);


			GetWindowRect(hWnd, &rc);
			hObjWnd = CreateWindowEx(WS_EX_CLIENTEDGE, "OBJECTWNDCLASS",NULL,
				WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,rc.right-rc.left - 200 - 30,0,
				200,200, hViewWnd, NULL, hAppInst, NULL);

 */			

			infoFont = CreateFont(
				16,
				0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
				ANSI_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				VARIABLE_PITCH,
				"Arial" );

/*			if(autoLoad)
			{
					LoadBankFile();
					SetAppState();			
					InvalidateRect(hViewWnd,NULL,FALSE);					
					RefreshInfoBar(hWnd);

					if((mode == TEXTURES) && (PicyList.numEntries > 0))
						ExportAllPicys();
					if((mode == OBJECTS) && (ObjectList.numEntries > 0))
					{
						StoreOptions();
						ExportDrawList();
					}
					DestroyWindow(hWnd);
			}
*/
			return 0;			
		case WM_SIZE:	
			SendMessage(hSBarWnd,WM_SIZE,0,0);
			SendMessage(hTBarWnd,WM_SIZE,0,0);
			GetWindowRect(hSBarWnd,&rc);
			y=rc.bottom-rc.top;
			GetWindowRect(hTBarWnd,&rc);
			u=rc.bottom-rc.top;
			MoveWindow(hViewWnd,0,u,LOWORD(lParam),HIWORD(lParam)-y-u,TRUE);			
//			RefreshInfoBar(hWnd);

			return 0;

		case WM_INITMENU:
//			SetAppState();			
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case 0:
					break;
				default:
					break;
			}
			break;
		case WM_CLOSE:
			DestroyWindow(hWnd);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		
	}
	return DefWindowProc(hWnd,message,wParam,lParam);
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

BOOL FAR PASCAL mainDlgProc(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam)
{
	char	buf[256];
	int x = 1;
	NMHDR		*nmh;
	PAINTSTRUCT ps;
	static int xPos, yPos;
	HWND	tempWnd;


	dlgWnd = hwnd;

	switch(msg)
	{
		case WM_LBUTTONDOWN:
		case WM_MOUSEMOVE:
			mouse.xPos = LOWORD(lParam);  // horizontal position of cursor 
			mouse.yPos = HIWORD(lParam);  // vertical position of cursor 
			mouse.flags = wParam; 
			UpdateMouse();

			break;
		case WM_LBUTTONDBLCLK:
			if((mouse.selectedTile[0].tileValid) && (mouse.selectedTile[0].window == TEXTURE_WINDOW))
			{
				DialogBox(hAppInst, "BITMAPPROPERTIES", dlgWnd, (DLGPROC)BitmapProperties);
			}
			break;

		case	WM_COMMAND:
			if(LOWORD(wParam) == IDOK)
			{
			 	EndDialog(hwnd, TRUE);
			}
			if(LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hwnd, FALSE);
			}

			//bitmap scroll controls
			if(LOWORD(wParam) == IDC_OBJTEXDN)
			{
				if(numObjectBitmaps)
				{
					if(objectTexturesOffset <= (numObjectBitmaps - OBJECT_TEXTURES_X * OBJECT_TEXTURES_Y))
					{
						objectTexturesOffset += OBJECT_TEXTURES_X;
//						iRect.left = 400;iRect.right = 630;iRect.top = 30;iRect.bottom = 180;
						InvalidateRect(hwnd,&objectTextureRect,FALSE);
					}
				}
			}
			if(LOWORD(wParam) == IDC_OBJTEXUP)
			{
				if(numObjectBitmaps)
				{
					if(objectTexturesOffset > 0)
					{
						objectTexturesOffset -= OBJECT_TEXTURES_X;
						InvalidateRect(hwnd,&objectTextureRect,FALSE);
					}
				}
			}
			if(LOWORD(wParam) == IDC_TEXDN)
			{
				if(selectedTextureBank != -1)
				{
					if(textureBankOffset < (textureBanks[selectedTextureBank].numTextures - TEXTURE_BANK_X * TEXTURE_BANK_Y))
					{				
						textureBankOffset += TEXTURE_BANK_X;
						InvalidateRect(hwnd,&textureRect,FALSE);
//						ClearMouseSelection();
					}
				}
			}
			if(LOWORD(wParam) == IDC_TEXUP)
			{
				if(selectedTextureBank != -1)
				{
					if(textureBankOffset > 0)
					{
						textureBankOffset -= TEXTURE_BANK_X;
						InvalidateRect(hwnd,&textureRect,FALSE);
//						ClearMouseSelection();
					}
				}
			}

			if(LOWORD(wParam) == IDC_EXPORT)
			{
				if(selectedObjectBank != -1)
				{
					if(objectBanks[selectedObjectBank].bankFile[0] == 0)// || (objectBanks[selectedObjectBank].objectOutput[0] == 0) || (objectBanks[selectedObjectBank].textureOutput[0] == 0))
//					if((objectBanks[selectedObjectBank].bankFile[0] == 0) || (objectBanks[selectedObjectBank].objectOutput[0] == 0) || (objectBanks[selectedObjectBank].textureOutput[0] == 0))
						MessageBox(NULL,"Bank properties must be set correctly","Error",MB_ICONWARNING|MB_TASKMODAL);
					else
					{
						if(objectBanks[selectedObjectBank].objectList.numEntries)
						{
							ExportObjectTextureBank(selectedObjectBank);
							ExportDrawList(selectedObjectBank);
							CopyLNDFile(selectedObjectBank);
						}
					}
				}
			}
			if(LOWORD(wParam) == IDNEWOBJBANK)
			{
				selectedObjectBank = numObjectBanks;
				DialogBox(hAppInst, "OBJBANKPROPERTIES", dlgWnd, (DLGPROC)NewObjBankProperties);
			}


			if(LOWORD(wParam) == IDC_EXPORTTEX)
			{
				if(selectedTextureBank != -1)
				{
					ExportTextureBank(selectedTextureBank);
				}
			}

			if(LOWORD(wParam) == IDC_UNUSEDTEX)
			{
				if(selectedTextureBank != -1)
				{
					LoadAllNDOS();
					ShowUnusedTextures(selectedTextureBank);
				}
			}
			
			if(LOWORD(wParam) == IDC_OBJBANKPROPERTIES)
			{
				if(selectedObjectBank != -1)
				{
					DialogBox(hAppInst, "OBJBANKPROPERTIES", dlgWnd, (DLGPROC)ObjBankProperties);
				}
			}

			if(LOWORD(wParam) == IDC_TEXPROPERTIES)
			{
				if(selectedTextureBank != -1)
				{
					DialogBox(hAppInst, "TEXBANKPROPERTIES", dlgWnd, (DLGPROC)TexBankProperties);
				}
			}
			if(LOWORD(wParam) == IDC_NEWTEXBANK)
			{
				selectedTextureBank = numTextureBanks;
				DialogBox(hAppInst, "TEXBANKPROPERTIES", dlgWnd, (DLGPROC)TexBankProperties);
			}

/*
			if(LOWORD(wParam) == IDC_TEXPROPERTIES)
			{
				if((mouse.selectedTile[0].tileValid) && (mouse.selectedTile[0].window == TEXTURE_WINDOW))
				{
					DialogBox(hAppInst, "BITMAPPROPERTIES", dlgWnd, (DLGPROC)BitmapProperties);
				}
			}
*/
			if(LOWORD(wParam) == IDC_ADDTEX)
			{
				if(selectedTextureBank != -1)
				{
					AddTextures();
					DialogBox(hAppInst, "FLYNN", hwnd, (DLGPROC)WaitForFlynn);
//					WaitForCommand();
					RefreshTextureBank(FALSE);
				}
			}
			if(LOWORD(wParam) == IDC_DELTEX)
			{
				if(selectedTextureBank != -1)
				{

					for(int x = 0; x < numObjectBanks; x++)
					{
						if(objectBanks[x].loaded == 0)
							LoadNDOSFromBank(x);
					}

					if(MessageBox(dlgWnd,"Delete textures?","Are you sure?",MB_ICONQUESTION|MB_TASKMODAL|MB_YESNO) == IDYES)
						DeleteTextures();
				}
			}
			if(LOWORD(wParam) == IDC_REFRESHTEX)
			{
				if(selectedTextureBank != -1)
				{
					RefreshTextureBank(TRUE);
				}
			}

			if(LOWORD(wParam) == IDC_LOADALLBANKS)
			{
				for(x = 0; x < numTextureBanks; x++)
					if(textureBanks[x].numTextures == 0)
						LoadTextureBank(x, textureBanks[x].filename);
			}

			if(LOWORD(wParam) == IDC_ADDNDO)
			{
				if(selectedObjectBank != -1)
				{
					AddNDOToBank();					
					selectedObject = -1;
					FillObjList();
					InitTreeWindow();
					objectBanks[selectedObjectBank].saveMe = TRUE;
				}
			}						
			if(LOWORD(wParam) == IDC_DELNDO)
			{
				if(selectedObject != -1)
				{
					FreeOBE(currentObject);
					SubObject(&objectBanks[selectedObjectBank].objectList, currentObject);
					selectedObject = -1;
					FillObjList();
					InitTreeWindow();
					objectBanks[selectedObjectBank].saveMe = TRUE;

				}
			}

			if(LOWORD(wParam) == IDC_CONFIGURE)
			{
				DialogBox(hAppInst, "CONFIGURE", dlgWnd, (DLGPROC)Config);
			}




			//user clicked on an entry in the object list
			if(HIWORD(wParam) == LBN_SELCHANGE)
			{
				switch(LOWORD(wParam))
				{
					case IDC_LIST1:
//						FreeNDOSFromBank(selectedObjectBank);
						selectedObjectBank = SendDlgItemMessage(hwnd,IDC_LIST1, LB_GETCURSEL, 0, 0);
						if(objectBanks[selectedObjectBank].loaded == 0)
							LoadNDOSFromBank(selectedObjectBank);

						CreateTreeWindow();//kill object tree
						FillObjList();
						GetObjectBitmapsFromBank(selectedObjectBank);
						InvalidateRect(hwnd,NULL,FALSE);
						ShowWindow(GetDlgItem(hwnd, IDC_TREE1), SW_SHOWDEFAULT);
//						ShowWindow(GetDlgItem(hwnd, IDC_DRAWLIST), SW_SHOWDEFAULT);
						ShowWindow(GetDlgItem(hwnd, IDC_DLTEXT), SW_SHOWDEFAULT);
						ShowWindow(GetDlgItem(hwnd, IDC_CHECK2), SW_SHOWDEFAULT);
						SetDlgItemText(dlgWnd,IDC_POLYCOUNT,(LPSTR)"");
						break;
					case IDC_LIST2:
						selectedTextureBank = SendDlgItemMessage(hwnd,IDC_LIST2, LB_GETCURSEL, 0, 0);
						if(textureBanks[selectedTextureBank].numTextures == 0)
							LoadTextureBank(selectedTextureBank, textureBanks[selectedTextureBank].filename);
						textureBankOffset = 0;
						UpdateTextureStatusBar();
						InvalidateRect(hwnd,NULL,FALSE);
						break;
					//must fill tree with current object stuff
					case IDC_OBJLIST:
						//store drawlist state
						char tmp[32];

						if(currentObject)
						{
							if(SendDlgItemMessage(dlgWnd,IDC_CHECK2, BM_GETCHECK, 0, 0) == BST_CHECKED)
								AddObjectFlags(currentObject->object, OBJECT_FLAGS_CHANGEDRAWLIST);
							else
								StripObjectFlags(currentObject->object, OBJECT_FLAGS_CHANGEDRAWLIST);
						}	
						selectedObject = SendDlgItemMessage(hwnd,IDC_OBJLIST, LB_GETCURSEL, 0, 0);
						currentObject = objectBanks[selectedObjectBank].objectList.head.next;
						for(int i = 0; i < selectedObject; i++)
						{
							currentObject = currentObject->next;
						}
						SendDlgItemMessage(dlgWnd,IDC_CHECK2, BM_SETCHECK, (currentObject->object->flags & OBJECT_FLAGS_CHANGEDRAWLIST) ? BST_CHECKED : BST_UNCHECKED, 0);

						sprintf(tmp,"%d faces",currentObject->object->mesh->numFaces);
						SetDlgItemText(dlgWnd,IDC_POLYCOUNT,(LPSTR)tmp);

						InitTreeWindow();
						break;
				}
			}
			break;									   

		case	WM_INITDIALOG: 
			SendDlgItemMessage(hwnd,IDC_CHECK1, BM_SETCHECK, BST_CHECKED, 0);
			for(x = 0; x < numObjectBanks; x++)
			{
				SendDlgItemMessage(hwnd,IDC_LIST1, LB_INSERTSTRING, x, (long)objectBanks[x].filename);
			}					

			for(x = 0; x < numTextureBanks; x++)
			{
				SendDlgItemMessage(hwnd,IDC_LIST2, LB_INSERTSTRING, x, (long)textureBanks[x].filename);
			}					

 			SendDlgItemMessage(hwnd,IDC_TEXUP, BM_SETIMAGE, IMAGE_BITMAP, (long)LoadBitmap(hAppInst, MAKEINTRESOURCE(IDB_UP)));
 			SendDlgItemMessage(hwnd,IDC_TEXDN, BM_SETIMAGE, IMAGE_BITMAP, (long)LoadBitmap(hAppInst, MAKEINTRESOURCE(IDB_DOWN)));
 			SendDlgItemMessage(hwnd,IDC_OBJTEXUP, BM_SETIMAGE, IMAGE_BITMAP, (long)LoadBitmap(hAppInst, MAKEINTRESOURCE(IDB_UP)));
 			SendDlgItemMessage(hwnd,IDC_OBJTEXDN, BM_SETIMAGE, IMAGE_BITMAP, (long)LoadBitmap(hAppInst, MAKEINTRESOURCE(IDB_DOWN)));

			SetUserFace(hwnd);
			
			return TRUE;


		case WM_CLOSE:
			CleanUp();
			DestroyWindow(hwnd);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
				


//tree view stuff

/*		case WM_NOTIFY:
			nmh = (NMHDR*)lParam;
			if (nmh->hwndFrom == objectexplorer.treehandle)
				return HandleTreeNotification(nmh);
			break;
*/


		case WM_NOTIFY:
			nmh = (NMHDR*)lParam;
//			if (nmh->hwndFrom == objectexplorer.treehandle)
				return HandleTreeNotification(nmh);
			break;




		case WM_PAINT:			
			BeginPaint(hwnd,&ps);

//			iRect.left = 397;iRect.right = 625;iRect.top = 11;iRect.bottom = 162;
			FillRect(ps.hdc,&objectTextureRect/*ps.rcPaint*/,(HBRUSH)(COLOR_3DFACE+2));

//			iRect.left = 236;iRect.right = 542;iRect.top = 200;iRect.bottom = 352;
			FillRect(ps.hdc,&textureRect/*ps.rcPaint*/,(HBRUSH)(COLOR_3DFACE+2));



			DrawObjectTextures(hwnd, &ps);
			DrawTextureBankTextures(hwnd, &ps);

			EndPaint(hwnd,&ps);

   //			PrintObjectStats();

			return 0;





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
void SetUserFace(HWND hwnd)
{
	if(strcmp(userName, "dwood") == 0)
	 	SendDlgItemMessage(hwnd,IDC_USERFACE, BM_SETIMAGE, IMAGE_BITMAP, (long)LoadBitmap(hAppInst, MAKEINTRESOURCE(IDB_WOODY)));
	if(strcmp(userName, "dflynn2") == 0)
	 	SendDlgItemMessage(hwnd,IDC_USERFACE, BM_SETIMAGE, IMAGE_BITMAP, (long)LoadBitmap(hAppInst, MAKEINTRESOURCE(IDB_FLYNN)));
	if(strcmp(userName, "dmanuel") == 0)
	 	SendDlgItemMessage(hwnd,IDC_USERFACE, BM_SETIMAGE, IMAGE_BITMAP, (long)LoadBitmap(hAppInst, MAKEINTRESOURCE(IDB_DAVE)));
	if(strcmp(userName, "jsteele") == 0)
	 	SendDlgItemMessage(hwnd,IDC_USERFACE, BM_SETIMAGE, IMAGE_BITMAP, (long)LoadBitmap(hAppInst, MAKEINTRESOURCE(IDB_JIM)));
	if(strcmp(userName, "nadams") == 0)
	 	SendDlgItemMessage(hwnd,IDC_USERFACE, BM_SETIMAGE, IMAGE_BITMAP, (long)LoadBitmap(hAppInst, MAKEINTRESOURCE(IDB_NICK)));
	if(strcmp(userName, "npettitt") == 0)
	 	SendDlgItemMessage(hwnd,IDC_USERFACE, BM_SETIMAGE, IMAGE_BITMAP, (long)LoadBitmap(hAppInst, MAKEINTRESOURCE(IDB_NEIL)));
	if(strcmp(userName, "sbond") == 0)
	 	SendDlgItemMessage(hwnd,IDC_USERFACE, BM_SETIMAGE, IMAGE_BITMAP, (long)LoadBitmap(hAppInst, MAKEINTRESOURCE(IDB_STEVE)));
	if(strcmp(userName, "shayes") == 0)
	 	SendDlgItemMessage(hwnd,IDC_USERFACE, BM_SETIMAGE, IMAGE_BITMAP, (long)LoadBitmap(hAppInst, MAKEINTRESOURCE(IDB_SIMON)));

}
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CleanUp()
{
	int x, i;
	char buf[MAX_PATH];
	FILE *fp;

	SaveObjectBanks(TRUE);

	sprintf(buf, "%s\\%s", commandPath, uniqueUserFilename);
	DeleteFile(buf);

}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
BOOL AmITheOnlyUser()
{
	FILE *fp;
	char buf[MAX_PATH];

	return TRUE;

	sprintf(buf, "%s\\%s", commandPath, uniqueUserFilename);
	fp = fopen(buf, "rt");
	if(fp)
	{
		dbprintf("OOps, other user present!");
		fgets(userName, MAX_PATH, fp);
		fclose(fp);
		DialogBox(hAppInst, "ONEUSER", dlgWnd, (DLGPROC)OnlyUser);
		return FALSE;
 	}
	else
	{
		fp = fopen(buf, "wt");
		unsigned long size = MAX_PATH;
		if(fp)
		{
			GetComputerName(userName, &size);
			strlwr(userName);
	//		GetUserName(userName, &size);
			fprintf(fp, userName);
			fclose(fp);
			return TRUE;
		}
		else
		{
			
			MessageBox(dlgWnd,"Specified command path appears to be invalid","Error:",MB_ICONERROR|MB_TASKMODAL);
			return FALSE;
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
BOOL FAR PASCAL OnlyUser(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam)
{
	int timerID;

	switch(msg)
	{
		case	WM_COMMAND:
			if(LOWORD(wParam) == IDC_USERFACE)
			{
				EndDialog(hwnd, FALSE);
			}
			break;
		case	WM_INITDIALOG:
			SetUserFace(hwnd);
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
void SetupProgressBar(int start, int end, char *text)
{
	if(dlgWnd)
	{
		SendDlgItemMessage(dlgWnd,IDC_PROGRESS1,PBM_SETRANGE,0,MAKELPARAM(0, end * 10));
		SetDlgItemText(dlgWnd, IDC_PROGTEXT, text);
	}
	else
	{
		hWorkingWnd=CreateDialog(hAppInst,"WORKING",hMainWnd,NULL);//WorkingProc);
		ShowWindow(hWorkingWnd,SW_SHOW);
		SendDlgItemMessage(hWorkingWnd,IDC_PROGRESS1,PBM_SETPOS,0,0);
		UpdateWindow(hWorkingWnd);

		SendDlgItemMessage(hWorkingWnd,IDC_PROGRESS1,PBM_SETRANGE,0,MAKELPARAM(0, end * 10));
		SetDlgItemText(hWorkingWnd, IDC_PROGTEXT, text);
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void StepProgressBar()
{
//	SendDlgItemMessage(hWorkingWnd,IDC_PROGRESS1,PBM_STEPIT,0,0);
	if(dlgWnd)
		SendDlgItemMessage(dlgWnd,IDC_PROGRESS1,PBM_STEPIT,0,0);
	else
		SendDlgItemMessage(hWorkingWnd,IDC_PROGRESS1,PBM_STEPIT,0,0);
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void KillProgressBar()
{
	if(dlgWnd)
	{
		SendDlgItemMessage(dlgWnd,IDC_PROGRESS1,PBM_SETPOS,0,0);
		SetDlgItemText(dlgWnd, IDC_PROGTEXT, "");
	}
	else
	{
		DestroyWindow(hWorkingWnd);
		SetDlgItemText(hWorkingWnd, IDC_PROGTEXT, "");
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
BOOL FAR PASCAL Config(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam)
{
	char bankBuf[MAX_PATH];
	char cmpBuf[MAX_PATH];

	switch(msg)
	{
		case	WM_COMMAND:
			if(LOWORD(wParam) == IDOK)
			{
				GetDlgItemText(hwnd, IDC_CMDPATH, cmpBuf, MAX_PATH);
				if(strcmp(cmpBuf, commandPath))
				{
					//if command path has changed, remove old hermit.crab					
					sprintf(bankBuf, "%s\\%s", commandPath, uniqueUserFilename);
					DeleteFile(bankBuf);

					//then write out new crab
					strcpy(commandPath, cmpBuf);
					AmITheOnlyUser();
				}
				//must remove unique id file, and replace it in new commandpath
				GetDlgItemText(hwnd, IDC_BANKPATH, cmpBuf, MAX_PATH);
				if(strcmp(cmpBuf, filePath))
				{
					strcpy(filePath, cmpBuf);
					MessageBox(hwnd,"You must exit and restart Bankmanager to load files from the new location.","Warning",MB_ICONWARNING|MB_TASKMODAL);
				}

				EndDialog(hwnd, FALSE);
			}
			if(LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hwnd, FALSE);
			}
			if(LOWORD(wParam) == IDC_BANKBROWSE)
			{
				if(BrowseForDirectory(hwnd, "Select bank file directory", bankBuf))
					SetDlgItemText(hwnd, IDC_BANKPATH, bankBuf);
					
			}
			if(LOWORD(wParam) == IDC_CMDBROWSE)
			{
				if(BrowseForDirectory(hwnd, "Select command path", bankBuf))
					SetDlgItemText(hwnd, IDC_CMDPATH, bankBuf);
			}
			break;
		case	WM_INITDIALOG:
			SetDlgItemText(hwnd, IDC_BANKPATH, filePath);
			SetDlgItemText(hwnd, IDC_CMDPATH, commandPath);
			return TRUE;
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
void FreeGlobals()
{
	int i;

	for(i = 0; i < MAX_TEXTURE_BANKS+1; i++)
	{
		free(textureBanks[i].textures);
	}
}