/**
 * @brief		
 * @details		
 * @date		2018-09-07
 **/

/* Includes ------------------------------------------------------------------*/
#include "magnetic.h"

#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
#include "button.h"
#else

#if defined (DEMO_STM32F091)
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
static enum __dev_status hsensor_status(void)
{
    return(DEVICE_INIT);
}

/**
  * @brief  
  */
static void hsensor_init(enum __dev_state state)
{
#if defined (DEMO_STM32F091)
    GPIO_InitTypeDef GPIO_InitStruct;
    
    if(state == DEVICE_NORMAL)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
        
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOE, &GPIO_InitStruct);
        
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        
        GPIO_Init(GPIOE, &GPIO_InitStruct);
        
        GPIO_ResetBits(GPIOE, GPIO_Pin_6);
    }
#endif
}

/**
  * @brief  
  */
static void hsensor_suspend(void)
{
    
}

static void hsensor_runner(uint16_t msecond)
{
}

static enum __switch_status hsensor_get(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    return(mailslot_magnetic());
#else

#if defined (DEMO_STM32F091)
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) == Bit_RESET)
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

static uint8_t hsensor_set(enum __switch_status status)
{
    return(0);
}

const struct __switch hsensor = 
{
	.control        = 
	{
		.name       = "hall sensor",
		.status     = hsensor_status,
		.init       = hsensor_init,
		.suspend    = hsensor_suspend,
	},
    
    .runner         = hsensor_runner,
    .get            = hsensor_get,
    .set            = hsensor_set,
};

