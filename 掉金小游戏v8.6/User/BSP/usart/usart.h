#ifndef __USART1_H
#define	__USART1_H

#include "stm32f10x.h"
#include <stdio.h>

#define USART2_Baud								115200				//����ͨ�Ų�����
#define USART2_Data_ADDRESS						0x40004400+0x04		//����2���ݼĴ�����ַ
#define Receive_number							1024				//����һ֡���ݵ���󳤶�

typedef struct
{
	char	Rx_Buf[Receive_number];			//�������ݻ�����
	u16 	Rx_Number;						//�������ݴ�С
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
