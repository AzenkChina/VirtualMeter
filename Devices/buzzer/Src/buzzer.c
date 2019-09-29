/**
 * @brief		
 * @details		
 * @date		2016-08-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "buzzer.h"

#if defined ( _WIN32 ) || defined ( _WIN64 )
#include <windows.h>
#elif defined ( __linux )
#include <unistd.h>
#else

#if defined (STM32F091)
#include "stm32f0xx.h"
#endif

#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  
  */
static enum __dev_status buz_status(void)
{
    return(DEVICE_INIT);
}

/**
  * @brief  
  */
static void buz_init(enum __dev_state state)
{
    
}

/**
  * @brief  
  */
static void buz_suspend(void)
{
    
}

/**
  * @brief  
  */
static void buz_runner(uint16_t msecond)
{
    
}

/**
  * @brief  
  */
static enum __switch_status buz_get(void)
{
    return(SWITCH_OPEN);
}

/**
  * @brief  
  */
static uint8_t buz_set(enum __switch_status status)
{
    return((uint8_t)SWITCH_OPEN);
}

const struct __switch buzzer = 
{
	.control        = 
	{
		.name       = "buzzer",
		.status     = buz_status,
		.init       = buz_init,
		.suspend    = buz_suspend,
	},
    
    .runner         = buz_runner,
    .get            = buz_get,
    .set            = buz_set,
};

