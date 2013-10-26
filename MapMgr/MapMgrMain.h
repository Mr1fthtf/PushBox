//////////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////////
#ifndef MAP_MGR_H
#define MAP_MGR_H
//////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
//////////////////////////////////////////////////////////////////////////
#include "resource.h"
//#include "../MapInfo.h"
#include "../BoxMap/BoxMap.h"
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* Just_Fancy															*/
/* Dec 16, 2010                                                         */
/************************************************************************/
#define DEF_CXWND		500		//窗口默认大小
#define DEF_CYWND		460
//=========================================================================
const TCHAR szClsName[] = TEXT("MapMgr");
const TCHAR szCaption[] = TEXT("推箱子 - 地图管理");
//=========================================================================
HINSTANCE	ghInst			= NULL;
HWND		hwndMain		= NULL;

HWND		hwndDlgMi		= NULL;		//地图信息对话框句柄
//=========================================================================
MAPINFO newMap	 = {0};					//当前编辑地图
MAPINFO oldMap	 = {0};					//

HBITMAP hBitmap	 = NULL;				//背景位图
HDC		hdcMem	 = NULL;				//
//=========================================================================
int nOffsetX = 0, nOffsetY = 0;			//地图相对于客户区(0, 0)的偏移

int oldX = -1, oldY = -1;				//记录上次鼠标经过的位置
MapType prevBombObject	= MAP_BACK;			//鼠标上次飘过的Object
MapType dragObject	= MAP_BACK;			//鼠标按住时所选中的Object
BOOL	fDragOk		= FALSE;			//标记是否点中了待选位图

char szOpenedFilePath[MAX_PATH] = {0};	//记录打开的文件

MAPLIST *mapList	 = 0;	//地图列表
int		nMapCount	 = 0;	//打开的地图文件里的地图数
int		nWhichOpened = -1;	//标记是哪个打开编辑了
//=========================================================================
LRESULT CALLBACK MapWndProc(HWND hWnd, UINT uMsg,
						 WPARAM wParam, LPARAM lParam);

INT_PTR CALLBACK MapInfoDlgProc(HWND hDlg, UINT uMsg,
						 WPARAM wParam, LPARAM lParam);

INT_PTR CALLBACK MapListDlgProc(HWND hDlg, UINT uMsg,
						 WPARAM wParam, LPARAM lParam);

BOOL Register(HINSTANCE hInst);
BOOL InitWnd();
void CenterWnd(HWND hwndTo, HWND hwndParent);
//=========================================================================
void OnClose(HWND hWnd);
void OnPaint(HWND hWnd);
BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
void OnGetMinMaxInfo(HWND hWnd, LPMINMAXINFO lpMinMaxInfo);
void OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags);
void OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
void OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags);
void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify);
//=========================================================================
void PaintLeft(HDC hdc, int offsetY, LPCRECT pRect);	//在窗口左边画出预选图片
void Refresh(int x, int y);	//刷新(x, y)位置的区域, x, y为地图行列; 当两者之中一个为-1时刷新整个客户区
BOOL IsMapValid();			//简单判断所编辑地图是否有效
//=========================================================================
void doOpen();
void doSave();				//保存当前编辑地图
//=========================================================================

#endif