/**
 * @brief		
 * @details		低功耗下需要正常运行
 * @date		2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#include "power.h"

#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux ) 
#include "stm32f0xx.h"
#else
#include "meter.h"
#endif


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __power_status status_current = SUPPLY_BATTERY;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static enum __power_status power_check(void)
{
    //...检测电源状态
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux ) 
    return(SUPPLY_AC);
#else
    if(is_powered())
    {
        return(SUPPLY_AC);
    }
    else
    {
        return(SUPPLY_BATTERY);
    }
#endif

}





/**
  * @brief  
  */
static void power_init(void)
{
    
}



/**
  * @brief  
  */
static enum __power_status power_status(void)
{
    enum __power_status status = power_check();
    
    if((status_current == POWER_WAKEUP) && (status == SUPPLY_BATTERY))
    {
        return(status_current);
    }
    else if((status_current == POWER_REBOOT) || (status_current == POWER_RESET))
    {
    	return(status_current);
    }
    else
    {
        status_current = status;
        
        return(status_current);
    }
}

/**
  * @brief  
  */
static enum __power_status power_alter(enum __power_status status)
{
    if(status == POWER_WAKEUP)
    {
        if(status_current == SUPPLY_BATTERY)
        {
            status_current = POWER_WAKEUP;
        }
    }
    else if(status == SUPPLY_BATTERY)
    {
        status_current = power_check();
    }
    else if(status == POWER_REBOOT)
    {
        status_current = POWER_REBOOT;
    }
    else if(status == POWER_RESET)
    {
        if((status_current == SUPPLY_AC) || (status_current == SUPPLY_DC) || (status_current == SUPPLY_AUX))
        {
            status_current = POWER_RESET;
        }
    }

    return(status_current);
}

/**
  * @brief  
  */
const struct __power power = 
{
    .init               = power_init,
    .status             = power_status,
    .alter              = power_alter,
};
