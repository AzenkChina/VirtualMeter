/**
 * @brief		
 * @details		
 * @date		2016-08-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "leds.h"
#include "trace.h"

#if defined (DEMO_STM32F091)
#include "stm32f0xx.h"
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
#if defined (DEMO_STM32F091)
    GPIO_InitTypeDef GPIO_InitStruct;
    
    if(state == DEVICE_NORMAL)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
        
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        
        GPIO_Init(GPIOA, &GPIO_InitStruct);
        
        GPIO_SetBits(GPIOA, GPIO_Pin_7);
    }
#endif
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
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    return(LED_OFF);
#else

#if defined (DEMO_STM32F091)
    if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_7) == Bit_SET)
    {
        return(LED_OFF);
    }
    else
    {
        return(LED_ON);
    }
#endif

#endif
}

/**
  * @brief  
  */
static uint8_t led_relay_set(enum __led_status value)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    return(0);
#else

#if defined (DEMO_STM32F091)
    if(value == LED_ON)
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_7);
    }
    else
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_7);
    }
    
    return((uint8_t)value);
#endif

#endif
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
#if defined (DEMO_STM32F091)
    GPIO_InitTypeDef GPIO_InitStruct;
    
    if(state == DEVICE_NORMAL)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
        
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        
        GPIO_Init(GPIOA, &GPIO_InitStruct);
        
        GPIO_SetBits(GPIOA, GPIO_Pin_3);
    }
#endif
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
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    return(LED_OFF);
#else

#if defined (DEMO_STM32F091)
    if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_3) == Bit_SET)
    {
        return(LED_OFF);
    }
    else
    {
        return(LED_ON);
    }
#endif

#endif
}

/**
  * @brief  
  */
static uint8_t led_warn_set(enum __led_status value)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    return(0);
#else

#if defined (DEMO_STM32F091)
    if(value == LED_ON)
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_3);
    }
    else
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_3);
    }
    
    return((uint8_t)value);
#endif

#endif
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

