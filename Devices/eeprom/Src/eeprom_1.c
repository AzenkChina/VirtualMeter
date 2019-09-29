/**
 * @brief		
 * @details		
 * @date		2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#include "eeprom_1.h"
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
#include "viic.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
//Page size
#define EEP_PAGE_SIZE          ((uint32_t)(64))
//Page amount
#define EEP_PAGE_AMOUNT        ((uint32_t)(512))

//Chip size
#define EEP_CHIP_SIZE          ((uint32_t)(EEP_PAGE_SIZE * EEP_PAGE_AMOUNT))

/* Private variables ---------------------------------------------------------*/
static enum __dev_status status = DEVICE_NOTINIT;

#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
#define FIL_PATH    "./memory/eeprom_1.bin"
#define DIR_PATH    "./memory"
#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  
  */
static enum __dev_status eep_status(void)
{
    return(status);
}

/**
  * @brief  
  */
static void eep_init(enum __dev_state state)
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
        
        mem = malloc(EEP_CHIP_SIZE);
        
        if(!mem)
        {
            return;
        }
        
        memset((void *)mem, 0xff, EEP_CHIP_SIZE);
        
        fseek(fp, 0, 0);
        fwrite(mem, 1, EEP_CHIP_SIZE, fp);
        fflush(fp);
        fclose(fp);
        
        free(mem);
		
#if defined ( __linux )
		usleep(10*1000);
#else
		Sleep(10);
#endif
    }
#else
    //...
#endif
    status = DEVICE_INIT;
}

/**
  * @brief  
  */
static void eep_suspend(void)
{
    status = DEVICE_SUSPENDED;
}

static uint32_t eep_page_read(uint32_t page, uint16_t offset, uint16_t size, uint8_t * buffer)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    FILE *fp;
    
    if(page >= EEP_PAGE_AMOUNT)
    {
        return(0);
    }
    
    if(offset >= EEP_PAGE_SIZE)
    {
        return(0);
    }
    
    if(!size || (size > EEP_PAGE_SIZE))
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
    
    fseek(fp, (page * EEP_PAGE_SIZE + offset), 0);
    fread(buffer, 1, size, fp);
    fclose(fp);
	
#if defined ( __linux )
	usleep(10*1000*size/EEP_PAGE_SIZE);
#else
	Sleep(10*size/EEP_PAGE_SIZE);
#endif
    
    return(size);
#else
    return(0);
#endif
}

static uint32_t eep_page_write(uint32_t page, uint16_t offset, uint16_t size, const uint8_t *buffer)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    FILE *fp;
    
    if(page >= EEP_PAGE_AMOUNT)
    {
        return(0);
    }
    
    if(offset >= EEP_PAGE_SIZE)
    {
        return(0);
    }
    
    if(!size || (size > EEP_PAGE_SIZE))
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
    
    fseek(fp, (page * EEP_PAGE_SIZE + offset), 0);
    fwrite(buffer, 1, size, fp);
    fflush(fp);
    fclose(fp);
	
#if defined ( __linux )
	usleep(10*1000*size/EEP_PAGE_SIZE);
#else
	Sleep(10*size/EEP_PAGE_SIZE);
#endif
    
    return(size);
#else
    return(0);
#endif
}

static uint32_t eep_erase(void)
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
    
    mem = malloc(EEP_CHIP_SIZE);
    
    if(!mem)
    {
        return(0);
    }
    
    memset((void *)mem, 0xff, EEP_CHIP_SIZE);
    
    fseek(fp, 0, 0);
    fwrite(mem, 1, EEP_CHIP_SIZE, fp);
    fflush(fp);
    fclose(fp);
    
    free(mem);
	
#if defined ( __linux )
	usleep(50*1000);
#else
	Sleep(50);
#endif
    
    return(EEP_CHIP_SIZE);
#else
    return(0);
#endif
}

static uint32_t eep_pagesize(void)
{
    return(EEP_PAGE_SIZE);
}

static uint32_t eep_pageamount(void)
{
    return(EEP_PAGE_AMOUNT);
}

static uint32_t eep_chipsize(void)
{
    return(EEP_CHIP_SIZE);
}

/**
  * @brief  
  */
const struct __eeprom eeprom_1 = 
{
    .control        = 
    {
        .name       = "eeprom#1",
        .status     = eep_status,
        .init       = eep_init,
        .suspend    = eep_suspend,
    },
    
    .page           = 
    {
        .read       = eep_page_read,
        .write      = eep_page_write,
    },
    
    .info           = 
    {
        .pagesize   = eep_pagesize,
        .pageamount = eep_pageamount,
        .chipsize   = eep_chipsize,
    },
    
    .erase          = eep_erase,
};
