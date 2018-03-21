#include "../include/yj6701.h"

/******************************************************************
******************************************************************/
void save(unsigned short frameCnt, unsigned short *starCnt, short moveCnt)
{
	unsigned short	frm = 0, k = 0, row = 0, col = 0;
	short			i = 0, j = 0;
	unsigned short	img[225];			//��ͼ
	unsigned char	emifBuf[256];		//EMIF���ݻ���
	memset(emifBuf, 0, 256);

	//���͸������ݰ�
	/*------------------------------------------------------------------------*/
	emifBuf[0] = 0xEB;	emifBuf[1] = 0x90;													//֡ͷ
	emifBuf[2] = frameCnt / 10 / 256;	emifBuf[3] = frameCnt / 10 % 256;					//�����
	emifBuf[4] = g_thresholdMW;																//ȥ������
	emifBuf[5] = g_thresholdBW / 256;	emifBuf[6] = g_thresholdBW % 256;					//��ֵ����ֵ
	emifBuf[7] = g_thresholdSTAR / 256; emifBuf[8] = g_thresholdSTAR % 256;					//������ȡ��Ŀ
	emifBuf[11] = moveCnt%256;												//GEOĿ����
	emifBuf[12] = 0;																		//С֡���
	emifBuf[13] = g_thresholdBCK / 256;	emifBuf[14] = g_thresholdBCK % 256;					//����Ԥ����
	emifBuf[15] = g_frameNumber / 256;	emifBuf[16] = g_frameNumber % 256;					//����֡��ʼ
	for (j = 0; j<5; j++)
	{
		memcpy(emifBuf + 17 + j * 7, g_captureTime[j], 7);
		//emifBuf[17 + j * 7 + 0] = *((unsigned char *)g_captureTime[j] + 2);
		//emifBuf[17 + j * 7 + 1] = *((unsigned char *)g_captureTime[j] + 3);
		//emifBuf[17 + j * 7 + 2] = *((unsigned char *)g_captureTime[j] + 0);
		//emifBuf[17 + j * 7 + 3] = *((unsigned char *)g_captureTime[j] + 1);
		//emifBuf[17 + j * 7 + 4] = *((unsigned char *)g_captureTime[j] + 7);
		//emifBuf[17 + j * 7 + 5] = *((unsigned char *)g_captureTime[j] + 4);
		//emifBuf[17 + j * 7 + 6] = *((unsigned char *)g_captureTime[j] + 5);
	}
	for (j = 0; j<5; j++)
	{
		memcpy(emifBuf + 52 + j * 4, &g_controlWord[j], 4);
	}
	emifBuf[72] = (unsigned char)starCnt[0];								//������
	emifBuf[73] = (unsigned char)starCnt[1];								//������
	emifBuf[74] = (unsigned char)starCnt[2];								//������
	emifBuf[75] = (unsigned char)starCnt[3];								//������
	emifBuf[76] = (unsigned char)starCnt[4];								//������
	emifBuf[253] = 0;
	for (j = 0; j<253; j++) {
		emifBuf[253] += emifBuf[j];
	}												//У���
	emifBuf[254] = 0x09;	emifBuf[255] = 0xD7;	//֡β
	fwrite(emifBuf, 1, 256, fWriteHex);
	fflush(fWriteHex);

	//���ͺ���
	for (frm = 0; frm<5; frm++)			//ѭ��5֡
	{
		for (i = 0; i<starCnt[frm]; i++)		//ѭ�����Ǹ���
		{
			k = 0;
			memset(img, 0, 450);
			row = (unsigned short)g_starPosi[frm][i * 3];
			col = (unsigned short)g_starPosi[frm][i * 3 + 1];
			for (j = row - 7; j <= row + 7; j++)	//ͼ������
			{
				if ((j >= 0) && (j <= 1023)) {
					if (col < 7)
						memcpy(img + k + (7 - col), (unsigned char *)(g_imgOrig[frm] + j*IMG_WID), (8 + col) * 2);
					else if (col > 1016)
						memcpy(img + k, (unsigned char *)(g_imgOrig[frm] + j*IMG_WID + col - 7), (1023 - col + 8) * 2);
					else
						memcpy(img + k, (unsigned char *)(g_imgOrig[frm] + j*IMG_WID + col - 7), 30);
					k += 15;
				}
			}
			/*------------------------------------------------------------------------*/
			emifBuf[9] = frm + 1;												//֡���
			emifBuf[10] = starCnt[frm] / 256;	emifBuf[11] = starCnt[frm] % 256;		//Ŀ������
			emifBuf[12] = 1;													//С֡���
			emifBuf[13] = i / 256;	emifBuf[14] = i % 256;						//Ŀ�����
			emifBuf[15] = row / 256;											//��--����
			emifBuf[16] = row % 256;
			emifBuf[17] = col / 256;											//��--����
			emifBuf[18] = col % 256;
			/*------------------------------------------------------------------------*/
			emifBuf[19] = *(((unsigned char*)g_starPosi[frm]) + i * 12 + 0);	//��--����
			emifBuf[20] = *(((unsigned char*)g_starPosi[frm]) + i * 12 + 1);
			emifBuf[21] = *(((unsigned char*)g_starPosi[frm]) + i * 12 + 2);
			emifBuf[22] = *(((unsigned char*)g_starPosi[frm]) + i * 12 + 3);
			emifBuf[23] = *(((unsigned char*)g_starPosi[frm]) + i * 12 + 4);	//��--����
			emifBuf[24] = *(((unsigned char*)g_starPosi[frm]) + i * 12 + 5);
			emifBuf[25] = *(((unsigned char*)g_starPosi[frm]) + i * 12 + 6);
			emifBuf[26] = *(((unsigned char*)g_starPosi[frm]) + i * 12 + 7);
			/*------------------------------------------------------------------------*/
			emifBuf[27] = 0;													//Ŀ������(0����,1�˶�)
			memcpy(emifBuf + 28, img, 225);
			emifBuf[253] = 0;
			for (j = 0; j<253; j++) {
				emifBuf[253] += emifBuf[j];
			}																	//У���
			fwrite(emifBuf, 1, 256, fWriteHex);
			fflush(fWriteHex);
			/*------------------------------------------------------------------------*/
			emifBuf[12] = 2;													//С֡���
			memcpy(emifBuf + 28, (unsigned char *)img + 225, 225);
			/*------------------------------------------------------------------------*/
			emifBuf[253] = 0;
			for (j = 0; j<253; j++) {
				emifBuf[253] += emifBuf[j];
			}							//У���
			fwrite(emifBuf, 1, 256, fWriteHex);
			fflush(fWriteHex);
		}
	}
	for (frm = 0; frm<5; frm++)
	{
		for (i = 0; i<moveCnt; i++)		//ѭ���˶�Ŀ�����
		{
			k = 0;
			memset(img, 0, 450);
			row = (unsigned short)g_allObjInf[15 * i + 3 * frm];
			col = (unsigned short)g_allObjInf[15 * i + 3 * frm + 1];
			for (j = row - 7; j <= row + 7; j++)	//ͼ������
			{
				if ((j >= 0) && (j <= 1023)) {
					if (col < 7)
						memcpy(img + k + col, (unsigned char *)(g_imgOrig[frm] + j*IMG_WID), 30 - col * 2);
					else if (col > 1016)
						memcpy(img + k, (unsigned char *)(g_imgOrig[frm] + j*IMG_WID + col - 7), (1023 - col + 8) * 2);
					else
						memcpy(img + k, (unsigned char *)(g_imgOrig[frm] + j*IMG_WID + col - 7), 30);
					k += 15;
				}
			}

			/*------------------------------------------------------------------------*/
			emifBuf[9] = frm + 1;														//֡���
			emifBuf[10] = moveCnt / 256;	emifBuf[11] = moveCnt % 256;				//Ŀ������
			emifBuf[12] = 1;															//С֡���
			emifBuf[13] = i / 256;	emifBuf[14] = i % 256;								//Ŀ�����
			emifBuf[15] = row / 256;													//��--����
			emifBuf[16] = row % 256;
			emifBuf[17] = col / 256;													//��--����
			emifBuf[18] = col % 256;
			/*------------------------------------------------------------------------*/
			emifBuf[19] = *(((unsigned char*)g_allObjInf) + 60 * i + 12 * frm);			//��--����
			emifBuf[20] = *(((unsigned char*)g_allObjInf) + 60 * i + 12 * frm + 1);
			emifBuf[21] = *(((unsigned char*)g_allObjInf) + 60 * i + 12 * frm + 2);
			emifBuf[22] = *(((unsigned char*)g_allObjInf) + 60 * i + 12 * frm + 3);
			emifBuf[23] = *(((unsigned char*)g_allObjInf) + 60 * i + 12 * frm + 4);		//��--����
			emifBuf[24] = *(((unsigned char*)g_allObjInf) + 60 * i + 12 * frm + 5);
			emifBuf[25] = *(((unsigned char*)g_allObjInf) + 60 * i + 12 * frm + 6);
			emifBuf[26] = *(((unsigned char*)g_allObjInf) + 60 * i + 12 * frm + 7);
			/*------------------------------------------------------------------------*/
			emifBuf[27] = 1;															//Ŀ������(0����,1�˶�)
			memcpy(emifBuf + 28, img, 225);
			/*------------------------------------------------------------------------*/
			emifBuf[253] = 0;
			for (j = 0; j<253; j++) {
				emifBuf[253] += emifBuf[j];
			}							//У���
			fwrite(emifBuf, 1, 256, fWriteHex);
			fflush(fWriteHex);
			/*------------------------------------------------------------------------*/
			emifBuf[12] = 2;															//С֡���
			memcpy(emifBuf + 28, (unsigned char *)img + 225, 225);
			emifBuf[253] = 0;
			for (j = 0; j<253; j++) {
				emifBuf[253] += emifBuf[j];
			}							//У���
			fwrite(emifBuf, 1, 256, fWriteHex);
			fflush(fWriteHex);
			/*------------------------------------------------------------------------*/
		}
	}
}