//////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include "resource.h"
//////////////////////////////////////////////////////////////////////////
//========================================================================
extern int nCurrentGate;
extern int nTotalGates;
//========================================================================
extern void NewGate(int nGate);
//========================================================================
INT_PTR CALLBACK DlgGotoProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		break;

	case WM_INITDIALOG:
		{
			SetDlgItemInt(hDlg, IDC_EDIT1, nCurrentGate+1, TRUE);
			SetDlgItemInt(hDlg, IDS_MAX_GATE, nTotalGates, TRUE);
			return TRUE;
		}

	case WM_COMMAND:
		if(LOWORD(wParam) == IDOK)
		{
			int Gate = GetDlgItemInt(hDlg, IDC_EDIT1, NULL, TRUE);
			if (Gate <= 0 || Gate > nTotalGates)
			{
				MessageBox(hDlg, TEXT("关卡无效！请重输"), TEXT("推箱子"), MB_ICONERROR);
				SetFocus(GetDlgItem(hDlg, IDC_EDIT1));
				SendDlgItemMessage(hDlg, IDC_EDIT1, EM_SETSEL, 0, -1);
				break ;
			}

			if (Gate - 1 != nCurrentGate)
				NewGate(Gate - 1);

			EndDialog(hDlg, 0);
		}
		break;
	}

	return FALSE;
}