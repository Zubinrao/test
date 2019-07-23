#include "in_output.h"

void delay(void)
{
	u32 i=0xfffff;
	
	while(i--);
}

void IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|\
						   RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|\
						   RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG,ENABLE);
	
	/*板载LED*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIOE->BRR=GPIO_Pin_0;
	GPIOE->BRR=GPIO_Pin_5;
	GPIOE->BRR=GPIO_Pin_6;
	
	/*TF卡电源控制*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	TF_Power(OFF);
	delay();
	TF_Power(ON);
	
	/*转把开关*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* 拨码开关 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	/*SIM900A复位控制引脚*/
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	SIM900A_RST_H;
}

/**
 *@brief	：读取拨码开关状态
 *@param	：none
 *@retval	：拨码开关输入值
 */
uint8_t Read_SW(void)
{
	uint8_t buf=0;
	
	buf |= SW_1;
	buf |= SW_2 << 1;
	buf |= SW_3 << 2;
	buf |= SW_4 << 3;
	
	return buf;
}
