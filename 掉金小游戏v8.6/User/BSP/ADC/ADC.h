#ifndef __ADC_H
#define	__ADC_H

#include "stm32f10x.h"

#define Throttle_Min	0x450				//油门最小信号
#define Throttle_Max	0xC70				//油门最大信号

#define OUT_MAX_Value	0xFF				//输出最大值

#define Sampling_N			10				//每个通道的采样数量
#define CH_N				4				//ADC通道数量

void ADC_Mcode_init(void);
void ADC_Data_Sorting(void);

#endif /* __ADC_H */
