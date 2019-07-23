#include "Drawing.h"
#include "stdlib.h"
#include "string.h"
#include "ASCLL.h"
#include "os.h"
#include "debug.h"
extern OS_TCB   LED_CtrTCB;
//static u32 color=0x000000;
//static u8 order=0;

LEDControl_TypeDef LED;
//灯带点阵示意图
//控|(0,0)*******************************************
//  |************************************************
//  |************************************************
//制|************************************************
//  |************************************************
//  |************************************************
//器|*******************************************(x,y)

/**
 *@brief	更新显示层(将绘图层内容复制到显示层)
 *@param	None
 *@retval	None
 */
void Update_Layer(void)
{
	OS_ERR err;
	
	memcpy(LED.RGB,LED.Draw_layer,LED_CH*LEDNumber*3);
	// char str[50];
	// sprintf(str, "Color : %x", (int)(LED.RGB[14][0][0]));
	// Debug_SendMsg("Drawing", str, DEBUG_MSG_INFO);
	OSTaskSemPost(&LED_CtrTCB,OS_OPT_POST_NO_SCHED,&err);
}

/**
 *@brief	清楚屏幕显示内容
 *@param	None
 *@retval	None
 */
void Clear_screen(void)
{
	memset(LED.Draw_layer,0,sizeof(LED.Draw_layer));
}

/**
 *@brief	检测对应点像素值
 *@param	x,y - 位置
 *@retval	24位颜色值(RGB)
 */
u32 Detection_Pixel(u8 x,u8 y)
{
	u32 buf=0;
	
	if((x < LED_X) && (y < LED_Y))
	{
		buf = LED.Draw_layer[y][x][0];
		buf = (buf<<8) | LED.Draw_layer[y][x][1];
		buf = (buf<<8) | LED.Draw_layer[y][x][2];
	}
	else
		buf = 0;
	
	return buf;
}

/**
 *@brief	指定位置画点
 *@param	x,y - 位置
 *			colour - 24位颜色值(8R8G8B)
 *			flag = 0 - 覆盖原来的颜色
 *			     = 1 - 保留原来的颜色(0x000000除外)
 *               = 2 - 透明显示
 *@retval	None
 */
void Draw_Point(int8_t	x,int8_t y,u32 colour,u8 flag)
{
	if((x >= LED_X) || (x < 0)) return;
	if((y >= LED_CH) || (y < 0)) return;
	
	if(flag == 0)
	{
		LED.Draw_layer[y][x][0] = (colour & 0x00ff0000) >> 16;
		LED.Draw_layer[y][x][1] = (colour & 0x0000ff00) >> 8;
		LED.Draw_layer[y][x][2] = (colour & 0x000000ff);
	}
	else if(flag == 1)
	{
		if((LED.RGB[y][x][0] == 0x00)&&(LED.RGB[y][x][1] == 0x00)&&(LED.RGB[y][x][2] == 0x00))
		{
			LED.Draw_layer[y][x][0] = (colour & 0x00ff0000) >> 16;
			LED.Draw_layer[y][x][1] = (colour & 0x0000ff00) >> 8;
			LED.Draw_layer[y][x][2] = (colour & 0x000000ff);
		}
	}
	else if(flag == 2)
	{
		if(colour != 0x000000)
		{
			LED.Draw_layer[y][x][0] = (colour & 0x00ff0000) >> 16;
			LED.Draw_layer[y][x][1] = (colour & 0x0000ff00) >> 8;
			LED.Draw_layer[y][x][2] = (colour & 0x000000ff);
		}
	}
}

/**
 *@brief	指定位置画点
 *@param	x,y - 位置
 *			colour - 24位颜色值(8R8G8B)
 *			flag = 0 - 覆盖原来的颜色
 *			     = 1 - 保留原来的颜色(0x000000除外)
 *               = 2 - 透明显示
 *@retval	None
 */
void Draw_Point_b(int8_t	x,int8_t y,u32 colour,u8 flag, float brightness)
{
	if((x >= LED_X) || (x < 0)) return;
	if((y >= LED_CH) || (y < 0)) return;
	
	if(flag == 0)
	{
		LED.Draw_layer[y][x][0] = (u8)(((colour & 0x00ff0000) >> 16) * brightness);
		LED.Draw_layer[y][x][1] = (u8)(((colour & 0x0000ff00) >> 8) * brightness);
		LED.Draw_layer[y][x][2] = (u8)(((colour & 0x000000ff)) * brightness);
	}
	else if(flag == 1)
	{
		if((LED.RGB[y][x][0] == 0x00)&&(LED.RGB[y][x][1] == 0x00)&&(LED.RGB[y][x][2] == 0x00))
		{
			LED.Draw_layer[y][x][0] = (u8)(((colour & 0x00ff0000) >> 16) * brightness);
			LED.Draw_layer[y][x][1] = (u8)(((colour & 0x0000ff00) >> 8) * brightness);
			LED.Draw_layer[y][x][2] = (u8)(((colour & 0x000000ff)) * brightness);
		}
	}
	else if(flag == 2)
	{
		if(colour != 0x000000)
		{
			LED.Draw_layer[y][x][0] = (u8)(((colour & 0x00ff0000) >> 16) * brightness);
			LED.Draw_layer[y][x][1] = (u8)(((colour & 0x0000ff00) >> 8) * brightness);
			LED.Draw_layer[y][x][2] = (u8)(((colour & 0x000000ff)) * brightness);
		}
	}
}

void Draw_LineX(u8 X0,u8 X1,u8 Y,u32 Color )
{
	u8 Temp ;
	
    if( X0 > X1 )
    {
        Temp = X1 ;
        X1  = X0 ;
        X0  = Temp ;
    }
    for( ; X0 <= X1 ; X0++ )
    Draw_Point( X0,Y,Color,0);
}

void Draw_LineY(u8 X,u8 Y0,u8 Y1,u32 Color )
{
    u8 Temp ;
	
    if( Y0 > Y1 )
    {
        Temp = Y1 ;
        Y1  = Y0 ;
        Y0  = Temp ;
    }
    for(; Y0 <= Y1 ; Y0++)
    Draw_Point(X,Y0,Color,0);
}

void Draw_Line(u8 StartX,u8 StartY,u8 EndX, u8 EndY,u32 Color)
{
	int t, distance;      
    int x = 0 , y = 0 , delta_x, delta_y ;
    int incx, incy ;

    delta_x = EndX - StartX ;
    delta_y = EndY - StartY ;

    if( delta_x > 0 )
    {
        incx = 1;
    }
    else if( delta_x == 0 )
    {
        Draw_LineY( StartX, StartY, EndY, Color ) ;
        return ;
    }
    else
    {
        incx = -1 ;
    }
    if( delta_y > 0 )
    {
        incy = 1 ;
    }
    else if(delta_y == 0 )
    {
        Draw_LineX( StartX, EndX, StartY, Color ) ;   
        return ;
    }
    else
    {
        incy = -1 ;
    }

    delta_x = abs( delta_x );   
    delta_y = abs( delta_y );
    if( delta_x > delta_y )
    {
        distance = delta_x ;
    }
    else
    {
        distance = delta_y ;
    }
    Draw_Point(StartX,StartY,Color,0) ;   
   
    for( t = 0 ; t <= distance+1  ; t++ )
    {
        Draw_Point(StartX,StartY,Color,0) ;
        x += delta_x ;
        y += delta_y ;
        if( x > distance )
        {
            x -= distance ;
            StartX += incx ;
        }
        if( y > distance )
        {
            y -= distance ;
            StartY += incy ;
        }
    }
}

/*描述：在绘图层设置显示的字符串
 *输入：str - 字符串指针
 *		flag = 0 - 覆盖原来的颜色
 *			 = 1 - 保留原来的颜色(0x000000除外)
 *           = 2 - 透明显示
 *输出：void
*/
void DispStrCH(int8_t x,char *str,u32 Color,u8 flag)
{
	u8 		i;
	u8 		buf;
	int8_t 	X;
	
	while(*str != '\0')
	{
		for(i=0;i<8;i++)
		{
			X=x;
			buf=0x01;
			while(buf)
			{
				if(ASCLL_Font[*str-32][i] & buf)
					Draw_Point(X,i,Color,flag);
				else
					Draw_Point(X,i,0x000000,flag);
				buf <<= 1;
				X++;
			}
		}
		str++;
		x+=8;
	}
}


