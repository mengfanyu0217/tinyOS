//PendSVC�쳣��������
//��C����Ƕ�����__��ͷ
__asm void PendSV_Handler (void)
{
	IMPORT blockPtr//����blockPtr���൱��externָ��
	
	LDR R0, =blockPtr//��blockPtr�����ĵ�ַ���ص�R0��
	LDR R0, [R0]//�Ӹõ�ַ����32λ����ֵ��Ҳ����block�ṹ����ʼ��ַ
	LDR R0, [R0]//�ٴμ���32λ���ݣ�Ҳ����stackPtr��ֵ��Ҳ����&stackBuffer[1024]����ʱR0��ֵΪ&stackBuffer[1024]	
	//STMDB Rd!, {Rn, ..., Rm}��Rd��������εݼ���ַ�洢32λ���ݣ���������Rn, ..., Rm�б�
	STMDB   R0!, {R4-R11}//��R4~R11�Ĵ����е�ֵд�뵽stackBuffer�������У��������д��󣬽����ĵ�ַ���ǵ�R0��
	
	LDR R1, =blockPtr//��blockPtr�����ĵ�ַ���ص�R1��
	LDR R1, [R1]//�Ӹõ�ַ����32λ����ֵ��Ҳ����block�ṹ����ʼ��ַ
	STR R0, [R1]//��R1�Ĵ�����ȡ����ַ����R0�е�32λ���ݴ洢����Ӧ��ַ����ִ�к�R0��ֵ
	//�޸�R4��R5�Ĵ�����ֵ�����ڲ��ԣ���������ָ���ȷ����ôִ��LDMIA��R4��R5Ӧ�ָ�֮ǰ��ֵ
	ADD R4, R4, #1 //R4=R4+1
	ADD R5, R5, #1 //R5=R5+1
	//LDMIA Rd!, {Rn, ..., Rm}��Rd��������ε�����ַ��ȡ32λ���ݣ��洢��Rn, ..., Rm�б�
	LDMIA R0!, {R4-R11}
	//�쳣����
	BX LR
	NOP
}
