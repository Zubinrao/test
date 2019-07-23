#ifndef __DRAWING__H
#define __DRAWING__H

#include "stm32f10x.h"

#define LED_CH					18
#define LEDNumber				120

#define LED_X					48
#define LED_Y					8

typedef struct
{
	u16 	size;								//指示显示层字节总数
	u8  	RGB[LED_CH][LEDNumber][3];			//显示层
	u8		Draw_layer[LED_CH][LEDNumber][3];	//绘图层
}LEDControl_TypeDef;

void Update_Layer(void);
void Clear_screen(void);
void Draw_Point(int8_t	x,int8_t y,u32 colour,u8 flag);
u32  Detection_Pixel(u8 x,u8 y);
void Draw_LineX(u8 X0,u8 X1,u8 Y,u32 Color );
void Draw_LineY(u8 X,u8 Y0,u8 Y1,u32 Color );
void Draw_Line(u8 StartX,u8 StartY,u8 EndX, u8 EndY,u32 Color);
void DispStrCH(int8_t x,char *str,u32 Color,u8 flag);
void Draw_Point_b(int8_t	x,int8_t y,u32 colour,u8 flag, float brightness);
//void breath_light(void);
extern LEDControl_TypeDef LED;

#endif
