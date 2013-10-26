//////////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////////
#ifndef _WIN_MAIN_H_
#define _WIN_MAIN_H_
//////////////////////////////////////////////////////////////////////////
#include <Windows.h>
#include <tchar.h>
#include <commctrl.h>

#include "../BoxMap/BoxMap.h"
#include "resource.h"
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* Just_Fancy															*/
/* Dec 16, 2010                                                         */
/************************************************************************/
//========================================================================
#define NUM_BUTTONS		6		//工具栏图标位图中包含的图标个数

#define TIMER_GAME_ID	1		//游戏用时ID
#define TIMER_BOMB_ID	2		//炸弹
//========================================================================
const TCHAR szClsName[] = TEXT("PushBox");
const TCHAR szCaption[] = TEXT("推箱子");
//========================================================================
DWORD dwStatusWidth[] = {80, 160, 255, -1};	//状态栏分区大小

TBBUTTON tbs[] = {
	{0, IDM_REPLAY,	 TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{1, IDM_PREV,	 TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{2, IDM_NEXT,	 TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{3, IDM_ABOUT,	 TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{0, 0,			 TBSTATE_ENABLED, TBSTYLE_SEP,	  0, 0 },
	{4, IDM_EXIT,	 TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 }
};
//========================================================================
HINSTANCE ghInst = NULL;

HWND hwndMain	 = NULL;	// 主窗口句柄
HWND hwndToolbar = NULL;	// 工具栏句柄
HWND hwndStatus	 = NULL;	// 状态栏句柄
//========================================================================
HBITMAP hBitmap	 = NULL;	// 背景位图句柄
HDC		hdcMem	 = NULL;

int		nCurrentGate = -1;				//当前关卡, 从0计起
int		nTotalGates	 = 0;				//共有多少关卡

MAPLIST *mapList	 = NULL;			//所有地图列表
MAPLIST oriMap		 = {0};				//原来地图
MAPINFO curMap		 = {0};				//当前地图

int		nStepCount	 = 0;				//记录已走步数
DWORD	dwTimeBegin	 = 0;				//当前关卡开始时间
DWORD	dwTimePause	 = 0;				//暂停了多长（秒）
BOOL	fGamePaused	 = FALSE;			//游戏是否是暂停状态

BOOL	fUseBomb	 = FALSE;			//是否使用炸弹
int		nBombX = -1, nBombY = -1;		//记录炸弹位置
MapType prevBombObject = MAP_WALL;		//记录炸弹取代位置原来的图

typedef struct _STEP_RECORD
{
	int manOldX;	//人物原来位置
	int manOldY;
	int manNewX;	//新位置
	int manNewY;
	MapType manNewObject;	//人物取代新位置原来的图

	BOOL fBoxMoved;	// 标记是否移动了箱子
	int boxOldX;
	int boxOldY;
	MapType oldBoxObject;	//原来箱子的图形
	int boxNewX;
	int boxNewY;
	MapType boxNewObject;
}RECORD;

RECORD lastRecord = {0};
//========================================================================
LRESULT CALLBACK MapWndProc(HWND hWnd, UINT uMsg,
						 WPARAM wParam, LPARAM lParam);

INT_PTR CALLBACK DlgAboutProc(HWND hDlg, UINT uMsg,
							  WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DlgGotoProc(HWND hDlg, UINT uMsg,
							  WPARAM wParam, LPARAM lParam);
//========================================================================
BOOL Register(HINSTANCE hInst);
BOOL InitWnd();
void CenterWnd(HWND hwndTo, HWND hwndParent);
//========================================================================
void OnPaint(HWND hWnd);
BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify);
void OnClose(HWND hWnd);
LRESULT OnNotify(HWND hWnd, int id, NMHDR *pnmhdr);
void OnKeydown(HWND hWnd, UINT uKey, BOOL fDown, int cRepeat, UINT flags);
void OnTimer(HWND hWnd, UINT uID);
void OnSysCommand(HWND hWnd, UINT cmd, int x, int y);
//========================================================================
void doOpen();		//打开地图文件
void doBombKey();	//处理按键B
//========================================================================
void	NewGate(int nGate);								//新关卡
int		GetPos(int *x, int *y, MapType type);			//返回第一个type的位置，无则-1
MapType	GetType(int x, int y);							//返回(x, y)的图片类型
void	SetObject(int x, int y, MapType type);			//设置(x, y)的图片
void	MoveMan(int oldX, int oldY, int newX, int newY);//移动人物
BOOL	MoveBox(int x, int y, UINT uKey);				//移动箱子, 返回TRUE表示可移动
void	MoveBomb(int oldX, int oldY, int newX, int newY);//移动炸弹
BOOL	DoneGame();				//是否已完成
void	Refresh(int x, int y);	//更新(x, y)区域
//========================================================================
void	ShowStep();				//在状态栏显示已走步数
void	ShowTime();				//显示已用时间
void	ShowBomb();				//显示炸弹数
//========================================================================
BOOL	LoadMaps();				//加载地图
//========================================================================
//////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////