#include <STDIO.H>    
#include <CSL_IRQ.H>
#include <math.h>
#include <yj6701.h>
/***************************************************************/
/*    EMIF����ͼ��                                             */
/***************************************************************/
void emifRecvImg(void)
{
	unsigned int i=0;
	int *recv_To_sd;

	if(g_frameNum_r%2)	//����Ԥ�����ͼ��
	{
		i = 0;
		//memset(g_imgRmb[g_frameNum_r/2%5],0,IMG_WID*IMG_HEI);
		recv_To_sd = (int *)(g_imgRmb[g_frameNum_r/2%5]);
		g_emifImg = (unsigned int *)0x03040000;		//ָ��Ԥ�����ͼ���ַ
		while(i<1024*512){
			//memcpy((float *)(g_imgRmb[g_frameNum_r/2%5])+i++,g_emifImg+(i%65536),4);
			*(recv_To_sd+i) = *g_emifImg;i++;}
	}
	else	//����ԭʼͼ��
	{
		i = 0;
		//memset(g_imgOrig[g_frameNum_r/2%5],0,IMG_WID*IMG_HEI);
		recv_To_sd = (int *)(g_imgOrig[g_frameNum_r/2%5]);
		g_emifImg = (unsigned int *)0x03000000;		//ָ��ԭʼͼ���ַ
		while(i<1024*512){
			//memcpy((float *)(g_imgOrig[g_frameNum_r/2%5])+i++,g_emifImg+(i%65536),4);
			*(recv_To_sd+i) = *g_emifImg;i++;}
	}

	return;
}
/***************************************************************/
/*    �����ж�		                                           */
/***************************************************************/
void SetupInterrupts(void)
{
    IER=0x0032;		//�����ж�
    CSR=CSR|0x1;	//���ж�
}
/***************************************************************/
/*    EMIF����ע������                                         */
/***************************************************************/
void emifRecvEE(void)
{
	unsigned short i=0;
	g_emifImg = (unsigned int *)0x03180000;		//ָ�����ע�����ݵĵ�ַ

	while(i<512){
		memcpy(g_AreaEE+g_injectNum_r*512+i,g_emifImg+(i%65536),4);i++;}	//һ�ν���2KB����

	if(g_injectSum == 0)
		g_injectSum = *((unsigned char *)g_AreaEE+2);	//��ȡ��Ҫ���յ����жϴ���

	return;
}
/***************************************************************/
/*    ��ע����ȷ���                                           */
/***************************************************************/
void sendStepInject(unsigned int step)
{
	int i=0,j=0;
	unsigned char stepBuf[256];
	g_stepInject = step;
	g_emifImg = (unsigned int *)0x033C0000;			//EMIF����
	memset(stepBuf,0,256);
	stepBuf[0] = 0xEB;
	stepBuf[1] = 0x90;
	stepBuf[8] = step;			//���ȱ�ʶλ
	stepBuf[253] = 0x7B + step;	//У���
	stepBuf[254] = 0x09;
	stepBuf[255] = 0xD7;
	for(j=0;j<64;j++){
		*g_emifImg = *(unsigned int *)(stepBuf+j*4);}	//���͸�FPGA
	for (i=0;i<3000;i++){;}		//��ʱ
}
/***************************************************************/
/*    ������ע������                                           */
/***************************************************************/
void analyzeData(void)
{
	unsigned int checkSum = 0,checkSum_r = 0,i = 0;
	g_lenArea00 = (((unsigned int)(*((unsigned char *)g_AreaEE+3)))<<24) | (((unsigned int)(*((unsigned char *)g_AreaEE+4)))<<16) | (((unsigned int)(*((unsigned char *)g_AreaEE+5)))<<8) | ((unsigned int)(*((unsigned char *)g_AreaEE+6)));	//00�����ݳ���
	g_lenArea80 = (((unsigned int)(*((unsigned char *)g_AreaEE+7)))<<24) | (((unsigned int)(*((unsigned char *)g_AreaEE+8)))<<16) | (((unsigned int)(*((unsigned char *)g_AreaEE+9)))<<8) | ((unsigned int)(*((unsigned char *)g_AreaEE+10)));	//80�����ݳ���
	
	checkSum = 0;	//У���
	checkSum_r = 0;	//У���λ
	for(i = 0;i < 252;)	//��У���
	{
		checkSum += (((unsigned int)(*((unsigned char *)g_AreaEE+i)))<<24) | (((unsigned int)(*((unsigned char *)g_AreaEE+i+1)))<<16) | (((unsigned int)(*((unsigned char *)g_AreaEE+i+2)))<<8) | ((unsigned int)(*((unsigned char *)g_AreaEE+i+3)));
		i += 4;
	}
	checkSum_r = (((unsigned int)(*((unsigned char *)g_AreaEE+i)))<<24) | (((unsigned int)(*((unsigned char *)g_AreaEE+i+1)))<<16) | (((unsigned int)(*((unsigned char *)g_AreaEE+i+2)))<<8) | ((unsigned int)(*((unsigned char *)g_AreaEE+i+3)));
	i += 4;
	if(checkSum != checkSum_r)
	{
		sendStepInject(2);	//��ͷ����
		return;
	}
	
	if(g_lenArea00 != 0)
	{
		checkSum = 0;		//������ʼ��
		checkSum_r = 0;		//������ʼ��
		for(;i < g_lenArea00 + 256 - 4;)	//��У���
		{
			checkSum += (((unsigned int)(*((unsigned char *)g_AreaEE+i)))<<24) | (((unsigned int)(*((unsigned char *)g_AreaEE+i+1)))<<16) | (((unsigned int)(*((unsigned char *)g_AreaEE+i+2)))<<8) | ((unsigned int)(*((unsigned char *)g_AreaEE+i+3)));
			i += 4;
		}
		checkSum_r = (((unsigned int)(*((unsigned char *)g_AreaEE+i)))<<24) | (((unsigned int)(*((unsigned char *)g_AreaEE+i+1)))<<16) | (((unsigned int)(*((unsigned char *)g_AreaEE+i+2)))<<8) | ((unsigned int)(*((unsigned char *)g_AreaEE+i+3)));
		i += 4;
		if(checkSum != checkSum_r)	//���У�����
		{
			sendStepInject(3);	//00������
			return;
		}
	}
	
	if(g_lenArea80 != 0)
	{
		checkSum = 0;		//������ʼ��
		checkSum_r = 0;		//������ʼ��
		for(;i < g_lenArea00 + g_lenArea80 + 256 - 4;)	//��У���
		{
			checkSum += (((unsigned int)(*((unsigned char *)g_AreaEE+i)))<<24) | (((unsigned int)(*((unsigned char *)g_AreaEE+i+1)))<<16) | (((unsigned int)(*((unsigned char *)g_AreaEE+i+2)))<<8) | ((unsigned int)(*((unsigned char *)g_AreaEE+i+3)));
			i += 4;
		}
		checkSum_r = (((unsigned int)(*((unsigned char *)g_AreaEE+i)))<<24) | (((unsigned int)(*((unsigned char *)g_AreaEE+i+1)))<<16) | (((unsigned int)(*((unsigned char *)g_AreaEE+i+2)))<<8) | ((unsigned int)(*((unsigned char *)g_AreaEE+i+3)));
		i += 4;
		if(checkSum != checkSum_r)	//���У�����
		{
			sendStepInject(4);	//80������
			return;
		}
	}
	sendStepInject(5);	//���ݽ�����ȷ
	return;
}
/***************************************************************/
/*    ��ע������д��EE                                         */
/***************************************************************/
void begWrEE(void)
{
	unsigned int i = 0,j = 0,m = 0;
	unsigned char *src,*des;
	*(unsigned int *)0x1800004 = 0xFFFF3F23;	//CE1

   	*(unsigned int *)(0x01500000+(0x05555<<2)) = 0xaa;    //first    
	*(unsigned int *)(0x01500000+(0x0AAAA<<2)) = 0x55;    //second
	*(unsigned int *)(0x01500000+(0x05555<<2)) = 0xA0;    //third

	src = (unsigned char *)g_AreaEE+256;
	des = (unsigned char *)0x01500000;
	m=0;
	while(m<g_lenArea00)	//д00��
	{
		for (i=0;i<50;i++)
			for (j=0;j<100;j++);
		if(*(des+m*4) == *(src+m))
		{
			m++;
			if(m%1024 == 0)
				sendStepInject(5);
		}
		else
		{
			*(des+m*4) = *(src+m);
		}
	}
	sendStepInject(6);	//00��д��

	*(unsigned int *)(0x01580000+(0x05555<<2)) = 0xaa;    //first
	*(unsigned int *)(0x01580000+(0x0AAAA<<2)) = 0x55;    //second
	*(unsigned int *)(0x01580000+(0x05555<<2)) = 0xA0;    //third

	m=0;
	src = (unsigned char *)g_AreaEE+256+g_lenArea00;
	des = (unsigned char *)0x01580000;
	while(m<g_lenArea80)	//д80��
	{
		if(m<1)
		{
			for (i=0;i<5000;i++)
				for (j=0;j<100;j++);
		}
		for (i=0;i<50;i++)
			for (j=0;j<100;j++);
		if(*(des+m*4) == *(src+m))
		{
			m++;
			if(m%1024 == 0)
				sendStepInject(6);
		}
		else
		{
			*(des+m*4) = *(src+m);
		}
	}
	sendStepInject(7);	//80��д��
}

void checkEE(void)
{
	unsigned int checkSum = 0,checkSum_r = 0,i = 0,j = 0;
	*(unsigned int *)0x1800004 = 0xFFFF3F03;	//CE1

	if(g_lenArea00 != 0)
	{
		checkSum = 0;		//������ʼ��
		checkSum_r = 0;		//������ʼ��
		for(i=0;i < g_lenArea00 - 4;)
		{
			checkSum += (((unsigned int)(*((unsigned char *)0x01440000+i)))<<24) | (((unsigned int)(*((unsigned char *)0x01440000+i+1)))<<16) | (((unsigned int)(*((unsigned char *)0x01440000+i+2)))<<8) | ((unsigned int)(*((unsigned char *)0x01440000+i+3)));
			i += 4;
			for (j=0;j<5000;j++){;}
		}
		checkSum_r = (((unsigned int)(*((unsigned char *)0x01440000+i)))<<24) | (((unsigned int)(*((unsigned char *)0x01440000+i+1)))<<16) | (((unsigned int)(*((unsigned char *)0x01440000+i+2)))<<8) | ((unsigned int)(*((unsigned char *)0x01440000+i+3)));
		i += 4;
		if(checkSum != checkSum_r)
		{
			sendStepInject(8);
			return;
		}
	}
	sendStepInject(9);
	
	if(g_lenArea80 != 0)
	{
		checkSum = 0;		//������ʼ��
		checkSum_r = 0;		//������ʼ��
		for(i=0;i < g_lenArea80 - 4;)
		{
			checkSum += (((unsigned int)(*((unsigned char *)0x01460000+i)))<<24) | (((unsigned int)(*((unsigned char *)0x01460000+i+1)))<<16) | (((unsigned int)(*((unsigned char *)0x01460000+i+2)))<<8) | ((unsigned int)(*((unsigned char *)0x01460000+i+3)));
			i += 4;
			for (j=0;j<5000;j++){;}
		}
		checkSum_r = (((unsigned int)(*((unsigned char *)0x01460000+i)))<<24) | (((unsigned int)(*((unsigned char *)0x01460000+i+1)))<<16) | (((unsigned int)(*((unsigned char *)0x01460000+i+2)))<<8) | ((unsigned int)(*((unsigned char *)0x01460000+i+3)));
		i += 4;
		if(checkSum != checkSum_r)
		{
			sendStepInject(10);
			return;
		}
	}
	sendStepInject(11);
}

void beginInject(void)
{
	unsigned int i = 0,j = 0;
	sendStepInject(1);	//ָʾ�������
	analyzeData();		//��ʼ��������
	if(g_stepInject != 5)
	{
		while(1)
		{
			for (i=0;i<50000;i++){
				for (j=0;j<5000;j++){;}}
			sendStepInject(g_stepInject);	//���Ͷ�Ӧ״̬
		}
	}
	//��һ��д
	begWrEE();			//��ʼдEEPROM
	checkEE();			//��ʼУ��EEPROM
	//�ڶ���д
	if(g_stepInject != 11)
	{
		begWrEE();			//��ʼдEEPROM
		checkEE();			//��ʼУ��EEPROM
	}
	while(1)
	{
		for (i=0;i<10000;i++){
			for (j=0;j<10000;j++){;}}
		sendStepInject(g_stepInject);	//�ظ���������״ֱ̬����λ
	}
}


