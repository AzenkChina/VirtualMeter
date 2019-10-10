/**
 * @brief		
 * @details		
 * @date		2019-07-03
 **/

/* Includes ------------------------------------------------------------------*/
#include "kernel.h"
#include "allocator.h"
#include "allocator_ctrl.h"
#include "tasks.h"
#include "string.h"

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  
  */
struct __nvram_entry
{
    char						*name;
    uint32_t					size;
};

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define NVRAM_ENTRY_AMOUNT			((uint16_t)(sizeof(nvram_entry)/sizeof(struct __nvram_entry)))
#define NVRAM_SIZE                  ((uint32_t)(2*1024))

/* Private variables ---------------------------------------------------------*/
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
static uint8_t nvpool[NVRAM_SIZE];
#else

#if defined ( __ICCARM__ )
__no_init static uint8_t nvpool[NVRAM_SIZE];
#elif defined ( __CC_ARM )
__attribute__((zero_init)) static uint8_t nvpool[NVRAM_SIZE];
#elif defined ( __GNUC__ )
static uint8_t nvpool[NVRAM_SIZE] __attribute__ ((section ("noinit")));
#else
#error compiler not support.
#endif


#endif


/**
  * @brief  系统文件表
  */
static const struct __nvram_entry nvram_entry[] = 
{
    /* 文件名  文件大小(需要保证是8的整数倍) */
	{"energy", 256},
};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  
  */
static void * nvram_address(const char *name)
{
	uint16_t loop;
	uint32_t address = 0;
    
    for(loop=0; loop<NVRAM_ENTRY_AMOUNT; loop++)
    {
        if(strcmp(nvram_entry[loop].name, name) == 0)
        {
            if((address + nvram_entry[loop].size) >= NVRAM_SIZE)
            {
                return((void *)0);
            }
            
            return(nvpool + address);
        }
        
        address += nvram_entry[loop].size;
        
        if(address >= NVRAM_SIZE)
        {
            return((void *)0);
        }
    }
    
    return((void *)0);
}

/**
  * @brief  
  */
static uint32_t nvram_size(const char *name)
{
	uint16_t loop;
    uint32_t address = 0;
    
    for(loop=0; loop<NVRAM_ENTRY_AMOUNT; loop++)
    {
        if(strcmp(nvram_entry[loop].name, name) == 0)
        {
            if((address + nvram_entry[loop].size) >= NVRAM_SIZE)
            {
                return(0);
            }
            
            return(nvram_entry[loop].size);
        }
        
        address += nvram_entry[loop].size;
        
        if(address >= NVRAM_SIZE)
        {
            return(0);
        }
    }
    
    return(0);
}


/**
  * @brief  
  */
struct __nvram nvram = 
{
	.address            = nvram_address,
	.size				= nvram_size,
};
