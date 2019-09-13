/*
任务说明：使用PendSVC触发异常。在异常处理函数中，先将寄存器R4-R11的值保存至缓冲区，
          再恢复寄存器R4-R11的值，以此来模拟任务切换时寄存器的保存与恢复。
*/

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

int main()
{
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
