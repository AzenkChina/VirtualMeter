/**
 * @brief		
 * @details		
 * @date		2017-01-09
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "task_disconnect.h"
#include "types_disconnect.h"
#include "config_disconnect.h"

#include "jiffy.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __relay_req_status req_status = RELAY_STA_CLOSE;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  
  */
static const struct __disconnect disconnecter = 
{
    .request            = (void *)0,
};





/**
  * @brief  根据全部的请求和请求的优先级仲裁出当前动作
  */
static enum __relay_req_action req_arbitrate(void)
{
    return(RELAY_ACT_RELEASE);
}

/**
  * @brief  根据继电器当前状态和当前请求的动作确定当前策略
  */
static enum __relay_req_policy req_policy(enum __relay_req_status status, enum __relay_req_action * action)
{
    return(RELAY_REQ_ACCEPT);
}

/**
  * @brief  更新继电器当前状态
  */
static void req_update(enum __relay_req_action action)
{
    return;
}






/**
  * @brief  
  */
static void disconnect_init(void)
{
    
}

/**
  * @brief  
  */
static void disconnect_loop(void)
{
    static uint32_t timing = 0;
    
    enum __relay_req_action action;
    enum __relay_req_policy policy;
    
    if(jiffy.after(timing) < 1000)
    {
        return;
    }
    
    timing = jiffy.value();
    
    
    //获取当前继电器请求
    action = req_arbitrate();
    
    //获取当前继电器策略
    policy = req_policy(req_status, &action);
    
    if(policy != RELAY_REQ_DROP)
    {
        //更新当前继电器状态
        req_update(action);
    }
    
}

/**
  * @brief  
  */
static void disconnect_exit(void)
{
    
}

/**
  * @brief  
  */
static void disconnect_reset(void)
{
    
}

/**
  * @brief  
  */
static enum __task_status disconnect_status(void)
{
    return(TASK_INIT);
}


/**
  * @brief  
  */
const struct __task_sched task_disconnect = 
{
    .name               = NAME_DISCONNECT,
    .init               = disconnect_init,
    .loop               = disconnect_loop,
    .exit               = disconnect_exit,
    .reset              = disconnect_reset,
    .status             = disconnect_status,
    .api                = (void *)&disconnecter,
};
