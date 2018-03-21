#include <stdio.h>
#include <math.h>
#include "../include/yj6701.h"

/********************************��ȡ���ֵ֡����************************************/
void getMaxIndex(void)	//1560ms
{
	int				j = 0, row = 0, col = 0, max = 0, index = 0;
	float			mean = 0, realVal = 0;

	memset(g_maxIndex, 0, IMG_FRM);

	for (row = 1; row<IMG_HEI; row++)
	{
		//ȡ�˳���ͼ��ֵ
		for (j = 0; j<5; j++)
		{
			memset(s_origVal[j], 0, 2048);
			//ֻ������Ч
			if (((g_offsetR[j] <= 0) && (row + g_offsetR[j] >= 0)) || ((g_offsetR[j]>0) && (row + g_offsetR[j]<IMG_WID)))
			{
				if (g_offsetC[j] >= 0)
				{
					memcpy(s_origVal[j], g_imgRmb[j] + (row + g_offsetR[j])*IMG_WID + g_offsetC[j], 2048 - g_offsetC[j] * 2);
				}
				else
				{
					memcpy(s_origVal[j] - g_offsetC[j], g_imgRmb[j] + (row + g_offsetR[j])*IMG_WID, 2048 + g_offsetC[j] * 2);
				}
			}
		}

		memset(s_maxIdx, 0, 1024);
		for (col = 0; col<IMG_WID; col++)
		{
			//�����ֵ�����ֵ֡������1-5��
			max = 0;
			index = 0;
			mean = 0;
			for (j = 0; j<5; j++)
			{
				mean += s_origVal[j][col];
				if (s_origVal[j][col] > max)
				{
					max = s_origVal[j][col];
					index = j + 1;
				}
			}
			//������ֵ
			mean = (mean - max) / 4.0;
			//��ʵֵ=���ֵ-��ֵ
			realVal = max - mean;
			//��������Ϊ0
			if (mean>0)
			{
				if ((realVal > 3.8 * mean) && (realVal > 2.0 * g_thresholdBCK))
				{
					s_maxIdx[col] = index;
				}
			}
			else
			{
				//���ֵ-��ֵ����2.0*�������������ֵ֡��������Ч
				if (realVal > 2.0 * g_thresholdBCK)
				{
					s_maxIdx[col] = index;
				}
			}
		}
		//���ֵ֡����Ӧ����ԭ�Σ���Ϊ�����Ĺ���Ҫ�õ�
		memcpy(g_maxIndex + row*IMG_WID, s_maxIdx, 1024);
	}
	return;
}

/*************************ȥ�����ֵ����֡���ϸ���ӳ���*****************************/
void clear(void)	//610ms
{
	unsigned short	i = 0, j = 0;
	unsigned char	val = 0, sum = 0, val_r = 0;
	unsigned short	sum_r = 0;

	if (g_thresholdMW > 8)		{g_thresholdMW = 8;}
	if (g_thresholdMW < 1)		{g_thresholdMW = 1;}
	//��ʼ��ȥ��󻺴�
	memset(g_corImg, 0, IMG_FRM);

	for (i = 1; i<IMG_HEI - 1; i++)
	{
		for (j = 1; j<IMG_WID - 1; j++)
		{
			//ȡ֡����
			val = g_maxIndex[i*IMG_WID + j];
			if (val == 0) { sum_r = 0; }
			else
			{
				//Ŀ�곬��25*25��Ϊͼ������
				if (val != val_r) { val_r = val;	sum_r = 0; }
				else { sum_r++;	if (sum_r > 25) { break; } }

				//��8�������
				sum = 0;
				if (g_maxIndex[(i - 1)*IMG_WID + j - 1] == val)		{sum++;}
				if (g_maxIndex[(i - 1)*IMG_WID + j] == val)			{sum++;}
				if (g_maxIndex[(i - 1)*IMG_WID + j + 1] == val)		{sum++;}
				if (g_maxIndex[i	  *IMG_WID + j - 1] == val)		{sum++;}
				if (g_maxIndex[i	  *IMG_WID + j + 1] == val)		{sum++;}
				if (g_maxIndex[(i + 1)*IMG_WID + j - 1] == val)		{sum++;}
				if (g_maxIndex[(i + 1)*IMG_WID + j] == val)			{sum++;}
				if (g_maxIndex[(i + 1)*IMG_WID + j + 1] == val)		{sum++;}
				if (sum >= g_thresholdMW) { g_corImg[i*IMG_WID + j] = val; }
			}
		}
	}
}

/********************************��ͨ��**********************************************/
unsigned short region(void)
{
	unsigned short regionCount = 0;					//��ͨ�����
	unsigned short minX,maxX,minY,maxY;				//��ͨ��ı߽�����
	unsigned short i=0,j=0,a=0,b=0,queueCount=0;
	unsigned char  m;
	unsigned short flag = 6;						//��ͨ���(6,7,8,9...)
	unsigned short pointQueue = 0;					//queue��ָ��
	unsigned short queue[2048];
	
	//�����ͨ�򻺴�
	memset(g_objNum,0,10);
	memset(g_objPntLst,0,4*4096);

	//��ȡ��ͨ��
	for(i=1; i<IMG_HEI-1; i++)						//ɨ����
	{
		for(j=1; j<IMG_WID-1; j++)					//ɨ����
		{
			m = g_corImg[i*IMG_WID+j];			

			if((m<=5)&&(m>0))						//���Ϊ1,2,3,4,5
			{
				g_corImg[i*IMG_WID+j] = flag;		//�޸���ͨ���
				minX = maxX = j;					//���õ�������ʱ��Ϊx�߽�ֵ
				minY = maxY = i;					//���õ�������ʱ��Ϊy�߽�ֵ

				pointQueue = 0;						//queue��ָ��
				memset(queue,0,4096);				//queue��ʼ��Ϊ0
				queue[pointQueue++] = j;			//��1�����ص�x
				queue[pointQueue++] = i;			//��1�����ص�y

				for(queueCount=0;queueCount<pointQueue/2;queueCount++)				//����queue�еĵ�queueCount��
				{
					for(a=queue[queueCount*2+1]-1;a<=queue[queueCount*2+1]+1;a++)	//������
					{
						for(b=queue[queueCount*2]-1;b<=queue[queueCount*2]+1;b++)	//������
						{
							if(g_corImg[a*IMG_WID+b] == m)			//�ж��Ƿ�Ϊm
							{
								g_corImg[a*IMG_WID+b] = flag;		//���
								queue[pointQueue++] = b;			//��һ�����ص�x
								queue[pointQueue++] = a;			//��һ�����ص�y
								if (b < minX)						//���С��minX
								{
									minX = b;						//�޸�minX
								}
								else if (b > maxX)					//�������maxX
								{
									maxX = b;						//�޸�maxX
								}
								else { b = b; }
								
								if (a < minY)						//���С��minY
								{
									minY = a;						//�޸�minY
								}
								else if (a > maxY)					//�������maxY
								{
									maxY = a;						//�޸�maxY
								}
								else { a = a; }
								if(pointQueue==2048)
								{
									return regionCount;
								}
							}
						}
					}
				}
				g_objPntLst[regionCount*4] = m;						//֡��(1.2.3.4.5)
				g_objPntLst[regionCount*4+1] = flag;				//��ͨ����(6,7,8,9...)
				g_objPntLst[regionCount*4+2] = (float)(minY+maxY)/2;//��
				g_objPntLst[regionCount*4+3] = (float)(minX+maxX)/2;//��
				g_objNum[m-1]++;									//ÿ֡��ͨ�����+1
				regionCount++;										//��ͨ�����+1
				if(regionCount >= 768)
				{
					return regionCount;
				}
				flag++;	if (flag == 255) { flag = 6; }				//���Ʒ�Χ
			}
		}
	}

	return regionCount;
}

/********************************�˶��켣********************************************/
/*******************************����˵��********************************************
	objNum����¼ÿ֡ͼ�����ͨ����						��������
	objPointList���洢֡�š���ͨ�򡢼���������			��������
	allObjInf��ÿ�����ͨ����*5��3��,Ҫ����ľ���
			   (ÿһ����һ֡��Ϣ,0.1��Ϊ����,2��Ϊ֡��)	�������
				��1.2��	��0.1��	��3.4��	��0.2��
				0.	��	��	��	��	��----��
				1.	��	��	��	��	��----��
				2.	��	��	��	��	��----��
				3.	��	��	��	��	��----��
				4.	��	��	��	��	��----��
				5.	��	��	��	��	��----��
				6.	��	��	��	��	��----��
				7.	��	��	��	��	��----��
				8.	��	��	��	��	��----��
				9.	��	��	��	��	��----��
				10.	��	��	��	��	��----��
				11.	��	��	��	��	��----��
				12.	��	��	��	��	��----��
				13.	��	��	��	��	��----��
				14.	��	��	��	��	��----��
				15.	��	��	��	��	��----��
************************************************************************************/
unsigned short movepath(unsigned short regionCount)
{
	float			err=3.3;						//��ֵ
	unsigned short	Vr = 30;						//�����˶��ٶȵ�����
	unsigned short	curObjNum = 0,preObjNum = 0;	//allObjInf��Ŀ����
	unsigned short	i=0,j=0,k=0,obj=0;				//����ѭ��
	float			a=0,b=0,c=0,d=0,e=0,f=0;		//��0,1,2,3,4֡ͼ��ͨ���������Ϣ;abcdef���3���������ֵ;abΪ�м��;cdΪǰ���;efΪ�����
	unsigned short	p[5]={0,0,0,0,0};					//�������ʱѰַ
	float curDisErr = 0,curDP = 0,curDPErr =0,minDisErr=0,minDPErr=0;	//�ж��Ƿ���������
	float dis01=0,dis02=0,dis03=0,dis12=0,dis13=0,dis14=0,dis23=0,dis24=0,dis34=0;
	float r01[2]={0,0},r02[2]={0,0},r03[2]={0,0},r12[2]={0,0},r13[2]={0,0},r14[2]={0,0},r23[2]={0,0},r24[2]={0,0},r34[2]={0,0};
	unsigned char is_continue = 1;							//�Ƿ����ִ����һ��forѭ��

	memset(g_allObjInf,0,4096*2);
	#ifdef TI_PLATFORM
	memset(objPointF[0],0,8192*5);
	#endif
	
	for(i=0;i<regionCount;i++)
	{
		j = (int)g_objPntLst[i*4]-1;
		objPointF[j][p[j]*2] = g_objPntLst[i*4+2];
		objPointF[j][p[j]*2+1] = g_objPntLst[i*4+3];
		p[j]++;
	}

	//��1��:1.2��-----------------------------------------------------------------------------------
	for(i=0;i<g_objNum[2];i++){									//������2֡����ͨ������
		for(j=0;j<g_objNum[1];j++)								//������1֡����ͨ������
		{
			//-------------------------------------12-------------------------------------------------
			a = objPointF[2][i*2];
			b = objPointF[2][i*2+1];
			c = objPointF[1][j*2];
			d = objPointF[1][j*2+1];
			if((fabs(a+1)>0.0000001) && (fabs(b+1)>0.0000001) && (fabs(c+1)>0.0000001) && (fabs(d+1)>0.0000001)){
			dis12 = sqrt((a-c)*(a-c) + (b-d)*(b-d));			//2->1����
			r12[0] = a-c;										//2->1����
			r12[1] = b-d;										//2->1����
			if(dis12<=Vr)
			{
				//-------------------------------------23-------------------------------------------------
				minDisErr = Vr;	minDPErr = Vr;	is_continue = 1;
				for(k=0;k<g_objNum[3];k++)
				{
					e = objPointF[3][k*2];
					f = objPointF[3][k*2+1];
					if((fabs(e+1)>0.0000001) && (fabs(f+1)>0.0000001)){
					dis23 = sqrt((e-a)*(e-a) + (f-b)*(f-b));	//3->2����
					r23[0] = e-a;								//3->2����
					r23[1] = f-b;								//3->2����

					curDP = (r12[0]*r23[0]+r12[1]*r23[1])/(dis12*dis23);	//cos��
					curDPErr = 1-curDP;										//1-cos��
					curDisErr = abs(dis12-dis23);							//����ƫ�����ֵ
				
					if((curDisErr<minDisErr)&&(curDisErr<=err)&&(curDPErr<minDPErr)&&(curDPErr<=0.1))	//����ͽǶȶ�����
					{
						minDisErr = curDisErr;	minDPErr = curDPErr;
						for(obj=curObjNum*5;obj<(curObjNum+1)*5;obj++){	//g_allObjInf((curObjNum-1)*5+1:curObjNum*5,3) = curObjNum;
							g_allObjInf[obj*3+2] = curObjNum+1;}
						g_allObjInf[(curObjNum*5+1)*3]		= objPointF[1][j*2];
						g_allObjInf[(curObjNum*5+1)*3+1]	= objPointF[1][j*2+1];
						g_allObjInf[(curObjNum*5+2)*3]		= objPointF[2][i*2];
						g_allObjInf[(curObjNum*5+2)*3+1]	= objPointF[2][i*2+1];
						g_allObjInf[(curObjNum*5+3)*3]		= objPointF[3][k*2];
						g_allObjInf[(curObjNum*5+3)*3+1]	= objPointF[3][k*2+1];
						objPointF[1][j*2]	= -1;
						objPointF[1][j*2+1]	= -1;
						objPointF[2][i*2]	= -1;
						objPointF[2][i*2+1]	= -1;
						objPointF[3][k*2]	= -1;
						objPointF[3][k*2+1]	= -1;
						curObjNum++;
						if(curObjNum>39)	return curObjNum;
						is_continue			= 0;
					}}
				}
				//-------------------------------------24-------------------------------------------------
				if(is_continue != 0){			//������������
				minDisErr = Vr;	minDPErr = Vr;
				for(k=0;k<g_objNum[4];k++)
				{
					e = objPointF[4][k*2];
					f = objPointF[4][k*2+1];
					if((fabs(e+1)>0.0000001) && (fabs(f+1)>0.0000001)){
					dis24 = sqrt((e-a)*(e-a) + (f-b)*(f-b));	//4->2����
					r24[0] = e-a;								//4->2����
					r24[1] = f-b;								//4->2����

					curDP = (r12[0]*r24[0]+r12[1]*r24[1])/(dis12*dis24);	//cos��
					curDPErr = 1-curDP;										//1-cos��
					curDisErr = abs(dis12-dis24/2);							//����ƫ�����ֵ
				
					if((curDisErr<minDisErr)&&(curDisErr<=err)&&(curDPErr<minDPErr)&&(curDPErr<=0.1))	//����ͽǶȶ�����
					{
						minDisErr = curDisErr;	minDPErr = curDPErr;
						for(obj=curObjNum*5;obj<(curObjNum+1)*5;obj++){	//g_allObjInf((curObjNum-1)*5+1:curObjNum*5,3) = curObjNum;
							g_allObjInf[obj*3+2] = curObjNum+1;}
						g_allObjInf[(curObjNum*5+1)*3]		= objPointF[1][j*2];
						g_allObjInf[(curObjNum*5+1)*3+1]	= objPointF[1][j*2+1];
						g_allObjInf[(curObjNum*5+2)*3]		= objPointF[2][i*2];
						g_allObjInf[(curObjNum*5+2)*3+1]	= objPointF[2][i*2+1];
						g_allObjInf[(curObjNum*5+4)*3]		= objPointF[4][k*2];
						g_allObjInf[(curObjNum*5+4)*3+1]	= objPointF[4][k*2+1];
						objPointF[1][j*2]	= -1;
						objPointF[1][j*2+1]	= -1;
						objPointF[2][i*2]	= -1;
						objPointF[2][i*2+1]	= -1;
						objPointF[4][k*2]	= -1;
						objPointF[4][k*2+1]	= -1;
						curObjNum++;
						if(curObjNum>39)	return curObjNum;
					}}
				}}
			}
		}}}
	
	for(i=0;i<curObjNum;i++)
	{
		//---------------------------------01-12-----------------------------------------
		minDisErr = Vr;	minDPErr = Vr;
		a = g_allObjInf[(i*5+1)*3];		//1����
		b = g_allObjInf[(i*5+1)*3+1];
		e = g_allObjInf[(i*5+2)*3];		//2����
		f = g_allObjInf[(i*5+2)*3+1];
		for(j=0;j<g_objNum[0];j++)								//������0֡����ͨ������
		{
			c		= objPointF[0][j*2];						//0����
			d		= objPointF[0][j*2+1];
			if((fabs(c+1)>0.0000001) && (fabs(d+1)>0.0000001)){
			dis01	= sqrt((a-c)*(a-c) + (b-d)*(b-d));			//1->0����
			dis12	= sqrt((e-a)*(e-a) + (f-b)*(f-b));			//2->1����
			r01[0]	= a-c;										//1->0����
			r01[1]	= b-d;										//1->0����
			r12[0]	= e-a;										//2->1����
			r12[1]	= f-b;										//2->1����
			
			curDP = (r01[0]*r12[0]+r01[1]*r12[1])/(dis01*dis12);//cos��
			curDPErr = 1-curDP;									//1-cos��
			curDisErr = abs(dis01-dis12);						//����ƫ�����ֵ
			
			if((curDisErr<minDisErr)&&(curDisErr<=err)&&(curDPErr<minDPErr)&&(curDPErr<=0.1))	//����ͽǶȶ�����
			{
				minDPErr=curDPErr;
				minDisErr=curDisErr;
				g_allObjInf[(i*5)*3]	= objPointF[0][j*2];
				g_allObjInf[(i*5)*3+1]	= objPointF[0][j*2+1];
				objPointF[0][j*2]		= -1;
				objPointF[0][j*2+1]		= -1;
			}}
		}
		
		//--------------------------------23-34------------------------------------------
		minDisErr = Vr;	minDPErr = Vr;
		a = g_allObjInf[(i*5+3)*3];		//3����
		b = g_allObjInf[(i*5+3)*3+1];
		c = g_allObjInf[(i*5+2)*3];		//2����
		d = g_allObjInf[(i*5+2)*3+1];
		if((fabs(a) > 0.0000001) && (fabs(b) > 0.0000001)){					//û��3˵����4������
		for(j=0;j<g_objNum[4];j++)				//������4֡����ͨ������
		{
			e		= objPointF[4][j*2];					//4����
			f		= objPointF[4][j*2+1];
			if((fabs(e+1)>0.0000001) && (fabs(f+1)>0.0000001)){
			dis23	= sqrt((a-c)*(a-c) + (b-d)*(b-d));			//3->2����
			dis34	= sqrt((e-a)*(e-a) + (f-b)*(f-b));			//4->3����
			r23[0]	= a-c;										//3->2����
			r23[1]	= b-d;										//3->2����
			r34[0]	= e-a;										//4->3����
			r34[1]	= f-b;										//4->3����

			curDP = (r34[0]*r23[0]+r34[1]*r23[1])/(dis34*dis23);//cos��
			curDPErr = 1-curDP;									//1-cos��
			curDisErr = abs(dis34-dis23);						//����ƫ�����ֵ

			if((curDisErr<minDisErr)&&(curDisErr<=err)&&(curDPErr<minDPErr)&&(curDPErr<=0.1))	//����ͽǶȶ�����
			{
				minDPErr=curDPErr;
				minDisErr=curDisErr;
				g_allObjInf[(i*5+4)*3]		= objPointF[4][j*2];
				g_allObjInf[(i*5+4)*3+1]	= objPointF[4][j*2+1];
				objPointF[4][j*2]			= -1;
				objPointF[4][j*2+1]			= -1;
			}}
		}}
	}
	preObjNum = curObjNum;
	
	//��2��:0.1��-----------------------------------------------------------------------------------
	for(i=0;i<g_objNum[1];i++){									//������1֡����ͨ������
		for(j=0;j<g_objNum[0];j++)								//������0֡����ͨ������
		{
			//-------------------------------------01-------------------------------------------------
			a = objPointF[1][i*2];
			b = objPointF[1][i*2+1];
			c = objPointF[0][j*2];
			d = objPointF[0][j*2+1];
			if((fabs(a+1)>0.0000001) && (fabs(b+1)>0.0000001) && (fabs(c+1)>0.0000001) && (fabs(d+1)>0.0000001)){
			dis01 = sqrt((a-c)*(a-c) + (b-d)*(b-d));			//1->0����
			r01[0] = a-c;										//1->0����
			r01[1] = b-d;										//1->0����
			if(dis01<=Vr)
			{
				//-------------------------------------12-------------------------------------------------
				minDisErr = Vr;	minDPErr = Vr;	is_continue = 1;
				for(k=0;k<g_objNum[2];k++)
				{
					e = objPointF[2][k*2];
					f = objPointF[2][k*2+1];
					if((fabs(e+1)>0.0000001) && (fabs(f+1)>0.0000001)){
					dis12 = sqrt((e-a)*(e-a) + (f-b)*(f-b));	//2->1����
					r12[0] = e-a;								//2->1����
					r12[1] = f-b;								//2->1����

					curDP = (r01[0]*r12[0]+r01[1]*r12[1])/(dis01*dis12);	//cos��
					curDPErr = 1-curDP;										//1-cos��
					curDisErr = abs(dis01-dis12);							//����ƫ�����ֵ
				
					if((curDisErr<minDisErr)&&(curDisErr<=err)&&(curDPErr<minDPErr)&&(curDPErr<=0.1))	//����ͽǶȶ�����
					{
						minDisErr = curDisErr;	minDPErr = curDPErr;
						for(obj=curObjNum*5;obj<(curObjNum+1)*5;obj++){	//g_allObjInf((curObjNum-1)*5+1:curObjNum*5,3) = curObjNum;
							g_allObjInf[obj*3+2] = curObjNum+1;}
						g_allObjInf[(curObjNum*5+0)*3]		= objPointF[0][j*2];
						g_allObjInf[(curObjNum*5+0)*3+1]	= objPointF[0][j*2+1];
						g_allObjInf[(curObjNum*5+1)*3]		= objPointF[1][i*2];
						g_allObjInf[(curObjNum*5+1)*3+1]	= objPointF[1][i*2+1];
						g_allObjInf[(curObjNum*5+2)*3]		= objPointF[2][k*2];
						g_allObjInf[(curObjNum*5+2)*3+1]	= objPointF[2][k*2+1];
						objPointF[0][j*2]	= -1;
						objPointF[0][j*2+1]	= -1;
						objPointF[1][i*2]	= -1;
						objPointF[1][i*2+1]	= -1;
						objPointF[2][k*2]	= -1;
						objPointF[2][k*2+1]	= -1;
						curObjNum++;
						if(curObjNum>39)	return curObjNum;
						is_continue			= 0;
					}}
				}
				//-------------------------------------13-------------------------------------------------(û��2)
				if(is_continue != 0){			//������������
				minDisErr = Vr;	minDPErr = Vr;
				for(k=0;k<g_objNum[3];k++)
				{
					e = objPointF[3][k*2];
					f = objPointF[3][k*2+1];
					if((fabs(e+1)>0.0000001) && (fabs(f+1)>0.0000001)){
					dis13 = sqrt((e-a)*(e-a) + (f-b)*(f-b));	//3->1����
					r13[0] = e-a;								//3->1����
					r13[1] = f-b;								//3->1����

					curDP = (r01[0]*r13[0]+r01[1]*r13[1])/(dis01*dis13);	//cos��
					curDPErr = 1-curDP;										//1-cos��
					curDisErr = abs(dis01-dis13/2);							//����ƫ�����ֵ
				
					if((curDisErr<minDisErr)&&(curDisErr<=err)&&(curDPErr<minDPErr)&&(curDPErr<=0.1))	//����ͽǶȶ�����
					{
						minDisErr = curDisErr;	minDPErr = curDPErr;
						for(obj=curObjNum*5;obj<(curObjNum+1)*5;obj++){	//g_allObjInf((curObjNum-1)*5+1:curObjNum*5,3) = curObjNum;
							g_allObjInf[obj*3+2] = curObjNum+1;}
						g_allObjInf[(curObjNum*5+0)*3]		= objPointF[0][j*2];
						g_allObjInf[(curObjNum*5+0)*3+1]	= objPointF[0][j*2+1];
						g_allObjInf[(curObjNum*5+1)*3]		= objPointF[1][i*2];
						g_allObjInf[(curObjNum*5+1)*3+1]	= objPointF[1][i*2+1];
						g_allObjInf[(curObjNum*5+3)*3]		= objPointF[3][k*2];
						g_allObjInf[(curObjNum*5+3)*3+1]	= objPointF[3][k*2+1];
						objPointF[0][j*2]	= -1;
						objPointF[0][j*2+1]	= -1;
						objPointF[1][i*2]	= -1;
						objPointF[1][i*2+1]	= -1;
						objPointF[3][k*2]	= -1;
						objPointF[3][k*2+1]	= -1;
						curObjNum++;
						if(curObjNum>39)	return curObjNum;
						is_continue			= 0;
					}}
				}
				//-------------------------------------14-------------------------------------------------(û��2.3)
				if(is_continue != 0){	//������������
				minDisErr = Vr;	minDPErr = Vr;
				for(k=0;k<g_objNum[4];k++)
				{
					e = objPointF[4][k*2];
					f = objPointF[4][k*2+1];
					if((fabs(e+1)<0.0000001) || (fabs(f+1)<0.0000001))	{;}	else{
					dis14 = sqrt((e-a)*(e-a) + (f-b)*(f-b));	//3->1����
					r14[0] = e-a;								//3->1����
					r14[1] = f-b;								//3->1����

					curDP = (r01[0]*r14[0]+r01[1]*r14[1])/(dis01*dis14);	//cos��
					curDPErr = 1-curDP;										//1-cos��
					curDisErr = abs(dis01-dis14/3);							//����ƫ�����ֵ
				
					if((curDisErr<minDisErr)&&(curDisErr<=err)&&(curDPErr<minDPErr)&&(curDPErr<=0.1))	//����ͽǶȶ�����
					{
						minDisErr = curDisErr;	minDPErr = curDPErr;
						for(obj=curObjNum*5;obj<(curObjNum+1)*5;obj++){	//g_allObjInf((curObjNum-1)*5+1:curObjNum*5,3) = curObjNum;
							g_allObjInf[obj*3+2] = curObjNum+1;}
						g_allObjInf[(curObjNum*5+0)*3]		= objPointF[0][j*2];
						g_allObjInf[(curObjNum*5+0)*3+1]	= objPointF[0][j*2+1];
						g_allObjInf[(curObjNum*5+1)*3]		= objPointF[1][i*2];
						g_allObjInf[(curObjNum*5+1)*3+1]	= objPointF[1][i*2+1];
						g_allObjInf[(curObjNum*5+4)*3]		= objPointF[4][k*2];
						g_allObjInf[(curObjNum*5+4)*3+1]	= objPointF[4][k*2+1];
						objPointF[0][j*2]	= -1;
						objPointF[0][j*2+1]	= -1;
						objPointF[1][i*2]	= -1;
						objPointF[1][i*2+1]	= -1;
						objPointF[4][k*2]	= -1;
						objPointF[4][k*2+1]	= -1;
						curObjNum++;
						if(curObjNum>39)	return curObjNum;
						is_continue			= 0;
					}}
				}}}
			}}
		}}
	
	for(i=preObjNum;i<curObjNum;i++)
	{
		minDisErr = Vr;	minDPErr = Vr;
		a = g_allObjInf[(i*5+2)*3];				//2����
		b = g_allObjInf[(i*5+2)*3+1];
		if((fabs(a)<0.0000001) || (fabs(b)<0.0000001))				//û��2
		{
			a = g_allObjInf[(i*5+3)*3];			//3����
			b = g_allObjInf[(i*5+3)*3+1];
			if((fabs(a)>0.0000001) && (fabs(b)>0.0000001)){	//û��3
			//---------------------------------13-34-----------------------------------------
			c = g_allObjInf[(i*5+1)*3];			//1����
			d = g_allObjInf[(i*5+1)*3+1];
			for(j=0;j<g_objNum[4];j++)								//������4֡����ͨ������
			{
				e		= objPointF[4][j*2];						//4����
				f		= objPointF[4][j*2+1];
				if((fabs(e+1)>0.0000001) && (fabs(f+1)>0.0000001)){
				dis13	= sqrt((a-c)*(a-c) + (b-d)*(b-d));			//3->1����
				dis34	= sqrt((e-a)*(e-a) + (f-b)*(f-b));			//4->3����
				r13[0]	= a-c;										//3->1����
				r13[1]	= b-d;										//3->1����
				r34[0]	= e-a;										//4->3����
				r34[1]	= f-b;										//4->3����
			
				curDP = (r13[0]*r34[0]+r13[1]*r34[1])/(dis13*dis34);//cos��
				curDPErr = 1-curDP;									//1-cos��
				curDisErr = abs(dis13/2-dis34);						//����ƫ�����ֵ
			
				if((curDisErr<minDisErr)&&(curDisErr<=err)&&(curDPErr<minDPErr)&&(curDPErr<=0.1))	//����ͽǶȶ�����
				{
					minDPErr=curDPErr;
					minDisErr=curDisErr;
					g_allObjInf[(i*5+4)*3]		= objPointF[4][j*2];
					g_allObjInf[(i*5+4)*3+1]	= objPointF[4][j*2+1];
					objPointF[4][j*2]			= -1;
					objPointF[4][j*2+1]			= -1;
				}}
			}}
		}
	}
	preObjNum = curObjNum;
	
	//��3��:3.4��-----------------------------------------------------------------------------------
	for(i=0;i<g_objNum[3];i++){									//������3֡����ͨ������
		for(j=0;j<g_objNum[4];j++)								//������4֡����ͨ������
		{
			//-------------------------------------34-------------------------------------------------
			a = objPointF[3][i*2];
			b = objPointF[3][i*2+1];
			e = objPointF[4][j*2];
			f = objPointF[4][j*2+1];
			if((fabs(a+1)>0.0000001) && (fabs(b+1)>0.0000001) && (fabs(e+1)>0.0000001) && (fabs(f+1)>0.0000001)){
			dis34 = sqrt((e-a)*(e-a) + (f-b)*(f-b));			//4->3����
			r34[0] = e-a;										//4->3����
			r34[1] = f-b;										//4->3����
			if(dis34<=Vr)
			{
				//-------------------------------------23-------------------------------------------------
				minDisErr = Vr;	minDPErr = Vr;	is_continue = 1;
				for(k=0;k<g_objNum[2];k++)
				{
					c = objPointF[2][k*2];
					d = objPointF[2][k*2+1];
					if((fabs(c+1)>0.0000001) && (fabs(d+1)>0.0000001)){
					dis23 = sqrt((a-c)*(a-c) + (b-d)*(b-d));	//3->2����
					r23[0] = a-c;								//3->2����
					r23[1] = b-d;								//3->2����

					curDP = (r23[0]*r34[0]+r23[1]*r34[1])/(dis23*dis34);	//cos��
					curDPErr = 1-curDP;										//1-cos��
					curDisErr = abs(dis23-dis34);							//����ƫ�����ֵ
				
					if((curDisErr<minDisErr)&&(curDisErr<=err)&&(curDPErr<minDPErr)&&(curDPErr<=0.1))	//����ͽǶȶ�����
					{
						minDisErr = curDisErr;	minDPErr = curDPErr;
						for(obj=curObjNum*5;obj<(curObjNum+1)*5;obj++){	//g_allObjInf((curObjNum-1)*5+1:curObjNum*5,3) = curObjNum;
							g_allObjInf[obj*3+2] = curObjNum+1;}
						g_allObjInf[(curObjNum*5+4)*3]		= objPointF[4][j*2];
						g_allObjInf[(curObjNum*5+4)*3+1]	= objPointF[4][j*2+1];
						g_allObjInf[(curObjNum*5+3)*3]		= objPointF[3][i*2];
						g_allObjInf[(curObjNum*5+3)*3+1]	= objPointF[3][i*2+1];
						g_allObjInf[(curObjNum*5+2)*3]		= objPointF[2][k*2];
						g_allObjInf[(curObjNum*5+2)*3+1]	= objPointF[2][k*2+1];
						objPointF[4][j*2]	= -1;
						objPointF[4][j*2+1]	= -1;
						objPointF[3][i*2]	= -1;
						objPointF[3][i*2+1]	= -1;
						objPointF[2][k*2]	= -1;
						objPointF[2][k*2+1]	= -1;
						curObjNum++;
						if(curObjNum>39)	return curObjNum;
						is_continue			= 0;
					}}
				}
				//-------------------------------------13-------------------------------------------------(û��2)
				if(is_continue != 0){	//������������
				minDisErr = Vr;	minDPErr = Vr;
				for(k=0;k<g_objNum[1];k++)
				{
					c = objPointF[1][k*2];
					d = objPointF[1][k*2+1];
					if((fabs(c+1)>0.0000001) && (fabs(d+1)>0.0000001)){
					dis13 = sqrt((a-c)*(a-c) + (b-d)*(b-d));	//3->1����
					r13[0] = a-c;								//3->1����
					r13[1] = b-d;								//3->1����

					curDP = (r13[0]*r34[0]+r13[1]*r34[1])/(dis13*dis34);	//cos��
					curDPErr = 1-curDP;										//1-cos��
					curDisErr = abs(dis13/2-dis34);							//����ƫ�����ֵ
				
					if((curDisErr<minDisErr)&&(curDisErr<=err)&&(curDPErr<minDPErr)&&(curDPErr<=0.1))	//����ͽǶȶ�����
					{
						minDisErr = curDisErr;	minDPErr = curDPErr;
						for(obj=curObjNum*5;obj<(curObjNum+1)*5;obj++){	//g_allObjInf((curObjNum-1)*5+1:curObjNum*5,3) = curObjNum;
							g_allObjInf[obj*3+2] = curObjNum+1;}
						g_allObjInf[(curObjNum*5+4)*3]		= objPointF[4][j*2];
						g_allObjInf[(curObjNum*5+4)*3+1]	= objPointF[4][j*2+1];
						g_allObjInf[(curObjNum*5+3)*3]		= objPointF[3][i*2];
						g_allObjInf[(curObjNum*5+3)*3+1]	= objPointF[3][i*2+1];
						g_allObjInf[(curObjNum*5+1)*3]		= objPointF[1][k*2];
						g_allObjInf[(curObjNum*5+1)*3+1]	= objPointF[1][k*2+1];
						objPointF[4][j*2]	= -1;
						objPointF[4][j*2+1]	= -1;
						objPointF[3][i*2]	= -1;
						objPointF[3][i*2+1]	= -1;
						objPointF[1][k*2]	= -1;
						objPointF[1][k*2+1]	= -1;
						curObjNum++;
						if(curObjNum>39)	return curObjNum;
						is_continue			= 0;
					}}
				}
				//-------------------------------------03-------------------------------------------------(û��1.2)
				if(is_continue != 0){	//������������
				minDisErr = Vr;	minDPErr = Vr;
				for(k=0;k<g_objNum[0];k++)
				{
					c = objPointF[0][k*2];
					d = objPointF[0][k*2+1];
					if((fabs(c+1)>0.0000001) && (fabs(d+1)>0.0000001)){
					dis03 = sqrt((a-c)*(a-c) + (b-d)*(b-d));	//3->0����
					r03[0] = a-c;								//3->0����
					r03[1] = b-d;								//3->0����

					curDP = (r03[0]*r34[0]+r03[1]*r34[1])/(dis03*dis34);	//cos��
					curDPErr = 1-curDP;										//1-cos��
					curDisErr = abs(dis03/3-dis34);							//����ƫ�����ֵ
				
					if((curDisErr<minDisErr)&&(curDisErr<=err)&&(curDPErr<minDPErr)&&(curDPErr<=0.1))	//����ͽǶȶ�����
					{
						minDisErr = curDisErr;	minDPErr = curDPErr;
						for(obj=curObjNum*5;obj<(curObjNum+1)*5;obj++){	//g_allObjInf((curObjNum-1)*5+1:curObjNum*5,3) = curObjNum;
							g_allObjInf[obj*3+2] = curObjNum+1;}
						g_allObjInf[(curObjNum*5+4)*3]		= objPointF[4][j*2];
						g_allObjInf[(curObjNum*5+4)*3+1]	= objPointF[4][j*2+1];
						g_allObjInf[(curObjNum*5+3)*3]		= objPointF[3][i*2];
						g_allObjInf[(curObjNum*5+3)*3+1]	= objPointF[3][i*2+1];
						g_allObjInf[(curObjNum*5+0)*3]		= objPointF[0][k*2];
						g_allObjInf[(curObjNum*5+0)*3+1]	= objPointF[0][k*2+1];
						objPointF[4][j*2]	= -1;
						objPointF[4][j*2+1]	= -1;
						objPointF[3][i*2]	= -1;
						objPointF[3][i*2+1]	= -1;
						objPointF[0][k*2]	= -1;
						objPointF[0][k*2+1]	= -1;
						curObjNum++;
						if(curObjNum>39)	return curObjNum;
						is_continue			= 0;
					}}
				}}}
			}}
		}}
	
	for(i=preObjNum;i<curObjNum;i++)
	{
		minDisErr = Vr;	minDPErr = Vr;
		a = g_allObjInf[(i*5+2)*3];				//2����
		b = g_allObjInf[(i*5+2)*3+1];
		if((fabs(a)>0.0000001) && (fabs(b)>0.0000001)){	//û��2
		//---------------------------------02-23-----------------------------------------
		e = g_allObjInf[(i*5+3)*3];				//3����
		f = g_allObjInf[(i*5+3)*3+1];
		for(j=0;j<g_objNum[0];j++)								//������0֡����ͨ������
		{
			c		= objPointF[0][j*2];						//0����
			d		= objPointF[0][j*2+1];
			if((fabs(c+1)>0.0000001) && (fabs(d+1)>0.0000001)){
			dis02	= sqrt((a-c)*(a-c) + (b-d)*(b-d));			//2->0����
			dis23	= sqrt((e-a)*(e-a) + (f-b)*(f-b));			//3->2����
			r02[0]	= a-c;										//2->0����
			r02[1]	= b-d;										//2->0����
			r23[0]	= e-a;										//3->2����
			r23[1]	= f-b;										//3->2����
			
			curDP = (r02[0]*r23[0]+r02[1]*r23[1])/(dis02*dis23);//cos��
			curDPErr = 1-curDP;									//1-cos��
			curDisErr = abs(dis02/2-dis23);						//����ƫ�����ֵ
			
			if((curDisErr<minDisErr)&&(curDisErr<=err)&&(curDPErr<minDPErr)&&(curDPErr<=0.1))	//����ͽǶȶ�����
			{
				minDPErr=curDPErr;
				minDisErr=curDisErr;
				g_allObjInf[(i*5+0)*3]		= objPointF[0][j*2];
				g_allObjInf[(i*5+0)*3+1]	= objPointF[0][j*2+1];
				objPointF[0][j*2]			= -1;
				objPointF[0][j*2+1]			= -1;
			}}
		}}
	}
	
	//��4��:һ��3��Ŀ��,0.2��-----------------------------------------------------------------------------------
	for(i=0;i<g_objNum[2];i++){									//������2֡����ͨ������
		for(j=0;j<g_objNum[0];j++)								//������0֡����ͨ������
		{
			//-------------------------------------02-------------------------------------------------
			a = objPointF[2][i*2];
			b = objPointF[2][i*2+1];
			c = objPointF[0][j*2];
			d = objPointF[0][j*2+1];
			if((fabs(a+1)>0.0000001) && (fabs(b+1)>0.0000001) && (fabs(c+1)>0.0000001) && (fabs(d+1)>0.0000001)){
			dis02 = sqrt((a-c)*(a-c) + (b-d)*(b-d));			//2->0����
			r02[0] = a-c;										//2->0����
			r02[1] = b-d;										//2->0����
			if(dis02/2<=Vr)
			{
				//-------------------------------------23-------------------------------------------------
				minDisErr = Vr;	minDPErr = Vr;	is_continue = 1;
				for(k=0;k<g_objNum[3];k++)
				{
					e = objPointF[3][k*2];
					f = objPointF[3][k*2+1];
					if((fabs(e+1)>0.0000001) && (fabs(f+1)>0.0000001)){
					dis23 = sqrt((e-a)*(e-a) + (f-b)*(f-b));	//3->2����
					r23[0] = e-a;								//3->2����
					r23[1] = f-b;								//3->2����

					curDP = (r23[0]*r02[0]+r23[1]*r02[1])/(dis23*dis02);	//cos��
					curDPErr = 1-curDP;										//1-cos��
					curDisErr = abs(dis23-dis02/2);							//����ƫ�����ֵ
				
					if((curDisErr<minDisErr)&&(curDisErr<=err)&&(curDPErr<minDPErr)&&(curDPErr<=0.1))	//����ͽǶȶ�����
					{
						minDisErr = curDisErr;	minDPErr = curDPErr;
						for(obj=curObjNum*5;obj<(curObjNum+1)*5;obj++){	//g_allObjInf((curObjNum-1)*5+1:curObjNum*5,3) = curObjNum;
							g_allObjInf[obj*3+2] = curObjNum+1;}
						g_allObjInf[(curObjNum*5+0)*3]		= objPointF[0][j*2];
						g_allObjInf[(curObjNum*5+0)*3+1]	= objPointF[0][j*2+1];
						g_allObjInf[(curObjNum*5+2)*3]		= objPointF[2][i*2];
						g_allObjInf[(curObjNum*5+2)*3+1]	= objPointF[2][i*2+1];
						g_allObjInf[(curObjNum*5+3)*3]		= objPointF[3][k*2];
						g_allObjInf[(curObjNum*5+3)*3+1]	= objPointF[3][k*2+1];
						objPointF[0][j*2]	= -1;
						objPointF[0][j*2+1]	= -1;
						objPointF[2][i*2]	= -1;
						objPointF[2][i*2+1]	= -1;
						objPointF[3][k*2]	= -1;
						objPointF[3][k*2+1]	= -1;
						curObjNum++;
						if(curObjNum>39)	return curObjNum;
						is_continue			= 0;
					}}
				}
				//-------------------------------------24-------------------------------------------------(û��3)
				if(is_continue != 0){	//������������
				minDisErr = Vr;	minDPErr = Vr;
				for(k=0;k<g_objNum[4];k++)
				{
					e = objPointF[4][k*2];
					f = objPointF[4][k*2+1];
					if((fabs(e+1)>0.0000001) && (fabs(f+1)>0.0000001)){
					dis24 = sqrt((e-a)*(e-a) + (f-b)*(f-b));	//4->2����
					r24[0] = e-a;								//4->2����
					r24[1] = f-b;								//4->2����

					curDP = (r24[0]*r02[0]+r24[1]*r02[1])/(dis24*dis02);	//cos��
					curDPErr = 1-curDP;										//1-cos��
					curDisErr = abs(dis24/2-dis02/2);							//����ƫ�����ֵ
				
					if((curDisErr<minDisErr)&&(curDisErr<=err)&&(curDPErr<minDPErr)&&(curDPErr<=0.1))	//����ͽǶȶ�����
					{
						minDisErr = curDisErr;	minDPErr = curDPErr;
						for(obj=curObjNum*5;obj<(curObjNum+1)*5;obj++){	//g_allObjInf((curObjNum-1)*5+1:curObjNum*5,3) = curObjNum;
							g_allObjInf[obj*3+2] = curObjNum+1;}
						g_allObjInf[(curObjNum*5+0)*3]		= objPointF[0][j*2];
						g_allObjInf[(curObjNum*5+0)*3+1]	= objPointF[0][j*2+1];
						g_allObjInf[(curObjNum*5+2)*3]		= objPointF[2][i*2];
						g_allObjInf[(curObjNum*5+2)*3+1]	= objPointF[2][i*2+1];
						g_allObjInf[(curObjNum*5+4)*3]		= objPointF[4][k*2];
						g_allObjInf[(curObjNum*5+4)*3+1]	= objPointF[4][k*2+1];
						objPointF[0][j*2]	= -1;
						objPointF[0][j*2+1]	= -1;
						objPointF[2][i*2]	= -1;
						objPointF[2][i*2+1]	= -1;
						objPointF[4][k*2]	= -1;
						objPointF[4][k*2+1]	= -1;
						curObjNum++;
						if(curObjNum>39)	return curObjNum;
						is_continue			= 0;
					}}
				}}
			}}
		}}
	return curObjNum;
}

/********************************����ƫ����******************************************/
void modify(unsigned short cnt)
{
	int i=0;
	if(cnt == 0)
		return;
	else
	{
		for(i=0;i<cnt;i++)
		{
			if(fabs(g_allObjInf[i*15+0]) > 0.0000001)
				g_allObjInf[i*15+0] += g_offsetR[0];
			if((g_allObjInf[i*15+0]<0) || (g_allObjInf[i*15+0]>1023))	g_allObjInf[i*15+0] = 0;
			if(fabs(g_allObjInf[i*15+1]) > 0.0000001)
				g_allObjInf[i*15+1] += g_offsetC[0];
			if((g_allObjInf[i*15+1]<0) || (g_allObjInf[i*15+1]>1023))	g_allObjInf[i*15+1] = 0;
			if(fabs(g_allObjInf[i*15+3]) > 0.0000001)
				g_allObjInf[i*15+3] += g_offsetR[1];
			if((g_allObjInf[i*15+3]<0) || (g_allObjInf[i*15+3]>1023))	g_allObjInf[i*15+3] = 0;
			if(fabs(g_allObjInf[i*15+4]) > 0.0000001)
				g_allObjInf[i*15+4] += g_offsetC[1];
			if((g_allObjInf[i*15+4]<0) || (g_allObjInf[i*15+4]>1023))	g_allObjInf[i*15+4] = 0;
			if(fabs(g_allObjInf[i*15+6]) > 0.0000001)
				g_allObjInf[i*15+6] += g_offsetR[2];
			if((g_allObjInf[i*15+6]<0) || (g_allObjInf[i*15+6]>1023))	g_allObjInf[i*15+6] = 0;
			if(fabs(g_allObjInf[i*15+7]) > 0.0000001)
				g_allObjInf[i*15+7] += g_offsetC[2];
			if((g_allObjInf[i*15+7]<0) || (g_allObjInf[i*15+7]>1023))	g_allObjInf[i*15+7] = 0;
			if(fabs(g_allObjInf[i*15+9]) > 0.0000001)
				g_allObjInf[i*15+9] += g_offsetR[3];
			if((g_allObjInf[i*15+9]<0) || (g_allObjInf[i*15+9]>1023))	g_allObjInf[i*15+9] = 0;
			if(fabs(g_allObjInf[i*15+10]) > 0.0000001)
				g_allObjInf[i*15+10] += g_offsetC[3];
			if((g_allObjInf[i*15+10]<0) || (g_allObjInf[i*15+10]>1023))	g_allObjInf[i*15+10] = 0;
			if(fabs(g_allObjInf[i*15+12]) > 0.0000001)
				g_allObjInf[i*15+12] += g_offsetR[4];
			if((g_allObjInf[i*15+12]<0) || (g_allObjInf[i*15+12]>1023))	g_allObjInf[i*15+12] = 0;
			if(fabs(g_allObjInf[i*15+13]) > 0.0000001)
				g_allObjInf[i*15+13] += g_offsetC[4];
			if((g_allObjInf[i*15+13]<0) || (g_allObjInf[i*15+13]>1023))	g_allObjInf[i*15+13] = 0;
		}
	}
}

extern void getCentroid(void);
extern void moveFilter(void);
extern void moveFill(void);

void move(void)	//2150ms
{
	unsigned short countRegion = 0;
	getMaxIndex();
	clear();
	countRegion = region();
	g_moveSum = movepath(countRegion);
	modify(g_moveSum);
	getCentroid();
	moveFilter();
	moveFill();
	return;
}

