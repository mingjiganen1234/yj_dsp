#include <stdio.h>
#include "../include/yj6701.h"
#include <string.h>

/*****************************************����*********************************************/
unsigned short	g_thresholdBW	= 150;								//��ֵ����ֵ
unsigned short	g_thresholdMW	= 3;								//ȥ����ֵ
unsigned short	g_thresholdSTAR	= 150;								//����Ŀ������
unsigned short	g_thresholdBCK	= 7;								//����Ԥ����
unsigned int	g_controlWord[5]= {0,0,0,0,0};						//���Ʋ���
unsigned int	g_captureTime[5][2]= {{0,0},{0,0},{0,0},{0,0},{0,0}};//����ʱ��

unsigned int	g_frameNumber	= 0;								//ͼ���֡����
unsigned int	g_frameNum		= 0;								//֡��
unsigned int	g_frameNum_r	= 0;								//֡��_r
/******************************************************************************************/
																							
/***************************************��׼***********************************************/
unsigned char	*g_regLabelMap = (unsigned char *)REG_LABEL;		//��׼��ͨ��ͼ(1024*1024B)
struct star		*g_stars[5]	={  (struct star *)STARS_0,
								(struct star *)STARS_1,
								(struct star *)STARS_2,
								(struct star *)STARS_3,
								(struct star *)STARS_4};		//��׼������Ϣ
struct tri		*g_tris[5]	={  (struct tri *)TRIS_0,
								(struct tri *)TRIS_1,
								(struct tri *)TRIS_2,
								(struct tri *)TRIS_3,
								(struct tri *)TRIS_4};			//��׼������Ϣ
unsigned char	g_triCnt[5];									//��׼���Ǹ���

short			g_offsetR[5]	= {0,0,0,0,0};						//��ƫ����
short			g_offsetC[5]	= {0,0,0,0,0};						//��ƫ����
/******************************************************************************************/

/***************************************origin*********************************************/
unsigned int 	*g_emifImg		=	(unsigned int   *)0x03000000;	//EMIF���պͷ�������
unsigned short	*g_imgOrig[5]	= { (unsigned short *)ORI_IMG_0,
									(unsigned short *)ORI_IMG_1,
									(unsigned short *)ORI_IMG_2,
									(unsigned short *)ORI_IMG_3,
									(unsigned short *)ORI_IMG_4};	//ԭʼͼ��洢ָ��
unsigned short	*g_imgRmb[5]	= { (unsigned short *)RMB_IMG_0,
									(unsigned short *)RMB_IMG_1,
									(unsigned short *)RMB_IMG_2,
									(unsigned short *)RMB_IMG_3,
									(unsigned short *)RMB_IMG_4};	//ȥ��������ͼ��洢ָ��
/******************************************************************************************/

/****************************************star**********************************************/
unsigned short	 g_starSum[5]	=	{0,0,0,0,0};					//����Ŀ������
float			 g_starPosi[5][160*3];								//����Ŀ��ϸ��(3��*160��)
/******************************************************************************************/

/****************************************move**********************************************/
unsigned short	s_origVal[5][1024];
unsigned char	s_maxIdx[1024];
unsigned char	*g_maxIndex		=	(unsigned char  *)MAX_INDEX;	//���ֵ����֡
unsigned char	*g_corImg		=	(unsigned char  *)COR_MAX_IDX;	//ȥ���
unsigned short	 g_objNum[5]	=	{0,0,0,0,0};					//ÿ֡ͼ����ͨ����(5��1��)
float			*g_objPntLst	=	(float *)OBJ_PNT_LST;			//�洢֡�š���ͨ�򡢼���������
float			*objPointF[5]	={	(float *)OBJ_PNT_F0,
									(float *)OBJ_PNT_F1,
									(float *)OBJ_PNT_F2,
									(float *)OBJ_PNT_F3,
									(float *)OBJ_PNT_F4};			//��0,1,2,3,4֡ͼ��ͨ���������Ϣ
float			*g_allObjInf	=	(float *)OBJ_MOV_INFO;			//�˶�Ŀ��ϸ��(5��3�еı���)
unsigned short	 g_moveSum		=	0;								//�˶�Ŀ������
/******************************************************************************************/

/****************************************ע��**********************************************/
unsigned short	g_injectNum		= 0;								//ע������жϼ���
unsigned short	g_injectNum_r	= 0;								//ע������жϼ���_r
unsigned short	g_injectSum		= 0;								//�ó���ε��ж��ܴ���
unsigned int	*g_AreaEE		= (unsigned int   *)0x00400000;		//��ע�������ݴ�ĵ�ַ
unsigned int	g_lenArea00		= 0;								//00��ռ�ֽ���
unsigned int	g_lenArea80		= 0;								//80��ռ�ֽ���
unsigned int	g_stepInject	= 0;								//ע����е��Ĳ���
/******************************************************************************************/

/**************************************��������********************************************/
extern void matchMain(unsigned int curID,unsigned short **img);
extern void star(float starList[5][480], unsigned short *starNum);
extern void move(void);
extern void emifSend(unsigned short *starCnt,short moveCnt);
extern void para2value(unsigned int *para);
/******************************************************************************************/
#ifdef TI_PLATFORM
extern void SetupInterrupts(void);
extern void emifRecvImg(void);
extern void emifRecvEE(void);
extern void beginInject(void);

/****************************************�ж�**********************************************/
interrupt void FrameIsr4 (void)
{
	if(g_frameNum%10 == 0)							//1�����һ�β���
	{
		g_thresholdBW	= (unsigned short)(*(unsigned int *)0x03080000);	//"0010"��ֵ����ֵ
		g_thresholdMW	= (unsigned short)(*(unsigned int *)0x030C0000);	//"0011"ȥ������
		g_thresholdSTAR = (unsigned short)(*(unsigned int *)0x03100000);	//"0100"������ȡ��Ŀ
		g_thresholdBCK	= (unsigned short)(*(unsigned int *)0x031C0000);	//"0111"����Ԥ����
		g_frameNumber	= (unsigned short)(*(unsigned int *)0x03200000);	//"1000"����֡��ʼ
	}

	g_frameNum++;

	return;
}

interrupt void EEIsr5 (void)
{
	g_injectNum++;

	return;
}
/******************************************************************************************/

void main(void)
{
	*(unsigned int *)EMIF_GBLCTL	= 0x000037DB;	//GBLCTL
	*(unsigned int *)EMIF_CE1		= 0xFFFF3F03;	//CE1
	*(unsigned int *)EMIF_CE0		= 0xFFF10533;	//CE0
	*(unsigned int *)EMIF_CE2		= 0xFFFF3F33;	//CE2
	*(unsigned int *)EMIF_CE3		= 0x21520F21;	//CE3
	*(unsigned int *)EMIF_SDCTL		= 0x03115000;	//SDCTL
	SetupInterrupts();								//�����ж�

	memset((unsigned char *)0x00400000,0,IMG_WID*IMG_HEI*2);
	while(1)
	{
		if((g_frameNum_r < g_frameNum) && (g_injectNum == 0))//�жϽ��յ��µ�ͼ���Ҳ�ע��
		{
			emifRecvImg();									//����ͼ��
			if((g_frameNum_r%2) && (g_frameNum_r != 0))		//��׼
			{
				g_controlWord[g_frameNum_r/2%5]		= *(unsigned int *)0x03140000;		//"0101"���Ʋ���
				g_captureTime[g_frameNum_r/2%5][0]	= *(unsigned int *)0x03240000;		//"1001"ʱ����ֵ
				g_captureTime[g_frameNum_r/2%5][1]	= *(unsigned int *)0x03280000;		//"1010"ʱ��΢��ֵ
				matchMain(g_frameNum_r/2%5,g_imgRmb);
			}
			g_frameNum_r = g_frameNum;						//����֡��_r
			if((g_frameNum%10 == 0) && (g_frameNum != 0))	//�����չ�5֡ͼ��
			{
				para2value(g_controlWord);
				star(g_starPosi,g_starSum);					//ȷ������Ŀ��
				move();										//ȷ���˶�Ŀ��
				emifSend(g_starSum,g_moveSum);				//��������
				if(g_frameNum>=172800)
				{
					g_frameNum = 0;
					g_frameNum_r = 0;
				}
			}
		}
		else if(g_injectNum_r < g_injectNum)
		{
			emifRecvEE();							//�����յ�������д��SDRAM
			g_injectNum_r++;						//�����жϺ�_r
			if(g_injectNum == g_injectSum)			//��������
			{
				beginInject();
			}
		}
		else
		{
			*(unsigned char *)0x00e00000 = 0x55;
			*(unsigned char *)0x00e00000 = 0xAA;
		}
	}
}
#else
int main(void)
{
	memset((unsigned char *)0x00400000,0,IMG_WID*IMG_HEI*2);
	while(1)
	{
		if((g_frameNum_r < g_frameNum) && (g_injectNum == 0))//�жϽ��յ��µ�ͼ���Ҳ�ע��
		{
			if((g_frameNum_r%2) && (g_frameNum_r != 0))		//��׼
			{
				g_controlWord[g_frameNum_r/2%5]		= *(unsigned int *)0x03140000;		//"0101"���Ʋ���
				g_captureTime[g_frameNum_r/2%5][0]	= *(unsigned int *)0x03240000;		//"1001"ʱ����ֵ
				g_captureTime[g_frameNum_r/2%5][1]	= *(unsigned int *)0x03280000;		//"1010"ʱ��΢��ֵ
				matchMain(g_frameNum_r/2%5,g_imgRmb);
			}
			g_frameNum_r = g_frameNum;						//����֡��_r
			if((g_frameNum%10 == 0) && (g_frameNum != 0))	//�����չ�5֡ͼ��
			{
				para2value(g_controlWord);
				star(g_starPosi,g_starSum);					//ȷ������Ŀ��
				move();										//ȷ���˶�Ŀ��
				emifSend(g_starSum,g_moveSum);				//��������
				if(g_frameNum>=172800)
				{
					g_frameNum = 0;
					g_frameNum_r = 0;
				}
			}
		}
	}

	return 1;
}
#endif
