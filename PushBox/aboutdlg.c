
#include <windows.h>
#include "resource.h"

extern HINSTANCE ghInst;
//========================================================================
INT_PTR CALLBACK DlgAboutProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		break;

	case WM_INITDIALOG:
		{
			HICON hIcon = LoadIcon(ghInst, MAKEINTRESOURCE(ICO_MSG));
			SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			return TRUE;
		}

	case WM_COMMAND:
		if(LOWORD(wParam) == IDOK)
			EndDialog(hDlg, 0);
		break;
	}

	return FALSE;
}