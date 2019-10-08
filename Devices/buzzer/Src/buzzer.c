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

#if defined (DEMO_STM32F091)
#include "stm32f0xx.h"
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
static enum __dev_status buz_status(void)
{
    return(status);
}

/**
  * @brief  
  */
static void buz_init(enum __dev_state state)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    status = DEVICE_INIT;
#else
    
#if defined (DEMO_STM32F091)
    GPIO_InitTypeDef GPIO_InitStruct;
    
    if(state == DEVICE_NORMAL)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
        
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOE, &GPIO_InitStruct);
        GPIO_ResetBits(GPIOE, GPIO_Pin_10);
        status = DEVICE_INIT;
    }
#endif
    
#endif
}

/**
  * @brief  
  */
static void buz_suspend(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    status = DEVICE_SUSPENDED;
#else
    
#if defined (DEMO_STM32F091)
    status = DEVICE_SUSPENDED;
#endif
    
#endif
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
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    return(SWITCH_OPEN);
#else

#if defined (DEMO_STM32F091)
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_10) == Bit_SET)
    {
        return(SWITCH_CLOSE);
    }
    else
    {
        return(SWITCH_OPEN);
    }
#endif

#endif
}

/**
  * @brief  
  */
static uint8_t buz_set(enum __switch_status status)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    return((uint8_t)SWITCH_OPEN);
#else

#if defined (DEMO_STM32F091)
    if(status == SWITCH_CLOSE)
    {
        GPIO_SetBits(GPIOE, GPIO_Pin_10);
        return(SWITCH_CLOSE);
    }
    else
    {
        GPIO_ResetBits(GPIOE, GPIO_Pin_10);
        return(SWITCH_OPEN);
    }
#endif

#endif
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

