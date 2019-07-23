#ifndef __IOI2C_H
#define __IOI2C_H
#include "stm32f10x.h"

//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))  

//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) 	//0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) 	//0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) 	//0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) 	//0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) 	//0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) 	//0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) 	//0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) 	//0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) 	//0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) 	//0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) 	//0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) 	//0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) 	//0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) 	//0x40011E08 

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入 

//IO方向设置
#define SDA_IN  	GPIOG->CRL &= 0X0FFFFFFF; GPIOG->CRL |= (u32)8<<28;
#define SDA_OUT 	GPIOG->CRL &= 0X0FFFFFFF; GPIOG->CRL |= (u32)3<<28;

//IO操作函数	 
#define IIC_SCL    PGout(6) 	//SCL
#define IIC_SDA    PGout(7) 	//SDA
#define READ_SDA   PGin(7)		//输入SDA

void IIC_Init(void);                //初始化IIC的IO口				 
int IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
int IIC_Wait_Ack(void); 			//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

u8 IICreadByte(u8 dev, u8 reg);
u8 IICreadBytes(u8 dev, u8 reg, u8 length, u8 *data);
u8 IICwriteByte(u8 dev, u8 reg, u8 data);
u8 IICwriteBytes(u8 dev, u8 reg, u8 length, u8* data);

u8 IICwriteBit(u8 dev,u8 reg,u8 bitNum,u8 data);
u8 IICwriteBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data);

#endif

//------------------End of File----------------------------
