/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTI
  
  AL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "os.h"
#include "Control.h"
#include "usart.h"
#include "WavPlay.h"
#include "GPRS.h"
#include "iwdg.h"

extern OS_TCB   DMPTCB;
extern OS_TCB   LED_CtrTCB;
extern OS_TCB   Music_Play_CtrTCB;
extern OS_TCB   Serial_port_CtrTCB;
extern OS_TCB	ADCTCB;

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	OSIntEnter();
	OSTimeTick();
	feed_dog;
	OSIntExit();
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

void EXTI4_IRQHandler(void)
{
	OS_ERR   err;
	
	if((EXTI->PR&EXTI_Line4)==EXTI_Line4)
	{
		EXTI->PR = EXTI_Line4;
		
		/***发布信号量给任务***/
		OSTaskSemPost((OS_TCB	*)&DMPTCB,
					  (OS_OPT	 )OS_OPT_POST_NONE,
					  (OS_ERR	*)&err);
	}
}

/* IIS数据传输完成中断 */
void DMA1_Channel5_IRQHandler(void)
{
	OS_ERR err;
	
	if((DMA1->ISR&DMA1_FLAG_TC5)==DMA1_FLAG_TC5)
	{
		DMA1->IFCR |= DMA1_FLAG_TC5;
		OSFlagPost(&Wav_Play.Flag,DMA1_5_flag,OS_OPT_POST_FLAG_SET,&err);
	}
}

/* 串口2数据发送完成中断 */
void DMA1_Channel7_IRQHandler(void)
{
	OS_ERR err;
	
	if((DMA1->ISR&DMA1_FLAG_TC7)==DMA1_FLAG_TC7)
	{
		DMA1->IFCR |= DMA1_FLAG_TC7;
		OSFlagPost(&Ctl_Data.Flag,DMA1_7_flag,OS_OPT_POST_FLAG_SET,&err);
	}
}

/* 串口2中断 */
void USART2_IRQHandler(void)
{
	OS_ERR err;
	
	if((USART2->SR & USART_FLAG_IDLE) == USART_FLAG_IDLE)					//空闲中断
	{
		OSIntEnter();
		
		USART2->DR;															//读取一次，清除中断标志位
		usart2.Rx_Number = Receive_number - DMA1_Channel6->CNDTR;			//计算接收到的数据大小
		SetDMA_MemoryBaseAddr();											//重新设置内存指针以及数据大小
		
		/***发布信号量给任务***/
		OSTaskSemPost((OS_TCB	*)&Serial_port_CtrTCB,
					  (OS_OPT	 )OS_OPT_POST_NONE,
					  (OS_ERR	*)&err);
		
		OSIntExit();
	}
}

void DMA1_Channel3_IRQHandler(void)
{
	OS_ERR err;

	DMA1->IFCR = DMA1_FLAG_TC3;
	DMA1_Channel3->CCR &= ~DMA_CCR3_EN;
	OSTaskSemPost(&LED_CtrTCB,OS_OPT_POST_NO_SCHED,&err);
}
/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
