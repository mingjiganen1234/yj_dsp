#include "../include/yj6701.h"

void init()
{
    int value = 0;
    printf("���롰��ֵ����������70\n");
    g_thresholdBW = 70;
    printf("���롰ȥ��������������0\n");
    // scanf("%d",&value);
    g_thresholdMW = value;
    printf("���롰����Ԥ�����������0\n");
    // scanf("%d",&value);
    g_thresholdBCK = value;
    for(int i=0;i<5;i++)
    {
        g_imgOrig[i] = (unsigned short *)malloc(IMG_WID*IMG_HEI*sizeof(unsigned short));	//ԭʼͼ��洢ָ��
        memset(g_imgOrig[i],0,IMG_WID*IMG_HEI*sizeof(unsigned short));
        g_imgRmb[i] = (unsigned short *)malloc(IMG_WID*IMG_HEI*sizeof(unsigned short));	    //ȥ��������ͼ��洢ָ��
        memset(g_imgRmb[i],0,IMG_WID*IMG_HEI*sizeof(unsigned short));
        g_tris[i]	=(struct tri*)malloc(120*sizeof(struct tri));                           //��׼��������
        memset(g_tris[i],0,120*sizeof(struct tri));
        g_stars[i]	=(struct star*)malloc(10*sizeof(struct star));                         //��׼����
        memset(g_stars[i],0,10*sizeof(struct star));
        objPointF[i]    = (float *)malloc(2048*sizeof(float));				                //��0,1,2,3,4֡ͼ��ͨ���������Ϣ
        memset(objPointF[i],0,2048*sizeof(float));
    }
    g_regLabelMap = (unsigned char *)malloc(IMG_WID*IMG_HEI*sizeof(unsigned char));     //��׼��ͨ��ͼ(1024*1024B)
    memset(g_regLabelMap,0,IMG_WID*IMG_HEI*sizeof(unsigned char));
    g_maxIndex = (unsigned char *)malloc(IMG_WID*IMG_HEI*sizeof(unsigned char));	    //���ֵ����֡
    memset(g_maxIndex,0,IMG_WID*IMG_HEI*sizeof(unsigned char));
    g_corImg = (unsigned char *)malloc(IMG_WID*IMG_HEI*sizeof(unsigned char));	        //ȥ���
    memset(g_corImg,0,IMG_WID*IMG_HEI*sizeof(unsigned char));
    g_objPntLst		= (float *)malloc(4096*sizeof(float));				                //�洢֡�š���ͨ�򡢼���������
    g_allObjInf		= (float *)malloc(2048*sizeof(float));				                //�˶�Ŀ��ϸ��(5��3�еı���)
    g_moveSum		= 0;                    							                //�˶�Ŀ������
    fWriteHex = fopen("data\\result.hex", "wb+");
}

/******************��ͼ��*********************
����str���ļ�·����������.raw��β��
����frmBeg��������ļ���ʼ֡��
����framePara��ͼ��֡������2B��
����time���ع���ʼʱ�䣨7B*5��
����control_word�������֣�4B��
**********************************************/
int readFile(char *str,int frmBeg,unsigned int time[5][2], unsigned int *framePara,unsigned int *control_word)
{
    FILE *fp[5];
    char strName[260];
    int i = 0,j=0;

    //���ļ�
    for(i=0;i<5;i++)
    {
        sprintf(strName,"%s%d.raw",str,frmBeg+i);
        printf("��--%s\n",strName);
        fp[i] = fopen(strName,"rb");
        if(fp[i] == NULL)
            return 0;
    }

    //�ж��Ƿ�Ϊ1072*1028
    fread(g_imgOrig[0],1,8,fp[0]);
    fseek(fp[0],0,0);
    if((g_imgOrig[0][0] == 0x5449 && g_imgOrig[0][1] == 0x1FCE)||(g_imgOrig[0][0] == 0x4954 && g_imgOrig[0][1] == 0xCE1F))
    {
        printf("Դͼ��ߴ磺1072_1028pix\n");
        //��ȡ�ļ�
        for(i=0;i<5;i++)
        {
            for(j=1;j<1025;j++)
            {
                fseek(fp[i],j*1072*2+48,0);
                fread(g_imgOrig[i]+1024*(j-1),1,1024*2,fp[i]);
            }
            fseek(fp[i], 8, 0);								//��λ����8���ֽ�
            fread(time[i], 1, 7, fp[i]);					//��ȡ7���ֽ�ʱ����
            if (i == 0)
            {
                fseek(fp[i], 22, 0);						//��λ����22���ֽ�
                fread((char *)framePara + 1, 1, 1, fp[i]);	//��ȡ2���ֽ�֡����
                fread((char *)framePara, 1, 1, fp[i]);		//��ȡ2���ֽ�֡����
            }
            fseek(fp[i], 24, 0);							//��λ����24���ֽ�
            fread(&control_word[i], 1, 4, fp[i]);			//��ȡ4���ֽڿ��Ʋ���
        }
    }
    else
    {
        printf("1024_1024pix\n");
        //��ȡ�ļ�
        for(i=0;i<5;i++)
            fread(g_imgOrig[i],1,IMG_FRM*2,fp[i]);
    }
    //�ر��ļ�
    for(i=0;i<5;i++)
        fclose(fp[i]);
    return 1;
}

//д�˲����ͼ
void write12345()
{
	FILE *fp;
	fp = fopen("..\\data\\�˲����1.raw","wb+");
	fwrite(g_imgRmb[0],1,IMG_FRM*2,fp);
	fclose(fp);
	fp = fopen("..\\data\\�˲����2.raw","wb+");
	fwrite(g_imgRmb[1],1,IMG_FRM*2,fp);
	fclose(fp);
	fp = fopen("..\\data\\�˲����3.raw","wb+");
	fwrite(g_imgRmb[2],1,IMG_FRM*2,fp);
	fclose(fp);
	fp = fopen("..\\data\\�˲����4.raw","wb+");
	fwrite(g_imgRmb[3],1,IMG_FRM*2,fp);
	fclose(fp);
	fp = fopen("..\\data\\�˲����5.raw","wb+");
	fwrite(g_imgRmb[4],1,IMG_FRM*2,fp);
	fclose(fp);
	return;
}

//дͼ
void writeImg(void *p,char *str,unsigned int len)
{
	FILE *fp;
	fp = fopen(str,"wb+");
	fwrite(p,1,len,fp);
	fclose(fp);
	return;
}

//дĿ��λ��
void writePosi(int frm)
{
	FILE *fp;
	char str[260];
	int i=0,j=0,k=0;

    if(frm==1)
    {
        fp = fopen("data\\starPosi.txt","wb+");
        sprintf(str,"֡��   Ŀ���        ��             ��\n");
        fprintf(fp,str);
    }
    else
        fp = fopen("data\\starPosi.txt","a");
    for(i=0;i<5;i++)
    {
        for(j=0;j<g_starSum[i];j++)
        {
            sprintf(str,"%03d     %03d      %f     %f	%f\n",frm+i,j, g_starPosi[i][3*j], g_starPosi[i][3 * j + 1], g_starPosi[i][3 * j + 2]);	//֡��||Ŀ���||��||��||����
            fprintf(fp,str);
        }
    }
    fclose(fp);
    fp=NULL;
    if(frm==1)
    {
        fp = fopen("data\\movePosi.txt","wb+");
        sprintf(str,"֡��   Ŀ���        ��             ��\n");
        fprintf(fp,str);
    }
    else
        fp = fopen("data\\movePosi.txt","a");
    for(i=0,k=0;i<g_moveSum;i++,k++)
    {
        if(g_allObjInf[15*k+2] == 0)
        {
            i--;
            continue;
        }
        for(j=0;j<5;j++)
        {
            sprintf(str,"%03d     %03d      %.4f     %.4f\n",frm+j,i,g_allObjInf[15*k+3*j],g_allObjInf[15*k+3*j+1]);	//֡��||Ŀ���||��||��
            fprintf(fp,str);
        }
    }
    fclose(fp);
}