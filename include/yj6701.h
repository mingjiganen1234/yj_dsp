/****************************************
    This is head program
    Auther by duanxiaofeng
*****************************************/
#ifndef GCC_PLATFORM
#define GCC_PLATFORM
#endif

#ifndef YJ6701_H_
#define YJ6701_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <STDIO.H>
#include <string.h>
#include <malloc.h>
/**********************************************************************************/
/*********************�ڴ�ӳ�����map1��ʽ*****************************************/
/********0000 0000 - 0000 FFFF     64KBYTE   ����洢��****************************/
/********0040 0000 - 00FF FFFF     12MBYTE   CE0ͼ��洢��0 ***********************/
/********0100 0000 - 0013 FFFF     04MBYTE   CE0ͼ��洢��1 ***********************/
/********0140 0000 - 0140 FFFF     64KBYTE   CE1����洢��0 ***********************/
/********0141 0000 - 017F FFFF     04MBYTE   CE1����洢��1 ***********************/
/********0200 0000 - 02FF FFFF     16MBYTE   CE2ͼ��洢��2 ***********************/
/********0300 0000 - 03FF FFFF     16MBYTE   CE3 EMIF �ӿ�  ***********************/ 
/**********************************************************************************/

/***EMIF***/
#define EMIF_GBLCTL		0x01800000
#define EMIF_CE1		0x01800004
#define EMIF_CE0		0x01800008
#define EMIF_CE2		0x01800010
#define EMIF_CE3		0x01800014
#define EMIF_SDCTL		0x01800018

/***EEPROM***/
#define EE150			0x01500000
#define EE158       	0x01580000

/**ͼ����ce0 16Mbyte	0x00400000-0x013FFFFF**/
#define ORI_IMG_0		0x00400000		//g_imgOrig[0]
#define ORI_IMG_1		0x00600000		//g_imgOrig[1]
#define ORI_IMG_2		0x00800000		//g_imgOrig[2]
#define ORI_IMG_3		0x00A00000		//g_imgOrig[3]
#define ORI_IMG_4		0x00C00000		//g_imgOrig[4]
#define RMB_IMG_0		0x00E00000		//g_imgRmb[0]
#define RMB_IMG_1		0x01000000		//g_imgRmb[1]
#define RMB_IMG_2		0x01200000		//g_imgRmb[2]

/**ͼ����ce2 16Mbyte	0x02000000-0x02FFFFFF**/
#define RMB_IMG_3		0x02000000		//g_imgRmb[3]
#define RMB_IMG_4		0x02200000		//g_imgRmb[4]

#define MAX_INDEX		0x02400000		//g_maxIndex
#define COR_MAX_IDX		0x02500000		//g_corImg

#define OBJ_PNT_LST		0x0270B000		//g_objPntLst(���4*4096/16=1024����ͨ��)
#define OBJ_PNT_F0		0x0270F000      //objPointF[0]
#define OBJ_PNT_F1		0x02711000      //objPointF[1]
#define OBJ_PNT_F2		0x02713000      //objPointF[2]
#define OBJ_PNT_F3		0x02715000      //objPointF[3]
#define OBJ_PNT_F4		0x02717000      //objPointF[4]
#define OBJ_MOV_INFO	0x02719000		//g_allObjInf(���2*4096/(5��*3��*4)=68*2���˶�Ŀ��)
#define	EMIF_OUT		0x0271B000		//emifBuf

#define REG_LABEL		0x02800000		//g_regLabelMap(1024*1024B)
#define STARS_0			0x02900000		//g_stars[0](8B*10=80B)
#define STARS_1			0x02900050		//g_stars[1](8B*10=80B)
#define STARS_2			0x029000A0		//g_stars[2](8B*10=80B)
#define STARS_3			0x029000F0		//g_stars[3](8B*10=80B)
#define STARS_4			0x02900140		//g_stars[4](8B*10=80B)
#define TRIS_0			0x02900190		//g_tris[0](12B*120=1440B)
#define TRIS_1			0x02900730		//g_tris[1](12B*120=1440B)
#define TRIS_2			0x02900CD0		//g_tris[2](12B*120=1440B)
#define TRIS_3			0x02901270		//g_tris[3](12B*120=1440B)
#define TRIS_4			0x02901810		//g_tris[4](12B*120=1440B)

// ͼ�����
#define IMG_WID			1024
#define IMG_HEI			1024
#define IMG_FRM			1048576
#define REG_STAR_CNT	8						//��׼������Ŀ
#define REG_TRI_CNT		56						//��׼��������Ŀ

/*****************************************����*********************************************/	
extern unsigned short	g_thresholdBW;			//��ֵ����ֵ	
extern unsigned short	g_thresholdMW;			//ȥ����ֵ		
extern unsigned short	g_thresholdSTAR;		//����Ŀ����
extern unsigned short	g_thresholdBCK;			//����Ԥ����
extern unsigned int		g_controlWord[5];		//���Ʋ���
extern unsigned int		g_captureTime[5][2];	//����ʱ��
	
extern unsigned int		g_frameNumber;			//ͼ���֡����
extern unsigned int		g_frameNum;				//֡��	
extern unsigned int		g_frameNum_r;			//֡��_r	

/***************************************��׼***********************************************/
// ���ǽṹ�� Structure for stars
struct star
{
	unsigned short r;							//����������	Row
	unsigned short c;							//����������	Column
	unsigned short b;							//��������		Brightness
	short label;								//���Ǵ���		Label
};
// �����νṹ�� Structure for triangles
struct tri
{
	unsigned short s[3];						//���������߳�		Edges of triangle
	short labels[3];							//����������������	Labels of vertex

};
extern unsigned char	*g_regLabelMap;			//��׼��ͨ��ͼ
extern struct star		*g_stars[5];			//��׼������Ϣ
extern struct tri		*g_tris[5];				//��׼������Ϣ
extern unsigned char	 g_triCnt[5];			//��׼���Ǹ���

extern short			 g_offsetR[5];			//��ƫ����
extern short			 g_offsetC[5];			//��ƫ����

/***************************************origin*********************************************/
extern unsigned int 	*g_emifImg;				//EMIF���պͷ���ͼ������
extern unsigned short	*g_imgOrig[5];			//ԭʼͼ��洢ָ��
extern unsigned short	*g_imgRmb[5];			//ȥ��������ͼ��洢ָ��

/***************************************search*********************************************/

/****************************************star**********************************************/
extern unsigned short	 g_starSum[5];			//����Ŀ������
extern float			 g_starPosi[5][160*3];	//����Ŀ��ϸ��(3��*160��)

/****************************************move**********************************************/
extern unsigned short	 s_origVal[5][1024];	//������
extern unsigned char	 s_maxIdx[1024];     	//������
extern unsigned char	*g_maxIndex;			//���ֵ����֡
extern unsigned char	*g_corImg;				//ȥ���	
extern unsigned short	 g_objNum[5];			//ÿ֡ͼ����ͨ����(5��1��)	
extern float			*g_objPntLst;			//�洢֡�š���ͨ�򡢼���������	
extern float			*objPointF[5];			//��0,1,2,3,4֡ͼ��ͨ���������Ϣ
extern float			*g_allObjInf;			//�˶�Ŀ��ϸ��	
extern unsigned short	 g_moveSum;				//�˶�Ŀ������

/******************************ע��************************************/
extern unsigned short	 g_injectNum;			//ע������жϼ���
extern unsigned short	 g_injectNum_r;			//ע������жϼ���_r
extern unsigned short	 g_injectSum;			//�ó���ε��ж��ܴ���
extern unsigned int		*g_AreaEE;				//��ע�������ݴ�ĵ�ַ
extern unsigned int		 g_lenArea00;			//00��ռ�ֽ���
extern unsigned int	 	 g_lenArea80;			//80��ռ�ֽ���
extern unsigned int	 	 g_stepInject;			//ע����е��Ĳ���
/**********************************************************************/
#ifdef GCC_PLATFORM
extern void init();
extern int readFile(char *str,int frmBeg,unsigned int time[5][2], unsigned int *framePara,unsigned int *control_word);
extern void filter(unsigned short **inImg,unsigned short **outImg,unsigned short cnt,unsigned short window,unsigned short wid,unsigned short hei);
extern void writeImg(void *p,char *str,unsigned int len);
extern void writePosi(int frm);
extern FILE *fWriteHex;
extern void save(unsigned short frameCnt, unsigned short *starCnt, short moveCnt);
#endif
#ifdef __cplusplus
}
#endif

#endif /* YJ6701_H_ */
