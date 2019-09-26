#ifndef TINYOS_H
#define TINYOS_H

#include <stdint.h>  //包含了常见的数据类型的定义，比如uint32_t
#include "tLib.h"
#include "tConfig.h"
#include "tEvent.h"
#include "tTask.h"
#include "tSem.h"

typedef enum _tError
{
	tErrorNoError = 0,
	tErrorTimeout,
	tErrorResourceUnvaliable,
}tError;

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

void tSetSysTickPeriod (uint32_t ms);
void tInitApp (void);
void tTimeTaskRemove (tTask * task);
void tTaskSchedRemove (tTask * task);

#endif
