#include <windows.h>
#include "resource.h"
#include "main.h"

#define NUM_LANGUAGES 5

BOOL CALLBACK LangDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	int i;
	static HBITMAP flags[5];

	switch (msg)
	{
	case WM_INITDIALOG:
		{
			for (i=0; i<NUM_LANGUAGES; i++)
			{
				flags[i] = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_ENGLISH+i));
				SendMessage(GetDlgItem(hwnd, IDC_LANGUAGE1+i), BM_SETIMAGE, IMAGE_BITMAP, (long)flags[i]);
			}
			return TRUE;
		}

	case WM_COMMAND:
		if (wparam == IDCANCEL)
		{
			EndDialog(hwnd, -1);
			break;
		}
		else if (wparam >= IDC_LANGUAGE1 && wparam < (IDC_LANGUAGE1+NUM_LANGUAGES))
		{
			EndDialog(hwnd, wparam - IDC_LANGUAGE1);
			break;
		}
		return FALSE;

	case WM_DESTROY:
		for (i=0; i<NUM_LANGUAGES; i++)
			DeleteObject(flags[i]);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

int SelectLanguage(void)
{
	return DialogBox(hinstance, MAKEINTRESOURCE(IDD_LANGUAGE), NULL, LangDialog);
}

