#include "flash.h"

/*
*名称:	FLASH_WriteByte
*描述:	写内部FLASH
*输入:	addr： 	写入flash页的首地址；
		p	：	待写入数据的首地址	
	Byte_Num：	写入数据的个数(以半字为单位)
*输出:	无
*/
void FLASH_WriteHalfWord(uint32_t addr , uint16_t *p , uint16_t HalfWord_Num)  
{
    FLASH_Unlock();												//解锁flash				
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP 
					|FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);  	//清除相关标志位
    FLASH_ErasePage(addr);										//擦除地址所在页，一次性擦除2Kbyte
	
    while(HalfWord_Num --)  
    {
        FLASH_ProgramHalfWord(addr, *p++);  					//写flash
        addr += 2; 
    }  
    FLASH_Lock();
}

/*
*名称:	FLASH_ReadByte
*描述:	读内部FLASH
*输入:	addr： 	读取地址
		p	：	待写入数据的首地址	
	Byte_Num：	读取数据的个数(以半字为单位)
*输出:	无
*/
void FLASH_ReadHalfWord(uint32_t addr , uint16_t *p , uint16_t HalfWord_Num)  
{  
	while(HalfWord_Num--)  
    {  
		*(p++)=*((uint16_t*)addr);
		addr+=2;
    }  
}
