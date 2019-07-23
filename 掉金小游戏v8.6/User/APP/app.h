#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#include "stm32f10x.h"
#include "bsp.h"
#include "gameapi.h"

#define  	START_PRIO           	3
#define  	Control_PRIO           	10
#define  	DMP_PRIO             	4
#define 	LEDCtr_PRIO				6
#define 	Trigger_PRIO			5
#define 	Music_Play_PRIO			3
#define 	Serial_port_PRIO		4
#define  	KEY_PRIO				5
#define  	ADC_PRIO				6

#define  	START_STK_SIZE       	128
#define  	Control_STK_SIZE        512  //128
#define  	DMP_STK_SIZE        	256
#define  	LEDCtr_STK_SIZE      	512		//128
#define  	Trigger_STK_SIZE      	512
#define  	Music_Play_STK_SIZE    	512
#define  	Serial_port_STK_SIZE   	128
#define  	KEY_STK_SIZE			128
#define  	ADC_STK_SIZE			128

OS_TCB    	STARTTCB;
OS_TCB    	ControlTCB;
OS_TCB    	DMPTCB;
OS_TCB    	LED_CtrTCB;
OS_TCB    	Trigger_CtrTCB;
OS_TCB    	Music_Play_CtrTCB;
OS_TCB    	Serial_port_CtrTCB;
OS_TCB		KEYTCB;
OS_TCB		ADCTCB;

CPU_STK		StartStk[START_STK_SIZE];
CPU_STK		ControlStk[Control_STK_SIZE];
CPU_STK   	DMPStk[DMP_STK_SIZE];
CPU_STK   	LED_CtrStk[LEDCtr_STK_SIZE];
CPU_STK   	Trigger_CtrStk[Trigger_STK_SIZE];
CPU_STK   	Music_Play_CtrStk[Music_Play_STK_SIZE];
CPU_STK   	Serial_port_CtrStk[Serial_port_STK_SIZE];
CPU_STK		KEYStk[KEY_STK_SIZE];
CPU_STK		ADCStk[ADC_STK_SIZE];

static void AppTaskStart(void *p_arg );
static void AppTaskControl(void *p_arg);
static void AppTaskDMP(void *p_arg);
static void AppTaskLEDCtr(void *p_arg);
static void AppTaskTrigger(void *p_arg);
static void AppTaskMusicPlay(void *p_arg);
static void AppTaskSerial_port(void *p_arg);
static void AppTaskKey(void *p_arg);
static void AppTaskADC(void *p_arg);
extern u8 piano_time_count;

static void TMR_1sCallback(OS_TMR *p_tmr, void *p_arg);


#endif
