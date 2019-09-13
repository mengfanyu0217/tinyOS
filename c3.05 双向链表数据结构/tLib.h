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

typedef struct _tNode//���Ķ���
{
	struct _tNode * preNode;//ǰ��ָ��
	struct _tNode * nextNode;//����ָ��
}tNode;

void tNodeInit (tNode * node);

typedef struct _tList//����Ķ���
{
	tNode headNode;//ͷ���
	uint32_t nodeCount;//����������ʾ�������ж��ٸ���Ч�Ľ��
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
