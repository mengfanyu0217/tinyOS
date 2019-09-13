#ifndef TINYOS_H
#define TINYOS_H

#include <stdint.h>  //�����˳������������͵Ķ��壬����uint32_t
#include "tLib.h"
#include "tConfig.h"
#include "tEvent.h"

typedef enum _tError
{
	tErrorNoError = 0,
}tError;

#define TINYOS_TASK_STATE_RDY            0
#define TINYOS_TASK_STATE_DESTROYED      (1 << 1)//ɾ��״̬λ
#define TINYOS_TASK_STATE_DELAYED        (1 << 2)
#define TINYOS_TASK_STATE_SUSPEND        (1 << 3)//����״̬λ

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
	void (*clean) (void * param);
	void * cleanParam;
	uint8_t requestDeleteFlag;
	
	tEvent * waitEvent;
	void * eventMsg;
	uint32_t waitEventResult;
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

typedef struct _tTaskInfo//�����ѯ�ṹ
{
	uint32_t delayTicks;
	uint32_t prio;
	uint32_t state;
	uint32_t slice;
	uint32_t suspendCount;
}tTaskInfo;

void tTaskInit (tTask * task, void (*entry)(void*), void * param, uint32_t prio, tTaskStack * stack);
void tSetSysTickPeriod (uint32_t ms);
void tInitApp (void);
void tTaskSuspend (tTask * task);
void tTaskWakeUp (tTask * task);
void tTimeTaskRemove (tTask * task);
void tTaskSchedRemove (tTask * task);
void tTaskSetClearCallFunc (tTask * task, void (*clean)(void * param), void * param);
void tTaskForceDelete (tTask * task);
void tTaskRequestDelete (tTask * task);
uint8_t tTaskIsRequestDelete (void);
void tTaskDeleteSelf (void);

void tTaskGetInfo (tTask * task, tTaskInfo * info);

#endif
