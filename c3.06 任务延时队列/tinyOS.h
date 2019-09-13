#ifndef TINYOS_H
#define TINYOS_H

#include <stdint.h>  //包含了常见的数据类型的定义，比如uint32_t
#include "tLib.h"
#include "tConfig.h"

#define TINYOS_TASK_STATE_RDY            0
#define TINYOS_TASK_STATE_DELAYED        (1 << 1)

typedef uint32_t tTaskStack;//定义堆栈单元的类型
typedef struct _tTask  //定义任务结构
{
	tTaskStack * stack;//在任务结构中只需要定义一个指向堆栈任务的指针
	uint32_t delayTicks;
	uint32_t prio;//任务的优先级
	tNode delayNode;
	uint32_t state;//指示任务是否处于延时状态
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
