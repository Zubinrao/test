#include "flash.h"

/*
*����:	FLASH_WriteByte
*����:	д�ڲ�FLASH
*����:	addr�� 	д��flashҳ���׵�ַ��
		p	��	��д�����ݵ��׵�ַ	
	Byte_Num��	д�����ݵĸ���(�԰���Ϊ��λ)
*���:	��
*/
void FLASH_WriteHalfWord(uint32_t addr , uint16_t *p , uint16_t HalfWord_Num)  
{
    FLASH_Unlock();												//����flash				
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP 
					|FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);  	//�����ر�־λ
    FLASH_ErasePage(addr);										//������ַ����ҳ��һ���Բ���2Kbyte
	
    while(HalfWord_Num --)  
    {
        FLASH_ProgramHalfWord(addr, *p++);  					//дflash
        addr += 2; 
    }  
    FLASH_Lock();
}

/*
*����:	FLASH_ReadByte
*����:	���ڲ�FLASH
*����:	addr�� 	��ȡ��ַ
		p	��	��д�����ݵ��׵�ַ	
	Byte_Num��	��ȡ���ݵĸ���(�԰���Ϊ��λ)
*���:	��
*/
void FLASH_ReadHalfWord(uint32_t addr , uint16_t *p , uint16_t HalfWord_Num)  
{  
	while(HalfWord_Num--)  
    {  
		*(p++)=*((uint16_t*)addr);
		addr+=2;
    }  
}
