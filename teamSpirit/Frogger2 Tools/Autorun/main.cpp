
#include "language.h"
#include <windows.h>
#include "resource.h"
#include "main.h"

HINSTANCE hinstance;

HBITMAP hbmp, hmask;
HWND	hwndMain;

#define WINDOWCLASS "Frogger2autorun"

#define BMPWIDTH	357
#define BMPHEIGHT	363
#define IDC_INSTALL	1001

#define COLOUR_TEXTSEL	RGB(255,228,0)
#define COLOUR_TEXTDROP	RGB(3,132,0)
#define COLOUR_TEXT		RGB(150,255,0)

POINT textPositions[3] = { {178, 218}, {178, 255}, {178,292} };

RECT  buttons[3] = { {50,200,308,234}, {50,240,308,272}, {50,278,308,310} };

char options[3][40] = { "Play", "Setup", "Close" };

int selection = -1;

// -------------------------------------------------------------------


void RunInstaller(void)
{	
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (!CreateProcess("setup.exe", 0, NULL, NULL, 0, 0, 0, 0, &si, &pi))
		MessageBox(hwndMain, "BROKEN", "Arse", MB_ICONEXCLAMATION|MB_OK);
}


// -------------------------------------------------------------------

int PaintWindow(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC hdc, hbmpDC;
	HBITMAP holdbmp;

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

	ReleaseDC(hwnd, hbmpDC);

	// Step 2: Print text onto the window

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

	EndPaint(hwnd, &ps);

	return 0;
}


int CreateMainWindow(HWND hwnd)
{
	HWND wnd; 

	hbmp = (HBITMAP)LoadImage(hinstance, MAKEINTRESOURCE(IDB_FROGGER2), IMAGE_BITMAP, 0, 0, 0);
	hmask = (HBITMAP)LoadImage(hinstance, MAKEINTRESOURCE(IDB_FROGGER2MASK), IMAGE_BITMAP, 0, 0, 0);
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
			int i; POINT p = { LOWORD(lparam), HIWORD(lparam) };
			InvalidateRect(hwnd, &buttons[selection], 0);

			selection = -1;
			for (i=0; i<3; i++)
				if (PtInRect(&buttons[i], p))
				{
					selection = i;
					break;
				}
				
			InvalidateRect(hwnd, &buttons[i], 0);
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
		DestroyWindow(hwnd);
		break;

	case WM_ACTIVATE:
		if (LOWORD(wparam) == WA_INACTIVE)
			DestroyWindow(hwnd);
		break;

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
		wc.hIcon = LoadIcon(hinstance, MAKEINTRESOURCE(IDI_BLITZ));
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = WINDOWCLASS;

		res = RegisterClass(&wc);
	}

	SelectLanguage();

	hwndMain = CreateWindowEx(0,WINDOWCLASS, "Frogger2 - Swampy's Revenge",
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