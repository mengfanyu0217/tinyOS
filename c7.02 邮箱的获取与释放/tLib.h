#ifndef TLIB_H
#define TLIB_H

#include <stdint.h>

typedef struct//位图数据结构
{
	uint32_t bitmap;
}tBitmap;

void tBitmapInit (tBitmap * bitmap);//位图初始化
uint32_t tBitmapPosCount (void);//获取bitmap标志位的数量
void tBitmapSet (tBitmap * bitmap, uint32_t pos);//设置位图和设置哪一位
void tBitmapClear (tBitmap * bitmap, uint32_t pos);//清空位图和清空哪一位
uint32_t tBitmapGetFirstSet (tBitmap * bitmap);//查找第一个置位

typedef struct _tNode
{
	struct _tNode * preNode;
	struct _tNode * nextNode;
}tNode;

void tNodeInit (tNode * node);

typedef struct _tList
{
	tNode headNode;
	uint32_t nodeCount;
}tList;

#define tNodeParent(node, parent, name) (parent *)((uint32_t)node - (uint32_t)&((parent *)0)->name)

void tListInit (tList * list);
uint32_t tListCount (tList * list);
tNode * tListFirst (tList * list);	
tNode * tListLast (tList * list);	
tNode * tListPre (tList * list, tNode * node);
tNode * tListNext (tList * list, tNode * node);
void tListRemoveAll (tList * list);
void tListAddFirst (tList * list, tNode * node);
void tListAddLast (tList * list, tNode * node);
tNode * tListRemoveFirst (tList * list);
void tListInsertAfter (tList * list, tNode * nodeAfter, tNode * nodeToInsert);
void tListRemove (tList * list, tNode * node);

#endif
