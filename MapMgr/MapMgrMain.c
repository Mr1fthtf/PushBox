//////////////////////////////////////////////////////////////////////////
#if defined( _MSC_VER ) && (_MSC_VER > 1400)	//VC2005����
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' "\
	"version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
//////////////////////////////////////////////////////////////////////////
#include "MapMgrMain.h"
#include <commctrl.h>
#include <stdio.h>
#include <shlwapi.h>
#include <strsafe.h>
//////////////////////////////////////////////////////////////////////////
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "strsafe.lib")
#pragma comment(lib, "boxmap.lib")
//========================================================================
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
	MSG msg = {0};
	HACCEL hAcc = NULL;

	//ֻ����ͬʱ����һ��ʵ��
	HANDLE hMutex = CreateMutex(NULL, FALSE, szClsName);

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		HWND hwndMapMgr = FindWindow(szClsName, NULL);
		SetForegroundWindow(hwndMapMgr);

		if (IsIconic(hwndMapMgr)){
			SendMessage(hwndMapMgr, WM_SYSCOMMAND, SC_RESTORE, 0);
		}

		CloseHandle(hMutex);

		return 0;
	}

	if (!Register(hInstance) || !InitWnd())
		return 0;

	hAcc = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!IsDialogMessage(hwndDlgMi, &msg) && !TranslateAccelerator(msg.hwnd, hAcc, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}
//========================================================================
BOOL Register(HINSTANCE hInst)
{
	WNDCLASSEX wcex = {0};

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcex.hInstance = hInst;
	wcex.lpfnWndProc = MapWndProc;
	wcex.lpszClassName = szClsName;
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU_MAIN);
	wcex.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_MAIN));

	if (!RegisterClassEx(&wcex))
		return FALSE;

	ghInst = hInst;

	return TRUE;
}
//========================================================================
BOOL InitWnd()
{
	hwndMain = CreateWindow(szClsName, szCaption,
		WS_OVERLAPPEDWINDOW,
		0, 0, DEF_CXWND, DEF_CYWND,
		NULL, NULL, ghInst, NULL);

	if (!hwndMain){
		return FALSE;
	}

	//���д���
	CenterWnd(hwndMain, GetDesktopWindow());

	ShowWindow(hwndMain, SW_SHOW);
	UpdateWindow(hwndMain);

	return TRUE;
}
//=========================================================================
LRESULT CALLBACK MapWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	HANDLE_MSG(hWnd, WM_CLOSE,			OnClose);
	HANDLE_MSG(hWnd, WM_PAINT,			OnPaint);
	HANDLE_MSG(hWnd, WM_CREATE,			OnCreate);
	HANDLE_MSG(hWnd, WM_GETMINMAXINFO,	OnGetMinMaxInfo);
	HANDLE_MSG(hWnd, WM_MOUSEMOVE,		OnMouseMove);
	HANDLE_MSG(hWnd, WM_LBUTTONDOWN,	OnLButtonDown);
	HANDLE_MSG(hWnd, WM_LBUTTONUP,		OnLButtonUp);
	HANDLE_MSG(hWnd, WM_COMMAND,		OnCommand);

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
//=========================================================================
INT_PTR CALLBACK MapInfoDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TCHAR szVer[32] = {0};
	switch(uMsg)
	{
	case WM_INITDIALOG:
		SendDlgItemMessage(hDlg, IDE_VER, EM_SETLIMITTEXT, 10, 0);
		SendDlgItemMessage(hDlg, IDE_NAME, EM_SETLIMITTEXT, 31, 0);
		SendDlgItemMessage(hDlg, IDE_AUTHOR, EM_SETLIMITTEXT, 19, 0);
		SendDlgItemMessage(hDlg, IDE_BOMB, EM_SETLIMITTEXT, 2, 0);

		StringCchPrintf(szVer, 32, TEXT("%d.%d"), LOWORD(newMap.ver), HIWORD(newMap.ver));
		SetDlgItemText(hDlg, IDE_VER, szVer);
		SetDlgItemInt(hDlg, IDE_BOMB, newMap.bomb, TRUE);
		SetDlgItemTextA(hDlg, IDE_NAME, newMap.name);
		SetDlgItemTextA(hDlg, IDE_AUTHOR, newMap.author);

		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			WORD hi = 0, lo = 0;
			TCHAR *p = 0;
			TCHAR tmp[10] = {0};
			int i = 0;

			GetDlgItemTextA(hDlg, IDE_AUTHOR, newMap.author, 20);
			GetDlgItemTextA(hDlg, IDE_NAME, newMap.name, 32);
			GetDlgItemText(hDlg, IDE_VER, szVer, 32);
			newMap.bomb = GetDlgItemInt(hDlg, IDE_BOMB, NULL, TRUE);

			p = szVer;
			//sscanfò��������
			if (*p)
			{
				while (*p && *p != TEXT('.'))
					tmp[i++] = *p++;

				if (i > 0){
					lo = _ttoi(tmp);
				}else{
					lo = 1;
				}

				if (*p == TEXT('.'))
					p++;

				SecureZeroMemory(tmp, 10);
				i = 0;
				while ( *p )
					tmp[i++] = *p++;

				if (i > 0){
					hi = _ttoi(tmp);
				}else{
					hi = 0;
				}
			}
			else{
				lo = 1, hi = 0;
			}

			newMap.ver = MAKELONG(lo, hi);

			SendMessage(hDlg, WM_CLOSE, 0, 0);
		}

		break;

	case WM_CLOSE:
		DestroyWindow(hDlg);
		hwndDlgMi = NULL;
		break;
	}

	return FALSE;
}
//=========================================================================
void CenterWnd(HWND hwndTo, HWND hwndParent)
{
	RECT rcParent;
	RECT rcTo;

	int x = 0, y = 0;
	int nWidth, nHeight;

	if (hwndParent == NULL){
		hwndParent = GetDesktopWindow();
	}
	
	GetClientRect(hwndParent, &rcParent);
	GetWindowRect(hwndTo, &rcTo);

	nWidth = rcTo.right - rcTo.left;
	nHeight = rcTo.bottom - rcTo.top;

	x = rcParent.left + (rcParent.right - rcParent.left - nWidth)/2;
	y = rcParent.top + (rcParent.bottom - rcParent.top - nHeight)/2;

	MoveWindow(hwndTo, x, y, nWidth, nHeight, TRUE);
}
//=========================================================================
void OnClose(HWND hWnd)
{
	FreeMaps(&mapList);
	if (hBitmap){
		DeleteObject(hBitmap);
	}

	if (hdcMem){
		DeleteDC(hdcMem);
	}
	DestroyWindow(hWnd);
	PostQuitMessage(0);
}
//=========================================================================
void OnPaint(HWND hWnd)
{
	HDC hdc;
	PAINTSTRUCT ps;
	int row, col;
	RECT rect;

	GetClientRect(hWnd, &rect);

	//Ϊ��ʹ��ͼ������ʾ
	//�������BMP_WIDTH��϶
	nOffsetX = rect.left + ((rect.right - rect.left) - COL_NUM * BMP_WIDTH)/2 + BMP_WIDTH;
	nOffsetY = rect.top + ((rect.bottom - rect.top) - ROW_NUM * BMP_HEIGHT)/2;

	hdc = BeginPaint(hWnd, &ps);

	//�����
	PaintLeft(hdc, nOffsetY, &rect);

	for (row=0; row<ROW_NUM; row++)
	{
		for (col=0; col<COL_NUM; col++)
		{
			DrawObject(hdc, hdcMem, nOffsetX + col*BMP_WIDTH,
				nOffsetY + row*BMP_HEIGHT,
				newMap.data[row][col]);
		}
	}

	EndPaint(hWnd, &ps);
}
//========================================================================
BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	HDC hdc = NULL;

	hdc = GetDC(hWnd);
	hdcMem = CreateCompatibleDC(hdc);

	if (PathFileExists(TEXT("Map\\back.bmp"))){
		hBitmap = (HBITMAP)LoadImage(NULL, TEXT("Map\\back.bmp"),
			IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}else{
		hBitmap = GetBackBitmap();
	}
	
	SelectObject(hdcMem, hBitmap);

	ReleaseDC(hWnd, hdc);

	//��ʼ����ͼ
	newMap.ver		= MAKELONG(1, 0);
	newMap.bomb		= 1;

	StringCchCopyA(newMap.author, 20, "UNKNOWN");
	StringCchCopyA(newMap.name, 32, "GeneralMap");

	memset(newMap.data, MAP_BACK, ROW_NUM*COL_NUM);
	
	return TRUE;
}
//========================================================================
void OnGetMinMaxInfo(HWND hWnd, LPMINMAXINFO lpMinMaxInfo)
{
	lpMinMaxInfo->ptMinTrackSize.x = DEF_CXWND;
	lpMinMaxInfo->ptMinTrackSize.y = DEF_CYWND;
}
//========================================================================
void OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags)
{
	int row = 0, col = 0;

	//��꾭�����ͼƬ����ʱ
	if (!keyFlags & MK_LBUTTON && x < BMP_WIDTH &&
		y >= nOffsetY && y <= (nOffsetY + BMP_HEIGHT * 7))
	{
		SetCursor(LoadCursor(NULL, IDC_HAND));
		return ;
	}

	if (!fDragOk)
		return ;

	SetCursor(LoadCursor(NULL, IDC_HAND));

	//������꾭��ʱ���ڵ�ͼ����
	row = (y - nOffsetY)/BMP_HEIGHT;
	col = (x - nOffsetX)/BMP_WIDTH;

	if (row < 0 || row >= ROW_NUM || col < 0 || col >= COL_NUM)
	{
		if (oldX != -1 && oldY != -1)	//����ʱҲ��ͼƬ���ϡ�����
		{
			newMap.data[oldX][oldY] = prevBombObject;		
			Refresh(oldX, oldY);
		}
		
		return ;
	}

	//��ԭ�ϴ�Ʈ��ʱ��λͼ
	if (oldX != -1 && oldY != -1)
	{
		newMap.data[oldX][oldY] = prevBombObject;		
		Refresh(oldX, oldY);
	}

	//���漴�����ǵ�λͼ
	oldX = row;
	oldY = col;
	prevBombObject = newMap.data[row][col];

	//����Ϊ�϶���λͼ
	newMap.data[row][col] = dragObject;

	Refresh(row, col);
}
//========================================================================
void OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	int n = 0;

	if (x > BMP_WIDTH || y < nOffsetY || y > (nOffsetY + BMP_HEIGHT * 7))
		return ;

	SetCursor(LoadCursor(NULL, IDC_HAND));

	//����λͼ����
	n = (y - nOffsetY)/BMP_HEIGHT;

	if (n < 7){
		dragObject = (MapType)(MAP_BACK + n);
		fDragOk = TRUE;
	}
}
//========================================================================
void OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
{
	int row = 0, col = 0;

	if (!fDragOk){
		return ;
	}

	row = (y - nOffsetY)/BMP_HEIGHT;
	col = (x - nOffsetX)/BMP_WIDTH;

	if (row < 0 || row >= ROW_NUM || col < 0 || col >= COL_NUM)
	{
		oldX = oldY = -1;
		fDragOk = FALSE;

		return ;
	}

	//
	newMap.data[row][col] = dragObject;

	Refresh(row, col);

	oldX = oldY = -1;
	fDragOk = FALSE;

	SetCursor(LoadCursor(NULL, IDC_ARROW));
}
//========================================================================
void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
	{
	case IDM_OPEN:	//�򿪵�ͼ
		doOpen();
		break;

	case IDM_NEW:	//�½���ͼ
		newMap.ver		= MAKELONG(1, 0);
		newMap.bomb		= 1;
		
		StringCchCopyA(newMap.author, 20, "UNKNOWN");
		StringCchCopyA(newMap.name, 32, "GeneralMap");
		
		memset(newMap.data, MAP_BACK, ROW_NUM * COL_NUM);
		Refresh(-1, -1);

		//
		nMapCount = 0;
		nWhichOpened = -1;

		break;

	case IDM_SAVE:	//�����ͼ
		doSave();
		break;

	case IDM_EXIT:
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	case IDM_MAP_INFO:
		if (hwndDlgMi == NULL){
			hwndDlgMi = CreateDialogParam(ghInst, MAKEINTRESOURCE(IDD_MAP_INFO),
				hWnd, MapInfoDlgProc, 0);
		}else{
			SetFocus(hwndDlgMi);
		}
		break;

	case IDM_ABOUT:
		MessageBox(hWnd, TEXT("�����ӵ�ͼ�༭�� v1.0\r\n\r\n")
			TEXT("\tBy: Just_Fancy\r\n")
			TEXT("\tDec, 2010")
			, TEXT("����"), MB_ICONINFORMATION);
		break;

	case IDM_HELP:
		MessageBox(hWnd, TEXT("������ߵ�ͼƬ��ס�����϶����ұ߼���\r\n")
			TEXT("��ͼ�豣�����������Ŀ¼�µ�MapĿ¼�����Զ���ӵ���Ϸ��\r\n")
			TEXT("������Ҫ�ֶ�����")
			, szCaption, MB_ICONINFORMATION);
		break;
	}
}
//========================================================================
void PaintLeft(HDC hdc, int offsetY, LPCRECT pRect)
{
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
	HGDIOBJ hOldObj;
	int i = 0;

	for (i=0; i<7; i++){
		BitBlt(hdc, 2, offsetY + i *BMP_HEIGHT,
			BMP_WIDTH, BMP_HEIGHT, hdcMem, i * BMP_WIDTH, 0, SRCCOPY);
	}

	hOldObj = SelectObject(hdc, hPen);

	//��һ����
	MoveToEx(hdc, BMP_WIDTH + 5, 0, NULL);
	LineTo(hdc, BMP_WIDTH + 5, pRect->bottom - pRect->top);

	SelectObject(hdc, hOldObj);
	DeleteObject(hPen);
}
//========================================================================
void Refresh(int x, int y)
{
	RECT rect;

	if (x == -1 || y == -1)
	{
		GetClientRect(hwndMain, &rect);
	}
	else
	{
		rect.left = y * BMP_WIDTH + nOffsetX;
		rect.top = x * BMP_HEIGHT + nOffsetY;
		rect.right = rect.left + BMP_WIDTH;
		rect.bottom = rect.top + BMP_HEIGHT;
	}

	InvalidateRect(hwndMain, &rect, FALSE);
}
//========================================================================
BOOL IsMapValid()
{
	int row, col;
	int nMan = 0,
		nDest = 0,
		nBox = 0,
		nWall = 0;

	for (row = 0; row<ROW_NUM; row++)
	{
		for (col=0; col<COL_NUM; col++)
		{
			if (newMap.data[row][col] == MAP_MAN)
				nMan++;
			else if(newMap.data[row][col] == MAP_DEST)
				nDest++;
			else if(newMap.data[row][col] == MAP_BOX_NRM)
				nBox++;
			else if(newMap.data[row][col] == MAP_WALL)
				nWall++;
			else if(newMap.data[row][col] == MAP_BOX_FIS)
				nBox++, nDest++;
		}
	}

	//������ֻ����һ��
	if (nMan != 1)
		return FALSE;

	//���Ӹ�Ŀ����ĿҪһ��, �Ҳ���Ϊ0
	if (nBox != nDest || nBox < 1)
		return FALSE;

	//Ҫ��ǽ
	if (nWall < 1)
		return FALSE;

	return TRUE;
}
//========================================================================
void doOpen()
{
	if (!SelFilePath(TRUE, szOpenedFilePath, MAX_PATH))
		return ;

	nMapCount = 0;
	FreeMaps(&mapList);

	ReadMaps(szOpenedFilePath, &mapList, &nMapCount);

	if (nMapCount > 1)
	{
		//����ɵ���
		memcpy(&oldMap, &newMap, sizeof(MAPINFO));

		if (DialogBoxParam(ghInst, MAKEINTRESOURCE(IDD_MAP_LIST),
			hwndMain, MapListDlgProc, 0) == IDCANCEL)
		{
			memcpy(&newMap, &oldMap, sizeof(MAPINFO));
			Refresh(-1, -1);
		}
	}
	else
	{
		memcpy(&newMap, mapList, sizeof(MAPINFO));
		Refresh(-1, -1);
	}
}
//========================================================================
void doSave()
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD dwWritten;
	char szFilePath[MAX_PATH] = {0};
	DWORD dwCreateFlag = OPEN_ALWAYS;
	BOOL fAddToEnd = TRUE;

	if (!IsMapValid())
	{
		MessageBox(hwndMain, TEXT("�������Ƶĵ�ͼ������Ҫ��!"), szCaption, MB_ICONERROR);
		return ;
	}

	if (*szOpenedFilePath){
		StringCchCopyA(szFilePath, MAX_PATH, szOpenedFilePath);
	}

	if (!SelFilePath(FALSE, szFilePath, MAX_PATH))
		return ;

	__try
	{
		char *ext = PathFindExtensionA(szFilePath);
		if (*ext == 0){
			StringCchCatA(szFilePath, MAX_PATH, ".map");
		}

		if (nMapCount > 0 && mapList)	//�����Ǵ��ļ��򿪵ĵ�ͼ
		{
			//ѡ�����ԭ���ĵ�ͼ�ļ�
			if (StrCmpIA(szOpenedFilePath, szFilePath) == 0)
			{
				if (nWhichOpened != -1)
				{
					int sel = MessageBox(hwndMain,
						TEXT("��ѡ�����ԭ���ĵ�ͼ�ļ����Ƿ��޸�ԭ���ĵ�ͼ��\r\n")
						TEXT("�ǣ��޸�\r\n����ӵ�ĩβ\r\nȡ����������"),
						szCaption, MB_ICONQUESTION | MB_YESNOCANCEL);

					if (sel == IDCANCEL)
						__leave;
					else if(sel == IDYES){
						dwCreateFlag = CREATE_ALWAYS;
						fAddToEnd = FALSE;
					}
					else if(sel == IDNO){
						dwCreateFlag = OPEN_ALWAYS;
						fAddToEnd = TRUE;
					}
				}
			}
		}

		hFile = CreateFileA(szFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL,
			dwCreateFlag, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile == INVALID_HANDLE_VALUE)
			__leave;

		if (fAddToEnd && GetFileSize(hFile, NULL) > 0){	//ֱ�Ӽӵ�ĩβȥ
			SetFilePointer(hFile, 0, 0, FILE_END);
		}

		if (fAddToEnd)
		{
			if (!WriteFile(hFile, &newMap, sizeof(MAPINFO), &dwWritten, NULL)){
				MessageBox(hwndMain, TEXT("�����ͼʧ����"), szCaption, MB_ICONERROR);
				__leave;
			}
		}
		else
		{
			int i;
			MAPINFO *p = mapList;
			for (i=0; i<nMapCount; i++)
			{
				if (i == nWhichOpened){
					WriteFile(hFile, &newMap, sizeof(MAPINFO), &dwWritten, NULL);
				}else{
					WriteFile(hFile, p, sizeof(MAPINFO), &dwWritten, NULL);
				}
				p = p->next;
			}
		}
	}
	__finally
	{
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
	}
}