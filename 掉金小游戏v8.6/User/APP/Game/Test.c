#include "Drawing.h"
#include "Control.h"
#include "WavPlay.h"
#include "math.h"
#include "Test.h"

static void Tank_Model(u8 x,u8 y,u32 colour,u8 flag);

void Test_Init(void)
{
	Tank_Model(8,2,0xff0000,0);
	Tank_Model(1,5,0xff0000,0);
	
	Update_Layer();
}


/*����: ̹��ģ��
 *���룺y - ̹��λ��
 *	    colour - ��ɫ
 *		flag = 0 - ��̹��
 *           = 1 - ��̹��
 *�����void
 */
static void Tank_Model(u8 x,u8 y,u32 colour,u8 flag)
{
	if(flag == 0)
	{
		if(y != 0)
		{
			Draw_Point(x,y-1,colour,0);
			Draw_Point(x+1,y-1,colour,0);
		}
		if(y < LED_Y-1)
		{
			Draw_Point(x,y+1,colour,0);
			Draw_Point(x+1,y+1,colour,0);
		}
		Draw_Point(x+1,y,colour,0);
		Draw_Point(x+2,y,colour,0);
	}
	else if(flag == 1)
	{
		if(y != 0)
		{
			Draw_Point(x,y-1,colour,0);
			Draw_Point(x-1,y-1,colour,0);
		}
		
		if(y < LED_Y-1)
		{
			Draw_Point(x,y+1,colour,0);
			Draw_Point(x-1,y+1,colour,0);
		}
		
		Draw_Point(x-1,y,colour,0);
		Draw_Point(x-2,y,colour,0);
	}
}

void Test_Game(void)
{
	
}
