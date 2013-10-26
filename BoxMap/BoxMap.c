
#include <Windows.h>

#define BOXMAPAPI __declspec(dllexport)

#include "BoxMap.h"
#include "resource.h"

HINSTANCE ghInst = NULL;

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID lpReserved)
{
	ghInst = hInst;

	return TRUE;
}

void DrawObject(HDC hdcDest, HDC hdcSrc,  int x, int y, MapType type)
{
	int bmpOffset = BMP_WIDTH * (int)(type - MAP_BACK);

	BitBlt(hdcDest, x, y, BMP_WIDTH, BMP_HEIGHT, hdcSrc, bmpOffset, 0, SRCCOPY);
}

BOOL ReadMaps(LPCSTR filePath, PMAPLIST *list, DWORD *dwCount)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD dwRead = 0;
	MAPINFO *pMi;
	const DWORD dwMapSize = sizeof(MAPINFO);
	BOOL fOk = FALSE;

	__try
	{
		hFile = CreateFileA(filePath, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile == INVALID_HANDLE_VALUE)
			__leave;

		dwRead = GetFileSize(hFile, NULL);
		if (dwRead < dwMapSize){
			__leave;
		}else{
			*dwCount = dwRead / dwMapSize;
		}

		do
		{
			pMi = (MAPINFO *)calloc(1, dwMapSize);

			if (!ReadFile(hFile, pMi, dwMapSize, &dwRead, NULL)){
				__leave;
			}else if (dwRead == dwMapSize){
				pMi->next = 0;
				InsertMap(list, pMi);
			}

		}while(dwRead == dwMapSize);

		fOk = TRUE;

	}
	__finally
	{
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
	}

	return fOk;
}
//都先当单向链表吧，双向还不太熟悉
MAPINFO *GetMapAt(MAPLIST *list, int i)
{
	int j = 0;
	MAPINFO *p = 0;

	if (!list || i<0){
		return NULL;
	}

	p = list/*->next*/;

	while (p /*!= list*/ && j < i)
	{
		p = p->next;
		j++;
	}

	if (p /*!= list*/ && j > i)
		return NULL;

	return p;
}

void InsertMap(PMAPLIST *list, MAPINFO *pMi)
{
	MAPINFO *p = NULL, *q = NULL;

	if (!*list)
	{
		pMi->next = NULL;
		*list = pMi;
		(*list)->prev = NULL;
		(*list)->next = NULL;

		return ;
	}
	
	//p = (*list)->next;
	p = (*list);
	while(p/* != *list*/)
	{
		q = p;
		p = p->next;
	}
	
	if (q)
	{
		pMi->next = NULL;
		q->next = pMi;
	/*	pMi->prev = q->prev;
		q->prev->next = pMi;
		pMi->next = q;
		q->prev = pMi;
	*/
	}
}

void FreeMaps(PMAPLIST *list)
{
	MAPLIST *prev = 0;

	while(*list)
	{
		prev = *list;
		*list = (*list)->next;

		free(prev);
	}

	*list = 0;
}

HBITMAP GetBackBitmap()
{
	return LoadBitmap(ghInst, MAKEINTRESOURCE(IDB_BACK));
}

BOOL SelFilePath(BOOL fOpen, LPSTR pszPath, DWORD dwMax)
{
	OPENFILENAMEA ofn = {0};

	ofn.lStructSize = sizeof(OPENFILENAMEA);

	ofn.Flags		= OFN_SHOWHELP;
	ofn.hInstance	= NULL;
	ofn.hwndOwner	= NULL;
	ofn.lpstrFile	= pszPath;
	ofn.nMaxFile	= dwMax;
	ofn.lpstrFilter	= "地图文件(*.map)\0*.map\0所有文件(*.*)\0*.*\0\0";

	if (fOpen){
		ofn.lpstrTitle	= "请选择地图文件";
	}else{
		ofn.lpstrTitle	= "保存地图文件";
	}

	if (fOpen){
		return GetOpenFileNameA(&ofn);
	}else{
		return GetSaveFileNameA(&ofn);
	}
}