//////////////////////////////////////////////////////////////////////////
#if defined( _MSC_VER ) && (_MSC_VER > 1400)	//VC2005以上
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' "\
	"version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
//////////////////////////////////////////////////////////////////////////
#include <WindowsX.h>

#include "WinMain.h"
#include <shlwapi.h>
#include <strsafe.h>
//////////////////////////////////////////////////////////////////////////
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "boxmap.lib")
#pragma comment(lib, "strsafe.lib")
//////////////////////////////////////////////////////////////////////////
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
	MSG msg = {0};
	HACCEL hAcc = NULL;

	if (!Register(hInstance) || !InitWnd())
		return 0;
	
	if (!LoadMaps())
	{
		EnableMenuItem(GetMenu(hwndMain), IDM_NEXT, MF_DISABLED);
		EnableMenuItem(GetMenu(hwndMain), IDM_PREV, MF_DISABLED);
		EnableMenuItem(GetMenu(hwndMain), IDM_JUMP, MF_DISABLED);

		SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_PREV, MAKELONG(FALSE, 0));
		SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_NEXT, MAKELONG(FALSE, 0));

		SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_REPLAY, MAKELONG(FALSE, 0));
		EnableMenuItem(GetMenu(hwndMain), IDM_REPLAY, MF_DISABLED);

		MessageBox(hwndMain, TEXT("加载地图失败！！！"), szCaption, MB_ICONERROR);
	}
	else
	{
		NewGate(0);

		if (nTotalGates < 2)
		{
			SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_NEXT, MAKELONG(FALSE, 0));
			EnableMenuItem(GetMenu(hwndMain), IDM_NEXT, MF_DISABLED);			
		}
	}
	
	hAcc = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAcc, &msg))
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
	wcex.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(ICO_MAIN));

	if (!RegisterClassEx(&wcex))
		return FALSE;

	ghInst = hInst;

	return TRUE;
}
//========================================================================
BOOL InitWnd()
{
	hwndMain = CreateWindow(szClsName, szCaption,
		WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, 0,
		425, 460, NULL, NULL, ghInst, NULL);

	if (!hwndMain){
		return FALSE;
	}

	CenterWnd(hwndMain, GetDesktopWindow());

	ShowWindow(hwndMain, SW_SHOW);
	UpdateWindow(hwndMain);

	return TRUE;
}
//========================================================================
LRESULT CALLBACK MapWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	HANDLE_MSG(hWnd, WM_CLOSE,		OnClose);
	HANDLE_MSG(hWnd, WM_COMMAND,	OnCommand);
	HANDLE_MSG(hWnd, WM_PAINT,		OnPaint);
	HANDLE_MSG(hWnd, WM_CREATE,		OnCreate);
	HANDLE_MSG(hWnd, WM_NOTIFY,		OnNotify);
	HANDLE_MSG(hWnd, WM_KEYDOWN,	OnKeydown);
	HANDLE_MSG(hWnd, WM_TIMER,		OnTimer);
	HANDLE_MSG(hWnd, WM_SYSCOMMAND, OnSysCommand);

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}
//========================================================================
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
//========================================================================
void OnPaint(HWND hWnd)
{
	HDC hdc;
	PAINTSTRUCT ps;
	int row, col;
	RECT tbRect;	//工具栏

	GetWindowRect(hwndToolbar, &tbRect);

	hdc = BeginPaint(hWnd, &ps);

	for (row=0; row<ROW_NUM; row++)
	{
		for (col=0; col<COL_NUM; col++)
		{
			DrawObject(hdc, hdcMem, col*BMP_WIDTH, 
				(tbRect.bottom - tbRect.top) + row*BMP_HEIGHT,
				curMap.data[row][col]
				);
		}
	}

	EndPaint(hWnd, &ps);
}
//========================================================================
BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	HDC hdc = NULL;

	hwndToolbar = CreateToolbarEx(hWnd, WS_VISIBLE | WS_CHILD |\
		TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | CCS_ADJUSTABLE,\
		-1, NUM_BUTTONS,\
		ghInst, IDC_TOOLBAR,
		tbs,\
		NUM_BUTTONS, 16, 16, 16, 16, sizeof(TBBUTTON));
	
	hwndStatus = CreateStatusWindow(WS_CHILD | WS_VISIBLE,
		NULL, hWnd, -1);
	
	SendMessage(hwndStatus, SB_SETPARTS, 4,
		(LPARAM)dwStatusWidth);

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

	return TRUE;
}
//========================================================================
void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
	{
	case IDM_LOAD_MAP:
		doOpen();
		break;

	case IDM_MAP_MGR:
		ShellExecute(hWnd, TEXT("open"), TEXT("MapMgr.exe"), NULL, NULL, SW_SHOW);
		break;

	case IDM_REPLAY:
		if (nStepCount > 0)	//一步都没走就不用重来了吧
		{
			fGamePaused = TRUE;
			if (MessageBox(hWnd, TEXT("真的要放弃当前游戏重玩吗？"),
				szCaption, MB_ICONQUESTION | MB_YESNO) == IDYES)
				NewGate(nCurrentGate);
			fGamePaused = FALSE;
		}
		break;
		
	case IDM_PREV:
		if (nStepCount > 0 && !DoneGame())
		{
			fGamePaused = TRUE;
			if (MessageBox(hWnd, TEXT("确定放弃当前游戏？"),
				szCaption, MB_ICONQUESTION | MB_YESNO) == IDNO)
			{
				fGamePaused = FALSE;
				break;
			}
		}
		NewGate(nCurrentGate-1);
		break;
		
	case IDM_NEXT:
		if (nStepCount > 0 && !DoneGame())
		{
			fGamePaused = TRUE;
			if (MessageBox(hWnd, TEXT("确定放弃当前游戏？"),
				szCaption, MB_ICONQUESTION | MB_YESNO) == IDNO)
			{
				fGamePaused = FALSE;
				break;
			}
		}
		NewGate(nCurrentGate+1);
		break;
		
	case IDM_JUMP:
		DialogBoxParam(ghInst, MAKEINTRESOURCE(IDD_GOTO),
			hWnd, DlgGotoProc, 0);	
		break;

	case IDM_UNSTEP:
		curMap.data[lastRecord.manOldX][lastRecord.manOldY] = MAP_MAN;
		curMap.data[lastRecord.manNewX][lastRecord.manNewY] = lastRecord.manNewObject;
		Refresh(lastRecord.manOldX, lastRecord.manOldY);
		Refresh(lastRecord.manNewX, lastRecord.manNewY);
		
		if (lastRecord.fBoxMoved)
		{
			curMap.data[lastRecord.boxOldX][lastRecord.boxOldY] = lastRecord.oldBoxObject;
			curMap.data[lastRecord.boxNewX][lastRecord.boxNewY] = lastRecord.boxNewObject;
			Refresh(lastRecord.boxOldX, lastRecord.boxOldY);
			Refresh(lastRecord.boxNewX, lastRecord.boxNewY);
		}
		
		nStepCount -- ;
		ShowStep();

		EnableMenuItem(GetMenu(hWnd), IDM_UNSTEP, MF_DISABLED);
		break;

	case IDM_ABOUT:
		DialogBoxParam(ghInst, MAKEINTRESOURCE(IDD_ABOUT),
			hWnd, DlgAboutProc, 0);
		break;
		
	case IDM_EXIT:
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	case IDM_HELP:
		MessageBox(hWnd, TEXT("移动人物：用方向键或W,A,S,D\r\n")
			TEXT("使用炸弹：先按B键显示炸弹，移动到目的地后按Enter使用\r\n")
			TEXT("再次按B可隐藏炸弹\r\n")
			TEXT("使用快捷键Ctrl+Z可对当前游戏进行后退一步\r\n")
			TEXT("如需要不同的背景图片，可替换Map目录下的back.bmp位图")
			,
			TEXT("推箱子－帮助"), MB_ICONINFORMATION);
		break;
	}
}
//========================================================================
void OnClose(HWND hWnd)
{
	if (nStepCount > 0 && !DoneGame()){
		fGamePaused = TRUE;
		if (MessageBox(hWnd, TEXT("游戏未完成，是否真的要退出？"),
			szCaption, MB_ICONQUESTION | MB_YESNO) == IDNO)
		{
			fGamePaused = FALSE;
			return ;
		}
	}

	if (hBitmap){
		DeleteObject(hBitmap);
	}
	if (hdcMem){
		DeleteDC(hdcMem);
	}

	FreeMaps(&mapList);

	KillTimer(hWnd, TIMER_GAME_ID);

	DestroyWindow(hWnd);
	PostQuitMessage(0);
}
//========================================================================
LRESULT OnNotify(HWND hWnd, int id, NMHDR *pnmhdr)
{	
	if (pnmhdr->code == TTN_NEEDTEXT)
	{
		((TOOLTIPTEXT *)pnmhdr)->lpszText = (TCHAR*)pnmhdr->idFrom;
	}

	return TRUE;
}
//========================================================================
/*void DrawObject(HDC hdc, int x, int y, MapType type)
{
	int bmpOffset = BMP_WIDTH * (int)(type - MAP_BACK);

	BitBlt(hdc, x, y, BMP_WIDTH, BMP_HEIGHT, hdcMem, bmpOffset, 0, SRCCOPY);
}*/
//========================================================================
void NewGate(int nGate)
{
	TCHAR szInfo[256] = {0};
	MAPINFO	*p = NULL;
	int i = 0;
	HMENU hMenu = GetMenu(hwndMain);

	if (nGate < 0){
		MessageBox(hwndMain, TEXT("非法关卡数字！！！"), szCaption, MB_ICONWARNING);
		return ;
	}

	if (nGate >= nTotalGates){
		MessageBox(hwndMain, TEXT("已超出游戏关卡范围！"), szCaption, MB_ICONWARNING);
		return ;
	}

	nCurrentGate = nGate;

	if (nCurrentGate == 0)
	{
		EnableMenuItem(hMenu, IDM_PREV, MF_DISABLED);
		SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_PREV, MAKELONG(FALSE, 0));
		if (nTotalGates > 1){
			SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_NEXT, MAKELONG(TRUE, 0));
			EnableMenuItem(hMenu, IDM_NEXT, MF_ENABLED);
		}
	}
	else if (nCurrentGate == nTotalGates -1)
	{
		EnableMenuItem(hMenu, IDM_NEXT, MF_DISABLED);
		SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_NEXT, MAKELONG(FALSE, 0));
		if (nTotalGates > 1){
			SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_PREV, MAKELONG(TRUE, 0));
			EnableMenuItem(hMenu, IDM_PREV, MF_ENABLED);
		}
	}
	else
	{
		EnableMenuItem(hMenu, IDM_NEXT, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_PREV, MF_ENABLED);
		SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_PREV, MAKELONG(TRUE, 0));
		SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_NEXT, MAKELONG(TRUE, 0));
	}
	SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_REPLAY, MAKELONG(FALSE, 0));
	EnableMenuItem(hMenu, IDM_REPLAY, MF_DISABLED);
	EnableMenuItem(hMenu, IDM_UNSTEP, MF_DISABLED);

	__try
	{
		p = GetMapAt(mapList, nGate);

		if (!p){
			return ;
		}
		ZeroMemory(&curMap, sizeof(MAPINFO));
		ZeroMemory(&oriMap, sizeof(MAPINFO));

		memcpy(&curMap, p, sizeof(MAPINFO));
		memcpy(&oriMap, p, sizeof(MAPINFO));	
	}

	__finally{
		Refresh(-1, -1);
	}
	
	if (strlen(curMap.name) > 0){	//用%hs可以偷下懒，在UNICODE时省的转换
		StringCchPrintf(szInfo, 256, TEXT("%s - 第 %d 关 - %hs"), szCaption, nCurrentGate + 1, curMap.name);

	}else{
		StringCchPrintf(szInfo, 256, TEXT("%s - 第 %d 关"), szCaption, nCurrentGate + 1);
	}
	SetWindowText(hwndMain, szInfo);

	nStepCount = 0;
	dwTimeBegin = GetTickCount();
	dwTimePause = 0;
	fGamePaused = FALSE;

	KillTimer(hwndMain, TIMER_GAME_ID);
	SetTimer(hwndMain,	TIMER_GAME_ID, 1000, NULL);

	StringCchPrintf(szInfo, 256, TEXT("\tv%d.%d by %hs"),
		LOWORD(curMap.ver), HIWORD(curMap.ver), curMap.author);

	SendMessage(hwndStatus, SB_SETTEXT, 3, (LPARAM)szInfo);

	ShowStep();
	ShowTime();
	ShowBomb();
}
//========================================================================
void OnKeydown(HWND hWnd, UINT uKey, BOOL fDown, int cRepeat, UINT flags)
{
	int oldX  = 0, oldY = 0;
	int newX  = 0, newY = 0;
	MapType type;

	if (uKey == TEXT('B'))
	{
		doBombKey();
		return ;
	}
	else if (uKey == VK_RETURN)
	{
		if (!fUseBomb || prevBombObject != MAP_WALL)
			return ;

		KillTimer(hWnd, TIMER_BOMB_ID);

		PlaySound(TEXT("Sound\\bomb.wav"), NULL, SND_ASYNC | SND_FILENAME);

		SetObject(nBombX, nBombY, MAP_ROAD);

		Refresh(nBombX, nBombY);

		nBombX = nBombY = -1;
		curMap.bomb--;
		prevBombObject = MAP_WALL;

		ShowBomb();

		fUseBomb = FALSE;

		return ;
	}

	if (fUseBomb)
	{
		oldX = nBombX;
		oldY = nBombY;
	}
	else
	{
		if (GetPos(&oldX, &oldY, MAP_MAN) == -1)
			return ;
	}

	newX = oldX;
	newY = oldY;

	switch(uKey)
	{
	case TEXT('A'):
	case VK_LEFT:	newY--;	break;

	case TEXT('D'):
	case VK_RIGHT:	newY++;	break;

	case TEXT('W'):
	case VK_UP:		newX--;	break;

	case TEXT('S'):
	case VK_DOWN:	newX++;	break;
	}

	if (fUseBomb)
	{
		MoveBomb(nBombX, nBombY, newX, newY);
		return ;
	}

	type = GetType(newX, newY);

	//下一步有箱子挡住
	if (type == MAP_BOX_FIS || type == MAP_BOX_NRM)
	{
		//箱子可以移动
		if (MoveBox(newX, newY, uKey))
		{
			//人继续走
			MoveMan(oldX, oldY, newX, newY);

			//判断游戏完成了没有
			if (DoneGame())
			{
				if (nCurrentGate < nTotalGates - 1)
				{
					int sel = MessageBox(hWnd, TEXT("已完成当前关卡， 是否继续"),
						szCaption, MB_ICONINFORMATION | MB_YESNO);
					if (sel == IDYES)
						NewGate(nCurrentGate+1);
				}
				else
				{
					MessageBox(hWnd, TEXT("恭喜！已完成所有关卡！"),
						szCaption, MB_ICONINFORMATION);
				}
			}
		}
	}
	else if (type == MAP_ROAD || type == MAP_DEST)
	{
		MoveMan(oldX, oldY, newX, newY);
		lastRecord.fBoxMoved = FALSE;
	}	
}
//========================================================================
void OnTimer(HWND hWnd, UINT uID)
{
	if (uID == TIMER_GAME_ID)
	{
		if (fGamePaused){
			dwTimePause ++;
		}else{
			ShowTime();
		}
	}

	else if (uID == TIMER_BOMB_ID)
	{
		if (prevBombObject == MAP_WALL || prevBombObject == MAP_DEST)
		{
			if (GetType(nBombX, nBombY) == MAP_BOMB){
				SetObject(nBombX, nBombY, prevBombObject);
			}else{
				if (prevBombObject == MAP_WALL)
					PlaySound(TEXT("Sound\\di.wav"), NULL, SND_ASYNC | SND_FILENAME);

				SetObject(nBombX, nBombY, MAP_BOMB);
			}

			Refresh(nBombX, nBombY);
		}
	}
}
//========================================================================
void OnSysCommand(HWND hWnd, UINT cmd, int x, int y)
{
	if (cmd == SC_MINIMIZE)
		fGamePaused = TRUE;
	else if (cmd == SC_RESTORE)
		fGamePaused = FALSE;

	DefWindowProc(hWnd, WM_SYSCOMMAND, cmd, MAKELPARAM(x, y));
}
//========================================================================
int GetPos(int *x, int *y, MapType type)
{
	int row, col;

	*x = *y = 0;

	if (nCurrentGate < 0 || nCurrentGate >= nTotalGates)
		return -1;

	for (row=0; row<ROW_NUM; row++)
	{
		for (col=0; col<COL_NUM; col++)
		{
			if (curMap.data[row][col] == type )
			{
				*x = row;
				*y = col;

				return MAKELPARAM(row, col);
			}
		}
	}

	return -1;
}
//========================================================================
MapType GetType(int x, int y)
{
	if (x < 0 || x > ROW_NUM ||
		y < 0 || y > COL_NUM
		)
		return -1;

	return curMap.data[x][y];
}
//========================================================================
void SetObject(int x, int y, MapType type)
{
	if (x<0 || x>=ROW_NUM || y<0 || y>=COL_NUM)
		return ;

	curMap.data[x][y] = type;
}
//========================================================================
void MoveMan(int oldX, int oldY, int newX, int newY)
{
	MapType type;

	type = oriMap.data[oldX][oldY];

	//MAP_BOX_FIS也要处理，不然就完玩了
	if (type == MAP_DEST || type == MAP_BOX_FIS)	//还原前一步的图片
		curMap.data[oldX][oldY] = MAP_DEST;
	else
		curMap.data[oldX][oldY] = MAP_ROAD;

	lastRecord.manOldX = oldX;
	lastRecord.manOldY = oldY;
	lastRecord.manNewObject = curMap.data[newX][newY];
	lastRecord.manNewX = newX;
	lastRecord.manNewY = newY;

	//设置新的
	curMap.data[newX][newY] = MAP_MAN;

	nStepCount ++;

 	Refresh(oldX, oldY);
 	Refresh(newX, newY);

	if (nStepCount > 0)
	{
		SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_REPLAY, MAKELONG(TRUE, 0));
		EnableMenuItem(GetMenu(hwndMain), IDM_REPLAY, MF_ENABLED);		
	}

	EnableMenuItem(GetMenu(hwndMain), IDM_UNSTEP, MF_ENABLED);

	ShowStep();
}
//========================================================================
BOOL MoveBox(int x, int y, UINT uKey)
{
	int newX = x, newY = y;
	MapType type;

	switch(uKey)
	{
	case TEXT('A'):
	case VK_LEFT:	newY--;	break;
	case TEXT('D'):
	case VK_RIGHT:	newY++;	break;
	case TEXT('W'):
	case VK_UP:		newX--;	break;
	case TEXT('S'):
	case VK_DOWN:	newX++;	break;
	}

	type = GetType(newX, newY);

	if (type == MAP_BACK || type == MAP_WALL ||
		type == MAP_BOX_NRM || type == MAP_BOX_FIS)
		return FALSE;

	lastRecord.fBoxMoved = TRUE;
	lastRecord.boxOldX = x;
	lastRecord.boxOldY = y;
	lastRecord.oldBoxObject = GetType(x, y);
	lastRecord.boxNewX = newX;
	lastRecord.boxNewY = newY;
	lastRecord.boxNewObject = type;

	if (type == MAP_DEST)
		curMap.data[newX][newY] = MAP_BOX_FIS;
	else
		curMap.data[newX][newY] = MAP_BOX_NRM;

	type = oriMap.data[x][y];
	//包括已移到目标的箱子
	if (type == MAP_DEST || type == MAP_BOX_FIS)
		curMap.data[x][y] = MAP_DEST;
	else
		curMap.data[x][y] = MAP_ROAD;

	Refresh(x, y);
	Refresh(newX, newY);

	return TRUE;
}
//========================================================================
void MoveBomb(int oldX, int oldY, int newX, int newY)
{
	MapType type;

	type = GetType(newX, newY);

	if (type == MAP_BOX_NRM || type == MAP_BOX_FIS ||
		type == MAP_MAN || type == MAP_BACK
		)
		return ;

	nBombX = newX;
	nBombY = newY;

	//恢复上次的
	SetObject(oldX, oldY, prevBombObject);

	//保存即将走上的
	prevBombObject = curMap.data[newX][newY];

	SetObject(newX, newY, MAP_BOMB);

	Refresh(oldX, oldY);
	Refresh(newX, newY);
}
//========================================================================
BOOL DoneGame()
{
	int i, j;
	if (nCurrentGate < 0 || nCurrentGate >= nTotalGates )
		return FALSE;

	for (i=0; i<ROW_NUM; i++)
	{
		for (j=0; j<COL_NUM; j++)
		{
			if (curMap.data[i][j] == MAP_BOX_NRM)
				return FALSE;
			if (curMap.data[i][j] == MAP_DEST)
				return FALSE;
		}
	}

	return TRUE;
}
//========================================================================
void Refresh(int x, int y)
{
	RECT rect;
	RECT tbRect;

	if (x == -1 || y == -1)
	{
		GetClientRect(hwndMain, &rect);
	}
	else
	{
		GetWindowRect(hwndToolbar, &tbRect);

		rect.left = y * BMP_WIDTH;
		rect.top = x * BMP_HEIGHT + (tbRect.bottom - tbRect.top);	//加上工具栏高度
		rect.right = rect.left + BMP_WIDTH;
		rect.bottom = rect.top + BMP_HEIGHT;
	}

	InvalidateRect(hwndMain, &rect, FALSE);
}
//========================================================================
void ShowStep()
{
	TCHAR szInfo[128];

	StringCchPrintf(szInfo, 128, TEXT("移动次数：%d"), nStepCount);

	SendMessage(hwndStatus, SB_SETTEXT, 2, (LPARAM)szInfo);
}
//========================================================================
void ShowTime()
{
	TCHAR szInfo[128];

	DWORD dwTotal = (GetTickCount() - dwTimeBegin)/1000 - dwTimePause;

	StringCchPrintf(szInfo, 128, TEXT("\t%02d:%02d:%02d"), dwTotal / 3600, dwTotal / 60, dwTotal % 60);

	SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)szInfo);
}
//========================================================================
void ShowBomb()
{
	TCHAR szInfo[128];

	StringCchPrintf(szInfo, 128, TEXT("炸弹数量：%d"), curMap.bomb);
	SendMessage(hwndStatus, SB_SETTEXT, 1, (LPARAM)szInfo);
}
//========================================================================
BOOL LoadMaps()
{
	WIN32_FIND_DATAA wfd = {0};
	HANDLE hFind = INVALID_HANDLE_VALUE;
	char filePath[MAX_PATH] = {0};
	BOOL fOk = FALSE;
	char *ext = 0;

	__try
	{
		hFind = FindFirstFileA("Map\\*", &wfd);
		if (hFind == INVALID_HANDLE_VALUE)
			__leave;
		
		do
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;
			
			ext = PathFindExtensionA(wfd.cFileName);
			if (*ext && StrCmpIA(ext, ".map") == 0)
			{
				StringCchPrintfA(filePath, MAX_PATH, "Map\\%s", wfd.cFileName);
				
				ReadMaps(filePath, &mapList, &nTotalGates);
			}
			
		}while (FindNextFileA(hFind, &wfd));

		if (nTotalGates > 0){
			fOk = TRUE;
		}
	}

	__finally{
		FindClose(hFind);
	}

	return fOk;
}
//========================================================================
void doBombKey()
{
	if (curMap.bomb < 1)
		return ;
	
	fUseBomb = !fUseBomb;
	
	if (fUseBomb)
	{
		if (nBombX == -1 || nBombY == -1)//还没初始化过炸弹
		{
			if (GetPos(&nBombX, &nBombY, MAP_WALL) == -1)//没墙？不太可能吧
			{
				nBombX = nBombY = -1;
				return ;
			}
		}
		
		SetObject(nBombX, nBombY, MAP_BOMB);
		Refresh(nBombX, nBombY);
		
		SetTimer(hwndMain, TIMER_BOMB_ID, 500, NULL);
	}
	else
	{
		SetObject(nBombX, nBombY, prevBombObject);
		Refresh(nBombX, nBombY);
		
		if (prevBombObject != MAP_WALL)
		{
			nBombX = nBombY = -1;
			prevBombObject = MAP_WALL;
		}
		
		KillTimer(hwndMain, TIMER_BOMB_ID);
	}
}
//========================================================================
void doOpen()
{
	char szFilePath[MAX_PATH] = {0};
	DWORD dwCount = 0;

	if (!SelFilePath(TRUE, szFilePath, MAX_PATH))
		return ;

	if (!ReadMaps(szFilePath, &mapList, &dwCount)){
		MessageBox(hwndMain, TEXT("加载地图失败了!"), szCaption, MB_ICONERROR);
	}
	else
	{
		nTotalGates += dwCount;

		if (nCurrentGate == -1){
			NewGate(0);
		}

		if (nTotalGates > 1 && nCurrentGate != nTotalGates - 1)
		{
			SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_NEXT, MAKELONG(TRUE, 0));
			EnableMenuItem(GetMenu(hwndMain), IDM_NEXT, MF_ENABLED);

			SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_JUMP, MAKELONG(TRUE, 0));
			EnableMenuItem(GetMenu(hwndMain), IDM_JUMP, MF_ENABLED);
		}
	}
}
//========================================================================