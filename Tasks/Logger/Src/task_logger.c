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
  * @brief  
  */
static const struct __logger logger = 
{
    .record             = (void *)0,
    .get                = (void *)0,
    .amount             = (void *)0,
    .clear              = (void *)0,
};




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
