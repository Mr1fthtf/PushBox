
#pragma once

#ifndef _BOX_MAP_H_
#define _BOX_MAP_H_

#ifndef BOXMAPAPI
#define BOXMAPAPI __declspec(dllimport)
#endif

#define BMP_WIDTH		30		//位图大小
#define BMP_HEIGHT		30

#include "MapInfo.h"

//画图形
BOXMAPAPI void DrawObject(HDC hdcDest, HDC hdcSrc, int x, int y, MapType type);

//读取地图文件
BOXMAPAPI BOOL ReadMaps(LPCSTR filePath, PMAPLIST *list, DWORD *dwCount);

//取得第i位置的地图, 从0计起
BOXMAPAPI MAPINFO *GetMapAt(MAPLIST *list, int i);

//将pcMi加到list链表
BOXMAPAPI void InsertMap(PMAPLIST *list, MAPINFO *pMi);

//释放地图内存
BOXMAPAPI void FreeMaps(PMAPLIST *list);

//取得DLL里的图片资源
BOXMAPAPI HBITMAP GetBackBitmap();

//选择一个文件路径
BOXMAPAPI BOOL SelFilePath(BOOL fOpen, LPSTR pszPath, DWORD dwMax);

#endif