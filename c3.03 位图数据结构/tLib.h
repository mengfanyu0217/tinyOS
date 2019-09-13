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

#endif
