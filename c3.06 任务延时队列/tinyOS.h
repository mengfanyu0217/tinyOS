#ifndef TINYOS_H
#define TINYOS_H

#include <stdint.h>  //�����˳������������͵Ķ��壬����uint32_t
#include "tLib.h"
#include "tConfig.h"

#define TINYOS_TASK_STATE_RDY            0
#define TINYOS_TASK_STATE_DELAYED        (1 << 1)

typedef uint32_t tTaskStack;//�����ջ��Ԫ������
typedef struct _tTask  //��������ṹ
{
	tTaskStack * stack;//������ṹ��ֻ��Ҫ����һ��ָ���ջ�����ָ��
	uint32_t delayTicks;
	uint32_t prio;//��������ȼ�
	tNode delayNode;
	uint32_t state;//ָʾ�����Ƿ�����ʱ״̬
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
