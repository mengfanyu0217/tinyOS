#ifndef TLIB_H
#define TLIB_H

#include <stdint.h>

typedef struct//λͼ���ݽṹ
{
	uint32_t bitmap;
}tBitmap;

void tBitmapInit (tBitmap * bitmap);//λͼ��ʼ��
uint32_t tBitmapPosCount (void);//��ȡbitmap��־λ������
void tBitmapSet (tBitmap * bitmap, uint32_t pos);//����λͼ��������һλ
void tBitmapClear (tBitmap * bitmap, uint32_t pos);//���λͼ�������һλ
uint32_t tBitmapGetFirstSet (tBitmap * bitmap);//���ҵ�һ����λ

#endif
