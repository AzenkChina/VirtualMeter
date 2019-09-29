/**
 * @brief		
 * @details		
 * @date		2016-08-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
#include <time.h>
#else

#if defined (STM32F091)
#include "stm32f0xx.h"
#include "viic.h"
#endif

#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __dev_status status = DEVICE_NOTINIT;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  
  */
static enum __dev_status rtc_status(void)
{
    return(status);
}

/**
  * @brief  
  */
static void rtc_init(enum __dev_state state)
{
    status = DEVICE_INIT;
}

/**
  * @brief  
  */
static void rtc_suspend(void)
{
    status = DEVICE_NOTINIT;
}

/**
  * @brief  
  */
static uint8_t rtc_config_read(uint8_t *param)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	return(0);
#else
	return(0);
#endif
}


/**
  * @brief  
  */
static uint8_t rtc_config_write(const uint8_t *param)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	return(0);
#else
	return(0);
#endif
}

/**
  * @brief  
  */
static uint64_t rtc_read(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	time_t stamp;
	time(&stamp);
	
	return((uint64_t)stamp);
#else
	return(0);
#endif
}


/**
  * @brief  
  */
static uint64_t rtc_write(uint64_t stamp)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	return(0);
#else
	return(0);
#endif
}


const struct __rtc rtc = 
{
    .control        = 
    {
        .name       = "rx8025t",
        .status     = rtc_status,
        .init       = rtc_init,
        .suspend    = rtc_suspend,
    },
	
	.read			= rtc_read,
	.write			= rtc_write,
    
    .config			= 
    {
        .read		= rtc_config_read,
        .write		= rtc_config_write,
    },
};

