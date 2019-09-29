/**
 * @brief		
 * @details		
 * @date		2016-08-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "leds.h"
#include "trace.h"

#if defined ( _WIN32 ) || defined ( _WIN64 )

#elif defined ( __linux )

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
static enum __dev_status led_relay_status(void)
{
    return(DEVICE_INIT);
}

/**
  * @brief  
  */
static void led_relay_init(enum __dev_state state)
{
}

/**
  * @brief  
  */
static void led_relay_suspend(void)
{
}

/**
  * @brief  
  */
static enum __led_status led_relay_get(void)
{
	return(LED_OFF);
}

/**
  * @brief  
  */
static uint8_t led_relay_set(enum __led_status value)
{
	return(0);
}

/**
  * @brief  
  */
static enum __dev_status led_warn_status(void)
{
    return(DEVICE_INIT);
}

/**
  * @brief  
  */
static void led_warn_init(enum __dev_state state)
{
}

/**
  * @brief  
  */
static void led_warn_suspend(void)
{
}

/**
  * @brief  
  */
static enum __led_status led_warn_get(void)
{
	return(LED_OFF);
}

/**
  * @brief  
  */
static uint8_t led_warn_set(enum __led_status value)
{
	return(0);
}

const struct __led led[LED_AMOUNT] = 
{
	{
		.control        = 
		{
			.name       = "led relay",
			.status     = led_relay_status,
			.init       = led_relay_init,
			.suspend    = led_relay_suspend,
		},
		
		.get			= led_relay_get,
		.set			= led_relay_set,
	},
	{
		.control        = 
		{
			.name       = "led warning",
			.status     = led_warn_status,
			.init       = led_warn_init,
			.suspend    = led_warn_suspend,
		},
		
		.get			= led_warn_get,
		.set			= led_warn_set,
	},
};

