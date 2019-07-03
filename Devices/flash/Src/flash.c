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
/* Private macro -------------------------------------------------------------*/
//Block size
#define FLASH_BLOCK_SIZE            ((uint32_t)(4096))
//Block amount
#define FLASH_BLOCK_AMOUNT          ((uint32_t)(1024))
//Chip size
#define FLASH_CHIP_SIZE             ((uint32_t)(FLASH_BLOCK_SIZE * FLASH_BLOCK_AMOUNT))

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

#endif
    status = DEVICE_INIT;
}

/**
  * @brief  
  */
static void flash_suspend(void)
{
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
    
#endif
    status = DEVICE_SUSPENDED;
}





/**
  * @brief  
  */
static uint32_t flash_readblock(uint32_t block, uint16_t offset, uint16_t size, uint8_t * buffer)
{
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
    
    return(0);
    
#else
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
#endif
}

/**
  * @brief  
  */
static uint32_t flash_writeblock(uint32_t block, uint16_t offset, uint16_t size, const uint8_t *buffer)
{
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
    
    return(0);
    
#else
    
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
