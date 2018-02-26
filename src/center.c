#include <stdio.h>
#include <math.h>
#include <yj6701.h>

/*************************����Ŀ�����ġ�ȡĿ������ͼ*********************************
**********************************����˵��*******************************************
	R��		������						��������
	C��		������						��������
	frm��	��ǰ֡��					��������
	idx_in��Ŀ������					��������
	cenR:	������						�����������
	cenC:	������						�����������
**********************************��ʱ����*******************************************
	row��col��							----ѭ��ʹ���С���
	max_Row��max_Col��					----����������ص��С���
	max_Val��							----����������صĻҶ�ֵ
	curIdx��							----��ǰĿ�������
	k��									----ָ��
	pointQueue��						----����ָ��
	queue��								----�������ʹ�õĶ���
	SUM_MSK��							----�������õ���15*15ͼ(�ѹ�һ��)
	SUM_RMB��							----���������ĵ�����15*15ͼ
	var��								----ɸѡ������������ֵ
	cur��								----��ǰֵ
	m��sumM��sumC��sumR:				----������ʹ��
************************************************************************************/
void getCenIM(short R, short C, unsigned short frm, unsigned char *idx_in, float *cenR, float *cenC)
{
	short row=0,col=0,max_Row=0,max_Col=0,curIdx=0,k=0,pointQueue=0;
	unsigned short queue[225][2],max_Val=0;
	float SUM_RMB[225],SUM_MSK[225],var = 0.1,cur=0,m=0,sumM=0,sumC=0,sumR=0;
	
	if ((R - g_offsetR[frm] < 0) || (C - g_offsetC[frm] < 0) || (R - g_offsetR[frm] >= IMG_WID) || (C - g_offsetC[frm] >= IMG_WID) || (R >= IMG_HEI) || (C >= IMG_WID) || (R < 0) || (C < 0))
	{
		return;
	}
	
	memset(SUM_RMB,0,900);
	memset(SUM_MSK,0,900);
	memset(queue,0,900);
	
	//ȡĿ����������ȵ�����λ��
	max_Row=R;	max_Col=C;	max_Val=0;
	curIdx = idx_in[(R-g_offsetR[frm])*IMG_WID+C-g_offsetC[frm]];
	if (curIdx == 0)
	{
		return;
	}
	for(row=R-2;row<=R+2;row++)
	{
		for(col=C-2;col<=C+2;col++)
		{
			if((row-g_offsetR[frm]>=0) && (col-g_offsetC[frm]>=0) && (row-g_offsetR[frm]<IMG_WID) && (col-g_offsetC[frm]<IMG_WID) && (row>=0) && (row<IMG_HEI) && (col>=0) && (col< IMG_WID))
			{
				if((idx_in[(row-g_offsetR[frm])*IMG_WID+col-g_offsetC[frm]] == curIdx) && (g_imgRmb[frm][row*IMG_WID+col]>max_Val))
				{
					max_Row = row;
					max_Col = col;
					max_Val = g_imgRmb[frm][row*IMG_WID+col];
				}
			}
		}
	}
	if (max_Val == 0)
	{
		return;
	}

	//ȡͼ����һ��
	k = 0;
	for(row=max_Row-7;row<=max_Row+7;row++)
	{
		for(col=max_Col-7;col<=max_Col+7;col++)
		{
			if ((row < 0) || (col < 0) || (row >= IMG_WID) || (col >= IMG_WID))
			{
				SUM_MSK[k++] = 0;
			}
			else
			{
				SUM_MSK[k++] = pow(((double)g_imgRmb[frm][row*IMG_WID + col]) / max_Val, 2.0);	//��һ����ƽ��(�����лҶ�ֵ��Ϊ0��1�м��ֵ)
			}
		}
	}
		
	//4�������
	pointQueue=0;
	queue[pointQueue][0]	= 7;
	queue[pointQueue++][1]	= 7;
	SUM_RMB[7*15+7] = SUM_MSK[7*15+7];		//ȡС��
	for(k=0;k<pointQueue;k++)				//����queue�еĵ�queueCount��
	{
		row = queue[k][0];
		col = queue[k][1];
		cur = SUM_MSK[row*15+col];
		SUM_MSK[row*15+col] = 0;

		if((SUM_MSK[(row-1)*15+col]<=cur) && (SUM_MSK[(row-1)*15+col]>var) && (row>0))
		{
			queue[pointQueue][0]	= row-1;
			queue[pointQueue++][1]	= col;
			SUM_RMB[(row-1)*15+col]	= SUM_MSK[(row-1)*15+col];									//ȡС��
		}
		if((SUM_MSK[row*15+col-1]<=cur) && (SUM_MSK[row*15+col-1]>var) && (col>0))
		{
			queue[pointQueue][0]	= row;
			queue[pointQueue++][1]	= col-1;
			SUM_RMB[row*15+col-1]	= SUM_MSK[row*15+col-1];									//ȡС��
		}
		if((SUM_MSK[(row+1)*15+col]<=cur) && (SUM_MSK[(row+1)*15+col]>var) && (row<14))
		{
			queue[pointQueue][0]	= row+1;
			queue[pointQueue++][1]	= col;
			SUM_RMB[(row+1)*15+col]	= SUM_MSK[(row+1)*15+col];									//ȡС��
		}
		if((SUM_MSK[row*15+col+1]<=cur) && (SUM_MSK[row*15+col+1]>var) && (col<14))
		{
			queue[pointQueue][0]	= row;
			queue[pointQueue++][1]	= col+1;
			SUM_RMB[row*15+col+1]	= SUM_MSK[row*15+col+1];									//ȡС��
		}
	}

	////������
	for(row=0;row<15;row++)
	{
		for(col=0;col<15;col++)
		{
			m = SUM_RMB[row*15+col];
			sumM += m;
			sumC += col*m;
			sumR += row*m;
		}
	}
	*cenR = max_Row+sumR/sumM-7;
	*cenC = max_Col+sumC/sumM-7;
}

/**********************************������*******************************************/
void getCentroid(void)
{
	unsigned short frm = 0, i = 0;

	for(i=0;i<g_moveSum;i++)
	{
		for(frm=0;frm<5;frm++)
		{
			getCenIM(g_allObjInf[15 * i + frm * 3], g_allObjInf[15 * i + frm * 3 + 1], frm, g_corImg, &g_allObjInf[15 * i + frm * 3], &g_allObjInf[15 * i + frm * 3 + 1]);
		}
	}
}

/***********************************
	0.	��	��	��	��	��----
	1.	��	��	��	��	��----��
	2.	��	��	��	��	��----��
	3.	��	��	��	��	��----��
	4.	��	��	��	��	��----��
	5.	��	��	��	��	��----��
	6.	��	��	��	��	��----��
	7.	��	��	��	��	��----��
	8.	��	��	��	��	��----��
	9.	��	��	��	��	��----
	10.	��	��	��	��	��----
	11.	��	��	��	��	��----
	12.	��	��	��	��	��----
	13.	��	��	��	��	��----
	14.	��	��	��	��	��----
	15.	��	��	��	��	��----
***********************************/
void moveFilter(void)
{
	float a[5][2] = {{0,0},{0,0},{0,0},{0,0},{0,0}};			//���10������ֵ
	float r[4][2]={{0,0},{0,0},{0,0},{0,0}},dis[4]={0,0,0,0};	//����
	float curDP = 0;											//�жϽǶ�
	unsigned short cnt = 0,i = 0,j = 0,error = 0,frmBuf[5] = {0,0,0,0,0};
	
	for(cnt=0;cnt<g_moveSum;cnt++)
	{
		memset(a[0],0,40);
		memset(r[0],0,32);
		memset(dis,0,16);
		memset(frmBuf,0,10);
		for(i=0,j=0;i<5;i++)
		{
			a[j][0] = g_allObjInf[cnt*15+i*3+0];
			a[j][1] = g_allObjInf[cnt*15+i*3+1];
			if((abs(a[j][0]) > 0.0000001) && (abs(a[j][1]) > 0.0000001))
			{
				a[j][0] -= g_offsetR[i];
				a[j][1] -= g_offsetC[i];
				frmBuf[j] = i;
				if(j>0)
				{
					r[j-1][0] = a[j][0] - a[j-1][0];
					r[j-1][1] = a[j][1] - a[j-1][1];
					dis[j-1] = sqrt(pow((double)(a[j][0] - a[j-1][0]),2.0) + pow((double)(a[j][1] - a[j-1][1]),2.0));
				}
				j++;
			}
		}
		
		if(j < 3)
		{
			memset(g_allObjInf+cnt*15,0,15*4);
			error++;
		}
		if(j == 3)
		{
			if((a[0][0] - (int)a[0][0] > 0) && (a[0][1] - (int)a[0][1] > 0) && (a[1][0] - (int)a[1][0] > 0) && (a[1][1] - (int)a[1][1] > 0) && (a[2][0] - (int)a[2][0] > 0) && (a[2][1] - (int)a[2][1] > 0))
			{
				for(i=1;i<j-1;i++)
				{
					curDP = 1 - (r[i][0]*r[i-1][0]+r[i][1]*r[i-1][1])/(dis[i]*dis[i-1]);
					if((curDP > 0.01) || (curDP < 0) || (dis[i]/(frmBuf[i+1] - frmBuf[i]) < 2) || (dis[i-1]/(frmBuf[i] - frmBuf[i-1]) < 2) || (abs(dis[i]/(frmBuf[i+1] - frmBuf[i]) - dis[i-1]/(frmBuf[i] - frmBuf[i-1])) > 0.8))
					{
						memset(g_allObjInf+cnt*15,0,15*4);
						error++;
						break;
					}
				}
			}
			else
			{
				memset(g_allObjInf+cnt*15,0,15*4);
				error++;
			}
		}
	}
	g_moveSum -= error;

	return;
}

void moveFill(void)
{
	int i=0,j=0,m=0;
	float row = 0,col = 0,a[2][3]={{0,0,0},{0,0,0}};

	for(i=0,j=0;i<g_moveSum;j++)
	{
		if(abs(g_allObjInf[15*j+2]) > 0.0000001)
		{
			if(i<j)
			{
				memcpy(g_allObjInf+15*i,g_allObjInf+15*j,60);
				memset(g_allObjInf+15*j,0,60);
			}
			i++;
		}
	}
	
	for(i=0;i<g_moveSum;i++)
	{
		m=0;
		for(j=0;(j<5)&&(m<2);j++)
		{
			if(abs(g_allObjInf[15*i+j*3]) > 0.0000001)
			{
				a[m][0] = g_allObjInf[15*i+j*3];	//��
				a[m][1] = g_allObjInf[15*i+j*3+1];	//��
				a[m][2] = j;						//֡��
				m++;
			}
		}
		row = (a[1][0] - a[0][0])/(a[1][2] - a[0][2]);
		col = (a[1][1] - a[0][1])/(a[1][2] - a[0][2]);
		for(j=0;j<5;j++)
		{
			if(abs(g_allObjInf[15*i+j*3]) < 0.0000001)
			{
				g_allObjInf[15*i+j*3] = a[0][0] + row*(j - a[0][2]);
				g_allObjInf[15*i+j*3+1] = a[0][1] + col*(j - a[0][2]);
				if((g_allObjInf[15*i+j*3]<0) || (g_allObjInf[15*i+j*3]>1023) || (g_allObjInf[15*i+j*3+1]<0) || (g_allObjInf[15*i+j*3+1]>1023))
				{
					g_allObjInf[15*i+j*3] = 0;
					g_allObjInf[15*i+j*3+1] = 0;
				}
			}
		}
	}

	return;
}
