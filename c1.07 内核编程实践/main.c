/*
����˵����ʹ��PendSVC�����쳣�����쳣�������У��Ƚ��Ĵ���R4-R11��ֵ��������������
          �ٻָ��Ĵ���R4-R11��ֵ���Դ���ģ�������л�ʱ�Ĵ����ı�����ָ���
*/

//ʹ��PendSVC����ʱ����Ҫ���ü����Ĵ���
#define NVIC_INT_CTRL     0xE000ED04  //�Ĵ����ĵ�ַ
#define NVIC_PENDSVSET    0x10000000  //��Ĵ���д���ֵ
#define NVIC_SYSPRI2      0xE000ED22  //�Ĵ����ĵ�ַ
#define NVIC_PENDSV_PRI   0x000000FF  //��Ĵ���д���ֵ
//��Ĵ�����д��ֵ�ĺ�
#define MEM32(addr)       *(volatile unsigned long *)(addr)
#define MEM8(addr)        *(volatile unsigned char *)(addr)
//PendSVC�쳣��������
void triggerPendSVC (void)
{
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;  //����PendSVC�쳣���ȼ�Ϊ���
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET; //����
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
