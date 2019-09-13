#include "tinyOS.h"

void tTaskInit (tTask * task, void (*entry)(void*), void * param, uint32_t prio, tTaskStack * stack)
{
	//进入PendSV和退出PendSV时硬件自动保存的寄存器，和PendSV退出时寄存器恢复顺序一致。
	//这部分的值在任务恢复时运行，在退出PendSVC时弹出。
	//这个顺序要和退出异常时硬件自动弹栈的顺序一致，否则会将相应的值恢复到错误的寄存器。
	*(--stack) = (unsigned long)(1 << 24);//xPSR寄存器
	*(--stack) = (unsigned long)entry;//R15寄存器的值，也是PC寄存器的值，程序的入口地址
	*(--stack) = (unsigned long)0x14;//R14寄存器的值，也是LR寄存器的值，未使用，用数字14填充即可
	*(--stack) = (unsigned long)0x12;//R12寄存器的值，未使用，用数字12填充即可
	*(--stack) = (unsigned long)0x3;//R3寄存器的值，未使用，用数字3填充即可
	*(--stack) = (unsigned long)0x2;//R2寄存器的值，未使用，用数字2填充即可
	*(--stack) = (unsigned long)0x1;//R1寄存器的值，未使用，用数字1填充即可
	*(--stack) = (unsigned long)param;//R0寄存器的值，R0用来保存入口参数
	
	//下半部分与PendSVC中的LDMIA指令有关，使用LDMIA R0!, {R4-R11}时会自动按一定顺序从内存中加载值到R4-R11中，为了保证加载正确，顺序要一致。
	//PendSV发生时未自动保存的内核寄存器（R4-R11），需要手动保存
	*(--stack) = (unsigned long)0x11;//R11寄存器的值，未使用，用数字11填充即可
	*(--stack) = (unsigned long)0x10;//R10寄存器的值，未使用，用数字10填充即可
	*(--stack) = (unsigned long)0x9;//R9寄存器的值，未使用，用数字9填充即可
	*(--stack) = (unsigned long)0x8;//R8寄存器的值，未使用，用数字8填充即可
	*(--stack) = (unsigned long)0x7;//R7寄存器的值，未使用，用数字7填充即可
	*(--stack) = (unsigned long)0x6;//R6寄存器的值，未使用，用数字6填充即可
	*(--stack) = (unsigned long)0x5;//R5寄存器的值，未使用，用数字5填充即可
	*(--stack) = (unsigned long)0x4;//R4寄存器的值，未使用，用数字4填充即可
	
	task->slice = TINYOS_SLICE_MAX;
	
	task->stack = stack;//保存最终的值
	task->delayTicks = 0;
	task->prio = prio;
	task->state = TINYOS_TASK_STATE_RDY;
	task->suspendCount = 0;
	task->clean = (void (*)(void*))0;
	task->cleanParam = (void *)0;
	task->requestDeleteFlag = 0;
	
	tNodeInit(&(task->delayNode));//延时结点初始化
	tNodeInit(&(task->linkNode));

	tTaskSchedRdy(task);
}

void tTaskSuspend (tTask * task)//任务挂起函数
{
	uint32_t status = tTaskEnterCritical();//添加临界区保护
	if (!(task->state & TINYOS_TASK_STATE_SUSPEND))//检查任务是否处于延时状态，若在延时状态，则不能挂起
		if (++task->suspendCount <= 1)//若是第一次挂起
		{
			task->state |= TINYOS_TASK_STATE_SUSPEND;//则需将任务的状态位设置为挂起状态
			tTaskSchedUnRdy(task);//再将任务从就绪表中移除
			if (task == currentTask)//判断这个任务是不是当前任务
				tTaskSched();//若是当前任务，则需切换到其他任务运行
		}
	tTaskExitCritical(status);
}

void tTaskWakeUp (tTask * task)//任务恢复函数
{
	uint32_t status = tTaskEnterCritical();//添加临界区保护
	if (task->state & TINYOS_TASK_STATE_SUSPEND)//判断任务是否处于挂起状态
		if (--task->suspendCount == 0)
		{
			task->state &= ~TINYOS_TASK_STATE_SUSPEND;
			tTaskSchedRdy(task);
			tTaskSched();
		}
	tTaskExitCritical(status);
}

void tTaskSetClearCallFunc (tTask * task, void (*clean)(void * param), void * param)
{
	task->clean = clean;
	task->cleanParam = param;
}

void tTaskForceDelete (tTask * task)
{
	uint32_t status = tTaskEnterCritical();//添加临界区保护
	if (task->state & TINYOS_TASK_STATE_DELAYED)
		tTimeTaskRemove(task);
	else if (!(task->state & TINYOS_TASK_STATE_SUSPEND))
		tTaskSchedRemove(task);
	
	if (task->clean)
		task->clean(task->cleanParam);

	if (currentTask == task)
		tTaskSched();

	tTaskExitCritical(status);
}

void tTaskRequestDelete (tTask * task)
{
	uint32_t status = tTaskEnterCritical();//添加临界区保护
	task->requestDeleteFlag = 1;
	tTaskExitCritical(status);
}

uint8_t tTaskIsRequestDelete (void)
{
	uint8_t delete;
	uint32_t status = tTaskEnterCritical();//添加临界区保护
	delete = currentTask->requestDeleteFlag;
	tTaskExitCritical(status);
	return delete;
}

void tTaskDeleteSelf (void)
{
	uint32_t status = tTaskEnterCritical();//添加临界区保护
	tTaskSchedRemove(currentTask);
	if (currentTask->clean)
		currentTask->clean(currentTask->cleanParam);
	tTaskSched();	
	tTaskExitCritical(status);
}

void tTaskGetInfo (tTask * task, tTaskInfo * info)
{
	uint32_t status = tTaskEnterCritical();//添加临界区保护

	info->delayTicks = task->delayTicks;
	info->prio = task->prio;
	info->state = task->state;
	info->slice = task->slice;
	info->suspendCount = task->suspendCount;
	
	tTaskExitCritical(status);
}


