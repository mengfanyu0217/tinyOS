//PendSVC异常触发函数
//在C中内嵌汇编用__开头
__asm void PendSV_Handler (void)
{
	IMPORT blockPtr//导入blockPtr，相当于extern指令
	
	LDR R0, =blockPtr//将blockPtr变量的地址加载到R0中
	LDR R0, [R0]//从该地址记载32位数据值，也就是block结构的起始地址
	LDR R0, [R0]//再次加载32位数据，也就是stackPtr的值，也就是&stackBuffer[1024]，此时R0的值为&stackBuffer[1024]	
	//STMDB Rd!, {Rn, ..., Rm}从Rd处连续多次递减地址存储32位数据，数据来自Rn, ..., Rm列表
	STMDB   R0!, {R4-R11}//将R4~R11寄存器中的值写入到stackBuffer缓冲区中，完成所有写入后，将最后的地址覆盖到R0中
	
	LDR R1, =blockPtr//将blockPtr变量的地址加载到R1中
	LDR R1, [R1]//从该地址记载32位数据值，也就是block结构的起始地址
	STR R0, [R1]//从R1寄存器中取出地址，将R0中的32位数据存储到相应地址处。执行后，R0的值
	//修改R4和R5寄存器的值，用于测试，若保存与恢复正确，那么执行LDMIA后R4和R5应恢复之前的值
	ADD R4, R4, #1 //R4=R4+1
	ADD R5, R5, #1 //R5=R5+1
	//LDMIA Rd!, {Rn, ..., Rm}从Rd处连续多次递增地址读取32位数据，存储到Rn, ..., Rm列表
	LDMIA R0!, {R4-R11}
	//异常返回
	BX LR
	NOP
}
