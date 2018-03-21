#include <stdio.h>
#include <math.h>
#include "../include/yj6701.h"

//��ʼ����׼Ҫ�õĻ�����
//stars[5][10]���ṹ�壬�洢����׼��
//tris[5][10]���ṹ�壬�洢����������
//triCnt[5]�������εĸ���
void initPara(struct star **stars,struct tri **tris,unsigned char *triCnt)
{
	int i=0,j=0;
	//��ʼ���ǵ�����
	for (i = 0; i < 5; i++)
	{
		for(j=0;j<REG_STAR_CNT;j++)
		{
			stars[i][j].r = 0;
			stars[i][j].c = 0;
			stars[i][j].b = 0;
			stars[i][j].label = -1;
		}
	}
	//��ʼ������������
	for (i = 0; i < 5; i++)
	{
		for(j=0;j<REG_TRI_CNT;j++)
		{
			tris[i][j].s[0] = 0;
			tris[i][j].s[1] = 0;
			tris[i][j].s[2] = 0;
			tris[i][j].labels[0] = 0;
			tris[i][j].labels[1] = 0;
			tris[i][j].labels[2] = 0;
		}
		triCnt[i] = 0;
	}
}

//��ֵ��ͼ��
//img 	ͼ��
//map	��ֵ��ͼ��
//th 	��ֵ Threshold
void Threshold(unsigned short *img,unsigned char *map,unsigned short th)
{
	int i=0,j=0;
	unsigned short bufImg[IMG_WID];
	unsigned char bufMap[IMG_WID];
	memset(map,0,IMG_FRM);

	for(i=124;i<IMG_HEI-124;i++)
	{
		memcpy(bufImg,img+i*IMG_WID,IMG_WID*2);
		memset(bufMap,0,IMG_WID);
		for(j=124;j<IMG_WID-124;j++)
		{
			if(bufImg[j]>th)
			{
				bufMap[j] = 1;
			}
		}
		memcpy(map+i*IMG_WID,bufMap,IMG_WID);
	}
}

//��ͨ����
//����map����ֵ��ͼ
//����img��ԭͼ
//����stars���Ǳ�
unsigned char regRegion(unsigned char *map, unsigned short *img,struct star *stars)
{
	unsigned short i=0, j=0, a=0, b=0;						//����ѭ��
	unsigned short queue[4096],queueCount=0,pointQueue=0;	//queue��ָ��
	unsigned int r=0,c=0,bright=0;							//���꼰����

	for (i = 100; i<IMG_HEI - 100; i++){						//ɨ����
		for (j = 100; j<IMG_WID - 100; j++){					//ɨ����
			if(map[i*IMG_WID + j]==1)
			{
				map[i*IMG_WID + j] = 0;						//�޸���ͨ���
				pointQueue = 0;								//queue��ָ��
				memset(queue, 0, 8192);						//queue��ʼ��Ϊ0
				queue[pointQueue++] = j;					//��1�����ص���
				queue[pointQueue++] = i;					//��1�����ص���

				for (queueCount = 0; queueCount<pointQueue / 2; queueCount++){							//����queue�еĵ�queueCount��
					for (a = queue[queueCount * 2 + 1] - 1; a <= queue[queueCount * 2 + 1] + 1; a++){	//������
						for (b = queue[queueCount * 2] - 1; b <= queue[queueCount * 2] + 1; b++){		//������
							if (map[a*IMG_WID + b] == 1)			//�ж��Ƿ�Ϊ1
							{
								map[a*IMG_WID + b] = 0;				//���
								queue[pointQueue++] = b;			//��һ�����ص�x
								queue[pointQueue++] = a;			//��һ�����ص�y
								if (pointQueue == 4096)				//������˵�����쳣
								{
									return 0;
								}
							}
						}
					}
				}
				if (pointQueue>32)
				{
					//Ҫ�����16�����ص��������Ч
					r = 0;	c = 0;	bright = 0;
					//����ƽ�����Ȳ�����
					for (queueCount = 0; queueCount < pointQueue / 2; queueCount++){
						a = queue[queueCount * 2 + 1];
						b = queue[queueCount * 2];
						bright += img[a*IMG_WID + b];
						r += a*img[a*IMG_WID + b];
						c += b*img[a*IMG_WID + b];
					}
					r = r / bright;
					c = c / bright;
					bright = bright / queueCount;
					for (a=0; a<REG_STAR_CNT; a++){
						if(stars[a].b<bright){
							memcpy(stars+a+1,stars+a,(REG_STAR_CNT-1-a)*8);
							break;
						}
					}
					if(a<REG_STAR_CNT){
						stars[a].r		= r;
						stars[a].c		= c;
						stars[a].b		= bright;
						stars[a].label	= 1;
					}
				}
			}
		}
	}

	for (a=0; a<REG_STAR_CNT; a++)
	{
		if(stars[a].label!=-1)
		{
			stars[a].label = a;
		}
		else
		{
			return (unsigned char)a;
		}
	}
	return REG_STAR_CNT;
}

//��ȡ����Ϣ Detect star
//img 	ͼ��2MB��
//map	��ֵ��ͼ��1MB��
//stars	ָ������Ϣ��ָ��	Pointer to stars info
unsigned char getStar(unsigned short *img,unsigned short th,unsigned char *map,struct star *stars)
{
	Threshold(img, map, th);
	return regRegion(map,img,stars);
}

//���������� getTri
//����stars����
//����starCnt���Ǹ���
//����tris��������
void getTri(struct star * stars,unsigned char starCnt,struct tri *tris,unsigned char *triCnt)
{
	unsigned char i=0, j=0, k=0, pt=0;
	if(starCnt<3){
		return;
	}
	for(i=0;i<starCnt-2;i++){
		for (j = i+1; j<starCnt; j++){
			for (k = j + 1; k < starCnt; k++)
			{
				tris[pt].s[0] = abs(stars[i].r - stars[j].r) + abs(stars[i].c - stars[j].c);
				tris[pt].s[1] = abs(stars[j].r - stars[k].r) + abs(stars[j].c - stars[k].c);
				tris[pt].s[2] = abs(stars[k].r - stars[i].r) + abs(stars[k].c - stars[i].c);
				tris[pt].labels[0] = stars[i].label;
				tris[pt].labels[1] = stars[j].label;
				tris[pt++].labels[2] = stars[k].label;
			}
		}
	}
	*triCnt = pt;
}

//��׼ Registration
//����ref����׼ͼ��֡��
//����est������׼ͼ��֡��
//����stars����
//����triCnt����������
//����tris��������
//����r����ƫ����
//����c����ƫ����
void triReg(int ref,int est,struct star **stars,unsigned char *triCnt,struct tri **tris,short *r,short *c)
{
	unsigned char i=0, j=0,map[REG_STAR_CNT*REG_STAR_CNT];
	short max=0,cnt=0;
	int x=0,y=0;
	memset(map, 0, REG_STAR_CNT * REG_STAR_CNT);
	for(i=0;i<triCnt[ref];i++){
		for (j = 0; j < triCnt[est]; j++){
			if (abs(tris[ref][i].s[0] - tris[est][j].s[0]) + abs(tris[ref][i].s[1] - tris[est][j].s[1]) + abs(tris[ref][i].s[2] - tris[est][j].s[2])<9){
				map[tris[ref][i].labels[0]*REG_STAR_CNT+tris[est][j].labels[0]]++;
				map[tris[ref][i].labels[1]*REG_STAR_CNT+tris[est][j].labels[1]]++;
				map[tris[ref][i].labels[2]*REG_STAR_CNT+tris[est][j].labels[2]]++;
			}
		}
	}
	max = 0;
	for (i = 0; i<REG_STAR_CNT; i++){
		for (j = 0; j < REG_STAR_CNT; j++){
			if (map[i*REG_STAR_CNT+j] > max){
				max = map[i*REG_STAR_CNT+j];
			}
		}
	}
	if(max==0){
		return;
	}
	for (i = 0; i<REG_STAR_CNT; i++){
		for (j = 0; j < REG_STAR_CNT; j++){
			if (map[i*REG_STAR_CNT+j] == max){
				cnt++;
				x -= stars[ref][i].r - stars[est][j].r;
				y -= stars[ref][i].c - stars[est][j].c;
			}
		}
	}
	*r = x/cnt;
	*c = y/cnt;
	return;
}

//ͼ����׼�ĺ������
//����curID����ǰ֡�ţ�0.1.2.3.4��
//����img��ʹ�õ�ͼ��
//����winSize����׼���ڴ�С
//����offsetR����ƫ����
//����offsetC����ƫ����
void matchMain(unsigned int curID,unsigned short **img)
{
	unsigned char starCnt = 0;
	if(curID==0)
	{
		initPara(g_stars,g_tris,g_triCnt);
	}
	starCnt = getStar(img[curID],g_thresholdBW,g_regLabelMap,g_stars[curID]);
	getTri(g_stars[curID],starCnt,g_tris[curID],g_triCnt+curID);

	if(curID>0)
	{
		triReg(curID-1, curID, g_stars, g_triCnt,g_tris, g_offsetR+curID,g_offsetC+curID);
		g_offsetR[curID] += g_offsetR[curID-1];
		g_offsetC[curID] += g_offsetC[curID-1];
	}
}
