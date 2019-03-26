/**
 * @brief		
 * @details		
 * @date		2018-09-07
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "task_keyboard.h"
#include "types_keyboard.h"

#include "power.h"
#include "crc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define WAKE_DEFAULT			((uint16_t)30*1000)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static struct __wake_time
{
    uint16_t config;
    uint16_t counter;
    
} wake_time;

/* Private function prototypes -----------------------------------------------*/
CALLBACK_SLOT(display_key_changed);

/* Private functions ---------------------------------------------------------*/

static void key_changed(uint16_t id, enum __key_status status)
{
    struct __keyboard_event event;
    
    if((id != KEY_ID_UP) && (id != KEY_ID_DOWN))
    {
        return;
    }
    else
    {
	    if(status != KEY_PRESS)
	    {
	        return;
	    }
	    
    	wake_time.counter = 0;
    	
	    if(system_status() == SYSTEM_SLEEP)
	    {
	    	power.alter(POWER_WAKEUP);
	    	return;
		}
        
        event.id = id;
        event.status = status;
        
        CALLBACK_TRIGGER(display_key_changed, &event);
    }
}

static uint8_t waketime_get(void)
{
    return((uint8_t)(wake_time.config / 1000));
}

static uint8_t waketime_set(uint8_t sec)
{
    if(sec > 60)
    {
        return((uint8_t)(wake_time.config / 1000));
    }
    
    wake_time.config = sec * 1000;
    
    return(sec);
}

static const struct __keyboard keyboard = 
{
    .waketime           = 
    {
        .get            = waketime_get,
        .set            = waketime_set,
    },
};


/**
  * @brief  
  */
static void keyboard_init(void)
{
    wake_time.config = WAKE_DEFAULT;
    wake_time.counter = 0;
    
    if((system_status() == SYSTEM_RUN) || (system_status() == SYSTEM_WAKEUP))
    {
        keys.control.init(DEVICE_NORMAL);
	}
    else
    {
        keys.control.init(DEVICE_LOWPOWER);
    }
    
    keys.handler.filling(key_changed);
}

/**
  * @brief  
  */
static void keyboard_loop(void)
{
    keys.runner(KERNEL_PERIOD);
    
    if(system_status() == SYSTEM_WAKEUP)
    {
    	if(wake_time.counter < wake_time.config)
    	{
        	wake_time.counter += KERNEL_PERIOD;
    	}
        else
        {
        	power.alter(SUPPLY_BATTERY);
        }
	}
	else
	{
		wake_time.counter = 0;
	}
}

/**
  * @brief  
  */
static void keyboard_exit(void)
{
    keys.handler.remove();
    keys.control.suspend();
}

/**
  * @brief  
  */
static void keyboard_reset(void)
{
    keys.handler.remove();
    keys.control.suspend();
}

/**
  * @brief  
  */
static enum __task_status keyboard_status(void)
{
    return(TASK_INIT);
}


/**
  * @brief  
  */
const struct __task_sched task_keyboard = 
{
    .name               = "task_keyboard",
    .init               = keyboard_init,
    .loop               = keyboard_loop,
    .exit               = keyboard_exit,
    .reset              = keyboard_reset,
    .status             = keyboard_status,
    .api                = (void *)&keyboard,
};
