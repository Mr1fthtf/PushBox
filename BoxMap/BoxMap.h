
#pragma once

#ifndef _BOX_MAP_H_
#define _BOX_MAP_H_

#ifndef BOXMAPAPI
#define BOXMAPAPI __declspec(dllimport)
#endif

#define BMP_WIDTH		30		//λͼ��С
#define BMP_HEIGHT		30

#include "MapInfo.h"

//��ͼ��
BOXMAPAPI void DrawObject(HDC hdcDest, HDC hdcSrc, int x, int y, MapType type);

//��ȡ��ͼ�ļ�
BOXMAPAPI BOOL ReadMaps(LPCSTR filePath, PMAPLIST *list, DWORD *dwCount);

//ȡ�õ�iλ�õĵ�ͼ, ��0����
BOXMAPAPI MAPINFO *GetMapAt(MAPLIST *list, int i);

//��pcMi�ӵ�list����
BOXMAPAPI void InsertMap(PMAPLIST *list, MAPINFO *pMi);

//�ͷŵ�ͼ�ڴ�
BOXMAPAPI void FreeMaps(PMAPLIST *list);

//ȡ��DLL���ͼƬ��Դ
BOXMAPAPI HBITMAP GetBackBitmap();

//ѡ��һ���ļ�·��
BOXMAPAPI BOOL SelFilePath(BOOL fOpen, LPSTR pszPath, DWORD dwMax);

#endif