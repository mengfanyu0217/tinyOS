/**************************************************************
 * 设计目标：任务状态查询
**************************************************************/

#include "tinyOS.h"
#include "ARMCM3.h"

tTask * currentTask;
tTask * nextTask;
tTask * idleTask;

tBitmap taskPrioBitmap;//优先级位图
tList taskTable[TINYOS_PRO_COUNT];//所有任务的指针数组

uint8_t schedLockCount;//调度锁计数器
tList tTaskDelayedList;//定义延时队列

tTask * tTaskHighestReady (void)
{
	uint32_t highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);
	tNode * node = tListFirst(&taskTable[highestPrio]);
	return (tTask *)tNodeParent(node, tTask, linkNode);
}

void tTaskSchedInit (void)//调度锁初始化
{
	int i;
	schedLockCount = 0;
	tBitmapInit(&taskPrioBitmap);
	for (i = 0; i < TINYOS_PRO_COUNT; i++)
		tListInit(&taskTable[i]);
}

void tTaskSchedDisable (void)//上锁函数
{
	uint32_t status = tTaskEnterCritical();
	if (schedLockCount < 255)
		schedLockCount++;
	tTaskExitCritical(status);
}

void tTaskSchedEnable (void)//解锁函数
{
	uint32_t status = tTaskEnterCritical();
	if (schedLockCount > 0)
		if (--schedLockCount == 0)
			tTaskSched();
	tTaskExitCritical(status);
}

void tTaskSchedRdy (tTask * task)
{
	tListAddFirst(&taskTable[task->prio], &(task->linkNode));
	tBitmapSet(&taskPrioBitmap, task->prio);
}

void tTaskSchedUnRdy (tTask * task)
{
	tListRemove(&taskTable[task->prio], &(task->linkNode));
	if (tListCount(&taskTable[task->prio]) == 0)
		tBitmapClear(&taskPrioBitmap, task->prio);
}

void tTaskSchedRemove (tTask * task)
{
	tListRemove(&taskTable[task->prio], &(task->linkNode));
	if (tListCount(&taskTable[task->prio]) == 0)
		tBitmapClear(&taskPrioBitmap, task->prio);
}

void tTaskSched (void) //任务调度函数
{
	tTask * tempTask;
	uint32_t status = tTaskEnterCritical();//添加临界区保护
	if (schedLockCount > 0)//判断调度器是否上锁
	{
		tTaskExitCritical(status);
		return;
	}
	
	//找到优先级最高的任务，如果其优先级比当前任务的优先级还高，那么就切换到这个任务
	tempTask = tTaskHighestReady();
	if (tempTask != currentTask)
	{
		nextTask = tempTask;
		tTaskSwitch();
	}
	
	tTaskExitCritical(status);
}

void tTaskDelayedInit (void)//延时队列初始化
{
	tListInit(&tTaskDelayedList);
}

void tTimeTaskWait (tTask * task, uint32_t ticks)
{
	task->delayTicks = ticks;
	tListAddFirst(&tTaskDelayedList, &(task->delayNode));
	task->state |= TINYOS_TASK_STATE_DELAYED;
}

void tTimeTaskWakeUp (tTask * task)
{
	tListRemove(&tTaskDelayedList, &(task->delayNode));
	task->state &= ~TINYOS_TASK_STATE_DELAYED;
}

void tTimeTaskRemove (tTask * task)
{
	tListRemove(&tTaskDelayedList, &(task->delayNode));
}

void tTaskSystemTickHandler (void)
{
	tNode * node;
	uint32_t status = tTaskEnterCritical();//添加临界区保护
	for (node = tTaskDelayedList.headNode.nextNode; node != &(tTaskDelayedList.headNode); node = node->nextNode)
	{
		tTask * task = tNodeParent(node, tTask, delayNode);
		if (--task->delayTicks == 0)
		{
			tTimeTaskWakeUp(task);
			tTaskSchedRdy(task);
		}		
	}
	
	if (--currentTask->slice == 0)//判断当前任务的时间片是否已经用完
		if (tListCount(&taskTable[currentTask->prio]) > 0)//如果已经用完了，并且还有其他任务，进行切换轮转
		{
			tListRemoveFirst(&taskTable[currentTask->prio]);
			tListAddLast(&taskTable[currentTask->prio], &(currentTask->linkNode));
			currentTask->slice = TINYOS_SLICE_MAX;
		}
	
	tTaskExitCritical(status);
	tTaskSched();
}

//空闲任务
tTask tTaskIdle;
tTaskStack idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE];

void idleTaskEntry (void * param)
{
	for(;;)
	{
		;
	}
}

int main()
{
	tTaskSchedInit();
	tTaskDelayedInit();
	
	tInitApp();

	tTaskInit(&tTaskIdle, idleTaskEntry, (void *)0, TINYOS_PRO_COUNT - 1, &idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE]);
	idleTask = &tTaskIdle;
	
	nextTask = tTaskHighestReady();
		
	tTaskRunFirst();//触发PendSVC进入异常处理函数中
	
	return 0;
}


//思考：
