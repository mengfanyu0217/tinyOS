/**************************************************************
 * 设计目标：双向链表结构
**************************************************************/

#include "tinyOS.h"
#include "ARMCM3.h"

tTask * currentTask;
tTask * nextTask;
tTask * idleTask;

tBitmap taskPrioBitmap;//优先级位图
tTask * taskTable[TINYOS_PRO_COUNT];//所有任务的指针数组

uint8_t schedLockCount;//调度锁计数器

//对任务堆栈初始化
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
	
	task->stack = stack;//保存最终的值
	task->delayTicks = 0;
	task->prio = prio;
	
	taskTable[prio] = task;
	tBitmapSet(&taskPrioBitmap, prio);
}

tTask * tTaskHighestReady (void)
{
	uint32_t highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);
	return taskTable[highestPrio];
}

void tTaskSchedInit (void)//调度锁初始化
{
	schedLockCount = 0;
	tBitmapInit(&taskPrioBitmap);
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

void tTaskSystemTickHandler ()
{
	int i;
	uint32_t status = tTaskEnterCritical();//添加临界区保护
	for (i = 0; i < TINYOS_PRO_COUNT; i++)
	{
		if (taskTable[i]->delayTicks > 0)
			taskTable[i]->delayTicks--;
		else
			tBitmapSet(&taskPrioBitmap, i);
	}		
	tTaskExitCritical(status);
	tTaskSched();
}

void tTaskDelay (uint32_t delay)
{
	uint32_t status = tTaskEnterCritical();//添加临界区保护
	currentTask->delayTicks = delay;
	tBitmapClear(&taskPrioBitmap, currentTask->prio);
	tTaskExitCritical(status);
	tTaskSched();	
}

void tSetSysTickPeriod (uint32_t ms)
{
	SysTick->LOAD = ms * SystemCoreClock / 1000 - 1;
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Handler ()
{
	tTaskSystemTickHandler();
}

int task1Flag;
tList list;
tNode node[8];
void task1Entry (void * param)
{
	int i;
	tSetSysTickPeriod(10);
	
	tListInit(&list);
	for (i = 0; i < 8; i++)
	{
		tNodeInit(&node[i]);
		tListAddFirst(&list, &node[i]);	
	}
	
	for (i = 0; i < 8; i++)
		tListRemoveFirst(&list);
	
	for(;;)
	{
		task1Flag = 0;
		tTaskDelay(1);
		task1Flag = 1;
		tTaskDelay(1);
	}
}

int task2Flag;
void task2Entry (void * param)
{
	for(;;)
	{
		task2Flag = 0;
		tTaskDelay(1);
		task2Flag = 1;
		tTaskDelay(1);
	}
}

//定义两个任务
tTask tTask1;
tTask tTask2;
//为task1和task2配备两个独立的堆栈空间,堆栈空间大小为1024即可
tTaskStack task1Env[1024];
tTaskStack task2Env[1024];
//空闲任务
tTask tTaskIdle;
tTaskStack idleTaskEnv[1024];

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
	
	tTaskInit(&tTask1, task1Entry, (void *)0x11111111, 0, &task1Env[1024]);
	tTaskInit(&tTask2, task2Entry, (void *)0x22222222, 1, &task2Env[1024]);
	
	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;
	
	tTaskInit(&tTaskIdle, idleTaskEntry, (void *)0, TINYOS_PRO_COUNT - 1, &idleTaskEnv[1024]);
	idleTask = &tTaskIdle;
	
	nextTask = tTaskHighestReady();
		
	tTaskRunFirst();//触发PendSVC进入异常处理函数中
	
	return 0;
}


//思考：每个优先级只能有一个任务能否多个任务共享一个优先级？
