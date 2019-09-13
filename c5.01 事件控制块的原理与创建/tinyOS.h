#ifndef TINYOS_H
#define TINYOS_H

#include <stdint.h>  //包含了常见的数据类型的定义，比如uint32_t
#include "tLib.h"
#include "tConfig.h"
#include "tEvent.h"

typedef enum _tError
{
	tErrorNoError = 0,
}tError;

#define TINYOS_TASK_STATE_RDY            0
#define TINYOS_TASK_STATE_DESTROYED      (1 << 1)//删除状态位
#define TINYOS_TASK_STATE_DELAYED        (1 << 2)
#define TINYOS_TASK_STATE_SUSPEND        (1 << 3)//挂起状态位

typedef uint32_t tTaskStack;//定义堆栈单元的类型
typedef struct _tTask  //定义任务结构
{
	tTaskStack * stack;//在任务结构中只需要定义一个指向堆栈任务的指针
	tNode linkNode;
	uint32_t delayTicks;
	uint32_t prio;//任务的优先级
	tNode delayNode;
	uint32_t state;//指示任务是否处于延时状态
	uint32_t slice;//时间片的计数器
	uint32_t suspendCount;//挂起计数器
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

typedef struct _tTaskInfo//任务查询结构
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
