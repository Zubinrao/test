#ifndef __ADC_H
#define	__ADC_H

#include "stm32f10x.h"

#define Throttle_Min	0x450				//������С�ź�
#define Throttle_Max	0xC70				//��������ź�

#define OUT_MAX_Value	0xFF				//������ֵ

#define Sampling_N			10				//ÿ��ͨ���Ĳ�������
#define CH_N				4				//ADCͨ������

void ADC_Mcode_init(void);
void ADC_Data_Sorting(void);

#endif /* __ADC_H */
