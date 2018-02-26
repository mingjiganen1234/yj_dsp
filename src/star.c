#include <stdio.h>
#include <math.h>
#include <C6X.h>
#include <yj6701.h>

/******************************��õ�0֡ͼ����****************************************/
/*******************************����˵��**********************************************
img		����0֡ͼ��			--����
starList�������б�			--���
**************************************************************************************/
unsigned short mid_star(unsigned short *img, float *starList)
{
	unsigned short value = 0;					//�Ҷ�ֵ
	unsigned short point = 0;					//ָ��
	float		   trans[3];					//���ڽ�������
	unsigned short i = 0, j = 0, k =0;
	memset(starList, 0, 160 * 3 * 4);

	if (g_thresholdSTAR < 1) { g_thresholdSTAR = 1; }
	if (g_thresholdSTAR > 160) { g_thresholdSTAR = 160; }

	for (i = 2; i < IMG_HEI - 2; i++)						//ɨ����
	{
		for (j = 2; j < IMG_WID - 2; j++)					//ɨ����
		{
			value = img[i*IMG_WID + j];
			if (value > g_thresholdBW)
			{
				//���ھ���1�İ�����ֵ���Ҵ��ھ���2��������ֵ
				if ((value > img[(i - 1)*IMG_WID + (j - 1)]) && (value > img[(i - 1)*IMG_WID + j]) && (value > img[(i - 1)*IMG_WID + j + 1]) && (value > img[i * IMG_WID + j - 1]) && (value > img[i * IMG_WID + j + 1]) && (value > img[(i + 1)*IMG_WID + j - 1]) && (value > img[(i + 1)*IMG_WID + j]) && (value > img[(i + 1)*IMG_WID + j + 1]))
				{
					if ((value > img[(i - 2)*IMG_WID + (j - 2)]) && (value > img[(i - 2)*IMG_WID + j]) && (value > img[(i - 2)*IMG_WID + j + 2]) && (value > img[i * IMG_WID + j - 2]) && (value > img[i * IMG_WID + j + 2]) && (value > img[(i + 2)*IMG_WID + j - 2]) && (value > img[(i + 2)*IMG_WID + j]) && (value > img[(i + 2)*IMG_WID + j + 2]))
					{
						//�����û����
						if (point == 0)
						{
							starList[point * 3] = i;
							starList[point * 3 + 1] = j;
							starList[point * 3 + 2] = value;
							point++;
						}
						//����Ǳ�û��
						else if (point < g_thresholdSTAR)
						{
							starList[point * 3] = i;
							starList[point * 3 + 1] = j;
							starList[point * 3 + 2] = value;
							for (k = point; k > 0; k--)
							{
								if (value > starList[(k - 1) * 3 + 2])
								{
									memcpy(trans, starList + (k - 1) * 3, 12);
									memcpy(starList + (k - 1) * 3, starList + k * 3, 12);
									memcpy(starList + k * 3, trans, 12);
								}
								else
								{
									k = 1;
								}
							}
							point++;
						}
						//����Ǳ�����
						else
						{
							if (value > starList[(point - 1) * 3 + 2])
							{
								starList[(point - 1) * 3] = i;
								starList[(point - 1) * 3 + 1] = j;
								starList[(point - 1) * 3 + 2] = value;
							}
							for (k = point - 1; k > 0; k--)
							{
								if (value > starList[(k - 1) * 3 + 2])
								{
									memcpy(trans, starList + (k - 1) * 3, 12);
									memcpy(starList + (k - 1) * 3, starList + k * 3, 12);
									memcpy(starList + k * 3, trans, 12);
								}
								else
								{
									k = 1;
								}
							}
						}
					}
				}
			}
			else
			{
				k = 0;
			}
		}
	}

	return point;
}

/******************************���1.2.3.4֡ͼ����************************************/
/*******************************����˵��**********************************************
img		: ͼ��				--����
starList�������б�			--�������
starNum ����������			--�������
row		����ƫ����			--����
col		����ƫ����			--����
**************************************************************************************/
void star_posi(unsigned short **img, float starList[5][480], unsigned short *starNum, short *row, short *col)
{
	int i = 0, j = 0, r = 0, c = 0, k = 0, pt = 0, m = 0;
	float		   trans[3];					//���ڽ�������

	//����׼ƫ������ӵ���4֡ͼ��
	for (j = 1; j < 5; j++)
	{
		memset(starList[j], 0, 160 * 3 * 4);
		starNum[j] = 0;
		for (i = 0; i < starNum[0]; i++)
		{
			//��֤�������ӳ�
			if ((starList[0][i * 3] + row[j] > 0) && (starList[0][i * 3] + row[j] < IMG_HEI - 1) && (starList[0][i * 3 + 1] + col[j] > 0) && (starList[0][i * 3 + 1] + col[j] < IMG_WID - 1))
			{
				r = starList[0][i * 3] + row[j];
				c = starList[0][i * 3 + 1] + col[j];
				m = 0;
				//����5*5ȡ���ֵ
				for (k = r - 2; k <= r + 2; k++)
				{
					for (pt = c - 2; pt <= c + 2; pt++)
					{
						if ((k >= 0) && (k < IMG_HEI) && (pt >= 0) && (pt < IMG_WID) && (img[j][k*IMG_WID + pt] > m))
						{
							m = img[j][k*IMG_WID + pt];
							starList[j][starNum[j] * 3	  ] = k;
							starList[j][starNum[j] * 3 + 1] = pt;
							starList[j][starNum[j] * 3 + 2] = m;
						}
					}
				}
				//����
				for (k = starNum[j]; k > 0; k--)
				{
					if (m > starList[j][(k - 1) * 3 + 2])
					{
						memcpy(trans, starList[j] + (k - 1) * 3, 12);
						memcpy(starList[j] + (k - 1) * 3, starList[j] + k * 3, 12);
						memcpy(starList[j] + k * 3, trans, 12);
					}
				}
				starNum[j]++;
			}
		}
	}
}

/******************************������****************************************/
/*********************************����˵��*********************************************
f		��֡��				--����
i		���к�				--����
j		���к�				--����
img		: ͼ��				--����
starList�������б�			--�������
starNum ����������			--�������
**************************************************************************************/
void star_detect(short f, short i, short j, unsigned short **img, float starList[5][480], unsigned short *starNum)
{
	unsigned short value = 0;		//�Ҷ�ֵ
	float trans[3];					//���ڽ�������
	short k = 0;
	value = img[f][i*IMG_WID + j];
	if (value > g_thresholdBW)
	{
		if ((value > img[f][(i - 1)*IMG_WID + (j - 1)]) && (value > img[f][(i - 1)*IMG_WID + j]) && (value > img[f][(i - 1)*IMG_WID + j + 1]) && (value > img[f][i * IMG_WID + j - 1]) && (value > img[f][i * IMG_WID + j + 1]) && (value > img[f][(i + 1)*IMG_WID + j - 1]) && (value > img[f][(i + 1)*IMG_WID + j]) && (value > img[f][(i + 1)*IMG_WID + j + 1]))
		{
			if ((value > img[f][(i - 2)*IMG_WID + (j - 2)]) && (value > img[f][(i - 2)*IMG_WID + j]) && (value > img[f][(i - 2)*IMG_WID + j + 2]) && (value > img[f][i * IMG_WID + j - 2]) && (value > img[f][i * IMG_WID + j + 2]) && (value > img[f][(i + 2)*IMG_WID + j - 2]) && (value > img[f][(i + 2)*IMG_WID + j]) && (value > img[f][(i + 2)*IMG_WID + j + 2]))
			{
				if (starNum[f] == 0)
				{
					starList[f][starNum[f] * 3] = i;
					starList[f][starNum[f] * 3 + 1] = j;
					starList[f][starNum[f] * 3 + 2] = value;
					starNum[f]++;
				}
				else if (starNum[f] < g_thresholdSTAR)
				{
					starList[f][starNum[f] * 3] = i;
					starList[f][starNum[f] * 3 + 1] = j;
					starList[f][starNum[f] * 3 + 2] = value;
					for (k = starNum[f]; k > 0; k--)
					{
						if (value > starList[f][(k - 1) * 3 + 2])
						{
							memcpy(trans, starList[f] + (k - 1) * 3, 12);
							memcpy(starList[f] + (k - 1) * 3, starList[f] + k * 3, 12);
							memcpy(starList[f] + k * 3, trans, 12);
						}
					}
					starNum[f]++;
				}
				else
				{
					if (value > starList[f][(starNum[f] - 1) * 3 + 2])
					{
						starList[f][(starNum[f] - 1) * 3] = i;
						starList[f][(starNum[f] - 1) * 3 + 1] = j;
						starList[f][(starNum[f] - 1) * 3 + 2] = value;
					}
					for (k = starNum[f] - 1; k > 0; k--)
					{
						if (value > starList[f][(k - 1) * 3 + 2])
						{
							memcpy(trans, starList[f] + (k - 1) * 3, 12);
							memcpy(starList[f] + (k - 1) * 3, starList[f] + k * 3, 12);
							memcpy(starList[f] + k * 3, trans, 12);
						}
						else
						{
							k = 1;
						}
					}
				}
			}
		}
	}
	else
	{
		k = 0;
	}
}

/****************************����1.2.3.4֡ͼʣ�ಿ��**********************************/
/*******************************����˵��**********************************************
img		: ͼ��				--����
starList�������б�			--�������
starNum ����������			--�������
row		����ƫ����			--����
col		����ƫ����			--����
**************************************************************************************/
void cut_star(unsigned short **img, float starList[5][480], unsigned short *starNum, short *row, short *col)
{
	short f = 0, i = 0, j = 0;

	if (g_thresholdSTAR < 1) { g_thresholdSTAR = 1; }
	if (g_thresholdSTAR > 160) { g_thresholdSTAR = 160; }

	//��1��4֡����
	for (f = 1; f < 5; f++)
	{
		//�л�����ƫ�Ʋ�ִ��
		if ((row[f] != 0) || (col[f] != 0))
		{
			if ((row[f] > 2) && (row[f] < IMG_HEI - 2))				//��������ƫ��
			{
				for (i = 2; i < row[f]; i++)						//ɨ����
				{
					for (j = 2; j < IMG_WID - 2; j++)				//ɨ����
					{
						star_detect(f, i, j, img, starList, starNum);
					}
				}
			}

			if ((row[f] < -2) && (row[f] > 2 - IMG_HEI))			//���з���ƫ��
			{
				for (i = IMG_HEI + row[f]; i < IMG_HEI - 2; i++)	//ɨ����
				{
					for (j = 2; j < IMG_WID - 2; j++)				//ɨ����
					{
						star_detect(f, i, j, img, starList, starNum);
					}
				}
			}
			if ((col[f] > 2) && (col[f] < IMG_WID - 2))				//��������ƫ��
			{
				for (i = 2; i < IMG_HEI - 2; i++)					//ɨ����
				{
					for (j = 2; j < col[f]; j++)					//ɨ����
					{
						star_detect(f, i, j, img, starList, starNum);
					}
				}
			}

			if ((col[f] < -2) && (col[f] > 2 - IMG_WID))
			{
				for (i = 2; i < IMG_HEI - 2; i++)					//ɨ����
				{
					for (j = IMG_WID + col[f]; j < IMG_WID - 2; j++)//ɨ����
					{
						star_detect(f, i, j, img, starList, starNum);
					}
				}
			}
		}
	}
}

/***********************************������********************************************/
/*********************************����˵��*********************************************
img		: ͼ��				--����
starList�������б�			--�������
starNum ����������			--�������
**************************************************************************************/
void star_centre(unsigned short **img, float starList[5][480], unsigned short *starNum)
{
	short tmpRow = 0,tmpCol = 0;
	short frm = 0, i = 0, k = 0, row = 0, col = 0, pointQueue = 0;
	float SUM_MSK[225], SUM_RMB[225], var = 0.01, cur = 0, m = 0, tmpValue = 0;
	double sumM = 0, sumC = 0, sumR = 0;
	unsigned short queue[225][2];

	for (frm = 0; frm < 5; frm++)
	{
		for (i = 0; i < starNum[frm]; i++)
		{
			memset(SUM_RMB, 0, 900);
			memset(SUM_MSK, 0, 900);
			memset(queue, 0, 900);
			//ȡͼ����һ��
			k = 0;
			tmpRow = starList[frm][i * 3];
			tmpCol = starList[frm][i * 3 + 1];
			for (row = tmpRow - 7; row <= tmpRow + 7; row++)
			{
				for (col = tmpCol - 7; col <= tmpCol + 7; col++)
				{
					if ((row < 0) || (col < 0) || (row >= IMG_HEI) || (col >= IMG_WID))
					{
						SUM_MSK[k++] = 0;
					}
					else
					{
						tmpValue = starList[frm][i * 3 + 2];
						if(tmpValue > 0.0000001)
						{
							SUM_MSK[k++] = pow(((double)img[frm][row*IMG_WID + col]) / tmpValue, 2.0);	//��һ����ƽ��(�����лҶ�ֵ��Ϊ0��1�м��ֵ)
						}
						else
						{
							SUM_MSK[k++] = 0;
						}
					}
				}
			}

			//4�������
			pointQueue = 0;
			queue[pointQueue][0] = 7;
			queue[pointQueue++][1] = 7;
			SUM_RMB[7 * 15 + 7] = SUM_MSK[7 * 15 + 7];		//ȡС��
			for (k = 0; k<pointQueue; k++)					//����queue�еĵ�queueCount��
			{
				row = queue[k][0];
				col = queue[k][1];
				cur = SUM_MSK[row * 15 + col];
				SUM_MSK[row * 15 + col] = 0;

				if ((SUM_MSK[(row - 1) * 15 + col] <= cur) && (SUM_MSK[(row - 1) * 15 + col]>var) && (row>0))
				{
					queue[pointQueue][0] = row - 1;
					queue[pointQueue++][1] = col;
					SUM_RMB[(row - 1) * 15 + col] = SUM_MSK[(row - 1) * 15 + col];									//ȡС��
				}
				if ((SUM_MSK[row * 15 + col - 1] <= cur) && (SUM_MSK[row * 15 + col - 1]>var) && (col>0))
				{
					queue[pointQueue][0] = row;
					queue[pointQueue++][1] = col - 1;
					SUM_RMB[row * 15 + col - 1] = SUM_MSK[row * 15 + col - 1];									//ȡС��
				}
				if ((SUM_MSK[(row + 1) * 15 + col] <= cur) && (SUM_MSK[(row + 1) * 15 + col]>var) && (row<14))
				{
					queue[pointQueue][0] = row + 1;
					queue[pointQueue++][1] = col;
					SUM_RMB[(row + 1) * 15 + col] = SUM_MSK[(row + 1) * 15 + col];									//ȡС��
				}
				if ((SUM_MSK[row * 15 + col + 1] <= cur) && (SUM_MSK[row * 15 + col + 1]>var) && (col<14))
				{
					queue[pointQueue][0] = row;
					queue[pointQueue++][1] = col + 1;
					SUM_RMB[row * 15 + col + 1] = SUM_MSK[row * 15 + col + 1];									//ȡС��
				}
			}

			////������
			sumM = 0; sumC = 0; sumR = 0;
			for (row = 0; row<15; row++)
			{
				for (col = 0; col<15; col++)
				{
					m = SUM_RMB[row * 15 + col];
					sumM += m;
					sumC += col*m;
					sumR += row*m;
				}
			}
			if((sumM > 0.0000001) || (sumM < -0.0000001))
			{
				starList[frm][i * 3 + 0] = starList[frm][i * 3 + 0] + sumR / sumM - 7;
				starList[frm][i * 3 + 1] = starList[frm][i * 3 + 1] + sumC / sumM - 7;
			}
		}
	}
}

void star(float starList[5][480], unsigned short *starNum)	//1770ms
{
	starNum[0] = mid_star(g_imgRmb[0], starList[0]);
	star_posi(g_imgRmb, starList, starNum, g_offsetR, g_offsetC);
	cut_star(g_imgRmb, starList, starNum, g_offsetR, g_offsetC);
	star_centre(g_imgRmb, starList, starNum);
}