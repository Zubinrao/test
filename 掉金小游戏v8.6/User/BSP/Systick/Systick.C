#include "Systick.h"
#include "os_cfg_app.h"

void Systick_Init(void)
{
	SysTick_Config(SystemCoreClock/OS_CFG_TICK_RATE_HZ);
	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}
