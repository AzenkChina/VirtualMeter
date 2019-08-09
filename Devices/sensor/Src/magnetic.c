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
static enum __dev_status hsensor_status(void)
{
    return(status);
}

/**
  * @brief  
  */
static void hsensor_init(enum __dev_state state)
{
    status = DEVICE_INIT;
}

/**
  * @brief  
  */
static void hsensor_suspend(void)
{
    status = DEVICE_NOTINIT;
}

static void hsensor_runner(uint16_t msecond)
{
    
}

static enum __switch_status hsensor_get(void)
{
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
	return(SWITCH_OPEN);
#else
    return(mailslot_magnetic());
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

