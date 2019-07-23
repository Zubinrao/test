/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "sdio_sdcard.h"
#include "os.h"

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

#define SD_BLOCKSIZE     		512 
extern  SD_CardInfo SDCardInfo;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS status = STA_NOINIT;

	switch (pdrv) 
	{
		case DEV_RAM :
			break;
		case DEV_MMC :
			status &= ~STA_NOINIT;
			break;
		case DEV_USB :
			break;
		default:
			status = STA_NOINIT;
			break;
	}
	return status;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS status = STA_NOINIT;

	switch (pdrv) 
	{
		case DEV_RAM :
			break;
		case DEV_MMC :
			if(SD_Init()==SD_OK)
			{
				status &= ~STA_NOINIT;
			}
			else 
			{
				status = STA_NOINIT;
			}
			break;
		case DEV_USB :
			status = STA_NOINIT;
			break;
		default:
			status = STA_NOINIT;
			break;
	}
	return status;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT status = RES_PARERR;
	SD_Error SD_state = SD_OK;
	CPU_SR_ALLOC();

	switch (pdrv) 
	{
		case DEV_RAM :
			break;
		case DEV_MMC :
			OS_CRITICAL_ENTER();
			if(count > 1)
			{
				SD_state=SD_ReadMultiBlocks(buff,(uint64_t)sector*SD_BLOCKSIZE,SD_BLOCKSIZE,count);
				if(SD_state==SD_OK)
				{
					/* Check if the Transfer is finished */
					SD_state=SD_WaitReadOperation();
					while(SD_GetStatus() != SD_TRANSFER_OK);
				}
			}
			else
			{
				SD_state=SD_ReadBlock(buff, (uint64_t)sector*SD_BLOCKSIZE, SD_BLOCKSIZE);
				if(SD_state==SD_OK)
				{
					/* Check if the Transfer is finished */
					SD_state=SD_WaitReadOperation();
					while(SD_GetStatus() != SD_TRANSFER_OK);
				}
			}
			OS_CRITICAL_EXIT();
			if(SD_state!=SD_OK)
				status = RES_PARERR;
			else
				status = RES_OK;	
			break;
		case DEV_USB :
			break;
		default:
			status = RES_PARERR;
			break;
	}

	return status;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	SD_Error SD_state = SD_OK;
	DRESULT status = RES_PARERR;
	if (!count) {
		return RES_PARERR;		/* Check parameter */
	}

	switch (pdrv) 
	{
		case DEV_RAM :
			break;
		case DEV_MMC :
			if (count > 1)
			{
				SD_state=SD_WriteMultiBlocks((uint8_t *)buff,(uint64_t)sector*SD_BLOCKSIZE,SD_BLOCKSIZE,count);
				if(SD_state==SD_OK)
				{
					SD_state=SD_WaitWriteOperation();
					while(SD_GetStatus() != SD_TRANSFER_OK);			
				}
			}
			else
			{
				SD_state=SD_WriteBlock((uint8_t *)buff, (uint64_t)sector*SD_BLOCKSIZE, SD_BLOCKSIZE);
				if(SD_state==SD_OK)
				{
					SD_state=SD_WaitWriteOperation();
					while(SD_GetStatus() != SD_TRANSFER_OK);			
				}
			}
		
			
			if(SD_state!=SD_OK)
				status = RES_PARERR;
			else
				status = RES_OK;	
			break;
		case DEV_USB :
			break;
		default:
			status = RES_PARERR;
			break;
	}

	return status;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT status = RES_PARERR;

	switch (pdrv) 
	{
		case DEV_RAM :
			break;
		case DEV_MMC :
			switch (cmd) 
			{
				// Get R/W sector size (WORD) 
				case GET_SECTOR_SIZE :    
					*(WORD * )buff = SD_BLOCKSIZE;
				break;
				// Get erase block size in unit of sector (DWORD)
				case GET_BLOCK_SIZE :      
					*(DWORD * )buff = 1;
				break;

				case GET_SECTOR_COUNT:
					*(DWORD * )buff = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;
					break;
				case CTRL_SYNC :
				break;
			}
			status = RES_OK;
			break;
		case DEV_USB :
			break;
		default:
			status = RES_PARERR;
			break;
	}

	return status;
}

__weak DWORD get_fattime(void) {
	/* Returns current time packed into a DWORD variable */
	return	  ((DWORD)(2015 - 1980) << 25)	/* Year 2013 */
			| ((DWORD)7 << 21)				/* Month 7 */
			| ((DWORD)15 << 16)				/* Mday 28 */
			| ((DWORD)11 << 11)				/* Hour 0 */
			| ((DWORD)59 << 5)				/* Min 0 */
			| ((DWORD)59 >> 1);				/* Sec 0 */
}

