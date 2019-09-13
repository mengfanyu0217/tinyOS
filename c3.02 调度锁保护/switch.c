#include "tinyOS.h"
#include "ARMCM3.h"//ARM的头文件

//使用PendSVC触发时，需要配置几个寄存器
#define NVIC_INT_CTRL     0xE000ED04  //寄存器的地址
#define NVIC_PENDSVSET    0x10000000  //向寄存器写入的值
#define NVIC_SYSPRI2      0xE000ED22  //寄存器的地址
#define NVIC_PENDSV_PRI   0x000000FF  //向寄存器写入的值
//向寄存器中写入值的宏
#define MEM32(addr)       *(volatile unsigned long *)(addr)
#define MEM8(addr)        *(volatile unsigned char *)(addr)

uint32_t tTaskEnterCritical (void)
{
	uint32_t primask = __get_PRIMASK();//获取PRIMASK的值
	__disable_irq();//关中断
	return primask;
}

void tTaskExitCritical (uint32_t status)
{
	__set_PRIMASK(status);//将PRIMASK的值再写会PRIMASK寄存器中
}

//PendSVC异常触发函数
//在C中内嵌汇编用__开头
__asm void PendSV_Handler (void)
{
	IMPORT currentTask
	IMPORT nextTask
	
	MRS R0, PSP//将PSP寄存器保存到R0中
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
	__set_PSP(0);//将PSP赋值为0
	
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;  //设置PendSVC异常优先级为最低
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET; //触发
}

void tTaskSwitch ()
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET; //触发
}
