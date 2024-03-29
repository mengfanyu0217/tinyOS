#ifndef TINYOS_H
#define TINYOS_H

#include <stdint.h>  //包含了常见的数据类型的定义，比如uint32_t

typedef uint32_t tTaskStack;//定义堆栈单元的类型
typedef struct _tTask  //定义任务结构
{
	tTaskStack * stack;//在任务结构中只需要定义一个指向堆栈任务的指针
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
