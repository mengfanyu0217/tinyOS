#include "tinyOS.h"

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
	
	task->slice = TINYOS_SLICE_MAX;
	
	task->stack = stack;//�������յ�ֵ
	task->delayTicks = 0;
	task->prio = prio;
	task->state = TINYOS_TASK_STATE_RDY;
	task->suspendCount = 0;
	task->clean = (void (*)(void*))0;
	task->cleanParam = (void *)0;
	task->requestDeleteFlag = 0;
	
	tNodeInit(&(task->delayNode));//��ʱ����ʼ��
	tNodeInit(&(task->linkNode));

	tTaskSchedRdy(task);
}

void tTaskSuspend (tTask * task)//���������
{
	uint32_t status = tTaskEnterCritical();//����ٽ�������
	if (!(task->state & TINYOS_TASK_STATE_SUSPEND))//��������Ƿ�����ʱ״̬��������ʱ״̬�����ܹ���
		if (++task->suspendCount <= 1)//���ǵ�һ�ι���
		{
			task->state |= TINYOS_TASK_STATE_SUSPEND;//���轫�����״̬λ����Ϊ����״̬
			tTaskSchedUnRdy(task);//�ٽ�����Ӿ��������Ƴ�
			if (task == currentTask)//�ж���������ǲ��ǵ�ǰ����
				tTaskSched();//���ǵ�ǰ���������л���������������
		}
	tTaskExitCritical(status);
}

void tTaskWakeUp (tTask * task)//����ָ�����
{
	uint32_t status = tTaskEnterCritical();//����ٽ�������
	if (task->state & TINYOS_TASK_STATE_SUSPEND)//�ж������Ƿ��ڹ���״̬
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
	uint32_t status = tTaskEnterCritical();//����ٽ�������
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
	uint32_t status = tTaskEnterCritical();//����ٽ�������
	task->requestDeleteFlag = 1;
	tTaskExitCritical(status);
}

uint8_t tTaskIsRequestDelete (void)
{
	uint8_t delete;
	uint32_t status = tTaskEnterCritical();//����ٽ�������
	delete = currentTask->requestDeleteFlag;
	tTaskExitCritical(status);
	return delete;
}

void tTaskDeleteSelf (void)
{
	uint32_t status = tTaskEnterCritical();//����ٽ�������
	tTaskSchedRemove(currentTask);
	if (currentTask->clean)
		currentTask->clean(currentTask->cleanParam);
	tTaskSched();	
	tTaskExitCritical(status);
}

void tTaskGetInfo (tTask * task, tTaskInfo * info)
{
	uint32_t status = tTaskEnterCritical();//����ٽ�������

	info->delayTicks = task->delayTicks;
	info->prio = task->prio;
	info->state = task->state;
	info->slice = task->slice;
	info->suspendCount = task->suspendCount;
	
	tTaskExitCritical(status);
}


