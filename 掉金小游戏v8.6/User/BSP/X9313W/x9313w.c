#include "x9313w.h"

u8	position;					//¼ÇÂ¼X9313W³éÍ·Î»ÖÃ

void X9313W_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	
	/*°åÔØLED*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	X9313W_Reset();
	X9313W_Set(5);
}

void X9313W_Reset(void)
{
	u8 i;
	u16 j;
	
	UD_L;
	INC_H;
	CS_H;
	j=2000; while(j--);
	CS_L;
	j=1; while(j--);
	for(i=0;i<x9313w_Grade;i++)
	{
		INC_L;
		j=5; while(j--);
		INC_H;
		j=5; while(j--);
	}
	
	INC_H;
	j=10; while(j--);
	CS_H;
	
	position = 0;
}

void X9313W_Set(u8 p)
{
	u8 i,j;
	
	if(p > (x9313w_Grade-1))
		return;
	
	INC_H;
	CS_L;
	if(p > position)
	{
		UD_H;
		j=1; while(j--);
		for(i=0;i<(p-position);i++)
		{
			INC_L;
			j=5; while(j--);
			INC_H;
			j=5; while(j--);
		}
	}
	else if(p < position)
	{
		UD_L;
		j=1; while(j--);
		for(i=0;i<(position-p);i++)
		{
			INC_L;
			j=5; while(j--);
			INC_H;
			j=5; while(j--);
		}
	}
	
	INC_H;
	j=10; while(j--);
	CS_H;
	
	position = p;
}
