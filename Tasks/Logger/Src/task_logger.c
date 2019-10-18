/**
 * @brief		
 * @details		
 * @date		2017-01-09
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "task_logger.h"
#include "types_logger.h"
#include "config_logger.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  获取指定的事件的状态
  */
static enum __event_status logger_event_status(enum __event_id id)
{
    return(EVES_END);
}

/**
  * @brief  获取指定的事件发生次数
  */
static uint32_t logger_event_amount(enum __event_id id)
{
    return(0);
}

/**
  * @brief  清除指定的事件发生次数
  */
static uint32_t logger_event_clear(enum __event_id id)
{
    return(0);
}

/**
  * @brief  
  */
static const struct __logger logger = 
{
    .status     = logger_event_status,
    .amount     = logger_event_amount,
    .clear      = logger_event_clear,
};



/**
  * @brief  事件记录器
  */
void __log_trrgger(enum __event_id id)
{
    
}




/**
  * @brief  
  */
static void logger_init(void)
{
    
}

/**
  * @brief  
  */
static void logger_loop(void)
{
    
}

/**
  * @brief  
  */
static void logger_exit(void)
{
    
}

/**
  * @brief  
  */
static void logger_reset(void)
{
    
}

/**
  * @brief  
  */
static enum __task_status logger_status(void)
{
    
}


/**
  * @brief  
  */
const struct __task_sched task_logger = 
{
    .name               = NAME_LOGGER,
    .init               = logger_init,
    .loop               = logger_loop,
    .exit               = logger_exit,
    .reset              = logger_reset,
    .status             = logger_status,
    .api                = (void *)&logger,
};
