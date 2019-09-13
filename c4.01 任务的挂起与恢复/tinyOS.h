#ifndef TINYOS_H
#define TINYOS_H

#include <stdint.h>  //�����˳������������͵Ķ��壬����uint32_t
#include "tLib.h"
#include "tConfig.h"

#define TINYOS_TASK_STATE_RDY            0
#define TINYOS_TASK_STATE_DELAYED        (1 << 1)
#define TINYOS_TASK_STATE_SUSPEND        (1 << 2)//����״̬λ

typedef uint32_t tTaskStack;//�����ջ��Ԫ������
typedef struct _tTask  //��������ṹ
{
	tTaskStack * stack;//������ṹ��ֻ��Ҫ����һ��ָ���ջ�����ָ��
	tNode linkNode;
	uint32_t delayTicks;
	uint32_t prio;//��������ȼ�
	tNode delayNode;
	uint32_t state;//ָʾ�����Ƿ�����ʱ״̬
	uint32_t slice;//ʱ��Ƭ�ļ�����
	uint32_t suspendCount;//���������
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
void tTaskSchedRdy (tTask * task);
void tTaskSchedUnRdy (tTask * task);
void tTimeTaskWait (tTask * task, uint32_t ticks);
void tTimeTaskWakeUp (tTask * task);
void tTaskSystemTickHandler (void);
void tTaskDelay (uint32_t delay);
void tTaskInit (tTask * task, void (*entry)(void*), void * param, uint32_t prio, tTaskStack * stack);
void tSetSysTickPeriod (uint32_t ms);
void delay (void);
void tInitApp (void);
void tTaskSuspend (tTask * task);
void tTaskWakeUp (tTask * task);

#endif
