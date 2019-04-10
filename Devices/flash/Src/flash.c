/**
 * @brief		
 * @details		
 * @date		2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#include "flash.h"
#include "delay.h"

#if defined ( _WIN32 ) || defined ( _WIN64 )
#include <windows.h>
#include <direct.h>
#include <io.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#elif defined ( __linux )
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#else
#include "spi2.h"
#include "stm32f0xx.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
//Main Memory Page Read D2H
#define AT45_CMD_RDPG          0xD2
//Buffer 1 Read 54H
#define AT45_CMD_RDBF1         0x54
//Buffer 2 Read 56H
#define AT45_CMD_RDBF2         0x56
//Page Erase 81H
#define AT45_CMD_ERPG          0x81
//Block Erase 50H
#define AT45_CMD_ERBL          0x50
//Sector Erase 7CH
#define AT45_CMD_ERSE          0x7C
//Chip Erase C7H, 94H, 80H, 9AH
#define AT45_CMD_ERIC          0xC7, 0x94, 0x80, 0x9A
//Buffer 1 Write 84H
#define AT45_CMD_WRBF1          0x84
//Buffer 2 Write 87H
#define AT45_CMD_WRBF2          0x87
//Buffer 1 to Main Memory Page Program with Built-in Erase 83H
#define AT45_CMD_WREPBF1        0x83
//Buffer 2 to Main Memory Page Program with Built-in Erase 86H
#define AT45_CMD_WREPBF2        0x86
//Buffer 1 to Main Memory Page Program without Built-in Erase 88H
#define AT45_CMD_WRPBF1         0x88
//Buffer 2 to Main Memory Page Program without Built-in Erase 89H
#define AT45_CMD_WRPBF2         0x89
//Status Register Read D7H
#define AT45_CMD_SR             0xD7
//Manufacturer and Device ID Read 9FH
#define AT45_CMD_RDID           0x9F
#endif

/* Private macro -------------------------------------------------------------*/
//Page size
#define AT45_PAGE_SIZE          ((uint32_t)(512))
//Block size
#define AT45_BLOCK_SIZE         ((uint32_t)(512))
//Block amount
#define AT45_BLOCK_AMOUNT        ((uint32_t)(8192))
//Chip size
#define AT45_CHIP_SIZE          ((uint32_t)(AT45_BLOCK_SIZE * AT45_BLOCK_AMOUNT))

/* Private variables ---------------------------------------------------------*/
static enum __dev_status status = DEVICE_NOTINIT;

#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
#define FIL_PATH    "./memory/flash_1.bin"
#define DIR_PATH    "./memory"
#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  
  */
static enum __dev_status flash_status(void)
{
    return(status);
}

/**
  * @brief  
  */
static void flash_init(enum __dev_state state)
{
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
    GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    
    //PD8 n reset
    //PD9 n power
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
    GPIO_Init(GPIOD, &GPIO_InitStruct);
    
    GPIO_ResetBits(GPIOD, GPIO_Pin_9);
    GPIO_ResetBits(GPIOD, GPIO_Pin_8);
    
    mdelay(50);
    GPIO_SetBits(GPIOD, GPIO_Pin_8);
    mdelay(50);
#else
    FILE *fp;
    char *mem = (char *)0;
    
#if defined ( __linux )
    if(access(FIL_PATH, 0) != 0)
    {
        mkdir(DIR_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#else
    if(_access(FIL_PATH, 0) != 0)
    {
        _mkdir(DIR_PATH);
#endif
        
        fp = fopen(FIL_PATH,"wb+");
        
        if(!fp)
        {
            return;
        }
        
        mem = malloc(AT45_CHIP_SIZE);
        
        if(!mem)
        {
            return;
        }
        
        memset((void *)mem, 0xff, AT45_CHIP_SIZE);
        
        fseek(fp, 0, 0);
        fwrite(mem, 1, AT45_CHIP_SIZE, fp);
        fflush(fp);
        fclose(fp);
        
        free(mem);
    }
    
#if defined ( __linux )
    usleep(100*1000);
#else
    Sleep(100);
#endif

#endif
    status = DEVICE_INIT;
}

/**
  * @brief  
  */
static void flash_suspend(void)
{
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
    GPIO_InitTypeDef GPIO_InitStruct;
    
    //PD8 n reset
    //PD9 n power
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
    GPIO_Init(GPIOD, &GPIO_InitStruct);
    
    GPIO_SetBits(GPIOD, GPIO_Pin_8);
    GPIO_SetBits(GPIOD, GPIO_Pin_9);
#endif
    status = DEVICE_SUSPENDED;
}





/**
  * @brief  
  */
static uint32_t flash_readblock(uint32_t block, uint8_t * buffer)
{
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
	uint32_t addr_sent = 0;
    
    if(block >= AT45_BLOCK_AMOUNT)
    {
        return(0);
    }
    
    addr_sent = (block << 10); 
    addr_sent |= 0x00C00000;
    
    spi2.select(0);
    
	//写入命令
	spi2.octet.write(AT45_CMD_RDPG);
    
	//写入地址
	spi2.octet.write(addr_sent>>16);
	spi2.octet.write(addr_sent>>8);
	spi2.octet.write(addr_sent);
    
    spi2.octet.write(0xff);
    spi2.octet.write(0xff);
    spi2.octet.write(0xff);
    spi2.octet.write(0xff);
    
	//开始读数据
    spi2.read(AT45_BLOCK_SIZE, buffer);
    
	spi2.release(0);
    
    return(AT45_BLOCK_SIZE);
#else
    FILE *fp;
    
    if(page >= AT45_BLOCK_SIZE)
    {
        return(0);
    }
    
#if defined ( __linux )
    if(access(FIL_PATH, 0) != 0)
    {
        return(0);
	}
#else
    if(_access(FIL_PATH, 0) != 0)
    {
        return(0);
	}
#endif
    
    fp = fopen(FIL_PATH,"rb+");
    
    if(!fp)
    {
        return(0);
    }
    
    fseek(fp, (block * AT45_BLOCK_SIZE), 0);
    fread(buffer, 1, AT45_BLOCK_SIZE, fp);
    fclose(fp);
	
#if defined ( __linux )
	usleep(40*1000);
#else
	Sleep(40);
#endif
	
    return(AT45_BLOCK_SIZE);
#endif
}

/**
  * @brief  
  */
static uint32_t flash_writeblock(uint32_t block, const uint8_t *buffer)
{
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
	uint32_t addr_sent = 0;
    uint8_t status;
    uint8_t count_try = 50;
    
    if(block >= AT45_BLOCK_AMOUNT)
    {
        return(0);
    }
    
    spi2.select(0);
    
    addr_sent = (block << 10);
    addr_sent |= 0x00C00000;
    
	//写入命令
	spi2.octet.write(AT45_CMD_WRBF1);
    
	//写入地址
	spi2.octet.write(0);
	spi2.octet.write(0);
	spi2.octet.write(0);
    
    spi2.write(AT45_BLOCK_SIZE, (const uint8_t *)buffer);
    
	spi2.release(0);
    
    
    
    spi2.select(0);
    
	//写入命令
	spi2.octet.write(AT45_CMD_WREPBF1);
    
	//写入地址
	spi2.octet.write(addr_sent>>16);
	spi2.octet.write(addr_sent>>8);
	spi2.octet.write(addr_sent);
    
    spi2.write(AT45_BLOCK_SIZE, (const uint8_t *)buffer);
    
	spi2.release(0);
    
    do
    {
        spi2.select(0);
        spi2.octet.write(AT45_CMD_SR);
        status = spi2.octet.read();
        count_try -= 1;
        udelay(100);
        spi2.release(0);
    }
    while(((status&0x3c) == 0x34) && ((status&0x80) == 0) && count_try);
    
    
    if(((status&0x3c) != 0x34) || (!count_try))
    {
        return(0);
    }
    
    return(AT45_BLOCK_SIZE);
    
#else
    
    FILE *fp;
    uint8_t current_page[AT45_BLOCK_SIZE];
    uint16_t cnt;
    
    if(block >= AT45_BLOCK_AMOUNT)
    {
        return(0);
    }
    
#if defined ( __linux )
    if(access(FIL_PATH, 0) != 0)
    {
        return(0);
	}
#else
    if(_access(FIL_PATH, 0) != 0)
    {
        return(0);
	}
#endif
    
    fp = fopen(FIL_PATH,"rb+");
    
    if(!fp)
    {
        return(0);
    }
    
    memset((void *)current_page, 0, sizeof(current_page));
    
    fseek(fp, (block * AT45_BLOCK_SIZE), 0);
    fread(current_page, 1, AT45_BLOCK_SIZE, fp);
    
    for(cnt=0; cnt<AT45_BLOCK_SIZE; cnt++)
    {
    	current_page[cnt] &= buffer[cnt];
    }
    
    fseek(fp, (block * AT45_BLOCK_SIZE), 0);
    fwrite(current_page, 1, AT45_BLOCK_SIZE, fp);
    fflush(fp);
    fclose(fp);
	
#if defined ( __linux )
	usleep(60*1000);
#else
	Sleep(60);
#endif
    
    return(AT45_BLOCK_SIZE);
#endif
}

/**
  * @brief  
  */
static uint32_t flash_eraseblock(uint32_t block)
{
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
    return(0);
#else
    
    FILE *fp;
    char *mem = (char *)0;
    
    if(block >= AT45_BLOCK_AMOUNT)
    {
        return(0);
    }
    
#if defined ( __linux )
    if(access(FIL_PATH, 0) != 0)
    {
        return(0);
	}
#else
    if(_access(FIL_PATH, 0) != 0)
    {
        return(0);
	}
#endif
    
    fp = fopen(FIL_PATH,"wb+");
    
    if(!fp)
    {
        return(0);
    }
    
    mem = malloc(AT45_BLOCK_SIZE);
    
    if(!mem)
    {
        return(0);
    }
    
    memset((void *)mem, 0xff, AT45_BLOCK_SIZE);
    
    fseek(fp, (block * AT45_BLOCK_SIZE), 0);
    fwrite(mem, 1, AT45_BLOCK_SIZE, fp);
    fflush(fp);
    fclose(fp);
    
    free(mem);
	
#if defined ( __linux )
	usleep(40*1000);
#else
	Sleep(40);
#endif
    
    return(AT45_BLOCK_SIZE);
#endif
}

/**
  * @brief  
  */
static uint32_t flash_readpage(uint32_t block, uint8_t page, uint8_t * buffer)
{
    if(page)
    {
        return(0);
    }
    
    return(flash_readblock(block, buffer));
}

/**
  * @brief  
  */
static uint32_t flash_writepage(uint32_t block, uint8_t page, const uint8_t *buffer)
{
    if(page)
    {
        return(0);
    }
    
    return(flash_writeblock(block, buffer));
}

static uint32_t flash_pagesize(void)
{
    return(AT45_PAGE_SIZE);
}

static uint32_t flash_blocksize(void)
{
    return(AT45_BLOCK_SIZE);
}

static uint32_t flash_blockmount(void)
{
    return(AT45_BLOCK_AMOUNT);
}

static uint32_t flash_chipsize(void)
{
    return(AT45_CHIP_SIZE);
}


/**
  * @brief  
  */
static uint32_t flash_eraseall(void)
{
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
    return(0);
#else
    
    FILE *fp;
    char *mem = (char *)0;
    
#if defined ( __linux )
    if(access(FIL_PATH, 0) != 0)
    {
        return(0);
	}
#else
    if(_access(FIL_PATH, 0) != 0)
    {
        return(0);
	}
#endif
    
    fp = fopen(FIL_PATH,"wb+");
    
    if(!fp)
    {
        return(0);
    }
    
    mem = malloc(AT45_CHIP_SIZE);
    
    if(!mem)
    {
        return(0);
    }
    
    memset((void *)mem, 0xff, AT45_CHIP_SIZE);
    
    fseek(fp, 0, 0);
    fwrite(mem, 1, AT45_CHIP_SIZE, fp);
    fflush(fp);
    fclose(fp);
    
    free(mem);
	
#if defined ( __linux )
	usleep(100*1000);
#else
	Sleep(100);
#endif
    
    return(AT45_CHIP_SIZE);
#endif
}

/**
  * @brief  
  */
const struct __flash flash = 
{
    .control        = 
    {
        .name       = "at45db321",
        .status     = flash_status,
        .init       = flash_init,
        .suspend    = flash_suspend,
        
    },
    
    .page           = 
    {
        .read       = flash_readpage,
        .write      = flash_writepage,
    },
    
    .block          = 
    {
        .read       = flash_readblock,
        .write      = flash_writeblock,
        .erase      = flash_eraseblock,
    },
    
    .info           = 
    {
        .pagesize   = flash_pagesize,
        .blocksize  = flash_blocksize,
        .blockmount = flash_blockmount,
        .chipsize   = flash_chipsize,
    },
    
    .erase          = flash_eraseall,
};
