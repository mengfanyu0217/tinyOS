#include "tinyOS.h"
#include "ARMCM3.h"//ARM��ͷ�ļ�

//ʹ��PendSVC����ʱ����Ҫ���ü����Ĵ���
#define NVIC_INT_CTRL     0xE000ED04  //�Ĵ����ĵ�ַ
#define NVIC_PENDSVSET    0x10000000  //��Ĵ���д���ֵ
#define NVIC_SYSPRI2      0xE000ED22  //�Ĵ����ĵ�ַ
#define NVIC_PENDSV_PRI   0x000000FF  //��Ĵ���д���ֵ
//��Ĵ�����д��ֵ�ĺ�
#define MEM32(addr)       *(volatile unsigned long *)(addr)
#define MEM8(addr)        *(volatile unsigned char *)(addr)

uint32_t tTaskEnterCritical (void)
{
	uint32_t primask = __get_PRIMASK();//��ȡPRIMASK��ֵ
	__disable_irq();//���ж�
	return primask;
}

void tTaskExitCritical (uint32_t status)
{
	__set_PRIMASK(status);//��PRIMASK��ֵ��д��PRIMASK�Ĵ�����
}

//PendSVC�쳣��������
//��C����Ƕ�����__��ͷ
__asm void PendSV_Handler (void)
{
	IMPORT currentTask
	IMPORT nextTask
	
	MRS R0, PSP//��PSP�Ĵ������浽R0��
	CBZ R0, PendSVHandler_nosave
	
	STMDB R0!, {R4-R11}

	LDR R1, =currentTask
	LDR R1, [R1]
	STR R0, [R1]
	
PendSVHandler_nosave
	LDR R0, =currentTask
	LDR R1, =nextTask
	LDR R2, [R1]
	STR R2, [R0]
	
	LDR R0, [R2]
	LDMIA R0!, {R4-R11}
	
	MSR PSP, R0
	ORR LR, LR, #0x04
	BX LR
}

void tTaskRunFirst ()
{
	__set_PSP(0);//��PSP��ֵΪ0
	
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;  //����PendSVC�쳣���ȼ�Ϊ���
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET; //����
}

void tTaskSwitch ()
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET; //����
}
