/**************************************************************
 * ���Ŀ�꣺˫������ṹ
**************************************************************/

#include "tinyOS.h"
#include "ARMCM3.h"

tTask * currentTask;
tTask * nextTask;
tTask * idleTask;

tBitmap taskPrioBitmap;//���ȼ�λͼ
tTask * taskTable[TINYOS_PRO_COUNT];//���������ָ������

uint8_t schedLockCount;//������������

//�������ջ��ʼ��
void tTaskInit (tTask * task, void (*entry)(void*), void * param, uint32_t prio, tTaskStack * stack)
{
	//����PendSV���˳�PendSVʱӲ���Զ�����ļĴ�������PendSV�˳�ʱ�Ĵ����ָ�˳��һ�¡�
	//�ⲿ�ֵ�ֵ������ָ�ʱ���У����˳�PendSVCʱ������
	//���˳��Ҫ���˳��쳣ʱӲ���Զ���ջ��˳��һ�£�����Ὣ��Ӧ��ֵ�ָ�������ļĴ�����
	*(--stack) = (unsigned long)(1 << 24);//xPSR�Ĵ���
	*(--stack) = (unsigned long)entry;//R15�Ĵ�����ֵ��Ҳ��PC�Ĵ�����ֵ���������ڵ�ַ
	*(--stack) = (unsigned long)0x14;//R14�Ĵ�����ֵ��Ҳ��LR�Ĵ�����ֵ��δʹ�ã�������14��伴��
	*(--stack) = (unsigned long)0x12;//R12�Ĵ�����ֵ��δʹ�ã�������12��伴��
	*(--stack) = (unsigned long)0x3;//R3�Ĵ�����ֵ��δʹ�ã�������3��伴��
	*(--stack) = (unsigned long)0x2;//R2�Ĵ�����ֵ��δʹ�ã�������2��伴��
	*(--stack) = (unsigned long)0x1;//R1�Ĵ�����ֵ��δʹ�ã�������1��伴��
	*(--stack) = (unsigned long)param;//R0�Ĵ�����ֵ��R0����������ڲ���
	
	//�°벿����PendSVC�е�LDMIAָ���йأ�ʹ��LDMIA R0!, {R4-R11}ʱ���Զ���һ��˳����ڴ��м���ֵ��R4-R11�У�Ϊ�˱�֤������ȷ��˳��Ҫһ�¡�
	//PendSV����ʱδ�Զ�������ں˼Ĵ�����R4-R11������Ҫ�ֶ�����
	*(--stack) = (unsigned long)0x11;//R11�Ĵ�����ֵ��δʹ�ã�������11��伴��
	*(--stack) = (unsigned long)0x10;//R10�Ĵ�����ֵ��δʹ�ã�������10��伴��
	*(--stack) = (unsigned long)0x9;//R9�Ĵ�����ֵ��δʹ�ã�������9��伴��
	*(--stack) = (unsigned long)0x8;//R8�Ĵ�����ֵ��δʹ�ã�������8��伴��
	*(--stack) = (unsigned long)0x7;//R7�Ĵ�����ֵ��δʹ�ã�������7��伴��
	*(--stack) = (unsigned long)0x6;//R6�Ĵ�����ֵ��δʹ�ã�������6��伴��
	*(--stack) = (unsigned long)0x5;//R5�Ĵ�����ֵ��δʹ�ã�������5��伴��
	*(--stack) = (unsigned long)0x4;//R4�Ĵ�����ֵ��δʹ�ã�������4��伴��
	
	task->stack = stack;//�������յ�ֵ
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

void tTaskSchedInit (void)//��������ʼ��
{
	schedLockCount = 0;
	tBitmapInit(&taskPrioBitmap);
}

void tTaskSchedDisable (void)//��������
{
	uint32_t status = tTaskEnterCritical();
	if (schedLockCount < 255)
		schedLockCount++;
	tTaskExitCritical(status);
}

void tTaskSchedEnable (void)//��������
{
	uint32_t status = tTaskEnterCritical();
	if (schedLockCount > 0)
		if (--schedLockCount == 0)
			tTaskSched();
	tTaskExitCritical(status);
}

void tTaskSched (void) //������Ⱥ���
{
	tTask * tempTask;
	uint32_t status = tTaskEnterCritical();//����ٽ�������
	if (schedLockCount > 0)//�жϵ������Ƿ�����
	{
		tTaskExitCritical(status);
		return;
	}
	
	//�ҵ����ȼ���ߵ�������������ȼ��ȵ�ǰ��������ȼ����ߣ���ô���л����������
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
	uint32_t status = tTaskEnterCritical();//����ٽ�������
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
	uint32_t status = tTaskEnterCritical();//����ٽ�������
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

//������������
tTask tTask1;
tTask tTask2;
//Ϊtask1��task2�䱸���������Ķ�ջ�ռ�,��ջ�ռ��СΪ1024����
tTaskStack task1Env[1024];
tTaskStack task2Env[1024];
//��������
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
		
	tTaskRunFirst();//����PendSVC�����쳣��������
	
	return 0;
}


//˼����ÿ�����ȼ�ֻ����һ�������ܷ���������һ�����ȼ���
