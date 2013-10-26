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
#define DEF_CXWND		500		//����Ĭ�ϴ�С
#define DEF_CYWND		460
//=========================================================================
const TCHAR szClsName[] = TEXT("MapMgr");
const TCHAR szCaption[] = TEXT("������ - ��ͼ����");
//=========================================================================
HINSTANCE	ghInst			= NULL;
HWND		hwndMain		= NULL;

HWND		hwndDlgMi		= NULL;		//��ͼ��Ϣ�Ի�����
//=========================================================================
MAPINFO newMap	 = {0};					//��ǰ�༭��ͼ
MAPINFO oldMap	 = {0};					//

HBITMAP hBitmap	 = NULL;				//����λͼ
HDC		hdcMem	 = NULL;				//
//=========================================================================
int nOffsetX = 0, nOffsetY = 0;			//��ͼ����ڿͻ���(0, 0)��ƫ��

int oldX = -1, oldY = -1;				//��¼�ϴ���꾭����λ��
MapType prevBombObject	= MAP_BACK;			//����ϴ�Ʈ����Object
MapType dragObject	= MAP_BACK;			//��갴סʱ��ѡ�е�Object
BOOL	fDragOk		= FALSE;			//����Ƿ�����˴�ѡλͼ

char szOpenedFilePath[MAX_PATH] = {0};	//��¼�򿪵��ļ�

MAPLIST *mapList	 = 0;	//��ͼ�б�
int		nMapCount	 = 0;	//�򿪵ĵ�ͼ�ļ���ĵ�ͼ��
int		nWhichOpened = -1;	//������ĸ��򿪱༭��
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
void PaintLeft(HDC hdc, int offsetY, LPCRECT pRect);	//�ڴ�����߻���ԤѡͼƬ
void Refresh(int x, int y);	//ˢ��(x, y)λ�õ�����, x, yΪ��ͼ����; ������֮��һ��Ϊ-1ʱˢ�������ͻ���
BOOL IsMapValid();			//���ж����༭��ͼ�Ƿ���Ч
//=========================================================================
void doOpen();
void doSave();				//���浱ǰ�༭��ͼ
//=========================================================================

#endif