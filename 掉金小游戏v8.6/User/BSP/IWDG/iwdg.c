#include "iwdg.h"

void iwdg_init(void)
{
	IWDG->KR = 0x5555;						//����д
	IWDG->PR = IWDG_Prescaler_4;			//4��Ƶ
	IWDG->RLR = 400;						//Tout = Ԥ��Ƶֵ/40 * ����ֵ  4/40*400=40ms
	IWDG->KR = 0x0000;						//��ֹд
	IWDG->KR = 0xCCCC;						//�������Ź�
}
