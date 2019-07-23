#include "iwdg.h"

void iwdg_init(void)
{
	IWDG->KR = 0x5555;						//允许写
	IWDG->PR = IWDG_Prescaler_4;			//4分频
	IWDG->RLR = 400;						//Tout = 预分频值/40 * 重载值  4/40*400=40ms
	IWDG->KR = 0x0000;						//禁止写
	IWDG->KR = 0xCCCC;						//启动看门狗
}
