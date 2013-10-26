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
#define NUM_BUTTONS		6		//������ͼ��λͼ�а�����ͼ�����

#define TIMER_GAME_ID	1		//��Ϸ��ʱID
#define TIMER_BOMB_ID	2		//ը��
//========================================================================
const TCHAR szClsName[] = TEXT("PushBox");
const TCHAR szCaption[] = TEXT("������");
//========================================================================
DWORD dwStatusWidth[] = {80, 160, 255, -1};	//״̬��������С

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

HWND hwndMain	 = NULL;	// �����ھ��
HWND hwndToolbar = NULL;	// ���������
HWND hwndStatus	 = NULL;	// ״̬�����
//========================================================================
HBITMAP hBitmap	 = NULL;	// ����λͼ���
HDC		hdcMem	 = NULL;

int		nCurrentGate = -1;				//��ǰ�ؿ�, ��0����
int		nTotalGates	 = 0;				//���ж��ٹؿ�

MAPLIST *mapList	 = NULL;			//���е�ͼ�б�
MAPLIST oriMap		 = {0};				//ԭ����ͼ
MAPINFO curMap		 = {0};				//��ǰ��ͼ

int		nStepCount	 = 0;				//��¼���߲���
DWORD	dwTimeBegin	 = 0;				//��ǰ�ؿ���ʼʱ��
DWORD	dwTimePause	 = 0;				//��ͣ�˶೤���룩
BOOL	fGamePaused	 = FALSE;			//��Ϸ�Ƿ�����ͣ״̬

BOOL	fUseBomb	 = FALSE;			//�Ƿ�ʹ��ը��
int		nBombX = -1, nBombY = -1;		//��¼ը��λ��
MapType prevBombObject = MAP_WALL;		//��¼ը��ȡ��λ��ԭ����ͼ

typedef struct _STEP_RECORD
{
	int manOldX;	//����ԭ��λ��
	int manOldY;
	int manNewX;	//��λ��
	int manNewY;
	MapType manNewObject;	//����ȡ����λ��ԭ����ͼ

	BOOL fBoxMoved;	// ����Ƿ��ƶ�������
	int boxOldX;
	int boxOldY;
	MapType oldBoxObject;	//ԭ�����ӵ�ͼ��
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
void doOpen();		//�򿪵�ͼ�ļ�
void doBombKey();	//������B
//========================================================================
void	NewGate(int nGate);								//�¹ؿ�
int		GetPos(int *x, int *y, MapType type);			//���ص�һ��type��λ�ã�����-1
MapType	GetType(int x, int y);							//����(x, y)��ͼƬ����
void	SetObject(int x, int y, MapType type);			//����(x, y)��ͼƬ
void	MoveMan(int oldX, int oldY, int newX, int newY);//�ƶ�����
BOOL	MoveBox(int x, int y, UINT uKey);				//�ƶ�����, ����TRUE��ʾ���ƶ�
void	MoveBomb(int oldX, int oldY, int newX, int newY);//�ƶ�ը��
BOOL	DoneGame();				//�Ƿ������
void	Refresh(int x, int y);	//����(x, y)����
//========================================================================
void	ShowStep();				//��״̬����ʾ���߲���
void	ShowTime();				//��ʾ����ʱ��
void	ShowBomb();				//��ʾը����
//========================================================================
BOOL	LoadMaps();				//���ص�ͼ
//========================================================================
//////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////