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
#include "stm32f0xx.h"
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
static enum __dev_status buz_status(void)
{
    return(status);
}

/**
  * @brief  
  */
static void buz_init(enum __dev_state state)
{
    status = DEVICE_INIT;
}

/**
  * @brief  
  */
static void buz_suspend(void)
{
    status = DEVICE_NOTINIT;
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
};

