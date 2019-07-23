#ifndef  BSP_PRESENT
#define  BSP_PRESENT

#include "stm32f10x.h"
#include "os_cfg_app.h"
#include  <cpu_core.h>
#include  <app_cfg.h>
#include "cpu.h"
#include "lib_mem.h"
#include "os.h"
#include "math.h"

#include "Systick.h"
#include "Control.h"
#include "NVIC.h"
#include "Exti.h"
#include "in_output.h"
#include "TIM.H"
#include "sdio_sdcard.h"
#include "PCM1770.h"
#include "iis.h"
#include "ioi2c.h"
#include "mpu6050.h"
#include "update.h"

#include "WavPlay.h"
#include "stdlib.h"
#include "usart.h"
#include "GPRS.h"
#include "iwdg.h"
#include "SPI.h"
#include "ADC.h"
#include "x9313w.h"
#include "Drawing.h"

void  BSP_Init(void);


#endif                                                          /* End of module include.                               */




