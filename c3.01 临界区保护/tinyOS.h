#ifndef TINYOS_H
#define TINYOS_H

#include <stdint.h>  //�����˳������������͵Ķ��壬����uint32_t

typedef uint32_t tTaskStack;//�����ջ��Ԫ������
typedef struct _tTask  //��������ṹ
{
	tTaskStack * stack;//������ṹ��ֻ��Ҫ����һ��ָ���ջ�����ָ��
	uint32_t delayTicks;
}tTask;

extern tTask * currentTask;
extern tTask * nextTask;

void tTaskRunFirst(void);
void tTaskSwitch(void);

uint32_t tTaskEnterCritical (void);
void tTaskExitCritical (uint32_t status);

#endif
