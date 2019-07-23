#ifndef _IIS_H
#define _IIS_H

#include "stm32f10x.h"

static void I2S_GPIO_Config(void);
static void I2S_Mode_Config(void);
static void DMA_I2S_Init(void);
void I2S_Bus_Init(void);

void I2S_Stop(void);
void I2S_Freq_Config(uint16_t SampleFreq);
void DMA_I2S_Send_Data(u32 addr, uint32_t size);

#endif
