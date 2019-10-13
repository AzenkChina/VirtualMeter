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

#if defined (DEMO_STM32F091)
#include "vspi2.h"
#include "stm32f0xx.h"
#endif

#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define devspi      vspi2

/* Private macro -------------------------------------------------------------*/
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
//Block size
#define FLASH_BLOCK_SIZE            ((uint32_t)(4096))
//Block amount
#define FLASH_BLOCK_AMOUNT          ((uint32_t)(1024))
//Chip size
#define FLASH_CHIP_SIZE             ((uint32_t)(FLASH_BLOCK_SIZE * FLASH_BLOCK_AMOUNT))
#else

#if defined (DEMO_STM32F091)
//Main Memory Page Read D2H
#define AT45_CMD_RDPG           0xD2
//Main Memory Page to Buffer 1 Transfer
#define AT45_CMD_RDTBF1         0x53
//Buffer 1 Read 54H
#define AT45_CMD_RDBF1          0x54
//Buffer 2 Read 56H
#define AT45_CMD_RDBF2          0x56
//Page Erase 81H
#define AT45_CMD_ERPG           0x81
//Block Erase 50H
#define AT45_CMD_ERBL           0x50
//Sector Erase 7CH
#define AT45_CMD_ERSE           0x7C
//Chip Erase C7H, 94H, 80H, 9AH
#define AT45_CMD_ERIC           0xC7, 0x94, 0x80, 0x9A
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

//Block size
#define FLASH_BLOCK_SIZE            ((uint32_t)(512))
//Block amount
#define FLASH_BLOCK_AMOUNT          ((uint32_t)(8192))
//Chip size
#define FLASH_CHIP_SIZE             ((uint32_t)(FLASH_BLOCK_SIZE * FLASH_BLOCK_AMOUNT))
#endif

#endif


/* Private variables ---------------------------------------------------------*/
static enum __dev_status status = DEVICE_NOTINIT;

#if defined ( _WIN32 ) || defined ( _WIN64 )
#define FIL_PATH    "./memory/flash_1.bin"
#define DIR_PATH    "./memory"
#elif defined ( __linux )
#if defined ( BUILD_DAEMON )
#define FIL_PATH    "/var/virtual_meter/flash_1.bin"
#define DIR_PATH    "/var/virtual_meter"
#else
#define FIL_PATH    "./memory/flash_1.bin"
#define DIR_PATH    "./memory"
#endif
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
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
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
        
        mem = malloc(FLASH_CHIP_SIZE);
        
        if(!mem)
        {
            return;
        }
        
        memset((void *)mem, 0xff, FLASH_CHIP_SIZE);
        
        fseek(fp, 0, 0);
        fwrite(mem, 1, FLASH_CHIP_SIZE, fp);
        fflush(fp);
        fclose(fp);
        
        free(mem);
    }
    
#if defined ( __linux )
    usleep(100*1000);
#else
    Sleep(100);
#endif
#else
    
#if defined (DEMO_STM32F091)
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
    
    devspi.control.init(state);
#endif
    
#endif
    status = DEVICE_INIT;
}

/**
  * @brief  
  */
static void flash_suspend(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    
#else
    
#if defined (DEMO_STM32F091)
    devspi.control.suspend();
#endif
    
#endif
    status = DEVICE_SUSPENDED;
}





/**
  * @brief  
  */
static uint32_t flash_readblock(uint32_t block, uint16_t offset, uint16_t size, uint8_t * buffer)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    FILE *fp;
    
    if(block >= FLASH_BLOCK_AMOUNT)
    {
        return(0);
    }
    
    if(offset >= FLASH_BLOCK_SIZE)
    {
        return(0);
    }
    
    if(!size || (size > FLASH_BLOCK_SIZE))
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
    
    fseek(fp, (block * FLASH_BLOCK_SIZE + offset), 0);
    fread(buffer, 1, size, fp);
    fclose(fp);
	
#if defined ( __linux )
	usleep(40*1000*size/FLASH_BLOCK_SIZE);
#else
	Sleep(40*size/FLASH_BLOCK_SIZE);
#endif
	
    return(size);
#else
    
#if defined (DEMO_STM32F091)
	uint32_t addr_sent = 0;
    
    if(block >= FLASH_BLOCK_AMOUNT)
    {
        return(0);
    }
    
    if(offset >= FLASH_BLOCK_SIZE)
    {
        return(0);
    }
    
    if(!size || (size > FLASH_BLOCK_SIZE))
    {
        return(0);
    }
    
    addr_sent = (block << 10); 
    addr_sent |= 0x00C00000;
    addr_sent += offset;
    
    devspi.select(0);
    
	//写入命令
	devspi.octet.write(AT45_CMD_RDPG);
    
	//写入地址
	devspi.octet.write(addr_sent>>16);
	devspi.octet.write(addr_sent>>8);
	devspi.octet.write(addr_sent);
    
    devspi.octet.write(0xff);
    devspi.octet.write(0xff);
    devspi.octet.write(0xff);
    devspi.octet.write(0xff);
    
	//开始读数据
    devspi.read(size, buffer);
    
	devspi.release(0);
    
    return(size);
#endif

#endif
}

/**
  * @brief  
  */
static uint32_t flash_writeblock(uint32_t block, uint16_t offset, uint16_t size, const uint8_t *buffer)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    FILE *fp;
    char *mem = (char *)0;
    uint16_t cnt;
    
    if(block >= FLASH_BLOCK_AMOUNT)
    {
        return(0);
    }
    
    if(offset >= FLASH_BLOCK_SIZE)
    {
        return(0);
    }
    
    if(!size || (size > FLASH_BLOCK_SIZE))
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
    
    mem = malloc(FLASH_BLOCK_SIZE);
    
    if(!mem)
    {
        return(0);
    }
    
    memset((void *)mem, 0, FLASH_BLOCK_SIZE);
    
    fseek(fp, (block * FLASH_BLOCK_SIZE + offset), 0);
    fread(mem, 1, size, fp);
    
    for(cnt=0; cnt<size; cnt++)
    {
    	mem[cnt] &= buffer[cnt];
    }
    
    fseek(fp, (block * FLASH_BLOCK_SIZE + offset), 0);
    fwrite(mem, 1, size, fp);
    fflush(fp);
    fclose(fp);
    
    free(mem);
	
#if defined ( __linux )
	usleep(60*1000*size/FLASH_BLOCK_SIZE);
#else
	Sleep(60*size/FLASH_BLOCK_SIZE);
#endif
    
    return(size);
#else
    
#if defined (DEMO_STM32F091)
	uint32_t page = 0;
    uint8_t status;
    uint8_t timeout = 50;
    
    if(block >= FLASH_BLOCK_AMOUNT)
    {
        return(0);
    }
    
    if(offset >= FLASH_BLOCK_SIZE)
    {
        return(0);
    }
    
    if(!size || (size > FLASH_BLOCK_SIZE))
    {
        return(0);
    }
    
    page = (block << 10);
    page |= 0x00C00000;
    
    /** 第一步，读取整页到缓存中 */
    devspi.select(0);
    
	//写入命令
	devspi.octet.write(AT45_CMD_RDTBF1);
    
	//写入地址
	devspi.octet.write(page>>16);
	devspi.octet.write(page>>8);
	devspi.octet.write(page);
    
	devspi.release(0);
    
    do
    {
        devspi.select(0);
        devspi.octet.write(AT45_CMD_SR);
        status = devspi.octet.read();
        timeout -= 1;
        udelay(100);
        devspi.release(0);
    }
    while(((status&0x3c) == 0x34) && ((status&0x80) == 0) && timeout);
    
    /** 第二步，修改缓存内容 */
    devspi.select(0);
    
	//写入命令
	devspi.octet.write(AT45_CMD_WRBF1);
    
	//写入地址
	devspi.octet.write(0);
	devspi.octet.write((uint8_t)(offset >> 8));
	devspi.octet.write((uint8_t)(offset));
    
    devspi.write(size, (const uint8_t *)buffer);
    
	devspi.release(0);
    
    /** 第三步，从缓存中写入整页 */
    devspi.select(0);
    
	//写入命令
	devspi.octet.write(AT45_CMD_WREPBF1);
    
	//写入地址
	devspi.octet.write(page>>16);
	devspi.octet.write(page>>8);
	devspi.octet.write(page);
    
	devspi.release(0);
    
    do
    {
        devspi.select(0);
        devspi.octet.write(AT45_CMD_SR);
        status = devspi.octet.read();
        timeout -= 1;
        udelay(100);
        devspi.release(0);
    }
    while(((status&0x3c) == 0x34) && ((status&0x80) == 0) && timeout);
    
    if(((status&0x3c) != 0x34) || (!timeout))
    {
        return(0);
    }
    
    return(size);
#endif
    
#endif
}

/**
  * @brief  
  */
static uint32_t flash_eraseblock(uint32_t block)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    FILE *fp;
    char *mem = (char *)0;
    
    if(block >= FLASH_BLOCK_AMOUNT)
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
    
    mem = malloc(FLASH_BLOCK_SIZE);
    
    if(!mem)
    {
        return(0);
    }
    
    memset((void *)mem, 0xff, FLASH_BLOCK_SIZE);
    
    fseek(fp, (block * FLASH_BLOCK_SIZE), 0);
    fwrite(mem, 1, FLASH_BLOCK_SIZE, fp);
    fflush(fp);
    fclose(fp);
    
    free(mem);
	
#if defined ( __linux )
	usleep(40*1000);
#else
	Sleep(40);
#endif
    
    return(FLASH_BLOCK_SIZE);
#else
    
#if defined (DEMO_STM32F091)
    return(FLASH_BLOCK_SIZE);
#endif
    
#endif
}

static uint32_t flash_blocksize(void)
{
    return(FLASH_BLOCK_SIZE);
}

static uint32_t flash_blockcount(void)
{
    return(FLASH_BLOCK_AMOUNT);
}

static uint32_t flash_chipsize(void)
{
    return(FLASH_CHIP_SIZE);
}


/**
  * @brief  
  */
static uint32_t flash_eraseall(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
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
    
    mem = malloc(FLASH_CHIP_SIZE);
    
    if(!mem)
    {
        return(0);
    }
    
    memset((void *)mem, 0xff, FLASH_CHIP_SIZE);
    
    fseek(fp, 0, 0);
    fwrite(mem, 1, FLASH_CHIP_SIZE, fp);
    fflush(fp);
    fclose(fp);
    
    free(mem);
	
#if defined ( __linux )
	usleep(100*1000);
#else
	Sleep(100);
#endif
    
    return(FLASH_CHIP_SIZE);
#else
    
#if defined (DEMO_STM32F091)
    return(FLASH_CHIP_SIZE);
#endif
    
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
    
    .block          = 
    {
        .read       = flash_readblock,
        .write      = flash_writeblock,
        .erase      = flash_eraseblock,
    },
    
    .info           = 
    {
        .blocksize  = flash_blocksize,
        .blockcount = flash_blockcount,
        .chipsize   = flash_chipsize,
    },
    
    .erase          = flash_eraseall,
};
