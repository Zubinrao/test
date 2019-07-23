#ifndef __FLASH__H
#define __FLASH__H

#include "stm32f10x.h"

#define Page_size	0x800

#define Data_Add0	0x08070000
#define Data_Add1	Data_Add0+Page_size
 
void FLASH_WriteHalfWord(uint32_t addr , uint16_t *p , uint16_t HalfWord_Num);
void FLASH_ReadHalfWord(uint32_t addr , uint16_t *p , uint16_t HalfWord_Num);

#endif
