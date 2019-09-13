/**************************************************************
 * ���Ŀ�꣺����״̬��ѯ
**************************************************************/

#include "tinyOS.h"
#include "ARMCM3.h"

tTask * currentTask;
tTask * nextTask;
tTask * idleTask;

tBitmap taskPrioBitmap;//���ȼ�λͼ
tList taskTable[TINYOS_PRO_COUNT];//���������ָ������

uint8_t schedLockCount;//������������
tList tTaskDelayedList;//������ʱ����

tTask * tTaskHighestReady (void)
{
	uint32_t highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);
	tNode * node = tListFirst(&taskTable[highestPrio]);
	return (tTask *)tNodeParent(node, tTask, linkNode);
}

void tTaskSchedInit (void)//��������ʼ��
{
	int i;
	schedLockCount = 0;
	tBitmapInit(&taskPrioBitmap);
	for (i = 0; i < TINYOS_PRO_COUNT; i++)
		tListInit(&taskTable[i]);
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

void tTaskDelayedInit (void)//��ʱ���г�ʼ��
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
	uint32_t status = tTaskEnterCritical();//����ٽ�������
	for (node = tTaskDelayedList.headNode.nextNode; node != &(tTaskDelayedList.headNode); node = node->nextNode)
	{
		tTask * task = tNodeParent(node, tTask, delayNode);
		if (--task->delayTicks == 0)
		{
			tTimeTaskWakeUp(task);
			tTaskSchedRdy(task);
		}		
	}
	
	if (--currentTask->slice == 0)//�жϵ�ǰ�����ʱ��Ƭ�Ƿ��Ѿ�����
		if (tListCount(&taskTable[currentTask->prio]) > 0)//����Ѿ������ˣ����һ����������񣬽����л���ת
		{
			tListRemoveFirst(&taskTable[currentTask->prio]);
			tListAddLast(&taskTable[currentTask->prio], &(currentTask->linkNode));
			currentTask->slice = TINYOS_SLICE_MAX;
		}
	
	tTaskExitCritical(status);
	tTaskSched();
}

//��������
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
		
	tTaskRunFirst();//����PendSVC�����쳣��������
	
	return 0;
}


//˼����
