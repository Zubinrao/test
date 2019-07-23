#ifndef __USART1_H
#define	__USART1_H

#include "stm32f10x.h"
#include <stdio.h>

#define USART2_Baud								115200				//调试通信波特率
#define USART2_Data_ADDRESS						0x40004400+0x04		//串口2数据寄存器地址
#define Receive_number							1024				//接收一帧数据的最大长度

typedef struct
{
	char	Rx_Buf[Receive_number];			//接收数据缓冲区
	u16 	Rx_Number;						//接收数据大小
}USART_Rx_TypeDef;

extern USART_Rx_TypeDef	usart2;

void 	USART_Config(void);
void 	USART1_Send(char *p,uint16_t size);
void 	SetDMA_MemoryBaseAddr(void);
void 	UART2_Send_Data(char *p,u16 Size);
void	USART3_Init(void);
void	USART3_Send(char *p,uint16_t size);


int fputc(int ch, FILE *f);
int fgetc(FILE *f);

#endif /* __USART1_H */
