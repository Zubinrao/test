#ifndef __PCM1770__H
#define __PCM1770__H

#include "stm32f10x.h"

#define PCM_PD_L 				GPIOB->BRR = GPIO_Pin_14
#define PCM_PD_H				GPIOB->BSRR = GPIO_Pin_14

#define PCM_MS_L         		GPIOD->BRR = GPIO_Pin_14
#define PCM_MS_H         		GPIOD->BSRR = GPIO_Pin_14

#define PCM_MC_L        		GPIOD->BRR = GPIO_Pin_13
#define PCM_MC_H		 		GPIOD->BSRR = GPIO_Pin_13

#define PCM_MD_L        		GPIOD->BRR = GPIO_Pin_12
#define PCM_MD_H		 		GPIOD->BSRR = GPIO_Pin_12

#define C_VOLUME_MAX			0x3F					//◊Ó¥Û“Ù¡ø

void PCM1770_Init(void);
void PCM_WriteData(const u8 Reg, const u8 Data);
void PCM1770_VolumeSet(vu8 vol);

#endif
