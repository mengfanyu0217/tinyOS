#ifndef TINYOS_H
#define TINYOS_H

#include <stdint.h>  //�����˳������������͵Ķ��壬����uint32_t
#include "tLib.h"

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

void tTaskSched (void);
void tTaskSchedInit (void);
void tTaskSchedDisable (void);
void tTaskSchedEnable (void);

#endif
