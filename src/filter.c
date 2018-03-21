#include "../include/yj6701.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

extern void writeImg(void *p,char *str,unsigned int len);

//�þ����ͼ���˲�1�Σ�inͼ��Ϊ������С��outͼ��Ϊԭͼ��С,wid��heiΪ�����ĳߴ�
void mirrorFileter(unsigned short *inImg,unsigned short *outImg,unsigned short window,unsigned short wid,unsigned short hei)
{
	unsigned short i=0,j=0,k=0,n=0;
	unsigned int sum=0,rowSum=0;

	unsigned short *IM = (unsigned short*)malloc(window*window*sizeof(unsigned short));		//���ֵ����

	for(i=window/2;i<=hei-window/2;i++)		//��
		for(j=window/2;j<=wid-window/2;j++)	//��
		{
			//��ͼ32*32
			n=0;
			for(k=i-16;k<i+16;k++)
				memcpy(IM+n++*window,inImg+k*wid+j-window/2,window*2);
			//���ֵ
			sum = 0;
			for(n=0;n<window;n++)
			{
				rowSum = 0;
				for(k=0;k<window;k++)
					rowSum += IM[n*window+k];
				sum += rowSum>>5;
			}
			sum = sum>>5;
			//��ֵ
			//if(inImg[i*wid+j]<sum+g_thresholdBCK)
			//	outImg[(i-window/2)*(wid-window+1)+(j-window/2)] = inImg[i*wid+j];
			//else
			//	outImg[(i-window/2)*(wid-window+1)+(j-window/2)] = sum + g_thresholdBCK;
			outImg[(i-window/2)*(wid-window+1)+(j-window/2)] = sum;
		}
}

//��ͼ����о�����outͼ��Ϊ������С��inͼ��Ϊԭͼ��С,wid��heiΪԭͼ�ĳߴ�
void mirrorImg(unsigned short *inImg,unsigned short *outImg,unsigned short window,unsigned short wid,unsigned short hei)
{
	unsigned short outWid = wid+window-1;
	unsigned short outHei = hei+window-1;
	unsigned short i=0,j=0;
	for(i=0;i<hei;i++)
		memcpy(outImg+(i+window/2)*outWid+window/2,inImg+i*wid,wid*2);
	for(i=window/2;i<window/2+hei;i++)
	{
		for(j=0;j<window/2;j++)
			outImg[i*outWid+j] = outImg[i*outWid+window-1-j];
		for(j=window/2+wid;j<outWid;j++)
			outImg[i*outWid+j] = outImg[i*outWid+(wid+outWid-j)];
	}
	for(i=0;i<window/2;i++)
		memcpy(outImg+i*outWid,outImg+(window-1-i)*outWid,outWid*2);
	for(i=hei+window/2;i<outHei;i++)
		memcpy(outImg+i*outWid,outImg+(hei+outHei-i)*outWid,outWid*2);
}

void filter(unsigned short **inImg,unsigned short **outImg,unsigned short cnt,unsigned short window,unsigned short wid,unsigned short hei)
{
	if(cnt==0)	return;
	unsigned int a,f,i,j;
	unsigned short *inImgL = (unsigned short*)malloc(wid*hei/2*sizeof(unsigned short));							//����ͼ
	unsigned short *inImgR = (unsigned short*)malloc(wid*hei/2*sizeof(unsigned short));							//�Ұ��ͼ
	unsigned short *imgBck = (unsigned short*)malloc(wid*hei/2*sizeof(unsigned short));							//����ͼ
	memset(imgBck,0,wid*hei);
	unsigned short *imgMir = (unsigned short*)malloc((wid/2+window-1)*(hei+window-1)*sizeof(unsigned short));	//����ͼ
	memset(imgMir,0,(wid/2+window-1)*(hei+window-1)*2);

	//֡��
	for(f=0;f<5;f++)
	{
		printf("��ʼ�˲���%d֡\n",f);
		for(i=0;i<hei;i++)
		{
			memcpy(inImgL+i*wid/2,inImg[f]+i*wid,wid);
			memcpy(inImgR+i*wid/2,inImg[f]+i*wid+wid/2,wid);
		}
		/*-------------��---------------*/
		memcpy(imgBck,inImgL,wid*hei);
		//�˲�����
		for(a=0;a<cnt;a++)
		{
			mirrorImg(imgBck,imgMir,window,wid/2,hei);	//����
			/*FILE *f = fopen("1.raw","wb+");
			fwrite(imgMir,1,(wid/2+window-1)*(hei+window-1)*2,f);
			fclose(f);*/
			mirrorFileter(imgMir,imgBck,window,wid/2+window-1,hei+window-1);	//�˲�
		}
		for(i=0;i<hei;i++)
		{
			memcpy(outImg[f]+i*wid,imgBck+i*wid/2,wid);
		}
		/*-------------��---------------*/
		memcpy(imgBck,inImgR,wid*hei);
		//�˲�����
		for(a=0;a<cnt;a++)
		{
			mirrorImg(imgBck,imgMir,window,wid/2,hei);	//����
			mirrorFileter(imgMir,imgBck,window,wid/2+window-1,hei+window-1);	//�˲�
		}
		for(i=0;i<hei;i++)
		{
			memcpy(outImg[f]+i*wid+wid/2,imgBck+i*wid/2,wid);
		}
		char s[260];
		// sprintf(s,"data\\filter_background_%d.raw",f);
		// writeImg(outImg[f],s,IMG_FRM*2);

		for(a=0;a<wid*hei;a++)
		{
			if(inImg[f][a]>(outImg[f][a]))//+g_thresholdBCK))
				outImg[f][a] = inImg[f][a] - outImg[f][a];// - g_thresholdBCK;
			else
				outImg[f][a] = 0;
				//outImg[f][a] = outImg[f][a] + g_thresholdBCK - inImg[f][a];
		}
		sprintf(s,"data\\before_filter_%d.raw",f);
		writeImg(inImg[f],s,IMG_FRM*2);
		sprintf(s,"data\\after_filter_%d.raw",f);
		writeImg(outImg[f],s,IMG_FRM*2);
	}
	return;
}