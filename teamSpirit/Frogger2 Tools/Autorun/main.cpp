
#include "language.h"
#include <windows.h>
#include "resource.h"
#include "main.h"

HINSTANCE hinstance;

HBITMAP hbmp, hmask, hOpt[3];
HWND	hwndMain;

#define WINDOWCLASS "Frogger2autorun"

#define BMPWIDTH		357
#define BMPHEIGHT		363
#define BMPOPTWIDTH		88
#define BMPOPTHEIGHT	23
#define IDC_INSTALL		1001

#define COLOUR_TEXTSEL	RGB(255,228,0)
#define COLOUR_TEXTDROP	RGB(3,132,0)
#define COLOUR_TEXT		RGB(150,255,0)

POINT textPositions[3] = { {178, 218}, {178, 255}, {178,292} };

RECT  buttons[3] = { {50,200,308,234}, {50,240,308,272}, {50,278,308,310} };

//char options[3][40] = { "Play", "Setup", "Close" };

char *languages[5] = { "English", "Français", "Deutsch", "Italiano", "Espanõl" };

char baseDirectory[MAX_PATH] = "";

int selection = -1, installed = 0;

const char* REGISTRY_KEY	= "Software\\Hasbro Interactive\\Frogger2";
const char* FROGGER2		= "Frogger2 - Swampy's Revenge";
const char* SETUP_EXE		= "Frogger2\\setup.exe";
const char* FROGGER2_EXE	= "Frogger2.exe";

// -------------------------------------------------------------------

void StripPath(char *str)
{
	char *c; int len;

	for (c=str, len=0; *c; c++, len++);
	
	while (len)
	{
		if (*(--c) == '\\')
		{
			*(c+1) = 0;
			break;
		}
		len--;
	}
}

/*
BOOL CALLBACK MsgDlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		SetWindowText(GetDlgItem(hdlg, IDC_TEXT), (char*)lParam);
		break;

	case WM_COMMAND:
		switch (LOWORD(lParam))
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hdlg, LOWORD(lParam));
			return TRUE;
		}
		break;
	}

	return FALSE;
}
*/

int ShowMessage(UINT str_id)
{
	char str[1024];
	int res;

	LoadString(hinstance, str_id, str, 1024);
	
	res = MessageBox(hwndMain, str, FROGGER2, MB_ICONEXCLAMATION|MB_OK);

	return res; 
}


int StartApp(char* cmdline)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	int res = CreateProcess(0, cmdline, NULL, NULL, 0, 0, 0, 0, &si, &pi);

	return res;
}

/*
int StartApp(char* cmdline)
{
	int res = (int)ShellExecute(hwndMain, "open", cmdline, NULL, NULL, SW_SHOWNORMAL);
	return (res > 32);

}
*/
// -------------------------------------------------------------------

int RunInstaller(void)
{	
	char path[MAX_PATH];

	path[0] = '\"';
	GetModuleFileName(NULL, path+1, MAX_PATH);
	StripPath(path);
	strcat(path, SETUP_EXE);
	strcat(path, "\"");

	if (StartApp(path))
	{
		DestroyWindow(hwndMain);
		return 0;
	}
	else
	{
		ShowMessage(IDS_INSTALLER_ERROR);
		return 1;
	}
}

int RunUninstaller(void)
{	
	// C:\WINDOWS\IsUninst.exe -f"C:\Program Files\Atari\Frogger2\uninst.isu"

	char path[MAX_PATH];

	GetWindowsDirectory(path, MAX_PATH);
	strcat(path, "\\IsUninst.exe -f\"");
	strcat(path, baseDirectory);
	if (path[strlen(path)-1] != '\\')
		strcat(path, "\\");
	strcat(path, "uninst.isu\"");

	if (StartApp(path))
	{
		DestroyWindow(hwndMain);
		return 0;
	}
	else
	{
		ShowMessage(IDS_FROGGER2_ERROR);
		return 1;
	}
}

int RunFrogger2(int config)
{
	char path[MAX_PATH];

	// enclose the app name in ""s to keep windows happy.. sigh
	path[0] = '\"';
	strcpy(path+1, baseDirectory);
	
	if (path[strlen(path)-1] != '\\')
		strcat(path, "\\");

	strcat(path, FROGGER2_EXE);
	strcat(path, "\"");
	
	if (config)
		strcat(path, " -c");

	if (StartApp(path))
	{
		DestroyWindow(hwndMain);
		return 0;
	}
	else
	{
		ShowMessage(IDS_FROGGER2_ERROR);
		return 1;
	}
}


// -------------------------------------------------------------------

int PaintWindow(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC hdc, hbmpDC;
	HBITMAP holdbmp;
	int i;

	BeginPaint(hwnd, &ps);

	hdc = ps.hdc;

	// Step 1: Mask and paint our window
	
	hbmpDC = CreateCompatibleDC(hdc);

	holdbmp = (HBITMAP)SelectObject(hbmpDC, hmask);
	BitBlt(ps.hdc, 0,0, BMPWIDTH, BMPHEIGHT, hbmpDC, 0,0, SRCAND);
	SelectObject(hbmpDC, holdbmp);

	holdbmp = (HBITMAP)SelectObject(hbmpDC, hbmp);
	BitBlt(ps.hdc, 0,0, BMPWIDTH, BMPHEIGHT, hbmpDC, 0,0, SRCPAINT);
	SelectObject(hbmpDC, holdbmp);


	// Step 2: Print text onto the window

	for( i=0; i<3; i++ )
	{
		if( !hOpt[i] )
			continue;

		POINT pt;

		pt.x = textPositions[i].x-BMPOPTWIDTH/2;
		pt.y = textPositions[i].y-BMPOPTHEIGHT/2;

		holdbmp = (HBITMAP)SelectObject(hbmpDC, hOpt[i]);
		BitBlt(ps.hdc, pt.x, pt.y, BMPOPTWIDTH, BMPOPTHEIGHT, hbmpDC, 0,0, SRCCOPY);
		SelectObject(hbmpDC, holdbmp);
	}
	
	ReleaseDC(hwnd, hbmpDC);

	/*
	int i;
	SetBkMode(hdc, TRANSPARENT);

	HFONT font, oldFont;

	font = CreateFont(24, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "Arial");
	oldFont = (HFONT)SelectObject(hdc, font);


	for (i=0; i<3; i++)
	{
		SIZE size;
		POINT pt;
		char *text = options[i];
		int len = strlen(text);

		GetTextExtentPoint(hdc, text, len, &size);

		pt.x = textPositions[i].x-size.cx/2;
		pt.y = textPositions[i].y-size.cy/2;

		SetTextColor(hdc, COLOUR_TEXTDROP);
		TextOut(hdc, pt.x+2, pt.y+2, text, len);

		SetTextColor(hdc, (i==selection)?COLOUR_TEXTSEL:COLOUR_TEXT);

		TextOut(hdc, pt.x, pt.y, text, len);
	}

	SelectObject(hdc, oldFont);
	DeleteObject(font);
*/
	EndPaint(hwnd, &ps);

	return 0;
}


int IsFrogger2Installed(void)
{
	HKEY hkey;
	DWORD len;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGISTRY_KEY, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{	
		len = MAX_PATH;

		if (RegQueryValueEx(hkey, "InstallDir", NULL, NULL, (unsigned char*)baseDirectory, &len) == ERROR_SUCCESS)
			installed = true;

		RegCloseKey(hkey);
	}

	return installed;
}


int SetupLanguage()
{
	HRESULT res;
	HKEY hkey;
	int language = -1;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGISTRY_KEY, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{	
		DWORD len = 14;
		char lang[15];

		if (RegQueryValueEx(hkey, "Language", NULL, NULL, (unsigned char*)lang, &len) == ERROR_SUCCESS)
		{
			for (int l=0; l<5; l++)
				if (stricmp(lang, languages[l])==0)
				{
					language = l;
					break;
				}
		}

		RegCloseKey(hkey);
	}

	if (language < 0)
	{
		if ((language = SelectLanguage()) == -1)
			return -1;

		if ((res = RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGISTRY_KEY, 0, "",
			REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, 0)) == ERROR_SUCCESS)
		{
			RegSetValueEx(hkey, "Language", 0, REG_SZ,
				(unsigned char*)languages[language],
				strlen(languages[language])+1);

			RegCloseKey(hkey);
		}
	}

	return 0;
}


int CreateMainWindow(HWND hwnd)
{
	hbmp = (HBITMAP)LoadImage(hinstance, MAKEINTRESOURCE(IDB_FROGGER2), IMAGE_BITMAP, 0, 0, 0);
	hmask = (HBITMAP)LoadImage(hinstance, MAKEINTRESOURCE(IDB_FROGGER2MASK), IMAGE_BITMAP, 0, 0, 0);

	if (!IsFrogger2Installed())
	{
//		LoadString(hinstance, IDS_INSTALL, options[0], 40);
		hOpt[0] = (HBITMAP)LoadImage(hinstance, MAKEINTRESOURCE(IDB_BMPINSTALL), IMAGE_BITMAP, 0, 0, 0);
		hOpt[1] = 0;
//		options[1][0]=0;
	}
	else
	{
		hOpt[0] = (HBITMAP)LoadImage(hinstance, MAKEINTRESOURCE(IDB_BMPPLAY), IMAGE_BITMAP, 0, 0, 0);
		hOpt[1] = (HBITMAP)LoadImage(hinstance, MAKEINTRESOURCE(IDB_BMPUNINSTALL), IMAGE_BITMAP, 0, 0, 0);
//		LoadString(hinstance, IDS_PLAY, options[0], 40);
//		LoadString(hinstance, IDS_UNINST, options[1], 40);
	}

	hOpt[2] = (HBITMAP)LoadImage(hinstance, MAKEINTRESOURCE(IDB_BMPCLOSE), IMAGE_BITMAP, 0, 0, 0);
/*
	wnd = CreateWindow("BUTTON", "Play",
		BS_PUSHBUTTON|BS_TEXT|WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_DISABLED,
		140, 280, 170, 30, hwnd, (HMENU)IDOK, hinstance, 0);

	EnableWindow(wnd, 0);
	
	wnd = CreateWindow("BUTTON", "Install",
		BS_PUSHBUTTON|BS_TEXT|BS_DEFPUSHBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP,
		140, 320, 170, 30, hwnd, (HMENU)IDC_INSTALL, hinstance, 0);

	wnd = CreateWindow("BUTTON", "Close",
		BS_PUSHBUTTON|BS_TEXT|WS_CHILD|WS_VISIBLE|WS_TABSTOP,
		140, 360, 170, 30, hwnd, (HMENU)IDCANCEL, hinstance, 0);
*/
	return 0;
}


long CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_MOUSEMOVE:
		{
			int i, newsel;
			POINT p = { LOWORD(lparam), HIWORD(lparam) };

			newsel = -1;
			for (i=0; i<3; i++)
				if (PtInRect(&buttons[i], p))
				{
					newsel = i;
					break;
				}
				
			if (newsel != selection)
			{
				InvalidateRect(hwnd, &buttons[selection], 0);
				selection = newsel;
				if (selection >= 0)
					InvalidateRect(hwnd, &buttons[selection], 0);
			}
		}
		break;

	case WM_CREATE:
		return CreateMainWindow(hwnd);

	case WM_ERASEBKGND:
		return 1;

	case WM_PAINT:
		return PaintWindow(hwnd);

	case WM_DESTROY:
		DeleteObject(hbmp);
		PostQuitMessage(0);
		break;

	case WM_COMMAND:
		switch (wparam)
		{
		case IDCANCEL:
			DestroyWindow(hwnd);
		}		break;

	case WM_LBUTTONUP:
		switch (selection)
		{
		case 0:
			if (installed)
				RunFrogger2(0);
			else
				RunInstaller();
			break;

		case 1:
			if (installed)
				//RunFrogger2(1);
				RunUninstaller();
			break;

		case 2:
			DestroyWindow(hwnd);
			break;
		}
		break;

/*	How to shoot yourself in the foot, lesson 23:
	Deleting your parent window when a child window gets focus

	case WM_ACTIVATE:
		if (LOWORD(wparam) == WA_INACTIVE)
			DestroyWindow(hwnd);
		break;
*/

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}




int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hprevinstance, LPSTR cmdline, int cmdshow)
{
	HRESULT res;

	hinstance = hinst;

	if (!hprevinstance)
	{
		WNDCLASS wc;

		wc.style = 0;
		wc.lpfnWndProc = MainWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hinstance;
		wc.hIcon = LoadIcon(hinstance, MAKEINTRESOURCE(IDI_FROGGER));
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = WINDOWCLASS;

		res = RegisterClass(&wc);
	}

	//if (SetupLanguage()) return -1;

	hwndMain = CreateWindowEx(0,WINDOWCLASS, FROGGER2,
		WS_POPUP|WS_CLIPCHILDREN,
		(GetSystemMetrics(SM_CXSCREEN)-BMPWIDTH)/2,
		(GetSystemMetrics(SM_CYSCREEN)-BMPHEIGHT)/2,
		BMPWIDTH, BMPHEIGHT, NULL, NULL, hinstance, 0);

	if (!hwndMain)
	{
		res = GetLastError();
		return res;
	}

	ShowWindow(hwndMain,SW_SHOWNORMAL);

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))  
    {
		if (!IsDialogMessage(hwndMain, &msg))
		{
			if (msg.message == WM_QUIT) break;
			DispatchMessage(&msg);
		}
    }

	return 0;
}