
#include <Windows.h>
#include "../BoxMap/MapInfo.h"
#include "resource.h"

extern MAPLIST *mapList;
extern MAPINFO newMap;
extern int nWhichOpened;

extern void Refresh(int x, int y);
//=========================================================================
INT_PTR CALLBACK MapListDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			MAPINFO *p = 0;
			p = mapList;

			while(p != 0)
			{
				SendDlgItemMessageA(hDlg, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)p->name);
				p = p->next;
			}

			if (SendDlgItemMessage(hDlg, IDC_LIST1, LB_GETCOUNT, 0, 0) > 0)
			{
				SendDlgItemMessage(hDlg, IDC_LIST1, LB_SETCURSEL, 0, 0);
				memcpy(&newMap, mapList, sizeof(MAPINFO));
				Refresh(-1, -1);
			}
		}
		return TRUE;

	case WM_COMMAND:
		if (HIWORD(wParam) == LBN_SELCHANGE)
		{
			MAPINFO *p = mapList;
			int i = 0;
			nWhichOpened = SendDlgItemMessage(hDlg, IDC_LIST1, LB_GETCURSEL, 0, 0);
			if (nWhichOpened == LB_ERR)
				break;

			while (p && i < nWhichOpened)
				p = p->next, i++;

			if (p){
				memcpy(&newMap, p, sizeof(MAPINFO));
				Refresh(-1, -1);
			}
			break;
		}

		if (LOWORD(wParam) == IDOK)
		{
			nWhichOpened = SendDlgItemMessage(hDlg, IDC_LIST1, LB_GETCURSEL, 0, 0);
			EndDialog(hDlg, IDOK);
		}
		return FALSE;

	case WM_CLOSE:
		EndDialog(hDlg, IDCANCEL);
		return FALSE;
	}

	return FALSE;
}