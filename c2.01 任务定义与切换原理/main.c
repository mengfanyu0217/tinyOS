#include "tinyOS.h"

//使用PendSVC触发时，需要配置几个寄存器
#define NVIC_INT_CTRL     0xE000ED04  //寄存器的地址
#define NVIC_PENDSVSET    0x10000000  //向寄存器写入的值
#define NVIC_SYSPRI2      0xE000ED22  //寄存器的地址
#define NVIC_PENDSV_PRI   0x000000FF  //向寄存器写入的值
//向寄存器中写入值的宏
#define MEM32(addr)       *(volatile unsigned long *)(addr)
#define MEM8(addr)        *(volatile unsigned char *)(addr)
//PendSVC异常触发函数
void triggerPendSVC (void)
{
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;  //设置PendSVC异常优先级为最低
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET; //触发
}
//初始化函数，用于初始化任务
void tTaskInit (tTask * task, void (*entry)(void*), void * param, tTaskStack * stack)
{
	task->stack = stack;
}

typedef struct _BlockType_t
{
	unsigned long * stackPtr;
}BlockType_t;

BlockType_t * blockPtr;

void delay(int count)
{
	while(--count > 0);
}

int flag;
unsigned long stackBuffer[1024];
BlockType_t block;

//定义两个任务
tTask tTask1;
tTask tTask2;
//为task1和task2配备两个独立的堆栈空间,堆栈空间大小为1024即可
tTaskStack task1Env[1024];
tTaskStack task2Env[1024];

void task1 (void * param)
{
	for(;;)
	{
		;
	}
}

void task2 (void * param)
{
	for(;;)
	{
		;
	}
}

int main()
{
	tTaskInit(&tTask1, task1, (void *)0x11111111, &task1Env[1024]);
	tTaskInit(&tTask2, task2, (void *)0x22222222, &task2Env[1024]);
	
	block.stackPtr = &stackBuffer[1024];
	blockPtr = &block;
	for(;;)
	{
		flag = 0;
		delay(100);
		flag = 1;
		delay(100);
		triggerPendSVC();
	}
}
