#include "tinyOS.h"

tTask * currentTask;
tTask * nextTask;
tTask * taskTable[2];

//PendSVC�쳣��������
void triggerPendSVC (void)
{
	;
}
//�������ջ��ʼ��
void tTaskInit (tTask * task, void (*entry)(void*), void * param, tTaskStack * stack)
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
}

void tTaskSched () //������Ⱥ���
{
	if (currentTask == taskTable[0])//�����ǰʱ��0��������ô�¸������ǵ�1������
		nextTask = taskTable[1];
	else
		nextTask = taskTable[0];
	tTaskSwitch();
}

void delay(int count)
{
	while(--count > 0);
}

//������������
tTask tTask1;
tTask tTask2;
//Ϊtask1��task2�䱸���������Ķ�ջ�ռ�,��ջ�ռ��СΪ1024����
tTaskStack task1Env[1024];
tTaskStack task2Env[1024];

int task1Flag;
void task1Entry (void * param)
{
	unsigned long value = *(unsigned long *)param;
	value++;
	for(;;)
	{
		task1Flag = 0;
		delay(100);
		task1Flag = 1;
		delay(100);
		
		tTaskSched();
	}
}

int task2Flag;
void task2Entry (void * param)
{
	for(;;)
	{
		task2Flag = 0;
		delay(100);
		task2Flag = 1;
		delay(100);
		
		tTaskSched();
	}
}

int main()
{
	tTaskInit(&tTask1, task1Entry, (void *)0x11111111, &task1Env[1024]);
	tTaskInit(&tTask2, task2Entry, (void *)0x22222222, &task2Env[1024]);
	
	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;
	
	nextTask = taskTable[0];
	
	tTaskRunFirst();//����PendSVC�����쳣��������
	
	return 0;
}

//˼����ÿ�����������������tTaskSched()�����л�����һ�������������ִ���л�����һֱռ��CPU��
//�ܷ�������ã��ұ���CPUһֱ��ĳ������ռ�ã�




