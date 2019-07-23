#include "ADC.h"
#include "Control.h"
#include "string.h"

__IO u16 AD_Value[CH_N*Sampling_N];
float	K_factor;

/*
*Function nameDC_Mcode_init()
*Notes：		初始化ADC接口
*Input:			NO
*Output:		NO
*/
void ADC_Mcode_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;					//ADC1 CH10/CH11/CH12/CH13
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&ADC1->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = CH_N*Sampling_N;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc =DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 4;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_1Cycles5);
	
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
	K_factor = (float)OUT_MAX_Value / (Throttle_Max - Throttle_Min);
}

void ADC_Data_Sorting(void)
{
	u8 	i;
	u16 buf[CH_N][Sampling_N];
	u32	sum[CH_N];
	
	for(i=0;i<Sampling_N;i++)
	{
		buf[0][i] = AD_Value[i*CH_N];
		buf[1][i] = AD_Value[i*CH_N+1];
		buf[2][i] = AD_Value[i*CH_N+2];
		buf[3][i] = AD_Value[i*CH_N+3];
	}
	
	sum[0] = 0;
	sum[1] = 0;
	sum[2] = 0;
	sum[3] = 0;
	for(i=0;i<Sampling_N;i++)
	{
		sum[0] += buf[0][i];
		sum[1] += buf[1][i];
		sum[2] += buf[2][i];
		sum[3] += buf[3][i];
	}
	
	sum[0] /= Sampling_N;
	sum[1] /= Sampling_N;
	sum[2] /= Sampling_N;
	sum[3] /= Sampling_N;
	
	for(i=0;i<CH_N;i++)
	{
		if(sum[i] < Throttle_Min)
		{
			sum[i] = 0;
		}
		else if(sum[i] > Throttle_Max)
		{
			sum[i] = OUT_MAX_Value;
		}
		else
		{
			sum[i] = (sum[i]-Throttle_Min) * K_factor;
		}
	}
	
	Ctl_Data.TH_RL = Ctl_Data.TH_RL*0.95 + sum[0]*0.05;
	Ctl_Data.TH_RR = Ctl_Data.TH_RR*0.95 + sum[1]*0.05;
	Ctl_Data.TH_LL = Ctl_Data.TH_LL*0.95 + sum[2]*0.05;
	Ctl_Data.TH_LR = Ctl_Data.TH_LR*0.95 + sum[3]*0.05;
}
