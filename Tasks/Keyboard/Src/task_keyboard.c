/**
 * @brief		
 * @details		
 * @date		2018-09-07
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "task_keyboard.h"
#include "types_keyboard.h"
#include "config_keyboard.h"

#include "types_display.h"
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
/* Private functions ---------------------------------------------------------*/

static void key_changed(uint16_t id, enum __key_status status)
{
    struct __display *display;
    
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
        
        //显示处理
	    if((system_status() != SYSTEM_WAKEUP) && (system_status() != SYSTEM_RUN))
	    {
            return;
		}

        display = api("task_display");
        
        if(!display)
        {
            return;
        }
        
        //短按上键或者下键，当前显示列表不是键显列表，则切换到键显列表
        if(display->channel() != DISP_CHANNEL_KEY)
        {
            display->change(DISP_CHANNEL_KEY);
            return;
        }
        
        //短按上键，显示上一屏
        if(id == KEY_ID_UP)
        {
            display->list.show.last();
            return;
        }
        
        //短按下键，显示下一屏
        if(id == KEY_ID_DOWN)
        {
            display->list.show.next();
            return;
        }
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
    .name               = NAME_KEYBOARD,
    .init               = keyboard_init,
    .loop               = keyboard_loop,
    .exit               = keyboard_exit,
    .reset              = keyboard_reset,
    .status             = keyboard_status,
    .api                = (void *)&keyboard,
};
